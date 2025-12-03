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
* Source for module SGEMM.C from package CBLAS.
* Retrieved from NETLIB on Tue Mar 14 10:54:01 2000.
*
* UPDATED: 03/09/2005 by MDG
* ======================================================================
*/

/*  -- translated by f2c (version 19940927).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include <f2c.h>

/* Subroutine */ int sgemm_(char *transa, char *transb, int *m, int *
	n, int *k, real *alpha, real *a, int *lda, real *b, int *
	ldb, real *beta, real *c, int *ldc)
{


    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset;
*/
    int i__1, i__2, i__3;

    /* Local variables */
    static int info;
    static logical nota, notb;
    static real temp;
    static int i, j, l, ncola;
    extern logical lsame_(char *, char *);
    static int nrowa, nrowb;
    extern /* Subroutine */ int xerbla_(char *, int *);


/*  Purpose   
    =======   

    SGEMM  performs one of the matrix-matrix operations   

       C := alpha*op( A )*op( B ) + beta*C,   

    where  op( X ) is one of   

       op( X ) = X   or   op( X ) = X',   

    alpha and beta are scalars, and A, B and C are matrices, with op( A ) 
  
    an m by k matrix,  op( B )  a  k by n matrix and  C an m by n matrix. 
  

    Parameters   
    ==========   

    TRANSA - CHARACTER*1.   
             On entry, TRANSA specifies the form of op( A ) to be used in 
  
             the matrix multiplication as follows:   

                TRANSA = 'N' or 'n',  op( A ) = A.   

                TRANSA = 'T' or 't',  op( A ) = A'.   

                TRANSA = 'C' or 'c',  op( A ) = A'.   

             Unchanged on exit.   

    TRANSB - CHARACTER*1.   
             On entry, TRANSB specifies the form of op( B ) to be used in 
  
             the matrix multiplication as follows:   

                TRANSB = 'N' or 'n',  op( B ) = B.   

                TRANSB = 'T' or 't',  op( B ) = B'.   

                TRANSB = 'C' or 'c',  op( B ) = B'.   

             Unchanged on exit.   

    M      - INTEGER.   
             On entry,  M  specifies  the number  of rows  of the  matrix 
  
             op( A )  and of the  matrix  C.  M  must  be at least  zero. 
  
             Unchanged on exit.   

    N      - INTEGER.   
             On entry,  N  specifies the number  of columns of the matrix 
  
             op( B ) and the number of columns of the matrix C. N must be 
  
             at least zero.   
             Unchanged on exit.   

    K      - INTEGER.   
             On entry,  K  specifies  the number of columns of the matrix 
  
             op( A ) and the number of rows of the matrix op( B ). K must 
  
             be at least  zero.   
             Unchanged on exit.   

    ALPHA  - REAL            .   
             On entry, ALPHA specifies the scalar alpha.   
             Unchanged on exit.   

    A      - REAL             array of DIMENSION ( LDA, ka ), where ka is 
  
             k  when  TRANSA = 'N' or 'n',  and is  m  otherwise.   
             Before entry with  TRANSA = 'N' or 'n',  the leading  m by k 
  
             part of the array  A  must contain the matrix  A,  otherwise 
  
             the leading  k by m  part of the array  A  must contain  the 
  
             matrix A.   
             Unchanged on exit.   

    LDA    - INTEGER.   
             On entry, LDA specifies the first dimension of A as declared 
  
             in the calling (sub) program. When  TRANSA = 'N' or 'n' then 
  
             LDA must be at least  max( 1, m ), otherwise  LDA must be at 
  
             least  max( 1, k ).   
             Unchanged on exit.   

    B      - REAL             array of DIMENSION ( LDB, kb ), where kb is 
  
             n  when  TRANSB = 'N' or 'n',  and is  k  otherwise.   
             Before entry with  TRANSB = 'N' or 'n',  the leading  k by n 
  
             part of the array  B  must contain the matrix  B,  otherwise 
  
             the leading  n by k  part of the array  B  must contain  the 
  
             matrix B.   
             Unchanged on exit.   

    LDB    - INTEGER.   
             On entry, LDB specifies the first dimension of B as declared 
  
             in the calling (sub) program. When  TRANSB = 'N' or 'n' then 
  
             LDB must be at least  max( 1, k ), otherwise  LDB must be at 
  
             least  max( 1, n ).   
             Unchanged on exit.   

    BETA   - REAL            .   
             On entry,  BETA  specifies the scalar  beta.  When  BETA  is 
  
             supplied as zero then C need not be set on input.   
             Unchanged on exit.   

    C      - REAL             array of DIMENSION ( LDC, n ).   
             Before entry, the leading  m by n  part of the array  C must 
  
             contain the matrix  C,  except when  beta  is zero, in which 
  
             case C need not be set on entry.   
             On exit, the array  C  is overwritten by the  m by n  matrix 
  
             ( alpha*op( A )*op( B ) + beta*C ).   

    LDC    - INTEGER.   
             On entry, LDC specifies the first dimension of C as declared 
  
             in  the  calling  (sub)  program.   LDC  must  be  at  least 
  
             max( 1, m ).   
             Unchanged on exit.   


    Level 3 Blas routine.   

    -- Written on 8-February-1989.   
       Jack Dongarra, Argonne National Laboratory.   
       Iain Duff, AERE Harwell.   
       Jeremy Du Croz, Numerical Algorithms Group Ltd.   
       Sven Hammarling, Numerical Algorithms Group Ltd.   



       Set  NOTA  and  NOTB  as  true if  A  and  B  respectively are not 
  
       transposed and set  NROWA, NCOLA and  NROWB  as the number of rows 
  
       and  columns of  A  and the  number of  rows  of  B  respectively. 
  

    
   Parameter adjustments   
       Function Body */

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]
#define B(I,J) b[(I)-1 + ((J)-1)* ( *ldb)]
#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]

    nota = lsame_(transa, "N");
    notb = lsame_(transb, "N");
    if (nota) {
	nrowa = *m;
	ncola = *k;
    } else {
	nrowa = *k;
	ncola = *m;
    }
    if (notb) {
	nrowb = *k;
    } else {
	nrowb = *n;
    }

