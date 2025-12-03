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

      FILE:    GRP4COMP.C
      AUTHORS: CALS Test Network

      DATE:    01/25/1990

      Contains routines responsible for CCITT Group 4 encoding
      a binary image pixel datastream.

      ROUTINES:
#cat: grp4comp - CCITT Group 4 compresses a binary image (bitmap).
#cat:

***********************************************************************/

/********************************************************************
*   Modified:   Darlene E. Frederick				    *
*               Michael D. Garris                                   *
*   Date:	January 25, 1990				    *
*   Modified 12/90 by Stan Janet                                    *
*		flush_buffer() was adding an extra byte to data     *
*		whether it was already byte-aligned or not          *
*   Modified 12/94 by Patrick Grother                               *
*               Reclared the all variables of type "short" to be    *
*               "int", using a macro SHORT defined in the include   *
*               file grp4comp.h.                                    *
*               On images with more than 2^15 rows the              *
*               result was garbage because of an overflowed line    *
*               counter. The new declaration has a limit of 2^31    *
*   Updated:	03/15/2005 by MDG                                   *
*				   				    *
*   Contents:   ccitt4_compress()				    *
*		read_uncompressed_file_into_memory()		    *
*		control_compression()				    *
*		prepare_to_compress()				    *
*		compress_image()				    *
*		make_array_of_changing_elements()		    *
*		set_up_first_and_last_changing_elements_c()	    *
*		prepare_to_compress_next_line()			    *
*		set_up_first_line()				    *
*		crash_c()				  	    *
*								    *	
********************************************************************/

#ifdef TIME
#include <sys/time.h>
#endif
#include <memory.h>
#include <grp4comp.h>

/* Added by MDG in order have option of passing alloc responsibilities */
/* to caller */
#define NOALLOC 0
#define ALLOC	1
int comp_alloc_flag = ALLOC;
int comp_write_init_flag;
#ifdef TIME
  struct timeval  t1, t2;
  struct timezone tz;
#endif



/***********************************************************************
*   grp4comp is the main routine of this file.  It does pre-           *
*   liminary setup, calls routines, and does final processing.         *
************************************************************************/

/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   indata - buffer containing the uncompressed data.   *
*		   inbytes - the number of bytes in indata.            *
*  		   width - Width in pixels of scan line in indata.     *
*  		   height - Number of lines in indata.                 *
*	Returned:          					       *
*		   outdata - buffer containing the compressed data.    *
*		   outbytes - the number of bytes in outdata.          *
************************************************************************/

void grp4comp(unsigned char *indata, int inbytes, int width, int height,
             unsigned char *outdata, int *outbytes)
{
   struct uncompressed_descriptor uncompressed;
   struct compressed_descriptor compressed;

       	uncompressed.pixels_per_line = width;
       	uncompressed.number_of_lines = height;
        uncompressed.data = indata;
        comp_alloc_flag = NOALLOC;
        comp_write_init_flag = True;
	read_uncompressed_file_into_memory( &uncompressed);
        compressed.data = outdata;
	control_compression( &uncompressed, &compressed );
        *outbytes = compressed.length_in_bytes;
/*	printf("\ncompressed:lines: %d, pixels:%d, length:%d\n",
	 compressed.number_of_lines, compressed.pixels_per_line, 
	 compressed.length_in_bytes); */
}


/***************************** control_compression **************************
	
		calls the functions that compress the image
			
*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   compressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
************************************************************************/
void control_compression(struct uncompressed_descriptor *uncompressed,
                         struct compressed_descriptor *compressed)
{
struct parameters 				 sole_parameters;
struct parameters 				*params = &sole_parameters;

#ifdef TIME
  SHORT i;
            tz.tz_minuteswest = 0;
            tz.tz_dsttime     = 0;
            gettimeofday(&t1, &tz); 
#endif

        
	prepare_to_compress( uncompressed, compressed, params );
	compress_image( uncompressed, compressed, params );
        /* memory deallocation added by Michael D. Garris 2/26/90 */
        free(params->reference_line);
        free(params->coding_line);
#ifdef TIME
           gettimeofday(&t2, &tz); 
           printf("\ntime difference: %ld:%ld\n", t2.tv_sec - t1.tv_sec,
           t2.tv_usec - t1.tv_usec);
           for(i=0; i<5; i++) printf("%c",'\07');*/
#endif

}


