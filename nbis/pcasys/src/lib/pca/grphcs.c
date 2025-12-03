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
      LIBRARY: GRPHCS - PCASYS graphics utils

      FILE:    GRPHCS.C
      AUTHORS: Craig Watson
               cwatson@nist.gov
               G. T. Candela
      DATE:    10/01/2000
      UPDATED: 04/20/2005 by MDG

      Graphical routines (using X Windows) which display various stages
      of the processing (how segmentor decides what piece to snip out;
      raster-enhancing filter; etc.)  These routines assume that the
      screen is large enough to display an original raster in its full
      size (832 (width) by 768 (height) pixels); if screen is too small,
      this code may need to be modified.

      NOTE: There are many static variables used in these routines
            and the routines expect to be called in a certain order
            or "small" and "big" windows do not get destroy and
            created in the correct order.

      ROUTINES:
#cat: grphcs_init - Initializes the grphics mode screens.
#cat: grphcs_startwindow - Creates a window whose top-left corner is at
#cat:                      (x,0), of width w by height h.
#cat: grphcs_origras - Displays a fingerprint image
#cat: grphcs_segras - Displays a segmented fingerprint image.
#cat: grphcs_enhnc_outsquare - Updates a piece of the enhanced image.
#cat: grphcs_enhnc_sleep - Sleeps based on the sleeps.enhnc time.
#cat: grphcs_foundconup_sleep - Sleeps based on the sleeps.foundconup time.
#cat: grphcs_noconup_sleep - Sleeps based on the sleeps.noconup time.
#cat: grphcs_bars - Displays the orientation bars.
#cat: grphcs_xy_to_dmt - Converts orientation vector to an angle in degrees
#cat:                    and a magnitude, and finds the top magnitude.
#cat: grphcs_dmt_to_bars - Converts an array of angles and magnitudes
#cat:                      to an image.
#cat: grphcs_core_medcore - Displays a plus sign for core location found
#cat:                       by r92 and displays a plus sign surrounded by
#cat:                       square for the median core location.
#cat: grphcs_normacs - Displays the NN activations as a bar graph.
#cat: letter_adjust_xy - Adjust the (ALRSTW) Class letter x,y locations.
#cat: grphcs_sgmntwork_init - Initializes the segmentor-work window to
#cat:                         the 7 images showing the stages of segmentation.
#cat: grphcs_sgmntwork_fg - Stores provided foreground image in
#cat:                       the segmentor-work window.
#cat: grphcs_sgmntwork_edge - Stores the provided edge information
#cat:                         in the segmentor-work window.
#cat: grphcs_sgmntwork_lines - Stores the straight lines the segmentor
#cat:                          fit to the foreground-edges in the
#cat:                          segmentor-work window.
#cat: grphcs_sgmntwork_box - Stores, in the segmentor-work window, the box
#cat:                        showing the region of the foreground that will
#cat:                        be segmented.
#cat: grphcs_sgmntwork_finish - Displays the segmentor-work picture in its
#cat:                           window, to the right of the fingerprint
#cat:                           window.
#cat: grphcs_pseudo_cfgyow - Displays foreground image on which pseudo-
#cat:                        ridges will be displayed.
#cat: grphcs_pseudo_cfgyow_reput - Rereshes the screen when drawing
#cat:                              pseudoridges.
#cat: grphcs_pseudo_pseudoridge - Displays a pseudoridge or the final
#cat:                             concave-upward lobe if found.
#cat: grphcs_titlepage - Displays the title page.
#cat: grphcs_featvec - Displays a bar graph of the feature vectors.
#cat: grphcs_lastdisp - Displays the the actual class, found class
#cat:                   the confidence.
#cat: ImageBit8ToBit24Unit32 - Converts from 8 bit colormap to 24 bit
#cat:                          colormap (True Color).
#cat: xcreateimage - converts to True Color image before displaying
#cat:                if using a True Color Map.

***********************************************************************/

#include <usebsd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <event.h>
#include <ihead.h>
#include <img_io.h>
#include <pca.h>
#include <little.h>
#include <grphcs.h>  
#include <gr_cm.h>  

/* NIST logo */
static int nist_w = 70, nist_h = 22;
static char nist[] = "\
--###----------------####--####-----##################################\
-#####---------------####--####----###################################\
#######--------------####--####---####################################\
########-------------####--####--#####################################\
#########------------####--####--#####----------------------####------\
##########-----------####--####--####-----------------------####------\
####-######----------####--####--####-----------------------####------\
####--######---------####--####--####-----------------------####------\
####---######--------####--####--#####----------------------####------\
####----######-------####--####--######################-----####------\
####-----######------####--####---######################----####------\
####------######-----####--####----######################---####------\
####-------######----####--####------#####################--####------\
####--------######---####--####----------------------#####--####------\
####---------######--####--####-----------------------####--####------\
####----------######-####--####-----------------------####--####------\
####-----------##########--####-----------------------####--####------\
####------------#########--#####---------------------#####--####------\
####-------------########--###############################--####------\
####--------------#######--##############################---####------\
####---------------#####----############################----####------\
####----------------###------##########################-----####------";

#define BITMAP_UNIT_24          4 /* 4 bytes ==> 32 bits */

#define BACKGROUND_PIXVAL 150
#define BORDER_WIDTH (unsigned int)4

static Colormap cmap;
static Display *display;
static GC gc;
static Visual *visual;
static Window rw, sgmntwork_window, small_window, big_window;
static XImage *bars_image, *pseudo_cfgyow_image;
static XSetWindowAttributes wattr;
static unsigned char *sgmntwork_pic;
static int screen, warp_mouse, sgmntwork_window_xloc, fgw, fgh,
  std_corepixel_x, std_corepixel_y;
