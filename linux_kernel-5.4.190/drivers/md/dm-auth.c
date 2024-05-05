
/*
 * The Merkle tree is a sequence of hash lists.
 * Each hash list is a sequence of 16-byte MD5 hash values.
 *
 * Each 4K page of user data is hashed and the hash is stored in
 * the level 0 hash list.  If the user data consists of N 4K pages,
 * the level 0 hash list consists of N 16-byte hash values.
 *
 * Each 4K page of the level 0 hash is itself hashed and the hash is
 * stored in the level 1 hash list.
 * Similarly all subsequent levels are constructed.
 * The last level consists of one 4K page of hashes, the root hash page.
 * The hash of the root hash page is stored in the MerkleHeader.
 *
 * The MerkleHeader defines the position within the lower block device
 * of each level of the tree.
 */

#include "dm.h"

#include <linux/completion.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/blk-crypto.h>
#include <linux/mempool.h>
#include <linux/slab.h>
#include <crypto/hash.h>
#include <linux/workqueue.h>
#include <linux/backing-dev.h>
#include <linux/reboot.h>
#include <linux/atomic.h>
#include <asm/page.h>
#include <asm/unaligned.h>

#include <linux/dm-auth.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/hashtable.h>

#define DMAUTH_DEBUG 1

#ifdef KM_USER0
#define DMAUTH_KMAP(page)    kmap_atomic(page, KM_USER0)
#define DMAUTH_KUNMAP(addr)  kunmap_atomic(addr, KM_USER0)
#else
#define DMAUTH_KMAP(page)    kmap_atomic(page)
#define DMAUTH_KUNMAP(addr)  kunmap_atomic(addr)
#endif

#undef  SECTOR_SIZE
#define SECTOR_SIZE      (1 << SECTOR_SHIFT)

#define DM_MSG_PREFIX    "auth"
#define DM_MAX_HASH_SIZE    32

/*
 * context holding the current state of a verification
 */
struct verify_context {
	struct bio *bio_in;      /* original bio */
	struct bvec_iter iter_in;
	struct page *hash_page;  /* page holding hash data */
	unsigned char hash_value[DM_MAX_HASH_SIZE]; /* current hash */
	int hash_index;          /* index of hash in hash page */
	int verify_page_num;     /* curr page being verified (mh_page_size) */
	int merkle_level;        /* curr level in Merkle tree */
	off_t hash_udata_pos;    /* offset in dev of next udata to be hashed */
};

/*
 * per bio private data
 */
struct dm_auth_io {
	struct auth_config *cc;
	struct bio *base_bio;
	struct work_struct work;
	struct verify_context ctx;
	atomic_t io_pending;
	sector_t io_sector;
};

/*
 * main auth mapper: maps a linear range of a block device
 * and authenticates the data with a Merkle tree.
 */
struct auth_config {
	struct dm_dev *dev;
	sector_t start_sector;
	struct bio_set bs;
	struct MerkleHeader mheader;
	struct workqueue_struct *io_queue;
	struct workqueue_struct *verify_queue;
	unsigned int per_bio_data_size;
	char filename[DMAUTH_FILENAME_MAX_LEN];
};

#define MIN_IOS                 16
#define CORRUPTED_FILE_PROC     "dm_auth_corrupted_file"
#define CORRUPTED_MAGIC         0x12345678

static char __attribute__((section(".bss_noinit"))) corrupted_file[DMAUTH_FILENAME_MAX_LEN];
static unsigned int __attribute__((section(".bss_noinit"))) corrupted_magic;
#ifdef DMAUTH_DEBUG
static int dmauth_debug;
#endif

#define MAX_STICKY_MERKLE_HEADERS  4  /* currently we only use 1, for root */
struct {
	dev_t dev;
	struct MerkleHeader mheader;
} sticky_merkle_headers[MAX_STICKY_MERKLE_HEADERS];

/* Forward declarations */
static struct bio *auth_alloc_bio(struct dm_auth_io *io, sector_t sector,
	size_t size, int segments, struct bio_vec iovec, int is_hash_page);
static void endio_hash(struct bio *bio);
static void kauthd_hash_read_complete_w(struct work_struct *work);

/*
 * Initialize a verify_context.
 */
static int auth_verify_init(struct auth_config *cc,
	struct verify_context *ctx, struct bio *bio_in, sector_t sector)
{
	ctx->bio_in = bio_in;

	if (bio_in)
		ctx->iter_in = bio_in->bi_iter;
	ctx->hash_udata_pos = sector << SECTOR_SHIFT;
	ctx->verify_page_num = ctx->hash_udata_pos / cc->mheader.mh_page_size;
	ctx->hash_index = -1; /* indicates ctx->hash_value is empty/invalid */
	ctx->merkle_level = 0;
	ctx->hash_page = NULL;
	return 0;
}