/************************ read_uncompressed_file_into_memory *******************
	
		allocates memory for the uncompressed image.		
			
*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
************************************************************************/
void read_uncompressed_file_into_memory(
                       struct uncompressed_descriptor *uncompressed)
{
int file_size;

     if(comp_alloc_flag){
	file_size = uncompressed->pixels_per_line * uncompressed->number_of_lines
	 / Pixels_per_byte;
	 
	if((uncompressed->data = (unsigned char *)calloc( file_size,
                                  sizeof(unsigned char) )) == NULL) {
 	    printf("\nCannot allocate enough memory for uncomp file.\n");
 	    crash_c();
 	}
     }
     else
        if(uncompressed->data == NULL){
           printf("\nNo memory allocated for input data!\n");
           crash_c();
        }
} /* end read_uncompressed_file_into_memory() */



/*************************** prepare_to_compress ****************************
	
		initializes variables in preperation for compression		
			
*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   compressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
*		   params - structure storing information needed for   *
*		   	    comparison and other tasks.		       *
************************************************************************/
void prepare_to_compress(struct uncompressed_descriptor *uncompressed,
                         struct compressed_descriptor *compressed,
                         struct parameters *params)
{
	        	
	params->max_pixel   	    = uncompressed->pixels_per_line;
	compressed->pixels_per_line = uncompressed->pixels_per_line;
	compressed->number_of_lines = uncompressed->number_of_lines;
	
	set_up_first_line_c( params ); 
	prepare_to_write_bits_c( compressed );	
	 
} /* end prepare_to_compress() */



/****************************** compress_image *******************************
	
				compresses the image			
			
*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   compressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void compress_image(struct uncompressed_descriptor *uncompressed,
                    struct compressed_descriptor *compressed,
                    struct parameters *params)
{
SHORT  line;

	for(line = 0; line < uncompressed->number_of_lines; line++) {		
	     make_array_of_changing_elements( params, uncompressed, line );
	     set_up_first_and_last_changing_elements_c( params );
	     compress_line( params );
	     prepare_to_compress_next_line( params );
	} /* end for each line loop */
	
	write_bits_c("000000000001000000000001");
	compressed->length_in_bytes = flush_buffer();
}



/************************ make_array_of_changing_elements *********************
	
	stores in a list pointed to by "params->coding_line" the pixel numbers
	of all the changing elements in the coding line
	 						
*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*		   line_number -  the number of the line in the image  *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void make_array_of_changing_elements(struct parameters *params,
                            struct uncompressed_descriptor *uncompressed,
                            SHORT line_number)
{
SHORT 	bytes_per_line;
int	line_offset;
SHORT 	byte_offset;

	bytes_per_line = params->max_pixel / Pixels_per_byte;
	line_offset = bytes_per_line * line_number;
	for(byte_offset=0; byte_offset < bytes_per_line; byte_offset++) {
	    process_char(*(uncompressed->data+line_offset+byte_offset),params);
	} 
	
}	/* end make_array_of_changing_elements() */



/******************* set_up_first_and_last_changing_elements_c *****************
	
	initializes the first and last changing elements in the coding line			
			
******************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void set_up_first_and_last_changing_elements_c(struct parameters *params)
{
	*(params->coding_line) = Invalid;
	*(params->coding_line + ++params->index) = params->max_pixel;
	*(params->coding_line + ++params->index) = params->max_pixel;
	*(params->coding_line + ++params->index) = params->max_pixel;
	
	/* the previous lines may be necessary if when searching for b1, you
	skip some elements because you know that they are the wrong color */
}


/************************ prepare_to_compress_next_line ***********************
	
	initializes variables in preperation for compressing another line	
			
******************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void prepare_to_compress_next_line(struct parameters *params)
{
SHORT *temp;

	/* swap the reference and unchanged coding lines */
	
	temp = params->reference_line;
	params->reference_line = params->coding_line;
	params->coding_line = temp;
	
	params->pixel = 0;
	params->index = 0;
	params->previous_color = White;
	
} /* end prepare_to_read_next_line() */
	


