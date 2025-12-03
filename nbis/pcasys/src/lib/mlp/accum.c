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

      FILE:    ACCUM.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/16/2005 by MDG

      Routines for doing various weighted accumulations
      (they replace the original "confuse" routine).

      NOTE: This file has several static variables whose scope is the file,
      and they are used by the routines (i.e., side effects) without
      comments.

      ROUTINES:
#cat: accum_init - Initialization routine.
#cat: accum_zero - Zeros the accumulators.
#cat: accum_cpat - Accumulates for the current pattern.
#cat: accum_print - Prints some results.
#cat: accum_free - Frees used local buffers.
#cat: (accum_printer - Used by accum_print.)
#cat: (accum_sumout - Used by accum_print.)
#cat: (accum_yow - Used by accum_sumout.)

***********************************************************************/

#include <mlp.h>

#define ERRS_NERRS_DIM 11 /* The histogram of errors is made in the
  range 2^(-(ERRS_NERRS_DIM-1)) to 1 */

/* If following line is left in force, accum_printer will include
in its output a table showing, for each class, the "key" (short name)
and the "name" (long name); if commented out, no table. */
#define KEYS_NAMES_TABLE

static int nouts_loc, nerrs[ERRS_NERRS_DIM], *npats_bc, *iwtd_rpct_bc;
static float errs[ERRS_NERRS_DIM], *r_acc_bc, *w_acc_bc, *rej_acc_bc,
  oklvl_loc, *outrej, sum1, sum2;
static TDA_FLOAT confuse_acc, outlvl;

/*******************************************************************/

/* accum_init: Mallocs local (to this file) buffers, and stores a
local copy of oklvl.  Call this at start of each run.

Input args:
  nouts: Number of output nodes.
  do_confuse: If TRUE, mallocs some extra buffers.
  oklvl: Threshold for rejection.
*/

void accum_init(int nouts, char do_confuse, float oklvl)
{
  if((r_acc_bc = (float *)malloc(nouts * sizeof(float))) ==
    (float *)NULL)
    syserr("accum_init (accum.c)", "malloc", "r_acc_bc");
  if((w_acc_bc = (float *)malloc(nouts * sizeof(float))) ==
    (float *)NULL)
    syserr("accum_init (accum.c)", "malloc", "w_acc_bc");
  if((rej_acc_bc = (float *)malloc(nouts * sizeof(float))) ==
    (float *)NULL)
    syserr("accum_init (accum.c)", "malloc", "rej_acc_bc");
  if((iwtd_rpct_bc = (int *)malloc(nouts * sizeof(int))) ==
    (int *)NULL)
    syserr("accum_init (accum.c)", "malloc", "iwtd_pct_bc");
  if((outrej = (float *)malloc(nouts * sizeof(float))) ==
    (float *)NULL)
    syserr("accum_init (accum.c)", "malloc", "outrej");
  oklvl_loc = oklvl;
  if(do_confuse) {
    confuse_acc.dim2 = nouts;
    if((confuse_acc.buf = (float *)malloc(nouts * nouts *
      sizeof(float))) == (float *)NULL)
      syserr("accum_init (accum.c)", "malloc", "confuse_acc.buf");
    outlvl.dim2 = nouts;
    if((outlvl.buf = (float *)malloc(nouts * nouts * sizeof(float)))
      == (float *)NULL)
      syserr("accum_init (accum.c)", "malloc", "outlvl.buf");
    if((npats_bc = (int *)malloc(nouts * sizeof(float))) ==
      (int *)NULL)
      syserr("accum_init (accum.c)", "malloc", "npats_bc");
  }
  else {
    confuse_acc.buf = outlvl.buf = (float *)NULL;
    npats_bc = (int *)NULL;
  }
  nouts_loc = nouts;
}

/*******************************************************************/

/* accum_zero: Zeros out the accumulators, and initializes the errs
and nerrs arrays.  Call this from e_and_g before any calls of
accum_cpat.

Input arg:
  do_confuse: If TRUE, zeros out the confusion accumumlators, as
    well as the basic accumulators that it always zeros out.
*/

