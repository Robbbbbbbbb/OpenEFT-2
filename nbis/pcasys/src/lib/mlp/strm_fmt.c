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
      LIBRARY: MLP - Multi-Layer Perceptron Neural Network

      FILE:    STRM_FMT.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: strm_fmt - Used to format the first, "stream" part of a warning or
#cat:            error message.

***********************************************************************/

/*
Input arg:
  str_in: Input string.  Should consist of words separated by single
    spaces.

Output arg:
  str_out: Output string.  Allocated by caller.  (Allocate large
    enough for the insertion of spaces that implement the left
    margins.)
*/

#include <mlp.h>

void strm_fmt(char str_in[], char str_out[])
{
  char *ip, *ip_lastchar, *ip_lastcol, *ip_yow, *op,
    *op_e, firstline;
  int firstcol;

  ip_lastchar = str_in + strlen(str_in) - 1;
  for(firstline = TRUE, ip = str_in, op = str_out; ;
    firstline = FALSE) {

    firstcol = (firstline ? MESSAGE_FIRSTCOL_FIRSTLINE:
      MESSAGE_FIRSTCOL_LATERLINES);
    for(op_e = op + firstcol; op < op_e;)
      *op++ = ' ';

    ip_lastcol = ip + MESSAGE_LASTCOL - firstcol;
    if(ip_lastcol >= ip_lastchar) { /* last line of output */
      while(ip <= ip_lastchar)
	*op++ = *ip++;
      *op++ = '\n';
      *op = (char)0;	      /* changed NULL to 0 - jck 2009-02-04 */
      return;
    }

    /* Rest of input won't all fit on next line; find a suitable
    point, if one exists, at which to insert a line break. */
    if(*ip_lastcol == ' ' || *(ip_lastcol + 1) == ' ') {
      ip_yow = ((*ip_lastcol == ' ') ? ip_lastcol : ip_lastcol + 1);
      while(ip < ip_yow)
	*op++ = *ip++;
      *op++ = '\n';
      ip++;
    }
    else { /* Scan backward, stopping at first space. */
      for(ip_yow = ip_lastcol - 1; ip_yow > ip && *ip_yow != ' ';
        ip_yow--);
      if(ip_yow == ip) {
	/* There is no space at which to insert a line break, so as to
        make a sufficiently short output line.  So, just shear off
        the line. */
	while(ip <= ip_lastcol)
	  *op++ = *ip++;
	*op++ = '\n';
      }
      else { /* Found a suitable line-breaking point. */
	while(ip < ip_yow)
	  *op++ = *ip++;
	*op++ = '\n';
	ip++;
      }
    }
  }
}
