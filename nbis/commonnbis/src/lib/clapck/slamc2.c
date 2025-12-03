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
#include <stdio.h>
#include <f2c.h>

/* Subroutine */ int slamc2_(int *beta, int *t, logical *rnd, real *
	eps, int *emin, real *rmin, int *emax, real *rmax)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    SLAMC2 determines the machine parameters specified in its argument   
    list.   

    Arguments   
    =========   

    BETA    (output) INTEGER   
            The base of the machine.   

    T       (output) INTEGER   
            The number of ( BETA ) digits in the mantissa.   

    RND     (output) LOGICAL   
            Specifies whether proper rounding  ( RND = .TRUE. )  or   
            chopping  ( RND = .FALSE. )  occurs in addition. This may not 
  
            be a reliable guide to the way in which the machine performs 
  
            its arithmetic.   

    EPS     (output) REAL   
            The smallest positive number such that   

               fl( 1.0 - EPS ) .LT. 1.0,   

            where fl denotes the computed value.   

    EMIN    (output) INTEGER   
            The minimum exponent before (gradual) underflow occurs.   

    RMIN    (output) REAL   
            The smallest normalized number for the machine, given by   
            BASE**( EMIN - 1 ), where  BASE  is the floating point value 
  
            of BETA.   

    EMAX    (output) INTEGER   
            The maximum exponent before overflow occurs.   

    RMAX    (output) REAL   
            The largest positive number for the machine, given by   
            BASE**EMAX * ( 1 - EPS ), where  BASE  is the floating point 
  
            value of BETA.   

    Further Details   
    ===============   

    The computation of  EPS  is based on a routine PARANOIA by   
    W. Kahan of the University of California at Berkeley.   

   ===================================================================== 
*/
    /* Table of constant values */
/*  Unused variable commented out by MDG on 03-09-05
    static int c__1 = 1;
*/
    
    /* Initialized data */
    static logical first = TRUE_;
    static logical iwarn = FALSE_;
    /* System generated locals */
    int i__1;
    real r__1, r__2, r__3, r__4, r__5;
    /* Builtin functions */
    double pow_ri(real *, int *);
    /* Local variables */
    static logical ieee;
    static real half;
    static logical lrnd;
    static real leps, zero, a, b, c;
    static int i, lbeta;
    static real rbase;
    static int lemin, lemax, gnmin;
    static real small;
    static int gpmin;
    static real third, lrmin, lrmax, sixth;
    static logical lieee1;
    extern /* Subroutine */ int slamc1_(int *, int *, logical *, 
	    logical *);
    extern doublereal slamc3_(real *, real *);
    extern /* Subroutine */ int slamc4_(int *, real *, int *), 
	    slamc5_(int *, int *, int *, logical *, int *, 
	    real *);
    static int lt, ngnmin, ngpmin;
    static real one, two;



    if (first) {
	first = FALSE_;
	zero = 0.f;
	one = 1.f;
	two = 2.f;

/*        LBETA, LT, LRND, LEPS, LEMIN and LRMIN  are the local values
 of   
          BETA, T, RND, EPS, EMIN and RMIN.   

          Throughout this routine  we use the function  SLAMC3  to ens
ure   
          that relevant values are stored  and not held in registers, 
 or   
          are not affected by optimizers.   

          SLAMC1 returns the parameters  LBETA, LT, LRND and LIEEE1. 
*/

	slamc1_(&lbeta, &lt, &lrnd, &lieee1);

/*        Start to find EPS. */

	b = (real) lbeta;
	i__1 = -lt;
	a = pow_ri(&b, &i__1);
	leps = a;

/*        Try some tricks to see whether or not this is the correct  E
PS. */

	b = two / 3;
	half = one / 2;
	r__1 = -(doublereal)half;
	sixth = slamc3_(&b, &r__1);
	third = slamc3_(&sixth, &sixth);
	r__1 = -(doublereal)half;
	b = slamc3_(&third, &r__1);
	b = slamc3_(&b, &sixth);
	b = dabs(b);
	if (b < leps) {
	    b = leps;
	}

	leps = 1.f;

/* +       WHILE( ( LEPS.GT.B ).AND.( B.GT.ZERO ) )LOOP */
L10:
	if (leps > b && b > zero) {
	    leps = b;
	    r__1 = half * leps;
/* Computing 5th power */
	    r__3 = two, r__4 = r__3, r__3 *= r__3;
/* Computing 2nd power */
	    r__5 = leps;
	    r__2 = r__4 * (r__3 * r__3) * (r__5 * r__5);
	    c = slamc3_(&r__1, &r__2);
	    r__1 = -(doublereal)c;
	    c = slamc3_(&half, &r__1);
	    b = slamc3_(&half, &c);
	    r__1 = -(doublereal)b;
	    c = slamc3_(&half, &r__1);
	    b = slamc3_(&half, &c);
	    goto L10;
	}
/* +       END WHILE */

	if (a < leps) {
	    leps = a;
	}

/*        Computation of EPS complete.   

          Now find  EMIN.  Let A = + or - 1, and + or - (1 + BASE**(-3
)).   
          Keep dividing  A by BETA until (gradual) underflow occurs. T
his   
          is detected when we cannot recover the previous A. */

	rbase = one / lbeta;
	small = one;
	for (i = 1; i <= 3; ++i) {
	    r__1 = small * rbase;
	    small = slamc3_(&r__1, &zero);
/* L20: */
	}
	a = slamc3_(&one, &small);
	slamc4_(&ngpmin, &one, &lbeta);
	r__1 = -(doublereal)one;
	slamc4_(&ngnmin, &r__1, &lbeta);
	slamc4_(&gpmin, &a, &lbeta);
	r__1 = -(doublereal)a;
	slamc4_(&gnmin, &r__1, &lbeta);
	ieee = FALSE_;

	if (ngpmin == ngnmin && gpmin == gnmin) {
	    if (ngpmin == gpmin) {
		lemin = ngpmin;
/*            ( Non twos-complement machines, no gradual under
flow;   
                e.g.,  VAX ) */
	    } else if (gpmin - ngpmin == 3) {
		lemin = ngpmin - 1 + lt;
		ieee = TRUE_;
/*            ( Non twos-complement machines, with gradual und
erflow;   
                e.g., IEEE standard followers ) */
	    } else {
		lemin = min(ngpmin,gpmin);
/*            ( A guess; no known machine ) */
		iwarn = TRUE_;
	    }

	} else if (ngpmin == gpmin && ngnmin == gnmin) {
	    if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1) {
		lemin = max(ngpmin,ngnmin);
/*            ( Twos-complement machines, no gradual underflow
;   
                e.g., CYBER 205 ) */
	    } else {
		lemin = min(ngpmin,ngnmin);
/*            ( A guess; no known machine ) */
		iwarn = TRUE_;
	    }

	} else if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1 && gpmin == gnmin)
		 {
	    if (gpmin - min(ngpmin,ngnmin) == 3) {
		lemin = max(ngpmin,ngnmin) - 1 + lt;
/*            ( Twos-complement machines with gradual underflo
w;   
                no known machine ) */
	    } else {
		lemin = min(ngpmin,ngnmin);
/*            ( A guess; no known machine ) */
		iwarn = TRUE_;
	    }

	} else {
/* Computing MIN */
	    i__1 = min(ngpmin,ngnmin), i__1 = min(i__1,gpmin);
	    lemin = min(i__1,gnmin);
/*         ( A guess; no known machine ) */
	    iwarn = TRUE_;
	}