/******************************* set_up_first_line_c ***************************
	
	initializes variables in preperation for compressing the first line		
			
******************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void set_up_first_line_c(struct parameters *params)
{

	params->reference_line = (SHORT *) malloc( (params->max_pixel +
                                           Extra_positions) * sizeof(SHORT) );
	params->coding_line = (SHORT *) malloc( (params->max_pixel +
                                        Extra_positions) * sizeof(SHORT) );
	
	*(params->reference_line + 0) = Invalid;		
	*(params->reference_line + 1) = params->max_pixel;
	*(params->reference_line + 2) = params->max_pixel;
	*(params->reference_line + 3) = params->max_pixel;
	
	/* initialize first changing element on coding line (A0 = -1) */
	*(params->coding_line) = Invalid;
	
	params->pixel = 0;
	params->index = 0;
	params->previous_color = White;
	
} /* end set_up_first_line_c() */



/*********************************** crash_c ***********************************
	
		forces the program to crash and create a core file
			
*****************************************************************************/
void crash_c()
{
FILE *crash_program = NULL;
	fprintf(crash_program,"This will kill the program and create a core file");
}


/***************************************************************************/
/* Originally mode.c                                                       */
/***************************************************************************/

static SHORT	A0,		A0_color,
				A1,		a2,
				b1,		b2;



/******************************* compress_line ********************************
	
			compresses a single line of the image
			
*****************************************************************************/
void compress_line(struct parameters *params)
{
	
#if Debug
	static SHORT line = 0;
	printf("\nLINE %d.  ",line);
	line++;
#endif

	A0 = Invalid; /* set A0 equal to imaginary first array element */
	A0_color = White; 
	A1 = 1;
	initialize_b1(params);
	b2 = b1 + 1;
	
#if Debug
	printf("\nA0:%d  A1:%d  b1:%d  b2:%d  ",
     A0, *(params->coding_line+A1), 
     *(params->reference_line+b1),*(params->reference_line+b2));
#endif
		
	do {	
			 
		if (*(params->reference_line + b2) < *(params->coding_line + A1)) {
			pass_mode_c(params);
			continue;
		} else 	
			if (abs(*(params->coding_line+A1)-*(params->reference_line+b1)) <=3)
			 	vertical_mode_c(params);
		    else 
				horizontal_mode_c(params);				
#if Debug
	printf("\nA0:%d  A1:%d  b1:%d  b2:%d  ",
	 A0, *(params->coding_line+A1), 
	 *(params->reference_line+b1),*(params->reference_line+b2));
#endif
	 
	} while( A0 < params->max_pixel);
	
} 


/******************************* initialize_b1 ********************************
	
		locates b1's first position in the reference line
			
*****************************************************************************/
void initialize_b1(struct parameters *params)
{
SHORT last_bit_of_b1;

	b1 = 1;
	last_bit_of_b1 = b1 & Last_bit_mask;
		
	while( ((*(params->reference_line +b1) <=A0) || (A0_color ==last_bit_of_b1))
	 && (*(params->reference_line + b1) < params->max_pixel) ){
	 	b1++;
 		last_bit_of_b1 = b1 & Last_bit_mask;
	} /* end while loop */
	
#if Debug
	printf("\nb1:%d :%d, A0:%d", b1, 
	 *(params->reference_line+b1), A0);
#endif 
	 
}


/********************************** pass_mode_c ********************************
	
				compresses a pass mode
			
*****************************************************************************/
void pass_mode_c(struct parameters *params)
{
	write_bits_c("0001");
	
#if Debug
	printf(" P ");
#endif 
	
	/*
	 * Reset the value A0 points to to a'0 (the value that b2 points to).
	 */
	 	
	A0 = *(params->reference_line + b2);
	
	/*
	 * Since A0 is now greater than the pixel b1 points to, both b1 and b2
	 * must be advanced twice to maintain the color difference between A0 and 
	 * b1, and the positional requirement that b1 point to a pixel greater than
	 * the one A0 points to.
	 */
	 
	 b1 += 2;
	 b2 += 2;
	 
	 /* 
	  * Note that the b's can be advanced by two positions without fear of
	  * moving them beyond the last changing element because pass_mode cannot
	  * occur if b2 is already pointing to max_pixel.
	  */
	  
} 


