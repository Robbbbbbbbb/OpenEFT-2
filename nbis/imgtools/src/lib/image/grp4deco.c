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

      FILE:    GRP4DECO.C
      AUTHORS: CALS Test Network
               
      DATE:    01/25/1990

      Contains routines responsible for CCITT Group 4 encoding
      a binary image pixel datastream.

      ROUTINES:
#cat: grp4decomp - decodes and reconstructs a CCITT Group 4 compressed
#cat:              binary image (bitmap).

***********************************************************************/

/********************************************************************
*  Modified:   Darlene E. Frederick				    *
*              Michael D. Garris                                    *
*  Date:       January 25, 1990				    	    *
*  Updated:    03/14/2005 by MDG                                    *
*  Package:    CCITT4 compression routines			    *
*				   				    *
*  Contents:   ccitt4_decompress()				    *
*	       read_compressed_file_into_memory()		    *
*	       control_decompression()				    *
*	       prepare_to_decompress()				    *
*	       set_up_first_and_last_changing_elements_d()	    *
*	       prepare_to_decompress_next_line()		    *
*	       set_up_first_line_d()				    *
*	       crash_d()					    * 
********************************************************************/

#include <grp4deco.h>
#ifdef TIME
#include <sys/time.h>
#endif

#define NOALLOC 0
#define ALLOC 1
int decomp_alloc_flag;
int decomp_write_init_flag;
int decomp_read_init_flag;
static char 	*all_white,
                *output_area,
		*all_black;
				

#ifdef TIME
struct timeval  t1, t2;
struct timezone tz;
#endif


/********************************************************************
* grp4decomp is the main routine of this file.  It does pre-        *
* liminary setup, calls routines, and does final processing.        *
********************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:         					    *
*		   indata - buffer containing the compressed data.  *
*		   inbytes - the number of bytes in indata.         *
*  		   width - Width in pixels of uncompressed data.    *
*  		   height - Number of lines of uncompressed data.   *
*	Returned:          					    *
*		   outdata - buffer containing the decompressed data*
*		   outbytes - the number of bytes in outdata.       *
********************************************************************/
void grp4decomp(unsigned char *indata, int inbytes, int width, int height,
                unsigned char *outdata, int *outbytes)
{
struct compressed_descriptor 	compressed;
struct decompressed_descriptor  decompressed;

	compressed.pixels_per_line = width;
	compressed.number_of_lines = height;
	compressed.length_in_bytes = inbytes;
        compressed.data = (char *)indata;
        decomp_alloc_flag = NOALLOC;
        decomp_write_init_flag = True;
        decomp_read_init_flag = True;
	read_compressed_file_into_memory(&compressed);
	decompressed.data = (char *)outdata;
	control_decompression( &compressed, &decompressed );
	*outbytes = 
        (decompressed.pixels_per_line >> 3) * decompressed.number_of_lines;
}


/***************************** control_decompression **************************
	
	calls the functions that decompress the compressed file
			
*****************************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:						    *
*		   compressed - structure containing the # of       *
*			        pixels per line, the number of      *
*				lines, and the compressed data.     *
*	Returned:						    *
*		   decompressed - structure containing the # of     *
*			          pixels per line, the number of    *
*				  lines, and the compressed data.   *
********************************************************************/
void control_decompression( struct compressed_descriptor *compressed,
                            struct decompressed_descriptor *decompressed )
{
struct parameters  sole_parameters;
struct parameters *params = &sole_parameters;

#ifdef TIME
  SHORT i;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	gettimeofday(&t1, &tz);
#endif

	prepare_to_decompress( compressed, decompressed, params );
	while(decompress_line( params ) != EOFB )  
	      prepare_to_decompress_next_line( params );
        /* memory deallocation added by Michael D. Garris 2/23/90 */
        free(params->reference_line);
        free(params->coding_line);
        free(all_white);
        free(all_black);
#ifdef TIME
	gettimeofday(&t2, &tz);
	printf("\ntime difference: %ld:%ld\n", t2.tv_sec - t1.tv_sec,
	t2.tv_usec - t1.tv_usec);
	for(i=0; i<5; i++) printf("%c",'\07');
#endif

}


/************************ read_compressed_file_into_memory *********************
	
		allocates memory for the compressed image     		
			
*****************************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:						    *
*		   compressed - structure containing the # of       *
*			        pixels per line, the number of      *
*				lines, and the compressed data.     *
*	Returned:						    *
*		   compressed - structure containing the # of       *
*			          pixels per line, the number of    *
*				  lines, and the compressed data.   *
********************************************************************/
void read_compressed_file_into_memory(struct compressed_descriptor *compressed)
{

     if(decomp_alloc_flag){
 	if((compressed->data = (char *)calloc(compressed->length_in_bytes,
                                              sizeof(char))) == NULL) {
 	    printf("\nCannot allocate enough memory for compressed file.\n");
 	    exit(1);
 	}
     }
     else
        if(compressed->data == NULL){
           printf("\nNo memory allocated for input data!\n");
           exit(1);
        }
} /* end read_compressed_file_into_memory() */

	 		
	
/*************************** prepare_to_decompress ****************************
	
		initializes variables in preperation for decompression		
			
*****************************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:						    *
*		   compressed - structure containing the # of       *
*			        pixels per line, the number of      *
*				lines, and the compressed data.     *
*	Returned:						    *
*		   decompressed - structure containing the # of     *
*			          pixels per line, the number of    *
*				  lines, and the compressed data.   *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
********************************************************************/
void prepare_to_decompress(struct compressed_descriptor *compressed,
                           struct decompressed_descriptor  *decompressed,
                           struct parameters *params)
{
	
	params->max_pixel = compressed->pixels_per_line;
	decompressed->pixels_per_line = compressed->pixels_per_line;
	decompressed->number_of_lines = compressed->number_of_lines;
	
