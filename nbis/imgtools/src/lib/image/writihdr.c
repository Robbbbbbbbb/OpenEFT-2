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

      FILE:    WRITIHDR.C

      AUTHOR:  Michael Garris
      DATE:    04/26/1989
      UPDATED: 03/15/2005 by MDG

      Contains routines responsible for encoding/writing an
      IHead image file.

      ROUTINES:
#cat: writeihdrfile - writes the contents of an IHead structure and an
#cat:                 image memory to the specified file.
#cat: writeihdrsubimage - generates an IHead based on the parameters passed
#cat:                     and writes a subimage to the specified file.
#cat: write_fields - takes a list of IHead structures and binary images
#cat:                and writes them to individual files with the specified
#cat:                root filename and corresponding extension name.

***********************************************************************/
#include <img_io.h>
#include <sys/param.h>

/************************************************************/
/*         Routine:   Writeihdrfile()                       */
/*         Author:    Michael D. Garris                     */
/*         Date:      4/26/89                               */
/*         Modifications:                                   */
/*           9/20/90   (Stan Janet) check return codes      */
/*           2/20/91   (MDG) compression capability         */
/*   Modified 12/94 by Patrick Grother                         */
/*         Allocate 4 times the original image for the rare    */
/*         case (noisy image) where the compressed result has  */
/*         grown in size to be larger than the original image  */

/************************************************************/
/************************************************************/
/* Writeihdrfile() writes a ihead structure and correspon-  */
/* ding image data to the specified file.                   */
/************************************************************/
void writeihdrfile(char *file, IHEAD *head, unsigned char *data)
{
   FILE *fp;
   int width,height,depth,code,filesize,n, compbytes;
   unsigned char *compdata;

   /* reopen the image file for writing */
   fp = fopen(file,"wb");
   if (fp == NULL)
      syserr("writeihdrfile","fopen",file);

   n = sscanf(head->width,"%d",&width);
   if (n != 1)
      fatalerr("writeihdrfile","sscanf failed on width field",NULL);
   n = sscanf(head->height,"%d",&height);
   if (n != 1)
      fatalerr("writeihdrfile","sscanf failed on height field",NULL);
   n = sscanf(head->depth,"%d",&depth);
   if (n != 1)
      fatalerr("writeihdrfile","sscanf failed on depth field",NULL);
   n = sscanf(head->compress, "%d", &code);
   if (n != 1)
      fatalerr("writeihdrfile","sscanf failed on compression code field",NULL);

   filesize = SizeFromDepth(width,height,depth);
   if ( code == CCITT_G4 )  /* G4 compression breaks for noisy images since */
      filesize *= 4;        /* the result is larger than the original. The  */
                            /* buffer is exceeded and a segmentation fault  */
                            /* occurs. The factor 4 is empirically found    */
                            /* to be sufficient.                            */

   if(code == UNCOMP){
      sprintf(head->complen, "%d", 0);
      writeihdr(fp,head);
      n = fwrite(data,1,filesize,fp);
      if (n != filesize)
         syserr("writeihdrfile", "fwrite", file);
   }
   else{
      malloc_uchar(&compdata, filesize, "writeihdrfile : compdata");
      switch(code){
      case CCITT_G3:
         fatalerr("writeihdrfile","G3 compression not implemented.",NULL);
         break;
      case CCITT_G4:
         if(depth != 1)
            fatalerr("writeihdrfile",
                     "G4 compression requires a binary image.", NULL);
         grp4comp(data, filesize, width, height, compdata, &compbytes);
         break;
      case RL:
         rlcomp(data, filesize, compdata, &compbytes, filesize);
         break;
      default:
         fatalerr("writeihdrfile","Unknown compression",NULL);
         break;
      }
      sprintf(head->complen, "%d", compbytes);
      writeihdr(fp,head);
      n = fwrite(compdata,1,compbytes,fp);
      if (n != compbytes)
         syserr("writeihdrfile", "fwrite", file);
      free(compdata);
   }

   /* closes the new file */
   (void) fclose(fp);
}

/************************************************************/
/*         Routine:   Writeihdrsubimage()                   */
/*         Author:    Michael D. Garris                     */
/*         Date:      5/29/91                               */
/************************************************************/
/************************************************************/
/* Writeihdrsubimage() writes a subimage raster along with  */
/* a modified ihead structure to the specified file.        */
/************************************************************/
void writeihdrsubimage(char *name, unsigned char *data,
                       int w, int h, int d, char *parent, int par_x, int par_y)
{
   IHEAD *ihead;

   if((ihead = (IHEAD *)malloc(sizeof(IHEAD))) == NULL)
      syserr("writeihdrsubimage", "malloc", "ihead");
   nullihdr(ihead);
   strcpy(ihead->id, name);
   strcpy(ihead->created,current_time());
   sprintf(ihead->width, "%d", w);
   sprintf(ihead->height, "%d", h);
   sprintf(ihead->depth, "%d", d);
   sprintf(ihead->compress, "%d", CCITT_G4);
   sprintf(ihead->align, "%d", 8);
   sprintf(ihead->unitsize, "%d", 8);
   ihead->sigbit = MSBF;
   ihead->byte_order = HILOW;
   sprintf(ihead->pix_offset, "%d", 0);
   sprintf(ihead->whitepix, "%d", 0);
   ihead->issigned = UNSIGNED;
   ihead->rm_cm = ROW_MAJ;
   ihead->tb_bt = TOP2BOT;
   ihead->lr_rl = LEFT2RIGHT;
   strcpy(ihead->parent, parent);
   sprintf(ihead->par_x, "%d", par_x);
   sprintf(ihead->par_y, "%d", par_y);
   writeihdrfile(ihead->id, ihead, data);
   free(ihead);
}

/************************************************************/
void write_fields(char *ofile, char **name_list, int num_names,
                  IHEAD **heads, unsigned char **fields,
                  int count, int compression)
{
  char outname[MAXPATHLEN], tail[MAXPATHLEN];
  int i;

  if(num_names != count)
     fatalerr("write_fields",
              "Table_A name list not equal to number of fields", NULL);
  for(i = 0; i < count; i++){
     sprintf(outname,"%s.%s", ofile, name_list[i]);
     strcpy(tail, outname);
     filetail(tail);
     set_id(heads[i], tail);
     set_compression(heads[i], compression);
     writeihdrfile(outname, heads[i], fields[i]);
   }
}
