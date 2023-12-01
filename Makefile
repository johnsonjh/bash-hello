INC := $(shell pkg-config --cflags bash 2> /dev/null || \
               printf '%s\n' "-I/usr/include/bash/include")

hello.so: hello.c
	$(CC) $(INC) -O3 -Wall -shared -fPIC -o $@ $<
	strip -s $@ 2> /dev/null && sstrip -z $@ 2> /dev/null || true

.PHONY: test
test: hello.so
	@bash -xc 'enable -f ./hello.so hello && builtin hello'

.PHONY: clean
clean:
	@rm -f hello.so
