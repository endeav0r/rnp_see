CPP=g++
CFLAGS=-Wall -O2 -g --std=c++0x -Wno-switch
LIBS=-L/usr/local/lib -ludis86

_OBJS = debug.o elf.o instruction.o kernel.o memory.o \
       page.o see.o symbolicvalue.o translator.o uint.o vm.o

SRCDIR = src
OBJS = $(patsubst %,$(SRCDIR)/%,$(_OBJS))

all : $(OBJS)
	$(CPP) -o see $(OBJS) $(CFLAGS) $(LIBS)

%.o : %.cc %.h
	$(CPP) -c -o $@ $< $(CFLAGS)

clean :
	rm -f $(SRCDIR)/*.o
	rm -f see