static unsigned int cmap_size, dw, dh;
static unsigned long wmask;
static SLEEPS sleeps;

/*****************************************************************/

/* Graphics initialization */

void grphcs_init(SLEEPS *sleeps_in, const int warp_mouse_in,
          RGAR_PRS *rgar_prs_in)
{
  char str[100];

  memcpy(&sleeps, sleeps_in, sizeof(SLEEPS));
  warp_mouse = warp_mouse_in;
  std_corepixel_x = rgar_prs_in->std_corepixel_x;
  std_corepixel_y = rgar_prs_in->std_corepixel_y;
#ifdef HP
  setvbuf(stdout, (char *)NULL, _IOLBF, BUFSIZ);
#else
  setlinebuf(stdout);
#endif
  display = XOpenDisplay((char *)NULL);
  if(display == (Display *)NULL) {
    sprintf(str, "cannot connect to X11 server %s",
      XDisplayName((char *)NULL));
    fatalerr("grphcs_init", str, NULL);
  }
  screen = DefaultScreen(display);
  dw = DisplayWidth(display, screen);
  dh = DisplayHeight(display, screen);
  rw = RootWindow(display, screen);
  visual = DefaultVisual(display, screen);
  if(visual->class != TrueColor) {
     cmap = gray_colormap(display, &visual, 8);
     cmap_size = 256;
     set_gray_colormap(display, cmap, cmap_size, 255);
     wattr.colormap = cmap;
     wattr.background_pixel = BACKGROUND_PIXVAL;
     wmask = CWColormap | CWBackPixel;
  }
  gc = XDefaultGC(display, screen);
  grphcs_titlepage();
}

/*****************************************************************/

/* Creates a window whose top-left corner is at (x,0), of width w by
height h */

Window grphcs_startwindow(const int x, const int w, const int h)
{
  Window window;
  XSizeHints size_hints;

  window = XCreateWindow(display, rw, x, 0, w, h, BORDER_WIDTH,
    CopyFromParent, CopyFromParent, visual, wmask, &wattr);
  size_hints.x = x;
  size_hints.y = 0;
  size_hints.width = w;
  size_hints.height = h;
  size_hints.flags = (USSize | USPosition);
  XSetStandardProperties(display, window, "", "", None, (char **)NULL,
    0, &size_hints);
  XMapWindow(display, window);
  XFlush(display);
  if(warp_mouse)
    XWarpPointer(display, None, window, 0, 0, 0, 0, w - 10, h - 10);
  return window;
}

/*****************************************************************/

/* Makes the big window, destroys the small window, and displays an
original fingerprint raster in the big window */

void grphcs_origras(unsigned char *ras, const int w, const int h)
{
  unsigned char *buf;
  XImage *image;

  XDestroyWindow(display, small_window);
  big_window = grphcs_startwindow(0, w, h);
  buf = (unsigned char *)malloc_ch(w * h);
  memcpy(buf, ras, w * h * sizeof(unsigned char));
  image = xcreateimage(buf, 8, ZPixmap, 0, w, h, 8, w);
  XPutImage(display, big_window, gc, image, 0, 0, 0, 0, w, h);
  XFlush(display);
  XDestroyImage(image);
  sgmntwork_window_xloc = w + 8;
}

/*****************************************************************/

/* Makes the small window, destroys the big window and the
segmentor-work window, and displays the segmented raster in the small
window */

void grphcs_segras(unsigned char **segras, const int w, const int h)
{
  XImage *image;
  unsigned char *buf;

  small_window = grphcs_startwindow(0, w, h);
  XDestroyWindow(display, big_window);
  XDestroyWindow(display, sgmntwork_window);
  dptr2ptr_uchar(segras, &buf, w, h);
  image = xcreateimage(buf, 8, ZPixmap, 0, w, h, 8, w);
  XPutImage(display, small_window, gc, image, 0, 0, 0, 0, w, h);
  XFlush(display);
  XDestroyImage(image);
  sleepity(sleeps.segras);
}

/*****************************************************************/

/* Writes a wxh-pixel square of enhancer output at location
(x,y) in the small window */

void grphcs_enhnc_outsquare(unsigned char outsquare[WS][WS], const int w,
          const int h, const int x, const int y)
{
  XImage *image;
  unsigned char *buf;

  buf = (unsigned char *)malloc_ch(w * h);
  memcpy(buf, outsquare, w * h * sizeof(unsigned char));
  image = xcreateimage(buf, 8, ZPixmap, 0, w, h, 8, w);
  XPutImage(display, small_window, gc, image, 0, 0, x, y, w, h);
  XFlush(display);
  XDestroyImage(image);
}

/*****************************************************************/

void grphcs_enhnc_sleep(void)
{
  sleepity(sleeps.enhnc);
}

/*****************************************************************/

void grphcs_foundconup_sleep(void)
{
  sleepity(sleeps.foundconup);
}

/*****************************************************************/

void grphcs_noconup_sleep(void)
{
  sleepity(sleeps.noconup);
}

/*****************************************************************/

/* Produces an image that represents average orientations as bars, and
displays it in the small window.  When called to display the first
(original) average orientations, flag should be 0, and when called to
the display the second (registered) average orientations, flag should
be 1. */

