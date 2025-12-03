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

      FILE:    UNI.C
      AUTHORS: James Blue
               David Kahaner
               George Marsaglia
      DATE:    1981
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: uni - this routine generates quasi uniform random numbers on [0,1)
#cat:       and can be used on any computer with at least 16 bit integers.

***********************************************************************/

/*
***date written   810915
***revision date  810915
***category no.  g4a23
***keywords  random numbers, uniform random numbers
***author    blue, james, scientific computing division, nbs
             kahaner, david, scientific computing division, nbs
             marsaglia, george, computer science dept., wash state univ

***purpose  this routine generates quasi uniform random numbers on [0,1)
            and can be used on any computer with at least 16 bit integers,
            e.g., with a largest integer at least 32767.
***description

       this routine generates quasi uniform random numbers on the interval
       [0,1).  it can be used with any computer with at least 16 bit
       integers, e.g., with a largest integer at least equal to 32767.


   use
       first time....
                   z = uni(jd)
                     here jd is any  n o n - z e r o  integer.
                     this causes initialization of the program
                     and the first random number to be returned as z.
       subsequent times...
                   z = uni(0)
                     causes the next random number to be returned as z.

   machine dependencies...
      MDIG = a lower bound on the number of binary digits available
              for representing integers.

   remarks...
     a. this program can be used in two ways:
        (1) to obtain repeatable results on different computers,
            set MDIG to the smallest of its values on each, or,
        (2) to allow the longest sequence of random numbers to be
            generated without cycling (repeating) set MDIG to the
            largest possible value.
     b. the sequence of numbers generated depends on the initial
          input jd as well as the value of MDIG.
          if MDIG=16 one should find that
            the first evaluation
              z=uni(305) gives z=.027832881...
            the second evaluation
              z=uni(0) gives   z=.56102176...
            the third evaluation
              z=uni(0) gives   z=.41456343...
            the thousandth evaluation
              z=uni(0) gives   z=.19797357...

***references  (none)
***routines called  (none)
***end prologue  uni

  Converted from Fortran to C by gtc.
*/

#include <mlp.h>

/* Uncomment either line, to choose between 16 and 32 bits: */
/* #define MDIG_16 */
#define MDIG_32

/* Do NOT change these lines: */
#ifdef MDIG_16
#define MDIG 16
#define M1 ((int)0x7fff)
#else
#define MDIG 32
#define M1 ((int)0x7fffffff)
#endif
#define M2 ((int)(1<<(MDIG/2)))

float uni(int jd)
{
  int jseed, k0, k1, j0, j1, k;
  static int i, j, m[17];

  if(jd) {
    jseed = mlp_min(abs(jd), M1);
    if(!(jseed & 1))
      jseed--;
    k0 = 9069 % M2;
    k1 = 9069 / M2;
    j0 = jseed % M2;
    j1 = jseed / M2;
    for(i = 0; i < 17; i++) {
      jseed =j0 * k0;
      j1 = (jseed / M2 + j0 * k1 + j1 * k0) % (M2 / 2);
      j0 = jseed % M2;
      m[i] = j0 + M2 * j1;
    }
    i = 4;
    j = 16;
  }
  k = m[i] - m[j];
  if(k < 0)
    k += M1;
  m[j] = k;
  if(i)
    i--;
  else
    i = 16;
  if(j)
    j--;
  else
    j = 16;
  return (float)k / M1;
}
