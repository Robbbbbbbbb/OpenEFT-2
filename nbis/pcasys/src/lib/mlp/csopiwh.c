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

      FILE:    CSOPIWH.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/16/2005 by MDG

      Spec file parameter error checking.

      ROUTINES:
#cat: csopiwh - checks for setting of parms in weights-file header.

***********************************************************************/

/*
When there is
an input weights-file, these parms will get their values set from its
header, and they should not be set in the specfile.  (Only warnings.
If any of these parms are set in the specfile, those values will not
be used.) */

#include <mlp.h>

void csopiwh(PARMS *parms)
{
  char str[150];

  if(parms->purpose.ssl.set_tried) {
    sprintf(str, "purpose has been set (line %d); that value \
will not be used (the purpose will be read from the weights file).",
      parms->purpose.ssl.linenum);
    eb_cat_w(str);
  }
  if(parms->ninps.ssl.set_tried) {
    sprintf(str, "ninps has been set (line %d); that value \
will not be used (ninps will be read from the weights file).",
      parms->ninps.ssl.linenum);
    eb_cat_w(str);
  }
  if(parms->nhids.ssl.set_tried) {
    sprintf(str, "nhids has been set (line %d); that value \
will not be used (nhids will be read from the weights file).",
      parms->nhids.ssl.linenum);
    eb_cat_w(str);
  }
  if(parms->nouts.ssl.set_tried) {
    sprintf(str, "nouts has been set (line %d); that value \
will not be used (nouts will be read from the weights file).",
      parms->nouts.ssl.linenum);
    eb_cat_w(str);
  }
  if(parms->acfunc_hids.ssl.set_tried) {
    sprintf(str, "acfunc_hids has been set (line %d); that value \
will not be used (acfunc_hids will be read from the weights file).",
      parms->acfunc_hids.ssl.linenum);
    eb_cat_w(str);
  }
  if(parms->acfunc_outs.ssl.set_tried) {
    sprintf(str, "acfunc_outs has been set (line %d); that value \
will not be used (acfunc_outs will be read from the weights file).",
      parms->acfunc_outs.ssl.linenum);
    eb_cat_w(str);
  }
}
