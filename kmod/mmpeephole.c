#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <asm/uaccess.h>

#include "mmpeephole.h"

static long mmph_ioctl(struct file *, unsigned int, unsigned long);
static int mmph_open(struct inode *, struct file *);
static int mmph_flush(struct file *, fl_owner_t);
static int mmph_release(struct inode *, struct file *);

static int mmph_mmap(struct file *, struct vm_area_struct *);

static long mmph_mmap_handler_name(uint64_t, char *, size_t);
static long mmph_region_vm_ops_name(uint64_t, char *, size_t);

static struct file_operations mmph_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = mmph_ioctl,
	.mmap = mmph_mmap,
	.open = mmph_open,
	.flush = mmph_flush,
	.release = mmph_release,
};

static struct miscdevice mmph_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = MMPH_DRV_NAME,
	.fops = &mmph_fops,
};

struct mmph_cmd_entry {
	int cmd;
	long (*handler)(uint64_t, char *, size_t);
};

static struct mmph_cmd_entry mmph_cmd_table[] = {
	{ MMPH_IOC_CMD_MMAP, mmph_mmap_handler_name },
	{ MMPH_IOC_CMD_VM_OPS, mmph_region_vm_ops_name},
	{ 0, NULL },
};

static int
mmph_mmap(struct file *filp, struct vm_area_struct *vma)
{
	return -EFAULT;
}

static long
mmph_mmap_handler_name(uint64_t param, char *namebuf, size_t size)
{
	struct file *f;
	int fd = (int)param;
	f = fget(fd);
	if (f == NULL) {
		printk(KERN_INFO MMPH_DRV_NAME
		       ": can't get file (fd = %d)\n", fd);
		return -EINVAL;
	}
	if (f->f_op == NULL) {
		printk(KERN_INFO MMPH_DRV_NAME
		       ": f_op (fd = %d) is NULL\n", fd);
		namebuf[0] = '\0';
	} else {
		snprintf(namebuf, size, "%ps", f->f_op->mmap);
	}
	fput(f);
	return 0;
}

static long
mmph_region_vm_ops_name(uint64_t addr, char *namebuf, size_t size)
{
	struct vm_area_struct *vma;
	vma = find_vma(current->mm, addr);
	if (vma == NULL) {
		printk(KERN_INFO MMPH_DRV_NAME
		       ": virtual address %p is invalid\n", (void *)addr);
		return -EINVAL;
	}
	snprintf(namebuf, size, "%pS", vma->vm_ops);
	return 0;
}

static long
mmph_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long retval, copystatus;
	uint64_t param;
	struct mmph_cmd_entry *e;
	long (*h)(uint64_t, char *, size_t) = NULL;
	char namebuf[MMPH_NAME_LEN];

	struct mmph_ioc_param *uptr = (struct mmph_ioc_param *)arg;

	copystatus = get_user(param, &uptr->param);
	if (copystatus != 0) {
		return -EFAULT;
	}

	/* search handler */
	for (e = mmph_cmd_table; e->cmd != 0 && e->handler != NULL; ++e) {
		if (cmd == e->cmd) {
			h = e->handler;
		}
	}
	if (h == NULL) {
		printk(KERN_INFO MMPH_DRV_NAME
		       ": ioctl(cmd %d) is not found\n", cmd);
		return -EINVAL;
	}
	retval = h(param, namebuf, sizeof(namebuf));

	copystatus = copy_to_user(&uptr->name, namebuf, sizeof(namebuf));
	if (copystatus != 0) {
		return -EFAULT;
	}
	return retval;
}

void
mmph_exit(void)
{
	misc_deregister(&mmph_dev);
}

int
mmph_init(void)
{
	int ret;
	ret = misc_register(&mmph_dev);
	if (ret != 0) {
		printk(KERN_WARNING MMPH_DRV_NAME ": failed to register "
		       "misc device (returned %d)\n", ret);
	}
	return ret;
}

static int
mmph_open(struct inode *ino, struct file *filp)
{
	printk(KERN_DEBUG "%s(%p, %p) (current = %p)\n", __func__,
	       ino, filp, current);
	return 0;
}
static int
mmph_release(struct inode *ino, struct file *filp)
{
	printk(KERN_DEBUG "%s(%p, %p) (current = %p)\n", __func__,
	       ino, filp, current);
	return 0;
}
static int
mmph_flush(struct file *filp, fl_owner_t id)
{
	printk(KERN_DEBUG "%s(%p, %p) (current = %p)\n", __func__,
	       filp, id, current);
	return 0;
}

module_init(mmph_init);
module_exit(mmph_exit);
MODULE_LICENSE("GPL");
