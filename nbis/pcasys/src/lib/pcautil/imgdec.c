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

      FILE:    IMGDEC.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    02/15/2000
      UPDATED: 04/20/2005 by MDG
               01/24/2008 by Kenneth Ko
               01/06/2009 by Kenneth Ko - add support for HPUX compile
      UPDATED: 02/04/2009 by Joseph C. Konczal
	  UPDATED: 03/25/2011 by Kenneth Ko
               02/25/2015 (Kenneth Ko) - Updated everything related to
                                         OPENJPEG to OPENJP2
      
      Contains routines responsible for taking a formatted datastream
      of potentially compressed image pixels, identifying the format
      type of the datastream if possible, and then decoding the
      datastream returning a reconstructed pixmap and the fingerprint
      class.

      ROUTINES:
#cat: read_and_decode_pcasys - identifies and reconstructs a
#cat:          potentially compressed datastream of image pixels 
#cat:          for use by PCASYS.
#cat: get_nistcom_class - Gets a fingerprint class from a NISTCOM
#cat:          structure.
#cat: get_sd_ihead_class - Gets a fingerprint class from a Special
#cat:          Database IHEAD structure.
#cat: get_class_id - Gets the class ID # relative to a class string.

***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <imgdec.h>
#include <img_io.h>
#include <imgtype.h>
#include <wsq.h>
#include <jpegb.h>
#include <jpegl.h>
#include <jpeglsd4.h>
#include <ihead.h>
#include <nistcom.h>
#include <imgdecod.h>

#ifdef __NBIS_PNG__
   #include <png_dec.h>
#endif
#ifdef __NBIS_JASPER__
   #include <jpeg2k.h>
#endif
#ifdef __NBIS_OPENJP2__
   #include <jpeg2k.h>
#endif


extern char *get_id(IHEAD *);
extern int wsq14_decode_file(unsigned char **, int *, int *,
                 int *, int *, FILE *);

