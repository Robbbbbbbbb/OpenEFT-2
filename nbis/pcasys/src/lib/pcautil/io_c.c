/*******************************************************************************

License: 
This software and/or related materials was developed at the National Institute
of Standards and Technology (NIST) by employees of the Federal Government
in the course of their official duties. Pursuant to title 17 Section 105
of the United States Code, this software is not subject to copyright
protection and is in the public domain. 

This software and/or related materials have been determined to be not subject
to the EAR (see Part 734.3 of the EAR for exact details) because it is
a publicly available technology and software, and is freely distributed
to any interested party with no licensing requirements.  Therefore, it is 
permissible to distribute this software as a free download from the internet.

Disclaimer: 
This software and/or related materials was developed to promote biometric
standards and biometric technology testing for the Federal Government
in accordance with the USA PATRIOT Act and the Enhanced Border Security
and Visa Entry Reform Act. Specific hardware and software products identified
in this software were used in order to perform the software development.
In no case does such identification imply recommendation or endorsement
by the National Institute of Standards and Technology, nor does it imply that
the products and equipment identified are necessarily the best available
for the purpose.

This software and/or related materials are provided "AS-IS" without warranty
of any kind including NO WARRANTY OF PERFORMANCE, MERCHANTABILITY,
NO WARRANTY OF NON-INFRINGEMENT OF ANY 3RD PARTY INTELLECTUAL PROPERTY
or FITNESS FOR A PARTICULAR PURPOSE or for any purpose whatsoever, for the
licensed product, however used. In no event shall NIST be liable for any
damages and/or costs, including but not limited to incidental or consequential
damages of any kind, including economic damage or injury to property and lost
profits, regardless of whether NIST shall be advised, have reason to know,
or in fact shall know of the possibility.

By using this software, you agree to bear all risk relating to quality,
use and performance of the software and/or related materials.  You agree
to hold the Government harmless from any claim arising from your use
of the software.

*******************************************************************************/


/***********************************************************************
      LIBRARY: PCASYS_UTILS - Pattern Classification System Utils

      FILE:    IO_C.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/20/2005 by MDG
      UPDATE:  12/02/2008 by Kenneth Ko - Fix to support 64-bit

      Routines to read/write PCASYS fingerprint class files.

      ROUTINES:
#cat: classes_write_ind - Writes a buffer of classes as a file.
#cat: classes_read_n - Finds the number of entries in a classes file.
#cat: classes_read_ind - Reads a class file containing class indices.
#cat: classes_read_vec - Reads a class file containing class vectors.
#cat: classes_read_ncls - Finds number of classes in class file.
#cat: classes_read_pindex - Determines if class file contains indices
#cat:                       or vectors (floats) to define class of
#cat:                       each print.
#cat: classes_read_subvector_ind - Reads a subset of a class file
#cat:                              containing class indices.
#cat: number_classes - Determines the number of unique classes in
#cat:                  set of class indices.

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <datafile.h>
#include <mlp/lims.h>
#include <memalloc.h>
#include <swapbyte.h>
#include <swap.h>
#include <util.h>

/********************************************************************/

/* Writes a buffer of classes as a file.

Input parms:
  outfile: File to be produced.
  desc: Description string to be included in the file.  Must
        not contain any newlines.
  ascii: 1 if an ascii file is to be produced, 0 if binary.
  n: Number of elements in the array of classes.
  the_classes: The classes, which are unsigned chars.  If file
                   is to be ascii, they are written as decimal integers
                   (in the range 0 through 255); if binary, they are
                   written as unsigned characters, "as is".
  ncls: number unique classes (ie. number outputs in PNN or MLP)
  long_classnames: Strings (32bytes) of corresponding classnames for the
*/

