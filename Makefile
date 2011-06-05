default: all

OBJECTS = \
  git2.o \
  ls-remote.o

all: $(OBJECTS)
	gcc -o git2 $(OBJECTS)