	set_up_first_line_d( params );
	prepare_to_read_bits( compressed->data );
        if(decomp_alloc_flag){
           decompressed->data = (char *)calloc( 
           compressed->pixels_per_line *compressed->number_of_lines /Pixels_per_byte, 
	   sizeof(char) );
        }
        else
           if(decompressed->data == NULL){
              printf("\nNo memory allocated for decompressed data!\n");
              exit(1);
           }
	prepare_to_write_bits_d( decompressed->data, 
	 (compressed->pixels_per_line / Pixels_per_byte) );
				
} /* end decompress() */



/******************************* set_up_first_line_d ***************************
	
	initializes variables in preperation for decompressing the first line		
			
******************************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
*	Returned:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
********************************************************************/
void set_up_first_line_d(struct parameters *params)
{

	params->reference_line = (SHORT *)malloc( (params->max_pixel + 
                                          Extra_positions) * sizeof(SHORT) );
	params->coding_line = (SHORT *)malloc( (params->max_pixel +
                                       Extra_positions) * sizeof(SHORT) );
	
	*(params->reference_line + 0) = Invalid;		
	*(params->reference_line + 1) = params->max_pixel;
	*(params->reference_line + 2) = params->max_pixel;
	*(params->reference_line + 3) = params->max_pixel;
	
	/* initialize first changing element on coding line (a0 = -1) */
	*(params->coding_line) = Invalid;
	
	params->index = 0;

} /* end set_up_first_line_d() */



/******************* set_up_first_and_last_changing_elements_d *****************
	
	initializes the first and last changing elements in the coding line			
			
******************************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
*	Returned:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
********************************************************************/
void set_up_first_and_last_changing_elements_d(struct parameters *params)
{
	*(params->coding_line) = Invalid;
	
	/*
	 * set up the imaginary first changing pixel with an illegal value.
	 */
	 
	*(params->coding_line + ++params->index) = params->max_pixel;
	*(params->coding_line + ++params->index) = params->max_pixel;
	*(params->coding_line + ++params->index) = params->max_pixel;
	
	/* 
	 * set up three changing pixels at the end of the line, all of which
	 * contain the end-of-line flag "max_pixel."  It is necessary to create
	 * three of these flags because the changing elements a0 - b2 are sometimes
	 * advanced by more than one element at a time (and therefore could skip
	 * over a single flag).
	 */
}



/*********************** prepare_to_decompress_next_line *********************
	
	initializes variables in preperation for decompressing another line	
			
******************************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
*	Returned:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
********************************************************************/
void prepare_to_decompress_next_line(struct parameters *params)
{
	
	set_up_first_and_last_changing_elements_d( params );
	swap_the_reference_and_coding_lines( params );
	params->index = 0;

} /* end prepare_to_decompress_next_line() */ 



/******************** swap_the_reference_and_coding_lines ********************
	
	 	      swaps the reference and coding lines	
			
*****************************************************************************/
/********************************************************************
*  Arguments							    *
*  ---------							    *
*	Passed in:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
*	Returned:						    *
*		   params - structure storing information needed    *
*			    for comparison and other tasks.         *
********************************************************************/
void swap_the_reference_and_coding_lines(struct parameters *params)
{
SHORT *temp;
	
	temp = params->reference_line;
	params->reference_line 	= params->coding_line;
	params->coding_line = temp;
		
} /* end swap_the_reference_and_coding_lines() */ 



/*********************************** crash_d ***********************************
	
		forces the program to crash and create a core file
			
******************************************************************************/	void crash_d()
{
FILE *crash_program = NULL;
	fprintf(crash_program,"This will kill the program and create a core file");
}


/***************************************************************************/
/* Originally decomp.c                                                     */
/***************************************************************************/

static struct changing_element {
	SHORT color; 					/* the color of the pixel */
	SHORT pixel; 					/* the position of the pixel on the line */
} a0, a1; 		 		

static SHORT  	b1, b2;				/* an index into reference_line */



/******************************* decompress_line *****************************

					decompress one line of the compressed image
			
******************************************************************************/
SHORT decompress_line(struct parameters *params)
{
SHORT mode;

#if Debug
	static SHORT current_line = 0;
	printf("\n\nLINE: %d\n", current_line);
	current_line++;
#endif

	b1 = 1; /* this puts b1 on the first black element in the reference line ,
			 * which is appropriate  because a0 is white and on -1 */
	a0.pixel = 0;
	a0.color = White; 
	do {
		
#if Debug
		printf("a0:%d, a1:%d, b1:%d", a0.pixel, a1.pixel,
		 *(params->reference_line + b1));
#endif
		mode =  get_mode();
		switch( mode ) {

		case	V0:	
		
		case	VR1:
		
		case	VL1:
		
		case	VR2:
		
		case	VL2:
		
		case	VR3:
		
		case 	VL3:	vertical_mode_d( params, mode );
						break;

		case	H:		horizontal_mode_d( params );
						break;

		case	P:		pass_mode_d( params );
						continue;

		case	EOFB:   return( EOFB );

		default:		crash_d();

		} /* end case of different modes */
		
	} while(a0.pixel < params->max_pixel);
	return( Not_done_yet );
	
} 	/* end decompress_line */



/********************************** get_mode **********************************

					read a mode code from the compressed image
			
******************************************************************************/
SHORT get_mode()
{
SHORT i;

	if (read_bit() == 1)	 
		return(V0);								/* 1 */

	if (read_bit() == 1) {
		if (read_bit() == 1)  
			return(VR1);						/* 011 */
		else
		   	return(VL1);						/* 010 */
	}					
	else {
		if (read_bit() == 1)  
		   	return(H);							/* 001 */
		if (read_bit() == 1)  
		   	return(P);							/* 0001 */
	    if (read_bit() == 1) {
	    	if (read_bit() == 1)
	    		return(VR2);					/* 000011 */
	    	else
	    		return(VL2);					/* 000010 */
		}
		else {
			if (read_bit() == 1) {
		    	if (read_bit() == 1)
		    		return(VR3);				/* 0000011 */
		    	else
	    			return(VL3);				/* 0000010 */
	    	}
			else {
			
				/*
				 * Have read 6 zero's so far.  The only valid code now
				 * possible is EOFB: 00000000000100000000001.
				 */
				
				for(i=0;i<5;i++)
 					if (read_bit() != 0) 
 						crash_d();
 		
 				if (read_bit() != 1) 
 					crash_d();
 			
 				for(i=0;i<11;i++)
 					if (read_bit() != 0) 
 						crash_d();
	
 				if (read_bit() != 1) 
 					crash_d();
		
				return(EOFB);					/* 00000000000100000000001 */
			}
		}
	}
	
} /* end get_mode() */


