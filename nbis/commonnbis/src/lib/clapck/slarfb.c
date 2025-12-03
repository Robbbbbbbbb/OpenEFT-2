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

/* Subroutine */ int slarfb_(char *side, char *trans, char *direct, char *
	storev, int *m, int *n, int *k, real *v, int *ldv, 
	real *t, int *ldt, real *c, int *ldc, real *work, int *
	ldwork)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLARFB applies a real block reflector H or its transpose H' to a   
    real m by n matrix C, from either the left or the right.   

    Arguments   
    =========   

    SIDE    (input) CHARACTER*1   
            = 'L': apply H or H' from the Left   
            = 'R': apply H or H' from the Right   

    TRANS   (input) CHARACTER*1   
            = 'N': apply H (No transpose)   
            = 'T': apply H' (Transpose)   

    DIRECT  (input) CHARACTER*1   
            Indicates how H is formed from a product of elementary   
            reflectors   
            = 'F': H = H(1) H(2) . . . H(k) (Forward)   
            = 'B': H = H(k) . . . H(2) H(1) (Backward)   

    STOREV  (input) CHARACTER*1   
            Indicates how the vectors which define the elementary   
            reflectors are stored:   
            = 'C': Columnwise   
            = 'R': Rowwise   

    M       (input) INTEGER   
            The number of rows of the matrix C.   

    N       (input) INTEGER   
            The number of columns of the matrix C.   

    K       (input) INTEGER   
            The order of the matrix T (= the number of elementary   
            reflectors whose product defines the block reflector).   

    V       (input) REAL array, dimension   
                                  (LDV,K) if STOREV = 'C'   
                                  (LDV,M) if STOREV = 'R' and SIDE = 'L' 
  
                                  (LDV,N) if STOREV = 'R' and SIDE = 'R' 
  
            The matrix V. See further details.   

    LDV     (input) INTEGER   
            The leading dimension of the array V.   
            If STOREV = 'C' and SIDE = 'L', LDV >= max(1,M);   
            if STOREV = 'C' and SIDE = 'R', LDV >= max(1,N);   
            if STOREV = 'R', LDV >= K.   

    T       (input) REAL array, dimension (LDT,K)   
            The triangular k by k matrix T in the representation of the   
            block reflector.   

    LDT     (input) INTEGER   
            The leading dimension of the array T. LDT >= K.   

    C       (input/output) REAL array, dimension (LDC,N)   
            On entry, the m by n matrix C.   
            On exit, C is overwritten by H*C or H'*C or C*H or C*H'.   

    LDC     (input) INTEGER   
            The leading dimension of the array C. LDA >= max(1,M).   

    WORK    (workspace) REAL array, dimension (LDWORK,K)   

    LDWORK  (input) INTEGER   
            The leading dimension of the array WORK.   
            If SIDE = 'L', LDWORK >= max(1,N);   
            if SIDE = 'R', LDWORK >= max(1,M).   

    ===================================================================== 
  


       Quick return if possible   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    static real c_b14 = 1.f;
    static real c_b25 = -1.f;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int c_dim1, c_offset, t_dim1, t_offset, v_dim1, v_offset, work_dim1, 
	    work_offset;
*/
    int i__1, i__2;
    /* Local variables */
    static int i, j;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sgemm_(char *, char *, int *, int *, 
	    int *, real *, real *, int *, real *, int *, real *, 
	    real *, int *), scopy_(int *, real *, 
	    int *, real *, int *), strmm_(char *, char *, char *, 
	    char *, int *, int *, real *, real *, int *, real *, 
	    int *);
    static char transt[1];