/*
 * Release the hash page associated with a verify_context.
 */
static void release_hash_page(struct verify_context *ctx)
{
	if (!ctx->hash_page)
		return;
	put_page(ctx->hash_page);
	ctx->hash_page = NULL;
}

/*
 * Free resources associated with a verify_context.
 */
static void auth_verify_deinit(struct auth_config *cc, struct verify_context *ctx)
{
	release_hash_page(ctx); // hash_page should be NULL, so probably not necessary
}

/*
 * Init a struct dm_auth_io for an I/O request (bio).
 */
static void auth_io_init(struct dm_auth_io *io, struct auth_config *cc,
	struct bio *bio, sector_t sector)
{
	io->cc = cc;
	io->base_bio = bio;
	io->io_sector = sector; /* in the underlying block device */
	io->ctx.hash_page = NULL;
	atomic_set(&io->io_pending, 0);
}

/*
 * Mark an I/O request as in progress.
 */
static void auth_inc_pending(struct dm_auth_io *io)
{
	atomic_inc(&io->io_pending);
}

/*
 * Complete an I/O request (after full verification).
 */
static void auth_dec_pending_err(struct dm_auth_io *io, int error)
{
	struct auth_config *cc = io->cc;
	struct bio *base_bio = io->base_bio;

	if (!atomic_dec_and_test(&io->io_pending))
		return;
	auth_verify_deinit(cc, &io->ctx);
	base_bio->bi_status = error;
	bio_endio(base_bio);
}

/*
 * Initialize a hashdesc to perform a hash.
 */
static int hash_init_local(struct auth_config *cc, struct shash_desc **hashdesc)
{
	struct crypto_shash *hash;
	int descsize;
	int err;

	hash = crypto_alloc_shash(cc->mheader.mh_hash_type, 0, CRYPTO_ALG_ASYNC);
	if (unlikely(IS_ERR(hash)))
		return PTR_ERR(hash);
	if (unlikely(crypto_shash_digestsize(hash) > DM_MAX_HASH_SIZE)) {
		pr_err("wrong hash size %d, must be <= %d\n",
			crypto_shash_digestsize(hash), DM_MAX_HASH_SIZE);
		crypto_free_shash(hash);
		return -EINVAL;
	}

	descsize = sizeof(struct shash_desc) + crypto_shash_descsize(hash);
	*hashdesc = kzalloc(descsize, GFP_KERNEL);
	if (!*hashdesc) {
		crypto_free_shash(hash);
		return -ENOMEM;
	}

	(*hashdesc)->tfm = hash;
	err = crypto_shash_init(*hashdesc);
	if (err < 0) {
		crypto_free_shash(hash);
		kfree(*hashdesc);
		*hashdesc = 0;
	}

	return err;
}

/*
 * Free resources associated with a hashdesc.
 */
static void hash_deinit(struct shash_desc *hashdesc)
{
	crypto_free_shash(hashdesc->tfm);
	kfree(hashdesc);
}

#ifdef DMAUTH_DEBUG
static void mdump(unsigned char *data, unsigned int len)
{
	char line[100];
	char *linep = line;
	int i;

	for (i = 0; i < len; i++) {
		linep += sprintf(linep, "%02x", data[i]);
		if ((i % 32) == 31) {
			pr_info("%s\n", line);
			linep = line;
		}
	}
	if (linep > line)
		pr_info("%s\n", line);
}
#endif

/*
 * Hash page of user data referred to by ctx->bio_in
 */
