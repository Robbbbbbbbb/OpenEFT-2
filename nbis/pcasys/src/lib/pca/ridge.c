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

      FILE:    RIDGE.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG
               08/16/2009 by BBandini - gcc 4.4.1 won't compile math function
                             when passed a constant; two mods: atan(dd2)
                             instead of atan((double)2.) and atan(dd5) instead
                             of atan((double).5).

      Routines dealing with ridge-valley orientations.

      ROUTINES:
#cat: rors - extracts pixelwise orientations, and averages squares of them
#cat:        to make an array of local average orientations.
#cat: rgar - re-averages pixelwise orientations with registration-shifted
#cat:        squares, thereby making the registered orientation array.
#cat: ar2 - averages pixelwise orientations with overlapping squares, for
#cat:        a more finely spaced array of local average orientations
#cat:        (not registered) which will be used by the pseudoridge
#cat:        tracer (pseudo).

***********************************************************************/

#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

static float *c, *s;

/*******************************************************************/

/* Extracts from a raster the pixelwise ridge-orientation indices
(pixelrors) and also averages them in non-overlapping WSxWS-pixel
squares to make average ridge-orientation vectors (avrors_x,avrors_y).
Each vector (avrors_x[i][j],avrors_y[i][j]) is the average, over
square (i,j), of unit vectors pointing in the directions of the
doubled pixelwise-orientation angles; it will therefore have a short
length if the pixelwise orientations vary widely within its square,
because of cancellation.  Outer squares get pixelwise orientations
computed only for those of their pixels that are centers of slits that
fit entirely on the raster, and the average-orientations of outer
squares are not computed. */

void rors(unsigned char **ehras, const int w, const int h,
          const int rors_slit_range_thresh, char ***pixelrors,
          float ***avrors_x, float ***avrors_y, int *aw, int *ah)
{
  char apixelror;
  int ib, is, ie, i, jb, ojs, js, je, j, slit_minind, slit_maxind, aslit,
    slit_sum, slit_minval, slit_maxval, *hp;
  int nib, njb;
  static int f = 1, hist[8], *hpe;
  float *cp, *sp, x, y;
  char **pxrors;
  float **avx, **avy;

  if(isverbose())
    printf("  find local average ridge orientations\n");
  if(f) {
    f = 0;
    make_cs(&c, &s);
    hpe = hist + 8;
  }

  malloc_dbl_char(&pxrors, h, w, "rors pixelrors");
  *pixelrors = pxrors;

  for(i = 4; i < h-4; i++) {
    for(j = 4; j < w-4; j++) {
      slit_sum = slit_minval = slit_maxval =
        ehras[i-4][j]+ehras[i+4][j]+ehras[i-2][j]+ehras[i+2][j];
      slit_minind = slit_maxind = 0;
      slit_sum += (aslit = ehras[i-4][j+2] + ehras[i+4][j-2] +
        ehras[i-2][j+1] + ehras[i+2][j-1]);
      if(aslit < slit_minval) {
        slit_minval = aslit;
        slit_minind = 1;
      }
      else if(aslit > slit_maxval) {
        slit_maxval = aslit;
        slit_maxind = 1;
      }
      slit_sum += (aslit = ehras[i-4][j+4] + ehras[i+4][j-4] +
        ehras[i-2][j+2] + ehras[i+2][j-2]);
      if(aslit < slit_minval) {
        slit_minval = aslit;
        slit_minind = 2;
      }
      else if(aslit > slit_maxval) {
        slit_maxval = aslit;
        slit_maxind = 2;
      }
      slit_sum += (aslit = ehras[i-2][j+4] + ehras[i+2][j-4] +
        ehras[i-1][j+2] + ehras[i+1][j-2]);
      if(aslit < slit_minval) {
        slit_minval = aslit;
        slit_minind = 3;
      }
      else if(aslit > slit_maxval) {
        slit_maxval = aslit;
        slit_maxind = 3;
      }
      slit_sum += (aslit = ehras[i][j+4] + ehras[i][j-4] +
        ehras[i][j+2] + ehras[i][j-2]);
      if(aslit < slit_minval) {
        slit_minval = aslit;
        slit_minind = 4;
      }
      else if(aslit > slit_maxval) {
        slit_maxval = aslit;
        slit_maxind = 4;
      }
      slit_sum += (aslit = ehras[i+2][j+4] + ehras[i-2][j-4] +
        ehras[i+1][j+2] + ehras[i-1][j-2]);
      if(aslit < slit_minval) {
        slit_minval = aslit;
        slit_minind = 5;
      }
      else if(aslit > slit_maxval) {
        slit_maxval = aslit;
        slit_maxind = 5;
      }
      slit_sum += (aslit = ehras[i+4][j+4] + ehras[i-4][j-4] +
        ehras[i+2][j+2] + ehras[i-2][j-2]);
      if(aslit < slit_minval) {
        slit_minval = aslit;
        slit_minind = 6;
      }
      else if(aslit > slit_maxval) {
        slit_maxval = aslit;
        slit_maxind = 6;
      }
      slit_sum += (aslit = ehras[i+4][j+2] + ehras[i-4][j-2] +
        ehras[i+2][j+1] + ehras[i-2][j-1]);
      if(aslit < slit_minval) {
        slit_minval = aslit;
        slit_minind = 7;
      }
      else if(aslit > slit_maxval) {
        slit_maxval = aslit;
        slit_maxind = 7;
      }
      if(slit_maxval - slit_minval >= rors_slit_range_thresh)
        pxrors[i][j] = (((float)(slit_minval + slit_maxval +
          4 * ehras[i][j]) > .375 * (float)slit_sum) ? slit_maxind
          : slit_minind);
      else
        pxrors[i][j] = BAD_PIXELROR;
    }
  }

  *ah = (int)(h/WS)-2;
  *aw = (int)(w/WS)-2;
  malloc_dbl_flt(&avx, *ah, *aw, "rors avrors_x");
  *avrors_x = avx;
  malloc_dbl_flt(&avy, *ah, *aw, "rors avrors_y");
  *avrors_y = avy;
  nib = *ah;
  njb = *aw;
  is = (h-(nib*WS))/2;
  ie = is+WS;
  ojs = (w-(njb*WS))/2;
  for(ib = 0; ib < nib; ib++, is += WS, ie += WS) {
    js = ojs;
    je = js+WS;
    for(jb = 0; jb < njb; jb++, js += WS, je += WS) {
      memset(hist, 0, 8 * sizeof(int));
      for(i = is; i < ie; i++)
	for(j = js; j < je; j++)
	  if((apixelror = pxrors[i][j]) != BAD_PIXELROR)
	    hist[(int)apixelror]++;
      for(x = y = 0., cp = c, sp = s, hp = hist; hp < hpe;) {
	x += *cp++ * *hp;
	y += *sp++ * *hp++;
      }
      avx[ib][jb] = x;
      avy[ib][jb] = y;
    }
  }
#ifdef GRPHCS
  grphcs_bars(*avrors_x, *avrors_y, *aw, *ah, 0);
#endif
}

/*******************************************************************/

/* Takes an array of pixelwise ridge-orientations (pixelrors), a
"core" pixel location found by r92 (corepixel_x,corepixel_y), and the
"standard registration point" (i.e. median core) location
(srp_x,srp_y), and produces a registered average-ridge-orientation
array (reg_avrors_x, reg_avrors_y), by using averaging-squares that
are shifted so as to correspond to a movement that brings the core
pixel to the standard registration point. */

void rgar(char **pixelrors, const int w, const int h,
          const int corepixel_x, const int corepixel_y, RGAR_PRS *rgar_prs,
          float ***reg_avrors_x, float ***reg_avrors_y, int *aw, int *ah)
{
  char apixelror;
  int i, j, ib, is, ie, jb, js, jss, je, jes, *hp;
  static int f = 1, hist[8], *hpe;
  float *cp, *sp, x, y;
  int nib, njb;
  int rl, bl;
  float **ravx, **ravy;

  if(isverbose())
    printf("  make registered orientation array\n");
  if(f) {
    f = 0;
    hpe = hist + 8;
  }
  *ah = (int)(h/WS)-2;
  *aw = (int)(w/WS)-2;
  malloc_dbl_flt(&ravx, *ah, *aw, "rgar reg_avrors_x");
  *reg_avrors_x = ravx;
  malloc_dbl_flt(&ravy, *ah, *aw, "rgar reg_avrors_y");
  *reg_avrors_y = ravy;
  nib = *ah;
  njb = *aw;
  rl = w - 4;
  bl = h - 4;
  jes = (jss = WS + corepixel_x - rgar_prs->std_corepixel_x) + WS;
  for(ib = 0, ie = (is = WS + corepixel_y - rgar_prs->std_corepixel_y)
    + WS; ib < nib; ib++, is += WS, ie += WS)
    if(is < 4 || ie > bl)
      for(jb = 0; jb < njb; jb++)
	ravx[ib][jb] = ravy[ib][jb] = 0.;
    else
      for(jb = 0, js = jss, je = jes; jb < njb; jb++, js += WS,
        je += WS)
	if(js < 4 || je > rl)
	  ravx[ib][jb] = ravy[ib][jb] = 0.;
	else {
	  memset(hist, 0, 8 * sizeof(int));
	  for(i = is; i < ie; i++)
	    for(j = js; j < je; j++)
	      if((apixelror = pixelrors[i][j]) != BAD_PIXELROR)
		hist[(int)apixelror]++;
	  for(x = y = 0., cp = c, sp = s, hp = hist; hp < hpe;) {
	    x += *cp++ * *hp;
	    y += *sp++ * *hp++;
	  }
	  ravx[ib][jb] = x;
	  ravy[ib][jb] = y;
	}
#ifdef GRPHCS
  grphcs_bars(*reg_avrors_x, *reg_avrors_y, *aw, *ah, 1);
#endif
}

