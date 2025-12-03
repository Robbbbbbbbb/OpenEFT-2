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
      LIBRARY: AN2K - ANSI/NIST 2007 Reference Implementation
                    
      FILE:    DEC_JPEP2K
      AUTHOR:  Kenneth Ko
      DATE:    12/18/2007
      UPDATED: 10/02/2008 by Joseph C. Konczal - slight modification
                          to add support for 8-bit JP2 in addition to
                          24-bit.
               08/19/2016 (Kenneth Ko) - Add OpenJPEG2 support
               02/25/2015 (Kenneth Ko) - Updated everything related to
                                         OPENJPEG to OPENJP2
               02/26/2015 (Kenneth Ko) - Fixed JP2 image decoding issue
      
      Contains routines responsible for decoding image data contained
      in image records according to the ANSI/NIST 2007 standard.
      
***********************************************************************/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <jpeg2k.h>

#ifdef __NBIS_OPENJP2__
	#include "openjp2/openjpeg.h"
   #include "openjp2/cio.h"
   #ifdef OPJ_HAVE_LIBLCMS2
   #include <lcms2.h>
   #endif
   #ifdef OPJ_HAVE_LIBLCMS1
   #include <lcms.h>
   #endif

#endif

#ifdef __NBIS_JASPER__
	#include "jasper/jas_types.h"
#endif

#ifdef __NBIS_JASPER__
int jpeg2k_decode_mem(IMG_DAT **oimg_dat, int *lossyflag, 
                      unsigned char *idata, const int ilen)
{
   IMG_DAT           *img_dat;
   jas_image_t       *image;
   jas_stream_t      *in;

   if (jas_init() != 0){
      fprintf(stderr, "ERROR : jpeg2k_decode_mem: init : jas\n");
      return(-1);
   }

   /* The input image is to be read from buffer stream. */
   if ((in = jas_stream_memopen((char *)idata, ilen)) == NULL){
      fprintf(stderr, "ERROR : jpeg2k_decode_mem: failed to open jas stream\n");
      return(-2); 
   }

   /* Get the input image data. */
   if ((image = jas_image_decode(in, -1, (char *)NULL)) == NULL){
      fprintf(stderr, "error: cannot load image data\n");
      return(-3);
   }

   /* Early cleanup--input stream no longer needed. */
   (void) jas_stream_close(in);

   /* Gerenate Image data sturcture. */
   if ((img_dat_generate(&img_dat, image)) != 0){
      jas_image_destroy(image);
      fprintf(stderr, "ERROR : jpeg2k_decode_mem: failed to generate img_dat\n");
      return(-5);
   }

   /* General clean up. */  
   jas_image_destroy(image);
 
   *oimg_dat = img_dat;
   *lossyflag = 0;

   return(0);
}

int img_dat_generate(IMG_DAT **oimg_dat, jas_image_t *image)
{
   IMG_DAT *img_dat;
   int i;
   int cmptscnt;
   int rwcnt;
   int max_hor, max_vrt;

   /* Allocate memory for image data structure. */
   img_dat = (IMG_DAT *)calloc(1, sizeof(IMG_DAT));
   if(img_dat == NULL){
      fprintf(stderr, "ERROR : img_dat_generate : "
              "calloc : img_dat (%lu bytes)\n", (unsigned long)sizeof(IMG_DAT));
      return(-2);
   }

   /* Get component count. */
   cmptscnt = jas_image_numcmpts(image);

   /* Initialize img_dat info */
   img_dat->max_width = jas_image_brx(image);
   img_dat->max_height = jas_image_bry(image);
   img_dat->ppi = -1;
   img_dat->intrlv = 0;
   img_dat->n_cmpnts = cmptscnt;
   img_dat->cmpnt_depth = jas_image_cmptprec(image, 0);

   max_hor = -1;
   max_vrt = -1;
   img_dat->pix_depth = 0;

   for(i = 0; i < cmptscnt; i++){
      int cdepth;

      img_dat->hor_sampfctr[i] = 1;
      img_dat->vrt_sampfctr[i] = 1;
      cdepth = jas_image_cmptprec(image, i);
      img_dat->pix_depth += cdepth;
      if (cdepth != img_dat->cmpnt_depth) {
         fprintf(stderr, "WARNING : img_dat_generate: "
                 "unequal component depths, 0: %d and %d: %d\n",
                 i, cdepth, jas_image_cmptprec(image, i));
      }

      if(max_hor < img_dat->hor_sampfctr[i])
         max_hor = img_dat->hor_sampfctr[i];
      if(max_vrt < img_dat->vrt_sampfctr[i])
         max_vrt = img_dat->vrt_sampfctr[i];
   }

   for(i = 0; i < img_dat->n_cmpnts; i++){
      img_dat->samp_width[i] = (int)ceil(img_dat->max_width *
                   (img_dat->hor_sampfctr[i]/(double)max_hor));
      img_dat->samp_height[i] = (int)ceil(img_dat->max_height *
                   (img_dat->vrt_sampfctr[i]/(double)max_vrt));
   }

   /* Put the image raw pixels to image data sturcture component plains. */
   cmptscnt = jas_image_numcmpts(image);
   for (i = 0; i < cmptscnt; i++){
      jas_stream_rewind(image->cmpts_[i]->stream_);
      rwcnt = jas_stream_getrwcount(image->cmpts_[i]->stream_) - 1;
      img_dat->image[i]  = (unsigned char *)malloc(rwcnt * sizeof(unsigned char));
      if(img_dat->image[i] == NULL){
         fprintf(stderr, "ERROR : get_raw_image: "
                 "calloc : img_dat->image[%d] (%lu bytes)\n",
                 i, (unsigned long)(rwcnt * sizeof(unsigned char)));
         return(-3);
      }
      jas_stream_read(image->cmpts_[i]->stream_, img_dat->image[i], rwcnt);
   }

   *oimg_dat = img_dat;

   return(0);
}
#endif