static int hash_next_user_page(struct auth_config *cc, struct verify_context *ctx)
{
	struct shash_desc *hashdesc;
	size_t hashed_bytes;
	size_t avail_bytes;
	char zero[512];
	size_t zero_len = 0;
	int err;

	err = hash_init_local(cc, &hashdesc);
	if (unlikely(err < 0))
		return err;

	/*
	 * See how many bytes are available in the page.
	 * Normally mh_page_size, but the very last page may contain partial data.
	 */
	avail_bytes = cc->mheader.mh_udata_size - ctx->hash_udata_pos;
	if (avail_bytes > cc->mheader.mh_page_size)
		avail_bytes = cc->mheader.mh_page_size;
	hashed_bytes = 0;
#ifdef DMAUTH_DEBUG
	if (dmauth_debug)
		pr_info("hash user page %d, %ld bytes\n", ctx->verify_page_num, (long) avail_bytes);
#endif
	while (hashed_bytes < avail_bytes && ctx->iter_in.bi_size > 0) {
		struct bio_vec bv = bio_iter_iovec(ctx->bio_in, ctx->iter_in);
		int hsize = SECTOR_SIZE;

		if (hashed_bytes + hsize > avail_bytes)
			hsize = avail_bytes - hashed_bytes;
#ifdef DMAUTH_DEBUG
		if (dmauth_debug) {
			unsigned char *p = DMAUTH_KMAP(bv.bv_page);

			mdump(p + bv.bv_offset, hsize);
			DMAUTH_KUNMAP(p);
		}
#endif
		crypto_shash_update(hashdesc, page_to_virt(bv.bv_page) + bv.bv_offset, hsize);
		hashed_bytes += hsize;
		bio_advance_iter(ctx->bio_in, &ctx->iter_in, hsize);
	}

	/* On the last page, pad with zeros to page boundary. */
	while (hashed_bytes != cc->mheader.mh_page_size) {
		int pad_len = cc->mheader.mh_page_size - hashed_bytes;

		WARN_ON(hashed_bytes > cc->mheader.mh_page_size);
		if (pad_len > sizeof(zero))
			pad_len = sizeof(zero);
		if (pad_len > zero_len) { /* lazy zero */
			memset(&zero[zero_len], 0, pad_len - zero_len);
			zero_len = pad_len;
			WARN_ON(zero_len > sizeof(zero));
		}
		crypto_shash_update(hashdesc, zero, pad_len);
		hashed_bytes += pad_len;
	}
	crypto_shash_final(hashdesc, ctx->hash_value);
#ifdef DMAUTH_DEBUG
	if (dmauth_debug) {
		pr_info("final hash\n");
		mdump(ctx->hash_value, cc->mheader.mh_hash_size);
	}
#endif
	hash_deinit(hashdesc);
	ctx->hash_udata_pos += cc->mheader.mh_page_size;
	return 0;
}

/*
 * Hash the next page of hash data in the Merkle tree
 * and step to the next level.
 */
static int hash_next_hash_page(struct auth_config *cc, struct verify_context *ctx)
{
	struct shash_desc *hashdesc;
	int err;

	err = hash_init_local(cc, &hashdesc);
	if (unlikely(err < 0))
		return err;
	crypto_shash_update(hashdesc, page_to_virt(ctx->hash_page), cc->mheader.mh_page_size);
	crypto_shash_final(hashdesc, ctx->hash_value);
	hash_deinit(hashdesc);
	return 0;
}

/*
 * Start reading a page of hash data from the Merkle tree.
 */
static void start_hash_read(struct dm_auth_io *io, struct verify_context *ctx, off_t pos)
{
	struct bio_vec biovec;
	struct auth_config *cc = io->cc;
	sector_t sector = pos >> SECTOR_SHIFT;
	struct bio *bio;

	ctx->hash_page = find_or_create_page(cc->dev->bdev->bd_inode->i_mapping,
		cc->start_sector + (pos >> PAGE_SHIFT), GFP_KERNEL);
	if (unlikely(!ctx->hash_page))
		goto nomem;
	if (PageUptodate(ctx->hash_page)) {
		// Page was already in page cache; don't need to read it.
		unlock_page(ctx->hash_page);
		INIT_WORK(&io->work, kauthd_hash_read_complete_w);
		queue_work(cc->verify_queue, &io->work);
	} else {
		biovec.bv_page = ctx->hash_page;
		biovec.bv_len = cc->mheader.mh_page_size;
		biovec.bv_offset = 0;

		bio = auth_alloc_bio(io, sector, cc->mheader.mh_page_size, 1, biovec, 1);
		if (unlikely(!bio)) {
			unlock_page(ctx->hash_page);
			release_hash_page(ctx);
			goto nomem;
		}
		// do unlock_page in endio:
		// in case multiple requests for the same page come in at the same time,
		// we want to block multiple requests
		// since they'd be writing to the same page (from the cache)
		generic_make_request(bio);
	}
	return;
nomem:
	auth_dec_pending_err(io, -ENOMEM);
}

/*
 * Start reading the next page of hash data from the Merkle tree.
 */
static int start_next_hash_read(struct dm_auth_io *io,
	struct auth_config *cc, struct verify_context *ctx)
{
	/* level_pos = byte offset in device of hash list (curr Merkle level) */
	off_t level_pos = cc->mheader.mh_level_pos[ctx->merkle_level];
	off_t level_size = cc->mheader.mh_level_size[ctx->merkle_level];

	/* hash_offset = byte offset (in level) of verify_page's hash */
	off_t hash_offset = ctx->verify_page_num * cc->mheader.mh_hash_size;

	/* hash_page_offset = offset (in level) of start of hash_offset's page. */
	off_t hash_page_offset = hash_offset - (hash_offset % cc->mheader.mh_page_size);

	if (hash_page_offset + cc->mheader.mh_page_size > level_size) {
		pr_err("hash_offset %ld + %ld > level size %ld\n",
			(long) hash_offset, (long) cc->mheader.mh_page_size, (long) level_size);
		return -EINVAL;
	}

