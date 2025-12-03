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

      FILE:    DILATE.C

      AUTHORS: Michael Garris
      DATE:    09/20/2004
      UPDATED: 03/14/2005 by MDG

      Contains routines to dilate a char image.

***********************************************************************

      ROUTINES:

#cat: dilate_charimage - set false pixel to one if any of 4 neighbors is one
#cat:                    in a character image.
#cat: get_south8 - return value of char image pixel 1 below of current pixel
#cat:              if defined else return (char)0
#cat: get_north8 - return value of char image pixel 1 above of current pixel
#cat:              if defined else return (char)0
#cat: get_east8  - return value of char image pixel 1 right of current pixel
#cat:              if defined else return (char)0
#cat: get_west8  - return value of char image pixel 1 left  of current pixel
#cat:              if defined else return (char)0

***********************************************************************/

#include <memory.h>
#include <memalloc.h>
#include <dilate.h>

/******************************************************************/
/* dilate a one bit per byte char image, inp. Result is out which  */
/* must be disjoint with inp. The data in out before the call is  */
/* irrelevant, and is zeroed and filled by this routine. iw and   */
/* ih are the width and height of the image in pixels. Both inp   */
/* and out point to iw*ih bytes                                   */
/******************************************************************/
 
int dilate_charimage(unsigned char *inp, unsigned char **out, const int iw,
                 const int ih)
{
   int row, col, ret;
   unsigned char *itr, *otr;

   if((ret = malloc_uchar_ret(out, iw*ih, "dilate_charimage out")))
      return(ret);
   itr = inp;
   otr = *out;
 
   memcpy(otr, inp, iw*ih);
 
   /* for all pixels. set pixel if there is at least one true neighbor */
   for ( row = 0 ; row < ih ; row++ )
      for ( col = 0 ; col < iw ; col++ )
      {  
         if (!*itr)     /* pixel is already true, neighbors irrelevant */
         {
            /* more efficient with C's left to right evaluation of     */
            /* conjuctions. E N S functions not executed if W is false */
            if (get_west8 ((char *)itr, col        ) ||
                get_east8 ((char *)itr, col, iw    ) ||
                get_north8((char *)itr, row, iw    ) ||
                get_south8((char *)itr, row, iw, ih))
               *otr = 1;
         }
         itr++ ; otr++;
      }  

   return(0);
}

/************************************************************************/
/* routines for accessing individual neighbors of pixel at (row,col) 	*/
/* row and pixel are zero oriented. That is 0 <= row < ih and           */
/* 0 <= col < iw. ptr points to the (row,col) element of the ih by iw 	*/
/* sized char image which contains as many >bytes< as there are pixels  */
/* in the image. 							*/
/************************************************************************/

char get_south8(char *ptr, const int row, const int iw, const int ih)
{
   if (row >= ih-1) /* catch case where image is undefined southwards   */
      return 0;     /* use plane geometry and return false.             */

      return *(ptr+iw);
}

char get_north8(char *ptr, const int row, const int iw)
{
   if (row < 1)     /* catch case where image is undefined northwards   */
      return 0;     /* use plane geometry and return false.             */

      return *(ptr-iw);
}

char get_east8(char *ptr, const int col, const int iw)
{
   if (col >= iw-1) /* catch case where image is undefined eastwards    */
      return 0;     /* use plane geometry and return false.             */

      return *(ptr+ 1);
}

char get_west8(char *ptr, const int col)
{
   if (col < 1)     /* catch case where image is undefined westwards     */
      return 0;     /* use plane geometry and return false.              */

      return *(ptr- 1);
}
