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


/***********************************************************************
      LIBRARY: MLP - Multi-Layer Perceptron Neural Network

      FILE:    LBFGS.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      This is a C version of Nocedal's LBFGS routine, Limited-Memory BFGS
      optimizer, and its supporting routines LB1, MCSRCH, and MCSTEP.  It
      was translated by gtc from a provided Fortran version (which had
      already been modified by jlb), by a combination of editing, and
      running the f2c Fortran-to-C converter program.

      (The parm EPS which was in the Fortran version of LBFGS() is not
      present in this version.  It was removed because, in the Fortran
      version (as modified by jlb), it is not used.)

      ROUTINES:
#cat: lbfgs - Limited Memory BFGS Method for Large Scale Optimization by
#cat:         Jorge Nocedal
#cat: lb1 - Prints monitoring information w.r.t. lbfgs optimization.
#cat: mcsrch - Finds a step which satisfies a sufficient decrease condition
#cat:          and a curvature condition.
#cat: mcstep - Computes a safeguarded step for a linesearch and to update an
#cat:          interval of uncertainty for a minimizer of the function.
      
***********************************************************************/


#include <mlp.h>
#include <mlpcla.h>
/*
#include <mlp/blas.h>
*/

/* To activate surveying, uncomment following line: */
/* #define SURVEY */