	/* Set ctx parameters for next level of verification. */
	/* verify_page_num = page num (in level) containing verify_page's hash */
	/* hash_index = byte index in hash_page of (old) verify_page's hash. */
	ctx->verify_page_num = hash_offset / cc->mheader.mh_page_size;
	ctx->hash_index = hash_offset - hash_page_offset;

	/* Start a read of (old) verify_page's hash page. */
	start_hash_read(io, ctx, level_pos + hash_page_offset);
	return 0;
}

/*
 * Compare hash of a page with stored hash.
 */
static void verify_level(struct dm_auth_io *io)
{
	struct verify_context *ctx = &io->ctx;
	struct auth_config *cc = io->cc;
	unsigned char *page_addr;
	unsigned char *hash_addr;
	int error = 0;

	/*
	 * Verify that the previously calculated hash (ctx->hash_value)
	 * matches the hash stored in this page (at ctx->hash_index).
	 */
	WARN_ON(ctx->hash_index < 0);
	page_addr = DMAUTH_KMAP(ctx->hash_page);
	// FIXME: can adding hash_index overflow the mapped area?
	hash_addr = page_addr + ctx->hash_index;

	if (memcmp(ctx->hash_value, hash_addr, cc->mheader.mh_hash_size)) {
		pr_warn("dm-auth: checksum mismatch in sector %ld,"
			"hash index %d, level %d\n",
			(long) io->io_sector, ctx->hash_index,
			ctx->merkle_level);
#ifdef DMAUTH_DEBUG
		if (dmauth_debug) {
			pr_info("calc "); mdump(ctx->hash_value, cc->mheader.mh_hash_size);
			pr_info("stor "); mdump(hash_addr, cc->mheader.mh_hash_size);
		}
#endif
		if (cc->filename[0])
			corrupted_magic = CORRUPTED_MAGIC;
		memcpy(corrupted_file, cc->filename, sizeof(cc->filename));
		//disable log print for skip auth check temporarily --- shoufu.zhao
		//pr_warn("dm-auth: corrupted %s marked for deletion\n",
			//corrupted_file);
		error = -EACCES;
	}
	DMAUTH_KUNMAP(page_addr);
	if (error)
		kernel_restart(NULL);
}

/*
 * Finished reading a hash page.
 */
static int kauthd_hash_read_complete(struct dm_auth_io *io)
{
	struct auth_config *cc = io->cc;
	struct verify_context *ctx = &io->ctx;
	int err;

	WARN_ON(bio_data_dir(io->base_bio) != READ);
	verify_level(io);
	/* Hash the page we just read. */
	err = hash_next_hash_page(cc, ctx);
	release_hash_page(ctx);
	if (err < 0)
		return err;
	ctx->merkle_level++;
	if (ctx->merkle_level >= cc->mheader.mh_num_levels) {
		/* Reached last level, verify root. */
		if (memcmp(ctx->hash_value, cc->mheader.mh_root_hash, cc->mheader.mh_hash_size)) {
			pr_warn("*** root checksum mismatch\n");
			kernel_restart(NULL);
		}
		if (ctx->iter_in.bi_size == 0) {
			/* No more user data to verify, all done. */
			return 0;
		}
		/* Step to the next user page; start verifying from level 0 again. */
		ctx->merkle_level = 0;
		ctx->verify_page_num = ctx->hash_udata_pos / cc->mheader.mh_page_size;
		err = hash_next_user_page(cc, ctx);
		if (err < 0)
			return err;
	}
	err = start_next_hash_read(io, cc, ctx);
	if (err < 0)
		return err;
	return SINPROGRESS;
}

static void kauthd_hash_read_complete_w(struct work_struct *work)
{
	struct dm_auth_io *io = container_of(work, struct dm_auth_io, work);
	int err = kauthd_hash_read_complete(io);

	if (err != SINPROGRESS)
		auth_dec_pending_err(io, err);
}

static void endio_hash(struct bio *bio)
{
	struct dm_auth_io *io = bio->bi_private;
	struct auth_config *cc = io->cc;
	int error = bio->bi_status;

	WARN_ON(bio_data_dir(bio) != READ);
	bio_put(bio);
	if (error) {
		unlock_page(io->ctx.hash_page);
		release_hash_page(&io->ctx);
		auth_dec_pending_err(io, error);
		return;
	}
	SetPageUptodate(io->ctx.hash_page);
	unlock_page(io->ctx.hash_page);
	INIT_WORK(&io->work, kauthd_hash_read_complete_w);
	queue_work(cc->verify_queue, &io->work);
}

/*
 * Finished reading user data.  Now verify it.
 */
static int kauthd_udata_read_complete(struct dm_auth_io *io)
{
	struct auth_config *cc = io->cc;
	struct verify_context *ctx = &io->ctx;
	int err;

	WARN_ON(bio_data_dir(io->base_bio) != READ);
	auth_verify_init(cc, ctx, io->base_bio, io->io_sector);
	err = hash_next_user_page(cc, ctx);
	if (err < 0)
		return err;
	err = start_next_hash_read(io, cc, ctx);
	if (err < 0)
		return err;
	return SINPROGRESS;
}

