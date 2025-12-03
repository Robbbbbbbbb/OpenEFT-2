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

      FILE:    FINDBLOB.C
      AUTHORS: G. T. Candela
               
      DATE:    03/01/1994
      UPDATED: 03/14/2005 by MDG

      Routines for finding "blobs" in an image.

      ROUTINES:
#cat: findblob - finds a 4-connected blob of true pixels from within a
#cat:            binary character image, returning the blob as a character
#cat:            image.
#cat: findblob8 - finds an 8-connected blob of true pixels from within a
#cat:            binary character image, returning the blob as a character
#cat:            image.
#cat: findblobnruns - finds a 4-connected blob of true pixels from within a
#cat:            binary character image, returning the blob as a character
#cat:            image and the horizontal runs comprising the blob.
#cat: findblobnruns8 - finds an 8-connected blob of true pixels from within a
#cat:            binary character image, returning the blob as a character
#cat:            image and the horizontal runs comprising the blob.
#cat: findblob_stats_rw - finds a blob of true pixels in a binary char image
#cat:                     (search row maj) and returns the blob "stats"
#cat: findblob_stats_cl - finds a blob of true pixels in a binary char image
#cat:                     (search col maj) and returns the blob "stats"
#cat: end_findblobs - deallocates memory upon completion of a findblob session.
#cat:
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <findblob.h>
#include <util.h>

/************************************************************/
/*         Routine:   findblob()                            */
/*         Author:    G. T. Candela                         */
/*         Date:      3/1/94                                */
/*                                                          */
/*         Modifications:                                   */
/*           5/2/94 - faster version: uses runs             */
/************************************************************/

/* Each time findblob is called, it either returns one blob of the
true pixels of a binary raster, or indicates that it encountered no
blobs in its col-major scan from the desired starting position to the
bottom-right corner of the raster.  Parameters are:

  ras: Input binary raster, represented using one byte per pixel.
  w: Width of ras.
  h: Height of ras.
  erase_flag: Can be ERASE or NO_ERASE:
    ERASE     findblob erases blobs from input as it finds them.
    NO_ERASE  Doesn't erase blobs.  CAUTION: If findblob is called
     multiple times for a raster with NO_ERASE and caller does not
     erase blobs from input raster after they are returned, findblob
     will repeatedly find the same blob, unless caller changes
     (start_x, start_y) between calls.
  alloc_flag: Can be ALLOC or NO_ALLOC:
    ALLOC     findblob will allocate output blob-rasters.
    NO_ALLOC  findblob won't allocate rasters: caller must.
  out_flag: Can be ORIG_BLOB, W_H_BLOB, or BOUND_BLOB:
    ORIG_BLOB   Each blob is returned in an otherwise empty raster the
      same shape as the input raster, and its location in this output
      raster is the same as its location in the input raster.
    W_H_BLOB    Each blob is returned centered in a raster of width
      and height specified by box_w and box_h; but if ALLOC is
      is used and width or height of blob is larger than
      desired width or height, findblob allocates a raster just big
      enough to hold the blob, and its returned function value warns
      of this situation (return value 2).  If NO_ALLOC is used and
      blob doesn't fit, that is a fatal error.
    BOUND_BLOB  Each blob is returned in a raster just big enough to
      contain it.  This option cannot be used with NO_ALLOC.
  start_x, start_y: Addresses of the x- and y-coordinates of
    the pixel at which caller wants findblob to start -- or resume --
    its column-major scan for blobs.  (To find all blobs,
    caller can use a loop that initializes *start_x and
    *start_y to zeros and does not change them thereafter, and then
    should stop looping when findblob returns 0, which indicates
    that the previous call had returned the last blob; if raster has
    no blobs, first call of findblob will return 0.  But caller also
    has the options of starting *start_x and *start_y at
    something other than (0,0), and changing them between calls of
    findblob.)
  blobras: Address of the raster in which findblob returns the blob
    it found, representing it in one byte per pixel.
  box_x, box_y: Addresses of the coordinates of the top-left corner
    of the bounding box of the blob that was found; the coordinates
    are with respect to the input raster.
  box_w, box_h: Upon return, these always contain the width and
    height of the bounding box of the blob that was found; these
    may or may not be the width and height of the output raster.
    And if out_flag is W_H_BLOB, then box_w and
    box_h are used as INPUTS as well as outputs.  To summarize:
      If out_flag is ORIG_BLOB, then box_w and box_h are outputs
        only, indicating the dimensions of the bounding box of the
        blob, and the output raster containing the blob has the same
        dimensions as the input raster.
      If out_flag is W_H_BLOB, then box_w and box_h are both INPUTS
        specifying the desired width and height of the output raster
        in which the bounding box of the blob is to be centered (if
        it fits), and OUTPUTS indicating the dimensions of the
        bounding box.  (Therefore, a program that uses findblob to
        find multiple blobs with W_H_BLOB and always the same specified
        output raster dimensions, must reset these parms before each
        call, since findblob will change them.)  If the bounding box
        of the blob doesn't fit into the desired shape of output
        raster, then if alloc_flag is NO_ALLOC this is a fatal error,
        but if alloc_flag is ALLOC then findblob allocates an output
        raster of the same shape as the blob's bounding box and
        returns the blob in this raster.
      If out_flag is BOUND_BLOB, then box_w and box_h are outputs
        only, indicating the dimensions of the bounding box of the
        blob, which are also the dimensions of the output raster:
        findblob allocates the output raster just big enough
        to contain the blob.  (BOUND_BLOB cannot be used with
        NO_ALLOC.)

The possible function values returned by findblob are:
     0: It reached the bottom-right pixel without finding a blob.
     1: It found a blob, and is returning it in desired format.
     2: It found a blob, but ALLOC and W_H_BLOB are in effect and
          blob doesn't fit into a raster of specified shape; so, it
          has allocated a raster just big enough for the blob and is
          returning the blob in this raster (as if BOUND_BLOB had
          been used).

*********************************************************************/

