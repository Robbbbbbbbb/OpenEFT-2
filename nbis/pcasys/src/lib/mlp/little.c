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

      FILE:    LITTLE.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      Utility routines.

      ROUTINES:
#cat: intcomp         - integer comparison function
#cat: s2hms           - converts seconds to hours:minutes:seconds string
#cat: ups_secs        - user plus system seconds used by process
      
***********************************************************************/

/* [Perhaps some of these include files are not needed any more,
since this little.c has had many routines removed:] */
#include <mlp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __MSYS__
#include <sys/time.h>
#else
#include <sys/times.h>
#endif


/*******************************************************************/

/* Frees a pointer, but only if it is not (char *)NULL. */

void free_notnull(char *p)
{
  if(p != (char *)NULL)
    free(p);
}

/********************************************************************/

/* Integer comparison function. */

int intcomp(int *f, int *s)
{
  if(*f < *s)
    return -1;
  if(*f > *s)
    return 1;
  return 0;
}

/*******************************************************************/

/* Converts a float number of seconds to a string of the form
<hours>:<minutes>:<seconds>, with <seconds> rounded to the nearest
tenth.  CAUTION: Returned function value is the address of a static
buffer, whose contents are overwritten each time the function is
called. */

char *s2hms(float s)
{
  static char buf[100];
  int h, m;

  s = (int)(10. * s + .5) / 10.;
  h = (int)(s / 3600.);
  s -= 3600 * h;
  m = (int)(s / 60.);
  s -= 60 * m;
  sprintf(buf, "%d:%02d:%04.1f", h, m, s);
  return buf;
}

#ifdef __MSYS__
float ups_secs()
{
   struct timeval tp;

   gettimeofday(&tp, NULL);
   return (float)tp.tv_sec+(1.e-6)*tp.tv_usec;
}
#else
/******************************************************************/

/* Returns the sum of the user time and system time used so far by the
process in which it is called, in seconds.  (Does not count time used
by child processes, if any.)

NOTE.  It is possible that this routine is not totally portable: there
may be some systems on which CLK_TCK is not defined in limits.h and
yet the unit in which times() reports its results is not the
"standard" 1/60 second. */

float ups_secs()
{
  struct tms yow;

#ifdef CLK_TCK /* (may be defined in limits.h) */
/* Assume that on this system, the values set by times() are in units
of 1/CLK_TCK second: */
#define UPS_SECS_FAC (1./(float)CLK_TCK)
#else
/* Assume that the values set by times() are in the standard (?) units
of 1/60 second: */
#define UPS_SECS_FAC (1./60.)
#endif

  times(&yow);
  return (yow.tms_utime + yow.tms_stime) * UPS_SECS_FAC;
}

/********************************************************************/
#endif