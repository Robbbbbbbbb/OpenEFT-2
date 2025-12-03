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
      LIBRARY: IMAGE - Image Manipulation and Processing Routines

      FILE:    IMG_IO.C

      AUTHORS: Criag Watson
               Michael Garris
      DATE:    01/16/2001
      UPDATED: 03/15/2005 by MDG
      UPDATED: 12/22/2008 by Gregory Fiumara - add read_raw()/read_ihead()

      Contains routines responsible for alternatively reading/writing
      pixmaps from/to an IHead or raw image file.

      ROUTINES:
#cat: read_raw_from_filesize - reads a pixmap from an image file based
#cat:               on the size of the file in bytes.
#cat: write_raw_from_memsize - writes a pimap to an image file given
#cat:               a filled memory buffer.
#cat: read_ihead - reads a pixmap from an IHead image
#cat: read_raw - reads a pixmap from a raw image
#cat: read_raw_or_ihead_wsq - reads a pixmap from either an IHead or raw
#cat:               image file based on a specified flag and tests the
#cat:               pixmap's attributes to ensure WSQ compatability.
#cat: write_raw_or_ihead - writes a pixmap to either an IHead or
#cat:               raw image file based on a specified flag.

***********************************************************************/
#include <img_io.h>

/***********************************************************************/
/* Reads a pixmap from image file based on the byte size of the file . */
/***********************************************************************/
int read_raw_from_filesize(char *ifile, unsigned char **odata, int *ofsize)
{
   unsigned char *idata;
   int ret, n, fsize;
   FILE *infp;

   if((ret = filesize(ifile)) < 0)
      return(ret);
   fsize = ret;

   if((infp = fopen(ifile, "rb")) == (FILE *)NULL){
      fprintf(stderr, "ERORR : read_raw_from_filesize : fopen : %s\n", ifile);
      return(-2);
   }

   idata = (unsigned char *)malloc(fsize * sizeof(unsigned char));
   if(idata == (unsigned char *)NULL){
      fprintf(stderr, "ERORR : read_raw_from_filesize : malloc : idata\n");
      return(-3);
   }

   n = fread(idata, sizeof(unsigned char), fsize, infp);
   if(n != fsize){
      fprintf(stderr, "ERORR : main : read_raw_from_filesize : ");
      fprintf(stderr, "%d of %d bytes read from %s\n",
              n, fsize, ifile);
      return(-4);
   }

   fclose(infp);

   *odata = idata;
   *ofsize = fsize;

   return(0);
}

/***********************************************************************/
/* Writes a pixmap to an image file given a filled memory buffer.      */
/***********************************************************************/
int write_raw_from_memsize(char *ofile, unsigned char *odata, const int olen)
{
   FILE *outfp;
   int n;

   if((outfp = fopen(ofile, "wb")) == NULL) {
      fprintf(stderr, "ERROR: write_raw_from_memsize : fopen : %s\n", ofile);
      return(-2);
   }

   if((n = fwrite(odata, 1, olen, outfp)) != olen){
      fprintf(stderr, "ERROR: write_raw_from_memsize : fwrite : ");
      /* Typo corrected on following line by MDG on 03-15-05 */
      /* "%" with "%s" was missing */
      fprintf(stderr, "only %d of %d bytes written from file %s\n",
              n, olen, ofile);
      return(-3);
   }

   fclose(outfp);

   return(0);
}

/************************************************************/
/* Reads a pixmap from an IHead image file.                 */
/************************************************************/
int read_ihead(char *ifile, IHEAD **ohead, unsigned char **odata, int *owidth,
               int *oheight, int *odepth)
{
   IHEAD *ihead;
   unsigned char *idata;
   int width, height, depth;

    /* Read the specified input file as an IHead image. */
    ReadIheadRaster(ifile, &ihead, &idata, &width, &height, &depth);
    /* Image must be 8-bit grayscale. */
    if((depth != 8) && (depth != 24)){
       free(ihead);
       free(idata);
       fprintf(stderr, "ERROR: read_ihead : ");
       fprintf(stderr, "image depth = %d not 8 or 24\n", depth);
       return(-2);
    }
    *ohead = ihead;
    *odata = idata;
    *owidth = width;
    *oheight = height;
    *odepth = depth;
    return(0);
}

