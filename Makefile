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
BINARIES= create-disk${SUFFIX} dmps${SUFFIX} dvol${SUFFIX} frmt${SUFFIX} mkvol${SUFFIX} print_mbr${SUFFIX} mknfs${SUFFIX} test_file${SUFFIX}\
rvol${SUFFIX} tfs${SUFFIX} if_status${SUFFIX} if_pfile${SUFFIX} if_cfile${SUFFIX} if_nfile${SUFFIX} if_dfile${SUFFIX} shell${SUFFIX}
OBJECTS= $(addsuffix .o,\
	  mkhd)

#all: $(BINARIES) $(OBJECTS)
all: create-disk dmps frmt drive volume filesystem ifile dvol print_mbr mkvol mknfs rvol\
if_status if_pfile if_nfile if_dfile if_cfile test_file

###------------------------------
### Main rules
###------------------------------------------------------------
create-disk: drive ${SRCDIR}create_disk.c
	$(CC) $(CFLAGS) -o create-disk${SUFFIX} drive.o ${SRCDIR}create_disk.c ${LIBS}

dmps: drive volume ${SRCDIR}dmps.c
	$(CC) $(CFLAGS) -o dmps${SUFFIX} drive.o ${SRCDIR}dmps.c ${LIBS}

frmt: drive volume.o ${SRCDIR}frmt.c
	$(CC) $(CFLAGS) -o frmt${SUFFIX} drive.o volume.o tools.o ${SRCDIR}frmt.c ${LIBS}

drive: ${SRCDIR}drive.c ${SRCDIR}drive.h
	$(CC) $(CFLAGS) -o drive.o -c ${SRCDIR}drive.c ${INCDIR}

volume: tools drive ${SRCDIR}volume.c ${SRCDIR}volume.h
	$(CC) $(CFLAGS) -o volume.o -c ${SRCDIR}volume.c ${INCDIR}

filesystem: volume ${SRCDIR}filesystem.c ${SRCDIR}filesystem.h
	$(CC) $(CFLAGS) -o filesystem.o -c ${SRCDIR}filesystem.c ${INCDIR}

tools: ${SRCDIR}tools.c ${SRCDIR}tools.h
	$(CC) $(CFLAGS) -o tools.o -c ${SRCDIR}tools.c ${LIBS}

ifile: filesystem ${SRCDIR}ifile.c ${SRCDIR}ifile.h
	$(CC) $(CFLAGS) -o ifile.o -c ${SRCDIR}ifile.c ${INCDIR}

file: filesystem ${SRCDIR}file.c ${SRCDIR}file.h
	$(CC) $(CFLAGS) -o file.o -c ${SRCDIR}file.c ${INCDIR}

dir:  ${SRCDIR}dir.c ${SRCDIR}dir.h
	$(CC) $(CFLAGS) -o dir.o -c ${SRCDIR}dir.c ${INCDIR}

dvol: volume drive ${SRCDIR}dvol.c
	$(CC) $(CFLAGS) -o dvol${SUFFIX} drive.o volume.o tools.o ${SRCDIR}dvol.c ${LIBS}

mkvol: volume drive ${SRCDIR}mkvol.c
	$(CC) $(CFLAGS) -o mkvol${SUFFIX} drive.o volume.o tools.o ${SRCDIR}mkvol.c ${LIBS}

mknfs: volume drive filesystem tools ifile ${SRCDIR}mknfs.c
	$(CC) $(CFLAGS) -o mknfs${SUFFIX} drive.o volume.o filesystem.o tools.o dir.o ifile.o ${SRCDIR}mknfs.c ${LIBS}

print_mbr: volume drive ${SRCDIR}print_mbr.c
	$(CC) $(CFLAGS) -o print_mbr${SUFFIX} drive.o volume.o tools.o ${SRCDIR}print_mbr.c ${LIBS}

rvol: volume drive ${SRCDIR}rvol.c
	$(CC) $(CFLAGS) -o rvol${SUFFIX} drive.o volume.o tools.o ${SRCDIR}rvol.c ${LIBS}

if_status: filesystem ${SRCDIR}if_status.c
	$(CC) $(CFLAGS) -o if_status${SUFFIX} drive.o volume.o filesystem.o tools.o ${SRCDIR}if_status.c ${LIBS}

if_pfile: ifile ${SRCDIR}if_pfile.c
	$(CC) $(CFLAGS) -o if_pfile${SUFFIX} drive.o volume.o filesystem.o ifile.o tools.o ${SRCDIR}if_pfile.c ${LIBS}

if_nfile: ifile ${SRCDIR}if_nfile.c
	$(CC) $(CFLAGS) -o if_nfile${SUFFIX} drive.o volume.o filesystem.o ifile.o tools.o ${SRCDIR}if_nfile.c ${LIBS}

if_dfile: ifile ${SRCDIR}if_dfile.c
	$(CC) $(CFLAGS) -o if_dfile${SUFFIX} drive.o volume.o filesystem.o ifile.o tools.o ${SRCDIR}if_dfile.c ${LIBS}

if_cfile: ifile ${SRCDIR}if_cfile.c
	$(CC) $(CFLAGS) -o if_cfile${SUFFIX} drive.o volume.o filesystem.o ifile.o tools.o ${SRCDIR}if_cfile.c ${LIBS}

test_file: dir ifile file ${SRCDIR}test_file.c
	$(CC) $(CFLAGS) -o test_file${SUFFIX} drive.o volume.o filesystem.o ifile.o file.o dir.o tools.o ${SRCDIR}test_file.c ${LIBS}

shell:  cmd ${SRCDIR}shell.c
	$(CC) $(CFLAGS) -o shell${SUFFIX} cmd.o my_ls.o my_cd.o my_pwd.o ${SRCDIR}shell.c

my_ls: ${SRCDIR}my_ls.c ${SRCDIR}my_ls.h
	$(CC) $(CFLAGS) -o my_ls.o -c ${SRCDIR}my_ls.c

my_cd: ${SRCDIR}my_cd.c ${SRCDIR}my_cd.h
	$(CC) $(CFLAGS) -o my_cd.o -c ${SRCDIR}my_cd.c

my_pwd: ${SRCDIR}my_pwd.c ${SRCDIR}my_pwd.h
	$(CC) $(CFLAGS) -o my_pwd.o -c ${SRCDIR}my_pwd.c

cmd: my_cd my_ls my_pwd ${SRCDIR}cmd.c ${SRCDIR}cmd.h
	$(CC) $(CFLAGS) -o cmd.o -c ${SRCDIR}cmd.c

###------------------------------
### Testing rules
###------------------------------------------------------------

tconvert_blc: volume drive ${SRCDIR}test_convert_blc.c
	$(CC) $(CFLAGS) -o test_convert_blc${SUFFIX} drive.o volume.o ${SRCDIR}test_convert_blc.c ${LIBS}

tfilesystem: filesystem volume drive ${SRCDIR}test_filesystem.c
	$(CC) $(CFLAGS) -o tfs${SUFFIX} drive.o volume.o  filesystem.o ${SRCDIR}test_filesystem.c ${LIBS}

tfile: file drive volume filesystem volume ${SRCDIR}test_file.c
	$(CC) $(CFLAGS) -o tfile${SUFFIX} drive.o volume.o  filesystem.o file.o ${SRCDIR}test_file.c ${LIBS}


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin
