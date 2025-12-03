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


/* Changed each _( to plain (.  GTC 14 July 1995. */
/* UPDATED: 03/14/2005 by MDG
   UPDATED: 08/16/2009 by BBandini - gcc 4.4.1 won't compile math function
                          when passed a constant; one mod: atan(x)
                          instead of atan((float)1.).
*/

/* cffti1.f -- translated by f2c (version of 15 October 1990  19:58:17).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
*/

#include <math.h>
#include "f2c.h"

/* Subroutine */ int cffti1(int *n, real *wa, int *ifac)
{
    /* Initialized data */

    static int ntryh[4] = { 3,4,2,5 };

    /* System generated locals */
    int i_1, i_2, i_3;

    /* Builtin functions */
/*
    double atan(), cos(), sin();
*/

    /* Local variables */
    static real argh;
    static int idot, ntry, i, j;
    static real argld;
    static int i1, k1, l1, l2, ib;
    static real fi;
    static int ld, ii, nf, ip, nl, nq, nr;
    static real arg;
    static int ido, ipm;
    static real tpi;

/* ***BEGIN PROLOGUE  CFFTI1 */
/* ***PURPOSE  Initialize a real and an integer work array for CFFTF1 and 
*/
/*            CFFTB1. */
/* ***LIBRARY   SLATEC (FFTPACK) */
/* ***CATEGORY  J1A2 */
/* ***TYPE      COMPLEX (RFFTI1-S, CFFTI1-C) */
/* ***KEYWORDS  FFTPACK, FOURIER TRANSFORM */
/* ***AUTHOR  Swarztrauber, P. N., (NCAR) */
/* ***DESCRIPTION */

/*  Subroutine CFFTI1 initializes the work arrays WA and IFAC which are */

/*  used in both CFFTF1 and CFFTB1.  The prime factorization of N and a */

/*  tabulation of the trigonometric functions are computed and stored in 
*/
/*  IFAC and WA, respectively. */

/*  Input Parameter */

/*  N       the length of the sequence to be transformed */

/*  Output Parameters */

/*  WA      a real work array which must be dimensioned at least 2*N. */

/*  IFAC    an integer work array which must be dimensioned at least 15. 
*/

/*          The same work arrays can be used for both CFFTF1 and CFFTB1 */

/*          as long as N remains unchanged.  Different WA and IFAC arrays 
*/
/*          are required for different values of N.  The contents of */
/*          WA and IFAC must not be changed between calls of CFFTF1 or */
/*          CFFTB1. */

/* ***REFERENCES  P. N. Swarztrauber, Vectorizing the FFTs, in Parallel */

/*                 Computations (G. Rodrigue, ed.), Academic Press, */
/*                 1982, pp. 51-83. */
/* ***ROUTINES CALLED  (NONE) */
/* ***REVISION HISTORY  (YYMMDD) */
/*   790601  DATE WRITTEN */
/*   830401  Modified to use SLATEC library source file format. */
/*   860115  Modified by Ron Boisvert to adhere to Fortran 77 by */
/*           (a) changing dummy array size declarations (1) to (*), */
/*           (b) changing references to intrinsic function FLOAT */
/*               to REAL, and */
/*           (c) changing definition of variable TPI by using */
/*               FORTRAN intrinsic function ATAN instead of a DATA */
/*               statement. */
/*   881128  Modified by Dick Valent to meet prologue standards. */
/*   890531  Changed all specific intrinsics to generic.  (WRB) */
/*   891214  Prologue converted to Version 4.0 format.  (BAB) */
/*   900131  Routine changed from subsidiary to user-callable.  (WRB) */
/*   920501  Reformatted the REFERENCES section.  (WRB) */
/* ***END PROLOGUE  CFFTI1 */
    /* Parameter adjustments */
    --ifac;
    --wa;

    /* Function Body */
/* ***FIRST EXECUTABLE STATEMENT  CFFTI1 */
    nl = *n;
    nf = 0;
    j = 0;
L101:
    ++j;
    if (j - 4 <= 0) {
	goto L102;
    } else {
	goto L103;
    }
L102:
    ntry = ntryh[j - 1];
    goto L104;
L103:
    ntry += 2;
L104:
    nq = nl / ntry;
    nr = nl - ntry * nq;
    if (nr != 0) {
	goto L101;
    } else {
	goto L105;
    }
L105:
    ++nf;
    ifac[nf + 2] = ntry;
    nl = nq;
    if (ntry != 2) {
	goto L107;
    }
    if (nf == 1) {
	goto L107;
    }
    i_1 = nf;
    for (i = 2; i <= i_1; ++i) {
	ib = nf - i + 2;
	ifac[ib + 2] = ifac[ib + 1];
/* L106: */
    }
    ifac[3] = 2;
L107:
    if (nl != 1) {
	goto L104;
    }
    ifac[1] = *n;
    ifac[2] = nf;

    float x = 1.f;
    tpi = atan(x) * (float)8.;
    argh = tpi / *n;
    i = 2;
    l1 = 1;
    i_1 = nf;
    for (k1 = 1; k1 <= i_1; ++k1) {
	ip = ifac[k1 + 2];
	ld = 0;
	l2 = l1 * ip;
	ido = *n / l2;
	idot = ido + ido + 2;
	ipm = ip - 1;
	i_2 = ipm;
	for (j = 1; j <= i_2; ++j) {
	    i1 = i;
	    wa[i - 1] = (float)1.;
	    wa[i] = (float)0.;
	    ld += l1;
	    fi = (float)0.;
	    argld = ld * argh;
	    i_3 = idot;
	    for (ii = 4; ii <= i_3; ii += 2) {
		i += 2;
		fi += (float)1.;
		arg = fi * argld;
		wa[i - 1] = cos(arg);
		wa[i] = sin(arg);
/* L108: */
	    }
	    if (ip <= 5) {
		goto L109;
	    }
	    wa[i1 - 1] = wa[i - 1];
	    wa[i1] = wa[i];
L109:
	    ;
	}
	l1 = l2;
/* L110: */
    }
    return 0;
} /* cffti1_ */

