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

      FILE:    PARSIHDR.C
      AUTHOR:  Michael Garris
      DATE:    11/30/1989
      UPDATED: 03/14/2005 by MDG

      Contains routines responsible for parsing information stored
      in an IHead header's ID field.

      ROUTINES:
#cat: parseihdrid - takes the identity attribute from an hsf box image
#cat:               and parses out the box index and the reference string.

***********************************************************************/

#include <stdio.h>
#include <ihead.h>

/*******************************************************/
/*      Routine:  ParseIhdrID()                        */
/*      Author:   Michael D. Garris                    */
/*      Date:     11/30/89                             */
/*******************************************************/
/* Parseihdrid() takes a ihead id string and parses out*/
/* the index number and reference string.              */
/*******************************************************/
void parseihdrid(char *id, char *indxstr, char *refstr)
{
   char *iptr;

   iptr = id;
   /* EDIT MDG 1/25/99
   while((*iptr != NULL) && (*iptr != '_'))
   */
   while((*iptr != '\0') && (*iptr != '_'))
      iptr++;
   /* EDIT MDG 1/25/99
   if(*iptr == NULL){
   */
   if(*iptr == '\0'){
      /* start search for ref from beginning */
      iptr = id;
   }
   else{
      ++iptr;
      /* EDIT MDG 1/25/99
      while((*iptr != NULL) && (*iptr != '.'))
      */
      while((*iptr != '\0') && (*iptr != '.'))
         *indxstr++ = *iptr++;
   }
   /* EDIT MDG 1/25/99
   *indxstr = NULL;
   */
   *indxstr = '\0';
   /* EDIT MDG 1/25/99
   while((*iptr != NULL) && (*iptr != '\"'))
   */
   while((*iptr != '\0') && (*iptr != '\"'))
      iptr++;
   /* EDIT MDG 1/25/99
   if(*iptr != NULL){
   */
   if(*iptr != '\0'){
      ++iptr;
      /* EDIT MDG 1/25/99
      while((*iptr != NULL) && (*iptr != '\"'))
      */
      while((*iptr != '\0') && (*iptr != '\"'))
         *refstr++ = *iptr++;
   }
   /* EDIT MDG 1/25/99
   *refstr = NULL;
   */
   *refstr = '\0';
}
