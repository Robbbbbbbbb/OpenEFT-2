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

/* Subroutine */ int slamc4_(int *emin, real *start, int *base)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMC4 is a service routine for SLAMC2.   

    Arguments   
    =========   

    EMIN    (output) EMIN   
            The minimum exponent before (gradual) underflow, computed by 
  
            setting A = START and dividing by BASE until the previous A   
            can not be recovered.   

    START   (input) REAL   
            The starting point for determining EMIN.   

    BASE    (input) INTEGER   
            The base of the machine.   

   ===================================================================== 
*/
    /* System generated locals */
    int i__1;
    real r__1;
    /* Local variables */
    static real zero, a;
    static int i;
    static real rbase, b1, b2, c1, c2, d1, d2;
    extern doublereal slamc3_(real *, real *);
    static real one;



    a = *start;
    one = 1.f;
    rbase = one / *base;
    zero = 0.f;
    *emin = 1;
    r__1 = a * rbase;
    b1 = slamc3_(&r__1, &zero);
    c1 = a;
    c2 = a;
    d1 = a;
    d2 = a;
/* +    WHILE( ( C1.EQ.A ).AND.( C2.EQ.A ).AND.   
      $       ( D1.EQ.A ).AND.( D2.EQ.A )      )LOOP */
L10:
    if (c1 == a && c2 == a && d1 == a && d2 == a) {
	--(*emin);
	a = b1;
	r__1 = a / *base;
	b1 = slamc3_(&r__1, &zero);
	r__1 = b1 * *base;
	c1 = slamc3_(&r__1, &zero);
	d1 = zero;
	i__1 = *base;
	for (i = 1; i <= *base; ++i) {
	    d1 += b1;
/* L20: */
	}
	r__1 = a * rbase;
	b2 = slamc3_(&r__1, &zero);
	r__1 = b2 / rbase;
	c2 = slamc3_(&r__1, &zero);
	d2 = zero;
	i__1 = *base;
	for (i = 1; i <= *base; ++i) {
	    d2 += b2;
/* L30: */
	}
	goto L10;
    }
/* +    END WHILE */

    return 0;

/*     End of SLAMC4 */

} /* slamc4_ */
