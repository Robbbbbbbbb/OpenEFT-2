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
      LIBRARY: FING - NIST Fingerprint Systems Utilities

      FILE:           BZ_IO.C
      ALGORITHM:      Allan S. Bozorth (FBI)
      MODIFICATIONS:  Michael D. Garris (NIST)
                      Stan Janet (NIST)
      DATE:           09/21/2004
      UPDATED:        01/11/2012 by Kenneth Ko
      UPDATED:        03/08/2012 by Kenneth Ko
      UPDATED:        07/10/2014 by Kenneth Ko

      Contains routines responsible for supporting command line
      processing, file and data input to, and output from the
      Bozorth3 fingerprint matching algorithm.

***********************************************************************

      ROUTINES:
#cat: parse_line_range - parses strings of the form #-# into the upper
#cat:            and lower bounds of a range corresponding to lines in
#cat:            an input file list
#cat: set_progname - stores the program name for the current invocation
#cat: set_probe_filename - stores the name of the current probe file
#cat:            being processed
#cat: set_gallery_filename - stores the name of the current gallery file
#cat:            being processed
#cat: get_progname - retrieves the program name for the current invocation
#cat: get_probe_filename - retrieves the name of the current probe file
#cat:            being processed
#cat: get_gallery_filename - retrieves the name of the current gallery
#cat:            file being processed
#cat: get_next_file - gets the next probe (or gallery) filename to be
#cat:            processed, either from the command line or from a
#cat:            file list
#cat: get_score_filename - returns the filename to which the output line
#cat:            should be written
#cat: get_score_line - formats output lines based on command line options
#cat:            specified
#cat: bz_load -  loads the contents of the specified XYT file into
#cat:            structured memory
#cat: fd_readable - when multiple bozorth processes are being run
#cat:            concurrently and one of the processes determines a
#cat:            has been found, the other processes poll a file
#cat:            descriptor using this function to see if they
#cat:            should exit as well

***********************************************************************/

#include <usebsd.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <bozorth.h>

/***********************************************************************/
int parse_line_range( const char * sb, int * begin, int * end )
{
int ib, ie;
char * se;


if ( ! isdigit(*sb) )
   return -1;
ib = atoi( sb );

se = strchr( sb, '-' );
if ( se != (char *) NULL ) {
   se++;
   if ( ! isdigit(*se) )
      return -2;
   ie = atoi( se );
} else {
   ie = ib;
}

if ( ib <= 0 ) {
   if ( ie <= 0 ) {
      return -3;
   } else {
      return -4;
   }
}

if ( ie <= 0 ) {
   return -5;
}

if ( ib > ie )
   return -6;

*begin = ib;
*end   = ie;

return 0;
}

/***********************************************************************/

/* Used by the following set* and get* routines */
static char program_buffer[ 1024 ];
static char * pfile;
static char * gfile;

/***********************************************************************/
void set_progname( int use_pid, char * basename, pid_t pid )
{
if ( use_pid )
   sprintf( program_buffer, "%s pid %ld", basename, (long) pid );
else
   sprintf( program_buffer, "%s", basename );
}

/***********************************************************************/
void set_probe_filename( char * filename )
{
pfile = filename;
}

/***********************************************************************/
void set_gallery_filename( char * filename )
{
gfile = filename;
}

/***********************************************************************/
char * get_progname( void )
{
return program_buffer;
}

/***********************************************************************/
char * get_probe_filename( void )
{
return pfile;
}

/***********************************************************************/
char * get_gallery_filename( void )
{
return gfile;
}

