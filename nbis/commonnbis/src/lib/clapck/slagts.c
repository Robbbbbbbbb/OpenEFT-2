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

/* Subroutine */ int slagts_(int *job, int *n, real *a, real *b, real 
	*c, real *d, int *in, real *y, real *tol, int *info)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAGTS may be used to solve one of the systems of equations   

       (T - lambda*I)*x = y   or   (T - lambda*I)'*x = y,   

    where T is an n by n tridiagonal matrix, for x, following the   
    factorization of (T - lambda*I) as   

       (T - lambda*I) = P*L*U ,   

    by routine SLAGTF. The choice of equation to be solved is   
    controlled by the argument JOB, and in each case there is an option   
    to perturb zero or very small diagonal elements of U, this option   
    being intended for use in applications such as inverse iteration.   

    Arguments   
    =========   

    JOB     (input) INTEGER   
            Specifies the job to be performed by SLAGTS as follows:   
            =  1: The equations  (T - lambda*I)x = y  are to be solved,   
                  but diagonal elements of U are not to be perturbed.   
            = -1: The equations  (T - lambda*I)x = y  are to be solved   
                  and, if overflow would otherwise occur, the diagonal   
                  elements of U are to be perturbed. See argument TOL   
                  below.   
            =  2: The equations  (T - lambda*I)'x = y  are to be solved, 
  
                  but diagonal elements of U are not to be perturbed.   
            = -2: The equations  (T - lambda*I)'x = y  are to be solved   
                  and, if overflow would otherwise occur, the diagonal   
                  elements of U are to be perturbed. See argument TOL   
                  below.   

    N       (input) INTEGER   
            The order of the matrix T.   

    A       (input) REAL array, dimension (N)   
            On entry, A must contain the diagonal elements of U as   
            returned from SLAGTF.   

    B       (input) REAL array, dimension (N-1)   
            On entry, B must contain the first super-diagonal elements of 
  
            U as returned from SLAGTF.   

    C       (input) REAL array, dimension (N-1)   
            On entry, C must contain the sub-diagonal elements of L as   
            returned from SLAGTF.   

    D       (input) REAL array, dimension (N-2)   
            On entry, D must contain the second super-diagonal elements   
            of U as returned from SLAGTF.   

    IN      (input) INTEGER array, dimension (N)   
            On entry, IN must contain details of the matrix P as returned 
  
            from SLAGTF.   

    Y       (input/output) REAL array, dimension (N)   
            On entry, the right hand side vector y.   
            On exit, Y is overwritten by the solution vector x.   

    TOL     (input/output) REAL   
            On entry, with  JOB .lt. 0, TOL should be the minimum   
            perturbation to be made to very small diagonal elements of U. 
  
            TOL should normally be chosen as about eps*norm(U), where eps 
  
            is the relative machine precision, but if TOL is supplied as 
  
            non-positive, then it is reset to eps*max( abs( u(i,j) ) ).   
            If  JOB .gt. 0  then TOL is not referenced.   

            On exit, TOL is changed as described above, only if TOL is   
            non-positive on entry. Otherwise TOL is unchanged.   

    INFO    (output) INTEGER   
            = 0   : successful exit   
            .lt. 0: if INFO = -i, the i-th argument had an illegal value 
  
            .gt. 0: overflow would occur when computing the INFO(th)   
                    element of the solution vector x. This can only occur 
  
                    when JOB is supplied as positive and either means   
                    that a diagonal element of U is very small, or that   
                    the elements of the right-hand side vector y are very 
  
                    large.   

    ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
    int i__1;
    real r__1, r__2, r__3, r__4, r__5;
    /* Builtin functions */
    double r_sign(real *, real *);
    /* Local variables */
    static real temp, pert;
    static int k;
    static real absak, sfmin, ak;
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, int *);
    static real bignum, eps;


