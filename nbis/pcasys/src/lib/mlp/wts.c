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

      FILE:    WTS.C
      AUTHORS: Charles Wilson
               G. T. Candela
               Michael D. Garris
      DATE:    1992
      UPDATED: 09/10/2004
      UPDATED: 03/22/2005 by MDG

      Routines for setting the MLP weights -- either randomly or by
      reading a file -- and for writing the weights as a file:

      ROUTINES:
#cat: randwts - Gets random MLP network weights.
#cat: randwts_oldorder - Gets random MLP network weights in an old format
#cat:                    order.
#cat: readwts - Reads MLP weights file.  Returns the "network architecture"
#cat:           info stored in the weights file, as well as the weights.
#cat: readwts_np - Reads MLP weights file.  Returns the "network architecture"
#cat:              info stored in the weights file, as well as the weights,
#cat:              but NOT using structures in parms.h.
#cat: readwts_np2 - Reads MLP weights file.  Returns the "network
#cat:               architecture" info stored in the weights file,
#cat:               as well as the weights, but NOT using structures in
#cat:               parms.h.  This routine does not exit on error, but
#cat:               posts error msg to stderr and returns an error code.
#cat: putwts - Writes the MLP network weights as an ascii file.

***********************************************************************/

#include <mlp.h>

#define SCALE 0.5 /* Random weights will be uniformly distributed in
                  the range -SCALE through SCALE. */
#define WTS_BUFLEN  100

/********************************************************************/

/* randwts: Gets random network weights.

Input args:
  ninps, nhids, nouts: Numbers of input, hidden, and output nodes.
  seed: For the "uni" uniform pseudorandom number generator.  Must
    be a nonzero integer.

Output arg:
  w: The weights, in a buffer allocated by this routine.
*/

void randwts(int ninps, int nhids, int nouts, int seed, float **w)
{
  int numwts;
  float *p, *ep;

  numwts = nhids * (ninps + 1) + nouts * (nhids + 1);
  if((*w = (float *)malloc(numwts * sizeof(float))) == (float *)NULL)
    syserr("randwts (wts.c)", "malloc", "*w");
  uni(seed);
  ep = (p = *w) + numwts;
  while(p < ep)
    *p++ = 2. * SCALE * (uni(0) - 0.5);
}

/********************************************************************/

/* randwts_oldorder: The difference between this and randwts, is that
this version installs the weights in the order corresponding to the
order the old random weights installer (in the Fortran version)
used.

Input args:
  ninps, nhids, nouts: Numbers of input, hidden, and output nodes.
  seed: For the "uni" uniform pseudorandom number generator.  Must
    be a nonzero integer.

Output arg:
  w: The weights, in a buffer allocated by this routine.
*/

void randwts_oldorder(int ninps, int nhids, int nouts, int seed, float **w)
{
  int numwts, h, i, j;
  float *w1, *b1, *w2, *b2;

  numwts = nhids * (ninps + 1) + nouts * (nhids + 1);
  if((*w = (float *)malloc(numwts * sizeof(float))) == (float *)NULL)
    syserr("randwts_oldorder (wts.c)", "malloc", "*w");
  b2 = (w2 = (b1 = (w1 = *w) + nhids * ninps) + nhids) + nouts * nhids;
  uni(seed);
  for(h = 0; h < nhids; h++) {
    for(i = 0; i < ninps; i++)
      *(w1 + h * ninps + i) = 2. * SCALE * (uni(0) - 0.5);
    *(b1 + h) = 2. * SCALE * (uni(0) - 0.5);
  }
  for(j = 0; j < nouts; j++) {
    for(h = 0; h < nhids; h++)
      *(w2 + j * nhids + h) = 2. * SCALE * (uni(0) - 0.5);
    *(b2 + j) = 2. * SCALE * (uni(0) - 0.5);
  }
}

/********************************************************************/

/* readwts: Reads a weights file.  Returns the "network architecture"
info stored in the weights file, as well as the weights.

Input/output arg:
  parms: The PARMS structure, with its members set according to the
    specfile.  This routine finds wts_infile, the file from which to
    read the weights, in the parms structure, and it sets into the
    parms members the values of purpose, ninps, nhids, nouts,
    acfunc_hids, and acfunc_outs, which are specified at the top of
    wts_infile.  When its sets these values in, it also turns on the
    corresponding "set" members.

Output arg:
  w: The weights, in a buffer allocated by this routine.
*/