/****************************** vertical_mode_c ********************************
	
			compresses a vertical mode
			
*****************************************************************************/
void vertical_mode_c(struct parameters *params)
{
SHORT difference;

	difference = *(params->coding_line + A1) - *(params->reference_line + b1);
	A0 = *(params->coding_line + A1);
	A0_color = !A0_color;
	A1++;
	
#if Debug
				printf(" V%d ", difference);
#endif

	switch(difference) {

	case 0: 	
		write_bits_c("1");
		if(*(params->reference_line + b1) != params->max_pixel ) {
		    b1++;
		    b2++;
		} /* end if b1 is not on the last changing element */
		break;

	case 1: 		
		write_bits_c("011");
		b1++;
		b2++;
		if((*(params->reference_line + b1) <= A0)  && 
		   (*(params->reference_line + b1) != params->max_pixel) ) {
		    b1 += 2;
		    b2 += 2;
		}
		break;

	case -1: 	
		write_bits_c("010");
		if(*(params->reference_line + b1) != params->max_pixel ) {
		   b1++;
		   b2++;
		} /* end if b1 is not on the last changing element */
		break;

	case 2: 	
		write_bits_c("000011");
		b1++;
		b2++;
		if((*(params->reference_line + b1) <= A0)  && 
		   (*(params->reference_line + b1) != params->max_pixel) ) {
		    b1 += 2;
		    b2 += 2;
		}
		break;

	case -2: 	
		write_bits_c("000010");
		if(*(params->reference_line + b1 - 1) > A0 ) {
		   b1--;
		   b2--;
		} else if(*(params->reference_line + b1) != params->max_pixel){
			  b1++;
			  b2++;
			}						
		break;

	case 3: 	
		write_bits_c("0000011");
		b1++;
		b2++;
		while ((*(params->reference_line + b1) <= A0)  && 
		       (*(params->reference_line + b1) != params->max_pixel) ) {
			b1 += 2;
			b2 += 2;
		}
		break;

	case -3: 	
		write_bits_c("0000010");
		if(*(params->reference_line + b1 - 1) > A0 ) {
		   b1--;
		   b2--;
		} else if(*(params->reference_line + b1) != params->max_pixel){
			  b1++;
			  b2++;
			}				
		break;

	default: 
		printf("ERROR in vertical_mode_c() ");

	} /* end case of difference */	
	
} 


/**************************** horizontal_mode_c ********************************
	
			compresses a horizontal mode
			
*****************************************************************************/
void horizontal_mode_c(struct parameters *params)
{
SHORT run_length;

#if Debug
	printf(" a2:%d   H ",*(params->coding_line + a2));
#endif
	
	a2 = A1 + 1;
	write_bits_c("001");
	
	if(A0 == Invalid) /* on imaginary first pixel */
	   run_length = *(params->coding_line + A1);		
	else
	   run_length = *(params->coding_line + A1) - A0;
	write_run_length(run_length, A0_color );
	/* the last bit contains the color of the changing element */
	
	run_length = *(params->coding_line + a2) - *(params->coding_line + A1);
	write_run_length(run_length, !A0_color);
	
	/*
	 *  Must use !A0_color instead of A1 because in cases in which A1 occurs
	 *  on max_pixel, its color is bogus.
	 */
	
	/* NOTE: is the above statement true? if A1 were on max_pixel, you should
	not get horizontal mode. */
	
	 	
	A0 = *(params->coding_line + a2);
	A1 = a2 + 1;
	
	while((*(params->reference_line + b1) <= *(params->coding_line + a2)) &&
	       ( *(params->reference_line + b1) < params->max_pixel)  )
	{
   	   b1 += 2; /* must move ahead by 2 to maintain color difference with */
	   b2 += 2; /* A0, whose color does not change in this mode. */
	}
		
} 


/***************************************************************************/
/* Originally write_bits_c.c                                               */
/***************************************************************************/

static SHORT  bit_place_mark;	
	
static int  byte_place_mark;

static unsigned char  *output_area;
		
static char  write_one[Pixels_per_byte] = 
{
	(char)0x80,
	(char)0x40,
	(char)0x20,
	(char)0x10,
	(char)0x8,
	(char)0x4,
	(char)0x2,
	(char)0x1,
};

static char  write_zero[Pixels_per_byte] =
{
	(char)0x7F,
	(char)0xBF,
	(char)0xDF,
	(char)0xEF,
	(char)0xF7,
	(char)0xFB,
	(char)0xFD,
	(char)0xFE,
};


/*************************** prepare_to_write_bits_c **************************
	
	initializes variables in preperation for writing compressed images	
			
*****************************************************************************/
void prepare_to_write_bits_c(struct compressed_descriptor *compressed)
{
     if(comp_alloc_flag){
	compressed->data = (unsigned char *)calloc(
                            (compressed->pixels_per_line * 
                            compressed->number_of_lines / Pixels_per_byte),
                            sizeof(unsigned char) );
     }
   /*
    *  This allocation is usually very wasteful, but because there is no
    *  way of knowing how much space is needed, I decided to be generous.
    */
    
    if (compressed->data == NULL){
        printf("\nMemory allocation error for compressed output data.\n");
    	crash_c();
    }
	output_area = compressed->data;
}



/******************************** write_bits_c **********************************
	
	writes a variable length series of bits represented by a string of '1's
	and '0's, which it receives as a parameter	
			
*****************************************************************************/
void write_bits_c(char *string_ptr)
{
      /* global switch added by Michael D. Garris 2/26/90 */
      if(comp_write_init_flag){
         bit_place_mark = 0;
         byte_place_mark = 0;
         comp_write_init_flag = False;
      }
      /* EDIT MDG 1/25/99
	while(*string_ptr != NULL) {
      */
	while(*string_ptr != '\0') {
	      if(*string_ptr == '1')
	  	 *(output_area + byte_place_mark) |= write_one[bit_place_mark];
	      else
		 *(output_area + byte_place_mark) &= write_zero[bit_place_mark];
	      if(bit_place_mark == Last_bit_in_a_byte) {
	 	 bit_place_mark = 0;
		 byte_place_mark++;
	      } /* end if byte is full */ 
	      else
		 bit_place_mark++;
	      string_ptr++;
	} /* end while */
	
} 


/******************************** flush_buffer *******************************
	
	writes to memory whatever bits are left in the bit buffer followed by 
	enough zero-bits to pad the compressed image out to a byte boundary.	
			
*****************************************************************************/
unsigned int flush_buffer()
{
SHORT i;

	if (bit_place_mark != 0) {
       	    for (i=bit_place_mark; i<Pixels_per_byte; i++)
		*(output_area + byte_place_mark) &= write_zero[i];
		/*
		 * pad the rest of the last byte with '0' bits.
		 */
		++byte_place_mark;
	}
	return byte_place_mark;
} 


/***************************************************************************/
/* Originally write_run.c                                                  */
/***************************************************************************/

/******************************************************************************
	
	The arrays that follow contain character representations of the binary
	run length codes written during compression.
			
******************************************************************************/
static char *white_terminating_code[64] =
{
	"00110101",
	"000111",
	"0111",
	"1000",
	"1011",
	"1100",
	"1110",
	"1111",
	"10011",
	"10100",
	"00111",
	"01000",
	"001000",
	"000011",
	"110100",
	"110101",
	"101010",
	"101011",
	"0100111",
	"0001100",
	"0001000",
	"0010111",
	"0000011",
	"0000100",
	"0101000",
	"0101011",
	"0010011",
	"0100100",
	"0011000",
	"00000010",
	"00000011",
	"00011010",
	"00011011",
	"00010010",
	"00010011",
	"00010100",
	"00010101",
	"00010110",
	"00010111",
	"00101000",
	"00101001",
	"00101010",
	"00101011",
	"00101100",
	"00101101",
	"00000100",
	"00000101",
	"00001010",
	"00001011",
	"01010010",
	"01010011",
	"01010100",
	"01010101",
	"00100100",
	"00100101",
	"01011000",
	"01011001",
	"01011010",
	"01011011",
	"01001010",
	"01001011",
	"00110010",
	"00110011",
	"00110100",
};/* end array of white terminating code */