static RUN *list = (RUN *)NULL, *list_off, *list_h, *list_t;
static unsigned char *rasity;
static unsigned short ww, hh, hh_m1, nlim, slim, elim, wlim;

int findblob(unsigned char *ras, int w, int h, int erase_flag,
             int alloc_flag, int out_flag, int *start_x, int *start_y,
             unsigned char **blobras, int *box_x, int *box_y,
             int *box_w, int *box_h)
{
RUN *oruns, *oruns_t, *oruns_off;
return findblob_connect(ras, w, h, erase_flag, alloc_flag, out_flag, start_x,
                             start_y, blobras, box_x, box_y, box_w, box_h,
                             &oruns, &oruns_t, &oruns_off, CONNECT4);
}


/*********************************************************************/
int findblob8(unsigned char *ras, int w, int h, int erase_flag,
              int alloc_flag, int out_flag, int *start_x, int *start_y,
              unsigned char **blobras, int *box_x, int *box_y,
              int *box_w, int *box_h)
{
RUN *oruns, *oruns_t, *oruns_off;
return findblob_connect(ras, w, h, erase_flag, alloc_flag, out_flag, start_x,
                             start_y, blobras, box_x, box_y, box_w, box_h,
                             &oruns, &oruns_t, &oruns_off, CONNECT8);
}


/*********************************************************************/
int findblobnruns(unsigned char *ras, int w, int h, int erase_flag,
                  int alloc_flag, int out_flag, int *start_x, int *start_y,
                  unsigned char **blobras, int *box_x, int *box_y,
                  int *box_w, int *box_h,
                  RUN **oruns, RUN **oruns_t, RUN **oruns_off)
{
return findblob_connect(ras, w, h, erase_flag, alloc_flag, out_flag, start_x,
                             start_y, blobras, box_x, box_y, box_w, box_h,
                             oruns, oruns_t, oruns_off, CONNECT4);
}


/*********************************************************************/
int findblobnruns8(unsigned char *ras, int w, int h, int erase_flag,
                   int alloc_flag, int out_flag, int *start_x, int *start_y,
                   unsigned char **blobras, int *box_x, int *box_y,
                   int *box_w, int *box_h,
                   RUN **oruns, RUN **oruns_t, RUN **oruns_off)
{
return findblob_connect(ras, w, h, erase_flag, alloc_flag, out_flag, start_x,
                        start_y, blobras, box_x, box_y, box_w, box_h,
                        oruns, oruns_t, oruns_off, CONNECT8);
}