#define Y(I) y[(I)-1]
#define IN(I) in[(I)-1]
#define D(I) d[(I)-1]
#define C(I) c[(I)-1]
#define B(I) b[(I)-1]
#define A(I) a[(I)-1]


    *info = 0;
    if (abs(*job) > 2 || *job == 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SLAGTS", &i__1);
	return 0;
    }

    if (*n == 0) {
	return 0;
    }

    eps = slamch_("Epsilon");
    sfmin = slamch_("Safe minimum");
    bignum = 1.f / sfmin;

    if (*job < 0) {
	if (*tol <= 0.f) {
	    *tol = dabs(A(1));
	    if (*n > 1) {
/* Computing MAX */
		r__1 = *tol, r__2 = dabs(A(2)), r__1 = max(r__1,r__2), r__2 = 
			dabs(B(1));
		*tol = dmax(r__1,r__2);
	    }
	    i__1 = *n;
	    for (k = 3; k <= *n; ++k) {
/* Computing MAX */
		r__4 = *tol, r__5 = (r__1 = A(k), dabs(r__1)), r__4 = max(
			r__4,r__5), r__5 = (r__2 = B(k - 1), dabs(r__2)), 
			r__4 = max(r__4,r__5), r__5 = (r__3 = D(k - 2), dabs(
			r__3));
		*tol = dmax(r__4,r__5);
/* L10: */
	    }
	    *tol *= eps;
	    if (*tol == 0.f) {
		*tol = eps;
	    }
	}
    }

    if (abs(*job) == 1) {
	i__1 = *n;
	for (k = 2; k <= *n; ++k) {
	    if (IN(k - 1) == 0) {
		Y(k) -= C(k - 1) * Y(k - 1);
	    } else {
		temp = Y(k - 1);
		Y(k - 1) = Y(k);
		Y(k) = temp - C(k - 1) * Y(k);
	    }
/* L20: */
	}
	if (*job == 1) {
	    for (k = *n; k >= 1; --k) {
		if (k <= *n - 2) {
		    temp = Y(k) - B(k) * Y(k + 1) - D(k) * Y(k + 2);
		} else if (k == *n - 1) {
		    temp = Y(k) - B(k) * Y(k + 1);
		} else {
		    temp = Y(k);
		}
		ak = A(k);
		absak = dabs(ak);
		if (absak < 1.f) {
		    if (absak < sfmin) {
			if (absak == 0.f || dabs(temp) * sfmin > absak) {
			    *info = k;
			    return 0;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (dabs(temp) > absak * bignum) {
			*info = k;
			return 0;
		    }
		}
		Y(k) = temp / ak;
/* L30: */
	    }
	} else {
	    for (k = *n; k >= 1; --k) {
		if (k <= *n - 2) {
		    temp = Y(k) - B(k) * Y(k + 1) - D(k) * Y(k + 2);
		} else if (k == *n - 1) {
		    temp = Y(k) - B(k) * Y(k + 1);
		} else {
		    temp = Y(k);
		}
		ak = A(k);
		pert = r_sign(tol, &ak);
L40:
		absak = dabs(ak);
		if (absak < 1.f) {
		    if (absak < sfmin) {
			if (absak == 0.f || dabs(temp) * sfmin > absak) {
			    ak += pert;
			    pert *= 2;
			    goto L40;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (dabs(temp) > absak * bignum) {
			ak += pert;
			pert *= 2;
			goto L40;
		    }
		}
		Y(k) = temp / ak;
/* L50: */
	    }
	}
    } else {

/*        Come to here if  JOB = 2 or -2 */

	if (*job == 2) {
	    i__1 = *n;
	    for (k = 1; k <= *n; ++k) {
		if (k >= 3) {
		    temp = Y(k) - B(k - 1) * Y(k - 1) - D(k - 2) * Y(k - 2);
		} else if (k == 2) {
		    temp = Y(k) - B(k - 1) * Y(k - 1);
		} else {
		    temp = Y(k);
		}
		ak = A(k);
		absak = dabs(ak);
		if (absak < 1.f) {
		    if (absak < sfmin) {
			if (absak == 0.f || dabs(temp) * sfmin > absak) {
			    *info = k;
			    return 0;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (dabs(temp) > absak * bignum) {
			*info = k;
			return 0;
		    }
		}
		Y(k) = temp / ak;
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (k = 1; k <= *n; ++k) {
		if (k >= 3) {
		    temp = Y(k) - B(k - 1) * Y(k - 1) - D(k - 2) * Y(k - 2);
		} else if (k == 2) {
		    temp = Y(k) - B(k - 1) * Y(k - 1);
		} else {
		    temp = Y(k);
		}
		ak = A(k);
		pert = r_sign(tol, &ak);
L70:
		absak = dabs(ak);
		if (absak < 1.f) {
		    if (absak < sfmin) {
			if (absak == 0.f || dabs(temp) * sfmin > absak) {
			    ak += pert;
			    pert *= 2;
			    goto L70;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (dabs(temp) > absak * bignum) {
			ak += pert;
			pert *= 2;
			goto L70;
		    }
		}
		Y(k) = temp / ak;
/* L80: */
	    }
	}

	for (k = *n; k >= 2; --k) {
	    if (IN(k - 1) == 0) {
		Y(k - 1) -= C(k - 1) * Y(k);
	    } else {
		temp = Y(k - 1);
		Y(k - 1) = Y(k);
		Y(k) = temp - C(k - 1) * Y(k);
	    }
/* L90: */
	}
    }

/*     End of SLAGTS */

    return 0;
} /* slagts_ */
