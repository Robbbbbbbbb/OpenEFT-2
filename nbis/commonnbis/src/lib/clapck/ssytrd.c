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

/* Subroutine */ int ssytrd_(char *uplo, int *n, real *a, int *lda, 
	real *d, real *e, real *tau, real *work, int *lwork, int *
	info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SSYTRD reduces a real symmetric matrix A to real symmetric   
    tridiagonal form T by an orthogonal similarity transformation:   
    Q**T * A * Q = T.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            = 'U':  Upper triangle of A is stored;   
            = 'L':  Lower triangle of A is stored.   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading 
  
            N-by-N upper triangular part of A contains the upper   
            triangular part of the matrix A, and the strictly lower   
            triangular part of A is not referenced.  If UPLO = 'L', the   
            leading N-by-N lower triangular part of A contains the lower 
  
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

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.  LWORK >= 1.   
            For optimum performance LWORK >= N*NB, where NB is the   
            optimal blocksize.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   

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
    static int c_n1 = -1;
    static int c__3 = 3;
    static int c__2 = 2;
    static real c_b22 = -1.f;
    static real c_b23 = 1.f;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset;
*/
    int i__1, i__2, i__3;
    /* Local variables */
    static int i, j;
    extern logical lsame_(char *, char *);
    static int nbmin, iinfo;
    static logical upper;
    static int nb, kk;
    extern /* Subroutine */ int ssytd2_(char *, int *, real *, int *, 
	    real *, real *, real *, int *), ssyr2k_(char *, char *
	    , int *, int *, real *, real *, int *, real *, 
	    int *, real *, real *, int *);
    static int nx;
    extern /* Subroutine */ int xerbla_(char *, int *);
    extern int ilaenv_(int *, char *, char *, int *, int *, 
	    int *, int *, ftnlen, ftnlen);
    extern /* Subroutine */ int slatrd_(char *, int *, int *, real *, 
	    int *, real *, real *, real *, int *);
    static int ldwork, iws;



#define D(I) d[(I)-1]
#define E(I) e[(I)-1]
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
    } else if (*lwork < 1) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSYTRD", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	WORK(1) = 1.f;
	return 0;
    }

/*     Determine the block size. */

    nb = ilaenv_(&c__1, "SSYTRD", uplo, n, &c_n1, &c_n1, &c_n1, 6L, 1L);
    nx = *n;
    iws = 1;
    if (nb > 1 && nb < *n) {

/*        Determine when to cross over from blocked to unblocked code 
  
          (last block is always handled by unblocked code).   

   Computing MAX */
	i__1 = nb, i__2 = ilaenv_(&c__3, "SSYTRD", uplo, n, &c_n1, &c_n1, &
		c_n1, 6L, 1L);
	nx = max(i__1,i__2);
	if (nx < *n) {

/*           Determine if workspace is large enough for blocked co
de. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  deter
mine the   
                minimum value of NB, and reduce NB or force us
e of   
                unblocked code by setting NX = N.   

   Computing MAX */
		i__1 = *lwork / ldwork;
		nb = max(i__1,1);
		nbmin = ilaenv_(&c__2, "SSYTRD", uplo, n, &c_n1, &c_n1, &c_n1,
			 6L, 1L);
		if (nb < nbmin) {
		    nx = *n;
		}
	    }
	} else {
	    nx = *n;
	}
    } else {
	nb = 1;
    }

    if (upper) {

/*        Reduce the upper triangle of A.   
          Columns 1:kk are handled by the unblocked method. */

	kk = *n - (*n - nx + nb - 1) / nb * nb;
	i__1 = kk + 1;
	i__2 = -nb;
	for (i = *n - nb + 1; -nb < 0 ? i >= kk+1 : i <= kk+1; i += -nb) {

/*           Reduce columns i:i+nb-1 to tridiagonal form and form 
the   
             matrix W which is needed to update the unreduced part
 of   
             the matrix */

	    i__3 = i + nb - 1;
	    slatrd_(uplo, &i__3, &nb, &A(1,1), lda, &E(1), &TAU(1), &
		    WORK(1), &ldwork);

/*           Update the unreduced submatrix A(1:i-1,1:i-1), using 
an   
             update of the form:  A := A - V*W' - W*V' */

	    i__3 = i - 1;
	    ssyr2k_(uplo, "No transpose", &i__3, &nb, &c_b22, &A(1,i), lda, &WORK(1), &ldwork, &c_b23, &A(1,1), lda);

/*           Copy superdiagonal elements back into A, and diagonal
   
             elements into D */

	    i__3 = i + nb - 1;
	    for (j = i; j <= i+nb-1; ++j) {
		A(j-1,j) = E(j - 1);
		D(j) = A(j,j);
/* L10: */
	    }
/* L20: */
	}

/*        Use unblocked code to reduce the last or only block */

	ssytd2_(uplo, &kk, &A(1,1), lda, &D(1), &E(1), &TAU(1), &iinfo);
    } else {

/*        Reduce the lower triangle of A */

	i__2 = *n - nx;
	i__1 = nb;
	for (i = 1; nb < 0 ? i >= *n-nx : i <= *n-nx; i += nb) {

/*           Reduce columns i:i+nb-1 to tridiagonal form and form 
the   
             matrix W which is needed to update the unreduced part
 of   
             the matrix */

	    i__3 = *n - i + 1;
	    slatrd_(uplo, &i__3, &nb, &A(i,i), lda, &E(i), &TAU(i),
		     &WORK(1), &ldwork);

/*           Update the unreduced submatrix A(i+ib:n,i+ib:n), usin
g   
             an update of the form:  A := A - V*W' - W*V' */

	    i__3 = *n - i - nb + 1;
	    ssyr2k_(uplo, "No transpose", &i__3, &nb, &c_b22, &A(i+nb,i), lda, &WORK(nb + 1), &ldwork, &c_b23, &A(i+nb,i+nb), lda);

/*           Copy subdiagonal elements back into A, and diagonal 
  
             elements into D */

	    i__3 = i + nb - 1;
	    for (j = i; j <= i+nb-1; ++j) {
		A(j+1,j) = E(j);
		D(j) = A(j,j);
/* L30: */
	    }
/* L40: */
	}

/*        Use unblocked code to reduce the last or only block */

	i__1 = *n - i + 1;
	ssytd2_(uplo, &i__1, &A(i,i), lda, &D(i), &E(i), &TAU(i), &
		iinfo);
    }

    WORK(1) = (real) iws;
    return 0;

/*     End of SSYTRD */

} /* ssytrd_ */
