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

      FILE:    INITS.C
      AUTHORS: Craig Watson
               G. T. Candela
      DATE:    1995
      UPDATED: 04/19/2005 by MDG

      Initialization routines for mkoas (make orientation arrays command)
      and pcasys (pcasys.c, which is for the pcasys and pcasysx demos),
      and supporting paramaters-reading routines:

      ROUTINES:
#cat: mkoas_init - Initialization for mkoas command, which makes and stores
#cat:              orientation arrays from a set of fingerprints.
#cat: mkoas_readparms - Reads a file containing parms for mkoas.
#cat: mkoas_check_parms_allset - Checks that every mkoas parm has been set.
#cat: pcasys_init - Initialization for demo commands pcasys and pcasysx.
#cat: pcasys_readparms - Reads a file containing parms for pcasys.
#cat: check_cls_str - Check for valid class strings.

***********************************************************************/

#include <pca.h>
#include <datafile.h>
#ifdef GRPHCS
#include <grphcs.h>
#endif

static struct {
  char sgmnt_fac_n, sgmnt_min_fg, sgmnt_max_fg, sgmnt_nerode,
    sgmnt_rsblobs, sgmnt_fill, sgmnt_min_n, sgmnt_hist_thresh,
    sgmnt_origras_wmax, sgmnt_origras_hmax, sgmnt_fac_min,
    sgmnt_fac_del, sgmnt_slope_thresh, enhnc_rr1, enhnc_rr2,
    enhnc_pow, rors_slit_range_thresh, r92a_discard_thresh,
    rgar_std_corepixel_x, rgar_std_corepixel_y, ascii_oas,
    update_freq, clobber_oas_file, proc_images_list, oas_file;
} mkoas_setflags;

/******************************************************************/

/* Initialization routine for mkoas (make orientation arrays) command.
Reads default preprocessing-parms file, then user-provided parms file
(which overrides defaults, and also specifies two things that do not
have defaults: the list of fingeprints to process, and the output
file). */

void mkoas_init(char *prsfile, SGMNT_PRS *sgmnt_prs, ENHNC_PRS *enhnc_prs,
          int *rors_slit_range_thresh, float *r92a_discard_thresh,
          RGAR_PRS *rgar_prs, int *ascii_oas, int *update_freq,
          int *clobber_oas_file, FILE **fp_proc_images_list, int *n_oas,
          char *oas_file)
{
  char *datadir, proc_images_list[200], str[400];

  /* Reads first the oas (orientation arrays) parms file, then the
  mkoas additional-parms default file, then the required user parms
  file.  Checks that every parm was set. */
  memset(&mkoas_setflags, 0, sizeof(mkoas_setflags));

  datadir = get_datadir();
  sprintf(str, "%s/parms/oas.prs", datadir);

  mkoas_readparms(str, sgmnt_prs, enhnc_prs, rors_slit_range_thresh,
    r92a_discard_thresh, rgar_prs, ascii_oas, update_freq,
    clobber_oas_file, proc_images_list, oas_file);

  sprintf(str, "%s/parms/mkoas.prs", datadir);

  mkoas_readparms(str, sgmnt_prs, enhnc_prs, rors_slit_range_thresh,
    r92a_discard_thresh, rgar_prs, ascii_oas, update_freq,
    clobber_oas_file, proc_images_list, oas_file);
  mkoas_readparms(prsfile, sgmnt_prs, enhnc_prs,
    rors_slit_range_thresh,
    r92a_discard_thresh, rgar_prs, ascii_oas, update_freq,
    clobber_oas_file, proc_images_list, oas_file);
  mkoas_check_parms_allset();
  strcpy(str, tilde_filename(proc_images_list, 0));
  *n_oas = linecount(str);
  *fp_proc_images_list = fopen_ch(str, "rb");
}

/******************************************************************/