/********************************** pass_mode_d ********************************

							decompress a pass mode code
			
******************************************************************************/
void pass_mode_d(struct parameters *params)
{
SHORT run_length;
	
	b2 = b1 + 1;
	run_length = *(params->reference_line + b2) - a0.pixel;
	write_bits_d(run_length, a0.color);
	
#if Debug
	printf(" P   Run:%d,  Color:%d\n", run_length, a0.color);
#endif

	a0.pixel += run_length;
	/* a0.color does not change during pass_mode_d() */
	b1 += 2;
} /* end pass_mode_d() */


/******************************* vertical_mode_d *******************************

			decompress a vertical mode code
			
******************************************************************************/
void vertical_mode_d(struct parameters *params, SHORT offset)
{
SHORT run_length;
	
	a1.pixel = *(params->reference_line + b1) + offset;
	run_length = a1.pixel - a0.pixel;
	write_bits_d(run_length, a0.color);
	
#if Debug
	printf(" V%d   Run:%d,  Color:%d\n", offset, run_length, a0.color);
#endif

	a0.pixel = a1.pixel;
	a0.color = ! a0.color;
	*(params->coding_line + ++params->index) = a0.pixel;

	/* 
	 * The color of a0 changes after each vertical mode coding.
	 */
	 
	if ((offset == -1) || (offset == 0)) {
		if ( *(params->reference_line + b1) != params->max_pixel ) 
			b1++;
		return;
	}
	
	if ((offset == 1) || (offset == 2)) {
		b1++;
		if ( (*(params->reference_line + b1) <= a0.pixel)  && 
		 (*(params->reference_line + b1) != params->max_pixel) ) 
					b1 += 2;
		return;
	}
	
	if ((offset == -2) || (offset == -3)) {
		if ( *(params->reference_line + b1 - 1) > a0.pixel ) 
			b1--;
		else { if ( *(params->reference_line + b1) != params->max_pixel ) 
			b1++;
		return;
		}
	}	
				 
	if (offset == 3) {
		b1++;
		while ( (*(params->reference_line + b1) <= a0.pixel)  && 
		 (*(params->reference_line + b1) != params->max_pixel) ) 
			b1 += 2;
		return;
	}
	
} /* end vertical_mode_d() */

	


/******************************* horizontal_mode_d *****************************

			decompress a horizontal mode code
			
******************************************************************************/
void horizontal_mode_d(struct parameters *params)
{
SHORT length, total_length = 0;

	do {
		length = find_run_length_code(a0.color);	
		total_length += length;
	} while (length > Max_terminating_length);

	/*
	 *  Run lengths greater than 63 are followed by terminating codes.
	 *  Thus if "length" is greater than 63, the terminating code must
	 *  also be fetched in order to determine the total run length.
	 *
	 */
	
	write_bits_d(total_length, a0.color);

#if Debug
	printf(" H   Run:%d,  Color:%d\n", total_length, a0.color);
#endif

	a0.pixel += total_length;
	a0.color = !a0.color;
	*(params->coding_line + ++params->index) = a0.pixel;

	/*
	 * a0's color changes after each run color.
	 */

	total_length = 0;
	do {
		length = find_run_length_code(a0.color);	
		total_length += length;
	} while (length > Max_terminating_length);

	write_bits_d(total_length, a0.color);
	
#if Debug
	printf("   Run:%d,  Color:%d\n", total_length, a0.color);
#endif

	a0.pixel += total_length;
	a0.color = !a0.color;
	*(params->coding_line + ++params->index) = a0.pixel;
	
	while (  (*(params->reference_line + b1) <= a0.pixel)  && 
	 ( *(params->reference_line + b1) < params->max_pixel)  )
		{
		b1 += 2;   /* must move ahead by 2 to maintain color difference with */
				   /* a0, whose color does not change in this mode. */
		}
		
} /* end horizontal_mode_d() */


/***************************************************************************/
/* Originally write.c                                                      */
/***************************************************************************/

static char		write_one[Pixels_per_byte] = 
{
	(char)0x80, /* 10000000b: with | operator, it writes a one to bit 0 */
	(char)0x40, /* 01000000b: with | operator, it writes a one to bit 1 */
	(char)0x20, /* 00100000b: with | operator, it writes a one to bit 2 */
	(char)0x10, /* 00010000b: with | operator, it writes a one to bit 3 */
	(char)0x8,  /* 00001000b: with | operator, it writes a one to bit 4 */
	(char)0x4,  /* 00000100b: with | operator, it writes a one to bit 5 */
	(char)0x2,  /* 00000010b: with | operator, it writes a one to bit 6 */
	(char)0x1,  /* 00000001b: with | operator, it writes a one to bit 7 */
};

static char 	write_zero[Pixels_per_byte] =
{
	(char)0x7F, /* 01111111b: with & operator, it writes a zero to bit 0 */
	(char)0xBF, /* 10111111b: with & operator, it writes a zero to bit 1 */	
	(char)0xDF, /* 11011111b: with & operator, it writes a zero to bit 2 */
	(char)0xEF, /* 11101111b: with & operator, it writes a zero to bit 3 */
	(char)0xF7, /* 11110111b: with & operator, it writes a zero to bit 4 */
	(char)0xFB, /* 11111011b: with & operator, it writes a zero to bit 5 */
	(char)0xFD, /* 11111101b: with & operator, it writes a zero to bit 6 */
	(char)0xFE, /* 11111110b: with & operator, it writes a zero to bit 7 */
};