/*     Test the input parameters. */

    info = 0;
    if (! nota && ! lsame_(transa, "C") && ! lsame_(transa, "T")) {
	info = 1;
    } else if (! notb && ! lsame_(transb, "C") && ! lsame_(transb, 
	    "T")) {
	info = 2;
    } else if (*m < 0) {
	info = 3;
    } else if (*n < 0) {
	info = 4;
    } else if (*k < 0) {
	info = 5;
    } else if (*lda < max(1,nrowa)) {
	info = 8;
    } else if (*ldb < max(1,nrowb)) {
	info = 10;
    } else if (*ldc < max(1,*m)) {
	info = 13;
    }
    if (info != 0) {
	xerbla_("SGEMM ", &info);
	return 0;
    }

/*     Quick return if possible. */

/*
    if (*m == 0 || *n == 0 || (*alpha == 0.f || *k == 0) && *beta == 1.f) {
*/
/*  Parenteses added by MDG on 03/09/2005 */
    if (*m == 0 || *n == 0 || ((*alpha == 0.f || *k == 0) && *beta == 1.f)) {
	return 0;
    }

/*     And if  alpha.eq.zero. */

    if (*alpha == 0.f) {
	if (*beta == 0.f) {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = *m;
		for (i = 1; i <= *m; ++i) {
		    C(i,j) = 0.f;
/* L10: */
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = *m;
		for (i = 1; i <= *m; ++i) {
		    C(i,j) = *beta * C(i,j);
/* L30: */
		}
/* L40: */
	    }
	}
	return 0;
    }

/*     Start the operations. */

    if (notb) {
	if (nota) {

/*           Form  C := alpha*A*B + beta*C. */

	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		if (*beta == 0.f) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,j) = 0.f;
/* L50: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,j) = *beta * C(i,j);
/* L60: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= *k; ++l) {
		    if (B(l,j) != 0.f) {
			temp = *alpha * B(l,j);
			i__3 = *m;
			for (i = 1; i <= *m; ++i) {
			    C(i,j) += temp * A(i,l);
/* L70: */
			}
		    }
/* L80: */
		}
/* L90: */
	    }
	} else {

/*           Form  C := alpha*A'*B + beta*C */

	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = *m;
		for (i = 1; i <= *m; ++i) {
		    temp = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= *k; ++l) {
			temp += A(l,i) * B(l,j);
/* L100: */
		    }
		    if (*beta == 0.f) {
			C(i,j) = *alpha * temp;
		    } else {
			C(i,j) = *alpha * temp + *beta * C(i,j);
		    }
/* L110: */
		}
/* L120: */
	    }
	}
    } else {
	if (nota) {

/*           Form  C := alpha*A*B' + beta*C */

	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		if (*beta == 0.f) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,j) = 0.f;
/* L130: */
		    }
		} else if (*beta != 1.f) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,j) = *beta * C(i,j);
/* L140: */
		    }
		}
		i__2 = *k;
		for (l = 1; l <= *k; ++l) {
		    if (B(j,l) != 0.f) {
			temp = *alpha * B(j,l);
			i__3 = *m;
			for (i = 1; i <= *m; ++i) {
			    C(i,j) += temp * A(i,l);
/* L150: */
			}
		    }
/* L160: */
		}
/* L170: */
	    }
	} else {

/*           Form  C := alpha*A'*B' + beta*C */

	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
		i__2 = *m;
		for (i = 1; i <= *m; ++i) {
		    temp = 0.f;
		    i__3 = *k;
		    for (l = 1; l <= *k; ++l) {
			temp += A(l,i) * B(j,l);
/* L180: */
		    }
		    if (*beta == 0.f) {
			C(i,j) = *alpha * temp;
		    } else {
			C(i,j) = *alpha * temp + *beta * C(i,j);
		    }
/* L190: */
		}
/* L200: */
	    }
	}
    }

    return 0;

/*     End of SGEMM . */

} /* sgemm_ */

