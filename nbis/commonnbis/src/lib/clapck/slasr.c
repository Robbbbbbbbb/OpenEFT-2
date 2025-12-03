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

/* Subroutine */ int slasr_(char *side, char *pivot, char *direct, int *m,
	 int *n, real *c, real *s, real *a, int *lda)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLASR   performs the transformation   

       A := P*A,   when SIDE = 'L' or 'l'  (  Left-hand side )   

       A := A*P',  when SIDE = 'R' or 'r'  ( Right-hand side )   

    where A is an m by n real matrix and P is an orthogonal matrix,   
    consisting of a sequence of plane rotations determined by the   
    parameters PIVOT and DIRECT as follows ( z = m when SIDE = 'L' or 'l' 
  
    and z = n when SIDE = 'R' or 'r' ):   

    When  DIRECT = 'F' or 'f'  ( Forward sequence ) then   

       P = P( z - 1 )*...*P( 2 )*P( 1 ),   

    and when DIRECT = 'B' or 'b'  ( Backward sequence ) then   

       P = P( 1 )*P( 2 )*...*P( z - 1 ),   

    where  P( k ) is a plane rotation matrix for the following planes:   

       when  PIVOT = 'V' or 'v'  ( Variable pivot ),   
          the plane ( k, k + 1 )   

       when  PIVOT = 'T' or 't'  ( Top pivot ),   
          the plane ( 1, k + 1 )   

       when  PIVOT = 'B' or 'b'  ( Bottom pivot ),   
          the plane ( k, z )   

    c( k ) and s( k )  must contain the  cosine and sine that define the 
  
    matrix  P( k ).  The two by two plane rotation part of the matrix   
    P( k ), R( k ), is assumed to be of the form   

       R( k ) = (  c( k )  s( k ) ).   
                ( -s( k )  c( k ) )   

    This version vectorises across rows of the array A when SIDE = 'L'.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            Specifies whether the plane rotation matrix P is applied to   
            A on the left or the right.   
            = 'L':  Left, compute A := P*A   
            = 'R':  Right, compute A:= A*P'   

    DIRECT  (input) CHARACTER*1   
            Specifies whether P is a forward or backward sequence of   
            plane rotations.   
            = 'F':  Forward, P = P( z - 1 )*...*P( 2 )*P( 1 )   
            = 'B':  Backward, P = P( 1 )*P( 2 )*...*P( z - 1 )   

    PIVOT   (input) CHARACTER*1   
            Specifies the plane for which P(k) is a plane rotation   
            matrix.   
            = 'V':  Variable pivot, the plane (k,k+1)   
            = 'T':  Top pivot, the plane (1,k+1)   
            = 'B':  Bottom pivot, the plane (k,z)   

    M       (input) INTEGER   
            The number of rows of the matrix A.  If m <= 1, an immediate 
  
            return is effected.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  If n <= 1, an   
            immediate return is effected.   

    C, S    (input) REAL arrays, dimension   
                    (M-1) if SIDE = 'L'   
                    (N-1) if SIDE = 'R'   
            c(k) and s(k) contain the cosine and sine that define the   
            matrix P(k).  The two by two plane rotation part of the   
            matrix P(k), R(k), is assumed to be of the form   
            R( k ) = (  c( k )  s( k ) ).   
                     ( -s( k )  c( k ) )   

    A       (input/output) REAL array, dimension (LDA,N)   
            The m by n matrix A.  On exit, A is overwritten by P*A if   
            SIDE = 'R' or by A*P' if SIDE = 'L'.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    ===================================================================== 
  


       Test the input parameters   

    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int a_dim1, a_offset;
*/
    int i__1, i__2;
    /* Local variables */
    static int info;
    static real temp;
    static int i, j;
    extern logical lsame_(char *, char *);
    static real ctemp, stemp;
    extern /* Subroutine */ int xerbla_(char *, int *);


#define C(I) c[(I)-1]
#define S(I) s[(I)-1]

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]

    info = 0;
    if (! (lsame_(side, "L") || lsame_(side, "R"))) {
	info = 1;
    } else if (! (lsame_(pivot, "V") || lsame_(pivot, "T") || 
	    lsame_(pivot, "B"))) {
	info = 2;
    } else if (! (lsame_(direct, "F") || lsame_(direct, "B")))
	     {
	info = 3;
    } else if (*m < 0) {
	info = 4;
    } else if (*n < 0) {
	info = 5;
    } else if (*lda < max(1,*m)) {
	info = 9;
    }
    if (info != 0) {
	xerbla_("SLASR ", &info);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }
    if (lsame_(side, "L")) {

/*        Form  P * A */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= *m-1; ++j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *n;
			for (i = 1; i <= *n; ++i) {
			    temp = A(j+1,i);
			    A(j+1,i) = ctemp * temp - stemp * A(j,i);
			    A(j,i) = stemp * temp + ctemp * A(j,i);
/* L10: */
			}
		    }
