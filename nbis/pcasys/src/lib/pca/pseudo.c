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

      FILE:    PSEUDO.C
      AUTHORS: G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG
      UPDATED: 02/04/2009 by Joseph C. Konczal - ifdef around decl of pw and ph

      ROUTINES:
#cat: pseudo - Traces pseudoridges, searching for a concave-upward shape
               to determine if fingerprint is a whorl.
#cat: print_has_conup - Produces a pseudoridge by approximately traveling
#cat:                   along the ridge flow.
#cat: path_has_conup - Tests a pseudoridge path to find out whether it
#cat:                  has a conup shape.
#cat: lobe_is_conup - Tests a lobe (maximal constant-turn-polarity subsequence
#cat:                 of a path) to find out whether it is a conup.
#cat: pseudo_avrors2_smooth_iter - Performs an interation of smoothing on the
#cat:                              finely-spaced orientation array.
#cat: pseudo_avrors2_xys2has - Transforms orientations from (x,y) pairs to
#cat:                          degrees.

***********************************************************************/

/* Traces pseudoridges, searching for a concave-upward shape (conup).
A conup is defined to be a "lobe" (maximal subsequence of
same-polarity turns) whose direction at its "vertex", i.e. point of
sharpest turning (defined as the average of the step-directions on
either side of this corner) is within pseudo_prs->max_tilt degrees of
horizontal with the appropriate sense for the conup to be nearer to
upright than to upside-down, i.e. if the lobe is turning c-cl (cl),
then the sense has to be rightward (leftward)), with the additional
requirement that, on each side of the vertex separately, the
cumulative turn be >= pseudo_prs->min_side_turn degrees.  But the only
turns allowed to compete for sharpest are those whose following
direction has the appropriate sense for a bottom of a conup of the
lobe's turn polarity.  Returns 1 (0) if it finds (does not find) a
conup. */

#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

