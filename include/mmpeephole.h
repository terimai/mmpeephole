#ifndef MMPEEPHOLE_H_INCLUDE_
#define MMPEEPHOLE_H_INCLUDE_

#include <linux/ioctl.h>

#define MMPH_NAME_LEN 256 /* >= KSYM_SYMBOL_LEN */
struct mmph_ioc_param {
	uint64_t param;
	char name[MMPH_NAME_LEN];
};

#define MMPH_DRV_NAME "mmpeephole"

#define MMPH_IOC_CMD_MMAP _IOWR('m', 0x1, struct mmph_ioc_param)
#define MMPH_IOC_CMD_VM_OPS _IOWR('m', 0x2, struct mmph_ioc_param)
#define MMPH_IOC_CMD_VM_FAULT _IOWR('m', 0x3, struct mmph_ioc_param)

#endif /*MMPEEPHOLE_H_INCLUDE_*/