/*********************************************************************/
int findblob_connect(unsigned char *ras, int w, int h, int erase_flag,
                     int alloc_flag, int out_flag, int *start_x, int *start_y,
                     unsigned char **blobras, int *box_x, int *box_y,
                     int *box_w, int *box_h,
                     RUN **oruns, RUN **oruns_t, RUN **oruns_off,
                     int connectivity)
{
  RUN *runp;
  unsigned char *p, *ps, *q, *blobrasity;
  unsigned short x, y;
  int offset, inner_bw, inner_bh, outer_bw, outer_bh, inner_wlim,
    inner_nlim;
  if(list == (RUN *)NULL) {
    if(!(list = (RUN *)malloc(LIST_STARTSIZE * sizeof(RUN))))
      syserr("findblobnruns_malloc_list", "malloc", "list");
    list_off = list + LIST_STARTSIZE;
  }
  rasity = ras;
  ww = w;
  hh_m1 = (hh = h) - 1;
  if(*start_x < 0 || *start_x >= (int)ww ||
     *start_y < 0 || *start_y >= (int)hh)
    fatalerr("findblobnruns", "scan start position is off raster",
      "start_x, start_y");
  x = *start_x;
  y = *start_y;
  /* Col-majorly scan for a seed pixel. */
  for(p = ras + y * ww + x, ps = ras + hh_m1 * ww + x; !*p;) {
    if(p < ps)
      p += ww;
    else {
      if(++x == ww)
	return 0;
      p = ras + x;
      ps++;
    }
  }
  y = (p - ras) / (int)ww;
  /* Grow seed pixel to a seed run. */
  findblob_seed_to_run(y, p);
  /* Use a queue to grow seed run into a complete blob.  Queue starts
  as just the seed run; read run from queue head, produce connecting
  runs (if any) and put them on queue tail, read another run from
  head, etc., until queue becomes empty.  List space is not recycled:
  when growth of blob is finished, list contains all runs that were
  ever in the queue. */
  if (connectivity == CONNECT4)
     for(list_t = (list_h = list) + 1; list_h < list_t; list_h++)
     {
       findblob_grow_n();
       findblob_grow_s();
     }
  else
  if (connectivity == CONNECT8)
     for(list_t = (list_h = list) + 1; list_h < list_t; list_h++)
     {
       findblob_8grow_n();
       findblob_8grow_s();
     }
  else
     fatalerr("findblobnruns", "connectivity flag", "must be CONNECT4 or CONNECT8");

  /* Growth of blob is finished.  Go through list to find what pixels
  to set in output raster. */
  *start_x = x;
  *start_y = y;
  *box_x = wlim;
  *box_y = nlim;
  *oruns = list;
  *oruns_t = list_t;
  *oruns_off = list_off;
  inner_bw = elim - wlim + 1;
  inner_bh = slim - nlim + 1;
  if(out_flag == ORIG_BLOB) {
    if(alloc_flag == ALLOC) {
      if(!(*blobras = (unsigned char *)calloc(ww * hh, 1)))
	syserr("findblobnruns", "calloc", "blobras");
    }
    else if(alloc_flag == NO_ALLOC)
      memset(*blobras, 0, ww * hh);
    else
      fatalerr("findblobnruns", "illegal value for alloc_flag", NULL);
    offset = *blobras - ras;
    if(erase_flag == ERASE)
      for(runp = list; runp < list_t; runp++)
	for(ps = (p = runp->w_on + offset) + (runp->e_off -
          runp->w_on); p < ps; p++)
	  *p = 1;
    else if(erase_flag == NO_ERASE)
      for(runp = list; runp < list_t; runp++)
	for(ps = (p = (q = runp->w_on) + offset) + (runp->e_off -
          runp->w_on); p < ps; p++, q++)
	  *p = *q = 1;
    else
      fatalerr("findblobnruns", "illegal value for erase_flag", NULL);
    *box_w = inner_bw;
    *box_h = inner_bh;
    return 1;
  }
  else if(out_flag == W_H_BLOB) {
    outer_bw = *box_w;
    outer_bh = *box_h;
    if(inner_bw <= outer_bw && inner_bh <= outer_bh) {
      /* Blob's bounding box fits into a raster of the shape caller
      has specified in box_w and box_h; center the bounding box in
      such a raster. */
      if(alloc_flag == ALLOC) {
	if(!(*blobras = (unsigned char *)calloc(outer_bw * outer_bh,
          1)))
	  syserr("findblobnruns", "calloc", "blobras");
      }
      else if(alloc_flag == NO_ALLOC)
	memset(*blobras, 0, outer_bw * outer_bh);
      else
	fatalerr("findblobnruns", "illegal value for alloc_flag", NULL);
      inner_wlim = (outer_bw - inner_bw) / 2;
      inner_nlim = (outer_bh - inner_bh) / 2;
      blobrasity = *blobras;
      if(erase_flag == ERASE)
	for(runp = list; runp < list_t; runp++)
	  for(ps = (p = blobrasity + (runp->y - nlim + inner_nlim) *
            outer_bw + (runp->w_on - ras) % (int)ww - wlim + inner_wlim) +
            (runp->e_off - runp->w_on); p < ps; p++)
	    *p = 1;
      else if(erase_flag == NO_ERASE)
	for(runp = list; runp < list_t; runp++)
	  for(ps = (p = blobrasity + (runp->y - nlim + inner_nlim) *
            outer_bw + ((q = runp->w_on) - ras) % (int)ww - wlim +
            inner_wlim) + (runp->e_off - runp->w_on); p < ps; p++, q++)
	    *p = *q = 1;
      else
	fatalerr("findblobnruns", "illegal value for erase_flag", NULL);
      *box_w = inner_bw;
      *box_h = inner_bh;
      return 1;
    }
    else {
      /* Blob's bounding box doesn't fit into a raster of the shape
      caller has specified in box_w and box_h. */
      if(alloc_flag == ALLOC) {
	/* Allocate a raster of same shape as bounding box (as
        for BOUND_BLOB), and return function value 2, warning
        that a larger-than-specified raster was allocated. */
	if(!(*blobras = (unsigned char *)calloc(inner_bw * inner_bh,
          1)))
	  syserr("findblobnruns", "calloc", "blobras");
	blobrasity = *blobras;
	if(erase_flag == ERASE)
	  for(runp = list; runp < list_t; runp++)
	    for(ps = (p = blobrasity + (runp->y - nlim) * inner_bw +
              (runp->w_on - ras) % (int)ww - wlim) +  (runp->e_off -
              runp->w_on); p < ps; p++)
	      *p = 1;
	else if(erase_flag == NO_ERASE)
	  for(runp = list; runp < list_t; runp++)
	    for(ps = (p = blobrasity + (runp->y - nlim) * inner_bw +
              ((q = runp->w_on) - ras) % (int)ww - wlim) +  (runp->e_off -
              runp->w_on); p < ps; p++, q++)
	      *p = *q = 1;
	else
	  fatalerr("findblobnruns", "illegal value for erase_flag", NULL);
	*box_w = inner_bw;
	*box_h = inner_bh;
	return 2;
      }
      else if(alloc_flag == NO_ALLOC)
	fatalerr("findblobnruns", "Used NO_ALLOC and W_H_BLOB, and blob's \
bounding box doesn't fit into specified output raster shape", NULL);
      else
	fatalerr("findblobnruns", "illegal value for alloc_flag", NULL);
    }
  }
  else if(out_flag == BOUND_BLOB) {
    if(alloc_flag == ALLOC) {
      if(!(*blobras = (unsigned char *)calloc(inner_bw * inner_bh, 1)))
	syserr("findblobnruns", "calloc", "blobras");
    }
    else if(alloc_flag == NO_ALLOC)
      fatalerr("findblobnruns", "NO_ALLOC and BOUND_BLOB used together",
        NULL);
    else
      fatalerr("findblobnruns", "illegal value for alloc_flag", NULL);
    blobrasity = *blobras;
    if(erase_flag == ERASE)
      for(runp = list; runp < list_t; runp++)
        for(ps = (p = blobrasity + (runp->y - nlim) * inner_bw +
          (runp->w_on - ras) % (int)ww - wlim) + (runp->e_off -
          runp->w_on); p < ps; p++)
	  *p = 1;

    else if(erase_flag == NO_ERASE)
      for(runp = list; runp < list_t; runp++)
        for(ps = (p = blobrasity + (runp->y - nlim) * inner_bw +
          ((q = runp->w_on) - ras) % (int)ww - wlim) + (runp->e_off -
          runp->w_on); p < ps; p++, q++)
	  *p = *q = 1;
    else
      fatalerr("findblobnruns", "illegal value for erase_flag", NULL);
    *box_w = inner_bw;
    *box_h = inner_bh;
    return 1;
  }
  else
    fatalerr("findblobnruns", "illegal value for out_flag", NULL);

  /* Should never reach here, but to avoid compiler warning ... */
  return(0);
}

