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

      FILE:    GETPATO.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG
      UPDATE:  12/02/2008 by Kenneth Ko - Fix to support 64-bit

      Pattern input routines.

      ROUTINES:
#cat: getpat - Reads an MLP formatted patterns file.
#cat: got_mmm - Tries to read just mpats, minps and mouts from a MLP
#cat:           formatted patterns file.

***********************************************************************/

#include <mlp.h>

/********************************************************************/

/* getpat: Reads a patterns file.  Stores the feature vectors (only
the first ninps elts of each vector, as specified).  If the targets
are represented in the file as classes, i.e. indices (pindex TRUE),
then the indices are read into idpat.  If the targets are represented
in the file as target vectors, then these are either read into the
target buffer (if purpose is FITTER, i.e. function-approximator), or
they are converted into class indices which are then stored (if
purpose is CLASSIFIER; this saves memory).  This routine also returns
the long (full) names of the classes, and the full number of patterns
in the patterns file.

Input args:
  patterns_infile: File containing the patterns, i.e. feature-vector/
    class or feature-vector/target-vector pairs.
  patsfile_ascii_or_binary: ASCII (BINARY) if patterns_infile is in
    ascii (fortran-style binary) format.
  npats: Number of (first) patterns that should be read.  (Must be <=
    the full number of patterns stored in the patterns file.)
  ninps: Number of (first) elements to read of each feature vector.
    (Must be <= the full number of elts in each feature vector of the
    patterns file.)
  nouts: Number of outputs.  (Must equal the number of outputs as
    specified in the patterns file.)
  purpose: CLASSIFIER or FITTER.  Indicates whether the net is to be
    trained to classify, or to do function-fitting (approximation).
  trgoff: This controls how extremely the target values vary.  If
    purpose is CLASSIFIER but the patterns file contains target
    vectors rather than classes, this routine needs to know trgoff so
    it can compute the "high" value presumed to occur at the target
    vector position corresponding to the class.

Output args:
  long_classnames: Long names of the classes.  This is an array of
    nouts strings, with the outer array (of pointers), and the
    strings, allocated by this routine.  (Define a
    char-pointer-pointer and use an &.)
  vinp: Input activations, i.e. the feature vectors of the patterns,
    in an npats by ninps "matrix" allocated by this routine.
  target: If purpose is FITTER, then this will contain target vectors,
    in an npats by nouts "matrix" allocated by this routine; otherwise
    (CLASSIFIER), this will come back just (float *)NULL.
  idpat: If purpose is CLASSIFIER, then this will contain the actual
    classes of the patterns, in a buffer of npats shorts allocated by
    this routine; otherwise (FITTER), this will come back just
    (short *)NULL.
  mpats: Full number of patterns in the patterns file.
*/