void grphcs_bars(float **x, float **y, const int w, const int h, const int flag)
{
  static char *barpic_buf;
  float top;
  float **dg, **mag;
  int bw, bh;

  bw = (w+2)*WS;
  bh = (h+2)*WS;
  XDestroyWindow(display, small_window);
  small_window = grphcs_startwindow(0, bw, bh);
  malloc_dbl_flt(&dg, h, w, "grphcs_bars dg");
  malloc_dbl_flt(&mag, h, w, "grphcs_bars mag");
  grphcs_xy_to_dmt(x, y, dg, mag, w, h, &top);
  barpic_buf = malloc_ch(bh * bw);
  bars_image = grphcs_dmt_to_bars(dg, mag, w, h, top,
                                  (unsigned char *)barpic_buf, bw, bh);
  free_dbl_flt(dg, h);
  free_dbl_flt(mag, h);
  XPutImage(display, small_window, gc, bars_image, 0, 0, 0, 0, bw,
    bh);
  XFlush(display);
  if(flag) {
    sleepity(sleeps.regbars);
    XDestroyImage(bars_image);
  }
}

/*****************************************************************/

/* Converts each orientation vector (x,y) to an angle in degrees (dg)
and a magnitude (mag), and finds the top magnitude. */

void grphcs_xy_to_dmt(float **x, float **y, float **dg, float **mag,
          const int w, const int h, float *top_ret)
{
  int i, j;
  float top, themag;
  static float a = 28.6479;  /* 90./pi */

  for(i = 0, top = -1.; i < h; i++)
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
      if((themag = mag[i][j] = sqrt((double)(x[i][j] * x[i][j] +
        y[i][j] * y[i][j]))) > top)
	top = themag;
    }
  *top_ret = top;
}

/*****************************************************************/

/* Converts an array of angles and magnitudes (of orientation vectors)
and their top magnitude, to a picture in which each orientation vector
is represented as a small bar that should be approximately parallel to
the local ridges and valleys. */

XImage *grphcs_dmt_to_bars(float **dg, float **mag, const int w, const int h,
            const float top, unsigned char *barpic, const int bw, const int bh)
{
  int i, j, iangle, ii, jj, ilen, jjlo, jjhi, iyow, jyow;
  float angle, ac, as, x0[WS][WS], y0[WS][WS], hurl;
  static int f = 1, bars[WS][WS][32][4];
  static float del = .09817 /* pi/32. */, fac = .177778 /* 32./180.*/;
  float hwsp5;
  int jstp;

  if(f) {
    f = 0;
    if(HWS < 4 || HWS%4) {
       fprintf(stdout, "Warning From grphcs_dmt_to_bars in grphcs.o\n");
       fprintf(stdout, "Blocksize (HWS) should be >= 4 and even divisible by 4\n");
    }
    hwsp5 = (float)HWS - 0.5;
    jstp = HWS/4;
    for(i = 0; i < WS; i++)
      for(j = 0; j < WS; j++) {
	x0[i][j] = (float)j - hwsp5;
	y0[i][j] = hwsp5 - (float)i;
      }
    for(iangle = 0, angle = 0.; iangle < 32; iangle++, angle += del) {
      ac = cos((double)angle);
      as = sin((double)angle);
      for(i = 0; i < WS; i++)
	for(j = 0; j < WS; j++) {
	   hurl = ac * x0[i][j] + as * y0[i][j] + hwsp5;
	   jj = ((hurl >= 0.) ? hurl + .5 : hurl - .5);
	   hurl = as * x0[i][j] - ac * y0[i][j] + hwsp5;
	   ii = ((hurl >= 0.) ? hurl + .5 : hurl - .5);
           jjlo = HWS-jstp;
           jjhi = HWS+jstp-1;
	   for(ilen = 0; ilen < 4; ilen++, jjlo -= jstp, jjhi += jstp)
	     bars[i][j][iangle][ilen] = (ii >= HWS-1 && ii <= HWS
               && jj >= jjlo && jj <= jjhi);
	 }
    }
  }

  for(i = 0; i < bw*bh; i++)
    barpic[i] = 0;

  for(i = 0, iyow = WS; i < h; i++, iyow += WS)
    for(j = 0, jyow = WS; j < w; j++, jyow += WS) {
      ilen = 3. * mag[i][j] / top + .5;
      if(ilen == 0)
        continue;
      iangle = (int)(dg[i][j] * fac + .5);
      if(iangle == 32)
	iangle = 0;
      for(ii = 0; ii < WS; ii++)
	for(jj = 0; jj < WS; jj++)
	  if(bars[ii][jj][iangle][ilen])
            barpic[(iyow + ii)*bw + (jyow + jj)] = 255;
    }
  return xcreateimage(barpic, 8, ZPixmap, 0, bw, bh, 8, bw);
}

/*****************************************************************/

/* Marks (in the small window) the core found by r92, with a plus sign,
and marks the median core (registration standard point) with a plus
sign enclosed by a square */

