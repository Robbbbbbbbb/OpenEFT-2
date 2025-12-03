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

      FILE:    E_AND_G.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/16/2005 by MDG

      ROUTINES:
#cat: e_and_g - Computes the error value resulting from sending the
#cat:           patterns through the MLP.  Also can compute the gradient
#cat:           of the error w.r.t. the weights, and can do various other
#cat:           computations and writings.

***********************************************************************/

/*
The "do_"-prefixed args tell it which optional things to do.

Input args:
  do_grad: If TRUE, computes the gradient.
  doity_accum: If TRUE, uses accum_zero() and accum_cpat() to
    accumulate various weighted stuff, including (optionally, as
    controlled by do_confuse) accumulating for confusion matrices.
  do_confuse: (Used only if doity_accum is TRUE.)  This arg is passed
    along to accum_zero() and accum_cpat().  If TRUE, the accum
    routines do the work for confusion matrices, as well as for the
    minimal accumulating.
  do_long_outfile: If TRUE, makes long_outfile, which will have a line
    for each pattern showing: sequence number; actual class; whether
    right or wrong; hypothetical class; and activations.
  long_outfile: (Used only if do_long_outfile is TRUE.)  Name of long
    output file.
  show_acs_times_1000: (Used only if do_long_outfile is TRUE.)  If
    TRUE, the activations shown in the long outfile will have been
    multiplied by 1000 and then rounded to integers; if FALSE, the
    activations shown will be the original (floating-point) values.
  do_cvr: If TRUE, uses cvr_zero() and cvr_cpat() to set up data
    for later use by cvr_print(), which finishes and writes a
    correct-vs.-rejected table.
  ninps, nhids, nouts: Numbers of input, hidden, and output nodes.
  w: The network weights, in this order:
    1st-layer weights (nhids by ninps "matrix", row-major);
    1st-layer biases (nhids elts);
    2nd-layer weights (nouts by nhids "matrix", row-major);
    2nd-layer biases (nouts elts).
  npats: Number of patterns, i.e. feature-vector/class or
    feature-vector/target-vector pairs.
  featvecs: Feature vectors (npats by ninps "matrix", row-major).
  use_targvecs: If TRUE, parm targvecs below is used; if FALSE, parm
    classes below is used.  Note that if errfunc != MSE, use_targvecs
    must be FALSE.
  targvecs: (Used only if use_targvecs is TRUE.)  Target vectors
    (npats by nouts "matrix", row-major).  These must be used if the
    mlp is to be a FITTER (not CLASSIFIER).
  classes: (Used only if use_targvecs is FALSE.)  Classes of the
    patterns.  If the mlp is to be a CLASSIFIER (not FITTER), it is
    better to use classes rather than target vectors, to save memory.
  acfunc_and_deriv_hids: A function that computes the activation
    function to be used on the hidden nodes, and its derivative.
    This should be a void-returning function that takes three args:
    the input value (float), the output activation function value
    (float pointer), and the output activation function derivative
    value (float pointer).
  acfunc_and_deriv_outs: Like acfunc_and_deriv_hids, but for the
    output nodes.
  errfunc: Type of function used to compute the error contribution of
    a pattern from the activations vector and either the target vector
    or the actual class.  Must be one of the following (defined in
    parms.h):
    MSE: mean-squared error between activations and targets, or its
      equivalent using actual class instead of targets.
    TYPE_1: error function of type 1, using parm alpha (below).  (If
      this is used, use_targvecs must be FALSE.)
    POS_SUM: positive sum.  (If this is used, use_targvecs must be
      FALSE.)
  alpha: (Used only if errfunc is TYPE_1.)  A parm of the TYPE_1 error
    function: see comment of ef_t1_c() in ef.c.
  patwts: Pattern-weights.
  regfac: Regularization factor.  The regularization term of the error
    is this factor times half the average of the squares of the
    network weights.
  oklvl: Threshold for rejection.

Output args:
  err: Error.
  g: If do_grad is TRUE, then this buffer must be provided by caller
    and it will come back containing the gradient of the error w.r.t.
    the weights.  If do_grad is FALSE, then this is not filled in and
    it does not have to be the address of any memory.
  e1: Main part of error.
  e2: Mean squared weight.

Side effects:
  If doity_accum is TRUE, uses accum_zero() and accum_cpat() to set up
    data for later use by accum_print().
  If do_long_outfile is TRUE, makes a long outfile.
  If do_cvr is TRUE, uses cvr_zero() and cvr_cpat() to set up data for
    later use by cvr_print().
*/

