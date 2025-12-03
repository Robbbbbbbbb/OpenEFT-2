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

      FILE:    LGL_TAB.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      ROUTINES:
#cat: lgl_tbl - Given a set of legal-parm-value-strings and coresponding
#cat:           code-number-strings, makes a neat table of the strings
#cat:           and codes.
      
***********************************************************************/

/*

Input args:
  n_legal: Number of legal-value string-code pairs.
  legal_valname_p: The legal-parm-value strings, as an array of
    pointers.
  legal_valcode_str: The corresponding code-numbers, as strings stored
    in an array whose second dimension is 2.  These numbers must be in
    the range 0 through 9.

Output arg:
  table: The table of string-code pairs, as a string.  Allocated by
    caller; allocate large enough for added spaces, to the left of
    the first col and between first and second col.
*/

#include <mlp.h>

void lgl_tbl(int n_legal, char *legal_valname_p[],
             char legal_valcode_str[][2], char table[])
{
  char *op, *op_e, *ip;
  int i, j, maxlen, len;

  for(i = maxlen = 0; i < n_legal; i++)
    if((len = strlen(legal_valname_p[i])) > maxlen)
      maxlen = len;
  op = table;
  for(i = 0; i < n_legal; i++) {
    for(j = 0; j < MESSAGE_FIRSTCOL_TABLE; j++)
      *op++ = ' ';
    op_e = op + maxlen;
    for(ip = legal_valname_p[i]; *ip;)
      *op++ = *ip++;
    while(op <= op_e)
      *op++ = ' ';
    *op++ = legal_valcode_str[i][0];
    *op++ = '\n';
  }
  *op = (char)0;	      /* changed NULL to 0 - jck 2009-02-04 */
}
