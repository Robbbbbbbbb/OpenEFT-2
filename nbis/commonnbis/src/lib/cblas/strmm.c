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
* Source for module STRMM.C from package CBLAS.
* Retrieved from NETLIB on Tue Mar 14 10:53:42 2000.
*
* UPDATED: 03/09/2005 by MDG
* ======================================================================
*/

/*  -- translated by f2c (version 19940927).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include <f2c.h>

/* Subroutine */ int strmm_(char *side, char *uplo, char *transa, char *diag, 
	int *m, int *n, real *alpha, real *a, int *lda, real *b, 
	int *ldb)
{


    /* System generated locals */
/*  Unused variable commented out by MDG on 03-09-05
    int a_dim1, a_offset, b_dim1, b_offset;
*/
    int i__1, i__2, i__3;

    /* Local variables */
    static int info;
    static real temp;
    static int i, j, k;
    static logical lside;
    extern logical lsame_(char *, char *);
    static int nrowa;
    static logical upper;
    extern /* Subroutine */ int xerbla_(char *, int *);
    static logical nounit;


/*  Purpose   
    =======   

    STRMM  performs one of the matrix-matrix operations   

       B := alpha*op( A )*B,   or   B := alpha*B*op( A ),   

    where  alpha  is a scalar,  B  is an m by n matrix,  A  is a unit, or 
  
    non-unit,  upper or lower triangular matrix  and  op( A )  is one  of 
  

       op( A ) = A   or   op( A ) = A'.   

    Parameters   
    ==========   

    SIDE   - CHARACTER*1.   
             On entry,  SIDE specifies whether  op( A ) multiplies B from 
  
             the left or right as follows:   

                SIDE = 'L' or 'l'   B := alpha*op( A )*B.   

                SIDE = 'R' or 'r'   B := alpha*B*op( A ).   

             Unchanged on exit.   

    UPLO   - CHARACTER*1.   
             On entry, UPLO specifies whether the matrix A is an upper or 
  
             lower triangular matrix as follows:   

                UPLO = 'U' or 'u'   A is an upper triangular matrix.   

                UPLO = 'L' or 'l'   A is a lower triangular matrix.   

             Unchanged on exit.   

    TRANSA - CHARACTER*1.   
             On entry, TRANSA specifies the form of op( A ) to be used in 
  
             the matrix multiplication as follows:   

                TRANSA = 'N' or 'n'   op( A ) = A.   

                TRANSA = 'T' or 't'   op( A ) = A'.   

                TRANSA = 'C' or 'c'   op( A ) = A'.   

             Unchanged on exit.   

    DIAG   - CHARACTER*1.   
             On entry, DIAG specifies whether or not A is unit triangular 
  
             as follows:   

                DIAG = 'U' or 'u'   A is assumed to be unit triangular.   

                DIAG = 'N' or 'n'   A is not assumed to be unit   
                                    triangular.   

             Unchanged on exit.   

    M      - INTEGER.   
             On entry, M specifies the number of rows of B. M must be at 
  
             least zero.   
             Unchanged on exit.   

    N      - INTEGER.   
             On entry, N specifies the number of columns of B.  N must be 
  
             at least zero.   
             Unchanged on exit.   

    ALPHA  - REAL            .   
             On entry,  ALPHA specifies the scalar  alpha. When  alpha is 
  
             zero then  A is not referenced and  B need not be set before 
  
             entry.   
             Unchanged on exit.   

    A      - REAL             array of DIMENSION ( LDA, k ), where k is m 
  
             when  SIDE = 'L' or 'l'  and is  n  when  SIDE = 'R' or 'r'. 
  
             Before entry  with  UPLO = 'U' or 'u',  the  leading  k by k 
  
             upper triangular part of the array  A must contain the upper 
  
             triangular matrix  and the strictly lower triangular part of 
  
             A is not referenced.   
             Before entry  with  UPLO = 'L' or 'l',  the  leading  k by k 
  
             lower triangular part of the array  A must contain the lower 
  
             triangular matrix  and the strictly upper triangular part of 
  
             A is not referenced.   
             Note that when  DIAG = 'U' or 'u',  the diagonal elements of 
  
             A  are not referenced either,  but are assumed to be  unity. 
  
             Unchanged on exit.   

    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared 
  
             in the calling (sub) program.  When  SIDE = 'L' or 'l'  then 
  
             LDA  must be at least  max( 1, m ),  when  SIDE = 'R' or 'r' 
  
             then LDA must be at least max( 1, n ).   
             Unchanged on exit.   

    B      - REAL             array of DIMENSION ( LDB, n ).   
             Before entry,  the leading  m by n part of the array  B must 
  
             contain the matrix  B,  and  on exit  is overwritten  by the 
  
             transformed matrix.   

    LDB    - INTEGER.   
             On entry, LDB specifies the first dimension of B as declared 
  
             in  the  calling  (sub)  program.   LDB  must  be  at  least 
  
             max( 1, m ).   
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

    lside = lsame_(side, "L");
    if (lside) {
	nrowa = *m;
    } else {
	nrowa = *n;
    }
    nounit = lsame_(diag, "N");
    upper = lsame_(uplo, "U");

    info = 0;
    if (! lside && ! lsame_(side, "R")) {
	info = 1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	info = 2;
    } else if (! lsame_(transa, "N") && ! lsame_(transa, "T") 
	    && ! lsame_(transa, "C")) {
	info = 3;
    } else if (! lsame_(diag, "U") && ! lsame_(diag, "N")) {
	info = 4;
    } else if (*m < 0) {
	info = 5;
    } else if (*n < 0) {
	info = 6;
    } else if (*lda < max(1,nrowa)) {
	info = 9;
    } else if (*ldb < max(1,*m)) {
	info = 11;
    }
    if (info != 0) {
	xerbla_("STRMM ", &info);
	return 0;
    }

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    }

/*     And when  alpha.eq.zero. */

    if (*alpha == 0.f) {
	i__1 = *n;
	for (j = 1; j <= *n; ++j) {
	    i__2 = *m;
	    for (i = 1; i <= *m; ++i) {
		B(i,j) = 0.f;
/* L10: */
	    }
/* L20: */
	}
	return 0;
    }

/*     Start the operations. */

    if (lside) {
	if (lsame_(transa, "N")) {

/*           Form  B := alpha*A*B. */

	    if (upper) {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    i__2 = *m;
		    for (k = 1; k <= *m; ++k) {
			if (B(k,j) != 0.f) {
			    temp = *alpha * B(k,j);
			    i__3 = k - 1;
			    for (i = 1; i <= k-1; ++i) {
				B(i,j) += temp * A(i,k);
/* L30: */
			    }
			    if (nounit) {
				temp *= A(k,k);
			    }
			    B(k,j) = temp;
			}
/* L40: */
		    }
/* L50: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    for (k = *m; k >= 1; --k) {
			if (B(k,j) != 0.f) {
			    temp = *alpha * B(k,j);
			    B(k,j) = temp;
			    if (nounit) {
				B(k,j) *= A(k,k);
			    }
			    i__2 = *m;
			    for (i = k + 1; i <= *m; ++i) {
				B(i,j) += temp * A(i,k);
/* L60: */
			    }
			}
/* L70: */
		    }
/* L80: */
		}
	    }
	} else {

/*           Form  B := alpha*B*A'. */

	    if (upper) {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    for (i = *m; i >= 1; --i) {
			temp = B(i,j);
			if (nounit) {
			    temp *= A(i,i);
			}
			i__2 = i - 1;
			for (k = 1; k <= i-1; ++k) {
			    temp += A(k,i) * B(k,j);
/* L90: */
			}
			B(i,j) = *alpha * temp;
/* L100: */
		    }
/* L110: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			temp = B(i,j);
			if (nounit) {
			    temp *= A(i,i);
			}
			i__3 = *m;
			for (k = i + 1; k <= *m; ++k) {
			    temp += A(k,i) * B(k,j);
/* L120: */
			}
			B(i,j) = *alpha * temp;
/* L130: */
		    }
