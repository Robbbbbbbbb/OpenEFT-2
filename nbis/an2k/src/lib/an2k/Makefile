#*******************************************************************************
#
# License: 
# This software and/or related materials was developed at the National Institute
# of Standards and Technology (NIST) by employees of the Federal Government
# in the course of their official duties. Pursuant to title 17 Section 105
# of the United States Code, this software is not subject to copyright
# protection and is in the public domain. 
#
# This software and/or related materials have been determined to be not subject
# to the EAR (see Part 734.3 of the EAR for exact details) because it is
# a publicly available technology and software, and is freely distributed
# to any interested party with no licensing requirements.  Therefore, it is 
# permissible to distribute this software as a free download from the internet.
#
# Disclaimer: 
# This software and/or related materials was developed to promote biometric
# standards and biometric technology testing for the Federal Government
# in accordance with the USA PATRIOT Act and the Enhanced Border Security
# and Visa Entry Reform Act. Specific hardware and software products identified
# in this software were used in order to perform the software development.
# In no case does such identification imply recommendation or endorsement
# by the National Institute of Standards and Technology, nor does it imply that
# the products and equipment identified are necessarily the best available
# for the purpose.
#
# This software and/or related materials are provided "AS-IS" without warranty
# of any kind including NO WARRANTY OF PERFORMANCE, MERCHANTABILITY,
# NO WARRANTY OF NON-INFRINGEMENT OF ANY 3RD PARTY INTELLECTUAL PROPERTY
# or FITNESS FOR A PARTICULAR PURPOSE or for any purpose whatsoever, for the
# licensed product, however used. In no event shall NIST be liable for any
# damages and/or costs, including but not limited to incidental or consequential
# damages of any kind, including economic damage or injury to property and lost
# profits, regardless of whether NIST shall be advised, have reason to know,
# or in fact shall know of the possibility.
#
# By using this software, you agree to bear all risk relating to quality,
# use and performance of the software and/or related materials.  You agree
# to hold the Government harmless from any claim arising from your use
# of the software.
#
#*******************************************************************************

# SubTree:              /NBIS/Main/an2k/src/lib/an2k
# Filename:             Makefile
# Integrators:          Kenneth Ko
# Organization:         NIST/ITL
# Host System:          GNU GCC/GMAKE GENERIC (UNIX)
# Date Created:         01/14/2008
# Date Updated:         01/31/2008 (Kenneth Ko)
#                       04/09/2008 by Joseph C. Konczal
#                       09/03/2008 (Kenneth Ko)
#                       09/29/2008 (Kenneth Ko)
#                       01/06/2009 (Kenneth Ko) - add support for HPUX compile
#                       02/25/2015 (Kenneth Ko) - Renamed OPENJPEG to OPENJP2
#
# ******************************************************************************
#
# Makefile contains the variables to build library - "liban2k".
#
# ******************************************************************************
include ../../../p_rules.mak
#
LIBRARY	:= liban2k
#
SRC	:= \
	alloc.c \
	append.c \
	copy.c \
	date.c \
	decode.c \
	delete.c \
	flip.c \
	fmtstd.c \
	fmttext.c \
	getimg.c \
	globals.c \
	insert.c \
	is_an2k.c \
	lookup.c \
	print.c \
	read.c \
	select.c \
	size.c \
	subst.c \
	to_iafis.c \
	to_nist.c \
	type.c \
	type1314.c \
	update.c \
	util.c \
	value2.c \
	seg.c
#
ifeq ($(NBIS_JASPER_FLAG),-D__NBIS_JASPER__)
SRC	:= \
	$(SRC) \
	dec_jpeg2k.c
endif
#
ifeq ($(NBIS_OPENJP2_FLAG),-D__NBIS_OPENJP2__)
SRC	:= \
	$(SRC) \
	dec_jpeg2k.c
endif

#
ifeq ($(NBIS_PNG_FLAG),-D__NBIS_PNG__)
SRC	:= \
	$(SRC) \
	dec_png.c
endif
#
EXT_INCS	:= -I$(EXPORTS_INC_DIR)
#
EXT_LIBS	:=  

ifeq ($(MSYS_FLAG),-D__MSYS__)
EXT_LIBS	:= \
	$(EXT_LIBS) \
	-liberty 
endif
#
include $(DIR_ROOT_BUILDUTIL)/lib.mak
#