void classes_write_ind(char *outfile, char *desc, const int ascii, int n,
          unsigned char *the_classes, int ncls, char **long_classnames)
{
  FILE *fp;
  int i;
  unsigned char *ucp;
  int pindex = 1;

  if(!(fp = fopen(outfile, "wb")))
    fatalerr("classes_write", "fopen for writing failed", outfile);
  if(strchr(desc, '\n'))
    fatalerr("classes_write", "description string contains a \
newline", outfile);
  fprintf(fp, "%s\n%c %c\n", desc, PCASYS_CLASSES_FILE,
    (ascii ? PCASYS_ASCII_FILE : PCASYS_BINARY_FILE));
  if(ascii) {
    fprintf(fp, "%d\n", n);
    fprintf(fp, "%d\n", pindex);
    fprintf(fp, "%d\n", ncls);
    for(i = 0; i < ncls; i++)
       fprintf(fp, "%s\n", long_classnames[i]);

    for(i = 0, ucp = the_classes; i < n; i++)
      fprintf(fp, "%d\n", ucp[i]);
  }
  else { /* binary */
#ifdef __NBISLE__
    swap_int_bytes(n);
    swap_int_bytes(pindex);
    swap_int_bytes(ncls);
#endif
    fwrite(&n, sizeof(int), 1, fp);
    fwrite(&pindex, sizeof(int), 1, fp);
    fwrite(&ncls, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(n);
    swap_int_bytes(pindex);
    swap_int_bytes(ncls);
#endif
    for(i = 0; i < ncls; i++)
       fwrite(long_classnames[i], sizeof(char), LONG_CLASSNAME_MAXSTRLEN, fp);

    fwrite(the_classes, sizeof(unsigned char), n, fp);
  }
  fclose(fp);
}    

/********************************************************************/

/* Finds the number of entries in a classes file.

Input parm:
  infile: Classes file.

Output parm:
  n: Number of entries.
*/

void classes_read_n(char *infile, int *n)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200];
  int i;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("classes_read_n", "fopen for reading failed", infile);
  while((i = getc(fp)) != '\n')
    if(i == EOF)
      fatalerr("classes_read_n", "file ends partway through \
description field", infile);
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_CLASSES_FILE)
    fatalerr("classes_read_n", "file is not of type \
PCASYS_CLASSES_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE)
    fscanf(fp, "%d", n);
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(n, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(*n);
#endif
  }
  else
    fatalerr("classes_read_n", "illegal ascii-or-binary code",
      infile);
  fclose(fp);
}

/********************************************************************/

/* Reads a file of classes (indices) and stores them, as unsigned chars,
into a buffer which it allocates; also returns the number of entries.

Input parm:
  infile: Classes file to be read.

Output parms:
  desc: Description, as a null-terminated string in a buffer
          allocated by the routine.
  n: Number of entries.
  the_classes: The classes, stored as unsigned chars in
                   a buffer allocated by the routine.
  ncls: number unique classes (ie. number outputs in PNN or MLP)
  long_classnames: Strings (32bytes) of corresponding classnames for the
                   index values
*/

void classes_read_ind(char *infile, char **desc, int *n,
          unsigned char **the_classes, int *ncls, char ***long_classnames)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  unsigned char *ucp;
  int i, j, pindex;
  char **lcnptr;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("classes_read_ind", "fopen for reading failed", infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("classes_read_ind", "file ends partway through \
description field", infile);
  if(!(*desc = malloc(i + 1)))
    fatalerr("classes_read_ind", "malloc of description buffer \
failed", infile);
  rewind(fp);
  for(cp = *desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_CLASSES_FILE)
    fatalerr("classes_read_ind", "file is not of type \
PCASYS_CLASSES_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d", n);
    fscanf(fp, "%d", &pindex);
    fscanf(fp, "%d", ncls);
    /* Read the long (full) names of the classes. */
    malloc_dbl_char(&lcnptr, *ncls, LONG_CLASSNAME_MAXSTRLEN + 1,
                    "classes_read_ind long_classnames");
    *long_classnames = lcnptr;
    for(i = 0; i < *ncls; i++)
      fscanf(fp, "%s", lcnptr[i]);

    if(!pindex)
      fatalerr("classes_read_ind","pindex = 0","Need class indices not vectors");
    if(!(*the_classes = (unsigned char *)malloc(*n *
      sizeof(unsigned char))))
      fatalerr("classes_read_ind", "malloc of classes buffer failed",
        infile);
    for(i = 0, ucp = *the_classes; i < *n; i++) {
      fscanf(fp, "%d", &j);
      *ucp++ = j;
    }
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(n, sizeof(int), 1, fp);
    fread(&pindex, sizeof(int), 1, fp);
    fread(ncls, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(*n);
    swap_int_bytes(pindex);
    swap_int_bytes(*ncls);
#endif

    /* Read the long (full) names of the classes. */
    malloc_dbl_char(&lcnptr, *ncls, LONG_CLASSNAME_MAXSTRLEN + 1,
                    "classes_read_ind long_classnames");
    *long_classnames = lcnptr;
    for(i = 0; i < *ncls; i++)
      fread(lcnptr[i], sizeof(char), LONG_CLASSNAME_MAXSTRLEN, fp);

    if(!pindex)
      fatalerr("classes_read_ind","pindex = 0","Need class indices not vectors");

    if(!(*the_classes = (unsigned char *)malloc(*n *
      sizeof(unsigned char))))
      fatalerr("classes_read_ind", "malloc of classes buffer failed",
        infile);
    fread(*the_classes, sizeof(unsigned char), *n, fp);
  }
  else
    fatalerr("classes_read_ind", "illegal ascii-or-binary code", infile);
  fclose(fp);
}