/************************* prepare_to_write_bits_d ******************************
	
	initializes variables in preperation for writing decompressed data
			
******************************************************************************/	
void prepare_to_write_bits_d(char *output_pointer, SHORT bytes_per_line)
{
	output_area = output_pointer;
	
	all_white = (char *)calloc( bytes_per_line, sizeof(char) );
	all_black = (char *)calloc( bytes_per_line, sizeof(char) );
	memset(all_black, Black_byte, bytes_per_line);
	/* set all the pixels in "all_black" to '1' */
}
	
	

/******************************* write_bits_d ***********************************
	
  writes a number of bits of the same color to the memory buffer that holds
  the decompressed image.
  			
******************************************************************************/	
void write_bits_d(unsigned SHORT length, unsigned SHORT color)
{
static unsigned SHORT	 write_on_this_bit = 0;
static unsigned int 	 write_on_this_byte = 0;
unsigned int			 bytes;

        /* global switch added by Michael D. Garris 2/23/90 */
        if(decomp_write_init_flag){
           write_on_this_bit = 0;
           write_on_this_byte = 0;
           decomp_write_init_flag = False;
        }
	if (color == Black) {	
		while( (length>0) && (write_on_this_bit != 0) ) {
			*(output_area + write_on_this_byte) |= write_one[write_on_this_bit];
			length--;
			if (write_on_this_bit == Last_bit_in_a_byte) {
				write_on_this_bit = 0;
				write_on_this_byte++;
			}
			else
				write_on_this_bit++;
		}
	
		memcpy((output_area+write_on_this_byte), all_black, (bytes=(length/8)));
		write_on_this_byte += bytes;
		length %= Bits_per_byte;
	
		while( length>0 ) {
			*(output_area + write_on_this_byte) |= write_one[write_on_this_bit];
			length--;
			if (write_on_this_bit == Last_bit_in_a_byte) {
				write_on_this_bit = 0;
				write_on_this_byte++;
			}
			else
				write_on_this_bit++;
		}
	} 		/* end if color is black */
	else {	
		while( (length>0) && (write_on_this_bit != 0) ) {
			*(output_area + write_on_this_byte) &=write_zero[write_on_this_bit];
			length--;
			if (write_on_this_bit == Last_bit_in_a_byte) {
				write_on_this_bit = 0;
				write_on_this_byte++;
			}
			else
				write_on_this_bit++;
		}
	
		memcpy((output_area+write_on_this_byte), all_white, (bytes=(length/8)));
		write_on_this_byte += bytes;
		length %= Bits_per_byte;
	
		while( length>0 ) {
			*(output_area + write_on_this_byte) &=write_zero[write_on_this_bit];
			length--;
			if (write_on_this_bit == Last_bit_in_a_byte) {
				write_on_this_bit = 0;
				write_on_this_byte++;
			}
			else
				write_on_this_bit++;
		}
	} /* end if color is white */
} /* end write_bits_d() */


/***************************************************************************/
/* Originally read.c                                                       */
/***************************************************************************/

static char 	*input_area;

static char	read_bit_mask[Pixels_per_byte] = 
{
	(char)0x80, /* 10000000b: with & operator, it reads bit 0 */
	(char)0x40, /* 01000000b: with & operator, it reads bit 1 */
	(char)0x20, /* 00100000b: with & operator, it reads bit 2 */
	(char)0x10, /* 00010000b: with & operator, it reads bit 3 */
	(char)0x8,  /* 00001000b: with & operator, it reads bit 4 */
	(char)0x4,  /* 00000100b: with & operator, it reads bit 5 */
	(char)0x2,  /* 00000010b: with & operator, it reads bit 6 */
	(char)0x1,  /* 00000001b: with & operator, it reads bit 7 */
};



/************************* prepare_to_read_bits ******************************

		 initialize a local, static variable, input_area
	 		
******************************************************************************/	
void prepare_to_read_bits(char *input_pointer)
{
	input_area = input_pointer;
}


/******************************* read_bit *************************************

   returns the value of the current bit of the compressed image (e.g. 1 or 0)
	 		
******************************************************************************/
SHORT read_bit()
{
SHORT bit;
static unsigned SHORT read_this_bit  = 0;	
static unsigned int   read_this_byte = 0;
	
        /* global switch added by Michael D. Garris 2/23/90 */
        if(decomp_read_init_flag){
           read_this_bit = 0;
           read_this_byte = 0;
           decomp_read_init_flag = False;
        }
	/*
	 * read_bits gets one bit from the compressed data file,
	 * and returns it as an integer value of 0 or 1.
	 */
	
	bit = *(input_area + read_this_byte) & read_bit_mask[read_this_bit];
	if(read_this_bit == Last_bit_in_a_byte) {
	   read_this_bit = 0;
	   read_this_byte++;
	} /* end if current byte completely read */
	else
	   read_this_bit++;
	
	/*
	 * NOTE: bit contains 0 if the bit read was 0,
	 * or a non-zero number if the bit was 1.
	 */
	 	
	return( !(bit == 0) );
	
	/*
	 * the above operation returns a zero if bit is equal to zero, or a 
	 * one if bit is not equal to zero.
	 */

} /* end read_bit() */


/***************************************************************************/
/* Originally tree.c                                                       */
/***************************************************************************/

struct node {
	SHORT value;
	struct node *child_zero;
	struct node *child_one;
}; /* end node struct */

static struct node *node_ptr;


/*****************************************************************************
	
The following declarations create two ordered, unbalanced, binary trees.
The trees contain run length values, and are ordered by the Huffman codes
that correspond to those values. 
	
******************************************************************************/

