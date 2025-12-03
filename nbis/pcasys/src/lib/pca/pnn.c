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

      FILE:    PNN.C
      AUTHORS: G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG

      ROUTINES:
#cat: pnn - A simple implementation of Specht's Probabilistic Neural Net
#cat:       classifier.  Produces normalized activations, hypothetical
#cat:       class, and confidence.

***********************************************************************/

/* A simple implementation of Specht's Probabilistic Neural Net
classifier.  Produces normalized activations, hypothetical class, and
confidence.

(Note that the buffer of protos (prototype feature vectors) here
contains only the number of (first) protos specified in the parms, and
only the number of (first) features of each one as specified.  Only
the needed data was read.  So, this routine does not have to skip any
unused data.) */

#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

void pnn(float *featvec, PNN_PRS *pnn_prs, float *protos,
          unsigned char *proto_classes, float *normacs,
          unsigned char *hyp_class, float *confidence)
{
  unsigned char *cp, *cpe;
  static int f = 1, nclasses, nprotos_use, nfeats_use;
  float *aproto, *x, *y, *ye, a, sd, *acp, ac, acsum, maxac, *maxac_p,
    *normacp;
  static float osf, *activs, *acps, *acpe;

  if(isverbose())
    printf("  run Probabilistic Neural Net\n");
  if(f) {
    f = 0;
    nclasses = pnn_prs->nclasses;
    nprotos_use = pnn_prs->nprotos_use;
    nfeats_use = pnn_prs->nfeats_use;
    osf = pnn_prs->osf;
    activs = (float *)malloc_ch(nclasses * sizeof(float));
    acps = activs + 1;
    acpe = activs + nclasses;
  }

  /* For each class, accumulate an activation defined as the sum
  of Gaussian kernels centered at the prototype feature vectors of
  that class, all kernels being evaluated at the unknown feature
  vector (featvec).  The overall smoothing factor (osf) controls the
  size of the Gaussians and hence the amount of smoothing. */
  memset(activs, 0, nclasses * sizeof(float));
  for(sd = 0., x = protos, ye = (y = featvec) + nfeats_use; y < ye;) {
    a = *y++ - *x++;
    sd += a * a;
  }
  activs[*(cp = proto_classes)] = exp(-(double)(osf * sd));
  cpe = proto_classes + nprotos_use;
  for(cp++, aproto = protos + nfeats_use; cp < cpe; cp++,
    aproto += nfeats_use) {
    for(sd = 0., y = featvec, x = aproto; y < ye;) {
      a = *y++ - *x++;
      sd += a * a;
    }
    activs[*cp] += exp(-(double)(osf * sd));
  }

  /* Hypothesized class is defined to be the class whose activation
  is largest.  That highest activation, normalized by dividing it by
  the sum of all activations, is an estimate of the posterior
  probability of the hypothesized class and is used as the
  confidence. */
  for(acsum = maxac = *(maxac_p = activs), acp = acps; acp < acpe;
    acp++) {
    acsum += (ac = *acp);
    if(ac > maxac) {
      maxac = ac;
      maxac_p = acp;
    }
  }
  *hyp_class = maxac_p - activs;
  if(acsum > 0.) {
    *confidence = maxac / acsum;
    for(acp = activs, normacp = normacs; acp < acpe; acp++, normacp++)
      *normacp = *acp / acsum;
  }
  else {
    *confidence = 0.;
    memset(normacs, 0, nclasses * sizeof(float));
  }
#ifdef GRPHCS
  /* Display the normalized activations as a bar graph. */
  grphcs_normacs(normacs, nclasses, pnn_prs->cls_str);
#endif
}
