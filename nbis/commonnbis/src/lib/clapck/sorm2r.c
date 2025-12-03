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

/* Subroutine */ int sorm2r_(char *side, char *trans, int *m, int *n, 
	int *k, real *a, int *lda, real *tau, real *c, int *ldc, 
	real *work, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SORM2R overwrites the general real m by n matrix C with   

          Q * C  if SIDE = 'L' and TRANS = 'N', or   

          Q'* C  if SIDE = 'L' and TRANS = 'T', or   

          C * Q  if SIDE = 'R' and TRANS = 'N', or   

          C * Q' if SIDE = 'R' and TRANS = 'T',   

    where Q is a real orthogonal matrix defined as the product of k   
    elementary reflectors   

          Q = H(1) H(2) . . . H(k)   

    as returned by SGEQRF. Q is of order m if SIDE = 'L' and of order n   
    if SIDE = 'R'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply Q or Q' from the Left   
            = 'R': apply Q or Q' from the Right   

    TRANS   (input) CHARACTER*1   
            = 'N': apply Q  (No transpose)   
            = 'T': apply Q' (Transpose)   

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
  
            SGEQRF in the first k columns of its array argument A.   
            A is modified by the routine but restored on exit.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.   
            If SIDE = 'L', LDA >= max(1,M);   
            if SIDE = 'R', LDA >= max(1,N).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQRF.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the m by n matrix C.   
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace) REAL array, dimension   
                                     (N) if SIDE = 'L',   
                                     (M) if SIDE = 'R'   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument had an illegal value   

    ===================================================================== 
  


       Test the input arguments   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset, c_dim1, c_offset;
*/
    int i__1, i__2;
    /* Local variables */
    static logical left;
    static int i;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int slarf_(char *, int *, int *, real *, 
	    int *, real *, real *, int *, real *);
    static int i1, i2, i3, ic, jc, mi, ni, nq;
    extern /* Subroutine */ int xerbla_(char *, int *);
    static logical notran;
    static real aii;



#define TAU(I) tau[(I)-1]
#define WORK(I) work[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]
#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]

    *info = 0;
    left = lsame_(side, "L");
    notran = lsame_(trans, "N");

/*     NQ is the order of Q */

    if (left) {
	nq = *m;
    } else {
	nq = *n;
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
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORM2R", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || *k == 0) {
	return 0;
    }
/*
    if (left && ! notran || ! left && notran) {
*/
/*  Parenteses added by MDG on 03-09-05 */
    if ((left && ! notran) || (! left && notran)) {
	i1 = 1;
	i2 = *k;
	i3 = 1;
    } else {
	i1 = *k;
	i2 = 1;
	i3 = -1;
    }

    if (left) {
	ni = *n;
	jc = 1;
    } else {
	mi = *m;
	ic = 1;
    }

    i__1 = i2;
    i__2 = i3;
    for (i = i1; i3 < 0 ? i >= i2 : i <= i2; i += i3) {
	if (left) {

/*           H(i) is applied to C(i:m,1:n) */

	    mi = *m - i + 1;
	    ic = i;
	} else {

/*           H(i) is applied to C(1:m,i:n) */

	    ni = *n - i + 1;
	    jc = i;
	}

/*        Apply H(i) */

	aii = A(i,i);
	A(i,i) = 1.f;
	slarf_(side, &mi, &ni, &A(i,i), &c__1, &TAU(i), &C(ic,jc), ldc, &WORK(1));
	A(i,i) = aii;
/* L10: */
    }
    return 0;

/*     End of SORM2R */

} /* sorm2r_ */
