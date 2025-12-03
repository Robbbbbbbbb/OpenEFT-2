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

/* Subroutine */ int slarnv_(int *idist, int *iseed, int *n, real 
	*x)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SLARNV returns a vector of n random real numbers from a uniform or   
    normal distribution.   

    Arguments   
    =========   

    IDIST   (input) INTEGER   
            Specifies the distribution of the random numbers:   
            = 1:  uniform (0,1)   
            = 2:  uniform (-1,1)   
            = 3:  normal (0,1)   

    ISEED   (input/output) INTEGER array, dimension (4)   
            On entry, the seed of the random number generator; the array 
  
            elements must be between 0 and 4095, and ISEED(4) must be   
            odd.   
            On exit, the seed is updated.   

    N       (input) INTEGER   
            The number of random numbers to be generated.   

    X       (output) REAL array, dimension (N)   
            The generated random numbers.   

    Further Details   
    ===============   

    This routine calls the auxiliary routine SLARUV to generate random   
    real numbers from a uniform (0,1) distribution, in batches of up to   
    128 using vectorisable code. The Box-Muller method is used to   
    transform numbers from a uniform to a normal distribution.   

    ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* System generated locals */
    int i__1, i__2, i__3;
    /* Builtin functions */
    double log(doublereal), sqrt(doublereal), cos(doublereal);
    /* Local variables */
    static int i;
    static real u[128];
    static int il, iv, il2;
    extern /* Subroutine */ int slaruv_(int *, int *, real *);


#define X(I) x[(I)-1]
#define ISEED(I) iseed[(I)-1]


    i__1 = *n;
    for (iv = 1; iv <= *n; iv += 64) {
/* Computing MIN */
	i__2 = 64, i__3 = *n - iv + 1;
	il = min(i__2,i__3);
	if (*idist == 3) {
	    il2 = il << 1;
	} else {
	    il2 = il;
	}

/*        Call SLARUV to generate IL2 numbers from a uniform (0,1)   
          distribution (IL2 <= LV) */

	slaruv_(&ISEED(1), &il2, u);

	if (*idist == 1) {

/*           Copy generated numbers */

	    i__2 = il;
	    for (i = 1; i <= il; ++i) {
		X(iv + i - 1) = u[i - 1];
/* L10: */
	    }
	} else if (*idist == 2) {

/*           Convert generated numbers to uniform (-1,1) distribut
ion */

	    i__2 = il;
	    for (i = 1; i <= il; ++i) {
		X(iv + i - 1) = u[i - 1] * 2.f - 1.f;
/* L20: */
	    }
	} else if (*idist == 3) {

/*           Convert generated numbers to normal (0,1) distributio
n */

	    i__2 = il;
	    for (i = 1; i <= il; ++i) {
		X(iv + i - 1) = sqrt(log(u[(i << 1) - 2]) * -2.f) * cos(u[(i 
			<< 1) - 1] * 6.2831853071795864769252867663f);
/* L30: */
	    }
	}
/* L40: */
    }
    return 0;

/*     End of SLARNV */

} /* slarnv_ */