void readwts(PARMS *parms, float **w)
{
  FILE *fp;
  char line[100], name_str[100], val_str[100], errstr[200];
  int numwts;
  float *p, *pe;

  if((fp = fopen(parms->wts_infile.val, "rb")) == (FILE *)NULL)
    syserr("readwts (wts.c)", "fopen for reading",
      parms->wts_infile.val);

  /* Read the header info, which is in the form of name-value pairs
  in a defined order. */

  /* network_type: must be mlp.  (Other routines will read weights
  files for other types of network, e.g. rbf1.) */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", name_str, val_str) != 2 ||
    strcmp(name_str, "network_type"))
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  if(strcmp(val_str, "mlp")) {
    sprintf(errstr, "network_type must be mlp; it is %s", val_str);
    fatalerr("readwts (wts.c)", errstr, parms->wts_infile.val);
  }

  /* purpose: classifier or fitter. */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", name_str, val_str) != 2 ||
    strcmp(name_str, "purpose"))
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  if(!strcmp(val_str, "classifier"))
    parms->purpose.val = CLASSIFIER;
  else if(!strcmp(val_str, "fitter"))
    parms->purpose.val = FITTER;
  else
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  parms->purpose.ssl.set = TRUE;

  /* If wts_infile sets purpose to fitter, then check whether the
  parms sitaution in specfile is inconsistent with fitter, and if so,
  error exit. */
  if(parms->purpose.val == FITTER) {
    if(parms->class_wts_infile.ssl.set) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut class_wts_infile is set in specfile; class_wts_infile \
is used only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->lcn_scn_infile.ssl.set) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut lcn_scn_infile is set in specfile; lcn_scn_infile \
is used only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->nokdel.ssl.set) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut nokdel is set in specfile; nokdel is used only for \
classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->trgoff.ssl.set) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut trgoff is set in specfile; trgoff is used only for \
classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->scg_earlystop_pct.ssl.set) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut scg_earlystop_pct is set in specfile; \
scg_earlystop_pct is used only for classifier.",
        parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->alpha.ssl.set) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut alpha is set in specfile; alpha is used only for \
classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->oklvl.ssl.set) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut oklvl is set in specfile; oklvl is used only for \
classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->priors.val == CLASS) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut priors is set to class in specfile; that makes sense \
only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->priors.val == BOTH) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut priors is set to both in specfile; that makes sense \
only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->errfunc.val == TYPE_1) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut errfunc is set to type_1 in specfile; that makes sense \
only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->errfunc.val == POS_SUM) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut errfunc is set to pos_sum in specfile; that makes sense \
only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->do_confuse.val == TRUE) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut do_confuse is set to true in specfile; that makes sense \
only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
    if(parms->do_cvr.val == TRUE) {
      sprintf(errstr, "purpose, as read from wts_infile %s, is \
fitter,\nbut do_cvr is set to true in specfile; that makes sense \
only for classifier.", parms->wts_infile.val);
      fatalerr("readwts (wts.c)", errstr, NULL);
    }
  }

  /* ninps: number of input nodes */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %d", name_str, &(parms->ninps.val)) != 2 ||
    strcmp(name_str, "ninps"))
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  parms->ninps.ssl.set = TRUE;

  /* nhids: number of hidden nodes */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %d", name_str, &(parms->nhids.val)) != 2 ||
    strcmp(name_str, "nhids"))
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  parms->nhids.ssl.set = TRUE;

  /* nouts: number of output nodes */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %d", name_str, &(parms->nouts.val)) != 2 ||
    strcmp(name_str, "nouts"))
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  parms->nouts.ssl.set = TRUE;

  /* acfunc_hids: type of activation function on hidden nodes */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", name_str, val_str) != 2 ||
    strcmp(name_str, "acfunc_hids"))
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  if((parms->acfunc_hids.val = acsmaps_str_to_code(val_str)) ==
    BAD_AC_CODE)
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  parms->acfunc_hids.ssl.set = TRUE;

  /* acfunc_outs: type of activation function on output nodes */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", name_str, val_str) != 2 ||
    strcmp(name_str, "acfunc_outs"))
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  if((parms->acfunc_outs.val = acsmaps_str_to_code(val_str)) ==
    BAD_AC_CODE)
    fatalerr("readwts (wts.c)", "improper weights file",
      parms->wts_infile.val);
  parms->acfunc_outs.ssl.set = TRUE;

  /* Allocate buffer for weights, and read them. */
  numwts = parms->nhids.val * (parms->ninps.val + 1) +
    parms->nouts.val * (parms->nhids.val + 1);
  if((*w = (float *)malloc(numwts * sizeof(float))) == (float *)NULL)
    syserr("readwts (wts.c)", "malloc", "*w");
  for(pe = (p = *w) + numwts; p < pe; p++)
    if(fscanf(fp, "%f", p) != 1)
      fatalerr("readwts (wts.c)", "not enough weights in file",
        parms->wts_infile.val);
  fclose(fp);
}

