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

/* Subroutine */ int ssterf_(int *n, real *d, real *e, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SSTERF computes all eigenvalues of a symmetric tridiagonal matrix   
    using the Pal-Walker-Kahan variant of the QL or QR algorithm.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The order of the matrix.  N >= 0.   

    D       (input/output) REAL array, dimension (N)   
            On entry, the n diagonal elements of the tridiagonal matrix. 
  
            On exit, if INFO = 0, the eigenvalues in ascending order.   

    E       (input/output) REAL array, dimension (N-1)   
            On entry, the (n-1) subdiagonal elements of the tridiagonal   
            matrix.   
            On exit, E has been destroyed.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
            > 0:  the algorithm failed to find all of the eigenvalues in 
  
                  a total of 30*N iterations; if INFO = i, then i   
                  elements of E have not converged to zero.   

    ===================================================================== 
  


       Test the input parameters.   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__0 = 0;
    static int c__1 = 1;
    static real c_b32 = 1.f;
    
    /* System generated locals */
    int i__1;
    real r__1, r__2;
    /* Builtin functions */
    double sqrt(doublereal), r_sign(real *, real *);
    /* Local variables */
    static real oldc;
    static int lend, jtot;
    extern /* Subroutine */ int slae2_(real *, real *, real *, real *, real *)
	    ;
    static real c;
    static int i, l, m;
    static real p, gamma, r, s, alpha, sigma, anorm;
    static int l1, lendm1, lendp1;
    static real bb;
    extern doublereal slapy2_(real *, real *);
    static int iscale;
    static real oldgam;
    extern doublereal slamch_(char *);
    static real safmin;
    extern /* Subroutine */ int xerbla_(char *, int *);
    static real safmax;
    extern /* Subroutine */ int slascl_(char *, int *, int *, real *, 
	    real *, int *, int *, real *, int *, int *);
    static int lendsv;
    static real ssfmin;
    static int nmaxit;
    static real ssfmax;
    extern doublereal slanst_(char *, int *, real *, real *);
    extern /* Subroutine */ int slasrt_(char *, int *, real *, int *);
    static int lm1, mm1, nm1;
    static real rt1, rt2, eps, rte;
    static int lsv;
    static real tst, eps2;



#define E(I) e[(I)-1]
#define D(I) d[(I)-1]


    *info = 0;

/*     Quick return if possible */

    if (*n < 0) {
	*info = -1;
	i__1 = -(*info);
	xerbla_("SSTERF", &i__1);
	return 0;
    }
    if (*n <= 1) {
	return 0;
    }

/*     Determine the unit roundoff for this environment. */

    eps = slamch_("E");
/* Computing 2nd power */
    r__1 = eps;
    eps2 = r__1 * r__1;
    safmin = slamch_("S");
    safmax = 1.f / safmin;
    ssfmax = sqrt(safmax) / 3.f;
    ssfmin = sqrt(safmin) / eps2;

/*     Compute the eigenvalues of the tridiagonal matrix. */

    nmaxit = *n * 30;
    sigma = 0.f;
    jtot = 0;

/*     Determine where the matrix splits and choose QL or QR iteration   
       for each block, according to whether top or bottom diagonal   
       element is smaller. */

    l1 = 1;
    nm1 = *n - 1;

L10:
    if (l1 > *n) {
	goto L170;
    }
    if (l1 > 1) {
	E(l1 - 1) = 0.f;
    }
    if (l1 <= nm1) {
	i__1 = nm1;
	for (m = l1; m <= nm1; ++m) {
	    tst = (r__1 = E(m), dabs(r__1));
	    if (tst == 0.f) {
		goto L30;
	    }
	    if (tst <= sqrt((r__1 = D(m), dabs(r__1))) * sqrt((r__2 = D(m + 1)
		    , dabs(r__2))) * eps) {
		E(m) = 0.f;
		goto L30;
	    }
/* L20: */
	}
    }
    m = *n;

L30:
    l = l1;
    lsv = l;
    lend = m;
    lendsv = lend;
    l1 = m + 1;
    if (lend == l) {
	goto L10;
    }

/*     Scale submatrix in rows and columns L to LEND */

    i__1 = lend - l + 1;
    anorm = slanst_("I", &i__1, &D(l), &E(l));
    iscale = 0;
    if (anorm > ssfmax) {
	iscale = 1;
	i__1 = lend - l + 1;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &D(l), n, 
		info);
	i__1 = lend - l;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &E(l), n, 
		info);
    } else if (anorm < ssfmin) {
	iscale = 2;
	i__1 = lend - l + 1;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &D(l), n, 
		info);
	i__1 = lend - l;
	slascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &E(l), n, 
		info);
    }

    i__1 = lend - 1;
    for (i = l; i <= lend-1; ++i) {
/* Computing 2nd power */
	r__1 = E(i);
	E(i) = r__1 * r__1;
/* L40: */
    }

