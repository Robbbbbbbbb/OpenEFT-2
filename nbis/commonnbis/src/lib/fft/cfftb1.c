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
/* UPDATED: 03/14/2005 by MDG */

/* cfftb1.f -- translated by f2c (version of 15 October 1990  19:58:17).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
*/

#include "f2c.h"

extern /* Subroutine */ int passb(int *, int *, int *,
                           int *, int *, real *, real *,
                           real *, real *, real *, real *);
extern /* Subroutine */ int passb2(int *, int *,
                            real *, real *, real *);
extern /* Subroutine */ int passb3(int *, int *,
                            real *, real *, real *, real *);
extern /* Subroutine */ int passb4(int *, int *,
                            real *, real *, real *, real *, real *);
extern /* Subroutine */ int passb5(int *, int *, real *, real *,
                            real *, real *, real *, real *);

/* Subroutine */ int cfftb1(int *n,
                            real *c, real *ch, real *wa, int *ifac)
{
    /* System generated locals */
    int i_1;

    /* Local variables */
    static int idot, i;
    static int k1, l1, l2, n2;
    static int na, nf, ip, iw, ix2, ix3, ix4, nac, ido, idl1;

/* ***BEGIN PROLOGUE  CFFTB1 */
/* ***PURPOSE  Compute the unnormalized inverse of CFFTF1. */
/* ***LIBRARY   SLATEC (FFTPACK) */
/* ***CATEGORY  J1A2 */
/* ***TYPE      COMPLEX (RFFTB1-S, CFFTB1-C) */
/* ***KEYWORDS  FFTPACK, FOURIER TRANSFORM */
/* ***AUTHOR  Swarztrauber, P. N., (NCAR) */
/* ***DESCRIPTION */

/*  Subroutine CFFTB1 computes the backward complex discrete Fourier */
/*  transform (the Fourier synthesis).  Equivalently, CFFTB1 computes */
/*  a complex periodic sequence from its Fourier coefficients. */
/*  The transform is defined below at output parameter C. */

/*  A call of CFFTF1 followed by a call of CFFTB1 will multiply the */
/*  sequence by N. */

/*  The arrays WA and IFAC which are used by subroutine CFFTB1 must be */
/*  initialized by calling subroutine CFFTI1 (N, WA, IFAC). */

/*  Input Parameters */

/*  N       the length of the complex sequence C.  The method is */
/*          more efficient when N is the product of small primes. */

/*  C       a complex array of length N which contains the sequence */

/*  CH      a real work array of length at least 2*N */

/*  WA      a real work array which must be dimensioned at least 2*N. */

/*  IFAC    an integer work array which must be dimensioned at least 15. 
*/

/*          The WA and IFAC arrays must be initialized by calling */
/*          subroutine CFFTI1 (N, WA, IFAC), and different WA and IFAC */
/*          arrays must be used for each different value of N.  This */
/*          initialization does not have to be repeated so long as N */
/*          remains unchanged.  Thus subsequent transforms can be */
/*          obtained faster than the first.  The same WA and IFAC arrays 
*/
/*          can be used by CFFTF1 and CFFTB1. */

/*  Output Parameters */

/*  C       For J=1,...,N */

/*              C(J)=the sum from K=1,...,N of */

/*                 C(K)*EXP(I*(J-1)*(K-1)*2*PI/N) */

/*                         where I=SQRT(-1) */

/*  NOTE:   WA and IFAC contain initialization calculations which must */
/*          not be destroyed between calls of subroutine CFFTF1 or CFFTB1 
*/

/* ***REFERENCES  P. N. Swarztrauber, Vectorizing the FFTs, in Parallel */

/*                 Computations (G. Rodrigue, ed.), Academic Press, */
/*                 1982, pp. 51-83. */
/* ***ROUTINES CALLED  PASSB, PASSB2, PASSB3, PASSB4, PASSB5 */
/* ***REVISION HISTORY  (YYMMDD) */
/*   790601  DATE WRITTEN */
/*   830401  Modified to use SLATEC library source file format. */
/*   860115  Modified by Ron Boisvert to adhere to Fortran 77 by */
/*           changing dummy array size declarations (1) to (*). */
/*   881128  Modified by Dick Valent to meet prologue standards. */
/*   891214  Prologue converted to Version 4.0 format.  (BAB) */
/*   900131  Routine changed from subsidiary to user-callable.  (WRB) */
/*   920501  Reformatted the REFERENCES section.  (WRB) */
/* ***END PROLOGUE  CFFTB1 */
/* ***FIRST EXECUTABLE STATEMENT  CFFTB1 */
    /* Parameter adjustments */
    --ifac;
    --wa;
    --ch;
    --c;

    /* Function Body */
    nf = ifac[2];
    na = 0;
    l1 = 1;
    iw = 1;
    i_1 = nf;
    for (k1 = 1; k1 <= i_1; ++k1) {
	ip = ifac[k1 + 2];
	l2 = ip * l1;
	ido = *n / l2;
	idot = ido + ido;
	idl1 = idot * l1;
	if (ip != 4) {
	    goto L103;
	}
	ix2 = iw + idot;
	ix3 = ix2 + idot;
	if (na != 0) {
	    goto L101;
	}
	passb4(&idot, &l1, &c[1], &ch[1], &wa[iw], &wa[ix2], &wa[ix3]);
	goto L102;
L101:
	passb4(&idot, &l1, &ch[1], &c[1], &wa[iw], &wa[ix2], &wa[ix3]);
L102:
	na = 1 - na;
	goto L115;
L103:
	if (ip != 2) {
	    goto L106;
	}
	if (na != 0) {
	    goto L104;
	}
	passb2(&idot, &l1, &c[1], &ch[1], &wa[iw]);
	goto L105;
L104:
	passb2(&idot, &l1, &ch[1], &c[1], &wa[iw]);
L105:
	na = 1 - na;
	goto L115;
L106:
	if (ip != 3) {
	    goto L109;
	}
	ix2 = iw + idot;
	if (na != 0) {
	    goto L107;
	}
	passb3(&idot, &l1, &c[1], &ch[1], &wa[iw], &wa[ix2]);
	goto L108;
L107:
	passb3(&idot, &l1, &ch[1], &c[1], &wa[iw], &wa[ix2]);
L108:
	na = 1 - na;
	goto L115;
L109:
	if (ip != 5) {
	    goto L112;
	}
	ix2 = iw + idot;
	ix3 = ix2 + idot;
	ix4 = ix3 + idot;
	if (na != 0) {
	    goto L110;
	}
	passb5(&idot, &l1, &c[1], &ch[1], &wa[iw], &wa[ix2], &wa[ix3], &wa[
		ix4]);
	goto L111;
L110:
	passb5(&idot, &l1, &ch[1], &c[1], &wa[iw], &wa[ix2], &wa[ix3], &wa[
		ix4]);
L111:
	na = 1 - na;
	goto L115;
L112:
	if (na != 0) {
	    goto L113;
	}
	passb(&nac, &idot, &ip, &l1, &idl1, &c[1], &c[1], &c[1], &ch[1], &ch[
		1], &wa[iw]);
	goto L114;
L113:
	passb(&nac, &idot, &ip, &l1, &idl1, &ch[1], &ch[1], &ch[1], &c[1], &
		c[1], &wa[iw]);
L114:
	if (nac != 0) {
	    na = 1 - na;
	}
L115:
	l1 = l2;
	iw += (ip - 1) * idot;
/* L116: */
    }
    if (na == 0) {
	return 0;
    }
    n2 = *n + *n;
    i_1 = n2;
    for (i = 1; i <= i_1; ++i) {
	c[i] = ch[i];
/* L117: */
    }
    return 0;
} /* cfftb1_ */

