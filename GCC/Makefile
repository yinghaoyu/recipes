CFLAGS:= -m32
#CFLAGS+= -O0
#CFLAGS+= -static
#CFLAGS+= -fno-asynchronous-unwind-tables
#CFLAGS+= -fno-pic
#CFLAGS+= -mpreferred-stack-boundary=2
#CFLAGS+= -fno-stack-protector
#CFLAGS+= -fno-builtin
#CFLAGS+= -fomit-frame-pointer
CFLAGS+= -fverbose-asm
CFLAGS+= --verbose

CC:=gcc

DEBUG:=-g

.PHONY: hello.i
hello.i: hello.c
	$(CC) $(CFLAGS) $(DEBUG) -E $< -o $@

.PHONY: hello.s
hello.s: hello.i
	$(CC) $(CFLAGS) $(DEBUG) -S $< -o $@

.PHONY: hello.o
hello.o: hello.s
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@

.PHONY: hello
hello: hello.o
	$(CC) $(CFLAGS) $(DEBUG) $< -o $@

.PHONY: clean
clean:
	rm -rf *.i
	rm -rf *.s
	rm -rf *.o
	rm -rf *.bin
	rm -rf *.out
	rm -rf hello
