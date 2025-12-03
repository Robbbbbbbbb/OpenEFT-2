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

      FILE:    RUNMLP.C
      AUTHORS: Charles Wilson
               G. T. Candela
               Michael D. Garris
      DATE:    1992
      UPDATED: 09/10/2004
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: mlphypscons - classifies the given set of feature vectors using
#cat:               the MLP code.
#cat: runmlp - runs the MLP classifier on a given feature vector.
#cat:
#cat: runmlp2 - runs the MLP classifier on a given feature vector.
#cat:           This version of this routine returns on error and does not
#cat:           directly exit.

***********************************************************************/

#include <mlp.h>
#include <mlpcla.h>

/*************************************************************/
void mlphypscons(int ninps, int nhids, int nouts, char acfunc_hids,
                 char acfunc_outs, float *weights, float *klts,
                 int nklts, int *hyps_i, float *cons)
{
   int i;
   float *outacts, *kptr;

   malloc_flt(&outacts, nouts, "mlphypscons : outacts");

   for(i = 0, kptr = klts; i < nklts; i++, kptr += ninps){
      runmlp(ninps, nhids, nouts, acfunc_hids, acfunc_outs, weights,
             kptr, outacts, &(hyps_i[i]), &(cons[i]));
   }

   free(outacts);
}

/*************************************************************/
/* runmlp: Runs the Multi-Layer Perceptron (MLP) on a feature vector.

Input args:
  ninps, nhids, nouts: Numbers of input, hidden, and output nodes
    of the MLP.
  acfunc_hids_code: Code character specifying the type of activation
    function to be used on the hidden nodes: must be LINEAR,
    SIGMOID, or SINUSOID (defined in parms.h).
  acfunc_outs_code: Code character specifying the type of activation
    function to be used on the output nodes.
  w: The MLP weights.
  featvec: The feature vector that the MLP is to be run on; its first
    ninps elts will be used.

Output args:
  outacs: The output activations.  This buffer must be provided by
    caller, allocated to (at least) nouts floats.
  hypclass: The hypothetical class, as an integer in the range
    0 through nouts - 1.
  confidence: A floating-point value in the range 0. through 1.
    Defined to be outacs[hypclass], i.e. the highest
    output-activation value.
*/

void runmlp(int ninps, int nhids, int nouts, char acfunc_hids_code,
            char acfunc_outs_code, float *w, float *featvec, float *outacs,
            int *hypclass, float *confidence)
{
  char str[100];
  static char t = 't';
  static int i1 = 1;
  float *w1, *b1, *w2, *b2, hidacs[MAX_NHIDS], *p, *pe, *maxac_p,
    maxac, ac;
  static float f1 = 1.;
  void (*acfunc_hids)(float *), (*acfunc_outs)(float *);
  void ac_v_linear(float *), ac_v_sigmoid(float *), ac_v_sinusoid(float *);

  acfunc_hids = (void (*)(float *))NULL;
  acfunc_outs = (void (*)(float *))NULL;

  if(nhids > MAX_NHIDS) {
    sprintf(str, "nhids, %d, is > MAX_NHIDS, defined as %d in \
runmlp.c", nhids, MAX_NHIDS);
    fatalerr("runmlp", str, NULL);
  }

  /* Resolve the activation function codes to functions. */
  switch(acfunc_hids_code) {
  case LINEAR:
    acfunc_hids = ac_v_linear;
    break;
  case SIGMOID:
    acfunc_hids = ac_v_sigmoid;
    break;
  case SINUSOID:
    acfunc_hids = ac_v_sinusoid;
    break;
  default:
    sprintf(str, "unsupported acfunc_hids_code %d.\n\
Supported codes are LINEAR (%d), SIGMOID (%d), and SINUSOID \
(%d).", (int)acfunc_hids_code, (int)LINEAR, (int)SIGMOID,
      (int)SINUSOID);
    fatalerr("runmlp", str, NULL);
    break;
  }
  switch(acfunc_outs_code) {
  case LINEAR:
    acfunc_outs = ac_v_linear;
    break;
  case SIGMOID:
    acfunc_outs = ac_v_sigmoid;
    break;
  case SINUSOID:
    acfunc_outs = ac_v_sinusoid;
    break;
  default:
    sprintf(str, "unsupported acfunc_outs_code %d.\n\
Supported codes are LINEAR (%d), SIGMOID (%d), and SINUSOID \
(%d).", (int)acfunc_outs_code, (int)LINEAR, (int)SIGMOID,
      (int)SINUSOID);
    fatalerr("runmlp", str, NULL);
    break;
  }

  /* Where the weights and biases of the two layers begin in w. */
  b2 = (w2 = (b1 = (w1 = w) + nhids * ninps) + nhids) + nouts * nhids;

  /* Start hidden activations out as first-layer biases. */
  memcpy((char *)hidacs, (char *)b1, nhids * sizeof(float));

  /* Add product of first-layer weights with feature vector. */
  mlp_sgemv(t, ninps, nhids, f1, w1, ninps, featvec, i1, f1, hidacs, i1);

  /* Finish each hidden activation by applying activation function. */
  for(pe = (p = (float *)hidacs) + nhids; p < pe; p++)
    acfunc_hids(p);

  /* Same steps again for second layer. */
  memcpy((char *)outacs, (char *)b2, nouts * sizeof(float));
  mlp_sgemv(t, nhids, nouts, f1, w2, nhids, hidacs, i1, f1, outacs, i1);
  for(pe = (p = outacs) + nouts; p < pe; p++)
    acfunc_outs(p);

  /* Find the hypothetical class -- the class whose output node
  activated most strongly -- and the confidence -- that activation
  value. */
  for(pe = (maxac_p = p = outacs) + nouts, maxac = *p, p++; p < pe;
    p++)
    if((ac = *p) > maxac) {
      maxac = ac;
      maxac_p = p;
    }
  *hypclass = maxac_p - outacs;
  *confidence = maxac;
}