/* Reads mkoas parms from a file.  Knows about all the mkoas parms,
which are those in default parms file preproc.prs.  Returns the parms
in various structures, one for each routine, except that sometimes a
routine has just one parm instead of a structure.  Parms here mean
values that control the operation and that do not change during a run,
i.e. they do not mean just all the arguments of the routine.  This
routine also gets the list of fingerprint images to process, and the
name of the output file of orientation arrays (oa's) to produce, and
the ascii/binary switch for that output file.

This routine is set up for checking that every parm gets set.

Also, it checks whether update_freq < 0 (not allowed). */

void mkoas_readparms(char *prsfile, SGMNT_PRS *sgmnt_prs, ENHNC_PRS *enhnc_prs,
          int *rors_slit_range_thresh, float *r92a_discard_thresh,
          RGAR_PRS *rgar_prs, int *ascii_oas, int *update_freq,
          int *clobber_oas_file, char *proc_images_list, char *oas_file)
{
  FILE *fp;
  char line[1000], *p, namestr[100], valstr[1000], str[400],
    str2[400];

  fp = fopen_ch(prsfile, "rb");
  while(fgets(line, 1000, fp)) {
    if((p = strchr(line, '#')))
      *p = 0;
    if(sscanf(line, "%s %s", namestr, valstr) < 2)
      continue;

    /* sgmnt (segmentor) parms: */
    if(!strcmp(namestr, "sgmnt_fac_n")) {
      sgmnt_prs->fac_n = atoi(valstr);
      mkoas_setflags.sgmnt_fac_n = 1;
    }
    else if(!strcmp(namestr, "sgmnt_min_fg")) {
      sgmnt_prs->min_fg = atoi(valstr);
      mkoas_setflags.sgmnt_min_fg = 1;
    }
    else if(!strcmp(namestr, "sgmnt_max_fg")) {
      sgmnt_prs->max_fg = atoi(valstr);
      mkoas_setflags.sgmnt_max_fg = 1;
    }
    else if(!strcmp(namestr, "sgmnt_nerode")) {
      sgmnt_prs->nerode = atoi(valstr);
      mkoas_setflags.sgmnt_nerode = 1;
    }
    else if(!strcmp(namestr, "sgmnt_rsblobs")) {
      sgmnt_prs->rsblobs = atoi(valstr);
      mkoas_setflags.sgmnt_rsblobs = 1;
    }
    else if(!strcmp(namestr, "sgmnt_fill")) {
      sgmnt_prs->fill = atoi(valstr);
      mkoas_setflags.sgmnt_fill = 1;
    }
    else if(!strcmp(namestr, "sgmnt_min_n")) {
      sgmnt_prs->min_n = atoi(valstr);
      mkoas_setflags.sgmnt_min_n = 1;
    }
    else if(!strcmp(namestr, "sgmnt_hist_thresh")) {
      sgmnt_prs->hist_thresh = atoi(valstr);
      mkoas_setflags.sgmnt_hist_thresh = 1;
    }
    else if(!strcmp(namestr, "sgmnt_origras_wmax")) {
      sgmnt_prs->origras_wmax = atoi(valstr);
      mkoas_setflags.sgmnt_origras_wmax = 1;
    }
    else if(!strcmp(namestr, "sgmnt_origras_hmax")) {
      sgmnt_prs->origras_hmax = atoi(valstr);
      mkoas_setflags.sgmnt_origras_hmax = 1;
    }
    else if(!strcmp(namestr, "sgmnt_fac_min")) {
      sgmnt_prs->fac_min = atof(valstr);
      mkoas_setflags.sgmnt_fac_min = 1;
    }
    else if(!strcmp(namestr, "sgmnt_fac_del")) {
      sgmnt_prs->fac_del = atof(valstr);
      mkoas_setflags.sgmnt_fac_del = 1;
    }
    else if(!strcmp(namestr, "sgmnt_slope_thresh")) {
      sgmnt_prs->slope_thresh = atof(valstr);
      mkoas_setflags.sgmnt_slope_thresh = 1;
    }

    /* enhnc (enhancer) parms: */
    else if(!strcmp(namestr, "enhnc_rr1")) {
      enhnc_prs->rr1 = atoi(valstr);
      mkoas_setflags.enhnc_rr1 = 1;
    }
    else if(!strcmp(namestr, "enhnc_rr2")) {
      enhnc_prs->rr2 = atoi(valstr);
      mkoas_setflags.enhnc_rr2 = 1;
    }
    else if(!strcmp(namestr, "enhnc_pow")) {
      enhnc_prs->pow = atof(valstr);
      mkoas_setflags.enhnc_pow = 1;
    }

    /* rors (ridge-valley orientation finder) parm: */
    else if(!strcmp(namestr, "rors_slit_range_thresh")) {
      *rors_slit_range_thresh = atoi(valstr);
      mkoas_setflags.rors_slit_range_thresh = 1;
    }

    /* r92a (registration; calls r92) parm: */
    else if(!strcmp(namestr, "r92a_discard_thresh")) {
      *r92a_discard_thresh = atof(valstr);
      mkoas_setflags.r92a_discard_thresh = 1;
    }

    /* rgar parms, which are just the two coords of the
    standard core pixel, i.e. the standard registration point */
    else if(!strcmp(namestr, "rgar_std_corepixel_x")) {
      rgar_prs->std_corepixel_x = atoi(valstr);
      mkoas_setflags.rgar_std_corepixel_x = 1;
    }
    else if(!strcmp(namestr, "rgar_std_corepixel_y")) {
      rgar_prs->std_corepixel_y = atoi(valstr);
      mkoas_setflags.rgar_std_corepixel_y = 1;
    }

    /* ascii/binary switch for output file */
    else if(!strcmp(namestr, "ascii_oas")) {
      *ascii_oas = !strcmp(valstr, "y");
      mkoas_setflags.ascii_oas = 1;
    }

    /* frequency of status messages to standard output */
    else if(!strcmp(namestr, "update_freq")) {
      *update_freq = atoi(valstr);
      mkoas_setflags.update_freq = 1;
      if(*update_freq < 0) {
	sprintf(str, "update_freq is %d; it must be either 0 (for no \
 updates) or positive (for updates every that many prints)",
          *update_freq);
	fatalerr("mkoas_readparms (file inits.c)", str, prsfile);
      }
    }

    /* If y, then if oas_file already exists, overwrite it; if n,
    then if it already exists, write an error message to standard
    output and exit. */
    else if(!strcmp(namestr, "clobber_oas_file")) {
      *clobber_oas_file = !strcmp(valstr, "y");
      mkoas_setflags.clobber_oas_file = 1;
    }

    /* list of test fingerprint images to process: */
    else if(!strcmp(namestr, "proc_images_list")) {
      strcpy(proc_images_list, valstr);
      mkoas_setflags.proc_images_list = 1;
    }

    /* output file of orientation arrays to produce */
    else if(!strcmp(namestr, "oas_file")) {
      strcpy(oas_file, valstr);
      mkoas_setflags.oas_file = 1;
    }

    else {
      sprintf(str, "illegal parm name %s", namestr);
      sprintf(str2, "parms file %s", prsfile);
      fatalerr("mkoas_readparms (file init.c)", str, str2);
    }
  }
}

