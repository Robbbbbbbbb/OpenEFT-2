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

      FILE:    ST_NV_OK.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/22/2005 by MDG

      ROUTINES:
#cat: st_nv_ok - Given a supposed parmname-parmvalue pair, and the number
#cat:            of the specfile line from which it was read, this routine
#cat:            does a bunch of calls of mtch_pnm to find out if the parmname
#cat:            matches any of the legal parm names.

***********************************************************************/

/*
If there is a match, mtch_pnm sets the parm name
and parm value into the appropriate member of the parms structure
(unless it detects an error).  See the comment of mtch_pnm.

NOTE: It is essential, in the calls here of mtch_pnm, that the calls
that use MP_SWITCH have the last argument consistent with the defines
for that parm in parms.h.  For example, the call below for the
"purpose" parm uses MP_SWITCH and a last argument of "classifier 0
fitter 1", and this is consistent with the following lines in parms.h:
#define CLASSIFIER (char)0
#define FITTER     (char)1
It is essential that the numbers in the defines are the same as those
used in the last arg to mtch_pnm. */

/* [This should perhaps be changed so that the stopping-controlling
parms (e.g. gwgoal) do not all HAVE to be set: i.e., only the ones
that you set, are used.  The ones you omit can be set by the program
to huge values or whatever is appropriate, so as to nullify their
effects.  That would have the advantage that if there are some
stopping-parms that a user hardly ever wants to actually use, that
user can produce less-verbose specfiles by totally omitting to mention
these parms; but, they still exist, in case any user wants to use them
sometime.  Of course, if the short outfile and standard output are to
contain a stopping-conditions message, it should not waste lines on
any conditions whose parms were omitted.] */

#include <mlp.h>

char st_nv_ok(char *namestr, char *valstr, int linenum, PARMS *parms,
              char *errstr)
{
  char errstr2[200];
  NVEOL nveol;

  nveol.namestr = namestr;
  nveol.valstr = valstr;
  nveol.errstr = errstr;
  nveol.ok = FALSE;
  nveol.linenum = linenum;

  if(
     !mtch_pnm(&nveol,
      "long_outfile", &(parms->long_outfile), MP_FILENAME, NULL)
     && !mtch_pnm(&nveol,
       "short_outfile", &(parms->short_outfile), MP_FILENAME, NULL)
     && !mtch_pnm(&nveol,
       "errfunc", &(parms->errfunc), MP_SWITCH,
       "mse 0 type_1 1 pos_sum 2")
     && !mtch_pnm(&nveol,
       "regfac", &(parms->regfac), MP_FLOAT, "0. +inf")
     && !mtch_pnm(&nveol,
       "alpha", &(parms->alpha), MP_FLOAT, NULL)
     && !mtch_pnm(&nveol,
       "purpose", &(parms->purpose), MP_SWITCH,
       "classifier 0 fitter 1")
     && !mtch_pnm(&nveol,
       "boltzmann", &(parms->boltzmann), MP_SWITCH,
       "no_prune 0 abs_prune 2 square_prune 3")
     && !mtch_pnm(&nveol,
       "temperature", &(parms->temperature), MP_FLOAT, "0. +inf")
     && !mtch_pnm(&nveol,
       "patterns_infile", &(parms->patterns_infile), MP_FILENAME,
       NULL)
     && !mtch_pnm(&nveol,
       "wts_infile", &(parms->wts_infile), MP_FILENAME, NULL)
     && !mtch_pnm(&nveol,
       "wts_outfile", &(parms->wts_outfile), MP_FILENAME, NULL)
     && !mtch_pnm(&nveol,
       "npats", &(parms->npats), MP_INT, "1 +inf")
     && !mtch_pnm(&nveol,
       "ninps", &(parms->ninps), MP_INT, "1 +inf")
     && !mtch_pnm(&nveol,
       "nhids", &(parms->nhids), MP_INT, "1 +inf")
     && !mtch_pnm(&nveol,
       "nouts", &(parms->nouts), MP_INT, "1 +inf")
     && !mtch_pnm(&nveol,
       "seed", &(parms->seed), MP_INT, "1 +inf")
     && !mtch_pnm(&nveol,
       "niter_max", &(parms->niter_max), MP_INT, "1 +inf")
     && !mtch_pnm(&nveol,
       "egoal", &(parms->egoal), MP_FLOAT, NULL)
     && !mtch_pnm(&nveol,
       "gwgoal", &(parms->gwgoal), MP_FLOAT, NULL)
     && !mtch_pnm(&nveol,
       "nfreq", &(parms->nfreq), MP_INT, NULL)
     && !mtch_pnm(&nveol,
       "errdel", &(parms->errdel), MP_FLOAT, NULL)
     && !mtch_pnm(&nveol,
       "oklvl", &(parms->oklvl), MP_FLOAT, "0. 1.")
     && !mtch_pnm(&nveol,
       "nokdel", &(parms->nokdel), MP_INT, NULL)
     && !mtch_pnm(&nveol,
       "train_or_test", &(parms->train_or_test), MP_SWITCH,
       "train 0 test 1")
     && !mtch_pnm(&nveol,
       "acfunc_hids", &(parms->acfunc_hids), MP_SWITCH,
       "sinusoid 0 sigmoid 1 linear 2")
     && !mtch_pnm(&nveol,
       "acfunc_outs", &(parms->acfunc_outs), MP_SWITCH,
       "sinusoid 0 sigmoid 1 linear 2")
     && !mtch_pnm(&nveol,
       "priors", &(parms->priors), MP_SWITCH,
       "allsame 0 class 1 pattern 2 both 3")
     && !mtch_pnm(&nveol,
       "class_wts_infile", &(parms->class_wts_infile), MP_FILENAME,
       NULL)
     && !mtch_pnm(&nveol,
       "pattern_wts_infile", &(parms->pattern_wts_infile), MP_FILENAME,
       NULL)
     && !mtch_pnm(&nveol,
       "lcn_scn_infile", &(parms->lcn_scn_infile), MP_FILENAME, NULL)
     && !mtch_pnm(&nveol,
       "patsfile_ascii_or_binary", &(parms->patsfile_ascii_or_binary),
       MP_SWITCH, "ascii 0 binary 1")
     && !mtch_pnm(&nveol,
       "trgoff", &(parms->trgoff), MP_FLOAT, "0. 1.")
     && !mtch_pnm(&nveol,
       "lbfgs_mem", &(parms->lbfgs_mem), MP_INT, "1 +inf")
     && !mtch_pnm(&nveol,
       "scg_earlystop_pct", &(parms->scg_earlystop_pct), MP_FLOAT,
       "0. +inf")
     && !mtch_pnm(&nveol,
       "lbfgs_gtol", &(parms->lbfgs_gtol), MP_FLOAT, "1.e-04 +inf")
     && !mtch_pnm(&nveol,
       "do_confuse", &(parms->do_confuse), MP_SWITCH,
       "false 0 true 1")
     && !mtch_pnm(&nveol,
       "show_acs_times_1000", &(parms->show_acs_times_1000),
       MP_SWITCH, "false 0 true 1")
     && !mtch_pnm(&nveol,
       "do_cvr", &(parms->do_cvr), MP_SWITCH, "false 0 true 1")

     ) {

    /* supposed parm-name does not match any legal parm-name */
    sprintf(errstr2, "ERROR, line %d: illegal parm name %s",
      linenum, namestr);
    strm_fmt(errstr2, errstr);
    nveol.ok = FALSE;
  }

  return nveol.ok;
}
