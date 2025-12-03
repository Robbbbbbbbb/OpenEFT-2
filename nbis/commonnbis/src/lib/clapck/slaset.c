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

/* Subroutine */ int slaset_(char *uplo, int *m, int *n, real *alpha, 
	real *beta, real *a, int *lda)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLASET initializes an m-by-n matrix A to BETA on the diagonal and   
    ALPHA on the offdiagonals.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            Specifies the part of the matrix A to be set.   
            = 'U':      Upper triangular part is set; the strictly lower 
  
                        triangular part of A is not changed.   
            = 'L':      Lower triangular part is set; the strictly upper 
  
                        triangular part of A is not changed.   
            Otherwise:  All of the matrix A is set.   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    ALPHA   (input) REAL   
            The constant to which the offdiagonal elements are to be set. 
  

    BETA    (input) REAL   
            The constant to which the diagonal elements are to be set.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On exit, the leading m-by-n submatrix of A is set as follows: 
  

            if UPLO = 'U', A(i,j) = ALPHA, 1<=i<=j-1, 1<=j<=n,   
            if UPLO = 'L', A(i,j) = ALPHA, j+1<=i<=m, 1<=j<=n,   
            otherwise,     A(i,j) = ALPHA, 1<=i<=m, 1<=j<=n, i.ne.j,   

            and, for all UPLO, A(i,i) = BETA, 1<=i<=min(m,n).   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

   ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    integer a_dim1, a_offset;
*/
    int i__1, i__2, i__3;
    /* Local variables */
    static int i, j;
    extern logical lsame_(char *, char *);



#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    if (lsame_(uplo, "U")) {

/*        Set the strictly upper triangular or trapezoidal part of the
   
          array to ALPHA. */

	i__1 = *n;
	for (j = 2; j <= *n; ++j) {
/* Computing MIN */
	    i__3 = j - 1;
	    i__2 = min(i__3,*m);
	    for (i = 1; i <= min(j-1,*m); ++i) {
		A(i,j) = *alpha;
/* L10: */
	    }
/* L20: */
	}

    } else if (lsame_(uplo, "L")) {

/*        Set the strictly lower triangular or trapezoidal part of the
   
          array to ALPHA. */

	i__1 = min(*m,*n);
	for (j = 1; j <= min(*m,*n); ++j) {
	    i__2 = *m;
	    for (i = j + 1; i <= *m; ++i) {
		A(i,j) = *alpha;
/* L30: */
	    }
/* L40: */
	}

    } else {

/*        Set the leading m-by-n submatrix to ALPHA. */

	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
	    i__2 = *m;
	    for (i = 1; i <= *m; ++i) {
		A(i,j) = *alpha;
/* L50: */
	    }
/* L60: */
	}
    }

/*     Set the first min(M,N) diagonal elements to BETA. */

    i__1 = min(*m,*n);
    for (i = 1; i <= min(*m,*n); ++i) {
	A(i,i) = *beta;
/* L70: */
    }

    return 0;

/*     End of SLASET */

} /* slaset_ */
