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
      LIBRARY: FING - NIST Fingerprint Systems Utilities

      FILE:           NFSEG.C
      ALGORITHM:      Craig I. Watson
      DATE:           09/20/2004
      UPDATED:        03/11/2005 by MDG
      MODIFIED:       05/13/2008 by Joseph C. Konczal - added write SEG/ASEG
      UPDATED:        10/09/2008 by JCK

      Contains routines responsible for supporting
      NFSEG (NIST Fingerprint Segmentation) algorithm

***********************************************************************

      ROUTINES:
#cat: segment_fingers - Segments a single finger (removes white space) or
#cat:                   four-finger plain impression fingerprint image into
#cat:                   single finger images.
#cat: dynamic_threshold - Thresholds image, adjusting threshold based on
#cat:                     # of pixels below mean pixel value
#cat: remove_lines - Removes black lines from edge of image caused by 
#cat:                a fingerprint card.
#cat: accum_blk_wht - Counts number of black or white pixels along a line
#cat:                 of a given width, defined by BLK_DW and WHT_DW in nfseg.h
#cat: find_digits - Finds four "best" (based on a normalized score) fingers
#cat:               in the four-finger plain impression by searching over a
#cat:               range of angles.
#cat: find_digit_edges - Refine the edge locations between the fingers.
#cat: get_edge_coords - Convert edge locations to coordinates in the image.
#cat: get_fing_boxes - Compute the box coordinates of all four fingers.
#cat: get_fing_seg_pars - Compute the center (sx,sy) and dimensions (sw,sh)
#cat:                     of the four segmented fingers.
#cat: get_segfing_bounds - Fine tune image to minimize size and only include
#cat:                      fingerprint in the image.
#cat: accum_top_row - Counts white along rows. Used to detect top of
#cat:                 fingerprint.
#cat: accum_top_col_blk - Counts black pixels along columns. Used to
#cat:                     detect top of fingerprint.
#cat: accum_top_col_wht - Counts white pixels along columns. Used to
#cat:                     detect top of fingerprint.
#cat: get_top_score - Detects top of fingerprint from a normalized score.
#cat: adjust_top_up - Adjust location of top (especially if the print goes off
#cat:                 the top of the image.
#cat: find_segfing_bottom - Detects bottom of fingerprint.
#cat: find_segfing_sides - Detects sides of the fingerprint.
#cat: adjust_fing_seg_pars - Adjust parameters based on image rotation.
#cat: err_check_finger - Make checks of segmented width, height and spacing.
#cat: scale_seg_fingers - Scale parameters back to original image size.
#cat: parse_segfing - Parse fingers from original image data.
#cat: new_fgp - Calculate the new fgp for part of a segmented fingerprint 
#cat:           image.
#cat: write_parsefing - Write segmented/parsed fingers to files.
#cat: insert_int_item - Insert an integer item into an ANSI NIST file.
#cat: insert_parsefing - Add segmented/parsed fingers to a SEG or ASEG
#cat:                    field in a Type-14 record, with a comment
#cat:                    indicating the source.
***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <img_io.h>
#include <ioutil.h>
#include <an2k.h>
#include <nfseg.h>
#include <version.h>

