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

      FILE:    SGMNT.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG
               08/16/2009 by BBandini - gcc 4.4.1 won't compile math function
                             when passed a constant; one mod: sqrt(x20)
                             instead of sqrt(2.0).

      ROUTINES:
#cat: sgmnt - Segments a sw X sh window from the original raster.
#cat: sgmnt_make_fg - Makes a down-sampled foreground of the original raster.
#cat: sgmnt_ebfc - Cleans up a foreground image.
#cat: sgmnt_edges - Finds the left, top, and right edges of the "cleaned"
#cat:               foreground image.
#cat: scan_row_from_left_foundtrue - Scans a row of foreground from left,
#cat:                                trying to find a true pixel
#cat: scan_row_from_right_foundtrue - Scans a row of foregrround from right,
#cat:                                 trying to find a true pixel
#cat: scan_col_from_top_foundtrue - Scans a column of foreground from top,
#cat:                               trying to find a true pixel
#cat: sgmnt_decide_location - Decides exact location to segment fingerprint.
#cat: sgmnt_snip_interp - Segments image using interpolation (slow).
#cat: sgmnt_snip - Segments image rounding off pixel locations.

***********************************************************************/

/* This function snips a swxsh-pixel (it can interpolate the
pixel value from the 4 nearest neighbors) rectangle out of the original
fingerprint raster.  The rectangle it selects does not necessarily
have its sides parallel to those of the input raster: it is supposed
to cause the selected rectangle to have an angle such that the
fingerprint will be upright in the output raster. */

#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif


/******************************************************************/

void sgmnt(unsigned char *origras, const int w, const int h,
           SGMNT_PRS *sgmnt_prs, unsigned char ***segras,
           const int sw, const int sh, unsigned char ***segras_fg,
           int *sfgw, int *sfgh)
{
  char str[200];
  static unsigned char *fg;
  int fgw, fgh, xc, yc, x_centroid, y_centroid;
  static int f = 1, fgw_max, fgh_max;
  float radians;

  if(isverbose())
    printf("  segment\n");
  if(f) {
    f = 0;
    fg = (unsigned char *)malloc_ch(
      (fgw_max = sgmnt_prs->origras_wmax / HWS) *
      (fgh_max = sgmnt_prs->origras_hmax / HWS));
  }
  if(w < sw) {
    sprintf(str, "original-raster width, %d, is < sw", w);
    fatalerr("sgmnt", str, NULL);
  }
  if(h < sh) {
    sprintf(str, "original-raster height, %d, is < sh", h);
    fatalerr("sgmnt", str, NULL);
  }
  if(w > sgmnt_prs->origras_wmax) {
    sprintf(str, "original-raster width, %d, is > \
sgmnt_prs->origras_wmax, %d", w,
      sgmnt_prs->origras_wmax);
    fatalerr("sgmnt", str, NULL);
  }
  if(h > sgmnt_prs->origras_hmax) {
    sprintf(str, "original-raster height, %d, is > \
sgmnt_prs->origras_hmax, %d", h,
      sgmnt_prs->origras_hmax);
    fatalerr("sgmnt", str, NULL);
  }
  fgw = w / HWS;
  fgh = h / HWS;
#ifdef GRPHCS
  grphcs_sgmntwork_init(fgw, fgh);
#endif
  if(sgmnt_make_fg(origras, w, h, fg, fgw, fgh, fgw_max, fgh_max,
    sgmnt_prs->min_fg, sgmnt_prs->max_fg,
    sgmnt_prs->fac_min, sgmnt_prs->fac_del,
    sgmnt_prs->fac_n) ||
    sgmnt_ebfc(fg, fgw, fgh, fgw_max, fgh_max, &x_centroid,
    &y_centroid, sgmnt_prs->nerode, sgmnt_prs->rsblobs,
    sgmnt_prs->fill)) {
    xc = w / 2;
    yc = h / 2;
    radians = 0.;
  }
  else
    sgmnt_edges(fg, fgw, fgh, fgw_max, fgh_max, x_centroid,
      y_centroid, &xc, &yc, &radians, sgmnt_prs->min_n,
      sgmnt_prs->slope_thresh, sgmnt_prs->hist_thresh, sw/HWS, sh/HWS);
#ifdef GRPHCS
  grphcs_sgmntwork_finish();
#endif
  sgmnt_snip(origras, w, h, xc, yc, radians, segras, sw, sh, fg, fgw, fgh,
    segras_fg, sfgw, sfgh);
#ifdef GRPHCS
  grphcs_segras(*segras, sw, sh);
#endif
}