int pseudo(unsigned char **cfg, const int cw, const int ch, float **avrors2_x,
         float **avrors2_y, const int aw, const int ah, PSEUDO_PRS *pseudo_prs)
{
  char str[200], **bad;
  int i, j, ii, jj, found_conup;
  float afloat, **dg, **us, **vs;
  double adouble;
#ifdef GRPHCS
  int pw, ph;
#endif

  if(isverbose()) {
    printf("  trace pseudoridges, searching for concave-upward... ");
    fflush(stdout);
  }
  if(!(0 <= pseudo_prs->initi_s && pseudo_prs->initi_s <=
    pseudo_prs->initi_e && pseudo_prs->initi_e <= (ah-1) &&
    0 <= pseudo_prs->initj_s && pseudo_prs->initj_s <=
    pseudo_prs->initj_e && pseudo_prs->initj_e <= (aw-1))) {
    sprintf(str, "Must have \n0 <= pseudo_prs->initi_s <= \
pseudo_prs->initi_e <= (ah-1) and \n0 <= pseudo_prs->initj_s <= \
pseudo_prs->initj_e <= (aw-1);\n\ncurrently values are:\n\
pseudo_prs->initi_s = %d\npseudo_prs->initi_e = %d\n\
pseudo_prs->initj_s = %d\npseudo_prs->initj_e = %d\n\
aw = %d, ah = %d\nwhich violates these restrictions\n",
      pseudo_prs->initi_s, pseudo_prs->initi_e,
      pseudo_prs->initj_s, pseudo_prs->initj_e, aw, ah);
    fatalerr("pseudo", str, NULL);
  }
  i = ch-2;
  ii = ch-1;
  for(jj = 0; jj < cw; jj++) {
    cfg[0][jj] = 0;
    cfg[1][jj] = 0;
    cfg[i][jj] = 0;
    cfg[ii][jj] = 0;
  }
  j = cw-2;
  jj = cw-1;
  for(ii = 0; ii < ch; ii++) {
    cfg[ii][0] = 0;
    cfg[ii][1] = 0;
    cfg[ii][j] = 0;
    cfg[ii][jj] = 0;
  }
#ifdef GRPHCS  
  pw = (aw+2)*HWS;
  ph = (ah+2)*HWS;
  grphcs_pseudo_cfgyow(cfg, cw, ch, pw, ph);
#endif
  jj = aw-1;
  for(ii = 0; ii < ah; ii++) {
    avrors2_x[ii][0] = 0.;
    avrors2_y[ii][0] = 0.;
    avrors2_x[ii][jj] = 0.;
    avrors2_y[ii][jj] = 0.;
  }
  ii = ah-1;
  for(jj = 0; jj < aw; jj++) {
    avrors2_x[ii][jj] = 0.;
    avrors2_y[ii][jj] = 0.;
  }
  for(ii = 0; ii < ah; ii++)
    for(jj = 0; jj < aw; jj++)
      if(!cfg[ii + 1][jj + 1] ||
	avrors2_x[ii][jj] * avrors2_x[ii][jj] +
        avrors2_y[ii][jj] * avrors2_y[ii][jj] < pseudo_prs->slthresh0)
	avrors2_x[ii][jj] = avrors2_y[ii][jj] = 0.;
  if(pseudo_prs->nsmooth > 0)
    for(j = 0, afloat = (1. - pseudo_prs->smooth_cwt) / 4.;
      j < pseudo_prs->nsmooth; j++)
      pseudo_avrors2_smooth_iter(avrors2_x, avrors2_y, aw, ah,
        pseudo_prs->smooth_cwt, afloat);

  malloc_dbl_flt(&dg, ah, aw, "pseudo dg");
  malloc_dbl_flt(&us, ah, aw, "pseudo us");
  malloc_dbl_flt(&vs, ah, aw, "pseudo vs");
  malloc_dbl_char(&bad, ah, aw, "pseudo bad");

  pseudo_avrors2_xys2has(avrors2_x, avrors2_y, dg, aw, ah);
  for(ii = 0; ii < ah; ii++)
    for(jj = 0; jj < aw; jj++)
      if(avrors2_x[ii][jj] * avrors2_x[ii][jj] +
        avrors2_y[ii][jj] * avrors2_y[ii][jj] < pseudo_prs->slthresh1
        || !cfg[ii + 1][jj + 1])
	bad[ii][jj] = 1;
      else {
	bad[ii][jj] = 0;
	adouble = dg[ii][jj] * D2R;
	us[ii][jj] = pseudo_prs->stepsize * cos(adouble);
	vs[ii][jj] = pseudo_prs->stepsize * (-sin(adouble));
      }
  found_conup = print_has_conup(us, vs, pseudo_prs->maxturn,
    pseudo_prs->max_tilt, pseudo_prs->min_side_turn,
    pseudo_prs->initi_s, pseudo_prs->initi_e, pseudo_prs->initj_s,
    pseudo_prs->initj_e, pseudo_prs->maxsteps_eachdir, bad, dg, aw, ah
#ifdef GRPHCS
    , pw, ph
#endif
    );
  if(isverbose())
    printf("%s\n", found_conup ? "found" : "didn't find");
#ifdef GRPHCS
  found_conup ? grphcs_foundconup_sleep() : grphcs_noconup_sleep();
#endif

  free_dbl_flt(dg, ah);
  free_dbl_flt(us, ah);
  free_dbl_flt(vs, ah);
  free_dbl_char(bad, ah);

  return found_conup;
}

/*********************************************************************/

/* For each of a set of initial points, produces a pseudoridge by
approximately traveling along the ridge flow, starting in both
possible directions from the initial point and attaching the two
resulting trajectories end to end, in effect.  Searches each
pseudoridge for a concave-upward lobe (conup). */