/*******************************************************************/

  /* --------------
     function lbfgs
     --------------

     Limited Memory BFGS Method for Large Scale Optimization
                        Jorge Nocedal
                      *** July 1990 ***

  This function solves the unconstrained minimization problem

            min f(x),    x = (x[0],x[1],...,x[n-1]),

  using the limited memory BFGS method.  The routine is especially
  effective on problems involving a large number of variables.  In
  a typical iteration of this method an approximation Hk to the
  inverse of the Hessian is obtained by applying m BFGS updates to
  a diagonal matrix Hk0, using information from the previous m steps.
  The user specifies the number m, which determines the amount of
  storage required by the routine.  The user may also provide the
  diagonal matrices Hk0 if not satisfied with the default choice.

  The algorithm is described in "On the limited memory BFGS method
  for large scale optimization", by D. Liu and J. Nocedal,
  Mathematical Programming B 45 (1989) 503-528.

  The user is required to calculate the function value f and its
  gradient g.  In order to allow the user complete control over these
  computations, reverse communication is used.  The routine must be
  called repeatedly under the control of the parameter iflag.

  The steplength is determined at each iteration by means of the
  line search routine mcsrch, which is a slight modification of
  the routine CSRCH written by More' and Thuente.

  The calling statement is:

    lbfgs(n, m, x, f, g, diagco, diag, iprint, xtol, work, iflag,
      info, fp_mon, fp_err, gtol, stpmin, stpmax, itopt, iter, ierr,
      stp);

  where:

  [(input) int]
  n       is an int variable that must be set by the user to the
          number of variables.  It is not altered by the routine.
          Restriction: n > 0.

  [(input) int]
  m       is an int variable that must be set by the user to the
          number of corrections used in the bfgs update.  It is not
          altered by the routine. Values of m less than 3 are not
          recommended; large values of m will result in excessive
          computing time.  3 <= m <= 7 is recommended.
          Restriction: m > 0.

  [float vector]
  x       is a float array of length n.  On initial entry it must be
          set by the user to the values of the initial estimate of the
          solution vector.  On exit with *iflag = 0, it contains the
          values of the variables at the best point found (usually a
          solution).

  [(input) float]
  f       is a float variable.  Before initial entry and on a re-entry
          with *iflag = 1, it must be set by the user to contain the
          value of the function f at the point x.

  [float vector]
  g       is a float array of length n.  Before initial entry and on a
          re-entry with *iflag = 1, it must be set by the user to
          contain the components of the gradient at the point x.

  [(input) int]
  diagco  is an int variable that must be set to TRUE if the user
          wishes to provide the diagonal matrix Hk0 at each iteration.
          Otherwise it should be set to FALSE, in which case lbfgs
          will use a default value described below.  If diagco is set
          to TRUE the routine will return at each iteration of the
          algorithm with *iflag = 2, and the diagonal matrix Hk0 must
          be provided in the array diag.

  [float vector]
  diag    is a float array of length n.  If diagco is TRUE, then on
          initial entry or on re-entry with *iflag = 2, diag must be
          set by the user to contain the values of the diagonal matrix
          Hk0.  Restriction: all elements of diag must be positive.

  [int vector]
  iprint  is an int array of length two which must be set by the
          user as follows:

          iprint[0] specifies the frequency of the output:
             iprint[0] < 0 : no output is generated.
             iprint[0] = 0 : output only at first and last iteration.
             iprint[0] > 0 : output every iprint[0] iterations.

          iprint[1] specifies the type of output generated:
             iprint[1] = 0 : iteration count, number of function
                             evaluations, function value, norm of the
                             gradient, and steplength.
             iprint[1] = 1 : same as iprint[1] = 0, plus vector of
                             variables and  gradient vector at the
                             initial point.
             iprint[1] = 2 : same as iprint[1] = 1, plus vector of
                             variables.
             iprint[1] = 3 : same as iprint[1] = 2, plus gradient
                             vector.

  [(input) float]
  xtol    is a positive float variable that must be set by the user to
          an estimate of the machine precision (e.g. 10^(-7) on a SUN
          station 3/60).  The line search routine will terminate if
          the relative width of the interval of uncertainty is less
          than xtol.

  [float vector]
  work    is a float array of length n(2m+1)+2m used as workspace for
          lbfgs.  This array must not be altered by the user.

  [(input/output) int-pointer]
  iflag   is an int-pointer, used for input and output of an int
          defined in the calling routine; *iflag must be 0 on initial
          entry to lbfgs.  A return with *iflag < 0 indicates an error,
          and *iflag = 0 indicates that the routine has terminated
          without detecting errors.  On a return with *iflag = 1, the
          user must compute the function value f and gradient g.  On
          a return with *iflag = 2, the user must provide the diagonal
          matrix Hk0.

          The following negative values of *iflag, detecting an error,
          are possible:

          *iflag = -1  The line search routine mcsrch failed.  The
                       output parameter "info" provides more detailed
                       information (see also the documentation of
                       mcsrch):

                       *info = 0  Improper input parameters.

                       *info = 2  Relative width of the interval of
                                  uncertainty is at most xtol.

                       *info = 3  More than maxfev function evaluations
                                  were required at the present
                                  iteration.

                       *info = 4  The step is too small.

                       *info = 5  The step is too large.

                       *info = 6  Rounding errors prevent further
                                  progress.  There may not be a step
                                  which satisfies the sufficient
                                  decrease and curvature conditions.
                                  Tolerances may be too small.

          *iflag = -2  The i-th diagonal element of the diagonal
                       inverse Hessian approximation, given in diag,
                       is not positive.

          *iflag = -3  Improper input parameters for lbfgs (n or m is
                       not positive).

  [Parms info through stpmax added by gtc: info was always returned
  by mcsrch but was not returned by lbfgs, and fp_mon, fp_err, gtol,
  stpmin, and stpmax were global variables in the LB3 common block
  in the Fortran version.]

  [(output) int-pointer]
  info   returns the info value returned by mcsrch.  This is of
         interest only if *iflag = -1.

  [(input) FILE-pointer]
  fp_mon will be used for the writing of monitoring information, as
         controlled by iprint.

  [(input) FILE-pointer]
  fp_err will be used for the writing of error messages.  This writing
         may be suppressed by setting fp_err to (FILE *)NULL.

  [(input) float]
  gtol   controls the accuracy of the line search routine mcsrch.  If
         the function and gradient evaluations are inexpensive with
         respect to the cost of the iteration (which is sometimes the
         case when solving very large problems) it may be advantageous
         to set gtol to a small value.  A typical small value is 0.1.
         Restriction: gtol should be greater than 1.e-04.

  [(input) float]
  stpmin and stpmax
         are nonnegative variables which specify lower and upper
         bounds for the step in the line search.  The recommended
         values (in original Fortran version, default values) are
         1.e-20 and 1.e+20.  These values need not be modified unless
         the exponents are too large for the machine being used, or
         unless the problem is extremely badly scaled (in which case
         the exponents should be increased). 

  [Remaining parms added by jlb:]

  [int-pointer]
  itopt  is ? [This is just passed to optchk as its second arg,
    which is supposed to be how many iterations the optimization
    has used so far.  Should eventually be changed to an int, not
    int-pointer.]

  [int-pointer]
  iter   is ?

  [(output) int-pointer]
  ierr   returns the ierr value returned by optchk().  [Note that
         depending on *ierr, *iflag can (?) be set differently than
         indicated in its comment above, which therefore should be
         modified.]

  [float-pointer]
  stp    is ?


  Machine Dependencies:

    The only variables that are machine-dependent are xtol, stpmin
    and stpmax. 


  General Information:

    Other routines called directly:  saxpy (BLAS), sdot (BLAS),
      lb1 (code in this file), mcsrch (code in this file).

    Input/Output:  no input; diagnostic messages to fp_mon and
      error messages to fp_err. */

