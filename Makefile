# $Id: Makefile,v 1.2 2004/10/12 09:06:17 marquet Exp $
##############################################################################

ROOTDIR=.

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic
CFLAGS  += -g
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(LIBDIR) -lhardware
SUFFIX  = .bin

###------------------------------
### Main targets
###------------------------------------------------------------
BINARIES= create-disk${SUFFIX}  dmps${SUFFIX}  dvol${SUFFIX}  frmt${SUFFIX}  mkvol${SUFFIX}  print_mbr${SUFFIX}
OBJECTS	= $(addsuffix .o,\
	  mkhd)

#all: $(BINARIES) $(OBJECTS)
all: create-disk dmps frmt drive volume filesystem dvol print_mbr mkvol

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

filesystem: filesystem.c filesystem.h
	$(CC) $(CFLAGS) -o filesystem.o -c filesystem.c ${INCDIR}

file: filesystem file.c file.h
	$(CC) $(CFLAGS) -o file.o -c file.c ${INCDIR}

dvol: volume drive dvol.c
	$(CC) $(CFLAGS) -o dvol.bin drive.o volume.o dvol.c ${LIBS}

mkvol: volume drive mkvol.c
	$(CC) $(CFLAGS) -o mkvol.bin drive.o volume.o mkvol.c ${LIBS}

print_mbr: volume drive print_mbr.c
	$(CC) $(CFLAGS) -o print_mbr.bin drive.o volume.o print_mbr.c ${LIBS}

test_convert_blc: volume drive test/test_convert_blc.c
	$(CC) $(CFLAGS) -o test_convert_blc.bin drive.o volume.o test/test_convert_blc.c ${LIBS}


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin
