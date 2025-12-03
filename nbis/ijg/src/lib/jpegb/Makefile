# MAKEFILE.MAK created by ./config on RedHad LINUX 6.X and then
# manually edited by Michael D. Garris  12/11/2000
# Last Updated: Kenneth Ko  10/23/07

include ../../../p_rules.mak

# Generated automatically from makefile.cfg by configure.
# Makefile for Independent JPEG Group's software

# makefile.cfg is edited by configure to produce a custom Makefile.

# Read installation instructions before saying "make" !!

# For compiling with source and object files in different directories.
srcdir = .

# Where to install the programs and man pages.
prefix = $(DIR_ROOT)
exec_prefix = ${prefix}
bindir = $(INSTALL_ROOT_BIN_DIR)
libdir = $(INSTALL_ROOT_LIB_DIR)
includedir = $(EXPORTS_INC_DIR)
binprefix =
manprefix =
manext = 1
mandir = $(prefix)/man/man$(manext)
LIBRARY = libjpegb

# The name of your C compiler:
#CC= gcc

# You may need to adjust these cc options:
#CFLAGS= -O2
CCC= $(CC) $(CFLAGS) $(CDEFS) -I$(includedir) -I$(srcdir)
# Generally, we recommend defining any configuration symbols in jconfig.h,
# NOT via -D switches here.
# However, any special defines for ansi2knr.c may be included here:
ANSI2KNRFLAGS= 

# Link-time cc options:
#LDFLAGS= 

# To link any special libraries, add the necessary -l commands here.
LDLIBS= 

# If using GNU libtool, LIBTOOL references it; if not, LIBTOOL is empty.
LIBTOOL = 
# $(O) expands to "lo" if using libtool, plain "o" if not.
# Similarly, $(A) expands to "la" or "a".
O = o
A = a

# Library version ID; libtool uses this for the shared library version number.
# Note: we suggest this match the macro of the same name in jpeglib.h.
JPEG_LIB_VERSION = 62

# Put here the object file name for the correct system-dependent memory
# manager file.  For Unix this is usually jmemnobs.o, but you may want
# to use jmemansi.o or jmemname.o if you have limited swap space.
SYSDEPMEM= jmemnobs.$(O)

# miscellaneous OS-dependent stuff
SHELL= /bin/sh
# linker
LN= $(CC)
# file cp command
CP= cp -f
# file deletion command
RM= rm -f
# directory creation command
MKDIR= mkdir
# library (.a) file creation command
AR= ar rc
# second step in .a creation (use "touch" if not needed)
AR2= ranlib

# installation program
ifeq ($(TARGET_OS),0)
	INSTALL = /usr/bin/install -p -c
	INSTALL_PROGRAM = $(INSTALL)
	INSTALL_LIB = $(INSTALL) -m 644
	INSTALL_DATA = $(INSTALL) -m 644
else
	INSTALL = $(CP)
	INSTALL_PROGRAM = $(INSTALL)
	INSTALL_LIB = $(INSTALL)
	INSTALL_DATA = $(INSTALL)
endif

# End of configurable options.

# source files: JPEG library proper
LIBSOURCES= jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c \
        jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c \
        jcphuff.c jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c \
        jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c \
        jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c \
        jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c \
        jfdctint.c jidctflt.c jidctfst.c jidctint.c jidctred.c jquant1.c \
        jquant2.c jutils.c jmemmgr.c \
	membuf.c decoder.c encoder.c ppi.c marker.c

# memmgr back ends: compile only one of these into a working library
SYSDEPSOURCES= jmemansi.c jmemname.c jmemnobs.c jmemdos.c jmemmac.c

# source files: cjpeg/djpeg/jpegtran applications, also rdjpgcom/wrjpgcom
APPSOURCES= cjpeg.c djpeg.c jpegtran.c rdjpgcom.c wrjpgcom.c cdjpeg.c \
        rdcolmap.c rdswitch.c transupp.c rdppm.c wrppm.c rdgif.c wrgif.c \
        rdtarga.c wrtarga.c rdbmp.c wrbmp.c rdrle.c wrrle.c
SOURCES= $(LIBSOURCES) $(SYSDEPSOURCES) $(APPSOURCES)

# files included by source files
INCLUDES= jchuff.h jdhuff.h jdct.h jerror.h jinclude.h jmemsys.h jmorecfg.h \
        jpegint.h jpeglib.h jversion.h cdjpeg.h cderror.h transupp.h

# documentation, test, and support files
DOCS= README install.doc usage.doc cjpeg.1 djpeg.1 jpegtran.1 rdjpgcom.1 \
        wrjpgcom.1 wizard.doc example.c libjpeg.doc struct.doc \
        coderule.doc filelist.doc change.log

MKFILES= config makefile.cfg makefile.ans makefile.unx makefile.bcc \
        makefile.mc6 makefile.dj makefile.wat makefile.vc makelib.ds \
        makeapps.ds makeproj.mac makcjpeg.st makdjpeg.st makljpeg.st \
        maktjpeg.st makefile.mnx makefile.sas makefile.mms makefile.vms \
        makvms.opt

CONFIGFILES= jconfig.cfg jconfig.bcc jconfig.mc6 jconfig.dj jconfig.wat \
        jconfig.vc jconfig.mac jconfig.st jconfig.mnx jconfig.sas \
        jconfig.vms

CONFIGUREFILES= config.ges config.sub inst-sh ltconfig ltmain.sh

OTHERFILES= jconfig.doc ckconfig.c ansi2knr.c ansi2knr.1 jmemdosa.asm

TESTFILES= testorig.jpg testimg.ppm testimg.bmp testimg.jpg testprog.jpg \
        testimgp.jpg

DISTFILES= $(DOCS) $(MKFILES) $(CONFIGFILES) $(SOURCES) $(INCLUDES) \
        $(CONFIGUREFILES) $(OTHERFILES) $(TESTFILES)

# library object files common to compression and decompression
COMOBJECTS= jcomapi.$(O) jutils.$(O) \
	jerror.$(O) jmemmgr.$(O) \
	$(SYSDEPMEM) \
	membuf.$(O) marker.$(O)

# compression library object files
CLIBOBJECTS= jcapimin.$(O) jcapistd.$(O) \
	jctrans.$(O) jcparam.$(O) \
        jdatadst.$(O) jcinit.$(O) \
	jcmaster.$(O) jcmarker.$(O) \
	jcmainct.$(O) jcprepct.$(O) \
	jccoefct.$(O) jccolor.$(O) \
	jcsample.$(O) jchuff.$(O) \
        jcphuff.$(O) jcdctmgr.$(O) \
	jfdctfst.$(O) jfdctflt.$(O) \
        jfdctint.$(O) \
	encoder.$(O)

# decompression library object files
DLIBOBJECTS= jdapimin.$(O) jdapistd.$(O) \
	jdtrans.$(O) jdatasrc.$(O) \
        jdmaster.$(O) jdinput.$(O) \
	jdmarker.$(O) jdhuff.$(O) \
	jdphuff.$(O) jdmainct.$(O) \
	jdcoefct.$(O) jdpostct.$(O) \
	jddctmgr.$(O) jidctfst.$(O) \
	jidctflt.$(O) jidctint.$(O) \
	jidctred.$(O) jdsample.$(O) \
	jdcolor.$(O) jquant1.$(O) \
	jquant2.$(O) jdmerge.$(O) \
	decoder.$(O) ppi.$(O)

# These objectfiles are included in LIBRARY $(libjpegb.a)
LIBOBJECTS= $(CLIBOBJECTS) $(DLIBOBJECTS) $(COMOBJECTS)

# object files for sample applications (excluding library files)
COBJECTS= cjpeg.$(O) rdppm.$(O) \
	rdgif.$(O) rdtarga.$(O) \
	rdrle.$(O) rdbmp.$(O) \
	rdswitch.$(O) cdjpeg.$(O)

DOBJECTS= djpeg.$(O) wrppm.$(O) \
	wrgif.$(O) wrtarga.$(O) \
	wrrle.$(O) wrbmp.$(O) \
	rdcolmap.$(O) cdjpeg.$(O)

TROBJECTS= jpegtran.$(O) rdswitch.$(O) \
	cdjpeg.$(O) transupp.$(O)

all:	it
all:  $(LIBRARY).$(A) cjpeg \
	djpeg jpegtran rdjpgcom \
	wrjpgcom

%.o: %.c
	$(CCC) -c -o $@ $<

# the library:
$(LIBRARY).$(A):  $(LIBOBJECTS)
	$(RM) $(LIBRARY).$(A)
	$(AR) $(LIBRARY).$(A) $(LIBOBJECTS)
	$(AR2) $(LIBRARY).$(A)
	$(CP) $@ $(EXPORTS_LIB_DIR)
	
# generate catalog
catalog:
	@/bin/sh $(DIR_ROOT_BUILDUTIL)/catalog.sh \
		proc $(srcdir) c > /dev/null 2>&1
	$(MV) catalog.txt $(LIBRARY).txt
	$(CP) $(LIBRARY).txt $(DOC_CATS_DIR)

# sample programs:

