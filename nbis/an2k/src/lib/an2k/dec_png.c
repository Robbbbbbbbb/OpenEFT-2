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
      LIBRARY: AN2K - ANSI/NIST 2007 Reference Implementation
                    
      FILE:    DEC_PNG
      AUTHOR:  Kenneth Ko
      DATE:    01/14/2008
      MODIFIED: 10/07/2008 by Joseph C. Konczal - eliminated tmp file,
                implemented reading of a PNG image directly from memory.
      UPDATE:  01/26/2008 jck - report more details when things go wrong

      Contains routines responsible for decoding image data contained
      in image records according to the ANSI/NIST 2007 standard.
      
***********************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include <png_dec.h>
#include <an2k.h>

/***********************************************************************
************************************************************************
#cat: png_mem_read_data - A callback function for libpng to use to
#cat:              read a PNG image from a memory buffer instead of
#cat:              from a file.  The required form and usage is
#cat:              described in the PNG manual.

   Input:
      png_ptr
      length
   Output:
      data
   Return Code:
      none - calls png_error() on error, i.e., read past the end
************************************************************************/
void png_mem_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   struct png_mem_io_struct *iop = 
      (struct png_mem_io_struct *)png_get_io_ptr(png_ptr);
   int have = iop->end - iop->cur;

   if (have < (int)length) {
      png_error(png_ptr, "Read Error");
   }

   memcpy(data, iop->cur, length);
   iop->cur += length;
}

/***********************************************************************
************************************************************************
#cat: read_png_stream - Read a PNG image from a data stream that has
#cat:              already been initialized to read from a file or a
#cat:              memory buffer.

   Input:
      png_ptr    - a png_structp intialized for reading
      info_ptr   - a png_infop initialized for reading from png_ptr
   Output:
      oimg_dat   - a pointer to a pointer to an NBIS image data structure
                   that will contain the results
   Return Code:
      zero       - success
      negative   - error, an error message is written to stdout
************************************************************************/
static int read_png_stream(png_structp png_ptr, png_infop info_ptr,
			   IMG_DAT **oimg_dat)
{
   int y, ret;
   IMG_DAT           *img_dat;

   png_bytep *row_pointers;

   int number_of_passes;

   /* initialize stuff */
   png_read_info(png_ptr, info_ptr);

   number_of_passes = png_set_interlace_handling(png_ptr);
   png_read_update_info(png_ptr, info_ptr);

   /* read image */
   if (setjmp(png_jmpbuf(png_ptr)) != 0){
      fprintf(stderr, "ERROR : read_png_stream :"
	      "setjmp : error during JASPER decoding of PNG image\n");
      return (-6);
   }

   row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * info_ptr->height);
   if (row_pointers == NULL) {
      fprintf(stderr, "ERROR : read_png_stream : "
	      "malloc %u row_pointers (%lu bytes)\n",
	      (unsigned int)info_ptr->height,
	      info_ptr->height * sizeof(png_bytep));
      return (-8);
   }

   for (y=0; y<(int)info_ptr->height; y++) {
      row_pointers[y] = (png_byte*) malloc((size_t)info_ptr->rowbytes);
      if (row_pointers[y] == NULL) {
	 fprintf(stderr, "ERROR : read_png_stream : "
		 "malloc row %d (%u bytes)\n",
		 y, (unsigned int)info_ptr->rowbytes);
	 return (-9);
      }
   }

   png_read_image(png_ptr, row_pointers);

   if (get_raw_image(row_pointers, info_ptr, &img_dat) != 0)
      ret = -7;	    /* report the error after the row pointers are freed */
   else
      ret = 0;

   for (y=0; y<(int)info_ptr->height; y++)
      free(row_pointers[y]);
   free(row_pointers);

   if (ret == 0)
      *oimg_dat = img_dat;

   return ret;
}

/***********************************************************************
************************************************************************
#cat: png_decode_mem - Read a PNG image from a memory buffer.

   Input:
      idata      - input buffer containg a PNG image
      ilen       - size of image data, in bytes
   Output:
      oimg_dat   - a pointer to a pointer to an NBIS image data structure
                   that will contain the results
      lossyflag  - indicates whether lossy compression was used
   Return Code:
      zero       - success
      negative   - error, an error message is written to stdout
************************************************************************/
int png_decode_mem(IMG_DAT **oimg_dat, int *lossyflag,
                   unsigned char *idata, const int ilen)
{
   int ret;
   png_structp png_ptr;
   png_infop info_ptr;
   struct png_mem_io_struct ios;

   /* ANSI-C (C89) would not allow non-constant initialization of struct. */
   ios.cur = idata;
   ios.end = idata+ilen;

   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (NULL == png_ptr) {
      return -1;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (NULL == info_ptr) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      return -2;
   }

   if (setjmp(png_jmpbuf(png_ptr)) != 0) {
      png_destroy_read_struct(&png_ptr,&info_ptr, NULL);
      return -3;
   }

   png_set_read_fn(png_ptr, &ios, png_mem_read_data);

   ret = read_png_stream(png_ptr, info_ptr, oimg_dat);

   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

   *lossyflag = 0;		/* PNG is lossless */
   return ret;
}