/******************************************************************/

/*
Makes the "crunched foreground" of the original raster.
Return value:
  0: Normal; fg contains the crunched foreground.
  1: No candidate crunched foreground's number of true pixels was
       >= sgmnt_min_fg and <= sgmnt_max_fg
*/

int sgmnt_make_fg(unsigned char *origras, const int w, const int h,
                  unsigned char *fg, const int fgw, const int fgh,
                  const int fgw_max, const int fgh_max,
                  const int sgmnt_min_fg, const int sgmnt_max_fg,
                  const float sgmnt_fac_min, const float sgmnt_fac_del,
                  const int sgmnt_fac_n)
{
  unsigned char minpix, maxpix, yow, a_block_minpix;
  static unsigned char *block_minpix, **fg_arr;
  int range, ifac, i, j, ii, iis, iie, jj, jjs, jje, ntran,
    min_ntran = 0, best_ifac = 0, nfg, thresh, k;
  static int f = 1;
  float fac;

  if(f) {
    f = 0;
    block_minpix = (unsigned char *)malloc_ch(fgw_max * fgh_max);
    fg_arr = (unsigned char **)malloc_ch(sgmnt_fac_n *
      sizeof(unsigned char *));
    for(i = 0; i < sgmnt_fac_n; i++)
      fg_arr[i] = (unsigned char *)malloc_ch(fgw_max * fgh_max);
  }
  /* Find minimum of each HWSxHWS-pixel block, and also find overall
  minimum and maximum. */
  for(i = iis = 0, iie = HWS, minpix = maxpix = *origras; i < fgh; i++,
    iis += HWS, iie += HWS)
    for(j = jjs = 0, jje = HWS; j < fgw; j++, jjs += HWS, jje += HWS) {
      for(ii = iis, a_block_minpix = *(origras + iis * w + jjs);
        ii < iie; ii++)
	for(jj = jjs; jj < jje; jj++) {
	  if((yow = *(origras + ii * w + jj)) < a_block_minpix)
	    a_block_minpix = yow;
	  if(yow > maxpix)
	    maxpix = yow;
	}
      if((*(block_minpix + i * fgw + j) = a_block_minpix) < minpix)
	minpix = a_block_minpix;
    }

  /* Make a candidate foreground from each of the thresholds resulting
  from the several factors, by comparing threshold to block-minima
  array.  Of those candidates with numbers of true pixels within
  reasonable limits, select the one with the fewest transitions. */
  range = maxpix - minpix;
  for(i = 0; i < sgmnt_fac_n; i++)
    memset(fg_arr[i], 0, fgw * fgh * sizeof(unsigned char));
  for(ifac = k = 0, fac = sgmnt_fac_min; ifac < sgmnt_fac_n;
    ifac++, fac += sgmnt_fac_del) {
    thresh = minpix + fac * range + .5;
    for(nfg = i = 0; i < fgh; i++)
      for(j = 0; j < fgw; j++)
	if(*(block_minpix + i * fgw + j) <= thresh) {
	  *(fg_arr[ifac] + i * fgw + j) = 1;
	  nfg++;
	}
    if(nfg >= sgmnt_min_fg && nfg <= sgmnt_max_fg) {
      k++;
      for(i = ntran = 0; i < fgh; i++)
	for(j = 0; j < fgw - 1; j++)
	  if(*(fg_arr[ifac] + i * fgw + j) !=
	    *(fg_arr[ifac] + i * fgw + j + 1))
	    ntran++;
      for(j = 0; j < fgw; j++)
	for(i = 0; i < fgh - 1; i++)
	  if(*(fg_arr[ifac] + i * fgw + j) !=
	    *(fg_arr[ifac] + (i + 1) * fgw + j))
	    ntran++;
      if((k == 1) || ntran < min_ntran) {
	min_ntran = ntran;
	best_ifac = ifac;
      }
    }
  }
  if(!k)
    return 1;
  memcpy(fg, fg_arr[best_ifac], fgw * fgh * sizeof(unsigned char));
#ifdef GRPHCS
  grphcs_sgmntwork_fg(fg, 0);
#endif
  return 0;
}

/******************************************************************/

/* Takes a foreground map and performs: zero or more erosions;
possibly small-blob-removal; possibly row-hole and column-hole
filling; and computation of centroid.  Return value:
  0: Normal
  1: After erosions (if any), foreground was empty */

int sgmnt_ebfc(unsigned char *fg, const int fgw, const int fgh,
               const int fgw_max, const int fgh_max,
               int *x_centroid, int *y_centroid, const int sgmnt_nerode,
               const int sgmnt_rsblobs, const int sgmnt_fill)
{
  unsigned char *ucp, *ucpe;
  int i, x, y, k1, k2, empty, *ip, *ipe,
    *iq, *iqe;
  static int f = 1, *hhist, *vhist;

  if(f) {
    f = 0;
    hhist = (int *)malloc_ch(fgw_max * sizeof(int));
    vhist = (int *)malloc_ch(fgh_max * sizeof(int));
  }
  if(sgmnt_nerode)
    for(i = 0; i < sgmnt_nerode; i++)
      erode(fg, fgw, fgh);
  for(empty = 1, ucpe = (ucp = fg) + fgw * fgh; ucp < ucpe;)
    if(*ucp++) {
      empty = 0;
      break;
    }
  if(empty)
    return 1;
  if(sgmnt_rsblobs)
    rsblobs(fg, fgw, fgh);
  if(sgmnt_fill)
    rcfill(fg, fgw, fgh);
  memset(hhist, 0, fgw * sizeof(int));
  memset(vhist, 0, fgh * sizeof(int));
  for(ucp = fg, ipe = (ip = vhist) + fgh, iqe = hhist + fgw;
    ip < ipe; ip++)
    for(iq = hhist; iq < iqe; iq++)
      if(*ucp++) {
	(*ip)++;
	(*iq)++;
      }
  for(x = k1 = k2 = 0, ip = hhist; x < fgw; x++) {
    k1 += *ip * x;
    k2 += *ip++;
  }
  *x_centroid = (float)k1 / (float)k2 + .5;
  for(y = k1 = 0, ip = vhist; y < fgh;)
    k1 += *ip++ * y++;
  *y_centroid = (float)k1 / (float)k2 + .5;
#ifdef GRPHCS
  grphcs_sgmntwork_fg(fg, 1);
#endif
  return 0;
}

/*******************************************************************/

/* Finds the left, top, and right edges of the processed fg, fits
straight lines to them by linear regression, uses the slopes of the
lines to find the angle by which the fingerprint differs from
uprightness, and calls a routine that uses a histogram tilted by this
angle to find the top of the fingerprint and hence the location from
which the rotated-rectangle should be snipped.
Return value:
  0: Normal.
  1: An edge had < sgmnt_min_n points, so angle was set to zero;
     top-finder succeeded.
  2: An edge had < sgmnt_min_n points, so angle was set to zero;
     top-finder failed, so centroid was used.
  3: A call of linreg returned abnormally, so angle was set to zero;
     top-finder succeeded.
  4: A call of linreg returned abnormally, so angle was set to zero;
     top-finder failed, so centroid was used.
  5: An edge-line was slopally anomalous, so angle was set to zero;
     top-finder succeeded.
  6: An edge-line was slopally anomalous, so angle was set to zero;
     top-finder failed, so centroid was used.
  7: The above hurdles were passed and angle was computed, but
     top-finder failed, so centroid was used. */

int sgmnt_edges(unsigned char *fg, const int fgw, const int fgh,
                const int fgw_max, const int fgh_max,
                const int x_centroid, const int y_centroid,
                int *xc, int *yc, float *radians, const int sgmnt_min_n,
                const float sgmnt_slope_thresh, const int sgmnt_hist_thresh,
                const int sfgw, const int sfgh)
{
  int anint, hfgw, hfgh, x, y, prev_x, prev_y, midy_x, midx_y, n, i,
    *xsp, *ysp, k;
  static int f = 1, *xs, *ys;
  float a, b, slope[3], slope_acc, slope_avg;
#ifdef GRPHCS
  float as[3], bs[3];
#endif

  if(f) {
    f = 0;
    xs = (int *)malloc_ch(anint = max(fgw_max, fgh_max) * sizeof(int));
    ys = (int *)malloc_ch(anint);
  }
  hfgw = fgw / 2;
  hfgh = fgh / 2;
  slope_acc = 0.;

  /* find left edge of foreground */
  xsp = xs;
  ysp = ys;
  if(scan_row_from_left_foundtrue(fg, fgw, hfgh, &x)) {
    *xsp++ = x;
    *ysp++ = hfgh;
    for(prev_x = midy_x = x, y = hfgh - 1; y >= 0; y--) {
      if(!scan_row_from_left_foundtrue(fg, fgw, y, &x) ||
        abs(x - prev_x) > 1)
	break;
      *xsp++ = prev_x = x;
      *ysp++ = y;
    }
    for(prev_x = midy_x, y = hfgh + 1; y < fgh; y++) {
      if(!scan_row_from_left_foundtrue(fg, fgw, y, &x) ||
        abs(x - prev_x) > 1)
	break;
      *xsp++ = prev_x = x;
      *ysp++ = y;
    }
  }
  n = xsp - xs;
#ifdef GRPHCS
  grphcs_sgmntwork_edge(xs, ys, n, 0);
#endif
  if(n < sgmnt_min_n) {
    k = sgmnt_decide_location(fg, fgw, fgh, x_centroid, y_centroid,
      *radians = 0., xc, yc, sgmnt_hist_thresh, sfgw, sfgh);
    return ++k;
  }
  if(linreg(ys, xs, n, &a, &b)) {
    k = sgmnt_decide_location(fg, fgw, fgh, x_centroid, y_centroid,
      *radians = 0., xc, yc, sgmnt_hist_thresh, sfgw, sfgh);
    return k + 3;
  }
  slope_acc += (slope[0] = a);
#ifdef GRPHCS
  as[0] = a;
  bs[0] = b;
#endif

  /* find top edge of foreground */
  xsp = xs;
  ysp = ys;
  if(scan_col_from_top_foundtrue(fg, fgw, fgh, hfgw, &y)) {
    *xsp++ = hfgw;
    *ysp++ = y;
    for(prev_y = midx_y = y, x = hfgw - 1; x >= 0; x--) {
      if(!scan_col_from_top_foundtrue(fg, fgw, fgh, x, &y) ||
        abs(y - prev_y) > 1)
	break;
      *xsp++ = x;
      *ysp++ = prev_y = y;
    }
    for(prev_y = midx_y, x = hfgw + 1; x < fgw; x++) {
      if(!scan_col_from_top_foundtrue(fg, fgw, fgh, x, &y) ||
        abs(y - prev_y) > 1)
	break;
      *xsp++ = x;
      *ysp++ = prev_y = y;
    }
  }
  n = xsp - xs;
#ifdef GRPHCS
  grphcs_sgmntwork_edge(xs, ys, n, 1);
#endif
  if(n < sgmnt_min_n) {
    k = sgmnt_decide_location(fg, fgw, fgh, x_centroid, y_centroid,
      *radians = 0., xc, yc, sgmnt_hist_thresh, sfgw, sfgh);
    return ++k;
  }
  if(linreg(xs, ys, n, &a, &b)) {
    k = sgmnt_decide_location(fg, fgw, fgh, x_centroid, y_centroid,
      *radians = 0., xc, yc, sgmnt_hist_thresh, sfgw, sfgh);
    return k + 3;
  }
  slope_acc += (slope[1] = -a);
#ifdef GRPHCS
  as[1] = a;
  bs[1] = b;
#endif

  /* find right edge of foreground */
  xsp = xs;
  ysp = ys;
  if(scan_row_from_right_foundtrue(fg, fgw, hfgh, &x)) {
    *xsp++ = x;
    *ysp++ = hfgh;
    for(prev_x = midy_x = x, y = hfgh - 1; y >= 0; y--) {
      if(!scan_row_from_right_foundtrue(fg, fgw, y, &x) ||
        abs(x - prev_x) > 1)
	break;
      *xsp++ = prev_x = x;
      *ysp++ = y;
    }
    for(prev_x = midy_x, y = hfgh + 1; y < fgh; y++) {
      if(!scan_row_from_right_foundtrue(fg, fgw, y, &x) ||
        abs(x - prev_x) > 1)
	break;
      *xsp++ = prev_x = x;
      *ysp++ = y;
    }
  }
  n = xsp - xs;
#ifdef GRPHCS
  grphcs_sgmntwork_edge(xs, ys, n, 2);
#endif
  if(n < sgmnt_min_n) {
    k = sgmnt_decide_location(fg, fgw, fgh, x_centroid, y_centroid,
      *radians = 0., xc, yc, sgmnt_hist_thresh, sfgw, sfgh);
    return ++k;
  }
  if(linreg(ys, xs, n, &a, &b)) {
    k = sgmnt_decide_location(fg, fgw, fgh, x_centroid, y_centroid,
      *radians = 0., xc, yc, sgmnt_hist_thresh, sfgw, sfgh);
    return k + 3;
  }
  slope_acc += (slope[2] = a);
#ifdef GRPHCS
  as[2] = a;
  bs[2] = b;
#endif

  /* Check whether each "slope" is close to the "average slope" */
  for(i = 0, slope_avg = slope_acc / 3; i < 3; i++)
    if(fabs((double)(slope[i] - slope_avg)) > sgmnt_slope_thresh) {
      k = sgmnt_decide_location(fg, fgw, fgh, x_centroid,
        y_centroid, *radians = 0., xc, yc, sgmnt_hist_thresh, sfgw, sfgh);
      return k + 5;
    }
#ifdef GRPHCS
  grphcs_sgmntwork_lines(as, bs);
#endif

  k = sgmnt_decide_location(fg, fgw, fgh, x_centroid,
    y_centroid, *radians = atan((double)slope_avg), xc, yc,
    sgmnt_hist_thresh, sfgw, sfgh);
  return (k ? 7 : 0);
}

