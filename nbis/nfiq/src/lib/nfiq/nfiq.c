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

      FILE:           NFIQ.C
      IMPLEMENTATION: Michael D. Garris
      ALGORITHM:      Elham Tabassi
                      Charles L. Wilson
                      Craig I. Watson
      DATE:           09/09/2004
      UPDATED: 11/21/2006 by KKO

      Contains routines responsible for supporting
      NFIQ (NIST Fingerprint Image Quality) algorithm

***********************************************************************
               ROUTINES:
                        comp_nfiq_featvctr()
                        comp_nfiq()
                        comp_nfiq_flex()

***********************************************************************/

#include <stdio.h>
#include <nfiq.h>

/***********************************************************************
************************************************************************
#cat: comp_nfiq_featvctr - Routine takes results from NIST's Mindtct and
#cat:                      computes a feature vector for computing NFIQ

   Input:
      vctrlen     - allocated length of feature vector
      minutiae    - list of minutiae from NIST's Mindtct
      quality_map - quality map computed by NIST's Mindtct
      map_w       - width of map
      map_h       - height of map
   Output:
      featvctr    - resulting feature vector values
   Return Code:
      Zero        - successful completion
      EMPTY_IMG   - empty image detected (feature vector set to 0's)
************************************************************************/
int comp_nfiq_featvctr(float *featvctr, const int vctrlen, MINUTIAE *minutiae,
                       int *quality_map, const int map_w, const int map_h,
                       int *optflag)
{
   int i, t, foreground;
   float *featptr;
   int qmaphist[QMAP_LEVELS];
   int *qptr, qmaplen;
   int num_rel_bins = NFIQ_NUM_CLASSES;
   double rel_threshs[NFIQ_NUM_CLASSES] = {0.5, 0.6, 0.7, 0.8, 0.9};
   int rel_bins[NFIQ_NUM_CLASSES], passed_thresh;

   qmaplen = map_w * map_h;
   memset(qmaphist, 0, QMAP_LEVELS*sizeof(int));
   memset(rel_bins, 0, num_rel_bins*sizeof(int));

   /* Generate qmap histogram */
   qptr = quality_map;
   for(i = 0; i < qmaplen; i++){
      qmaphist[*qptr++]++;
   }

   /* Compute pixel foreground */
   foreground = qmaplen - qmaphist[0];

   if(foreground == 0){
      for(i = 0; i < vctrlen; i++)
          featvctr[i] = 0.0;
      return(EMPTY_IMG);
   }

   /* Compute reliability bins */
   for(i = 0; i < minutiae->num; i++){
      passed_thresh = 1;
      for(t = 0; t < num_rel_bins && passed_thresh; t++){
         if(minutiae->list[i]->reliability > rel_threshs[t]){
            rel_bins[t]++;
         }
         else{
            passed_thresh = 0;
         }
      }
   }

   featptr = featvctr;
   
   /* Load feature vector */
   /* 1. qmap foreground count */
   *featptr++ = foreground;
   /* 2. number of minutiae */
   *featptr++ = minutiae->num;
   /* 3. reliability count > 0.5 */
   t = 0;
   *featptr++ = rel_bins[t++];
   /* 4. reliability count > 0.6 */
   *featptr++ = rel_bins[t++];
   /* 5. reliability count > 0.7 */
   *featptr++ = rel_bins[t++];
   /* 6. reliability count > 0.8 */
   *featptr++ = rel_bins[t++];
   /* 7. reliability count > 0.9 */
   *featptr++ = rel_bins[t++];
   /* 8. qmap count == 1 */
   i = 1;
   *featptr++ = qmaphist[i++]/(float)foreground;
   /* 9. qmap count == 2 */
   *featptr++ = qmaphist[i++]/(float)foreground;
   /* 10. qmap count == 3 */
   *featptr++ = qmaphist[i++]/(float)foreground;
   /* 11. qmap count == 4 */
   *featptr++ = qmaphist[i++]/(float)foreground;
   
   if (*optflag == 1)
   {
      fprintf(stdout,"%d\t%d\t%d\t%d\t%d\t%d\t%d\n%f\t%f\t%f\t%f\n",
              foreground, minutiae->num,
              rel_bins[0], rel_bins[1], rel_bins[2], rel_bins[3], rel_bins[4],
              qmaphist[1]/(float)foreground, qmaphist[2]/(float)foreground,
              qmaphist[3]/(float)foreground, qmaphist[4]/(float)foreground);
   }

      /* return normally */
      return(0);

}

