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
      LIBRARY: IMAGE - Image Manipulation and Processing Routines

      FILE:    RGB_YCC.C

      AUTHOR:  Michael Garris
      DATE:    02/15/2001
      UPDATED: 03/15/2005 by MDG

      Contains routines responsible for converting between RGB
      and YCbCr image colorspace.

      ROUTINES:
#cat: rgb2ycc_mem - takes an RGB pixmap and converts it to the YCbCr
#cat:               colorspace.
#cat: rgb2ycc_intrlv_mem - takes an interleaved RGB pixmap and converts
#cat:               it to the YCbCr colorspace.
#cat: rgb2ycc_nonintrlv_mem - takes a non-interleaved RGB pixmap and
#cat:               converts it to the YCbCr colorspace.
#cat: downsample_cmpnts - takes a non-interleaved pixmap and downsamples
#cat:               component planes based on specified factors.
#cat: window_avr_plane - downsamples a component plane by replacing adjacent
#cat:               windows of specified dimension with the average
#cat:               component value in each window.
#cat: avr_window - computes the averate component value in a window
#cat:               of specified location and dimension.
#cat: ycc2rgb_mem - takes a YCbCr pixmap and converts it to the RGB
#cat:               colorspace.
#cat: ycc2rgb_intrlv_mem - takes an interleaved YCbCr pixmap and converts
#cat:               it to the RGB colorspace.
#cat: ycc2rgb_nonintrlv_mem - takes a non-interleaved YCbCr pixmap and
#cat:               converts it to the RGB colorspace.
#cat: upsample_cmpnts - takes a non-interleaved pixmap and upsample
#cat:               component planes based on specified factors.
#cat: window_fill_plane - upsamples a component plane by replicating a
#cat:               single value into an output window of specifiec
#cat:               dimension in an expanded output plane.
#cat: fill_window - replicates a component value in a window of specified
#cat:               location and dimension.
#cat: test_evenmult_sampfctrs - ensures smaller downsample factors are
#cat:               an even multiple of the maximum component downsample
#cat:               factor.

***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <rgb_ycc.h>

/*****************************************************************/
int rgb2ycc_mem(unsigned char **odata, int *olen, unsigned char *idata,
                const int width, const int height, const int depth,
                const int intrlvflag)
{
   int ret;

   if(intrlvflag){
      if((ret = rgb2ycc_intrlv_mem(odata, olen, idata, width, height, depth)))
         return(ret);
   }
   else{
      if((ret = rgb2ycc_nonintrlv_mem(odata, olen, idata,
                                     width, height, depth)))
         return(ret);
   }

   return(0);
}

