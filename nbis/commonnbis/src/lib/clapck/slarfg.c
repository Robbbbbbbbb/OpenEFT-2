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

/* Subroutine */ int slarfg_(int *n, real *alpha, real *x, int *incx, 
	real *tau)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLARFG generates a real elementary reflector H of order n, such   
    that   

          H * ( alpha ) = ( beta ),   H' * H = I.   
              (   x   )   (   0  )   

    where alpha and beta are scalars, and x is an (n-1)-element real   
    vector. H is represented in the form   

          H = I - tau * ( 1 ) * ( 1 v' ) ,   
                        ( v )   

    where tau is a real scalar and v is a real (n-1)-element   
    vector.   

    If the elements of x are all zero, then tau = 0 and H is taken to be 
  
    the unit matrix.   

    Otherwise  1 <= tau <= 2.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The order of the elementary reflector.   

    ALPHA   (input/output) REAL   
            On entry, the value alpha.   
            On exit, it is overwritten with the value beta.   

    X       (input/output) REAL array, dimension   
                           (1+(N-2)*abs(INCX))   
            On entry, the vector x.   
            On exit, it is overwritten with the vector v.   

    INCX    (input) INTEGER   
            The increment between elements of X. INCX > 0.   

    TAU     (output) REAL   
            The value tau.   

    ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
    int i__1;
    real r__1;
    /* Builtin functions */
    double r_sign(real *, real *);
    /* Local variables */
    static real beta;
    extern doublereal snrm2_(int *, real *, int *);
    static int j;
    extern /* Subroutine */ int sscal_(int *, real *, real *, int *);
    static real xnorm;
    extern doublereal slapy2_(real *, real *), slamch_(char *);
    static real safmin, rsafmn;
    static int knt;


#define X(I) x[(I)-1]


    if (*n <= 1) {
	*tau = 0.f;
	return 0;
    }

    i__1 = *n - 1;
    xnorm = snrm2_(&i__1, &X(1), incx);

    if (xnorm == 0.f) {

/*        H  =  I */

	*tau = 0.f;
    } else {

/*        general case */

	r__1 = slapy2_(alpha, &xnorm);
	beta = -(doublereal)r_sign(&r__1, alpha);
	safmin = slamch_("S") / slamch_("E");
	if (dabs(beta) < safmin) {

/*           XNORM, BETA may be inaccurate; scale X and recompute 
them */

	    rsafmn = 1.f / safmin;
	    knt = 0;
L10:
	    ++knt;
	    i__1 = *n - 1;
	    sscal_(&i__1, &rsafmn, &X(1), incx);
	    beta *= rsafmn;
	    *alpha *= rsafmn;
	    if (dabs(beta) < safmin) {
		goto L10;
	    }

/*           New BETA is at most 1, at least SAFMIN */

	    i__1 = *n - 1;
	    xnorm = snrm2_(&i__1, &X(1), incx);
	    r__1 = slapy2_(alpha, &xnorm);
	    beta = -(doublereal)r_sign(&r__1, alpha);
	    *tau = (beta - *alpha) / beta;
	    i__1 = *n - 1;
	    r__1 = 1.f / (*alpha - beta);
	    sscal_(&i__1, &r__1, &X(1), incx);

/*           If ALPHA is subnormal, it may lose relative accuracy 
*/

	    *alpha = beta;
	    i__1 = knt;
	    for (j = 1; j <= knt; ++j) {
		*alpha *= safmin;
/* L20: */
	    }
	} else {
	    *tau = (beta - *alpha) / beta;
	    i__1 = *n - 1;
	    r__1 = 1.f / (*alpha - beta);
	    sscal_(&i__1, &r__1, &X(1), incx);
	    *alpha = beta;
	}
    }

    return 0;

/*     End of SLARFG */

} /* slarfg_ */