/***********************************************************************/
char * get_next_file(
      char * fixed_file,
      FILE * list_fp,
      FILE * mates_fp,
      int * done_now,
      int * done_afterwards,
      char * line,
      int argc,
      char ** argv,
      int * optind,

      int * lineno,
      int begin,
      int end
      )
{
char * p;
FILE * fp;



if ( fixed_file != (char *) NULL ) {
   if ( verbose_main )
      fprintf( errorfp, "returning fixed filename: %s\n", fixed_file );
   return fixed_file;
}


fp = list_fp;
if ( fp == (FILE *) NULL )
   fp = mates_fp;
if ( fp != (FILE *) NULL ) {
   while (1) {
      if ( fgets( line, MAX_LINE_LENGTH, fp ) == (char *) NULL ) {
         *done_now = 1;
         if ( verbose_main )
            fprintf( errorfp, "returning NULL -- reached EOF\n" );
         return (char *) NULL;
      }
      ++*lineno;

      if ( begin <= 0 )         /* no line number range was specified */
         break;
      if ( *lineno > end ) {
         *done_now = 1;
         if ( verbose_main )
            fprintf( errorfp, "returning NULL -- current line (%d) > end line (%d)\n",
                              *lineno, end );
         return (char *) NULL;
      }
      if ( *lineno >= begin ) {
         break;
      }
      /* Otherwise ( *lineno < begin ) so read another line */
   }

   p = strchr( line, '\n' );
   if ( p == (char *) NULL ) {
      *done_now = 1;
      if ( verbose_main )
         fprintf( errorfp, "returning NULL -- missing newline character\n" );
      return (char *) NULL;
   }
   *p = '\0';

   p = line;
   if ( verbose_main )
      fprintf( errorfp, "returning filename from next line: %s\n", p );
   return p;
}


p = argv[*optind];
++*optind;
if ( *optind >= argc )
   *done_afterwards = 1;
if ( verbose_main )
   fprintf( errorfp, "returning next argv: %s [done_afterwards=%d]\n", p, *done_afterwards );
return p;
}

/***********************************************************************/
/* returns CNULL on error */
char * get_score_filename( const char * outdir, const char * listfile )
{
const char * basename;
int baselen;
int dirlen;
int extlen;
char * outfile;

/* These are now exteranlly defined in bozorth.h */
/* extern FILE * errorfp; */
/* extern char * get_progname( void ); */



basename = strrchr( listfile, '/' );
if ( basename == CNULL ) {
   basename = listfile;
} else {
   ++basename;
}
baselen = strlen( basename );
if ( baselen == 0 ) {
   fprintf( errorfp, "%s: ERROR: couldn't find basename of %s\n", get_progname(), listfile );
   return(CNULL);
}
dirlen = strlen( outdir );
if ( dirlen == 0 ) {
   fprintf( errorfp, "%s: ERROR: illegal output directory %s\n", get_progname(), outdir );
   return(CNULL);
}

extlen = strlen( SCOREFILE_EXTENSION );
outfile = malloc_or_return_error( dirlen + baselen + extlen + 2, "output filename" );
if ( outfile == CNULL)
   return(CNULL);

sprintf( outfile, "%s/%s%s", outdir, basename, SCOREFILE_EXTENSION );

return outfile;
}

/***********************************************************************/
char * get_score_line(
      const char * probe_file,
      const char * gallery_file,
      int n,
      int static_flag,
      const char * fmt
      )
{
int nchars;
char * bufptr;
static char linebuf[1024];

nchars = 0;
bufptr = &linebuf[0];
while ( *fmt ) {
   if ( nchars++ > 0 )
      *bufptr++ = ' ';
   switch ( *fmt++ ) {
      case 's':
         sprintf( bufptr, "%d", n );
         break;
      case 'p':
         sprintf( bufptr, "%s", probe_file );
         break;
      case 'g':
         sprintf( bufptr, "%s", gallery_file );
         break;
      default:
         return (char *) NULL;
   }
   bufptr = strchr( bufptr, '\0' );
}
*bufptr++ = '\n';
*bufptr   = '\0';

if (static_flag) {
   return &linebuf[0];
} else {
   size_t len = strlen(linebuf) + 1;
   char *buf = malloc(len);	/* Caller must free() */
   if (buf == NULL)
   	return buf;
   strncpy(buf, linebuf, len);
   return buf;
}
}