static char *black_terminating_code[64] =
{
	"0000110111",
	"010",
	"11",
	"10",
	"011",
	"0011",
	"0010",
	"00011",
	"000101",
	"000100",
	"0000100",
	"0000101",
	"0000111",
	"00000100",
	"00000111",
	"000011000",
	"0000010111",
	"0000011000",
	"0000001000",
	"00001100111",
	"00001101000",
	"00001101100",
	"00000110111",
	"00000101000",
	"00000010111",
	"00000011000",
	"000011001010",
	"000011001011",
	"000011001100",
	"000011001101",
	"000001101000",
	"000001101001",
	"000001101010",
	"000001101011",
	"000011010010",
	"000011010011",
	"000011010100",
	"000011010101",
	"000011010110",
	"000011010111",
	"000001101100",
	"000001101101",
	"000011011010",
	"000011011011",
	"000001010100",
	"000001010101",
	"000001010110",
	"000001010111",
	"000001100100",
	"000001100101",
	"000001010010",
	"000001010011",
	"000000100100",
	"000000110111",
	"000000111000",
	"000000100111",
	"000000101000",
	"000001011000",
	"000001011001",
	"000000101011",
	"000000101100",
	"000001011010",
	"000001100110",
	"000001100111",
}; /* end black_terminating_array */


static char *white_make_up_code[40] =
{
	"11011",
	"10010",
	"010111",
	"0110111",
	"00110110",
	"00110111",
	"01100100",
	"01100101",
	"01101000",
	"01100111",
	"011001100",
	"011001101",
	"011010010",
	"011010011",
	"011010100",
	"011010101",
	"011010110",
	"011010111",
	"011011000",
	"011011001",
	"011011010",
	"011011011",
	"010011000",
	"010011001",
	"010011010",
	"011000",
	"010011011",
	
   /*
    * from this line on, the codes are colorless and represnt runs from
    * 1792 pixels to 2560 pixels.  In other words, the longest run length 
    * codes have been added onto both the white make up codes and the black
    * make up codes.  This has been done to make the procedure 
    * "write_run_length()" easier to write and to understand.  No other 
    * procedure in the compression algorithm is affected by this merging of
    * different types of run length codes, and the compatibility of the
    * program is in no way effected.
    */
    
	"00000001000",
	"00000001100",
	"00000001101",
	"000000010010",
	"000000010011",
	"000000010100",
	"000000010101",
	"000000010110",
	"000000010111",
	"000000011100",
	"000000011101",
	"000000011110",
	"000000011111",
}; /* end case of white makeup code */

		
static char *black_make_up_code[40] =
{
	"0000001111",
	"000011001000",
	"000011001001",
	"000001011011",
	"000000110011",
	"000000110100",
	"000000110101",
	"0000001101100",
	"0000001101101",
	"0000001001010",
	"0000001001011",
	"0000001001100",
	"0000001001101",
	"0000001110010",
	"0000001110011",
	"0000001110100",
	"0000001110101",
	"0000001110110",
	"0000001110111",
	"0000001010010",
	"0000001010011",
	"0000001010100",
	"0000001010101",
	"0000001011010",
	"0000001011011",
	"0000001100100",
	"0000001100101",
	
   /*
    * from this line on, the codes are colorless and represnt runs from
    * 1792 pixels to 2560 pixels.  In other words, the longest run length 
    * codes have been added onto both the white make up codes and the black
    * make up codes.  This has been done to make the procedure 
    * "write_run_length()" easier to write and to understand.  No other 
    * procedure in the compression algorithm is affected by this merging of
    * different types of run length codes, and the compatibility of the
    * program is in no way compromised.
    */
    
	"00000001000",
	"00000001100",
	"00000001101",
	"000000010010",
	"000000010011",
	"000000010100",
	"000000010101",
	"000000010110",
	"000000010111",
	"000000011100",
	"000000011101",
	"000000011110",
	"000000011111",
}; /* end black makeup code */


char *largest_colorless_code =
{
	"000000011111"
};



/****************************** write_run_length() *****************************

	writes the code, or series of codes, that represent a given run length
	of a given color.
				
******************************************************************************/
void write_run_length(SHORT length, SHORT color)
{

SHORT	multiples_of_largest_code,			
		make_up_code_index,	
		remainder,
		i;	
		
	multiples_of_largest_code = length / Largest_code;
	length %= Largest_code;
	for(i=0 ; i < multiples_of_largest_code ; i++) 
	    write_bits_c( largest_colorless_code );

	remainder = length % Size_of_make_up_code_increments;
	
   /* remainder in the range 0 - 63 */
   
	make_up_code_index = length / Size_of_make_up_code_increments;
	
   /*
    * make_up_code_index in the range 0 - 39, and represents a run length 
    * of 64 times its value (i.e. 0 - 2496).  To translate this value into
    * an index into the arrays that store the bit sequence that represents
    * the appropriate run length, 1 must be subtracted from make_up_code_
    * index.  If this results in the value -1, no make up code should be
    * written.
    */
    
	make_up_code_index--;
	
	if(make_up_code_index != Invalid) {
	   if(color == White)
	      write_bits_c(white_make_up_code[make_up_code_index]);
	   else
	      write_bits_c(black_make_up_code[make_up_code_index]);
	}
	
	if(color == White)
	   write_bits_c(white_terminating_code[remainder]);
	else
	   write_bits_c(black_terminating_code[remainder]);

} 	/* end write run length() */


/***************************************************************************/
/* Originally table.c                                                      */
/***************************************************************************/