/******************************************************************/

/* Checks that every mkoas parm has been set. */

void mkoas_check_parms_allset()
{
  if(!mkoas_setflags.sgmnt_fac_n)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_fac_n was never set", NULL);
  if(!mkoas_setflags.sgmnt_min_fg)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_min_fg was never set", NULL);
  if(!mkoas_setflags.sgmnt_max_fg)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_max_fg was never set", NULL);
  if(!mkoas_setflags.sgmnt_nerode)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_nerode was never set", NULL);
  if(!mkoas_setflags.sgmnt_rsblobs)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_rsblobs was never set", NULL);
  if(!mkoas_setflags.sgmnt_fill)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_fill was never set", NULL);
  if(!mkoas_setflags.sgmnt_min_n)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_min_n was never set", NULL);
  if(!mkoas_setflags.sgmnt_hist_thresh)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_hist_thresh was never set", NULL);
  if(!mkoas_setflags.sgmnt_origras_wmax)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_origras_wmax was never set", NULL);
  if(!mkoas_setflags.sgmnt_origras_hmax)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_origras_hmax was never set", NULL);
  if(!mkoas_setflags.sgmnt_fac_min)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_fac_min was never set", NULL);
  if(!mkoas_setflags.sgmnt_fac_del)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_fac_del was never set", NULL);
  if(!mkoas_setflags.sgmnt_slope_thresh)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm sgmnt_slope_thresh was never set", NULL);
  if(!mkoas_setflags.enhnc_rr1)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm enhnc_rr1 was never set", NULL);
  if(!mkoas_setflags.enhnc_rr2)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm enhnc_rr2 was never set", NULL);
  if(!mkoas_setflags.enhnc_pow)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm enhnc_pow was never set", NULL);
  if(!mkoas_setflags.rors_slit_range_thresh)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm rors_slit_range_thresh was never set", NULL);
  if(!mkoas_setflags.r92a_discard_thresh)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm r92a_discard_thresh was never set", NULL);
  if(!mkoas_setflags.rgar_std_corepixel_x)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm rgar_std_corepixel_x was never set", NULL);
  if(!mkoas_setflags.rgar_std_corepixel_y)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm rgar_std_corepixel_y was never set", NULL);
  if(!mkoas_setflags.ascii_oas)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm ascii_oas was never set", NULL);
  if(!mkoas_setflags.update_freq)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm update_freq was never set", NULL);
  if(!mkoas_setflags.clobber_oas_file)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm clobber_oas_file was never set", NULL);
  if(!mkoas_setflags.proc_images_list)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm proc_images_list was never set", NULL);
  if(!mkoas_setflags.oas_file)
    fatalerr("mkoas_check_parms_allset (file inits.c)",
      "parm oas_file was never set", NULL);
}

