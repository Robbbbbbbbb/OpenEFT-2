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

* UPDATED: 08/16/2009 by BBandini - gcc 4.4.1 won't compile math function
                         when passed a constant; one mod: sqrt(x20)
                         instead of sqrt(2.0).

* ======================================================================
*/
#include <f2c.h>

/* Subroutine */ int slae2_(real *a, real *b, real *c, real *rt1, real *rt2)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAE2  computes the eigenvalues of a 2-by-2 symmetric matrix   
       [  A   B  ]   
       [  B   C  ].   
    On return, RT1 is the eigenvalue of larger absolute value, and RT2   
    is the eigenvalue of smaller absolute value.   

    Arguments   
    =========   

    A       (input) REAL   
            The (1,1) element of the 2-by-2 matrix.   

    B       (input) REAL   
            The (1,2) and (2,1) elements of the 2-by-2 matrix.   

    C       (input) REAL   
            The (2,2) element of the 2-by-2 matrix.   

    RT1     (output) REAL   
            The eigenvalue of larger absolute value.   

    RT2     (output) REAL   
            The eigenvalue of smaller absolute value.   

    Further Details   
    ===============   

    RT1 is accurate to a few ulps barring over/underflow.   

    RT2 may be inaccurate if there is massive cancellation in the   
    determinant A*C-B*B; higher precision or correctly rounded or   
    correctly truncated arithmetic would be needed to compute RT2   
    accurately in all cases.   

    Overflow is possible only if RT1 is within a factor of 5 of overflow. 
  
    Underflow is harmless if the input data is 0 or exceeds   
       underflow_threshold / macheps.   

   ===================================================================== 
  


       Compute the eigenvalues */
    /* System generated locals */
    real r__1;
    /* Builtin functions */
    double sqrt(doublereal);
    /* Local variables */
    static real acmn, acmx, ab, df, tb, sm, rt, adf;


    sm = *a + *c;
    df = *a - *c;
    adf = dabs(df);
    tb = *b + *b;
    ab = dabs(tb);
    if (dabs(*a) > dabs(*c)) {
	acmx = *a;
	acmn = *c;
    } else {
	acmx = *c;
	acmn = *a;
    }
    if (adf > ab) {
/* Computing 2nd power */
	r__1 = ab / adf;
	rt = adf * sqrt(r__1 * r__1 + 1.f);
    } else if (adf < ab) {
/* Computing 2nd power */
	r__1 = adf / ab;
	rt = ab * sqrt(r__1 * r__1 + 1.f);
    } else {

/*        Includes case AB=ADF=0 */
        float x20 = 2.;
	rt = ab * sqrt(x20);
    }
    if (sm < 0.f) {
	*rt1 = (sm - rt) * .5f;

/*        Order of execution important.   
          To get fully accurate smaller eigenvalue,   
          next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else if (sm > 0.f) {
	*rt1 = (sm + rt) * .5f;

/*        Order of execution important.   
          To get fully accurate smaller eigenvalue,   
          next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else {

/*        Includes case RT1 = RT2 = 0 */

	*rt1 = rt * .5f;
	*rt2 = rt * -.5f;
    }
    return 0;

/*     End of SLAE2 */

} /* slae2_ */