/*****************************************************************/
int rgb2ycc_intrlv_mem(unsigned char **oodata, int *oolen,
                       unsigned char *idata,
                       const int width, const int height, const int depth)
{
   int i, num_pix, olen;
   unsigned char *r_ptr, *g_ptr, *b_ptr;
   unsigned char *y_ptr, *cb_ptr, *cr_ptr;
   unsigned char *odata;
   double dy, dcb, dcr;
   int iy, icb, icr;

   /* If image has empty dimension, then done ... */
   if((width == 0) || (height == 0))
      return(0);

   if(depth != 24){
      fprintf(stderr, "ERROR : rgb2ycc_intrlv_mem : depth = %d != 24\n",
              depth);
      return(-2);
   }

   num_pix = width * height;
   olen = num_pix * (depth>>3);

   odata = (unsigned char *)malloc(olen * sizeof(unsigned char));
   if(odata == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : rgb2ycc_intrlv_mem : malloc : odata\n");
      return(-3);
   }
   for(i = 0,
       r_ptr = idata,
       g_ptr = idata+1,
       b_ptr = idata+2,
       y_ptr = odata,
       cb_ptr = odata+1,
       cr_ptr = odata+2;
       i < num_pix;
       i++,
       r_ptr+=3,
       g_ptr+=3,
       b_ptr+=3,
       y_ptr+=3,
       cb_ptr+=3,
       cr_ptr+=3){

      /* Compute float Y,Cb,Cr. */
      dy = (( 0.299  * (*r_ptr)) +
           ( 0.587  * (*g_ptr)) +
           ( 0.114  * (*b_ptr)));
      dcb = ((-0.1687 * (*r_ptr)) +
           (-0.3313 * (*g_ptr)) +
           ( 0.5    * (*b_ptr)) +
           128.0);
      dcr = (( 0.5    * (*r_ptr)) +
           (-0.4177 * (*g_ptr)) +
           (-0.0813 * (*b_ptr)) +
           128.0);

      /* Round to integer Y,Cb,Cr. */
      iy = sround(dy);
      icb = sround(dcb);
      icr = sround(dcr);

      /* Limit Y,Cb,Cr to [0..255]. */
      iy = (iy < 0) ? 0 : ((iy > 255) ? 255 : iy);
      icb = (icb < 0) ? 0 : ((icb > 255) ? 255 : icb);
      icr = (icr < 0) ? 0 : ((icr > 255) ? 255 : icr);

      /* Store uchar Y,Cb,Cr. */
      *y_ptr = iy;
      *cb_ptr = icb;
      *cr_ptr = icr;
   }

   *oodata = odata;
   *oolen = olen;

   return(0);
}

/*****************************************************************/
int rgb2ycc_nonintrlv_mem(unsigned char **oodata, int *oolen,
                       unsigned char *idata,
                       const int width, const int height, const int depth)
{
   int i, num_pix, olen;
   unsigned char *r_ptr, *g_ptr, *b_ptr;
   unsigned char *y_ptr, *cb_ptr, *cr_ptr;
   unsigned char *odata;
   double dy, dcb, dcr;
   int iy, icb, icr;

   /* If image has empty dimension, then done ... */
   if((width == 0) || (height == 0))
      return(0);

   if(depth != 24){
      fprintf(stderr, "ERROR : rgb2ycc_nonintrlv_mem : depth = %d != 24\n",
              depth);
      return(-2);
   }

   num_pix = width * height;
   olen = num_pix * (depth>>3);

   odata = (unsigned char *)malloc(olen * sizeof(unsigned char));
   if(odata == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : rgb2ycc_nonintrlv_mem : malloc : odata\n");
      return(-3);
   }
   r_ptr = idata;
   g_ptr = r_ptr+num_pix;
   b_ptr = g_ptr+num_pix;
   y_ptr = odata;
   cb_ptr = y_ptr+num_pix;
   cr_ptr = cb_ptr+num_pix;
   for(i = 0; i < num_pix; i++){

      /* Compute float Y,Cb,Cr. */
      dy = (( 0.299  * (*r_ptr)) +
           ( 0.587  * (*g_ptr)) +
           ( 0.114  * (*b_ptr)));
      dcb = ((-0.1687 * (*r_ptr)) +
           (-0.3313 * (*g_ptr)) +
           ( 0.5    * (*b_ptr)) +
           128.0);
      dcr = (( 0.5    * (*r_ptr)) +
           (-0.4177 * (*g_ptr)) +
           (-0.0813 * (*b_ptr)) +
           128.0);

      /* Round to integer Y,Cb,Cr. */
      iy = sround(dy);
      icb = sround(dcb);
      icr = sround(dcr);

      /* Limit Y,Cb,Cr to [0..255]. */
      iy = (iy < 0) ? 0 : ((iy > 255) ? 255 : iy);
      icb = (icb < 0) ? 0 : ((icb > 255) ? 255 : icb);
      icr = (icr < 0) ? 0 : ((icr > 255) ? 255 : icr);

      /* Store uchar Y,Cb,Cr. */
      *y_ptr = iy;
      *cb_ptr = icb;
      *cr_ptr = icr;

      r_ptr++;
      g_ptr++;
      b_ptr++;
      y_ptr++;
      cb_ptr++;
      cr_ptr++;
   }

   *oodata = odata;
   *oolen = olen;

   return(0);
}

