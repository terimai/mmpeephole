KDIR=/lib/modules/`uname -r`/build
.PHONY: all kmod kmodclean tools toolsclean
all: kmod tools
clean: kmodclean clean

kmod:
	$(MAKE) -C $(KDIR) modules M=`pwd`/kmod

tools:
	$(MAKE) -C tools all

kmodclean:
	$(MAKE) -C $(KDIR) clean M=`pwd`/kmod

clean:
	$(MAKE) -C tools clean