#ifdef __NBIS_OPENJP2__
#define J2K_CFMT 0
#define JP2_CFMT 1
#define JPT_CFMT 2

#define PXM_DFMT 10
#define PGX_DFMT 11
#define BMP_DFMT 12
#define YUV_DFMT 13
#define TIF_DFMT 14
#define RAW_DFMT 15
#define TGA_DFMT 16

static void error_callback(const char *msg, void *client_data)
{
   (void)client_data;
   fprintf(stdout, "[ERROR] %s", msg);
}
static void warning_callback(const char *msg, void *client_data)
{
   (void)client_data;
   fprintf(stdout, "[WARNING] %s", msg);
}
static void info_callback(const char *msg, void *client_data)
{
   (void)client_data;
   fprintf(stdout, "[INFO] %s", msg);
}

static void opj_free_from_idata(void *p_user_data)
{
   /* Intentionally Leave Blank */
}

static OPJ_UINT64 opj_get_data_length_from_idata(OPJ_SIZE_T p_nb_bytes)
{
   return (OPJ_UINT64)p_nb_bytes;
}

static OPJ_SIZE_T opj_read_from_idata(void * p_buffer, OPJ_SIZE_T p_nb_bytes,
                                      void * p_user_data)
{
   struct opj_dstream *pb = (struct opj_dstream *)p_user_data;

   if (pb->status != 0)
   {
      memcpy(p_buffer, pb->data, p_nb_bytes);
      pb->status = 0;
      return p_nb_bytes;
   }
   else
   {
      pb->status = 0;
      return -1;
   }
}