/***********************************************************************
************************************************************************
#cat: segment_fingers - Segments a single finger (removes white space)
#cat:              or four-finger plain impression fingerprint image
#cat:              into single finger images.

   Input:
      idata      - image data
      iw         - image width (pixels)
      ih         - image height (pixels)
      nf         - number of fingers expected
      fgp        - finger position, same as an2k
      bthr_adj   - boolean - adjust binarization threshold?
      rot_search - boolean - search for rotated fingerprints?
   Output:
      ofing_boxes -

   Return Code:
      Zero       - successful completion
      Negative   - system error
************************************************************************/
int segment_fingers(unsigned char *idata, const int iw, const int ih,
         seg_rec_coords **ofing_boxes, const int nf, const int fgp,
         const int bthr_adj, const int rot_search)
{
   unsigned char *fdata;
   int w, h;
   seg_rec_coords *fing_boxes;
   int i, ret;
   int *digits, *bst_digits;
   float score, bst_score;
   int offset, bst_offset;
   int *bacc, *baccn, abn;
   int *wacc, *waccn, awn;
   int *digit_edges, *bst_digit_edges;
   float theta;
   line_coords *edges;
   int mn_off, mx_off;

   if(rot_search) {
      if(fgp == 13) {
         mn_off = -OFF_DW2;
         mx_off = OFF_DW1;
      }
      else if(fgp == 14) {
         mn_off = -OFF_DW1;
         mx_off = OFF_DW2;
      }
      else {
         mn_off = -OFF_DW1;
         mx_off = OFF_DW1;
      }
   }
   else {
      mn_off = 0;
      mx_off = 0;
   }

   /* DOWN SAMPLE IMAGE BY Z_FAC */
   if((ret = average_blk(idata, iw, ih, 1.0/Z_FAC, 1.0/Z_FAC, &fdata, &w, &h)))
      return(ret);
   if((ret = dynamic_threshold(fdata, w, h, 1, nf, bthr_adj))) {
      free(fdata);
      return(ret);
   }
   remove_lines(fdata, w, h);

   fing_boxes = (seg_rec_coords *)calloc(nf,sizeof(seg_rec_coords));
   if(fing_boxes == (seg_rec_coords *)NULL) {
      fprintf(stderr, "ERROR: calloc: segment_fingers fing_boxes\n");
      free(fdata);
      return(-2);
   }

   for(i = 0; i < nf; i++)
      fing_boxes[i].err = 0;

   if(nf > 1) {
      if((ret = malloc_int_ret(&digits, nf, "segment_fingers digits"))) {
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
      if((ret = malloc_int_ret(&bst_digits, nf, "segment_fingers bst_digits"))) {
         free(digits);
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
      if((ret = malloc_int_ret(&digit_edges, nf+1,
                              "segment_fingers digits_edges"))) {
         free(bst_digits);
         free(digits);
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
      if((ret = malloc_int_ret(&bst_digit_edges, nf+1,
                              "segment_fingers bst_digits_edges"))) {
         free(digit_edges);
         free(bst_digits);
         free(digits);
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
   
      bst_score = -1.0;
      bst_offset = 0;  /* Added per CG by MDG */
      for(offset = mn_off; offset <= mx_off; offset += OFF_STP) {
         if((ret = accum_blk_wht(fdata, w, h, &bacc, &baccn, &abn, offset,
                                BLK_DW, 0))) {
            free(bst_digit_edges);
            free(digit_edges);
            free(bst_digits);
            free(digits);
            free(fing_boxes);
            free(fdata);
            return(ret);
         }
         if((ret = accum_blk_wht(fdata, w, h, &wacc, &waccn, &awn, offset,
                                WHT_DW, 1))){
            free(bacc);
            free(baccn);
            free(bst_digit_edges);
            free(digit_edges);
            free(bst_digits);
            free(digits);
            free(fing_boxes);
            free(fdata);
            return(ret);
         }
         if((ret = find_digits(bacc, baccn, abn, wacc, waccn, awn, &score,
                              digits, digit_edges, nf))) {
            free(wacc);
            free(waccn);
            free(bacc);
            free(baccn);
            free(bst_digit_edges);
            free(digit_edges);
            free(bst_digits);
            free(digits);
            free(fing_boxes);
            free(fdata);
            return(ret);
         }
         free(wacc);
         free(waccn);
         free(bacc);
         free(baccn);

         if(score > bst_score) {
            bst_score = score;
            bst_offset = offset;
            for(i = 0; i < nf; i++)
               bst_digits[i] = digits[i];
            for(i = 0; i < nf+1; i++)
               bst_digit_edges[i] = digit_edges[i];
         }
      }
      free(digit_edges);
      free(digits);

      theta = (float)atan2((double)bst_offset, (double)(h-1));

      edges = (line_coords *)calloc((nf+1),sizeof(line_coords));
      if(edges == (line_coords *)NULL) {
         fprintf(stderr, "ERROR: segment_fingers: allocating edges.\n");
         free(bst_digit_edges);
         free(bst_digits);
         free(fing_boxes);
         free(fdata);
         return(-3);
      }
      if((ret = get_edge_coords(w, h, bst_digit_edges, bst_offset,
                               edges, nf+1))) {
         free(edges);
         free(bst_digit_edges);
         free(bst_digits);
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
      free(bst_digit_edges);
      if((ret = get_fing_boxes(w, h, theta, edges, nf+1, fing_boxes, nf))) {
         free(edges);
         free(bst_digits);
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
      free(edges);
      get_fing_seg_pars(theta, fing_boxes, nf);
      if((ret = get_segfing_bounds(fdata, w, h, fing_boxes, nf))) {
         free(bst_digits);
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
      free(fdata);

      /* CONVERT SEGMENTATION COORDINATES BASED ON ROTATION ANGLE */
      /* AND ADJUSTED TOP/BOTTOM/SIDES                            */
      adjust_fing_seg_pars(fing_boxes, nf);

      err_check_finger(bst_digits, fing_boxes, nf);
      free(bst_digits);
   
      /* SCALE DIMENSIONS OF LOCATED FINGERS BACK TO ORIGINAL IMAGE SIZE */
      scale_seg_fingers(fing_boxes, nf, iw, ih, Z_FAC);
   }
   else {
      fing_boxes[0].tlx = 0;
      fing_boxes[0].tly = 0;
      fing_boxes[0].tRightX = w-1;
      fing_boxes[0].tRightY = 0;
      fing_boxes[0].blx = 0;
      fing_boxes[0].bly = h-1;
      fing_boxes[0].brx = w-1;
      fing_boxes[0].bry = h-1;
      fing_boxes[0].sx = w/2;
      fing_boxes[0].sy = h/2;
      fing_boxes[0].sw = w;
      fing_boxes[0].sh = h;
      fing_boxes[0].theta = 0.0;
      fing_boxes[0].err = 0;
      if((ret = get_segfing_bounds(fdata, w, h, fing_boxes, nf))) {
         free(fing_boxes);
         free(fdata);
         return(ret);
      }
      free(fdata);
      err_check_finger((int *)NULL, fing_boxes, nf);

      fing_boxes[0].sw = fing_boxes[0].drx - fing_boxes[0].dlx;
      fing_boxes[0].sh = fing_boxes[0].dby - fing_boxes[0].dty;
      fing_boxes[0].sx = fing_boxes[0].dlx + (fing_boxes[0].sw/2);
      fing_boxes[0].sy = fing_boxes[0].dty + (fing_boxes[0].sh/2);
      fing_boxes[0].tlx = fing_boxes[0].dlx;
      fing_boxes[0].tly = fing_boxes[0].dty;
      fing_boxes[0].blx = fing_boxes[0].dlx;
      fing_boxes[0].bly = fing_boxes[0].dby;
      fing_boxes[0].tRightX = fing_boxes[0].drx;
      fing_boxes[0].tRightY = fing_boxes[0].dty;
      fing_boxes[0].brx = fing_boxes[0].drx;
      fing_boxes[0].bry = fing_boxes[0].dby;
      scale_seg_fingers(fing_boxes, nf, iw, ih, Z_FAC);
   }

   *ofing_boxes = fing_boxes;
   return(0);
}

/*******************************************************************/
int dynamic_threshold(unsigned char *fdata, const int w, const int h,
                       const int bval, const int nf, const int bthr_adj)
{
   int j, tpnt, bcnt;
   int bcnt_thr;

   bcnt_thr = (int)(0.4 * (float)(w*h));
   tpnt = 0;
   for(j=0; j<w*h; j++)
      tpnt += fdata[j];
   tpnt /= w*h;

   if(bthr_adj) {
      if(nf == 1)
         tpnt += 5;
      else {
         /* FOUND THAT IF ADJUSTED SLIGHTLY BASED ON PIXEL COUNT
            WORKS BETTER FOR INKED PAPER*/
         bcnt = 0;
         for(j = 0; j < w*h; j++)
            if(fdata[j] < tpnt)
               bcnt++;


         if(bcnt == 0) {
            fprintf(stderr, "Error: Blank Image\n");
            return(-1000);
         }
         if(bcnt < bcnt_thr)
            tpnt += 5;
         else
            tpnt -= 5;
      }
   }

   /* BINARIZE IMAGE */
   thresh_charimage(fdata, w, h, tpnt, bval);

   /* CONVERT BINARIZED IMAGE FROM 0:1 to 0:255 */
   for(j = 0; j < w*h; j++)
      if(fdata[j])
         fdata[j] = 255;
      else
         fdata[j] = 0;

   return(0);
}

/*******************************************************************/
void remove_lines(unsigned char *fdata, const int w, const int h)
{
   int i, j, bcnt;

   for(i = 0; i < w/6; i++) {
      bcnt = 0;
      for(j = 0; j < h; j++) {
         if(fdata[i+j*w] == 0)
            bcnt++;
      }
      if(bcnt > h-5) {
         for(j = 0; j < h; j++)
            fdata[i+j*w] = 255;
      }
      else
         i = w;
   }
   for(i = w-1; i > w-(w/6); i--) {
      bcnt = 0;
      for(j = 0; j < h; j++) {
         if(fdata[i+j*w] == 0)
            bcnt++;
      }
      if(bcnt > h-5) {
         for(j = 0; j < h; j++)
            fdata[i+j*w] = 255;
      }
      else
         i = 0;
   }
}

/*******************************************************************/
int accum_blk_wht(unsigned char *fdata, const int w, const int h,
         int **oacc, int **oaccn, int *oan, const int offset, const int lndw,
         const int blkwht)
{
   int ret;
   int *acc, *accn, an;
   int x, *xp, *yp, np, ap;
   int i, n, k;
   int lx, rx;

   if(offset < 0) {
      lx = 0;
      rx = w - offset;
   }
   else {
      lx = -1 * offset;
      rx = w;
   }

   an = rx - lx;
   if((ret = calloc_int_ret(&acc, an, "accum_blk_wht")))
      return(ret);
   if((ret = calloc_int_ret(&accn, an, "accum_blk_wht"))) {
      free(acc);
      return(ret);
   }

   np = ap = 0;
   k = 0;
   if(blkwht)
      for(i = lx; i < rx; i++) {
         if((ret = bres_line_alloc(i, 0, i+offset, h-1, &xp, &yp, &np, &ap))) {
            free(accn);
            free(acc);
            return(ret);
         }
         for(n = 0; n < np; n++) {
            for(x = xp[n]-lndw; x <= xp[n]+lndw; x++)
               if(x >= 0 && x < w)
                  if(yp[n] >= 0 && yp[n] < h) {
                     accn[k]++;
                     if(fdata[x+yp[n]*w])
                        acc[k]++;
                  }
         }
         k++;
      }
   else
      for(i = lx; i < rx; i++) {
         if((ret = bres_line_alloc(i, 0, i+offset, h-1, &xp, &yp, &np, &ap))) {
            free(accn);
            free(acc);
            return(ret);
         }
         for(n = 0; n < np; n++) {
            for(x = xp[n]-lndw; x <= xp[n]+lndw; x++)
               if(x >= 0 && x < w)
                  if(yp[n] >= 0 && yp[n] < h) {
                     accn[k]++;
                     if(!fdata[x+yp[n]*w])
                        acc[k]++;
                  }
         }
         k++;
      }

   free(xp);
   free(yp);
   *oacc = acc;
   *oaccn = accn;
   *oan = an;
   return(0);
}

/*******************************************************************/
int find_digits(int *bacc, int *baccn, const int abn, int *wacc, int *waccn,
                 const int awn, float *score, int *digits, int *digit_edges,
                 const int nf)
{
   int i, n, ret;
   int *tdigits;
   float escore;

   *score = 0.0;
   for(i = 0; i < nf; i++) {
      digits[i] = 0;
      for(n = 0; n < abn; n++)
         if(bacc[n] > bacc[digits[i]])
            digits[i] = n;
      *score += (float)bacc[digits[i]]/(float)baccn[digits[i]];

      for(n = digits[i]-ZERO_DW; n < digits[i]+ZERO_DW; n++)
         if(n >= 0 && n < abn)
            bacc[n] = 0;
   }

   if((ret = malloc_int_ret(&tdigits, nf, "find_digits tdigits")))
      return(ret);

   bubble_sort_int(digits, nf);
   for(i = 0; i < nf; i++)
      tdigits[i] = digits[nf-1-i];
   for(i = 0; i < nf; i++)
      digits[i] = tdigits[i];
   free(tdigits);

   find_digit_edges(digits, nf, wacc, waccn, awn, digit_edges, &escore);
   *score += escore;
   return(0);
}

/*******************************************************************/
void find_digit_edges(int *digits, const int nf, int *wacc, int *waccn,
          const int awn, int *digit_edges, float *oscore)
{
   int i, n;
   float score, bst_score, tscore;
   int dx1, dx2;

   tscore = 0.0;
   for(i = 1; i < nf; i++) {
      bst_score = -1.0;
      digit_edges[i] = digits[i-1];
      for(n = digits[i-1]+FIN_DW; n < digits[i]-FIN_DW; n++)
         if(n >= 0 && n < awn) {
            score = (float)wacc[n]/(float)waccn[n];
            if(score > bst_score) {
               bst_score = score;
               digit_edges[i] = n;
            }
         }
      tscore += bst_score;
   }
   *oscore = tscore;

   dx1 = digit_edges[1] - digits[0];
   dx2 = dx1 + EDGE_DW;
   bst_score = -1.0;
   digit_edges[0] = digits[0]-dx1;
   for(n = digits[0]-dx1; n >= digits[0]-dx2; n--) {
      if(n >= 0) {
         score = (float)wacc[n]/(float)waccn[n];
         if(score > bst_score) {
            bst_score = score;
            digit_edges[0] = n;
         }
      }
   }
   if(digit_edges[0] < 0)
      digit_edges[0] = 0;

   dx1 = digits[i-1] - digit_edges[i-1];
   dx2 = dx1 + EDGE_DW;
   bst_score = -1.0;
   digit_edges[i] = digits[i-1]+dx1;
   for(n = digits[i-1]+dx1; n < digits[i-1]+dx2; n++) {
      if(n < awn ) {
         score = (float)wacc[n]/(float)waccn[n];
         if(score > bst_score) {
            bst_score = score;
            digit_edges[i] = n;
         }
      }
   }
   if(digit_edges[i] >= awn)
      digit_edges[i] = awn-1;

}

/*******************************************************************/
int get_edge_coords(const int w, const int h, int *digit_edges,
          const int bst_offset, line_coords *edges, const int dn)
{
   int *xp, *yp, np, ap;
   int i, n, ret;

   np = ap = 0;
   for(i = 0; i < dn; i++) {
      if(bst_offset < 0) {
         if((ret = bres_line_alloc(digit_edges[i], 0, digit_edges[i]+bst_offset,
				   h-1, &xp, &yp, &np, &ap)))
            return(ret);
      }
      else {
         if((ret = bres_line_alloc(digit_edges[i]-bst_offset, 0, digit_edges[i],
				   h-1, &xp, &yp, &np, &ap)))
            return(ret);
      }
      for(n = 0; n < np; n++)
         if(xp[n] >= 0 && xp[n] < w)
            if(yp[n] >= 0 && yp[n] < h) {
               edges[i].tx = xp[n];
               edges[i].ty = yp[n];
               n = np;
            }
      for(n = np-1; n >= 0; n--)
         if(xp[n] >= 0 && xp[n] < w)
            if(yp[n] >= 0 && yp[n] < h) {
               edges[i].bx = xp[n];
               edges[i].by = yp[n];
               n = -1;
            }
   }

   free(xp);
   free(yp);
   return(0);
}

/*******************************************************************/
int get_fing_boxes(const int w, const int h, const float theta,
          line_coords *edges, const int ne,
          seg_rec_coords *fing_boxes, const int nf)
{
   int i;
   float ct, st;
   float dx1, dy1;
   float dl, dr, dlx, dly, drx, dry;

   if(ne != nf+1) {
      fprintf(stderr, "ERROR: get_fing_boxes: ne != nf+1\n");
      fprintf(stderr, "ne {%d} should be 1 larger than nf {%d}\n", ne, nf);
      return(-2);
   }
   for(i = 0; i < nf; i++)
      fing_boxes[i].theta = theta;

   if(theta == 0.0) {
      for(i = 0; i < nf; i++) {
         fing_boxes[i].tlx = edges[i].tx;
         fing_boxes[i].tly = edges[i].ty;
         fing_boxes[i].blx = edges[i].bx;
         fing_boxes[i].bly = edges[i].by;
         fing_boxes[i].tRightX = edges[i+1].tx;
         fing_boxes[i].tRightY = edges[i+1].ty;
         fing_boxes[i].brx = edges[i+1].bx;
         fing_boxes[i].bry = edges[i+1].by;
      }
   }
   else if (theta > 0.0) {
      ct = cos(theta);
      st = sin(theta);
      for(i = 0; i < nf; i++) {
/* TOP */
         dx1 = (float)edges[i+1].tx - (float)edges[i].tx;
         dy1 = (float)edges[i].ty - (float)edges[i+1].ty;
         dl = dy1 * ct;
         dr = dx1 * st;
         dly = dl * ct;
         dlx = dl * st;
         dry = dr * ct;
         drx = dr * st;
         fing_boxes[i].tlx = edges[i].tx - sround(dlx);
         fing_boxes[i].tly = edges[i].ty - sround(dly);
         fing_boxes[i].tRightX = edges[i+1].tx - sround(drx);
         fing_boxes[i].tRightY = edges[i+1].ty - sround(dry);
/* BOTTOM */
         dx1 = (float)edges[i+1].bx - (float)edges[i].bx;
         dy1 = (float)edges[i].by - (float)edges[i+1].by;
         dl = dx1 * st;
         dr = dy1 * ct;
         dly = dl * ct;
         dlx = dl * st;
         dry = dr * ct;
         drx = dr * st;
         fing_boxes[i].blx = edges[i].bx + sround(dlx);
         fing_boxes[i].bly = edges[i].by + sround(dly);
         fing_boxes[i].brx = edges[i+1].bx + sround(drx);
         fing_boxes[i].bry = edges[i+1].by + sround(dry);
      }
   }
   else {
      ct = cos(-theta);
      st = sin(-theta);
      for(i = 0; i < nf; i++) {
/* TOP */
         dx1 = (float)edges[i+1].tx - (float)edges[i].tx;
         dy1 = (float)edges[i+1].ty - (float)edges[i].ty;
         dl = dx1 * st;
         dr = dy1 * ct;
         dly = dl * ct;
         dlx = dl * st;
         dry = dr * ct;
         drx = dr * st;
         fing_boxes[i].tlx = edges[i].tx + sround(dlx);
         fing_boxes[i].tly = edges[i].ty - sround(dly);
         fing_boxes[i].tRightX = edges[i+1].tx + sround(drx);
         fing_boxes[i].tRightY = edges[i+1].ty - sround(dry);
/* BOTTOM */
         dx1 = (float)edges[i+1].bx - (float)edges[i].bx;
         dy1 = (float)edges[i+1].by - (float)edges[i].by;
         dl = dy1 * ct;
         dr = dx1 * st;
         dly = dl * ct;
         dlx = dl * st;
         dry = dr * ct;
         drx = dr * st;
         fing_boxes[i].blx = edges[i].bx - sround(dlx);
         fing_boxes[i].bly = edges[i].by + sround(dly);
         fing_boxes[i].brx = edges[i+1].bx - sround(drx);
         fing_boxes[i].bry = edges[i+1].by + sround(dry);
      }
   }
   return(0);
}

/*******************************************************************/
void get_fing_seg_pars(const float theta,
          seg_rec_coords *fing_boxes, const int nf)
{
   int i;
   float dxt, dyt;
   float dx, dy;
   float d;
   float theta2;

   for(i = 0; i < nf; i++) {
      if(theta == 0.0) {
         fing_boxes[i].sw = fing_boxes[i].tRightX - fing_boxes[i].tlx;
         fing_boxes[i].sh = fing_boxes[i].bly - fing_boxes[i].tly;
         fing_boxes[i].sx = fing_boxes[i].tlx + (fing_boxes[i].sw/2);
         fing_boxes[i].sy = fing_boxes[i].tly + (fing_boxes[i].sh/2);
      }
      else {
         fing_boxes[i].theta = theta;
         dxt = (float)(fing_boxes[i].tRightX - fing_boxes[i].tlx);
         dyt = (float)(fing_boxes[i].tRightY - fing_boxes[i].tly);
         dx = sqrt((dxt*dxt) + (dyt*dyt));
         fing_boxes[i].sw = sround(dx);
         dxt = (float)(fing_boxes[i].blx - fing_boxes[i].tlx);
         dyt = (float)(fing_boxes[i].bly - fing_boxes[i].tly);
         dy = sqrt((dxt*dxt) + (dyt*dyt));
         fing_boxes[i].sh = sround(dy);

         dx /= 2.0;
         dy /= 2.0;
         theta2 = atan(dx/dy);
         theta2 += fabs(theta);
         d = sqrt((dx*dx) + (dy*dy));
         dxt = d * sin(theta2);
         dyt = d * cos(theta2);
         if(theta > 0.0) {
            fing_boxes[i].sx = fing_boxes[i].tlx + sround(dxt);
            fing_boxes[i].sy = fing_boxes[i].tly + sround(dyt);
         }
         else {
            fing_boxes[i].sx = fing_boxes[i].tRightX - sround(dxt);
            fing_boxes[i].sy = fing_boxes[i].tRightY + sround(dyt);
         }
      }
   }
}

/*******************************************************/
int get_segfing_bounds(unsigned char *fdata, const int w, const int h,
          seg_rec_coords *fing_boxes, const int n)
{
   int ret, i, j, sw, sh, cenoff;
   unsigned char *sdata, *tdata, *gdata;
   int *twlrow, *twcrow, *twrrow, ntw;
   int *blcenter, *bccenter, *brcenter, bn;
   int *wlcenter, *wccenter, *wrcenter, wn;

   for(i = 0; i < n; i++) {
      sw = fing_boxes[i].sw;
      sh = fing_boxes[i].sh;
      if((ret = malloc_uchar_ret(&sdata, sw*sh, "get_segfing_bounds sdata")))
         return(ret);
      snip_rot_subimage(fdata, w, h, sdata, sw, sh, fing_boxes[i].sx,
                        fing_boxes[i].sy, fing_boxes[i].theta, 128);

      if((ret = accum_top_row(sdata, sw, sh, &twlrow, &twcrow, &twrrow, &ntw))) {
         free(sdata);
         return(ret);
      }
      if((ret = accum_top_col_blk(sdata, sw, sh, &blcenter, &bccenter,
                                 &brcenter, &bn))) {
         free(twlrow);
         free(twcrow);
         free(twrrow);
         free(sdata);
         return(ret);
      }
      if((ret = accum_top_col_wht(sdata, sw, sh, &wlcenter, &wccenter,
                                 &wrcenter, &wn))) { 
         free(blcenter);
         free(bccenter);
         free(brcenter);
         free(twlrow);
         free(twcrow);
         free(twrrow);
         free(sdata);
         return(ret);
      }
      if((ret = get_top_score(&(fing_boxes[i].dty), sw, sh, twlrow, twcrow,
                             twrrow, ntw, blcenter, bccenter, brcenter, bn,
                             wlcenter, wccenter, wrcenter, wn, &cenoff))) {
         free(wlcenter);
         free(wccenter);
         free(wrcenter);
         free(blcenter);
         free(bccenter);
         free(brcenter);
         free(twlrow);
         free(twcrow);
         free(twrrow);
         free(sdata);
         return(ret);
      }
      free(wlcenter);
      free(wccenter);
      free(wrcenter);
      free(blcenter);
      free(bccenter);
      free(brcenter);
      free(twlrow);
      free(twcrow);
      free(twrrow);
      
      if((ret = malloc_uchar_ret(&gdata, sw*sh, "get_segfing_bounds gdata"))) {
         free(sdata);
         return(ret);
      }
      for(j = 0; j < sw*sh; j++)
         if(sdata[j])
            gdata[j] = 0;
         else
            gdata[j] = 1;

      if((ret = dilate_charimage(gdata, &tdata, sw, sh))) {
         free(gdata);
         free(sdata);
         return(ret);
      }
      if(n == 1) {
         free(gdata);
         if((ret = dilate_charimage(tdata, &gdata, sw, sh))) {
            free(tdata);
            free(sdata);
            return(ret);
         }
         free(tdata);
      }
      else {
         free(gdata);
         gdata = tdata;
      }

      for(j = 0; j < sw*sh; j++) {
         if(gdata[j])
            gdata[j] = 0;
         else if(sdata[j] == 128)
            gdata[j] = 128;
         else
            gdata[j] = 255;
      }
      free(sdata);

      if(n == 1) {
         if((ret = adjust_top_up(&(fing_boxes[i].dty), gdata, sw, sh, cenoff,
                                10))) {
            free(gdata);
            return(ret);
         }
         find_segfing_bottom(fing_boxes, i, gdata, sw, sh, cenoff, 600, 0.7);
      }
      else {
         if((ret = adjust_top_up(&(fing_boxes[i].dty), gdata, sw, sh, cenoff,
                                15))) {
            free(gdata);
            return(ret);
         }
         find_segfing_bottom(fing_boxes, i, gdata, sw, sh, cenoff, 600, 0.25);
      }
      find_segfing_sides(fing_boxes, i, gdata, sw, sh, cenoff);

      free(gdata);
   }
   return(0);
}

/*******************************************************/
int accum_top_row(unsigned char *sdata, const int sw, const int sh,
          int **otwlrow, int **otwcrow, int **otwrrow, int *ntw)
{
   int x, y, ret;
   int xl, xc, xr;
   int *twlrow, *twcrow, *twrrow;
   unsigned char *sptr1;

   if((ret = calloc_int_ret(&twcrow, sh, "accum_top_row twcrow")))
      return(ret);
   if((ret = calloc_int_ret(&twlrow, sh, "accum_top_row twlrow"))) {
      free(twcrow);
      return(ret);
   }
   if((ret = calloc_int_ret(&twrrow, sh, "accum_top_row twrrow"))) {
      free(twcrow);
      free(twlrow);
      return(ret);
   }
   sptr1 = sdata;
   /* IF xl, xc, or xr ARE CHANGED dy in get_top_score MAY NEED CHANGED */
   xl = sw/4;
   xc = sw/2;
   xr = xc+xl;
   for(y = 0; y < sh; y++) {
      for(x = 0; x < sw; x++) {
         if(*sptr1++) {
            if(x < xc) {
               twlrow[y]++;
               if(x > xl)
                  twcrow[y]++;
            }
            else {
               twrrow[y]++;
               if(x < xr)
                  twcrow[y]++;
            }
         }
      }
   }

   *otwlrow = twlrow;
   *otwcrow = twcrow;
   *otwrrow = twrrow;
   *ntw = sh;
   return(0);
}

/*******************************************************/
int accum_top_col_blk(unsigned char *sdata, const int sw, const int sh,
          int **oblcenter, int **obccenter, int **obrcenter, int *bn)
{
   int j, x, y, ret;
   int dx, dy;
   int *blcenter, *bccenter, *brcenter;
   int loff, coff, roff;
   unsigned char *slptr1, *slptr2;
   unsigned char *scptr1, *scptr2;
   unsigned char *srptr1, *srptr2;

   /* IF dy IS CHANGED dy in get_top_score MUST BE CHANGED */
   dy = sw/2;
   dx = BLK_DW/2;
   if(dx > sw/4)
      dx = sw/4;

   loff = sw/4;
   coff = sw/2;
   roff = loff+coff;
   if((ret = calloc_int_ret(&blcenter, sh, "accum_top_col_blk blcenter")))
      return(ret);
   if((ret = calloc_int_ret(&bccenter, sh, "accum_top_col_blk bccenter"))) {
      free(blcenter);
      return(ret);
   }
   if((ret = calloc_int_ret(&brcenter, sh, "accum_top_col_blk brcenter"))) {
      free(bccenter);
      free(blcenter);
      return(ret);
   }
   for(j = 0; j < sh-dy; j++) {
      slptr1 = sdata + (loff - dx) + (j * sw);
      scptr1 = sdata + (coff - dx) + (j * sw);
      srptr1 = sdata + (roff - dx) + (j * sw);
      for(y = j; y < j+dy; y++) {
         slptr2 = slptr1;
         scptr2 = scptr1;
         srptr2 = srptr1;
         for(x = 0; x < BLK_DW; x++) {
            if(!(*slptr2++))
               blcenter[j]++;
            if(!(*scptr2++))
               bccenter[j]++;
            if(!(*srptr2++))
               brcenter[j]++;
         }
         slptr1 += sw;
         scptr1 += sw;
         srptr1 += sw;
      }
   }

   *oblcenter = blcenter;
   *obccenter = bccenter;
   *obrcenter = brcenter;
   *bn = sh;
   return(0);
}

/*******************************************************/
int accum_top_col_wht(unsigned char *sdata, const int sw, const int sh,
          int **owlcenter, int **owccenter, int **owrcenter, int *wn)
{
   int j, x, y, ret;
   int dx, dy;
   int *wlcenter, *wccenter, *wrcenter;
   int loff, coff, roff;
   unsigned char *slptr1, *slptr2;
   unsigned char *scptr1, *scptr2;
   unsigned char *srptr1, *srptr2;

   /* IF dy IS CHANGED dy in get_top_score MUST BE CHANGED */
   dy = sw/4;
   dx = BLK_DW/2;
   if(dx > sw/4)
      dx = sw/4;

   loff = sw/4;
   coff = sw/2;
   roff = loff+coff;
   if((ret = calloc_int_ret(&wlcenter, sh, "accum_top_col_wht wlcenter")))
      return(ret);
   if((ret = calloc_int_ret(&wccenter, sh, "accum_top_col_wht wccenter"))) {
      free(wlcenter);
      return(ret);
   }
   if((ret = calloc_int_ret(&wrcenter, sh, "accum_top_col_wht wrcenter"))) {
      free(wccenter);
      free(wlcenter);
      return(ret);
   }
   for(j = 0; j < sh-dy-W_STP; j++) {
      slptr1 = sdata + (loff - dx) + (j * sw);
      scptr1 = sdata + (coff - dx) + (j * sw);
      srptr1 = sdata + (roff - dx) + (j * sw);
      for(y = j; y < j+dy; y++) {
         slptr2 = slptr1;
         scptr2 = scptr1;
         srptr2 = srptr1;
         for(x = 0; x < BLK_DW; x++) {
            if(*slptr2++)
               wlcenter[j+dy+W_STP]++;
            else
               wlcenter[j+dy+W_STP] -= 2;
            if(*scptr2++)
               wccenter[j+dy+W_STP]++;
            else
               wccenter[j+dy+W_STP] -= 2;
            if(*srptr2++)
               wrcenter[j+dy+W_STP]++;
            else
               wrcenter[j+dy+W_STP] -= 2;
         }
         slptr1 += sw;
         scptr1 += sw;
         srptr1 += sw;
      }
   }

   *owlcenter = wlcenter;
   *owccenter = wccenter;
   *owrcenter = wrcenter;
   *wn = sh;
   return(0);
}

/*******************************************************/
int get_top_score(int *dty, const int sw, const int sh, int *twlrow,
                   int *twcrow, int *twrrow, const int ntw, int *blcenter,
                   int *bccenter, int *brcenter, const int bn, int *wlcenter,
                   int *wccenter, int *wrcenter, const int wn, int *cenoff)
{
   int y, y2, dy, ylim, blim;
   int *topl, *topc, *topr;
   int tscore, ret;

   /* IF dy IS CHANGED SCALE FACTORS 2 and 1.333333 MUST BE CHANGED */
   dy = sw/4;
   blim = sround(0.98 * (float)((sw/2)*BLK_DW));
   if((ret = calloc_int_ret(&topl, sh, "get_top_score topl")))
      return(ret);
   if((ret = calloc_int_ret(&topc, sh, "get_top_score topc"))) {
      free(topl);
      return(ret);
   }
   if((ret = calloc_int_ret(&topr, sh, "get_top_score topr"))) {
      free(topl);
      free(topc);
      return(ret);
   }
   for(y = 0; y < W_STP; y++) {
      topl[y] += sround(2 * (float)blcenter[y]);
      topc[y] += sround(2 * (float)bccenter[y]);
      topr[y] += sround(2 * (float)brcenter[y]);
   }
   for(y = W_STP; y < W_STP+dy; y++) {
      for(y2 = y-W_STP; y2 < y; y2++) {
         topl[y] += twlrow[y2];
         topc[y] += twcrow[y2];
         topr[y] += twrrow[y2];
      }
      topl[y] = sround(1.333333 * (float)(blcenter[y]+topl[y]));
      topc[y] = sround(1.333333 * (float)(bccenter[y]+topc[y]));
      topr[y] = sround(1.333333 * (float)(brcenter[y]+topr[y]));
   }

   for(y = W_STP+dy; y < sh-dy-W_STP; y++) {
      for(y2 = y-W_STP; y2 < y; y2++) {
         topl[y] += twlrow[y2];
         topc[y] += twcrow[y2];
         topr[y] += twrrow[y2];
      }

      topl[y] += wlcenter[y] + blcenter[y];
      topc[y] += wccenter[y] + bccenter[y];
      topr[y] += wrcenter[y] + brcenter[y];
   }

   tscore = -1;
   ylim = sh;
   if(ylim > TOP_LI)
      ylim = TOP_LI;
   for(y = 0; y < ylim; y++) {
      if(topl[y] > tscore) {
         tscore = topl[y];
         *dty = y;
         *cenoff = sw/4;
      }
      if(topc[y] > tscore) {
         tscore = topc[y];
         *dty = y;
         *cenoff = sw/2;
      }
      if(topr[y] > tscore) {
         tscore = topr[y];
         *dty = y;
         *cenoff = 3*(sw/4);
      }
   }
   free(topl);
   free(topc);
   free(topr);
   return(0);
}

/*******************************************************/
int adjust_top_up(int *dty, unsigned char *gdata, const int sw, const int sh,
                   const int cenoff, const int dy)
{
   int ret, x, y, dx, tlim1, tlim2;
   unsigned char *sptr1, *sptr2;
   int *wdata, score;

   if((ret = calloc_int_ret(&wdata, sh, "adjust_top_up wdata")))
      return(ret);

   tlim1 = *dty;
   tlim2 = tlim1 - dy;
   if(tlim2 < 0)
      tlim2 = 0;

   dx = sw/4;
   sptr1 = gdata+cenoff-dx+(tlim2*sw);
   for(y = tlim2; y < tlim1; y++) {
      sptr2 = sptr1;
      for(x = -dx; x < dx; x++) {
         if(*sptr2++ == 255)
            wdata[y]++;
      }
      sptr1 += sw;
   }

   score = wdata[tlim1];
   for(y = tlim1; y > tlim2; y--) {
      if(wdata[y] > score) {
         score = wdata[y];
         *dty = y;
         if(score > sround(0.8*(float)(2*dx)))
            y = tlim2;
      }
   }

   free(wdata);
   return(0);
}

/*******************************************************/
void find_segfing_bottom(seg_rec_coords *fing_boxes, const int i,
          unsigned char *gdata, const int sw, const int sh, const int cenoff,
          const int dh, const float sfac)
{
   int x, y, dx, botlim, wacc, score;
   unsigned char *sptr1, *sptr2;
   int lim;

   /* Try to preserve Bottoms along the edge and corners */
   if(sh < dh/Z_FAC) {
      fing_boxes[i].dby = fing_boxes[i].dty + sh;
      return;
   }

   /* Try to detect a good place to segment print at bottom */
   dx = sround(0.2 * (float)sw);
   if(cenoff < sw/2 || cenoff > sw/2)
      lim = sround(0.7 * (float)(2*dx));
   else
      lim = sround(0.35 * (float)(2*dx));
   sptr1 = gdata+((fing_boxes[i].dty+15)*sw)+cenoff-dx;
   botlim = fing_boxes[i].dty + (dh/Z_FAC);
   if(botlim > sh)
      botlim = sh-1;
   fing_boxes[i].dby = botlim;
   score = sround(sfac * (float)(2*dx));
   for(y = fing_boxes[i].dty+15; y < botlim; y++) {
      sptr2 = sptr1;
      wacc = 0;
      for(x = -dx; x < dx; x++) {
         if(*sptr2++)
            wacc++;
      }
      sptr1 += sw;
      if(wacc > score) {
         fing_boxes[i].dby = y;
         score = wacc;
         if(score > lim)
            y = botlim;
      }
   }
}

/*******************************************************/
void find_segfing_sides(seg_rec_coords *fing_boxes, const int i,
          unsigned char *gdata, const int sw, const int sh, const int cenoff)
{
   int x, y, ylim, wacc, score;
   unsigned char *sptr1, *sptr2;

   /* FIND LEFT SIDE */
   ylim = sround(0.5*(float)(fing_boxes[i].dby-fing_boxes[i].dty));
   sptr1 = gdata+(fing_boxes[i].dty*sw)+cenoff;
   fing_boxes[i].dlx = 0;
   score = sround(0.25*(float)(fing_boxes[i].dby-fing_boxes[i].dty));
   for(x = cenoff; x > 0; x--) {
      sptr2 = sptr1;
      wacc = 0;
      for(y = fing_boxes[i].dty; y < fing_boxes[i].dby; y++) {
         if(*sptr2)
            wacc++;
         sptr2 += sw;
      }
      if(wacc > score) {
         score = wacc;
         fing_boxes[i].dlx = x;
         if(wacc > ylim)
            x = 0;
      }
      sptr1--;
   }

   /* FIND RIGHT SIDE */
   sptr1 = gdata+(fing_boxes[i].dty*sw)+cenoff;
   fing_boxes[i].drx = sw-1;
   score = sround(0.25*(float)(fing_boxes[i].dby-fing_boxes[i].dty));
   for(x = cenoff; x < sw-1; x++) {
      sptr2 = sptr1;
      wacc = 0;
      for(y = fing_boxes[i].dty; y < fing_boxes[i].dby; y++) {
         if(*sptr2)
            wacc++;
         sptr2 += sw;
      }
      if(wacc > score) {
         score = wacc;
         fing_boxes[i].drx = x;
         if(wacc > ylim)
            x = sw;
      }
      sptr1++;
   }
}

/*******************************************************************/
void adjust_fing_seg_pars(seg_rec_coords *fing_boxes, const int nf)
{
   int i;
   int nsw, nsh, hnsw, hnsh, hsw, hsh;
   float dxt, dyt;
   float dx, dy;
   float d;
   float theta2, dtheta;

   for(i = 0; i < nf; i++) {
      nsw = fing_boxes[i].drx-fing_boxes[i].dlx;
      nsh = fing_boxes[i].dby-fing_boxes[i].dty;
      hnsw = nsw/2;
      hnsh = nsh/2;

      if(fing_boxes[i].theta == 0.0) {
         fing_boxes[i].sx = fing_boxes[i].tlx + fing_boxes[i].dlx + hnsw;
         fing_boxes[i].sy = fing_boxes[i].tly + fing_boxes[i].dty + hnsh;
      }
      else {
         if(fing_boxes[i].theta > 0.0)
            dx = fing_boxes[i].dlx + hnsw;
         else
            dx = (fing_boxes[i].sw-fing_boxes[i].drx) + hnsw;
         dy = fing_boxes[i].dty + hnsh;
         theta2 = atan(dx/dy);
         theta2 += fabs(fing_boxes[i].theta);
         d = sqrt((dx*dx) + (dy*dy));
         dxt = d * sin(theta2);
         dyt = d * cos(theta2);
         if(fing_boxes[i].theta > 0.0) {
            fing_boxes[i].sx = fing_boxes[i].tlx + sround(dxt);
            fing_boxes[i].sy = fing_boxes[i].tly + sround(dyt);
         }
         else {
            fing_boxes[i].sx = fing_boxes[i].tRightX - sround(dxt);
            fing_boxes[i].sy = fing_boxes[i].tRightY + sround(dyt);
         }
      }
      fing_boxes[i].sw = nsw + 4;
      fing_boxes[i].sh = nsh + 4;

      hsw = fing_boxes[i].sw/2;
      hsh = fing_boxes[i].sh/2;
      if(fing_boxes[i].theta == 0.0) {
         fing_boxes[i].tlx = fing_boxes[i].sx - hsw;
         fing_boxes[i].tly = fing_boxes[i].sy - hsh;
         fing_boxes[i].tRightX = fing_boxes[i].sx + hsw;
         fing_boxes[i].tRightY = fing_boxes[i].sy - hsh;
         fing_boxes[i].blx = fing_boxes[i].sx - hsw;
         fing_boxes[i].bly = fing_boxes[i].sy + hsh;
         fing_boxes[i].brx = fing_boxes[i].sx + hsw;
         fing_boxes[i].bry = fing_boxes[i].sy + hsh;
      }
      else {
         d = sqrt((hsw*hsw)+(hsh*hsh));
         theta2 = atan((double)hsh/(double)hsw);
         if(fing_boxes[i].theta > 0.0) {
            dtheta = theta2 - fing_boxes[i].theta;
            dx = d * cos(dtheta);
            dy = d * sin(dtheta);
            fing_boxes[i].tlx = fing_boxes[i].sx - sround(dx);
            fing_boxes[i].tly = fing_boxes[i].sy - sround(dy);
            fing_boxes[i].brx = fing_boxes[i].sx + sround(dx);
            fing_boxes[i].bry = fing_boxes[i].sy + sround(dy);
            dx = fing_boxes[i].sw * cos(fing_boxes[i].theta);
            dy = fing_boxes[i].sw * sin(fing_boxes[i].theta);
            fing_boxes[i].tRightX = fing_boxes[i].tlx + sround(dx);
            fing_boxes[i].tRightY = fing_boxes[i].tly - sround(dy);
            fing_boxes[i].blx = fing_boxes[i].brx - sround(dx);
            fing_boxes[i].bly = fing_boxes[i].bry + sround(dy);
         }
         else {
            dtheta = theta2 + fing_boxes[i].theta;
            dx = d * cos(dtheta);
            dy = d * sin(dtheta);
            fing_boxes[i].tRightX = fing_boxes[i].sx + sround(dx);
            fing_boxes[i].tRightY = fing_boxes[i].sy - sround(dy);
            fing_boxes[i].blx = fing_boxes[i].sx - sround(dx);
            fing_boxes[i].bly = fing_boxes[i].sy + sround(dy);
            dx = fing_boxes[i].sw * cos(fing_boxes[i].theta);
            dy = fing_boxes[i].sw * sin(fing_boxes[i].theta);
            fing_boxes[i].tlx = fing_boxes[i].tRightX - sround(dx);
            fing_boxes[i].tly = fing_boxes[i].tRightY + sround(dy);
            fing_boxes[i].brx = fing_boxes[i].blx + sround(dx);
            fing_boxes[i].bry = fing_boxes[i].bly - sround(dy);
         }
      }
   }
}

/*******************************************************************/
void err_check_finger(int *digits, seg_rec_coords *fing_boxes, const int n)
{
   int i, w, h, dx, tdx;

   for(i = 0; i < n; i++) {
      w = fing_boxes[i].drx - fing_boxes[i].dlx;
      h = fing_boxes[i].dby - fing_boxes[i].dty;
      if(w < FING_WIDTH_MIN)
         fing_boxes[i].err = 1;
      if(h < FING_HEIGHT_MIN)
         fing_boxes[i].err = 2;
   }

   if(n > 1) {
      for(i = 0; i < n-1; i++) {
         tdx = digits[i+1] - digits[i];
         dx = sround((float)tdx * fabs(cos(fing_boxes[i].theta)));
         if(dx < FING_SPACE_MIN) {
            fing_boxes[i].err = 3;
            fing_boxes[i+1].err = 3;
         }
         if(dx > FING_SPACE_MAX) {
            fing_boxes[i].err = 4;
            fing_boxes[i+1].err = 4;
         }
      }
   }
}

/*******************************************************/
void scale_seg_fingers(seg_rec_coords *fing_boxes, const int n,
          const int w, const int h, const int zm)
{
   int i;

   for(i = 0; i < n; i++) {
      if(fing_boxes[i].dty < 2)
         fing_boxes[i].dty = 0;
      if(fing_boxes[i].dlx < 2)
         fing_boxes[i].dlx = 0;
      if(fing_boxes[i].drx > fing_boxes[i].sw-2)
         fing_boxes[i].drx = fing_boxes[i].sw;
      if(fing_boxes[i].dby > fing_boxes[i].sh-2)
         fing_boxes[i].dby = fing_boxes[i].sh;

      fing_boxes[i].tlx *= zm;
      fing_boxes[i].tly *= zm;
      fing_boxes[i].tRightX *= zm;
      fing_boxes[i].tRightY *= zm;
      fing_boxes[i].blx *= zm;
      fing_boxes[i].bly *= zm;
      fing_boxes[i].brx *= zm;
      fing_boxes[i].bry *= zm;
      fing_boxes[i].sx *= zm;
      fing_boxes[i].sy *= zm;
      fing_boxes[i].sw *= zm;
      fing_boxes[i].sh *= zm;
      fing_boxes[i].dty *= zm;
      fing_boxes[i].dby *= zm;
      fing_boxes[i].dlx *= zm;
      fing_boxes[i].drx *= zm;


      if(fing_boxes[i].drx > w)
         fing_boxes[i].drx = w;
      if(fing_boxes[i].dby > h)
         fing_boxes[i].dby = h;

      if(fing_boxes[i].theta >= 0.0) {
         fing_boxes[i].nrsw = fing_boxes[i].brx - fing_boxes[i].tlx;
         fing_boxes[i].nrsh = fing_boxes[i].bly - fing_boxes[i].tRightY;
      }
      else {
         fing_boxes[i].nrsw = fing_boxes[i].tRightX - fing_boxes[i].blx;
         fing_boxes[i].nrsh = fing_boxes[i].bry - fing_boxes[i].tly;
      }
   }

}

/*******************************************************************/
int parse_segfing(unsigned char ***pdata, unsigned char *data, const int w,
                  const int h, seg_rec_coords *fing_boxes, const int nf,
                  const int rot)
{
   unsigned char *sdata, **pptr;
   int ret, i, j, n, e;
   int *xp, *yp, np, ap;

   (*pdata) = (unsigned char **)malloc(nf*sizeof(unsigned char *));
   if(*pdata == NULL) {
      fprintf(stderr, "ERROR: malloc pdata in parse_segfing\n");
      return(-2);
   }
   pptr = *pdata;

      for(n = 0; n < nf; n++) {

         if(rot) {
	   if((ret = malloc_uchar_ret(&(pptr[n]),
                                      fing_boxes[n].sw*fing_boxes[n].sh,
                                      "parse_segfing pptr"))) {
               for(e = 0; e < n; e++)
                  free(pptr[e]);
               free(pptr);
               return(ret);
            }
            sdata = pptr[n];

            snip_rot_subimage_interp(data, w, h, sdata, fing_boxes[n].sw,
                     fing_boxes[n].sh, fing_boxes[n].sx, fing_boxes[n].sy,
                     fing_boxes[n].theta, 255);
         }
         else {
	   if((ret = malloc_uchar_ret(&(pptr[n]),
                                      fing_boxes[n].nrsw*fing_boxes[n].nrsh,
                                      "parse_segfing pptr"))) {
               for(e = 0; e < n; e++)
                  free(pptr[e]);
               free(pptr);
               return(ret);
            }
            sdata = pptr[n];

            snip_rot_subimage(data, w, h, sdata, fing_boxes[n].nrsw,
                     fing_boxes[n].nrsh, fing_boxes[n].sx, fing_boxes[n].sy,
                     0.0, 255);

            /* FILL AREAS OUTSIDE FINGERPRINT BOUNDING BOX WITH WHITE PIXELS */
            ap = 0;
            np = 0;
            if(fing_boxes[n].theta > 0.0) {
               if((ret = bres_line_alloc(
                               fing_boxes[n].tRightX-fing_boxes[n].tlx,
                               fing_boxes[n].tRightY-fing_boxes[n].tRightY,
                               fing_boxes[n].tlx-fing_boxes[n].tlx,
                               fing_boxes[n].tly-fing_boxes[n].tRightY,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = fing_boxes[n].tlx-fing_boxes[n].tlx; i < xp[j]; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               if((ret = bres_line_alloc(fing_boxes[n].tlx-fing_boxes[n].tlx,
                               fing_boxes[n].tly-fing_boxes[n].tRightY,
                               fing_boxes[n].blx-fing_boxes[n].tlx,
                               fing_boxes[n].bly-fing_boxes[n].tRightY-1,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = fing_boxes[n].tlx-fing_boxes[n].tlx; i < xp[j]; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               if((ret = bres_line_alloc(
                               fing_boxes[n].tRightX-fing_boxes[n].tlx,
                               fing_boxes[n].tRightY-fing_boxes[n].tRightY,
                               fing_boxes[n].brx-fing_boxes[n].tlx,
                               fing_boxes[n].bry-fing_boxes[n].tRightY,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = xp[j]; i < fing_boxes[n].brx-fing_boxes[n].tlx; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               if((ret = bres_line_alloc(fing_boxes[n].brx-fing_boxes[n].tlx,
                               fing_boxes[n].bry-fing_boxes[n].tRightY,
                               fing_boxes[n].blx-fing_boxes[n].tlx,
                               fing_boxes[n].bly-fing_boxes[n].tRightY-1,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = xp[j]; i < fing_boxes[n].brx-fing_boxes[n].tlx; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               free(xp);
               free(yp);
            }
            else if(fing_boxes[n].theta < 0.0) {
               if((ret = bres_line_alloc(fing_boxes[n].tlx-fing_boxes[n].blx,
                               fing_boxes[n].tly-fing_boxes[n].tly,
                               fing_boxes[n].blx-fing_boxes[n].blx,
                               fing_boxes[n].bly-fing_boxes[n].tly,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = fing_boxes[n].blx-fing_boxes[n].blx; i < xp[j]; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               if((ret = bres_line_alloc(fing_boxes[n].blx-fing_boxes[n].blx,
                               fing_boxes[n].bly-fing_boxes[n].tly,
                               fing_boxes[n].brx-fing_boxes[n].blx,
                               fing_boxes[n].bry-fing_boxes[n].tly-1,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = fing_boxes[n].blx-fing_boxes[n].blx; i < xp[j]; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               if((ret = bres_line_alloc(fing_boxes[n].tlx-fing_boxes[n].blx,
                               fing_boxes[n].tly-fing_boxes[n].tly,
                               fing_boxes[n].tRightX-fing_boxes[n].blx,
                               fing_boxes[n].tRightY-fing_boxes[n].tly,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = xp[j]; i < fing_boxes[n].tRightX-fing_boxes[n].blx; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               if((ret = bres_line_alloc(
                               fing_boxes[n].tRightX-fing_boxes[n].blx,
                               fing_boxes[n].tRightY-fing_boxes[n].tly,
                               fing_boxes[n].brx-fing_boxes[n].blx,
                               fing_boxes[n].bry-fing_boxes[n].tly-1,
                               &xp, &yp, &np, &ap))) {
                  for(e = 0; e < n; e++)
                     free(pptr[e]);
                  free(pptr);
                  return(ret);
               }
               for(j = 0; j < np; j++)
                  for(i = xp[j]; i < fing_boxes[n].tRightX-fing_boxes[n].blx; i++)
                     if(i >= 0 && i < fing_boxes[n].nrsw &&
                             yp[j] >= 0 && yp[j] < fing_boxes[n].nrsh)
                        sdata[i+(yp[j]*fing_boxes[n].nrsw)] = 255;
               free(xp);
               free(yp);
            }
            ap = 0;
            np = 0;
         }
      }
      return(0);
}

/***********************************************************************
************************************************************************
#cat: new_fgp - Calculate the new fgp for part of a segmented fingerprint 
#cat:              image.

   Input:
      fgp        - original fgp of whole image
      n          - finger/segment number (assigned left to right across image)
      fing_boxes - 
   Return Code:
      Positive   - new fgp
      Negative   - error
************************************************************************/
static int new_fgp(const int fgp, const int n, 
		   const seg_rec_coords *const fing_boxes) {
   int nfgp;

   if(fgp < 13 && fgp > 0)	/* single finger */
      nfgp = fgp;
   else if(fgp == 13)		/* right hand, fingers 2 through 5. */
      nfgp = 2 + n;
   else if(fgp == 14)		/* left hand, fingers 10 through 6  */
      nfgp = 10 - n;
   else if(fgp == 15)		/* ANSI/NIST two-thumbs, 12 and 11 */
      nfgp = 12 - n;
   else if(fgp == 0 && fing_boxes[n].theta >= 0.0)
		    /* unknown, slanted right or straight, assume right hand */
      nfgp = 2 + n;
   else if(fgp == 0)		/* unknown, slanted left, assume left hand */
      nfgp = 10 - n;
   else {
      fprintf(stderr, "ERROR: new_fgp: Invalid FGP %d\n", fgp);
      return -3;
   }
   return nfgp;
}

/***********************************************************************
************************************************************************
#cat: write_parsefing - Write out parsed finger records to files, with
#cat:            filenames based on the input filename.  For ANSI/NIST
#cat:            files, the record number is also appended to make
#cat:            sure each filename is unique.  For single-image
#cat:            files, use a negative record number and it will be
#cat:            ignored.

   Input:
      file       - the input filename, used to generate output file names
      rec_i      - the record number, or negative, used to generate unique
                   filenames for multiple images in an ANSI/NIST file
      fgp        - original fgp of whole image
      n          - finger/segment number (assigned left to right across image)
      fing_boxes - 
   Return Code:
      Positive   - new fgp
      Negative   - error
************************************************************************/
int write_parsefing(char *file, const int rec_i, const int fgp,
		    const int comp, const int ppi,
		    const int lossyflag, unsigned char **pdata,
		    seg_rec_coords *fing_boxes, const int nf, const int rot)
{
   int hor_sampfctr[MAX_CMPNTS], vrt_sampfctr[MAX_CMPNTS];
   IMG_DAT *img_dat;
   char *comment_text;
   unsigned char *cdata;
   int ret, n, nfgp, clen;
   char *ofile;
   int sw, sh;

   comment_text = (char *)NULL;
   for(n = 0; n < MAX_CMPNTS; n++) {
      hor_sampfctr[n] = 1;
      vrt_sampfctr[n] = 1;
   }

   if((ret = malloc_char_ret(&ofile, (strlen(file)+10),
			     "write_parsefing ofile"))) {
      return(ret);
   }
   fileroot(file);

   for(n = 0; n < nf; n++) {
      if(rot){
         sw = fing_boxes[n].sw;
         sh = fing_boxes[n].sh;
      }
      else {
         sw = fing_boxes[n].nrsw;
         sh = fing_boxes[n].nrsh;
      }

      if ((nfgp = new_fgp(fgp, n, fing_boxes)) < 0) {
	 free(ofile);
	 return nfgp;
      }

/*    NULL replace with '\0' by MDG on 03-11-05
      sprintf(ofile, "%s_%02d.unk%c", file, nfgp, NULL);
*/
      if (rec_i >= 0)
	 sprintf(ofile, "%s_%02d_%02d.unk%c", file, rec_i+1, nfgp, '\0');
      else 
	 sprintf(ofile, "%s_%02d.unk%c", file, nfgp, '\0');

/* JPEGL COMPRESS */
      if(comp == 0) {
         newext(ofile, strlen(ofile), "jpl");
         if((ret = setup_IMG_DAT_nonintrlv_encode(&img_dat, pdata[n],
                            sw, sh, 8, ppi, hor_sampfctr, vrt_sampfctr,
                            1, 0, PRED4))){
            free(ofile);
            return(ret);
         }

         if((ret = jpegl_encode_mem(&cdata, &clen, img_dat, comment_text))){
            free(ofile);
            free_IMG_DAT(img_dat, FREE_IMAGE);
            return(ret);
         }
         free_IMG_DAT(img_dat, FREE_IMAGE);
      }
/* WSQ COMPRESS 5:1 */
      else if(comp == 1) {
         newext(ofile, strlen(ofile), "wsq");
         if((ret = wsq_encode_mem(&cdata, &clen, 2.25, pdata[n],
                                 sw, sh, 8, ppi, comment_text))){
            free(ofile);
            return(ret);
         }
      }
/* WSQ COMPRESS 15:1 */
      else if(comp == 2) {
         newext(ofile, strlen(ofile), "wsq");
         if((ret = wsq_encode_mem(&cdata, &clen, 0.75, pdata[n],
                                 sw, sh, 8, ppi, comment_text))){
            free(ofile);
            return(ret);
         }
      }
/* UNCOMPRESSED RAW */
      else if(comp == 3) {
         newext(ofile, strlen(ofile), "raw");
         clen = sw * sh;
         if((ret = malloc_uchar_ret(&cdata, clen, "write_parsefings cdata"))) {
            free(ofile);
            return(ret);
         }
         memcpy(cdata, pdata[n], clen);
      }
/* UNDEFINED COMPRESSION VALUE */
      else {
	 fprintf(stderr, "ERROR : write_parsefing : "
		 "unknown compression type: %d\n", comp);
	 return -1;
      }

/* WRITE SEGENTED DATA */
      printf("FILE %s -> e %d sw %d sh %d sx %d sy %d th %2.1f\n", ofile,
           fing_boxes[n].err, sw, sh, fing_boxes[n].sx, fing_boxes[n].sy,
           fing_boxes[n].theta*DEG2RAD);
      if((ret = write_raw_from_memsize(ofile, cdata, clen))) {
         free(ofile);
         free(cdata);
         return(ret);
      }
      free(cdata);

   }
   free(ofile);
   return(0);
}

/***********************************************************************
************************************************************************
#cat: insert_int_item - Insert an integer item into an ANSI NIST file
#cat:              structure at the specified location.

   Input:
      record_i   - the record number, record must already exist
      field_i    - the field number, field must already exist
      subfield_i - the subfield number, subfield must already exist
      item_i     - the item number, item must not exist already
      value      - the integer value
      name       - a brief string indicating the nature of the item,
                   for self-documenting code and error messages
      ansi_nist  - the ANSI NIST structure into which the item is inserted
   Return Code:
      Zero       - Success
      Negative   - error
************************************************************************/
static int insert_int_item(const int record_i, const int field_i, 
			   const int subfield_i, const int item_i,
			   const int value, const char* name, 
			   ANSI_NIST *const ansi_nist)
{
   int ret;
   char str_val[20];
   
   ret = snprintf(str_val, sizeof str_val, "%d", value);
   if (ret >= sizeof str_val) {
      fprintf(stderr, "ERROR : insert_int_item : "
	      "converting %s value %d to string, result too long '%.*s'...\n", 
	      name, value, sizeof str_val, str_val);
      return -1;
   } else if (ret < 0) {
      fprintf(stderr, "ERROR : insert_int_item : "
	      "converting %s value %d to string '%.*s'\n",
	      name, value, sizeof str_val, str_val);
      return -2;
   }
   if (insert_ANSI_NIST_item(record_i, field_i, subfield_i, item_i,
			     str_val, ansi_nist) < 0)
      return -3;   
   
   return 0;
}

/***********************************************************************
***********************************************************************
#cat: insert_parsefing - Add segmented/parsed fingers to a SEG or ASEG
#cat:                    field in a Type-14 record, with a comment
#cat:                    indicating the source.  One subfield is used
#cat:                    for each finger found, and a comment
#cat:                    indicates that the corresponding subfields
#cat:                    were generated by nfseg.

   Input:
      ansi_nist  - the ANSI/NIST file structure
      record_i   - the record number, must be Type-14
      fgp        - the original finger position of the segmented image
      fing_boxes - the segmented finger boxes
      nf         - the number of fingers extracted
      rot_search - indicates whether the search was rotated, which
                   determines whether to use SEG or ASEG
   Output
      ansi_nist  - the ANSI/NIST file structure will be modified

   Return Code:
      Zero       - Success
      Negative   - error
************************************************************************/
int insert_parsefing(ANSI_NIST *const ansi_nist, const int imgrecord_i,
		     const int fgp, const seg_rec_coords *const fing_boxes,
		     const int nf, const int rot_search)
{
   /* Write SEG or ASEG data into the Type 14 record */
   const int seg_id = (rot_search) ? ASEG_ID : SEG_ID;
   const RECORD *imgrecord = ansi_nist->records[imgrecord_i];
   FIELD *field;
   SUBFIELD *subfield;
   int ret, field_i, first_subfield, subfield_i, item_i, fing_i, nfgp,
      comment_size;
   char comment[127], *old_comment, subfield_str[16];
   seg_rec_coords *fing_box;

   /* Check whether the SEG or ASEG field already exists, in which
      case we notify the user and add another subfield.  (We assume
      that extra sets of ASEG are allowed, although it is not
      mentioned in the standard like it is for SEG.)  Otherwise we
      create the subfield.  In either case, field_i and first_subfield
      are set when the if-else statment is finished. */
   if (lookup_ANSI_NIST_field(&field, &field_i, seg_id, imgrecord)) {
      first_subfield = field->num_subfields;
      fprintf(stderr, "WARNING: %s field already exists, "
	      "adding subfields starting with %d\n",
	      rot_search ? "ASEG" : "SEG", first_subfield+1);

   } else {
      /* Given a record, find the right place to insert the field.  The
	 first fields through FGP=13 are required.  The others between
	 FGP=13 and the image data are optional. */
      for (field_i = 12; field_i < imgrecord->num_fields; field_i++){
	 if (imgrecord->fields[field_i]->field_int > seg_id) {
	    break;
	 }
      }

      if (new_ANSI_NIST_field(&field, imgrecord->type, seg_id) < 0)
	 return -1;
      if (insert_ANSI_NIST_field_frmem(imgrecord_i, field_i, field, 
				       ansi_nist) < 0)
	 return -2;

      first_subfield = 0;
   }

   /* for each finger/segment */
   for (fing_i = 0; fing_i < nf; fing_i++) {
      subfield_i = first_subfield + fing_i;
      item_i = 0;

      /* Create and insert a new subfield. */
      if (alloc_ANSI_NIST_subfield(&subfield) < 0)
	 return -3;
      if (insert_ANSI_NIST_subfield_frmem(imgrecord_i, field_i, subfield_i,
					  subfield, ansi_nist) < 0)
	 return -4;

      /* Calculate and insert a new finger position corresponding to a
	 segmented finger. */
      if ((nfgp = new_fgp(fgp, fing_i, fing_boxes)) < 0)
	 return nfgp;
      if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			  nfgp, "FGP", ansi_nist) < 0)
	 return -5;

      /* Insert segment position data. */
      if (rot_search) {		/* rotated - four clockwise corner points */
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     4, "Point Count", ansi_nist) < 0)
	    return -6;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].tlx, "TLX", ansi_nist) < 0)
	    return -7;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].tly, "TLY", ansi_nist) < 0)
	    return -8;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].tRightX, "TRX", ansi_nist) < 0)
	    return -9;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].tRightY, "TRY", ansi_nist) < 0)
	    return -10;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].brx, "BRX", ansi_nist) < 0)
	    return -11;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].bry, "BRY", ansi_nist) < 0)
	    return -12;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].blx, "BLX", ansi_nist) < 0)
	    return -13;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].bly, "BLY", ansi_nist) < 0)
	    return -14;

      } else {			/* not rotated - horiz and vert limits */
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].tlx, "Left", ansi_nist) < 0)
	    return -15;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].tRightX, "Right", ansi_nist) < 0)
	    return -16;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].tly, "Top", ansi_nist) < 0)
	    return -17;
	 if (insert_int_item(imgrecord_i, field_i, subfield_i, item_i++,
			     fing_boxes[fing_i].bly, "Bottom", ansi_nist) < 0)
	    return -18;
      }
   }

   /* add a comment telling that nfseg was used to generate the SEG or
      ASEG subfields that were added */

   /* First, check if there is already a comment, to which we will
      append the new comment, with a suitable separator in between */
   if (lookup_ANSI_NIST_field(&field, &field_i, COM_ID, imgrecord)) {
      old_comment = field->subfields[0]->items[0]->value;
      comment_size = strlen(old_comment);
   } else {
      /* no existing comment, must create the field and subfield... */
      old_comment = NULL;
      comment_size = 0;
      
      /* find the right place in the record to insert the new field */
      for (field_i++; field_i < imgrecord->num_fields; field_i++){
	 if (imgrecord->fields[field_i]->field_int > COM_ID) {
	    break;
	 }
      }

      /* create and insert the required field and subfield */
      if (new_ANSI_NIST_field(&field, imgrecord->type, COM_ID) < 0)
	 return -19;
      if (insert_ANSI_NIST_field_frmem(imgrecord_i, field_i,
				       field, ansi_nist) < 0)
	 return -20;
      if (alloc_ANSI_NIST_subfield(&subfield) < 0)
	 return -21;
      if (insert_ANSI_NIST_subfield_frmem(imgrecord_i, field_i, 0,
					  subfield, ansi_nist) < 0)
	 return -22;
   }

   /* generate string containing subfield range or single subfield number */
   if (nf > 1)
      ret = snprintf(subfield_str, sizeof subfield_str, "%d-%d",
		     first_subfield+1, subfield_i+1);
   else
      ret = snprintf(subfield_str, sizeof subfield_str, "%d",
		     first_subfield+1);
   if (sizeof(subfield_str) == ret) {
      subfield_str[sizeof subfield - 1] = '\0';
      fprintf(stderr, "WARNING : insert_parsfing : "
	      "subfield range in COM field truncated: %s\n",
	      subfield_str);
   }

   /* assemble the whole comment string */
   ret = snprintf(comment, sizeof(comment), "%s%s%sSEG %s by nfseg %s",
		  old_comment ? old_comment : "", old_comment ? ", " : "",
		  rot_search ? "A" : "", subfield_str, NBIS_NON_EXPORT_CONTROL_VERSION);
   if (sizeof(comment) <= ret) {
      comment[sizeof(comment) - 1] = '\0';
      fprintf(stderr, "WARNING : insert_parsfing : "
	      "comment truncated: size %d, limit %d, '%s'\n",
	      ret, sizeof(comment), comment);
   }

   /* insert or substitute the new comment string */
   if (NULL == old_comment) {
      if (insert_ANSI_NIST_item(imgrecord_i, field_i, 0, 0,
				comment, ansi_nist) <0)
	 return -23;
   } else {
      if (substitute_ANSI_NIST_item(imgrecord_i, field_i, 0, 0,
				    comment, ansi_nist) < 0)
	 return -24;
   }
  
   return 0;
}
