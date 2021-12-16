VPATH = ../include:../arch/riscv/include

objects = print.o

all : $(objects)
	$(GCC) -o edit $(objects) $(CFLAG)

print.o : print.c print.h sbi.h
	$(GCC) -c print.c $(CFLAG)

.PHONY : clean
clean : 
	-rm all $(objects)
