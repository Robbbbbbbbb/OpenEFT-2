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

      FILE:    IMGDECOD.C
      AUTHORS: Michael Garris
               
      DATE:    03/08/2001
      UPDATED: 03/15/2005 by MDG
               01/24/2008 by Kenneth Ko
      UPDATED: 01/06/2009 by Kenneth Ko - add support for HPUX compile
			02/25/2015 (Kenneth Ko) - Updated everything related to
                                      OPENJPEG to OPENJP2

      Contains routines responsible for taking a formatted datastream
      of potentially compressed image pixels, identifying the format
      type of the datastream if possible, and then decoding the
      datastream returning a reconstructed pixmap.

      ROUTINES:
#cat: read_and_decode_grayscale_image - identifies and reconstructs a
#cat:          potentially compressed datastream of grayscale image pixels
#cat:          (including AN2K files) for use by fingerprint image applications
#cat: read_and_decode_image - identifies and reconstructs a
#cat:          potentially compressed datastream of image pixels.
#cat: ihead_decode_mem - decodes (if necessary) a datastream of
#cat:          IHead formatted pixels from a memory buffer.

***********************************************************************/
#include <stdio.h>
#include <defs.h>
#include <img_io.h>
#include <imgutil.h>
#include <imgdecod.h>
#include <grp4deco.h>
#include <intrlv.h>
#include <util.h>

#ifdef __NBIS_PNG__
   #include <png_dec.h>
#endif
#ifdef __NBIS_JASPER__
   #include <jpeg2k.h>
#endif
#ifdef __NBIS_OPENJP2__
   #include <jpeg2k.h>
#endif


/*******************************************************************/
int read_and_decode_grayscale_image(char *ifile, int *oimg_type,
                    unsigned char **odata, int *olen,
                    int *ow, int *oh, int *od, int *oppi)
{
   int ret;
   unsigned char *idata;
   int img_type, ilen;
   int w, h, d, ppi;
   int intrlvflag;
   int hor_sampfctr[MAX_CMPNTS], vrt_sampfctr[MAX_CMPNTS], n_cmpnts;
   ANSI_NIST *ansi_nist;
   RECORD *imgrecord;
   int img_idc, img_imp, imgrecord_i;
   double ppmm;

   *odata = (unsigned char *)NULL;
   *olen = 0;

   /* Is input image an ANSI_NIST file?*/
   ret = is_ANSI_NIST_file(ifile);
   /* If system error ... */
   if(ret < 0)
      return(ret);
   /* YES, image is ANSI_NIST */
   if(ret == TRUE){
      /* Read the ANSI/NIST file into memory. */
      if((ret = read_ANSI_NIST_file(ifile, &ansi_nist)))
         return(ret);

      /* Get first grayscale fingerprint record in ansi/nist file. */
      ret = get_first_grayprint(&idata, &w, &h, &d,
                                &ppmm, &img_idc, &img_imp,
                                &imgrecord, &imgrecord_i, ansi_nist);
      /* If error ... */
      if(ret < 0){
         free_ANSI_NIST(ansi_nist);
         return(ret);
      }
      /* If grayscale fingerprint not found ... */
      if(!ret){
         fprintf(stderr, "ERROR : read_and_decode_grayscale_image : ");
         fprintf(stderr, "grayscale image record not found in %s\n", ifile);
         free_ANSI_NIST(ansi_nist);
         return(-2);
      }

      free_ANSI_NIST(ansi_nist);
      img_type = ANSI_NIST_IMG;
      ilen = w * h;
      ppi = sround(ppmm * MM_PER_INCH);
   }
   /* Image is not ANSI_NIST ... */
   else {

      /* Read in and decode image file. */
      if((ret = read_and_decode_image(ifile, &img_type, &idata, &ilen,
                                     &w, &h, &d, &ppi, &intrlvflag,
                                     hor_sampfctr, vrt_sampfctr, &n_cmpnts))){
         return(ret);
      }

      /* Image type UNKNOWN (perhaps raw), not supported */
      if(img_type == UNKNOWN_IMG){
         free(idata);
         fprintf(stderr, "ERROR : read_and_decode_grayscale_image : ");
         fprintf(stderr, "%s : image type UNKNOWN : not supported\n",
                 ifile);
         return(-3);
      }

      /* Only desire grayscale images ... */
      if(d != 8){
         free(idata);
         fprintf(stderr, "ERROR : read_and_decode_grayscale_image : ");
         fprintf(stderr, "%s : image depth : %d != 8\n",
              ifile, d);
         return(-4);
      }
   }

   *oimg_type = img_type;
   *odata = idata;
   *olen = ilen;
   *ow = w;
   *oh = h;
   *od = d;
   *oppi = ppi;

   return(0);
}

