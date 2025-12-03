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

/* Subroutine */ int sormql_(char *side, char *trans, int *m, int *n, 
	int *k, real *a, int *lda, real *tau, real *c, int *ldc, 
	real *work, int *lwork, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SORMQL overwrites the general real M-by-N matrix C with   

                    SIDE = 'L'     SIDE = 'R'   
    TRANS = 'N':      Q * C          C * Q   
    TRANS = 'T':      Q**T * C       C * Q**T   

    where Q is a real orthogonal matrix defined as the product of k   
    elementary reflectors   

          Q = H(k) . . . H(2) H(1)   

    as returned by SGEQLF. Q is of order M if SIDE = 'L' and of order N   
    if SIDE = 'R'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q or Q**T from the Left;   
            = 'R': apply Q or Q**T from the Right.   

    TRANS   (input) CHARACTER*1   
            = 'N':  No transpose, apply Q;   
            = 'T':  Transpose, apply Q**T.   

    M       (input) INTEGER   
            The number of rows of the matrix C. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix C. N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines   
            the matrix Q.   
            If SIDE = 'L', M >= K >= 0;   
            if SIDE = 'R', N >= K >= 0.   

    A       (input) REAL array, dimension (LDA,K)   
            The i-th column must contain the vector which defines the   
            elementary reflector H(i), for i = 1,2,...,k, as returned by 
  
            SGEQLF in the last k columns of its array argument A.   
            A is modified by the routine but restored on exit.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.   
            If SIDE = 'L', LDA >= max(1,M);   
            if SIDE = 'R', LDA >= max(1,N).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQLF.   

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
    /* Table of constant values */
    static int c__1 = 1;
    static int c_n1 = -1;
    static int c__2 = 2;
    static int c__65 = 65;
    
    /* System generated locals */
    address a__1[2];
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset, c_dim1, c_offset;
*/
    int i__1, i__2, i__3[2], i__4, i__5;
    char ch__1[2];
    /* Builtin functions   
       Subroutine */ int s_cat(char *, char **, int *, int *, ftnlen);
    /* Local variables */
    static logical left;
    static int i;
    static real t[4160]	/* was [65][64] */;
    extern logical lsame_(char *, char *);
    static int nbmin, iinfo, i1, i2, i3, ib;
    extern /* Subroutine */ int sorm2l_(char *, char *, int *, int *, 
	    int *, real *, int *, real *, real *, int *, real *, 
	    int *);
    static int nb, mi, ni, nq, nw;
    extern /* Subroutine */ int slarfb_(char *, char *, char *, char *, 
	    int *, int *, int *, real *, int *, real *, 
	    int *, real *, int *, real *, int *), xerbla_(char *, int *);
    extern int ilaenv_(int *, char *, char *, int *, int *, 
	    int *, int *, ftnlen, ftnlen);
    extern /* Subroutine */ int slarft_(char *, char *, int *, int *, 
	    real *, int *, real *, real *, int *);
    static logical notran;
    static int ldwork, iws;



#define TAU(I) tau[(I)-1]
#define WORK(I) work[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]
#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]

    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");

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
    } else if (! notran && ! lsame_(trans, "T")) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*k < 0 || *k > nq) {
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
	xerbla_("SORMQL", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	WORK(1) = 1.f;
	return 0;
    }

/*     Determine the block size.  NB may be at most NBMAX, where NBMAX   
       is used to define the local array T.   

   Computing MIN   
   Writing concatenation */
    i__3[0] = 1, a__1[0] = side;
    i__3[1] = 1, a__1[1] = trans;
    s_cat(ch__1, a__1, i__3, &c__2, 2L);
    i__1 = 64, i__2 = ilaenv_(&c__1, "SORMQL", ch__1, m, n, k, &c_n1, 6L, 2L);
    nb = min(i__1,i__2);
    nbmin = 2;
    ldwork = nw;
    if (nb > 1 && nb < *k) {
	iws = nw * nb;
	if (*lwork < iws) {
	    nb = *lwork / ldwork;
/* Computing MAX   
   Writing concatenation */
	    i__3[0] = 1, a__1[0] = side;
	    i__3[1] = 1, a__1[1] = trans;
	    s_cat(ch__1, a__1, i__3, &c__2, 2L);
	    i__1 = 2, i__2 = ilaenv_(&c__2, "SORMQL", ch__1, m, n, k, &c_n1, 
		    6L, 2L);
	    nbmin = max(i__1,i__2);
	}
    } else {
	iws = nw;
    }

    if (nb < nbmin || nb >= *k) {

/*        Use unblocked code */

	sorm2l_(side, trans, m, n, k, &A(1,1), lda, &TAU(1), &C(1,1)
		, ldc, &WORK(1), &iinfo);
    } else {

/*        Use blocked code */

/*
	if (left && notran || ! left && ! notran) {
*/
/*      Parenthesis added by MDG on 03-09-05 */
	if ((left && notran) || (! left && ! notran)) {
	    i1 = 1;
	    i2 = *k;
	    i3 = nb;
	} else {
	    i1 = (*k - 1) / nb * nb + 1;
	    i2 = 1;
	    i3 = -nb;
	}

	if (left) {
	    ni = *n;
	} else {
	    mi = *m;
	}

	i__1 = i2;
	i__2 = i3;
	for (i = i1; i3 < 0 ? i >= i2 : i <= i2; i += i3) {
/* Computing MIN */
	    i__4 = nb, i__5 = *k - i + 1;
	    ib = min(i__4,i__5);

/*           Form the triangular factor of the block reflector   
             H = H(i+ib-1) . . . H(i+1) H(i) */

	    i__4 = nq - *k + i + ib - 1;
	    slarft_("Backward", "Columnwise", &i__4, &ib, &A(1,i), 
		    lda, &TAU(i), t, &c__65);
	    if (left) {

/*              H or H' is applied to C(1:m-k+i+ib-1,1:n) */

		mi = *m - *k + i + ib - 1;
	    } else {

/*              H or H' is applied to C(1:m,1:n-k+i+ib-1) */

		ni = *n - *k + i + ib - 1;
	    }

/*           Apply H or H' */

	    slarfb_(side, trans, "Backward", "Columnwise", &mi, &ni, &ib, &A(1,i), lda, t, &c__65, &C(1,1), ldc, &WORK(
		    1), &ldwork);
/* L10: */
	}
    }
    WORK(1) = (real) iws;
    return 0;

/*     End of SORMQL */

} /* sormql_ */