static void kauthd_udata_read_complete_w(struct work_struct *work)
{
	struct dm_auth_io *io = container_of(work, struct dm_auth_io, work);
	int err = kauthd_udata_read_complete(io);

	if (err != SINPROGRESS)
		auth_dec_pending_err(io, err);
}

static void endio_udata(struct bio *bio)
{
	struct dm_auth_io *io = bio->bi_private;
	struct auth_config *cc = io->cc;
	int error = bio->bi_status;

	WARN_ON(bio_data_dir(bio) != READ);
	bio_put(bio);
	if (error) {
		auth_dec_pending_err(io, error);
		return;
	}
	INIT_WORK(&io->work, kauthd_udata_read_complete_w);
	queue_work(cc->verify_queue, &io->work);
}

/*
 * Work queue to run user I/O.
 */
static void kauthd_io_work(struct work_struct *work)
{
	struct dm_auth_io *io = container_of(work, struct dm_auth_io, work);
	struct bio *base_bio = io->base_bio;
	struct bio *bio;

	WARN_ON(bio_data_dir(io->base_bio) != READ);
	auth_inc_pending(io);
	bio = auth_alloc_bio(io, io->io_sector, base_bio->bi_iter.bi_size,
			bio_segments(base_bio), bio_iovec(base_bio), 0);
	if (likely(bio))
		generic_make_request(bio);
	else
		auth_dec_pending_err(io, -ENOMEM);
}

static struct bio *dm_auth_clone_bio(struct bio *bio_src, gfp_t gfp_mask,
		struct bio_set *bs)
{
	struct bvec_iter iter;
	struct bio_vec bv;
	struct bio *bio;

	/*
	 * Pre immutable biovecs, __bio_clone() used to just do a memcpy from
	 * bio_src->bi_io_vec to bio->bi_io_vec.
	 *
	 * We can't do that anymore, because:
	 *
	 *  - The point of cloning the biovec is to produce a bio with a biovec
	 *    the caller can modify: bi_idx and bi_bvec_done should be 0.
	 *
	 *  - The original bio could've had more than BIO_MAX_PAGES biovecs; if
	 *    we tried to clone the whole thing bio_alloc_bioset() would fail.
	 *    But the clone should succeed as long as the number of biovecs we
	 *    actually need to allocate is fewer than BIO_MAX_PAGES.
	 *
	 *  - Lastly, bi_vcnt should not be looked at or relied upon by code
	 *    that does not own the bio - reason being drivers don't use it for
	 *    iterating over the biovec anymore, so expecting it to be kept up
	 *    to date (i.e. for clones that share the parent biovec) is just
	 *    asking for trouble and would force extra work on
	 *    __bio_clone_fast() anyways.
	 */

	bio = bio_alloc_bioset(gfp_mask, bio_segments(bio_src), bs);
	if (!bio)
		return NULL;
	bio->bi_disk		= bio_src->bi_disk;
	bio->bi_opf		= bio_src->bi_opf;
	bio->bi_ioprio		= bio_src->bi_ioprio;
	bio->bi_write_hint	= bio_src->bi_write_hint;
	bio->bi_iter.bi_sector	= bio_src->bi_iter.bi_sector;
	bio->bi_iter.bi_size	= bio_src->bi_iter.bi_size;

	switch (bio_op(bio)) {
	case REQ_OP_DISCARD:
	case REQ_OP_SECURE_ERASE:
	case REQ_OP_WRITE_ZEROES:
		break;
	case REQ_OP_WRITE_SAME:
		bio->bi_io_vec[bio->bi_vcnt++] = bio_src->bi_io_vec[0];
		break;
	default:
		bio_for_each_segment(bv, bio_src, iter)
			bio->bi_io_vec[bio->bi_vcnt++] = bv;
		break;
	}

	if (bio_crypt_clone(bio, bio_src, gfp_mask) < 0)
		goto err_put;

	if (bio_integrity(bio_src) &&
	    bio_integrity_clone(bio, bio_src, gfp_mask) < 0)
		goto err_put;

	bio_clone_blkg_association(bio, bio_src);
	blkcg_bio_issue_init(bio);

	return bio;

err_put:
	bio_put(bio);
	return NULL;
}

/*
 * Allocate a struct bio to read from the lower device,
 * either to perform a user-initiated read, or to read
 * from the Merkle tree.
 */