int print_has_conup(float **us, float **vs, const int maxturn,
         const int max_tilt, const int min_side_turn, const int initi_s,
         const int initi_e, const int initj_s, const int initj_e,
         const int maxsteps_eachdir, char **bad, float **dg, const int w,
         const int h
#ifdef GRPHCS
         , const int pw, const int ph
#endif
         )
{
  int anint, initi, initj, i, ii = 0, j, k = 0, k2 = 0;
  int idir, flip, previ, prevj, prevflip;
  static int f = 1;
  float u, v, ri = 0.0, rj = 0.0, aturn, adg, newdg;
  static float *turn, *flowdg;
#ifdef GRPHCS
  static float *riarr, *rjarr;
#endif

  if(f) {
    f = 0;
    turn = (float *)malloc_ch(anint = 2 * maxsteps_eachdir *
      sizeof(float));
    flowdg = (float *)malloc_ch(anint);
#ifdef GRPHCS
    riarr = (float *)malloc_ch(anint += 2 * sizeof(float));
    rjarr = (float *)malloc_ch(anint);
#endif
  }
  for(initi = initi_s; initi <= initi_e; initi++)
    for(initj = initj_s; initj <= initj_e; initj++) {
      if(bad[initi][initj])
	continue;
      for(idir = 1; idir <= 2; idir++) {
	adg = dg[initi][initj];
	if(idir == 1) {
	  u = .5 * us[initi][initj];
	  v = .5 * vs[initi][initj];
	  previ = initi;
	  prevj = initj;
	  prevflip = 0;
	}
	else {
#ifdef GRPHCS
	  riarr[--ii] = ri;
	  rjarr[ii] = rj;
#endif
          k2 = k;
	  u = -.5 * us[initi][initj];
	  v = -.5 * vs[initi][initj];
	  prevflip = 1;
	}
	for(ri = initi, rj = initj, k = 0; k < maxsteps_eachdir; k++) {
	  ri += v;
	  rj += u;
	  i = (ri >= 0. ? ri + .5 : ri - .5);
	  j = (rj >= 0. ? rj + .5 : rj - .5);
	  if(i < 0 || i >= h || j < 0 || j >= w || bad[i][j])
	    break;
	  if((aturn = (newdg = dg[i][j]) - adg) > 90.) {
	    flip = !prevflip;
	    aturn -= 180.;
	  }
	  else if(aturn < -90.) {
	    flip = !prevflip;
	    aturn += 180.;
	  }
	  else
	    flip = prevflip;
	  if(fabs((double)aturn) > maxturn)
	    break;
	  if(idir == 1) {
	    turn[ii = maxsteps_eachdir - 1 - k] = -aturn;
	    flowdg[ii] = (prevflip ? adg : adg + 180.);
#ifdef GRPHCS
	    riarr[++ii] = ri;
	    rjarr[ii] = rj;
#endif
	    previ = i;
	    prevj = j;
	  }
	  else {
	    turn[ii = maxsteps_eachdir + k] = aturn;
	    flowdg[ii] = (flip ? newdg + 180. : newdg);
#ifdef GRPHCS
	    riarr[++ii] = ri;
	    rjarr[ii] = rj;
#endif
	  }
	  adg = newdg;
	  if((prevflip = flip)) {
	    u = -us[i][j];
	    v = -vs[i][j];
	  }
	  else {
	    u = us[i][j];
	    v = vs[i][j];
	  }
	}
	if(k == maxsteps_eachdir) {
	  ri += v;
	  rj += u;
	}
      }
#ifdef GRPHCS
      riarr[++ii] = ri;
      rjarr[ii] = rj;
#endif
      anint = maxsteps_eachdir - k2;
#ifdef GRPHCS
      grphcs_pseudo_pseudoridge(riarr + anint, rjarr + anint,
        k2 + k + 2, 0, pw, ph);
#endif
      if(path_has_conup(
#ifdef GRPHCS
                      pw, ph, riarr + anint, rjarr + anint,
#endif
                      turn + anint, flowdg + anint, k2 + k, max_tilt,
        min_side_turn))
	return 1;
    }
  return 0;
}

/*********************************************************************/

