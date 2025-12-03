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

      FILE:    FSASO.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      ROUTINES:
#cat: fsaso_init - Initialization call Before first call of fsaso.
#cat: fsaso - Fputs's a string both to stderr and to the short outfile.

***********************************************************************/

/* Routines can use the "fsaso" utility routine to succintly write a
string both to stderr and to the short outfile.  Example usages:

  (Before first call of fsaso:)
  fsaso_init(short_outfile);

  (Then, can use fsaso like the following, in any routine:)

  sprintf(str, "foo: %d; bar: %f\n", foo, bar);
  fsaso(str);

  (or:)

  fsaso(already_filled_in_string);

  (or:)

  fsaso("This is a literal string.\n");

CAUTION: Since fsaso.c has fp as its _private_ stream to the short
outfile, it is impossible to produce good results by doing a sequence
of writes to the short outfile IN WHICH some writes use fsaso and
others explicitly use another stream to the short outfile: that will
fail because neither stream will, when it does a write, move the
location pointer in the other stream.  So, if you are going to use
fsaso at all, then _every_ write to the short outfile should use
fsaso, and so every write to the short outfile will have a mirror
write to stderr, which seems good for mlp.  However, it is ok to
interleave fsaso calls with explicit writes to stderr, since stderr is
the same stream no matter what routine is using it, with the same
location pointer.  So, one can do writes to stderr which are not
mirrored as writes to short outfile, and that definitely is a good
thing for mlp, since some progress writes to stderr do not go with a
particular run and hence should not be written into any short outfile:

  fprintf(stderr, "Will do %d runs.\n", nruns);
  ...
  fsaso(string_for_stderr_and_short_outfile);

So, mlp should write its non-run-specific messages directly to
stderr, and should write all other messages, which go with the
current run, using fsaso.
*/

#include <mlp.h>

static FILE *fp = (FILE *)NULL;

/*******************************************************************/

/* fsaso_init: Before first call of fsaso for a new short outfile
(i.e. a new run), call this with the filename. */

void fsaso_init(char short_outfile[])
{
  /* If fp is a stream to preceding short_outfile, fclose it. */
  if(fp != (FILE *)NULL)
    fclose(fp);

  if((fp = fopen(short_outfile, "wb")) == (FILE *)NULL)
    syserr("fsaso_init (fsaso.c)", "fopen for writing failed",
      short_outfile);
}

/*******************************************************************/

/* fsaso: Fputs's a string both to stderr and to the short outfile,
also fflushing both streams. */

void fsaso(char str[])
{
  fputs(str, stderr);
  fflush(stderr);
  fputs(str, fp);
  fflush(fp);
}

/*******************************************************************/
