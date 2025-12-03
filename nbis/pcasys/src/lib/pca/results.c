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

      FILE:    RESULTS.C
      AUTHORS: G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG

      ROUTINES:
#cat: results - Writes classification system results for a test print to
#cat:           output file and if verbose to stdout.

***********************************************************************/

/* Writes classification system results for a test print to output
file and if verbose to stdout, and if graphical version also writes a
terser verion to a graphical window. */

#include <pca.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

void results(const int actual_classind, const int nn_hyp_classind,
          const float nn_confidence, const int found_conup,
          const int hyp_classind, const float confidence, FILE *fp_outfile,
          char *test_rasterfile, char *cls_str, const int nout)
{
  char str[200];

  sprintf(str, "is %c; nn: hyp %c, conf %.2f; conup %c; hyp %c, \
conf %.2f; %s\n", cls_str[actual_classind],
    cls_str[nn_hyp_classind], nn_confidence, (found_conup ? 'y' :
    'n'), cls_str[hyp_classind], confidence, ((hyp_classind ==
    actual_classind) ? "right" : "wrong"));
  fprintf(fp_outfile, "%s: %s", lastcomp(test_rasterfile), str);
  fflush(fp_outfile);
  if(isverbose())
    printf("  %s\n", str);
#ifdef GRPHCS
  grphcs_lastdisp(actual_classind, hyp_classind, confidence, cls_str, nout);
#endif
}
