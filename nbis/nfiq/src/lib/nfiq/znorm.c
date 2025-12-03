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

      FILE:           ZNORM.C
      IMPLEMENTATION: Michael D. Garris
      ALGORITHM:      Elham Tabassi
                      Charles L. Wilson
                      Craig I. Watson
      DATE:           09/09/2004

      Contains routines responsible for supporting
      Z-Normalization of feature vectors used by
      NFIQ (NIST Fingerprint Image Quality) algorithm

***********************************************************************
               ROUTINES:
                        znorm_fniq_fetvctr()
                        comp_znorm_stats()

***********************************************************************/

#include <stdio.h>
#include <util.h>
#include <nfiq.h>

/***********************************************************************
************************************************************************
#cat: znorm_fniq_featvctr - Routine Z-Normalized an NFIQ feature vector

   Input:
      featvctr    - NFIQ feature vector
      znorm_means - global means for each coef in the feature vector
      znorm_stds  - global stddev for each coef in the feature vector
      vctrlen     - allocated length of feature vector
   Output:
      featvctr    - resulting normalized feature vector values
************************************************************************/
void znorm_fniq_featvctr(float *featvctr,
           float *znorm_means, float *znorm_stds, const int vctrlen)
{
   int i;

   for(i = 0; i < vctrlen; i++)
      featvctr[i] = (featvctr[i] - znorm_means[i])/znorm_stds[i];
}

/***********************************************************************
************************************************************************
#cat: comp_znorm_stats - Routine takes a list of feature vectors
#cat:             (a matrix) and computes the mean and stddev for each
#cat:             column of features coefs in the matrix.

   Input:
      feats       - list of input feature vectors
      nfeatvctrs  - number of vectors in the list
      nfeats      - number of coefs in each vector
   Output:
      omeans      - resulting allocated list of coef means
      ostddevs    - resulting allocated list of coef stddevs
   Return Code:
      Zero        - successful completion
      Negative    - system error
************************************************************************/
int comp_znorm_stats(float **omeans, float **ostddevs,
                              float *feats,
                              const int nfeatvctrs, const int nfeats)
{
   int f, v;
   float fret;
   float *means, *stddevs;
   float *mptr, *sdptr;
   float *sfeatptr, *featptr;
   float sum_x, sum_x2;

   means = (float *)malloc(nfeats * sizeof(float));
   if(means == (float *)NULL){
      fprintf(stderr, "ERROR : comp_znorm_stats : malloc : means\n");
      return(-2);
   }

   stddevs = (float *)malloc(nfeats * sizeof(float));
   if(stddevs == (float *)NULL){
      fprintf(stderr, "ERROR : comp_znorm_stats : malloc : stddevs\n");
      free(means);
      return(-3);
   }

   /* foreach column of feature vector matrix */
   sfeatptr = feats;
   mptr = means;
   sdptr = stddevs;
   for(f = 0; f < nfeats; f++){
      featptr = sfeatptr;
      /* sum_x column of features */
      sum_x = 0.0;
      sum_x2 = 0.0;
      for(v = 0; v < nfeatvctrs; v++){
         sum_x += *featptr;
         sum_x2 += (*featptr) * (*featptr);
         featptr += nfeats;
      }
      /* compute mean of column features */
      *mptr++ = sum_x / nfeatvctrs;
      fret = (float)ssx_stddev(sum_x, sum_x2, nfeatvctrs);
      if(fret < 0){
         free(means);
         free(stddevs);
         return(-4);
      }
      *sdptr++ = fret;

      /* bump to next column in feature vector matrix */
      sfeatptr++;
   }

   *omeans = means;
   *ostddevs = stddevs;

   return(0);
}