/********************************************************************/

/* Reads a subvector of the items in a classes file (indices) and stores
them, as unsigned chars, into a buffer, which it allocates: reads the
start through finish items, numbering starting at 0.

(One might want to use this to read just a block from a file of
classes, to go with partial versions of featvecs: e.g., start =
500, finish = 1000, and for floats_matrix_read_submatrix, let
row_start = 500, row_finish = 1000, and let column_start 0 and
column_finish = 49 to read the first 50 features of each featvec.)

Input parms:
  infile: Classes file from which to read a subvector.
  start, finish: The subvector to be read consists of entries
                   start through finish, numbering starting at 0.

Output parms:
  desc: Description, as a null-terminated string in a buffer
          allocated by the routine.
  the_subvec: The specified subvector of entries, stored as
                   unsigned chars in a buffer the routine allocates
                   to the proper size (without padding).
  ncls: number unique classes (ie. number outputs in PNN or MLP)
  long_classnames: Strings (32bytes) of corresponding classnames for the
                   index values
*/

void classes_read_subvector_ind(char *infile, const int start, const int finish,
          char **desc, unsigned char **the_subvec, int *ncls, char ***long_classnames)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  unsigned char *ucp;
  int i, j, n, subvec_n;
  char **lcnptr;
  int pindex;

  if(start < 0) {
    sprintf(str, "start (%d) is < 0", start);
    fatalerr("classes_read_subvector_ind", str, infile);
  }
  if(finish < start) {
    sprintf(str, "finish (%d) is < start (%d)", finish, start);
    fatalerr("classes_read_subvector_ind", str, infile);
  }
  if(!(fp = fopen(infile, "rb")))
    fatalerr("classes_read_subvector_ind", "fopen for reading failed",
      infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("classes_read_subvector_ind", "file ends partway \
through description field", infile);
  if(!(*desc = malloc(i + 1)))
    fatalerr("classes_read_subvector_ind", "malloc of description \
buffer failed", infile);
  rewind(fp);
  for(cp = *desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_CLASSES_FILE)
    fatalerr("classes_read_subvector_ind", "file is not of type \
PCASYS_CLASSES_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d", &n);
    if(finish >= n) {
      sprintf(str, "finish (%d) is >= n (%d)", finish, n);
      fatalerr("classes_read_subvector_ind", str, infile);
    }

    fscanf(fp, "%d", &pindex);
    if(!pindex)
      fatalerr("classes_read_subvector_ind","pindex != 1",
               "expecting class indices not vectors");

    fscanf(fp, "%d", ncls);
    /* Read the long (full) names of the classes. */
    malloc_dbl_char(&lcnptr, *ncls, LONG_CLASSNAME_MAXSTRLEN + 1,
                    "classes_read long_classnames");
    *long_classnames = lcnptr;
    for(i = 0; i < *ncls; i++)
      fscanf(fp, "%s", lcnptr[i]);

    if(!(*the_subvec = (unsigned char *)malloc((finish - start + 1) *
      sizeof(unsigned char))))
      fatalerr("classes_read_subvector_ind", "malloc of subvector \
buffer failed", infile);
    for(i = 0; i < start; i++)
      fscanf(fp, "%*d");
    for(ucp = *the_subvec; i <= finish; i++) {
      fscanf(fp, "%d", &j);
      *ucp++ = j;
    }
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(&n, sizeof(int), 1, fp);
    fread(&pindex, sizeof(int), 1, fp);
    fread(ncls, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(n);
    swap_int_bytes(pindex);
    swap_int_bytes(*ncls);
#endif
    if(!pindex)
      fatalerr("classes_read_subvector_ind","pindex != 1",
               "expecting class indices not vectors");

    /* Read the long (full) names of the classes. */
    malloc_dbl_char(&lcnptr, *ncls, LONG_CLASSNAME_MAXSTRLEN + 1,
                    "classes_read long_classnames");
    *long_classnames = lcnptr;
    for(i = 0; i < *ncls; i++)
      fread(lcnptr[i], sizeof(char), LONG_CLASSNAME_MAXSTRLEN, fp);


    if(finish >= n) {
      sprintf(str, "finish (%d) is >= n (%d)", finish, n);
      fatalerr("classes_read_subvector_ind", str, infile);
    }
    if(start > 0)
      fseek(fp, start * sizeof(unsigned char), 0);
    subvec_n = finish - start + 1;
    if(!(*the_subvec = (unsigned char *)malloc(subvec_n *
      sizeof(unsigned char))))
      fatalerr("classes_read_subvector_ind", "malloc of subvector \
buffer failed", infile);
    fread(*the_subvec, sizeof(unsigned char), subvec_n, fp);
  }
  else
    fatalerr("classes_read_subvector_ind", "illegal ascii-or-binary \
code", infile);
  fclose(fp);
}

