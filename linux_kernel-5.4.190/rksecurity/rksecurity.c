// #include <linux/rksecurity.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/security.h>
#include <linux/init_task.h>
#include <linux/mnt_namespace.h>
#include <linux/namei.h>

#define RK_GLOBAL_UID(x) from_kuid_munged(&init_user_ns, (x))
#define RK_GLOBAL_GID(x) from_kgid_munged(&init_user_ns, (x))

static int is_roku_sdk_launcher(struct task_struct *t) {
	if (t->group_leader) t = t->group_leader;
	return (strcmp(t->comm, "SdkLauncher") == 0);
}

static int proc_is_chrooted(struct task_struct *p) {
	if (task_pid_nr(p) > 1 && p && p->fs && (init_task.fs->root.dentry != p->fs->root.dentry))
		return 1;
	return 0;
}

static void print_process_info(const char *msg, struct task_struct *task) {
	const struct cred *cred;
	uid_t uid;
	gid_t gid;
	pid_t pid;
	cred = get_task_cred(task);
	uid = RK_GLOBAL_UID(cred->uid);
	gid = RK_GLOBAL_GID(cred->gid);
	pid = task->pid;
	if (msg) {
		printk(KERN_ERR "%s %s pid:%d uid:%d gid=%d\n", msg, task->comm, pid, uid, gid );
	} else {
		printk(KERN_ERR "%s pid:%d uid:%d gid=%d\n", task->comm, pid, uid, gid );
	}
}

int have_same_chroot(const struct task_struct *task1,
					 const struct task_struct *task2) {
	if (task1->fs->root.dentry == task2->fs->root.dentry)
		return 1;
	return 0;
}

// Symlink restriction.
int rksec_handle_follow_link(const struct inode *parent,
						  const struct inode *inode,
						  const struct dentry *dentry,
						  const struct vfsmount *mnt) {
	// Do not allow following symlinks from nosuid media if process has CHROOT capability
	if ((mnt->mnt_flags & MNT_NOSUID) &&
		(capable(CAP_SYS_CHROOT) || is_roku_sdk_launcher(current))) {
		return -EPERM;
	}
	return 0;
}

// Usermode helper restrictions. Add allowed UMH to this function
int	rksec_allow_usermode_helper(const char *path) {
	if (!strncmp(path, "/sbin/ltcore", 12)) {
		return 1;
	}
	printk(KERN_ALERT "rksec: denied exec of untrusted usermode helper %s\n", path);
	return 0;
}

int rksec_allow_shmat(const struct task_struct *shm_creator,int shmid) {
	if (likely(!proc_is_chrooted(current))) {
		return 1;
	}

	if (capable(CAP_SYS_CHROOT)) {
		return 1;
	}

	if (!have_same_chroot(shm_creator, current)) {
		rcu_read_unlock();
		print_process_info("rksec: denied attach of shared memory outside chroot by:", current);
		return 0;
	}
	return 1;
}

#define RESTRICT_UID_TRANSITIONS
#define RESTRICT_GID_TRANSITIONS

int rksec_allow_uid_transition(kuid_t real, kuid_t effective, kuid_t fs) {
#ifdef RESTRICT_UID_TRANSITIONS
	const struct cred *cred;
	uid_t uid;
	cred = get_task_cred(current);
    /* Find UID in global namespace */
	uid = RK_GLOBAL_UID(cred->uid);
	if((uid != 0) && (real.val == 0 || effective.val == 0 || fs.val == 0)) {
		printk(KERN_ALERT "Change to uid (r=%d,e=%d,f=%d) denied\n", real.val,effective.val,fs.val);
		print_process_info("rksec: denied uid transition by:", current);
		return 0;
	}
#endif
	return 1;
}

int rksec_allow_gid_transition(kgid_t real, kgid_t effective, kgid_t fs) {
#ifdef RESTRICT_GID_TRANSITIONS
	const struct cred *cred;
	gid_t gid;
	cred = get_task_cred(current);
    /* Find GID in global namespace */
	gid = RK_GLOBAL_GID(cred->gid);
	if((gid != 0) && (real.val == 0 || effective.val == 0 || fs.val == 0)) {
		printk(KERN_ALERT "Change to gid (r=%d,e=%d,f=%d) denied\n", real.val,effective.val,fs.val);
		print_process_info("rksec: denied gid transition by:", current);
		return 0;
	}
#endif
	return 1;
}

const char *captab[] = {
	"CAP_CHOWN",
	"CAP_DAC_OVERRIDE",
	"CAP_DAC_READ_SEARCH",
	"CAP_FOWNER",
	"CAP_FSETID",
	"CAP_KILL",
	"CAP_SETGID",
	"CAP_SETUID",
	"CAP_SETPCAP",
	"CAP_LINUX_IMMUTABLE",
	"CAP_NET_BIND_SERVICE",
	"CAP_NET_BROADCAST",
	"CAP_NET_ADMIN",
	"CAP_NET_RAW",
	"CAP_IPC_LOCK",
	"CAP_IPC_OWNER",
	"CAP_SYS_MODULE",
	"CAP_SYS_RAWIO",
	"CAP_SYS_CHROOT",
	"CAP_SYS_PTRACE",
	"CAP_SYS_PACCT",
	"CAP_SYS_ADMIN",
	"CAP_SYS_BOOT",
	"CAP_SYS_NICE",
	"CAP_SYS_RESOURCE",
	"CAP_SYS_TIME",
	"CAP_SYS_TTY_CONFIG",
	"CAP_MKNOD",
	"CAP_LEASE",
	"CAP_AUDIT_WRITE",
	"CAP_AUDIT_CONTROL",
	"CAP_SETFCAP",
	"CAP_MAC_OVERRIDE",
	"CAP_MAC_ADMIN",
	"CAP_SYSLOG",
	"CAP_WAKE_ALARM",
    "CAP_BLOCK_SUSPEND",
    "CAP_AUDIT_READ"
};

