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

      FILE:    LGL_PNM.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      Used in error checking specfile.

      ROUTINES:
#cat: lgl_pnm - Finds out whether a word is one of the legal parameter-names.
      
***********************************************************************/

/*
Input arg:
  word: The word to be tested to find out if it is a legal parm-name.

Return value:
  TRUE (FALSE): The input word is (is not) a legal parm-name.
*/

#include <mlp.h>

char lgl_pnm(char word[])
{
  /* (Not a very efficient way to check whther a string matches one
  of a set of stored strings, but probably fast enough for this
  application.) */
  if(!strcmp(word, "errfunc"))
    return TRUE;
  if(!strcmp(word, "purpose"))
    return TRUE;
  if(!strcmp(word, "boltzmann"))
    return TRUE;
  if(!strcmp(word, "train_or_test"))
    return TRUE;
  if(!strcmp(word, "acfunc_hids"))
    return TRUE;
  if(!strcmp(word, "acfunc_outs"))
    return TRUE;
  if(!strcmp(word, "priors"))
    return TRUE;
  if(!strcmp(word, "long_outfile"))
    return TRUE;
  if(!strcmp(word, "short_outfile"))
    return TRUE;
  if(!strcmp(word, "patterns_infile"))
    return TRUE;
  if(!strcmp(word, "wts_infile"))
    return TRUE;
  if(!strcmp(word, "wts_outfile"))
    return TRUE;
  if(!strcmp(word, "class_wts_infile"))
    return TRUE;
  if(!strcmp(word, "pattern_wts_infile"))
    return TRUE;
  if(!strcmp(word, "lcn_scn_infile"))
    return TRUE;
  if(!strcmp(word, "npats"))
    return TRUE;
  if(!strcmp(word, "ninps"))
    return TRUE;
  if(!strcmp(word, "nhids"))
    return TRUE;
  if(!strcmp(word, "nouts"))
    return TRUE;
  if(!strcmp(word, "seed"))
    return TRUE;
  if(!strcmp(word, "niter_max"))
    return TRUE;
  if(!strcmp(word, "nfreq"))
    return TRUE;
  if(!strcmp(word, "nokdel"))
    return TRUE;
  if(!strcmp(word, "regfac"))
    return TRUE;
  if(!strcmp(word, "alpha"))
    return TRUE;
  if(!strcmp(word, "temperature"))
    return TRUE;
  if(!strcmp(word, "egoal"))
    return TRUE;
  if(!strcmp(word, "gwgoal"))
    return TRUE;
  if(!strcmp(word, "errdel"))
    return TRUE;
  if(!strcmp(word, "oklvl"))
    return TRUE;
  if(!strcmp(word, "patsfile_ascii_or_binary"))
    return TRUE;
  if(!strcmp(word, "trgoff"))
    return TRUE;
  if(!strcmp(word, "lbfgs_mem"))
    return TRUE;
  if(!strcmp(word, "scg_earlystop_pct"))
    return TRUE;
  if(!strcmp(word, "lbfgs_gtol"))
    return TRUE;
  if(!strcmp(word, "do_confuse"))
    return TRUE;
  if(!strcmp(word, "show_acs_times_1000"))
    return TRUE;
  if(!strcmp(word, "do_cvr"))
    return TRUE;

  return FALSE;
}
