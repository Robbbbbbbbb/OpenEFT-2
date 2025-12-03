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
      LIBRARY: PCASYS - Pattern Classification System

      FILE:    COMBINE.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG

      ROUTINES:
#cat: combine - Combines the outputs of the Neural Network (NN) and
#cat:           the pseudoridge tracer (whorl detector).

***********************************************************************/

/* Combines the outputs of the Neural Network (NN) and
the pseudoridge tracer (whorl detector), so as to produce the final
output of the classifier, namely the hypothetical class and the
confidence. */

#include <pca.h>

void combine(const unsigned char nn_hyp_class, const float nn_conf,
          const int found_conup, const float clash_conf,
          unsigned char *hyp_class, float *conf, char *cls_str)
{
  unsigned char whorl_ind;

  /* If a concave-upward shape was found by the pseudoridge tracer,
  then set hypothetical class to whorl, with high confidence (1.) if
  main (NN) classifier also thought the print was a whorl, and with
  lower confidence otherwise.  If no concave-upward shape was found,
  then let the NN's hypothetical class and confidence stand. */
  if(isverbose())
    printf("  combine outputs of NN and pseudoridge-tracer\n");
  if(found_conup) {
    if(strchr(cls_str, 'W') == '\0') {
       *hyp_class = nn_hyp_class;
       *conf = nn_conf;
    }
    else {
      whorl_ind = strchr(cls_str, 'W') - cls_str;
      *hyp_class = whorl_ind;
      *conf = ((nn_hyp_class == whorl_ind) ? 1. : clash_conf);
    }
  }
  else {
    *hyp_class = nn_hyp_class;
    *conf = nn_conf;
  }
}
