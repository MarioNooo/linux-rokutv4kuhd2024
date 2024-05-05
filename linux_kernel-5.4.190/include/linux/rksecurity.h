#ifndef RK_SECURITY_H
#define RK_SECURITY_H
#include <linux/fs.h>


int rksec_handle_follow_link(const struct inode *parent,
                          const struct inode *inode,
                          const struct dentry *dentry,
                          const struct vfsmount *mnt);
int rksec_allow_usermode_helper(const char *path);
int rksec_allow_shmat(const struct task_struct *shm_creator,int shmid);
int rksec_allow_uid_transition(kuid_t real, kuid_t effective, kuid_t fs);
int rksec_allow_gid_transition(kgid_t real, kgid_t effective, kgid_t fs);
int rksec_allow_capability(int cap, unsigned int opts);
int rksec_allow_usb_device(const char *bus_name, __u16 vid, __u16 pid);
int rksec_allow_fifo(const struct dentry *dentry, const struct vfsmount *mnt,
		const struct dentry *dir, const int flag, const int acc_mode);
int rksec_allow_mknod(const struct dentry *dentry, const int mode);
int rksec_allow_sysctl(const int op);
#endif