/*************************************************************/
/* runmlp2: Runs the Multi-Layer Perceptron (MLP) on a feature vector.

Input args:
  ninps, nhids, nouts:
     Numbers of input, hidden, and output nodes
     of the MLP.
  acfunc_hids_code:
     Code character specifying the type of activation
     function to be used on the hidden nodes: must be LINEAR,
     SIGMOID, or SINUSOID (defined in parms.h).
  acfunc_outs_code:
     Code character specifying the type of activation
     function to be used on the output nodes.
  w: 
     The MLP weights.
  featvec:
     The feature vector that the MLP is to be run on; its first
     ninps elts will be used.

Output args:
  outacs:
     The output activations.  This buffer must be provided by
     caller, allocated to (at least) nouts floats.
  hypclass:
     The hypothetical class, as an integer in the range
     0 through nouts - 1.
  confidence:
     A floating-point value in the range 0. through 1.
     Defined to be outacs[hypclass], i.e. the highest
     output-activation value.
*/

int runmlp2(const int ninps, const int nhids, const int nouts,
            const char acfunc_hids_code, const char acfunc_outs_code,
            float *w, float *featvec,
            float *outacs, int *hypclass, float *confidence)
{
   static char t = 't';
   static int i1 = 1;
   static float f1 = 1.0;
   float *w1, *b1, *w2, *b2, hidacs[MAX_NHIDS], *p, *pe, *maxac_p,
         maxac, ac;
   void (*acfunc_hids)(float *), (*acfunc_outs)(float *);
   void ac_v_linear(float *), ac_v_sigmoid(float *), ac_v_sinusoid(float *);

   acfunc_hids = (void (*)(float *))NULL;
   acfunc_outs = (void (*)(float *))NULL;

   if(nhids > MAX_NHIDS) {
      fprintf(stderr, "ERROR : runmlp2 : nhids : %d > %d\n",
              nhids, MAX_NHIDS);
      return(-2);
   }

   /* Resolve the activation function codes to functions. */
   switch(acfunc_hids_code) {
   case LINEAR:
      acfunc_hids = ac_v_linear;
      break;
   case SIGMOID:
      acfunc_hids = ac_v_sigmoid;
      break;
   case SINUSOID:
      acfunc_hids = ac_v_sinusoid;
      break;
   default:
      fprintf(stderr, "ERROR : runmlp2 : acfunc_hids_code : %d unsupported\n",
              acfunc_hids_code);
      return(-3);
      break;
   }
   switch(acfunc_outs_code) {
   case LINEAR:
      acfunc_outs = ac_v_linear;
      break;
   case SIGMOID:
      acfunc_outs = ac_v_sigmoid;
      break;
   case SINUSOID:
      acfunc_outs = ac_v_sinusoid;
      break;
   default:
      fprintf(stderr, "ERROR : runmlp2 : acfunc_outs_code : %d unsupported\n",
              acfunc_outs_code);
      return(-4);
      break;
   }

   /* Where the weights and biases of the two layers begin in w. */
   b2 = (w2 = (b1 = (w1 = w) + nhids * ninps) + nhids) + nouts * nhids;

   /* Start hidden activations out as first-layer biases. */
   memcpy((char *)hidacs, (char *)b1, nhids * sizeof(float));

   /* Add product of first-layer weights with feature vector. */
   mlp_sgemv(t, ninps, nhids, f1, w1, ninps, featvec, i1, f1, hidacs, i1);

   /* Finish each hidden activation by applying activation function. */
   for(pe = (p = (float *)hidacs) + nhids; p < pe; p++)
      acfunc_hids(p);

   /* Same steps again for second layer. */
   memcpy((char *)outacs, (char *)b2, nouts * sizeof(float));
   mlp_sgemv(t, nhids, nouts, f1, w2, nhids, hidacs, i1, f1, outacs, i1);
   for(pe = (p = outacs) + nouts; p < pe; p++)
      acfunc_outs(p);

   /* Find the hypothetical class -- the class whose output node
      activated most strongly -- and the confidence -- that activation
      value. */
   for(pe = (maxac_p = p = outacs) + nouts, maxac = *p, p++; p < pe; p++){
      if((ac = *p) > maxac) {
         maxac = ac;
         maxac_p = p;
      }
   }

   *hypclass = maxac_p - outacs;
   *confidence = maxac;

   return(0);
}