struct node black_tree[]  =
	{
	  {-1,    &black_tree[  2],    &black_tree[  1]},
	  {-1,    &black_tree[  4],    &black_tree[  3]},
	  {-1,    &black_tree[  6],    &black_tree[  5]},
	   {2,    NULL,                NULL},            
	   {3,    NULL,                NULL},            
	  {-1,    &black_tree[  8],    &black_tree[  7]},
	  {-1,    &black_tree[ 10],    &black_tree[  9]},
	   {4,    NULL,                NULL},            
	   {1,    NULL,                NULL},            
	  {-1,    &black_tree[ 12],    &black_tree[ 11]},
	  {-1,    &black_tree[ 14],    &black_tree[ 13]},
	   {5,    NULL,                NULL},            
	   {6,    NULL,                NULL},            
	  {-1,    &black_tree[ 16],    &black_tree[ 15]},
	  {-1,    &black_tree[ 18],    &black_tree[ 17]},
	   {7,    NULL,                NULL},            
	  {-1,    &black_tree[ 20],    &black_tree[ 19]},
	  {-1,    &black_tree[ 22],    &black_tree[ 21]},
	  {-1,    &black_tree[ 24],    &black_tree[ 23]},
	   {8,    NULL,                NULL},            
	   {9,    NULL,                NULL},            
	  {-1,    &black_tree[ 26],    &black_tree[ 25]},
	  {-1,    &black_tree[ 28],    &black_tree[ 27]},
	  {-1,    &black_tree[ 30],    &black_tree[ 29]},
	  {-1,    &black_tree[ 32],    &black_tree[ 31]},
	  {12,    NULL,                NULL},            
	  {-1,    &black_tree[ 34],    &black_tree[ 33]},
	  {11,    NULL,                NULL},            
	  {10,    NULL,                NULL},            
	  {-1,    &black_tree[ 36],    &black_tree[ 35]},
	  {-1,    &black_tree[ 38],    &black_tree[ 37]},
	  {-1,    &black_tree[ 40],    &black_tree[ 39]},
	  {-1,    NULL,                &black_tree[ 41]},
	  {-1,    &black_tree[ 43],    &black_tree[ 42]},
	  {-1,    &black_tree[ 45],    &black_tree[ 44]},
	  {14,    NULL,                NULL},            
	  {-1,    &black_tree[ 47],    &black_tree[ 46]},
	  {-1,    &black_tree[ 49],    &black_tree[ 48]},
	  {13,    NULL,                NULL},            
	  {-1,    &black_tree[ 51],    &black_tree[ 50]},
	  {-1,    &black_tree[ 53],    &black_tree[ 52]},
	  {-1,    &black_tree[ 55],    &black_tree[ 54]},
	  {-1,    &black_tree[ 57],    &black_tree[ 56]},
	  {-1,    &black_tree[ 59],    &black_tree[ 58]},
	  {-1,    &black_tree[ 61],    &black_tree[ 60]},
	  {15,    NULL,                NULL},            
	  {-1,    &black_tree[ 63],    &black_tree[ 62]},
	  {-1,    &black_tree[ 65],    &black_tree[ 64]},
	  {-1,    &black_tree[ 67],    &black_tree[ 66]},
	  {-1,    &black_tree[ 69],    &black_tree[ 68]},
	  {-1,    &black_tree[ 71],    &black_tree[ 70]},
	  {-1,    &black_tree[ 73],    &black_tree[ 72]},
	  {-1,    &black_tree[ 75],    &black_tree[ 74]},
	  {-1,    &black_tree[ 77],    &black_tree[ 76]},
	  {-1,    &black_tree[ 79],    &black_tree[ 78]},
	  {-1,    &black_tree[ 81],    &black_tree[ 80]},
	   {0,    NULL,                NULL},            
	  {-1,    &black_tree[ 83],    &black_tree[ 82]},
	  {-1,    &black_tree[ 85],    &black_tree[ 84]},
	  {-1,    &black_tree[ 87],    &black_tree[ 86]},
	  {-1,    &black_tree[ 89],    &black_tree[ 88]},
	  {-1,    &black_tree[ 91],    &black_tree[ 90]},
	  {-1,    &black_tree[ 93],    &black_tree[ 92]},
	  {-1,    &black_tree[ 95],    &black_tree[ 94]},
	  {-1,    &black_tree[ 97],    &black_tree[ 96]},
	  {17,    NULL,                NULL},            
	  {16,    NULL,                NULL},            
	  {-1,    &black_tree[ 99],    &black_tree[ 98]},
	  {-1,    &black_tree[101],    &black_tree[100]},
	  {-1,    &black_tree[103],    &black_tree[102]},
	  {64,    NULL,                NULL},            
	  {-1,    &black_tree[105],    &black_tree[104]},
	  {-1,    &black_tree[107],    &black_tree[106]},
	  {-1,    &black_tree[109],    &black_tree[108]},
	  {-1,    &black_tree[111],    &black_tree[110]},
	  {-1,    &black_tree[113],    &black_tree[112]},
	  {-1,    &black_tree[115],    &black_tree[114]},
	  {18,    NULL,                NULL},            
	  {-1,    &black_tree[117],    &black_tree[116]},
	  {-1,    &black_tree[119],    &black_tree[118]},
	  {-1,    &black_tree[121],    &black_tree[120]},
	  {-1,    &black_tree[123],    &black_tree[122]},
	  {-1,    &black_tree[125],    &black_tree[124]},
	  {21,    NULL,                NULL},            
	  {-1,    &black_tree[127],    &black_tree[126]},
	  {-1,    &black_tree[129],    &black_tree[128]},
	  {-1,    &black_tree[131],    &black_tree[130]},
	  {20,    NULL,                NULL},            
	  {19,    NULL,                NULL},            
	  {-1,    &black_tree[133],    &black_tree[132]},
	  {-1,    &black_tree[135],    &black_tree[134]},
	  {-1,    &black_tree[137],    &black_tree[136]},
	  {22,    NULL,                NULL},            
	  {-1,    &black_tree[139],    &black_tree[138]},
	  {-1,    &black_tree[141],    &black_tree[140]},
	  {-1,    &black_tree[143],    &black_tree[142]},
	  {-1,    &black_tree[145],    &black_tree[144]},
	  {-1,    &black_tree[147],    &black_tree[146]},
	  {-1,    &black_tree[149],    &black_tree[148]},
	  {-1,    &black_tree[151],    &black_tree[150]},
	  {-1,    &black_tree[153],    &black_tree[152]},
	  {-1,    &black_tree[155],    &black_tree[154]},
	  {-1,    &black_tree[157],    &black_tree[156]},
	  {23,    NULL,                NULL},            
	  {-1,    &black_tree[159],    &black_tree[158]},
	  {-1,    &black_tree[161],    &black_tree[160]},
	  {-1,    &black_tree[163],    &black_tree[162]},
	  {-1,    &black_tree[165],    &black_tree[164]},
	  {-1,    &black_tree[167],    &black_tree[166]},
	  {25,    NULL,                NULL},            
	  {24,    NULL,                NULL},            
	  {-1,    &black_tree[169],    &black_tree[168]},
	  {-1,    &black_tree[171],    &black_tree[170]},
	  {-1,    &black_tree[173],    &black_tree[172]},
	  {-1,    &black_tree[175],    &black_tree[174]},
	  {-1,    &black_tree[177],    &black_tree[176]},
	  {-1,    &black_tree[179],    &black_tree[178]},
	  {-1,    &black_tree[181],    &black_tree[180]},
	{1920,    NULL,                NULL},            
	{1856,    NULL,                NULL},            
	  {-1,    &black_tree[183],    &black_tree[182]},
	  {-1,    &black_tree[185],    &black_tree[184]},
	  {-1,    &black_tree[187],    &black_tree[186]},
	{1792,    NULL,                NULL},            
	  {43,    NULL,                NULL},            
	  {42,    NULL,                NULL},            
	  {39,    NULL,                NULL},            
	  {38,    NULL,                NULL},            
	  {37,    NULL,                NULL},            
	  {36,    NULL,                NULL},            
	  {35,    NULL,                NULL},            
	  {34,    NULL,                NULL},            
	  {29,    NULL,                NULL},            
	  {28,    NULL,                NULL},            
	  {27,    NULL,                NULL},            
	  {26,    NULL,                NULL},            
	 {192,    NULL,                NULL},            
	 {128,    NULL,                NULL},            
	  {41,    NULL,                NULL},            
	  {40,    NULL,                NULL},            
	  {33,    NULL,                NULL},            
	  {32,    NULL,                NULL},            
	  {31,    NULL,                NULL},            
	  {30,    NULL,                NULL},            
	  {63,    NULL,                NULL},            
	  {62,    NULL,                NULL},            
	  {49,    NULL,                NULL},            
	  {48,    NULL,                NULL},            
	 {256,    NULL,                NULL},            
	  {61,    NULL,                NULL},            
	  {58,    NULL,                NULL},            
	  {57,    NULL,                NULL},            
	  {47,    NULL,                NULL},            
	  {46,    NULL,                NULL},            
	  {45,    NULL,                NULL},            
	  {44,    NULL,                NULL},            
	  {51,    NULL,                NULL},            
	  {50,    NULL,                NULL},            
	  {-1,    &black_tree[189],    &black_tree[188]},
	  {-1,    &black_tree[191],    &black_tree[190]},
	  {-1,    &black_tree[193],    &black_tree[192]},
	  {54,    NULL,                NULL},            
	  {53,    NULL,                NULL},            
	  {-1,    &black_tree[195],    &black_tree[194]},
	 {448,    NULL,                NULL},            
	 {384,    NULL,                NULL},            
	 {320,    NULL,                NULL},            
	  {-1,    &black_tree[197],    &black_tree[196]},
	  {-1,    &black_tree[199],    &black_tree[198]},
	  {60,    NULL,                NULL},            
	  {59,    NULL,                NULL},            
	  {-1,    &black_tree[201],    &black_tree[200]},
	  {-1,    &black_tree[203],    &black_tree[202]},
	  {56,    NULL,                NULL},            
	  {55,    NULL,                NULL},            
	  {-1,    &black_tree[205],    &black_tree[204]},
	  {-1,    &black_tree[207],    &black_tree[206]},
	  {52,    NULL,                NULL},            
	{2560,    NULL,                NULL},            
	{2496,    NULL,                NULL},            
	{2432,    NULL,                NULL},            
	{2368,    NULL,                NULL},            
	{2304,    NULL,                NULL},            
	{2240,    NULL,                NULL},            
	{2176,    NULL,                NULL},            
	{2112,    NULL,                NULL},            
	{2048,    NULL,                NULL},            
	{1984,    NULL,                NULL},            
	{1216,    NULL,                NULL},            
	{1152,    NULL,                NULL},            
	{1088,    NULL,                NULL},            
	{1024,    NULL,                NULL},            
	 {960,    NULL,                NULL},            
	 {896,    NULL,                NULL},            
	 {576,    NULL,                NULL},            
	 {512,    NULL,                NULL},            
	{1728,    NULL,                NULL},            
	{1664,    NULL,                NULL},            
	{1600,    NULL,                NULL},            
	{1536,    NULL,                NULL},            
	{1472,    NULL,                NULL},            
	{1408,    NULL,                NULL},            
	{1344,    NULL,                NULL},            
	{1280,    NULL,                NULL},            
	 {832,    NULL,                NULL},            
	 {768,    NULL,                NULL},            
	 {704,    NULL,                NULL},            
	 {640,    NULL,                NULL}
}; /* end black_tree */
	
	 
	 
