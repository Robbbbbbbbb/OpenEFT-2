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

      FILE:    LITTLE.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/20/2005 by MDG
               03/02/2007 by Kenneth Ko

      Utility routines for PCASYS.

      ROUTINES:
#cat: creat_ch - tries to creat a file
#cat: dptr2ptr_uchar - converts [][] into *
#cat: erode - erodes a raster
#cat: exists - finds out whether a file exists
#cat: fopen_ch - tries to fopen a file
#cat: fopen_noclobber - tries to fopen a file for writing, unless it exists
#cat: get_datadir - tries to find the pcasys data directory
#cat: isverbose - finds out whether the verbosity is on
#cat: lastcomp - finds last component of a pathname
#cat: linecount - counts the lines in a file
#cat: linreg - linear regression
#cat: malloc_ch - tries to malloc a buffer
#cat: open_read_ch - tries to open a file for reading
#cat: rcfill - fills holes in the rows and cols of a raster
#cat: rsblobs - removes small blobs from a raster
#cat: setverbose - sets the verbosity
#cat: sleepity - sleep, or wait for user to hit return key
#cat: summary - computes and writes summary info for a test run
#cat: tilde_filename - changes ~/string to home-dir/string
#cat: usage_func - for use by the "usage" macro
#cat: Usage_func - for use by the "Usage" macro
#cat: write_ihdr_std - writes ihdr file with many "standard" fields

***********************************************************************/

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <little.h>
#include <findblob.h>
#include <ihead.h>
#include <img_io.h>
#include <memalloc.h>
#include <util.h>
#include <fixup.h>
#ifndef __MSYS__
#include <libgen.h>
#endif
static int verbosity;

/*******************************************************************/

/* Tries to creat a file with mode 0644.  If successful, returns file
descriptor; otherwise, calls syserr */

int creat_ch(char *filename)
{
  int fd;

  if((fd = creat(filename, 0644)) == -1)
    syserr("creat_ch", "creat", filename);
  return fd;
}

/*******************************************************************/
void dptr2ptr_uchar(unsigned char **segras, unsigned char **buf,
          const int w, const int h)
{
   unsigned char *bptr;
   int i, j;

   malloc_uchar(buf, w*h, "dptr2ptr_uchar buf");
   bptr = *buf;
   for(j = 0; j < h; j++)
      for(i = 0; i < w; i++)
         *bptr++ = segras[j][i];
}

/*******************************************************************/

/* Performs an erosion on a raster, i.e. sets to false every pixel
that has a false pixel next to it */

void erode(unsigned char *ras, const int w, const int h)
{
  unsigned char *p, *q, *qmw, *qpw;
  static unsigned char *buf;
  int wh, wd, hd, i, j;
  static int max_wh = 0;

  if((wh = w*h) > max_wh) {
    if(max_wh)
      free((char *)buf);
    buf = (unsigned char *)malloc_ch(wh);
    max_wh = wh;
  }
  wd = w - 1;
  hd = h - 1;
  memcpy(buf, ras, wh * sizeof(unsigned char));
  for(i = 0, p = buf, q = ras; i < h; i++, q += w) {
    qmw = q - w;
    qpw = q + w;
    for(j = 0; j < w; j++, p++)
      if((i && !*(qmw + j)) || (i != hd && !*(qpw + j)) ||
        (j && !*(q + j - 1)) || (j != wd && !*(q + j + 1)))
        *p = 0;
  }
  memcpy(ras, buf, wh * sizeof(unsigned char));
}

/*******************************************************************/

/* Returns 1 if given file exists, 0 otherwise.  (Presumes that the
reason for a "stat" failure is nonexistence of the file.  Actually, it
is possible for stat to fail for an existing file because this process
lacks execute permission for one or more of the directories in the
path of the file.) */

int exists(char *filename)
{
  struct stat strstat;

  return (stat(filename, &strstat) != -1);
}

/*******************************************************************/

/* Tries to fopen a file; fatalerr upon failure */

FILE *fopen_ch(char *filename, char *type)
{
  FILE *fp;
  char str[200];

  if(!(fp = fopen(filename, type))) {
    sprintf(str, "fopen of %s with type %s failed", filename, type);
    fatalerr("fopen_ch", str, NULL);
  }
  return fp;
}

/*******************************************************************/

/* No-clobber fopen for writing.  Checks if a file already exists; if
so, calls fatalerr, and if not, uses fopen_ch to try to fopen it
for writing. */

FILE * fopen_noclobber(char *filename)
{
  if(exists(filename))
    fatalerr("fopen_noclobber", filename, "already exists");
  return fopen_ch(filename, "wb");
}

/*******************************************************************/

/* Tries to find the pcasys data directory. */

char *get_datadir()
{
  static char datadir[200];

  sprintf(datadir, "%s/pcasys", INSTALL_DATA_DIR);
  return datadir;
}

/*******************************************************************/

/* Finds out whether verbosity is on */

int isverbose()
{
  return verbosity;
}

