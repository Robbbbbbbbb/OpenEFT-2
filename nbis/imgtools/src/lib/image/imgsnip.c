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

      FILE:    IMGSNIP.C

      AUTHORS: Craig Watson
      DATE:    09/20/2004
      UPDATED: 08/16/2009 by BBandini - gcc 4.4.1 won't compile math function
                             when passed a constant; one mod: sqrt(x20)
                             instead of sqrt(2.0).

      Contains routines to snip a subimage (can be rotated) from the
      original.

***********************************************************************

      ROUTINES:

#cat: snip_rot_subimage - snips a section of an image based on a center
#cat:                     point, angle, width and height into a new image.
#cat:                     Takes nearest pixel value, does not interpolate.
#cat: snip_rot_subimage_interp - snips a section of an image based on a center
#cat:                     point, angle, width and height into a new image.
#cat:                     It interpolates pixel values based on 4 neighbors.

***********************************************************************/

#include <stdlib.h>
#include <defs.h>
#include <math.h>

/*******************************************************************/
void snip_rot_subimage(unsigned char *data, const int w, const int h,
            unsigned char *idata, const int iw, const int ih, const int cx,
            const int cy, const float ang, unsigned char pix)
{
   float c, s, fx, fy, sfx, sfy;
   int ix, iy, x, y;
   float hw, hh;

   hw = (float)iw / 2.0;
   hh = (float)ih / 2.0;

   c = cos(ang);
   s = sin(ang);

   sfx = (cx - (hw * c) - (hh * s));
   sfy = (cy + (hw * s) - (hh * c));

   for(y = 0; y < ih; y++, sfx += s, sfy += c) {
      for(x = 0, fx = sfx, fy = sfy; x < iw; x++, fx += c, fy -= s) {
         ix = sround(fx);
         iy = sround(fy);
         *idata++ = ((ix >= 0) && (ix < w) && (iy >= 0) && (iy < h))?data[ix+iy*w]:pix;
      }
   }
}

/*******************************************************************/
void snip_rot_subimage_interp(unsigned char *data, const int w, const int h,
            unsigned char *idata, const int iw, const int ih, const int cx,
            const int cy, const float ang, unsigned char pix)
{
   float c, s, fx, fy, sfx, sfy;
   int x, y;
   float hw, hh;
   float lx, ly, hx, hy;
   float ilx, ily, ihx, ihy;
   float dlx, dly, dhx, dhy;
   float d1, d2, d3, d4;
   float w1, w2, w3, w4, sw;
   float sq2;
   int p1, p2, p3, p4;

   double x20 = 2.0;
   sq2 = sqrt(x20);

   hw = (float)iw / 2.0;
   hh = (float)ih / 2.0;

   c = cos(ang);
   s = sin(ang);

   sfx = (cx - (hw * c) - (hh * s));
   sfy = (cy + (hw * s) - (hh * c));

   for(y = 0; y < ih; y++, sfx += s, sfy += c) {
      for(x = 0, fx = sfx, fy = sfy; x < iw; x++, fx += c, fy -= s) {
         lx = (float)floor((double)fx);
         ly = (float)floor((double)fy);
         hx = (float)ceil((double)fx);
         hy = (float)ceil((double)fy);
         ilx = (int)lx;
         ily = (int)ly;
         ihx = (int)hx;
         ihy = (int)hy;

         if((ilx >= 0) && (ihx < w) && (ily >= 0) && (ihy < h)) {
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
            *idata = sround((w1*(float)data[p1] + w2*(float)data[p2] +
                             w3*(float)data[p3] + w4*(float)data[p4])/sw);
         }
         else
            *idata = pix;

         idata++;
      }
   }
}
