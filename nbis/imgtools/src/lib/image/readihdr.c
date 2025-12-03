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

      FILE:    READIHDR.C

      AUTHOR:  Michael Garris
      DATE:    04/28/1989
      UPDATED: 03/15/2005 by MDG

      Contains routines responsible for reading/reconstructing the
      pixmap contained in an IHead image file into memory.

      ROUTINES:
#cat: ReadBinaryRaster - reads the contents of a binary IHead image file
#cat:                   into an IHead structure and image memory.
#cat: ReadIheadRaster - reads the contents of a multi-level IHead image
#cat:                   file into an IHead structure and image memory.

***********************************************************************/
#include <img_io.h>

/************************************************************/
/*         Routine:   ReadBinaryRaster()                    */
/*         Author:    Michael D. Garris                     */
/*         Date:      4/28/89                               */
/*         Modifications:                                   */
/*           8/90 Stan Janet                                */
/*                     only malloc 1 buffer if data is not  */
/*                        compressed                        */
/*                     free() up temp buffer                */
/*           9/20/90 Stan Janet                             */
/*                     check return codes                   */
/*           1/11/91 Stan Janet                             */
/*                     put filename in error messages       */
/*           11/15/95 Patrick Grother			    */
/*                     use malloc instead of calloc	    */
/************************************************************/
/************************************************************/
/* ReadBinaryRaster() reads in a "headered" binary raster   */
/* file and returns an ihead structure, raster data, and    */
/* integer file specs.                                      */
/************************************************************/

void ReadBinaryRaster(file,head,data,bpi,width,height)
char *file;
IHEAD **head;
unsigned char **data;
int *bpi,*width,*height;
{
   FILE *fp;
   IHEAD *ihead;
   int outbytes, depth, comp, filesize, complen, n;
   unsigned char *indata, *outdata;

   /* open the image file */
   fp = fopen(file,"rb");
   if (fp == NULL)
      syserr("ReadBinaryRaster",file,"fopen");

   /* read in the image header */
   (*head) = readihdr(fp);
   ihead = *head;

   depth = get_depth(ihead);
   if(depth != 1)
      fatalerr("ReadBinaryRaster",file,"not a binary file");
   (*width) = get_width(ihead);
   (*height) = get_height(ihead);
   (*bpi) = get_density(ihead);
   comp = get_compression(ihead);
   complen = get_complen(ihead);

   /* allocate a raster data buffer */
   filesize = SizeFromDepth(*width, *height, depth);
   malloc_uchar(&outdata, filesize, "ReadIheadRaster : outdata");

   /* read in the raster data */
   if(comp == UNCOMP) {   /* file is uncompressed */
      n = fread(outdata,1,filesize,fp);
      if (n != filesize) {
	 (void) fprintf(stderr,
		"ReadBinaryRaster: %s: fread returned %d (expected %d)\n",
		file,n,filesize);
         exit(1);
      } /* IF */
   } else {
      malloc_uchar(&indata, complen, "ReadBinaryRaster : indata");
      n = fread(indata,1,complen,fp); /* file compressed */
      if (n != complen) {
         (void) fprintf(stderr,
		"ReadBinaryRaster: %s: fread returned %d (expected %d)\n",
		file,n,complen);
      } /* IF */
   }

   switch (comp) {
      case RL:
	 rldecomp(indata,complen,outdata,&outbytes,filesize);
         set_compression(ihead, UNCOMP);
         set_complen(ihead, 0);
         free((char *)indata);
         break;
      case CCITT_G4:
         if((*head)->sigbit == LSBF) {
           inv_bytes(indata, complen);
           (*head)->sigbit = MSBF;
           (*head)->byte_order = HILOW;
         }
	 grp4decomp(indata,complen,*width,*height,outdata,&outbytes);
         set_compression(ihead, UNCOMP);
         set_complen(ihead, 0);
         free((char *)indata);
         break;
      case UNCOMP:
         break;
      default:
         fatalerr("ReadBinaryRaster",file,"Invalid compression code");
      break;
   }

   *data = outdata;
   /* close the image file */
   (void) fclose(fp);
}

