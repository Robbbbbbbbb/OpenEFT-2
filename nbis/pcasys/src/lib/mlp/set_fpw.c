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

      FILE:    SET_FPW.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: set_fpw - Sets the final pattern-weights.
#cat: compute_new_priors - Adjusts the class_wts if the input distribution
#cat:                      does not have an equal number of samples from
#cat:                      each class.

***********************************************************************/

#include <mlp.h>

void set_fpw(char priors, char *class_wts_infile, int nouts,
             char **short_classnames, char *pattern_wts_infile,
             int npats, short *classes, float **patwts)
{
  FILE *fp;
  char str[100];
  static char first = TRUE;
  short *classes_p;
  int i;
  static int nouts_maxsofar;
  float patwts_val, *patwts_p, *patwts_e, aweight, *class_wts;
  static float *patwts_byclass;

  if(first || nouts > nouts_maxsofar) {
    if(!first)
      free((char *)patwts_byclass);
    nouts_maxsofar = nouts;
    if((patwts_byclass = (float *)malloc(nouts * sizeof(float))) ==
      (float *)NULL)
      syserr("set_fpw", "malloc", "patwts_byclass");
    first = FALSE;
  }

  if((*patwts = (float *)malloc(npats * sizeof(float))) ==
    (float *)NULL)
    syserr("set_fpw", "malloc", "*patwts");

  switch(priors) {

  case ALLSAME:
    patwts_val = 1. / (float)npats;
    for(patwts_e = (patwts_p = *patwts) + npats;
      patwts_p < patwts_e; patwts_p++)
      *patwts_p = patwts_val;
    break;

  case CLASS:
    rd_cwts(nouts, short_classnames, class_wts_infile, &class_wts);
    compute_new_priors(nouts, short_classnames, classes, npats, class_wts);

    for(i = 0; i < nouts; i++)
      patwts_byclass[i] = class_wts[i] / npats;
    free((char *)class_wts);
    for(patwts_e = (patwts_p = *patwts) + npats,
      classes_p = classes; patwts_p < patwts_e; patwts_p++,
      classes_p++)
      *patwts_p = patwts_byclass[*classes_p];
    break;

  case PATTERN:
    if((fp = fopen(pattern_wts_infile, "rb")) == (FILE *)NULL)
      syserr("set_fpw", "fopen for reading failed",
        pattern_wts_infile);
    for(patwts_e = (patwts_p = *patwts) + npats;
      patwts_p < patwts_e; patwts_p++) {
      if(fscanf(fp, "%f", &aweight) != 1)
	fatalerr("set_fpw", "fscanf of aweight failed (probably not \
enough weights)", pattern_wts_infile);
      *patwts_p = aweight / (float)npats;
    }
    fclose(fp);
    break;

  case BOTH:
    rd_cwts(nouts, short_classnames, class_wts_infile, &class_wts);
    compute_new_priors(nouts, short_classnames, classes, npats, class_wts);

    if((fp = fopen(pattern_wts_infile, "rb")) == (FILE *)NULL)
      syserr("set_fpw", "fopen for reading failed",
        pattern_wts_infile);
    for(patwts_e = (patwts_p = *patwts) + npats,
      classes_p = classes; patwts_p < patwts_e; patwts_p++,
      classes_p++) {
      if(fscanf(fp, "%f", &aweight) != 1)
	fatalerr("set_fpw", "fscanf of aweight failed (probably not \
enough weights)", pattern_wts_infile);
      *patwts_p = class_wts[*classes_p] * aweight / (float)npats;
    }
    free((char *)class_wts);
    fclose(fp);
    break;

  default:
    sprintf(str, "priors must be ALLSAME (%d), CLASS (%d), \
PATTERN (%d), or BOTH (%d); it is %d", (int)ALLSAME, (int)CLASS,
      (int)PATTERN, (int)BOTH, (int)priors);
    fatalerr("set_fpw", str, NULL);
    break;

  } /* switch(priors) */
}

/**********************************************************/
void compute_new_priors(const int nouts, char **short_classnames,
             short *classes, const int npats, float *class_wts)
{
  int i, *dist_class_tots;
  float *dist_class_wts, wts_sum;
  char str[200];

  if((dist_class_tots = (int *)calloc(nouts, sizeof(int))) == (int *)NULL)
    syserr("compute_new_priors", "calloc", "dist_class_tots");
  if((dist_class_wts = (float *)calloc(nouts, sizeof(float))) == (float *)NULL)
    syserr("compute_new_priors", "calloc", "dist_class_wts");
  for(i = 0; i < npats; i++)
    dist_class_tots[classes[i]] += 1;
  for(i = 0; i < nouts; i++)
    dist_class_wts[i] = (float)dist_class_tots[i]/(float)npats;
  free(dist_class_tots);

  fsaso(" Given and Actual Prior Weights\n");
  for(i = 0; i < nouts; i++) {
    sprintf(str, "  %s => %f %f\n", short_classnames[i], class_wts[i],
                  dist_class_wts[i]);
    fsaso(str);
  }

  wts_sum = 0.0;
  for(i = 0; i < nouts; i++) {
    if(dist_class_wts[i] != 0.0)
       class_wts[i] /= dist_class_wts[i];
    else
       class_wts[i] = 0.0;
    wts_sum += class_wts[i];
  }
  free(dist_class_wts);
  for(i = 0; i < nouts; i++)
    class_wts[i] /= wts_sum;

  fsaso(" Given/Actual = New Prior Weights\n");
  for(i = 0; i < nouts; i++) {
    sprintf(str, "  %s -> %f\n", short_classnames[i], class_wts[i]);
    fsaso(str);
  }
}
