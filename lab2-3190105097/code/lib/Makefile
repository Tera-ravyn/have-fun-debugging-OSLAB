VPATH = ../include:../arch/riscv/include

objects = printk.o

all : $(objects)
	$(GCC) -o edit $(objects) $(CFLAG)

printk.o : printk.c printk.h sbi.h
	$(GCC) -c printk.c $(CFLAG)

.PHONY : clean
clean : 
	-rm all $(objects)