static struct bio *auth_alloc_bio(struct dm_auth_io *io, sector_t sector,
	size_t size, int segments, struct bio_vec iovec, int is_hash_page)
{
	struct auth_config *cc = io->cc;
	struct bio *bio;
	struct bvec_iter iter;
	struct bio_vec bv;
	struct bio *base_bio = io->base_bio;
	int i = 0;

	bio = dm_auth_clone_bio(io->base_bio, GFP_NOIO, &cc->bs);
	if (unlikely(!bio))
		return NULL;
	bio->bi_private = io;
	bio->bi_disk = cc->dev->bdev->bd_disk;
	bio->bi_end_io = is_hash_page ? endio_hash : endio_udata;
	bio->bi_iter.bi_sector = cc->start_sector + sector;
	//DMR memcpy(bio->bi_io_vec, &iovec, segments * sizeof(struct bio_vec));
	bio->bi_vcnt = segments;
	bio->bi_iter.bi_size = size;
	bio->bi_iter.bi_idx = 0;
	if (!is_hash_page) {
		bio_for_each_segment(bv, base_bio, iter) {
			bio->bi_io_vec[i++] = bv;
			if (i == segments)
				break;
		}
	} else
	    memcpy(bio->bi_io_vec, &iovec, 1 * sizeof(struct bio_vec));

	return bio;
}

/*
 * Manage list of sticky MerkleHeaders, which persist after
 * the device is closed.
 */
static void set_sticky_merkle_header(struct auth_config *cc)
{
	dev_t dev = cc->dev->bdev->bd_dev;
	int i;

	for (i = 0;  i < MAX_STICKY_MERKLE_HEADERS;  i++) {
		if (sticky_merkle_headers[i].dev == dev ||
			sticky_merkle_headers[i].dev == 0) {
			sticky_merkle_headers[i].dev = dev;
			sticky_merkle_headers[i].mheader = cc->mheader;
			return;
		}
	}
	pr_warn("*** OUT OF SPACE: increase MAX_STICKY_MERKLE_HEADERS?\n");
}

static struct MerkleHeader *get_sticky_merkle_header(struct auth_config *cc)
{
	dev_t dev = disk_devt(cc->dev->bdev->bd_disk);
	int i;

	for (i = 0;  i < MAX_STICKY_MERKLE_HEADERS;  i++) {
		if (sticky_merkle_headers[i].dev == dev)
			return &sticky_merkle_headers[i].mheader;
	}
	return NULL;
}

/*
 * Create struct auth_config.
 */
static int auth_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
	struct auth_config *cc;
	unsigned long long tmpll;
	struct MerkleHeader *mheader;

	if (argc != 2) {
		ti->error = "Not enough arguments";
		return -EINVAL;
	}

	cc = kzalloc(sizeof(*cc), GFP_KERNEL);
	if (cc == NULL) {
		ti->error = "Cannot allocate transparent encryption context";
		return -ENOMEM;
	}

	if (bioset_init(&cc->bs, MIN_IOS, 0, BIOSET_NEED_BVECS))
		goto bad_device;

	/* (sscanf(argv[1], "%llu", &tmpll) != 1) */
	if (kstrtoull(argv[1], 0, &tmpll)) {
		ti->error = "Invalid device sector";
		goto bad_device;
	}
	cc->start_sector = tmpll;

	if (dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &cc->dev)) {
		ti->error = "Device lookup failed";
		goto bad_device;
	}

	cc->per_bio_data_size = ti->per_io_data_size =
		ALIGN(sizeof(struct dm_auth_io), ARCH_KMALLOC_MINALIGN);

	cc->io_queue = create_singlethread_workqueue("kauthd_io");
	if (!cc->io_queue) {
		ti->error = "Couldn't create kauthd io queue";
		goto bad_io_queue;
	}

	cc->verify_queue = create_singlethread_workqueue("kauthd");
	if (!cc->verify_queue) {
		ti->error = "Couldn't create kauthd queue";
		goto bad_verify_queue;
	}

#if 1 // FIXME
	ti->flush_supported = 1;
#else
	ti->num_flush_requests = 1;
#endif
	ti->private = cc;

	mheader = get_sticky_merkle_header(cc);
	if (mheader)
		cc->mheader = *mheader;
	return 0;

bad_verify_queue:
	destroy_workqueue(cc->io_queue);
bad_io_queue:
	dm_put_device(ti, cc->dev);
bad_device:
	kzfree(cc);
	return -EINVAL;
}

/*
 * Destroy struct auth_config.
 */
static void auth_dtr(struct dm_target *ti)
{
	struct auth_config *cc = (struct auth_config *) ti->private;

	destroy_workqueue(cc->io_queue);
	destroy_workqueue(cc->verify_queue);
	dm_put_device(ti, cc->dev);
	bioset_exit(&cc->bs);
	kzfree(cc);
}

/*
 * Is a struct MerkleHeader valid?
 */