/********************************************************************/

/* Scans a row of fg from left, trying to find a true pixel */

int scan_row_from_left_foundtrue(unsigned char *fg, const int fgw,
                                 const int y, int *x)
{
  unsigned char *p, *ps, *pe;

  for(pe = (ps = p = fg + y * fgw) + fgw; p < pe && !*p; p++);
  if(p == pe)
    return 0;
  *x = p - ps;
  return 1;
}

/********************************************************************/

/* Scans a row of fg from right, trying to find a true pixel */

int scan_row_from_right_foundtrue(unsigned char *fg, const int fgw,
                                  const int y, int *x)
{
  unsigned char *p, *ps, *pe;

  for(ps = p = (pe = fg + y * fgw - 1) + fgw; p > pe && !*p; p--);
  if(p == pe)
    return 0;
  *x = fgw - 1 + (p - ps);
  return 1;
}

/********************************************************************/

/* Scans a column of fg from top, trying to find a true pixel */

int scan_col_from_top_foundtrue(unsigned char *fg, const int fgw, const int fgh,
                                const int x, int *y)
{
  unsigned char *p, *pe;

  for(pe = (p = fg + x) + fgw * fgh; p < pe && !*p; p += fgw);
  if(p == pe)
    return 0;
  *y = (p - fg) / fgw;
  return 1;
}

/********************************************************************/

/* Decides the location from which the possibly rotated snip-rectangle
should be taken, given the cleaned-up foreground, its centroid, and
the fingerprint angle.  The algorithm is to scan downward along a
column histogram tilted by the angle, stopping at the first bin of
value >= sgmnt_hist_thresh whose tilted-row fits entirely on the
foreground raster.  Return value:
  0: Normal; xc and yc return the location at which the snip-rectangle
       should be centered.
  1: Search hit bottom without finding a satisfactory tilted-row, so
       routine just says to use the centroid as the snip-rectangle
       center.
*/

int sgmnt_decide_location(unsigned char *fg, const int fgw, const int fgh,
                          const int x_centroid, const int y_centroid,
                          const float radians, int *xc, int *yc,
                          const int sgmnt_hist_thresh, const int sfgw,
                          const int sfgh)
{
  int x, yow, x2, y2, ytop, ytop_e;
  float c, s;

  c = cos(radians);
  s = sin(radians);
  for(ytop_e = (ytop = -y_centroid) + fgh; ytop < ytop_e; ytop++) {
    for(x = -(sfgw/2), yow = 0; x < sfgw/2; x++) {
      x2 = c * x + s * ytop + x_centroid + .5;
      y2 = -s * x + c * ytop + y_centroid + .5;
      if(x2 < 0 || x2 >= fgw || y2 < 0 || y2 >= fgh) {
	yow = 0;
	break;
      }
      if(*(fg + y2 * fgw + x2))
	yow++;
    }
    if(yow >= sgmnt_hist_thresh) {
      *xc = HWS * (s * (ytop + (sfgh/2)) + x_centroid) + .5;
      *yc = HWS * (c * (ytop + (sfgh/2)) + y_centroid) + .5;
#ifdef GRPHCS
      grphcs_sgmntwork_box(fg, c, s, x_centroid, y_centroid, ytop, sfgw, sfgh);
#endif
      return 0;
    }
  }
  *xc = HWS * x_centroid + .5;
  *yc = HWS * y_centroid + .5;
  return 1;
}

/********************************************************************/

/* Snips (interpolating pixel values from four nearest neighbors)
from the original raster the decided-upon segmented raster, a
rectangle centered at (xc, yc) and rotated by radians,
and snips from the original-raster foreground map a corresponding
segmented-raster foreground map */

void sgmnt_snip_interp(unsigned char *origras, const int w, const int h,
                       const int xc, const int yc, const float radians,
                       unsigned char ***segras, const int iw, const int ih,
                       unsigned char *fg, const int fgw, const int fgh,
                       unsigned char ***segras_fg, int *sfgw, int *sfgh)
{
   double c, s, fx, fy, sfx, sfy;
   int ix, iy, x, y;
   float hw, hh;
   float lx, ly, hx, hy;
   float ilx, ily, ihx, ihy;
   float dlx, dly, dhx, dhy;
   float d1, d2, d3, d4;
   float w1, w2, w3, w4, sw;
   float sq2;
   int p1, p2, p3, p4;
   int tfgw, tfgh;
   unsigned char **sras, **sras_fg;

   malloc_dbl_uchar(&sras, ih, iw, "sgmnt_snip segras");
   *segras = sras;

   double x20 = 2.0;
   sq2 = sqrt(x20);

   hw = (float)iw / 2.0;
   hh = (float)ih / 2.0;

   c = cos(radians);
   s = sin(radians);

   sfx = (xc - (hw * c) - (hh * s));
   sfy = (yc + (hw * s) - (hh * c));

   for(y = 0; y < ih; y++, sfx += s, sfy += c) {
      for(x = 0, fx = sfx, fy = sfy; x < iw; x++, fx += c, fy -= s) {
         ix = sround(fx);
         iy = sround(fy);

         if((ix >= 0) && (ix < w) && (iy >= 0) && (iy < h)) {
            lx = floor(fx);
            ly = floor(fy);
            hx = ceil(fx);
            hy = ceil(fy);
            ilx = (int)lx;
            ily = (int)ly;
            ihx = (int)hx;
            ihy = (int)hy;
            dlx = pow(fx-lx, 2.0);
            dly = pow(fy-ly, 2.0);
            dhx = pow(hx-fx, 2.0);
            dhy = pow(hy-fy, 2.0);
            d1 = sqrt(dlx+dly);
            d2 = sqrt(dhx+dly);
            d3 = sqrt(dlx+dhy);
            d4 = sqrt(dhx+dhy);
            w1 = 1.0 - (d1/sq2);
            w2 = 1.0 - (d2/sq2);
            w3 = 1.0 - (d3/sq2);
            w4 = 1.0 - (d4/sq2);
            sw = w1+w2+w3+w4;


            p1 = ilx + ily*w;
            p2 = ihx + ily*w;
            p3 = ilx + ihy*w;
            p4 = ihx + ihy*w;
            sras[y][x] = sround((w1*(float)origras[p1] + w2*(float)origras[p2] +
                           w3*(float)origras[p3] + w4*(float)origras[p4])/sw);
         }
         else
            sras[y][x] = 255;
      }
   }

   *sfgw = (tfgw = iw/HWS);
   *sfgh = (tfgh = ih/HWS);
   malloc_dbl_uchar(&sras_fg, tfgh, tfgw, "sgmnt_snip segras_fg");
   *segras_fg = sras_fg;
   hw = tfgw/2;
   hh = tfgh/2;

   c = cos(radians);
   s = sin(radians);

   sfx = (xc/(iw/tfgw) - (hw * c) - (hh * s));
   sfy = (yc/(ih/tfgh) + (hw * s) - (hh * c));

   for(y = 0; y < tfgh; y++, sfx += s, sfy += c) {
      for(x = 0, fx = sfx, fy = sfy; x < tfgw; x++, fx += c, fy -= s) {
         ix = sround(fx);
         iy = sround(fy);
         sras_fg[y][x] = ((ix >= 0) && (ix < fgw) && (iy >= 0) && (iy < fgh))?fg[ix+iy*fgw]:0;
      }
   }
}

