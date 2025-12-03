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
* ======================================================================
*/
#include <f2c.h>

/* Subroutine */ int slagtf_(int *n, real *a, real *lambda, real *b, real 
	*c, real *tol, real *d, int *in, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAGTF factorizes the matrix (T - lambda*I), where T is an n by n   
    tridiagonal matrix and lambda is a scalar, as   

       T - lambda*I = PLU,   

    where P is a permutation matrix, L is a unit lower tridiagonal matrix 
  
    with at most one non-zero sub-diagonal elements per column and U is   
    an upper triangular matrix with at most two non-zero super-diagonal   
    elements per column.   

    The factorization is obtained by Gaussian elimination with partial   
    pivoting and implicit row scaling.   

    The parameter LAMBDA is included in the routine so that SLAGTF may   
    be used, in conjunction with SLAGTS, to obtain eigenvectors of T by   
    inverse iteration.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The order of the matrix T.   

    A       (input/output) REAL array, dimension (N)   
            On entry, A must contain the diagonal elements of T.   

            On exit, A is overwritten by the n diagonal elements of the   
            upper triangular matrix U of the factorization of T.   

    LAMBDA  (input) REAL   
            On entry, the scalar lambda.   

    B       (input/output) REAL array, dimension (N-1)   
            On entry, B must contain the (n-1) super-diagonal elements of 
  
            T.   

            On exit, B is overwritten by the (n-1) super-diagonal   
            elements of the matrix U of the factorization of T.   

    C       (input/output) REAL array, dimension (N-1)   
            On entry, C must contain the (n-1) sub-diagonal elements of   
            T.   

            On exit, C is overwritten by the (n-1) sub-diagonal elements 
  
            of the matrix L of the factorization of T.   

    TOL     (input) REAL   
            On entry, a relative tolerance used to indicate whether or   
            not the matrix (T - lambda*I) is nearly singular. TOL should 
  
            normally be chose as approximately the largest relative error 
  
            in the elements of T. For example, if the elements of T are   
            correct to about 4 significant figures, then TOL should be   
            set to about 5*10**(-4). If TOL is supplied as less than eps, 
  
            where eps is the relative machine precision, then the value   
            eps is used in place of TOL.   

    D       (output) REAL array, dimension (N-2)   
            On exit, D is overwritten by the (n-2) second super-diagonal 
  
            elements of the matrix U of the factorization of T.   

    IN      (output) INTEGER array, dimension (N)   
            On exit, IN contains details of the permutation matrix P. If 
  
            an interchange occurred at the kth step of the elimination,   
            then IN(k) = 1, otherwise IN(k) = 0. The element IN(n)   
            returns the smallest positive integer j such that   

               abs( u(j,j) ).le. norm( (T - lambda*I)(j) )*TOL,   

            where norm( A(j) ) denotes the sum of the absolute values of 
  
            the jth row of the matrix A. If no such j exists then IN(n)   
            is returned as zero. If IN(n) is returned as positive, then a 
  
            diagonal element of U is small, indicating that   
            (T - lambda*I) is singular or nearly singular,   

    INFO    (output)   
            = 0   : successful exit   
            .lt. 0: if INFO = -k, the kth argument had an illegal value   

   ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
    int i__1;
    real r__1, r__2;
    /* Local variables */
    static real temp, mult;
    static int k;
    static real scale1, scale2, tl;
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, int *);
    static real eps, piv1, piv2;


#define IN(I) in[(I)-1]
#define D(I) d[(I)-1]
#define C(I) c[(I)-1]
#define B(I) b[(I)-1]
#define A(I) a[(I)-1]


    *info = 0;
    if (*n < 0) {
	*info = -1;
	i__1 = -(*info);
	xerbla_("SLAGTF", &i__1);
	return 0;
    }

    if (*n == 0) {
	return 0;
    }

    A(1) -= *lambda;
    IN(*n) = 0;
    if (*n == 1) {
	if (A(1) == 0.f) {
	    IN(1) = 1;
	}
	return 0;
    }

    eps = slamch_("Epsilon");

    tl = dmax(*tol,eps);
    scale1 = dabs(A(1)) + dabs(B(1));
    i__1 = *n - 1;
    for (k = 1; k <= *n-1; ++k) {
	A(k + 1) -= *lambda;
	scale2 = (r__1 = C(k), dabs(r__1)) + (r__2 = A(k + 1), dabs(r__2));
	if (k < *n - 1) {
	    scale2 += (r__1 = B(k + 1), dabs(r__1));
	}
	if (A(k) == 0.f) {
	    piv1 = 0.f;
	} else {
	    piv1 = (r__1 = A(k), dabs(r__1)) / scale1;
	}
	if (C(k) == 0.f) {
	    IN(k) = 0;
	    piv2 = 0.f;
	    scale1 = scale2;
	    if (k < *n - 1) {
		D(k) = 0.f;
	    }
	} else {
	    piv2 = (r__1 = C(k), dabs(r__1)) / scale2;
	    if (piv2 <= piv1) {
		IN(k) = 0;
		scale1 = scale2;
		C(k) /= A(k);
		A(k + 1) -= C(k) * B(k);
		if (k < *n - 1) {
		    D(k) = 0.f;
		}
	    } else {
		IN(k) = 1;
		mult = A(k) / C(k);
		A(k) = C(k);
		temp = A(k + 1);
		A(k + 1) = B(k) - mult * temp;
		if (k < *n - 1) {
		    D(k) = B(k + 1);
		    B(k + 1) = -(doublereal)mult * D(k);
		}
		B(k) = temp;
		C(k) = mult;
	    }
	}
	if (dmax(piv1,piv2) <= tl && IN(*n) == 0) {
	    IN(*n) = k;
	}
/* L10: */
    }
    if ((r__1 = A(*n), dabs(r__1)) <= scale1 * tl && IN(*n) == 0) {
	IN(*n) = *n;
    }

    return 0;

/*     End of SLAGTF */

} /* slagtf_ */
