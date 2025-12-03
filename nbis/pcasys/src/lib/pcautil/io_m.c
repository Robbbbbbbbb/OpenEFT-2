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

      FILE:    IO_M.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/20/2005 by MDG
      UPDATE:  12/02/2008 by Kenneth Ko - Fix to support 64-bit

      Routines to read/write PCASYS "matrix" files.

      ROUTINES:
#cat: matrix_writerow_init - An initialization routine, called once
#cat:                        before multiple calls of matrix_writerow.
#cat: matrix_writerow - Writes one row to a matrix file.
#cat: matrix_write -  Writes a matrix of floats as a file.
#cat: matrix_readrow_init - An initialization routine, called once
#cat:                       before multiple calls of matrix_readrow.
#cat: matrix_readrow - Reads one row of a matrix file.
#cat: matrix_read_dims - Returns the dimensions of the matrix.
#cat: matrix_read - Reads a matrix file of floats.
#cat: matrix_read_submatrix - Reads a submatrix of the matrix file.

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <datafile.h>
#include <swapbyte.h>
#include <swap.h>
#include <util.h>

/********************************************************************/

/* An initialization routine, to be called once before multiple calls
of matrix_writerow.  Fopens the file for writing, writes
header information, and returns the FILE pointer.

Input parms:
  outfile: Matrix file to be made.
  desc: Description string to be included in the file.  Must
        not contain any newlines.
  ascii: 1 if an ascii file is being made, 0 if binary.
  dim1, dim2: Dimensions of the matrix.

Output parm:
  fp: FILE pointer of output file.
*/

void matrix_writerow_init(char *outfile, char *desc, const int ascii,
          int dim1, int dim2, FILE **fp)
{
  FILE *the_fp;

  if(!(the_fp = fopen(outfile, "wb")))
    fatalerr("matrix_writerow_init", "fopen for writing \
failed", outfile);
  if(strchr(desc, '\n'))
    fatalerr("matrix_writerow_init", "description \
string contains a newline", outfile);
  fprintf(the_fp, "%s\n%c %c\n", desc, PCASYS_MATRIX_FILE,
    (ascii ? PCASYS_ASCII_FILE : PCASYS_BINARY_FILE));
  if(ascii)
    fprintf(the_fp, "%d %d\n\n", dim1, dim2);
  else { /* binary */
#ifdef __NBISLE__
    swap_int_bytes(dim1);
    swap_int_bytes(dim2);
#endif
    fwrite(&dim1, sizeof(int), 1, the_fp);
    fwrite(&dim2, sizeof(int), 1, the_fp);
  }
  *fp = the_fp;
}

/********************************************************************/

/* Writes one row to a matrix file.  The initialization routine
matrix_writerow_init should be called once, before calling this
routine dim1 times.

Input parms:
  fp: The FILE pointer of the matrix file being made.
  ascii: 1 if an ascii file is being made, 0 if binary.
  dim2: Second dimension of matrix.
  rowbuf: Row to be written next.
*/

void matrix_writerow(FILE *fp, const int ascii, const int dim2, float *rowbuf)
{
  int c;
  float *p, *trowbuf;

  if(ascii) {
    for(c = 0, p = rowbuf; c < dim2 - 1; c++)
      fprintf(fp, "%14.7e%c", *p++, ((c % 5 < 4) ? ' ' : '\n'));
    fprintf(fp, "%14.7e\n\n", *p);
  }
  else {/* binary */
#ifdef __NBISLE__
    swap_float_bytes_vec_cpy(rowbuf, dim2, &trowbuf);
    p = rowbuf;
    rowbuf = trowbuf;
#endif
    fwrite(rowbuf, sizeof(float), dim2, fp);
#ifdef __NBISLE__
    free(trowbuf);
    rowbuf = p;
#endif
  }
}

/********************************************************************/

/* Writes a matrix of floats as a file.  Function value is size, in
bytes, of file produced.

Input parms:
  outfile: File to be made.
  desc: Description string to be included in the file.  Must
        not contain any newlines.
  ascii: 1 if an ascii file is being made, 0 if binary.
  dim1, dim2: Dimensions of the matrix.
  the_floats: Buffer of floats comprising the matrix.  Must be in
              row-major order, without padding.
*/