#include <mlp.h>
#include <mlpcla.h>
/*
void e_and_g(do_grad, doity_accum, do_confuse, do_long_outfile,
  long_outfile, show_acs_times_1000, do_cvr, ninps, nhids, nouts, w,
  npats, featvecs, use_targvecs, targvecs, classes,
  acfunc_and_deriv_hids, acfunc_and_deriv_outs, errfunc, alpha,
  patwts, regfac, oklvl, err, g, e1, e2)
*/

void e_and_g(char do_grad, char doity_accum, char do_confuse,
             char do_long_outfile, char long_outfile[],
             char  show_acs_times_1000, char do_cvr, int ninps,
             int nhids, int nouts, float *w, int npats, float *featvecs,
             char use_targvecs, float *targvecs, short *classes,
             void (*acfunc_and_deriv_hids)(float, float *, float *),
             void (*acfunc_and_deriv_outs)(float, float *, float *),
             char errfunc, float alpha, float *patwts, float regfac,
             float oklvl, float *err, float *g, float *e1, float *e2)
{
  FILE *fp_long_outfile = (FILE *)NULL;
  static char first = TRUE, t = 't';
  short *cp = (short *)NULL, class, hyp_class;
  int i, w1n, w2n, numwts;
  static int i1 = 1, nhids_maxsofar, nouts_maxsofar;
  float *w1, *b1, *w2, *b2, *w1g, *b1g, *w2g, *b2g, a, b, err_acc,
    *fv, *fv_e, *tv, *hidacs_p, *hidyow_e, *hidyow_p, *outacs_p,
    *outacs_e, *maxac_p, maxac, ac, *b2g_p, *w2g_p, *b1g_p, *w2_p,
    *w1g_p, fac1, fac2, af, *hidbarf_p, ec, *ec_grad_p,
    *ec_grad_e, *af_derivs_p, wsq, *patwts_p, confidence;
  static float f1 = 1., *hidacs, *hidyow, *hidbarf, *outacs, *ec_grad,
    *af_derivs;
  int seed_fake = 0;

  cp = (short *)NULL;
  class = hyp_class = 0;
  w1g = b1g = w2g = b2g = tv = maxac_p = (float *)NULL;

  if(errfunc != MSE && use_targvecs)
    fatalerr("e_and_g", "Must not have errfunc != MSE and \
use_targvecs", NULL);

  if(do_long_outfile) {
    /* [For now, use fake "seed" value of zero here; later, figure
    out what really should go in the header of a long outfile.] */
    seed_fake = 0;
    if((fp_long_outfile = fopen(long_outfile, "wb")) == (FILE *)NULL)
      syserr("e_and_g", "fopen for writing failed", long_outfile);
    fprintf(fp_long_outfile, "%d  %d  %d  %d  %d  %.3f\n", npats,
      ninps, nhids, nouts, seed_fake, oklvl);
    for(i = 1; i <= nouts; i++)
      fprintf(fp_long_outfile, " %2d", i);
    fprintf(fp_long_outfile, "\n");
  }

  if(do_cvr)
    cvr_zero();
  if(doity_accum)
    accum_zero(do_confuse);

  /* Allocate work buffers, on first call and whenever they are no
  longer big enough. */
  if(first || nhids > nhids_maxsofar) {
    if(!first) {
      free(hidacs);
      free(hidyow);
      free(hidbarf);
    }
    nhids_maxsofar = nhids;
    if((hidacs = (float *)malloc(nhids * sizeof(float))) ==
      (float *)NULL)
      syserr("e_and_g", "malloc", "hidacs");
    if((hidyow = (float *)malloc(nhids * sizeof(float))) ==
      (float *)NULL)
      syserr("e_and_g", "malloc", "hidyow");
    if((hidbarf = (float *)malloc(nhids * sizeof(float))) ==
      (float *)NULL)
      syserr("e_and_g", "malloc", "hidbarf");
  }
  if(first || nouts > nouts_maxsofar) {
    if(!first) {
      free((char *)outacs);
      free((char *)ec_grad);
      free((char *)af_derivs);
    }
    nouts_maxsofar = nouts;
    if((outacs = (float *)malloc(nouts * sizeof(float))) ==
      (float *)NULL)
      syserr("e_and_g", "malloc", "outacs");
    if((ec_grad = (float *)malloc(nouts * sizeof(float))) ==
      (float *)NULL)
      syserr("e_and_g", "malloc", "ec_grad");
    if((af_derivs = (float *)malloc(nouts * sizeof(float))) ==
      (float *)NULL)
      syserr("e_and_g", "malloc", "af_derivs");
  }
  first = FALSE;

  /* For each pattern (feature vector, and its class or target vector)
  in turn, accumulate contribution to the error, and also possibly
  accumulate various other info (as specified by the do-switches). */
  /* (set up some stuff for patterns loop:) */
  numwts = (w1n = nhids * ninps) + nhids + (w2n = nouts * nhids)
    + nouts;
  b2 = (w2 = (b1 = (w1 = w) + w1n) + nhids) + w2n;
  err_acc = 0.;
  if(use_targvecs)
    tv = targvecs;
  else
    cp = classes;
  outacs_e = outacs + nouts;
  ec_grad_e = ec_grad + nouts;
  hidyow_e = hidyow + nhids;
  if(do_grad) {
    b2g = (w2g = (b1g = (w1g = g) + w1n) + nhids) + w2n;
    memset(g, 0, numwts * sizeof(float));
  }

  /* (patterns loop:) */
  for(fv_e = (fv = featvecs) + npats * ninps, patwts_p = patwts;
    fv < fv_e; fv += ninps, patwts_p++) {

    if(!use_targvecs)
      class = *cp;

    /* Start hidden activations out as the 1st-layer biases, then
    add product of 1st-layer weights with feature vector. */
    memcpy(hidacs, b1, nhids * sizeof(float));
    mlp_sgemv(t, ninps, nhids, f1, w1, ninps, fv, i1, f1, hidacs, i1);

    /* For each hidden node, compute activation function derivative
    and store it, and also finish the hidden activation by applying
    activation function.
    [Maybe change the activation functions so each takes a switch arg
    telling it whether to compute the derivative.] */
    for(hidyow_p = hidyow, hidacs_p = hidacs; hidyow_p < hidyow_e;
      hidyow_p++) {
      acfunc_and_deriv_hids(*hidacs_p, &af, hidyow_p);
      *hidacs_p++ = af;
    }

    /* Start output activations out as the 2nd-layer biases, then add
    product of 2nd-layer weights with hidden activations vector. */
    memcpy(outacs, b2, nouts * sizeof(float));
    mlp_sgemv(t, nhids, nouts, f1, w2, nhids, hidacs, i1, f1, outacs, i1);

    /* For each output node, compute activation function derivative
    and store it, and also finish the output activation by applying
    activation function.  [Same note as above concerning switchable-
    deriv-computing ac functions.] */
    for(outacs_p = outacs, af_derivs_p = af_derivs;
      outacs_p < outacs_e; outacs_p++, af_derivs_p++) {
      acfunc_and_deriv_outs(*outacs_p, &af, af_derivs_p);
      *outacs_p = af;
    }

    if(do_long_outfile || do_cvr) {
      /* Finish computing hypothetical class for current pattern: just
      whichever class has the highest activation. */
      for(maxac = *(maxac_p = outacs_p = outacs), outacs_p++;
        outacs_p < outacs_e; outacs_p++)
	if((ac = *outacs_p) > maxac) {
	  maxac = ac;
	  maxac_p = outacs_p;
	}
      hyp_class = maxac_p - outacs;
    }      

    if(do_long_outfile) {
      /* Write to the long outfile a line for current pattern, showing:
      sequence number; class; whether right or wrong; hypothetical
      class; and the output activations.  (Show class and hyp class
      as 1-based numbers for compatibility with old files, even though
      they are internally represented as 0-based.) */
      fprintf(fp_long_outfile, "%6d = %2d %c %2d",
        patwts_p - patwts + 1, class + 1,
        (class == hyp_class ? 'R' : 'W'), hyp_class + 1);
      for(outacs_p = outacs; outacs_p < outacs_e; outacs_p++)
	if(show_acs_times_1000)
	  fprintf(fp_long_outfile, "  %4d", sround(1000. * *outacs_p));
	else
	  fprintf(fp_long_outfile, "  %e", *outacs_p);
      fprintf(fp_long_outfile, "\n");
    }

    if(doity_accum)
      accum_cpat(do_confuse, CLASSIFIER, outacs, class, (float *)NULL,
        *patwts_p);

    if(do_cvr) {
      /* Update the accumulators for correct-vs.-rejected table. */
      confidence = *maxac_p;
      cvr_cpat(confidence, class, hyp_class, *patwts_p);
    }

    /* Compute error contribution of current pattern, and its gradient
    w.r.t. the output activations, using specified error function.
    [Maybe change the ef_ functions so each takes a switch telling it
    whether to compute gradient of error contrib. w.r.t. output
    activs.] */
    switch(errfunc) {
    case MSE:
      if(use_targvecs)
	ef_mse_t(nouts, outacs, tv, &ec, ec_grad);
      else /* use classes instead of target vectors */
	ef_mse_c(nouts, outacs, class, &ec, ec_grad);
      break;
    case TYPE_1: /* (only classes allowed) */
      ef_t1_c(nouts, outacs, class, alpha, &ec, ec_grad);
      break;
    default: /* POS_SUM (only classes allowed) */
      ef_ps_c(nouts, outacs, class, &ec, ec_grad);
      break;
    }

    /* Apply pattern-weight of current pattern to its error
    contribution and accumulate that.  If computing gradient, also
    apply pattern-weight to the gradient of the error contrib. w.r.t.
    the output activations. */
    err_acc += *patwts_p * ec;
    if(do_grad)
      mlp_sscal(nouts, *patwts_p, ec_grad, i1);

    if(do_grad) {
      /* Back-propagate: compute partial derivs. of error, first
      w.r.t. 2nd-layer biases and weights, then w.r.t. 1st-layer
      biases and weights. */
      memset(hidbarf, 0, nhids * sizeof(float));
      for(ec_grad_p = ec_grad, af_derivs_p = af_derivs, b2g_p = b2g,
        w2g_p = w2g, w2_p = w2; ec_grad_p < ec_grad_e; ec_grad_p++,
        af_derivs_p++, b2g_p++, w2g_p += nhids, w2_p += nhids) {
	*b2g_p += (a = *ec_grad_p * *af_derivs_p);
	mlp_saxpy(nhids, a, hidacs, i1, w2g_p, i1);
	mlp_saxpy(nhids, a, w2_p, i1, hidbarf, i1);
      }
      for(hidbarf_p = hidbarf, hidyow_p = hidyow, b1g_p = b1g,
        w1g_p = w1g; hidyow_p < hidyow_e; hidbarf_p++, hidyow_p++,
        b1g_p++, w1g_p += ninps) {
	*b1g_p += (b = *hidbarf_p * *hidyow_p);
	mlp_saxpy(ninps, b, fv, i1, w1g_p, i1);
      }
    }

    if(use_targvecs)
      tv += nouts;
    else
      cp++;

  } /* (patterns loop) */

  /* Done streaming all the patterns through the net and accumulating
  things; now do whatever finishing work is required. */

  /* Finish computing error: normalize w.r.t. number of outputs, and
  add regularization term.  Also set main part of error (*e1) and mean
  squared weight (*e2). */
  err_acc *= (fac1 = 1. / ((errfunc == MSE) ? (2. * nouts) : nouts));
  *e1 = ((errfunc == POS_SUM) ? err_acc :
    sqrt((double)(2. * err_acc)));
  wsq = (mlp_sdot(w1n, w1, i1, w1, i1) +
         mlp_sdot(nhids, b1, i1, b1, i1) +
         mlp_sdot(w2n, w2, i1, w2, i1) +
         mlp_sdot(nouts, b2, i1, b2, i1)) / (2. * numwts);
  *e2 = sqrt((double)(2. * wsq));
  *err = err_acc + regfac * wsq;

  if(do_grad) {
    /* Finish computing gradient: normalize w.r.t. number of outputs,
    and add the contribution of the regularization term. */
    fac2 = regfac / numwts;
    mlp_sscal(w1n, fac1, w1g, i1);
    mlp_saxpy(w1n, fac2, w1, i1, w1g, i1);
    mlp_sscal(nhids, fac1, b1g, i1);
    mlp_saxpy(nhids, fac2, b1, i1, b1g, i1);
    mlp_sscal(w2n, fac1, w2g, i1);
    mlp_saxpy(w2n, fac2, w2, i1, w2g, i1);
    mlp_sscal(nouts, fac1, b2g, i1);
    mlp_saxpy(nouts, fac2, b2, i1, b2g, i1);
  }

  if(do_long_outfile)
    fclose(fp_long_outfile);

}