/******************************************************************/

/* Finds last component of a pathname */

char * lastcomp(char *path)
{
  static char value[200];
  char *p;

  p = strrchr(path, '/');
  if(p)
    strcpy(value, p + 1);
  else
    strcpy(value, path);
  return value;
}

/*******************************************************************/

/* Returns no. of lines in a file.  If stat fails on filename
(perhaps because the file does not exist), returns 0. */

int linecount(char *filename)
{
  FILE *fp;
  int c, n;
  struct stat strstat;

  if(stat(filename, &strstat) == -1)
    return 0;
  fp = fopen_ch(filename, "rb");
  for(n = 0; (c = fgetc(fp)) != EOF; )
    if(c == '\n')
      n++;
  fclose(fp);
  return n;
}

/*******************************************************************/

/* Linear regression for integer coordinate-pairs.  Return value:
  0: OK; *a_ret and *b_ret are the line parameters.
  1: n is < 2.
  2: Denominator in calculation came out zero.
*/

int linreg(int *xp, int *yp, const int n, float *a_ret, float *b_ret)
{
  int sumx, sumy, sumxy, sumx2, *xps, x, y, denom;

  if(n < 2)
    return 1;
  for(sumx = sumy = sumxy = sumx2 = 0, xps = xp + n; xp < xps; xp++,
    yp++) {
    sumx += (x = *xp);
    sumy += (y = *yp);
    sumxy += x * y;
    sumx2 += x * x;
  }
  denom = n * sumx2 - sumx * sumx;
  if(denom) {
    *a_ret = (n * sumxy - sumx * sumy) / (float)denom;
    *b_ret = (sumx2 * sumy - sumx * sumxy) / (float)denom;
    return 0;
  }
  else
    return 2;
}

/*******************************************************************/

/* Tries a malloc; calls fatalerr in case of failure */

char *malloc_ch(const int nbytes)
{
  char *p, str[100];

  if(!(p = malloc(nbytes))) {
    sprintf(str, "malloc of %d bytes failed", nbytes);
    fatalerr("malloc_ch", str, NULL);
  }
  return p;
}

/*******************************************************************/

/* Tries to open a file for reading; calls syserr if open fails */

int open_read_ch(char *filename)
{
  int fd;

  if((fd = open(filename, 0)) == -1)
    syserr("open_read_ch", "open", filename);
  return fd;
}

/*******************************************************************/

/* Fills holes in the rows and columns of a raster */

void rcfill(unsigned char *ras, const int w, const int h)
{
  int x, xw, xe, y, yn, ys;

  for(y = 0; y < h; y++)
    for(xw = 0; xw < w; xw++)
      if(*(ras + y * w + xw)) {
        for(xe = w - 1; !*(ras + y * w + xe); xe--);
        for(x = xw; x <= xe; x++)
          *(ras + y * w + x) = 1;
      }
  for(x = 0; x < w; x++)
    for(yn = 0; yn < h; yn++)
      if(*(ras + yn * w + x)) {
        for(ys = h - 1; !*(ras + ys * w + x); ys--);
        for(y = yn; y <= ys; y++)
          *(ras + y * w + x) = 1;
      }
}

/*******************************************************************/

/* Removes small blobs.  Given a binary raster (using one byte per
pixel, with false and true represented by 0 and 1) of height h by
width w, this routine sets to false all pixels except those belonging
to the 4-connected blob that is largest among all blobs, in the sense
of having the largest number of true pixels.  OK if input raster is
empty (i.e. all pixels false): if so, it is left unchanged. */

void rsblobs(unsigned char *ras, const int w, const int h)
{
  unsigned char *p, *pe;
  static unsigned char *bigblob, *blob;
  int wh, start_x, start_y, box_x, box_y, box_w, box_h, max_ntrue,
    ntrue;
  static int max_wh = 0;

  if((wh = w * h) > max_wh) {
    if(max_wh) {
      free((char *)blob);
      free((char *)bigblob);
    }
    blob = (unsigned char *)malloc_ch(wh);
    bigblob = (unsigned char *)malloc_ch(wh);
    max_wh = wh;
  }
  start_x = start_y = 0;
  if(!(findblob(ras, w, h, ERASE, NO_ALLOC, ORIG_BLOB, &start_x,
    &start_y, &bigblob, &box_x, &box_y, &box_w, &box_h)))
    return;
  for(pe = (p = bigblob) + wh, max_ntrue = 0; p < pe;)
    if(*p++)
      max_ntrue++;
  while(findblob(ras, w, h, ERASE, NO_ALLOC, ORIG_BLOB, &start_x,
    &start_y, &blob, &box_x, &box_y, &box_w, &box_h)) {
    for(pe = (p = blob) + wh, ntrue = 0; p < pe;)
      if(*p++)
        ntrue++;
    if(ntrue > max_ntrue) {
      max_ntrue = ntrue;
      memcpy(bigblob, blob, wh * sizeof(unsigned char));
    }
  }
  memcpy(ras, bigblob, wh * sizeof(unsigned char));
}