/********************************************************************/
/* how many unique classes */
void number_classes(unsigned char *classes, const int n_cls, int *n)
{
  unsigned char cls[100];
  int i, j, nc, match;

  nc = 1;
  cls[0] = classes[0];
  for(i = 1; i < n_cls; i++) {
     match = 0;
     for(j = 0; j < nc; j++)
        if(classes[i] == cls[j])
           match = 1;
     if(!match) {
        nc++;
        if(nc > 99)
           fatalerr("number_classes","cls","number classes > 100 (hard coded value)");
        cls[nc-1] = classes[i];
     }
  }
  *n = nc;
}

/********************************************************************/

/* Reads a file of classes vectors and stores them, as floats,
into a buffer which it allocates; also returns the number of entries.

Input parm:
  infile: Classes file to be read.

Output parms:
  desc: Description, as a null-terminated string in a buffer
          allocated by the routine.
  n: Number of entries.
  the_classes: The class vectors, stored as floats in
                   a buffer allocated by the routine.
  ncls: number unique classes (ie. number outputs in PNN or MLP)
  long_classnames: Strings (32bytes) of corresponding classnames for the
                   index values
*/

void classes_read_vec(char *infile, char **desc, int *n, float **the_classes,
          int *ncls, char ***long_classnames)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  int i, j;
  char **lcnptr;
  float *tcptr;
  int pindex;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("classes_read_vec", "fopen for reading failed", infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("classes_read_vec", "file ends partway through \
description field", infile);
  if(!(*desc = malloc(i + 1)))
    fatalerr("classes_read_vec", "malloc of description buffer \
failed", infile);
  rewind(fp);
  for(cp = *desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_CLASSES_FILE)
    fatalerr("classes_read_vec", "file is not of type \
PCASYS_CLASSES_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d", n);
    fscanf(fp, "%d", &pindex);
    fscanf(fp, "%d", ncls);
    /* Read the long (full) names of the classes. */
    malloc_dbl_char(&lcnptr, *ncls, LONG_CLASSNAME_MAXSTRLEN + 1,
                    "classes_read_vec long_classnames");
    *long_classnames = lcnptr;
    for(i = 0; i < *ncls; i++)
      fscanf(fp, "%s", lcnptr[i]);

    if(pindex)
      fatalerr("classes_read_vec","pindex = 1","Need class vectors not indices");
    malloc_flt(&tcptr, *n * *ncls, "classes_read_vec the_classes");
    *the_classes = tcptr;
    for(i = 0; i < *n * *ncls; i++)
      fscanf(fp, "%f", &(tcptr[i]));
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(n, sizeof(int), 1, fp);
    fread(&pindex, sizeof(int), 1, fp);
    fread(ncls, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(*n);
    swap_int_bytes(pindex);
    swap_int_bytes(*ncls);
#endif

    /* Read the long (full) names of the classes. */
    malloc_dbl_char(&lcnptr, *ncls, LONG_CLASSNAME_MAXSTRLEN + 1,
                    "classes_read_vec long_classnames");
    *long_classnames = lcnptr;
    for(i = 0; i < *ncls; i++)
      fread(lcnptr[i], sizeof(char), LONG_CLASSNAME_MAXSTRLEN, fp);

    if(pindex)
      fatalerr("classes_read_vec","pindex = 1","Need class vectors not indices");

    malloc_flt(&tcptr, *n * *ncls, "classes_read_vec the_classes");
    *the_classes = tcptr;
    fread(tcptr, sizeof(float), *n * *ncls, fp);
#ifdef __NBISLE__
    swap_float_bytes_vec(tcptr, *n * *ncls);
#endif
  }
  else
    fatalerr("classes_read_vec", "illegal ascii-or-binary code", infile);
  fclose(fp);
}