void lbfgs(int n, int m, float *x, float f, float *g, int diagco,
           float *diag, int *iprint, float xtol, float *work, int *iflag,
           int *info, FILE *fp_mon, FILE *fp_err, float gtol, float stpmin,
           float stpmax, int *itopt, int *iter, int *ierr, float *stp)
{
#ifdef SURVEY
  char str[50];
  void survey();
#endif  
  static int inmc, iscn, nfev, iycn, nfun, ispt, iypt, i, bound,
    point, cp, finish, maxfev, npt, one = 1;
  float r1;
  static float beta, ftol, gnorm, xnorm, sq, yr, ys, yy, pctmin_junk,
    stp1;

  /* parameter adjustments */
  --work;
  --iprint;
  --diag;
  --g;
  --x;

  /* Initialize
     ---------- */
  if(*iflag == 0)
    goto L10;
  switch ((int)(*iflag)) {
  case 1:  goto L172;
  case 2:  goto L100;
  }
 L10:
  *iter = 0;
  if(n <= 0 || m <= 0)
    goto L196;
  if(gtol <= 1.e-4) {
    if(fp_err != (FILE *)NULL)
      fprintf(fp_err, "\n gtol is less than or equal to 1.e-04 ;\n\
 it has been reset to 9.e-01 .\n");
    gtol = .9;
  }
  nfun = 1;
  point = 0;
  finish = FALSE;
  if(diagco) {
    for(i = 1; i <= n; i++)
      if(diag[i] <= 0.)
	goto L195;
  }
  else
    for(i = 1; i <= n; i++)
      diag[i] = 1.;

  /* The work vector w is divided as follows:
     ----------------------------------------
     Locations 1,...,n are used to store the gradient and other
       temporary information.
     Locations n+1,...,n+m store the scalars rho.
     Locations n+m+1,...,n+2m store the numbers alpha used in the
       formula that computes h*g.
     Locations n+2m+1,...,n+2m+nm store the last m search steps.
     Locations n+2m+nm+1,...,n+2m+2nm store the last m gradient
       differences.

     The search steps and gradient differences are stored in a
     circular order controlled by the parameter point. */

  ispt = n + (m << 1);
  iypt = ispt + n * m;
  for(i = 1; i <= n; i++)
    work[ispt + i] = -g[i] * diag[i];
  gnorm = mlp_snrm2(n, &g[1], one);
  stp1 = 1. / gnorm;

  /* parameters for line search routine */
  /* ftol = 1.e-4; */
  /* maxfev = 20; */
  /* jlb: */
  ftol = 1.e-10;
  maxfev = 5;

  if(iprint[1] >= 0)
    lb1(&iprint[1], *iter, nfun, gnorm, n, m, &x[1], f, &g[1], *stp,
      finish, fp_mon);

  /* -------------------
     Main iteration loop
     ------------------- */

 L80:
  (*iter)++;
  *info = 0;
  bound = *iter - 1;
  if(*iter == 1)
    goto L165;
  if(*iter > m)
    bound = m;

  ys = mlp_sdot(n, &work[iypt + npt + 1], one, &work[ispt + npt + 1], one);
  if(!diagco) {
    yy = mlp_sdot(n, &work[iypt + npt + 1], one, &work[iypt + npt + 1], one);
    for(i = 1; i <= n; i++)
      diag[i] = ys / yy;
  }
  else {
    *iflag = 2;
    return;
  }
 L100:
  if(diagco)
    for(i = 1; i <= n; i++)
      if(diag[i] <= 0.)
	goto L195;

  /* -----------------------------------------------------------
     Compute -H*G using the formula given in:  Nocedal, J. 1980,
     "Updating quasi-Newton matrices with limited storage",
     Mathematics of Computation, Vol. 24, No. 151, pp. 773-782.
     ----------------------------------------------------------- */
  cp = point;
  if(point == 0)
    cp = m;
  work[n + cp] = 1. / ys;
  for(i = 1; i <= n; i++)
    work[i] = -g[i];
  cp = point;
  for(i = 1; i <= bound; i++) {
    --cp;
    if(cp == -1)
      cp = m - 1;
    sq = mlp_sdot(n, &work[ispt + cp * n + 1], one, &work[1], one);
    inmc = n + m + cp + 1;
    iycn = iypt + cp * n;
    work[inmc] = work[n + cp + 1] * sq;
    r1 = -work[inmc];
    mlp_saxpy(n, r1, &work[iycn + 1], one, &work[1], one);
  }

  for(i = 1; i <= n; i++)
    work[i] = diag[i] * work[i];

  for(i = 1; i <= bound; i++) {
    yr = mlp_sdot(n, &work[iypt + cp * n + 1], one, &work[1], one);
    beta = work[n + cp + 1] * yr;
    inmc = n + m + cp + 1;
    beta = work[inmc] - beta;
    iscn = ispt + cp * n;
    mlp_saxpy(n, beta, &work[iscn + 1], one, &work[1], one);
    if(++cp == m)
      cp = 0;
  }

  /* -------------------------------
     Store the new search direction.
     ------------------------------- */
  for(i = 1; i <= n; i++)
    work[ispt + point * n + i] = work[i];

  /* -------------------------------------------------------------
     Obtain the one-dimensional minimizer of the function by using
     the line search routine mcsrch.
     ------------------------------------------------------------- */
 L165:
  nfev = 0;
  *stp = 1.;
  if(*iter == 1)
    *stp = stp1;
  for(i = 1; i <= n; i++)
    work[i] = g[i];

#ifdef SURVEY
  survey(n, &x[1], &work[ispt + point * n + 1], *stp);
  sprintf(str, " calling mcsrch with *stp %e\n\n", *stp);
  fsaso(str);
#endif

 L172:
  mcsrch(n, &x[1], f, &g[1], &work[ispt + point * n + 1], stp, ftol,
    gtol, xtol, stpmin, stpmax, maxfev, info, &nfev, &diag[1], fp_err);
  if(*info == -1) {
    *iflag = 1;
    return;
  }
  if(*info != 1)
    goto L190;
  nfun += nfev;

  /* -----------------------------------------
     Compute the new step and gradient change.
     ----------------------------------------- */
  npt = point * n;
  for(i = 1; i <= n; i++) {
    work[ispt + npt + i] = *stp * work[ispt + npt + i];
    work[iypt + npt + i] = g[i] - work[i];
  }
  if(++point == m)
    point = 0;

  /* -----------------
     Termination test.
     ---------------- */
  gnorm = mlp_snrm2(n, &g[1], one);
  xnorm = mlp_snrm2(n, &x[1], one);
  xnorm =  (int)mlp_max(1., xnorm);
  /* jlb change:  The original Fortran had
      IF (GNORM/XNORM .LE. EPS) FINISH=.TRUE.
  here, but now the gnorm/xnorm termination is handled by
  optchk(). */

  if(iprint[1] >= 0)
    lb1(&iprint[1], *iter, nfun, gnorm, n, m, &x[1], f, &g[1],
      *stp, finish, fp_mon);
  optchk(FALSE, *itopt, &x[1], f, ierr, &pctmin_junk);
  if(*ierr != 0) {
    *iflag = 10;
    return;
  }
  if(finish) {
    *iflag = 0;
    return;
  }
  goto L80;

  /* -----------------------------------------
     End of main iteration loop.  Error exits.
     ----------------------------------------- */
 L190:
  *iflag = -1;
  if(fp_err != (FILE *)NULL)
    fprintf(fp_err, "\n *iflag == -1\n line search failed. see \
documentation of routine mcsrch\n error return of line search: \
*info = %d\n possible causes: function or gradient are incorrect or \
incorrect tolerances\n", *info);
  return;
 L195:
  *iflag = -2;
  if(fp_err != (FILE *)NULL)
    fprintf(fp_err, "\n *iflag == -2\n the %d-th diagonal element of \
the\n inverse Hessian approximation is not positive\n", i);
  return;
 L196:
  *iflag = -3;
  if(fp_err != (FILE *)NULL)
    fprintf(fp_err, "\n *iflag == -3\n improper input parameters \
(n or m is not positive)\n");
  return;
}

