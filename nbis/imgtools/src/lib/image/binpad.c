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

      FILE:    BINPAD.C

      AUTHORS: Stan Janet
      DATE:    12/14/1991
      UPDATED: 03/14/2005 by MDG

      Contains routines responsible for enlarging and padding a
      binary image bitmap.

      ROUTINES:
#cat: binary_image_pad - enlarges the dimensions of a binary bitmap by
#cat:                    padding its byte-aligned width and pixel height.
#cat: binary_image_mpad - enlarges the dimensions of a binary bitmap by
#cat:                     padding accordiing to specified multiples.

***********************************************************************/

/* LINTLIBRARY */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <sys/param.h>
#include <binops.h>
#include <util.h>

#define SUBR_STR		"binary_image_pad"
#ifndef roundup
#define roundup(x, y) ((((x)+((y)-1))/(y))*(y))
#endif


/***********************************************************************/
int binary_image_pad(unsigned char **image, unsigned int w, unsigned int h,
                     unsigned int padw, unsigned int padh, int bg)
{
unsigned char *new_image;
unsigned int new_size;

if (image == (unsigned char **) NULL)
	fatalerr(SUBR_STR,"null image pointer address",(char *)NULL);
if (*image == (unsigned char *) NULL)
	fatalerr(SUBR_STR,"null image pointer",(char *)NULL);

if (padw == 0)
	fatalerr(SUBR_STR,"pad width is zero",(char *)NULL);
if (padh == 0)
	fatalerr(SUBR_STR,"pad height is zero",(char *)NULL);
if (padw % BITSPERBYTE)
	fatalerr(SUBR_STR,"pad width is not a multiple of 8",(char *)NULL);

if ((w == 0) || (h == 0))		/* no image to pad */
	return 0;

if ((w == padw) && (h == padh))		/* no growth */
	return 0;

new_size = (padw / BITSPERBYTE) * padh;

new_image = (unsigned char *) malloc(new_size);
if (new_image == (unsigned char *) NULL)
	fatalerr(SUBR_STR,"malloc failed",(char *)NULL);

(void) memset((char *)new_image, (bg ? ~0 : 0), (int)new_size);
binary_subimage_copy(*image,w,h,new_image,padw,padh,0,0,w,h,0,0);

*image = new_image;
return 1;
}




/***********************************************************************/
#undef SUBR_STR
#define SUBR_STR	"binary_image_mpad"

int binary_image_mpad(unsigned char **image, unsigned int *w, unsigned int *h,
                      unsigned int mpadw, unsigned int mpadh, int bg)
{
unsigned int new_w, new_h;
int n;

if (image == (unsigned char **) NULL)
	fatalerr(SUBR_STR,"null image pointer address",(char *)NULL);
if (*image == (unsigned char *) NULL)
	fatalerr(SUBR_STR,"null image pointer",(char *)NULL);
if (w == (unsigned int *) NULL)
	fatalerr(SUBR_STR,"null width pointer",(char *)NULL);
if (h == (unsigned int *) NULL)
	fatalerr(SUBR_STR,"null height pointer",(char *)NULL);

if (mpadw == 0)
	fatalerr(SUBR_STR,"pad width is zero",(char *)NULL);
if (mpadh == 0)
	fatalerr(SUBR_STR,"pad height is zero",(char *)NULL);
if (mpadw % BITSPERBYTE)
	fatalerr(SUBR_STR,"pad width is not a multiple of 8",(char *)NULL);

if ((*w == 0) || (*h == 0))
	return 0;

if ((*w % mpadw == 0) && (*h % mpadh == 0))
	return 0;

new_w = roundup(*w,mpadw);
new_h = roundup(*h,mpadh);
n = binary_image_pad(image,*w,*h,new_w,new_h,bg);
if (n) {
	*w = new_w;
	*h = new_h;
}
return n;
}