/************************************************************************
Load a 3-4 column (X,Y,T[,Q]) set of minutiae from the specified file
and return a XYT sturcture.
Row 3's value is an angle which is normalized to the interval (-180,180].
A maximum of MAX_BOZORTH_MINUTIAE minutiae can be returned -- fewer if
"max_minutiae" is smaller.  If the file contains more minutiae than are
to be returned, the highest-quality minutiae are returned.
*************************************************************************/

/***********************************************************************/
struct xyt_struct * bz_load( const char * xyt_file )
{
   int nminutiae;
   int m;
   int i;
   int nargs_expected;
   FILE * fp;
   struct xyt_struct * xyt_s;
   struct xytq_struct * xytq_s;
   int xvals_lng[MAX_FILE_MINUTIAE],   /* Temporary lists to store all the minutaie from a file */
       yvals_lng[MAX_FILE_MINUTIAE],
       tvals_lng[MAX_FILE_MINUTIAE],
       qvals_lng[MAX_FILE_MINUTIAE];
   char xyt_line[ MAX_LINE_LENGTH ];

   /* This is now externally defined in bozorth.h */
   /* extern FILE * errorfp; */

   fp = fopen( xyt_file, "r" );
   if ( fp == (FILE *) NULL ) 
   {
      fprintf( errorfp, "%s: ERROR: fopen() of minutiae file \"%s\" failed: %s\n",
                get_progname(), xyt_file, strerror(errno) );
      return XYT_NULL;
   }

   nminutiae = 0;
   nargs_expected = 0;

   while ( fgets( xyt_line, sizeof xyt_line, fp ) != CNULL ) 
   {
      m = sscanf( xyt_line, "%d %d %d %d",
                   &xvals_lng[nminutiae],
                   &yvals_lng[nminutiae],
                   &tvals_lng[nminutiae],
                   &qvals_lng[nminutiae] );

      if ( nminutiae == 0 ) 
      {
         if ( m != 3 && m != 4 ) 
         {
            fprintf( errorfp, "%s: ERROR: sscanf() failed on line %u in minutiae file \"%s\"\n",
                     get_progname(), nminutiae+1, xyt_file );
            return XYT_NULL;
         }
         nargs_expected = m;
      } 
      else 
      {
         if ( m != nargs_expected ) 
         {
            fprintf( errorfp, "%s: ERROR: inconsistent argument count on line %u of minutiae file \"%s\"\n",
                     get_progname(), nminutiae+1, xyt_file );
            return XYT_NULL;
         }
      }
      if ( m == 3 )
         qvals_lng[nminutiae] = 1;

      ++nminutiae;
      if ( nminutiae == MAX_FILE_MINUTIAE )
         break;
   }

   if ( fclose(fp) != 0 ) 
   {
      fprintf( errorfp, "%s: ERROR: fclose() of minutiae file \"%s\" failed: %s\n",
                     get_progname(), xyt_file, strerror(errno) );
      return XYT_NULL;
   }
   
   xytq_s = (struct xytq_struct *)malloc(sizeof(struct xytq_struct));
   if ( xytq_s == XYTQ_NULL )
   {
      fprintf( errorfp, "%s: ERROR: malloc() failure while loading minutiae buffer failed: %s\n",
                                                     get_progname(),
                                                     strerror(errno)
                                                     );
      return XYT_NULL;
   }

   xytq_s->nrows = nminutiae;
   for (i=0; i<nminutiae; i++)
   {
      xytq_s->xcol[i] = xvals_lng[i];
      xytq_s->ycol[i] = yvals_lng[i];
      xytq_s->thetacol[i] = tvals_lng[i];
      xytq_s->qualitycol[i] = qvals_lng[i];
   }

   xyt_s = bz_prune(xytq_s, 0);
   
   if ( verbose_load )
      fprintf( errorfp, "Loaded %s\n", xyt_file );

