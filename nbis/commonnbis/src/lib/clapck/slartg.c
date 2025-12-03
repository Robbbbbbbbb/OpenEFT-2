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

/* Subroutine */ int slartg_(real *f, real *g, real *cs, real *sn, real *r)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLARTG generate a plane rotation so that   

       [  CS  SN  ]  .  [ F ]  =  [ R ]   where CS**2 + SN**2 = 1.   
       [ -SN  CS  ]     [ G ]     [ 0 ]   

    This is a slower, more accurate version of the BLAS1 routine SROTG,   
    with the following other differences:   
       F and G are unchanged on return.   
       If G=0, then CS=1 and SN=0.   
       If F=0 and (G .ne. 0), then CS=0 and SN=1 without doing any   
          floating point operations (saves work in SBDSQR when   
          there are zeros on the diagonal).   

    If F exceeds G in magnitude, CS will be positive.   

    Arguments   
    =========   

    F       (input) REAL   
            The first component of vector to be rotated.   

    G       (input) REAL   
            The second component of vector to be rotated.   

    CS      (output) REAL   
            The cosine of the rotation.   

    SN      (output) REAL   
            The sine of the rotation.   

    R       (output) REAL   
            The nonzero component of the rotated vector.   

    ===================================================================== 
*/
    /* Initialized data */
    static logical first = TRUE_;
    /* System generated locals */
    int i__1;
    real r__1, r__2;
    /* Builtin functions */
    double log(doublereal), pow_ri(real *, int *), sqrt(doublereal);
    /* Local variables */
    static int i;
    static real scale;
    static int count;
    static real f1, g1, safmn2, safmx2;
    extern doublereal slamch_(char *);
    static real safmin, eps;



    if (first) {
	first = FALSE_;
	safmin = slamch_("S");
	eps = slamch_("E");
	r__1 = slamch_("B");
	i__1 = (int) (log(safmin / eps) / log(slamch_("B")) / 2.f);
	safmn2 = pow_ri(&r__1, &i__1);
	safmx2 = 1.f / safmn2;
    }
    if (*g == 0.f) {
	*cs = 1.f;
	*sn = 0.f;
	*r = *f;
    } else if (*f == 0.f) {
	*cs = 0.f;
	*sn = 1.f;
	*r = *g;
    } else {
	f1 = *f;
	g1 = *g;
/* Computing MAX */
	r__1 = dabs(f1), r__2 = dabs(g1);
	scale = dmax(r__1,r__2);
	if (scale >= safmx2) {
	    count = 0;
L10:
	    ++count;
	    f1 *= safmn2;
	    g1 *= safmn2;
/* Computing MAX */
	    r__1 = dabs(f1), r__2 = dabs(g1);
	    scale = dmax(r__1,r__2);
	    if (scale >= safmx2) {
		goto L10;
	    }
/* Computing 2nd power */
	    r__1 = f1;
/* Computing 2nd power */
	    r__2 = g1;
	    *r = sqrt(r__1 * r__1 + r__2 * r__2);
	    *cs = f1 / *r;
	    *sn = g1 / *r;
	    i__1 = count;
	    for (i = 1; i <= count; ++i) {
		*r *= safmx2;
/* L20: */
	    }
	} else if (scale <= safmn2) {
	    count = 0;
L30:
	    ++count;
	    f1 *= safmx2;
	    g1 *= safmx2;
/* Computing MAX */
	    r__1 = dabs(f1), r__2 = dabs(g1);
	    scale = dmax(r__1,r__2);
	    if (scale <= safmn2) {
		goto L30;
	    }
/* Computing 2nd power */
	    r__1 = f1;
/* Computing 2nd power */
	    r__2 = g1;
	    *r = sqrt(r__1 * r__1 + r__2 * r__2);
	    *cs = f1 / *r;
	    *sn = g1 / *r;
	    i__1 = count;
	    for (i = 1; i <= count; ++i) {
		*r *= safmn2;
/* L40: */
	    }
	} else {
/* Computing 2nd power */
	    r__1 = f1;
/* Computing 2nd power */
	    r__2 = g1;
	    *r = sqrt(r__1 * r__1 + r__2 * r__2);
	    *cs = f1 / *r;
	    *sn = g1 / *r;
	}
	if (dabs(*f) > dabs(*g) && *cs < 0.f) {
	    *cs = -(doublereal)(*cs);
	    *sn = -(doublereal)(*sn);
	    *r = -(doublereal)(*r);
	}
    }
    return 0;

/*     End of SLARTG */

} /* slartg_ */
