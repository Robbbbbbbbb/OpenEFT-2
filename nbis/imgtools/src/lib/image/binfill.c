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

      FILE:    BINFILL.C

      AUTHORS: Stan Janet
      DATE:    11/16/1990
      UPDATED: 03/14/2005 by MDG

      Contains routines responsible for filling a specified subimage
      of a binary image bitmap.

      ROUTINES:
#cat: binary_fill_partial - uses a logical operator to copy pixels from a
#cat:                       location in one binary scanline to a location in
#cat:                       another binary scanline.
#cat: gb - gets a pixel from a binary scanline.
#cat:
#cat: sb - sets a pixel in a binary scanline.
#cat:

***********************************************************************/

/* LINTLIBRARY */

#include <stdio.h>
#include <limits.h>
#include <binops.h>
#include <util.h>

void binary_fill_partial(int op, unsigned char *src, int i, unsigned char *dst,
                         int j, int bits)
{
int n;
/*
int i_inv;
int j_inv;
int diff;
unsigned char sc;
unsigned char src_begin, src_end;
unsigned char dst_begin, dst_end;

i_inv = BITSPERBYTE - i;
j_inv = BITSPERBYTE - j;

src_begin = mask_begin_1[i];
src_end   = mask_end_1[i_inv];

dst_begin = mask_begin_1[j];
dst_end   = mask_end_1[j_inv];


if (i + bits <= BITSPERBYTE)
	sc = *src << i;
else
	sc = (*src << i) | (*(src+1) >> i_inv);
*/

for (n=0; n<bits; n++) {
	int srcbit, dstbit;

	switch (op) {

		case BINARY_COPY:
			srcbit = gb(src,i+n);
			sb(dst,j+n,srcbit);
			break;

		case BINARY_OR:
			srcbit = gb(src,i+n);
			dstbit = gb(dst,j+n);
			sb(dst,j+n,srcbit|dstbit);
			break;

		case BINARY_AND:
			srcbit = gb(src,i+n);
			dstbit = gb(dst,j+n);
			sb(dst,j+n,srcbit&dstbit);
			break;

		case BINARY_XOR:
			srcbit = gb(src,i+n);
			dstbit = gb(dst,j+n);
			sb(dst,j+n,srcbit^dstbit);
			break;

		case BINARY_INVERT:
			srcbit = gb(src,i+n);
			sb(dst,j+n,~srcbit);
			break;  

		case BINARY_ONE:
			sb(dst,j+n,1);
			break;  

		case BINARY_ZERO:
			sb(dst,j+n,0);
			break;  

		default:
			fatalerr("binary_fill_partial","bad operator",
                                 (char *)NULL);
	} /* SWITCH */
} /* FOR */

/*
diff = bits - j;
m = mask_begin_1[(j+bits) % BITSPERBYTE];
switch (op) {

  case BINARY_COPY:
	if (diff > 0) {
		*dst = (*dst & dst_begin) | ((sc >> j) & dst_end);
		dst++;
		*dst = (*dst & ~m) | ((sc << j_inv) & m);
		dst--;
	} else if (diff < 0)
		*dst = (*dst & dst_begin) | 
	else
		*dst = (*dst & dst_begin) | ((sc >> j) & dst_end);
	break;

  case BINARY_OR:
		break;
'
  case BINARY_AND:
		break;

  default:
		fatalerr("binary_fill_partial","bad opcode",(char *)NULL);
}
*/
}

int gb(unsigned char *p, int i)
{

p += i / BITSPERBYTE;
i %= BITSPERBYTE;
return (*p >> ((BITSPERBYTE - 1) - i)) & 0x1;
}

void sb(unsigned char *p, int i, int v)
{
unsigned char m;

p += i / BITSPERBYTE;
i %= BITSPERBYTE;
m = 0x1 << ((BITSPERBYTE - 1) - i);
if (v)
	*p |= m;
else
	*p &= ~m;
}
