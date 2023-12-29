INC := $(shell pkg-config --cflags bash 2> /dev/null || \
               printf '%s\n' "-I/usr/include/bash/include")

all: hello.so myprint.so

hello.so: hello.c
	$(CC) $(INC) -O3 -Wall -shared -fPIC -o $@ $<
	strip -s $@ 2> /dev/null && sstrip -z $@ 2> /dev/null || true

myprint.so: myprint.c
	$(CC) $(INC) -O3 -Wall -shared -fPIC -o $@ $<
	strip -s $@ 2> /dev/null && sstrip -z $@ 2> /dev/null || true

.PHONY: test
test: hello.so myprint.so
	@bash -xc 'enable -f ./hello.so hello && builtin hello'
	@bash -xc 'enable -f ./myprint.so myprint && builtin myprint -f "%s\n" "Hello world!"'

.PHONY: clean
clean:
	@rm -f hello.so myprint.so