/********************************************************************/

/* Grows seed pixel into seed run. */

void findblob_seed_to_run(unsigned short y, unsigned char *q)
{
  unsigned char *r, *w_on, *e_off;

  nlim = slim = y;
  e_off = (w_on = rasity + y * ww) + ww;
  for(*q = 0, r = q + 1; r < e_off && *r; r++)
    *r = 0;
  for(q--; q >= w_on && *q; q--)
    *q = 0;
  q++;
  list->y = y;
  wlim = (list->w_on = q) - w_on;
  list->e_off = r;
  elim = --r - w_on;
}

/********************************************************************/

/* Grows a run northward, i.e. finds any runs that touch its north
side. four way connectivity */

void findblob_grow_n()
{
  unsigned char *q, *qs, *qe, *r, *w_on, *e_off;
  unsigned short y, yow;

  if(!(y = list_h->y))
    return;
  for(q = qs = list_h->w_on - ww, qe = list_h->e_off - ww;
    q < qe && !*q; q++);
  if(q >= qe)
    return;
  if(--y < nlim)
    nlim = y;
  e_off = (w_on = rasity + y * ww) + ww;
  for(*q = 0, r = q + 1; r < e_off && *r; r++)
    *r = 0;
  if(q == qs) {
    for(q--; q >= w_on && *q; q--)
      *q = 0;
    q++;
  }
  if(list_t == list_off)
    findblob_realloc_list();
  list_t->y = y;
  if((yow = (list_t->w_on = q) - w_on) < wlim)
    wlim = yow;
  (list_t++)->e_off = r;
  while(1) {
    for(q = r + 1; q < qe && !*q; q++);
    if(q >= qe)
      break;
    for(*q = 0, r = q + 1; r < e_off && *r; r++)
      *r = 0;
    if(list_t == list_off)
      findblob_realloc_list();
    list_t->y = y;
    list_t->w_on = q;
    (list_t++)->e_off = r;
  }
  if((yow = --r - w_on) > elim)
    elim = yow;
}

