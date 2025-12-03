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

/* Subroutine */ int sorgtr_(char *uplo, int *n, real *a, int *lda, 
	real *tau, real *work, int *lwork, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SORGTR generates a real orthogonal matrix Q which is defined as the   
    product of n-1 elementary reflectors of order N, as returned by   
    SSYTRD:   

    if UPLO = 'U', Q = H(n-1) . . . H(2) H(1),   

    if UPLO = 'L', Q = H(1) H(2) . . . H(n-1).   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            = 'U': Upper triangle of A contains elementary reflectors   
                   from SSYTRD;   
            = 'L': Lower triangle of A contains elementary reflectors   
                   from SSYTRD.   

    N       (input) INTEGER   
            The order of the matrix Q. N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the vectors which define the elementary reflectors, 
  
            as returned by SSYTRD.   
            On exit, the N-by-N orthogonal matrix Q.   

    LDA     (input) INTEGER   
            The leading dimension of the array A. LDA >= max(1,N).   

    TAU     (input) REAL array, dimension (N-1)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SSYTRD.   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= max(1,N-1).   
            For optimum performance LWORK >= (N-1)*NB, where NB is   
            the optimal blocksize.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    ===================================================================== 
  


       Test the input arguments   

    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset;
*/
    int i__1, i__2, i__3;
    /* Local variables */
    static int i, j;
    extern logical lsame_(char *, char *);
    static int iinfo;
    static logical upper;
    extern /* Subroutine */ int xerbla_(char *, int *), sorgql_(
	    int *, int *, int *, real *, int *, real *, real *
	    , int *, int *), sorgqr_(int *, int *, int *, 
	    real *, int *, real *, real *, int *, int *);


#define TAU(I) tau[(I)-1]
#define WORK(I) work[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = 1, i__2 = *n - 1;
	if (*lwork < max(i__1,i__2)) {
	    *info = -7;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORGTR", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	WORK(1) = 1.f;
	return 0;
    }

    if (upper) {

/*        Q was determined by a call to SSYTRD with UPLO = 'U'   

          Shift the vectors which define the elementary reflectors one
   
          column to the left, and set the last row and column of Q to 
  
          those of the unit matrix */

	i__1 = *n - 1;
	for (j = 1; j <= *n-1; ++j) {
	    i__2 = j - 1;
	    for (i = 1; i <= j-1; ++i) {
		A(i,j) = A(i,j+1);
/* L10: */
	    }
	    A(*n,j) = 0.f;
/* L20: */
	}
	i__1 = *n - 1;
	for (i = 1; i <= *n-1; ++i) {
	    A(i,*n) = 0.f;
/* L30: */
	}
	A(*n,*n) = 1.f;

/*        Generate Q(1:n-1,1:n-1) */

	i__1 = *n - 1;
	i__2 = *n - 1;
	i__3 = *n - 1;
	sorgql_(&i__1, &i__2, &i__3, &A(1,1), lda, &TAU(1), &WORK(1), 
		lwork, &iinfo);

    } else {

/*        Q was determined by a call to SSYTRD with UPLO = 'L'.   

          Shift the vectors which define the elementary reflectors one
   
          column to the right, and set the first row and column of Q t
o   
          those of the unit matrix */

	for (j = *n; j >= 2; --j) {
	    A(1,j) = 0.f;
	    i__1 = *n;
	    for (i = j + 1; i <= *n; ++i) {
		A(i,j) = A(i,j-1);
/* L40: */
	    }
/* L50: */
	}
	A(1,1) = 1.f;
	i__1 = *n;
	for (i = 2; i <= *n; ++i) {
	    A(i,1) = 0.f;
/* L60: */
	}
	if (*n > 1) {

/*           Generate Q(2:n,2:n) */

	    i__1 = *n - 1;
	    i__2 = *n - 1;
	    i__3 = *n - 1;
	    sorgqr_(&i__1, &i__2, &i__3, &A(2,2), lda, &TAU(1), 
		    &WORK(1), lwork, &iinfo);
	}
    }
    return 0;

/*     End of SORGTR */

} /* sorgtr_ */
