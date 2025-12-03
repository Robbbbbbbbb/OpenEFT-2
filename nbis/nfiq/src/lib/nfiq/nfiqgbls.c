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


/************************************************************************/
/***********************************************************************
      LIBRARY: FING - NIST Fingerprint Systems Utilities

      FILE:           NFISGBLS.C
      IMPLEMENTATION: Michael D. Garris
      ALGORITHM:      Elham Tabassi
                      Charles L. Wilson
                      Craig I. Watson
      DATE:           09/09/2004

      Contains global variable definitions and initializations
      supporting NFIQ (NIST Fingerprint Image Quality) algorithm

***********************************************************************
***********************************************************************/

#include <nfiq.h>

/***********************************************************************
  Default global means for Z-Normalization of feature vectors
************************************************************************/
float dflt_znorm_means[NFIQ_VCTRLEN] = {
   2881.918457,
   119.406013,
   42.890446,
   42.011002,
   33.318542,
   18.573952,
   5.602001,
   0.122406,
   0.206616,
   0.223217,
   0.447761};


/***********************************************************************
  Default global stddevs for Z-Normalization of feature vectors
************************************************************************/
float dflt_znorm_stds[NFIQ_VCTRLEN] = {
   1.522167e+03,
   6.759113e+01,
   2.685183e+01,
   2.699416e+01,
   2.686451e+01,
   2.199553e+01,
   1.067551e+01,
   5.670758e-02,
   9.548551e-02,
   7.412220e-02,
   1.551811e-01};


