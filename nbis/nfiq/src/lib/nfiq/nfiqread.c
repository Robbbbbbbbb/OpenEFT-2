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


/************************************************************************/
/***********************************************************************
      LIBRARY: FING - NIST Fingerprint Systems Utilities

      FILE:           NFIQREAD.C
      IMPLEMENTATION: Michael D. Garris
      ALGORITHM:      Elham Tabassi
                      Charles L. Wilson
                      Craig I. Watson
      DATE:           09/09/2004

      Contains read routines for various files supporting
      NFIQ (NIST Fingerprint Image Quality)

***********************************************************************
               ROUTINES:
                        read_imgcls_file()
                        read_znorm_file()

***********************************************************************/

#include <ioutil.h>
#include <nfiq.h>

/***********************************************************************
************************************************************************
#cat: read_imgcls_file - Routine opens a column-formatted text file
#cat:                    parsing in the first column of filenames as a
#cat:                    list of strings and the second column of
#cat:                    target image qualities as a list of strings.

   Input:
      imgclsfilep - input file name to be read
   Output:
      oimglist    - allocated list of file names read
      oclslist    - allocated list of target image qualities read
      onum_ims    - number of images read
   Return Code:
      Zero        - successful completion
      Negative    - system error
************************************************************************/
int read_imgcls_file(char *imgclsfile, char ***oimglist, char ***oclslist,
                      int *onum_imgs)
{
   int ret, alloc_flag;

   alloc_flag = TRUE;
   ret = read_strstr_file(imgclsfile, oimglist, oclslist, onum_imgs,
                          alloc_flag);

   return(ret);
}

/***********************************************************************
************************************************************************
#cat: read_znorm_file - Routine opens a column-formatted text file
#cat:                   parsing in the first column of mean values as a
#cat:                   list of floats and the second column of stddev
#cat:                   values as a list of floats.

   Input:
      znormfile   - input file name to be read
   Output:
      znorm_means - preallocated list of mean values read
      znorm_stds  - preallocated list of stddev values read
      num_znorms  - allocated length of output vectors
   Return Code:
      Zero        - successful completion
      Negative    - system error
************************************************************************/
int read_znorm_file(char *znormfile,
         float *znorm_means, float *znorm_stds, const int num_znorms)
{
   int ret, tnum_znorms, alloc_flag;

   tnum_znorms = num_znorms;
   alloc_flag = FALSE;
   ret = read_fltflt_file(znormfile, &znorm_means, &znorm_stds, &tnum_znorms,
                          alloc_flag);
   return(ret);
}