/********************************************************************/

/* Snips from the original raster the decided-upon segmented raster, a
wxh-pixel rectangle centered at (xc, yc) and rotated by radians,
and snips from the original-raster foreground map a corresponding
segmented-raster foreground map */

void sgmnt_snip(unsigned char *origras, const int w, const int h, const int xc,
                const int yc, const float radians, unsigned char ***segras,
                const int iw, const int ih, unsigned char *fg, const int fgw,
                const int fgh, unsigned char ***segras_fg, int *sfgw, int *sfgh)
{
  int x, y, ixf, iyf;
  double c, s, xfs, yfs, xf, yf;
  int hw, hh;
  int tfgw, tfgh;
  unsigned char **sras, **sras_fg;

  malloc_dbl_uchar(&sras, ih, iw, "sgmnt_snip segras");
  *segras = sras;

  hw = iw/2;
  hh = ih/2;

  c = cos(radians);
  s = sin(radians);
  for(y = 0, xfs = -hw * c - hh * s + xc +.5, yfs = hw * s -
    hh * c + yc + .5; y < ih; y++, xfs += s, yfs += c)
    for(x = 0, xf = xfs, yf = yfs; x < iw; x++, xf += c, yf -= s)
      sras[y][x] = (((ixf = xf) >= 0 && ixf < w && (iyf = yf) >= 0
        && iyf < h) ? *(origras + iyf * w + ixf) : 255);

  *sfgw = tfgw = iw/HWS;
  *sfgh = tfgh = ih/HWS;
  malloc_dbl_uchar(&sras_fg, tfgh, tfgw, "sgmnt_snip segras_fg");
  *segras_fg = sras_fg;
  hw = tfgw/2;
  hh = tfgh/2;
  for(y = 0, xfs = -hw * c - hh * s + xc / HWS +.5, yfs = hw * s -
    hh * c + yc / HWS + .5; y < tfgh; y++, xfs += s, yfs += c)
    for(x = 0, xf = xfs, yf = yfs; x < tfgw; x++, xf += c, yf -= s) {
      if((ixf = xf) >= 0 && ixf < fgw && (iyf = yf) >= 0 &&
        iyf < fgh && *(fg + iyf * fgw + ixf))
	sras_fg[y][x] = 1;
      else
	sras_fg[y][x] = 0;
    }
}

/********************************************************************/