struct byte_descriptor {
    SHORT pixel[9]; 
};
    
    
static struct byte_descriptor table[Number_of_different_bytes] =
{    
    {{-1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{6,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{5,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{5,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{5,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{5,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{4,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{4,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{4,    5,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{4,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{4,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{4,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{4,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{4,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    4,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{3,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    4,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{3,    4,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{3,    4,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{3,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    5,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{3,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{3,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{2,    3,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    4,    5,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    4,    5,    7,   -1,   -1,   -1,   -1}},
    {{2,    3,    4,    5,    6,    7,   -1,   -1,   -1}},
    {{2,    3,    4,    5,    6,   -1,   -1,   -1,   -1}},
    {{2,    3,    4,    6,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    4,    6,    7,   -1,   -1,   -1,   -1}},
    {{2,    3,    4,    7,   -1,   -1,   -1,   -1,   -1}},
    {{2,    3,    4,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    4,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{2,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    4,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{2,    4,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{2,    4,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{2,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    5,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{2,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{2,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{1,    2,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    4,    5,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    4,    5,    7,   -1,   -1,   -1,   -1}},
    {{1,    2,    4,    5,    6,    7,   -1,   -1,   -1}},
    {{1,    2,    4,    5,    6,   -1,   -1,   -1,   -1}},
    {{1,    2,    4,    6,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    4,    6,    7,   -1,   -1,   -1,   -1}},
    {{1,    2,    4,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    4,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    4,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    4,    7,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    4,    6,    7,   -1,   -1,   -1}},
    {{1,    2,    3,    4,    6,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    4,    5,    6,   -1,   -1,   -1}},
    {{1,    2,    3,    4,    5,    6,    7,   -1,   -1}},
    {{1,    2,    3,    4,    5,    7,   -1,   -1,   -1}},
    {{1,    2,    3,    4,    5,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    5,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    5,    7,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    5,    6,    7,   -1,   -1,   -1}},
    {{1,    2,    3,    5,    6,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    6,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    6,    7,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    2,    3,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{1,    3,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    4,    5,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    4,    5,    7,   -1,   -1,   -1,   -1}},
    {{1,    3,    4,    5,    6,    7,   -1,   -1,   -1}},
    {{1,    3,    4,    5,    6,   -1,   -1,   -1,   -1}},
    {{1,    3,    4,    6,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    4,    6,    7,   -1,   -1,   -1,   -1}},
    {{1,    3,    4,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    3,    4,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    4,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    4,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{1,    4,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{1,    4,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    5,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{1,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    4,    5,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    4,    5,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    4,    5,    6,    7,   -1,   -1,   -1}},
    {{0,    1,    4,    5,    6,   -1,   -1,   -1,   -1}},
    {{0,    1,    4,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    4,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    4,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    4,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    4,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    4,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    4,    6,    7,   -1,   -1,   -1}},
    {{0,    1,    3,    4,    6,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    4,    5,    6,   -1,   -1,   -1}},
    {{0,    1,    3,    4,    5,    6,    7,   -1,   -1}},
    {{0,    1,    3,    4,    5,    7,   -1,   -1,   -1}},
    {{0,    1,    3,    4,    5,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    5,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    5,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    5,    6,    7,   -1,   -1,   -1}},
    {{0,    1,    3,    5,    6,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    3,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    3,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    6,    7,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    6,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    5,    6,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    5,    6,    7,   -1,   -1}},
    {{0,    1,    2,    3,    5,    7,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    5,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    4,    5,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    4,    5,    7,   -1,   -1}},
    {{0,    1,    2,    3,    4,    5,    6,    7,   -1}},
    {{0,    1,    2,    3,    4,    5,    6,   -1,   -1}},
    {{0,    1,    2,    3,    4,    6,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    4,    6,    7,   -1,   -1}},
    {{0,    1,    2,    3,    4,    7,   -1,   -1,   -1}},
    {{0,    1,    2,    3,    4,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    4,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    4,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    4,    6,    7,   -1,   -1,   -1}},
    {{0,    1,    2,    4,    6,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    4,    5,    6,   -1,   -1,   -1}},
    {{0,    1,    2,    4,    5,    6,    7,   -1,   -1}},
    {{0,    1,    2,    4,    5,    7,   -1,   -1,   -1}},
    {{0,    1,    2,    4,    5,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    5,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    5,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    5,    6,    7,   -1,   -1,   -1}},
    {{0,    1,    2,    5,    6,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    1,    2,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    2,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    4,    5,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    4,    5,    7,   -1,   -1,   -1,   -1}},
    {{0,    2,    4,    5,    6,    7,   -1,   -1,   -1}},
    {{0,    2,    4,    5,    6,   -1,   -1,   -1,   -1}},
    {{0,    2,    4,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    4,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    2,    4,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    4,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    4,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    4,    7,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    4,    6,    7,   -1,   -1,   -1}},
    {{0,    2,    3,    4,    6,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    4,    5,    6,   -1,   -1,   -1}},
    {{0,    2,    3,    4,    5,    6,    7,   -1,   -1}},
    {{0,    2,    3,    4,    5,    7,   -1,   -1,   -1}},
    {{0,    2,    3,    4,    5,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    5,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    5,    7,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    5,    6,    7,   -1,   -1,   -1}},
    {{0,    2,    3,    5,    6,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    2,    3,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    3,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    4,    5,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    4,    5,    7,   -1,   -1,   -1,   -1}},
    {{0,    3,    4,    5,    6,    7,   -1,   -1,   -1}},
    {{0,    3,    4,    5,    6,   -1,   -1,   -1,   -1}},
    {{0,    3,    4,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    4,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    3,    4,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    3,    4,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    4,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    4,    5,    6,   -1,   -1,   -1,   -1,   -1}},
    {{0,    4,    5,    6,    7,   -1,   -1,   -1,   -1}},
    {{0,    4,    5,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    5,    6,    7,   -1,   -1,   -1,   -1,   -1}},
    {{0,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1}},
    {{0,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}}
}; /* end of data for list of byte descriptors */


    
/****************************** process_char *********************************

	writes the pixel number of each changing element within the character
	being processed to the list "params->coding_line"
				
******************************************************************************/
void process_char(unsigned char data_byte, struct parameters *params)
{
static char color = 0;
SHORT i = 0;

	color = -(data_byte & Last_bit_mask); 
	data_byte ^= params->previous_color; 
	
/* if the previous color is black - which is contrary to our assumptions -
* the bits in the byte must all be changed so that the result, when used
* as an index into the array 'bytes,' yields the correct result.  In the
* above operation, if the previous color is black (11111111b), all bits
* are changed; if the previous color is white (00000000b), no bits are
* changed. */
	
	while(table[data_byte].pixel[i] != Invalid) 
	      *( params->coding_line + ++params->index ) = 
	      params->pixel + table[data_byte].pixel[i++];
		
	params->pixel += Pixels_per_byte;
	params->previous_color = color;
	
	/* 'color' is a temporary holding place for the value of previous color */ 
}