void grphcs_core_medcore(const int pixel_x, const int pixel_y, const int w,
          const int h)
{
  int x, xmin, xmax, y, ymin, ymax;

  xmin = pixel_x - WS;
  xmax = pixel_x + WS;
  ymin = pixel_y - WS;
  ymax = pixel_y + WS;
  if(xmin >= 0 && xmax < w && ymin >= 0 && ymax < h) {
    for(x = xmin; x <= xmax; x++) {
      XPutPixel(bars_image, x, pixel_y - 1, 0xffffffff);
      XPutPixel(bars_image, x, pixel_y, 0xffffffff);
      XPutPixel(bars_image, x, pixel_y + 1, 0xffffffff);
    }
    for(y = ymin; y <= ymax; y++) {
      XPutPixel(bars_image, pixel_x - 1, y, 0xffffffff);
      XPutPixel(bars_image, pixel_x, y, 0xffffffff);
      XPutPixel(bars_image, pixel_x + 1, y, 0xfffffff);
    }
  }
  if(std_corepixel_x > w) {
     printf("Change avg core x location from %d to %d\n",
             std_corepixel_x, w/2);
     std_corepixel_x = w/2;
  }
  if(std_corepixel_y > h) {
     printf("Change avg core y location from %d to %d\n",
             std_corepixel_y, h/2);
     std_corepixel_y = h/2;
  }
  xmin = std_corepixel_x - WS;
  xmax = std_corepixel_x + WS;
  ymin = std_corepixel_y - WS;
  ymax = std_corepixel_y + WS;
  for(x = xmin; x <= xmax; x++) {
    XPutPixel(bars_image, x, ymin, 0xffffffff);
    XPutPixel(bars_image, x, ymin + 1, 0xffffffff);
    XPutPixel(bars_image, x, ymin + 2, 0xffffffff);
    XPutPixel(bars_image, x, std_corepixel_y - 1, 0xffffffff);
    XPutPixel(bars_image, x, std_corepixel_y, 0xffffffff);
    XPutPixel(bars_image, x, std_corepixel_y + 1, 0xffffffff);
    XPutPixel(bars_image, x, ymax - 2, 0xffffffff);
    XPutPixel(bars_image, x, ymax - 1, 0xffffffff);
    XPutPixel(bars_image, x, ymax, 0xffffffff);
  }
  for(y = ymin; y <= ymax; y++) {
    XPutPixel(bars_image, xmin, y, 0xffffffff);
    XPutPixel(bars_image, xmin + 1, y, 0xffffffff);
    XPutPixel(bars_image, xmin + 2, y, 0xffffffff);
    XPutPixel(bars_image, std_corepixel_x - 1, y, 0xffffffff);
    XPutPixel(bars_image, std_corepixel_x, y, 0xffffffff);
    XPutPixel(bars_image, std_corepixel_x + 1, y, 0xffffffff);
    XPutPixel(bars_image, xmax - 2, y, 0xffffffff);
    XPutPixel(bars_image, xmax - 1, y, 0xffffffff);
    XPutPixel(bars_image, xmax, y, 0xffffffff);
  }
  XPutImage(display, small_window, gc, bars_image, 0, 0, 0, 0, w,
    h);
  XFlush(display);
  XDestroyImage(bars_image);
  sleepity(sleeps.core_medcore);
}

/*****************************************************************/

/* Displays the normalized NN activations as a bar graph in the
small window */

void grphcs_normacs(float *normacs, const int n, char *cls_str)
{
  IHEAD *head;
  XImage *image;
  static XImage *letter_image[6];
  char *datadir, str[200];
  unsigned char *pic, *buf, *p, *pe;
  int iac, bar_xmin, bar_xmax, bar_ymin, x, y, depth,
    letter_x;
  static int f = 1, letters_x_start,
    letters_y, letter_adjust_x[6], letter_adjust_y[6],
    letter_width[6], letter_height[6];
  static int w, h, bxmn, bxmx, bxs, bars_ymax;

  if(f) {
    if(n > 6)
       fatalerr("grphcs_normacs",
                "currently can only support a maximum of 6 letters","ALRSTW");
    f = 0;
    datadir = get_datadir();
    for(iac = 0; iac < n; iac++) {
      sprintf(str, "%s/images/big%c.pct", datadir, (cls_str[iac]+0x20));
      ReadIheadRaster(str, &head, &buf, &(letter_width[iac]),
        &(letter_height[iac]), &depth);
      free(head);
      for(pe = (p = buf) + letter_width[iac] * letter_height[iac];
        p < pe; p++)
        if(*p)
          *p = 255;
        else
          *p = 0;
      letter_image[iac] = xcreateimage(buf, 8, ZPixmap, 0, letter_width[iac],
                                       letter_height[iac], 8, letter_width[iac]);
      letter_adjust_xy(cls_str[iac], &(letter_adjust_x[iac]), &(letter_adjust_y[iac]));
    }
    w = 512 - ((6-n)*80);
    h = 480;
    bxmn = 40;
    bxmx = 80;
    bxs = 80;
    bars_ymax = 400;
    letters_y = 420;
    letters_x_start = bxmn - 3;
  }
  XDestroyWindow(display, small_window);
  small_window = grphcs_startwindow(0, w, h);
  calloc_uchar(&pic, w*h, "grphcs_normacs pic");
  for(iac = 0, bar_xmin = bxmn, bar_xmax = bxmx; iac < n; iac++,
    bar_xmin += bxs, bar_xmax += bxs) {
    bar_ymin = (1. - normacs[iac]) * bars_ymax + .5;
    for(y = bar_ymin; y <= bars_ymax; y++)
      for(x = bar_xmin; x <= bar_xmax; x++)
	*(pic + y * w + x) = 200;
  }
  image = xcreateimage(pic, 8, ZPixmap, 0, w, h, 8, w);
  XPutImage(display, small_window, gc, image, 0, 0, 0, 0, w, h);
  XFlush(display);
  XDestroyImage(image);
  for(iac = 0, letter_x = letters_x_start; iac < n; iac++,
    letter_x += bxs) {
    XPutImage(display, small_window, gc, letter_image[iac], 0, 0,
      letter_x + letter_adjust_x[iac], letters_y +
      letter_adjust_y[iac], letter_width[iac], letter_height[iac]);
  }
  XFlush(display);
  sleepity(sleeps.normacs);
}

/*****************************************************************/
void letter_adjust_xy(const char let, int *dx, int *dy)
{
   if(let == 'A') {
      *dx = 0;
      *dy = 1;
   }
   else if(let == 'L') {
      *dx = -2;
      *dy = 0;
   }
   else if(let == 'R') {
      *dx = 2;
      *dy = 0;
   }
   else if(let == 'S') {
      *dx = 6;
      *dy = 0;
   }
   else if(let == 'T') {
      *dx = 3;
      *dy = 0;
   }
   else if(let == 'W') {
      *dx = -6;
      *dy = 0;
   }
   else
      fatalerr("letter_adjust_xy","Invalid let value","Not a ALRSTW");
}