void accum_zero(char do_confuse)
{
  int i;

  memset((char *)r_acc_bc, 0, nouts_loc * sizeof(float));
  memset((char *)w_acc_bc, 0, nouts_loc * sizeof(float));
  memset((char *)rej_acc_bc, 0, nouts_loc * sizeof(float));
  memset((char *)outrej, 0, nouts_loc * sizeof(float));

  if(do_confuse)
    memset((char *)confuse_acc.buf, 0, nouts_loc * nouts_loc *
      sizeof(float));

  if(do_confuse) {
    memset((char *)outlvl.buf, 0, nouts_loc * nouts_loc * sizeof(float));
    memset((char *)npats_bc, 0, nouts_loc * sizeof(int));
  }
  sum1 = sum2 = 0.;
  for(i = 0; i < ERRS_NERRS_DIM; i++) {
    errs[i] = pow((double)2, (double)(i - ERRS_NERRS_DIM + 1));
    nerrs[i] = 0;
  }
}

/*******************************************************************/

/* accum_cpat: Updates the accumulators according to the current
pattern.

Input args:
  do_confuse: If TRUE, accumulate into the confusion accumulators.
  purpose: CLASSIFIER or FITTER.
  vout_cpat: output activations vector for the current pattern.
  idpat_cpat: Id (class) of the current pattern, if purpose is
    CLASSIFIER.
  target_cpat: Target vector of current pattern, if purpose is FITTER.
  patwt_cpat: Pattern-weight of the current pattern.
*/

void accum_cpat(char do_confuse, char purpose, float *vout_cpat,
                short idpat_cpat, float *target_cpat, float patwt_cpat)
{
  short idres_cpat;
  char okay;
  int ibig1, ibig2, j, jj, k;
  float big1, big2, ee;

  /* Find biggest two activation levels. */
  ibig1 = ((vout_cpat[0] >= vout_cpat[1]) ? 0 : 1);
  ibig2 = 1 - ibig1;
  big1 = vout_cpat[ibig1];
  big2 = vout_cpat[ibig2];
  for(j = 2; j < nouts_loc; j++) {
    if(vout_cpat[j] > big1) {
      big2 = big1;
      ibig2 = ibig1;
      big1 = vout_cpat[j];
      ibig1 = j;
    }
    else if(vout_cpat[j] > big2) {
      big2 = vout_cpat[j];
      ibig2 = j;
    }
  }

  okay = (idpat_cpat < 0 ? FALSE : big1 > oklvl_loc);
  if(okay) {
    if(idpat_cpat == ibig1)
      r_acc_bc[idpat_cpat] += patwt_cpat;
    else
      w_acc_bc[idpat_cpat] += patwt_cpat;

    if(do_confuse)
      e(confuse_acc, ibig1, idpat_cpat) += patwt_cpat;

    if(do_confuse)
      e(outlvl, ibig1, idpat_cpat) += big1;

    idres_cpat = ibig1;
  }
  else {
    if(idpat_cpat >= 0) {
      rej_acc_bc[idpat_cpat] += patwt_cpat;
      outrej[idpat_cpat] += big1;
    }
    idres_cpat = ibig1 - nouts_loc; /* makes value betw. -nouts_loc
                                    and -1 */
  }
  sum1 += big1;
  sum2 += big2;
  if(do_confuse) { /* Count nos. of patterns by class; (unrelatedly)
                   accumulate histogram of output errors */
    npats_bc[idpat_cpat]++;
    for(j = 0; j < nouts_loc; j++) {
      ee = fabs((double)(vout_cpat[j] -
        (purpose == CLASSIFIER ?
          (j == idpat_cpat ? 1. : 0.) :
          target_cpat[j])));
      jj = ERRS_NERRS_DIM - 1;
      for(k = 0; k < ERRS_NERRS_DIM - 1; k++)
	if(ee <= errs[k]) {
	  jj = k;
	  break;
	}
      nerrs[jj]++;
    }
  }
}

/*******************************************************************/

/* accum_print: Prints info from the finished counters, and
optionally also prints the confusion matrices; and, returns some
info.

Input args:
  do_confuse: If TRUE, print the confusion information.
  purpose: CLASSIFIER or FITTER.
  npats: Number of patterns.
  iter: Current iteration of the optimization.
  err: Error, including regularization term.
  e1: Main part of error, basically.
  e2: Mean squared weight.
  c: Passed to accum_sumout().
  w: Passed to accum_sumout(): weights.
  long_classnames: Passed to accum_printer(): long names of the
    classes.
  short_classnames: Passed to accum_printer(): short names of the
    classes.

Output args:
  wtd_nr: Weighted "number right".
  wtd_nw: Weighted "number wrong".
  wtd_rpct_min: Minimum, across classes, of weighted
    "right-percentage by class".
*/

