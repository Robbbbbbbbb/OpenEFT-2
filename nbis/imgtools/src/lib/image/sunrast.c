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

      FILE:    SUNRAST.C

      AUTHOR:  Michael Garris
      DATE:    08/19/1990
      UPDATED: 03/15/2005 by MDG
      UPDATE:  12/02/2008 by Kenneth Ko - Fix to support 64-bit

      Contains routines responsible for reading and writing
      Sun Rasterfiles.

      ROUTINES:
#cat: ReadSunRaster -  takes the name of a binary or grayscale Sun Rasterfile
#cat:                  and loads the image into memory, returning relevant
#cat:                  image attributes.
#cat: WriteSunRaster - writes the given binary or grayscale image data to
#cat:                  the specified file in Sun Rasterfile format.

***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sunrast.h>
#include <binops.h>
#include <swap.h>

/* Grayscale RGB Colormap (static==>keep it local to the file for now). */
#define COLORMAP_LEN   768
static unsigned char colormap[COLORMAP_LEN] = {
  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

/************************************************************/
int ReadSunRaster(const char *ifile, SUNHEAD **osunhead,
            unsigned char **ocolormap, int *omaplen, unsigned char **odata,
            int *oscan_w, int *oimg_w, int *oimg_h, int *oimg_d)
{
   SUNHEAD *sunhead;
   FILE *fp;
   unsigned char *colormap, *idata;
   int scan_width;

   if((fp = fopen(ifile,"rb")) == NULL){
      fprintf(stderr,"ERROR : ReadSunRaster : fopen : %s\n", ifile);
      return(-2);
   }

   sunhead = (SUNHEAD *)malloc(sizeof(SUNHEAD));
   if(sunhead == (SUNHEAD *)NULL){
      fprintf(stderr,"ERROR : ReadSunRaster : malloc : sunhead\n");
      return(-3);
   }

   if(fread(sunhead, sizeof(SUNHEAD), 1, fp) != 1){
      fprintf(stderr,"ERROR : ReadSunRaster : fread : sunhead\n");
      free(sunhead);
      fclose(fp);
      return(-4);
   }

#ifdef __NBISLE__
/*
   fprintf(stderr, "Bytes in Rasterfile header being swapped.\n");
*/
   swap_uint_bytes(sunhead->magic);
   swap_uint_bytes(sunhead->width);
   swap_uint_bytes(sunhead->height);
   swap_uint_bytes(sunhead->depth);
   swap_uint_bytes(sunhead->raslength);
   swap_uint_bytes(sunhead->rastype);
   swap_uint_bytes(sunhead->maptype);
   swap_uint_bytes(sunhead->maplength);
#endif

   if(sunhead->rastype != SUN_STANDARD){
      free(sunhead);
      fprintf(stderr,
              "ERROR : ReadSunRaster : unsupported Sun raster type %d\n",
              sunhead->rastype);
      return(-5);
   }

   if(sunhead->maplength == 0){
      colormap = (unsigned char *)NULL;
   }
   else{
      colormap = (unsigned char *)malloc(sunhead->maplength);
      if(colormap == (unsigned char *)NULL){
         free(sunhead);
         fclose(fp);
         fprintf(stderr,"ERROR : ReadSunRaster : malloc : colormap\n");
         return(-6);
      }
      if(fread(colormap, 1, sunhead->maplength, fp) != sunhead->maplength){
         free(sunhead);
         free(colormap);
         fclose(fp);
         fprintf(stderr,"ERROR : ReadSunRaster : fread : colormap\n");
         return(-7);
      }
   }

   idata = (unsigned char *)malloc(sunhead->raslength);
   if(idata == (unsigned char *)NULL){
      free(sunhead);
      if(colormap != (unsigned char *)NULL)
         free(colormap);
      fclose(fp);
      fprintf(stderr,"ERROR : ReadSunRaster : malloc : idata\n");
      return(-8);
   }

   if(fread(idata, 1, sunhead->raslength,fp) != sunhead->raslength){
      free(sunhead);
      if(colormap != (unsigned char *)NULL)
         free(colormap);
      free(idata);
      fclose(fp);
      fprintf(stderr,"ERROR : ReadSunRaster : fread : colormap\n");
      return(-9);
   }

   fclose(fp);

   /* Sun Rasterfiles permit the image width to be less than the actual */
   /* scanline.  Return both the scan width and the image width, and    */
   /* let the application decide which it wants to deal with.           */
   if((sunhead->depth == 1) &&
      ((sunhead->width * sunhead->height)>>3 != sunhead->raslength)){
      scan_width = (sunhead->raslength / sunhead->height)<<3;
   }
   else
      scan_width = sunhead->width;

   *osunhead  = sunhead;
   *ocolormap = colormap;
   *omaplen   = sunhead->maplength;
   *odata     = idata;
   *oscan_w   = scan_width;
   *oimg_w    = sunhead->width;
   *oimg_h    = sunhead->height;
   *oimg_d    = sunhead->depth;

   return(0);
}

/************************************************************/
int WriteSunRaster(char *ofile, unsigned char *data,
                    const int width, const int height, const int depth)
{
   SUNHEAD sunhead;
   int ret, pad = 0;
   int ras_length, map_length;
   int iw, ih;
   FILE *fp;

   iw = width;
   ih = height;

   switch (depth){
   case 1:
      if(iw%8){
         fprintf(stderr,
    "ERROR : WriteSunRaster : pixel width of bitmap must be multiple of 8\n");
         return(-2);
      }
      /* Sun rasterfile bitmaps need to be 16-bit aligned in width. */
      if(iw%16){
         /* NOTE: Data will point to a new image memory, and the memory  */
         /*       previously pointed to WILL remain allocated/unchanged. */
         /*       I have accounted for this.                             */
         if((ret = binary_image_mpad(&data, (unsigned int *)&iw,
                                            (unsigned int *)&ih,
                                            16, 1, 0)) <= 0)
            return(-3);
         pad = 1;
      }
      sunhead.raslength = (iw * ih)>>3;
      sunhead.maplength = 0;
      sunhead.maptype   = MAP_NONE;
   break;
   case 8:
      sunhead.raslength = iw * ih;
      sunhead.maplength = COLORMAP_LEN;
      sunhead.maptype   = MAP_EQUAL_RGB;
   break;
   default:
      fprintf(stderr, "ERROR : WriteSunRaster : can't handle depth = %d\n",
              depth);
      return(-4);
   }

   sunhead.magic   = SUN_MAGIC;
   sunhead.width   = width;
   sunhead.height  = ih;
   sunhead.depth   = depth;
   ras_length        = sunhead.raslength;
   map_length        = sunhead.maplength;
   sunhead.rastype = SUN_STANDARD;

#ifdef __NBISLE__
/*
   fprintf(stderr, "Bytes in Rasterfile header being swapped.\n");
*/
   swap_uint_bytes(sunhead.magic);
   swap_uint_bytes(sunhead.width);
   swap_uint_bytes(sunhead.height);
   swap_uint_bytes(sunhead.depth);
   swap_uint_bytes(sunhead.raslength);
   swap_uint_bytes(sunhead.rastype);
   swap_uint_bytes(sunhead.maptype);
   swap_uint_bytes(sunhead.maplength);
#endif

   if ((fp = fopen(ofile, "wb")) == (FILE *)NULL){
      fprintf(stderr, "ERROR : WriteSunRaster : fopen : %s\n", ofile);
      return(-5);
   }

   if (1 != fwrite(&sunhead, sizeof(SUNHEAD), 1, fp)){
      fprintf(stderr, "ERROR : WriteSunRaster : fwrite : sunhead\n");
      return(-6);
   }

/*
   printf("wrote header = %d bytes\n", sizeof(SUNHEAD));
*/

   if (map_length){
      if (map_length != fwrite(colormap, 1, map_length, fp)){
         fprintf(stderr, "ERROR : WriteSunRaster : fwrite : colormap\n");
         return(-7);
      }
   }

/*
   printf("wrote colormap = %d bytes\n", map_length);
*/

   if (ras_length != fwrite(data, 1, ras_length,fp)){
      fprintf(stderr, "ERROR : WriteSunRaster : fwrite : data\n");
      return(-8);
   }

   /* Image_mpad makes a copy of the input image data, so free up    */
   /* the copy.  The caller still points to the original image data. */
   if(pad)
      free(data);

/*
   printf("wrote image = %d bytes\n", ras_length);
*/

   if (ferror(fp)){
      (void)unlink(ofile);
      fprintf(stderr, "ERROR : WriteSunRaster : ferror : %s\n", ofile);
      return(-9);
   }

   if (fclose(fp) == EOF){
      fprintf(stderr, "ERROR : WriteSunRaster : fclose : %s\n", ofile);
      return(-10);
   }

   return(0);
}