/*****************************************************************/

/* Makes the segmentor-work window (in which will be displayed 7 small
pictures showing stages of the segmentor's processing as it decides
where to snip), and allocates and clears a picture buffer for this
window */

void grphcs_sgmntwork_init(const int fgw_in, const int fgh_in)
{

  fgw = fgw_in;
  fgh = fgh_in;
  sgmntwork_window = grphcs_startwindow(sgmntwork_window_xloc, fgw,
    7 * fgh);
  sgmntwork_pic = (unsigned char *)malloc_ch(fgw * 7 * fgh);
  memset(sgmntwork_pic, 255, fgw * 7 * fgh);
}

/*****************************************************************/

/* If flag is zero, writes the provided foreground in the
segmentor-work-picture's position 0; if flag is nonzero, writes it at
position 1 */

void grphcs_sgmntwork_fg(unsigned char *fg, const int flag)
{
  int x, y, yloc;

  yloc = ((flag == 0) ? 0 : fgh);
  for(y = 0; y < fgh; y++)
    for(x = 0; x < fgw; x++)
      *(sgmntwork_pic + (yloc + y) * fgw + x) = (*(fg + y * fgw +
        x) ? 0 : 255);
}

/*****************************************************************/

/* Writes, in the segmentor-work-picture, an edge that the segmentor
found in its processed foreground-map.  Flag values of 0, 1, and 2
mean left, top, and right edges, which are displayed in positions 2,
3, and 4 */

void grphcs_sgmntwork_edge(int *xx, int *yy, const int n, const int flag)
{
  int yloc, i;

  if(flag == 0)      /* left edge */
    yloc = 2 * fgh;
  else if(flag == 1) /* top edge */
    yloc = 3 * fgh;
  else               /* right edge */
    yloc = 4 * fgh;
  for(i = 0; i < n; i++)
    *(sgmntwork_pic + (yloc + yy[i]) * fgw + xx[i]) = 0;
}

/*****************************************************************/

/* Writes, in position 5 of the segmentor-work-picture, the straight
lines that the segmentor has fitted to the foreground-edges it found
*/

void grphcs_sgmntwork_lines(float as[3], float bs[3])
{
  int i, x, y, yloc;
  float a, b;

  yloc = 5 * fgh;
  for(i = 0; i < 3; i++) {
    a = as[i];
    b = bs[i];
    if(i == 0 || i == 2)
      for(y = 0; y < fgh; y++) {
	x = a * y + b + .5;
	if(x >= 0 && x < fgw)
	  *(sgmntwork_pic + (yloc + y) * fgw + x) = 0;
      }
    else
      for(x = 0; x < fgw; x++) {
	y = a * x + b + .5;
	if(y >= 0 && y < fgh)
	  *(sgmntwork_pic + (yloc + y) * fgw + x) = 0;
      }
  }
}

/*****************************************************************/

/* Writes, in position 6 of the segmentor-work picture, superimposed
on a copy of the segmentor's processed foreground-map, a box
corresponding to the (possibly rotated) rectangular subraster that the
segmentor has decided to snip out of the original fingerprint raster
*/

void grphcs_sgmntwork_box(unsigned char *fg, const float c, const float s,
          const int x_centroid, const int y_centroid, const int ytop,
          const int w, const int h)
{
  unsigned char *p;
  int x, yloc, y, ye, x2, y2;

  yloc = 6 * fgh;
  for(y = 0; y < fgh; y++)
    for(x = 0; x < fgw; x++)
      *(sgmntwork_pic + (yloc + y) * fgw + x) = (*(fg + y * fgw +
        x) ? 0 : 255);
  for(x = -(w/2); x < w/2; x++) {
    x2 = c * x + s * ytop + x_centroid + .5;
    y2 = -s * x + c * ytop + y_centroid + .5;
    if(x2 >= 0 && x2 < fgw && y2 >= 0 && y2 < fgh) {
      p = (sgmntwork_pic + (yloc + y2) * fgw + x2);
      *p = 255 - *p;
    }
    x2 = c * x + s * (ytop + (h-1)) + x_centroid + .5;
    y2 = -s * x + c * (ytop + (h-1)) + y_centroid + .5;
    if(x2 >= 0 && x2 < fgw && y2 >= 0 && y2 < fgh) {
      p = (sgmntwork_pic + (yloc + y2) * fgw + x2);
      *p = 255 - *p;
    }
  }
  for(ye = (y = ytop) + h; y < ye; y++) {
    x2 = c * (-(w/2)) + s * y + x_centroid + .5;
    y2 = -s * (-(w/2)) + c * y + y_centroid + .5;
    if(x2 >= 0 && x2 < fgw && y2 >= 0 && y2 < fgh) {
      p = (sgmntwork_pic + (yloc + y2) * fgw + x2);
      *p = 255 - *p;
    }
    x2 = c * ((w/2)-1) + s * y + x_centroid + .5;
    y2 = -s * ((w/2)-1) + c * y + y_centroid + .5;
    if(x2 >= 0 && x2 < fgw && y2 >= 0 && y2 < fgh) {
      p = (sgmntwork_pic + (yloc + y2) * fgw + x2);
      *p = 255 - *p;
    }
  }
}

/*****************************************************************/

/* Displays the segmentor-work-picture in its window (to the right of
the original-raster window) */

