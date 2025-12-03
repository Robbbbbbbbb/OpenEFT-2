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

/* Subroutine */ int slarft_(char *direct, char *storev, int *n, int *
	k, real *v, int *ldv, real *tau, real *t, int *ldt)
{
/*  -- LAPACK auxiliary routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       February 29, 1992   


    Purpose   
    =======   

    SLARFT forms the triangular factor T of a real block reflector H   
    of order n, which is defined as a product of k elementary reflectors. 
  

    If DIRECT = 'F', H = H(1) H(2) . . . H(k) and T is upper triangular; 
  

    If DIRECT = 'B', H = H(k) . . . H(2) H(1) and T is lower triangular. 
  

    If STOREV = 'C', the vector which defines the elementary reflector   
    H(i) is stored in the i-th column of the array V, and   

       H  =  I - V * T * V'   

    If STOREV = 'R', the vector which defines the elementary reflector   
    H(i) is stored in the i-th row of the array V, and   

       H  =  I - V' * T * V   

    Arguments   
    =========   

    DIRECT  (input) CHARACTER*1   
            Specifies the order in which the elementary reflectors are   
            multiplied to form the block reflector:   
            = 'F': H = H(1) H(2) . . . H(k) (Forward)   
            = 'B': H = H(k) . . . H(2) H(1) (Backward)   

    STOREV  (input) CHARACTER*1   
            Specifies how the vectors which define the elementary   
            reflectors are stored (see also Further Details):   
            = 'C': columnwise   
            = 'R': rowwise   

    N       (input) INTEGER   
            The order of the block reflector H. N >= 0.   

    K       (input) INTEGER   
            The order of the triangular factor T (= the number of   
            elementary reflectors). K >= 1.   

    V       (input/output) REAL array, dimension   
                                 (LDV,K) if STOREV = 'C'   
                                 (LDV,N) if STOREV = 'R'   
            The matrix V. See further details.   

    LDV     (input) INTEGER   
            The leading dimension of the array V.   
            If STOREV = 'C', LDV >= max(1,N); if STOREV = 'R', LDV >= K. 
  

    TAU     (input) REAL array, dimension (K)   
            TAU(i) must contain the scalar factor of the elementary   
            reflector H(i).   

    T       (output) REAL array, dimension (LDT,K)   
            The k by k triangular factor T of the block reflector.   
            If DIRECT = 'F', T is upper triangular; if DIRECT = 'B', T is 
  
            lower triangular. The rest of the array is not used.   

    LDT     (input) INTEGER   
            The leading dimension of the array T. LDT >= K.   

    Further Details   
    ===============   

    The shape of the matrix V and the storage of the vectors which define 
  
    the H(i) is best illustrated by the following example with n = 5 and 
  
    k = 3. The elements equal to 1 are not stored; the corresponding   
    array elements are modified but restored on exit. The rest of the   
    array is not used.   

    DIRECT = 'F' and STOREV = 'C':         DIRECT = 'F' and STOREV = 'R': 
  

                 V = (  1       )                 V = (  1 v1 v1 v1 v1 ) 
  
                     ( v1  1    )                     (     1 v2 v2 v2 ) 
  
                     ( v1 v2  1 )                     (        1 v3 v3 ) 
  
                     ( v1 v2 v3 )   
                     ( v1 v2 v3 )   

    DIRECT = 'B' and STOREV = 'C':         DIRECT = 'B' and STOREV = 'R': 
  

                 V = ( v1 v2 v3 )                 V = ( v1 v1  1       ) 
  
                     ( v1 v2 v3 )                     ( v2 v2 v2  1    ) 
  
                     (  1 v2 v3 )                     ( v3 v3 v3 v3  1 ) 
  
                     (     1 v3 )   
                     (        1 )   

    ===================================================================== 
  


       Quick return if possible   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    static real c_b8 = 0.f;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int t_dim1, t_offset, v_dim1, v_offset;
*/
    int i__1, i__2, i__3;
    real r__1;
    /* Local variables */
    static int i, j;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int sgemv_(char *, int *, int *, real *, 
	    real *, int *, real *, int *, real *, real *, int *), strmv_(char *, char *, char *, int *, real *, 
	    int *, real *, int *);
    static real vii;



#define TAU(I) tau[(I)-1]

#define V(I,J) v[(I)-1 + ((J)-1)* ( *ldv)]
#define T(I,J) t[(I)-1 + ((J)-1)* ( *ldt)]

    if (*n == 0) {
	return 0;
    }

    if (lsame_(direct, "F")) {
	i__1 = *k;
	for (i = 1; i <= *k; ++i) {
	    if (TAU(i) == 0.f) {

/*              H(i)  =  I */

		i__2 = i;
		for (j = 1; j <= i; ++j) {
		    T(j,i) = 0.f;
/* L10: */
		}
	    } else {

/*              general case */

		vii = V(i,i);
		V(i,i) = 1.f;
		if (lsame_(storev, "C")) {

/*                 T(1:i-1,i) := - tau(i) * V(i:n,1:i-1)' 
* V(i:n,i) */

		    i__2 = *n - i + 1;
		    i__3 = i - 1;
		    r__1 = -(doublereal)TAU(i);
		    sgemv_("Transpose", &i__2, &i__3, &r__1, &V(i,1), 
			    ldv, &V(i,i), &c__1, &c_b8, &T(1,i), &c__1);
		} else {

/*                 T(1:i-1,i) := - tau(i) * V(1:i-1,i:n) *
 V(i,i:n)' */

		    i__2 = i - 1;
		    i__3 = *n - i + 1;
		    r__1 = -(doublereal)TAU(i);
		    sgemv_("No transpose", &i__2, &i__3, &r__1, &V(1,i), ldv, &V(i,i), ldv, &c_b8, &T(1,i), &c__1);
		}
		V(i,i) = vii;

/*              T(1:i-1,i) := T(1:i-1,1:i-1) * T(1:i-1,i) */

		i__2 = i - 1;
		strmv_("Upper", "No transpose", "Non-unit", &i__2, &T(1,1), ldt, &T(1,i), &c__1);
		T(i,i) = TAU(i);
	    }
/* L20: */
	}
    } else {
	for (i = *k; i >= 1; --i) {
	    if (TAU(i) == 0.f) {

/*              H(i)  =  I */

		i__1 = *k;
		for (j = i; j <= *k; ++j) {
		    T(j,i) = 0.f;
/* L30: */
		}
	    } else {

/*              general case */

		if (i < *k) {
		    if (lsame_(storev, "C")) {
			vii = V(*n-*k+i,i);
			V(*n-*k+i,i) = 1.f;

/*                    T(i+1:k,i) :=   
                              - tau(i) * V(1:n-k+i,i+1
:k)' * V(1:n-k+i,i) */

			i__1 = *n - *k + i;
			i__2 = *k - i;
			r__1 = -(doublereal)TAU(i);
			sgemv_("Transpose", &i__1, &i__2, &r__1, &V(1,i+1), ldv, &V(1,i), &c__1, &
				c_b8, &T(i+1,i), &c__1);
			V(*n-*k+i,i) = vii;
		    } else {
			vii = V(i,*n-*k+i);
			V(i,*n-*k+i) = 1.f;

/*                    T(i+1:k,i) :=   
                              - tau(i) * V(i+1:k,1:n-k
+i) * V(i,1:n-k+i)' */

			i__1 = *k - i;
			i__2 = *n - *k + i;
			r__1 = -(doublereal)TAU(i);
			sgemv_("No transpose", &i__1, &i__2, &r__1, &V(i+1,1), ldv, &V(i,1), ldv, &c_b8, &
				T(i+1,i), &c__1);
			V(i,*n-*k+i) = vii;
		    }

/*                 T(i+1:k,i) := T(i+1:k,i+1:k) * T(i+1:k,
i) */

		    i__1 = *k - i;
		    strmv_("Lower", "No transpose", "Non-unit", &i__1, &T(i+1,i+1), ldt, &T(i+1,i)
			    , &c__1);
		}
		T(i,i) = TAU(i);
	    }
/* L40: */
	}
    }
    return 0;

/*     End of SLARFT */

} /* slarft_ */
