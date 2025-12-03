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
      LIBRARY: PCASYS_UTILS - Pattern Classification System Utils

      FILE:    OPTRWS_R.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/20/2005 by MDG

      This contains routines called by optrws (optimize regional weights
      command) and by optrwsgw (gradient-estimation worker program which
      optrws can run multiple instances of).  The routines are for
      computing the activation error rate of a Probabilistic Neural
      Net (PNN) when run on a set of example fingerprints, using the
      same set of examples as the PNN prototypes but leaving out of the
      prototypes set the particular example being classified.  The error
      rate is computed either as a function of a trial single value to
      which all weights are in effect set, or as a function of a vector
      of different regional weights.

      ROUTINES:
#cat: rws_to_acerror - activation error as a function of regional weights.
#cat: make_tranmat - (called by rws_to_acerror.)
#cat: diag_to_tranmat - (called by make_tranmat.)
#cat: transform_featvecs - (called by rws_to_acerror.)
#cat: optrws_pnn_acerror - runs PNN on feature vectors, computing
#cat:                      error rate.

***********************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <little.h>
#include <optrws_r.h>
#include <memalloc.h>
#include <util.h>

/********************************************************************/

/* Computes the activation error as a function of the regional
weights. */

float rws_to_acerror(float *rws, const int w, const int h, float *eigvecs,
           const int evt_sz, const int n_feats_use, const int n_klfvs_use,
           float *klfvs, unsigned char *classes, const int n_cls)
{
  static int firstcall = 1;
  static float *tranmat, *trfvs;

  if(firstcall) {
    firstcall = 0;
    /* Allocate reusable buffers for transform matrix and for
    transformed feature vectors. */
    tranmat = (float *)malloc_ch(n_feats_use * n_feats_use *
      sizeof(float));
    trfvs = (float *)malloc_ch(n_klfvs_use * n_feats_use *
      sizeof(float));
  }
  make_tranmat(rws, w, h, n_feats_use, eigvecs, evt_sz, tranmat);
  transform_featvecs(n_feats_use, tranmat, n_klfvs_use, klfvs, trfvs);
  return optrws_pnn_acerror(n_feats_use, n_klfvs_use, trfvs, classes,
    1., n_cls);
}

/******************************************************************/

/* Takes a set of regional weights and makes a transform matrix that
approximately emulates their effect.  If W is the order-evt_sz diagonal
matrix that would implement the weights in orientation-array space and
Psi has as its columns the first n_feats_use eigenvectors, then the
matrix produced is Psi^t * W * Psi, which is a square matrix of order
n_feats_use.  The Euclidean squared-distance between the transformed
versions of two K-L feature vectors, as transformed by the matrix
produced here, is an approximation of the squared-distance that would
have resulted between the corresponding orientation arrays if the
regional weights had been applied to them. */

void make_tranmat(float *rws, const int rw, const int rh,
          const int n_feats_use, float *eigvecs, const int evt_sz,
          float *tranmat)
{
  int i, ii, iis, iie, j, jj, jjs, jje, k;
  static int firstcall = 1;
  float a;
  static float *diag, *workbuf;
  float *dptr0, *dptr1;
  int dw;

  if(firstcall) {
    firstcall = 0;
    workbuf = (float *)malloc_ch(n_feats_use * evt_sz * sizeof(float));
    diag = (float *)malloc_ch(evt_sz * sizeof(float));
  }

  /* Make the diagonal matrix that corresponds to the regional
  weights (essentially, W), making only the elts on the diagonal. */
  dw = 2*rw;
  dptr0 = diag;
  dptr1 = diag + 4*(rw*rh);
  for(i = iis = k = 0, iie = 2; i < rh; i++, iis += 2, iie += 2)
    for(j = jjs = 0, jje = 2; j < rw; j++, jjs += 2, jje += 2, k++)
      for(ii = iis, a = rws[k]; ii < iie; ii++)
	for(jj = jjs; jj < jje; jj++)
	  dptr0[ii*dw+jj] = dptr1[ii*dw+jj] = a;

  /* Call a routine that finishes the work. */
  diag_to_tranmat(diag, n_feats_use, eigvecs, evt_sz, workbuf, tranmat);
}

/********************************************************************/

/* From diagonal matrix (diagonal elements only stored) to transform
matrix.  This is in its own routine becuase here it is simpler to
represent diag as having dimension [evt_sz]. */

void diag_to_tranmat(float *diag, const int n_feats_use, float *eigvecs,
          const int evt_sz, float *workbuf, float *tranmat)
{
  int i, j, k;
  float *p;
  double a;

  for(j = 0; j < evt_sz; j++) {
    a = diag[j];
    for(i = 0; i < n_feats_use; i++)
      workbuf[i*evt_sz+j] = eigvecs[i*evt_sz+j] * a;
  }
  for(i = 0, p = tranmat; i < n_feats_use; i++)
    for(j = 0; j < n_feats_use; j++) {
      for(k = 0, a = 0.; k < evt_sz; k++)
	a += workbuf[i*evt_sz+k] * eigvecs[j*evt_sz+k];
      *p++ = a;
    }
}

/********************************************************************/

/* Runs a (regional-weights-emulating) linear transform on a set of
K-L feature vectors. */

void transform_featvecs(const int n_feats_use, float *tranmat,
          const int n_fvs, float *fvs, float *fvs_tr)
{
  int i, j, k;
  float *p, *q, *r, *s;
  double a;

  for(k = 0, p = fvs, q = fvs_tr; k < n_fvs; k++, p += n_feats_use)
    for(i = 0, r = tranmat; i < n_feats_use; i++) {
      for(j = 0, s = p, a = 0.; j < n_feats_use; j++)
	a += *r++ * *s++;
      *q++ = a;
    }
}

/********************************************************************/

/* Computes the PNN activation error rate when a set of feature
vectors is classified, using the same set as the PNN prototypes but
leaving out the example being classified each time.  Uses smoothing
factor fac. */

float optrws_pnn_acerror(const int n_feats_use, const int n_fvs_use,
           float *fvs, unsigned char *classes, const float fac,
           const int n_cls)
{
  unsigned char *tu_classes_p, *pr_classes_p;
  int itu, ipr, i;
  float *tu_p, *pr_p, *p, *pe, *q, *ac;
  double accm, acsum, a, sd;

  malloc_flt(&ac, n_cls, "optrws_pnn_acerror ac");
  accm = 0.;
  for(itu = 0, tu_p = fvs, tu_classes_p = classes;
    itu < n_fvs_use; itu++, tu_p += n_feats_use, tu_classes_p++) {
    for(i = 0; i < n_cls; i++)
       ac[i] = 0.0;
    for(ipr = 0, pr_p = fvs, pr_classes_p = classes;
      ipr < n_fvs_use; ipr++, pr_p += n_feats_use, pr_classes_p++) {
      if(itu == ipr)
	continue;
      for(sd = 0., pe = (p = tu_p) + n_feats_use, q = pr_p;
        p < pe; p++, q++) {
	a = *p - *q;
	sd += a * a;
      }
      ac[*pr_classes_p] += exp((double)(-fac * sd));
    }
    for(acsum = ac[0], i = 1; i < n_cls; i++)
      acsum += ac[i];
    if(acsum > 0.) {
      a = 1. - ac[*tu_classes_p] / acsum;
      accm += a * a;
    }
    else
      accm += 1.;
  }
  free(ac);
  return accm / n_fvs_use;
}

/********************************************************************/