/************************************************************/
/*         Routine:   ReadIheadRaster()                     */
/*         Author:    Michael D. Garris                     */
/*         Date:      4/28/89                               */
/*         Modifications:                                   */
/*           8/90    (Stan Janet) see ReadBinaryRaster      */
/*           9/20/90 (Stan Janet)  "       "                */
/************************************************************/
/************************************************************/
/* ReadIheadRaster() reads in a "iheadered" raster file and */
/* returns an ihead structure, raster data, and integer file*/
/* specs.                                                   */
/************************************************************/
void ReadIheadRaster(file,head,data,width,height,depth)
char *file;
IHEAD **head;
unsigned char **data;
int *width,*height,*depth;
{
   FILE *fp;
   IHEAD *ihead;
   int outbytes, comp, filesize, complen, n;
   unsigned char *indata, *outdata;

   /* open the image file */
   fp = fopen(file,"rb");
   if (fp == NULL)
      syserr("ReadIheadRaster",file,"fopen failed");

   /* read in the image header */
   *head = readihdr(fp);
   ihead = *head;

   n = sscanf((*head)->compress,"%d",&comp);
   if (n != 1)
      fatalerr("ReadIheadRaster",file,"sscanf failed on compress field");

   /* convert string fields to integers */
   n = sscanf((*head)->depth,"%d",depth);
   if (n != 1)
      fatalerr("ReadIheadRaster",file,"sscanf failed on depth field");
   n = sscanf((*head)->width,"%d",width);
   if (n != 1)
      fatalerr("ReadIheadRaster",file,"sscanf failed on width field");
   n = sscanf((*head)->height,"%d",height);
   if (n != 1)
      fatalerr("ReadIheadRaster",file,"sscanf failed on height field");
   n = sscanf((*head)->complen,"%d",&complen);
   if (n != 1)
      fatalerr("ReadIheadRaster",file,"sscanf failed on complen field");

   /* allocate a raster data buffer */
   filesize = SizeFromDepth(*width,*height,*depth);
   malloc_uchar(&outdata, filesize, "ReadIheadRaster : outdata");

   /* read in the raster data */
   if(comp == UNCOMP) {   /* file is uncompressed */
      n = fread(outdata,1,filesize,fp);
      if (n != filesize) {
         (void) fprintf(stderr,
		"ReadIheadRaster: %s: fread returned %d (expected %d)\n",
		file,n,filesize);
         exit(1);
      } /* IF */
   } else {
      malloc_uchar(&indata, complen, "ReadIheadRaster : indata");
      n = fread(indata,1,complen,fp); /* file compressed */
      if (n != complen) {
         /* Typo corrected by MDG on 03-15-05 */
         /* arg 'file' added for "%s" */
         (void) fprintf(stderr,
		"ReadIheadRaster: %s: fread returned %d (expected %d)\n",
		file, n,complen);
         exit(1);
      } /* IF */
   }

   switch (comp) {
      case RL:
	rldecomp(indata,complen,outdata,&outbytes,filesize);
	memset((*head)->complen,0,SHORT_CHARS);
	memset((*head)->compress,0,SHORT_CHARS);
        (void) sprintf((*head)->complen,"%d",0);
        (void) sprintf((*head)->compress,"%d",UNCOMP);
        *data = outdata;
        free((char *)indata);
      break;
      case CCITT_G4:
        if((*head)->sigbit == LSBF) {
          inv_bytes(indata, complen);
          (*head)->sigbit = MSBF;
          (*head)->byte_order = HILOW;
        }
        grp4decomp(indata,complen,*width,*height,outdata,&outbytes);
	memset((*head)->complen,0,SHORT_CHARS);
	memset((*head)->compress,0,SHORT_CHARS);
        (void) sprintf((*head)->complen,"%d",0);
        (void) sprintf((*head)->compress,"%d",UNCOMP);
        *data = outdata;
        free((char *)indata);
      break;
      case UNCOMP:
        *data = outdata;
      break;
      default:
         fatalerr("ReadIheadRaster",file,"Invalid compression code");
      break;
   }
   /* close the image file */
   (void) fclose(fp);
}
