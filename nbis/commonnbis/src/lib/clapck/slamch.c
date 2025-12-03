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

doublereal slamch_(char *cmach)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMCH determines single precision machine parameters.   

    Arguments   
    =========   

    CMACH   (input) CHARACTER*1   
            Specifies the value to be returned by SLAMCH:   
            = 'E' or 'e',   SLAMCH := eps   
            = 'S' or 's ,   SLAMCH := sfmin   
            = 'B' or 'b',   SLAMCH := base   
            = 'P' or 'p',   SLAMCH := eps*base   
            = 'N' or 'n',   SLAMCH := t   
            = 'R' or 'r',   SLAMCH := rnd   
            = 'M' or 'm',   SLAMCH := emin   
            = 'U' or 'u',   SLAMCH := rmin   
            = 'L' or 'l',   SLAMCH := emax   
            = 'O' or 'o',   SLAMCH := rmax   

            where   

            eps   = relative machine precision   
            sfmin = safe minimum, such that 1/sfmin does not overflow   
            base  = base of the machine   
            prec  = eps*base   
            t     = number of (base) digits in the mantissa   
            rnd   = 1.0 when rounding occurs in addition, 0.0 otherwise   
            emin  = minimum exponent before (gradual) underflow   
            rmin  = underflow threshold - base**(emin-1)   
            emax  = largest exponent before overflow   
            rmax  = overflow threshold  - (base**emax)*(1-eps)   

   ===================================================================== 
*/
/* >>Start of File<<   
       Initialized data */
    static logical first = TRUE_;
    /* System generated locals */
    int i__1;
    real ret_val;
    /* Builtin functions */
    double pow_ri(real *, int *);
    /* Local variables */
    static real base;
    static int beta;
    static real emin, prec, emax;
    static int imin, imax;
    static logical lrnd;
    static real rmin, rmax, t, rmach;
    extern logical lsame_(char *, char *);
    static real small, sfmin;
    extern /* Subroutine */ int slamc2_(int *, int *, logical *, real 
	    *, int *, real *, int *, real *);
    static int it;
    static real rnd, eps;



    if (first) {
	first = FALSE_;
	slamc2_(&beta, &it, &lrnd, &eps, &imin, &rmin, &imax, &rmax);
	base = (real) beta;
	t = (real) it;
	if (lrnd) {
	    rnd = 1.f;
	    i__1 = 1 - it;
	    eps = pow_ri(&base, &i__1) / 2;
	} else {
	    rnd = 0.f;
	    i__1 = 1 - it;
	    eps = pow_ri(&base, &i__1);
	}
	prec = eps * base;
	emin = (real) imin;
	emax = (real) imax;
	sfmin = rmin;
	small = 1.f / rmax;
	if (small >= sfmin) {

/*           Use SMALL plus a bit, to avoid the possibility of rou
nding   
             causing overflow when computing  1/sfmin. */

	    sfmin = small * (eps + 1.f);
	}
    }

    if (lsame_(cmach, "E")) {
	rmach = eps;
    } else if (lsame_(cmach, "S")) {
	rmach = sfmin;
    } else if (lsame_(cmach, "B")) {
	rmach = base;
    } else if (lsame_(cmach, "P")) {
	rmach = prec;
    } else if (lsame_(cmach, "N")) {
	rmach = t;
    } else if (lsame_(cmach, "R")) {
	rmach = rnd;
    } else if (lsame_(cmach, "M")) {
	rmach = emin;
    } else if (lsame_(cmach, "U")) {
	rmach = rmin;
    } else if (lsame_(cmach, "L")) {
	rmach = emax;
    } else if (lsame_(cmach, "O")) {
	rmach = rmax;
    }

    ret_val = rmach;
    return ret_val;

/*     End of SLAMCH */

} /* slamch_ */