   return xyt_s;
} 

/************************************************************************
Load a XYTQ structure and return a XYT struct. 
Row 3's value is an angle which is normalized to the interval (-180,180].
A maximum of MAX_BOZORTH_MINUTIAE minutiae can be returned -- fewer if
"max_minutiae" is smaller.  If the file contains more minutiae than are
to be returned, the highest-quality minutiae are returned.
*************************************************************************/
struct xyt_struct * bz_prune(struct xytq_struct *xytq_s, int verbose_load)
{

   int nminutiae;
   int index;
   int j;
   int m;
   struct xyt_struct * xyt_s;
   int * xptr;
   int * yptr;
   int * tptr;
   int * qptr;
   struct minutiae_struct c[MAX_FILE_MINUTIAE];
   int xvals_lng[MAX_FILE_MINUTIAE],
       yvals_lng[MAX_FILE_MINUTIAE],
       tvals_lng[MAX_FILE_MINUTIAE],
       qvals_lng[MAX_FILE_MINUTIAE];
   int order[MAX_FILE_MINUTIAE];       
   int xvals[MAX_BOZORTH_MINUTIAE],
       yvals[MAX_BOZORTH_MINUTIAE],
       tvals[MAX_BOZORTH_MINUTIAE],
       qvals[MAX_BOZORTH_MINUTIAE];
   char xyt_line[ MAX_LINE_LENGTH ];
   
   #define C1 0
   #define C2 1

   int i;
   nminutiae = xytq_s->nrows;  
   for (i=0; i<nminutiae; i++)
   {
      xvals_lng[i] = xytq_s->xcol[i];
      yvals_lng[i] = xytq_s->ycol[i];

      if ( xytq_s->thetacol[i] > 180 )
         tvals_lng[i] = xytq_s->thetacol[i] - 360;
      else
         tvals_lng[i] = xytq_s->thetacol[i];

      qvals_lng[i] = xytq_s->qualitycol[i];
   }

   if ( nminutiae > max_minutiae ) 
   {
      if ( verbose_load )
         fprintf( errorfp, "%s: WARNING: bz_prune(): trimming minutiae to the %d of highest quality\n",
                     get_progname(), max_minutiae );

      if ( verbose_load )
         fprintf( errorfp, "Before quality sort:\n" );
      if ( sort_order_decreasing( qvals_lng, nminutiae, order )) 
      {
         fprintf( errorfp, "%s: ERROR: sort failed and returned on error\n", get_progname());
         return XYT_NULL;
      }

      for ( j = 0; j < nminutiae; j++ ) 
      {
         if ( verbose_load )
            fprintf( errorfp, "   %3d: %3d %3d %3d ---> order = %3d\n",
                     j, xvals_lng[j], yvals_lng[j], qvals_lng[j], order[j] );

         if ( j == 0 )
            continue;
         if ( qvals_lng[order[j]] > qvals_lng[order[j-1]] ) {
            fprintf( errorfp, "%s: ERROR: sort failed: j=%d; qvals_lng[%d] > qvals_lng[%d]\n",
                     get_progname(), j, order[j], order[j-1] );
            return XYT_NULL;
         }
      }


      if ( verbose_load )
         fprintf( errorfp, "\nAfter quality sort:\n" );
      for ( j = 0; j < max_minutiae; j++ ) 
      {
         xvals[j] = xvals_lng[order[j]];
         yvals[j] = yvals_lng[order[j]];
         tvals[j] = tvals_lng[order[j]];
         qvals[j] = qvals_lng[order[j]];
         if ( verbose_load )
            fprintf( errorfp, "   %3d: %3d %3d %3d\n", j, xvals[j], yvals[j], qvals[j] );
      }


      if ( C1 ) 
      {
         if ( verbose_load )
            fprintf( errorfp, "\nAfter qsort():\n" );
         qsort( (void *) &c, (size_t) nminutiae, sizeof(struct minutiae_struct), sort_quality_decreasing );
         for ( j = 0; j < nminutiae; j++ ) 
         {
            if ( verbose_load )
               fprintf( errorfp, "Q  %3d: %3d %3d %3d\n",
                     j, c[j].col[0], c[j].col[1], c[j].col[3] );

            if ( j > 0 && c[j].col[3] > c[j-1].col[3] ) 
            {
               fprintf( errorfp, "%s: ERROR: sort failed: c[%d].col[3] > c[%d].col[3]\n",
                     get_progname(), j, j-1 );
               return XYT_NULL;
            }
         }
      }

      if ( verbose_load )
         fprintf( errorfp, "\n" );

      xptr = xvals;
      yptr = yvals;
      tptr = tvals;
      qptr = qvals;

      nminutiae = max_minutiae;
   } 
   else
   {
      xptr = xvals_lng;
      yptr = yvals_lng;
      tptr = tvals_lng;
      qptr = qvals_lng;
   }


