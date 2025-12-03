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

/* Subroutine */ int ssytd2_(char *uplo, int *n, real *a, int *lda, 
	real *d, real *e, real *tau, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SSYTD2 reduces a real symmetric matrix A to symmetric tridiagonal   
    form T by an orthogonal similarity transformation: Q' * A * Q = T.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            Specifies whether the upper or lower triangular part of the   
            symmetric matrix A is stored:   
            = 'U':  Upper triangular   
            = 'L':  Lower triangular   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading 
  
            n-by-n upper triangular part of A contains the upper   
            triangular part of the matrix A, and the strictly lower   
            triangular part of A is not referenced.  If UPLO = 'L', the   
            leading n-by-n lower triangular part of A contains the lower 
  
            triangular part of the matrix A, and the strictly upper   
            triangular part of A is not referenced.   
            On exit, if UPLO = 'U', the diagonal and first superdiagonal 
  
            of A are overwritten by the corresponding elements of the   
            tridiagonal matrix T, and the elements above the first   
            superdiagonal, with the array TAU, represent the orthogonal   
            matrix Q as a product of elementary reflectors; if UPLO   
            = 'L', the diagonal and first subdiagonal of A are over-   
            written by the corresponding elements of the tridiagonal   
            matrix T, and the elements below the first subdiagonal, with 
  
            the array TAU, represent the orthogonal matrix Q as a product 
  
            of elementary reflectors. See Further Details.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,N).   

    D       (output) REAL array, dimension (N)   
            The diagonal elements of the tridiagonal matrix T:   
            D(i) = A(i,i).   

    E       (output) REAL array, dimension (N-1)   
            The off-diagonal elements of the tridiagonal matrix T:   
            E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'. 
  

    TAU     (output) REAL array, dimension (N-1)   
            The scalar factors of the elementary reflectors (see Further 
  
            Details).   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   

    Further Details   
    ===============   

    If UPLO = 'U', the matrix Q is represented as a product of elementary 
  
    reflectors   

       Q = H(n-1) . . . H(2) H(1).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in   
    A(1:i-1,i+1), and tau in TAU(i).   

    If UPLO = 'L', the matrix Q is represented as a product of elementary 
  
    reflectors   

       Q = H(1) H(2) . . . H(n-1).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i), 
  
    and tau in TAU(i).   

    The contents of A on exit are illustrated by the following examples   
    with n = 5:   

    if UPLO = 'U':                       if UPLO = 'L':   

      (  d   e   v2  v3  v4 )              (  d                  )   
      (      d   e   v3  v4 )              (  e   d              )   
      (          d   e   v4 )              (  v1  e   d          )   
      (              d   e  )              (  v1  v2  e   d      )   
      (                  d  )              (  v1  v2  v3  e   d  )   

    where d and e denote diagonal and off-diagonal elements of T, and vi 
  
    denotes an element of the vector defining H(i).   

    ===================================================================== 
  


       Test the input parameters   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    static real c_b8 = 0.f;
    static real c_b14 = -1.f;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset;
*/
    int i__1, i__2, i__3;
    /* Local variables */
    static real taui;
    extern doublereal sdot_(int *, real *, int *, real *, int *);
    static int i;
    extern /* Subroutine */ int ssyr2_(char *, int *, real *, real *, 
	    int *, real *, int *, real *, int *);
    static real alpha;
    extern logical lsame_(char *, char *);
    static logical upper;
    extern /* Subroutine */ int saxpy_(int *, real *, real *, int *, 
	    real *, int *), ssymv_(char *, int *, real *, real *, 
	    int *, real *, int *, real *, real *, int *), 
	    xerbla_(char *, int *), slarfg_(int *, real *, 
	    real *, int *, real *);

/* Unused variable commented out by MDG on 03-09-05
int k;
*/

#define D(I) d[(I)-1]
#define E(I) e[(I)-1]
#define TAU(I) tau[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSYTD2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

    if (upper) {

/*        Reduce the upper triangle of A */

	for (i = *n - 1; i >= 1; --i) {

/*           Generate elementary reflector H(i) = I - tau * v * v'
   
             to annihilate A(1:i-1,i+1) */

	    slarfg_(&i, &A(i,i+1), &A(1,i+1), &
		    c__1, &taui);
	    E(i) = A(i,i+1);

	    if (taui != 0.f) {

/*              Apply H(i) from both sides to A(1:i,1:i) */

		A(i,i+1) = 1.f;

/*              Compute  x := tau * A * v  storing x in TAU(1:
i) */

		ssymv_(uplo, &i, &taui, &A(1,1), lda, &A(1,i+1), &c__1, &c_b8, &TAU(1), &c__1);

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		alpha = taui * -.5f * sdot_(&i, &TAU(1), &c__1, &A(1,i+1), &c__1);
		saxpy_(&i, &alpha, &A(1,i+1), &c__1, &TAU(1), &
			c__1);

/*              Apply the transformation as a rank-2 update: 
  
                   A := A - v * w' - w * v' */

		ssyr2_(uplo, &i, &c_b14, &A(1,i+1), &c__1, &
			TAU(1), &c__1, &A(1,1), lda);

		A(i,i+1) = E(i);
	    }
	    D(i + 1) = A(i+1,i+1);
	    TAU(i) = taui;
/* L10: */
	}
	D(1) = A(1,1);
    } else {

/*        Reduce the lower triangle of A */

	i__1 = *n - 1;
	for (i = 1; i <= *n-1; ++i) {

/*           Generate elementary reflector H(i) = I - tau * v * v'
   
             to annihilate A(i+2:n,i) */

	    i__2 = *n - i;
/* Computing MIN */
	    i__3 = i + 2;
	    slarfg_(&i__2, &A(i+1,i), &A(min(i+2,*n),i), &c__1, &taui);
	    E(i) = A(i+1,i);

	    if (taui != 0.f) {

/*              Apply H(i) from both sides to A(i+1:n,i+1:n) 
*/

		A(i+1,i) = 1.f;

/*              Compute  x := tau * A * v  storing y in TAU(i:
n-1) */

		i__2 = *n - i;
		ssymv_(uplo, &i__2, &taui, &A(i+1,i+1), lda, 
			&A(i+1,i), &c__1, &c_b8, &TAU(i), &c__1);

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		i__2 = *n - i;
		alpha = taui * -.5f * sdot_(&i__2, &TAU(i), &c__1, &A(i+1,i), &c__1);
		i__2 = *n - i;
		saxpy_(&i__2, &alpha, &A(i+1,i), &c__1, &TAU(i), 
			&c__1);

/*              Apply the transformation as a rank-2 update: 
  
                   A := A - v * w' - w * v' */

		i__2 = *n - i;
		ssyr2_(uplo, &i__2, &c_b14, &A(i+1,i), &c__1, &
			TAU(i), &c__1, &A(i+1,i+1), lda);

		A(i+1,i) = E(i);
	    }
	    D(i) = A(i,i);
	    TAU(i) = taui;
/* L20: */
	}
	D(*n) = A(*n,*n);
    }

    return 0;

/*     End of SSYTD2 */

} /* ssytd2_ */
