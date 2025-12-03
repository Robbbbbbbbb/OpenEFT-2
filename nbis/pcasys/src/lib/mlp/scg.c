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

      FILE:    SCG.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: scg - Uses a Scaled Conjugate Gradients (SCG) algorithm to train
#cat:       (optimize) the MLP, optionally performing Boltzmann pruning during
#cat:       training.

***********************************************************************/

/* [Check whether some of the loops in here should be replaced by
vectorish blas calls; already uses snrm2 and sdot, and maybe should
now use saxpy or other blas routines.] */

/*
Input args:
  do_confuse: If TRUE, will compute, and write to stderr and to the
    short outfile, the confusion matrices for the network at the
    end of the scg training run.
  do_long_outfile: If TRUE, will produce long_outfile at the end of
    the scg training run.
  long_outfile: (Used only if do_long_outfile is TRUE.)  Filename of
    the long outfile to be produced.
  show_acs_times_1000: (Used only if do_long_outfile is TRUE.)
    Passed to e_and_g(); see comment in e_and_g.c.
  do_cvr: If TRUE, will compute, and write to stderr and to the short
    outfile, a correct-vs.-rejected table.
  niter_max: Maximum number of training iterations allowed.
  ninps, nhids, nouts: Numbers of input, hidden, and output nodes.
  npats: Number of patterns.
  featvecs: Feature vectors of the patterns, an npats by ninps matrix.
  use_targvecs: If TRUE, parm targvecs below is used; if FALSE, parm
    classes below is used.  Note that if errfunc != MSE, use_targvecs
    must be FALSE.
  targvecs: Target vectors, an npats by nouts matrix; used if
    use_targvecs is TRUE.  These must be used if the mlp is to be a
    function FITTER (not CLASSIFIER).  (If use_targvecs is FALSE, just
    set this to (float *)NULL.)
  classes: Classes of the patterns, an array of npats shorts;
    used if use_targvecs is FALSE.  (If use_targvecs is TRUE, just set
    this to (short *)NULL.)
  acfunc_and_deriv_hids: A function that computes the activation
    function to be used on the hidden nodes, and its derivative.
    This should be a void-returning function that takes three args:
    the input value (float), the output activation function value
    (float pointer), and the output activation function derivative
    value (float pointer).
  acfunc_and_deriv_outs: Like acfunc_and_deriv_hids, but for the
    output nodes.
  errfunc: Type of function used to compute the error contribution
    of a pattern from the activations vector and either the target
    vector or the actual class.  Must be one of the following
    (defined in parms.h):
    MSE: mean-squared error between activations and targets, or its
      equivalent using actual class instead of targets.
    TYPE_1: error function of type 1, using parm alpha (below).  (If
      this is used, use_targvecs must be FALSE.)
    POS_SUM: positive sum.  (If this is used, use_targvecs must be
      FALSE.)
  alpha: A parm that must be set if errfunc is TYPE_1.  (If errfunc is
    not TYPE_1, set value 0. for this parm.)
  patwts: Pattern-weights.
  regfac: Regularization factor.  The regularization term of the error
    is this factor times half the average of the squares of the
    weights.
  pct: A threshold for pctmin, such that if pctmin >= pct then
    the routine returns (under some conditions).
  boltzmann: Decides whether to use Boltzmann pruning, and if
    so, what kind of threshold to use (see boltz.c).  Must be one of:
      NO_PRUNE: Do not prune.
      ABS_PRUNE: Prune using threshold exp(-|wt|/temperature),
        where wt is a weight being considered for pruning.
      SQUARE_PRUNE: Prune using threshold exp(-wt^t/temperature).
  temperature: For Boltzmann pruning.  (Not used if boltzmann
    is NO_PRUNE.)
  nfreq:
  egoal: If *rmserr becomes < egoal, the routine returns.
  gwgoal: If size(g) / size(w) becomes < gwgoal, the routine returns.
  oklvl: Threshold for rejection.
  purpose, long_classnames, short_classnames: Passed to accum_print.

Input/output args:
  w: The network weights, in this order:
    1st-layer weights (nhids by ninps "matrix", row-major);
    1st-layer biases (nhids elts);
    2nd-layer weights (nouts by nhids "matrix", row-major);
    2nd-layer biases (nouts elts).
  ncalls: A counter which this routine increments each time it calls
    e_and_g(), which computes the error and its gradient.

Scratch-buffer arg:
  g: For holding the gradient of the error w.r.t the network weights.
    Caller is to provide this buffer, allocated to (at least) as many
    floats as there are weights, i.e. nhids * (ninps + 1) + nouts *
    (nhids + 1) floats.  This buffer's contents upon entry to this
    routine do not matter, and its contents after return are not
    intended to be used.

Output args:
  rmserr: Error value.
  gw: Size(gradient) / size(weights).
  iter: How many iterations were used.
  ierr: Error code. [Show what the values mean; also, better yet,
    define names for the values in an scg.h.  And perhaps it would
    be better to let this integer code be the function value, instead
    of an arg.]
*/

#include <mlp.h>
#include <mlpcla.h>

