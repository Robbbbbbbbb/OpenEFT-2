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
      LIBRARY: PCASYS_UTILS - Pattern Classification System Utils

      FILE:    TABLE.C
      AUTHORS: G. T. Candela
      DATE:    1995
      UPDATED: 04/20/2005 by MDG

      A simple linear search utility for a table of pairs of floats.  To
      use, include <table.h> and define one or more TABLES, and use their
      addresses as args of these routines.  To free the buffer used by,
      say, TABLE a_table, just do free(a_table.buf).

      NOTE: If a table is small, and the program that uses it will spend
            a lot of cycles on other work, then the overall loss of time
            caused by using this instead of a more efficient table
            (e.g. hash table) is going to be insignificant.

      ROUTINES:
#cat: table_init - allocates a TABLE that can hold a specified no. of pairs.
#cat: table_store - stores a pair in the table.
#cat: table_lookup - looks up a float against the first elts of the pairs.
#cat: table_clear - sets the number stored back to zero, in effect clearing
#cat:               the table.

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <table.h>
#include <util.h>

/********************************************************************/

void table_init(TABLE *table, const int size)
{
  if(!(table->buf = (float *)malloc(size * 2 * sizeof(float))))
    fatalerr("table_init (file table.c)", "malloc", "table->buf");
  table->size = size;
  table->n_stored = 0;
}

/********************************************************************/

/* Stores a pair at the current end of the table. */

void table_store(TABLE *table, const float f, const float s)
{
  float *p;

  if(table->n_stored == table->size)
    fatalerr("table_store (file table.c)", "table full", NULL);
  *(p = table->buf + 2 * (table->n_stored)) = f;
  *++p = s;
  (table->n_stored)++;
}

/********************************************************************/

/* Looks up f in the table.  If found, sets *s and returns 1;
otherwise, returns 0. */

int table_lookup(TABLE *table, const float f, float *s)
{
  float *p, *pe;

  for(pe = (p = table->buf) + 2 * (table->n_stored); p < pe; p += 2)
    if(f == *p) {
      *s = *++p;
      return 1;
    }
  return 0;
}

/********************************************************************/

void table_clear(TABLE *table)
{
  table->n_stored = 0;
}

/********************************************************************/
