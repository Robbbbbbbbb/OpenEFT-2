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
      LIBRARY: MLP - Multi-Layer Perceptron Neural Network

      FILE:    RD_CWTS.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      ROUTINES:
#cat: rd_cwts - Reads a class-weights file.

***********************************************************************/

/*
Input args:
  nouts: Number of output nodes, which is the number of classes.
  short_classnames: The short names of the classes, in their standard
    order, in their form as provided by lng2shrt, i.e. padded with a
    space in front if original short name read from file had only one
    char.
  class_wts_infile: The class-weights file to be read.  Each line
    consists of a short class-name, non-newline whitespace char(s),
    then the class-weight (prior weight) for this class.  The order of
    the lines of this file does not matter.  Whether or not the short
    class-names in this file have a padding blank in front when only
    1 char long, or not, does not matter.

Output arg:
  class_wts: Buffer containing the class-weights, malloced by this
    routine.  On return, for each 0 <= i < nouts, (*class_wts)[i] will
    contain the class-weight that goes with the class whose short name
    is short_classnames[i]; so, these weights will be in the standard
    order, since the elts of short_classnames (supposedly) are.
*/

#include <mlp.h>

void rd_cwts(int nouts, char **short_classnames, char *class_wts_infile,
             float **class_wts)
{
  FILE *fp;
  char str[100], line[100], *set, ascn[50], yowchar;
  int nlines, iline, i;
  float awt;

  if((fp = fopen(class_wts_infile, "rb")) == (FILE *)NULL)
    syserr("rd_cwts", "fopen for reading failed", class_wts_infile);
  for(nlines = 0; fgets(line, 100, fp); nlines++);
  rewind(fp);
  if(nlines != nouts) {
    sprintf(str, "No. of lines in %s, %d, does not equal\n\
nouts arg, %d", class_wts_infile, nlines, nouts);
    fatalerr("rd_cwts", str, NULL);
  }
  if((set = calloc(nouts, sizeof(char))) == (char *)NULL)
    syserr("rd_cwts", "calloc", "set");
  if((*class_wts = (float *)malloc(nouts * sizeof(float))) ==
    (float *)NULL)
    syserr("rd_cwts", "malloc", "*class_wts");
  for(iline = 1; iline <= nouts; iline++) {
    fgets(line, 100, fp);
    if(sscanf(line, "%s %f", ascn, &awt) != 2) {
      sprintf(str, "line %d of %s does not consist of a string (a\n\
short class-name) and a floating-point no. (class-weight), as \
required", iline, class_wts_infile);
      fatalerr("rd_cwts", str, NULL);
    }
    if(strlen(ascn) > 2) {
      sprintf(str, "line %d of %s contains short name %s with\n> 2 \
characters", iline, class_wts_infile, ascn);
      fatalerr("rd_cwts", str, NULL);
    }
    /* Since the names in short_classnames have already been padded
    to 2 chars (blank in front) when necessary, do the same to the
    current supposed short class-name before comparing. */
    if(strlen(ascn) == 1) {
      yowchar = ascn[0];
      sprintf(ascn, " %c", yowchar);
    }
    for(i = 0; i < nouts; i++)
      if(!strcmp(ascn, short_classnames[i])) {
	(*class_wts)[i] = awt;
	set[i] = TRUE;
	break;
      }
  }
  fclose(fp);
  for(i = 0; i < nouts; i++)
    if(!set[i]) {
      sprintf(str, "%s does not set a class-weight for\n\
short class-name %s", class_wts_infile, short_classnames[i]);
      fatalerr("rd_cwts", str, NULL);
    }
  free(set);
}