/* L20: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *n;
			for (i = 1; i <= *n; ++i) {
			    temp = A(j+1,i);
			    A(j+1,i) = ctemp * temp - stemp * A(j,i);
			    A(j,i) = stemp * temp + ctemp * A(j,i);
/* L30: */
			}
		    }
/* L40: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m;
		for (j = 2; j <= *m; ++j) {
		    ctemp = C(j - 1);
		    stemp = S(j - 1);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *n;
			for (i = 1; i <= *n; ++i) {
			    temp = A(j,i);
			    A(j,i) = ctemp * temp - stemp * A(1,i);
			    A(1,i) = stemp * temp + ctemp * A(1,i);
/* L50: */
			}
		    }
/* L60: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m; j >= 2; --j) {
		    ctemp = C(j - 1);
		    stemp = S(j - 1);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *n;
			for (i = 1; i <= *n; ++i) {
			    temp = A(j,i);
			    A(j,i) = ctemp * temp - stemp * A(1,i);
			    A(1,i) = stemp * temp + ctemp * A(1,i);
/* L70: */
			}
		    }
/* L80: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= *m-1; ++j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *n;
			for (i = 1; i <= *n; ++i) {
			    temp = A(j,i);
			    A(j,i) = stemp * A(*m,i) + 
				    ctemp * temp;
			    A(*m,i) = ctemp * A(*m,i) - 
				    stemp * temp;
/* L90: */
			}
		    }
/* L100: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *n;
			for (i = 1; i <= *n; ++i) {
			    temp = A(j,i);
			    A(j,i) = stemp * A(*m,i) + 
				    ctemp * temp;
			    A(*m,i) = ctemp * A(*m,i) - 
				    stemp * temp;
/* L110: */
			}
		    }
/* L120: */
		}
	    }
	}
    } else if (lsame_(side, "R")) {

/*        Form A * P' */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= *n-1; ++j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *m;
			for (i = 1; i <= *m; ++i) {
			    temp = A(i,j+1);
			    A(i,j+1) = ctemp * temp - stemp * 
				    A(i,j);
			    A(i,j) = stemp * temp + ctemp * A(i,j);
/* L130: */
			}
		    }
/* L140: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *m;
			for (i = 1; i <= *m; ++i) {
			    temp = A(i,j+1);
			    A(i,j+1) = ctemp * temp - stemp * 
				    A(i,j);
			    A(i,j) = stemp * temp + ctemp * A(i,j);
/* L150: */
			}
		    }
/* L160: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n;
		for (j = 2; j <= *n; ++j) {
		    ctemp = C(j - 1);
		    stemp = S(j - 1);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *m;
			for (i = 1; i <= *m; ++i) {
			    temp = A(i,j);
			    A(i,j) = ctemp * temp - stemp * A(i,1);
			    A(i,1) = stemp * temp + ctemp * A(i,1);
/* L170: */
			}
		    }
/* L180: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n; j >= 2; --j) {
		    ctemp = C(j - 1);
		    stemp = S(j - 1);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *m;
			for (i = 1; i <= *m; ++i) {
			    temp = A(i,j);
			    A(i,j) = ctemp * temp - stemp * A(i,1);
			    A(i,1) = stemp * temp + ctemp * A(i,1);
/* L190: */
			}
		    }
/* L200: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= *n-1; ++j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__2 = *m;
			for (i = 1; i <= *m; ++i) {
			    temp = A(i,j);
			    A(i,j) = stemp * A(i,*n) + 
				    ctemp * temp;
			    A(i,*n) = ctemp * A(i,*n) - 
				    stemp * temp;
/* L210: */
			}
		    }
/* L220: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = C(j);
		    stemp = S(j);
		    if (ctemp != 1.f || stemp != 0.f) {
			i__1 = *m;
			for (i = 1; i <= *m; ++i) {
			    temp = A(i,j);
			    A(i,j) = stemp * A(i,*n) + 
				    ctemp * temp;
			    A(i,*n) = ctemp * A(i,*n) - 
				    stemp * temp;
/* L230: */
			}
		    }
/* L240: */
		}
	    }
	}
    }

    return 0;

/*     End of SLASR */

} /* slasr_ */