   for ( j=0; j < nminutiae; j++ ) 
   {
      c[j].col[0] = xptr[j];
      c[j].col[1] = yptr[j];
      c[j].col[2] = tptr[j];
      c[j].col[3] = qptr[j];
   }
   qsort( (void *) &c, (size_t) nminutiae, sizeof(struct minutiae_struct), sort_x_y );

   if ( verbose_load ) {
      fprintf( errorfp, "\nSorted on increasing x, then increasing y\n" );
      for ( j = 0; j < nminutiae; j++ ) 
      {
         fprintf( errorfp, "%d : %3d, %3d, %3d, %3d\n", j, c[j].col[0], c[j].col[1], c[j].col[2], c[j].col[3] );
         if ( j > 0 ) 
         {
            if ( c[j].col[0] < c[j-1].col[0] ) 
            {
               fprintf( errorfp, "%s: ERROR: sort failed: c[%d].col[0]=%d > c[%d].col[0]=%d\n",
                        get_progname(),
                        j, c[j].col[0], j-1, c[j-1].col[0]
                        );
               return XYT_NULL;
            }
            if ( c[j].col[0] == c[j-1].col[0] && c[j].col[1] < c[j-1].col[1] ) 
            {
               fprintf( errorfp, "%s: ERROR: sort failed: c[%d].col[0]=%d == c[%d].col[0]=%d; c[%d].col[0]=%d == c[%d].col[0]=%d\n",
                        get_progname(),
                        j, c[j].col[0], j-1, c[j-1].col[0],
                        j, c[j].col[1], j-1, c[j-1].col[1]
                        );
               return XYT_NULL;
            }
         }
      }
   }

   xyt_s = (struct xyt_struct *) malloc( sizeof( struct xyt_struct ) );
   if ( xyt_s == XYT_NULL ) 
   {
      fprintf( errorfp, "ERROR: malloc() failure of xyt_struct.");
      return XYT_NULL;
   }

   for ( j = 0; j < nminutiae; j++ ) 
   {
      xyt_s->xcol[j]     = c[j].col[0];
      xyt_s->ycol[j]     = c[j].col[1];
      xyt_s->thetacol[j] = c[j].col[2];
   }
   xyt_s->nrows = nminutiae;

   return xyt_s;
}

/***********************************************************************/
#ifdef PARALLEL_SEARCH
int fd_readable( int fd )
{
int retval;
fd_set rfds;
struct timeval tv;


FD_ZERO( &rfds );
FD_SET( fd, &rfds );
tv.tv_sec = 0;
tv.tv_usec = 0;

retval = select( fd+1, &rfds, NULL, NULL, &tv );

if ( retval < 0 ) {
   perror( "select() failed" );
   return 0;
}

if ( FD_ISSET( fd, &rfds ) ) {
   /*fprintf( stderr, "data is available now.\n" );*/
   return 1;
}

/* fprintf( stderr, "no data is available\n" ); */
return 0;
}
#endif
