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

/* passb2.f -- translated by f2c (version of 15 October 1990  19:58:17).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
*/

#include "f2c.h"

/* Subroutine */ int passb2(int *ido, int *l1,
                            real *cc, real *ch, real *wa1)
{
    /* System generated locals */
    int cc_dim1, cc_offset, ch_dim1, ch_dim2, ch_offset, i_1, i_2;

    /* Local variables */
    static int i, k;
    static real ti2, tr2;

    /* Parameter adjustments */
    --wa1;
    ch_dim1 = *ido;
    ch_dim2 = *l1;
    ch_offset = ch_dim1 * (ch_dim2 + 1) + 1;
    ch -= ch_offset;
    cc_dim1 = *ido;
    cc_offset = cc_dim1 * 3 + 1;
    cc -= cc_offset;

    /* Function Body */
    if (*ido > 2) {
	goto L102;
    }
    i_1 = *l1;
    for (k = 1; k <= i_1; ++k) {
	ch[(k + ch_dim2) * ch_dim1 + 1] = cc[((k << 1) + 1) * cc_dim1 + 1] + 
		cc[((k << 1) + 2) * cc_dim1 + 1];
	ch[(k + (ch_dim2 << 1)) * ch_dim1 + 1] = cc[((k << 1) + 1) * cc_dim1 
		+ 1] - cc[((k << 1) + 2) * cc_dim1 + 1];
	ch[(k + ch_dim2) * ch_dim1 + 2] = cc[((k << 1) + 1) * cc_dim1 + 2] + 
		cc[((k << 1) + 2) * cc_dim1 + 2];
	ch[(k + (ch_dim2 << 1)) * ch_dim1 + 2] = cc[((k << 1) + 1) * cc_dim1 
		+ 2] - cc[((k << 1) + 2) * cc_dim1 + 2];
/* L101: */
    }
    return 0;
L102:
    i_1 = *l1;
    for (k = 1; k <= i_1; ++k) {
	i_2 = *ido;
	for (i = 2; i <= i_2; i += 2) {
	    ch[i - 1 + (k + ch_dim2) * ch_dim1] = cc[i - 1 + ((k << 1) + 1) * 
		    cc_dim1] + cc[i - 1 + ((k << 1) + 2) * cc_dim1];
	    tr2 = cc[i - 1 + ((k << 1) + 1) * cc_dim1] - cc[i - 1 + ((k << 1) 
		    + 2) * cc_dim1];
	    ch[i + (k + ch_dim2) * ch_dim1] = cc[i + ((k << 1) + 1) * cc_dim1]
		     + cc[i + ((k << 1) + 2) * cc_dim1];
	    ti2 = cc[i + ((k << 1) + 1) * cc_dim1] - cc[i + ((k << 1) + 2) * 
		    cc_dim1];
	    ch[i + (k + (ch_dim2 << 1)) * ch_dim1] = wa1[i - 1] * ti2 + wa1[i]
		     * tr2;
	    ch[i - 1 + (k + (ch_dim2 << 1)) * ch_dim1] = wa1[i - 1] * tr2 - 
		    wa1[i] * ti2;
/* L103: */
	}
/* L104: */
    }
    return 0;
} /* passb2_ */