void accum_print(char do_confuse, char purpose, int npats, int iter,
                 float err, float e1, float e2, char c, float w[],
                 char **long_classnames, char **short_classnames,
                 int *wtd_nr, int *wtd_nw, float *wtd_rpct_min)
{
  int i, j, ntotal, *wtd_nrej_bc;
  float a;
  TDA_INT confuse_wtd_counts;

  if(do_confuse) {
    sum1 /= (float)npats;
    sum2 /= (float)npats;
    if((wtd_nrej_bc = (int *)malloc(nouts_loc * sizeof(int))) ==
      (int *)NULL)
      syserr("accum_print (accum.c)", "malloc", "wtd_nrej_bc");
    confuse_wtd_counts.dim2 = nouts_loc;
    if((confuse_wtd_counts.buf = (int *)malloc(nouts_loc * nouts_loc *
      sizeof(int))) == (int *)NULL)
      syserr("accum_print (accum.c)", "malloc",
        "confuse_wtd_counts.buf");
    for(i = 0; i < nouts_loc; i++) {
      for(j = 0, a = rej_acc_bc[i]; j < nouts_loc; j++)
	a += e(confuse_acc, j, i);
      if(a > 0.)
	a = npats_bc[i] / a;
      for(j = 0; j < nouts_loc; j++)
	e(confuse_wtd_counts, j, i) = sround(a * e(confuse_acc, j, i));
      if((r_acc_bc[i] + w_acc_bc[i] + rej_acc_bc[i]) > 0.0)
        wtd_nrej_bc[i] = sround((float)npats_bc[i] * rej_acc_bc[i] /
          (r_acc_bc[i] + w_acc_bc[i] + rej_acc_bc[i]));
      else
        wtd_nrej_bc[i] = 0.0;
      outrej[i] *= (100. / (float)mlp_max(1, wtd_nrej_bc[i]));
      for(j = 0; j < nouts_loc; j++)
	e(outlvl, j, i) *= (100. / (float)mlp_max(1, e(confuse_wtd_counts,
          j, i)));
    }
    ntotal = nouts_loc * npats;
    accum_printer(long_classnames, short_classnames, ntotal, *wtd_nw,
      wtd_nrej_bc, &confuse_wtd_counts);
    free((char *)wtd_nrej_bc);
    free((char *)(confuse_wtd_counts.buf));
  }
  accum_sumout(purpose, npats, iter, c, w,
    (float)sqrt((double)(err * 2.)), e1, e2, wtd_nr, wtd_nw,
    wtd_rpct_min);
}

/******************************************************************/

/* accum_free: Frees the local (to this file) buffers.  Call this at
end of each run. */

void accum_free()
{
  free((char *)r_acc_bc);
  free((char *)w_acc_bc);
  free((char *)rej_acc_bc);
  free((char *)iwtd_rpct_bc);
  free((char *)outrej);
  free_notnull((char *)confuse_acc.buf);
  free_notnull((char *)outlvl.buf);
  free_notnull((char *)npats_bc);
}

/*******************************************************************/

/* accum_printer: Used by accum_print() to nicely format some
info and write it to stderr and to the short outfile.

Input args:
  long_classnames: Long names of the classes.
  short_classnames: Short names of the classes.
  ntotal: Number of outputs times number of patterns.
  wtd_nw: Weighted "number wrong".
  wtd_nrej_bc: Weighted "number rejected by class".
  confuse_wtd_counts: Confusion matrix of weighted "counts".
*/

