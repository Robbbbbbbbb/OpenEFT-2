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

doublereal slansy_(char *norm, char *uplo, int *n, real *a, int *lda, 
	real *work)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLANSY  returns the value of the one norm,  or the Frobenius norm, or 
  
    the  infinity norm,  or the  element of  largest absolute value  of a 
  
    real symmetric matrix A.   

    Description   
    ===========   

    SLANSY returns the value   

       SLANSY = ( max(abs(A(i,j))), NORM = 'M' or 'm'   
                (   
                ( norm1(A),         NORM = '1', 'O' or 'o'   
                (   
                ( normI(A),         NORM = 'I' or 'i'   
                (   
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'   

    where  norm1  denotes the  one norm of a matrix (maximum column sum), 
  
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and 
  
    normF  denotes the  Frobenius norm of a matrix (square root of sum of 
  
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.   

    Arguments   
    =========   

    NORM    (input) CHARACTER*1   
            Specifies the value to be returned in SLANSY as described   
            above.   

    UPLO    (input) CHARACTER*1   
            Specifies whether the upper or lower triangular part of the   
            symmetric matrix A is to be referenced.   
            = 'U':  Upper triangular part of A is referenced   
            = 'L':  Lower triangular part of A is referenced   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.  When N = 0, SLANSY is   
            set to zero.   

    A       (input) REAL array, dimension (LDA,N)   
            The symmetric matrix A.  If UPLO = 'U', the leading n by n   
            upper triangular part of A contains the upper triangular part 
  
            of the matrix A, and the strictly lower triangular part of A 
  
            is not referenced.  If UPLO = 'L', the leading n by n lower   
            triangular part of A contains the lower triangular part of   
            the matrix A, and the strictly upper triangular part of A is 
  
            not referenced.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(N,1).   

    WORK    (workspace) REAL array, dimension (LWORK),   
            where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise,   
            WORK is not referenced.   

   ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset;
*/
    int i__1, i__2;
    real ret_val, r__1, r__2, r__3;
    /* Builtin functions */
    double sqrt(doublereal);
    /* Local variables */
    static real absa;
    static int i, j;
    static real scale;
    extern logical lsame_(char *, char *);
    static real value;
    extern /* Subroutine */ int slassq_(int *, real *, int *, real *, 
	    real *);
    static real sum;



#define WORK(I) work[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    if (*n == 0) {
	value = 0.f;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.f;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = j;
		for (i = 1; i <= j; ++i) {
/* Computing MAX */
		    r__2 = value, r__3 = (r__1 = A(i,j), dabs(r__1)
			    );
		    value = dmax(r__2,r__3);
/* L10: */
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = *n;
		for (i = j; i <= *n; ++i) {
/* Computing MAX */
		    r__2 = value, r__3 = (r__1 = A(i,j), dabs(r__1)
			    );
		    value = dmax(r__2,r__3);
/* L30: */
		}
/* L40: */
	    }
	}
    } else if (lsame_(norm, "I") || lsame_(norm, "O") || *(
	    unsigned char *)norm == '1') {

/*        Find normI(A) ( = norm1(A), since A is symmetric). */

	value = 0.f;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		sum = 0.f;
		i__2 = j - 1;
		for (i = 1; i <= j-1; ++i) {
		    absa = (r__1 = A(i,j), dabs(r__1));
		    sum += absa;
		    WORK(i) += absa;
/* L50: */
		}
		WORK(j) = sum + (r__1 = A(j,j), dabs(r__1));
/* L60: */
	    }
	    i__1 = *n;
	    for (i = 1; i <= *n; ++i) {
/* Computing MAX */
		r__1 = value, r__2 = WORK(i);
		value = dmax(r__1,r__2);
/* L70: */
	    }
	} else {
	    i__1 = *n;
	    for (i = 1; i <= *n; ++i) {
		WORK(i) = 0.f;
/* L80: */
	    }
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		sum = WORK(j) + (r__1 = A(j,j), dabs(r__1));
		i__2 = *n;
		for (i = j + 1; i <= *n; ++i) {
		    absa = (r__1 = A(i,j), dabs(r__1));
		    sum += absa;
		    WORK(i) += absa;
/* L90: */
		}
		value = dmax(value,sum);
/* L100: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.f;
	sum = 1.f;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 2; j <= *n; ++j) {
		i__2 = j - 1;
		slassq_(&i__2, &A(1,j), &c__1, &scale, &sum);
/* L110: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= *n-1; ++j) {
		i__2 = *n - j;
		slassq_(&i__2, &A(j+1,j), &c__1, &scale, &sum);
/* L120: */
	    }
	}
	sum *= 2;
	i__1 = *lda + 1;
	slassq_(n, &A(1,1), &i__1, &scale, &sum);
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of SLANSY */

} /* slansy_ */
