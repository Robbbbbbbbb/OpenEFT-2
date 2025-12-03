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
      LIBRARY: PCASYS - Pattern Classification System

      FILE:    R92A.C
      AUTHORS: G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG
               01/21/2009 (Greg Fiumara) - Fix to support 64-bit

      ROUTINES:
#cat: r92a - Converts orientation vectors to angles for r92, transposing and
#cat:        padding to 32x32, and then calls r92 to find the core.

***********************************************************************/


#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

void r92a(float **avrors_x, float **avrors_y, const int aw, const int ah,
          const float discard_thresh, int *corepixel_x, int *corepixel_y)
{
  int i, j, im1, jm1;
  static int f = 1;
  float x, y, dg[32][32];
  static float nopi;
  int t_corepixel_x, t_corepixel_y, r92class;

  if(isverbose())
    printf("  find core\n");

  if(aw > 32 || ah > 32)
    fatalerr("r92a","aw or ah dimension greater than 32",
             "max width of orientation feature for r92 is 32");
  if(f) {
    f = 0;
    nopi = 90. / PI;
  }
  /* Convert average-orientation vectors to angles for use by r92,
  also transposing (because pcasys_r92_prog was originally in
  fortran) and padding from awxah to 32x32. */
  for(i = 0, im1 = -1; i < 32; i++, im1++)
    for(j = 0, jm1 = -1; j < 32; j++, jm1++) {
      if(im1 < 0 || im1 >= aw || jm1 < 0 || jm1 >= ah)
	dg[i][j] = 100.;
      else {
	x = avrors_x[jm1][im1];
	y = avrors_y[jm1][im1];
	if(x * x + y * y < discard_thresh)
	  dg[i][j] = 100.;
	else {
	  if(x < 0.) {
	    if(y < 0.)
	      dg[i][j] = nopi * atan((double)(y / x)) - 90.;
	    else if(y == 0.)
	      dg[i][j] = 90.;
	    else
	      dg[i][j] = nopi * atan((double)(y / x)) + 90.;
	  }
	  else if(x == 0.) {
	    if(y < 0.)
	      dg[i][j] = -45.;
	    else if(y == 0.)
	      dg[i][j] = 0.;
	    else
	      dg[i][j] = 45.;
	  }
	  else
	    dg[i][j] = nopi * atan((double)(y / x));
	}
      }
    }
  r92((float *)dg, &t_corepixel_x, &t_corepixel_y, &r92class);
  *corepixel_x = t_corepixel_x;
  *corepixel_y = t_corepixel_y;
  if(*corepixel_x > (aw+1)*16)
     *corepixel_x = ((aw+2)*16)/2;
  if(*corepixel_y > (ah+1)*16)
     *corepixel_y = ((ah+2)*16)/2;
#ifdef GRPHCS
  grphcs_core_medcore(*corepixel_x, *corepixel_y, (aw+2)*16, (ah+2)*16);
#endif
}