void accum_printer(char **long_classnames, char **short_classnames,
                   int ntotal, int wtd_nw, int *wtd_nrej_bc,
                   TDA_INT *confuse_wtd_counts)
{
  char str[200];
  int j, k, isum;

  sprintf(str, "\n oklvl %.2f\n # Highest two outputs (mean) %.3f \
%.3f; mean diff %.3f\n", oklvl_loc, sum1, sum2, sum1 - sum2);
  fsaso(str);
#ifdef KEYS_NAMES_TABLE
  fsaso("   key  name\n");
  for(j = 0; j < nouts_loc; j++) {
    sprintf(str, "   %s   %s\n", short_classnames[j],
      long_classnames[j]);
    fsaso(str);
  }
#endif
  fsaso(" #  key:   ");
  for(j = 0; j < nouts_loc; j++) {
    sprintf(str, "  %s", short_classnames[j]);
    fsaso(str);
  }
  fsaso("\n");
  fsaso(" #  row: correct, column: actual\n");
  for(j = 0; j < nouts_loc; j++) {
    sprintf(str, " #      %s:", short_classnames[j]);
    fsaso(str);
    for(k = 0; k < nouts_loc; k++) {
      sprintf(str, " %3d", e(*confuse_wtd_counts, k, j));
      fsaso(str);
    }
    fsaso("\n");
  }
  fsaso(" #  unknown\n #    * ");
  for(j = 0; j < nouts_loc; j++) {
    sprintf(str, " %3d", wtd_nrej_bc[j]);
    fsaso(str);
  }
  fsaso("\n");

  /* row sums: percent of each class correctly identified */
  fsaso("\n percent of true IDs correctly identified (rows)\n\
        ");
  for(j = 0; j < nouts_loc; j++) {
    /* The following line computes percentages which, if priors is
    allsame, should come out the same as (or very close to) the
    percentages produced by the non-weighting version, i.e. in which
    the "counts" used as the basis for the calculations really are
    just counts; but, it may be that it would make more sense _not_
    to subtract wtd_nrej_bc[j] from npats_bc[j]. */
    sprintf(str, " %3d", sround(100. * (float)e(*confuse_wtd_counts, j,
      j) / (float)mlp_max(1, npats_bc[j] - wtd_nrej_bc[j])));
    fsaso(str);
  }
  fsaso("\n");

  /* [Do the "column sum" numbers as computed here make sense for the
  _weighted_ version?  Because of the particular weighted method used
  to compute confuse_wtd_counts, the sum of its i'th column might
  _not_ be the number of examples whose hyp class is i.] */
  /* column sums: percent of each predicted class correctly
  identified */
  fsaso("percent of predicted IDs correctly identified (cols)\n\
        ");
  for(k = 0; k < nouts_loc; k++) {
    for(j = isum = 0; j < nouts_loc; j++)
      isum += e(*confuse_wtd_counts, k, j);
    sprintf(str, " %3d", sround(100. * (float)e(*confuse_wtd_counts, k,
      k) / (float)mlp_max(1, isum)));
    fsaso(str);
  }

  fsaso("\n\n #  mean highest activation level\n #  row: \
correct, column: actual\n #  key:   ");
  for(j = 0; j < nouts_loc; j++) {
    sprintf(str, "  %s", short_classnames[j]);
    fsaso(str);
  }
  fsaso("\n");
  for(j = 0; j < nouts_loc; j++) {
    sprintf(str, " #      %s:", short_classnames[j]);
    fsaso(str);
    for(k = 0; k < nouts_loc; k++) {
      sprintf(str, " %3d", sround(e(outlvl, k, j)));
      fsaso(str);
    }
    fsaso("\n");
  }
  fsaso(" #  unknown\n #    * ");
  for(k = 0; k < nouts_loc; k++) {
    sprintf(str, " %3d", sround(outrej[k]));
    fsaso(str);
  }
  sprintf(str, "\n\n Histogram of errors, from 2^(-%d) to 1\n",
    ERRS_NERRS_DIM - 1);
  fsaso(str);
  for(j = 0; j < ERRS_NERRS_DIM; j++) {
    sprintf(str, " %6d", nerrs[j]);
    fsaso(str);
  }
  fsaso("\n");
  for(j = 0; j < ERRS_NERRS_DIM; j++) {
    sprintf(str, " %6.1f", 100. * (float)nerrs[j] / (float)ntotal);
    fsaso(str);
  }
  fsaso("%\n \n");
}

/*******************************************************************/

/* accum_sumout: Writes some summary output to stderr, to the short
outfile, and possibly to the "NN.PROGRESS" file.

Input args:
  purpose: CLASSIFIER or FITTER.
  npats: Number of patterns.
  iter: Current iteration.
  c: A character to be used at the beginning of the first summary
    line: should be either ' ' (blank), or 'F', meaning "final".
    'F' is suitable if the summary line results from a final sending
    of the patterns through the net during a training run.
  w: Network weights.
  rmserr: Root-mean-square error.
  e1: Main part of error, basically.
  e2: Mean squared weight.

Output args:
  wtd_nr: Weighted "number right".
  wtd_nw: Weighted "number wrong".
  wtd_rpct_min: Minimum, across classes, of weighted
    "right-percentage by class".
*/

