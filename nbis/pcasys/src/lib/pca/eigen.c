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

      FILE:    EIGEN.C
      AUTHORS: Craig Watson
               Patrick Grother
      DATE:    10/01/2000
      UPDATED: 04/19/2005 by MDG
      
      Finds eigenvalues and eigenvectors of a symmetric real matrix.  (A
      wrapper for SSYEVX, a Fortran routine in LAPACK.) Then removes
      any that fail to converge.
 
      ROUTINES:
#cat: eigen - Finds eigenvalues and eigenvectors of a symmetric
#cat:         real matrix and removes any that fail to converge.
#cat: diag_mat_eigen - Finds eigenvalues and eigenvectors of a symmetric
#cat:                  real matrix.

***********************************************************************/

#include <pca.h>
#include <defs.h>

/* External references to routines in src/lib/clapck */
extern double slamch_(char *);
extern int ilaenv_(int *, char *, char *, int *, int *, int *, int *,
                 const long, const long);
extern void ssyevx_(char *, char *, char *, const int *, float *, int *,
                 float *, float *, int *, int *, float *, int *, float *,
                 float *, const int *, float *, int *, int *, int *, int *);

/* External references to routines in src/lib/cblas */
extern int sswap_(const int *, float *, int *, float *, int *);


/***********************************************************************/
void eigen(const int nevtr, int *nevtf, float **evts, float **evas,
          float *cov, const int order)
{
   int i, n;
   float *tevas;
   float *tevts;
   int *ifail;
   int nfail;
   int good, bad;
   float *pevts, *pevas;

   diag_mat_eigen(order, cov, nevtr, &tevas, &tevts, &ifail, &nfail);

   *nevtf = nevtr - nfail;

   *evts = (float *)malloc(*nevtf * order * sizeof(float));
   *evas = (float *)malloc(*nevtf * sizeof(float));

   pevts = *evts;
   pevas = *evas;
   good = 0;
   bad = 0;
   for(i = 0; i < *nevtf; i++) {
      if(nfail && ifail[bad] == i) {
         fprintf(stderr, "failed convergence on evect %d ", i);
         fprintf(stderr, "evect / eval pair deleted\n");
         bad++;
      }
      else {
         for (n = 0; n < order; n++)
            pevts[good * order + n] = tevts[i * order + n];
         pevas[good] = tevas[i];
         good++;
      }
   }

   free(tevas);
   free(tevts);
   free(ifail);
   return;
}

/***********************************************************************
Finds eigenvalues and eigenvectors of a symmetric real matrix.  (A
wrapper for SSYEVX, a Fortran routine in LAPACK.)  This doesn't allow
all the possibilities of SSYEVX, but hides some of the messiness
associated with using SSYEVX directly.)  If n_find > order or if the
args given result in one of the Fortran routines -- ILAENV and SSYEVX
-- getting an illegal argument, then an error message will be written
to stderr and the process will exit; otherwise the routine will return
0 (normal) or a positive integer (some eigenvectors failed to
converge; see below under return value).

Input args:
  order: Order of matrix mat.
  mat: Symmetric matrix of floats.  This is assumed to be a buffer of
    order^2 floats, but only the the nonstrict lower triangle (when
    interpreted as matrix in C or other row-major language; nonstrict
    upper triangle to Fortran) need be filled in.
    CAUTION: On return, the nonstrict lower triangle of mat will have
    been destroyed (by SSYEVX).
  n_find: Number of largest eigenvalues, and their corresponding
    eigenvectors, that are to be found; must be <= order.  If n_find
    exceeds the rank of the matrix, some of the resulting eigenvalues
    and eigenvectors may be meaningless [?].
  verbose: If nonzero, the routine writes a message when it starts,
    and reports the clock time elapsed.

Output args:
  evals: The n_find largest eigenvalues, in decreasing order; this
    routine causes the buffer to be malloced.
  evecs: The corresponding eigenvectors, as the rows of an
    n_find x order "matrix" in row-major order; this routine causes
    the buffer to be malloced.
  didnt_converge_indices: See below under return value positive.

Return value:
  0: Normal; evals and evecs contains the largest n_find eigenvalues
    and the corresponding eigenvectors, and didnt_converge_indices
    will not have been allocated (so no need to free it).
  <a positive integer n>: n of the eigenvectors failed to converge;
    their indices (with 1 subtracted to convert them from 1-based
    Fortran to 0-based C indices) are stored in didnt_converge_indices;
    this routine will have caused the buffer to be malloced; but if
    you don't want to receive this list of indices in this event, just
    use (int **)NULL for the didnt_converge_indices arg.  The
    remaining eigenvalues and eigenvectors will be in evals and evecs.
***********************************************************************/