static int valid_merkle_header(struct MerkleHeader *header)
{
	int level;

	if (header->mh_magic != MH_MAGIC ||
		header->mh_num_levels >= MH_MAX_LEVELS ||
		header->mh_header_size < sizeof(struct MerkleHeader) ||
		header->mh_page_size < SECTOR_SIZE || header->mh_page_size > 16*1024) {
		pr_err("invalid merkle header: magic %lx, %ld levels, header %ld, hash %ld, page %ld\n",
			(long)header->mh_magic, (long)header->mh_num_levels,
			(long)header->mh_header_size, (long)header->mh_hash_size,
			(long)header->mh_page_size);
		return 0;
	}
	if (header->mh_min_version > MH_VERSION) {
		pr_err("merkle header too new, version %ld",
			(long) header->mh_min_version);
		return 0;
	}
	for (level = 0;  level < header->mh_num_levels;  level++) {
		if (header->mh_level_pos[level] < header->mh_udata_size ||
			header->mh_level_pos[level] % header->mh_page_size ||
			header->mh_level_size[level] == 0 ||
			header->mh_level_size[level] % header->mh_page_size) {
			pr_err("invalid merkle header: level %d at %ld,%ld, udata %ld\n",
				level, (long) header->mh_level_pos[level],
				(long) header->mh_level_size[level],
				(long) header->mh_udata_size);
			return 0;
		}
	}
	return 1;
}

/*
 * Start user-initiated I/O.
 */
#if 1 // FIXME
static int auth_map(struct dm_target *ti, struct bio *bio)
#else
static int auth_map(struct dm_target *ti, struct bio *bio, union map_info *map_context)
#endif
{
	struct dm_auth_io *io;
	struct auth_config *cc = ti->private;

	if (bio_data_dir(bio) != READ)
		return -EACCES;
#if !defined(REQ_PREFLUSH)
	if (bio_empty_barrier(bio))
#elif !defined(REQ_DISCARD)
	if (unlikely(bio->bi_opf & REQ_PREFLUSH))
#else
	if (unlikely(bio->bi_opf & (REQ_PREFLUSH | REQ_DISCARD)))
#endif
	{
		bio->bi_disk = cc->dev->bdev->bd_disk;
		if (bio_sectors(bio))
			bio->bi_iter.bi_sector = cc->start_sector +
				dm_target_offset(ti, bio->bi_iter.bi_sector);
		return DM_MAPIO_REMAPPED;
	}
	if (!valid_merkle_header(&cc->mheader))
		return -EACCES;

	if ((bio->bi_iter.bi_sector << SECTOR_SHIFT) + bio->bi_iter.bi_size
	> cc->mheader.mh_udata_size) {
		pr_err("transfer %ld,%ld beyond udata %ld\n",
			(long) bio->bi_iter.bi_sector, (long) bio->bi_iter.bi_size,
			(long) cc->mheader.mh_udata_size);
		return -ENXIO;
	}

	io = dm_per_bio_data(bio, cc->per_bio_data_size);
	auth_io_init(io, cc, bio, dm_target_offset(ti, bio->bi_iter.bi_sector));

	INIT_WORK(&io->work, kauthd_io_work);
	queue_work(cc->io_queue, &io->work);
	return DM_MAPIO_SUBMITTED;
}

static void auth_status(struct dm_target *ti, status_type_t status_type,
	unsigned int status_flags, char *result, unsigned int maxlen)
{
	//struct auth_config *cc = (struct auth_config *) ti->private;

	switch (status_type) {
	case STATUSTYPE_INFO:
		result[0] = '\0';
		break;

	case STATUSTYPE_TABLE:
		result[0] = '\0';
		break;
	}
}

int auth_set_merkle(struct block_device *bdev, fmode_t mode, unsigned int cmd,
			unsigned long arg)
{
	struct mapped_device *md = bdev->bd_disk->private_data;
	struct dm_target *tgt;
	struct dm_table *map;
	int srcu_idx, r;
	struct MerkleHeader mheader;
	struct auth_config *cc;

	r = -ENOTTY;

	if (copy_from_user(&mheader, (void __user *) arg, sizeof(mheader)))
		return -EFAULT;

	if (!valid_merkle_header(&mheader))
		return -EINVAL;

	map = dm_get_live_table(md, &srcu_idx);
	if (!map || !dm_table_get_size(map))
		goto out;

	tgt = dm_table_get_target(map, 0);
	cc = tgt->private;

	r = 0;
	cc->mheader = mheader;
	if (cmd == DMAUTH_IOCTL_SET_MERKLE_STICKY)
		set_sticky_merkle_header(cc);

out:
	dm_put_live_table(md, srcu_idx);
	return r;
}