/*****************************************************************/
int downsample_cmpnts(unsigned char **oodata, int *oolen,
               unsigned char *idata,
               const int width, const int height, const int depth,
               int *hor_sampfctr, int *vrt_sampfctr, const int n_cmpnts)
{
   int i, sampling, max_olen, olen, ow, oh;
   int iplane_size, oplane_size;
   unsigned char *odata, *iptr, *optr;
   int max_hor, max_vrt;
   int win_hor[MAX_CMPNTS], win_vrt[MAX_CMPNTS];

   if(n_cmpnts <= 1){
      fprintf(stderr, "ERROR : downsample_cmpnts : ");
      fprintf(stderr, "# of components = %d < 2\n", n_cmpnts);
      return(-2);
   }

   if(!test_evenmult_sampfctrs(&max_hor, &max_vrt,
                               hor_sampfctr, vrt_sampfctr, n_cmpnts)){
      fprintf(stderr, "ERROR : downsample_cmpnts : ");
      fprintf(stderr, "sample factors must be even multiples\n");
      return(-3);
   }

   sampling = 0;
   for(i = 0; i < n_cmpnts; i++){
      win_hor[i] = max_hor / hor_sampfctr[i];
      win_vrt[i] = max_vrt / vrt_sampfctr[i];
      if((win_hor[i] != 1) || (win_vrt[i] != 1))
         sampling = 1;
   }

   max_olen = width * height * n_cmpnts;
   odata = (unsigned char *)malloc(max_olen * sizeof(unsigned char));
   if(odata == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : downsample_cmpnts : malloc : odata\n");
      return(-4);
   }

   /* If no downsampling to be done, then memcpy entire input image        */
   /* and return, rather than do all the pixel addressing and assignments. */
   if(!sampling){
      memcpy(odata, idata, max_olen);
      *oodata = odata;
      *oolen = max_olen;
      return(0);
   }

   /* Foreach component plane ... */
   iptr = idata;
   iplane_size = width * height;
   optr = odata;
   olen = 0;
   for(i = 0; i < n_cmpnts; i++){
      window_avr_plane(optr, &ow, &oh, win_hor[i], win_vrt[i],
                       iptr, width, height);
      iptr += iplane_size;
      oplane_size = ow * oh;
      optr += oplane_size;
      olen += oplane_size;
   }

   *oodata = odata;
   *oolen = olen;

   return(0);
}

/*****************************************************************/
void window_avr_plane(unsigned char *odata, int *oow, int *ooh,
                      const int win_w, const int win_h,
                      unsigned char *idata, const int iw, const int ih)
{
   int ox, oy, ow, oh;
   unsigned char *siptr, *wiptr, *optr;
   int ivrt_offset, last_win_w, last_win_h;

   ow = (int)ceil(iw / (double)win_w);
   oh = (int)ceil(ih / (double)win_h);
   last_win_w = iw % win_w;
   if(last_win_w == 0)
      last_win_w = win_w;
   last_win_h = ih % win_h;
   if(last_win_h == 0)
      last_win_h = win_h;
   ivrt_offset = win_h * iw;

   siptr = idata;
   optr = odata;
   for(oy = 0; oy < oh-1; oy++){
      wiptr = siptr;
      for(ox = 0; ox < ow-1; ox++){
         *optr++ = avr_window(wiptr, win_w, win_h, iw, ih);
         wiptr += win_w;
      }
      *optr++ = avr_window(wiptr, last_win_w, win_h, iw, ih);
      siptr += ivrt_offset;
   }
   wiptr = siptr;
   for(ox = 0; ox < ow-1; ox++){
      *optr++ = avr_window(wiptr, win_w, last_win_h, iw, ih);
      wiptr += win_w;
   }
   *optr++ = avr_window(wiptr, last_win_w, last_win_h, iw, ih);

   *oow = ow;
   *ooh = oh;

   return;
}