/***********************************************************************
  Default MLP weights & attributes used to classify NFIQ feature vectors
************************************************************************/
char dflt_purpose = CLASSIFIER;
int  dflt_nInps = NFIQ_VCTRLEN;
int  dflt_nHids = 22;
int  dflt_nOuts = NFIQ_NUM_CLASSES;
char dflt_acfunc_hids = SINUSOID;
char dflt_acfunc_outs = SINUSOID;
float dflt_wts[] = {
-3.119589e-01, 6.611657e-01,-5.026219e-01, 3.649307e-01,-8.559146e-01,
 0.000000e+00, 0.000000e+00, 6.067616e-01,-1.805089e-01,-1.131759e-01,
-4.456701e-02,
 1.485702e-01,-8.047382e-01, 1.536431e+00,-6.267055e-01,-2.291293e-01,
 3.588256e-01,-4.772107e-02,-3.209697e-02, 5.032505e-02, 8.261050e-02,
-1.254393e-01,
 6.132897e-01,-1.411894e+00,-2.838681e+00, 3.381080e-01, 6.434316e-01,
-3.236280e-01,-7.699983e-01, 6.468319e-01,-4.339712e-01, 3.309879e-01,
-1.358493e-01,
 5.641593e-01,-5.056323e-01, 1.904472e+00,-5.811639e-01, 1.072900e-01,
 5.268215e-01,-4.427979e-02,-4.738337e-01,-4.169644e-01, 2.207548e-01,
 3.257259e-01,
 1.309097e+00,-2.789800e-01, 7.167372e-01,-1.587375e+00,-1.048402e+00,
-6.534492e-01, 7.294747e-01,-6.075797e-01,-4.250264e-01,-1.189583e+00,
 1.053694e+00,
 9.861280e-01,-2.313339e+00, 1.757847e+00,-1.355448e+00, 1.940423e-01,
 2.536710e-01, 1.017378e-01,-1.868141e-01,-7.930036e-01, 4.395724e-02,
 5.294373e-01,
 3.463260e-01,-7.978010e-01, 1.308727e+00,-6.636613e-01,-6.908953e-01,
 2.089418e-01,-3.036060e-01, 1.602989e-01,-3.998688e-01,-1.066859e-01,
 2.452043e-01,
-2.258594e+00, 8.253821e-01,-1.959279e+00,-1.823486e+00, 2.705060e-01,
 2.474845e-01,-5.042679e-01,-7.290037e-01,-9.019606e-01, 1.191169e+00,
 3.203081e-01,
-4.853204e-03, 1.646509e+00, 3.500651e+00,-8.528205e-01,-2.984882e-01,
 4.489827e-01,-2.571398e-01,-1.674349e-01, 5.363578e-01, 7.658816e-02,
-3.055720e-01,
 5.880828e-02,-1.379533e-01,-6.978128e-02,-1.724346e-01,-1.093761e+00,
 2.889399e-01,-4.144008e-01, 3.218620e-01, 1.125677e-01, 6.271239e-01,
-4.318374e-01,
 1.436580e-01, 6.330154e-01,-1.869801e+00,-7.242393e-02, 4.479175e-01,
-2.368623e-01, 3.373586e-01,-4.974723e-01, 1.072444e+00,-5.307779e-01,
-2.307383e-01,
-7.633747e-01,-2.442474e+00, 8.769030e-01, 0.000000e+00,-1.343813e-01,
-1.110623e-01,-4.376957e-01, 2.013028e-01, 4.305054e-01, 1.585102e-01,
-3.958981e-01,
 5.240926e-02,-2.490937e-01, 1.546568e+00, 0.000000e+00,-1.498653e-01,
 6.201565e-01, 2.109777e-01,-2.393391e-02, 3.447105e-01, 2.182600e-01,
-3.149046e-01,
 4.906907e-01, 1.403351e-01, 9.017189e-01,-1.210668e-01, 0.000000e+00,
-1.031165e+00, 3.980837e-01,-4.873019e-01, 4.677160e-01,-7.404075e-01,
 2.573620e-01,
 0.000000e+00, 7.471633e-02, 0.000000e+00, 0.000000e+00, 4.203801e-01,
 3.415256e-01,-7.038820e-01,-3.723057e-01,-1.196930e-01, 5.805833e-01,
-4.376991e-02,
 2.435902e-01,-8.516605e-01, 1.756720e+00,-3.907821e-01,-5.916777e-01,
 2.893333e-01,-2.783922e-01, 4.216255e-01,-2.463683e-01, 5.500950e-02,
 0.000000e+00,
 1.970175e+00,-2.958485e+00, 1.532450e+00,-8.923713e-01, 2.417961e-01,
 5.628923e-01,-2.440243e-01,-5.965174e-01, 1.139410e-01,-2.022855e-01,
 2.361645e-01,
 0.000000e+00, 6.754364e-01,-7.820091e-01, 1.013413e+00, 2.463124e-01,
 0.000000e+00, 1.023046e-01, 9.711869e-01,-1.343744e-01, 1.707461e-01,
-4.111827e-01,
 1.260770e-01,-2.684742e-01, 2.472874e-01, 1.606629e+00, 1.411805e+00,
 3.064711e-01,-7.956589e-01, 8.479106e-01,-5.768294e-01,-1.585826e-01,
 1.294553e-01,
 2.168621e+00,-3.034966e+00,-2.278084e+00, 4.302388e-01,-2.894798e-01,
 1.411841e-01, 9.723052e-01,-3.195443e-01, 1.879465e-01, 2.471027e-01,
-1.112849e-01,
 1.656582e+00, 4.157036e-01,-2.338839e+00, 1.443451e-01, 1.116763e+00,
-9.645813e-01, 3.676646e-01, 3.832221e-02, 4.517725e-01, 3.238751e-01,
-4.588898e-01,
 1.916666e+00, 1.443587e-01,-1.433865e-01, 3.240053e-01,-4.565576e-01,
 5.283880e-01,-4.196049e-01,-5.186083e-01,-2.434821e-01,-1.174421e-01,
 4.113020e-01,

-1.783172e-01, 0.000000e+00,-4.627872e-01,-3.577171e-01,-1.231293e-01,
-2.868853e-02, 7.272963e-01,-1.405041e+00, 2.315883e-01,-9.232272e-02,
-8.280038e-01,-8.069845e-01, 8.964362e-01,-1.068020e+00, 5.136124e-01,
 1.601462e-01, 1.949571e+00, 9.790514e-02,-1.881924e+00,-1.167217e+00,
-1.862109e+00, 2.797367e-01,

-1.119622e+00, 6.913635e-01,-2.314126e+00, 5.009105e-01, 8.618460e-01,
-1.282761e-01, 7.247450e-01, 1.302407e+00,-2.491474e+00, 8.938180e-01,
-9.503597e-01,-8.490843e-01, 2.466252e-01,-2.921042e-01,-1.599071e-01,
 6.380814e-01, 1.745111e+00,-6.558891e-01, 1.536597e+00,-1.801105e+00,
 8.278723e-01,-1.115732e+00,
 0.000000e+00, 4.914292e-01, 2.043260e+00,-1.017582e+00,-1.337804e+00,
 3.123492e-01, 2.861827e-01,-5.710307e-01, 3.570163e-01,-7.774934e-01,
 1.263964e-01,-1.019178e+00, 4.599980e-01, 1.334275e+00,-8.112756e-01,
 2.932526e-01, 8.066791e-01,-3.043762e-01,-6.072499e-01, 7.365437e-01,
-2.282936e+00, 2.773401e-01,
-4.716995e-01,-1.667477e-01, 1.097124e+00,-1.401806e+00,-4.670044e-02,
-1.744312e+00, 2.863425e-01,-6.154195e-01, 1.668778e+00,-3.543290e-01,
-5.540912e-01, 2.741340e-01, 1.720121e-01,-1.260201e+00,-1.101267e-01,
 2.400029e-01, 3.960383e-02,-6.528167e-01,-8.679667e-01, 1.132805e+00,
-1.164291e-01, 8.910555e-01,
-6.713058e-01,-2.950077e-01,-9.163866e-01,-5.672185e-01, 3.286342e-01,
-8.068405e-01,-5.663291e-01, 6.598314e-01, 1.728234e+00,-2.267976e-01,
 7.203241e-02,-1.469119e-01,-7.360125e-01,-5.350198e-01, 7.811673e-01,
-1.242507e-01,-3.277558e-01, 1.590503e-01,-6.800562e-01, 5.582014e-01,
 1.338933e+00,-2.008687e+00,
 7.849125e-01,-1.091962e+00,-1.454492e-01, 1.507305e+00,-5.163293e-01,
 5.807534e-01,-9.741927e-01,-1.288107e-01, 6.773449e-01, 2.835115e-02,
 0.000000e+00, 2.863308e+00,-9.951057e-01, 1.037028e-01,-8.899538e-01,
-1.521524e+00,-2.041125e+00, 4.552743e-01, 3.357547e-01,-2.086011e-01,
-8.699499e-01, 2.495338e-01,

-6.955023e-01,-1.218332e+00,-3.902654e-01,-2.646753e-01,-8.862965e-01,
};
