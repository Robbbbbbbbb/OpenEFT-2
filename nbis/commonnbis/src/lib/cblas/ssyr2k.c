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
* Fullsource for module SSYR2K.C from package CBLAS.
* Retrieved from NETLIB on Tue Mar 14 10:28:34 2000.
* ======================================================================
*/

/*  -- translated by f2c (version 19940927).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include <f2c.h>

/* Subroutine */ int ssyr2k_(char *uplo, char *trans, int *n, int *k, 
	real *alpha, real *a, int *lda, real *b, int *ldb, real *beta,
	 real *c, int *ldc)
{


    /* System generated locals */
/*  Unused variable commented out by MDG on 03-09-05
    int a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset;
*/
    int i__1, i__2, i__3;

    /* Local variables */
    static int info;
    static real temp1, temp2;
    static int i, j, l;
    extern logical lsame_(char *, char *);
    static int nrowa;
    static logical upper;
    extern /* Subroutine */ int xerbla_(char *, int *);


/*  Purpose   
    =======   

    SSYR2K  performs one of the symmetric rank 2k operations   

       C := alpha*A*B' + alpha*B*A' + beta*C,   

    or   

       C := alpha*A'*B + alpha*B'*A + beta*C,   

    where  alpha and beta  are scalars, C is an  n by n  symmetric matrix 
  
    and  A and B  are  n by k  matrices  in the  first  case  and  k by n 
  
    matrices in the second case.   

    Parameters   
    ==========   

    UPLO   - CHARACTER*1.   
             On  entry,   UPLO  specifies  whether  the  upper  or  lower 
  
             triangular  part  of the  array  C  is to be  referenced  as 
  
             follows:   

                UPLO = 'U' or 'u'   Only the  upper triangular part of  C 
  
                                    is to be referenced.   

                UPLO = 'L' or 'l'   Only the  lower triangular part of  C 
  
                                    is to be referenced.   

             Unchanged on exit.   

    TRANS  - CHARACTER*1.   
             On entry,  TRANS  specifies the operation to be performed as 
  
             follows:   

                TRANS = 'N' or 'n'   C := alpha*A*B' + alpha*B*A' +   
                                          beta*C.   

                TRANS = 'T' or 't'   C := alpha*A'*B + alpha*B'*A +   
                                          beta*C.   

                TRANS = 'C' or 'c'   C := alpha*A'*B + alpha*B'*A +   
                                          beta*C.   

             Unchanged on exit.   

    N      - INTEGER.   
             On entry,  N specifies the order of the matrix C.  N must be 
  
             at least zero.   
             Unchanged on exit.   

    K      - INTEGER.   
             On entry with  TRANS = 'N' or 'n',  K  specifies  the number 
  
             of  columns  of the  matrices  A and B,  and on  entry  with 
  
             TRANS = 'T' or 't' or 'C' or 'c',  K  specifies  the  number 
  
             of rows of the matrices  A and B.  K must be at least  zero. 
  
             Unchanged on exit.   

    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   

    A      - REAL             array of DIMENSION ( LDA, ka ), where ka is 
  
             k  when  TRANS = 'N' or 'n',  and is  n  otherwise.   
             Before entry with  TRANS = 'N' or 'n',  the  leading  n by k 
  
             part of the array  A  must contain the matrix  A,  otherwise 
  
             the leading  k by n  part of the array  A  must contain  the 
  
             matrix A.   
             Unchanged on exit.   

    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared 
  
             in  the  calling  (sub)  program.   When  TRANS = 'N' or 'n' 
  
             then  LDA must be at least  max( 1, n ), otherwise  LDA must 
  
             be at least  max( 1, k ).   
             Unchanged on exit.   

    B      - REAL             array of DIMENSION ( LDB, kb ), where kb is 
  
             k  when  TRANS = 'N' or 'n',  and is  n  otherwise.   
             Before entry with  TRANS = 'N' or 'n',  the  leading  n by k 
  
             part of the array  B  must contain the matrix  B,  otherwise 
  
             the leading  k by n  part of the array  B  must contain  the 
  
             matrix B.   
             Unchanged on exit.   

    LDB    - INTEGER.   
             On entry, LDB specifies the first dimension of B as declared 
  
             in  the  calling  (sub)  program.   When  TRANS = 'N' or 'n' 
  
             then  LDB must be at least  max( 1, n ), otherwise  LDB must 
  
             be at least  max( 1, k ).   
             Unchanged on exit.   

    BETA   - REAL            .   
             On entry, BETA specifies the scalar beta.   
             Unchanged on exit.   

    C      - REAL             array of DIMENSION ( LDC, n ).   
             Before entry  with  UPLO = 'U' or 'u',  the leading  n by n 
  
             upper triangular part of the array C must contain the upper 
  
             triangular part  of the  symmetric matrix  and the strictly 
  
             lower triangular part of C is not referenced.  On exit, the 
  
             upper triangular part of the array  C is overwritten by the 
  
             upper triangular part of the updated matrix.   
             Before entry  with  UPLO = 'L' or 'l',  the leading  n by n 
  
             lower triangular part of the array C must contain the lower 
  
             triangular part  of the  symmetric matrix  and the strictly 
  
             upper triangular part of C is not referenced.  On exit, the 
  
             lower triangular part of the array  C is overwritten by the 
  
             lower triangular part of the updated matrix.   

    LDC    - INTEGER.   
             On entry, LDC specifies the first dimension of C as declared 
  
             in  the  calling  (sub)  program.   LDC  must  be  at  least 
  
             max( 1, n ).   
             Unchanged on exit.   


    Level 3 Blas routine.   


    -- Written on 8-February-1989.   
       Jack Dongarra, Argonne National Laboratory.   
       Iain Duff, AERE Harwell.   
       Jeremy Du Croz, Numerical Algorithms Group Ltd.   
       Sven Hammarling, Numerical Algorithms Group Ltd.   



       Test the input parameters.   

    
   Parameter adjustments   
       Function Body */

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]
#define B(I,J) b[(I)-1 + ((J)-1)* ( *ldb)]
#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]

    if (lsame_(trans, "N")) {
	nrowa = *n;
    } else {
	nrowa = *k;
    }
    upper = lsame_(uplo, "U");

    info = 0;
    if (! upper && ! lsame_(uplo, "L")) {
	info = 1;
    } else if (! lsame_(trans, "N") && ! lsame_(trans, "T") &&
	     ! lsame_(trans, "C")) {
	info = 2;
    } else if (*n < 0) {
	info = 3;
    } else if (*k < 0) {
	info = 4;
    } else if (*lda < max(1,nrowa)) {
	info = 7;
    } else if (*ldb < max(1,nrowa)) {
	info = 9;
    } else if (*ldc < max(1,*n)) {
	info = 12;
    }
    if (info != 0) {
	xerbla_("SSYR2K", &info);
	return 0;
    }