/*******************************************************************/
int read_and_decode_dpyimage(char *ifile, int *oimg_type,
                    unsigned char **odata, int *olen,
                    int *ow, int *oh, int *od, int *oppi)
{
   int ret, i, found;
   unsigned char *idata, *ndata;
   int img_type, ilen, nlen;
   int w, h, d, ppi;
   int intrlvflag;
   int hor_sampfctr[MAX_CMPNTS], vrt_sampfctr[MAX_CMPNTS], n_cmpnts;

   *odata = (unsigned char *)NULL;
   *olen = 0;

   /* Read in and decode image file. */
   if((ret = read_and_decode_image(ifile, &img_type, &idata, &ilen,
                                  &w, &h, &d, &ppi, &intrlvflag,
                                  hor_sampfctr, vrt_sampfctr, &n_cmpnts))){
      return(ret);
   }

   /* If image type is UNKNOWN ... simply return what was read. */
   if(img_type == UNKNOWN_IMG){
      *oimg_type = img_type;
      *odata = idata;
      *olen = ilen;
      *ow = w;
      *oh = h;
      *od = d;
      *oppi = ppi;

      return(0);
   }

   /* Dpyimage can only handle these pixel depths ... */
   if((d != 1) && (d != 8) && (d != 24)){
      fprintf(stderr, "WARNING : read_and_decode_dpyimage : ");
      fprintf(stderr, "file %s IGNORED : pixdepth = %d != {1,8,24}\n",
              ifile, d);
      return(IMG_IGNORE);
   }

   /* Dpyimage cannot handle downsampled component planes. */
   if((img_type == JPEGL_IMG) && (n_cmpnts > 1)){
      found = 0;
      for(i = 0; i < n_cmpnts; i++){
         if((hor_sampfctr[i] != 1) ||
            (vrt_sampfctr[i] != 1)){
             found = 1;
             break;
         }
      }
      if(found){
         fprintf(stderr, "WARNING : read_and_decode_dpyimage : ");
         fprintf(stderr, "file %s IGNORED : ", ifile);
         fprintf(stderr, "contains HV sample factor(s) != 1\n");
         return(IMG_IGNORE);
      }
   }

   /* Dpyimage cannot handle non-interleaved pixel data, */
   /* so interleave the image data if necessary. */
   if((d == 24) && (!intrlvflag)){
      if((ret = not2intrlv_mem(&ndata, &nlen, idata, w, h, d,
                              hor_sampfctr, vrt_sampfctr, n_cmpnts))){
         free(idata);
         return(ret);
      }
      free(idata);
      idata = ndata;
      ilen = nlen;
   }

   *oimg_type = img_type;
   *odata = idata;
   *olen = ilen;
   *ow = w;
   *oh = h;
   *od = d;
   *oppi = ppi;

   return(0);
}

