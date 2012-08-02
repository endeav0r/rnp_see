CPP=g++
CFLAGS=-Wall -O2 -g --std=c++0x -Wno-switch
LIBS=-L/usr/local/lib -ludis86 -lz3 

_OBJS = translator.o debug.o elf.o engine.o instruction.o kernel.o \
	    lx86.o memory.o page.o symbolicvalue.o uint.o vm.o

SRCDIR = src
OBJS = $(patsubst %,$(SRCDIR)/%,$(_OBJS))

all : $(OBJS) $(SRCDIR)/see.o
	$(CPP) -o see $(OBJS) $(SRCDIR)/see.o $(CFLAGS) $(LIBS)

%.o : %.cc %.h
	$(CPP) -c -o $@ $< $(CFLAGS)

%.o : %.cc
	$(CPP) -c -o $@ $< $(CFLAGS)

test_vm : $(OBJS) src/test/test_vm.cc
	$(CPP) -o test_vm src/test/test_vm.cc $(OBJS) $(CFLAGS) $(LIBS)

test_memory : $(OBJS) src/test/test_memory.cc
	$(CPP) -o test_memory src/test/test_memory.cc $(OBJS) $(CFLAGS) $(LIBS)

test_symbolicvalue : $(OBJS) src/test/test_symbolicvalue.cc
	$(CPP) -o test_symbolicvalue src/test/test_symbolicvalue.cc $(OBJS) $(CFLAGS) $(LIBS)

tests : test_vm test_memory test_symbolicvalue

clean :
	rm -f $(SRCDIR)/*.o
	rm -f see
	rm -f test_vm
	rm -f test_memory
	rm -f test_symbolicvalue
