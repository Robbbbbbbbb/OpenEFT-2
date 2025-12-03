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

      FILE:    ENHNC.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG

      ROUTINES:
#cat: enhnc - Enhances the segmented fingerprint raster.

***********************************************************************/


#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

/* Added by MDG on 04-19-05 - function found in src/lib/fft/fft2dr.c */
extern void fft2dr(float [32][32], int);


void enhnc(unsigned char **segras, ENHNC_PRS *enhnc_prs,
          unsigned char ***ehras, const int w, const int h)
{
#ifdef GRPHCS
  unsigned char outsquare[WS][WS];
#endif
  static char discard[32][17];
  int i, j, i2, j2, i3, j3, i4, j4, i5, j5, k, m, m2, p, p2;
  static int f = 1;
  float r[32][32], a, maxabs;
  int ic, is;
  int jc, js;
  int jsi, jso, isi, iso, jls, jrs, ils, irs;
  unsigned char **eptr;

  if(isverbose())
    printf("  enhance\n");
  if(f) {
    f = 0;
    /* Make mask for discarding low- and high-frequency noise. */
    memset(discard, 0, 32 * 17 * sizeof(char));
    for(m = 1, m2 = -15; m < 17; m++, m2++)
      if((k = sq(m2) + 256) < enhnc_prs->rr1 ||
        k > enhnc_prs->rr2)
	discard[m][0] = 1;
    for(m = 0, m2 = -16; m < 32; m++, m2++)
      for(p = 1, p2 = -15; p < 16; p++, p2++)
	if((k = slen(m2, p2)) < enhnc_prs->rr1 ||
          k > enhnc_prs->rr2)
	  discard[m][p] = 1;
    for(m = 0, m2 = -16; m < 17; m++, m2++)
      if((k = sq(m2)) < enhnc_prs->rr1 ||
        k > enhnc_prs->rr2)
	discard[m][16] = 1;
  }

  malloc_dbl_uchar(&eptr, h, w, "enhnc ehras");
  *ehras = eptr;
  for(j = 0; j < h; j++)
     for(i = 0; i < w; i++)
        eptr[j][i] = 0;

  jc = (w/WS)-2;
  jsi = (w-(jc*WS))/2;
  jso = jsi-(32-WS)/2;
  ic = (h/WS)-2;
  isi = (h-(ic*WS))/2;
  iso = isi-(32-WS)/2;

  /* Go through image with a step size of WS pixels */
  for(i = iso, i4 = isi; i < (WS*ic+iso); i += WS, i4 += WS) {
    for(j = jso, j4 = jsi; j < (WS*jc+jso); j += WS, j4 += WS) {
      js = j;
      is = i;
      jls = ils = (32-WS)/2;
      if(is < 0) {
         ils += is;
         is = 0;
      }
      if(is > h-32) {
         ils += (is-(h-32));
         is = h-32;
      }
      if(js < 0) {
         jls += js;
         js = 0;
      }
      if(js > w-32) {
         jls += (js-(w-32));
         js = w-32;
      }
      jrs = jls + WS;
      irs = ils + WS;

      /* Start enhancing current 32x32-pixel square, by taking
      forward FFT. */
      for(i2 = 0, i3 = is; i2 < 32; i2++, i3++)
	for(j2 = 0, j3 = js; j2 < 32; j2++, j3++)
	  r[i2][j2] = segras[i3][j3];
      fft2dr(r, 1);

      /* For each element of the FFT output, either set it to zero
      (if mask indicates that should be done), or take its
      squared length and raise that to the enhnc_prs->pow power
      (slpow) and then multiply the result by the original (complex)
      number.  Since the FFT is real (not complex), the desired result
      is had by the following code. */
      r[0][0] = 0.;
      for(m = 1; m < 16; m++) {
	if(discard[m][0])
	  r[2*m-1][0] = r[2*m][0] = 0.;
	else {
	  a = slpow(r[2*m-1][0], r[2*m][0], enhnc_prs->pow);
	  r[2*m-1][0] *= a;
	  r[2*m][0] *= a;
	}
      }
      if(discard[16][0])
	r[31][0] = 0.;
      else
	r[31][0] *= sqpow(r[31][0], enhnc_prs->pow);
      for(m = 0; m < 32; m++)
	for(p = 1; p < 16; p++) {
	  if(discard[m][p])
	    r[m][2*p-1] = r[m][2*p] = 0.;
	  else {
	    a = slpow(r[m][2*p-1], r[m][2*p], enhnc_prs->pow);
	    r[m][2*p-1] *= a;
	    r[m][2*p] *= a;
	  }
	}
      if(discard[0][16])
	r[0][31] = 0.;
      else
	r[0][31] *= sqpow(r[0][31], enhnc_prs->pow);
      for(m = 1; m < 16; m++) {
	if(discard[m][16])
	  r[2*m-1][31] = r[2*m][31] = 0.;
	else {
	  a = slpow(r[2*m-1][31], r[2*m][31], enhnc_prs->pow);
	  r[2*m-1][31] *= a;
	  r[2*m][31] *= a;
	}
      }
      if(discard[16][16])
	r[31][31] = 0.;
      else
	r[31][31] *= sqpow(r[31][31], enhnc_prs->pow);

      /* Finish enhancing the square: take backward fft, then apply a
      reasonable affine transform to middle WS x WS pixels and load
      them into output raster. */
      fft2dr(r, 0);
      for(i2 = 0, maxabs = 0.; i2 < 32; i2++)
	for(j2 = 0; j2 < 32; j2++)
	  if((a = fabs((double)r[i2][j2])) > maxabs)
	    maxabs = a;
      if(maxabs == 0.)
	for(i2 = ils, i3 = i4, i5 = 0; i2 < irs; i2++, i3++, i5++)
	  for(j2 = jls, j3 = j4, j5 = 0; j2 < jrs; j2++, j3++, j5++)
#ifdef GRPHCS
	    outsquare[i5][j5] =
#endif
	    eptr[i3][j3] = 128;
      else {
	a = 127. / maxabs;
	for(i2 = ils, i3 = i4, i5 = 0; i2 < irs; i2++, i3++, i5++)
	  for(j2 = jls, j3 = j4, j5 = 0; j2 < jrs; j2++, j3++, j5++)
#ifdef GRPHCS
	    outsquare[i5][j5] =
#endif
	    eptr[i3][j3] = a * r[i2][j2] + 128.5;
      }
#ifdef GRPHCS
      grphcs_enhnc_outsquare(outsquare, WS, WS, j4, i4);
#endif
    }
  }
#ifdef GRPHCS
  grphcs_enhnc_sleep();
#endif
}
