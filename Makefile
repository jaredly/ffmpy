#
# http://www.gnu.org/software/make/manual/make.html
#
CC:=gcc
INCLUDES:=$(shell pkg-config --cflags libavformat libavcodec libswscale libavutil sdl)
CFLAGS:=-Wall -ggdb
LDFLAGS:=$(shell pkg-config --libs libavformat libavcodec libswscale libavutil sdl) -lm

SO:=libffmpy.so

#
# This is here to prevent Make from deleting secondary files.
#
.SECONDARY:

#
# $< is the first dependency in the dependency list
# $@ is the target name
#
all: dirs bin/ffmpy.out $(addprefix lib/, $(SO)) tags

py:
	@rm frame_getter.c
	python setup.py build_ext -i

dirs:
	mkdir -p obj
	mkdir -p bin
	mkdir -p lib

tags: *.c
	ctags *.c

bin/%.out: obj/%.o
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

lib/lib%.so : %.c
	$(CC) $(CFLAGS) $< $(INCLUDES) $(LDFLAGS) -fPIC -shared -o $@

obj/%.o : %.c
	$(CC) $(CFLAGS) $< $(INCLUDES) -c -o $@

clean:
	rm -f obj/*
	rm -f bin/*
	rm -f tags

test:
	@LD_LIBRARY_PATH="$(shell pwd)/lib" python tester.py

.PHONY: test clean
