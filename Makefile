# $Id: Makefile,v 1.2 2004/10/12 09:06:17 marquet Exp $
##############################################################################

ROOTDIR=.

CC	= gcc
CFLAGS	= -Wall -ansi # -pedantic
CFLAGS  += -g
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(LIBDIR) -lhardware
SUFFIX  = .bin

###------------------------------
### Main targets
###------------------------------------------------------------
BINARIES= create-disk${SUFFIX}  dmps${SUFFIX}  dvol${SUFFIX}  frmt${SUFFIX}  mkvol${SUFFIX}  print_mbr${SUFFIX}
OBJECTS= $(addsuffix .o,\
	  mkhd)

#all: $(BINARIES) $(OBJECTS)
all: create-disk dmps frmt drive volume filesystem dvol print_mbr mkvol mknfs

###------------------------------
### Main rules
###------------------------------------------------------------
create-disk: drive create_disk.c
	$(CC) $(CFLAGS) -o create-disk${SUFFIX} drive.o create_disk.c ${LIBS}

dmps: drive dmps.c
	$(CC) $(CFLAGS) -o dmps${SUFFIX} drive.o dmps.c ${LIBS}

frmt: drive volume.o frmt.c
	$(CC) $(CFLAGS) -o frmt${SUFFIX} drive.o volume.o frmt.c ${LIBS}

drive: drive.c drive.h
	$(CC) $(CFLAGS) -o drive.o -c drive.c ${INCDIR}

volume: volume.c volume.h
	$(CC) $(CFLAGS) -o volume.o -c volume.c ${INCDIR}

filesystem: filesystem.c filesystem.h
	$(CC) $(CFLAGS) -o filesystem.o -c filesystem.c ${INCDIR}

file: filesystem file.c file.h
	$(CC) $(CFLAGS) -o file.o -c file.c ${INCDIR}

dvol: volume drive dvol.c
	$(CC) $(CFLAGS) -o dvol${SUFFIX} drive.o volume.o dvol.c ${LIBS}

mkvol: volume drive mkvol.c
	$(CC) $(CFLAGS) -o mkvol${SUFFIX} drive.o volume.o mkvol.c ${LIBS}

mknfs: volume drive filesystem mknfs.c
	$(CC) $(CFLAGS) -o mknfs${SUFFIX} drive.o volume.o filesystem.o mknfs.c ${LIBS}

print_mbr: volume drive print_mbr.c
	$(CC) $(CFLAGS) -o print_mbr${SUFFIX} drive.o volume.o print_mbr.c ${LIBS}

test_convert_blc: volume drive test/test_convert_blc.c
	$(CC) $(CFLAGS) -o test_convert_blc${SUFFIX} drive.o volume.o test/test_convert_blc.c ${LIBS}

tfile: file drive volume filesystem volume t_file.c
	$(CC) $(CFLAGS) -o tfile drive.o volume.o  filesystem.o file.o t_file.c ${LIBS}


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin
