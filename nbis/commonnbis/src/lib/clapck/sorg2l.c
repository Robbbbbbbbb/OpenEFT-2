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

/* Subroutine */ int sorg2l_(int *m, int *n, int *k, real *a, 
	int *lda, real *tau, real *work, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SORG2L generates an m by n real matrix Q with orthonormal columns,   
    which is defined as the last n columns of a product of k elementary   
    reflectors of order m   

          Q  =  H(k) . . . H(2) H(1)   

    as returned by SGEQLF.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix Q. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix Q. M >= N >= 0.   

    K       (input) INTEGER   
            The number of elementary reflectors whose product defines the 
  
            matrix Q. N >= K >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the (n-k+i)-th column must contain the vector which 
  
            defines the elementary reflector H(i), for i = 1,2,...,k, as 
  
            returned by SGEQLF in the last k columns of its array   
            argument A.   
            On exit, the m by n matrix Q.   

    LDA     (input) INTEGER   
            The first dimension of the array A. LDA >= max(1,M).   

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i), as returned by SGEQLF.   

    WORK    (workspace) REAL array, dimension (N)   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -i, the i-th argument has an illegal value   

    ===================================================================== 
  


       Test the input arguments   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset;
*/
    int i__1, i__2, i__3;
    real r__1;
    /* Local variables */
    static int i, j, l;
    extern /* Subroutine */ int sscal_(int *, real *, real *, int *), 
	    slarf_(char *, int *, int *, real *, int *, real *, 
	    real *, int *, real *);
    static int ii;
    extern /* Subroutine */ int xerbla_(char *, int *);



#define TAU(I) tau[(I)-1]
#define WORK(I) work[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0 || *n > *m) {
	*info = -2;
    } else if (*k < 0 || *k > *n) {
	*info = -3;
    } else if (*lda < max(1,*m)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SORG2L", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

/*     Initialise columns 1:n-k to columns of the unit matrix */

    i__1 = *n - *k;
    for (j = 1; j <= *n-*k; ++j) {
	i__2 = *m;
	for (l = 1; l <= *m; ++l) {
	    A(l,j) = 0.f;
/* L10: */
	}
	A(*m-*n+j,j) = 1.f;
/* L20: */
    }

    i__1 = *k;
    for (i = 1; i <= *k; ++i) {
	ii = *n - *k + i;

/*        Apply H(i) to A(1:m-k+i,1:n-k+i) from the left */

	A(*m-*n+ii,ii) = 1.f;
	i__2 = *m - *n + ii;
	i__3 = ii - 1;
	slarf_("Left", &i__2, &i__3, &A(1,ii), &c__1, &TAU(i), &A(1,1), lda, &WORK(1));
	i__2 = *m - *n + ii - 1;
	r__1 = -(doublereal)TAU(i);
	sscal_(&i__2, &r__1, &A(1,ii), &c__1);
	A(*m-*n+ii,ii) = 1.f - TAU(i);

/*        Set A(m-k+i+1:m,n-k+i) to zero */

	i__2 = *m;
	for (l = *m - *n + ii + 1; l <= *m; ++l) {
	    A(l,ii) = 0.f;
/* L30: */
	}
/* L40: */
    }
    return 0;

/*     End of SORG2L */

} /* sorg2l_ */
