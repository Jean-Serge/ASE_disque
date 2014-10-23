
ROOTDIR=/home/m1/monbailly/Documents/ASE/ASE_disque

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic 
CFLAGS  += -g
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(LIBDIR) -lhardware

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= main 
OBJECTS	= $(addsuffix .o,\
	  main)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Main rules 
###------------------------------------------------------------
main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(INCDIR)
disque.o : disque.c
	$(CC) $(CFLAGS) -c disque.c $(INCDIR) 

main: main.o disque.o 
	$(CC) $(CFLAGS) -o main main.o disque.o $(LIBS)

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES) *~ *#
realclean: clean 
	$(RM) vdiskA.bin vdiskB.bin
