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

      FILE:    GR_CM.C
      AUTHORS: Stan Janet
      DATE:    12/03/1990
      UPDATED: 04/20/2005 by MDG

      Handles grayscale colormaps for image displaying.

      ROUTINES:
#cat: gr_cm - Returns a gray colormap of the specified depth.
#cat: 
#cat: set_gray_colormap - Set the colormap intensities to be gray.
#cat: 

***********************************************************************/

/* LINTLIBRARY */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <util.h>
#include <gr_cm.h>

/************************************************************************/
/* Returns a gray colormap of the specified depth, setting *bp and *wp  */
/* to the correct values for the blackest and whitest pixels.           */
/* Visual should be set to the DefaultVisual. If the class for this     */
/*	visual is non-static (GrayScale or PseudoColor), a suitable     */
/*	visual must be found. If it is, visual is reset; otherwise      */
/*	returns None.                                                   */
/* A fatalerr error occurs if XCreateColormap() fails.                     */
/************************************************************************/

Colormap gray_colormap(Display *display, Visual **visual,
              const unsigned int depth)
{
XVisualInfo vinfo;
Colormap cmap;
int screen;

screen = DefaultScreen(display);

if (((*visual)->class != GrayScale) && ((*visual)->class != PseudoColor)) {
	if ((! XMatchVisualInfo(display,screen,depth,GrayScale,&vinfo)) &&
	    (! XMatchVisualInfo(display,screen,depth,PseudoColor,&vinfo))) {
#ifdef DEBUG
		(void) fprintf(stderr,"Cannot find a suitable visual\n");
#endif
		return None;
	}
	*visual = vinfo.visual;
}

cmap = XCreateColormap(display,RootWindow(display,screen),*visual,AllocAll);
if (cmap == None)
	fatalerr("gray_colormap","XCreateColormap failed",(char *)NULL);

return cmap;
}

/*****************************************************************/
/* Sets the colormap intensities to be gray (equal r,g&b values) */
/*	and evenly spread out between black (0) and white        */
/*	(the maximum value for a unsigned short).                */
/*****************************************************************/

void set_gray_colormap(Display *display, Colormap cmap,
          const unsigned int cmap_size, const unsigned long wp)
{
register unsigned long i;
unsigned short m;
unsigned short flags = (DoRed | DoGreen | DoBlue);
XColor *cmap_defs;

if (! cmap_size)
	fatalerr("set_gray_colormap","colormap size is zero",(char *)NULL);

if (wp && (wp != (unsigned long)cmap_size - 1L))
	fatalerr("set_gray_colormap",
		"white pixel must be zero or (colormap size - 1)",
		(char *)NULL);

cmap_defs = (XColor *) malloc((unsigned int) (cmap_size * sizeof(XColor)));
if (cmap_defs == (XColor *) NULL)
	fatalerr("set_gray_colormap","malloc failed",(char *)NULL);

for (i=0; i < cmap_size; i++) {
	cmap_defs[i].pixel = i;
	m = ((wp ? i : ((cmap_size-1)-i)) / (double) (cmap_size - 1)) * ((unsigned short) ~0) + 0.5;
#ifdef DEBUG
	(void) printf("m[%d] = %u\n",i,m);
#endif
	cmap_defs[i].red   = m;
	cmap_defs[i].green = m;
	cmap_defs[i].blue  = m;
	cmap_defs[i].flags = flags;
}

XStoreColors(display,cmap,cmap_defs,cmap_size);

free((char *) cmap_defs);
}
