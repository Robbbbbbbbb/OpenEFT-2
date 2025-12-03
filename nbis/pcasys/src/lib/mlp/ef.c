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

      FILE:    EF.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      "Error functions" each of which, given an output activations
      vector, and a target vector or an actual class, and possibly a parm
      (alpha, for type_1), computes the resulting error contribution and its
      gradient w.r.t. the activations vector.  For use by e_and_g, which
      computes the error and its gradient w.r.t. the weights.

      ROUTINES:
#cat: ef_mse_t - computes "mse" error function, using a target vector.
#cat: ef_mse_c - computes "mse" error function, using a target class.
#cat: ef_t1_c - computes "type_1" error function, using a class.
#cat: ef_ps_c - computes "pos_sum" error function, using a class.

***********************************************************************/

#include <mlp.h>

/*******************************************************************/

/* For "mse" error function, using a target vector.

Input args:
  nouts: Number of output nodes.
  acsvec: Output activations vector.
  targvec: Target activations vector.

Output args:
  e: Error contribution of this pattern.
  g: Gradient of the error contribution of this pattern w.r.t. the
     output activations.  A vector of nouts elts, to be allocated by
     caller.
*/

void ef_mse_t(int nouts, float *acsvec, float *targvec, float *e, float *g)
{
  float a, e_yow, *ac_e;

  ac_e = acsvec + nouts;
  e_yow = 0.;
  while(acsvec < ac_e) {
    *g++ = 2. * (a = *acsvec++ - *targvec++);
    e_yow += a * a;
  }
  *e = e_yow;
}

/*******************************************************************/

/* For "mse" error function, using a class.

Input args:
  nouts: Number of output nodes.
  acsvec: Output activations vector.
  actual_class: The actual class of this pattern (in range 0 through
    nouts - 1).

Output args:
  e: Error contribution of this pattern.
  g: Gradient of the error contribution of this pattern w.r.t. the
     output activations.  A vector of nouts elts, to be allocated by
     caller.
*/

void ef_mse_c(int nouts, float *acsvec, short actual_class,
              float *e, float *g)
{
  float *actual_p, *ac_e, e_yow, a;

  actual_p = acsvec + actual_class;
  ac_e = acsvec + nouts;
  e_yow = 0.;
  while(acsvec < ac_e) {
    *g++ = 2. * (a = (acsvec == actual_p ? *acsvec++ - 1. :
      *acsvec++));
    e_yow += a * a;
  }
  *e = e_yow;
}

/*******************************************************************/

/* For "type_1" error function (using a class, the only possibility
for type_1).

Input args:
  nouts: Number of output nodes.
  acsvec: Output activations vector.
  actual_class: The actual class of this pattern (in range 0 through
    nouts - 1).
  alpha: Factor used (with minus sign) before taking the exp.

Output args:
  e: Error contribution of this pattern.
  g: Gradient of the error contribution of this pattern w.r.t. the
     output activations.  A vector of nouts elts, to be allocated by
     caller.
*/

void ef_t1_c(int nouts, float *acsvec, short actual_class, float alpha,
             float *e, float *g)
{
  float *actual_p, actual_ac, *ac_e, beta, *g_p, ep;

  actual_ac = *(actual_p = acsvec + actual_class);
  ac_e = acsvec + nouts;
  beta = 0.;
  for(g_p = g; acsvec < ac_e; acsvec++, g_p++)
    if(acsvec != actual_p) {
      beta += (ep = exp((double)(-alpha * (actual_ac - *acsvec))));
      *g_p = alpha * ep;
    }
  *e = 1. - 1. / (1. + beta);
  *(g + actual_class) = -alpha * beta;
}

/*******************************************************************/

/* For "pos_sum" error function (using a class, the only possibility
for pos_sum).

Input args:
  nouts: Number of output nodes.
  acsvec: Output activations vector.
  actual_class: The actual class of this pattern (in range 0 through
    nouts - 1).

Output args:
  e: Error contribution of this pattern.
  g: Gradient of the error contribution of this pattern w.r.t. the
     output activations.  A vector of nouts elts, to be allocated by
     caller.
*/

void ef_ps_c(int nouts, float *acsvec, short actual_class, float *e, float *g)
{
  float *actual_p, *ac_e, e_yow, a;

  actual_p = acsvec + actual_class;
  ac_e = acsvec + nouts;
  e_yow = 0.;
  while(acsvec < ac_e) {
    if(acsvec == actual_p) {
      a = 1. - *acsvec++;
      *g++ = -20. * a - 1.;
    }
    else {
      a = *acsvec++;
      *g++ = 20. * a + 1.;
    }
    e_yow += (10. * a + 1.) * a;
  }
}

/*******************************************************************/
