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
      LIBRARY: IHEAD - IHead Image Utilities

      FILE:    READIHDR.C
      AUTHOR:  Michael Garris
      DATE:    04/26/1989
      UPDATED: 03/14/2005 by MDG

      Contains routines responsible for reading an IHead header
      from an open file.

      ROUTINES:
#cat: readihdr - reads the contents of an open file pointer into an
#cat:            IHead structure.

***********************************************************************/

#include <stdio.h>
#include <ihead.h>
#include <util.h>

/************************************************************/
/* Readihdr() allocates and reads header information into an*/
/* ihead structure and returns the initialized structure.   */
/*                                                          */
/*         Modifications:                                   */
/*		1/11/91 Stan Janet                          */
/*			check return codes                  */
/*			declare malloc()                    */
/************************************************************/
IHEAD *readihdr(register FILE *fp)
{
   IHEAD *head;
   char lenstr[SHORT_CHARS];
   int n, len;

   n = fread(lenstr,1,SHORT_CHARS,fp);
   if (n != SHORT_CHARS) {
	(void) fprintf(stderr,"readihdr: fread returned %d (expected %d)\n",
		n,SHORT_CHARS);
	exit(1);
   }

   if (sscanf(lenstr,"%d",&len) != 1)
	fatalerr("readihdr","cannot parse length field",(char *)NULL);

   if (len != IHDR_SIZE)
      fatalerr("readihdr","Record Sync Error: Header not found or old format.",
            NULL);

   head = (IHEAD *) malloc(sizeof(IHEAD));
   if (head == (IHEAD *) NULL)
      syserr("readihdr","malloc","head");

   n = fread(head,1,len,fp);
   if (n != len) {
	(void) fprintf(stderr,"readihdr: fread returned %d (expected %d)\n",
		n,len);
	exit(1);
   }

   return head;
}