/********************************************************************/

/*
  Determines if class file contains indices (ints) or vectors (floats)
  to define class of each print

Input parm:
  infile: Classes file to be read.

Output parms:
  pindex: class types. ie. indices or vectors
*/

void classes_read_pindex(char *infile, int *pindex)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  int i, j;
  char *desc;
  int n;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("classes_read_pindex", "fopen for reading failed", infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("classes_read_pindex", "file ends partway through \
description field", infile);
  if(!(desc = malloc(i + 1)))
    fatalerr("classes_read_pindex", "malloc of description buffer \
failed", infile);
  rewind(fp);
  for(cp = desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  free(desc);
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_CLASSES_FILE)
    fatalerr("classes_read_pindex", "file is not of type \
PCASYS_CLASSES_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d", &n);
    fscanf(fp, "%d", pindex);
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(&n, sizeof(int), 1, fp);
    fread(pindex, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(n);
    swap_int_bytes(*pindex);
#endif
  }
  else
    fatalerr("classes_read_pindex", "illegal ascii-or-binary code", infile);
  fclose(fp);
}

/********************************************************************/

/*
  Reads number of classes

Input parm:
  infile: Classes file to be read.

Output parms:
  ncls: # classes (nouts)
*/

void classes_read_ncls(char *infile, int *ncls)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  int i, j;
  char *desc;
  int n, pindex;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("classes_read_ncls", "fopen for reading failed", infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("classes_read_ncls", "file ends partway through \
description field", infile);
  if(!(desc = malloc(i + 1)))
    fatalerr("classes_read_ncls", "malloc of description buffer \
failed", infile);
  rewind(fp);
  for(cp = desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  free(desc);
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_CLASSES_FILE)
    fatalerr("classes_read_ncls", "file is not of type \
PCASYS_CLASSES_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d", &n);
    fscanf(fp, "%d", &pindex);
    fscanf(fp, "%d", ncls);
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(&n, sizeof(int), 1, fp);
    fread(&pindex, sizeof(int), 1, fp);
    fread(ncls, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(n);
    swap_int_bytes(pindex);
    swap_int_bytes(*ncls);
#endif
  }
  else
    fatalerr("classes_read_ncls", "illegal ascii-or-binary code", infile);
  fclose(fp);
}