/*     Quick return if possible. */

/*
    if (*n == 0 || (*alpha == 0.f || *k == 0) && *beta == 1.f) {
*/
/*  Parenteses added by MDG on 03-09-05 */
    if (*n == 0 || ((*alpha == 0.f || *k == 0) && *beta == 1.f)) {
	return 0;
    }

/*     And when  alpha.eq.zero. */

    if (*alpha == 0.f) {
	if (upper) {
	    if (*beta == 0.f) {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    i__2 = j;
		    for (i = 1; i <= j; ++i) {
			C(i,j) = 0.f;
/* L10: */
		    }
/* L20: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    i__2 = j;
		    for (i = 1; i <= j; ++i) {
			C(i,j) = *beta * C(i,j);
/* L30: */
		    }
/* L40: */
		}
	    }
	} else {
	    if (*beta == 0.f) {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    i__2 = *n;
		    for (i = j; i <= *n; ++i) {
			C(i,j) = 0.f;
/* L50: */
		    }
/* L60: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    i__2 = *n;
		    for (i = j; i <= *n; ++i) {
			C(i,j) = *beta * C(i,j);
/* L70: */
		    }
/* L80: */
		}
	    }
	}
	return 0;
    }

/*     Start the operations. */

    if (lsame_(trans, "N")) {

/*        Form  C := alpha*A*B' + alpha*B*A' + C. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		if (*beta == 0.f) {
		    i__2 = j;
		    for (i = 1; i <= j; ++i) {
			C(i,j) = 0.f;
/* L90: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = j;
		    for (i = 1; i <= j; ++i) {
			C(i,j) = *beta * C(i,j);
/* L100: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= *k; ++l) {
		    if (A(j,l) != 0.f || B(j,l) != 0.f) 
			    {
			temp1 = *alpha * B(j,l);
			temp2 = *alpha * A(j,l);
			i__3 = j;
			for (i = 1; i <= j; ++i) {
			    C(i,j) = C(i,j) + A(i,l) * temp1 + B(i,l) * 
				    temp2;
/* L110: */
			}
		    }
/* L120: */
		}
/* L130: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		if (*beta == 0.f) {
		    i__2 = *n;
		    for (i = j; i <= *n; ++i) {
			C(i,j) = 0.f;
/* L140: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = *n;
		    for (i = j; i <= *n; ++i) {
			C(i,j) = *beta * C(i,j);
/* L150: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= *k; ++l) {
		    if (A(j,l) != 0.f || B(j,l) != 0.f) 
			    {
			temp1 = *alpha * B(j,l);
			temp2 = *alpha * A(j,l);
			i__3 = *n;
			for (i = j; i <= *n; ++i) {
			    C(i,j) = C(i,j) + A(i,l) * temp1 + B(i,l) * 
				    temp2;
/* L160: */
			}
		    }
/* L170: */
		}
/* L180: */
	    }
	}
    } else {

/*        Form  C := alpha*A'*B + alpha*B'*A + C. */

	if (upper) {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = j;
		for (i = 1; i <= j; ++i) {
		    temp1 = 0.f;
		    temp2 = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= *k; ++l) {
			temp1 += A(l,i) * B(l,j);
			temp2 += B(l,i) * A(l,j);
/* L190: */
		    }
		    if (*beta == 0.f) {
			C(i,j) = *alpha * temp1 + *alpha * temp2;
		    } else {
			C(i,j) = *beta * C(i,j) + *
				alpha * temp1 + *alpha * temp2;
		    }
/* L200: */
		}
/* L210: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = *n;
		for (i = j; i <= *n; ++i) {
		    temp1 = 0.f;
		    temp2 = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= *k; ++l) {
			temp1 += A(l,i) * B(l,j);
			temp2 += B(l,i) * A(l,j);
/* L220: */
		    }
		    if (*beta == 0.f) {
			C(i,j) = *alpha * temp1 + *alpha * temp2;
		    } else {
			C(i,j) = *beta * C(i,j) + *
				alpha * temp1 + *alpha * temp2;
		    }
/* L230: */
		}
/* L240: */
	    }
	}
    }

    return 0;

/*     End of SSYR2K. */

} /* ssyr2k_ */
