#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <crypto/hash.h>
#include <linux/scatterlist.h>

#define GET_AIMAGE_FIELD(_name)		\
	aimage_field(sbuf, user_bufs,	\
	offsetof(struct aimage_header_t, _name), &_name)

#define SIZEOF_AIMAGE_HEADER	256
#define IMG_MAGIC		0x41676d69
#define IMG_MAGIC2		0x43634d52
#define IMG_TYPE_WIFI_FILE_SYSTEM	(0x02)
#define IMG_TYPE_APPFS_CRAMFS		(0x0D)
#define IMG_TYPE_AUTH_SIGNATURE		(0x16)
#define IMG_TYPE_CRAMFS_AUTH		(0x101)

#define CRAMFS_SUPER_SIZE  512
#define CRAMFS_AUTH_SCRATCH_SIZE  (2 * CRAMFS_SUPER_SIZE)

static int custom_pkg_sideload;

extern unsigned char roothash[];

/* Yet another partial definition of aimage_header_t. */
struct aimage_header_t {
	unsigned int v1;
	unsigned int v2;
	unsigned int magic;
	unsigned int magic2;
	unsigned int re;
	unsigned int pl;
	unsigned int aimage_type;
	unsigned int data_length;
	unsigned int dl;
	unsigned int data_start_offset;
	unsigned int data_link_address;
	unsigned int data_entry_point_offset;
	unsigned int flags;
	unsigned int build_time;
	unsigned int build_host_offset;
	unsigned int signature[32];
	unsigned int hash[5];
	unsigned int boot_version;
	unsigned int subtype;
};

static int aimage_field(void *vp, int user_bufs,
			int offset, unsigned int *p_value)
{
	struct aimage_header_t __user *up;
	unsigned char __user *ufield;
	unsigned char *field;

	if (user_bufs) {
		up = vp;
		ufield = ((unsigned char __user *)up) + offset;
		if (copy_from_user(p_value, ufield, sizeof(unsigned int)))
			return -EFAULT;
	} else {
		field = ((unsigned char *)vp) + offset;
		*p_value = *(unsigned int *)field;
	}
	return 0;
}

static ssize_t file_read(struct file *file, void *data, int datalen, loff_t pos)
{
	vfs_llseek(file, pos, SEEK_SET);
	return vfs_read(file, data, datalen, &pos);
}

/* Read authdata (MerkleHeader) from an open file descriptor. */
static int cramfs_get_authdata2(struct file *file, unsigned int wtype,
	unsigned int wsubtype, void *data, int datalen,
	void *sbuf, int sbuflen, int user_bufs)
{
	off_t pos = 0;
	unsigned int data_length = 0;
	unsigned int aimage_type;
	unsigned int subtype;
	unsigned int data_start_offset;
	unsigned int magic, magic2;
	int partition_offset = -1;
	int err;
	ssize_t nread;

	for (pos = 0; ;  pos += data_length) {
		nread = file_read(file, sbuf, SIZEOF_AIMAGE_HEADER, pos);
		if (nread != SIZEOF_AIMAGE_HEADER) {
			pr_err("auth read error [a] %zd/%d\n",
				nread, SIZEOF_AIMAGE_HEADER);
			return -EIO;
		}

		err = GET_AIMAGE_FIELD(magic);
		if (err < 0)
			return err;
		err = GET_AIMAGE_FIELD(magic2);
		if (err < 0)
			return err;

		if (magic != IMG_MAGIC || magic2 != IMG_MAGIC2) {
			pr_err("invalid magic at %lx: %x %x\n",
			       (long)pos, magic, magic2);
			return -EINVAL;
		}

		err = GET_AIMAGE_FIELD(aimage_type);
		if (err < 0)
			return err;
		err = GET_AIMAGE_FIELD(subtype);
		if (err < 0)
			return err;
		err = GET_AIMAGE_FIELD(data_length);
		if (err < 0)
			return err;
		err = GET_AIMAGE_FIELD(data_start_offset);
		if (err < 0)
			return err;

		pr_info("img info: %d %d %d\n",
			aimage_type, data_length, data_start_offset);
		if (partition_offset == -1) {
			/* We haven't found wtype yet. */
			if (aimage_type == wtype) { /* Here it is. */
				if (wtype != IMG_TYPE_WIFI_FILE_SYSTEM ||
				   (wtype == IMG_TYPE_WIFI_FILE_SYSTEM &&
				    wsubtype == subtype)) {
					/* Check subtype if needed. */
					partition_offset =
						pos + data_start_offset;
				}
			}
		} else if (aimage_type == IMG_TYPE_CRAMFS_AUTH) {
			/* Read the Merkle header. */
			nread = file_read(file, data,
					  datalen, pos + data_start_offset);
			if (nread != datalen) {
				pr_err("auth read error [c] %zd/%d\n",
				       nread, datalen);
				return -EIO;
			}
			break;
		}
	}

	return partition_offset;
}