void grphcs_sgmntwork_finish(void)
{
  XImage *image;
  int i;

  image = xcreateimage(sgmntwork_pic, 8, ZPixmap, 0, fgw, 7*fgh, 8, fgw);
  for(i = 0; i < 5; i++)
    XPutImage(display, sgmntwork_window, gc, image, 0, 0, 0, 0, fgw,
      7 * fgh);
  XFlush(display);
  XDestroyImage(image);
  sleepity(sleeps.sgmntwork);
}

/*****************************************************************/

/* Draws, in the small window, a background picture (on which
depictions of pseudoridges will be superimposed) consisting of black
corresponding to cfgyow (a cwXch-sized foreground map, after its
processing by pseudo: erosions, etc.) and light gray elsewhere. */

void grphcs_pseudo_cfgyow(unsigned char **cfgyow, const int cw, const int ch,
          const int w, const int h)
{
  static unsigned char *pic;
  int i, j, iis, iie, jjs, jje, ii, jj;
  static int f = 1;

  if(f)
    f = 0;
  else
    XDestroyImage(pseudo_cfgyow_image);
  XDestroyWindow(display, small_window);
  small_window = grphcs_startwindow(0, w, h);
  pic = (unsigned char *)malloc_ch(h * w);
  for(i = iis = 0, iie = 8; i < ch; i++, iis += 8, iie += 8)
    for(j = jjs = 0, jje = 8; j < cw; j++, jjs += 8, jje += 8)
      if(cfgyow[i][j])
	for(ii = iis; ii < iie; ii++)
	  for(jj = jjs; jj < jje; jj++)
	    *(pic + ii * w + jj) = 0;
      else
	for(ii = iis; ii < iie; ii++)
	  for(jj = jjs; jj < jje; jj++)
	    *(pic + ii * w + jj) = 200;
  pseudo_cfgyow_image = xcreateimage(pic, 8, ZPixmap, 0, w, h, 8, w);
  XPutImage(display, small_window, gc, pseudo_cfgyow_image, 0, 0, 0, 0,
    w, h);
}

/*****************************************************************/

void grphcs_pseudo_cfgyow_reput(const int w, const int h)
{
  XPutImage(display, small_window, gc, pseudo_cfgyow_image, 0, 0, 0, 0,
    w, h);
}

/*****************************************************************/

/* Draws a pseudoridge (or just a lobe of one) in the small window.
If flag is true, the routine assumes the pseudoridge is really a conup
(concave-upward lobe), and it first redraws the depiction of the
foreground and background, then draws the conup as a bold line.  If
flag is false, the routine assumes the pseudoridge is just any
pseudoridge that was produced, and it draws it as a thin line. */

void grphcs_pseudo_pseudoridge(float *ris, float *rjs, const int npoints,
          const int flag, const int w, const int h)
{
  XPoint *point_p;
  static XPoint *points;
  XGCValues values;
  static GC path_gc, hasconup_path_gc;
  int i;
  static int f = 1, max_npoints = 0;
  float *ris_p, *rjs_p;

  if(f) {
    f = 0;
    path_gc = XCreateGC(display, small_window, 0, 0);
    XCopyGC(display, gc, (unsigned long)0xffffffff, path_gc);
    values.foreground = 0xffffffff;
    values.background = 0; /* maybe doesn't matter */
    values.line_width = 1;
    values.line_style = LineSolid;
    values.cap_style = CapRound;
    values.join_style = JoinRound;
    XChangeGC(display, path_gc, GCForeground | GCBackground |
      GCLineWidth | GCLineStyle | GCCapStyle | GCJoinStyle, &values);
    hasconup_path_gc = XCreateGC(display, small_window, 0, 0);
    XCopyGC(display, path_gc, (unsigned long)0xffffffff,
      hasconup_path_gc);
    values.line_width = 4;
    XChangeGC(display, hasconup_path_gc, GCLineWidth, &values);
  }
  if(npoints > max_npoints) {
    if(max_npoints)
      free(points);
    points = (XPoint *)malloc_ch(npoints * sizeof(XPoint));
    max_npoints = npoints;
  }
  for(i = 0, point_p = points, ris_p = ris, rjs_p = rjs; i < npoints;
    i++, point_p++, ris_p++, rjs_p++) {
    point_p->x = 12 + (int)(8. * *rjs_p + .5);
    point_p->y = 12 + (int)(8. * *ris_p + .5);
  }
  if(flag) {
    grphcs_pseudo_cfgyow_reput(w, h);
    XDrawLines(display, small_window, hasconup_path_gc, points,
      npoints, CoordModeOrigin);
    XFlush(display);
  }
  else {
    XDrawLines(display, small_window, path_gc, points, npoints,
      CoordModeOrigin);
    XFlush(display);
  }
}

/*****************************************************************/

/* Makes the small window and displays the "title page" in it. */