cjpeg: $(COBJECTS) $(LIBRARY).$(A)
	$(LN) $(LDFLAGS) -o cjpeg $(COBJECTS) \
		$(LIBRARY).$(A) $(LDLIBS)

djpeg: $(DOBJECTS) $(LIBRARY).$(A)
	$(LN) $(LDFLAGS) -o djpeg $(DOBJECTS) \
		$(LIBRARY).$(A) $(LDLIBS)

jpegtran: $(TROBJECTS) $(LIBRARY).$(A)
	$(LN) $(LDFLAGS) -o jpegtran $(TROBJECTS) \
		$(LIBRARY).$(A) $(LDLIBS)

rdjpgcom: rdjpgcom.$(O)
	$(LN) $(LDFLAGS) -o rdjpgcom \
		rdjpgcom.$(O) $(LDLIBS)

wrjpgcom: wrjpgcom.$(O)
	$(LN) $(LDFLAGS) -o wrjpgcom \
		wrjpgcom.$(O) $(LDLIBS)

# Installation rules:

install: install-lib install-bin

install-lib: $(libdir)/$(binprefix)$(LIBRARY).$(A) install-headers
$(libdir)/$(binprefix)$(LIBRARY).$(A): $(LIBRARY).$(A)
	$(INSTALL_LIB) $(LIBRARY).$(A) \
		$(libdir)/$(binprefix)$(LIBRARY).$(A)

install-bin: $(bindir)/$(binprefix)/cjpeg $(bindir)/$(binprefix)/djpeg $(bindir)/$(binprefix)/jpegtran $(bindir)/$(binprefix)/rdjpgcom $(bindir)/$(binprefix)/wrjpgcom install-lib
	
$(bindir)/$(binprefix)/cjpeg : cjpeg
	$(INSTALL_PROGRAM) cjpeg$(EXEEXT) $(bindir)/$(binprefix)cjpeg$(EXEEXT)

$(bindir)/$(binprefix)/djpeg : djpeg
	$(INSTALL_PROGRAM) djpeg$(EXEEXT) $(bindir)/$(binprefix)djpeg$(EXEEXT)

$(bindir)/$(binprefix)/jpegtran : jpegtran
	$(INSTALL_PROGRAM) jpegtran$(EXEEXT) $(bindir)/$(binprefix)jpegtran$(EXEEXT)

$(bindir)/$(binprefix)/rdjpgcom : rdjpgcom
	$(INSTALL_PROGRAM) rdjpgcom$(EXEEXT) $(bindir)/$(binprefix)rdjpgcom$(EXEEXT)

$(bindir)/$(binprefix)/wrjpgcom : wrjpgcom
	$(INSTALL_PROGRAM) wrjpgcom$(EXEEXT) $(bindir)/$(binprefix)wrjpgcom$(EXEEXT)

# These header files have been manually installed in $(includedir)
#install-headers: jconfig.h
#	$(INSTALL_DATA) jconfig.h $(includedir)/jconfig.h
#	$(INSTALL_DATA) $(srcdir)/jpeglib.h $(includedir)/jpeglib.h
#	$(INSTALL_DATA) $(srcdir)/jmorecfg.h $(includedir)/jmorecfg.h
#	$(INSTALL_DATA) $(srcdir)/jerror.h $(includedir)/jerror.h

depend:
encoder.$(O): encoder.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
decoder.$(O): decoder.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
ppi.$(O): ppi.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
membuf.$(O): membuf.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
marker.$(O): marker.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h

bare:	clean
	$(RM) *~

clean:
	@( \
		echo "$(RM) $(CURDIR)/*.o"; \
		echo "$(RM) $(CURDIR)/*.a"; \
		$(RM) *.o $(LIBRARY).$(A); \
		$(RM) cjpeg$(EXEEXT) djpeg$(EXEEXT) jpegtran$(EXEEXT) \
			rdjpgcom$(EXEEXT) wrjpgcom$(EXEEXT) ; \
		$(RM) core testout* config.log config.status ; \
		echo "$(RM) ./catalog_$(LIBRARY).txt"; \
		$(RM) $(srcdir)/catalog_$(LIBRARY).txt; \
	);

distclean: clean
	$(RM) jconfig.h config.cache