/***********************************************************************
************************************************************************
#cat: read_png_file - Read a PNG image from a file.

   Input:
      filename   - the name of a PNG image file
   Output:
      oimg_dat   - a pointer to a pointer to an NBIS image data structure
                   that will contain the results
   Return Code:
      zero       - success
      negative   - error, an error message is written to stdout
************************************************************************/
int read_png_file(char *filename, IMG_DAT **oimg_dat)
{
   int ret, nread;
   unsigned char header[8];

   png_structp png_ptr;
   png_infop info_ptr;

   /* open file and test for it being a png */
   FILE *fp = fopen(filename, "rb");
   if (NULL == fp){
      fprintf(stderr, "ERROR : read_png_file: failed to open file '%s': %s\n",
	      filename, strerror(errno));
      return (-1);
   }

   nread = (int)fread((char *)header, 1, 8, fp);

   if (nread != 8) {
      char *errmsg = SHORT_READ_ERR_MSG(fp);
      fprintf(stderr, "ERROR: read_png_file : "
	      "fread header: only %d bytes read of 8, at byte %ld, %s\n",
	      nread, ftell(fp), errmsg);
      return(-11);
   }

   if (png_sig_cmp(header, 0, 8) != 0){
      fprintf(stderr, "ERROR : read_png_file: expected 8-byte PNG signature, "
	      "found 0x %02x %02x %02x %02x %02x %02x %02x %02x\n",
	      (unsigned int)header[0], (unsigned int)header[1],
	      (unsigned int)header[2], (unsigned int)header[3],
	      (unsigned int)header[4], (unsigned int)header[5],
	      (unsigned int)header[6], (unsigned int)header[7]);
      return (-2);
   }

   /* initialize stuff */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
   if (NULL == png_ptr){
      fprintf(stderr, "ERROR : read_png_file: null png_ptr\n");
      return (-3);
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (NULL == info_ptr){
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      fprintf(stderr, "ERROR : read_png_file: null png_ptr\n");
      return (-4);
   }

   if (setjmp(png_jmpbuf(png_ptr)) != 0) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return (-5);
   }

   png_init_io(png_ptr, fp);
   png_set_sig_bytes(png_ptr, 8);

   ret = read_png_stream(png_ptr, info_ptr, oimg_dat);

   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   (void)fclose(fp);
   
   return ret;
}

int get_raw_image(png_bytep *row_pointers, png_info *info_ptr, 
                  IMG_DAT **oimg_dat)
{
   IMG_DAT *img_dat;
   int y, x, i, j;
   int rwcnt;
   int index;
   int max_hor, max_vrt;
   int pixel_d;

   /* Allocate memory for image data structure. */
   img_dat = (IMG_DAT *)calloc(1, sizeof(IMG_DAT));
   if(img_dat == NULL){
      fprintf(stderr, "ERROR : img_dat_generate: "
	      "calloc : img_dat (%lu bytes)\n", (unsigned long)sizeof(IMG_DAT));
      return(-2);
   }

   /* Initialize img_dat info */
   img_dat->max_width = info_ptr->width;
   img_dat->max_height = info_ptr->height;

   if (info_ptr->channels == 4){
      img_dat->pix_depth = 8 * (info_ptr->channels - 1);
      img_dat->n_cmpnts = info_ptr->channels - 1;
   }
   else{
      img_dat->pix_depth = 8 * info_ptr->channels;
      img_dat->n_cmpnts = info_ptr->channels;
   }

   img_dat->ppi = -1;
   img_dat->intrlv = 0;
   img_dat->cmpnt_depth = 8;

   max_hor = -1;
   max_vrt = -1;

   for(i = 0; i < img_dat->n_cmpnts; i++){
      img_dat->hor_sampfctr[i] = 1;
      img_dat->vrt_sampfctr[i] = 1;
      if(max_hor < img_dat->hor_sampfctr[i])
         max_hor = img_dat->hor_sampfctr[i];
      if(max_vrt < img_dat->vrt_sampfctr[i])
         max_vrt = img_dat->vrt_sampfctr[i];
   }

   for(i = 0; i < img_dat->n_cmpnts; i++){
      img_dat->samp_width[i] = (int)ceil(img_dat->max_width *
                   (img_dat->hor_sampfctr[i]/(double)max_hor));
      img_dat->samp_height[i] = (int)ceil(img_dat->max_height *
                   (img_dat->vrt_sampfctr[i]/(double)max_vrt));
   }

   rwcnt = img_dat->max_width * img_dat->max_height;

   /* Allocate memory for each image data -> component plains. */
   for(i = 0; i < img_dat->n_cmpnts; i++){
      img_dat->image[i] = 
                 (unsigned char *)malloc(rwcnt * sizeof(unsigned char));
      if(img_dat->image[i] == NULL){
         fprintf(stderr, "ERROR : get_raw_image : "
		 "calloc : img_dat->image[%d] (%lu bytes)\n",
		 i, (unsigned long)(rwcnt * sizeof(unsigned char)));
	 for(j = 0; j < i; j++)
	    free(img_dat->image[j]);
	 free(img_dat);
	 return(-3);  
      }
   }

   /* Put the image raw pixels to image data sturcture component plans. */
   pixel_d = info_ptr->channels;
   index = 0;
   for(y = 0; y < info_ptr->height; y++) {
      png_byte* row = row_pointers[y];
      for(x = 0; x < info_ptr->width; x++) {
         png_byte* ptr = &(row[x * pixel_d]);
         for(i = 0; i < img_dat->n_cmpnts; i++){
            *(img_dat->image[i]+index) = ptr[i];
         }
         index++;
      }
   }

   *oimg_dat = img_dat;

   return (0);
}

