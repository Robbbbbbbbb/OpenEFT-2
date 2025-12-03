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

      FILE:    IMGAVG.C

      AUTHORS: Craig Watson
      DATE:    09/21/2004

      Contains routines to change the image size by pixel averaging.

***********************************************************************

      ROUTINES:

#cat: average_blk - scales an image a specified amount by using
#cat:               uneven block spacing to average the pixel values

***********************************************************************/

#include <stdio.h>
#include <defs.h>
#include <math.h>
#include <memalloc.h>

/*************************************************************************/
int average_blk(unsigned char *data, const int w, const int h,
                const float facx, const float facy, unsigned char **adata,
                int *aw, int *ah)
{
   int i, j, ret;
   unsigned char *dptr, *wdptr, *swdptr;
   unsigned char *aptr;
   int x, y, a, b;
   int sum, n;
   float tfacy, tfacx, fx, fy;
   int bszx, bszy;

   *ah = sround((float)h*facy);
   tfacy = 1.0/facy;

   *aw = sround((float)w*facx);
   tfacx = 1.0/facx;

   bszx = (int)ceil(tfacx);
   bszy = (int)ceil(tfacy);

   if((ret = malloc_uchar_ret(adata, *aw * *ah, "average_blk adata")))
      return(ret);

   n = bszx * bszy;
   aptr = *adata;
   for(fy = 0, j = 0; j < *ah; j++, fy += tfacy) {
      y = min(sround(fy), h-bszy);
      dptr = data + (y * w);
      for(fx = 0, i = 0; i < *aw; i++, fx += tfacx) {
         x = min(sround(fx), w-bszx);
         sum = 0;
         swdptr = dptr + x;
         for(b = 0; b < bszy; b++) {
            wdptr = swdptr;
            for(a = 0; a < bszx; a++) {
               sum += *wdptr++;
            }
            swdptr += w;
         }
         *aptr++ = sum / n;
      }
   }   
   return(0);
}
