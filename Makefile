# $Id: Makefile,v 1.2 2004/10/12 09:06:17 marquet Exp $
##############################################################################

ROOTDIR=.
SRCDIR=src/

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
BINARIES= create-disk${SUFFIX} dmps${SUFFIX} dvol${SUFFIX} frmt${SUFFIX} mkvol${SUFFIX} print_mbr${SUFFIX} mknfs${SUFFIX}\
rvol${SUFFIX} tfs${SUFFIX}
OBJECTS= $(addsuffix .o,\
	  mkhd)

#all: $(BINARIES) $(OBJECTS)
all: create-disk dmps frmt drive volume filesystem dvol print_mbr mkvol mknfs rvol

###------------------------------
### Main rules
###------------------------------------------------------------
create-disk: drive ${SRCDIR}create_disk.c
	$(CC) $(CFLAGS) -o create-disk${SUFFIX} drive.o ${SRCDIR}create_disk.c ${LIBS}

dmps: drive volume ${SRCDIR}dmps.c
	$(CC) $(CFLAGS) -o dmps${SUFFIX} drive.o ${SRCDIR}dmps.c ${LIBS}

frmt: drive volume.o ${SRCDIR}frmt.c
	$(CC) $(CFLAGS) -o frmt${SUFFIX} drive.o volume.o ${SRCDIR}frmt.c ${LIBS}

drive: ${SRCDIR}drive.c ${SRCDIR}drive.h
	$(CC) $(CFLAGS) -o drive.o -c ${SRCDIR}drive.c ${INCDIR}

volume: ${SRCDIR}volume.c ${SRCDIR}volume.h
	$(CC) $(CFLAGS) -o volume.o -c ${SRCDIR}volume.c ${INCDIR}

filesystem: volume ${SRCDIR}filesystem.c ${SRCDIR}filesystem.h
	$(CC) $(CFLAGS) -o filesystem.o -c ${SRCDIR}filesystem.c ${INCDIR}

file: filesystem ${SRCDIR}file.c ${SRCDIR}file.h
	$(CC) $(CFLAGS) -o file.o -c ${SRCDIR}file.c ${INCDIR}

dvol: volume drive ${SRCDIR}dvol.c
	$(CC) $(CFLAGS) -o dvol${SUFFIX} drive.o volume.o ${SRCDIR}dvol.c ${LIBS}

mkvol: volume drive ${SRCDIR}mkvol.c
	$(CC) $(CFLAGS) -o mkvol${SUFFIX} drive.o volume.o ${SRCDIR}mkvol.c ${LIBS}

mknfs: volume drive filesystem ${SRCDIR}mknfs.c
	$(CC) $(CFLAGS) -o mknfs${SUFFIX} drive.o volume.o filesystem.o ${SRCDIR}mknfs.c ${LIBS}

print_mbr: volume drive ${SRCDIR}print_mbr.c
	$(CC) $(CFLAGS) -o print_mbr${SUFFIX} drive.o volume.o ${SRCDIR}print_mbr.c ${LIBS}

rvol: volume drive ${SRCDIR}rvol.c
	$(CC) $(CFLAGS) -o rvol${SUFFIX} drive.o volume.o ${SRCDIR}rvol.c ${LIBS}

tconvert_blc: volume drive ${SRCDIR}test_convert_blc.c
	$(CC) $(CFLAGS) -o test_convert_blc${SUFFIX} drive.o volume.o ${SRCDIR}test_convert_blc.c ${LIBS}

tfile: file drive volume filesystem volume ${SRCDIR}t_file.c
	$(CC) $(CFLAGS) -o tfile${SUFFIX} drive.o volume.o  filesystem.o file.o ${SRCDIR}t_file.c ${LIBS}

tfilesystem: filesystem volume drive ${SRCDIR}test_filesystem.c
	$(CC) $(CFLAGS) -o tfs${SUFFIX} drive.o volume.o  filesystem.o ${SRCDIR}test_filesystem.c ${LIBS}

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin
