# $Id: Makefile,v 1.2 2004/10/12 09:06:17 marquet Exp $
##############################################################################

ROOTDIR=.

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic
CFLAGS  += -g
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(LIBDIR) -lhardware

###------------------------------
### Main targets
###------------------------------------------------------------
BINARIES= mkhd
OBJECTS	= $(addsuffix .o,\
	  mkhd)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Main rules
###------------------------------------------------------------
create-disk: drive create_disk.c
	$(CC) $(CFLAGS) -o create-disk.bin drive.o create_disk.c ${LIBS}

dmps: drive dmps.c
	$(CC) $(CFLAGS) -o dmps.bin drive.o dmps.c ${LIBS}

drive: drive.c drive.h
	$(CC) $(CFLAGS) -o drive.o -c drive.c ${INCDIR}

test_drive: drive
	$(CC) $(CFLAGS) -o test drive.o test.c ${LIBS}


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin
