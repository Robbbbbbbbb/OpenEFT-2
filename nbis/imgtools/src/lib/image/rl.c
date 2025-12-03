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

      FILE:    RL.C
      AUTHOR:  Darlene E. Frederick
               
      DATE:    12/28/1989
      UPDATED: 03/15/2005 by MDG

      Contains routines responsible for Run-Length encoding
      a binary image pixel datastream.

      ROUTINES:
#cat: rlcomp - run length compresses an image.
#cat:
#cat: PutNchar - generates the next run length code.
#cat:
#cat: rldecomp - decompresses a run length encoded image.
#cat:
#cat: RLL_putc - expands a given run length code.
#cat:

***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <imgutil.h>

#define DLE 0x90			/* repeat sequence marker */
#define NOHIST	0			/* don't consider previous input*/
#define INREP 1 			/* sending a repeated value */
#define SENTCHAR 1			/* lastchar set, no lookahead yet */
#define SENDNEWC 2			/* run over, send new char next */
#define SENDCNT 3			/* newchar set, send count next */
#define BUFFSIZE	80
#define  ESCAPE		0x90
#define min(a, b)    ((a) < (b) ? (a) : (b))
#define TRUE	1
#define FALSE	0

/*********************************************************************
*	Routine:  rlcomp()					     *
*  	Author:   Darlene E. Frederick				     *
*  	Date:     12/28/89					     *
*********************************************************************/
/*********************************************************************
* rlcomp() compresses a file                                      *
*								     *
* Arguments  							     *
* ---------							     *
*	passed in:						     *
*		   indata - buffer containing data to be compressed  *
*		   inbytes - number of bytes to be compressed        *
*		   outsize - number of allocated bytes in outdata    *
*       returned:  						     *
*		   outdata - buffer containing compressed data       *
*		   outbytes - number of bytes in outdata after       *
*			      compression			     *
*********************************************************************/
void rlcomp(unsigned char *indata, int inbytes,
            unsigned char *outdata, int *outbytes, int outsize)
{
    int ch, last, in_count;
    int count;
    unsigned char *outptr;


    outptr = outdata;
    last  = *indata;
    in_count = 1;
    count = 1;
    *outbytes = 0;

    while (in_count < inbytes) {
        ch = *(++indata);
        in_count++;
	if (ch == last) 
	    count++;
        else
        {
	    PutNchar (count, last, &outptr, outbytes, outsize);
	    last  = ch;
	    count = 1;
	}
    }
    PutNchar (count, last, &outptr, outbytes, outsize);
}

/*********************************************************************
*	Routine:  putNchar()					     *
*  	Author:   Darlene E. Frederick				     *
*  	Date:     12/28/89					     *
*********************************************************************/
/*********************************************************************
* putNchar() outputs the compressed data which consists of a         *
* particular sequence of bytes based on how many of the same         *
* characters are found sequentially in the file                      *
*								     *
* Arguments  							     *
* ---------							     *
*	passed in:						     *
*		   n - number of the same characters found           *
*		       sequentially in the buffer  		     *
*		   ch - last character read from the  input buffer   *
*		   outptr - pointer to the output buffer             *
*		   outsize - size of outdata                         *
*	returned: 						     *
*		   outbytes - number of bytes in outdata after       *
*			      compression			     *
*********************************************************************/
void PutNchar (int n, int ch,
               unsigned char **outptr, int *outbytes, int outsize)
{
    int   count, tmpbytes;

    if (ch == ESCAPE) {
	while (n-- > 0){
          if((tmpbytes = *outbytes + 2) > outsize)
          {
              fprintf(stderr,"Output buffer Overflow in PutNchar().\n");
              exit(-1);
          }
          else
          {
              **outptr = ESCAPE;
              (*outptr)++;
	      **outptr = 0;
              (*outptr)++;
              *outbytes += 2;
         }
       }
       return;
    }

    while (n >= 4) {
      if((tmpbytes = *outbytes + 3) > outsize)
      {
          fprintf(stderr,"Output buffer Overflow in PutNchar().\n");
          exit(-1);
      }
      else
      {
	   count = min(n, 255);
	   **outptr = ch;
           (*outptr)++;
	   **outptr = ESCAPE;
           (*outptr)++;
	   **outptr = count;
           (*outptr)++;
           *outbytes += 3;
	   n -= count;
      }
    }

    while (n-- > 0){
      if((tmpbytes = *outbytes + 1) > outsize)
      {
          fprintf(stderr,"Output buffer Overflow in PutNchar().\n");
          exit(-1);
      }
      else
      {
	   **outptr = ch;
           (*outptr)++;
           *outbytes += 1;
      }
    }
}


/*********************************************************************
*	Routine:  rldecomp()					     *
*  	Author:   Darlene E. Frederick				     *
*  	Date:     12/28/89					     *
*********************************************************************/
/*********************************************************************
* rldecomp() decompresses a file                                     *
*								     *
* Arguments  							     *
* ---------							     *
*	passed in:						     *
*		   indata - buffer containing data to be decompressed*
*		   inbytes - number of bytes in input buffer.        *
*                  outsize - number of allocated bytes in outdata.   *
*       returned:  						     *
*		   outdata - buffer containing uncompressed data     *
*		   outbytes - number of bytes in outdata after       *
*			      decompression			     *
*********************************************************************/
void rldecomp(unsigned char *indata, int inbytes,
              unsigned char *outdata, int *outbytes, int outsize)
{
	int  ch;
        unsigned char  *outptr;
        int incount;

        outptr = outdata;
        *outbytes = 0;
        ch = *indata;
        incount = 1;
	RLL_putc (&outptr,ch,outsize,outbytes);
	while (incount < inbytes)
        {
                indata++;
                ch = *indata;
                incount++;
		RLL_putc (&outptr,ch,outsize,outbytes);
	}
}


int	  lastchar;		/* last output character */
int	  escape = FALSE;	/* last char was ESCAPE code */

/*********************************************************************
*	Routine:  RLL_putc()					     *
*  	Author:   Darlene E. Frederick				     *
*  	Date:     12/28/89					     *
*********************************************************************/
/*********************************************************************
* RLL_putc() outputs the decompressed data                           *
*								     *
* Arguments  							     *
* ---------							     *
*	passed in:						     *
*		   outptr - pointer to the output buffer             *
*		   code - byte to be decompressed		     *
*		   outsize - size of output buffer                  *
*	returned:						     *
*		   outcount - number of bytes output                 *
*********************************************************************/
void RLL_putc (unsigned char **outptr, unsigned char code,
               int outsize, int *out_count)
{
 int tmpbytes;
	if(escape) {
	    escape = FALSE;
	    if(code)  
	       while (--code)  
               {
	          if((tmpbytes = *out_count + 1) > outsize)
		  {
                      fprintf(stderr,"Output Buffer Overflow in RLL_putc.\n");
		      exit(-1);
		  }
		  **outptr = lastchar;
                  (*outptr)++;
		  (*out_count)++;
               }
	    else
            {
	       if((tmpbytes = *out_count + 1) > outsize)
	       {
                   fprintf(stderr,"Output Buffer Overflow in RLL_putc.\n");
	           exit(-1);
	       }
	       **outptr = ESCAPE;
               (*outptr)++;
	       (*out_count)++;
            }
	    return;
	}

	if (code == ESCAPE) {
	    escape = TRUE;
	    return;
	}

        if((tmpbytes = *out_count + 1) > outsize)
        {
          fprintf(stderr,"Output Buffer Overflow in RLL_putc.\n");
          exit(-1);
	}
        **outptr = code;
        (*outptr)++;
        (*out_count)++;
	lastchar = code;
}