/* end of function lbfgs */

/*******************************************************************/

  /* ------------
     function lb1
     ------------

  Prints monitoring information.  The frequency and amount of output
  are controlled by iprint.
  */
/*
  Input args:
    iprint: Array of two ints, controlling printing.
    iter:
    nfun:
    gnorm:
    n:
    m:
    x:
    f:
    g:
    stp:
    finish:
    fp_mon;
*/

void lb1(int *iprint, int iter, int nfun, float gnorm, int n, int m,
         float *x, float f, float *g, float stp, int finish, FILE *fp_mon)
{
  int i;
  static int one = 1;
  static float gbyx, xnorm;

  /* parameter adjustments */
  --g;
  --x;
  --iprint;

  /* jlb added next line (gtc changed use of sdot_ and sqrt, to a call
  of snrm2): */
  xnorm = mlp_snrm2(n, &x[1], one);

  gbyx = 0.;
  if(xnorm > 0.)
    gbyx = gnorm / xnorm;
  if(iter == 0) {
    fprintf(fp_mon, "**********************************************\
***\n");
    fprintf(fp_mon, "  n = %d   number of corrections = %d\n\
       initial values:\n", n, m);
    fprintf(fp_mon, " f = %10.3e   gnorm = %10.3e\n", f, gnorm);
    if(iprint[2] >= 1) {
      fprintf(fp_mon, " vector x = ");
      for(i = 0; i < n; i++)
	fprintf(fp_mon, "  %10.3e", x[i]);
      fprintf(fp_mon, "\n");
      fprintf(fp_mon, " gradient vector g = ");
      for(i = 0; i < n; i++)
	fprintf(fp_mon, "  %10.3e", g[i]);
      fprintf(fp_mon, "\n");
    }
    fprintf(fp_mon, "**********************************************\
***\n");
    fprintf(fp_mon, "\n   i   nfn    func        gnorm       \
steplength\n");
  }
  else {
    if(iprint[1] == 0 && (iter != 1 && !finish))
      return;
    if(iprint[1] != 0) {
      if((iter - 1) % iprint[1] == 0 || finish) {
	if(iprint[2] > 1 && iter > 1)
	  fprintf(fp_mon, "\n   i   nfn    func        gnorm       \
steplength\n");
	fprintf(fp_mon, "%d %d    %10.3e  %10.3e  %10.3e  %10.3e\n",
          iter, nfun, f, gnorm, stp, gbyx);
      }
      else
	return;
    }
    else {
      if(iprint[2] > 1 && finish)
	fprintf(fp_mon, "\n   i   nfn    func        gnorm       \
steplength\n");
      fprintf(fp_mon, "%d %d    %10.3e  %10.3e  %10.3e  %10.3e\n",
          iter, nfun, f, gnorm, stp, gbyx);
    }      
    if(iprint[2] == 2 || iprint[2] == 3) {
      if(finish)
	fprintf(fp_mon, " final point x = ");
      else
	fprintf(fp_mon, " vector x = ");
      for(i = 0; i < n; i++)
	fprintf(fp_mon, "  %10.3e", x[i]);
      fprintf(fp_mon, "\n");
      if(iprint[2] == 3) {
	fprintf(fp_mon, " gradient vector g = ");
	for(i = 0; i < n; i++)
	  fprintf(fp_mon, "  %10.3e", g[i]);
	fprintf(fp_mon, "\n");
      }
    }
    if(finish)
      fprintf(fp_mon, "\n the minimization terminated without \
detecting errors.\n iflag = 0\n");
  }
  return;
}

