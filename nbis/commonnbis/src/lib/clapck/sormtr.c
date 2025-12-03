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

/* Subroutine */ int sormtr_(char *side, char *uplo, char *trans, int *m, 
	int *n, real *a, int *lda, real *tau, real *c, int *ldc, 
	real *work, int *lwork, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SORMTR overwrites the general real M-by-N matrix C with   

                    SIDE = 'L'     SIDE = 'R'   
    TRANS = 'N':      Q * C          C * Q   
    TRANS = 'T':      Q**T * C       C * Q**T   

    where Q is a real orthogonal matrix of order nq, with nq = m if   
    SIDE = 'L' and nq = n if SIDE = 'R'. Q is defined as the product of   
    nq-1 elementary reflectors, as returned by SSYTRD:   

    if UPLO = 'U', Q = H(nq-1) . . . H(2) H(1);   

    if UPLO = 'L', Q = H(1) H(2) . . . H(nq-1).   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q or Q**T from the Left;   
            = 'R': apply Q or Q**T from the Right.   

    UPLO    (input) CHARACTER*1   
            = 'U': Upper triangle of A contains elementary reflectors   
                   from SSYTRD;   
            = 'L': Lower triangle of A contains elementary reflectors   
                   from SSYTRD.   

    TRANS   (input) CHARACTER*1   
            = 'N':  No transpose, apply Q;   
            = 'T':  Transpose, apply Q**T.   

    M       (input) INTEGER   
            The number of rows of the matrix C. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix C. N >= 0.   

    A       (input) REAL array, dimension   
                                 (LDA,M) if SIDE = 'L'   
                                 (LDA,N) if SIDE = 'R'   
            The vectors which define the elementary reflectors, as   
            returned by SSYTRD.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.   
            LDA >= max(1,M) if SIDE = 'L'; LDA >= max(1,N) if SIDE = 'R'. 
  

    TAU     (input) REAL array, dimension   
                                 (M-1) if SIDE = 'L'   
                                 (N-1) if SIDE = 'R'   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SSYTRD.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the M-by-N matrix C.   
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q. 
  

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.   
            If SIDE = 'L', LWORK >= max(1,N);   
            if SIDE = 'R', LWORK >= max(1,M).   
            For optimum performance LWORK >= N*NB if SIDE = 'L', and   
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal   
            blocksize.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

    ===================================================================== 
  


       Test the input arguments   

    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset, c_dim1, c_offset;
*/
    int i__1;
    /* Local variables */
    static logical left;
    extern logical lsame_(char *, char *);
    static int iinfo, i1;
    static logical upper;
    static int i2, mi, ni, nq, nw;
    extern /* Subroutine */ int xerbla_(char *, int *), sormql_(
	    char *, char *, int *, int *, int *, real *, int *
	    , real *, real *, int *, real *, int *, int *), sormqr_(char *, char *, int *, int *, int *,
	     real *, int *, real *, real *, int *, real *, int *, 
	    int *);


#define TAU(I) tau[(I)-1]
#define WORK(I) work[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]
#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]

    *info = 0;
    left = lsame_(side, "L");
    upper = lsame_(uplo, "U");

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if (! left && ! lsame_(side, "R")) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (! lsame_(trans, "N") && ! lsame_(trans, "T")) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < max(1,nq)) {
	*info = -7;
    } else if (*ldc < max(1,*m)) {
	*info = -10;
    } else if (*lwork < max(1,nw)) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORMTR", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || nq == 1) {
	WORK(1) = 1.f;
	return 0;
    }

    if (left) {
	mi = *m - 1;
	ni = *n;
    } else {
	mi = *m;
	ni = *n - 1;
    }

    if (upper) {

/*        Q was determined by a call to SSYTRD with UPLO = 'U' */

	i__1 = nq - 1;
	sormql_(side, trans, &mi, &ni, &i__1, &A(1,2), lda, &
		TAU(1), &C(1,1), ldc, &WORK(1), lwork, &iinfo);
    } else {

/*        Q was determined by a call to SSYTRD with UPLO = 'L' */

	if (left) {
	    i1 = 2;
	    i2 = 1;
	} else {
	    i1 = 1;
	    i2 = 2;
	}
	i__1 = nq - 1;
	sormqr_(side, trans, &mi, &ni, &i__1, &A(2,1), lda, &TAU(1), &
		C(i1,i2), ldc, &WORK(1), lwork, &iinfo);
    }
    return 0;

/*     End of SORMTR */

} /* sormtr_ */