void grphcs_titlepage(void)
{
  XImage *image;
  IHEAD *head;
  char *nistp, *datadir, str[200];
  unsigned char *pic, *buf;
  int tw, th, x, y, x2, y2, w, h, d, xloc, isub, i;
  static int pcasys_yloc = 115, subtitle_yloc[2] = {281, 325};

  tw = 512;
  th = 480;
  small_window = grphcs_startwindow(0, tw, th);
  pic = (unsigned char *)malloc_ch(th * tw);
  memset(pic, 255, th * tw);

  /* NIST logo */
  for(y = 0, y2 = th - 2 * nist_h - 30, nistp = nist; y < nist_h; y++,
    y2 += 2)
    for(x = 0, x2 = (tw - 2 * nist_w) / 2; x < nist_w; x++, x2 += 2,
      nistp++)
      if(*nistp == '#')
	*(pic + y2 * tw + x2) =
	*(pic + y2 * tw + x2 + 1) =
	*(pic + (y2 + 1) * tw + x2) =
	*(pic + (y2 + 1) * tw + x2 + 1) = 0;

  /* Main title in large characters */
  datadir = get_datadir();
  sprintf(str, "%s/images/pcasys.pct", datadir);
  ReadIheadRaster(str, &head, &buf, &w, &h, &d);
  free(head);
  xloc = (tw - w) / 2;
  for(y = 0; y < h; y++)
    for(x = 0; x < w; x++)
      if(*(buf + y * w + x))
	*(pic + (pcasys_yloc + y) * tw + xloc + x) = 0;
  free(buf);

  /* Subtitle (two lines) in smaller characters */
  for(isub = 0; isub < 2; isub++) {
    sprintf(str, "%s/images/sub_%d.pct", datadir, isub);
    ReadIheadRaster(str, &head, &buf, &w, &h, &d);
    free(head);
    xloc = (tw - w) / 2;
    for(y = 0; y < h; y++)
      for(x = 0; x < w; x++)
	if(*(buf + y * w + x))
	  *(pic + (subtitle_yloc[isub] + y) * tw + xloc + x) = 0;
    free(buf);
  }

  image = xcreateimage(pic, 8, ZPixmap, 0, tw, th, 8, tw);

  for(i = 0; i < 5; i++)
    XPutImage(display, small_window, gc, image, 0, 0, 0, 0, tw, th);
  XFlush(display);
  XDestroyImage(image);
  sleepity(sleeps.titlepage);
}

/*****************************************************************/

/* Displays, in the small window, a bar graph representing the feature
vector that is the result of the application of a linear transform to
the registered orientation-grid of the test print.  The bars actually
show the result of subtracting from the test feature vector, the
average of the proto feature vectors; this eliminates large "offsets"
that are constant, so that the interesting part that distinguishes
between classes shows up better.  If a bar would not have fit (when
using the scale which is determined by val_lim), it is displayed in
white instead of the usual bright gray. */

void grphcs_featvec(float *featvec, const int nfeats)
{
  XImage *image;
  unsigned char *pic;
  int fw, ifeat, xmin, xmax, ymin, ymax, x, y, tone;
  float val;
  static float val_lim = 2.5; /* should be tuned so that feature bars
    use much of the space, but without overflowing often */
  /* 0 - 479: above-middle is 239 */      
  int w, h;

  w = 512;
  fw = w/nfeats;
  if(fw < 4) {
     fw = 4;
     w = nfeats * fw;
  }
  h = 480;

  XDestroyWindow(display, small_window);
  small_window = grphcs_startwindow(0, w, h);
  calloc_uchar(&pic, w*h, "grphcs_featvec pic");
  for(ifeat = xmin = 0, xmax = fw - 1; ifeat < nfeats; ifeat++,
    xmin += fw, xmax += fw) {
    val = featvec[ifeat];
    if(val >= 0.) {
      ymax = 239;
      if(val <= val_lim) {
	ymin = (1. - val / val_lim) * 239. + .5;
	tone = 200;
      }
      else {
	ymin = 0;
	tone = 255;
      }
    }
    else {
      ymin = 239;
      if(val >= -val_lim) {
	ymax = (1. - val / val_lim) * 239. + .5;
	tone = 200;
      }
      else {
	ymax = 478;
	tone = 255;
      }
    }
    for(y = ymin; y <= ymax; y++)
      for(x = xmin; x <= xmax; x++)
	*(pic + y * w + x) = tone;
  }
  image = xcreateimage(pic, 8, ZPixmap, 0, w, h, 8, w);
  XPutImage(display, small_window, gc, image, 0, 0, 0, 0, w, h);
  XFlush(display);
  XDestroyImage(image);
  sleepity(sleeps.featvec);
}

/*****************************************************************/

/* Displays the following in the small window: actual class of a test
fingerprint; and its hypothetical class and confidence, which are the
classifier's final output after using the NN and the
pseudoridge-tracer. */