/* end of function lb1 */

/*******************************************************************/

  /* ---------------
     function mcsrch
     ---------------

  A slight modification of the subroutine CSRCH of More' and Thuente.
  The changes are to allow reverse communication, and do not affect
  the performance of the routine.

  The purpose of mcsrch is to find a step which satisfies a sufficient
  decrease condition and a curvature condition.

  At each stage mcsrch updates an interval of uncertainty with
  endpoints stx and sty.  The interval of uncertainty is initially
  chosen so that it contains a minimizer of the modified function

       f(x+stp*s) - f(x) - ftol*stp*(gradf(x)'s).

  If a step is obtained for which the modified function has a
  nonpositive function value and nonnegative derivative, then the
  interval of uncertainty is chosen so that it contains a minimizer
  of f(x+stp*s).

  The algorithm is designed to find a step which satisfies the
  sufficient decrease condition

        f(x+stp*s) <= f(x) + ftol*stp*(gradf(x)'s),

  and the curvature condition

        abs(gradf(x+stp*s)'s)) <= gtol*abs(gradf(x)'s).

  If ftol is less than gtol and if, for example, the function is
  bounded below, then there is always a step which satisfies both
  conditions.  If no step can be found which satisfies both
  conditions, then the algorithm usually stops when rounding errors
  prevent further progress.  In this case stp only satisfies the
  sufficient decrease condition.

  The calling statement is

     mcsrch(n, x, f, g, s, stp, ftol, gtol, xtol, stpmin, stpmax,
       maxfev, info, nfev, wa, fp_err);

  where

    n is a positive integer input variable set to the number
      of variables.

    x is an array of length n.  On input it must contain the
      base point for the line search.  On output it contains
      x + stp*s.

    f is a variable.  On input it must contain the value of f
      at x.  On output it contains the value of f at x + stp*s.

    g is an array of length n.  On input it must contain the
      gradient of f at x.  On output it contains the gradient
      of f at x + stp*s.

    s is an input array of length n which specifies the
      search direction.

    stp is a nonnegative variable.  On input stp contains an
      initial estimate of a satisfactory step.  On output
      stp contains the final estimate.

    ftol and gtol are nonnegative input variables.
      Termination occurs when the sufficient decrease
      condition and the directional derivative condition are
      satisfied.

    xtol is a nonnegative input variable.  Termination occurs
      when the relative width of the interval of uncertainty
      is at most xtol.

    stpmin and stpmax are nonnegative input variables which
      specify lower and upper bounds for the step.

    maxfev is a positive integer input variable.  Termination
      occurs when the number of calls to fcn is at least
      maxfev by the end of an iteration.

    info is an integer output variable set as follows:

      info == 0  Improper input parameters.

      info == -1 A return is made to compute the function and gradient.

      info == 1  The sufficient decrease condition and the
                 directional derivative condition hold.

      info == 2  Relative width of the interval of uncertainty is at
                 most xtol.

      info == 3  Number of calls to fcn has reached maxfev.

      info == 4  The step is at the lower bound stpmin.

      info == 5  The step is at the upper bound stpmax.

      info == 6  Rounding errors prevent further progress.  There
                 may not be a step which satisfies the sufficient
                 decrease and curvature conditions.  Tolerances may
                 be too small.

    nfev is an integer output variable set to the number of
    calls to fcn.

    wa is a work array of length n.

    fp_err is a FILE pointer to which to write error messages.
    If it is (FILE *)NULL, then no error messages are produced.

  Subprograms called:

    mcstep (source in this file)

  Argonne National Laboratory.  MINPACK Project.  June 1983.
  Jorge J. More', David J. Thuente. */