/*****************************************************************/
int avr_window(unsigned char *wptr, const int win_w, const int win_h,
               const int img_w, const int img_h)
{
   unsigned char *pptr, *sptr;
   int x, y;
   int win_sum, win_num;
   int win_avr;

   win_num = win_w * win_h;
   win_sum = 0;

   sptr = wptr;
   for(y = 0; y < win_h; y++){
      pptr = sptr;
      for(x = 0; x < win_w; x++){
         win_sum += *pptr++;
      }
      sptr += img_w;
   }

   win_avr = (int)((win_sum / (double)win_num) + 0.5);

   return(win_avr);
}

/*****************************************************************/
int ycc2rgb_mem(unsigned char **odata, int *olen, unsigned char *idata,
                const int width, const int height, const int depth,
                const int intrlvflag)
{
   int ret;

   if(intrlvflag){
      if((ret = ycc2rgb_intrlv_mem(odata, olen, idata, width, height, depth)))
         return(ret);
   }
   else{
      if((ret = ycc2rgb_nonintrlv_mem(odata, olen, idata,
                                     width, height, depth)))
         return(ret);
   }

   return(0);
}

/*****************************************************************/
int ycc2rgb_intrlv_mem(unsigned char **oodata, int *oolen,
                       unsigned char *idata,
                       const int width, const int height, const int depth)
{
   int i, num_pix, olen;
   unsigned char *y_ptr, *cb_ptr, *cr_ptr;
   unsigned char *r_ptr, *g_ptr, *b_ptr;
   unsigned char *odata;
   double dr, dg, db;
   int ir, ig, ib;

   /* If image has empty dimension, then done ... */
   if((width == 0) || (height == 0))
      return(0);

   if(depth != 24){
      fprintf(stderr, "ERROR : ycc2rgb_intrlv_mem : depth = %d != 24\n",
              depth);
      return(-2);
   }

   num_pix = width * height;
   olen = num_pix * (depth>>3);

   odata = (unsigned char *)malloc(olen * sizeof(unsigned char));
   if(odata == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : ycc2rgb_intrlv_mem : malloc : odata\n");
      return(-3);
   }
   for(i = 0,
       y_ptr = idata,
       cb_ptr = idata+1,
       cr_ptr = idata+2,
       r_ptr = odata,
       g_ptr = odata+1,
       b_ptr = odata+2;
       i < num_pix;
       i++,
       y_ptr+=3,
       cb_ptr+=3,
       cr_ptr+=3,
       r_ptr+=3,
       g_ptr+=3,
       b_ptr+=3){

      /* Compute float R,G,B. */
      dr = ((*y_ptr)  + ( 1.402   * ((*cr_ptr) - 128.0)));
      dg = ((*y_ptr)  + (-0.34414 * ((*cb_ptr) - 128.0)) +
                        (-0.71414 * ((*cr_ptr) - 128.0)));
      db = ((*y_ptr)  + ( 1.772   * ((*cb_ptr) - 128.0)));

      /* Compute integer R,G,B. */
      ir = sround(dr);
      ig = sround(dg);
      ib = sround(db);

      /* Limit R,G,B to [0..255]. */
      ir = (ir < 0) ? 0 : ((ir > 255) ? 255 : ir);
      ig = (ig < 0) ? 0 : ((ig > 255) ? 255 : ig);
      ib = (ib < 0) ? 0 : ((ib > 255) ? 255 : ib);

      /* Store uchar R,G,B. */
      *r_ptr = ir;
      *g_ptr = ig;
      *b_ptr = ib;

   }

   *oodata = odata;
   *oolen = olen;

   return(0);
}