/*******************************************************************/

/* Sets the verbosity (0 = quiet, 1 = verbose) */

void setverbose(const int verbose)
{
  verbosity = verbose;
}

/******************************************************************/

/* If arg is positive, sleeps for that many seconds; if negative,
prints a prompt and waits for user to hit return key; otherwise,
does nothing */

void sleepity(const int sec)
{
  if(sec > 0)
    sleep(sec);
  else if(sec < 0) {
    printf("(Enter to continue)");
    getchar();
  }
}

/******************************************************************/

/* Computes and writes summary information for a test run: error rate
and confusion matrix */

void summary(const int nwrong, const int ntest, int *confuse, FILE *fp_out,
          const int nout, char *cls_str)
{
  char str[2000], str2[20];
  float pct_error;
  int rowsum, i, j;

  pct_error = 100. * (float)nwrong / (float)ntest;
  sprintf(str, "\npct error: %.2f\n\n", pct_error);
  for(i = 0; i < nout; i++) {
     sprintf(str2, "          %c ", cls_str[i]);
     strcat(str, str2);
  }
  strcat(str, "\n");
  for(i = 0; i < nout; i++) {
    sprintf(str2, "%c ", cls_str[i]);
    strcat(str, str2);
    for(rowsum = j = 0; j < nout; j++)
      rowsum += confuse[i*nout+j];
    for(j = 0; j < nout; j++) {
      sprintf(str2, "  %3d", confuse[i*nout+j]);
      strcat(str, str2);
      if(rowsum) {
        sprintf(str2, "(%5.1f)", 100. * (float)confuse[i*nout+j] /
          rowsum);
        strcat(str, str2);
      }
      else
        strcat(str, "(  -  )");
    }
    strcat(str, "\n");
  }
  fputs(str, fp_out);
  fclose(fp_out);
  if(isverbose())
    fputs(str, stdout);
}

/******************************************************************/

/* If input string begins with ~/, this expands the ~ to the user's
home dir; otherwise, it returns the input and attatches the INSTALL_DIR
if requested.
CAUTION: Returns the address of a static buffer; so, if you call it
multiple times with the intention of storing the several resulting
strings, you must copy the strings into other buffers, not just store
the returned addresses (which will all be the same). */

char *tilde_filename(char str[], const int add_path)
{
  char *p, *s;
  static char homedir[200], outstr[200];
  static int f = 1;

  if(f) {
    f = 0;
    if(!(p = getenv("HOME")))
      fatalerr("tilde_filename", "getenv of HOME failed", NULL);
    strcpy(homedir, p);
  }
  if(str[0] == '~' && str[1] == '/')
    sprintf(outstr, "%s/%s", homedir, str + 2);
  else if(add_path == 1)
    sprintf(outstr, "%s/%s", INSTALL_DATA_DIR, str);
  else if(add_path == 2)
    sprintf(outstr, "%s/%s", INSTALL_NBIS_DIR, str);
  else if(add_path == 3)
  { 
    s = basename(str);
    sprintf(outstr, "./%s", s);
  }
  else
    sprintf(outstr, "%s", str);
  return outstr;
}

/******************************************************************/

/* For use by the usage macro and by Usage_func. */

void usage_func(char *argv0, char *str)
{
  char usagestr[500];

  sprintf(usagestr, "%s %s", lastcomp(argv0), str);
  fatalerr("Usage", usagestr, NULL);
}

/*******************************************************************/

/* For use by the "Usage" macro. */

void Usage_func(const int argc, char *argv0, char *str)
{
  int n_space_or_newline;
  char *p;

  for(n_space_or_newline = 0, p = str; *p; p++)
    if(*p == ' ' || *p == '\n')
      n_space_or_newline++;
  if(argc - 2 != n_space_or_newline)
    usage_func(argv0, str);
}

/*******************************************************************/

/* Writes data to an ihdr file, putting "standard" values into many
ihdr fields */

void write_ihdr_std(unsigned char *data, const int width, const int height,
         const int depth, char *outfile)
{
  IHEAD head;
  char timestring[100];
  time_t thetime;

  nullihdr(&head);
  thetime = time(0);
  strcpy(timestring, ctime(&thetime));
  timestring[strlen(timestring) - 1] = 0;
  strcpy(head.created, timestring);
  sprintf(head.width, "%d", width);
  sprintf(head.height, "%d", height);
  sprintf(head.depth, "%d", depth);
  strcpy(head.compress, "0");
  strcpy(head.complen, "0");
  strcpy(head.align, "32");
  strcpy(head.unitsize, "32");
  head.sigbit = '0';
  head.byte_order = '0';
  strcpy(head.pix_offset, "0");
  strcpy(head.whitepix, "255");
  head.issigned = '0';
  head.rm_cm = '0';
  head.tb_bt = '0';
  head.lr_rl = '0';
  writeihdrfile(outfile, &head, data);
}