/*******************************************************************/

/* readwts_np: Reads a file containing MLP weights.  Returns the "network
architecture" info stored in the weights file, as well as the weights, but
NOT using structures in parms.h.

Input arg:
  wts_infile: MLP weights file to read.

Output args:
  purpose: Code character for purpose of the mlp: will be CLASSIFIER
    or FITTER (defined in parms.h).
  ninps, nhids, nouts: Numbers of input, hidden, and output nodes.
  acfunc_hids, acfunc_outs: Code characters of the types of activation
    function to be used on the hidden and output nodes respectively.
    Each of these will be LINEAR, SIGMOID, or SINUSOID (defined in
    parms.h).
  w: The mlp weights, in a buffer allocated by this routine.
*/

void readwts_np(char wts_infile[], char *purpose, int *ninps, int *nhids,
                int *nouts, char *acfunc_hids, char *acfunc_outs, float **w)
{
  FILE *fp;
  char line[100], namestr[100], valstr[100], errstr[100];
  int numwts;
  float *p, *pe;

  if((fp = fopen(wts_infile, "rb")) == (FILE *)NULL)
    syserr("readwts_np", "fopen for reading", wts_infile);

  /* Read the header lines, which are name-value pairs: */

  /* network_type: must be mlp.  (Other routines will read weights
  files for other types of network, e.g. rbf1.) */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", namestr, valstr) != 2 ||
    strcmp(namestr, "network_type"))
    fatalerr("readwts_np (wts.c)", "improper weights file", wts_infile);
  if(strcmp(valstr, "mlp")) {
    sprintf(errstr, "network_type must be mlp; it is %s", valstr);
    fatalerr("readwts_np (wts.c)", errstr, wts_infile);
  }

  /* purpose */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", namestr, valstr) != 2 ||
    strcmp(namestr, "purpose"))
    fatalerr("readwts_np", "improper weights file", wts_infile);
  if(!strcmp(valstr, "classifier"))
    *purpose = CLASSIFIER;
  else if(!strcmp(valstr, "fitter"))
    *purpose = FITTER;
  else
    fatalerr("readwts_np", "improper weights file", wts_infile);

  /* ninps */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %d", namestr, ninps) != 2 ||
    strcmp(namestr, "ninps"))
    fatalerr("readwts_np", "improper weights file", wts_infile);

  /* nhids */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %d", namestr, nhids) != 2 ||
    strcmp(namestr, "nhids"))
    fatalerr("readwts_np", "improper weights file", wts_infile);

  /* nouts */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %d", namestr, nouts) != 2 ||
    strcmp(namestr, "nouts"))
    fatalerr("readwts_np", "improper weights file", wts_infile);

  /* acfunc_hids */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", namestr, valstr) != 2 ||
    strcmp(namestr, "acfunc_hids"))
    fatalerr("readwts_np", "improper weights file", wts_infile);
  if(!strcmp(valstr, "linear"))
    *acfunc_hids = LINEAR;
  else if(!strcmp(valstr, "sigmoid"))
    *acfunc_hids = SIGMOID;
  else if(!strcmp(valstr, "sinusoid"))
    *acfunc_hids = SINUSOID;
  else
    fatalerr("readwts_np", "improper weights file", wts_infile);

  /* acfunc_outs */
  if(!fgets(line, 100, fp) ||
    sscanf(line, "%s %s", namestr, valstr) != 2 ||
    strcmp(namestr, "acfunc_outs"))
    fatalerr("readwts_np", "improper weights file", wts_infile);
  if(!strcmp(valstr, "linear"))
    *acfunc_outs = LINEAR;
  else if(!strcmp(valstr, "sigmoid"))
    *acfunc_outs = SIGMOID;
  else if(!strcmp(valstr, "sinusoid"))
    *acfunc_outs = SINUSOID;
  else
    fatalerr("readwts_np", "improper weights file", wts_infile);

  /* Allocate buffer for weights, and read them. */
  numwts = *nhids * (*ninps + 1) + *nouts * (*nhids + 1);
  if((*w = (float *)malloc(numwts * sizeof(float))) == (float *)NULL)
    syserr("readwts_np", "malloc", "*w");
  for(pe = (p = *w) + numwts; p < pe; p++)
    if(fscanf(fp, "%f", p) != 1)
      fatalerr("readwts_np", "improper weights file", wts_infile);
  fclose(fp);
}