/* Tests a path to find out whether it has a conup. */

int path_has_conup(
#ifdef GRPHCS
         const int pw, const int ph, float *riarr, float *rjarr,
#endif
         float *turn, float *flowdg, const int n, const int max_tilt,
         const int min_side_turn)
{
  int ccl, istart, i, shp_i;
  float aturn, shp;

  if(n < 3)
    return 0;
  for(ccl = (turn[0] >= 0.), shp = -1., shp_i = -1, istart = 0, i = 1;
    i < n; i++) {
    if(((aturn = turn[i]) >= 0.) == ccl) {
      if(ccl) {
	if((flowdg[i] < 90. || flowdg[i] > 270.) && aturn > shp) {
	  shp = aturn;
	  shp_i = i;
	}
      }
      else if((flowdg[i] > 90. && flowdg[i] < 270.) && -aturn > shp) {
	shp = -aturn;
	shp_i = i;
      }
    }
    else {
      if(lobe_is_conup(
#ifdef GRPHCS
                     pw, ph, riarr + istart, rjarr + istart,
#endif
                     turn + istart, flowdg + istart, i - istart, ccl,
        (shp_i == -1), shp_i - istart, max_tilt, min_side_turn))
	return 1;
      ccl = !ccl;
      shp = -1.;
      shp_i = -1;
      istart = i;
    }
  }
  return lobe_is_conup(
#ifdef GRPHCS
                     pw, ph, riarr + istart, rjarr + istart,
#endif

                     turn + istart, flowdg + istart, i - istart, ccl,
           (shp_i == -1), shp_i - istart, max_tilt, min_side_turn);
}

/*********************************************************************/

/* Tests a lobe (maximal constant-turn-polarity subsequence of a path)
to find out whether it is a conup.  If graphical, then calls a routine
which draws the lobe (as a bolder line), if it is a conup. */

int lobe_is_conup(
#ifdef GRPHCS
         const int pw, const int ph, float *riarr, float *rjarr,
#endif
         float *turn, float *flowdg, const int n, const int ccl,
         const int sien1, const int shp_i, const int max_tilt,
         const int min_side_turn)
{
  int i;
  float vtx_flowdg, acc;

  if(sien1 || (n < 3))
    return 0;
  vtx_flowdg = flowdg[shp_i] - .5 * turn[shp_i];
  if(ccl) {
    if(vtx_flowdg < 0.)
      vtx_flowdg += 360.;
    if(vtx_flowdg > max_tilt && 360. - vtx_flowdg > max_tilt)
      return 0;
    for(acc = 0., i = 0; i < shp_i; i++)
      acc += turn[i];
    if(acc < min_side_turn)
      return 0;
    for(acc = 0., i++; i < n; i++)
      acc += turn[i];
  }
  else {
    if(vtx_flowdg > 360.)
      vtx_flowdg -= 360.;
    if(vtx_flowdg < 180 - max_tilt || vtx_flowdg > 180 + max_tilt)
      return 0;
    for(acc = 0., i = 0; i < shp_i; i++)
      acc -= turn[i];
    if(acc < min_side_turn)
      return 0;
    for(acc = 0., i++; i < n; i++)
      acc -= turn[i];
  }
  if(acc >= min_side_turn) {
#ifdef GRPHCS
    grphcs_pseudo_pseudoridge(riarr, rjarr, n + 2, 1, pw, ph);
#endif
    return 1;
  }
  return 0;
}

/*********************************************************************/

/* Performs an interation of smoothing on the finely-spaced
orientation array (x and y), consisting of replacing each vector by a
weighted average of itself and its four neighbors, with itself having
weight cwt (center weight) and the neighbors dividing the remaining
weight equally among themselves.  The weights add up to 1. */

