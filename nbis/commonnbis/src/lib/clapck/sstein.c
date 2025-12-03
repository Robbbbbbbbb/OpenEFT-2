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

/* Subroutine */ int sstein_(int *n, real *d, real *e, int *m, real *
	w, int *iblock, int *isplit, real *z, int *ldz, real *
	work, int *iwork, int *ifail, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SSTEIN computes the eigenvectors of a real symmetric tridiagonal   
    matrix T corresponding to specified eigenvalues, using inverse   
    iteration.   

    The maximum number of iterations allowed for each eigenvector is   
    specified by an internal parameter MAXITS (currently set to 5).   

    Arguments   
    =========   

    N       (input) INTEGER   
            The order of the matrix.  N >= 0.   

    D       (input) REAL array, dimension (N)   
            The n diagonal elements of the tridiagonal matrix T.   

    E       (input) REAL array, dimension (N)   
            The (n-1) subdiagonal elements of the tridiagonal matrix   
            T, in elements 1 to N-1.  E(N) need not be set.   

    M       (input) INTEGER   
            The number of eigenvectors to be found.  0 <= M <= N.   

    W       (input) REAL array, dimension (N)   
            The first M elements of W contain the eigenvalues for   
            which eigenvectors are to be computed.  The eigenvalues   
            should be grouped by split-off block and ordered from   
            smallest to largest within the block.  ( The output array   
            W from SSTEBZ with ORDER = 'B' is expected here. )   

    IBLOCK  (input) INTEGER array, dimension (N)   
            The submatrix indices associated with the corresponding   
            eigenvalues in W; IBLOCK(i)=1 if eigenvalue W(i) belongs to   
            the first submatrix from the top, =2 if W(i) belongs to   
            the second submatrix, etc.  ( The output array IBLOCK   
            from SSTEBZ is expected here. )   

    ISPLIT  (input) INTEGER array, dimension (N)   
            The splitting points, at which T breaks up into submatrices. 
  
            The first submatrix consists of rows/columns 1 to   
            ISPLIT( 1 ), the second of rows/columns ISPLIT( 1 )+1   
            through ISPLIT( 2 ), etc.   
            ( The output array ISPLIT from SSTEBZ is expected here. )   

    Z       (output) REAL array, dimension (LDZ, M)   
            The computed eigenvectors.  The eigenvector associated   
            with the eigenvalue W(i) is stored in the i-th column of   
            Z.  Any vector which fails to converge is set to its current 
  
            iterate after MAXITS iterations.   

    LDZ     (input) INTEGER   
            The leading dimension of the array Z.  LDZ >= max(1,N).   

    WORK    (workspace) REAL array, dimension (5*N)   

    IWORK   (workspace) INTEGER array, dimension (N)   

    IFAIL   (output) INTEGER array, dimension (M)   
            On normal exit, all elements of IFAIL are zero.   
            If one or more eigenvectors fail to converge after   
            MAXITS iterations, then their indices are stored in   
            array IFAIL.   

    INFO    (output) INTEGER   
            = 0: successful exit.   
            < 0: if INFO = -i, the i-th argument had an illegal value   
            > 0: if INFO = i, then i eigenvectors failed to converge   
                 in MAXITS iterations.  Their indices are stored in   
                 array IFAIL.   

    Internal Parameters   
    ===================   

    MAXITS  INTEGER, default = 5   
            The maximum number of iterations performed.   

    EXTRA   INTEGER, default = 2   
            The number of iterations performed after norm growth   
            criterion is satisfied, should be at least 1.   

    ===================================================================== 
  


       Test the input parameters.   

    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__2 = 2;
    static int c__1 = 1;
    static int c_n1 = -1;
    
    /* System generated locals */
/*  Unused variables commented out by MDG on 03-09-05
    int z_dim1, z_offset;
*/
    int i__1, i__2, i__3;
    real r__1, r__2, r__3, r__4, r__5;
    /* Builtin functions */
    double sqrt(doublereal);
    /* Local variables */
    static int jblk, nblk, jmax;
    extern doublereal sdot_(int *, real *, int *, real *, int *), 
	    snrm2_(int *, real *, int *);
    static int i, j, iseed[4], gpind, iinfo;
    extern /* Subroutine */ int sscal_(int *, real *, real *, int *);
    static int b1;
    extern doublereal sasum_(int *, real *, int *);
    static int j1;
    extern /* Subroutine */ int scopy_(int *, real *, int *, real *, 
	    int *);
    static real ortol;
    extern /* Subroutine */ int saxpy_(int *, real *, real *, int *, 
	    real *, int *);
    static int indrv1, indrv2, indrv3, indrv4, indrv5, bn;
    static real xj;
    extern doublereal slamch_(char *);
    extern /* Subroutine */ int xerbla_(char *, int *), slagtf_(
	    int *, real *, real *, real *, real *, real *, real *, 
	    int *, int *);
    static int nrmchk;
    extern int isamax_(int *, real *, int *);
    extern /* Subroutine */ int slagts_(int *, int *, real *, real *, 
	    real *, real *, int *, real *, real *, int *);
    static int blksiz;
    static real onenrm, pertol;
    extern /* Subroutine */ int slarnv_(int *, int *, int *, real 
	    *);
    static real stpcrt, scl, eps, ctr, sep, nrm, tol;
    static int its;
    static real xjm, eps1;



#define ISEED(I) iseed[(I)]
#define D(I) d[(I)-1]
#define E(I) e[(I)-1]
#define W(I) w[(I)-1]
#define IBLOCK(I) iblock[(I)-1]
#define ISPLIT(I) isplit[(I)-1]
#define WORK(I) work[(I)-1]
#define IWORK(I) iwork[(I)-1]
#define IFAIL(I) ifail[(I)-1]

#define Z(I,J) z[(I)-1 + ((J)-1)* ( *ldz)]

    *info = 0;
    i__1 = *m;
    for (i = 1; i <= *m; ++i) {
	IFAIL(i) = 0;
/* L10: */
    }

    if (*n < 0) {
	*info = -1;
    } else if (*m < 0 || *m > *n) {
	*info = -4;
    } else if (*ldz < max(1,*n)) {
	*info = -9;
    } else {
	i__1 = *m;
	for (j = 2; j <= *m; ++j) {
	    if (IBLOCK(j) < IBLOCK(j - 1)) {
		*info = -6;
		goto L30;
	    }
	    if (IBLOCK(j) == IBLOCK(j - 1) && W(j) < W(j - 1)) {
		*info = -5;
		goto L30;
	    }
/* L20: */
	}
L30:
	;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSTEIN", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *m == 0) {
	return 0;
    } else if (*n == 1) {
	Z(1,1) = 1.f;
	return 0;
    }

/*     Get machine constants. */

    eps = slamch_("Precision");

/*     Initialize seed for random number generator SLARNV. */

    for (i = 1; i <= 4; ++i) {
	ISEED(i - 1) = 1;
/* L40: */
    }

/*     Initialize pointers. */

    indrv1 = 0;
    indrv2 = indrv1 + *n;
    indrv3 = indrv2 + *n;
    indrv4 = indrv3 + *n;
    indrv5 = indrv4 + *n;

/*     Compute eigenvectors of matrix blocks. */

    j1 = 1;
    i__1 = IBLOCK(*m);
    for (nblk = 1; nblk <= IBLOCK(*m); ++nblk) {

/*        Find starting and ending indices of block nblk. */

	if (nblk == 1) {
	    b1 = 1;
	} else {
	    b1 = ISPLIT(nblk - 1) + 1;
	}
	bn = ISPLIT(nblk);
	blksiz = bn - b1 + 1;
	if (blksiz == 1) {
	    goto L60;
	}
	gpind = b1;

/*        Compute reorthogonalization criterion and stopping criterion
. */

	onenrm = (r__1 = D(b1), dabs(r__1)) + (r__2 = E(b1), dabs(r__2));
/* Computing MAX */
	r__3 = onenrm, r__4 = (r__1 = D(bn), dabs(r__1)) + (r__2 = E(bn - 1), 
		dabs(r__2));
	onenrm = dmax(r__3,r__4);
	i__2 = bn - 1;
	for (i = b1 + 1; i <= bn-1; ++i) {
/* Computing MAX */
	    r__4 = onenrm, r__5 = (r__1 = D(i), dabs(r__1)) + (r__2 = E(i - 1)
		    , dabs(r__2)) + (r__3 = E(i), dabs(r__3));
	    onenrm = dmax(r__4,r__5);
/* L50: */
	}
	ortol = onenrm * .001f;

	stpcrt = sqrt(.1f / blksiz);

/*        Loop through eigenvalues of block nblk. */

L60:
	jblk = 0;
	i__2 = *m;
	for (j = j1; j <= *m; ++j) {
	    if (IBLOCK(j) != nblk) {
		j1 = j;
		goto L160;
	    }
	    ++jblk;
	    xj = W(j);

/*           Skip all the work if the block size is one. */

	    if (blksiz == 1) {
		WORK(indrv1 + 1) = 1.f;
		goto L120;
	    }

/*           If eigenvalues j and j-1 are too close, add a relativ
ely   
             small perturbation. */

	    if (jblk > 1) {
		eps1 = (r__1 = eps * xj, dabs(r__1));
		pertol = eps1 * 10.f;
		sep = xj - xjm;
		if (sep < pertol) {
		    xj = xjm + pertol;
		}
	    }

	    its = 0;
	    nrmchk = 0;

/*           Get random starting vector. */

	    slarnv_(&c__2, iseed, &blksiz, &WORK(indrv1 + 1));

/*           Copy the matrix T so it won't be destroyed in factori
zation. */

	    scopy_(&blksiz, &D(b1), &c__1, &WORK(indrv4 + 1), &c__1);
	    i__3 = blksiz - 1;
	    scopy_(&i__3, &E(b1), &c__1, &WORK(indrv2 + 2), &c__1);
	    i__3 = blksiz - 1;
	    scopy_(&i__3, &E(b1), &c__1, &WORK(indrv3 + 1), &c__1);

/*           Compute LU factors with partial pivoting  ( PT = LU )
 */

	    tol = 0.f;
	    slagtf_(&blksiz, &WORK(indrv4 + 1), &xj, &WORK(indrv2 + 2), &WORK(
		    indrv3 + 1), &tol, &WORK(indrv5 + 1), &IWORK(1), &iinfo);

/*           Update iteration count. */

L70:
	    ++its;
	    if (its > 5) {
		goto L100;
	    }

/*           Normalize and scale the righthand side vector Pb.   

   Computing MAX */
	    r__2 = eps, r__3 = (r__1 = WORK(indrv4 + blksiz), dabs(r__1));
	    scl = blksiz * onenrm * dmax(r__2,r__3) / sasum_(&blksiz, &WORK(
		    indrv1 + 1), &c__1);
	    sscal_(&blksiz, &scl, &WORK(indrv1 + 1), &c__1);

/*           Solve the system LU = Pb. */

	    slagts_(&c_n1, &blksiz, &WORK(indrv4 + 1), &WORK(indrv2 + 2), &
		    WORK(indrv3 + 1), &WORK(indrv5 + 1), &IWORK(1), &WORK(
		    indrv1 + 1), &tol, &iinfo);

/*           Reorthogonalize by modified Gram-Schmidt if eigenvalu
es are   
             close enough. */

	    if (jblk == 1) {
		goto L90;
	    }
	    if ((r__1 = xj - xjm, dabs(r__1)) > ortol) {
		gpind = j;
	    }
	    if (gpind != j) {
		i__3 = j - 1;
		for (i = gpind; i <= j-1; ++i) {
		    ctr = -(doublereal)sdot_(&blksiz, &WORK(indrv1 + 1), &
			    c__1, &Z(b1,i), &c__1);
		    saxpy_(&blksiz, &ctr, &Z(b1,i), &c__1, &WORK(
			    indrv1 + 1), &c__1);
/* L80: */
		}
	    }

/*           Check the infinity norm of the iterate. */

L90:
	    jmax = isamax_(&blksiz, &WORK(indrv1 + 1), &c__1);
	    nrm = (r__1 = WORK(indrv1 + jmax), dabs(r__1));

/*           Continue for additional iterations after norm reaches
   
             stopping criterion. */

	    if (nrm < stpcrt) {
		goto L70;
	    }
	    ++nrmchk;
	    if (nrmchk < 3) {
		goto L70;
	    }

	    goto L110;

/*           If stopping criterion was not satisfied, update info 
and   
             store eigenvector number in array ifail. */

L100:
	    ++(*info);
	    IFAIL(*info) = j;

/*           Accept iterate as jth eigenvector. */

L110:
	    scl = 1.f / snrm2_(&blksiz, &WORK(indrv1 + 1), &c__1);
	    jmax = isamax_(&blksiz, &WORK(indrv1 + 1), &c__1);
	    if (WORK(indrv1 + jmax) < 0.f) {
		scl = -(doublereal)scl;
	    }
	    sscal_(&blksiz, &scl, &WORK(indrv1 + 1), &c__1);
L120:
	    i__3 = *n;
	    for (i = 1; i <= *n; ++i) {
		Z(i,j) = 0.f;
/* L130: */
	    }
	    i__3 = blksiz;
	    for (i = 1; i <= blksiz; ++i) {
		Z(b1+i-1,j) = WORK(indrv1 + i);
/* L140: */
	    }

/*           Save the shift to check eigenvalue spacing at next   
             iteration. */

	    xjm = xj;

/* L150: */
	}
L160:
	;
    }

    return 0;

/*     End of SSTEIN */

} /* sstein_ */
