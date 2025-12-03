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

/* Subroutine */ int slascl_(char *type, int *kl, int *ku, real *
	cfrom, real *cto, int *m, int *n, real *a, int *lda, 
	int *info)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLASCL multiplies the M by N real matrix A by the real scalar   
    CTO/CFROM.  This is done without over/underflow as long as the final 
  
    result CTO*A(I,J)/CFROM does not over/underflow. TYPE specifies that 
  
    A may be full, upper triangular, lower triangular, upper Hessenberg, 
  
    or banded.   

    Arguments   
    =========   

    TYPE    (input) CHARACTER*1   
            TYPE indices the storage type of the input matrix.   
            = 'G':  A is a full matrix.   
            = 'L':  A is a lower triangular matrix.   
            = 'U':  A is an upper triangular matrix.   
            = 'H':  A is an upper Hessenberg matrix.   
            = 'B':  A is a symmetric band matrix with lower bandwidth KL 
  
                    and upper bandwidth KU and with the only the lower   
                    half stored.   
            = 'Q':  A is a symmetric band matrix with lower bandwidth KL 
  
                    and upper bandwidth KU and with the only the upper   
                    half stored.   
            = 'Z':  A is a band matrix with lower bandwidth KL and upper 
  
                    bandwidth KU.   

    KL      (input) INTEGER   
            The lower bandwidth of A.  Referenced only if TYPE = 'B',   
            'Q' or 'Z'.   

    KU      (input) INTEGER   
            The upper bandwidth of A.  Referenced only if TYPE = 'B',   
            'Q' or 'Z'.   

    CFROM   (input) REAL   
    CTO     (input) REAL   
            The matrix A is multiplied by CTO/CFROM. A(I,J) is computed   
            without over/underflow if the final result CTO*A(I,J)/CFROM   
            can be represented without over/underflow.  CFROM must be   
            nonzero.   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,M)   
            The matrix to be multiplied by CTO/CFROM.  See TYPE for the   
            storage type.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    INFO    (output) INTEGER   
            0  - successful exit   
            <0 - if INFO = -i, the i-th argument had an illegal value.   

    ===================================================================== 
  


       Test the input arguments   

    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    integer a_dim1, a_offset;
*/
    int i__1, i__2, i__3, i__4, i__5;
    /* Local variables */
    static logical done;
    static real ctoc;
    static int i, j;
    extern logical lsame_(char *, char *);
    static int itype, k1, k2, k3, k4;
    static real cfrom1;
    extern doublereal slamch_(char *);
    static real cfromc;
    extern /* Subroutine */ int xerbla_(char *, int *);
    static real bignum, smlnum, mul, cto1;



#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    *info = 0;

    if (lsame_(type, "G")) {
	itype = 0;
    } else if (lsame_(type, "L")) {
	itype = 1;
    } else if (lsame_(type, "U")) {
	itype = 2;
    } else if (lsame_(type, "H")) {
	itype = 3;
    } else if (lsame_(type, "B")) {
	itype = 4;
    } else if (lsame_(type, "Q")) {
	itype = 5;
    } else if (lsame_(type, "Z")) {
	itype = 6;
    } else {
	itype = -1;
    }

    if (itype == -1) {
	*info = -1;
    } else if (*cfrom == 0.f) {
	*info = -4;
    } else if (*m < 0) {
	*info = -6;
/*
    } else if (*n < 0 || itype == 4 && *n != *m || itype == 5 && *n != *m) {
*/
/*  Parentheses added by MDG on 03-09-05 */
    } else if (*n < 0 || (itype == 4 && *n != *m) ||
                         (itype == 5 && *n != *m)) {
	*info = -7;
    } else if (itype <= 3 && *lda < max(1,*m)) {
	*info = -9;
    } else if (itype >= 4) {
/* Computing MAX */
	i__1 = *m - 1;
	if (*kl < 0 || *kl > max(i__1,0)) {
	    *info = -2;
	} else /* if(complicated condition) */ {
/* Computing MAX */
	    i__1 = *n - 1;
/*
	    if (*ku < 0 || *ku > max(i__1,0) || (itype == 4 || itype == 5) && 
		    *kl != *ku) {
*/
/*          Parentheses added by MDG on 03-09-05 */
	    if (*ku < 0 || *ku > max(i__1,0) ||
                ((itype == 4 || itype == 5) && *kl != *ku)) {
		*info = -3;
/*
	    } else if (itype == 4 && *lda < *kl + 1 || itype == 5 && *lda < *
		    ku + 1 || itype == 6 && *lda < (*kl << 1) + *ku + 1) {
*/
/*          Parentheses added by MDG on 03-09-05 */
	    } else if ((itype == 4 && *lda < *kl + 1) ||
                       (itype == 5 && *lda < *ku + 1) ||
                       (itype == 6 && *lda < (*kl << 1) + *ku + 1)) {
		*info = -9;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLASCL", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *m == 0) {
	return 0;
    }

/*     Get machine parameters */

    smlnum = slamch_("S");
    bignum = 1.f / smlnum;

    cfromc = *cfrom;
    ctoc = *cto;

L10:
    cfrom1 = cfromc * smlnum;
    cto1 = ctoc / bignum;
    if (dabs(cfrom1) > dabs(ctoc) && ctoc != 0.f) {
	mul = smlnum;
	done = FALSE_;
	cfromc = cfrom1;
    } else if (dabs(cto1) > dabs(cfromc)) {
	mul = bignum;
	done = FALSE_;
	ctoc = cto1;
    } else {
	mul = ctoc / cfromc;
	done = TRUE_;
    }

    if (itype == 0) {

/*        Full matrix */

	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
	    i__2 = *m;
	    for (i = 1; i <= *m; ++i) {
		A(i,j) *= mul;
/* L20: */
	    }
/* L30: */
	}

    } else if (itype == 1) {

/*        Lower triangular matrix */

	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
	    i__2 = *m;
	    for (i = j; i <= *m; ++i) {
		A(i,j) *= mul;
/* L40: */
	    }
/* L50: */
	}

    } else if (itype == 2) {

/*        Upper triangular matrix */

	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
	    i__2 = min(j,*m);
	    for (i = 1; i <= min(j,*m); ++i) {
		A(i,j) *= mul;
/* L60: */
	    }
/* L70: */
	}

    } else if (itype == 3) {

/*        Upper Hessenberg matrix */

	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
/* Computing MIN */
	    i__3 = j + 1;
	    i__2 = min(i__3,*m);
	    for (i = 1; i <= min(j+1,*m); ++i) {
		A(i,j) *= mul;
/* L80: */
	    }
/* L90: */
	}

    } else if (itype == 4) {

/*        Lower half of a symmetric band matrix */

	k3 = *kl + 1;
	k4 = *n + 1;
	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
/* Computing MIN */
	    i__3 = k3, i__4 = k4 - j;
	    i__2 = min(i__3,i__4);
	    for (i = 1; i <= min(k3,k4-j); ++i) {
		A(i,j) *= mul;
/* L100: */
	    }
/* L110: */
	}

    } else if (itype == 5) {

/*        Upper half of a symmetric band matrix */

	k1 = *ku + 2;
	k3 = *ku + 1;
	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
/* Computing MAX */
	    i__2 = k1 - j;
	    i__3 = k3;
	    for (i = max(k1-j,1); i <= k3; ++i) {
		A(i,j) *= mul;
/* L120: */
	    }
/* L130: */
	}

    } else if (itype == 6) {

/*        Band matrix */

	k1 = *kl + *ku + 2;
	k2 = *kl + 1;
	k3 = (*kl << 1) + *ku + 1;
	k4 = *kl + *ku + 1 + *m;
	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
/* Computing MAX */
	    i__3 = k1 - j;
/* Computing MIN */
	    i__4 = k3, i__5 = k4 - j;
	    i__2 = min(i__4,i__5);
	    for (i = max(k1-j,k2); i <= min(k3,k4-j); ++i) {
		A(i,j) *= mul;
/* L140: */
	    }
/* L150: */
	}

    }

    if (! done) {
	goto L10;
    }

    return 0;

/*     End of SLASCL */

} /* slascl_ */