int cramfs_get_authdata(struct file *file, unsigned int wtype,
	unsigned int wsubtype, void *data,
	int datalen, void *sbuf, int sbuflen)
{
	int user_bufs = (sbuf != NULL);
	int sbufsize = CRAMFS_SUPER_SIZE;
	struct crypto_shash *tfm;
	struct shash_desc *desc = 0;
	int descsize;
	unsigned char const *stored_hash, *stored_hash2 = NULL;
	unsigned char calc_hash[16];
	unsigned int digestsize;
	void *udata = 0;
	int position;
	int err = 0;

	pr_info("%s %p %d\n", __func__, sbuf, sbuflen);
	if (!user_bufs) {
		sbuf = (void *)kmalloc(sbufsize, GFP_KERNEL);
		if (!sbuf)
			return -ENOMEM;
		sbuflen = sbufsize;
	}
	if (sbuflen < sbufsize)
		return -EINVAL;
	position = cramfs_get_authdata2(file, wtype, wsubtype,
		data, datalen, sbuf, sbuflen, user_bufs);
	pr_info("img position: %d\n", position);

	if (!user_bufs)
		kfree(sbuf);
	if (position < 0)
		return position;

	tfm = crypto_alloc_shash("md5", 0, 0);
	if (IS_ERR(tfm))
		return PTR_ERR(tfm);

	digestsize = crypto_shash_digestsize(tfm);
	WARN_ON(digestsize != sizeof(calc_hash));

	descsize = sizeof(struct shash_desc) + crypto_shash_descsize(tfm);
	desc = kzalloc(descsize, GFP_KERNEL);
	if (!desc) {
		err = -ENOMEM;
		goto out;
	}
	desc->tfm = tfm;

	err = crypto_shash_init(desc);
	if (err < 0)
		goto out;

	if (!user_bufs) {
		udata = data;
	} else {
		udata = kmalloc(datalen, GFP_KERNEL);
		if (copy_from_user(udata, data, datalen)) {
			err = -EFAULT;
			goto out;
		}
	}

	crypto_shash_digest(desc, udata, datalen, calc_hash);

	if (wtype == IMG_TYPE_APPFS_CRAMFS) {
		stored_hash = &roothash[sizeof(calc_hash)];
	} else if (wtype == IMG_TYPE_WIFI_FILE_SYSTEM) {
		stored_hash = &roothash[4 * sizeof(calc_hash)];
		stored_hash2 = &roothash[5 * sizeof(calc_hash)];
	} else {
		stored_hash = &roothash[0];
	}

	if (memcmp(stored_hash, calc_hash, sizeof(calc_hash))) {
		if (!stored_hash2 ||
		    memcmp(stored_hash2, calc_hash, sizeof(calc_hash))) {
			/* allow custom package hash fail on dev units */
			/* or with custom_pkg_sideload permission*/
			/* all other types require hash match */
			if ((strncmp(saved_command_line, "dev=1 ", 6) &&
			     !custom_pkg_sideload) ||
			     wtype != IMG_TYPE_APPFS_CRAMFS)
				err = -EKEYREJECTED;
		}
	}

	err = 0;
out:
	if (user_bufs && udata)
		kfree(udata);
	crypto_free_shash(tfm);
	kfree(desc);

	if (err < 0)
		return err;

	pr_info("%s success: %d\n", __func__, position);
	return position;
}
EXPORT_SYMBOL(cramfs_get_authdata);
