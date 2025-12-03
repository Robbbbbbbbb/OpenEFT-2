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

      FILE:    IO_V.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/20/2005 by MDG
      UPDATE:  12/02/2008 by Kenneth Ko - Fix to support 64-bit

      Routines to read/write PCASYS "covariance" files.

      CAUTION: The format of the covariance buffer is different
               for the writing routine than it is for the reading
               routine: see initial comments.

      ROUTINES:
#cat: covariance_write - Writes a covariance buffer to a file.
#cat: covariance_read_order_nvecs - Reads the order, and no. of vectors
#cat:                               used, of a covariance file.
#cat: covariance_read - Reads a covariance file into a buffer.  The buffer
#cat:                   contains the nonstrict lower triangle of the
#cat:                   covariance matrix only, in row-major order.
#cat: covariance_read_old - Reads a covariance file into a buffer.
#cat:                       Allocates the buffer to the size needed to
#cat:                       contain the full matrix, then loads the data
#cat:                       into the nonstrict upper triangle.

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <datafile.h>
#include <swapbyte.h>
#include <swap.h>
#include <util.h>

/********************************************************************/

/* Writes a covariance buffer as a file.  The buffer should
contain the nonstrict lower triangle of the matrix in row-major order,
and that is what the routine writes.

Input parms:
  outfile: Covariance file to be produced.
  desc: Description string to be included in the file.  Must
        not contain any newlines.
  ascii: 1 if an ascii file is to be produced, 0 if binary.
  order: Order of the (symmetric) covariance matrix.
  nvecs: Number of vectors that were used to make the covariance.
  the_floats: Buffer containing the nonstrict lower triangle of the
                matrix.  Must be in row-major order, without padding.
*/

void covariance_write(char *outfile, char *desc, const int ascii,
          int order, int nvecs, float *the_floats)
{
  FILE *fp;
  int r, c;
  float *p;
  float *tfloats;

  if(!(fp = fopen(outfile, "wb")))
    fatalerr("covariance_write", "fopen for writing failed", outfile);
  if(strchr(desc, '\n'))
    fatalerr("covariance_write", "description string contains a \
newline", outfile);
  fprintf(fp, "%s\n%c %c\n", desc, PCASYS_COVARIANCE_FILE,
    (ascii ? PCASYS_ASCII_FILE : PCASYS_BINARY_FILE));
  if(ascii) {
    fprintf(fp, "%d %d\n\n", order, nvecs);
    for(r = 0, p = the_floats; r < order; r++) {
      for(c = 0; c < r; c++)
	fprintf(fp, "%14.7e%c", *p++, ((c % 5 < 4) ? ' ' : '\n'));
      fprintf(fp, "%14.7e\n\n", *p++);
    }
  }
  else { /* binary */
#ifdef __NBISLE__
    swap_int_bytes(order);
    swap_int_bytes(nvecs);
#endif

    fwrite(&order, sizeof(int), 1, fp);
    fwrite(&nvecs, sizeof(int), 1, fp);

#ifdef __NBISLE__
    swap_int_bytes(order);
    swap_int_bytes(nvecs);
    swap_float_bytes_vec_cpy(the_floats, (order * (order+1))/2, &tfloats);
    p = the_floats;
    the_floats = tfloats;
#endif

    fwrite(the_floats, sizeof(float), (order * (order + 1)) / 2, fp);

#ifdef __NBISLE__
    free(tfloats);
    the_floats = p;
#endif
  }
  fclose(fp);
}

/********************************************************************/

/* Reads the order, and no. of vectors used, of a covariance file.

Input parm:
  infile: Covariance file.

Output parms:
  order: Order of the (symmetric) covariance matrix.
  nvecs: Number of vectors that were used to make the covariance.
*/

void covariance_read_order_nvecs(char *infile, int *order, int *nvecs)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200];
  int i, j;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("covariance_read_order_nvecs", "fopen for reading \
failed", infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("covariance_read_order_nvecs", "file ends partway \
through description field", infile);
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_COVARIANCE_FILE)
    fatalerr("covariance_read_order_nvecs", "file is not of type \
PCASYS_COVARIANCE_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE)
    fscanf(fp, "%d %d", order, nvecs);
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(order, sizeof(int), 1, fp);
    fread(nvecs, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(*order);
    swap_int_bytes(*nvecs);
#endif
  }
  else
    fatalerr("covariance_read_order_nvecs", "illegal ascii-or-binary \
code", infile);
  fclose(fp);
}


/********************************************************************/

/* Reads a covariance file into a buffer.  The buffer contains
the nonstrict lower triangle of the covariance matrix only, in
row-major order.

Input parm:
  infile: File to be read.

Output parms:
  desc: Description, as a null-terminated string in a buffer
          allocated by the routine.
  order: Order of the covariance matrix.
  nvecs: Number of vectors that were used to make the covariance.
  the_floats: Contains only the nonstrict lower triangle data
              of the covariance matrix in row-major order.
              So its size = (order * (order + 1)) / 2
*/

