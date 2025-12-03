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

      FILE:           BZ_SORT.C
      ALGORITHM:      Allan S. Bozorth (FBI)
      MODIFICATIONS:  Michael D. Garris (NIST)
                      Stan Janet (NIST)
      DATE:           09/21/2004

      Contains sorting routines responsible for supporting the
      Bozorth3 fingerprint matching algorithm.

***********************************************************************

      ROUTINES:
#cat: sort_quality_decreasing - comparison function passed to stdlib
#cat:            qsort() used to sort minutia qualities
#cat: sort_x_y - comparison function passed to stdlib qsort() used
#cat:            to sort minutia coordinates increasing first on x
#cat:            then on y
#cat: sort_order_decreasing - calls a custom quicksort that sorts
#cat:            a list of integers in decreasing order

***********************************************************************/

#include <stdio.h>
#include <bozorth.h>

/* These are now externally defined in bozorth.h */
/* extern FILE * errorfp; */
/* extern char * get_progname( void ); */

/***********************************************************************/
int sort_quality_decreasing( const void * a, const void * b )
{
struct minutiae_struct * af;
struct minutiae_struct * bf;

af = (struct minutiae_struct *) a;
bf = (struct minutiae_struct *) b;

if ( af->col[3] > bf->col[3] )
	return -1;
if ( af->col[3] < bf->col[3] )
	return 1;
return 0;
}

/***********************************************************************/
int sort_x_y( const void * a, const void * b )
{
struct minutiae_struct * af;
struct minutiae_struct * bf;

af = (struct minutiae_struct *) a;
bf = (struct minutiae_struct *) b;

if ( af->col[0] < bf->col[0] )
	return -1;
if ( af->col[0] > bf->col[0] )
	return 1;

if ( af->col[1] < bf->col[1] )
	return -1;
if ( af->col[1] > bf->col[1] )
	return 1;

return 0;
}

/********************************************************
qsort_decreasing() - quicksort an array of integers in decreasing
                     order [based on multisort.c, by Michael Garris
                     and Ted Zwiesler, 1986]
********************************************************/
/* Used by custom quicksort code below */
static int   stack[BZ_STACKSIZE];
static int * stack_pointer = stack;

/***********************************************************************/
/* return values: 0 == successful, 1 == error */
static int popstack( int *popval )
{
if ( --stack_pointer < stack ) {
	fprintf( errorfp, "%s: ERROR: popstack(): stack underflow\n", get_progname() );
	return 1;
}

*popval = *stack_pointer;
return 0;
}

/***********************************************************************/
/* return values: 0 == successful, 1 == error */
static int pushstack( int position )
{
*stack_pointer++ = position;
if ( stack_pointer > ( stack + BZ_STACKSIZE ) ) {
	fprintf( errorfp, "%s: ERROR: pushstack(): stack overflow\n", get_progname() );
	return 1;
}
return 0;
}

/***********************************************************************/
/*******************************************************************
select_pivot()
selects a pivot from a list being sorted using the Singleton Method.
*******************************************************************/
static int select_pivot( struct cell v[], int left, int right )
{
int midpoint;


midpoint = ( left + right ) / 2;
if ( v[left].index <= v[midpoint].index ) {
	if ( v[midpoint].index <= v[right].index ) {
		return midpoint;
	} else {
		if ( v[right].index > v[left].index ) {
			return right;
		} else {
			return left;
		}
	}
} else {
	if ( v[left].index < v[right].index ) {
		return left;
	} else {
		if ( v[right].index < v[midpoint].index ) {
			return midpoint;
		} else {
			return right;
		}
	}
}
}

/***********************************************************************/
/********************************************************
partition_dec()
Inputs a pivot element making comparisons and swaps with other elements in a list,
until pivot resides at its correct position in the list.
********************************************************/
static void partition_dec( struct cell v[], int *llen, int *rlen, int *ll, int *lr, int *rl, int *rr, int p, int l, int r )
{
#define iswap(a,b) { int itmp = (a); a = (b); b = itmp; }

*ll = l;
*rr = r;
while ( 1 ) {
	if ( l < p ) {
		if ( v[l].index < v[p].index ) {
			iswap( v[l].index, v[p].index )
			iswap( v[l].item,  v[p].item )
			p = l;
		} else {
			l++;
		}
	} else {
		if ( r > p ) {
			if ( v[r].index > v[p].index ) {
				iswap( v[r].index, v[p].index )
				iswap( v[r].item,  v[p].item )
				p = r;
				l++;
			} else {
				r--;
			}
		} else {
			*lr = p - 1;
			*rl = p + 1;
			*llen = *lr - *ll + 1;
			*rlen = *rr - *rl + 1;
			break;
		}
	}
}
}

/***********************************************************************/
/********************************************************
qsort_decreasing()
This procedure inputs a pointer to an index_struct, the subscript of an index array to be
sorted, a left subscript pointing to where the  sort is to begin in the index array, and a right
subscript where to end. This module invokes a  decreasing quick-sort sorting the index array  from l to r.
********************************************************/
/* return values: 0 == successful, 1 == error */
static int qsort_decreasing( struct cell v[], int left, int right )
{
int pivot;
int llen, rlen;
int lleft, lright, rleft, rright;


if ( pushstack( left  ))
	return 1;
if ( pushstack( right ))
	return 2;
while ( stack_pointer != stack ) {
	if (popstack(&right))
		return 3;
	if (popstack(&left ))
		return 4;
	if ( right - left > 0 ) {
		pivot = select_pivot( v, left, right );
		partition_dec( v, &llen, &rlen, &lleft, &lright, &rleft, &rright, pivot, left, right );
		if ( llen > rlen ) {
			if ( pushstack( lleft  ))
				return 5;
			if ( pushstack( lright ))
				return 6;
			if ( pushstack( rleft  ))
				return 7;
			if ( pushstack( rright ))
				return 8;
		} else{
			if ( pushstack( rleft  ))
				return 9;
			if ( pushstack( rright ))
				return 10;
			if ( pushstack( lleft  ))
				return 11;
			if ( pushstack( lright ))
				return 12;
		}
	}
}
return 0;
}

/***********************************************************************/
/* return values: 0 == successful, 1 == error */
int sort_order_decreasing(
		int values[],		/* INPUT:  the unsorted values themselves */
		int num,		/* INPUT:  the number of values */
		int order[]		/* OUTPUT: the order for each of the values if sorted */
		)
{
int i;
struct cell * cells;


cells = (struct cell *) malloc( num * sizeof(struct cell) );
if ( cells == (struct cell *) NULL ){
	fprintf( errorfp, "%s: ERROR: malloc(): struct cell\n", get_progname() );
        return 1;
}

for( i = 0; i < num; i++ ) {
	cells[i].index = values[i];
	cells[i].item  = i;
}

if ( qsort_decreasing( cells, 0, num-1 ) < 0)
	return 2;

for( i = 0; i < num; i++ ) {
	order[i] = cells[i].item;
}

free( (void *) cells );

return 0;
}