/*******************************************************************/

/* readwts_np2: Reads a file containing MLP weights.  Returns the "network
architecture" info stored in the weights file, as well as the weights, but
NOT using structures in parms.h.  This routine does not exit on error,
but posts error msg to stderr and returns an error code.

Input arg:
  wts_infile:
     MLP weights file to read.

Output args:
  purpose:
     Code character for purpose of the mlp: will be CLASSIFIER
     or FITTER (defined in parms.h).
  ninps, nhids, nouts:
     Numbers of input, hidden, and output nodes.
  acfunc_hids, acfunc_outs:
     Code characters of the types of activation
     function to be used on the hidden and output nodes respectively.
     Each of these will be LINEAR, SIGMOID, or SINUSOID (defined in
     parms.h).
  w:
     The mlp weights, in a buffer allocated by this routine.
*/

int readwts_np2(char *wts_infile, char *purpose,
                int *ninps, int *nhids, int *nouts,
                char *acfunc_hids, char *acfunc_outs, float **ow)
{
   FILE *fp;
   char line[WTS_BUFLEN];
   char namestr[WTS_BUFLEN], valstr[WTS_BUFLEN];
   int numwts;
   float *w, *p, *pe;

   if((fp = fopen(wts_infile, "rb")) == (FILE *)NULL){
      fprintf(stderr, "ERROR : readwts_np2 : fopen : %s\n", wts_infile);
      return(-2);
   }

   /* Read the header lines, which are name-value pairs: */

   /* network_type: must be mlp.  (Other routines will read weights
   files for other types of network, e.g. rbf1.) */
   if(!fgets(line, WTS_BUFLEN, fp) ||
      sscanf(line, "%s %s", namestr, valstr) != 2 ||
      strcmp(namestr, "network_type")){
      fprintf(stderr, "ERROR : readwts_np2 : network_type : not found\n");
      fclose(fp);
      return(-3);
   }
   if(strcmp(valstr, "mlp")) {
      fprintf(stderr, "ERROR : readwts_np2 : network_type : %s != \"mlp\"\n",
              valstr);
      fclose(fp);
      return(-4);
   }

   /* purpose */
   if(!fgets(line, WTS_BUFLEN, fp) ||
      sscanf(line, "%s %s", namestr, valstr) != 2 ||
      strcmp(namestr, "purpose")){
      fprintf(stderr, "ERROR : readwts_np2 : purpose : not found\n");
      fclose(fp);
      return(-5);
   }
   if(!strcmp(valstr, "classifier"))
      *purpose = CLASSIFIER;
   else if(!strcmp(valstr, "fitter"))
      *purpose = FITTER;
   else{
      fprintf(stderr, "ERROR : readwts_np2 : unknown purpose : %s\n",
              valstr);
      fclose(fp);
      return(-6);
   }

   /* ninps */
   if(!fgets(line, WTS_BUFLEN, fp) ||
      sscanf(line, "%s %d", namestr, ninps) != 2 ||
      strcmp(namestr, "ninps")){
      fprintf(stderr, "ERROR : readwts_np2 : ninps : not found\n");
      fclose(fp);
      return(-7);
   }

   /* nhids */
   if(!fgets(line, WTS_BUFLEN, fp) ||
      sscanf(line, "%s %d", namestr, nhids) != 2 ||
      strcmp(namestr, "nhids")){
      fprintf(stderr, "ERROR : readwts_np2 : nhids : not found\n");
      fclose(fp);
      return(-8);
   }

   /* nouts */
   if(!fgets(line, WTS_BUFLEN, fp) ||
      sscanf(line, "%s %d", namestr, nouts) != 2 ||
      strcmp(namestr, "nouts")){
      fprintf(stderr, "ERROR : readwts_np2 : nouts : not found\n");
      fclose(fp);
      return(-8);
   }

   /* acfunc_hids */
   if(!fgets(line, WTS_BUFLEN, fp) ||
      sscanf(line, "%s %s", namestr, valstr) != 2 ||
      strcmp(namestr, "acfunc_hids")){
      fprintf(stderr, "ERROR : readwts_np2 : acfunc_hids : not found\n");
      fclose(fp);
      return(-9);
   }
   if(!strcmp(valstr, "linear"))
      *acfunc_hids = LINEAR;
   else if(!strcmp(valstr, "sigmoid"))
      *acfunc_hids = SIGMOID;
   else if(!strcmp(valstr, "sinusoid"))
      *acfunc_hids = SINUSOID;
   else{
      fprintf(stderr, "ERROR : readwts_np2 : unknown acfunc_hids : %s\n",
              valstr);
      fclose(fp);
      return(-10);
   }

   /* acfunc_outs */
   if(!fgets(line, WTS_BUFLEN, fp) ||
      sscanf(line, "%s %s", namestr, valstr) != 2 ||
      strcmp(namestr, "acfunc_outs")){
      fprintf(stderr, "ERROR : readwts_np2 : acfunc_outs : not found\n");
      fclose(fp);
      return(-11);
   }
   if(!strcmp(valstr, "linear"))
      *acfunc_outs = LINEAR;
   else if(!strcmp(valstr, "sigmoid"))
      *acfunc_outs = SIGMOID;
   else if(!strcmp(valstr, "sinusoid"))
      *acfunc_outs = SINUSOID;
   else{
      fprintf(stderr, "ERROR : readwts_np2 : unknown acfunc_outs : %s\n",
              valstr);
      fclose(fp);
      return(-12);
   }

   /* Allocate buffer for weights, and read them. */
   numwts = *nhids * (*ninps + 1) + *nouts * (*nhids + 1);
   if((w = (float *)malloc(numwts * sizeof(float))) == (float *)NULL){
      fprintf(stderr, "ERROR : readwts_np2 : malloc : w\n");
      fclose(fp);
      return(-13);
   }

   for(pe = (p = w) + numwts; p < pe; p++){
      if(fscanf(fp, "%f", p) != 1){
         fprintf(stderr, "ERROR : readwts_np2 : fscanf : wts\n");
         free(w);
         fclose(fp);
         return(-14);
      }
   }

   fclose(fp);

   *ow = w;
   return(0);
}