void covariance_read(char *infile, char **desc, int *order,
                     int *nvecs, float **the_floats)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  int i, j, the_order;
  float *the_floats_yow;

  if(!(fp = fopen(infile, "rb"))) {
     fprintf(stderr, "fopen for reading failed %s\n", infile);
     exit(-1);
  }
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF) {
      fprintf(stderr, "file ends partway through description field %s\n", infile);
      exit(-1);
    }
  if(!(*desc = malloc(i + 1))) {
     fprintf(stderr, "malloc of description buffer failed %s\n", infile);
     exit(-1);
  }
  rewind(fp);
  for(cp = *desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_COVARIANCE_FILE) {
     fprintf(stderr, "file is not of type PCASYS_COVARIANCE_FILE %s\n", infile);
     exit(-1);
  }
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d %d", order, nvecs);
    the_order = *order;
    if(!(*the_floats = (float *)malloc(((the_order * (the_order + 1)) / 2) *
      sizeof(float)))) {
       fprintf(stderr, "malloc of *the_floats failed %s\n", infile);
       exit(-1);
    }
    the_floats_yow = *the_floats;
    for(j = 0; j < (the_order * (the_order + 1)) / 2; j++)
	fscanf(fp, "%f", &(the_floats_yow[j]));
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    if(fread(order, sizeof(int), 1, fp) != 1) {
       fprintf(stderr, "Error reading order\n");
       exit(-1);
    }
    if(fread(nvecs, sizeof(int), 1, fp) != 1) {
       fprintf(stderr, "Error reading nvecs\n");
       exit(-1);
    }

#ifdef __NBISLE__
    swap_int_bytes(*order);
    swap_int_bytes(*nvecs);
#endif

    the_order = *order;
    if(!(*the_floats = (float *)malloc(((the_order * (the_order + 1)) / 2) *
      sizeof(float)))) {
       fprintf(stderr, "malloc of *the_floats failed %s\n", infile);
       exit(-1);
    }
    the_floats_yow = *the_floats;
    fread(the_floats_yow, sizeof(float), (the_order * (the_order + 1)) / 2, fp);
#ifdef __NBISLE__
    swap_float_bytes_vec(the_floats_yow, (the_order * (the_order + 1)) / 2);
#endif
  }
  else {
     fprintf(stderr, "illegal ascii-or-binary code %s\n", infile);
     exit(-1);
  }
  fclose(fp);
}

/********************************************************************/

/* Reads a covariance file into a buffer.  Allocates the buffer to the
size needed to contain the full matrix, then loads the data into the
nonstrict upper triangle.  (This is appropriate for the only use of
this routine in PCASYS: loading the covariance before calling tred1,
the first of the sequence of EISPACK routines called to find some of
the eigenvectors.  Since C stores a matrix in row-major order and
Fortran in column-major, the nonstrict upper triangle (in this C
program) that gets loaded with the data looks like the nonstrict lower
triangle when the buffer is sent to the Fortran program tred1, and
tred1 wants the lower triangle to contain the input data.  (Presumably
this will also work properly if tred1 is replaced by a C version made
by f2c.)

Input parm:
  infile: File to be read.

Output parms:
  desc: Description, as a null-terminated string in a buffer
          allocated by the routine.
  order: Order of the (symmetric) covariance matrix.
  nvecs: Number of vectors that were used to make the covariance.
  the_floats: The matrix, allocated by the routine to full size and
                containing the data in its nonstrict upper triangle.
*/

void covariance_read_old(char *infile, char **desc, int *order,
                     int *nvecs, float **the_floats)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  int i, j, the_order;
  float *p, *the_floats_yow, *abuf;

  if(!(fp = fopen(infile, "rb"))) {
     fprintf(stderr, "fopen for reading failed %s\n", infile);
     exit(-1);
  }
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF) {
      fprintf(stderr, "file ends partway through description field %s\n", infile);
      exit(-1);
    }
  if(!(*desc = malloc(i + 1))) {
     fprintf(stderr, "malloc of description buffer failed %s\n", infile);
     exit(-1);
  }
  rewind(fp);
  for(cp = *desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_COVARIANCE_FILE) {
     fprintf(stderr, "file is not of type PCASYS_COVARIANCE_FILE %s\n", infile);
     exit(-1);
  }
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d %d", order, nvecs);
    the_order = *order;
    if(!(*the_floats = (float *)malloc(the_order * the_order *
      sizeof(float)))) {
       fprintf(stderr, "malloc of *the_floats failed %s\n", infile);
       exit(-1);
    }
    the_floats_yow = *the_floats;
    for(j = 0; j < the_order; j++)
      for(i = 0; i <= j; i++)
	fscanf(fp, "%f", the_floats_yow + i * the_order + j);
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    if(fread(order, sizeof(int), 1, fp) != 1) {
       fprintf(stderr, "Error reading order\n");
       exit(-1);
    }
    if(fread(nvecs, sizeof(int), 1, fp) != 1) {
       fprintf(stderr, "Error reading nvecs\n");
       exit(-1);
    }

#ifdef __NBISLE__
    swap_int_bytes(*order);
    swap_int_bytes(*nvecs);
#endif

    the_order = *order;
    if(!(*the_floats = (float *)malloc(the_order * the_order *
      sizeof(float)))) {
       fprintf(stderr, "malloc of *the_floats failed %s\n", infile);
       exit(-1);
    }
    the_floats_yow = *the_floats;
    if(!(abuf = (float *)malloc(the_order * sizeof(float)))) {
       fprintf(stderr, "malloc of abuf failed %s\n", infile);
       exit(-1);
    }
    for(j = 0; j < the_order; j++) {
      fread(abuf, sizeof(float), j + 1, fp);
      for(i = 0, p = abuf; i <= j; i++) {
#ifdef __NBISLE__
        swap_float_bytes(*p);
#endif
	*(the_floats_yow + i * the_order + j) = *p++;
      }
    }
  }
  else {
     fprintf(stderr, "illegal ascii-or-binary code %s\n", infile);
     exit(-1);
  }
  fclose(fp);
}

/********************************************************************/
