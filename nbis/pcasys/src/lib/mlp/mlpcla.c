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


/*****************************************************************/
/* File:   mlpcla.c                                              */
/* Author: Michael D. Garris                                     */
/* Date:   03/17/2005                                            */
/*                                                               */
/* To handle proper prototyping and argument passing to CLAPCK   */
/* routines used by MLP library codes.  E.g. MLP codes are       */
/* written in single percision integer while the CBLAS routines  */
/* are written using long ints.                                  */
/*****************************************************************/

#include <mlpcla.h>


/*****************************************************************/
int mlp_sgemv(char trans, int m, int n, float alpha, 
	float *a, int lda, float *x, int incx, float beta, float *y, 
	int incy)
{
   int ret;
   int t_m, t_n, t_lda, t_incx, t_incy;

   t_m = m;
   t_n = n;
   t_lda = lda;
   t_incx = incx;
   t_incy = incy;

   ret = sgemv_(&trans, &t_m, &t_n, &alpha, a, &t_lda, x, &t_incx,
                &beta, y, &t_incy);

   return(ret);
}

/*****************************************************************/
int mlp_sscal(int n, float sa, float *sx, int incx)
{
   int ret;
   int t_n, t_incx;

   t_n = n;
   t_incx = incx;

   ret = sscal_(&t_n, &sa, sx, &t_incx);

   return(ret);
}

/*****************************************************************/
int mlp_saxpy(int n, float sa, float *sx, int incx, float *sy, int incy)
{
   int ret;
   int t_n, t_incx, t_incy;

   t_n = n;
   t_incx = incx;
   t_incy = incy;

   ret = saxpy_(&t_n, &sa, sx, &t_incx, sy, &t_incy);

   return(ret);
}

/*****************************************************************/
float mlp_sdot(int n, float *sx, int incx, float *sy, int incy)
{
   double dret;
   float fret;
   int t_n, t_incx, t_incy;

   t_n = n;
   t_incx = incx;
   t_incy = incy;

   dret = sdot_(&t_n, sx, &t_incx, sy, &t_incy);

   fret = (float)dret;

   return(fret);
}

/*****************************************************************/
float mlp_snrm2(int n, float *x, int incx)
{
   double dret;
   float fret;
   int t_n, t_incx;

   t_n = n;
   t_incx = incx;

   dret = snrm2_(&t_n, x, &t_incx);

   fret = (float)dret;

   return(fret);
}