void mcsrch(int n, float *x, float f, float *g, float *s, float *stp,
            float ftol, float gtol, float xtol, float stpmin, float stpmax,
            int maxfev, int *info, int *nfev, float *wa, FILE *fp_err)
{
  static int j, infoc, stage1, brackt, one = 1;
  /* float r1; */
  static float dgxm, dgym, finit, width, stmin, stmax, width1, ftest1,
    dg, gs, fx, fy, dginit, dgtest, dgm, dgx, dgy, fxm, fym, stx,
    sty;
  static double fm, t_f;

#define XTRAPF 4.

  /* parameter adjustments */
  --wa;
  --s;
  --g;
  --x;

  if(*info == -1)
    goto L45;
  gs = mlp_sdot(n, &g[1], one, &s[1], one) /
       (mlp_snrm2(n, &g[1], one) * mlp_snrm2(n, &s[1], one));
  infoc = 1;

  /* Check the input parameters for errors. */
  if(n <= 0 || *stp <= 0. || ftol < 0. || gtol < 0. || xtol < 0. ||
    stpmin < 0. || stpmax < stpmin || maxfev <= 0)
    return;

  /* Compute the initial gradient in the search direction and check
  that s is a descent direction. */
  dginit = 0.;
  for(j = 1; j <= n; j++)
    dginit += g[j] * s[j];
  if(dginit >= 0.) {
    if(fp_err != (FILE *)NULL)
      fprintf(fp_err, "\n  the search direction is not a descent \
direction\n");
    return;
  }

  /* Initialize local variables. */
  brackt = FALSE;
  stage1 = TRUE;
  *nfev = 0;
  finit = f;
  dgtest = ftol * dginit;
  width = stpmax - stpmin;
  width1 = width / .5;
  for(j = 1; j <= n; j++)
    wa[j] = x[j];

  /* The variables stx, fx, dgx contain the values of the step,
  function, and directional derivative at the best step.  The
  variables sty, fy, dgy contain the value of the step, function, and
  derivative at the other endpoint of the interval of uncertainty.
  The variables stp, f, dg contain the values of the step, function,
  and derivative at the current step. */
  stx = 0.;
  fx = finit;
  dgx = dginit;
  sty = 0.;
  fy = finit;
  dgy = dginit;

  /* Start of iteration. */

  /* [Main loop:] */
  while(1) {

    /* Set the minimum and maximum steps to correspond to the present
    interval of uncertainty. */
    if(brackt) {
      stmin = mlp_min(stx, sty);
      stmax = mlp_max(stx, sty);
    }
    else {
      stmin = stx;
      stmax = *stp + XTRAPF * (*stp - stx);
    }

    /* Force the step to be within the bounds stpmax and stpmin. */
    *stp = mlp_max(*stp, stpmin);
    *stp = mlp_min(*stp, stpmax);

    /* If an unusual termination is to occur then let stp be the
    lowest point obtained so far. */
    /* Previously was ... changed by MDG on 03/21/05
    if(brackt && (*stp <= stmin || *stp >= stmax) || *nfev >=
      maxfev - 1 || infoc == 0 || brackt && stmax - stmin <= xtol *
      stmax)
    */
    if((brackt && (*stp <= stmin || *stp >= stmax)) ||
       (*nfev >= maxfev - 1) ||
       (infoc == 0) ||
       (brackt && (stmax - stmin <= xtol * stmax)))
      *stp = stx;

    /* Evaluate the function and gradient at stp and compute the
    directional derivative.  We return to main program to obtain f and
    g. */
    for(j = 1; j <= n; j++)
      x[j] = wa[j] + *stp * s[j];
    *info = -1;
    return;

  L45:
    *info = 0;
    (*nfev)++;
    dg = 0.;
    for(j = 1; j <= n; j++)
      dg += g[j] * s[j];
    ftest1 = finit + *stp * dgtest;

    /* Test for convergence. */
    if((brackt && (*stp <= stmin || *stp >= stmax)) || infoc == 0)
      *info = 6;
    if(*stp == stpmax && f <= ftest1 && dg <= dgtest)
      *info = 5;
    if(*stp == stpmin && (f > ftest1 || dg >= dgtest))
      *info = 4;
    if(*nfev >= maxfev)
      *info = 3;
    if(brackt && stmax - stmin <= xtol * stmax)
      *info = 2;
    if(f <= ftest1 && fabs((double)dg) <= gtol * (-dginit))
      *info = 1;

    /* Check for termination. */
    if(*info != 0)
      return;

    /* In the first stage we seek a step for which the modified
    function has a nonpositive value and nonnegative derivative. */
    if(stage1 && f <= ftest1 && dg >= mlp_min(ftol, gtol) * dginit)
      stage1 = FALSE;

    /* A modified function is used to predict the step only if we have
    not obtained a step for which the modified function has a
    nonpositive function value and nonnegative derivative, and if a
    lower function value has been obtained but the decrease is not
    sufficient. */
    if(stage1 && f <= fx && f > ftest1) {
      /* Define the modified function and derivative values. */
      fm = f - *stp * dgtest;
      fxm = fx - stx * dgtest;
      fym = fy - sty * dgtest;
      dgm = dg - dgtest;
      dgxm = dgx - dgtest;
      dgym = dgy - dgtest;

      /* Call cstep to update the interval of uncertainty and to
      compute the new step. */
      mcstep(&stx, &fxm, &dgxm, &sty, &fym, &dgym, stp, &fm, &dgm,
        &brackt, stmin, stmax, &infoc);

      /* Reset the function and gradient values for f. */
      fx = fxm + stx * dgtest;
      fy = fym + sty * dgtest;
      dgx = dgxm + dgtest;
      dgy = dgym + dgtest;
    }
    else {
      /* Call mcstep to update the interval of uncertainty and to
      compute the new step. */
      t_f = (double)f;
      mcstep(&stx, &fx, &dgx, &sty, &fy, &dgy, stp, &t_f, &dg, &brackt,
        stmin, stmax, &infoc);
    }

    /* Force a sufficient decrease in the size of the interval of
    uncertainty. */
    if(brackt) {
/*      if((r1 = sty - stx, fabs((double)r1)) >= .66 * width1) */
      if(fabs((double)(sty - stx)) >= .66 * width1)
	*stp = stx + .5 * (sty - stx);
      width1 = width;
/*      width = (r1 = sty - stx, fabs((double)r1)); */
      width = fabs((double)(sty - stx));
    }

    /* End of iteration. */
  } /* while(1) [main loop] */

}

