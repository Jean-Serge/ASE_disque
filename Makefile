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

#all: $(BINARIES) $(OBJECTS)
all: create-disk dmps frmt drive volume test_mbr dvol init_mbr

###------------------------------
### Main rules
###------------------------------------------------------------
create-disk: drive create_disk.c
	$(CC) $(CFLAGS) -o create-disk.bin drive.o create_disk.c ${LIBS}

dmps: drive dmps.c
	$(CC) $(CFLAGS) -o dmps.bin drive.o dmps.c ${LIBS}

frmt: drive volume.o frmt.c
	$(CC) $(CFLAGS) -o frmt.bin drive.o volume.o frmt.c ${LIBS}

drive: drive.c drive.h
	$(CC) $(CFLAGS) -o drive.o -c drive.c ${INCDIR}

volume: volume.c volume.h
	$(CC) $(CFLAGS) -o volume.o -c volume.c ${INCDIR}

test_mbr: drive volume test_mbr.c
	$(CC) $(CFLAGS) -o test_mbr.bin drive.o volume.o test_mbr.c ${LIBS}

dvol: volume drive dvol.c
	$(CC) $(CFLAGS) -o dvol.bin drive.o volume.o dvol.c ${LIBS}

mkvol: volume mkvol.c
	$(CC) $(CFLAGS) -o mkvol.bin drive.o volume.o mkvol.c ${LIBS}

init_mbr: volume drive init_mbr.c
	$(CC) $(CFLAGS) -o init_mbr.bin drive.o volume.o init_mbr.c ${LIBS}


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin
