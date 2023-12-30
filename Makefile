OS     := $(shell uname -s 2> /dev/null)
CFLAGS := -O3 -fPIC -Wall
SHARED := -shared
INC    := $(shell pkg-config --cflags bash 2> /dev/null || \
                  printf '%s\n' "-I/usr/include/bash/include")

ifeq ($(OS),Darwin)
  CFLAGS += -Wl,-undefined,dynamic_lookup
endif

.PHONY: all
all: hello.so myprint.so

hello.so: hello.c
	$(CC) $(INC) $(CFLAGS) $(SHARED) -o $@ $<

myprint.so: myprint.c
	$(CC) $(INC) $(CFLAGS) $(SHARED) -o $@ $<

.PHONY: test
test: hello.so myprint.so
	@bash -xc 'enable -f ./hello.so hello && builtin hello'
	@bash -xc 'enable -f ./myprint.so myprint && builtin myprint -f "%s\n" "Hello, world!"'

.PHONY: clean
clean:
	@rm -f hello.so myprint.so