/*******************************************************************/
int read_and_decode_pcasys(char *ifile, unsigned char **odata, int *ow,
                           int *oh, int *olossy_flag, char *oclass)
{
   int ret;
   FILE *fp_wsq14;
   unsigned char *idata, *ndata, *iptr;
   int img_type, ilen, nlen;
   int w, h, d, ppi, lossyflag;
   char class;
   IMG_DAT *img_dat;
   IHEAD *ihead, *ihead_wsq14;
   NISTCOM *nistcom;
   int lossy_flag, compcode;


   nistcom = NULL;
   class = (char)0;
   lossy_flag = -1;
   iptr = NULL;
   ihead = NULL;
   compcode = -1;

   if((ret = read_raw_from_filesize(ifile, &idata, &ilen)))
      return(ret);

   if((ret = image_type(&img_type, idata, ilen))){
      free(idata);
      return(ret);
   }

   switch(img_type){
      case UNKNOWN_IMG:
           fprintf(stderr, "ERROR : read_and_decode_pcasys : unknown file");
           fprintf(stderr, "type for file %s\n", ifile);
           free(idata);
           return(-2);
           break;
      case WSQ_IMG:
           if((ret = wsq_decode_mem(&ndata, &w, &h, &d, &ppi, &lossyflag,
                                   idata, ilen))){
              free(idata);
              return(ret);
           }

           if (*oclass == '\0') {
              if((ret = getc_nistcom_wsq(&nistcom, idata, ilen))){
                 free(idata);
                 free(ndata);
                 return(ret);
              }
              free(idata);

              if((ret = get_nistcom_class(nistcom, &class))) {
                 free(ndata);
                 freefet(nistcom);
                 return(ret);
              }
              freefet(nistcom);
           }
           else {
              class = *oclass;
           }
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
           free_IMG_DAT(img_dat, FREE_IMAGE);

           if(d != 8) {
              fprintf(stderr, "ERROR : read_and_decode_pcasys : jpegl");
              fprintf(stderr, " : pcasys only accepts grayscale images\n");
              free(idata);
              free(ndata);
              return(-3);
           }

           if (*oclass == '\0') {
              if((ret = getc_nistcom_jpegl(&nistcom, idata, ilen))){
                 free(idata);
                 free(ndata);
                 return(ret);
              } 
              free(idata);

              if((ret = get_nistcom_class(nistcom, &class))){
                 free(ndata);
                 freefet(nistcom);
                 return(ret);
              }
              freefet(nistcom);
           }
           else {
              class = *oclass;
           }
           break;

      case JPEGB_IMG:
           if((ret = jpegb_decode_mem(&ndata, &w, &h, &d, &ppi, &lossyflag,
                                     idata, ilen))){
              free(idata);
              return(ret);
           }

           if(d != 8) {
              fprintf(stderr, "ERROR : read_and_decode_pcasys : jpegb");
              fprintf(stderr, " : pcasys only accepts grayscale images\n");
              free(idata);
              free(ndata);
              return(-4);
           }

          if (*oclass == '\0') {
             if((ret = getc_nistcom_jpegl(&nistcom, idata, ilen))){
                free(idata);
                free(ndata);
                return(ret);
             }
             free(idata);

             if((ret = get_nistcom_class(nistcom, &class))){
                free(ndata);
                freefet(nistcom);
                return(ret);
             }
              freefet(nistcom);
           }  
           else {
              class = *oclass;
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
           free_IMG_DAT(img_dat, FREE_IMAGE);

           if(d != 8) {
              fprintf(stderr, "ERROR : read_and_decode_pcasys : jp2");
              fprintf(stderr, " : pcasys only accepts grayscale images\n");
              free(idata);
              free(ndata);
              return(-3);
           }

           class = *oclass;
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
           free_IMG_DAT(img_dat, FREE_IMAGE);

           if(d != 8) {
              fprintf(stderr, "ERROR : read_and_decode_pcasys : jp2");
              fprintf(stderr, " : pcasys only accepts grayscale images\n");
              free(idata);
              free(ndata);
              return(-3);
           }

           class = *oclass;
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
           free_IMG_DAT(img_dat, FREE_IMAGE);

           if(d != 8) {
              fprintf(stderr, "ERROR : read_and_decode_pcasys : png");
              fprintf(stderr, " : pcasys only accepts grayscale images\n");
              free(idata);
              free(ndata);
              return(-3);
           }

           class = *oclass;
           break;
#endif
      case IHEAD_IMG:

           if(img_type == IHEAD_IMG) {
              /* Skip first fized length size field. */
              iptr = idata + SHORT_CHARS;
              ihead = (IHEAD *)iptr;
              iptr += sizeof(IHEAD);
              compcode = get_compression(ihead);
           }

           if(compcode == JPEG_SD) {
              w = get_width(ihead);
              h = get_height(ihead);
              d = get_depth(ihead);
              lossy_flag = 0;
              ndata = (unsigned char *)malloc(w*h*sizeof(unsigned char));
              if(ndata == NULL){
                 fprintf(stderr, "ERROR : read_and_decode_pcasys : malloc");
                 fprintf(stderr, " ndata for JPEGL_SD\n");
                 free(idata);
                 return(-5);
              }

              if (*oclass == '\0') {
                 if((ret = getc_nistcom_jpegl(&nistcom, idata, ilen))){
                    free(idata);
                    free(ndata);
                    return(ret);
                 }
                 free(idata);

                 if((ret = get_nistcom_class(nistcom, &class))){
                    free(ndata);
                    freefet(nistcom);
                    return(ret);
                 }
                 freefet(nistcom);
              }  
              else {
                 class = *oclass;
              }
           }
           else if(compcode == WSQ_SD14) {
              if((fp_wsq14 = fopen(ifile,"rb")) == NULL) {
                 fprintf(stderr, "ERROR: read_and_decode_pcasys : fopen : ");
                 fprintf(stderr, "%s\n",ifile);
                 return(-6);
              }
              ihead_wsq14 = readihdr(fp_wsq14);
              free(ihead_wsq14);
              if((ret = wsq14_decode_file(&ndata, &w, &h, &d, &lossyflag,
                                         fp_wsq14))){
                 free(idata);
                 fclose(fp_wsq14);
                 return(ret);
              }
              fclose(fp_wsq14);

              if (*oclass == '\0') {
                 if((ret = get_sd_ihead_class(ihead, &class))){
                    free(idata);
                    free(ndata);
                    return(ret);
                 }
              }
              else {
                 class = *oclass;
              }
           }
           else {
              if((ret = ihead_decode_mem(&ndata, &w, &h, &d, &ppi, &lossyflag,
                                        idata, ilen))){
                 free(idata);
                 return(ret);
              }

              if(d != 8) {
                 fprintf(stderr, "ERROR : read_and_decode_pcasys : ihead");
                 fprintf(stderr, " : pcasys only accepts grayscale images\n");
                 free(ndata);
                 free(idata);
                 return(-7);
              }
           }
           free(idata);
           break;

      default:
           fprintf(stderr, "ERROR : read_and_decode_pcasys : illegal file");
           fprintf(stderr, "type (%d) for file %s\n", img_type, ifile);
           free(idata);
           return(-8);
   }

   *odata = ndata;
   *ow = w;
   *oh = h;
   *oclass = class;
   *olossy_flag = lossy_flag;

   return(0);
}

/*******************************************************************/
int get_nistcom_class(NISTCOM *nistcom, char *oclass)
{
   int ret;
   char *class_str;

   if(nistcom == NULL){
      *oclass = '\0';
      return(0);
   }

   if((ret = extractfet_ret(&class_str, NCM_FING_CLASS, nistcom)))
      return(ret);

   *oclass = class_str[0];
   return(0);
}

/*******************************************************************/
int get_sd_ihead_class(IHEAD *ihead, char *oclass)
{
   int ret, n, sd_id;
   char class, *id_str, name[BUFSIZE];

   if(ihead == NULL) {
      *oclass = '\0';
      return(0);
   }

   id_str = get_id(ihead);
   if((n = sscanf(id_str, "%s", name)) <= 0){
      fprintf(stderr, "ERROR : get_sd_ihead_class : getting file");
      fprintf(stderr, " name from ihead structure\n");
      return(-2);
   }

   if(strchr(name, '_')) { /* Special Database 4 */
      if((ret = get_sd_class(id_str, 4, &class)))
         return(ret);
   }
   else{ /* Special Database 9, 10, or 14 */
      if(!strncmp(id_str, "f0", 2) || !strncmp(id_str, "s0", 2))
         sd_id = 9;
      else
         sd_id = 10;

      if((ret = get_sd_class(id_str, sd_id, &class)))
         return(ret);
   }

   *oclass = class;
   return(0);
}

/*******************************************************************/
void get_class_id(const char class_let, char *cls_str,
          unsigned char *oclass_id)
{
   unsigned char class_id;
   char *cptr;

   cptr = strchr(cls_str, class_let);

   if(cptr == NULL)
      class_id = 255;
   else
      class_id = cptr - cls_str;

   *oclass_id = class_id;
   return;
}
