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

      FILE:    IMGTYPE.C

      AUTHOR:  Criag Watson & Mike Garris
      DATE:    08/31/2004
      UPDATED: 03/15/2005 by MDG
      UPDATED: 10/07/2008 by Joseph C. Konczal
      UPDATED: 01/06/2008 by Kenneth Ko - add support for HPUX compile
      UPDATED: 07/10/2014 by Kenneth Ko
               02/25/2015 (Kenneth Ko) - Updated everything related to
                                         OPENJPEG to OPENJP
      Contains routines responsible for automatically determining the
      format of a pixmap file based on its contents.

      ROUTINES:
#cat: image_type - takes an image data stream and determines if it
#cat:              is from a WSQ, JPEGL, JPEGB, IHead, ANSI_NIST
#cat:              or UNKNOWN file.
#cat: jpeg_type - takes an image data stream and determines if it
#cat:             is from a JPEGL or JPEGB file.

***********************************************************************/
#include <stdio.h>
#include <imgtype.h>

/*******************************************************************/
/* Determine if image data is of type IHEAD, WSQ, JPEGL, JPEGB or  */
/*    ANSI_NIST                                                    */
/*******************************************************************/
int image_type(int *img_type, unsigned char *idata, const int ilen)
{
   int ret;
   unsigned short marker;
   unsigned char *cbufptr, *ebufptr;
   char ihdr_size[SHORT_CHARS];
   unsigned char header[8];

   cbufptr = idata;
   ebufptr = idata + ilen;

   if((ret = getc_ushort(&marker, &cbufptr, ebufptr)))
      return(ret);
   if(marker == SOI_WSQ){
      *img_type = WSQ_IMG;
      return(0);
   } else  if(marker == SOI){
      if((ret = jpeg_type(img_type, idata, ilen)))
         return(ret);
      return(0);
   }

   sprintf(ihdr_size, "%d", IHDR_SIZE);
   if(strncmp((char *)idata, ihdr_size, strlen(ihdr_size)) == 0){
      *img_type = IHEAD_IMG;
      return(0);
   }

#ifdef __NBIS_PNG__
   memcpy(header, idata, 8);
   if (!(png_sig_cmp(header, 0, 8))){
      *img_type = PNG_IMG;
      return(0);
   }
#endif

#ifdef __NBIS_JASPER__
   if (is_jp2(idata, ilen) > 0){
      *img_type = JP2_IMG;
      return(0);
   }
#endif

#ifdef __NBIS_OPENJP2__
   if (is_jp2(idata, ilen) > 0){
      *img_type = JP2_IMG;
      return(0);
   }
#endif


   ret = is_ANSI_NIST(idata, ilen);
   /* if system error */
   if(ret < 0)
      return(ret);
   if(ret == TRUE){
      *img_type = ANSI_NIST_IMG;
      return(0);
   }

   /* Otherwise, image type is UNKNOWN ... */
   *img_type = UNKNOWN_IMG;

   return(0);
}

/****************************************************************************/
/* Determines JPEG image type by finding SOF marker in the compressed data. */
/* The SOF's are different for each JPEG compression type JPEGL and JPEGB.  */
/****************************************************************************/
int jpeg_type(int *img_type, unsigned char *idata, const int ilen)
{
   int ret;
   unsigned short marker;
   unsigned char *cbufptr, *ebufptr;

   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Get SOI */
   if((ret = getc_marker_jpegl(&marker, SOI, &cbufptr, ebufptr)))
      return(ret);

   /* Get next marker. */
   if((ret = getc_marker_jpegl(&marker, ANY, &cbufptr, ebufptr)))
      return(ret);

   /* While not at Start of Scan (SOS) -     */
   /*    the start of encoded image data ... */
   while(marker != SOS){
      if(marker == SOF3){
         *img_type = JPEGL_IMG;
         return(0);
      }
      else if(marker == SOF0){
         *img_type = JPEGB_IMG;
         return(0);
      }
      /* Skip marker segment. */
      if((ret = getc_skip_marker_segment(marker, &cbufptr, ebufptr)))
         return(ret);

      /* Get next marker. */
      if((ret = getc_marker_jpegl(&marker, ANY, &cbufptr, ebufptr)))
         return(ret);
   }

   /* JPEG type not found ... */
   fprintf(stderr, "ERROR : jpeg_type : Could not determine JPEG type ");
   fprintf(stderr, "(ie. baseline or lossless)\n");
   *img_type = -1;
   return(-2);
}

/****************************************************************************/
/* Determines is JP2 format. */
/****************************************************************************/
#ifdef __NBIS_JASPER__
int is_jp2(unsigned char *idata, const int ilen)
{
   jas_stream_t      *in;
   int               ret;

   if (jas_init()){
      fprintf(stderr, "ERROR : is_jp2: init : jas\n");
      return(-1);
   }
   
   /* The input image is to be read from buffer stream. */
   in = jas_stream_memopen((char *)idata, ilen);
   if (in == NULL){
      fprintf(stderr, "ERROR : is_jp2: failed to open jas stream\n");
      return(-2);
   }

   /* Check is JP2 format. */
   ret = jas_image_getfmt(in);

   /* General clean up. */  
   (void) jas_stream_close(in);

   return(ret);
}
#endif

#ifdef __NBIS_OPENJP2__
int is_jp2(unsigned char *idata, const int ilen)
{
   int	ret;
   int 	i;
   unsigned char *nptr;
   unsigned char buf[4];

   ret = 0;
   nptr = idata;
   nptr += 4;
   memcpy(buf, nptr, 4 * sizeof(unsigned char));
   
   if (memcmp(buf, "jP  ", 4 * sizeof(unsigned char)) == 0)
   {
      ret = 1;
   }

   return(ret);
}
#endif

