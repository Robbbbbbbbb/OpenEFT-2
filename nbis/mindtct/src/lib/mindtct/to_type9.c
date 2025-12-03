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
      LIBRARY: LFS - NIST Latent Fingerprint System

      FILE:    TO_TYPE9.C
      AUTHOR:  Michael D. Garris
      DATE:    10/23/2000
      UPDATED: 09/13/2004
      UPDATED: 03/16/2005 by MDG
      UPDATED: 01/31/2008 by Kenneth Ko
      UPDATED: 09/04/2008 by Kenneth Ko

      Contains routines responsible for converting LFS minutiae
      results to an ANSI/NIST 2007 Type-9 record.

***********************************************************************
               ROUTINES:
                        minutiae2type_9()
                        minutiae2field_12()

***********************************************************************/

#include <stdio.h>
#include <an2k.h>
#include <lfs.h>

/*************************************************************************
**************************************************************************
#cat: minutiae2type_9 - Takes a structure containing detected minutiae
#cat:                and converts it into an ANSI/NIST 2000 Type-9 record
#cat:                with Fields 001-012 populated.  Note this record is not
#cat:                compatible with the FBI/IAFIS EFTS Version 7.

   Input:
      img_idc  - new Type-9 record's IDC
      minutiae - structure containing detected minutiae
      iw       - width (in pixels) of the fingerprint image used to
                 detect the minutiae
      ih       - height (in pixels) of the fingerprint image used to
                 detect the minutiae
      ppmm     - the scan resolution (in pixels/mm) of the fingerprint image
                 used to detect the minutiae
   Output:
      otype_9  - points to resulting Type-9 minutiae record
   Return Code:
      Zero     - successful completion
      Negative - system error
**************************************************************************/
int minutiae2type_9(RECORD **otype_9, const int img_idc, MINUTIAE *minutiae,
                    const int iw, const int ih, const double ppmm)
{
   int ret;
   RECORD *record;
   FIELD *field;
   SUBFIELD *subfield;
   ITEM *item;
   char uint_str[MAX_UINT_CHARS+1];

   /* Allocate new record. */
   if((ret = new_ANSI_NIST_record(&record, TYPE_9_ID)))
      return(ret);

   /* 9.001: LEN Field */
   /* Create LEN field with value == "0" (place holder for now). */
   if((ret = value2field(&field, TYPE_9_ID, LEN_ID, "0"))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with LEN field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.002: IDC Field */
   /* Create IDC field with default value == 1. */
   /* Remember that IDC is 2 bytes according to EFTS, so stay */
   /* consistent here. */
   sprintf(uint_str, "%02d", img_idc);
   if((ret = value2field(&field, TYPE_9_ID, IDC_ID, uint_str))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with IDC field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.003: IMP Field */
   /* Create IMP field with default value == 4. */
   /* ASSUMED latent impression image processed. */
   if((ret = value2field(&field, TYPE_9_ID, IMP_ID, "4"))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with IMP field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.004: FMT Field */
   /* Create FMT field with standard value == "S". */
   if((ret = value2field(&field, TYPE_9_ID, FMT_ID, STD_STR))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with FMT field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.005: OFR Field */
   /* Create OFR subfield with first item (system) == "NIST_LFS_VER2". */
   if((ret = value2subfield(&subfield, LFS_VERSION_STR))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Create second item (method) with value == "A" (automatic). */
   if((ret = value2item(&item, AUTO_STR))){
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append subfield with 2nd item. */
   if((ret = append_ANSI_NIST_subfield(subfield, item))){
      free_ANSI_NIST_item(item);
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Create new OFR field. */
   if((ret = new_ANSI_NIST_field(&field, TYPE_9_ID, OFR_ID))){
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append subfield into new OFR field. */
   if((ret = append_ANSI_NIST_field(field, subfield))){
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with OFR field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.006: FGP2 Field */
   /* Create FGP2 field with standard value == "00" (unknown). */
   /* Remember that Finger Position is 2 bytes according to EFTS, */
   /* so stay consistent here. */
   if((ret = value2field(&field, TYPE_9_ID, FGP2_ID, "00"))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with FGP2 field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.007: FPC Field */
   /* Create FPC subfield with first item == "T" (standard table). */
   if((ret = value2subfield(&subfield, TBL_STR))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Create second item (pattern class) with value == "UN" (unknown). */
   if((ret = value2item(&item, "UN"))){
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append subfield with pattern class item. */
   if((ret = append_ANSI_NIST_subfield(subfield, item))){
      free_ANSI_NIST_item(item);
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Create new FPC field. */
   if((ret = new_ANSI_NIST_field(&field, TYPE_9_ID, FPC_ID))){
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append subfield into new FPC field. */
   if((ret = append_ANSI_NIST_field(field, subfield))){
      free_ANSI_NIST_subfield(subfield);
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with FPC field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.010: MIN Field */
   /* Create MIN field with value == number of minutiae. */
   /* Convert number of minutiae into string. */
   sprintf(uint_str, "%d", minutiae->num);
   if((ret = value2field(&field, TYPE_9_ID, MIN_ID, uint_str))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with MIN field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* 9.011: RDG Field */
   /* Create RDG field with value == "1" (ridges present). */
   if((ret = value2field(&field, TYPE_9_ID, RDG_ID, "1"))){
      free_ANSI_NIST_record(record);
      return(ret);
   }
   /* Append record with RDG field. */
   if((ret = append_ANSI_NIST_record(record, field))){
      free_ANSI_NIST_field(field);
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* !!!! This test added 09-13-04 !!!! */
   /* Only create an MRC Field if there are minutiae recorded */
   if(minutiae->num > 0){
      /* 9.012: MRC Field */
      /* Create MRC field from minutiae structure. */
      if((ret = mintiae2field_12(&field, minutiae, iw, ih, ppmm))){
         free_ANSI_NIST_record(record);
         return(ret);
      }
      /* Append record with MRC field. */
      if((ret = append_ANSI_NIST_record(record, field))){
         free_ANSI_NIST_field(field);
         free_ANSI_NIST_record(record);
         return(ret);
      }
   }

   /* Update the LEN field with length of record. */
   if((ret = update_ANSI_NIST_tagged_record_LEN(record))){
      free_ANSI_NIST_record(record);
      return(ret);
   }

   /* Set output pointer. */
   *otype_9 = record;
   /* Return normally. */
   return(0);
}

/*************************************************************************
**************************************************************************
#cat: minutiae2field_12 - Takes a structure containing detected minutiae
#cat:                and converts it into an ANSI/NIST 2000 Type-9.012
#cat:                Minutiae and Ridge Count (MRC) field structure.
#cat:                Note this is field is not compatible with the
#cat:                FBI/IAFIS EFTS Version 7.

   Input:
      minutiae - structure containing detected minutiae
      iw       - width (in pixels) of the fingerprint image used to
                 detect the minutiae
      ih       - height (in pixels) of the fingerprint image used to
                 detect the minutiae
      ppmm     - the scan resolution (in pixels/mm) of the fingerprint image
                 used to detect the minutiae
   Output:
      ofield   - points to resulting Type-9.012 (MRC) minutiae field
   Return Code:
      Zero     - successful completion
      Negative - system error
**************************************************************************/
int mintiae2field_12(FIELD **ofield, MINUTIAE *minutiae,
                     const int iw, const int ih, const double ppmm)
{
   int i, j, ret;
   ITEM *item;
   SUBFIELD *subfield;
   FIELD *field;
   MINUTIA *minutia;
   char value[(MAX_UINT_CHARS*3)+1]; /* Large enough to hold 3 uints. */
   int x_units, h_units, y_units;
   double deg_per_idir;
   int native_deg, std_deg;
   int min_qual;
   char *std_type;

   /* ERROR if no minutiae are recorded as no 9.12 Field should be */
   /* created */
   /* !!!! This test added 09-13-04 !!!! */
   if(minutiae->num < 1){
      fprintf(stderr, "ERROR : minutiae2field_12 : ");
      fprintf(stderr, "no minutiae recorded\n");
      return(-2);
   }

   /* Allocate new MRC field. */
   if((ret = new_ANSI_NIST_field(&field, TYPE_9_ID, MRC_ID)))
      return(ret);

   /* Compute degrees per LFS unit of direction. */
   deg_per_idir = 180.0 / NUM_DIRECTIONS;

   /* Foreach minutiae in list ... */
   for(i = 0; i < minutiae->num; i++){
      /* Set minutia pointer. */
      minutia = minutiae->list[i];

      /* 1. Index item (pad 3) */
      sprintf(value, "%03d", i+1);
      /* Create subfield from Index string. */
      if((ret = value2subfield(&subfield, value))){
         free_ANSI_NIST_field(field);
         return(ret);
      }

      /* 2. XXXXYYYYTTT item */
      /* Convert x-pixel coord to 0.01 millimeter units. */
      x_units = sround((minutia->x / ppmm) * 100.0);
      /* Convert image pixel hieght to 0.01 millimeter units. */
      h_units = sround((ih / ppmm) * 100.0);
      /* Convert y-pixel coord to 0.01 millimeter units with */
      /* origin at the bottom of the image. */
      y_units = h_units - 1 - sround((minutia->y / ppmm) * 100.0);
      /* Convert LFS integer direction to ANSI/NIST standard degrees. */
      /* Convert idir to degrees with 0 degrees vertical and increasing */
      /* clockwise (this is the scheme used by LFS). */
      native_deg = sround(minutia->direction * deg_per_idir);
      /* Convert to standard degrees by converting to degrees increasing */
      /* counter-clockwise and then shifting 0 degrees from vertical to  */
      /* horizontal and to the right (360 - X + 90).  The ANSI/NIST      */
      /* standard specifies directional angles to be 180 degrees         */
      /* rotated from the directions computed by LFS, so the final       */
      /* standard degrees formula is (360 - X + 90 - 180).               */
      std_deg = (270 - native_deg) % 360;
      /* Ensure that orientation is on range [0..359] */
      if(std_deg < 0)
         std_deg += 360;
      /* Construct XXXXYYYYTTT string. */
      sprintf(value, "%04d%04d%03d", x_units, y_units, std_deg);
      /* If new XXXXYYYYTTT value != 11 characters long ... */
      if(strlen(value) > 11){
         fprintf(stderr, "ERROR : minutiae2field_12 : ");
         fprintf(stderr, "minutia %d XXXXYYYYTTT value = %s > 11 chars\n",
                 i+1, value);
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(-3);
      }
      /* Create item from xyt string. */
      if((ret = value2item(&item, value))){
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(ret);
      }
      /* Append subfield with xyt item. */
      if((ret = append_ANSI_NIST_subfield(subfield, item))){
         free_ANSI_NIST_item(item);
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(ret);
      }

      /* 3. Quality item (pad 2) */
      /* Standard quality is on the range [2..63] with 2 being highest */
      /* and 63 being lowest.  The formula below maps [0.0 .. 1.0]     */
      /* onto the range [63..2];  (61 - (R * 61) + 2)                  */
      min_qual = sround(63.0 - (minutia->reliability * 61.0));
      /* Construct quality string string. */
      sprintf(value, "%02d", min_qual);
      /* Create item from quality string. */
      if((ret = value2item(&item, value))){
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(ret);
      }
      /* Append subfield with quality item. */
      if((ret = append_ANSI_NIST_subfield(subfield, item))){
         free_ANSI_NIST_item(item);
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(ret);
      }

      /* 4. Type item */
      switch(minutia->type){
      /* Bifurcation */
      case 0:
         std_type = "B";
         break;
      /* Ridge Ending */
      case 1:
         std_type = "A";
         break;
      default:
         fprintf(stderr, "ERROR : minutiae2field_12 : ");
         fprintf(stderr, "unsupported minutia type = %d\n", minutia->type);
         free_ANSI_NIST_field(field);
         return(-4);
      }
      /* Create item from type string. */
      if((ret = value2item(&item, std_type))){
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(ret);
      }
      /* Append subfield with type item. */
      if((ret = append_ANSI_NIST_subfield(subfield, item))){
         free_ANSI_NIST_item(item);
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(ret);
      }

      /* 4. Ridge Count items (pad nbr indices to 3 & ridge counts to 2) */
      for(j = 0; j < minutia->num_nbrs; j++){
         /* Next Neighbor with ridge count ... formatted as "NNN,CC" */
         /* where NNN = nieghbor index and CC = ridge count. The     */
         /* six character string easily fits in the value string     */
         /* statically allocated.                                    */
         sprintf(value, "%03d,%02d",
                 minutia->nbrs[j], minutia->ridge_counts[j]);
         /* Create item from type string. */
         if((ret = value2item(&item, value))){
            free_ANSI_NIST_subfield(subfield);
            free_ANSI_NIST_field(field);
            return(ret);
         }
         /* Append subfield with type item. */
         if((ret = append_ANSI_NIST_subfield(subfield, item))){
            free_ANSI_NIST_item(item);
            free_ANSI_NIST_subfield(subfield);
            free_ANSI_NIST_field(field);
            return(ret);
         }
      }

      /* Append field with minutia subfield */
      if((ret = append_ANSI_NIST_field(field, subfield))){
         free_ANSI_NIST_subfield(subfield);
         free_ANSI_NIST_field(field);
         return(ret);
      }

   }

   /* Set output pointer. */
   *ofield = field;
   /* Return normally. */
   return(0);
}
