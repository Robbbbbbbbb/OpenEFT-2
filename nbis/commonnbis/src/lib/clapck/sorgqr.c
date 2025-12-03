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


/*
* ======================================================================
* NIST Guide to Available Math Software.
* Fullsource for module SSYEVX.C from package CLAPACK.
* Retrieved from NETLIB on Fri Mar 10 14:23:44 2000.
*
* UPDATED: 03/09/2005 by MDG
* ======================================================================
*/
#include <f2c.h>

/* Subroutine */ int sorgqr_(int *m, int *n, int *k, real *a, 
	int *lda, real *tau, real *work, int *lwork, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SORGQR generates an M-by-N real matrix Q with orthonormal columns,   
    which is defined as the first N columns of a product of K elementary 
  
    reflectors of order M   

          Q  =  H(1) H(2) . . . H(k)   

    as returned by SGEQRF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. M >= N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the 
  
            matrix Q. N >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the i-th column must contain the vector which   
            defines the elementary reflector H(i), for i = 1,2,...,k, as 
  
            returned by SGEQRF in the first k columns of its array   
            argument A.   
            On exit, the M-by-N matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= max(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQRF.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= max(1,N).   
            For optimum performance LWORK >= N*NB, where NB is the   
            optimal blocksize.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument has an illegal value   

    ===================================================================== 
  


       Test the input arguments   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    static int c_n1 = -1;
    static int c__3 = 3;
    static int c__2 = 2;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset;
*/
    int i__1, i__2, i__3;
    /* Local variables */
    static int i, j, l, nbmin, iinfo, ib;
    extern /* Subroutine */ int sorg2r_(int *, int *, int *, real 
	    *, int *, real *, real *, int *);
    static int nb, ki, kk, nx;
    extern /* Subroutine */ int slarfb_(char *, char *, char *, char *, 
	    int *, int *, int *, real *, int *, real *, 
	    int *, real *, int *, real *, int *), xerbla_(char *, int *);
    extern int ilaenv_(int *, char *, char *, int *, int *, 
	    int *, int *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(char *, char *, int *, int *, 
	    real *, int *, real *, real *, int *);
    static int ldwork, iws;



#define TAU(I) tau[(I)-1]
#define WORK(I) work[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    } else if (*lwork < max(1,*n)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGQR", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	WORK(1) = 1.f;
	return 0;
    }

/*     Determine the block size. */

    nb = ilaenv_(&c__1, "SORGQR", " ", m, n, k, &c_n1, 6L, 1L);
    nbmin = 2;
    nx = 0;
    iws = *n;
    if (nb > 1 && nb < *k) {

/*        Determine when to cross over from blocked to unblocked code.
   

   Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "SORGQR", " ", m, n, k, &c_n1, 6L, 1L)
		;
	nx = max(i__1,i__2);
	if (nx < *k) {

/*           Determine if workspace is large enough for blocked co
de. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduc
e NB and   
                determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "SORGQR", " ", m, n, k, &c_n1,
			 6L, 1L);
		nbmin = max(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < *k && nx < *k) {

/*        Use blocked code after the last block.   
          The first kk columns are handled by the block method. */

	ki = (*k - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = *k, i__2 = ki + nb;
	kk = min(i__1,i__2);

/*        Set A(1:kk,kk+1:n) to zero. */

	i__1 = *n;
	for (j = kk + 1; j <= *n; ++j) {
	    i__2 = kk;
	    for (i = 1; i <= kk; ++i) {
		A(i,j) = 0.f;
/* L10: */
	    }
/* L20: */
	}
    } else {
	kk = 0;
    }

/*     Use unblocked code for the last or only block. */

    if (kk < *n) {
	i__1 = *m - kk;
	i__2 = *n - kk;
	i__3 = *k - kk;
	sorg2r_(&i__1, &i__2, &i__3, &A(kk+1,kk+1), lda, &
		TAU(kk + 1), &WORK(1), &iinfo);
    }

    if (kk > 0) {

/*        Use blocked code */

	i__1 = -nb;
	for (i = ki + 1; -nb < 0 ? i >= 1 : i <= 1; i += -nb) {
/* Computing MIN */
	    i__2 = nb, i__3 = *k - i + 1;
	    ib = min(i__2,i__3);
	    if (i + ib <= *n) {

/*              Form the triangular factor of the block reflec
tor   
                H = H(i) H(i+1) . . . H(i+ib-1) */

		i__2 = *m - i + 1;
		slarft_("Forward", "Columnwise", &i__2, &ib, &A(i,i), lda, &TAU(i), &WORK(1), &ldwork);

/*              Apply H to A(i:m,i+ib:n) from the left */

		i__2 = *m - i + 1;
		i__3 = *n - i - ib + 1;
		slarfb_("Left", "No transpose", "Forward", "Columnwise", &
			i__2, &i__3, &ib, &A(i,i), lda, &WORK(1), &
			ldwork, &A(i,i+ib), lda, &WORK(ib + 1),
			 &ldwork);
	    }

/*           Apply H to rows i:m of current block */

	    i__2 = *m - i + 1;
	    sorg2r_(&i__2, &ib, &ib, &A(i,i), lda, &TAU(i), &WORK(
		    1), &iinfo);

/*           Set rows 1:i-1 of current block to zero */

	    i__2 = i + ib - 1;
	    for (j = i; j <= i+ib-1; ++j) {
		i__3 = i - 1;
		for (l = 1; l <= i-1; ++l) {
		    A(l,j) = 0.f;
/* L30: */
		}
/* L40: */
	    }
/* L50: */
	}
    }

    WORK(1) = (real) iws;
    return 0;

/*     End of SORGQR */

} /* sorgqr_ */