void grphcs_lastdisp(const int actual_classind, const int hyp_classind,
          const float confidence, char *cls_str, const int n)
{
  XImage *image;
  IHEAD *head;
  char *datadir, str[200];
  unsigned char *pic, *buf;
  static unsigned char *actual_class_buf, *alrstw_buf[6],
    *digits_buf, *point_buf, *hyp_class_buf, *conf_buf;
  int i, x, xe, xx, y, d, yow, ichar, ichar_yow, h,
    w, xloc, yloc;
  static int f = 1, digits_indelx = 29, digits_outdelx = 23,
    hyp_class_xloc = 39, hyp_class_yloc = 260, conf_xloc = 300,
    conf_yloc = 259, conf_val_firstdig_xloc = 393,
    conf_val_last2digs_xloc = 428, conf_val_digs_yloc = 255,
    point_xloc = 416, point_yloc = 256,
    actual_class_xloc = 40, actual_class_yloc = 170,
    actual_alrstw_xloc = 230, actual_alrstw_yloc = 169,
    hyp_alrstw_xloc = 215, hyp_alrstw_yloc = 261, hyp_class_w,
    hyp_class_h, conf_w, conf_h, actual_class_w, actual_class_h,
    digits_w, digits_h, point_w, point_h, alrstw_w[6], alrstw_h[6];
  static int dw, dh;

  if(f) {
    f = 0;
    dw = 512; dh = 480;
    datadir = get_datadir();
    sprintf(str, "%s/images/a_c.pct", datadir);
    ReadIheadRaster(str, &head, &actual_class_buf, &actual_class_w,
      &actual_class_h, &d);
    free(head);
    for(i = 0; i < n; i++) {
      sprintf(str, "%s/images/%c.pct", datadir, (cls_str[i]+0x20));
      ReadIheadRaster(str, &head, &(alrstw_buf[i]), &(alrstw_w[i]),
        &(alrstw_h[i]), &d);
      free(head);
    }
    sprintf(str, "%s/images/h_c.pct", datadir);
    ReadIheadRaster(str, &head, &hyp_class_buf, &hyp_class_w,
      &hyp_class_h, &d);
    free(head);
    sprintf(str, "%s/images/conf.pct", datadir);
    ReadIheadRaster(str, &head, &conf_buf, &conf_w,
      &conf_h, &d);
    free(head);
    sprintf(str, "%s/images/digits.pct", datadir);
    ReadIheadRaster(str, &head, &digits_buf, &digits_w,
      &digits_h, &d);
    free(head);
    sprintf(str, "%s/images/point.pct", datadir);
    ReadIheadRaster(str, &head, &point_buf, &point_w,
      &point_h, &d);
    free(head);
  }
  XDestroyWindow(display, small_window);
  small_window = grphcs_startwindow(0, dw, dh);
  pic = (unsigned char *)malloc_ch(dh * dw);
  memset(pic, 255, dh * dw);
  for(y = 0; y < actual_class_h; y++)
    for(x = 0; x < actual_class_w; x++)
      if(*(actual_class_buf + y * actual_class_w + x))
	*(pic + (actual_class_yloc + y) * dw + actual_class_xloc
          + x) = 0;
  h = alrstw_h[actual_classind];
  w = alrstw_w[actual_classind];
  buf = alrstw_buf[actual_classind];
  yloc = actual_alrstw_yloc;
  xloc = actual_alrstw_xloc;
  for(y = 0; y < h; y++)
    for(x = 0; x < w; x++)
      if(*(buf + y * w + x))
	*(pic + (yloc + y) * dw + xloc + x) = 0;
  for(y = 0; y < hyp_class_h; y++)
    for(x = 0; x < hyp_class_w; x++)
      if(*(hyp_class_buf + y * hyp_class_w + x))
	*(pic + (hyp_class_yloc + y) * dw + hyp_class_xloc + x) = 0;
  h = alrstw_h[hyp_classind];
  w = alrstw_w[hyp_classind];
  buf = alrstw_buf[hyp_classind];
  yloc = hyp_alrstw_yloc;
  xloc = hyp_alrstw_xloc;
  for(y = 0; y < h; y++)
    for(x = 0; x < w; x++)
      if(*(buf + y * w + x))
	*(pic + (yloc + y) * dw + xloc + x) = 0;
  for(y = 0; y < conf_h; y++)
    for(x = 0; x < conf_w; x++)
      if(*(conf_buf + y * conf_w + x))
	*(pic + (conf_yloc + y) * dw + conf_xloc + x) = 0;
  sprintf(str, "%.2f", confidence);
  for(ichar = 0; ichar < 4; ichar++) {
    if(ichar == 1) {
      for(y = 0; y < point_h; y++)
	for(x = 0; x < point_w; x++)
	  if(*(point_buf + y * point_w + x))
	    *(pic + (point_yloc + y) * dw + point_xloc + x) = 0;
    }
    else {
      yow = str[ichar] - '0' - 1;
      if(yow == -1)
	yow = 9;
      if(ichar == 0) {
	xloc = conf_val_firstdig_xloc;
	ichar_yow = 0;
      }
      else {
	xloc = conf_val_last2digs_xloc;
	ichar_yow = ichar - 2;
      }
      for(y = 0; y < digits_h; y++)
	for(xe = (x = yow * digits_indelx) + digits_indelx, xx = 0;
          x < xe; x++, xx++)
	  if(x < digits_w && *(digits_buf + y * digits_w + x))
	    *(pic + (conf_val_digs_yloc + y) * dw + xloc + ichar_yow *
              digits_outdelx + xx) = 0;
    }
  }
  image = xcreateimage(pic, 8, ZPixmap, 0, dw, dh, 8, dw);
  XPutImage(display, small_window, gc, image, 0, 0, 0, 0, dw, dh);
  XFlush(display);
  XDestroyImage(image);
  sleepity(sleeps.lastdisp);
}

/*****************************************************************/
void ImageBit8ToBit24Unit32(unsigned char **data24, unsigned char *data8,
          const int ww, const int wh)
{
   unsigned char *sptr, *dptr;
   int i, n, pixlen;

   pixlen=ww*wh;
   (*data24) = (unsigned char *)calloc(pixlen,BITMAP_UNIT_24);
   if(*data24 == (unsigned char *)NULL){
      fprintf(stderr, "ImageBit8ToBit24Unit32 : calloc : data24\n");
      exit(1);
   }

   sptr = data8;
   dptr = *data24;
   for(i = 0; i < pixlen; i++){
      for(n = 0; n < BITMAP_UNIT_24-1; n++)
         *dptr++ = *sptr;
      dptr++;
      sptr++;
   }
}

/*****************************************************************/
XImage *xcreateimage(unsigned char *pic, const int d, const int format,
            const int offset, const int w, const int h, const int bm_pad,
            const int bpl)
{
  unsigned char *data24;
  int td, tbpl;

  td = d;
  tbpl = bpl;
  if(visual->class == TrueColor) {
     ImageBit8ToBit24Unit32(&data24, pic, w, h);
     td = DefaultDepth(display, screen);
     tbpl *= BITMAP_UNIT_24;
     free(pic);
     pic = data24;
  }

  return(XCreateImage(display, visual, td, format, offset, (char *)pic,
                      w, h, bm_pad, tbpl));
}
