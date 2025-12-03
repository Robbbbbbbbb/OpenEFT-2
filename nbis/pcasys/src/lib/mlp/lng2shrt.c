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

      FILE:    LNG2SHRT.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      Used when reading specfile data.

      ROUTINES:
#cat: lng2shrt - Reads a file that shows the short class-name corresponding
#cat:            to each long class-name, and makes a buffer of the short
#cat:            names.
      
***********************************************************************/

/*

Input args:
  nouts: Number of output nodes, which is the number of classes.
  long_classnames: The long names of the classes, in their standard
    order, i.e. the order in which they appear in the patterns file.
  lcn_scn_infile: Input file.  Each line should consist of a long
    class-name, then non-newline whitespace char(s), then the
    corresponding short class-name, of 1 or 2 chars.  The order of
    the lines of this file does not matter.

Output arg:
  short_classnames: The short class-names read from the file, in a
    buffer malloced by this routine.  Their order will correspond to
    that of the provided long names, i.e. the i'th short name in this
    buffer will go with the i'th long name in long_classnames; so,
    these short names will be in the standard order, since the names
    in long_classnames (supposedly) are.  Any short name which, as
    read from lcn_scn_infile, has only 1 char, will be padded in front
    with a space by this routine.
*/  

#include <mlp.h>

void lng2shrt(int nouts, char **long_classnames, char *lcn_scn_infile,
              char ***short_classnames)
{
  FILE *fp;
  char str[200], line[100], *set, alcn[50], ascn[50];
  int nlines, iline, i;

  if((fp = fopen(lcn_scn_infile, "rb")) == (FILE *)NULL)
    syserr("lng2shrt", "fopen for reading failed",
      lcn_scn_infile);
  for(nlines = 0; fgets(line, 100, fp); nlines++);
  rewind(fp);
  if(nlines != nouts) {
    sprintf(str, "No. of lines in %s, %d, does not equal \
nouts arg, %d", lcn_scn_infile, nlines, nouts);
    fatalerr("lng2shrt", str, NULL);
  }
  if((set = calloc(nouts, sizeof(char))) == (char *)NULL)
    syserr("lng2shrt", "calloc", "set");
  if((*short_classnames = (char **)malloc(nouts * sizeof(char *)))
    == (char **)NULL)
    syserr("lng2shrt", "malloc", "*short_classnames");
  for(iline = 1; iline <= nouts; iline++) {
    fgets(line, 100, fp);
    if(sscanf(line, "%s %s", alcn, ascn) != 2) {
      sprintf(str, "line %d of %s does not consist of two strings\n\
(long class-name and corresponding short class-name) as required",
        iline, lcn_scn_infile);
      fatalerr("lng2shrt", str, NULL);
    }
    if(strlen(ascn) > 2) {
      sprintf(str, "line %d of %s contains supposed short\n\
class-name %s having more than 2 characters", iline, lcn_scn_infile,
        ascn);
      fatalerr("lng2shrt", str, NULL);
    }
    for(i = 0; i < nouts; i++)
      if(!strcmp(alcn, long_classnames[i])) {
	if(((*short_classnames)[i] = malloc(3)) == (char *)NULL)
	  syserr("lng2shrt", "malloc", "(*short_classnames)[i]");
	if(strlen(ascn) == 2)
	  strcpy((*short_classnames)[i], ascn);
	else /* strlen is 1 */
	  sprintf((*short_classnames)[i], " %c", ascn[0]);
	set[i] = TRUE;
	break;
      }
  }
  fclose(fp);
  for(i = 0; i < nouts; i++)
    if(!set[i]) {
      sprintf(str, "long class-name %s is not the first string of \
any line of\n%s", long_classnames[i], lcn_scn_infile);
      fatalerr("lng2shrt", str, NULL);
    }
  free(set);
}