/* same growing but with eight way connectivity */
void findblob_8grow_n()
{
  unsigned char *q, *qs, *qe, *r, *w_on, *e_off;
  unsigned short y, yow;

  if(!(y = list_h->y))
    return;
  e_off = (w_on = rasity + --y * ww) + ww;
  /* Extend limits by one on each end compared to 4-connected version,
  but careful not to fall off */
  if((qs = list_h->w_on - ww - 1) < w_on)
    qs++;
  if((qe = list_h->e_off - ww + 1) > e_off)
    qe--;
  for(q = qs; q < qe && !*q; q++);
  if(q >= qe)
    return;
  if(y < nlim)
    nlim = y;
  for(*q = 0, r = q + 1; r < e_off && *r; r++)
    *r = 0;
  if(q == qs) {
    for(q--; q >= w_on && *q; q--)
      *q = 0;
    q++;
  }
  if(list_t == list_off)
    findblob_realloc_list();
  list_t->y = y;
  if((yow = (list_t->w_on = q) - w_on) < wlim)
    wlim = yow;
  (list_t++)->e_off = r;
  while(1) {
    for(q = r + 1; q < qe && !*q; q++);
    if(q >= qe)
      break;
    for(*q = 0, r = q + 1; r < e_off && *r; r++)
      *r = 0;
    if(list_t == list_off)
      findblob_realloc_list();
    list_t->y = y;
    list_t->w_on = q;
    (list_t++)->e_off = r;
  }
  if((yow = --r - w_on) > elim)
    elim = yow;
}

/********************************************************************/

/* Grows a run southward, i.e. finds any runs that touch its south
side. uses four way connectivity */

void findblob_grow_s()
{
  unsigned char *q, *qs, *qe, *r, *w_on, *e_off;
  unsigned short y, yow;

  if((y = list_h->y + 1) == hh)
    return;
  for(q = qs = list_h->w_on + ww, qe = list_h->e_off + ww;
    q < qe && !*q; q++);
  if(q >= qe)
    return;
  if(y > slim)
    slim = y;
  e_off = (w_on = rasity + y * ww) + ww;
  for(*q = 0, r = q + 1; r < e_off && *r; r++)
    *r = 0;
  if(q == qs) {
    for(q--; q >= w_on && *q; q--)
      *q = 0;
    q++;
  }
  if(list_t == list_off)
    findblob_realloc_list();
  list_t->y = y;
  if((yow = (list_t->w_on = q) - w_on) < wlim)
    wlim = yow;
  (list_t++)->e_off = r;
  while(1) {
    for(q = r + 1; q < qe && !*q; q++);
    if(q >= qe)
      break;
    for(*q = 0, r = q + 1; r < e_off && *r; r++)
      *r = 0;
    if(list_t == list_off)
      findblob_realloc_list();
    list_t->y = y;
    list_t->w_on = q;
    (list_t++)->e_off = r;
  }
  if((yow = --r - w_on) > elim)
    elim = yow;
}