int auth_set_filename(struct block_device *bdev, fmode_t mode, unsigned int cmd,
			unsigned long arg)
{
	struct mapped_device *md = bdev->bd_disk->private_data;
	struct dm_target *tgt;
	struct dm_table *map;
	int srcu_idx, r;
	struct MerkleFile mfile;
	struct auth_config *cc;

	r = 0;

	map = dm_get_live_table(md, &srcu_idx);
	if (!map || !dm_table_get_size(map)) {
		r = -EINVAL;
		goto out;
	}

	tgt = dm_table_get_target(map, 0);
	cc = tgt->private;

	if (copy_from_user(&mfile, (void __user *) arg, sizeof(mfile))) {
		r = -EFAULT;
		goto out;
	}

	memcpy(cc->filename, mfile.name, sizeof(cc->filename));
	if (corrupted_magic) {
		corrupted_magic = 0;
		memset(corrupted_file, 0, sizeof(corrupted_file));
	}

out:
	dm_put_live_table(md, srcu_idx);
	return r;
}

static int auth_prepare_ioctl(struct dm_target *ti, struct block_device **bdev)
{
	struct auth_config *cc = ti->private;
	struct dm_dev *dev = cc->dev;

	*bdev = dev->bdev;

	/*
	 * Only pass ioctls through if the device sizes match exactly.
	 */
	if (cc->start_sector ||
	    ti->len != i_size_read(dev->bdev->bd_inode) >> SECTOR_SHIFT)
		return 1;
	return 0;
}

static int auth_ioctl_fn(struct dm_target *ti,
			 unsigned int cmd, unsigned long arg)
{
	struct auth_config *cc = ti->private;

	switch (cmd) {
	case DMAUTH_IOCTL_SET_MERKLE:
	case DMAUTH_IOCTL_SET_MERKLE_STICKY: {
		struct MerkleHeader mheader;

		if (copy_from_user(&mheader,
		    (void __user *) arg, sizeof(mheader)))
			return -EFAULT;
		if (!valid_merkle_header(&mheader))
			return -EINVAL;
		cc->mheader = mheader;
		if (cmd == DMAUTH_IOCTL_SET_MERKLE_STICKY)
			set_sticky_merkle_header(cc);
		return 0; }
	case DMAUTH_IOCTL_SET_FILENAME: {
		struct MerkleFile mfile;

		if (copy_from_user(&mfile, (void __user *)arg, sizeof(mfile)))
			return -EFAULT;
		memcpy(cc->filename, mfile.name, sizeof(cc->filename));
		if (corrupted_magic) {
			corrupted_magic = 0;
			memset(corrupted_file, 0, sizeof(corrupted_file));
		}
		return 0; }
	default:
		return __blkdev_driver_ioctl(cc->dev->bdev,
					     cc->dev->mode, cmd, arg);
	}
}

static int auth_iterate_devices(struct dm_target *ti,
				iterate_devices_callout_fn fn, void *data)
{
	struct auth_config *cc = ti->private;

	return fn(ti, cc->dev, cc->start_sector, ti->len, data);
}

static void auth_io_hints(struct dm_target *ti, struct queue_limits *limits)
{
	limits->max_segment_size = PAGE_SIZE;
}

static struct target_type auth_target = {
	.name     =  "auth",
	.version  = {1, 2, 0},
	.module   = THIS_MODULE,
	.ctr      = auth_ctr,
	.dtr      = auth_dtr,
	.map      = auth_map,
	.status   = auth_status,
	.prepare_ioctl = auth_prepare_ioctl,
	.io_hints = auth_io_hints,
	.ioctl = auth_ioctl_fn,
	.iterate_devices = auth_iterate_devices,
};

static int dm_auth_proc_show(struct seq_file *m, void *v)
{
	if (corrupted_magic == CORRUPTED_MAGIC) {
		seq_puts(m, "The corrupted file that is marked for deletion:\n");
		seq_printf(m, "%s\n", corrupted_file);
	}
	return 0;
}

static int dm_auth_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, dm_auth_proc_show, NULL);
}

static const struct file_operations dm_auth_proc_fops = {
	.open		= dm_auth_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init dm_auth_init(void)
{
	int r;
	struct proc_dir_entry *p;

	r = dm_register_target(&auth_target);
	if (r < 0)
		DMERR("register failed %d", r);

	p = proc_create(CORRUPTED_FILE_PROC, 0000, NULL, &dm_auth_proc_fops);
	if (!p)
		return -ENOMEM;

	return r;
}

static void __exit dm_auth_exit(void)
{
	dm_unregister_target(&auth_target);
	remove_proc_entry(CORRUPTED_FILE_PROC, NULL);
}

#ifdef DMAUTH_DEBUG
static int __init setup_dmauth_debug(char *str)
{
	if (kstrtoint(str, 0, &dmauth_debug))
		return 0;
	return 1;
}
__setup("dmauth_debug=", setup_dmauth_debug);
#endif

module_init(dm_auth_init);
module_exit(dm_auth_exit);

MODULE_AUTHOR("Mark Nudelman");
MODULE_DESCRIPTION(DM_NAME " target for authentication");
