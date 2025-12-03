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

      FILE:    ACSMAPS.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/16/2005 by MDG

      Trivial functions that map between different representations of
      activation functions.  These functions must have code added when
      a new activation function is implemented.  Contains:

      ROUTINES:
#cat: acsmaps_code_to_fn - Code char (e.g. SINUSOID) to function (in C
#cat:                      sense) (e.g. ac_sinusoid).
#cat: acsmaps_code_to_str - Code char (e.g. SINUSOID) to string (e.g.
#cat:                       "sinusoid").
#cat: acsmaps_str_to_code - String to code char.

***********************************************************************/

#include <mlp.h>

/*******************************************************************/

/* acsmaps_code_to_fn: Maps each activation function code char to the
function (in the C, not mathemetical, sense) that implements it.

Input arg:
  code: The code value of an activation function type.

Return value: The function (actually, pointer to void-returning
  function) that implements this activation function type, i.e. that
  computes the function and its derivative.
*/

void (*acsmaps_code_to_fn(char code))(float, float *, float *)
{
  char str[50];
  /*
  void ac_sinusoid(), ac_sigmoid(), ac_linear();
  */

  switch(code) {

  case SINUSOID: return ac_sinusoid; break;
  case SIGMOID:  return ac_sigmoid;  break;
  case LINEAR:   return ac_linear;   break;

  default:
    sprintf(str, "unsupported code value %d", (int)code);
    fatalerr("acsmaps_code_to_fn (acsmaps.c)", str, NULL);
    break;
  }

  /* Added by MDG on 03-16-05 */
  exit(1);
}

/*******************************************************************/

/* acsmaps_code_to_str: Maps each activation function code char to the
corresponding string.

Input arg:
  code: The code value of an activation function type.

Return value: Corresonding string.  CAUTION: return value is the
  address of a static buffer.
*/

char *acsmaps_code_to_str(char code)
{
  static char str[50];

  switch(code) {

  case SINUSOID: strcpy(str, "sinusoid"); break;
  case SIGMOID:  strcpy(str, "sigmoid");  break;
  case LINEAR:   strcpy(str, "linear");   break;

  default:
    sprintf(str, "unsupported code value %d", (int)code);
    fatalerr("acsmaps_code_to_str (acsmaps.c)", str, NULL);
    break;
  }

  return str;
}

/*******************************************************************/

/* acsmaps_str_to_code: Maps each activation function string to the
corresponding code char.

Input arg:
  str: The string representing an activation function type.

Return value: Corresonding code char; BAD_AC_CODE if str is not
  one that this routine knows about.
*/

char acsmaps_str_to_code(char str[])
{
  if(!strcmp(str, "sinusoid"))
    return SINUSOID;
  else if(!strcmp(str, "sigmoid"))
    return SIGMOID;
  else if(!strcmp(str, "linear"))
    return LINEAR;

  else
    return BAD_AC_CODE;
}

/*******************************************************************/

/* acsmaps_code_to_fn: Maps each activation function code char to the
function (in the C, not mathemetical, sense) that implements it.

Input arg:
  code: The code value of an activation function type.

Return value: The function (actually, pointer to void-returning
  function) that implements this activation function type, i.e. that
  computes the function and its derivative.
*/

void (*acsmaps_code_to_fn2(char code))(float *)
{
  char str[50];
  /*
  void ac_v_sinusoid(), ac_v_sigmoid(), ac_v_linear();
  */

  switch(code) {

  case SINUSOID: return ac_v_sinusoid; break;
  case SIGMOID:  return ac_v_sigmoid;  break;
  case LINEAR:   return ac_v_linear;   break;

  default:
    sprintf(str, "unsupported code value %d", (int)code);
    fatalerr("acsmaps_code_to_fn (acsmaps.c)", str, NULL);
    break;
  }

  /* Added by MDG on 03-16-05 */
  exit(1);
}