int matrix_write(char *outfile, char *desc, const int ascii, int dim1,
         int dim2, float *the_floats)
{
  FILE *fp;
  int r, c, file_nbytes = 0;
  float *p;
  float *tfloats;

  if(!(fp = fopen(outfile, "wb")))
    fatalerr("matrix_write", "fopen for writing failed",
      outfile);
  if(strchr(desc, '\n'))
    fatalerr("matrix_write", "description string contains a \
newline", outfile);
  file_nbytes += fprintf(fp, "%s\n%c %c\n", desc, PCASYS_MATRIX_FILE,
    (ascii ? PCASYS_ASCII_FILE : PCASYS_BINARY_FILE));
  if(ascii) {
    file_nbytes += fprintf(fp, "%d %d\n\n", dim1, dim2);
    for(r = 0, p = the_floats; r < dim1; r++) {
      for(c = 0; c < dim2 - 1; c++)
	file_nbytes += fprintf(fp, "%14.7e%c", *p++, ((c % 5 < 4) ?
          ' ' : '\n'));
      file_nbytes += fprintf(fp, "%14.7e\n\n", *p++);
    }
  }
  else { /* binary */
#ifdef __NBISLE__
    swap_int_bytes(dim1);
    swap_int_bytes(dim2);
#endif

    fwrite(&dim1, sizeof(int), 1, fp);
    fwrite(&dim2, sizeof(int), 1, fp);

#ifdef __NBISLE__
    swap_int_bytes(dim1);
    swap_int_bytes(dim2);
    swap_float_bytes_vec_cpy(the_floats, dim1*dim2, &tfloats);
    p = the_floats;
    the_floats = tfloats;
#endif

    fwrite(the_floats, sizeof(float), dim1 * dim2, fp);

#ifdef __NBISLE__
    free(tfloats);
    the_floats = p;
#endif
    file_nbytes += sizeof(int) * 2 + sizeof(float) * dim1 * dim2;
  }
  fclose(fp);
  return file_nbytes;
}    

/********************************************************************/

/* An initialization routine, to be called once before multiple calls
of matrix_readrow.  Fopens the file for reading, and returns
various information and a FILE pointer.

Input parm:
  infile: The matrix file.

Output parms:
  desc: Description string, in a buffer the routine allocates.
  ascii: 1 if file is ascii, 0 if binary.
  dim1, dim2: Dimensions of the matrix.
  fp: FILE pointer of input file, opened for reading by the routine.
*/

