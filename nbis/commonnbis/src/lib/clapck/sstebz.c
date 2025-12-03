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
* UPDATED: 08/16/2009 by BBandini - gcc 4.4.1 won't compile math function
                         when passed a constant; two mods: log(x20)
                         instead of log(float(2.0)). Also log(b20).
* ======================================================================
*/
#include <f2c.h>

/* Subroutine */ int sstebz_(char *range, char *order, int *n, real *vl, 
	real *vu, int *il, int *iu, real *abstol, real *d, real *e, 
	int *m, int *nsplit, real *w, int *iblock, int *
	isplit, real *work, int *iwork, int *info)
{
/*  -- LAPACK routine (version 2.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    SSTEBZ computes the eigenvalues of a symmetric tridiagonal   
    matrix T.  The user may ask for all eigenvalues, all eigenvalues   
    in the half-open interval (VL, VU], or the IL-th through IU-th   
    eigenvalues.   

    To avoid overflow, the matrix must be scaled so that its   
    largest element is no greater than overflow**(1/2) *   
    underflow**(1/4) in absolute value, and for greatest   
    accuracy, it should not be much smaller than that.   

    See W. Kahan "Accurate Eigenvalues of a Symmetric Tridiagonal   
    Matrix", Report CS41, Computer Science Dept., Stanford   
    University, July 21, 1966.   

    Arguments   
    =========   

    RANGE   (input) CHARACTER   
            = 'A': ("All")   all eigenvalues will be found.   
            = 'V': ("Value") all eigenvalues in the half-open interval   
                             (VL, VU] will be found.   
            = 'I': ("Index") the IL-th through IU-th eigenvalues (of the 
  
                             entire matrix) will be found.   

    ORDER   (input) CHARACTER   
            = 'B': ("By Block") the eigenvalues will be grouped by   
                                split-off block (see IBLOCK, ISPLIT) and 
  
                                ordered from smallest to largest within   
                                the block.   
            = 'E': ("Entire matrix")   
                                the eigenvalues for the entire matrix   
                                will be ordered from smallest to   
                                largest.   

    N       (input) INTEGER   
            The order of the tridiagonal matrix T.  N >= 0.   

    VL      (input) REAL   
    VU      (input) REAL   
            If RANGE='V', the lower and upper bounds of the interval to   
            be searched for eigenvalues.  Eigenvalues less than or equal 
  
            to VL, or greater than VU, will not be returned.  VL < VU.   
            Not referenced if RANGE = 'A' or 'I'.   

    IL      (input) INTEGER   
    IU      (input) INTEGER   
            If RANGE='I', the indices (in ascending order) of the   
            smallest and largest eigenvalues to be returned.   
            1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0.   
            Not referenced if RANGE = 'A' or 'V'.   

    ABSTOL  (input) REAL   
            The absolute tolerance for the eigenvalues.  An eigenvalue   
            (or cluster) is considered to be located if it has been   
            determined to lie in an interval whose width is ABSTOL or   
            less.  If ABSTOL is less than or equal to zero, then ULP*|T| 
  
            will be used, where |T| means the 1-norm of T.   

            Eigenvalues will be computed most accurately when ABSTOL is   
            set to twice the underflow threshold 2*SLAMCH('S'), not zero. 
  

    D       (input) REAL array, dimension (N)   
            The n diagonal elements of the tridiagonal matrix T.   

    E       (input) REAL array, dimension (N-1)   
            The (n-1) off-diagonal elements of the tridiagonal matrix T. 
  

    M       (output) INTEGER   
            The actual number of eigenvalues found. 0 <= M <= N.   
            (See also the description of INFO=2,3.)   

    NSPLIT  (output) INTEGER   
            The number of diagonal blocks in the matrix T.   
            1 <= NSPLIT <= N.   

    W       (output) REAL array, dimension (N)   
            On exit, the first M elements of W will contain the   
            eigenvalues.  (SSTEBZ may use the remaining N-M elements as   
            workspace.)   

    IBLOCK  (output) INTEGER array, dimension (N)   
            At each row/column j where E(j) is zero or small, the   
            matrix T is considered to split into a block diagonal   
            matrix.  On exit, if INFO = 0, IBLOCK(i) specifies to which   
            block (from 1 to the number of blocks) the eigenvalue W(i)   
            belongs.  (SSTEBZ may use the remaining N-M elements as   
            workspace.)   

    ISPLIT  (output) INTEGER array, dimension (N)   
            The splitting points, at which T breaks up into submatrices. 
  
            The first submatrix consists of rows/columns 1 to ISPLIT(1), 
  
            the second of rows/columns ISPLIT(1)+1 through ISPLIT(2),   
            etc., and the NSPLIT-th consists of rows/columns   
            ISPLIT(NSPLIT-1)+1 through ISPLIT(NSPLIT)=N.   
            (Only the first NSPLIT elements will actually be used, but   
            since the user cannot know a priori what value NSPLIT will   
            have, N words must be reserved for ISPLIT.)   

    WORK    (workspace) REAL array, dimension (4*N)   

    IWORK   (workspace) INTEGER array, dimension (3*N)   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
            > 0:  some or all of the eigenvalues failed to converge or   
                  were not computed:   
                  =1 or 3: Bisection failed to converge for some   
                          eigenvalues; these eigenvalues are flagged by a 
  
                          negative block number.  The effect is that the 
  
                          eigenvalues may not be as accurate as the   
                          absolute and relative tolerances.  This is   
                          generally caused by unexpectedly inaccurate   
                          arithmetic.   
                  =2 or 3: RANGE='I' only: Not all of the eigenvalues   
                          IL:IU were found.   
                          Effect: M < IU+1-IL   
                          Cause:  non-monotonic arithmetic, causing the   
                                  Sturm sequence to be non-monotonic.   
                          Cure:   recalculate, using RANGE='A', and pick 
  
                                  out eigenvalues IL:IU.  In some cases, 
  
                                  increasing the PARAMETER "FUDGE" may   
                                  make things work.   
                  = 4:    RANGE='I', and the Gershgorin interval   
                          initially used was too small.  No eigenvalues   
                          were computed.   
                          Probable cause: your machine has sloppy   
                                          floating-point arithmetic.   
                          Cure: Increase the PARAMETER "FUDGE",   
                                recompile, and try again.   

    Internal Parameters   
    ===================   

    RELFAC  REAL, default = 2.0e0   
            The relative tolerance.  An interval (a,b] lies within   
            "relative tolerance" if  b-a < RELFAC*ulp*max(|a|,|b|),   
            where "ulp" is the machine precision (distance from 1 to   
            the next larger floating point number.)   

    FUDGE   REAL, default = 2   
            A "fudge factor" to widen the Gershgorin intervals.  Ideally, 
  
            a value of 1 should work, but on machines with sloppy   
            arithmetic, this needs to be larger.  The default for   
            publicly released versions should be large enough to handle   
            the worst machine around.  Note that this has no effect   
            on accuracy of the solution.   

    ===================================================================== 
  


    
   Parameter adjustments   
       Function Body */
    /* Table of constant values */
    static int c__1 = 1;
    static int c_n1 = -1;
    static int c__3 = 3;
    static int c__2 = 2;
    static int c__0 = 0;
    
    /* System generated locals */
    int i__1, i__2, i__3;
    real r__1, r__2, r__3, r__4, r__5;
    /* Builtin functions */
    double sqrt(doublereal), log(doublereal);
    /* Local variables */
    static int iend, ioff, iout, itmp1, j, jdisc;
    extern logical lsame_(char *, char *);
    static int iinfo;
    static real atoli;
    static int iwoff;
    static real bnorm;
    static int itmax;
    static real wkill, rtoli, tnorm;
    static int ib, jb, ie, je, nb;
    static real gl;
    static int im, in, ibegin;
    static real gu;
    static int iw;
    static real wl;
    static int irange, idiscl;
    extern doublereal slamch_(char *);
    static real safemn, wu;
    static int idumma[1];
    extern int ilaenv_(int *, char *, char *, int *, int *, 
	    int *, int *, ftnlen, ftnlen);
    extern /* Subroutine */ int xerbla_(char *, int *);
    static int idiscu;
    extern /* Subroutine */ int slaebz_(int *, int *, int *, 
	    int *, int *, int *, real *, real *, real *, real *, 
	    real *, real *, int *, real *, real *, int *, int *, 
	    real *, int *, int *);
    static int iorder;
    static logical ncnvrg;
    static real pivmin;
    static logical toofew;
    static int nwl;
    static real ulp, wlu, wul;
    static int nwu;
    static real tmp1, tmp2;



#define IDUMMA(I) idumma[(I)]
#define IWORK(I) iwork[(I)-1]
#define WORK(I) work[(I)-1]
#define ISPLIT(I) isplit[(I)-1]
#define IBLOCK(I) iblock[(I)-1]
#define W(I) w[(I)-1]
#define E(I) e[(I)-1]
#define D(I) d[(I)-1]


    *info = 0;

/*     Decode RANGE */

    if (lsame_(range, "A")) {
	irange = 1;
    } else if (lsame_(range, "V")) {
	irange = 2;
    } else if (lsame_(range, "I")) {
	irange = 3;
    } else {
	irange = 0;
    }

/*     Decode ORDER */

    if (lsame_(order, "B")) {
	iorder = 2;
    } else if (lsame_(order, "E")) {
	iorder = 1;
    } else {
	iorder = 0;
    }

/*     Check for Errors */

    if (irange <= 0) {
	*info = -1;
    } else if (iorder <= 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (irange == 2 && *vl >= *vu) {
	*info = -5;
    } else if (irange == 3 && (*il < 1 || *il > max(1,*n))) {
	*info = -6;
    } else if (irange == 3 && (*iu < min(*n,*il) || *iu > *n)) {
	*info = -7;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("SSTEBZ", &i__1);
	return 0;
    }

/*     Initialize error flags */

    *info = 0;
    ncnvrg = FALSE_;
    toofew = FALSE_;

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

/*     Simplifications: */

    if (irange == 3 && *il == 1 && *iu == *n) {
	irange = 1;
    }

/*     Get machine constants   
       NB is the minimum vector length for vector bisection, or 0   
       if only scalar is to be done. */

    safemn = slamch_("S");
    ulp = slamch_("P");
    rtoli = ulp * 2.f;
    nb = ilaenv_(&c__1, "SSTEBZ", " ", n, &c_n1, &c_n1, &c_n1, 6L, 1L);
    if (nb <= 1) {
	nb = 0;
    }

/*     Special Case when N=1 */

    if (*n == 1) {
	*nsplit = 1;
	ISPLIT(1) = 1;
	if (irange == 2 && (*vl >= D(1) || *vu < D(1))) {
	    *m = 0;
	} else {
	    W(1) = D(1);
	    IBLOCK(1) = 1;
	    *m = 1;
	}
	return 0;
    }

/*     Compute Splitting Points */

    *nsplit = 1;
    WORK(*n) = 0.f;
    pivmin = 1.f;

    i__1 = *n;
    for (j = 2; j <= *n; ++j) {
/* Computing 2nd power */
	r__1 = E(j - 1);
	tmp1 = r__1 * r__1;
/* Computing 2nd power */
	r__2 = ulp;
	if ((r__1 = D(j) * D(j - 1), dabs(r__1)) * (r__2 * r__2) + safemn > 
		tmp1) {
	    ISPLIT(*nsplit) = j - 1;
	    ++(*nsplit);
	    WORK(j - 1) = 0.f;
	} else {
	    WORK(j - 1) = tmp1;
	    pivmin = dmax(pivmin,tmp1);
	}
/* L10: */
    }
    ISPLIT(*nsplit) = *n;
    pivmin *= safemn;

/*     Compute Interval and ATOLI */

    if (irange == 3) {

/*        RANGE='I': Compute the interval containing eigenvalues   
                     IL through IU.   

          Compute Gershgorin interval for entire (split) matrix   
          and use it as the initial interval */

	gu = D(1);
	gl = D(1);
	tmp1 = 0.f;

	i__1 = *n - 1;
	for (j = 1; j <= *n-1; ++j) {
	    tmp2 = sqrt(WORK(j));
/* Computing MAX */
	    r__1 = gu, r__2 = D(j) + tmp1 + tmp2;
	    gu = dmax(r__1,r__2);
/* Computing MIN */
	    r__1 = gl, r__2 = D(j) - tmp1 - tmp2;
	    gl = dmin(r__1,r__2);
	    tmp1 = tmp2;
/* L20: */
	}

/* Computing MAX */
	r__1 = gu, r__2 = D(*n) + tmp1;
	gu = dmax(r__1,r__2);
/* Computing MIN */
	r__1 = gl, r__2 = D(*n) - tmp1;
	gl = dmin(r__1,r__2);
/* Computing MAX */
	r__1 = dabs(gl), r__2 = dabs(gu);
	tnorm = dmax(r__1,r__2);
	gl = gl - tnorm * 2.f * ulp * *n - pivmin * 4.f;
	gu = gu + tnorm * 2.f * ulp * *n + pivmin * 2.f;

/*        Compute Iteration parameters */

        float x20 = 2.f;
	itmax = (int) ((log(tnorm + pivmin) - log(pivmin)) / log(x20)) + 2;

	if (*abstol <= 0.f) {
	    atoli = ulp * tnorm;
	} else {
	    atoli = *abstol;
	}

	WORK(*n + 1) = gl;
	WORK(*n + 2) = gl;
	WORK(*n + 3) = gu;
	WORK(*n + 4) = gu;
	WORK(*n + 5) = gl;
	WORK(*n + 6) = gu;
	IWORK(1) = -1;
	IWORK(2) = -1;
	IWORK(3) = *n + 1;
	IWORK(4) = *n + 1;
	IWORK(5) = *il - 1;
	IWORK(6) = *iu;

	slaebz_(&c__3, &itmax, n, &c__2, &c__2, &nb, &atoli, &rtoli, &pivmin, 
		&D(1), &E(1), &WORK(1), &IWORK(5), &WORK(*n + 1), &WORK(*n + 
		5), &iout, &IWORK(1), &W(1), &IBLOCK(1), &iinfo);

	if (IWORK(6) == *iu) {
	    wl = WORK(*n + 1);
	    wlu = WORK(*n + 3);
	    nwl = IWORK(1);
	    wu = WORK(*n + 4);
	    wul = WORK(*n + 2);
	    nwu = IWORK(4);
	} else {
	    wl = WORK(*n + 2);
	    wlu = WORK(*n + 4);
	    nwl = IWORK(2);
	    wu = WORK(*n + 3);
	    wul = WORK(*n + 1);
	    nwu = IWORK(3);
	}

	if (nwl < 0 || nwl >= *n || nwu < 1 || nwu > *n) {
	    *info = 4;
	    return 0;
	}
    } else {

/*        RANGE='A' or 'V' -- Set ATOLI   

   Computing MAX */
	r__3 = dabs(D(1)) + dabs(E(1)), r__4 = (r__1 = D(*n), dabs(r__1)) + (
		r__2 = E(*n - 1), dabs(r__2));
	tnorm = dmax(r__3,r__4);

	i__1 = *n - 1;
	for (j = 2; j <= *n-1; ++j) {
/* Computing MAX */
	    r__4 = tnorm, r__5 = (r__1 = D(j), dabs(r__1)) + (r__2 = E(j - 1),
		     dabs(r__2)) + (r__3 = E(j), dabs(r__3));
	    tnorm = dmax(r__4,r__5);
/* L30: */
	}

	if (*abstol <= 0.f) {
	    atoli = ulp * tnorm;
	} else {
	    atoli = *abstol;
	}

	if (irange == 2) {
	    wl = *vl;
	    wu = *vu;
	}
    }

/*     Find Eigenvalues -- Loop Over Blocks and recompute NWL and NWU.   
       NWL accumulates the number of eigenvalues .le. WL,   
       NWU accumulates the number of eigenvalues .le. WU */

    *m = 0;
    iend = 0;
    *info = 0;
    nwl = 0;
    nwu = 0;

    i__1 = *nsplit;
    for (jb = 1; jb <= *nsplit; ++jb) {
	ioff = iend;
	ibegin = ioff + 1;
	iend = ISPLIT(jb);
	in = iend - ioff;

	if (in == 1) {

/*           Special Case -- IN=1 */

	    if (irange == 1 || wl >= D(ibegin) - pivmin) {
		++nwl;
	    }
	    if (irange == 1 || wu >= D(ibegin) - pivmin) {
		++nwu;
	    }
/*
	    if (irange == 1 || wl < D(ibegin) - pivmin && wu >= D(ibegin) - 
		    pivmin) {
*/
/*          Paretheses added by MDG on 03-09-05 */
	    if (irange == 1 ||
                (wl < D(ibegin) - pivmin && wu >= D(ibegin) - pivmin)) {
		++(*m);
		W(*m) = D(ibegin);
		IBLOCK(*m) = jb;
	    }
	} else {

/*           General Case -- IN > 1   

             Compute Gershgorin Interval   
             and use it as the initial interval */

	    gu = D(ibegin);
	    gl = D(ibegin);
	    tmp1 = 0.f;

	    i__2 = iend - 1;
	    for (j = ibegin; j <= iend-1; ++j) {
		tmp2 = (r__1 = E(j), dabs(r__1));
/* Computing MAX */
		r__1 = gu, r__2 = D(j) + tmp1 + tmp2;
		gu = dmax(r__1,r__2);
/* Computing MIN */
		r__1 = gl, r__2 = D(j) - tmp1 - tmp2;
		gl = dmin(r__1,r__2);
		tmp1 = tmp2;
/* L40: */
	    }

/* Computing MAX */
	    r__1 = gu, r__2 = D(iend) + tmp1;
	    gu = dmax(r__1,r__2);
/* Computing MIN */
	    r__1 = gl, r__2 = D(iend) - tmp1;
	    gl = dmin(r__1,r__2);
/* Computing MAX */
	    r__1 = dabs(gl), r__2 = dabs(gu);
	    bnorm = dmax(r__1,r__2);
	    gl = gl - bnorm * 2.f * ulp * in - pivmin * 2.f;
	    gu = gu + bnorm * 2.f * ulp * in + pivmin * 2.f;

/*           Compute ATOLI for the current submatrix */

	    if (*abstol <= 0.f) {
/* Computing MAX */
		r__1 = dabs(gl), r__2 = dabs(gu);
		atoli = ulp * dmax(r__1,r__2);
	    } else {
		atoli = *abstol;
	    }

	    if (irange > 1) {
		if (gu < wl) {
		    nwl += in;
		    nwu += in;
		    goto L70;
		}
		gl = dmax(gl,wl);
		gu = dmin(gu,wu);
		if (gl >= gu) {
		    goto L70;
		}
	    }

/*           Set Up Initial Interval */

	    WORK(*n + 1) = gl;
	    WORK(*n + in + 1) = gu;
	    slaebz_(&c__1, &c__0, &in, &in, &c__1, &nb, &atoli, &rtoli, &
		    pivmin, &D(ibegin), &E(ibegin), &WORK(ibegin), idumma, &
		    WORK(*n + 1), &WORK(*n + (in << 1) + 1), &im, &IWORK(1), &
		    W(*m + 1), &IBLOCK(*m + 1), &iinfo);

	    nwl += IWORK(1);
	    nwu += IWORK(in + 1);
	    iwoff = *m - IWORK(1);

/*           Compute Eigenvalues */
	    float b20 = 2.f;
	    itmax = (int) ((log(gu - gl + pivmin) - log(pivmin)) / log(b20)) + 2;
	    slaebz_(&c__2, &itmax, &in, &in, &c__1, &nb, &atoli, &rtoli, &
		    pivmin, &D(ibegin), &E(ibegin), &WORK(ibegin), idumma, &
		    WORK(*n + 1), &WORK(*n + (in << 1) + 1), &iout, &IWORK(1),
		     &W(*m + 1), &IBLOCK(*m + 1), &iinfo);

/*           Copy Eigenvalues Into W and IBLOCK   
             Use -JB for block number for unconverged eigenvalues.
 */

	    i__2 = iout;
	    for (j = 1; j <= iout; ++j) {
		tmp1 = (WORK(j + *n) + WORK(j + in + *n)) * .5f;

/*              Flag non-convergence. */

		if (j > iout - iinfo) {
		    ncnvrg = TRUE_;
		    ib = -jb;
		} else {
		    ib = jb;
		}
		i__3 = IWORK(j + in) + iwoff;
		for (je = IWORK(j) + 1 + iwoff; je <= IWORK(j+in)+iwoff; ++je) {
		    W(je) = tmp1;
		    IBLOCK(je) = ib;
/* L50: */
		}
/* L60: */
	    }

	    *m += im;
	}
L70:
	;
    }

/*     If RANGE='I', then (WL,WU) contains eigenvalues NWL+1,...,NWU   
       If NWL+1 < IL or NWU > IU, discard extra eigenvalues. */

    if (irange == 3) {
	im = 0;
	idiscl = *il - 1 - nwl;
	idiscu = nwu - *iu;

	if (idiscl > 0 || idiscu > 0) {
	    i__1 = *m;
	    for (je = 1; je <= *m; ++je) {
		if (W(je) <= wlu && idiscl > 0) {
		    --idiscl;
		} else if (W(je) >= wul && idiscu > 0) {
		    --idiscu;
		} else {
		    ++im;
		    W(im) = W(je);
		    IBLOCK(im) = IBLOCK(je);
		}
/* L80: */
	    }
	    *m = im;
	}
	if (idiscl > 0 || idiscu > 0) {

/*           Code to deal with effects of bad arithmetic:   
             Some low eigenvalues to be discarded are not in (WL,W
LU],   
             or high eigenvalues to be discarded are not in (WUL,W
U]   
             so just kill off the smallest IDISCL/largest IDISCU 
  
             eigenvalues, by simply finding the smallest/largest 
  
             eigenvalue(s).   

             (If N(w) is monotone non-decreasing, this should neve
r   
                 happen.) */

	    if (idiscl > 0) {
		wkill = wu;
		i__1 = idiscl;
		for (jdisc = 1; jdisc <= idiscl; ++jdisc) {
		    iw = 0;
		    i__2 = *m;
		    for (je = 1; je <= *m; ++je) {
			if (IBLOCK(je) != 0 && (W(je) < wkill || iw == 0)) {
			    iw = je;
			    wkill = W(je);
			}
/* L90: */
		    }
		    IBLOCK(iw) = 0;
/* L100: */
		}
	    }
	    if (idiscu > 0) {

		wkill = wl;
		i__1 = idiscu;
		for (jdisc = 1; jdisc <= idiscu; ++jdisc) {
		    iw = 0;
		    i__2 = *m;
		    for (je = 1; je <= *m; ++je) {
			if (IBLOCK(je) != 0 && (W(je) > wkill || iw == 0)) {
			    iw = je;
			    wkill = W(je);
			}
/* L110: */
		    }
		    IBLOCK(iw) = 0;
/* L120: */
		}
	    }
	    im = 0;
	    i__1 = *m;
	    for (je = 1; je <= *m; ++je) {
		if (IBLOCK(je) != 0) {
		    ++im;
		    W(im) = W(je);
		    IBLOCK(im) = IBLOCK(je);
		}
/* L130: */
	    }
	    *m = im;
	}
	if (idiscl < 0 || idiscu < 0) {
	    toofew = TRUE_;
	}
    }

/*     If ORDER='B', do nothing -- the eigenvalues are already sorted   
          by block.   
       If ORDER='E', sort the eigenvalues from smallest to largest */

    if (iorder == 1 && *nsplit > 1) {
	i__1 = *m - 1;
	for (je = 1; je <= *m-1; ++je) {
	    ie = 0;
	    tmp1 = W(je);
	    i__2 = *m;
	    for (j = je + 1; j <= *m; ++j) {
		if (W(j) < tmp1) {
		    ie = j;
		    tmp1 = W(j);
		}
/* L140: */
	    }

	    if (ie != 0) {
		itmp1 = IBLOCK(ie);
		W(ie) = W(je);
		IBLOCK(ie) = IBLOCK(je);
		W(je) = tmp1;
		IBLOCK(je) = itmp1;
	    }
/* L150: */
	}
    }

    *info = 0;
    if (ncnvrg) {
	++(*info);
    }
    if (toofew) {
	*info += 2;
    }
    return 0;

/*     End of SSTEBZ */

} /* sstebz_ */
