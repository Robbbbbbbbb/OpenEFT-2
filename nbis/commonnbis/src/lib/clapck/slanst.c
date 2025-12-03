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

doublereal slanst_(char *norm, int *n, real *d, real *e)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLANST  returns the value of the one norm,  or the Frobenius norm, or 
  
    the  infinity norm,  or the  element of  largest absolute value  of a 
  
    real symmetric tridiagonal matrix A.   

    Description   
    ===========   

    SLANST returns the value   

       SLANST = ( max(abs(A(i,j))), NORM = 'M' or 'm'   
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
            Specifies the value to be returned in SLANST as described   
            above.   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.  When N = 0, SLANST is   
            set to zero.   

    D       (input) REAL array, dimension (N)   
            The diagonal elements of A.   

    E       (input) REAL array, dimension (N-1)   
            The (n-1) sub-diagonal or super-diagonal elements of A.   

    ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    
    /* System generated locals */
    int i__1;
    real ret_val, r__1, r__2, r__3, r__4, r__5;
    /* Builtin functions */
    double sqrt(doublereal);
    /* Local variables */
    static int i;
    static real scale;
    extern logical lsame_(char *, char *);
    static real anorm;
    extern /* Subroutine */ int slassq_(int *, real *, int *, real *, 
	    real *);
    static real sum;



#define E(I) e[(I)-1]
#define D(I) d[(I)-1]


    if (*n <= 0) {
	anorm = 0.f;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	anorm = (r__1 = D(*n), dabs(r__1));
	i__1 = *n - 1;
	for (i = 1; i <= *n-1; ++i) {
/* Computing MAX */
	    r__2 = anorm, r__3 = (r__1 = D(i), dabs(r__1));
	    anorm = dmax(r__2,r__3);
/* Computing MAX */
	    r__2 = anorm, r__3 = (r__1 = E(i), dabs(r__1));
	    anorm = dmax(r__2,r__3);
/* L10: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)norm == '1' || 
	    lsame_(norm, "I")) {

/*        Find norm1(A). */

	if (*n == 1) {
	    anorm = dabs(D(1));
	} else {
/* Computing MAX */
	    r__3 = dabs(D(1)) + dabs(E(1)), r__4 = (r__1 = E(*n - 1), dabs(
		    r__1)) + (r__2 = D(*n), dabs(r__2));
	    anorm = dmax(r__3,r__4);
	    i__1 = *n - 1;
	    for (i = 2; i <= *n-1; ++i) {
/* Computing MAX */
		r__4 = anorm, r__5 = (r__1 = D(i), dabs(r__1)) + (r__2 = E(i),
			 dabs(r__2)) + (r__3 = E(i - 1), dabs(r__3));
		anorm = dmax(r__4,r__5);
/* L20: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.f;
	sum = 1.f;
	if (*n > 1) {
	    i__1 = *n - 1;
	    slassq_(&i__1, &E(1), &c__1, &scale, &sum);
	    sum *= 2;
	}
	slassq_(n, &D(1), &c__1, &scale, &sum);
	anorm = scale * sqrt(sum);
    }

    ret_val = anorm;
    return ret_val;

/*     End of SLANST */

} /* slanst_ */