void matrix_readrow_init(char *infile, char **desc, int *ascii, int *dim1,
          int *dim2, FILE **fp)
{
  FILE *the_fp;
  char file_type, asc_or_bin, str[5], *cp, achar;
  int i, j;

  if(!(the_fp = fopen(infile, "rb")))
    fatalerr("matrix_readrow_init", "fopen for \
reading failed", infile);
  for(i = 0; (j = getc(the_fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("matrix_readrow_init", "file ends \
partway through description field", infile);
  if(!(*desc = malloc(i + 1)))
    fatalerr("matrix_readrow_init", "malloc of \
description buffer failed", infile);
  rewind(the_fp);
  for(cp = *desc; (achar = getc(the_fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 5, the_fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_MATRIX_FILE)
    fatalerr("matrix_readrow_init", "file is not of \
type PCASYS_MATRIX_FILE", infile);
  if(!(asc_or_bin == PCASYS_ASCII_FILE || asc_or_bin ==
    PCASYS_BINARY_FILE))
    fatalerr("matrix_readrow_init", "illegal ascii-or-binary \
code", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    *ascii = 1;
    fscanf(the_fp, "%d %d", dim1, dim2);
  }
  else { /* asc_or_bin == PCASYS_BINARY_FILE */
    *ascii = 0;
    fread(dim1, sizeof(int), 1, the_fp);
    fread(dim2, sizeof(int), 1, the_fp);
#ifdef __NBISLE__
    swap_int_bytes(*dim1);
    swap_int_bytes(*dim2);
#endif
  }
  *fp = the_fp;
}

/********************************************************************/

/* Reads one row of a matrix file.  The initialization routine
matrix_readrow_init must be called once, before calling this routine
dim1 (or fewer) times.

Input parms:
  fp: The FILE pointer of the matrix file.
  ascii: 1 if file is ascii, 0 if binary.
  dim2: Second dimension of matrix.

Output parms:
  rowbuf: The next row of the matrix is read into this buffer,
            provided by caller.
*/

void matrix_readrow(FILE *fp, const int ascii, int dim2, float *rowbuf)
{
  float *p, *pe;

  if(ascii)
    for(pe = (p = rowbuf) + dim2; p < pe;)
      fscanf(fp, "%f", p++);
  else { /* binary */ 
    fread(rowbuf, sizeof(float), dim2, fp);
#ifdef __NBISLE__
    swap_float_bytes_vec(rowbuf, dim2);
#endif
  }
}

/********************************************************************/

/* Returns the dimensions of the matrix contained in a matrix file.

Input parm:
  infile: The matrix file.

Output parms:
  dim1, dim2: Dimensions of the matrix.
*/

void matrix_read_dims(char *infile, int *dim1, int *dim2)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200];
  int i;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("matrix_read_dims", "fopen for reading failed",
      infile);
  while((i = getc(fp)) != '\n')
    if(i == EOF)
      fatalerr("matrix_read_dims", "file ends partway through \
description field", infile);
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_MATRIX_FILE)
    fatalerr("matrix_read_dims", "file is not of type \
PCASYS_MATRIX_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE)
    fscanf(fp, "%d %d", dim1, dim2);
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(dim1, sizeof(int), 1, fp);
    fread(dim2, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(*dim1);
    swap_int_bytes(*dim2);
#endif
  }
  else
    fatalerr("matrix_read_dims", "illegal ascii-or-binary \
code", infile);
  fclose(fp);
}

/********************************************************************/

/* Reads a matrix file into a buffer, which it allocates, and
also returns the description and dimensions.

Input parm:
  infile: File to be read.

Output parms:
  desc: Description, as a null-terminated string in a buffer
          allocated by the routine.
  dim1, dim2: Dimensions of the matrix.
  the_floats: The matrix elements, in a buffer allocated by the
                routine.
*/

void matrix_read(char *infile, char **desc, int *dim1, int *dim2, float **the_floats)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  int i, j;
  float *p, *pe;

  if(!(fp = fopen(infile, "rb")))
    fatalerr("matrix_read", "fopen for reading failed", infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("matrix_read", "file ends partway through \
description field", infile);
  if(!(*desc = malloc(i + 1)))
    fatalerr("matrix_read", "malloc of description buffer \
failed", infile);
  rewind(fp);
  for(cp = *desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_MATRIX_FILE)
    fatalerr("matrix_read", "file is not of type \
PCASYS_MATRIX_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d %d", dim1, dim2);
    i = *dim1 * *dim2;
    if(!(*the_floats = (float *)malloc(i * sizeof(float))))
      fatalerr("matrix_read", "malloc of floats buffer \
failed", infile);
    for(pe = (p = *the_floats) + i; p < pe;)
      fscanf(fp, "%f", p++);
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(dim1, sizeof(int), 1, fp);
    fread(dim2, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(*dim1);
    swap_int_bytes(*dim2);
#endif
    i = *dim1 * *dim2;
    if(!(*the_floats = (float *)malloc(i * sizeof(float))))
      fatalerr("matrix_read", "malloc of floats buffer \
failed", infile);
    fread(*the_floats, sizeof(float), i, fp);
#ifdef __NBISLE__
    swap_float_bytes_vec(*the_floats, i);
#endif
  }
  else
    fatalerr("matrix_read", "illegal ascii-or-binary \
code", infile);
  fclose(fp);
}

/********************************************************************/

/* Reads, from a matrix file, a submatrix consisting of rows row_start
through row_finish of columns column_start through column_finish,
numbering starting at 0; of course, must have 0 <= row_start <=
row_finish < dim1 where dim1 is the first dimension indicated in the
file, and similarly for columns; otherwise, fatal error.  (To avoid
this fatal error, without knowing dim1 and dim2 in advance, use
datafile_dimensions before calls of matrix_read.)  The routine
allocates a buffer for the floats.  It does not return the dim1 and
dim2 of the file, since presumably the calling program knows these
already, or it would not have been able to set row_start, etc.
sensibly.

(A typical use of this could be to read from a file containing
featvecs made using a large number of eigenvectors, of a large number
of examples; the submatrix feature allows experimenting with using
only some of the features (typically off the top for kl) of only a
block of the examples, without wasting any memory in the program, and
without having to make a new version of the data file each time
one wants to use a different submatrix of it.)

Input parms:
  infile: File a submatrix of which is to be read.
  row_start, row_finish, col_start, col_finish: Limits of the
    submatrix to be read -- it will consist of the elements (r,c)
    such that row_start <= r <= row_finish and
    col_start <= c <= col_finish, numbering starting at 0.  (Fatal
    error unless 0 <= row_start <= row_finish < dim1 and
    0 <= col_start <= col_finish < dim2, where dim1 and dim2 are
    the dimensions of the full matrix; a program that does not know
    dim1 and dim2 already can use matrix_read_dims to find
    them and avoid this fatal error.)

Output parms:
  desc: Description, as a null-terminated string in a buffer
          allocated by the routine.
  the_submat: The submatrix elements, in a buffer which the routine
                allocates to the proper size to hold them without
                padding.
*/

void matrix_read_submatrix(char *infile, const int row_start, const int row_finish,
          const int col_start, const int col_finish, char **desc, float **the_submat)
{
  FILE *fp;
  char file_type, asc_or_bin, str[200], *cp, achar;
  int i, j, k, dim1, dim2, r, c;
  float *p, a;

  if(row_start < 0) {
    sprintf(str, "row_start (%d) is < 0", row_start);
    fatalerr("matrix_read_submatrix", str, infile);
  }
  if(row_finish < row_start) {
    sprintf(str, "row_finish (%d) is < row_start (%d)", row_finish,
      row_start);
    fatalerr("matrix_read_submatrix", str, infile);
  }
  if(!(fp = fopen(infile, "rb")))
    fatalerr("matrix_read_submatrix", "fopen for reading \
failed", infile);
  for(i = 0; (j = getc(fp)) != '\n'; i++)
    if(j == EOF)
      fatalerr("matrix_read_submatrix", "file ends partway \
through description field", infile);
  if(!(*desc = malloc(i + 1)))
    fatalerr("matrix_read_submatrix", "malloc of description \
buffer failed", infile);
  rewind(fp);
  for(cp = *desc; (achar = getc(fp)) != '\n';)
    *cp++ = achar;
  *cp = 0;
  fgets(str, 200, fp);
  sscanf(str, "%c %c", &file_type, &asc_or_bin);
  if(file_type != PCASYS_MATRIX_FILE)
    fatalerr("matrix_read_submatrix", "file is not of type \
PCASYS_MATRIX_FILE", infile);
  if(asc_or_bin == PCASYS_ASCII_FILE) {
    fscanf(fp, "%d %d", &dim1, &dim2);
    if(row_finish >= dim1) {
      sprintf(str, "row_finish (%d) is >= dim1 (%d)", row_finish,
        dim1);
      fatalerr("matrix_read_submatrix", str, infile);
    }
    if(col_finish >= dim2) {
      sprintf(str, "col_finish (%d) is >= dim2 (%d)", col_finish,
        dim2);
      fatalerr("matrix_read_submatrix", str, infile);
    }
    i = (row_finish - row_start + 1) * (col_finish - col_start + 1);
    if(!(*the_submat = (float *)malloc(i * sizeof(float))))
      fatalerr("matrix_read_submatrix", "malloc of submatrix \
buffer failed", infile);
    for(r = 0; r < row_start; r++)
      fscanf(fp, "%*f");
    for(p = *the_submat; r <= row_finish; r++) {
      for(c = 0; c < col_start; c++)
	fscanf(fp, "%*f");
      for(; c <= col_finish; c++) {
	fscanf(fp, "%f", &a);
	*p++ = a;
      }
    }
  }
  else if(asc_or_bin == PCASYS_BINARY_FILE) {
    fread(&dim1, sizeof(int), 1, fp);
    fread(&dim2, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(dim1);
    swap_int_bytes(dim2);
#endif
    if(row_finish >= dim1) {
      sprintf(str, "row_finish (%d) is >= dim1 (%d)", row_finish,
        dim1);
      fatalerr("matrix_read_submatrix", str, infile);
    }
    if(col_finish >= dim2) {
      sprintf(str, "col_finish (%d) is >= dim2 (%d)", col_finish,
        dim2);
      fatalerr("matrix_read_submatrix", str, infile);
    }
    i = (row_finish - row_start + 1) * (col_finish - col_start + 1);
    if(!(*the_submat = (float *)malloc(i * sizeof(float))))
      fatalerr("matrix_read_submatrix", "malloc of submatrix \
buffer failed", infile);
    if(row_start > 0)
      fseek(fp, row_start * dim2 * sizeof(float), 0);
    i = col_finish - col_start + 1;
    j = col_start * sizeof(float);
    k = (dim2 - 1 - col_finish) * sizeof(float);
    for(r = row_start, p = *the_submat; r <= row_finish; r++,
      p += i) {
      if(j > 0)
	fseek(fp, j, 1);
      fread(p, sizeof(float), i, fp);
#ifdef __NBISLE__
      swap_float_bytes_vec(p, i);
#endif
      if(k > 0)
	fseek(fp, k, 1);
    }
  }
  else
    fatalerr("matrix_read_submatrix", "illegal \
ascii-or-binary code", infile);
  fclose(fp);
}

/********************************************************************/
