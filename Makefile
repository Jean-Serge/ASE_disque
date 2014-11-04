
ROOTDIR=/home/m1/monbailly/Documents/ASE/ASE_disque

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic 
CFLAGS  += -g
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(LIBDIR) -lhardware
RM 	= rm -rf

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= clean main  
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


.PHONY: clean
clean:
	$(RM) *.o $(BINARIES) *~ *# vdisk*
