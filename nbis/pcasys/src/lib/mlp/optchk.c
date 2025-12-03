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

      FILE:    OPTCHK.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      NOTE: This version uses file-scope static variables, set by calling
            helper routines, so as to keep many values that would normally
            be args of optchk out of its arg list.

      ROUTINES:
#cat: optchk_store_unchanging_vals - Copies its args to file-scope static
#cat:              variables which optchk reads later.
#cat: optchk_store_e1_e2 - Stores main part of error and the mean squared
#cat:                      weight.
#cat: optchk - Checks progress of neural net optimization and perhaps prints.
      
***********************************************************************/

#include <mlp.h>

/* These are set by optchk_store_unchanging_vals when it is called
(once, near start of training run), and later used repeatedly by
optchk. */
static int npats, nouts, nfreq, nokdel;
static char **long_classnames, **short_classnames, boltzmann;
static float temperature, errdel, oklvl;

/* These are repeatedly
  1. set by optchk_store_e1_e2 and then
  2. used by optchk. */
static float e1, e2;

/*******************************************************************/

/* optchk_store_unchanging_vals: Copies its args to file-scope static
variables which optchk reads later.  Since these values do not change
during a training run, it is sufficient to call this routine just once
in a run, before the first optchk call.

Input args:
  npats_in:
  long_classnames_in:
  short_classnames_in:
  nouts_in:
  boltzmann_in:
  temperature_in:
  nfreq_in:
  errdel_in:
  nokdel_in:
  oklvl_in:
  
Side effect: Copies its input args to file-scope static variables
  which are used later by optchk.
*/

void optchk_store_unchanging_vals(int npats_in, char **long_classnames_in,
            char **short_classnames_in, int nouts_in, char boltzmann_in,
            float temperature_in, int nfreq_in, float errdel_in,
            int nokdel_in, float oklvl_in)
{
  npats = npats_in;
  long_classnames = long_classnames_in;
  short_classnames = short_classnames_in;
  nouts = nouts_in;
  boltzmann = boltzmann_in;
  temperature = temperature_in;
  nfreq = nfreq_in;
  errdel = errdel_in;
  nokdel = nokdel_in;
  oklvl = oklvl_in;
}

/*******************************************************************/

/*
Input args:
  e1_in: Main part of error, basically.
  e2_in: Mean squared weight.

Side effect: Copies its input args to file-scope static variables
  which are used later by optchk.
*/

void optchk_store_e1_e2(float e1_in, float e2_in)
{
  e1 = e1_in;
  e2 = e2_in;
}

/*******************************************************************/

/* optchk: Checks progress of neural net optimization; perhaps prints.
Returns ierr of 0 if okay convergence so far.  After kmin iterations,
starts checking convergence.  Stops with error return in ierr if
convergence has been slow for NNOT times in a row.  If either of the
following two situations occurs, that is defined to be slow
convergence:
  3. Error hasn't gone down by at least a factor of errdel over the
       most recent block of nfreq iterations.
  4. For each of the most recent NNOT blocks of nfreq iterations,
       the weighted "number right", and the weighted "number right"
       minus weighted "number wrong", have both failed to increase by
       at least nokdel over the block.

Input args:
  do_confuse: Sent to accum_print if iter is 0.  If TRUE, accum_print
    finds and prints the confusion matrix and the matrix of the
    average output level.
  iter: How many iterations the optimization has used so far.
  w: Network weights.
  err: Error.

Output args:
  ierr: A code number whose values mean the following:
    0: Convergence is ok so far.
    3: Convergence is slow by criterion 3 above.
    4: Convergence is slow by criterion 4 above.
  wtd_rpct_min: Minimum, across classes, of weighted
    "right-percentage by class".

NOTICE: This routine is affected by the file-scope static variables
defined at the top of this file, some of which are set by
optchk_store_unchanging_vals and some of which are set by
optchk_store_e1_e2.
*/

#define OPTCHK_NF 2       /* Don't quit until NF * nfreq iters or */
                          /* until NITER iters, whichever is larger */
                          /* until NBOLTZ iters, if doing Boltzmann. */
                          /* Quit if not improving NNOT times in row. */

void optchk(char do_confuse, int iter, float w[], float err, int *ierr,
            float *wtd_rpct_min)
{
  static int wtd_nr, wtd_nw, wtd_nr_prv, wtd_nw_prv, ncount, notimp,
    kmin;
  float rmserr;
  static float rmsold;

  *ierr = 0;
  if(iter == 0) {
    accum_print(do_confuse, CLASSIFIER, npats, 0, err, e1, e2, ' ', w,
      long_classnames, short_classnames, &wtd_nr, &wtd_nw,
      wtd_rpct_min);
    ncount = 0; /* iterations since last check */
    notimp = 0; /* number of bad checks in a row */
    kmin = mlp_max(OPTCHK_NF * nfreq, NITER);
    if(boltzmann != NO_PRUNE && temperature > 0.)
      kmin = mlp_max(kmin, NBOLTZ);
    rmsold = sqrt((double)(2. * err));
  }
  else {
    if(iter == kmin) {
      wtd_nr_prv = wtd_nr;
      wtd_nw_prv = wtd_nw;
    }
    ncount++;
    if(ncount >= nfreq) {
      ncount = 0;
      accum_print(FALSE, CLASSIFIER, npats, iter, err, e1, e2, ' ',
        w, long_classnames, short_classnames, &wtd_nr, &wtd_nw,
        wtd_rpct_min);
      rmserr = sqrt((double)(2. * err));
      if(iter >= kmin && rmserr > errdel * rmsold) {
	*ierr = 3;
	return;
      }
      rmsold = rmserr;
      if(iter >= kmin && (wtd_nr - wtd_nr_prv < nokdel) &&
        (wtd_nr - wtd_nw) - (wtd_nr_prv - wtd_nw_prv) < nokdel)
	notimp++;
      else
	notimp = 0;
      if(notimp >= NNOT) {
	*ierr = 4;
	return;
      }
      wtd_nr_prv = mlp_max(wtd_nr, wtd_nr_prv);
      wtd_nw_prv = mlp_min(wtd_nw, wtd_nw_prv);
    }
    else
      cwrite(iter);
  }
}

/*******************************************************************/