/*******************************************************************/
int read_and_decode_image(char *ifile, int *oimg_type,
                    unsigned char **odata, int *olen,
                    int *ow, int *oh, int *od, int *oppi, int *ointrlvflag,
                    int *hor_sampfctr, int *vrt_sampfctr, int *on_cmpnts)
{
   int ret, i;
   unsigned char *idata, *ndata;
   int img_type, ilen, nlen;
   int w, h, d, ppi, lossyflag, intrlvflag = 0, n_cmpnts;
   IMG_DAT *img_dat;

   if((ret = read_raw_from_filesize(ifile, &idata, &ilen)))
      return(ret);

   if((ret = image_type(&img_type, idata, ilen))){
      free(idata);
      return(ret);
   }

   switch(img_type){
      case UNKNOWN_IMG:
           /* Return raw image data as read from file. */
           *oimg_type = img_type;
           *odata = idata;
           *olen = ilen;
           *ow = -1;
           *oh = -1;
           *od = -1;
           *oppi = -1;
           *ointrlvflag = -1;
           *on_cmpnts = -1;
           return(0);
      case WSQ_IMG:
           if((ret = wsq_decode_mem(&ndata, &w, &h, &d, &ppi, &lossyflag,
                                   idata, ilen))){
              free(idata);
              return(ret);
           }
           nlen = w * h;
           /* Pix depth always 8 for WSQ ... */
           n_cmpnts = 1;
           hor_sampfctr[0] = 1;
           vrt_sampfctr[0] = 1;
           break;
      case JPEGL_IMG:
           if((ret = jpegl_decode_mem(&img_dat, &lossyflag, idata, ilen))){
              free(idata);
              return(ret);
           }
           if((ret = get_IMG_DAT_image(&ndata, &nlen, &w, &h, &d, &ppi,
                                      img_dat))){
              free(idata);
              free_IMG_DAT(img_dat, FREE_IMAGE);
              return(ret);
           }
           /* JPEGL always returns non-interleaved data. */
           intrlvflag = 0;
           n_cmpnts = img_dat->n_cmpnts;
           if(d == 24){
              for(i = 0; i < n_cmpnts; i++){
                 hor_sampfctr[i] = img_dat->hor_sampfctr[i];
                 vrt_sampfctr[i] = img_dat->vrt_sampfctr[i];
              }
           }
           free_IMG_DAT(img_dat, FREE_IMAGE);
           break;
      case JPEGB_IMG:
           if((ret = jpegb_decode_mem(&ndata, &w, &h, &d, &ppi, &lossyflag,
                                     idata, ilen))){
              free(idata);
              return(ret);
           }
           if(d == 8){
              n_cmpnts = 1;
              intrlvflag = 0;
           }
           else if(d == 24){
              n_cmpnts = 3;
              intrlvflag = 1;
           }
           else{
              fprintf(stderr, "ERROR : read_and_decode_image : ");
              fprintf(stderr, "JPEGB decoder returned d=%d ", d);
              fprintf(stderr, "not equal to 8 or 24\n");
              free(idata);
              return(-2);
           }
           nlen = w * h * (d>>3);
           for(i = 0; i < n_cmpnts; i++){
              hor_sampfctr[i] = 1;
              vrt_sampfctr[i] = 1;
           }
           break;
      case IHEAD_IMG:
           if((ret = ihead_decode_mem(&ndata, &w, &h, &d, &ppi, &lossyflag,
                                     idata, ilen))){
              free(idata);
              return(ret);
           }

           nlen = SizeFromDepth(w,h,d);
           if((d == 1) || (d == 8)){
              n_cmpnts = 1;
              intrlvflag = 0;
           }
           else if(d == 24){
              n_cmpnts = 3;
              intrlvflag = 1;
           }
           else{
              fprintf(stderr, "ERROR : read_and_decode_image : ");
              fprintf(stderr, "IHead decoder returned d=%d ", d);
              fprintf(stderr, "not equal to {1,8,24}\n");
              free(idata);
              return(-2);
           }
           for(i = 0; i < n_cmpnts; i++){
              hor_sampfctr[i] = 1;
              vrt_sampfctr[i] = 1;
           }
           break;
#ifdef __NBIS_JASPER__
      case JP2_IMG:
           if((ret = jpeg2k_decode_mem(&img_dat, &lossyflag, idata, ilen))){
              free(idata);
              return(ret);
           }
           if((ret = get_IMG_DAT_image(&ndata, &nlen, &w, &h, &d, &ppi,
                                      img_dat))){
              free(idata);
              free_IMG_DAT(img_dat, FREE_IMAGE);
              return(ret);
           }
           /* JPEG2K always returns non-interleaved data. */
           intrlvflag = 0;
           n_cmpnts = img_dat->n_cmpnts;
           if(d == 24){
              for(i = 0; i < n_cmpnts; i++){
                 hor_sampfctr[i] = img_dat->hor_sampfctr[i];
                 vrt_sampfctr[i] = img_dat->vrt_sampfctr[i];
              }
           }
           free_IMG_DAT(img_dat, FREE_IMAGE);
           break;
#endif
#ifdef __NBIS_OPENJP2__
      case JP2_IMG:
           if((ret = openjpeg2k_decode_mem(&img_dat, &lossyflag, idata, ilen))){
              free(idata);
              return(ret);
           }

           if((ret = get_IMG_DAT_image(&ndata, &nlen, &w, &h, &d, &ppi,
                                      img_dat))){
              free(idata);
              free_IMG_DAT(img_dat, FREE_IMAGE);
              return(ret);
           }

           /* OPENJPEG always returns non-interleaved data. */
           intrlvflag = 0;
           n_cmpnts = img_dat->n_cmpnts;
           if(d == 24){
              for(i = 0; i < n_cmpnts; i++){
                 hor_sampfctr[i] = img_dat->hor_sampfctr[i];
                 vrt_sampfctr[i] = img_dat->vrt_sampfctr[i];
              }
           }
           free_IMG_DAT(img_dat, FREE_IMAGE);
           break;
#endif
#ifdef __NBIS_PNG__
      case PNG_IMG:
           if((ret = png_decode_mem(&img_dat, &lossyflag, idata, ilen))){
              free(idata);
              return(ret);
           }
           if((ret = get_IMG_DAT_image(&ndata, &nlen, &w, &h, &d, &ppi,
                                      img_dat))){
              free(idata);
              free_IMG_DAT(img_dat, FREE_IMAGE);
              return(ret);
           }
           /* PNG always returns non-interleaved data. */
           intrlvflag = 0;
           n_cmpnts = img_dat->n_cmpnts;
           if(d == 24){
              for(i = 0; i < n_cmpnts; i++){
                 hor_sampfctr[i] = img_dat->hor_sampfctr[i];
                 vrt_sampfctr[i] = img_dat->vrt_sampfctr[i];
              }
           }
           free_IMG_DAT(img_dat, FREE_IMAGE);
           break;
#endif
      default:
           fprintf(stderr, "ERROR : read_and_decode_image : ");
           fprintf(stderr, "illegal image type = %d\n", img_type);
           return(-3);
   }

   free(idata);

   *oimg_type = img_type;
   *odata = ndata;
   *olen = nlen;
   *ow = w;
   *oh = h;
   *od = d;
   *oppi = ppi;
   *ointrlvflag = intrlvflag;
   *on_cmpnts = n_cmpnts;

   return(0);
}

