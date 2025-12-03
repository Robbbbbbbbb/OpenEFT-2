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

      FILE:    IMGBOOST.C
      AUTHORS: Michael Garris
               Craig Watson
               
      DATE:    04/18/2002
      UPDATED: 03/15/2005 by MDG

      Contains routines responsible for taking "boosting" the contrast
      in an image.

      ROUTINES:
#cat: trim_histtails_contrast_boost - given an input grayscal image, this
#cat:          routine comptues and analyzes the pixel intensity historgram
#cat:          trimming both distribution tails and then "expands" pixel
#cat:          intensities across the computed intensity range.

***********************************************************************/
#include <imgboost.h>
#include <string.h>

/*************************************************************************
**************************************************************************

**************************************************************************/
void trim_histtails_contrast_boost(unsigned char *idata,
                                   const int iw, const int ih) 
{
   int bins[256];
   int i, inrun, cnt;
   int img_len, img_mn, img_mxv, img_mnv;
   unsigned char *iptr;
   float factr;

   /* Set histogram bins to 0 */
   memset(bins, 0, 256*sizeof(int));

   /* Compute pixel length of image */
   img_len = iw*ih;
   /* Set pixel intensity mean to 0 */
   img_mn = 0;
   /* Set pointer to start of image */
   iptr = idata;
   /* Foreach pixel in image ... */
   for(i = 0; i < img_len; i++) {
      /* Accumulate pixel intensity for mean */
      img_mn += *iptr;
      /* Bump pixel intensity bin in historgram */
      bins[*iptr]++;
      /* Bump to next pixel in image */
      iptr++;
   }

/*
   for(i = 0; i < 256; i++)
      printf("%d %d\n", i, bins[i]);
*/

   /* Compute mean from sum of intensities */
   img_mn /= img_len;


   /* 1. Find upper pixel limit on tail of intensity distribution */
   /* Set in low-bin run flag to FALSE */
   inrun = 0;
   /* Set upper pixel limit to brightest intesity */
   img_mxv = 255;
   /* Set duration count to 0 */
   cnt = 0;
   /* Foreach bin in historam starting at the mean intensity and */
   /* searching forward ... */
   for(i = img_mn; i < 256; i++) {
      /* If pixel intensity bin count is too low ... */
      if(bins[i] < BIN_TOO_LOW) {
         /* Then may be in upper tail of histogram distribution */
         /* So, if previously not in a low-bin run ... */ 
         if(!inrun) {
            /* Set low-bin run flag to TRUE */
            inrun = 1;
            /* Init duration count to 0 */
            cnt = 0;
         }
         /* Either way, we are in a low-bin run, so bump */
         /* duration count */
         cnt++;
      }
      /* Otherwise, we are not in a low-bin run */
      else
         /* Set low-bin run flag to FALSE */
         inrun = 0;

      /* If duration count is large enough ... */
      if(cnt == LONG_LOW_BIN_RUN) {
         /* Set upper pixel limit to current bin minus duration length */ 
         img_mxv = i - LONG_LOW_BIN_RUN;
         /* Stop searching forward */
         break;
      }
   }

   /* 2. Find lower pixel limit on tail of intensity distribution */
   /* Set in low-bin run flag to FALSE */
   inrun = 0;
   /* Set lower pixel limit to black intesity */
   img_mnv = 0;
   /* Set duration count to 0 */
   cnt = 0;
   /* Foreach bin in historam starting at the mean intensity and */
   /* searching backward ... */
   for(i = img_mn; i >= 0; i--) {
      /* If pixel intensity bin count is too low ... */
      if(bins[i] < BIN_TOO_LOW) {
         /* Then may be in lower tail of histogram distribution */
         /* So, if previously not in a low-bin run ... */ 
         if(!inrun) {
            /* Set low-bin run flag to TRUE */
            inrun = 1;
            /* Init duration count to 0 */
            cnt = 0;
         }
         /* Either way, we are in a low-bin run, so bump */
         /* duration count */
         cnt++;
      }
      /* Otherwise, we are not in a low-bin run */
      else
         /* Set low-bin run flag to FALSE */
         inrun = 0;

      /* If duration count is large enough ... */
      if(cnt == LONG_LOW_BIN_RUN) {
         img_mnv = i + LONG_LOW_BIN_RUN;
         /* Stop searching forward */
         break;
      }
   }

/*
   printf("img_mnv %d (%d), img_mn %d, img_mxv %d (%d)\n", img_mnv, bins[img_mnv], img_mn, img_mxv, bins[img_mxv]);
*/

   /* If pixel limits were adjusted ... */
   if(img_mnv != 0 || img_mxv != 255) {
      /* Set pointer to start of image */
      iptr = idata;
      /* Foreach pixel in the image */
      for(i = 0; i < img_len; i++){
         /* If pixel intensity is lower than the lower pixel limit ... */
         if(*iptr < img_mnv)
            /* Then reset pixel intensity to the lower limit */
            *iptr = img_mnv;
         else{
            /* If pixel intensity is greater than upper pixel limit ... */
            if(*iptr > img_mxv)
               /* Then reset pixel intensity to the upper limit */
               *iptr = img_mxv;
         }
         /* Bump to next pixel in the image */
         iptr++;
      }

      /* Compute factor based on range between pixel intensity limits */
      factr = 255.0/(float)(img_mxv-img_mnv);

      /* Set pointer to start of image */
      iptr = idata;
      /* Foreach pixel in the image */
      for(i = 0; i < img_len; i++){
         *iptr = (unsigned char)((float)(*iptr-img_mnv) * factr);
         iptr++;
      }
   }

   /* Otherwise, input image remains untouched */

}