void diag_mat_eigen(const int order, float *mat, const int nevtf,
          float **evas, float **evts, int **ifail, int *info)
{
   float *tevas;
   float *tevts;
   int nb;
   int fw_len;
   int iw_len;
   float *fw;
   int *iw;
   int il, iu, tnevtf;
   static char I, L, V, S;
   int lda;
   float abstol;
   int *tifail;
   int ie;
   int i, j;
   static int i1;
   float a;
   int *ffail;
   char *routine = "ssytrd";
   char *parms = "VIL";
   int ord;

   if (nevtf > order) {
      fprintf(stderr, "nevtf (%d) > order (%d)\n", nevtf, order);
      exit(-1);
   }

   *info = 0;

   if((*evas = (float *)malloc(nevtf * sizeof(float))) == NULL) {
      fprintf(stderr, "Error allocing evas\n");
      exit(-1);
   }
   if((*evts = (float *)malloc(nevtf*order * sizeof(float))) == NULL) {
      fprintf(stderr, "Error allocing evts\n");
      exit(-1);
   }
   tevts = *evts;
   tevas = *evas;
   if((*ifail = (int *)malloc(order * sizeof(int))) == NULL) {
      fprintf(stderr, "Error allocing ifail\n");
      exit(-1);
   }

   i = 1;
   j = -1;
   ord = order;
   nb = ilaenv_(&i, routine, parms, &ord, &j, &j, &j, 6L, 3L);
   fw_len = max(8, nb + 3) * order;
   iw_len = order * 5;

   if((tifail = (int *)malloc(order*sizeof(int))) == NULL) {
      fprintf(stderr, "Error allocing tfail\n");
      exit(-1);
   }
   if((fw = (float *)malloc(fw_len*sizeof(float))) == NULL) {
      fprintf(stderr, "Error allocing fw\n");
      exit(-1);
   }
   if((iw = (int *)malloc(iw_len*sizeof(int))) == NULL) {
      fprintf(stderr, "Error allocing iw\n");
      exit(-1);
   }

   il  = order - nevtf + 1;
   iu  = order;

   I = 'I';   /* means find evals on range il -> iu */
   L = 'L';   /* means lower triang of "mat" is stored */
   V = 'V';   /* compute evals AND evects. */
   S = 'S';   /* set to a safe minimum */

   tnevtf = 0;
   lda = order;
   abstol = 2.0 * (float)slamch_(&S);
  
   /*
   This LAPACK routine does the real work.
   on the SGI (atef) run:   man ssyevx
   */
   ssyevx_(&V, &I, &L, &order, mat, &lda,
       (float *)NULL,	/* lowerbound on EVA's to be found - not used with 'I' */
       (float *)NULL,	/* upperbound on EVA's to be found - not used with 'I' */
       &il,		/* upper and lower indices - in reverse order */
       &iu,		/* of eigenvalues to be found. */
       &abstol,		/* floating point tolerance set above */
       &tnevtf,		/* number actually found */
       tevas,		/* final evals in ascending order */
       tevts,		/* final evects corresponding to EVA's */
       &order,		/* leading dimension of evecs_yow */
       fw, &fw_len,	/* workspace and it's length */
       iw,		/* integer space */
       tifail,		/* indices of failed-to-converge evects */
       info);		/* error flag */

   free(fw);
   free(iw);

   if (*info < 0 || nevtf != tnevtf) {
      if (*info < 0)
         fprintf(stderr,
            "\nthe %d'th arg of ssyevx had an illegal value", -(*info));

      if (nevtf != tnevtf)
         fprintf(stderr,
              "no. of eigen{value,vector}s requested %d != no. found, %d",
               nevtf, tnevtf);
  
      exit(-1);
   }

   /*
   Rearrange eigenvalues and eigenvectors from their current
   order -- increasing eigenvalue -- to decreasing eigenvalue,
   which seems more convenient.
   */
   ie = nevtf / 2;
   j = nevtf - 1;
   i1 = 1;
   for (i = 0; i < ie; i++, j--) {
      a = tevas[i];
      tevas[i] = tevas[j];
      tevas[j] = a;
      sswap_(&order, tevts + i * order, &i1,
                     tevts + j * order, &i1);
   }

   if (*info) {  /* info > 0: then some (actually info) eigenvectors
   		   failed to converge, and their indices are in indexfail. */
      if (ifail) {	/* pointer acting as flag */
         ffail = (int *)malloc(*info*sizeof(int));

         /*
         This handles for the reversal of the order of the
         eigen{values,vectors}, and it converts from Fortran
         1-based indices to C 0-based indices.
         */

         for (i = 0; i < *info ; i++)
            ffail[i] = *info - tifail[i];
         *ifail = ffail;
      }
   }
   else {
      *ifail = (int *)malloc(sizeof(int));
      *ifail[0] = -1;
   }

   free(tifail);

   return;
}
