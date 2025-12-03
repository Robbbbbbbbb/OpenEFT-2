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

/* passf.f -- translated by f2c (version of 15 October 1990  19:58:17).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
*/

#include "f2c.h"

/* Subroutine */ int passf(int *nac, int *ido, int *ip,
                           int *l1, int *idl1, real *cc, real *c1,
                           real *c2, real *ch, real *ch2, real *wa)
{
    /* System generated locals */
    int ch_dim1, ch_dim2, ch_offset, cc_dim1, cc_dim2, cc_offset, c1_dim1,
	     c1_dim2, c1_offset, c2_dim1, c2_offset, ch2_dim1, ch2_offset, 
	    i_1, i_2, i_3;

    /* Local variables */
    static int idij, idlj, idot, ipph, i, j, k, l, jc, lc, ik, nt, idj, 
	    idl, inc, idp;
    static real wai, war;
    static int ipp2;

    /* Parameter adjustments */
    --wa;
    ch2_dim1 = *idl1;
    ch2_offset = ch2_dim1 + 1;
    ch2 -= ch2_offset;
    ch_dim1 = *ido;
    ch_dim2 = *l1;
    ch_offset = ch_dim1 * (ch_dim2 + 1) + 1;
    ch -= ch_offset;
    c2_dim1 = *idl1;
    c2_offset = c2_dim1 + 1;
    c2 -= c2_offset;
    c1_dim1 = *ido;
    c1_dim2 = *l1;
    c1_offset = c1_dim1 * (c1_dim2 + 1) + 1;
    c1 -= c1_offset;
    cc_dim1 = *ido;
    cc_dim2 = *ip;
    cc_offset = cc_dim1 * (cc_dim2 + 1) + 1;
    cc -= cc_offset;

    /* Function Body */
    idot = *ido / 2;
    nt = *ip * *idl1;
    ipp2 = *ip + 2;
    ipph = (*ip + 1) / 2;
    idp = *ip * *ido;

    if (*ido < *l1) {
	goto L106;
    }
    i_1 = ipph;
    for (j = 2; j <= i_1; ++j) {
	jc = ipp2 - j;
	i_2 = *l1;
	for (k = 1; k <= i_2; ++k) {
	    i_3 = *ido;
	    for (i = 1; i <= i_3; ++i) {
		ch[i + (k + j * ch_dim2) * ch_dim1] = cc[i + (j + k * cc_dim2)
			 * cc_dim1] + cc[i + (jc + k * cc_dim2) * cc_dim1];
		ch[i + (k + jc * ch_dim2) * ch_dim1] = cc[i + (j + k * 
			cc_dim2) * cc_dim1] - cc[i + (jc + k * cc_dim2) * 
			cc_dim1];
/* L101: */
	    }
/* L102: */
	}
/* L103: */
    }
    i_1 = *l1;
    for (k = 1; k <= i_1; ++k) {
	i_2 = *ido;
	for (i = 1; i <= i_2; ++i) {
	    ch[i + (k + ch_dim2) * ch_dim1] = cc[i + (k * cc_dim2 + 1) * 
		    cc_dim1];
/* L104: */
	}
/* L105: */
    }
    goto L112;
L106:
    i_1 = ipph;
    for (j = 2; j <= i_1; ++j) {
	jc = ipp2 - j;
	i_2 = *ido;
	for (i = 1; i <= i_2; ++i) {
	    i_3 = *l1;
	    for (k = 1; k <= i_3; ++k) {
		ch[i + (k + j * ch_dim2) * ch_dim1] = cc[i + (j + k * cc_dim2)
			 * cc_dim1] + cc[i + (jc + k * cc_dim2) * cc_dim1];
		ch[i + (k + jc * ch_dim2) * ch_dim1] = cc[i + (j + k * 
			cc_dim2) * cc_dim1] - cc[i + (jc + k * cc_dim2) * 
			cc_dim1];
/* L107: */
	    }
/* L108: */
	}
/* L109: */
    }
    i_1 = *ido;
    for (i = 1; i <= i_1; ++i) {
	i_2 = *l1;
	for (k = 1; k <= i_2; ++k) {
	    ch[i + (k + ch_dim2) * ch_dim1] = cc[i + (k * cc_dim2 + 1) * 
		    cc_dim1];
/* L110: */
	}
/* L111: */
    }
L112:
    idl = 2 - *ido;
    inc = 0;
    i_1 = ipph;
    for (l = 2; l <= i_1; ++l) {
	lc = ipp2 - l;
	idl += *ido;
	i_2 = *idl1;
	for (ik = 1; ik <= i_2; ++ik) {
	    c2[ik + l * c2_dim1] = ch2[ik + ch2_dim1] + wa[idl - 1] * ch2[ik 
		    + (ch2_dim1 << 1)];
	    c2[ik + lc * c2_dim1] = -(doublereal)wa[idl] * ch2[ik + *ip * 
		    ch2_dim1];
/* L113: */
	}
	idlj = idl;
	inc += *ido;
	i_2 = ipph;
	for (j = 3; j <= i_2; ++j) {
	    jc = ipp2 - j;
	    idlj += inc;
	    if (idlj > idp) {
		idlj -= idp;
	    }
	    war = wa[idlj - 1];
	    wai = wa[idlj];
	    i_3 = *idl1;
	    for (ik = 1; ik <= i_3; ++ik) {
		c2[ik + l * c2_dim1] += war * ch2[ik + j * ch2_dim1];
		c2[ik + lc * c2_dim1] -= wai * ch2[ik + jc * ch2_dim1];
/* L114: */
	    }
/* L115: */
	}
/* L116: */
    }
    i_1 = ipph;
    for (j = 2; j <= i_1; ++j) {
	i_2 = *idl1;
	for (ik = 1; ik <= i_2; ++ik) {
	    ch2[ik + ch2_dim1] += ch2[ik + j * ch2_dim1];
/* L117: */
	}
/* L118: */
    }
    i_1 = ipph;
    for (j = 2; j <= i_1; ++j) {
	jc = ipp2 - j;
	i_2 = *idl1;
	for (ik = 2; ik <= i_2; ik += 2) {
	    ch2[ik - 1 + j * ch2_dim1] = c2[ik - 1 + j * c2_dim1] - c2[ik + 
		    jc * c2_dim1];
	    ch2[ik - 1 + jc * ch2_dim1] = c2[ik - 1 + j * c2_dim1] + c2[ik + 
		    jc * c2_dim1];
	    ch2[ik + j * ch2_dim1] = c2[ik + j * c2_dim1] + c2[ik - 1 + jc * 
		    c2_dim1];
	    ch2[ik + jc * ch2_dim1] = c2[ik + j * c2_dim1] - c2[ik - 1 + jc * 
		    c2_dim1];
/* L119: */
	}
/* L120: */
    }
    *nac = 1;
    if (*ido == 2) {
	return 0;
    }
    *nac = 0;
    i_1 = *idl1;
    for (ik = 1; ik <= i_1; ++ik) {
	c2[ik + c2_dim1] = ch2[ik + ch2_dim1];
/* L121: */
    }
    i_1 = *ip;
    for (j = 2; j <= i_1; ++j) {
	i_2 = *l1;
	for (k = 1; k <= i_2; ++k) {
	    c1[(k + j * c1_dim2) * c1_dim1 + 1] = ch[(k + j * ch_dim2) * 
		    ch_dim1 + 1];
	    c1[(k + j * c1_dim2) * c1_dim1 + 2] = ch[(k + j * ch_dim2) * 
		    ch_dim1 + 2];
/* L122: */
	}
/* L123: */
    }
    if (idot > *l1) {
	goto L127;
    }
    idij = 0;
    i_1 = *ip;
    for (j = 2; j <= i_1; ++j) {
	idij += 2;
	i_2 = *ido;
	for (i = 4; i <= i_2; i += 2) {
	    idij += 2;
	    i_3 = *l1;
	    for (k = 1; k <= i_3; ++k) {
		c1[i - 1 + (k + j * c1_dim2) * c1_dim1] = wa[idij - 1] * ch[i 
			- 1 + (k + j * ch_dim2) * ch_dim1] + wa[idij] * ch[i 
			+ (k + j * ch_dim2) * ch_dim1];
		c1[i + (k + j * c1_dim2) * c1_dim1] = wa[idij - 1] * ch[i + (
			k + j * ch_dim2) * ch_dim1] - wa[idij] * ch[i - 1 + (
			k + j * ch_dim2) * ch_dim1];
/* L124: */
	    }
/* L125: */
	}
/* L126: */
    }
    return 0;
L127:
    idj = 2 - *ido;
    i_1 = *ip;
    for (j = 2; j <= i_1; ++j) {
	idj += *ido;
	i_2 = *l1;
	for (k = 1; k <= i_2; ++k) {
	    idij = idj;
	    i_3 = *ido;
	    for (i = 4; i <= i_3; i += 2) {
		idij += 2;
		c1[i - 1 + (k + j * c1_dim2) * c1_dim1] = wa[idij - 1] * ch[i 
			- 1 + (k + j * ch_dim2) * ch_dim1] + wa[idij] * ch[i 
			+ (k + j * ch_dim2) * ch_dim1];
		c1[i + (k + j * c1_dim2) * c1_dim1] = wa[idij - 1] * ch[i + (
			k + j * ch_dim2) * ch_dim1] - wa[idij] * ch[i - 1 + (
			k + j * ch_dim2) * ch_dim1];
/* L128: */
	    }
/* L129: */
	}
/* L130: */
    }
    return 0;
} /* passf_ */