/* **   
   Comment out this if block if EMIN is ok */
	if (iwarn) {
	    first = TRUE_;
	    printf("\n\n WARNING. The value EMIN may be incorrect:- ");
/*
	    printf("EMIN = %8i\n",lemin);
*/
/*          %8ld added by MDG on 03-09-05
	    changed to %8d by JCK on 2009-02-02 because of change in f2c.h */
	    printf("EMIN = %8d\n",lemin);
	    printf("If, after inspection, the value EMIN looks acceptable");
            printf("please comment out \n the IF block as marked within the"); 
            printf("code of routine SLAMC2, \n otherwise supply EMIN"); 
            printf("explicitly.\n");
	}
/* **   

          Assume IEEE arithmetic if we found denormalised  numbers abo
ve,   
          or if arithmetic seems to round in the  IEEE style,  determi
ned   
          in routine SLAMC1. A true IEEE machine should have both  thi
ngs   
          true; however, faulty machines may have one or the other. */

	ieee = ieee || lieee1;

/*        Compute  RMIN by successive division by  BETA. We could comp
ute   
          RMIN as BASE**( EMIN - 1 ),  but some machines underflow dur
ing   
          this computation. */

	lrmin = 1.f;
	i__1 = 1 - lemin;
	for (i = 1; i <= 1-lemin; ++i) {
	    r__1 = lrmin * rbase;
	    lrmin = slamc3_(&r__1, &zero);
/* L30: */
	}

/*        Finally, call SLAMC5 to compute EMAX and RMAX. */

	slamc5_(&lbeta, &lt, &lemin, &ieee, &lemax, &lrmax);
    }

    *beta = lbeta;
    *t = lt;
    *rnd = lrnd;
    *eps = leps;
    *emin = lemin;
    *rmin = lrmin;
    *emax = lemax;
    *rmax = lrmax;

    return 0;


/*     End of SLAMC2 */

} /* slamc2_ */