/*******************************************************************/

/* Takes pixelwise ridge-orientations and averages them in overlapping
16x16-pixel squares, in effect, so as to make a 58x62-orientation
array. */

void ar2(char **pixelrors, const int w, const int h, float ***avrors2_x,
          float ***avrors2_y, int *aw, int *ah)
{
  char apixelror;
  int i, j, ii, is, ie, jj, js, je, *hp;
  static int f = 1, hist[8], *hpe;
  float *cp, *sp, xx, yy, **x, **y;
  int nib, njb;
  float **avx, **avy;

  if(isverbose())
    printf("  average old pixelwise orientations into finer array\n");
  if(f) {
    f = 0;
    hpe = hist + 8;
  }
  /* Make averages for nonoverlapping HWSxHWS-pixel squares... */
  nib = (h/HWS)-1;
  njb = (w/HWS)-1;
  malloc_dbl_flt(&x, nib, njb, "ar2 x");
  malloc_dbl_flt(&y, nib, njb, "ar2 y");

  is = (h-(HWS*nib))/2;
  ie = is + HWS;
  for(ii = 0; ii < nib; ii++, is += HWS, ie += HWS) {
    js = (w-(HWS*njb))/2;
    je = js + HWS;
    for(jj = 0; jj < njb; jj++, js += HWS, je += HWS) {
      memset(hist, 0, 8 * sizeof(int));
      for(i = is; i < ie; i++)
	for(j = js; j < je; j++)
	  if((apixelror = pixelrors[i][j]) != BAD_PIXELROR)
	    hist[(int)apixelror]++;
      for(xx = yy = 0., cp = c, sp = s, hp = hist; hp < hpe;) {
	xx += *cp++ * *hp;
	yy += *sp++ * *hp++;
      }
      x[ii][jj] = xx;
      y[ii][jj] = yy;
    }
  }
  /* ... then make each average for a 16x16-pixel square by adding
  the averages for the 4 adjacent HWSxHWS-pixel squares comprising it */
  *aw = njb-1;
  *ah = nib-1;
  malloc_dbl_flt(&avx, *ah, *aw, "ar2 avrors2_x");
  *avrors2_x = avx;
  malloc_dbl_flt(&avy, *ah, *aw, "ar2 avrors2_y");
  *avrors2_y = avy;
  for(i = 0, ii = 1; i < *ah; i++, ii++)
    for(j = 0, jj = 1; j < *aw; j++, jj++) {
      avx[i][j] = x[i][j] + x[i][jj] + x[ii][j] + x[ii][jj];
      avy[i][j] = y[i][j] + y[i][jj] + y[ii][j] + y[ii][jj];
    }

  free_dbl_flt(x, *ah + 1);
  free_dbl_flt(y, *ah + 1);
}

/*******************************************************************/

/* Computes the cosines and sines, divided by 256., of the doubled
slit-angles, using this 8-slit pattern:

  6 7 0 1 2

  5 67012 3
    5   3
  4 4 C 4 4
    3   5
  3 21076 5

  2 1 0 7 6 */

void make_cs(float **c, float **s)
{
  int i;
  float angle[8], *cp, *sp;
  double d;

  /* variables for atan arguments */
  double dd2 = (double)2.0;
  double dd5 = (double)0.5;

  angle[0] = PI / 2.;
  angle[1] = atan(dd2);
  angle[2] = PI / 4.;
  angle[3] = atan(dd5);
  angle[4] = 0.;
  angle[5] = -angle[3];
  angle[6] = -angle[2];
  angle[7] = -angle[1];
  *c = (float *)malloc_ch(8 * sizeof(float));
  *s = (float *)malloc_ch(8 * sizeof(float));
  for(i = 0, cp = *c, sp = *s; i < 8; i++, cp++, sp++) {
    *cp = cos(d = (double)(2. * angle[i])) / 256.;
    *sp = sin(d) / 256.;
  }
}

/*******************************************************************/