void scg(char do_confuse, char do_long_outfile, char long_outfile[],
         char show_acs_times_1000, char do_cvr, int niter_max, int ninps,
         int nhids, int nouts, int npats, float *featvecs, char use_targvecs,
         float *targvecs, short *classes,
         void (*acfunc_and_deriv_hids)(float, float *, float *),
         void (*acfunc_and_deriv_outs)(float, float *, float *),
         char errfunc, float alpha, float *patwts, float regfac, float pct,
         char boltzmann, float temperature, int nfreq, float egoal,
         float gwgoal, float oklvl, char purpose, char **long_classnames,
         char **short_classnames, float *w, float *g, float *rmserr,
         float *gw, int *iter, int *ncalls, int *ierr)
{
  char str[100];
  int numwts, i, k,
    icount /* number of steps since last restart */,
    fcount /* number of consecutive failures */,
    check, success, kmin, iover, junkint0, junkint1;
  static int i1 = 1;
  float pctmin, deltak, err, enew, xl, xlb, wsiz, sigma, psiz, psq,
    sigmak, c, xmu, alphak, delta, beta, gsiz, e1, e2, junkfloat,
    *wnew, /* new weights */
    *p,    /* direction vector */
    *r,    /* remembered-g */
    *s;    /* second deriv. info along p direction */

  gsiz = 0.0;

  numwts = nhids * (ninps + 1) + nouts * (nhids + 1);

  kmin = mlp_max(NF * nfreq, NITER);
  if(boltzmann != NO_PRUNE && temperature > 0.)
    kmin = mlp_max(kmin, NBOLTZ);
  if(niter_max > 0) {
    sprintf(str, "\n SCG: doing <= %d iterations; %d variables.\n\n",
      niter_max, numwts);
    fsaso(str);
  }
  if(boltzmann != NO_PRUNE)
    boltz(ninps, nhids, nouts, boltzmann, temperature, w);

  /* Get inital error and gradient. */
  e_and_g(TRUE, TRUE, FALSE, FALSE, (char *)NULL, FALSE, FALSE, ninps,
    nhids, nouts, w, npats, featvecs, use_targvecs, targvecs, classes,
    acfunc_and_deriv_hids, acfunc_and_deriv_outs, errfunc, alpha,
    patwts, regfac, oklvl, &err, g, &e1, &e2);
  (*ncalls)++;

  wsiz = mlp_snrm2(numwts, w, i1);
  *iter = 0;
  optchk_store_e1_e2(e1, e2);
  optchk(FALSE, 0, w, err, ierr, &pctmin);
  *rmserr = sqrt((double)(err * 2.));

  if((wnew = (float *)malloc(i = numwts * sizeof(float))) ==
    (float *)NULL)
    syserr("scg", "malloc", "wnew");
  if((p = (float *)malloc(i)) == (float *)NULL)
    syserr("scg", "malloc", "p");
  if((r = (float *)malloc(i)) == (float *)NULL)
    syserr("scg", "malloc", "r");
  if((s = (float *)malloc(i)) == (float *)NULL)
    syserr("scg", "malloc", "s");

  *ierr = 1;
  sigma = 1.e-4; /* relative distance for numerical derivative */
  xl = XLSTART; /* lambda_k */
  xlb = 0.; /* lambda_k bar */
  deltak = 0.;
  success = TRUE;
  /* rmsold = *rmserr */
  for(i = 0; i < numwts; i++)
    p[i] = r[i] = -g[i];
  iover = mlp_min(numwts, NRESTART); /* how often to restart the
                                 algorithm */
  icount = 0; /* number of iterations since last restart */
  fcount = 0; /* number of failed iterations in a row */
  *iter = niter_max;
  k = 0;
  /* notimp = 0; (fortran original has this commented out) */

  while(k < niter_max) {
    icount++;
    psiz = mlp_snrm2(numwts, p, i1);
    psq = psiz * psiz;
    if(success) {
      sigmak = sigma * wsiz / psiz;
      for(i = 0; i < numwts; i++)
	wnew[i] = w[i] + sigmak * p[i];

      /* Get error and gradient 2nd derivative information. */
      e_and_g(TRUE, TRUE, FALSE, FALSE, (char *)NULL, FALSE, FALSE,
        ninps, nhids, nouts, wnew, npats, featvecs, use_targvecs,
        targvecs, classes, acfunc_and_deriv_hids,
        acfunc_and_deriv_outs, errfunc, alpha, patwts, regfac, oklvl,
        &enew, s, &e1, &e2);
      (*ncalls)++;

      /* dE/d(dist) along p is (enew-err)/sigmak. */
      for(i = 0; i < numwts; i++)
	s[i] = (s[i] - g[i]) / sigmak;
      deltak = mlp_sdot(numwts, s, i1, p, i1);
    }
    c = xl - xlb;
    if(c != 0.) {
      for(i = 0; i < numwts; i++)
	s[i] += c * p[i];
      deltak += c * psq;
    }
    /* Maybe need to make "Hessian" positive definite. */
    if(deltak <= 0) {
      c = xl - 2. * deltak / psq;
      for(i = 0; i < numwts; i++)
	s[i] += c * p[i];
      xlb = 2. * (xl - deltak / psq);
      deltak = -deltak + xl * psq;
      xl = xlb;
    }
    /* Get the right step size. */
    xmu = mlp_sdot(numwts, p, i1, r, i1);
    alphak = xmu / deltak;
    for(i = 0; i < numwts; i++)
      wnew[i] = w[i] + alphak * p[i];

    /* Get new error and gradient. */
    e_and_g(TRUE, TRUE, FALSE, FALSE, (char *)NULL, FALSE, FALSE,
      ninps, nhids, nouts, wnew, npats, featvecs, use_targvecs,
      targvecs, classes, acfunc_and_deriv_hids, acfunc_and_deriv_outs,
      errfunc, alpha, patwts, regfac, oklvl, &enew, g, &e1, &e2);
    (*ncalls)++;

    delta = 2. * deltak * (err - enew) / (xmu * xmu);
    if(delta >= 0.) {
      k++;
      fcount = 0;
      memcpy((char *)w, (char *)wnew, numwts * sizeof(float));

      if(boltzmann != NO_PRUNE) {
	boltz(ninps, nhids, nouts, boltzmann, temperature, w);
	e_and_g(TRUE, TRUE, FALSE, FALSE, (char *)NULL, FALSE, FALSE,
          ninps, nhids, nouts, w, npats, featvecs, use_targvecs,
          targvecs, classes, acfunc_and_deriv_hids,
          acfunc_and_deriv_outs, errfunc, alpha, patwts, regfac,
          oklvl, &enew, g, &e1, &e2);
	(*ncalls)++;
      }
      wsiz = mlp_snrm2(numwts, w, i1);
      err = enew;
      xlb = 0;
      success = check = TRUE;
      if(icount % iover == 0) /* restart */
	for(i = 0; i < numwts; i++)
	  p[i] = -g[i];
      else { /* find conjugate direction */
	beta = (mlp_sdot(numwts, g, i1, g, i1) +
                mlp_sdot(numwts, g, i1, r, i1)) / xmu;
	for(i = 0; i < numwts; i++)
	  p[i] = -g[i] + beta * p[i];
      }
      for(i = 0; i < numwts; i++)
	r[i] = -g[i];
      if(delta >= 0.75) /* trustworthy */
	xl /= 2; /* maybe try something else */
    }
    else {
      xlb = xl;
      success = check = FALSE;
      fcount++;
      if(fcount > 2){
	if(icount > fcount) { /* At least one good step since
                              restart. */
	  sprintf(str, " restart SCG %4d\n", k);
	  fsaso(str);
	  for(i = 0; i < numwts; i++)
	    p[i] = -g[i];
	  xl = XLSTART;
	  xlb = 0.;
	  success = TRUE;
	  delta = 1.;
	  icount = fcount = 0;
	}
	else {
	  *ierr = 3;
	  *iter = k;
	  break;
	}
      }
    }
    /* If not nearly as good as predicted, increase xl. */
    if(delta < 0.25)
      xl *= 4.;
    /* maybe try xl = xl + deltak * (1 - delta) / psq */
    *rmserr = sqrt((double)(err * 2.));
    gsiz = mlp_snrm2(numwts, r, i1);
    if(check) {
      optchk_store_e1_e2(e1, e2);
      optchk(FALSE, k, w, err, ierr, &pctmin);
      if(*ierr != 0 || pctmin >= pct) {
	*iter = k;
	break;
      }
    }
    /* Terminate when error satisfactory. */
    if(*rmserr < egoal) {
      *ierr = 0;
      *iter = k;
      break;
    }
    /* Terminate when gradient is too small. */
    if(gsiz < gwgoal * mlp_max(1., wsiz)) {
      *ierr = 2;
      *iter = k;
      break;
    }
  } /* while(k < niter_max) */

  if(k >= niter_max)
    *ierr = 1;
  *gw = gsiz / wsiz;

  /* Do another call of this with the (same) final weights, to
  accumulate at least the minimal counting information, which is
  always needed; and the switches also may activate optional
  computations: confusion matrices, long outfile, and
  correct-vs.-rejected table. */
  e_and_g(FALSE, TRUE, do_confuse, do_long_outfile, long_outfile,
    show_acs_times_1000, do_cvr, ninps, nhids, nouts, w, npats,
    featvecs, use_targvecs, targvecs, classes, acfunc_and_deriv_hids,
    acfunc_and_deriv_outs, errfunc, alpha, patwts, regfac, oklvl,
    &err, (float *)NULL, &e1, &e2);

  /* Finishes and writes the minimal counting info, and if desired
  also the confusion matrices. */
  accum_print(do_confuse, purpose, npats, *iter, err, e1, e2, 'F', w,
    long_classnames, short_classnames, &junkint0, &junkint1,
    &junkfloat);

  if(do_cvr)
    /* Finishes and writes correct-vs.-rejected table. */
    cvr_print(TRAIN, npats);

  free((char *)wnew);
  free((char *)p);
  free((char *)r);
  free((char *)s);
}
