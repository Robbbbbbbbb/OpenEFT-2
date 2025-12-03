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

      FILE:    SCANSPEC.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: scanspec - Makes a scan through the whole specfile.  Counts the "run
#cat:            blocks" and reports the count.  For each run block, checks
#cat:            the parm settings, and reports to stderr both illegal
#cat:            situations and situations which are worthy of warnings.

***********************************************************************/

/*
Some errors or
warnable situations may go unreported on first scan, if there are
multiple kinds of errors (e.g. a parm is set to an illegal value, and
is also set again later), but repeated fixing of the specfile and
re-running should turn up all errors and warnable situations, so that
eventually the specfile can be gotten into a correct state.

Input arg:
  specfile: The specification file, containing the name-value pairs
    for one or more runs, with each run-block separated from the next
    one by "newrun" or "NEWRUN", and with C-style comments allowed.

Output args:
  n_runblocks: How many run-blocks the specfile contains.
  any_warnings: TRUE (FALSE) if specfile contains (does not contain)
    any warnable situation (e.g. setting of a superfluous parm) in
    any run-block.
  any_errors: TRUE (FALSE) if specfile contains (does not contain)
    any error situation (e.g. omission of a mandotory parm) in any
    run-block.
*/

#include <mlp.h>

void scanspec(char specfile[], int *n_runblocks, char *any_warnings,
              char *any_errors)
{
  FILE *fp;
  char *barf, heading_str[100], gb_any_errors, cb_any_warnings,
    cb_any_errors;
  int i, runblock_start_linenum;
  static PARMS parms;

  if((fp = fopen(specfile, "rb")) == (FILE *)NULL)
    syserr("scanspec", "fopen for reading failed", specfile);
  for(*n_runblocks = 0, *any_warnings = *any_errors = FALSE; ;
    (*n_runblocks)++) {

    /* Read next run-block of parameter settings from specfile into
    a structure, doing some checking. */
    if(!got_blk(fp, &parms, &gb_any_errors, &runblock_start_linenum)) {
      fclose(fp);
      return;
    }
    if(gb_any_errors)
      *any_errors = TRUE;

    /* Do additional checking of the parm settings. */
    ch_bprms(&parms, &cb_any_warnings, &cb_any_errors);
    if(cb_any_warnings)
      *any_warnings = TRUE;
    if(cb_any_errors)
      *any_errors = TRUE;

    /* If any errors or warnings occurred in this run-block, write
    their error messages under a heading. */
    if(gb_any_errors || cb_any_warnings || cb_any_errors) {
      if(parms.train_or_test.ssl.set)
	barf = ((parms.train_or_test.val == TRAIN) ? "Training run" :
          "Testing run");
      else
	barf = "Run";
      sprintf(heading_str, "%s starting at line %d:", barf,
        runblock_start_linenum);
      fprintf(stderr, "\n  %s\n  ", heading_str);
      for(i = strlen(heading_str); i; i--)
	fprintf(stderr, "-");
      fprintf(stderr, "\n");
      fputs(eb_get(), stderr);
      eb_clr();
    }
  }
  fclose(fp);
}
