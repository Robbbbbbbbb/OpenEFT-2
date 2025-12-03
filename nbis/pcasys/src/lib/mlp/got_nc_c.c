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

      FILE:    GOT_NC_C.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      ROUTINES:
#cat: got_nc_c - Gets the next non-comment character of a specfile, and
#cat:            also tells its line-number.
      
***********************************************************************/

/*
The comment delimiters are those of the C
language, i.e. slash asterisk and asterisk slash.

Input arg:
  fp: FILE pointer.

Output args:
  thechar: Next non-comment character found.
  linenum: Number (starting at 1) of the line that contained the
    character.

Return value:
  TRUE: Got a non-comment character.
  FALSE: Reached end-of-file without encountering another non-comment
    character.
*/

#include <mlp.h>

#define OUT        0
#define ALMOST_IN  1
#define IN         2
#define ALMOST_OUT 3

char got_nc_c(FILE *fp, char *thechar, int *linenum)
{
  static char c;
  static int state = OUT, barf = FALSE, ln, ln_aislash;

  if(barf) {
    barf = FALSE;
    *thechar = c;
    *linenum = ln;
    return TRUE;
  }
  while(1) {
    if(!got_c(fp, &c, &ln)) {
      state = OUT;
      barf = FALSE;      
      return FALSE;
    }
    if(state == OUT) {
      if(c == '/') {
	state = ALMOST_IN;
	ln_aislash = ln;
      }
      else {
	*thechar = c;
	*linenum = ln;
	return TRUE;
      }
    }
    else if(state == ALMOST_IN) {
      if(c == '*')
	state = IN;
      else {
	*thechar = '/';
	*linenum = ln_aislash;
	if(c != '/') {
	  state = OUT;
	  barf = TRUE;
	}
	return TRUE;
      }
    }
    else if(state == IN) {
      if(c == '*')
	state = ALMOST_OUT;
    }
    else { /* state == ALMOST_OUT */
      if(c == '/')
	state = OUT;
      else if(c != '*')
	state = IN;
    }
  }
}
