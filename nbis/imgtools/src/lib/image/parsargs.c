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
      LIBRARY: IMAGE - Image Manipulation and Processing Routines

      FILE:    PARSARGS.C

      AUTHORS: Criag Watson
               Michael Garris
      DATE:    02/15/2001
      UPDATED: 03/15/2005 by MDG

      Contains routines responsible for parsing command line
      argument relevant to image pixmap attributes.

      ROUTINES:
#cat: parse_w_h_d_ppi - takes a string of comma-separated image attributes
#cat:              and parses in order a width, height, depth, and
#cat:              optional ppi value.
#cat: parse_h_v_sampfctrs - takes a formatted string and parses
#cat:              component plane downsampling factors.

***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <parsargs.h>

/*****************************************************************/
void parse_w_h_d_ppi(char *argstr, char *arg0,
                   int *width, int *height, int *depth, int *ppi)
{
   char cbuff[11], *cptr, *aptr;

   aptr = argstr;

   /* parse width */
   cptr = cbuff;
   while((*aptr != '\0') && (*aptr != ','))
      *cptr++ = *aptr++;
   if(*aptr == '\0'){
      print_usage(arg0);
      fprintf(stderr, "       height not found\n");
      exit(-1);
   }
   *cptr = '\0';
   *width = atoi(cbuff);

   /* parse height */
   cptr = cbuff;
   aptr++;
   while((*aptr != '\0') && (*aptr != ','))
      *cptr++ = *aptr++;
   if(*aptr == '\0'){
      print_usage(arg0);
      fprintf(stderr, "       depth not found\n");
      exit(-1);
   }
   *cptr = '\0';
   *height = atoi(cbuff);

   /* parse depth */
   cptr = cbuff;
   aptr++;
   while((*aptr != '\0') && (*aptr != ','))
      *cptr++ = *aptr++;
   *cptr = '\0';
   *depth = atoi(cbuff);

   if(*aptr != '\0'){
      /* parse ppi */
      cptr = cbuff;
      aptr++;
      while(*aptr != '\0')
         *cptr++ = *aptr++;
      *cptr = '\0';
      *ppi = atoi(cbuff);
   }
   /* Otherwise, no ppi passed. */
   else
      *ppi = -1;
}

/*****************************************************************/
void parse_h_v_sampfctrs(char *argstr, char *arg0,
                         int *hor_sampfctr, int *vrt_sampfctr, int *n_cmpnts)
{
   char cbuff[11], *cptr, *aptr;
   int t;

   aptr = argstr;

   *n_cmpnts = 0;

   while(*aptr != '\0'){
      if(*n_cmpnts > MAX_CMPNTS){
         print_usage(arg0);
         fprintf(stderr, "       number of components exceeds %d\n",
                 MAX_CMPNTS);
         exit(-1);
      }
      /* parse horizontal sample factor */
      cptr = cbuff;
      while((*aptr != '\0') && (*aptr != ','))
         *cptr++ = *aptr++;
      if(*aptr == '\0') {
         print_usage(arg0);
         fprintf(stderr, "       V[%d] not found\n", *n_cmpnts);
         exit(-1);
      }
      *cptr = '\0';
      t = hor_sampfctr[*n_cmpnts] = atoi(cbuff);
      if((t < 1) || (t > MAX_CMPNTS)){
         print_usage(arg0);
         fprintf(stderr, "       H[%d] = %d must be [1..%d]\n",
                 *n_cmpnts, t, MAX_CMPNTS);
         exit(-1);
      }
      aptr++;

      /* parse vertical sample factor */
      cptr = cbuff;
      while((*aptr != '\0') && (*aptr != ':'))
         *cptr++ = *aptr++;
      *cptr = '\0';
      t = vrt_sampfctr[*n_cmpnts] = atoi(cbuff);
      if((t < 1) || (t > MAX_CMPNTS)){
         print_usage(arg0);
         fprintf(stderr, "       V[%d] = %d must be [1..%d]\n",
                 *n_cmpnts, t, MAX_CMPNTS);
         exit(-1);
      }
      if(*aptr != '\0')
         aptr++;

      (*n_cmpnts)++;
   }
}