/* L140: */
		}
	    }
	}
    } else {
	if (lsame_(transa, "N")) {

/*           Form  B := alpha*B*A. */

	    if (upper) {
		for (j = *n; j >= 1; --j) {
		    temp = *alpha;
		    if (nounit) {
			temp *= A(j,j);
		    }
		    i__1 = *m;
		    for (i = 1; i <= *m; ++i) {
			B(i,j) = temp * B(i,j);
/* L150: */
		    }
		    i__1 = j - 1;
		    for (k = 1; k <= j-1; ++k) {
			if (A(k,j) != 0.f) {
			    temp = *alpha * A(k,j);
			    i__2 = *m;
			    for (i = 1; i <= *m; ++i) {
				B(i,j) += temp * B(i,k);
/* L160: */
			    }
			}
/* L170: */
		    }
/* L180: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= *n; ++j) {
		    temp = *alpha;
		    if (nounit) {
			temp *= A(j,j);
		    }
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			B(i,j) = temp * B(i,j);
/* L190: */
		    }
		    i__2 = *n;
		    for (k = j + 1; k <= *n; ++k) {
			if (A(k,j) != 0.f) {
			    temp = *alpha * A(k,j);
			    i__3 = *m;
			    for (i = 1; i <= *m; ++i) {
				B(i,j) += temp * B(i,k);
/* L200: */
			    }
			}
/* L210: */
		    }
/* L220: */
		}
	    }
	} else {

/*           Form  B := alpha*B*A'. */

	    if (upper) {
		i__1 = *n;
		for (k = 1; k <= *n; ++k) {
		    i__2 = k - 1;
		    for (j = 1; j <= k-1; ++j) {
			if (A(j,k) != 0.f) {
			    temp = *alpha * A(j,k);
			    i__3 = *m;
			    for (i = 1; i <= *m; ++i) {
				B(i,j) += temp * B(i,k);
/* L230: */
			    }
			}
/* L240: */
		    }
		    temp = *alpha;
		    if (nounit) {
			temp *= A(k,k);
		    }
		    if (temp != 1.f) {
			i__2 = *m;
			for (i = 1; i <= *m; ++i) {
			    B(i,k) = temp * B(i,k);
/* L250: */
			}
		    }
/* L260: */
		}
	    } else {
		for (k = *n; k >= 1; --k) {
		    i__1 = *n;
		    for (j = k + 1; j <= *n; ++j) {
			if (A(j,k) != 0.f) {
			    temp = *alpha * A(j,k);
			    i__2 = *m;
			    for (i = 1; i <= *m; ++i) {
				B(i,j) += temp * B(i,k);
/* L270: */
			    }
			}
/* L280: */
		    }
		    temp = *alpha;
		    if (nounit) {
			temp *= A(k,k);
		    }
		    if (temp != 1.f) {
			i__1 = *m;
			for (i = 1; i <= *m; ++i) {
			    B(i,k) = temp * B(i,k);
/* L290: */
			}
		    }
/* L300: */
		}
	    }
	}
    }

    return 0;

/*     End of STRMM . */

} /* strmm_ */

