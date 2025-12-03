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


/* UPDATED: 30/11/2005 by MDG to handle complex values consistently */
/* Changed each _( to plain (.  GTC 14 July 1995. */

/* Real 2-d FFT (fast Fourier transform) routine, forward or backward,
for order N as defined below (N must be even).  Uses C versions (made
by f2c) of the complex 1-d forward and backward FFT routines CFFTF and
CFFTB.  For forward (backward) FFT, set the "forward" parm to a
nonzero (zero) value.

The forward FFT is defined as follows.  Upon return, r will contain
the following function of what it originally contained (defined here
for general order N, i.e. for even or odd N).  If N is even let q =
N/2; if N is odd let q = (N+1)/2.  Let tpon be 2*pi/N, and let
sumij(expression) mean the double sum from i=0 to i=N-1 and from j=0
to j=N-1 of expression.  The output value of r is then:
  (Col 0:)
    r[0][0]=sumij(r[i][j])
    For m=1 through q-1:
      r[2*m-1][0]=sumij(r[i][j]*cos(m*i*tpon))
      r[2*m][0]=sumij(-r[i][j]*sin(m*i*tpon))
    If N is even:
      r[N-1][0]=sumij(r[i][j]*(-1)^i)
  (Cols 1 through 2*q-2:)
    For m=0 through N-1 and p=1 through q-1:
      r[m][2*p-1]=sumij(r[i][j]*cos((m*i+p*j)*tpon))
      r[m][2*p]=sumij(-r[i][j]*sin((m*i+p*j)*tpon))
  (If N is even, col N-1:)
    r[0][N-1]=sumij(r[i][j]*(-1)^j)
    For m=1 through q-1:
      r[2*m-1][N-1]=sumij(r[i][j]*cos(m*i*tpon)*(-1)^j)
      r[2*m][N-1]=sumij(-r[i][j]*sin(m*i*tpon)*(-1)^j)
    r[N-1][N-1] = sumij(r[i][j]*(-1)^(i+j))

The backward FFT is defined to be the inverse function of the forward
FFT. */

#include <stdio.h>
#include <f2c.h>
/* Following lines added by MDG on 03-10-05 */
#include <util.h>
#include <little.h>
extern int cffti(int *, real *);
extern int cfftf(int *, real *, real *);
extern int cfftb(int *, real *, real *);
/* End added lines */


#define N 32 /* must be even */