/* same growing except with eight way connectivity */
void findblob_8grow_s()
{                 
  unsigned char *q, *qs, *qe, *r, *w_on, *e_off;
  unsigned short y, yow;                         
                         
  if((y = list_h->y + 1) == hh)
    return;                     
  e_off = (w_on = rasity + y * ww) + ww;
  if((qs = list_h->w_on + ww - 1) < w_on)
    qs++;                                 
  if((qe = list_h->e_off + ww + 1) > e_off)
    qe--;                                   
  for(q = qs; q < qe && !*q; q++);
  if(q >= qe)                      
    return;   
  if(y > slim)
    slim = y;  
  for(*q = 0, r = q + 1; r < e_off && *r; r++)
    *r = 0;                                    
  if(q == qs) {
    for(q--; q >= w_on && *q; q--)
      *q = 0;                      
    q++;      
  }      
  if(list_t == list_off)
    findblob_realloc_list();
  list_t->y = y;             
  if((yow = (list_t->w_on = q) - w_on) < wlim)
    wlim = yow;                                
  (list_t++)->e_off = r;
  while(1) {             
    for(q = r + 1; q < qe && !*q; q++);
    if(q >= qe)                         
      break;    
    for(*q = 0, r = q + 1; r < e_off && *r; r++)
      *r = 0;                                    
    if(list_t == list_off)
      findblob_realloc_list();
    list_t->y = y;             
    list_t->w_on = q;
    (list_t++)->e_off = r;
  }                        
  if((yow = --r - w_on) > elim)
    elim = yow;                 
}

/********************************************************************/

/* Reallocates the list to a larger size. */

void findblob_realloc_list()
{
  unsigned int newsize;
  RUN *oldlist;

  if((newsize = list_off - (oldlist = list) + LIST_INCR) >
    LIST_MAXSIZE)
    fatalerr("findblob_realloc_list", "list would exceed \
LIST_MAXSIZE elts", NULL);
  if(!(list = (RUN *)realloc(oldlist, newsize * sizeof(RUN))))
    syserr("findblob_realloc_list", "realloc", "list");
  list_off = list + newsize;
  list_h = list + (list_h - oldlist);
  list_t = list + (list_t - oldlist);
}

/********************************************************************/


/************************************************************/
/*         Routine:   findblob_stats()                      */
/*         Author:    G. T. Candela -> "findblob()"         */
/*                                                          */
/*         Modifications:                                   */
/*           5/2/94 - faster version: uses runs             */
/*           Edited:    C. I. Watson  -> "findblob_stats()" */
/*           Date:      9/1/94                              */
/************************************************************/

/* Each time findblob_stats is called, it either returns blob stats of the
true pixels of a binary raster, or indicates that it encountered no
blobs in its col-major scan from the desired starting position to the
bottom-right corner of the raster.  Parameters are:

  ras: Input binary raster, represented using one byte per pixel.
  w: Width of ras.
  h: Height of ras.
  start_x, start_y: Addresses of the x- and y-coordinates of
    the pixel at which caller wants findblob to start -- or resume --
    its column-major scan for blobs.  (To find all blobs,
    caller can use a loop that initializes *start_x and
    *start_y to zeros and does not change them thereafter, and then
    should stop looping when findblob returns 0, which indicates
    that the previous call had returned the last blob; if raster has
    no blobs, first call of findblob will return 0.  But caller also
    has the options of starting *start_x and *start_y at
    something other than (0,0), and changing them between calls of
    findblob.)
  box_x, box_y: Addresses of the coordinates of the top-left corner
    of the bounding box of the blob that was found; the coordinates
    are with respect to the input raster.
  box_w, box_h: Upon return, these always contain the width and
    height of the bounding box of the blob that was found; these
    may or may not be the width and height of the output raster.

The possible function values returned by findblob_stats are:
     0: It reached the bottom-right pixel without finding a blob.
     1: It found a blob, and is returning it in desired format.

*********************************************************************/

