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

      FILE:    IMAGEOPS.C

      AUTHORS: Michael Garris
      DATE:    03/07/1990
      UPDATED: 03/15/2005 by MDG

      Contains routines responsible for general image operations.

      ROUTINES:
#cat: WordAlignImage - takes a binary image and pads out its scanlines to
#cat:                  an even word (16-bit) boundary.

***********************************************************************/
#include <stdio.h>
#include <defs.h>
#include <imgutil.h>
#include <memalloc.h>
#include <string.h>

/************************************************************/
/*         Routine:   WordAlignImage()                      */
/*         Author:    Michael D. Garris                     */
/*         Date:      03/07/90                              */
/************************************************************/
/* WordAlignImage() takes an input buffer and word aligns   */
/* the scan lines returning the new scan line pixel width   */
/* and the new byte length of the aligned image.            */
/************************************************************/
int WordAlignImage(unsigned char **adata, int *awidth, int *alength,
                   unsigned char *data, int width, int height, int depth)
{
   int i;
   int bytes_in_line, aligned_bytes_in_line, aligned_filesize;
   int aligned_pixels_in_line;
   unsigned char *inlinep, *outline;
   float pix_per_byte;

   bytes_in_line = SizeFromDepth(width,1,depth);
   aligned_pixels_in_line = WordAlignFromDepth(width,depth);
   pix_per_byte = PixPerByte(depth);
   aligned_bytes_in_line = (int)(aligned_pixels_in_line / pix_per_byte);
   if(bytes_in_line == aligned_bytes_in_line)
      return(FALSE);
   aligned_filesize = aligned_bytes_in_line * height;
   malloc_uchar(adata, aligned_filesize, "WordAlignImage : adata");
   memset((*adata), 0, aligned_filesize);
   inlinep = data;
   outline = (*adata);
   for(i = 0; i < height; i++){
      memcpy(outline,inlinep,bytes_in_line);
      outline += aligned_bytes_in_line;
      inlinep += bytes_in_line;
   }
   *awidth = aligned_pixels_in_line;
   *alength = aligned_filesize;
   return(TRUE);
}