/***********************************************************************
************************************************************************
#cat: comp_nfiq - Routine computes NFIQ given an input image.
#cat:             This routine uses default statistics for Z-Normalization
#cat:             and default weights for MLP classification.

   Input:
      idata       - grayscale fingerprint image data
      iw          - image pixel width
      ih          - image pixel height
      id          - image pixel depth (should always be 8)
      ippi        - image scan density in pix/inch
                    If scan density is unknown (pass in -1),
                    then default density of 500ppi is used.
   Output:
      onfiq       - resulting NFIQ value
      oconf       - max output class MLP activation
   Return Code:
      Zero        - successful completion
      EMPTY_IMG   - empty image detected (feature vector set to 0's)
      TOO_FEW_MINUTIAE - too few minutiae detected from fingerprint image,
                    indicating poor quality fingerprint
      Negative    - system error
************************************************************************/
int comp_nfiq(int *onfiq, float *oconf, unsigned char *idata,
              const int iw, const int ih, const int id, const int ippi,
              int *optflag)
{
   int ret;

   ret = comp_nfiq_flex(onfiq, oconf, idata, iw, ih, id, ippi,
                        dflt_znorm_means, dflt_znorm_stds,
                        dflt_nInps, dflt_nHids, dflt_nOuts,
                        dflt_acfunc_hids, dflt_acfunc_outs, dflt_wts,
                        optflag);

   return(ret);
}

/***********************************************************************
************************************************************************
#cat: comp_nfiq_flex - Routine computes NFIQ given an input image.
#cat:             This routine requires statistics for Z-Normalization
#cat:             and weights for MLP classification.

   Input:
      idata       - grayscale fingerprint image data
      iw          - image pixel width
      ih          - image pixel height
      id          - image pixel depth (should always be 8)
      ippi        - image scan density in pix/inch
                    If scan density is unknown (pass in -1),
                    then default density of 500ppi is used.
      znorm_means - global mean for each feature vector coef used for Z-Norm
      znorm_stds  - global stddev for each feature vector coef used for Z-Norm
      nInps       - feature vector length (number of MLP inputs)
      nHids       - number of hidden layer neurodes in MLP
      nOuts       - number of NFIQ levels (number of MLP output classes)
      acfunc_hids - type of MLP activiation function used at MLP hidden layer
      acfunc_outs - type of MLP activiation function used at MLP output layer
      wts         - MLP classification weights
   Output:
      onfiq       - resulting NFIQ value
      oconf       - max output class MLP activation
   Return Code:
      Zero        - successful completion
      EMPTY_IMG   - empty image detected (feature vector set to 0's)
      TOO_FEW_MINUTIAE - too few minutiae detected from fingerprint image,
                    indicating poor quality fingerprint
      EMPTY_IMG   - empty image detected (feature vector set to 0's)
      Negative    - system error
************************************************************************/
int comp_nfiq_flex(int *onfiq, float *oconf, unsigned char *idata,
              const int iw, const int ih, const int id, const int ippi,
              float *znorm_means, float *znorm_stds,
              const int nInps, const int nHids, const int nOuts,
              const char acfunc_hids, const char acfunc_outs, float *wts,
              int *optflag)
{
   int ret;
   float featvctr[NFIQ_VCTRLEN], outacs[NFIQ_NUM_CLASSES];
   unsigned char *bdata;
   int bw, bh, bd;
   double ippmm;
   MINUTIAE *minutiae;
   int *direction_map, *low_contrast_map, *low_flow_map;
   int *high_curve_map, *quality_map;
   int map_w, map_h;
   int class_i;
   float maxact;

   /* If image ppi not defined, then assume 500 */
   if(ippi == UNDEFINED)
      ippmm = DEFAULT_PPI / (double)MM_PER_INCH;
   else 
      ippmm = ippi / (double)MM_PER_INCH;

   /* Detect minutiae */
   if((ret = get_minutiae(&minutiae, &quality_map, &direction_map,
                         &low_contrast_map, &low_flow_map, &high_curve_map,
                         &map_w, &map_h, &bdata, &bw, &bh, &bd,
                         idata, iw, ih, id, ippmm, &lfsparms_V2))){
      return(ret);
   }
   free(direction_map);
   free(low_contrast_map);
   free(low_flow_map);
   free(high_curve_map);
   free(bdata);

   /* Catch case where too few minutiae detected */
   if(minutiae->num <= MIN_MINUTIAE){
      free_minutiae(minutiae);
      free(quality_map);
      *onfiq = MIN_MINUTIAE_QUAL;
      *oconf = 1.0;
      return(TOO_FEW_MINUTIAE);
   }

   /* Compute feature vector */
   ret = comp_nfiq_featvctr(featvctr, NFIQ_VCTRLEN,
                            minutiae, quality_map, map_w, map_h, optflag);
   if(ret == EMPTY_IMG){
      free_minutiae(minutiae);
      free(quality_map);
      *onfiq = EMPTY_IMG_QUAL;
      *oconf = 1.0;
      return(ret);
   }

   free_minutiae(minutiae);
   free(quality_map);

   /* ZNormalize feature vector */
   znorm_fniq_featvctr(featvctr, znorm_means, znorm_stds, NFIQ_VCTRLEN);

   /* Classify feature vector with feedforward MLP */
   if((ret = runmlp2(nInps, nHids, nOuts, acfunc_hids, acfunc_outs,
                    wts, featvctr, outacs, &class_i, &maxact))){
      return(ret);
   }

   *onfiq = class_i + 1;
   *oconf = maxact;

   /* return normally */
   return(0);
}