#define V(I,J) v[(I)-1 + ((J)-1)* ( *ldv)]
#define T(I,J) t[(I)-1 + ((J)-1)* ( *ldt)]
#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]
#define WORK(I,J) work[(I)-1 + ((J)-1)* ( *ldwork)]

    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    if (lsame_(trans, "N")) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

    if (lsame_(storev, "C")) {

	if (lsame_(direct, "F")) {

/*           Let  V =  ( V1 )    (first K rows)   
                       ( V2 )   
             where  V1  is unit lower triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in 
WORK)   

                W := C1' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(n, &C(j,1), ldc, &WORK(1,j), &
			    c__1);
/* L10: */
		}

/*              W := W * V1 */

		strmm_("Right", "Lower", "No transpose", "Unit", n, k, &c_b14,
			 &V(1,1), ldv, &WORK(1,1), ldwork);
		if (*m > *k) {

/*                 W := W + C2'*V2 */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "No transpose", n, k, &i__1, &c_b14, &
			    C(*k+1,1), ldc, &V(*k+1,1), ldv,
			     &c_b14, &WORK(1,1), ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Upper", transt, "Non-unit", n, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - V * W' */

		if (*m > *k) {

/*                 C2 := C2 - V2 * W' */

		    i__1 = *m - *k;
		    sgemm_("No transpose", "Transpose", &i__1, n, k, &c_b25, &
			    V(*k+1,1), ldv, &WORK(1,1), 
			    ldwork, &c_b14, &C(*k+1,1), ldc)
			    ;
		}

/*              W := W * V1' */

		strmm_("Right", "Lower", "Transpose", "Unit", n, k, &c_b14, &
			V(1,1), ldv, &WORK(1,1), ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *n;
		    for (i = 1; i <= *n; ++i) {
			C(j,i) -= WORK(i,j);
/* L20: */
		    }
/* L30: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )
   

                W := C * V  =  (C1*V1 + C2*V2)  (stored in WOR
K)   

                W := C1 */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(m, &C(1,j), &c__1, &WORK(1,j), &c__1);
/* L40: */
		}

/*              W := W * V1 */

		strmm_("Right", "Lower", "No transpose", "Unit", m, k, &c_b14,
			 &V(1,1), ldv, &WORK(1,1), ldwork);
		if (*n > *k) {

/*                 W := W + C2 * V2 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, k, &i__1, &
			    c_b14, &C(1,*k+1), ldc, &V(*k+1,1), ldv, &c_b14, &WORK(1,1), 
			    ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Upper", trans, "Non-unit", m, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - W * V' */

		if (*n > *k) {

/*                 C2 := C2 - W * V2' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, &i__1, k, &c_b25, &
			    WORK(1,1), ldwork, &V(*k+1,1), 
			    ldv, &c_b14, &C(1,*k+1), ldc);
		}

/*              W := W * V1' */

		strmm_("Right", "Lower", "Transpose", "Unit", m, k, &c_b14, &
			V(1,1), ldv, &WORK(1,1), ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,j) -= WORK(i,j);
/* L50: */
		    }
/* L60: */
		}
	    }

	} else {

/*           Let  V =  ( V1 )   
                       ( V2 )    (last K rows)   
             where  V2  is unit upper triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in 
WORK)   

                W := C2' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(n, &C(*m-*k+j,1), ldc, &WORK(1,j), &c__1);
/* L70: */
		}

/*              W := W * V2 */

		strmm_("Right", "Upper", "No transpose", "Unit", n, k, &c_b14,
			 &V(*m-*k+1,1), ldv, &WORK(1,1), 
			ldwork);
		if (*m > *k) {

/*                 W := W + C1'*V1 */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "No transpose", n, k, &i__1, &c_b14, &
			    C(1,1), ldc, &V(1,1), ldv, &c_b14, &
			    WORK(1,1), ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Lower", transt, "Non-unit", n, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - V * W' */

		if (*m > *k) {

/*                 C1 := C1 - V1 * W' */

		    i__1 = *m - *k;
		    sgemm_("No transpose", "Transpose", &i__1, n, k, &c_b25, &
			    V(1,1), ldv, &WORK(1,1), ldwork, &
			    c_b14, &C(1,1), ldc);
		}

/*              W := W * V2' */

		strmm_("Right", "Upper", "Transpose", "Unit", n, k, &c_b14, &
			V(*m-*k+1,1), ldv, &WORK(1,1), 
			ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *n;
		    for (i = 1; i <= *n; ++i) {
			C(*m-*k+j,i) -= WORK(i,j)
				;
/* L80: */
		    }
/* L90: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )
   

                W := C * V  =  (C1*V1 + C2*V2)  (stored in WOR
K)   

                W := C2 */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(m, &C(1,*n-*k+j), &c__1, &WORK(1,j), &c__1);
/* L100: */
		}

/*              W := W * V2 */

		strmm_("Right", "Upper", "No transpose", "Unit", m, k, &c_b14,
			 &V(*n-*k+1,1), ldv, &WORK(1,1), 
			ldwork);
		if (*n > *k) {

/*                 W := W + C1 * V1 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, k, &i__1, &
			    c_b14, &C(1,1), ldc, &V(1,1), ldv, &
			    c_b14, &WORK(1,1), ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Lower", trans, "Non-unit", m, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - W * V' */

		if (*n > *k) {

/*                 C1 := C1 - W * V1' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, &i__1, k, &c_b25, &
			    WORK(1,1), ldwork, &V(1,1), ldv, &
			    c_b14, &C(1,1), ldc);
		}

/*              W := W * V2' */

		strmm_("Right", "Upper", "Transpose", "Unit", m, k, &c_b14, &
			V(*n-*k+1,1), ldv, &WORK(1,1), 
			ldwork);

/*              C2 := C2 - W */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,*n-*k+j) -= WORK(i,j);
/* L110: */
		    }
/* L120: */
		}
	    }
	}

    } else if (lsame_(storev, "R")) {

	if (lsame_(direct, "F")) {

/*           Let  V =  ( V1  V2 )    (V1: first K columns)   
             where  V1  is unit upper triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V'  =  (C1'*V1' + C2'*V2') (stored i
n WORK)   

                W := C1' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(n, &C(j,1), ldc, &WORK(1,j), &
			    c__1);
/* L130: */
		}

/*              W := W * V1' */

		strmm_("Right", "Upper", "Transpose", "Unit", n, k, &c_b14, &
			V(1,1), ldv, &WORK(1,1), ldwork);
		if (*m > *k) {

/*                 W := W + C2'*V2' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", n, k, &i__1, &c_b14, &C(*k+1,1), ldc, &V(1,*k+1), 
			    ldv, &c_b14, &WORK(1,1), ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Upper", transt, "Non-unit", n, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - V' * W' */

		if (*m > *k) {

/*                 C2 := C2 - V2' * W' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", &i__1, n, k, &c_b25, &V(1,*k+1), ldv, &WORK(1,1), 
			    ldwork, &c_b14, &C(*k+1,1), ldc);
		}

/*              W := W * V1 */

		strmm_("Right", "Upper", "No transpose", "Unit", n, k, &c_b14,
			 &V(1,1), ldv, &WORK(1,1), ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *n;
		    for (i = 1; i <= *n; ++i) {
			C(j,i) -= WORK(i,j);
/* L140: */
		    }
/* L150: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )
   

                W := C * V'  =  (C1*V1' + C2*V2')  (stored in 
WORK)   

                W := C1 */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(m, &C(1,j), &c__1, &WORK(1,j), &c__1);
/* L160: */
		}

/*              W := W * V1' */

		strmm_("Right", "Upper", "Transpose", "Unit", m, k, &c_b14, &
			V(1,1), ldv, &WORK(1,1), ldwork);
		if (*n > *k) {

/*                 W := W + C2 * V2' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, k, &i__1, &c_b14, &
			    C(1,*k+1), ldc, &V(1,*k+1), ldv, &c_b14, &WORK(1,1), 
			    ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Upper", trans, "Non-unit", m, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - W * V */

		if (*n > *k) {

/*                 C2 := C2 - W * V2 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, &i__1, k, &
			    c_b25, &WORK(1,1), ldwork, &V(1,*k+1), ldv, &c_b14, &C(1,*k+1), ldc);
		}

/*              W := W * V1 */

		strmm_("Right", "Upper", "No transpose", "Unit", m, k, &c_b14,
			 &V(1,1), ldv, &WORK(1,1), ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,j) -= WORK(i,j);
/* L170: */
		    }
/* L180: */
		}

	    }

	} else {

/*           Let  V =  ( V1  V2 )    (V2: last K columns)   
             where  V2  is unit lower triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 )   
                                                    ( C2 )   

                W := C' * V'  =  (C1'*V1' + C2'*V2') (stored i
n WORK)   

                W := C2' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(n, &C(*m-*k+j,1), ldc, &WORK(1,j), &c__1);
/* L190: */
		}

/*              W := W * V2' */

		strmm_("Right", "Lower", "Transpose", "Unit", n, k, &c_b14, &
			V(1,*m-*k+1), ldv, &WORK(1,1)
			, ldwork);
		if (*m > *k) {

/*                 W := W + C1'*V1' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", n, k, &i__1, &c_b14, &C(1,1), ldc, &V(1,1), ldv, &c_b14, &WORK(1,1), ldwork);
		}

/*              W := W * T'  or  W * T */

		strmm_("Right", "Lower", transt, "Non-unit", n, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - V' * W' */

		if (*m > *k) {

/*                 C1 := C1 - V1' * W' */

		    i__1 = *m - *k;
		    sgemm_("Transpose", "Transpose", &i__1, n, k, &c_b25, &V(1,1), ldv, &WORK(1,1), ldwork, &
			    c_b14, &C(1,1), ldc);
		}

/*              W := W * V2 */

		strmm_("Right", "Lower", "No transpose", "Unit", n, k, &c_b14,
			 &V(1,*m-*k+1), ldv, &WORK(1,1), ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *n;
		    for (i = 1; i <= *n; ++i) {
			C(*m-*k+j,i) -= WORK(i,j)
				;
/* L200: */
		    }
/* L210: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 )
   

                W := C * V'  =  (C1*V1' + C2*V2')  (stored in 
WORK)   

                W := C2 */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    scopy_(m, &C(1,*n-*k+j), &c__1, &WORK(1,j), &c__1);
/* L220: */
		}

/*              W := W * V2' */

		strmm_("Right", "Lower", "Transpose", "Unit", m, k, &c_b14, &
			V(1,*n-*k+1), ldv, &WORK(1,1)
			, ldwork);
		if (*n > *k) {

/*                 W := W + C1 * V1' */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "Transpose", m, k, &i__1, &c_b14, &
			    C(1,1), ldc, &V(1,1), ldv, &c_b14, &
			    WORK(1,1), ldwork);
		}

/*              W := W * T  or  W * T' */

		strmm_("Right", "Lower", trans, "Non-unit", m, k, &c_b14, &T(1,1), ldt, &WORK(1,1), ldwork);

/*              C := C - W * V */

		if (*n > *k) {

/*                 C1 := C1 - W * V1 */

		    i__1 = *n - *k;
		    sgemm_("No transpose", "No transpose", m, &i__1, k, &
			    c_b25, &WORK(1,1), ldwork, &V(1,1), 
			    ldv, &c_b14, &C(1,1), ldc);
		}

/*              W := W * V2 */

		strmm_("Right", "Lower", "No transpose", "Unit", m, k, &c_b14,
			 &V(1,*n-*k+1), ldv, &WORK(1,1), ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= *k; ++j) {
		    i__2 = *m;
		    for (i = 1; i <= *m; ++i) {
			C(i,*n-*k+j) -= WORK(i,j);
/* L230: */
		    }
/* L240: */
		}

	    }

	}
    }

    return 0;

/*     End of SLARFB */

} /* slarfb_ */