int openjpeg2k_decode_mem(IMG_DAT **oimg_dat, int *lossyflag,
                      unsigned char *idata, const int ilen)
{
   IMG_DAT *img_dat;
   opj_dparameters_t parameters;   
   opj_image_t *image = NULL;
   opj_stream_t *l_stream = NULL;
   opj_codec_t* l_codec = NULL; 
   signed char* sgnd_buf = NULL;
   unsigned char* unsgnd_buf = NULL;
   struct opj_dstream s_stream;
   int size;

   /* set decoding parameters to default values */
   opj_set_default_decoder_parameters(&parameters);
   
   /* create l_stream */
   l_stream = opj_stream_create(ilen, OPJ_TRUE);
   if (! l_stream)
   {
      return EXIT_FAILURE;
   }
   
   /* Initialize opj_dstream structure */
   s_stream.status = 1;
   s_stream.data = idata;


   /* Set stream infromation */
   opj_stream_set_user_data(l_stream, &s_stream,
                            (opj_stream_free_user_data_fn) opj_free_from_idata);
   opj_stream_set_user_data_length(l_stream,
                                   opj_get_data_length_from_idata(ilen));
   
   opj_stream_set_read_function(l_stream,
                                (opj_stream_read_fn) opj_read_from_idata);
    
   /* Get a decoder handle */
   l_codec = opj_create_decompress(OPJ_CODEC_JP2);
   
   /* catch events using our callbacks and give a local context */
   opj_set_info_handler(l_codec, info_callback,00);
   opj_set_warning_handler(l_codec, warning_callback,00);
   opj_set_error_handler(l_codec, error_callback,00);
   
   /* Setup the decoder decoding parameters using user parameters */
   if ( !opj_setup_decoder(l_codec, &parameters) )
   {
      fprintf(stderr, "ERROR -> opj_compress: failed to setup the decoder\n");
      opj_stream_destroy(l_stream);
      opj_destroy_codec(l_codec);
      return EXIT_FAILURE;
   }
   
   /* Read the main header of the codestream and if necessary the JP2 boxes*/
   if(! opj_read_header(l_stream, l_codec, &image)){
      fprintf(stderr, "ERROR -> opj_decompress: failed to read the header\n");
      opj_stream_destroy(l_stream);
      opj_destroy_codec(l_codec);
      opj_image_destroy(image);
      return EXIT_FAILURE;
   }
   
   if (!parameters.nb_tile_to_decode)
   {
      /* Optional if you want decode the entire image */
      if (!opj_set_decode_area(l_codec, image, (OPJ_INT32)parameters.DA_x0,
            (OPJ_INT32)parameters.DA_y0, (OPJ_INT32)parameters.DA_x1,
            (OPJ_INT32)parameters.DA_y1))
      {
         fprintf(stderr,	"ERROR -> opj_decompress: failed to set the decoded area\n");
         opj_stream_destroy(l_stream);
         opj_destroy_codec(l_codec);
         opj_image_destroy(image);
         return EXIT_FAILURE;
      }
      
      /* Get the decoded image */
      if (!(opj_decode(l_codec, l_stream, image) && opj_end_decompress(l_codec,	l_stream)))
      {
         fprintf(stderr,"ERROR -> opj_decompress: failed to decode image!\n");
         opj_destroy_codec(l_codec);
         opj_stream_destroy(l_stream);
         opj_image_destroy(image);
         return EXIT_FAILURE;
      }
   }
   else
   {
      if (!opj_get_decoded_tile(l_codec, l_stream, image, parameters.tile_index))
      {
         fprintf(stderr, "ERROR -> opj_decompress: failed to decode tile!\n");
         opj_destroy_codec(l_codec);
         opj_stream_destroy(l_stream);
         opj_image_destroy(image);
         return EXIT_FAILURE;
      }
      fprintf(stdout, "tile %d is decoded!\n\n", parameters.tile_index);
   }
   
   /* Close the byte stream */
   opj_stream_destroy(l_stream);

   /* Calualate buffer size to hold decoded image */
   size = image->numcomps * image->comps[0].w * image->comps[0].h;

   /* only support unsigned jp2 image */
   if (image->comps[0].sgnd == 1)
   {
      fprintf(stderr, "ERROR -> Can't decode signed buffer!\n");
      opj_image_destroy(image);
      return(-2);
   }

   /* allocate buf for the Raw image */
   unsgnd_buf = (unsigned char *) malloc(size * sizeof(unsigned char));

   /* convert JP2 to Raw */
   if (image_to_raw(image, unsgnd_buf))
   {
      fprintf(stderr, "ERROR -> image_to_raw!\n");
      opj_image_destroy(image);
      return(-3);
   }

   /* consturct img_dat format */
   if ((img_dat_generate_openjpeg(&img_dat, image, unsgnd_buf)) != 0){
      free(unsgnd_buf);
      opj_image_destroy(image);
      fprintf(stderr, "ERROR : openjpeg2k_decode_mem: failed to generate img_dat\n");
      return(-4);
   }

   free(unsgnd_buf);

   opj_image_destroy(image);

   *oimg_dat = img_dat;
   *lossyflag = 0;

   return(0);
}