/*******************************************************************/
int ihead_decode_mem(unsigned char **oodata, int *ow, int *oh, int *od,
                     int *oppi, int *lossyflag,
                     unsigned char *idata, const int ilen)
{
   IHEAD *ihead;
   unsigned char *odata, *iptr;
   int olen, obytes, w, h, d, ppi;
   int compcode, complen = 0;

   /* Skip first fized length size field. */
   iptr = idata + SHORT_CHARS;
   ihead = (IHEAD *)iptr;
   iptr += sizeof(IHEAD);

   w = get_width(ihead);
   h = get_height(ihead);
   d = get_depth(ihead);
   ppi = get_density(ihead);
   compcode = get_compression(ihead);

   olen = SizeFromDepth(w,h,d);
   odata = (unsigned char *)malloc(olen);
   if(odata == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : ihead_decode_mem : malloc : odata\n");
      return(-2);
   }

   if(compcode != UNCOMP){
      complen = get_complen(ihead);
   }

   switch (compcode) {
      case RL:
         rldecomp(iptr, complen, odata, &obytes, olen);
         set_compression(ihead, UNCOMP);
         set_complen(ihead, 0);
         break;
      case CCITT_G4:
         if(ihead->sigbit == LSBF) {
           inv_bytes(iptr, complen);
           ihead->sigbit = MSBF;
           ihead->byte_order = HILOW;
         }
         grp4decomp(iptr, complen, w, h, odata, &obytes);
         set_compression(ihead, UNCOMP);
         set_complen(ihead, 0);
         break;
      case UNCOMP:
         memcpy(odata, iptr, olen);
      break;
      default:
         fprintf(stderr, "ERROR : ihead_decode_mem : ");
         fprintf(stderr, "invalid compression code = %d\n", compcode);
         return(-3);
      break;
   }

   *oodata = odata;
   *ow = w;
   *oh = h;
   *od = d;
   *oppi = ppi;
   *lossyflag = 0;

   return(0);
}