struct node white_tree[] =
	{	            
	  {-1,    &white_tree[  2],    &white_tree[  1]},
	  {-1,    &white_tree[  4],    &white_tree[  3]},
	  {-1,    &white_tree[  6],    &white_tree[  5]},
	  {-1,    &white_tree[  8],    &white_tree[  7]},
	  {-1,    &white_tree[ 10],    &white_tree[  9]},
	  {-1,    &white_tree[ 12],    &white_tree[ 11]},
	  {-1,    &white_tree[ 14],    &white_tree[ 13]},
	  {-1,    &white_tree[ 16],    &white_tree[ 15]},
	  {-1,    &white_tree[ 18],    &white_tree[ 17]},
	  {-1,    &white_tree[ 20],    &white_tree[ 19]},
	  {-1,    &white_tree[ 22],    &white_tree[ 21]},
	  {-1,    &white_tree[ 24],    &white_tree[ 23]},
	  {-1,    &white_tree[ 26],    &white_tree[ 25]},
	  {-1,    &white_tree[ 28],    &white_tree[ 27]},
	  {-1,    &white_tree[ 30],    &white_tree[ 29]},
	   {7,    NULL,                NULL},            
	   {6,    NULL,                NULL},            
	  {-1,    &white_tree[ 32],    &white_tree[ 31]},
	   {5,    NULL,                NULL},            
	   {4,    NULL,                NULL},            
	  {-1,    &white_tree[ 34],    &white_tree[ 33]},
	  {-1,    &white_tree[ 36],    &white_tree[ 35]},
	   {3,    NULL,                NULL},            
	   {2,    NULL,                NULL},            
	  {-1,    &white_tree[ 38],    &white_tree[ 37]},
	  {-1,    &white_tree[ 40],    &white_tree[ 39]},
	  {-1,    &white_tree[ 42],    &white_tree[ 41]},
	  {-1,    &white_tree[ 44],    &white_tree[ 43]},
	  {-1,    &white_tree[ 46],    &white_tree[ 45]},
	  {-1,    &white_tree[ 48],    &white_tree[ 47]},
	  {-1,    &white_tree[ 50],    &white_tree[ 49]},
	  {64,    NULL,                NULL},            
	  {-1,    &white_tree[ 52],    &white_tree[ 51]},
	  {-1,    &white_tree[ 54],    &white_tree[ 53]},
	   {9,    NULL,                NULL},            
	   {8,    NULL,                NULL},            
	 {128,    NULL,                NULL},            
	  {-1,    &white_tree[ 56],    &white_tree[ 55]},
	  {-1,    &white_tree[ 58],    &white_tree[ 57]},
	  {-1,    &white_tree[ 60],    &white_tree[ 59]},
	  {-1,    &white_tree[ 62],    &white_tree[ 61]},
	  {-1,    &white_tree[ 64],    &white_tree[ 63]},
	  {11,    NULL,                NULL},            
	  {10,    NULL,                NULL},            
	  {-1,    &white_tree[ 66],    &white_tree[ 65]},
	  {-1,    &white_tree[ 68],    &white_tree[ 67]},
	  {-1,    &white_tree[ 70],    &white_tree[ 69]},
	  {-1,    &white_tree[ 72],    &white_tree[ 71]},
	  {-1,    &white_tree[ 74],    &white_tree[ 73]},
	  {-1,    &white_tree[ 76],    &white_tree[ 75]},
	  {-1,    &white_tree[ 78],    &white_tree[ 77]},
	  {15,    NULL,                NULL},            
	  {14,    NULL,                NULL},            
	  {17,    NULL,                NULL},            
	  {16,    NULL,                NULL},            
	  {-1,    &white_tree[ 80],    &white_tree[ 79]},
	  {-1,    &white_tree[ 82],    &white_tree[ 81]},
	  {-1,    &white_tree[ 84],    &white_tree[ 83]},
	{1664,    NULL,                NULL},            
	 {192,    NULL,                NULL},            
	  {-1,    &white_tree[ 86],    &white_tree[ 85]},
	  {-1,    &white_tree[ 88],    &white_tree[ 87]},
	  {-1,    &white_tree[ 90],    &white_tree[ 89]},
	  {-1,    &white_tree[ 92],    &white_tree[ 91]},
	  {-1,    &white_tree[ 94],    &white_tree[ 93]},
	  {-1,    &white_tree[ 96],    &white_tree[ 95]},
	  {-1,    &white_tree[ 98],    &white_tree[ 97]},
	  {-1,    &white_tree[100],    &white_tree[ 99]},
	  {-1,    &white_tree[102],    &white_tree[101]},
	  {-1,    &white_tree[104],    &white_tree[103]},
	  {12,    NULL,                NULL},            
	   {1,    NULL,                NULL},            
	  {-1,    &white_tree[106],    &white_tree[105]},
	  {-1,    &white_tree[108],    &white_tree[107]},
	  {-1,    &white_tree[110],    &white_tree[109]},
	  {13,    NULL,                NULL},            
	  {-1,    &white_tree[112],    &white_tree[111]},
	  {-1,    &white_tree[114],    &white_tree[113]},
	  {-1,    &white_tree[116],    &white_tree[115]},
	 {256,    NULL,                NULL},            
	  {-1,    &white_tree[118],    &white_tree[117]},
	  {-1,    &white_tree[120],    &white_tree[119]},
	  {-1,    &white_tree[122],    &white_tree[121]},
	  {-1,    &white_tree[124],    &white_tree[123]},
	  {-1,    &white_tree[126],    &white_tree[125]},
	  {-1,    &white_tree[128],    &white_tree[127]},
	  {-1,    &white_tree[130],    &white_tree[129]},
	  {25,    NULL,                NULL},            
	  {-1,    &white_tree[132],    &white_tree[131]},
	  {-1,    &white_tree[134],    &white_tree[133]},
	  {24,    NULL,                NULL},            
	  {18,    NULL,                NULL},            
	  {-1,    &white_tree[136],    &white_tree[135]},
	  {-1,    &white_tree[138],    &white_tree[137]},
	  {27,    NULL,                NULL},            
	  {-1,    &white_tree[140],    &white_tree[139]},
	  {-1,    &white_tree[142],    &white_tree[141]},
	  {-1,    &white_tree[144],    &white_tree[143]},
	  {28,    NULL,                NULL},            
	  {21,    NULL,                NULL},            
	  {-1,    &white_tree[146],    &white_tree[145]},
	  {-1,    &white_tree[148],    &white_tree[147]},
	  {-1,    &white_tree[150],    &white_tree[149]},
	  {26,    NULL,                NULL},            
	  {-1,    &white_tree[152],    &white_tree[151]},
	  {-1,    &white_tree[154],    &white_tree[153]},
	  {19,    NULL,                NULL},            
	  {-1,    &white_tree[156],    &white_tree[155]},
	  {-1,    &white_tree[158],    &white_tree[157]},
	  {-1,    &white_tree[160],    &white_tree[159]},
	  {20,    NULL,                NULL},            
	  {-1,    &white_tree[162],    &white_tree[161]},
	  {23,    NULL,                NULL},            
	  {22,    NULL,                NULL},            
	  {-1,    &white_tree[164],    &white_tree[163]},
	  {-1,    &white_tree[166],    &white_tree[165]},
	  {-1,    NULL,                &white_tree[167]},
	  {-1,    &white_tree[169],    &white_tree[168]},
	  {-1,    &white_tree[171],    &white_tree[170]},
	  {-1,    &white_tree[173],    &white_tree[172]},
	  {-1,    &white_tree[175],    &white_tree[174]},
	  {-1,    &white_tree[177],    &white_tree[176]},
	 {576,    NULL,                NULL},            
	 {640,    NULL,                NULL},            
	  {-1,    &white_tree[179],    &white_tree[178]},
	 {512,    NULL,                NULL},            
	 {448,    NULL,                NULL},            
	  {58,    NULL,                NULL},            
	  {57,    NULL,                NULL},            
	  {56,    NULL,                NULL},            
	  {55,    NULL,                NULL},            
	  {52,    NULL,                NULL},            
	  {51,    NULL,                NULL},            
	  {50,    NULL,                NULL},            
	  {49,    NULL,                NULL},            
	  {-1,    &white_tree[181],    &white_tree[180]},
	  {-1,    &white_tree[183],    &white_tree[182]},
	  {60,    NULL,                NULL},            
	  {59,    NULL,                NULL},            
	 {384,    NULL,                NULL},            
	 {320,    NULL,                NULL},            
	   {0,    NULL,                NULL},            
	  {63,    NULL,                NULL},            
	  {62,    NULL,                NULL},            
	  {61,    NULL,                NULL},            
	  {44,    NULL,                NULL},            
	  {43,    NULL,                NULL},            
	  {42,    NULL,                NULL},            
	  {41,    NULL,                NULL},            
	  {40,    NULL,                NULL},            
	  {39,    NULL,                NULL},            
	  {54,    NULL,                NULL},            
	  {53,    NULL,                NULL},            
	  {32,    NULL,                NULL},            
	  {31,    NULL,                NULL},            
	  {38,    NULL,                NULL},            
	  {37,    NULL,                NULL},            
	  {36,    NULL,                NULL},            
	  {35,    NULL,                NULL},            
	  {34,    NULL,                NULL},            
	  {33,    NULL,                NULL},            
	  {48,    NULL,                NULL},            
	  {47,    NULL,                NULL},            
	  {46,    NULL,                NULL},            
	  {45,    NULL,                NULL},            
	  {30,    NULL,                NULL},            
	  {29,    NULL,                NULL},            
	  {-1,    &white_tree[185],    &white_tree[184]},
	{1408,    NULL,                NULL},            
	{1344,    NULL,                NULL},            
	{1280,    NULL,                NULL},            
	{1216,    NULL,                NULL},            
	{1152,    NULL,                NULL},            
	{1088,    NULL,                NULL},            
	{1024,    NULL,                NULL},            
	 {960,    NULL,                NULL},            
	 {896,    NULL,                NULL},            
	 {832,    NULL,                NULL},            
	 {768,    NULL,                NULL},            
	 {704,    NULL,                NULL},            
	{1728,    NULL,                NULL},            
	{1600,    NULL,                NULL},            
	{1536,    NULL,                NULL},            
	{1472,    NULL,                NULL},            
	  {-1,    &white_tree[187],    &white_tree[186]},
	  {-1,    &white_tree[189],    &white_tree[188]},
	  {-1,    &white_tree[191],    &white_tree[190]},
	  {-1,    &white_tree[193],    &white_tree[192]},
	  {-1,    &white_tree[195],    &white_tree[194]},
	  {-1,    &white_tree[197],    &white_tree[196]},
	  {-1,    &white_tree[199],    &white_tree[198]},
	  {-1,    &white_tree[201],    &white_tree[200]},
	{1920,    NULL,                NULL},            
	{1856,    NULL,                NULL},            
	  {-1,    &white_tree[203],    &white_tree[202]},
	  {-1,    &white_tree[205],    &white_tree[204]},
	  {-1,    &white_tree[207],    &white_tree[206]},
	{1792,    NULL,                NULL},            
	{2560,    NULL,                NULL},            
	{2496,    NULL,                NULL},            
	{2432,    NULL,                NULL},            
	{2368,    NULL,                NULL},            
	{2304,    NULL,                NULL},            
	{2240,    NULL,                NULL},            
	{2176,    NULL,                NULL},            
	{2112,    NULL,                NULL},            
	{2048,    NULL,                NULL},            
	{1984,    NULL,                NULL}
}; /* end white_tree */


/************************* find_run_length_code ******************************

		finds the length of the run in the compressed image
		by traversing the above declared tree of run length codes.
	 		
******************************************************************************/	
SHORT find_run_length_code(SHORT color)
{
	if(color == White)
	   node_ptr = white_tree; /* point to root node */
	else
	    node_ptr = black_tree; /* point to root node */

	while(node_ptr->value == Invalid) {  /* -1 */		
	      if((read_bit()) == 0)
	  	  node_ptr = node_ptr->child_zero;
	      else
	 	   node_ptr = node_ptr->child_one;
	} /* end while node does not contain a run length value */
	
	/*
	 *  When this line is reached, node_ptr points to a node that contains
	 *  the run length code: return that value.
	 */
	 
	return(node_ptr->value);

} /* end find_run_length_code */