/*****************************************************************/
int ycc2rgb_nonintrlv_mem(unsigned char **oodata, int *oolen,
                       unsigned char *idata,
                       const int width, const int height, const int depth)
{
   int i, num_pix, olen;
   unsigned char *y_ptr, *cb_ptr, *cr_ptr;
   unsigned char *r_ptr, *g_ptr, *b_ptr;
   unsigned char *odata;
   double dr, dg, db;
   int ir, ig, ib;

   /* If image has empty dimension, then done ... */
   if((width == 0) || (height == 0))
      return(0);

   if(depth != 24){
      fprintf(stderr, "ERROR : ycc2rgb_nonintrlv_mem : depth = %d != 24\n",
              depth);
      return(-2);
   }

   num_pix = width * height;
   olen = num_pix * (depth>>3);

   odata = (unsigned char *)malloc(olen * sizeof(unsigned char));
   if(odata == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : ycc2rgb_nonintrlv_mem : malloc : odata\n");
      return(-3);
   }

   y_ptr = idata;
   cb_ptr = y_ptr+num_pix;
   cr_ptr = cb_ptr+num_pix;
   r_ptr = odata;
   g_ptr = r_ptr+num_pix;
   b_ptr = g_ptr+num_pix;

   for(i = 0; i < num_pix; i++){

      /* Compute float R,G,B. */
      dr = ((*y_ptr)  + ( 1.402   * ((*cr_ptr) - 128.0)));
      dg = ((*y_ptr)  + (-0.34414 * ((*cb_ptr) - 128.0)) +
                        (-0.71414 * ((*cr_ptr) - 128.0)));
      db = ((*y_ptr)  + ( 1.772   * ((*cb_ptr) - 128.0)));

      /* Compute integer R,G,B. */
      ir = sround(dr);
      ig = sround(dg);
      ib = sround(db);

      /* Limit R,G,B to [0..255]. */
      ir = (ir < 0) ? 0 : ((ir > 255) ? 255 : ir);
      ig = (ig < 0) ? 0 : ((ig > 255) ? 255 : ig);
      ib = (ib < 0) ? 0 : ((ib > 255) ? 255 : ib);

      /* Store uchar R,G,B. */
      *r_ptr = ir;
      *g_ptr = ig;
      *b_ptr = ib;

      y_ptr++;
      cb_ptr++;
      cr_ptr++;
      r_ptr++;
      g_ptr++;
      b_ptr++;
   }

   *oodata = odata;
   *oolen = olen;

   return(0);
}

/*****************************************************************/
int upsample_cmpnts(unsigned char **oodata, int *oolen,
               unsigned char *idata,
               const int width, const int height, const int depth,
               int *hor_sampfctr, int *vrt_sampfctr, const int n_cmpnts)
{
   int i, sampling, olen;
   int iplane_size, oplane_size;
   unsigned char *odata, *iptr, *optr;
   int max_hor, max_vrt;
   int win_hor[MAX_CMPNTS], win_vrt[MAX_CMPNTS];
   int samp_width[MAX_CMPNTS], samp_height[MAX_CMPNTS];

   if(n_cmpnts <= 1){
      fprintf(stderr, "ERROR : upsample_cmpnts : ");
      fprintf(stderr, "# of components = %d < 2\n", n_cmpnts);
      return(-2);
   }

   if(!test_evenmult_sampfctrs(&max_hor, &max_vrt,
                               hor_sampfctr, vrt_sampfctr, n_cmpnts)){
      fprintf(stderr, "ERROR : upsample_cmpnts : ");
      fprintf(stderr, "sample factors must be even multiples\n");
      return(-3);
   }

   sampling = 0;
   for(i = 0; i < n_cmpnts; i++){
      win_hor[i] = max_hor / hor_sampfctr[i];
      win_vrt[i] = max_vrt / vrt_sampfctr[i];
      if((win_hor[i] != 1) || (win_vrt[i] != 1))
         sampling = 1;
      /* Compute the pixel width & height of the component's input plane.  */
      samp_width[i] = (int)ceil(width *
                                (hor_sampfctr[i] / (double)max_hor));
      samp_height[i] = (int)ceil(height *
                                 (vrt_sampfctr[i] / (double)max_vrt));
   }

   olen = width * height * n_cmpnts;
   odata = (unsigned char *)malloc(olen * sizeof(unsigned char));
   if(odata == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : upsample_cmpnts : malloc : odata\n");
      return(-4);
   }

   /* If no upsampling to be done, then memcpy entire input image and  */
   /* return, rather than do all the pixel addressing and assignments. */
   if(!sampling){
      memcpy(odata, idata, olen);
      *oodata = odata;
      *oolen = olen;
      return(0);
   }

   /* Foreach component plane ... */
   iptr = idata;
   optr = odata;
   oplane_size = width * height;
   for(i = 0; i < n_cmpnts; i++){
      window_fill_plane(optr, width, height, win_hor[i], win_vrt[i],
                        iptr, samp_width[i], samp_height[i]);
      iplane_size = samp_width[i] * samp_height[i];
      iptr += iplane_size;
      optr += oplane_size;
   }

   *oodata = odata;
   *oolen = olen;

   return(0);
}

