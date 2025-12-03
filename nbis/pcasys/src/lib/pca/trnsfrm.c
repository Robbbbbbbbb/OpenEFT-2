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

      FILE:    TRNSFRM.C
      AUTHORS: G. T. Candela
      DATE:    1995

      ROUTINES:
#cat: trnsfrm - Applies a linear transform to the registered orientation array.

***********************************************************************/

/* Applies a linear transform to the registered orientation array.
Uses the specified number of (first) rows of the provided transform
matrix, thereby making a feature vector with that many elements.

reg_avrors_x, reg_avrors_y: (registered) orientation array (oa) to
                              be transformed
transfrm_nrows_use: how many (first) rows of transform matrix to use
tm: transform matrix
featvec: feature vector that is the output of the transformation */

#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

void trnsfrm(float **reg_avrors_x, float **reg_avrors_y, const int aw,
          const int ah, const int trnsfrm_nrows_use, float *tm, const int tw,
          const int th, float *featvec)
{
  int i, j;
  float *p, *pe, *q, a;

  if(isverbose())
    printf("  apply unequal-weight and dim.-reducing transform\n");

  if(2*aw*ah != tw)
    fatalerr("trnsfrm","# oa (2*aw*ah) != tm width (tw)", "need 2*aw*ah == tw");
  if(trnsfrm_nrows_use != th)
    fatalerr("trnsfrm","# feats (trnsfrm_nrows_use) != tm height (th)",
             "need trnsfrm_nrows_use == th");

  pe = featvec + trnsfrm_nrows_use;
  for(p = featvec, q = tm; p < pe;) {
    a = 0.0;
    for(j = 0; j < ah; j++)
      for(i = 0; i < aw; i++)
        a += *q++ * reg_avrors_x[j][i];
    for(j = 0; j < ah; j++)
      for(i = 0; i < aw; i++)
        a += *q++ * reg_avrors_y[j][i];
    *p++ = a;
  }


#ifdef GRPHCS
  grphcs_featvec(featvec, trnsfrm_nrows_use);
#endif
}
