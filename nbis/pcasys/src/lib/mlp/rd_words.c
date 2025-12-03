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

      FILE:    RD_WORDS.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG
      UPDATE:  12/02/2008 by Kenneth Ko - Fix to support 64-bit

      ROUTINES:
#cat: rd_words - Reads a sequence of 4-byte words into a buffer, either
#cat:            from a Fortran-style binary file or from a text file.

***********************************************************************/

#include <mlp.h>

/* [Possibly add checking of the fread return values.] */
/*
Can use any specified
spacing between the positions in the buffer into which the words are
to be read.  (The words are assumed to form a contiguous block in the
file, i.e. there is no skipping of data in the file.)

Input args:
  ascii_or_binary: If ASCII, the file to be read is assumed
    to be in ascii form; if BINARY, the file is assumed to be in the
    form of a Fortran-style binary file into which the sequence of
    words that will now be read, were written in by a single Fortran
    binary write statement: i.e., the data block is preceded and
    followed by an int whose value is the number of bytes in the
    block.  (ASCII and BINARY are defined in parms.h.)
  fp: A FILE pointer already fopened for reading.
  nwords: How many words are to be read.
  bufinc: The words are to be read into positions 0, bufinc,
    2 * bufinc, etc. of buf.
  datatype: The type of data expected: RD_INT or RD_FLOAT (defined
    in rd_words.h).  This input does not matter if fortranbin is
    true.

Output arg:
  buf: Buffer into which the words are to be read.  Must be allocated
    by caller.
*/

void rd_words(char ascii_or_binary, FILE *fp, int nwords, int bufinc,
              char datatype, void *buf)
{
  char str[100];
  int nbytes_expected, nbytes;
  int *ibuf, *iptr;
  float *fbuf, *fptr;
  unsigned int *uibuf, *uiptr;

  if(sizeof(int) != 4 || sizeof(unsigned int) != 4 || sizeof(float) != 4)
    fatalerr("rd_words", "sizeof(int) and sizeof(float) must be 4",
      NULL);

  if(ascii_or_binary == ASCII) {
    if(datatype == RD_INT){
      ibuf = (int *)buf;
      for(iptr = ibuf; iptr < ibuf+(nwords*bufinc); iptr += bufinc)
	fscanf(fp, "%d", iptr);
    }
    else if(datatype == RD_FLOAT){
      fbuf = (float *)buf;
      for(fptr = fbuf; fptr < fbuf+(nwords*bufinc); fptr += bufinc)
	fscanf(fp, "%f", fptr);
    }
    else {
      sprintf(str, "datatype must be RD_INT, (char)%d, or RD_FLOAT, \
(char)%d; it is (char)%d", (int)RD_INT, (int)RD_FLOAT, (int)datatype);
      fatalerr("rd_words", str, NULL);
    }
  }
  else { /* ascii_or_binary == BINARY, meaning a fortran-style
         binary file */
    nbytes_expected = nwords * 4;
    fread(&nbytes, 4, 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(nbytes);
#endif
    if(nbytes != nbytes_expected) {
      sprintf(str, "nbytes is %d, but nbytes_expected is %d", nbytes,
        nbytes_expected);
      fatalerr("rd_words", str, NULL);
    }
    uibuf = (unsigned int *)buf;
    for(uiptr = uibuf; uiptr < uibuf+(nwords*bufinc); uiptr += bufinc) {

       fread(uiptr, 4, 1, fp);
#ifdef __NBISLE__
       swap_int_bytes(*uiptr);
#endif

    }

    fread(&nbytes, 4, 1, fp);
#ifdef __NBISLE__
    swap_int_bytes(nbytes);
#endif
    if(nbytes != nbytes_expected) {
      sprintf(str, "nbytes is %d, but nbytes_expected is %d", nbytes,
        nbytes_expected);
      fatalerr("rd_words", str, NULL);
    }
  }

}
