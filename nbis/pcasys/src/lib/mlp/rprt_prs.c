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

      FILE:    RPRT_PRS.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: rprt_prs - Given a PARMS structure, this routine makes a report of the
#cat:            values of the interesting parms, and it writes this report to
#cat:            stderr and to the short outfile.

***********************************************************************/

/*
Input args:
  parms: A filled-in PARMS structure (typedef in parms.h).
  mpats: Full number of patterns in the patterns file.

Side effect: Writes to stderr and to the short outfile.
*/

#include <mlp.h>

void rprt_prs(PARMS *parms, int mpats)
{
  char *p, str[200], yow[200], buf[5000], acfunc_hids_str[50],
    acfunc_outs_str[50];

  p = (char *)NULL;

  if(parms->train_or_test.val != TRAIN &&
    parms->train_or_test.val != TEST) {
    sprintf(str, "parms->train_or_test.val must be TRAIN (%d) or \
TEST (%d); it is %d", (int)TRAIN, (int)TEST,
      (int)(parms->train_or_test.val));
    fatalerr("rprt_prs", str, NULL);
  }
  if(parms->boltzmann.val != NO_PRUNE &&
    parms->boltzmann.val != ABS_PRUNE &&
    parms->boltzmann.val != SQUARE_PRUNE) {
    sprintf(str, "parms->boltzmann.val must be NO_PRUNE (%d), \
ABS_PRUNE (%d), or SQUARE_PRUNE (%d); it is %d", (int)NO_PRUNE,
      (int)ABS_PRUNE, (int)SQUARE_PRUNE, (int)(parms->boltzmann.val));
    fatalerr("rprt_prs", str, NULL);
  }

  buf[0] = 0;

  if(parms->purpose.val == CLASSIFIER)
    p = "Classifier";
  else if(parms->purpose.val == FITTER)
    p = "Fitter";
  else {
    sprintf(str, "parms->purpose.val must be CLASSIFIER (%d) or \
FITTER (%d); it is %d", (int)CLASSIFIER, (int)FITTER,
      (int)(parms->purpose.val));
    fatalerr("rprt_prs", str, NULL);
  }
  sprintf(yow, " %s MLP\n", p);
  strcat(buf, yow);

  if(parms->train_or_test.val == TRAIN)
    p = "Training";
  else /* parms->train_or_test.val == TEST */
    p = "Testing";
  sprintf(yow, " %s run\n", p);
  strcat(buf, yow);

  if(parms->npats.val == mpats)
    sprintf(str, "using all %d patterns", mpats);
  else
    sprintf(str, "using first %d of %d patterns", parms->npats.val,
      mpats);
  sprintf(yow, " Patterns file: %s; %s\n", parms->patterns_infile.val,
    str);
  strcat(buf, yow);

  if(parms->priors.val == ALLSAME)
    strcpy(str, "set all equal,\n  no files read");
  else if(parms->priors.val == CLASS)
    sprintf(str, "made from provided class-wts,\n  file %s",
      parms->class_wts_infile.val);
  else if(parms->priors.val == PATTERN)
    sprintf(str, "set to provided pattern-wts,\n  file %s",
      parms->pattern_wts_infile.val);
  else if(parms->priors.val == BOTH)
    sprintf(str, "made from provided class-wts and pattern-wts,\n\
  files %s and %s", parms->class_wts_infile.val,
      parms->pattern_wts_infile.val);
  else {
    sprintf(str, "parms->priors.val must be ALLSAME (%d), \
CLASS (%d), PATTERN (%d), or BOTH (%d); it is %d", (int)ALLSAME,
      (int)CLASS, (int)PATTERN, (int)BOTH, (int)(parms->priors.val));
    fatalerr("rprt_prs", str, NULL);
  }
  sprintf(yow, " Final pattern-wts: %s\n", str);
  strcat(buf, yow);

  if(parms->errfunc.val == MSE)
    strcpy(str, "sum of squares");
  else if(parms->errfunc.val == TYPE_1)
    sprintf(str, "type 1, alpha %.3e", parms->alpha.val);
  else if(parms->errfunc.val == POS_SUM)
    strcpy(str, "positive sum");
  else {
    sprintf(str, "parms->errfunc.val must be MSE (%d), TYPE_1 (%d), \
or POS_SUM (%d); it is %d", (int)MSE, (int)TYPE_1, (int)POS_SUM,
      (int)(parms->errfunc.val));
    fatalerr("rprt_prs", str, NULL);
  }
  sprintf(yow, " Error function: %s\n", str);
  strcat(buf, yow);

  sprintf(yow, " Reg. factor: %.3e\n", parms->regfac.val);
  strcat(buf, yow);

  strcpy(acfunc_hids_str, acsmaps_code_to_str(parms->acfunc_hids.val));
  strcpy(acfunc_outs_str, acsmaps_code_to_str(parms->acfunc_outs.val));
  sprintf(yow, " Activation fns. on hidden, output nodes: %s, %s\n",
    acfunc_hids_str, acfunc_outs_str);
  strcat(buf, yow);

  sprintf(yow, " Nos. of input, hidden, output nodes: %d, %d, \
%d\n", parms->ninps.val, parms->nhids.val, parms->nouts.val);
  strcat(buf, yow);

  if(parms->train_or_test.val == TRAIN) {
    if(parms->boltzmann.val == NO_PRUNE) {
      sprintf(yow, " Boltzmann pruning not used\n");
      strcat(buf, yow);
      sprintf(yow, " Will use hybrid SCG/LBFGS, with \
scg_earlystop_pct %.1f\n", parms->scg_earlystop_pct.val);
      strcat(buf, yow);
      sprintf(yow, " lbfgs_gtol: %.2e; lbfgs_mem: %d\n",
        parms->lbfgs_gtol.val, parms->lbfgs_mem.val);
      strcat(buf, yow);
    }
    else { /* parms->boltzmann.val is ABS_PRUNE or
           SQUARE_PRUNE */
      sprintf(yow, " Boltzmann pruning, thresh. exp(-%s/T), \
T %.3e\n", (parms->boltzmann.val == ABS_PRUNE ? "|w|" : "w^2"),
        parms->temperature.val);
      strcat(buf, yow);
      strcat(buf, " Will use SCG\n");
    }
  }    

  if(parms->train_or_test.val == TRAIN) {
    p = " Initial network weights:";
    if(parms->wts_infile.ssl.set)
      sprintf(yow, "%s from file %s\n", p, parms->wts_infile.val);
    else
      sprintf(yow, "%s random, seed %d\n", p, parms->seed.val);
    strcat(buf, yow);
    sprintf(yow, " Final network weights will be written as file \
%s\n", parms->wts_outfile.val);
    strcat(buf, yow);
  }
  else { /* parms->train_or_test.val == TEST */
    sprintf(yow, " Network weights: from file %s\n",
      parms->wts_infile.val);
    strcat(buf, yow);
  }

  if(parms->train_or_test.val == TRAIN) {
    sprintf(yow, " Stopping criteria (max. no. of iterations %d):\n",
      parms->niter_max.val);
    strcat(buf, yow);
    sprintf(yow, "  (RMS err) <= %.3e OR\n", parms->egoal.val);
    strcat(buf, yow);
    sprintf(yow, "  (RMS g) <= %.3e * (RMS w) OR\n",
      parms->gwgoal.val);
    strcat(buf, yow);
    sprintf(yow, "  (RMS err) > %.3e * (RMS err %d iters ago) OR\n",
      parms->errdel.val, parms->nfreq.val);
    strcat(buf, yow);
    sprintf(yow, "  (OK - NG count) < (count %d iters ago) + %d.  (OK level: %.3f)\n", parms->nfreq.val, parms->nokdel.val,
      parms->oklvl.val);
    strcat(buf, yow);
  }

  if(parms->long_outfile.ssl.set)
    sprintf(str, ": %s", parms->long_outfile.val);
  else
    sprintf(str, " not made");
  sprintf(yow, " Long outfile%s\n", str);
  strcat(buf, yow);

  strcat(buf, "\n");

  fsaso(buf);
}