/*****************************************************************/
void window_fill_plane(unsigned char *odata, const int ow, const int oh,
                       const int win_w, const int win_h,
                       unsigned char *idata, const int iw, const int ih)
{
   int ix, iy;
   unsigned char *soptr, *woptr, *iptr;
   int ovrt_offset, last_win_w, last_win_h;

   last_win_w = ow % win_w;
   if(last_win_w == 0)
      last_win_w = win_w;
   last_win_h = oh % win_h;
   if(last_win_h == 0)
      last_win_h = win_h;
   ovrt_offset = win_h * ow;

   soptr = odata;
   iptr = idata;
   for(iy = 0; iy < ih-1; iy++){
      woptr = soptr;
      for(ix = 0; ix < iw-1; ix++){
         fill_window(*iptr, woptr, win_w, win_h, ow, oh);
         iptr++;
         woptr += win_w;
      }
      fill_window(*iptr, woptr, last_win_w, win_h, ow, oh);
      iptr++;
      soptr += ovrt_offset;
   }
   woptr = soptr;
   for(ix = 0; ix < iw-1; ix++){
      fill_window(*iptr, woptr, win_w, last_win_h, ow, oh);
      iptr++;
      woptr += win_w;
   }
   fill_window(*iptr, woptr, last_win_w, last_win_h, ow, oh);

   return;
}

/*****************************************************************/
void fill_window(const unsigned char fillval, unsigned char *wptr,
                const int win_w, const int win_h,
                const int img_w, const int img_h)
{
   unsigned char *pptr, *sptr;
   int x, y;

   sptr = wptr;
   for(y = 0; y < win_h; y++){
      pptr = sptr;
      for(x = 0; x < win_w; x++){
         *pptr++ = fillval;
      }
      sptr += img_w;
   }
}

/*****************************************************************/
/* TRUE if even mulitples, FALSE if not.                         */
/*****************************************************************/
int test_evenmult_sampfctrs(int *omax_hor, int *omax_vrt,
                  int *hor_sampfctr, int *vrt_sampfctr, const int n_cmpnts)
{
   int i, max_hor, max_vrt;

   max_hor = -1;
   max_vrt = -1;
   for(i = 0; i < n_cmpnts; i++){
      if(max_hor < hor_sampfctr[i])
         max_hor = hor_sampfctr[i];
      if(max_vrt < vrt_sampfctr[i])
         max_vrt = vrt_sampfctr[i];
   }

   /* Test for non-even mulitple sample factors ... */
   for(i = 0; i < n_cmpnts; i++){
      if((max_hor % hor_sampfctr[i]) ||
         (max_vrt % vrt_sampfctr[i])){
         return(0);
      }
   }

   *omax_hor = max_hor;
   *omax_vrt = max_vrt;

   return(1);
}