int img_dat_generate_openjpeg(IMG_DAT **oimg_dat, opj_image_t *image, unsigned char* unsgnd_buf)
{
   IMG_DAT *img_dat;
   int i;
   int cmptscnt;
   int rwcnt;
   int max_hor, max_vrt;
   unsigned char *nptr;

   /* Allocate memory for image data structure. */
   img_dat = (IMG_DAT *)calloc(1, sizeof(IMG_DAT));
   if(img_dat == NULL){
      fprintf(stderr, "ERROR : img_dat_generate : "
              "calloc : img_dat (%lu bytes)\n", (unsigned long)sizeof(IMG_DAT));
      return(-2);
   }

   /* Get component count. */
   cmptscnt = image->numcomps;

   /* Initialize img_dat info */
   img_dat->max_width = image->x1;
   img_dat->max_height = image->y1;
   img_dat->ppi = -1;
   img_dat->intrlv = 0;
   img_dat->n_cmpnts = cmptscnt;
   img_dat->cmpnt_depth = image->comps[0].prec;

   max_hor = -1;
   max_vrt = -1;
   img_dat->pix_depth = 0;

   for(i = 0; i < cmptscnt; i++){
      int cdepth;

      img_dat->hor_sampfctr[i] = 1;
      img_dat->vrt_sampfctr[i] = 1;
      cdepth = image->comps[0].prec;
      img_dat->pix_depth += cdepth;
      if (cdepth != img_dat->cmpnt_depth) {
         fprintf(stderr, "WARNING : img_dat_generate: "
                 "unequal component depths, 0: %d and %d: %d\n",
                 i, cdepth, image->comps[i].prec);
      }

      if(max_hor < img_dat->hor_sampfctr[i])
         max_hor = img_dat->hor_sampfctr[i];
      if(max_vrt < img_dat->vrt_sampfctr[i])
         max_vrt = img_dat->vrt_sampfctr[i];
   }

   for(i = 0; i < img_dat->n_cmpnts; i++){
      img_dat->samp_width[i] = (int)ceil(img_dat->max_width *
                   (img_dat->hor_sampfctr[i]/(double)max_hor));
      img_dat->samp_height[i] = (int)ceil(img_dat->max_height *
                   (img_dat->vrt_sampfctr[i]/(double)max_vrt));
   }

   /* Put the image raw pixels to image data sturcture component plains. */
   cmptscnt = image->numcomps;
   rwcnt = image->x1 * image->y1;
   nptr = unsgnd_buf;
   
   for (i = 0; i < cmptscnt; i++){
      img_dat->image[i] = (unsigned char *)malloc(rwcnt * sizeof(unsigned char));
      if(img_dat->image[i] == NULL){
         fprintf(stderr, "ERROR : get_raw_image: "
                 "calloc : img_dat->image[%d] (%lu bytes)\n",
                 i, (unsigned long)(rwcnt * sizeof(unsigned char)));
         return(-3);
      }
      memcpy(img_dat->image[i], nptr, rwcnt * sizeof(unsigned char));
      nptr += rwcnt;
   }

   *oimg_dat = img_dat;

   return(0);
}

int image_to_raw(opj_image_t* image, unsigned char* unsgnd_buf)
{
   int compno;
   int w, h;
   int line, row;
   int *ptr;
   int index;

   if((image->numcomps * image->x1 * image->y1) == 0)
   {
      fprintf(stderr,"\nError: invalid raw image parameters\n");
      return 1;
   }
   fprintf(stdout,"Raw image characteristics: %d components\n", image->numcomps);

   index = 0;
   for(compno = 0; compno < image->numcomps; compno++)
   {
      fprintf(stdout,"Component %d characteristics: %dx%dx%d %s\n", compno, image->comps[compno].w,
      image->comps[compno].h, image->comps[compno].prec, image->comps[compno].sgnd==1 ? "signed": "unsigned");

      w = image->comps[compno].w;
      h = image->comps[compno].h;

      if(image->comps[compno].prec <= 8)
      {
         if(image->comps[compno].sgnd == 0)
         {
            unsigned char curr;
            int mask = (1 << image->comps[compno].prec) - 1;
            ptr = image->comps[compno].data;
            for (line = 0; line < h; line++) {
               for(row = 0; row < w; row++)    {
                  curr = (unsigned char) (*ptr & mask);
                  unsgnd_buf[index] = curr;
                  ptr++;
                  index++;
               }
            }
         }
      }
      else if(image->comps[compno].prec <= 16)
      {
         fprintf(stderr,"More than 8 bits per component no handled yet\n");
         return 1;
      }
      else if (image->comps[compno].prec <= 32)
      {
         fprintf(stderr,"More than 16 bits per component no handled yet\n");
         return 1;
      }
      else
      {
         fprintf(stderr,"Error: invalid precision: %d\n", image->comps[compno].prec);
         return 1;
      }
   }

   return 0;
}

int get_file_format(char *filename) 
{
   unsigned int i;
   static const char *extension[] = {
       "pgx", "pnm", "pgm", "ppm", "bmp", "tif", "raw", "tga", "j2k", "jp2", "j2c"
   };
   static const int format[] = {
   PGX_DFMT, PXM_DFMT, PXM_DFMT, PXM_DFMT, BMP_DFMT, TIF_DFMT, RAW_DFMT, TGA_DFMT, J2K_CFMT, JP2_CFMT, J2K_CFMT};
   char * ext = strrchr(filename, '.');
   if (ext == NULL)
      return -1;
   ext++;
   for(i = 0; i < sizeof(format)/sizeof(*format); i++) {
      if(strncasecmp(ext, extension[i], 3) == 0) {
         return format[i];
      }
   }
   return -1;
}
#endif