int findblob_stats_rw(unsigned char *ras, int w, int h,
                      int *start_x, int *start_y, int *box_x, int *box_y,
                      int *box_w, int *box_h)
{
  unsigned char *p, *ps;
  unsigned short x, y;

  if(list == (RUN *)NULL) {
    if(!(list = (RUN *)malloc(LIST_STARTSIZE * sizeof(RUN))))
      syserr("findblob_malloc_list", "malloc", "list");
    list_off = list + LIST_STARTSIZE;
  }
  rasity = ras;
  ww = w;
  hh_m1 = (hh = h) - 1;
  if(*start_x < 0 || *start_x >= (int)ww ||
     *start_y < 0 || *start_y >= (int)hh)
    fatalerr("findblob_stats_rw", "scan start position is off raster",
      "start_x, start_y");
  x = *start_x;
  y = *start_y;
  /* Col-majorly scan for a seed pixel. */
  for(p = ras + y * ww + x, ps = ras + y * ww + w-1; !*p;) {
    if(p < ps)
      p++;
    else {
      if(++y == hh)
	return 0;
      p = ras + y*ww;
      ps += ww;
    }
  }
  /* Grow seed pixel to a seed run. */
  findblob_seed_to_run(y, p);
  /* Use a queue to grow seed run into a complete blob.  Queue starts
  as just the seed run; read run from queue head, produce connecting
  runs (if any) and put them on queue tail, read another run from
  head, etc., until queue becomes empty.  List space is not recycled:
  when growth of blob is finished, list contains all runs that were
  ever in the queue. */
  for(list_t = (list_h = list) + 1; list_h < list_t; list_h++) {
    findblob_grow_n();
    findblob_grow_s();
  }
  /* Growth of blob is finished.  Go through list to find what pixels
  to set in output raster. */
  *start_x = x;
  *start_y = y;
  *box_x = wlim;
  *box_y = nlim;
  *box_w = elim - wlim + 1;
  *box_h = slim - nlim + 1;

  return 1;
}

/********************************************************************/
int findblob_stats_cl(unsigned char *ras, int w, int h,
                      int *start_x, int *start_y, int *box_x, int *box_y,
                      int *box_w, int *box_h)
{
  unsigned char *p, *ps;
  unsigned short x, y;

  if(list == (RUN *)NULL) {
    if(!(list = (RUN *)malloc(LIST_STARTSIZE * sizeof(RUN))))
      syserr("findblob_malloc_list", "malloc", "list");
    list_off = list + LIST_STARTSIZE;
  }
  rasity = ras;
  ww = w;
  hh_m1 = (hh = h) - 1;
  if(*start_x < 0 || *start_x >= (int)ww ||
     *start_y < 0 || *start_y >= (int)hh)
    fatalerr("findblob_stats_cl", "scan start position is off raster",
      "start_x, start_y");
  x = *start_x;
  y = *start_y;
  /* Col-majorly scan for a seed pixel. */
  for(p = ras + y * ww + x, ps = ras + hh_m1 * ww + x; !*p;) {
    if(p < ps)
      p += ww;
    else {
      if(++x == ww)
	return 0;
      p = ras + x;
      ps++;
    }
  }
  y = (p - ras) / (int)ww;
  /* Grow seed pixel to a seed run. */
  findblob_seed_to_run(y, p);
  /* Use a queue to grow seed run into a complete blob.  Queue starts
  as just the seed run; read run from queue head, produce connecting
  runs (if any) and put them on queue tail, read another run from
  head, etc., until queue becomes empty.  List space is not recycled:
  when growth of blob is finished, list contains all runs that were
  ever in the queue. */
  for(list_t = (list_h = list) + 1; list_h < list_t; list_h++) {
    findblob_grow_n();
    findblob_grow_s();
  }
  /* Growth of blob is finished.  Go through list to find what pixels
  to set in output raster. */
  *start_x = x;
  *start_y = y;
  *box_x = wlim;
  *box_y = nlim;
  *box_w = elim - wlim + 1;
  *box_h = slim - nlim + 1;

  return 1;
}

/********************************************************************/
/* should call this function when you are done with a session of    */
/* findblobs.                                                       */
/********************************************************************/
void end_findblobs()
{
   if(list != (RUN *)NULL){
      free(list);
      list = (RUN *)NULL;
   }
}