test: cjpeg djpeg jpegtran
	$(RM) testout*
	./djpeg -dct int -ppm -outfile testout.ppm \
		$(srcdir)/testorig.jpg
	./djpeg -dct int -bmp -colors 256 -outfile testout.bmp \
		$(srcdir)/testorig.jpg
	./cjpeg -dct int -outfile testout.jpg \
		$(srcdir)/testimg.ppm
	./djpeg -dct int -ppm -outfile testoutp.ppm \
		$(srcdir)/testprog.jpg
	./cjpeg -dct int -progressive -opt -outfile testoutp.jpg \
		$(srcdir)/testimg.ppm
	./jpegtran -outfile testoutt.jpg $(srcdir)/testprog.jpg
	cmp $(srcdir)/testimg.ppm testout.ppm
	cmp $(srcdir)/testimg.bmp testout.bmp
	cmp $(srcdir)/testimg.jpg testout.jpg
	cmp $(srcdir)/testimg.ppm testoutp.ppm
	cmp $(srcdir)/testimgp.jpg testoutp.jpg
	cmp $(srcdir)/testorig.jpg testoutt.jpg

check: test

# Mistake catcher:

jconfig.h: jconfig.doc
	echo You must prepare a system-dependent jconfig.h file.
	echo Please read the installation directions in install.doc.
	exit 1

# GNU Make likes to know which target names are not really files to be made:
.PHONY: all install install-lib install-bin install-headers clean distclean test check bare depend it


jcapimin.$(O): jcapimin.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcapistd.$(O): jcapistd.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jccoefct.$(O): jccoefct.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jccolor.$(O): jccolor.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcdctmgr.$(O): jcdctmgr.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jchuff.$(O): jchuff.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jchuff.h
jcinit.$(O): jcinit.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcmainct.$(O): jcmainct.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcmarker.$(O): jcmarker.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcmaster.$(O): jcmaster.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcomapi.$(O): jcomapi.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcparam.$(O): jcparam.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcphuff.$(O): jcphuff.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jchuff.h
jcprepct.$(O): jcprepct.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jcsample.$(O): jcsample.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jctrans.$(O): jctrans.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdapimin.$(O): jdapimin.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdapistd.$(O): jdapistd.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdatadst.$(O): jdatadst.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h
jdatasrc.$(O): jdatasrc.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h
jdcoefct.$(O): jdcoefct.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdcolor.$(O): jdcolor.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jddctmgr.$(O): jddctmgr.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jdhuff.$(O): jdhuff.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jchuff.h
jdinput.$(O): jdinput.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdmainct.$(O): jdmainct.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdmarker.$(O): jdmarker.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdmaster.$(O): jdmaster.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdmerge.$(O): jdmerge.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdphuff.$(O): jdphuff.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jchuff.h
jdpostct.$(O): jdpostct.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdsample.$(O): jdsample.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jdtrans.$(O): jdtrans.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jerror.$(O): jerror.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jversion.h $(includedir)/jerror.h
jfdctflt.$(O): jfdctflt.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jfdctfst.$(O): jfdctfst.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jfdctint.$(O): jfdctint.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jidctflt.$(O): jidctflt.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jidctfst.$(O): jidctfst.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jidctint.$(O): jidctint.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jidctred.$(O): jidctred.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jdct.h
jquant1.$(O): jquant1.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jquant2.$(O): jquant2.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jutils.$(O): jutils.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h
jmemmgr.$(O): jmemmgr.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jmemsys.h
jmemansi.$(O): jmemansi.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jmemsys.h
jmemname.$(O): jmemname.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jmemsys.h
jmemnobs.$(O): jmemnobs.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jmemsys.h
jmemdos.$(O): jmemdos.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jmemsys.h
jmemmac.$(O): jmemmac.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h jmemsys.h
cjpeg.$(O): cjpeg.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h jversion.h
djpeg.$(O): djpeg.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h jversion.h
jpegtran.$(O): jpegtran.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h transupp.h jversion.h
rdjpgcom.$(O): rdjpgcom.c jinclude.h $(includedir)/jconfig.h
wrjpgcom.$(O): wrjpgcom.c jinclude.h $(includedir)/jconfig.h
cdjpeg.$(O): cdjpeg.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
rdcolmap.$(O): rdcolmap.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
rdswitch.$(O): rdswitch.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
transupp.$(O): transupp.c jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h jpegint.h $(includedir)/jerror.h transupp.h
rdppm.$(O): rdppm.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
wrppm.$(O): wrppm.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
rdgif.$(O): rdgif.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
wrgif.$(O): wrgif.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
rdtarga.$(O): rdtarga.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
wrtarga.$(O): wrtarga.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
rdbmp.$(O): rdbmp.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
wrbmp.$(O): wrbmp.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
rdrle.$(O): rdrle.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
wrrle.$(O): wrrle.c cdjpeg.h jinclude.h $(includedir)/jconfig.h $(includedir)/jpeglib.h $(includedir)/jmorecfg.h $(includedir)/jerror.h cderror.h