/* end of function mcsrch */

/*******************************************************************/

  /* ---------------
     function mcstep
     ---------------

  The purpose of mcstep is to compute a safeguarded step for a
  linesearch and to update an interval of uncertainty for a minimizer
  of the function.

  The parameter stx contains the step with the least function value.
  The parameter stp contains the current step.  It is assumed that the
  derivative at stx is negative in the direction of the step.  If
  brackt is set to TRUE then a minimizer has been bracketed in an
  interval of uncertainty with endpoints stx and sty.

  The calling statement is:

    mcstep(stx, fx, dx, sty, fy, dy, stp, fp, dp, brackt, stpmin,
      stpmax, info);

  where:

    stx, fx, and dx are variables which specify the step, the
      function, and the derivative at the best step obtained so far.
      The derivative must be negative in the direction of the step,
      that is, dx and stp-stx must have opposite signs.  On output
      these parameters are updated appropriately.

    sty, fy, and dy are variables which specify the step, the
      function, and the derivative at the other endpoint of the
      interval of uncertainty.  On output these parameters are
      updated appropriately.

    stp, fp, and dp are variables which specify the step, the
      function, and the derivative at the current step.  If brackt is
      set to TRUE then on input stp must be between stx and sty.  On
      output stp is set to the new step.

    brackt is an int variable which specifies if a minimizer has been
      bracketed.  If the minimizer has not been bracketed then on
      input brackt must be set to FALSE.  If the minimizer is
      bracketed then on output brackt is set to TRUE.

    stpmin and stpmax are input variables which specify lower and
      upper bounds for the step.

    info is an int output variable set as follows: if info =
      1, 2, 3, 4, or 5, then the step has been computed according to
      one of the five cases below;  otherwise info = 0, and this
      indicates improper input parameters.


  Argonne National Laboratory.  MINPACK Project.  June 1983.
  Jorge J. More', David J. Thuente. */

