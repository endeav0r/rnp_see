CPP=g++
CFLAGS=-Wall -O2 -g --std=c++0x -Wno-switch
LIBS=-L/usr/local/lib -ludis86

_OBJS = debug.o elf.o instruction.o kernel.o lx86.o memory.o \
       page.o symbolicvalue.o translator.o uint.o vm.o

SRCDIR = src
OBJS = $(patsubst %,$(SRCDIR)/%,$(_OBJS))

all : $(OBJS) $(SRCDIR)/see.o
	$(CPP) -o see $(OBJS) $(SRCDIR)/see.o $(CFLAGS) $(LIBS)

%.o : %.cc %.h
	$(CPP) -c -o $@ $< $(CFLAGS)

tests : $(OBJS)
	$(CPP) -o test_memory src/test/test_memory.cc $(OBJS) $(CFLAGS) $(LIBS)

clean :
	rm -f $(SRCDIR)/*.o
	rm -f see