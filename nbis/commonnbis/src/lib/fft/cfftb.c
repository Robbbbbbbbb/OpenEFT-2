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

/* cfftb.f -- translated by f2c (version of 15 October 1990  19:58:17).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
*/

#include "f2c.h"

extern /* Subroutine */ int cfftb1(int *, real *, real *, real *,
                                   int *);

/* Subroutine */ int cfftb(int *n, real *c, real *wsave)
{
    static int iw1, iw2;

/* ***BEGIN PROLOGUE  CFFTB */
/* ***DATE WRITTEN   790601   (YYMMDD) */
/* ***REVISION DATE  830401   (YYMMDD) */
/* ***CATEGORY NO.  J1A2 */
/* ***KEYWORDS  FOURIER TRANSFORM */
/* ***AUTHOR  SWARZTRAUBER, P. N., (NCAR) */
/* ***PURPOSE  Unnormalized inverse of CFFTF. */
/* ***DESCRIPTION */

/*  Subroutine CFFTB computes the backward complex discrete Fourier */
/*  transform (the Fourier synthesis).  Equivalently, CFFTB computes */
/*  a complex periodic sequence from its Fourier coefficients. */
/*  The transform is defined below at output parameter C. */

/*  A call of CFFTF followed by a call of CFFTB will multiply the */
/*  sequence by N. */

/*  The array WSAVE which is used by subroutine CFFTB must be */
/*  initialized by calling subroutine CFFTI(N,WSAVE). */

/*  Input Parameters */


/*  N      the length of the complex sequence C.  The method is */
/*         more efficient when N is the product of small primes. */

/*  C      a complex array of length N which contains the sequence */

/*  WSAVE   a real work array which must be dimensioned at least 4*N+15 */

/*          in the program that calls CFFTB.  The WSAVE array must be */
/*          initialized by calling subroutine CFFTI(N,WSAVE), and a */
/*          different WSAVE array must be used for each different */
/*          value of N.  This initialization does not have to be */
/*          repeated so long as N remains unchanged.  Thus subsequent */
/*          transforms can be obtained faster than the first. */
/*          The same WSAVE array can be used by CFFTF and CFFTB. */

/*  Output Parameters */

/*  C      For J=1,...,N */

/*             C(J)=the sum from K=1,...,N of */

/*                   C(K)*EXP(I*J*K*2*PI/N) */

/*                         where I=SQRT(-1) */

/*  WSAVE   contains initialization calculations which must not be */
/*          destroyed between calls of subroutine CFFTF or CFFTB */
/* ***REFERENCES  (NONE) */
/* ***ROUTINES CALLED  CFFTB1 */
/* ***END PROLOGUE  CFFTB */
/* ***FIRST EXECUTABLE STATEMENT  CFFTB */
    /* Parameter adjustments */
    --wsave;
    --c;

    /* Function Body */
    if (*n == 1) {
	return 0;
    }
    iw1 = *n + *n + 1;
    iw2 = iw1 + *n + *n;
    cfftb1(n, &c[1], &wsave[1], &wsave[iw1], (int *)&wsave[iw2]);
    return 0;
} /* cfftb_ */

