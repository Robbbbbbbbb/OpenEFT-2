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

      FILE:    MLP_SING.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG

      ROUTINES:
#cat: mlp_single - Computes the error value resulting from sending the
#cat:              patterns through the MLP.  Also can compute the gradient
#cat:              of the error w.r.t. the weights, and can do various other
#cat:              computations and writings.

***********************************************************************/

/*
Input args:
  mlp_prs: parameters for the MLP network; ninps, nhids, nouts
           weights.
     ninps, nhids, nouts: Numbers of input, hidden, and output nodes.
     w: The network weights, in this order:
       1st-layer weights (nhids by ninps "matrix", row-major);
       1st-layer biases (nhids elts);
       2nd-layer weights (nouts by nhids "matrix", row-major);
       2nd-layer biases (nouts elts).
  featvecs: Feature vectors (npats by ninps "matrix", row-major).
  hyp_cl: Hypothesized class
  hyp_ac: Hypothesized activation
  acfunc_and_deriv_hids: A function that computes the activation
    function to be used on the hidden nodes, and its derivative.
    This should be a void-returning function that takes three args:
    the input value (float), the output activation function value
    (float pointer), and the output activation function derivative
    value (float pointer).
  acfunc_and_deriv_outs: Like acfunc_and_deriv_hids, but for the
    output nodes.

Output args:
*/

#include <pca.h>
#include <mlpcla.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

void mlp_single(MLP_PARAM mlp_prs, float *featvecs, char *hyp_cl, float *hyp_ac,          void (*acfunc_and_deriv_hids)(float *),
          void (*acfunc_and_deriv_outs)(float *))
{
  int ninps, nhids, nouts;
  float *w;
  static char t = 't', first = TRUE;
  int w1n, w2n;
  static int i1 = 1;
  float *w1, *b1, *w2, *b2, *hidacs_p, *hidacs_e, *outacs_p, *outacs_e;
  static float f1 = 1.;
  int mx_nhids = 0, mx_nouts = 0;
  static float *hidacs, *outacs;

  ninps = mlp_prs.ninps;
  nhids = mlp_prs.nhids;
  nouts = mlp_prs.nouts;
  w = mlp_prs.weights;

  /* Allocate work buffers, on first call and whenever they are no
  longer big enough. */
  if(first || nhids > mx_nhids) {
    if(!first)
      free(hidacs);
    mx_nhids = nhids;
    if((hidacs = (float *)malloc(nhids * sizeof(float))) ==
      (float *)NULL)
      syserr("mlp_single", "malloc", "hidacs");
  }
  if(first || nouts > mx_nouts) {
    if(!first)
      free((char *)outacs);
    mx_nouts = nouts;
    if((outacs = (float *)malloc(nouts * sizeof(float))) ==
      (float *)NULL)
      syserr("mlp_single", "malloc", "outacs");
  }
  first = FALSE;

  /* For each pattern (feature vector, and its class or target vector)
  in turn, accumulate contribution to the error, and also possibly
  accumulate various other info (as specified by the do-switches). */
  /* set up some stuff */
  w1n = nhids * ninps;
  w2n = nouts * nhids;
  b2 = (w2 = (b1 = (w1 = w) + w1n) + nhids) + w2n;

  outacs_e = outacs + nouts;
  hidacs_e = hidacs + nhids;

    /* Start hidden activations out as the 1st-layer biases, then
    add product of 1st-layer weights with feature vector. */
    memcpy(hidacs, b1, nhids * sizeof(float));
    mlp_sgemv(t, ninps, nhids, f1, w1, ninps, featvecs, i1, f1, hidacs, i1);

    /* For each hidden node, compute activation function derivative
    and store it, and also finish the hidden activation by applying
    activation function.
    [Maybe change the activation functions so each takes a switch arg
    telling it whether to compute the derivative.] */
    for(hidacs_p = hidacs; hidacs_p < hidacs_e; hidacs_p++) {
      acfunc_and_deriv_hids(hidacs_p);
    }

    /* Start output activations out as the 2nd-layer biases, then add
    product of 2nd-layer weights with hidden activations vector. */
    memcpy(outacs, b2, nouts * sizeof(float));
    mlp_sgemv(t, nhids, nouts, f1, w2, nhids, hidacs, i1, f1, outacs, i1);

    /* For each output node, compute activation function derivative
    and store it, and also finish the output activation by applying
    activation function.  [Same note as above concerning switchable-
    deriv-computing ac functions.] */
    *hyp_ac = 0.0;
    for(outacs_p = outacs; outacs_p < outacs_e; outacs_p++) {
      acfunc_and_deriv_outs(outacs_p);
      if(*outacs_p > *hyp_ac) {
         *hyp_ac = *outacs_p;
         *hyp_cl = outacs_p - outacs;
      }
    }
#ifdef GRPHCS
  /* Display the normalized activations as a bar graph. */
  grphcs_normacs(outacs, nouts, mlp_prs.cls_str);
#endif
}