/********************************************************************/

/* Initialization routine for pcasys demo.  Reads parms and data
(caller provides the structures and variables, which this routine
fills in, except that for data, this routine allocates the buffers);
also fopens the test image files list for reading, and fopens the
output file for writing, returning the FILE pointers. */

void pcasys_init(char *prsfile, SGMNT_PRS *sgmnt_prs, ENHNC_PRS *enhnc_prs,
          int *rors_slit_range_thresh, float *r92a_discard_thresh,
          RGAR_PRS *rgar_prs, int *trnsfrm_nrows_use, int *pnn_mlp,
          PNN_PRS *pnn_prs, MLP_PARAM *mlp_prs, PSEUDO_PRS *pseudo_prs,
          float *combine_clash_confidence, float **protos_fvs,
          unsigned char **protos_classes, float **tranmat,
          FILE **fp_demo_images_list, FILE **fp_out)
{
  char *datadir, demo_images_list[200], outfile[200], str[400],
    *desc, trnsfrm_matrix_file[200], pnn_protos_fvs_file[200],
    pnn_protos_classes_file[200], mlp_wts_file[200];
  int warp_mouse, clobber_outfile, verbose;
  SLEEPS sleeps;
  char purpose;
  int ncols, ncls;
  char **lcnptr;

  /* Reads first the oas (orientation arrays) parms file, then the
  parms file for the graphical (pcasysx) or non-graphical (pcasys)
  version as appropriate, then (if it exists) the optional user parms
  file. */
  datadir = get_datadir();
  sprintf(str, "%s/parms/oas.prs", datadir);

  pcasys_readparms(str, sgmnt_prs, enhnc_prs, rors_slit_range_thresh,
    r92a_discard_thresh, rgar_prs, trnsfrm_nrows_use,
    trnsfrm_matrix_file, pnn_mlp, pnn_prs, pnn_protos_fvs_file,
    pnn_protos_classes_file, mlp_wts_file, pseudo_prs, combine_clash_confidence,
    &sleeps, &warp_mouse, demo_images_list, outfile, &clobber_outfile,
    &verbose, mlp_prs);

  sprintf(str, "%s/parms/pcasys.prs", datadir);

  pcasys_readparms(str, sgmnt_prs, enhnc_prs, rors_slit_range_thresh,
    r92a_discard_thresh, rgar_prs, trnsfrm_nrows_use,
    trnsfrm_matrix_file, pnn_mlp, pnn_prs, pnn_protos_fvs_file,
    pnn_protos_classes_file, mlp_wts_file, pseudo_prs, combine_clash_confidence,
    &sleeps, &warp_mouse, demo_images_list, outfile, &clobber_outfile,
    &verbose, mlp_prs);

#ifdef __MSYS__
  /* pcasysx is not built for MSYS. */
  sprintf(str, "./pcasysx.prs");
#else
  sprintf(str, "%s/parms/pcasysx.prs", datadir);
#endif

#ifdef GRPHCS
  pcasys_readparms(str, sgmnt_prs, enhnc_prs, rors_slit_range_thresh,
    r92a_discard_thresh, rgar_prs, trnsfrm_nrows_use,
    trnsfrm_matrix_file, pnn_mlp, pnn_prs, pnn_protos_fvs_file,
    pnn_protos_classes_file, mlp_wts_file, pseudo_prs, combine_clash_confidence,
    &sleeps, &warp_mouse, demo_images_list, outfile, &clobber_outfile,
    &verbose, mlp_prs);
#endif
  if(prsfile != (char *)NULL)
    pcasys_readparms(prsfile, sgmnt_prs, enhnc_prs,
      rors_slit_range_thresh, r92a_discard_thresh, rgar_prs,
      trnsfrm_nrows_use, trnsfrm_matrix_file, pnn_mlp, pnn_prs,
      pnn_protos_fvs_file, pnn_protos_classes_file, mlp_wts_file, pseudo_prs,
      combine_clash_confidence, &sleeps, &warp_mouse, demo_images_list,
      outfile, &clobber_outfile, &verbose, mlp_prs);

   setverbose(verbose);
   if(isverbose())
     printf("initialization\n\n");

  if(*pnn_mlp == PNN_CLSFR) {
     /* Set the number of features to use parm for PNN, equal to the
     number of rows to use parm of the transform.  It does not make sense
     for these two numbers to be different. */
     pnn_prs->nfeats_use = *trnsfrm_nrows_use;

     /* If graphical, have graphics code do its initialization.  Send in
     the number of features to use, so the graphics can check whether
     that equals the (only) number of features it knows how to display.  Send in
     some info the graphics code needs: the sleep values, the warp mouse
     switch, and the rgar parms, which are just the standard core pixel
     location (standard registration point). */

     check_cls_str(pnn_prs->cls_str, pnn_prs->nclasses);

     char *msys_fname;
     msys_fname = tilde_filename(trnsfrm_matrix_file, 1);
     matrix_read_dims(msys_fname,
                      &(pnn_prs->trnsfrm_rws), &(pnn_prs->trnsfrm_cls));

     ncols = pnn_prs->trnsfrm_cls;
     if(*trnsfrm_nrows_use > pnn_prs->trnsfrm_rws)
        fatalerr("pcasys_init", "trnsfrm_nrows_use > # rows actually in trnsfrm matrix",
                 "Must be <= # rows");
     pnn_prs->trnsfrm_rws = *trnsfrm_nrows_use;

#ifdef GRPHCS
     grphcs_init(&sleeps, warp_mouse, rgar_prs);
#endif

     /* Read prototype feature vectors from specified file, reading just
     the specified part (how many first protos, and how many first
     features of each one); and, read their classes. */
     msys_fname = tilde_filename(pnn_protos_fvs_file, 1);
     matrix_read_submatrix(msys_fname, 0,
       pnn_prs->nprotos_use - 1, 0, pnn_prs->nfeats_use - 1, &desc,
       protos_fvs);

     free(desc);
     classes_read_subvector_ind(tilde_filename(pnn_protos_classes_file, 1), 0,
       pnn_prs->nprotos_use - 1, &desc, protos_classes, &ncls, &lcnptr);
     free(desc);
     free_dbl_char(lcnptr, ncls);
  }
  else if (*pnn_mlp == MLP_CLSFR) {
     /* Reads network weights, and also various "network architecture"
        specifications, returns architecture parameters and weights */

     char *msys_fname;
     msys_fname = tilde_filename(mlp_wts_file, 1);

     readwts_np(msys_fname, &purpose, &(mlp_prs->ninps),
                 &(mlp_prs->nhids), &(mlp_prs->nouts), &(mlp_prs->acfunc_hids),
                 &(mlp_prs->acfunc_outs), &(mlp_prs->weights));

     if(mlp_prs->ninps != *trnsfrm_nrows_use)
        fatalerr("pcasys_init2","mlp ninps != trnsfrm_nrows_use","must be equal");

     check_cls_str(mlp_prs->cls_str, mlp_prs->nouts);

     msys_fname = tilde_filename(trnsfrm_matrix_file, 1);

     matrix_read_dims(msys_fname, &(mlp_prs->trnsfrm_rws), &(mlp_prs->trnsfrm_cls));

     ncols = mlp_prs->trnsfrm_cls;
     if(*trnsfrm_nrows_use > mlp_prs->trnsfrm_rws)
        fatalerr("pcasys_init", "trnsfrm_nrows_use > # rows actually in trnsfrm matrix",
                 "Must be <= # rows");
     mlp_prs->trnsfrm_rws = *trnsfrm_nrows_use;

#ifdef GRPHCS
     grphcs_init(&sleeps, warp_mouse, rgar_prs);
#endif
  }
  else {
    fprintf(stdout, "Unknown classifier type (%d) 1-PNN, 2-MLP\n", *pnn_mlp);
    exit(-1);
  }

  /* Read the specified number of top rows of the specified
  transform matrix. */

  char *msys_fname = tilde_filename(trnsfrm_matrix_file, 1);

  matrix_read_submatrix(msys_fname, 0,
     *trnsfrm_nrows_use - 1, 0, ncols-1, &desc, tranmat);

  free(desc);

  msys_fname = tilde_filename(demo_images_list, 1);

  *fp_demo_images_list = fopen_ch(msys_fname, "r");

  if(clobber_outfile)
    *fp_out = fopen_ch(tilde_filename(outfile, 0), "wb");
  else
    *fp_out = fopen_noclobber(tilde_filename(outfile, 0));
}

