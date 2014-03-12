#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "mmpeephole.h"

void
print_mmap_handler(int mmph, const char *fname)
{
	struct mmph_ioc_sym_param ioc_param;
	int testfd;
	long ret;
	testfd = open(fname, O_RDONLY);
	if (testfd == -1) {
		fprintf(stderr, "open %s: %s\n", fname, strerror(errno));
		return;
	}
	ioc_param.param = testfd;
	ret = ioctl(mmph, MMPH_IOC_CMD_SYM_MMAP, &ioc_param);
	if (ret == -1) {
		fprintf(stderr, "ioctl MMPH_IOC_CMD_MMAP (file=%s): %s\n",
		        fname, strerror(errno));
		goto close_file;
	}
	printf("%s mmap operation = %s\n", fname, ioc_param.name);
close_file:
	close(testfd);
}

int
main(int argc, char *argv[])
{
	int fd;
	fd = open("/dev/mmpeephole", O_RDONLY);
	if (fd == -1) {
		perror("open mmpeephole device");
		exit(1);
	}
	while (*++argv) {
		print_mmap_handler(fd, *argv);
	}
	close(fd);
	return 0;
}
