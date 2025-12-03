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
#include <f2c.h>

/* Subroutine */ int slarf_(char *side, int *m, int *n, real *v, 
	int *incv, real *tau, real *c, int *ldc, real *work)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLARF applies a real elementary reflector H to a real m by n matrix   
    C, from either the left or the right. H is represented in the form   

          H = I - tau * v * v'   

    where tau is a real scalar and v is a real vector.   

    If tau = 0, then H is taken to be the unit matrix.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': form  H * C   
            = 'R': form  C * H   

    M       (input) INTEGER   
            The number of rows of the matrix C.   

    N       (input) INTEGER   
            The number of columns of the matrix C.   

    V       (input) REAL array, dimension   
                       (1 + (M-1)*abs(INCV)) if SIDE = 'L'   
                    or (1 + (N-1)*abs(INCV)) if SIDE = 'R'   
            The vector v in the representation of H. V is not used if   
            TAU = 0.   

    INCV    (input) INTEGER   
            The increment between elements of v. INCV <> 0.   

    TAU     (input) REAL   
            The value tau in the representation of H.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the m by n matrix C.   
            On exit, C is overwritten by the matrix H * C if SIDE = 'L', 
  
            or C * H if SIDE = 'R'.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDC >= max(1,M).   

    WORK    (workspace) REAL array, dimension   
                           (N) if SIDE = 'L'   
                        or (M) if SIDE = 'R'   

    ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static real c_b4 = 1.f;
    static real c_b5 = 0.f;
    static int c__1 = 1;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int c_dim1, c_offset;
*/
    real r__1;
    /* Local variables */
    extern /* Subroutine */ int sger_(int *, int *, real *, real *, 
	    int *, real *, int *, real *, int *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sgemv_(char *, int *, int *, real *, 
	    real *, int *, real *, int *, real *, real *, int *);



#define V(I) v[(I)-1]
#define WORK(I) work[(I)-1]

#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]

    if (lsame_(side, "L")) {

/*        Form  H * C */

	if (*tau != 0.f) {

/*           w := C' * v */

	    sgemv_("Transpose", m, n, &c_b4, &C(1,1), ldc, &V(1), incv, &
		    c_b5, &WORK(1), &c__1);

/*           C := C - v * w' */

	    r__1 = -(doublereal)(*tau);
	    sger_(m, n, &r__1, &V(1), incv, &WORK(1), &c__1, &C(1,1), 
		    ldc);
	}
    } else {

/*        Form  C * H */

	if (*tau != 0.f) {

/*           w := C * v */

	    sgemv_("No transpose", m, n, &c_b4, &C(1,1), ldc, &V(1), 
		    incv, &c_b5, &WORK(1), &c__1);

/*           C := C - w * v' */

	    r__1 = -(doublereal)(*tau);
	    sger_(m, n, &r__1, &WORK(1), &c__1, &V(1), incv, &C(1,1), 
		    ldc);
	}
    }
    return 0;

/*     End of SLARF */

} /* slarf_ */