void getpat(char patterns_infile[], char patsfile_ascii_or_binary,
            int npats, int ninps, int nouts,
            char purpose, float trgoff, char ***long_classnames,
            float **vinp, float **target, short **idpat, int *mpats)
{
  FILE *fp;
  char str[200], *cp, *cp_e, **a_name;
  static char first = TRUE;
  short *idpat_p;
  int nbytes, nbytes_expected, minps, mouts, idum1, idum2, idum3,
    j, p, pindex, tmpint;
  static int minps_maxsofar, nouts_maxsofar;
  float tmean, highval, *target_p, *vinp_p, *afp;
  static float *vtmp, *targvec;

  idpat_p = (short *)NULL;
  target_p = (float *)NULL;

  if((fp = fopen(patterns_infile, "rb")) == (FILE *)NULL)
    syserr("getpat", "fopen for reading", patterns_infile);
/*
  fprintf(stderr, " Reading patterns...\n");
*/

  /* Read header info. */
  if(patsfile_ascii_or_binary == ASCII)
    fscanf(fp, "%d %d %d %d %d %d", mpats, &minps, &mouts, &idum1,
      &idum2, &idum3);
  else { /* patsfile_ascii_or_binary == BINARY */
    nbytes_expected = 6 * sizeof(int);
    fread(&nbytes, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(nbytes);
#endif
    if(nbytes != nbytes_expected) {
      sprintf(str, "nbytes is %d, but nbytes_expected is %d",
        nbytes, nbytes_expected);
      fatalerr("getpat", str, NULL);
    }
    fread(mpats, sizeof(int), 1, fp);
    fread(&minps, sizeof(int), 1, fp);
    fread(&mouts, sizeof(int), 1, fp);
    fread(&idum1, sizeof(int), 1, fp);
    fread(&idum2, sizeof(int), 1, fp);
    fread(&idum3, sizeof(int), 1, fp);
    fread(&nbytes, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(*mpats);
    swap_int_bytes(minps);
    swap_int_bytes(mouts);
    swap_int_bytes(idum1);
    swap_int_bytes(idum2);
    swap_int_bytes(idum3);
    swap_int_bytes(nbytes);
#endif
    if(nbytes != nbytes_expected) {
      sprintf(str, "nbytes is %d, but nbytes_expected is %d",
        nbytes, nbytes_expected);
      fatalerr("getpat", str, NULL);
    }
  }

  /* Maintain buffers at sufficient sizes. */
  if(first || (minps > minps_maxsofar)) {
    if(!first)
      free((char *)vtmp);
    minps_maxsofar = minps;
    if((vtmp = (float *)malloc(minps * sizeof(float))) ==
      (float *)NULL)
      syserr("getpat", "malloc", "vtmp");
  }
  if(first || (nouts > nouts_maxsofar)) {
    if(!first)
      free((char *)targvec);
    nouts_maxsofar = nouts;
    if((targvec = (float *)malloc(nouts * sizeof(float))) ==
       (float *)NULL)
      syserr("getpat", "malloc", "targvec");
  }
  first = FALSE;

  if(npats > *mpats) {
    sprintf(str, "no. of first patterns to be read (npats, %d) is\n\
> no. of patterns in file (*mpats, %d)\n", npats, *mpats);
    fatalerr("getpat", str, NULL);
  }
  if(ninps > minps) {
    sprintf(str, "no. of first inputs to be read (ninps, %d) is\n\
> no. of inputs in file (minps, %d)\n", ninps, minps);
    fatalerr("getpat", str, NULL);
  }
  if(nouts != mouts) {
    sprintf(str, "no. of outputs to use (nouts, %d) is different \
than\nno. of outputs in file (mouts, %d)\n", nouts, mouts);
    fatalerr("getpat", str, NULL);
  }

  pindex = (idum1 == 1);
  if(pindex && (purpose == FITTER))
    fatalerr("getpat", "Target pointers cannot be used if purpose \
is FITTER:\nThis mode allows for varying target vectors.", NULL);

  /* Correspondence of class to output position.  (Read the long
  (full) names of the classes.) */
  if((*long_classnames = (char **)malloc(nouts * sizeof(char *))) ==
    (char **)NULL)
    syserr("getpat", "malloc", "*long_classnames");
  if(patsfile_ascii_or_binary == ASCII)
    for(j = 0, a_name = *long_classnames; j < nouts; j++, a_name++) {
      if((*a_name = malloc(LONG_CLASSNAME_MAXSTRLEN + 1)) ==
        (char *)NULL)
	syserr("getpat", "malloc", "*a_name");
      fscanf(fp, "%s", *a_name);
    }
  else {
    nbytes_expected = nouts * LONG_CLASSNAME_MAXSTRLEN;
    fread(&nbytes, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(nbytes);
#endif
    if(nbytes != nbytes_expected) {
      sprintf(str, "nbytes is %d, but nbytes_expected is %d",
        nbytes, nbytes_expected);
      fatalerr("getpat", str, NULL);
    }
    for(j = 0, a_name = *long_classnames; j < nouts; j++, a_name++) {
      if((*a_name = malloc(LONG_CLASSNAME_MAXSTRLEN + 1)) ==
        (char *)NULL)
	syserr("getpat", "malloc", "*a_name");
      fread(*a_name, LONG_CLASSNAME_MAXSTRLEN, 1, fp);
      for(cp_e = (cp = *a_name) + LONG_CLASSNAME_MAXSTRLEN; cp < cp_e
        && !strchr(" \t\n", *cp); cp++);
      *cp = (char)0;		/* changed NULL to 0 - jck 2009-02-04 */
    }
    fread(&nbytes, sizeof(int), 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(nbytes);
#endif
    if(nbytes != nbytes_expected) {
      sprintf(str, "nbytes is %d, but nbytes_expected is %d",
        nbytes, nbytes_expected);
      fatalerr("getpat", str, NULL);
    }
  }

  /* If trgoff is 0., then training is to 0. and 1.; if trgoff is 1.,
  then all are trained to 1./nouts; trgoff values between 0. and 1.
  produce effects between these extremes. */
  tmean = 1. / nouts;
  highval = tmean + (1. - tmean) * (1. - trgoff);
  /* lowval is not used in this routine, but leave its definition
  here: */
  /* lowval = tmean + (-tmean) * (1. - trgoff); */

  /* Read the patterns and store appropriate data into memory.  (Only
  load first ninps (can be < minps) elts of each feature vector.)
  First allocate output buffers. */
  if((*vinp = (float *)malloc(npats * ninps * sizeof(float))) ==
    (float *)NULL)
    syserr("getpat", "malloc", "*vinp");
  if(purpose == CLASSIFIER) {
    if((*idpat = (short *)malloc(npats * sizeof(short))) ==
      (short *)NULL)
      syserr("getpat", "malloc", "*idpat");
    idpat_p = *idpat;
    *target = (float *)NULL;
  }
  else { /* purpose == FITTER */
    if((*target = (float *)malloc(npats * nouts * sizeof(float))) ==
      (float *)NULL)
      syserr("getpat", "malloc", "*target");
    target_p = *target;
    *idpat = (short *)NULL;
  }

  for(p = 0, vinp_p = *vinp; p < npats; p++, vinp_p += ninps) {

    /* Read next feature vector.  Read whole vector (minps elts) to get
    through it, but then copy, as current vector, only its first ninp
    elts. */
    rd_words(patsfile_ascii_or_binary, fp, minps, 1, RD_FLOAT, vtmp);
    memcpy(vinp_p, vtmp, ninps * sizeof(float));

    /* Read target index or target vector. */
    if(pindex) { /* File contains target indices (classes); store the
           target index, but using only a short instead of an int. */
      rd_words(patsfile_ascii_or_binary, fp, 1, 1, RD_INT, &tmpint);
      *idpat_p = tmpint; /* [The decrementing is done so that old
        patfiles, in which indexing starts at 1, will still work.
        Eventually, patfiles should be redefined to index starting
        at 0, so decrementing won't be necessary.] */
    }
    else { /* File contains target vectors. */
      if(purpose == FITTER) /* load the target vector */
	rd_words(patsfile_ascii_or_binary, fp, nouts, 1, RD_FLOAT, target_p);
      else { /* purpose == CLASSIFIER; save memory by converting
        target vector to a class and loading that into idpat.  Assume
        the identifier is the only activation near highval.  Assign
        valid id (class) in range 0, 1, ..., nouts-1, or error id
        -1 (no high activation), or error id -2 (more than one high
        activation). */
	rd_words(patsfile_ascii_or_binary, fp, nouts, 1, RD_FLOAT, targvec);
	*idpat_p = -1;
	for(j = 0, afp = targvec; j < nouts; j++, afp++)
	  if(*afp >= highval - 0.001)
	    *idpat_p = ((*idpat_p == -1) ? j : -2);
	if(purpose == CLASSIFIER && *idpat_p < 0) {
	  sprintf(str, "purpose is CLASSIFIER, but pattern p = %d \
has error id %d", p, *idpat_p);
	  fatalerr("getpat", str, NULL);
	}
      }
    }
    if(purpose == CLASSIFIER)
      idpat_p++;
    else /* purpose == FITTER */
      target_p += nouts;

  } /* loop over patterns */

  fclose(fp);
/*
  fprintf(stderr, " ...done\n\n");
*/
}

/********************************************************************/

/* got_mmm: Tries to read just mpats, minps, and mouts (see their
comments below for explanation) from a patterns file.

Input args:
  patterns_infile: Patterns file.
  patsfile_ascii_or_binary: Storage mode of pattern file: ASCII or
    BINARY.

Output args:
  mpats: Number of patterns in pattern file.
  minps: Number of elements in each feature vector of pattern file.
  mouts: Number of elements in each target vector, or number of
    defined classes, of pattern file.
  errstr: If routine successfully reads mpats, etc. (i.e. return
    function value TRUE), then this comes back an empty string;
    otherwise (return function value FALSE) it comes back containing
    an error message suitable for sending to eb_cat_e.  Buffer must
    be provided by caller already allocated to sufficient size; 200
    bytes is very likely to be sufficient.

Return value:
  TRUE (FALSE) if routine was (was not) able to read mpats, etc. from
    patterns file.
*/

char got_mmm(char patterns_infile[], char patsfile_ascii_or_binary,
             int *mpats, int *minps, int *mouts, char errstr[])
{
  FILE *fp;
  char ok;
  int nbytes;

  if((fp = fopen(patterns_infile, "rb")) == (FILE *)NULL) {
    sprintf(errstr, "unable to fopen patterns file %s for reading",
      patterns_infile);
    return FALSE;
  }
  if(patsfile_ascii_or_binary == ASCII)
     ok = (fscanf(fp, "%d %d %d", mpats, minps, mouts) == 3);
  else {
    ok = (fread(&nbytes, sizeof(int), 1, fp) == 1);
#ifdef __NBISLE__
    swap_int_bytes(nbytes);
#endif
    ok = ok && (nbytes == 6 * sizeof(int));
    ok = ok && (fread(mpats, sizeof(int), 1, fp) == 1);
    ok = ok && (fread(minps, sizeof(int), 1, fp) == 1);
    ok = ok && (fread(mouts, sizeof(int), 1, fp) == 1);
#ifdef __NBISLE__
    swap_int_bytes(*mpats);
    swap_int_bytes(*minps);
    swap_int_bytes(*mouts);
#endif
  }
  fclose(fp);
  if(ok)
    strcpy(errstr, "");
  else
    sprintf(errstr, "patterns file %s has improper format",
      patterns_infile);
  return ok;
}

/********************************************************************/
