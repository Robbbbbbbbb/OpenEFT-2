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

      FILE:    GOT_BLK.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      Reads a block of data from the specfile

      ROUTINES:
#cat: got_blk - Scans enough of the specfile to get the next run-block (if
#cat:           there is one), filling the parms struture with the scanned and
#cat:           converted values and setting its setting-indicators.
      
***********************************************************************/

/*
Input arg:
  fp: FILE pointer of specfile.

Output args:
  parms: A PARMS structure.  The "val" members of its (parm-specific)
    members are set according to info scanned from the specfile; if
    yow is TRUE, the "set" members are also set, showing which parms
    have been set in the specfile and which have been left unset.
  gb_any_error: Comes back TRUE (FALSE) if any error situations
    were (were not) detected.
  runblock_start_linenum: The number of the first line of the block,
    defined as the first line used by an unignorable phrase of the
    block.

Return value:
  TRUE: Another run-block was scanned in.
  FALSE: The specfile is exhausted, i.e. no more run-blocks are left.
*/

#include <mlp.h>

int got_blk(FILE *fp, PARMS *parms, char *gb_any_error,
            int *runblock_start_linenum)
{
  char namestr[200], valstr[200], illegal_phrase[500], errstr[500],
    errstr2[500], some_content = FALSE, ret;
  int linenum;

  *gb_any_error = FALSE;
  memset(parms, 0, sizeof(PARMS));
  while(1) {
    ret = get_phr(fp, namestr, valstr, illegal_phrase, &linenum);
    if(ret == WORD_PAIR) {
      if(!some_content)
	*runblock_start_linenum = linenum;
      some_content = TRUE;
      if(!st_nv_ok(namestr, valstr, linenum, parms, errstr)) {
	eb_cat(errstr);
	*gb_any_error = TRUE;
      }
    }
    else if(ret == NEWRUN) {
      if(some_content)
	return TRUE;
      else
	continue;
    }
    else if(ret == ILLEGAL_PHRASE) {
      if(!some_content)
	*runblock_start_linenum = linenum;
      some_content = TRUE;
      sprintf(errstr2, "ERROR, line %d: illegal phrase %s", linenum,
        illegal_phrase);
      strm_fmt(errstr2, errstr);
      eb_cat(errstr);
      *gb_any_error = TRUE;
    }
    else /* ret == FINISHED */
      return some_content;
  }
}
