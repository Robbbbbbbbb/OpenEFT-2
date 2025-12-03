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

/* Subroutine */ int slassq_(int *n, real *x, int *incx, real *scale, 
	real *sumsq)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLASSQ  returns the values  scl  and  smsq  such that   

       ( scl**2 )*smsq = x( 1 )**2 +...+ x( n )**2 + ( scale**2 )*sumsq, 
  

    where  x( i ) = X( 1 + ( i - 1 )*INCX ). The value of  sumsq  is   
    assumed to be non-negative and  scl  returns the value   

       scl = max( scale, abs( x( i ) ) ).   

    scale and sumsq must be supplied in SCALE and SUMSQ and   
    scl and smsq are overwritten on SCALE and SUMSQ respectively.   

    The routine makes only one pass through the vector x.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The number of elements to be used from the vector X.   

    X       (input) REAL   
            The vector for which a scaled sum of squares is computed.   
               x( i )  = X( 1 + ( i - 1 )*INCX ), 1 <= i <= n.   

    INCX    (input) INTEGER   
            The increment between successive values of the vector X.   
            INCX > 0.   

    SCALE   (input/output) REAL   
            On entry, the value  scale  in the equation above.   
            On exit, SCALE is overwritten with  scl , the scaling factor 
  
            for the sum of squares.   

    SUMSQ   (input/output) REAL   
            On entry, the value  sumsq  in the equation above.   
            On exit, SUMSQ is overwritten with  smsq , the basic sum of   
            squares from which  scl  has been factored out.   

   ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
    int i__1, i__2;
    real r__1;
    /* Local variables */
    static real absxi;
    static int ix;


#define X(I) x[(I)-1]


    if (*n > 0) {
	i__1 = (*n - 1) * *incx + 1;
	i__2 = *incx;
	for (ix = 1; *incx < 0 ? ix >= (*n-1)**incx+1 : ix <= (*n-1)**incx+1; ix += *incx) {
	    if (X(ix) != 0.f) {
		absxi = (r__1 = X(ix), dabs(r__1));
		if (*scale < absxi) {
/* Computing 2nd power */
		    r__1 = *scale / absxi;
		    *sumsq = *sumsq * (r__1 * r__1) + 1;
		    *scale = absxi;
		} else {
/* Computing 2nd power */
		    r__1 = absxi / *scale;
		    *sumsq += r__1 * r__1;
		}
	    }
/* L10: */
	}
    }
    return 0;

/*     End of SLASSQ */

} /* slassq_ */