void accum_sumout(char purpose, int npats, int iter, char c, float w[],
                  float rmserr, float e1, float e2, int *wtd_nr, int *wtd_nw,
                  float *wtd_rpct_min)
{
  FILE *fp_nnp;
  char str[200];
  int i, wtd_nrej;
  float a, denom, r_acc, w_acc, rej_acc, wtd_nr_pct,
    wtd_nw_pct, wtd_nrej_pct;

  /* Compute weighted "right-percentages by class" and their minimum,
  and accumulate the accumulators across classes for next part */
  for(i = 0, *wtd_rpct_min = 100., r_acc = w_acc = rej_acc = 0.;
    i < nouts_loc; i++) {
    denom = r_acc_bc[i] + w_acc_bc[i] + rej_acc_bc[i];
    if(denom == 0.)
      denom = 1.;
    iwtd_rpct_bc[i] = sround(a = 100. * r_acc_bc[i] / denom);
    if(a < *wtd_rpct_min)
      *wtd_rpct_min = a;
    r_acc += r_acc_bc[i];
    w_acc += w_acc_bc[i];
    rej_acc += rej_acc_bc[i];
  }

  /* Compute (across-classes) weighted "numbers right, wrong, and
  rejected" and the corresponding percentages */
  denom = r_acc + w_acc + rej_acc;
  if(denom == 0.)
    denom = 1.;
  *wtd_nr = sround(npats * r_acc / denom);
  wtd_nr_pct = 100. * (float)(*wtd_nr) / (float)npats;
  *wtd_nw = sround(npats * w_acc / denom);
  wtd_nw_pct = 100. * (float)(*wtd_nw) / (float)npats;
  wtd_nrej = npats - *wtd_nr - *wtd_nw;
  wtd_nrej_pct = 100. * (float)wtd_nrej / (float)npats;

  if(iter >= 0) {
    if((fp_nnp = fopen("NN.PROGRESS", "wb")) == (FILE *)NULL)
      fatalerr("accum_sumout (accum.c)", "fopen of NN.PROGRESS \
for writing failed", NULL);
    if(iter == 0 || c == 'F') {
      if(purpose == CLASSIFIER)
	accum_yow(fp_nnp, "     Iter   Err (   Ep    Ew)     OK\
    UNK     NG      OK   UNK    NG\n");
      else
	accum_yow(fp_nnp, "     Iter   Err (   Ep    Ew)\n");
    }
    if(purpose == CLASSIFIER) {
      sprintf(str, "  %c %5d %5.3f (%5.3f %5.3f) %6d %6d %6d\
 = %5.1f %5.1f %5.1f %%\n", c, iter, rmserr, e1, e2, *wtd_nr,
        wtd_nrej, *wtd_nw, wtd_nr_pct, wtd_nrej_pct, wtd_nw_pct);
      accum_yow(fp_nnp, str);
      sprintf(str, " %5.1f  ", *wtd_rpct_min);
      accum_yow(fp_nnp, str);

      for(i = 0; i < nouts_loc; i++) {
	sprintf(str, " %2d", iwtd_rpct_bc[i]);
	accum_yow(fp_nnp, str);
      }
      accum_yow(fp_nnp, "\n");
    }
    else {
      sprintf(str, "  %c %5d %5.3f (%5.3f %5.3f)\n", c, iter,
        rmserr, e1, e2);
      accum_yow(fp_nnp, str);
    }
    fclose(fp_nnp);
  }

  else { /* (iter < 0) */
    if(purpose == CLASSIFIER) {
      fsaso("           Err (   Ep    Ew)     OK\
    UNK     NG      OK   UNK    NG\n");
      sprintf(str, "     Test %5.3f (%5.3f %5.3f) %6d %6d %6d\
 = %5.1f %5.1f %5.1f %%\n", rmserr, e1, e2, *wtd_nr, wtd_nrej,
        *wtd_nw, wtd_nr_pct, wtd_nrej_pct, wtd_nw_pct);
      fsaso(str);
      sprintf(str, " %5.1f  ", *wtd_rpct_min);
      fsaso(str);
      for(i = 0; i < nouts_loc; i++) {
	sprintf(str, " %2d", iwtd_rpct_bc[i]);
	fsaso(str);
      }
      fsaso("\n");
    }
    else {
      fsaso("           Err (   Ep    Ew)\n");
      sprintf(str, "     Test %5.3f (%5.3f %5.3f)\n", rmserr, e1, e2);
      fsaso(str);
    }
  }

}

/*******************************************************************/

/* accum_yow: Causes str to be fputs'd, with subsequent fflushing, to
stderr, to the short outfile, and to stream fp. */

void accum_yow(FILE *fp, char str[])
{
  fsaso(str);
  fputs(str, fp);
  fflush(fp);
}

/*******************************************************************/