void pseudo_avrors2_smooth_iter(float **x, float **y, const int w,
          const int h, const float cwt, const float a)
{
  int i, j;
  float **newx, **newy;
  int w1, w2, h1, h2;

  malloc_dbl_flt(&newx, h, w, "pseudo_avrors2_smooth_iter newx");
  malloc_dbl_flt(&newy, h, w, "pseudo_avrors2_smooth_iter newy");

  w1 = w-1;
  w2 = w-2;
  h1 = h-1;
  h2 = h-2;

  /* corners */
  newx[0][0] = cwt*x[0][0]+a*(x[0][1]+x[1][0]);
  newy[0][0] = cwt*y[0][0]+a*(y[0][1]+y[1][0]);
  newx[0][w1] = cwt*x[0][w1]+a*(x[0][w2]+x[1][w1]);
  newy[0][w1] = cwt*y[0][w1]+a*(y[0][w2]+y[1][w1]);
  newx[h1][0] = cwt*x[h1][0]+a*(x[h1][1]+x[h2][0]);
  newy[h1][0] = cwt*y[h1][0]+a*(y[h1][1]+y[h2][0]);
  newx[h1][w1] = cwt*x[h1][w1]+a*(x[h1][w2]+x[h2][w1]);
  newy[h1][w1] = cwt*y[h1][w1]+a*(y[h1][w2]+y[h2][w1]);

  /* first and last rows and cols, except corners */
  for(j = 1; j <= w2; j++) {
    newx[0][j] = cwt*x[0][j]+a*(x[1][j]+x[0][j-1]+x[0][j+1]);
    newy[0][j] = cwt*y[0][j]+a*(y[1][j]+y[0][j-1]+y[0][j+1]);
    newx[h1][j] = cwt*x[h1][j]+a*(x[h2][j]+x[h1][j-1]+x[h1][j+1]);
    newy[h1][j] = cwt*y[h1][j]+a*(y[h2][j]+y[h1][j-1]+y[h1][j+1]);
  }
  for(i = 1; i <= h2; i++) {
    newx[i][0] = cwt*x[i][0]+a*(x[i][1]+x[i-1][0]+x[i+1][0]);
    newy[i][0] = cwt*y[i][0]+a*(y[i][1]+y[i-1][0]+y[i+1][0]);
    newx[i][w1] = cwt*x[i][w1]+a*(x[i][w2]+x[i-1][w1]+x[i+1][w1]);
    newy[i][w1] = cwt*y[i][w1]+a*(y[i][w2]+y[i-1][w1]+y[i+1][w1]);
  }

  /* the rest (middle) */
  for(i = 1; i <= h2; i++)
    for(j = 1; j <= w2; j++) {
      newx[i][j] = cwt*x[i][j]+a*(x[i-1][j]+x[i+1][j]+x[i][j-1]
        + x[i][j+1]);
      newy[i][j] = cwt*y[i][j]+a*(y[i-1][j]+y[i+1][j]+y[i][j-1]
        + y[i][j+1]);
      }

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++) {
      x[i][j] = newx[i][j];
      y[i][j] = newy[i][j];
    }

  free_dbl_flt(newx, h);
  free_dbl_flt(newy, h);
}

/*******************************************************************/

/* Transforms orientations from (x,y) pairs to degrees. */

void pseudo_avrors2_xys2has(float **x, float **y, float **dg, const int w,
          const int h)
{
  static float a = 28.6479;  /* 90./pi */
  int i, j;

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++) {
      if(x[i][j] > 0.) {
	if(y[i][j] >= 0.)
	  dg[i][j] = a * atan((double)(y[i][j] / x[i][j]));
	else
	  dg[i][j] = a * atan((double)(y[i][j] / x[i][j])) + 180.;
      }
      else if(x[i][j] == 0.) {
	if(y[i][j] > 0.)
	  dg[i][j] = 45.;
	else if(y[i][j] < 0.)
	  dg[i][j] = 135.;
	else
	  dg[i][j] = 0.;
      }
      else
	dg[i][j] = a * atan((double)(y[i][j] / x[i][j])) + 90.;
    }
}	

/*******************************************************************/