/************************************************************/
/* Reads a pixmap from a "raw" image file.                  */
/************************************************************/
int read_raw(char *ifile, unsigned char **odata, int *owidth, int *oheight,
             int *odepth)
{
   unsigned char *idata;
   int width, height, depth;
   int num_pix, img_siz;
   FILE *infp;

    /* Compute total number of pixels in input image. */
    width = *owidth;
    height = *oheight;
    depth = *odepth;

    if((depth != 8) && (depth != 24)){
       fprintf(stderr, "ERROR: read_raw : ");
       fprintf(stderr, "image depth = %d not 8 or 24\n", depth);
       return(-3);
    }

    num_pix = width * height * (depth>>3);
    /* Allocate the pixmap buffer. */
    idata = (unsigned char *)malloc(num_pix*sizeof(unsigned char));
    if(idata == (unsigned char *)NULL) {
       fprintf(stderr,"ERROR : read_raw : malloc : idata\n");
       return(-4);
    }
    /* Open the input image file for reading ... */
    if((infp = fopen(ifile, "rb")) == (FILE *)NULL) {
       fprintf(stderr, "ERROR: read_raw : %s\n", ifile);
       return(-5);
    }
    /* Read the pixmap from the open file. */
    img_siz = fread(idata, sizeof(unsigned char), num_pix, infp);
    /* If anticipated number of pixels not read, then ERROR. */
    if(img_siz != num_pix) {
       free(idata);
       fprintf(stderr, "ERROR : read_raw : fread : ");
       fprintf(stderr, "only read %d of %d bytes\n",
               img_siz, num_pix);
       return(-6);
    }
    /* Close the file. */
    fclose(infp);

    *odata = idata;
    *owidth = width;
    *oheight = height;
    *odepth = depth;
    return(0);
}

/************************************************************/
/* Reads a pixmap from either an IHead or "raw" image file. */
/************************************************************/
int read_raw_or_ihead_wsq(const int iheadflag, char *ifile, IHEAD **ohead,
               unsigned char **odata, int *owidth, int *oheight, int *odepth)
{
   int ret, width, height;

   /* If IHead image flagged ... */
   if(iheadflag) {
      if((ret = read_ihead(ifile, ohead, odata, owidth, oheight, odepth)))
         return ret;
   }
   /* Otherwise, input image is a raw pixmap... */
   else {
      if((ret = read_raw(ifile, odata, owidth, oheight, odepth)))
         return ret;
   }

   if(*odepth != 8){
      fprintf(stderr, "ERROR: read_raw_or_ihead_wsq : ");
      fprintf(stderr, "image depth = %d not 8\n", *odepth);
      return(-2);
   }

   width = *owidth;
   height = *oheight;
   /* If image is too small ... */
   if(width < MIN_IMG_DIM || height < MIN_IMG_DIM) {
      if(iheadflag) {
         free(*ohead);
      }
      free(*odata);
      fprintf(stderr,
              "ERROR: read_raw_or_ihead_wsq : Image must be at least %d X %d\n",
              MIN_IMG_DIM, MIN_IMG_DIM);
      fprintf(stderr,
              "              width = %d  ::  height = %d\n",
              width, height);
      return(-3);
   }

   return(0);
}

/************************************************************/
/* Writes a pixmap to either an IHead or "raw" image file.  */
/************************************************************/
int write_raw_or_ihead(const int iheadflag, char *ofile,
              unsigned char *odata, const int width, const int height,
              const int depth, const int ppi)
{
   FILE *outfp;
   int n, olen;
   IHEAD *ihead;

   if((depth != 8) && (depth != 24)){
      fprintf(stderr, "ERROR: write_raw_or_ihead : ");
      fprintf(stderr, "image depth = %d not 8 or 24\n", depth);
      return(-2);
   }

   outfp = fopen(ofile,"wb");
   if (outfp == (FILE *)NULL) {
      fprintf(stderr, "ERROR : write_raw_or_ihead : fopen : %s\n", ofile);
      return(-3);
   }
   if(iheadflag){
      ihead = (IHEAD *)malloc(sizeof(IHEAD));
      if(ihead == (IHEAD *)NULL){
         fprintf(stderr, "ERROR : write_raw_or_ihead : malloc : ihead\n");
         return(-5);
      }
      nullihdr(ihead);
      set_id(ihead, ofile);
      set_created(ihead);
      set_width(ihead, width);
      set_height(ihead, height);
      set_depth(ihead, depth);
      set_density(ihead, ppi);
      set_align(ihead, 8);
      set_compression(ihead, 0);
      set_complen(ihead, 0);
      /* If grayscale ... */
      if(depth == 8)
         set_whitepix(ihead, 255);
      /* Otherwise, RGB truecolor, so whitepix is ignored. */
      else
         set_whitepix(ihead, -1);

      writeihdr(outfp, ihead);
      free(ihead);
   }

   olen = width * height * (depth>>3);
   n = fwrite(odata, 1, olen, outfp);
   if (n != olen) {
      fprintf(stderr, "ERROR : write_raw_or_ihead : fwrite : odata\n");
      return(-5);
   }

   fclose(outfp);
   return(0);
}
