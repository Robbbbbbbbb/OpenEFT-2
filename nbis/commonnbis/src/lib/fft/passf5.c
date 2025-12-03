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

/* passf5.f -- translated by f2c (version of 15 October 1990  19:58:17).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
*/

#include "f2c.h"

/* Subroutine */ int passf5(int *ido, int *l1,  real *cc, real *ch,
                            real *wa1, real *wa2, real *wa3, real *wa4)
{
    /* Initialized data */

    static real tr11 = (float).309016994374947;
    static real ti11 = (float)-.951056516295154;
    static real tr12 = (float)-.809016994374947;
    static real ti12 = (float)-.587785252292473;

    /* System generated locals */
    int cc_dim1, cc_offset, ch_dim1, ch_dim2, ch_offset, i_1, i_2;

    /* Local variables */
    static int i, k;
    static real ci2, ci3, ci4, ci5, di3, di4, di5, di2, cr2, cr3, cr5, cr4, 
	    ti2, ti3, ti4, ti5, dr3, dr4, dr5, dr2, tr2, tr3, tr4, tr5;

    /* Parameter adjustments */
    --wa4;
    --wa3;
    --wa2;
    --wa1;
    ch_dim1 = *ido;
    ch_dim2 = *l1;
    ch_offset = ch_dim1 * (ch_dim2 + 1) + 1;
    ch -= ch_offset;
    cc_dim1 = *ido;
    cc_offset = cc_dim1 * 6 + 1;
    cc -= cc_offset;

    /* Function Body */
    if (*ido != 2) {
	goto L102;
    }
    i_1 = *l1;
    for (k = 1; k <= i_1; ++k) {
	ti5 = cc[(k * 5 + 2) * cc_dim1 + 2] - cc[(k * 5 + 5) * cc_dim1 + 2];
	ti2 = cc[(k * 5 + 2) * cc_dim1 + 2] + cc[(k * 5 + 5) * cc_dim1 + 2];
	ti4 = cc[(k * 5 + 3) * cc_dim1 + 2] - cc[(k * 5 + 4) * cc_dim1 + 2];
	ti3 = cc[(k * 5 + 3) * cc_dim1 + 2] + cc[(k * 5 + 4) * cc_dim1 + 2];
	tr5 = cc[(k * 5 + 2) * cc_dim1 + 1] - cc[(k * 5 + 5) * cc_dim1 + 1];
	tr2 = cc[(k * 5 + 2) * cc_dim1 + 1] + cc[(k * 5 + 5) * cc_dim1 + 1];
	tr4 = cc[(k * 5 + 3) * cc_dim1 + 1] - cc[(k * 5 + 4) * cc_dim1 + 1];
	tr3 = cc[(k * 5 + 3) * cc_dim1 + 1] + cc[(k * 5 + 4) * cc_dim1 + 1];
	ch[(k + ch_dim2) * ch_dim1 + 1] = cc[(k * 5 + 1) * cc_dim1 + 1] + tr2 
		+ tr3;
	ch[(k + ch_dim2) * ch_dim1 + 2] = cc[(k * 5 + 1) * cc_dim1 + 2] + ti2 
		+ ti3;
	cr2 = cc[(k * 5 + 1) * cc_dim1 + 1] + tr11 * tr2 + tr12 * tr3;
	ci2 = cc[(k * 5 + 1) * cc_dim1 + 2] + tr11 * ti2 + tr12 * ti3;
	cr3 = cc[(k * 5 + 1) * cc_dim1 + 1] + tr12 * tr2 + tr11 * tr3;
	ci3 = cc[(k * 5 + 1) * cc_dim1 + 2] + tr12 * ti2 + tr11 * ti3;
	cr5 = ti11 * tr5 + ti12 * tr4;
	ci5 = ti11 * ti5 + ti12 * ti4;
	cr4 = ti12 * tr5 - ti11 * tr4;
	ci4 = ti12 * ti5 - ti11 * ti4;
	ch[(k + (ch_dim2 << 1)) * ch_dim1 + 1] = cr2 - ci5;
	ch[(k + ch_dim2 * 5) * ch_dim1 + 1] = cr2 + ci5;
	ch[(k + (ch_dim2 << 1)) * ch_dim1 + 2] = ci2 + cr5;
	ch[(k + ch_dim2 * 3) * ch_dim1 + 2] = ci3 + cr4;
	ch[(k + ch_dim2 * 3) * ch_dim1 + 1] = cr3 - ci4;
	ch[(k + (ch_dim2 << 2)) * ch_dim1 + 1] = cr3 + ci4;
	ch[(k + (ch_dim2 << 2)) * ch_dim1 + 2] = ci3 - cr4;
	ch[(k + ch_dim2 * 5) * ch_dim1 + 2] = ci2 - cr5;
/* L101: */
    }
    return 0;
L102:
    i_1 = *l1;
    for (k = 1; k <= i_1; ++k) {
	i_2 = *ido;
	for (i = 2; i <= i_2; i += 2) {
	    ti5 = cc[i + (k * 5 + 2) * cc_dim1] - cc[i + (k * 5 + 5) * 
		    cc_dim1];
	    ti2 = cc[i + (k * 5 + 2) * cc_dim1] + cc[i + (k * 5 + 5) * 
		    cc_dim1];
	    ti4 = cc[i + (k * 5 + 3) * cc_dim1] - cc[i + (k * 5 + 4) * 
		    cc_dim1];
	    ti3 = cc[i + (k * 5 + 3) * cc_dim1] + cc[i + (k * 5 + 4) * 
		    cc_dim1];
	    tr5 = cc[i - 1 + (k * 5 + 2) * cc_dim1] - cc[i - 1 + (k * 5 + 5) *
		     cc_dim1];
	    tr2 = cc[i - 1 + (k * 5 + 2) * cc_dim1] + cc[i - 1 + (k * 5 + 5) *
		     cc_dim1];
	    tr4 = cc[i - 1 + (k * 5 + 3) * cc_dim1] - cc[i - 1 + (k * 5 + 4) *
		     cc_dim1];
	    tr3 = cc[i - 1 + (k * 5 + 3) * cc_dim1] + cc[i - 1 + (k * 5 + 4) *
		     cc_dim1];
	    ch[i - 1 + (k + ch_dim2) * ch_dim1] = cc[i - 1 + (k * 5 + 1) * 
		    cc_dim1] + tr2 + tr3;
	    ch[i + (k + ch_dim2) * ch_dim1] = cc[i + (k * 5 + 1) * cc_dim1] + 
		    ti2 + ti3;
	    cr2 = cc[i - 1 + (k * 5 + 1) * cc_dim1] + tr11 * tr2 + tr12 * tr3;

	    ci2 = cc[i + (k * 5 + 1) * cc_dim1] + tr11 * ti2 + tr12 * ti3;
	    cr3 = cc[i - 1 + (k * 5 + 1) * cc_dim1] + tr12 * tr2 + tr11 * tr3;

	    ci3 = cc[i + (k * 5 + 1) * cc_dim1] + tr12 * ti2 + tr11 * ti3;
	    cr5 = ti11 * tr5 + ti12 * tr4;
	    ci5 = ti11 * ti5 + ti12 * ti4;
	    cr4 = ti12 * tr5 - ti11 * tr4;
	    ci4 = ti12 * ti5 - ti11 * ti4;
	    dr3 = cr3 - ci4;
	    dr4 = cr3 + ci4;
	    di3 = ci3 + cr4;
	    di4 = ci3 - cr4;
	    dr5 = cr2 + ci5;
	    dr2 = cr2 - ci5;
	    di5 = ci2 - cr5;
	    di2 = ci2 + cr5;
	    ch[i - 1 + (k + (ch_dim2 << 1)) * ch_dim1] = wa1[i - 1] * dr2 + 
		    wa1[i] * di2;
	    ch[i + (k + (ch_dim2 << 1)) * ch_dim1] = wa1[i - 1] * di2 - wa1[i]
		     * dr2;
	    ch[i - 1 + (k + ch_dim2 * 3) * ch_dim1] = wa2[i - 1] * dr3 + wa2[
		    i] * di3;
	    ch[i + (k + ch_dim2 * 3) * ch_dim1] = wa2[i - 1] * di3 - wa2[i] * 
		    dr3;
	    ch[i - 1 + (k + (ch_dim2 << 2)) * ch_dim1] = wa3[i - 1] * dr4 + 
		    wa3[i] * di4;
	    ch[i + (k + (ch_dim2 << 2)) * ch_dim1] = wa3[i - 1] * di4 - wa3[i]
		     * dr4;
	    ch[i - 1 + (k + ch_dim2 * 5) * ch_dim1] = wa4[i - 1] * dr5 + wa4[
		    i] * di5;
	    ch[i + (k + ch_dim2 * 5) * ch_dim1] = wa4[i - 1] * di5 - wa4[i] * 
		    dr5;
/* L103: */
	}
/* L104: */
    }
    return 0;
} /* passf5_ */