int max_caps = sizeof(captab)/sizeof(captab[0]);

#define RK_CHROOT_DISALLOWED_CAPS {{ \
	CAP_TO_MASK(CAP_LINUX_IMMUTABLE) | CAP_TO_MASK(CAP_NET_ADMIN)	| \
	CAP_TO_MASK(CAP_SYS_MODULE) | CAP_TO_MASK(CAP_SYS_RAWIO) 		| \
	CAP_TO_MASK(CAP_SYS_PACCT) | CAP_TO_MASK(CAP_SYS_ADMIN)			| \
	CAP_TO_MASK(CAP_SYS_BOOT) | CAP_TO_MASK(CAP_SYS_TIME)			| \
	CAP_TO_MASK(CAP_NET_RAW) | CAP_TO_MASK(CAP_SYS_TTY_CONFIG)		| \
	CAP_TO_MASK(CAP_IPC_OWNER) | CAP_TO_MASK(CAP_SETFCAP),			\
	CAP_TO_MASK(CAP_SYSLOG) | CAP_TO_MASK(CAP_MAC_ADMIN) 			| \
	CAP_TO_MASK(CAP_WAKE_ALARM) | CAP_TO_MASK(CAP_BLOCK_SUSPEND) 	| \
 	CAP_TO_MASK(CAP_AUDIT_READ) }}

int rksec_allow_capability(int cap, unsigned int opt) {
	int allowed = 1;
	kernel_cap_t chroot_caps = RK_CHROOT_DISALLOWED_CAPS;

	if (likely(!proc_is_chrooted(current))) {
		return allowed;
	}

	if (cap_raised(chroot_caps, cap)) {
		static const kuid_t BT_UID = KUIDT_INIT(498);
		const kuid_t euid = current_euid();

		if (euid.val == BT_UID.val) {
			// allow CAP_NET_ADMIN and CAP_NET_RAW to BT
			kernel_cap_t btcap = {{ CAP_TO_MASK(CAP_NET_RAW)    | \
									CAP_TO_MASK(CAP_NET_ADMIN), 0}};
			if (!cap_raised(btcap, cap))
				allowed = 0;
		} else {
			allowed = 0;
		}
	}

	if (allowed == 0 && opt != CAP_OPT_NOAUDIT) {
		printk(KERN_ERR "rksec:denying cap %s\n", captab[cap]);
		print_process_info("rksec: denied capability to process :", current);
	}

	return allowed;
}

#define RESTRICT_HIDUSB_DEVICES
#define HIDUSB_ID(vid, pid) ((((__u32)vid) << 16) | ((__u32)pid))

#ifdef RESTRICT_HIDUSB_DEVICES
static __u32 hid_whitelist[] = {
	HIDUSB_ID(0x1d5a, 0xc182) //NEEDS UPDATE
};

static bool hid_is_whitelisted(__u16 vid, __u16 pid)
{
	__u32 id = HIDUSB_ID(vid, pid);
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(hid_whitelist); ++i) {
		if (hid_whitelist[i] == id)
			return true;
	}
	return false;
}
#endif

int rksec_allow_usb_device(const char *bus_name, __u16 vid, __u16 pid) {
#ifdef RESTRICT_HIDUSB_DEVICES
	if (hid_is_whitelisted(vid, pid)) {
		return 1;
	}

	if (bus_name && strncmp(bus_name, "ozwpan", 6) == 0) {
		// allow usb device on ozwpan bus
		return 1;
	}
	printk(KERN_ERR "rksec: denied USB device");
	return 0;
#endif
	return 1;
}

int rksec_allow_fifo(const struct dentry *dentry, const struct vfsmount *mnt,
					 const struct dentry *dir, const int flag, const int acc_mode)
{
	const struct cred *cred = current_cred();

	if (S_ISFIFO(dentry->d_inode->i_mode) &&
		!(flag & O_EXCL) && (dir->d_inode->i_mode & S_ISVTX) &&
		!uid_eq(dentry->d_inode->i_uid, dir->d_inode->i_uid) &&
		!uid_eq(cred->fsuid, dentry->d_inode->i_uid)) {
		if (!inode_permission(dentry->d_inode, acc_mode)) {
			printk(KERN_ERR "rksec: fifo=%s currentcred->fsuid=%d  dentry.uid=%d",
				dentry->d_name.name, __kuid_val(cred->fsuid), __kuid_val(dentry->d_inode->i_uid));
			print_process_info("rksec: denied fifo access to: ", current);
		}
		return 0;
	}
	return 1;
}

int rksec_allow_mknod(const struct dentry *dentry, const int mode) {
	if (likely(!proc_is_chrooted(current))) {
		return 1;
	}
	/* process is chrooted. Do not let it create nodes for raw device through
	   which it can break out of chroot or exploit weakness in a driver. */
	if (!S_ISFIFO(mode) && !S_ISREG(mode)) {
		printk(KERN_ERR "rksec: process attempted to mknod %s", dentry->d_name.name);
		print_process_info("rksec: denied mknod creation: ", current);
		return 0;
	}
	return 1;
}

int rksec_allow_sysctl(const int op) {
	if (likely(!proc_is_chrooted(current))) {
		return 1;
	}
	/* process is chrooted. Do not let it write to sysctl */
	if (op & MAY_WRITE) {
		print_process_info("rksec: deny writing to sysfs: ", current);
		return 0;
	}
	return 1;
}