void fft2dr(float r[N][N], int forward)
{
  char str[100];
  int ir, iir, ic, iic, i, j, k, m, p;
  static int f = 1, n, q;
  static float rns, *w, buf[N][N];
  static complex c[N];

  if(f) {
    f = 0;
    if(N&1) {
      sprintf(str, "N is defined to be the odd number %d (N must be \
even)", N);
      fatalerr("fft2dr", str, NULL);
    }
    n = N;
    rns = 1./(N*N);
    w = (float *)malloc_ch((4*N+15) * sizeof(float));
    cffti(&n, w);
    q = N/2;
  }
  if(forward) { /* forward FFT */
    /* Do 1-d real FFTs of the rows, in pairs of rows, by using one
    call of a 1-d complex FFT routine (CFFTF), plus some additional
    work, to produce each pair of 1-d real FFTs. */
    for(ir = 0, iir = 1; ir < n; ir += 2, iir += 2) {
      for(ic = 0; ic < n; ic++) {
	c[ic].r = r[ir][ic] * rns;
	c[ic].i = r[iir][ic] * rns;
      }
      cfftf(&n, (real *)c, w);
      r[ir][0] = c[0].r;
      r[iir][0] = c[0].i;
      for(k = j = 1, i = n-1; k < q; k++, j += 2, i--) {
	r[ir][j] = .5 * (c[i].r + c[k].r);
	r[iir][j+1] =.5 * (c[i].r - c[k].r);
	r[iir][j] = .5 * (c[k].i + c[i].i);
	r[ir][j+1] = .5 * (c[k].i - c[i].i);
      }
      r[ir][n-1] = c[q].r;
      r[iir][n-1] = c[q].i;
    }
    /* Do 1-d real FFTs of the cols, in pairs of cols, storing the
    results as pairs of rows of buf. */
    for(ic = 0, iic = 1; ic < n; ic += 2, iic += 2) {
      for(ir = 0; ir < n; ir++) {
	c[ir].r = r[ir][ic];
	c[ir].i = r[ir][iic];
      }
      cfftf(&n, (real *)c, w);
      buf[ic][0] = c[0].r;
      buf[iic][0] = c[0].i;
      for(k = j = 1, i = n-1; k < q; k++, j += 2, i--) {
	buf[ic][j] = .5 * (c[i].r + c[k].r);
	buf[iic][j+1] = .5 * (c[i].r - c[k].r);
	buf[iic][j] = .5 * (c[k].i + c[i].i);
	buf[ic][j+1] = .5 * (c[k].i - c[i].i);
      }
      buf[ic][n-1] = c[q].r;
      buf[iic][n-1] = c[q].i;
    }
    /* Finish up by copying or combining appropriate elts of buf */
    for(ir = 0; ir < n; ir++)
      r[ir][0] = buf[0][ir];
    for(ic = 1; ic < n; ic++)
      r[0][ic] = buf[ic][0];
    for(p = 1; p < q; p++)
      for(m = 1; m < q; m++) {
	r[m][2*p-1] = buf[2*p-1][2*m-1] - buf[2*p][2*m];
	r[n-m][2*p-1] = buf[2*p-1][2*m-1] + buf[2*p][2*m];
	r[m][2*p] = buf[2*p][2*m-1] + buf[2*p-1][2*m];
	r[n-m][2*p] = buf[2*p][2*m-1] - buf[2*p-1][2*m];
      }
    for(ir = 1; ir < n; ir++)
      r[ir][n-1] = buf[n-1][ir];
  } /* end forward FFT */
  else { /* backward FFT */
    for(ir = 0; ir < n; ir++)
      buf[0][ir] = r[ir][0];
    for(ic = 1; ic < n; ic++)
      buf[ic][0] = r[0][ic];
    for(p = 1; p < q; p++)
      for(m = 1; m < q; m++) {
	buf[2*p-1][2*m-1] = .5 * (r[m][2*p-1] + r[n-m][2*p-1]);
	buf[2*p][2*m] = .5 * (r[n-m][2*p-1] - r[m][2*p-1]);
	buf[2*p][2*m-1] = .5 * (r[m][2*p] + r[n-m][2*p]);
	buf[2*p-1][2*m] = .5 * (r[m][2*p] - r[n-m][2*p]);
      }
    for(ir = 1; ir < n; ir++)
      buf[n-1][ir] = r[ir][n-1];
    for(ir = 0, iir = 1; ir < n; ir += 2, iir += 2) {
      c[0].r = buf[ir][0];
      c[0].i = buf[iir][0];
      for(k = j = 1, i = n-1; k < q; k++, j += 2, i--) {
	c[i].r = buf[ir][j] + buf[iir][j+1];
	c[k].r = buf[ir][j] - buf[iir][j+1];
	c[k].i = buf[iir][j] + buf[ir][j+1];
	c[i].i = buf[iir][j] - buf[ir][j+1];
      }
      c[q].r = buf[ir][n-1];
      c[q].i = buf[iir][n-1];
      cfftb(&n, (real *)c, w);
      for(ic = 0; ic < n; ic++) {
	r[ic][ir] = c[ic].r;
	r[ic][iir] = c[ic].i;
      }
    }
    for(ir = 0, iir = 1; ir < n; ir += 2, iir += 2) {
      c[0].r = r[ir][0];
      c[0].i = r[iir][0];
      for(k = j = 1, i = n-1; k < q; k++, j += 2, i--) {
	c[i].r = r[ir][j] + r[iir][j+1];
	c[k].r = r[ir][j] - r[iir][j+1];
	c[k].i = r[iir][j] + r[ir][j+1];
	c[i].i = r[iir][j] - r[ir][j+1];
      }
      c[q].r = r[ir][n-1];
      c[q].i = r[iir][n-1];
      cfftb(&n, (real *)c, w);
      for(ic = 0; ic < n; ic++) {
	r[ir][ic] = c[ic].r;
	r[iir][ic] = c[ic].i;
      }
    }
  } /* end backward FFT */
}