/********************************************************************/

/* putwts: Writes the network weights as an ascii file.  (Different
format than old weight files.  Contains more header info, which is now
in the form of name-value pairs.)

Input parms:
  wts_outfile: File to produce containing the weights.
  w: The weights.
  purpose: CLASSIFIER or FITTER (defined in parms.h).
  ninps, nhids, nouts: Numbers of input, hidden, and output nodes.
  acfunc_hids, acfunc_outs: Types of activation function used on the
    hidden and output nodes; legal values are defined in parms.h.
*/

void putwts(char wts_outfile[], float *w, char purpose, int ninps, int nhids,
            int nouts, char acfunc_hids, char acfunc_outs)
{
  FILE *fp;
  int h, i, j;

  if((fp = fopen(wts_outfile, "wb")) == (FILE *)NULL)
    syserr("putwts", "fopen for writing", wts_outfile);

  /* Write header info as name-value pairs. */
  fprintf(fp, "network_type mlp\n");
  fprintf(fp, "purpose %s\n", (purpose == CLASSIFIER ?
    "classifier" : "fitter"));
  fprintf(fp, "ninps %d\n", ninps);
  fprintf(fp, "nhids %d\n", nhids);
  fprintf(fp, "nouts %d\n", nouts);
  fprintf(fp, "acfunc_hids %s\n", acsmaps_code_to_str(acfunc_hids));
  fprintf(fp, "acfunc_outs %s\n", acsmaps_code_to_str(acfunc_outs));
  fprintf(fp, "\n");

  /* Write the weights. */
  for(h = 0; h < nhids; h++) {
    for(i = 0; i < ninps; i++) {
      if(i && !(i % 5))
	fprintf(fp, "\n");
      fprintf(fp, " %13.6e", *w++);
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  for(h = 0; h < nhids; h++) {
    if(h && !(h % 5))
      fprintf(fp, "\n");
    fprintf(fp, " %13.6e", *w++);
  }
  fprintf(fp, "\n\n");
  for(j = 0; j < nouts; j++) {
    for(h = 0; h < nhids; h++) {
      if(h && !(h % 5))
	fprintf(fp, "\n");
      fprintf(fp, " %13.6e", *w++);
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  for(j = 0; j < nouts; j++) {
    if(j && !(j % 5))
      fprintf(fp, "\n");
    fprintf(fp, " %13.6e", *w++);
  }
  fprintf(fp, "\n");

  fclose(fp);
}

/********************************************************************/