void mcstep(float *stx, float *fx, float *dx, float *sty, float *fy,
            float *dy, float *stp, double *fp, float *dp, int *brackt,
            float stpmin, float stpmax, int *info)
{
  static int bound;
  float r1, r2, r3;
  static float sgnd, stpc, stpf, stpq, p, q, gamma, r, s, theta;

  *info = 0;

  /* Check the input parameters for errors. */
  if((*brackt &&
      (*stp <= mlp_min(*stx, *sty) || *stp >= mlp_max(*stx, *sty))) ||
      (*dx * (*stp - *stx) >= 0.) ||
      (stpmax < stpmin))
    return;

  /* Determine if the derivatives have opposite sign. */
  sgnd = *dp * (*dx / fabs((double)(*dx)));

  if(*fp > *fx) {

    /* First case.  A higher function value.  The minimum is
    bracketed.  If the cubic step is closer to stx than the quadratic
    step, the cubic step is taken, else the average of the cubic and
    quadratic steps is taken. */
    *info = 1;
    bound = TRUE;
    theta = (*fx - *fp) * 3 / (*stp - *stx) + *dx + *dp;
    /* computing max */
    r1 = fabs((double)theta), r2 = fabs((double)(*dx)),
      r1 = mlp_max(r1, r2), r2 = fabs((double)(*dp));
    s = mlp_max(r1, r2);
    /* computing 2nd power */
    r1 = theta / s;
    gamma = s * sqrt((double)(r1 * r1 - *dx / s * (*dp / s)));
    if(*stp < *stx)
      gamma = -gamma;
    p = gamma - *dx + theta;
    q = gamma - *dx + gamma + *dp;
    r = p / q;
    stpc = *stx + r * (*stp - *stx);
    stpq = *stx + *dx / ((*fx - *fp) / (*stp - *stx) + *dx) / 2 *
      (*stp - *stx);
/*    if((r1 = stpc - *stx, fabs((double)r1)) < (r2 = stpq - *stx,
      fabs((double)r2))) */
    if(fabs((double)(stpc - *stx)) < fabs((double)(stpq - *stx)))
      stpf = stpc;
    else
      stpf = stpc + (stpq - stpc) / 2;
    *brackt = TRUE;
  }

  else if(sgnd < 0.) {

    /* Second case.  A lower function value and derivatives of
    opposite sign.  The minimum is bracketed.  If the cubic step is
    closer to stx than the quadratic (secant) step, the cubic step is
    taken, else the quadratic step is taken. */
    *info = 2;
    bound = FALSE;
    theta = (*fx - *fp) * 3 / (*stp - *stx) + *dx + *dp;
    /* computing max */
    r1 = fabs((double)theta), r2 = fabs((double)(*dx)),
      r1 = mlp_max(r1, r2), r2 = fabs((double)(*dp));
    s = mlp_max(r1, r2);
    /* computing 2nd power */
    r1 = theta / s;
    gamma = s * sqrt((double)(r1 * r1 - *dx / s * (*dp / s)));
    if(*stp > *stx)
      gamma = -gamma;
    p = gamma - *dp + theta;
    q = gamma - *dp + gamma + *dx;
    r = p / q;
    stpc = *stp + r * (*stx - *stp);
    stpq = *stp + *dp / (*dp - *dx) * (*stx - *stp);
/*    if((r1 = stpc - *stp, fabs((double)r1)) > (r2 = stpq - *stp,
      fabs((double)r2))) */
    if(fabs((double)(stpc - *stp)) > fabs((double)(stpq - *stp)))
      stpf = stpc;
    else
      stpf = stpq;
    *brackt = TRUE;
  }

  else if(fabs((double)(*dp)) < fabs((double)(*dx))) {

    /* Third case.  A lower function value, derivatives of the same
    sign, and the magnitude of the derivative decreases.  The cubic
    step is only used if the cubic tends to infinity in the direction
    of the step or if the minimum of the cubic is beyond stp.
    Otherwise the cubic step is defined to be either stpmin or stpmax.
    The quadratic (secant) step is also computed and if the minimum is
    bracketed then the the step closest to stx is taken, else the step
    farthest away is taken. */
    *info = 3;
    bound = TRUE;
    theta = (*fx - *fp) * 3 / (*stp - *stx) + *dx + *dp;
    /* computing max */
    r1 = fabs((double)theta), r2 = fabs((double)(*dx)),
      r1 = mlp_max(r1, r2), r2 = fabs((double)(*dp));
    s = mlp_max(r1, r2);
    /* The case gamma = 0 only arises if the cubic does not tend to
    infinity in the direction of the step. */
    /* computing max */
    /* computing 2nd power */
    r3 = theta / s;
    r1 = 0., r2 = r3 * r3 - *dx / s * (*dp / s);
    gamma = s * sqrt((double)mlp_max(r1, r2));
    if(*stp > *stx)
      gamma = -gamma;
    p = gamma - *dp + theta;
    q = gamma + (*dx - *dp) + gamma;
    r = p / q;
    if(r < 0. && gamma != 0.)
      stpc = *stp + r * (*stx - *stp);
    else if(*stp > *stx)
      stpc = stpmax;
    else
      stpc = stpmin;
    stpq = *stp + *dp / (*dp - *dx) * (*stx - *stp);
    if(*brackt) {
/*      if((r1 = *stp - stpc, fabs((double)r1)) < (r2 = *stp - stpq,
        fabs((double)r2))) */
      if(fabs((double)(*stp - stpc)) < fabs((double)(*stp - stpq)))
	stpf = stpc;
      else
	stpf = stpq;
    }
    else {
/*      if((r1 = *stp - stpc, fabs((double)r1)) > (r2 = *stp - stpq,
        fabs((double)r2))) */
      if(fabs((double)(*stp - stpc)) > fabs((double)(*stp - stpq)))
	stpf = stpc;
      else
	stpf = stpq;
    }
  }
  else {

    /* Fourth case.  A lower function value, derivatives of the same
    sign, and the magnitude of the derivative does not decrease.  If
    the minimum is not bracketed, the step is either stpmin or stpmax,
    else the cubic step is taken. */
    *info = 4;
    bound = FALSE;
    if(*brackt) {
      theta = (*fp - *fy) * 3 / (*sty - *stp) + *dy + *dp;
      /* computing max */
      r1 = fabs((double)theta), r2 = fabs((double)(*dy)),
        r1 = mlp_max(r1, r2), r2 = fabs((double)(*dp));
      s = mlp_max(r1, r2);
      /* computing 2nd power */
      r1 = theta / s;
      gamma = s * sqrt((double)(r1 * r1 - *dy / s * (*dp /s)));
      if(*stp > *sty)
	gamma = -gamma;
      p = gamma - *dp + theta;
      q = gamma - *dp + gamma + *dy;
      r = p / q;
      stpc = *stp + r * (*sty - *stp);
      stpf = stpc;
    }
    else if(*stp > *stx)
      stpf = stpmax;
    else
      stpf = stpmin;
  }

  /* Update the interval of uncertainty.  This update does not depend
  on the new step or the case analysis above. */
  if(*fp > *fx) {
    *sty = *stp;
    *fy = *fp;
    *dy = *dp;
  }
  else {
    if(sgnd < 0.) {
      *sty = *stx;
      *fy = *fx;
      *dy = *dx;
    }
    *stx = *stp;
    *fx = *fp;
    *dx = *dp;
  }

  /* Compute the new step and safeguard it. */
  stpf = mlp_min(stpmax, stpf);
  stpf = mlp_max(stpmin, stpf);
  *stp = stpf;
  if(*brackt && bound) {
    if(*sty > *stx) {
      /* computing min */
      r1 = *stx + (*sty - *stx) * .66;
      *stp = mlp_min(r1, *stp);
    }
    else {
      /* computing max */
      r1 = *stx + (*sty - *stx) * .66;
      *stp = mlp_max(r1, *stp);
    }
  }
  return;
}

/* end of function mcstep */

/*******************************************************************/
