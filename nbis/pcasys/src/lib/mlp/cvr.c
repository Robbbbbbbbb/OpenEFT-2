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

      FILE:    cvr.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/16/2005 by MDG

      Routines for making a weighted correct-vs.-rejected table:

      ROUTINES:
#cat: cvr_init - Initialization of a weighted correct-vs-rejected table.
#cat: cvr_zero - Zeros out the accumulators.
#cat: cvr_cpat - Accumulates for current pattern.
#cat: cvr_print - Finishes the table computations and writes table.

***********************************************************************/

#include <mlp.h>

static int n_threshes;
static float *threshes, *whright, *whwrong, *whunkwn;

/********************************************************************/

/* cvr_init: Initialization.  Mallocs the accumulators and thresholds,
and sets the threshold values.  (Mlp should call this just once,
before doing any of the runs.)

Side effects: Mallocs local (to this file) buffers, and sets values
  of local variables.
*/

void cvr_init()
{
#define NSEGS 4
  int j, k, n;
  int ibin[4] = {10, 12, 10, 20};
  float t1, t2, dt;
  float bdry[5] = {0., 0.5, 0.8, 0.9, 1.};

  n_threshes = 1;
  for(j = 0; j < NSEGS; j++)
    n_threshes += ibin[j];
  if(bdry[NSEGS] >= 1.)
    n_threshes--;
  if((whright = (float *)malloc(n_threshes * sizeof(float))) ==
    (float *)NULL)
    syserr("cvr_init (cvr.c)", "malloc", "whright");
  if((whwrong = (float *)malloc(n_threshes * sizeof(float))) ==
    (float *)NULL)
    syserr("cvr_init (cvr.c)", "malloc", "whwrong");
  if((whunkwn = (float *)malloc(n_threshes * sizeof(float))) ==
    (float *)NULL)
    syserr("cvr_init (cvr.c)", "malloc", "whunkwn");
  if((threshes = (float *)malloc(n_threshes * sizeof(float))) ==
    (float *)NULL)
    syserr("cvr_init (cvr.c)", "malloc", "threshes");
  threshes[0] = bdry[0];
  n = 0;
  for(j = 0; j < NSEGS; j++) {
    t1 = bdry[j];
    t2 = bdry[j + 1];
    dt = (t2 - t1) / ibin[j];
    for(k = 0; k < ibin[j] - 1; k++) {
      n++;
      threshes[n] = threshes[n - 1] + dt;
    }
    if(j < NSEGS - 1 || t2 < 1.) {
      /* don't go all the way to 1 */
      n++;
      threshes[n] = t2;
    }
  }
}

/********************************************************************/

/* cvr_zero: Zeros out the accumulators.  E_and_g should call this at
its start.

Side effect: Zeros the contents of local (to this file) buffers.
*/

void cvr_zero()
{
  memset(whright, 0, n_threshes * sizeof(float));
  memset(whwrong, 0, n_threshes * sizeof(float));
  memset(whunkwn, 0, n_threshes * sizeof(float));
}

/********************************************************************/

/* cvr_cpat: Updates the accumlators according to the current pattern.
E_and_g should call this for each pattern in turn, in its
patterns-loop.

Input args:
  confidence: Confidence of current pattern.  (Currently the highest
    activation is used, but it could be defined differently, e.g.
    highest activation minus second-highest.)
  class: Class of current pattern.
  hyp_class: Hypothetical class (i.e. according to mlp) of current
    pattern.
  patwt: Pattern-weight of current pattern.

Side effects: Updates the values of local (to this file) accumulators.
*/

void cvr_cpat(float confidence, short class, short hyp_class, float patwt)
{
  int i;

  for(i = 0; i < n_threshes; i++)
    if(confidence >= threshes[i])
      if(class == hyp_class)
	whright[i] += patwt;
      else
	whwrong[i] += patwt;
    else
      whunkwn[i] += patwt;
}

/********************************************************************/

/* cvr_print: Finishes computations for the weighted
correct-vs.-rejected table, and writes it.

Input args:
  train_or_test: TRAIN or TEST.
  npats: Number of patterns used.

Side effects: Writes weighted correct-vs.-rejected table to stderr and
  to the short outfile.
*/

void cvr_print(char train_or_test, int npats)
{
  char ch, str[100];
  int i, nright, nwrong, nunkwn;
  float denom, corr_pct, rej_pct;

  fsaso("\n          thresh     right   unknown     wrong\
   correct  rejected\n");
  ch = (train_or_test == TRAIN ? 'r' : 's');
  for(i = 0; i < n_threshes; i++) {
    denom = whright[i] + whwrong[i] + whunkwn[i];
    nright = sround((float)npats * whright[i] / denom);
    nwrong = sround((float)npats * whwrong[i] /denom);
    nunkwn = npats - nright - nwrong;
    corr_pct = 100. * (float)nright / (float)mlp_max(1, nright + nwrong);
    rej_pct = 100. * (float)nunkwn / (float)npats;
    sprintf(str, "%2dt%c %11.6f %9d %9d %9d %9.2f %9.2f\n", i + 1, ch,
      threshes[i], nright, nunkwn, nwrong, corr_pct, rej_pct);
    fsaso(str);
  }
}

/********************************************************************/
