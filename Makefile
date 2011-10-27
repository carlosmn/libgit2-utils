default: all

LIBGIT2_PATH ?= $(HOME)/staging/libgit2/lib

DEPS = $(shell PKG_CONFIG_PATH=$(LIBGIT2_PATH)/pkgconfig pkg-config --cflags --libs libgit2)

CFLAGS += -g -O0 -ggdb
CFLAGS += $(DEPS)

OBJECTS = \
  git2.o \
  ls-remote.o \
  fetch.o \
  index-pack.o \
  status.o
#  parse-pkt-line.o \
  show-remote.o \
  fetch.o

all: $(OBJECTS)
	gcc $(CFLAGS) -o git2 $(OBJECTS)