/*     Choose between QL and QR iteration */

    if ((r__1 = D(lend), dabs(r__1)) < (r__2 = D(l), dabs(r__2))) {
	lend = lsv;
	l = lendsv;
    }

    if (lend >= l) {

/*        QL Iteration   

          Look for small subdiagonal element. */

L50:
	if (l != lend) {
	    lendm1 = lend - 1;
	    i__1 = lendm1;
	    for (m = l; m <= lendm1; ++m) {
		tst = (r__1 = E(m), dabs(r__1));
		if (tst <= eps2 * (r__1 = D(m) * D(m + 1), dabs(r__1))) {
		    goto L70;
		}
/* L60: */
	    }
	}

	m = lend;

L70:
	if (m < lend) {
	    E(m) = 0.f;
	}
	p = D(l);
	if (m == l) {
	    goto L90;
	}

/*        If remaining matrix is 2 by 2, use SLAE2 to compute its   
          eigenvalues. */

	if (m == l + 1) {
	    rte = sqrt(E(l));
	    slae2_(&D(l), &rte, &D(l + 1), &rt1, &rt2);
	    D(l) = rt1;
	    D(l + 1) = rt2;
	    E(l) = 0.f;
	    l += 2;
	    if (l <= lend) {
		goto L50;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(E(l));
	sigma = (D(l + 1) - p) / (rte * 2.f);
	r = slapy2_(&sigma, &c_b32);
	sigma = p - rte / (sigma + r_sign(&r, &sigma));

	c = 1.f;
	s = 0.f;
	gamma = D(m) - sigma;
	p = gamma * gamma;

/*        Inner loop */

	mm1 = m - 1;
	i__1 = l;
	for (i = mm1; i >= l; --i) {
	    bb = E(i);
	    r = p + bb;
	    if (i != m - 1) {
		E(i + 1) = s * r;
	    }
	    oldc = c;
	    c = p / r;
	    s = bb / r;
	    oldgam = gamma;
	    alpha = D(i);
	    gamma = c * (alpha - sigma) - s * oldgam;
	    D(i + 1) = oldgam + (alpha - gamma);
	    if (c != 0.f) {
		p = gamma * gamma / c;
	    } else {
		p = oldc * bb;
	    }
/* L80: */
	}

	E(l) = s * p;
	D(l) = sigma + gamma;
	goto L50;

/*        Eigenvalue found. */

L90:
	D(l) = p;

	++l;
	if (l <= lend) {
	    goto L50;
	}
	goto L150;

    } else {

/*        QR Iteration   

          Look for small superdiagonal element. */

L100:
	if (l != lend) {
	    lendp1 = lend + 1;
	    i__1 = lendp1;
	    for (m = l; m >= lendp1; --m) {
		tst = (r__1 = E(m - 1), dabs(r__1));
		if (tst <= eps2 * (r__1 = D(m) * D(m - 1), dabs(r__1))) {
		    goto L120;
		}
/* L110: */
	    }
	}

	m = lend;

L120:
	if (m > lend) {
	    E(m - 1) = 0.f;
	}
	p = D(l);
	if (m == l) {
	    goto L140;
	}

/*        If remaining matrix is 2 by 2, use SLAE2 to compute its   
          eigenvalues. */

	if (m == l - 1) {
	    rte = sqrt(E(l - 1));
	    slae2_(&D(l), &rte, &D(l - 1), &rt1, &rt2);
	    D(l) = rt1;
	    D(l - 1) = rt2;
	    E(l - 1) = 0.f;
	    l += -2;
	    if (l >= lend) {
		goto L100;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(E(l - 1));
	sigma = (D(l - 1) - p) / (rte * 2.f);
	r = slapy2_(&sigma, &c_b32);
	sigma = p - rte / (sigma + r_sign(&r, &sigma));

	c = 1.f;
	s = 0.f;
	gamma = D(m) - sigma;
	p = gamma * gamma;

/*        Inner loop */

	lm1 = l - 1;
	i__1 = lm1;
	for (i = m; i <= lm1; ++i) {
	    bb = E(i);
	    r = p + bb;
	    if (i != m) {
		E(i - 1) = s * r;
	    }
	    oldc = c;
	    c = p / r;
	    s = bb / r;
	    oldgam = gamma;
	    alpha = D(i + 1);
	    gamma = c * (alpha - sigma) - s * oldgam;
	    D(i) = oldgam + (alpha - gamma);
	    if (c != 0.f) {
		p = gamma * gamma / c;
	    } else {
		p = oldc * bb;
	    }
/* L130: */
	}

	E(lm1) = s * p;
	D(l) = sigma + gamma;
	goto L100;

/*        Eigenvalue found. */

L140:
	D(l) = p;

	--l;
	if (l >= lend) {
	    goto L100;
	}
	goto L150;

    }

/*     Undo scaling if necessary */

L150:
    if (iscale == 1) {
	i__1 = lendsv - lsv + 1;
	slascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &D(lsv), n, 
		info);
    }
    if (iscale == 2) {
	i__1 = lendsv - lsv + 1;
	slascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &D(lsv), n, 
		info);
    }

/*     Check for no convergence to an eigenvalue after a total   
       of N*MAXIT iterations. */

    if (jtot == nmaxit) {
	i__1 = *n - 1;
	for (i = 1; i <= *n-1; ++i) {
	    if (E(i) != 0.f) {
		++(*info);
	    }
/* L160: */
	}
	return 0;
    }
    goto L10;

/*     Sort eigenvalues in increasing order. */

L170:
    slasrt_("I", n, &D(1), info);

    return 0;

/*     End of SSTERF */

} /* ssterf_ */