/******************************************************************/
/* Reads pcasys parms from a file. */
void pcasys_readparms(char *prsfile, SGMNT_PRS *sgmnt_prs, ENHNC_PRS *enhnc_prs,
          int *rors_slit_range_thresh, float *r92a_discard_thresh,
          RGAR_PRS *rgar_prs, int *trnsfrm_nrows_use,
          char trnsfrm_matrix_file[], int *pnn_mlp, PNN_PRS *pnn_prs,
          char pnn_protos_fvs_file[], char pnn_protos_classes_file[],
          char mlp_wts_file[], PSEUDO_PRS *pseudo_prs,
          float *combine_clash_confidence, SLEEPS *sleeps, int *warp_mouse,
          char demo_images_list[], char outfile[], int *clobber_outfile,
          int *verbose, MLP_PARAM *mlp_prs)
{
  FILE *fp;
  char line[1000], *p, namestr[100], valstr[1000], str[200],
    str2[400];

  fp = fopen_ch(prsfile, "rb");
  while(fgets(line, 1000, fp)) {
    if((p = strchr(line, '#')))
      *p = 0;
    if(sscanf(line, "%s %s", namestr, valstr) < 2)
      continue;

    /* sgmnt (segmentor) parms: */
    if(!strcmp(namestr, "sgmnt_fac_n"))
      sgmnt_prs->fac_n = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_min_fg"))
      sgmnt_prs->min_fg = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_max_fg"))
      sgmnt_prs->max_fg = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_nerode"))
      sgmnt_prs->nerode = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_rsblobs"))
      sgmnt_prs->rsblobs = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_fill"))
      sgmnt_prs->fill = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_min_n"))
      sgmnt_prs->min_n = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_hist_thresh"))
      sgmnt_prs->hist_thresh = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_origras_wmax"))
      sgmnt_prs->origras_wmax = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_origras_hmax"))
      sgmnt_prs->origras_hmax = atoi(valstr);
    else if(!strcmp(namestr, "sgmnt_fac_min"))
      sgmnt_prs->fac_min = atof(valstr);
    else if(!strcmp(namestr, "sgmnt_fac_del"))
      sgmnt_prs->fac_del = atof(valstr);
    else if(!strcmp(namestr, "sgmnt_slope_thresh"))
      sgmnt_prs->slope_thresh = atof(valstr);

    /* enhnc (enhancer) parms: */
    else if(!strcmp(namestr, "enhnc_rr1"))
      enhnc_prs->rr1 = atoi(valstr);
    else if(!strcmp(namestr, "enhnc_rr2"))
      enhnc_prs->rr2 = atoi(valstr);
    else if(!strcmp(namestr, "enhnc_pow"))
      enhnc_prs->pow = atof(valstr);

    /* rors (ridge-valley orientation finder) parm: */
    else if(!strcmp(namestr, "rors_slit_range_thresh"))
      *rors_slit_range_thresh = atoi(valstr);

    /* r92a (registration; calls r92) parm: */
    else if(!strcmp(namestr, "r92a_discard_thresh"))
      *r92a_discard_thresh = atof(valstr);

    /* rgar parms: */
    else if(!strcmp(namestr, "rgar_std_corepixel_x"))
      rgar_prs->std_corepixel_x = atoi(valstr);
    else if(!strcmp(namestr, "rgar_std_corepixel_y"))
      rgar_prs->std_corepixel_y = atoi(valstr);

    /* trnsform (linear transform) parm: */
    else if(!strcmp(namestr, "trnsfrm_nrows_use"))
      *trnsfrm_nrows_use = atoi(valstr);

    /* trnsform data: transform matrix file: */
    else if(!strcmp(namestr, "trnsfrm_matrix_file"))
      strcpy(trnsfrm_matrix_file, valstr);

    /* type of network to use */
    else if(!strcmp(namestr, "network_type"))
      *pnn_mlp = atoi(valstr);

    /* valid classes string */
    else if(!strcmp(namestr, "cls_str")) {
      strcpy(pnn_prs->cls_str, valstr);
      strcpy(mlp_prs->cls_str, valstr);
    }

    /* MLP parms */
    else if(!strcmp(namestr, "mlp_wts_file"))
      strcpy(mlp_wts_file, valstr);

    /* pnn (Probabilistic Neural Net) parms:
    (The nfeats_use structure member will still exist, but it will not
    be read in from parms file(s): instead, it will be set by
    copying trnsfrm_nrows_use.  It does not make sense for the
    two to be unequal.) */
    else if(!strcmp(namestr, "pnn_nprotos_use"))
      pnn_prs->nprotos_use = atoi(valstr);
    else if(!strcmp(namestr, "pnn_nclasses"))
      pnn_prs->nclasses = atoi(valstr);
    else if(!strcmp(namestr, "pnn_osf"))
      pnn_prs->osf = atof(valstr);

    /* pnn data: prototype feature vectors and their classes: */
    else if(!strcmp(namestr, "pnn_protos_fvs_file"))
      strcpy(pnn_protos_fvs_file, valstr);
    else if(!strcmp(namestr, "pnn_protos_classes_file"))
      strcpy(pnn_protos_classes_file, valstr);

    /* pseudo (pseudoridge tracer) parms: */
    else if(!strcmp(namestr, "pseudo_slthresh0"))
      pseudo_prs->slthresh0 = atof(valstr);
    else if(!strcmp(namestr, "pseudo_slthresh1"))
      pseudo_prs->slthresh1 = atof(valstr);
    else if(!strcmp(namestr, "pseudo_smooth_cwt"))
      pseudo_prs->smooth_cwt = atof(valstr);
    else if(!strcmp(namestr, "pseudo_stepsize"))
      pseudo_prs->stepsize = atof(valstr);
    else if(!strcmp(namestr, "pseudo_max_tilt"))
      pseudo_prs->max_tilt = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_min_side_turn"))
      pseudo_prs->min_side_turn = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_initi_s"))
      pseudo_prs->initi_s = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_initi_e"))
      pseudo_prs->initi_e = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_initj_s"))
      pseudo_prs->initj_s = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_initj_e"))
      pseudo_prs->initj_e = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_maxsteps_eachdir"))
      pseudo_prs->maxsteps_eachdir = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_nsmooth"))
      pseudo_prs->nsmooth = atoi(valstr);
    else if(!strcmp(namestr, "pseudo_maxturn"))
      pseudo_prs->maxturn = atoi(valstr);

    /* combine (pnn/pseudo combining rule) parm: */
    else if(!strcmp(namestr, "combine_clash_confidence"))
      *combine_clash_confidence = atof(valstr);

    /* sleeps values.  Zero means do nothing; positive int means
    sleep that many seconds; -1 means wait for operator to type
    enter key. */
    else if(!strcmp(namestr, "sleeps_titlepage"))
      sleeps->titlepage = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_sgmntwork"))
      sleeps->sgmntwork = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_segras"))
      sleeps->segras = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_enhnc"))
      sleeps->enhnc = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_core_medcore"))
      sleeps->core_medcore = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_regbars"))
      sleeps->regbars = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_featvec"))
      sleeps->featvec = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_normacs"))
      sleeps->normacs = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_foundconup"))
      sleeps->foundconup = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_noconup"))
      sleeps->noconup = atoi(valstr);
    else if(!strcmp(namestr, "sleeps_lastdisp"))
      sleeps->lastdisp = atoi(valstr);

    /* If y (yes), warp the mouse pointer into graphics window so
    its colormap gets used; if n (no), no warp. */
    else if(!strcmp(namestr, "warp_mouse"))
      *warp_mouse = !strcmp(valstr, "y");

    else if(!strcmp(namestr, "demo_images_list"))
      strcpy(demo_images_list, valstr);
    else if(!strcmp(namestr, "outfile"))
      strcpy(outfile, valstr);
    else if(!strcmp(namestr, "clobber_outfile"))
      *clobber_outfile = !strcmp(valstr, "y");
    else if(!strcmp(namestr, "verbose"))
      *verbose = !strcmp(valstr, "y");

    else {
      sprintf(str, "illegal parm name %s", namestr);
      sprintf(str2, "parms file %s", prsfile);
      fatalerr("pcasys_readparms (file init.c)", str, str2);
    }
  }
}

/******************************************************************/
void check_cls_str(char *cls_str, const int ncls)
{
   int i;

   if(strlen(cls_str) != ncls) {
      fprintf(stderr, "cls_str = %s :: ncls = %d\n", cls_str, ncls);
      fatalerr("check_cls_str", "length of cls_str != ncls", NULL);
   }

   for(i = 0; i < ncls; i++) {
      if(cls_str[i] != 'A' && cls_str[i] != 'L' && cls_str[i] != 'R' &&
         cls_str[i] != 'S' && cls_str[i] != 'T' && cls_str[i] != 'W') {
         fprintf(stderr, "cls_str = %s\n", cls_str);
         fatalerr("check_cls_str", "a cls_str value != 'ALRSTW'", "\0");
      }
   }
}
