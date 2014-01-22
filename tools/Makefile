CPPFLAGS=-I../include
CFLAGS=-g -O2
TOOLS= tool_mmap
.PHONY: all clean
all: $(TOOLS)

tool_mmap: tool_mmap.c

clean:
	$(RM) $(TOOLS) *.o core*
