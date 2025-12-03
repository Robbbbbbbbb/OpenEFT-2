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


/************************************************************************/
/***********************************************************************
      LIBRARY: MLP - NIST MLP Neural Network Utilities

      FILE:           PAT_IO.C
      AUTHOR:         Patrick Grother
                      Michael D. Garris
      UPDATED:        09/10/2004
      UPDATED:        03/21/2005 by MDG
      UPDATED:        12/02/2008 by Kenneth Ko - Fix to support 64-bit
      UPDATED:        07/10/2014 by Kenneth Ko 

      Contains routines responsible for reading and writing feature
      vector and target vector patterns files used by the NIST
      MLP neural network.

***********************************************************************
               ROUTINES:
                        read_bin_nnpats()
                        read_num_pats()
                        write_bin_nnpats()
                        write_text_nnpats()

***********************************************************************/

#include <mlp.h>

/***********************************************************************
************************************************************************
#cat: read_bin_nnpats - Routine reads feature vectors and their classes
#cat:                   from a file using fast binary io.

   Input:
      ifile       - name of input patterns file
   Output:
      ofeats      - feature vectors read from file
      otargs      - target vectors read from file
      oclasses    - class index read for each feature vector
      oclass_set  - the symbol (string) for each class
      onPats      - number of feature vectors read
      onInps      - length of each feature vector (number of coefs)
      onOuts      - length of each target vector (number of classes)
   Return Code:
      Zero        - successful completion
      Negative    - system error
************************************************************************/
int read_bin_nnpats(char *ifile, float **ofeats, float **otargs,
         int **oclasses, char ***oclass_set,
         int *onPats, int *onInps, int *onOuts)
{

   char **class_set;
   int *classes;
   float *feats, *targs;
   int nPats, nInps, nOuts;
   int nFeats, nTargs, nClass;
   int PINPS, POUTS;

   float *featsptr,  /* temporary pointer to input featss  */
         *targsptr;  /* temporary pointer to class outputs  */
   int   *anintptr;
   FILE  *fp;        /* source input pattern input file  */
   int i, j, k, n;
   int idum, tree, targ_typ, targid;

   if ((fp = fopen(ifile, "rb")) == NULL){
      fprintf(stderr, "ERROR : read_bin_nnpats : fopen : %s\n", ifile);
      return(-2);
   }

   if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : idum1a\n");
      return(-3);
   }

   if ((n = fread(&nPats, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : nPats\n");
      return(-4);
   }
#ifdef __NBISLE__
   swap_int_bytes(nPats);
#endif

   if ((n = fread(&nInps, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : nInps\n");
      return(-5);
   }
#ifdef __NBISLE__
   swap_int_bytes(nInps);
#endif

   if ((n = fread(&nOuts, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : nOuts\n");
      return(-6);
   }
#ifdef __NBISLE__
   swap_int_bytes(nOuts);
#endif

   if ((n = fread(&targ_typ, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : targ_typ\n");
      return(-7);
   }
#ifdef __NBISLE__
   swap_int_bytes(targ_typ);
#endif

   if ((n = fread(&tree, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : tree\n");
      return(-8);
   }
#ifdef __NBISLE__
   swap_int_bytes(tree);
#endif

   if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : nPats\n");
      return(-9);
   }

   if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_bin_nnpats : fread : idum1b\n");
      return(-10);
   }

   if (tree == TREEPATSFILE){
      fprintf(stderr, "ERROR : read_bin_nnpats : ");
      fprintf(stderr, "tree patsfile not supported at this time\n");
      return(-11);
   }

   PINPS = nInps;
   POUTS = nOuts;

   nFeats = nPats * PINPS;
   nTargs = nPats * POUTS;
   nClass = nPats;

   if ((feats = (float *)calloc(nFeats, sizeof(float))) == (float *)NULL){
      fprintf(stderr, "ERROR : read_bin_nnpats : calloc : feats\n");
      return(-12);
   }

   if ((targs = (float *)calloc(nTargs, sizeof(float))) == (float *)NULL){
      fprintf(stderr, "ERROR : read_bin_nnpats : calloc : targs\n");
      return(-13);
   }

   if ((classes = (int *)calloc(nClass, sizeof(int))) == (int *)NULL){
      fprintf(stderr, "ERROR : read_bin_nnpats : calloc : classes\n");
      return(-14);
   }

   if ((class_set = (char **)calloc(nOuts, sizeof(char *))) == (char **)NULL){
      fprintf(stderr, "ERROR : read_bin_nnpats : calloc : class_set\n");
      return(-15);
   }

   if (tree == JUSTPATSFILE){
      if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
         free(feats);
         free(targs);
         free(classes);
         free(class_set);
         fprintf(stderr, "ERROR : read_bin_nnpats : fread : idum2a\n");
         return(-16);
      }

      for(i = 0; i < nOuts; i++){
         if ((class_set[i] = (char *)calloc(32, sizeof(char))) == NULL){
            free(feats);
            free(targs);
            free(classes);
            free(class_set);
            fprintf(stderr,
            "ERROR : read_bin_nnpats : calloc : class_set[%d]\n", i);
            return(-17);
         }

         if ((n = fread(class_set[i], sizeof(char), 32, fp)) != 32){
            for(k = 0; k < nOuts; k++)
               free(class_set[k]);
            free(feats);
            free(targs);
            free(classes);
            free(class_set);
            fprintf(stderr,
            "ERROR : read_bin_nnpats : fread : class_set[%d]\n", i);
            return(-18);
         }
      }

      if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
         for(k = 0; k < nOuts; k++)
            free(class_set[k]);
         free(feats);
         free(targs);
         free(classes);
         free(class_set);
         fprintf(stderr,
            "ERROR : read_bin_nnpats : fread : idum2b\n");
         return(-19);
      }

      featsptr = feats;
      targsptr = targs;
      anintptr = classes;

      for (i = 0; i < nPats; i++){
         if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
            for(k = 0; k < nOuts; k++)
               free(class_set[k]);
            free(feats);
            free(targs);
            free(classes);
            free(class_set);
            fprintf(stderr,
               "ERROR : read_bin_nnpats : fread : idum3a\n");
            return(-20);
         }

         if ((n = fread(featsptr, sizeof(float), nInps, fp)) != nInps){
            for(k = 0; k < nOuts; k++)
               free(class_set[k]);
            free(feats);
            free(targs);
            free(classes);
            free(class_set);
            fprintf(stderr,
               "ERROR : read_bin_nnpats : fread : feature vector\n");
            return(-21);
         }
#ifdef __NBISLE__
         for(j = 0; j < nInps; j++)
            swap_float_bytes(featsptr[j]);
#endif

         featsptr += PINPS;

         if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
            for(k = 0; k < nOuts; k++)
               free(class_set[k]);
            free(feats);
            free(targs);
            free(classes);
            free(class_set);
            fprintf(stderr,
               "ERROR : read_bin_nnpats : fread : idum3b\n");
            return(-22);
         }

         if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
            for(k = 0; k < nOuts; k++)
               free(class_set[k]);
            free(feats);
            free(targs);
            free(classes);
            free(class_set);
            fprintf(stderr,
               "ERROR : read_bin_nnpats : fread : idum4a\n");
            return(-23);
         }

         if(targ_typ){
            if ((n = fread(&targid, sizeof(int), 1, fp)) != 1){
               for(k = 0; k < nOuts; k++)
                  free(class_set[k]);
               free(feats);
               free(targs);
               free(classes);
               free(class_set);
               fprintf(stderr,
                  "ERROR : read_bin_nnpats : fread : targid\n");
               return(-24);
            }
#ifdef __NBISLE__
            swap_int_bytes(targid);
#endif

            anintptr[i] = (targid - 1);

            for(j = 0; j < nOuts; j++)
               if(anintptr[i] == j)
                  *targsptr++ = 1.0;
               else
                  *targsptr++ = 0.0;

            targsptr += POUTS - nOuts;
         }
         else {
            if ((n = fread(targsptr, sizeof(float), nOuts, fp)) != nOuts){
               for(k = 0; k < nOuts; k++)
                  free(class_set[k]);
               free(feats);
               free(targs);
               free(classes);
               free(class_set);
               fprintf(stderr,
                  "ERROR : read_bin_nnpats : fread : target vector\n");
               return(-25);
            }
#ifdef __NBISLE__
            for(j = 0; j < nOuts; j++)
               swap_float_bytes(targsptr[j]);
#endif

            targsptr += POUTS;  /* pad to dimension with the */
            /* zeroes returned by calloc */
         }
         if ((n = fread(&idum, sizeof(int), 1, fp)) != 1){
            for(k = 0; k < nOuts; k++)
               free(class_set[k]);
            free(feats);
            free(targs);
            free(classes);
            free(class_set);
            fprintf(stderr,
               "ERROR : read_bin_nnpats : fread : idum4b\n");
            return(-26);
         }
      } /* end for */

      if(!targ_typ){
         targsptr = targs;
         anintptr = classes;

         for(i = 0; i < nPats; i++){ /* obtain from the target list */
            /* the node index (ie class) */
            anintptr[i] = -1;  /* of the node near 1.0  */
            for(j = 0; j < nOuts; j++) {/* 0 0 0 1 0 is class 4 */
               if (*targsptr++ >= 0.99){
                  if (anintptr[i] == -1) /* error: class -1 if no*/
                     anintptr[i] =  j; /* target near 1 is defined */
                  else
                     anintptr[i] = -2; /* error: class is -2 if more */
               }   /* than one target is 1  */
            }
            targsptr += POUTS - nOuts; /* jump over zeroes used to  */
            /* fill dap dimension   */
         }
      }
   }
   /* Otherwise, unrecognized tree type ... */
   else{
      for(k = 0; k < nOuts; k++)
         free(class_set[k]);
      free(feats);
      free(targs);
      free(classes);
      free(class_set);
      fprintf(stderr, "ERROR : read_bin_nnpats : unrecognized tree flag\n");
      return(-27);
   }

   fclose(fp);

   *ofeats = feats;
   *otargs = targs;
   *oclass_set = class_set;
   *oclasses = classes;
   *onPats = nPats;
   *onInps = nInps;
   *onOuts = nOuts;

   return(0);
}

/***********************************************************************
************************************************************************
#cat: read_num_pats - Routine opens a binary patterns file, reads, and
#cat:                  returns the number of pattern feature vectors
#cat:                  in the file.

   Input:
      fn           - name of input patterns file
   Return Code:
      Non-negative - number of pattern feature vectors in the file
      Negative     - system error
************************************************************************/
int read_num_pats(char *fn)
{
   FILE  *fp;
   int   n, idum, nPats;

   if ((fp = fopen(fn, "rb")) == (FILE *)NULL){
      fprintf(stderr, "ERROR : read_num_patterns : fopen : %s\n", fn);
      return(-2);
   }

   if ((n = fread(&idum, sizeof(int ), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_num_patterns : fread : idum1a\n");
      fclose(fp);
      return(-3);
   }

   if ((n = fread(&nPats, sizeof(int ), 1, fp)) != 1){
      fprintf(stderr, "ERROR : read_num_patterns : fread : nPats\n");
      fclose(fp);
      return(-4);
   }

   fclose(fp);

#ifdef __NBISLE__
   swap_int_bytes(nPats);
#endif

   return(nPats);
}

/***********************************************************************
************************************************************************
#cat: read_text_nnpats - Routine reads feature vectors and their class
#cat:                    target vectors from an ASCII file.

   Input:
      ifile       - name of output patterns file
   Output:
      ofeats      - feature vectors read from file
      otargs      - target vectors read from file
      oclass_set  - the symbol (string) for each class
      onPats      - number of feature vectors read
      onInps      - length of each feature vector (number of coefs)
      onOuts      - length of each target vector (number of classes)
   Return Code:
      Zero        - successful completion
      Negative    - system error
************************************************************************/
int read_text_nnpats(char *ifile, float **ofeats, float **otargs,
          char ***oclass_set, int *onPats, int *onInps, int *onOuts)
{
   int   i, j, k, n;
   int   nPats, nInps, nOuts;
   int   nFeats, nTargs;
   FILE  *fpPats;   /* source input pattern input file*/
   int   PINPS, POUTS;
   char  **class_set, localclass[100];
   float *feats, *targs;
   float *featsptr, /* temporary pointer to feature vectors */
         *targsptr; /* temporary pointer to target vectors */

   if ((fpPats = fopen(ifile, "rb")) == (FILE *)NULL){
      fprintf(stderr, "ERROR : read_text_nnpats : fopen : %s\n", ifile);
      return(-2);
   }

   if((n = fscanf(fpPats, "%d", &nPats)) != 1){
      fprintf(stderr, "ERROR : read_text_nnpats : fscanf : nPats\n");
      return(-3);
   }

   if((n = fscanf(fpPats, "%d", &nInps)) != 1){
      fprintf(stderr, "ERROR : read_text_nnpats : fscanf : nInps\n");
      return(-4);
   }

   if((n = fscanf(fpPats, "%d", &nOuts)) != 1){
      fprintf(stderr, "ERROR : read_text_nnpats : fscanf : nOuts\n");
      return(-5);
   }

   PINPS = nInps;
   POUTS = nOuts;

   if ((class_set = (char **)calloc(nOuts, sizeof(char *))) == NULL){
      fprintf(stderr, "ERROR : read_text_nnpats : calloc : class_set\n");
      return(-6);
   }

   for (i = 0; i < nOuts; i++){  /* read in class characters */
      if((n = fscanf(fpPats, "%s", localclass)) != 1){
         for(k = 0; k < i; k++)
             free(class_set[k]);
         free(class_set);
         fprintf(stderr, "ERROR : read_text_nnpats : fscanf : localclass\n");
         return(-7);
      }
      
      size_t len = strlen(localclass) + 1;
      char *value = malloc(len);
      if (value != (char *)NULL){       
         strncpy(value, localclass, len);
         class_set[i] = value;
      }
      else{
         for(k = 0; k < i; k++)
             free(class_set[k]);
         free(class_set);
         fprintf(stderr, "ERROR : read_text_nnpats : strdup : class_set[i]\n");
         return(-8);
      }
   }

   nFeats = nPats * PINPS;
   nTargs = nPats * POUTS;

   if ((feats = (float *)calloc(nFeats, sizeof(float))) == (float *)NULL){
      for(k = 0; k < nOuts; k++)
          free(class_set[k]);
      free(class_set);
      fprintf(stderr, "ERROR : read_text_nnpats : calloc : feats\n");
      return(-9);
   }

   if ((targs = (float *)calloc(nTargs, sizeof(float))) == (float *)NULL){
      for(k = 0; k < nOuts; k++)
          free(class_set[k]);
      free(class_set);
      free(feats);
      fprintf(stderr, "ERROR : read_text_nnpats : calloc : targs\n");
      return(-10);
   }

   featsptr = feats;
   targsptr = targs;

   for(i = 0; i < nPats; i++){
      for(j = 0; j < nInps; j++){   /* read in actual input data*/
         if((n = fscanf(fpPats, "%f", featsptr++)) != 1){
            for(k = 0; k < nOuts; k++)
                free(class_set[k]);
            free(class_set);
            free(feats);
            free(targs);
            fprintf(stderr, "ERROR : read_text_nnpats : fscanf : featsptr\n");
            return(-11);
         }
      }

      for(j = 0; j < nOuts; j++){ /* read in actual target data*/
         if((n = fscanf(fpPats, "%f", targsptr++)) != 1){
            for(k = 0; k < nOuts; k++)
                free(class_set[k]);
            free(class_set);
            free(feats);
            free(targs);
            fprintf(stderr, "ERROR : read_text_nnpats : fscanf : targsptr\n");
            return(-12);
         }
      }
   }
   fclose(fpPats);

   *ofeats = feats;
   *otargs = targs;
   *oclass_set = class_set;
   *onPats = nPats;
   *onInps = nInps;
   *onOuts = nOuts;

   return(0);
}

/***********************************************************************
************************************************************************
#cat: write_bin_nnpats - Routine writes feature vectors and their classes
#cat:                    to a binary file for use by NIST's MLP code.

   Input:
      ofile       - name of output patterns file
   Output:
      feats       - feature vectors to be written out
      targs       - target vectors to be written out
      class_set   - the symbol (string) for each class
      nPats       - number of feature vectors
      nInps       - length of each feature vector (number of coefs)
      nOuts       - length of each target vector (number of classes)
   Return Code:
      Zero        - successful completion
      Negative    - system error
************************************************************************/
int write_bin_nnpats(char *ofile, float *feats, float *targs,
          char **class_set, const int nPats, const int nInps, const int nOuts)
{
   float *featsptr, *targsptr;
   int   i, j, n, nChars;
   int   targ_typ = 0, idum = 0, tree = JUSTPATSFILE;
   int   wsize, wisize, wosize;
   FILE  *fp;
   unsigned int tuint;

   if ((fp = fopen(ofile, "wb")) == NULL){
      fprintf(stderr, "ERROR : write_bin_nnpats : fopen : %s\n", ofile);
      return(-2);
   }

   wsize = 24;
   tuint = wsize;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : wsize1a fwrite\n");
      return(-3);
   }

   tuint = nPats;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : nPats fwrite\n");
      return(-4);
   }

   tuint = nInps;
#ifdef __NBISLE__
    swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : nInps fwrite\n");
      return(-5);
   }

   tuint = nOuts;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : nOuts fwrite\n");
      return(-6);
   }

   tuint = targ_typ;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : targ_typ fwrite\n");
      return(-7);
   }

   tuint = tree;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : dummy1 fwrite\n");
      return(-8);
   }

   tuint = idum;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : dummy2 fwrite\n");
      return(-9);
   }

   tuint = wsize;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : wsize1b fwrite\n");
      return(-10);
   }

   wsize = 32*nOuts;
   tuint = wsize;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : wsize2a fwrite\n");
      return(-11);
   }

   for (i = 0; i < nOuts; i++)
   {
      nChars = strlen(class_set[i]);
      if ((n = fwrite(class_set[i], sizeof(char), nChars, fp)) != nChars){
         fprintf(stderr, "ERROR : write_bin_nnpats : string fwrite\n");
         return(-12);
      }
      for(j = 0; j < (32 - nChars); j++){
         if ((n = fwrite("\0", sizeof(char), 1, fp)) != 1){
            fprintf(stderr, "ERROR : write_bin_nnpats : null fwrite\n");
            return(-13);
         }
      }
   }

   tuint = wsize;
#ifdef __NBISLE__
   swap_uint_bytes(tuint);
#endif
   if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
      fprintf(stderr, "ERROR : write_bin_nnpats : wsize2b fwrite\n");
      return(-14);
   }

   featsptr = feats;
   targsptr = targs;

   wisize = 4*nInps;
   wosize = 4*nOuts;
   for(i = 0; i < nPats ; i++)
   {
      tuint = wisize;
#ifdef __NBISLE__
      swap_uint_bytes(tuint);
#endif
      if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
         fprintf(stderr, "ERROR : write_bin_nnpats : wsize3a fwrite\n");
         return(-15);
      }

#ifdef __NBISLE__
      for(j = 0; j < nInps; j++)
         swap_float_bytes(featsptr[j]);
#endif

      if ((n = fwrite(featsptr, sizeof(float), nInps, fp)) != nInps){
         fprintf(stderr, "ERROR : write_bin_nnpats : ");
         fprintf(stderr, "feature vector fwrite\n");
         return(-16);
      }

#ifdef __NBISLE__
      for(j = 0; j < nInps; j++)
         swap_float_bytes(featsptr[j]);
#endif

      tuint = wisize;
#ifdef __NBISLE__
      swap_uint_bytes(tuint);
#endif
      if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
         fprintf(stderr, "ERROR : write_bin_nnpats : wsize3b fwrite\n");
         return(-17);
      }

      tuint = wosize;
#ifdef __NBISLE__
      swap_uint_bytes(tuint);
#endif
      if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
         fprintf(stderr, "ERROR : write_bin_nnpats : wsize4a fwrite\n");
         return(-18);
      }

#ifdef __NBISLE__
      for(j = 0; j < nOuts; j++)
         swap_float_bytes(targsptr[j]);
#endif

      if ((n = fwrite(targsptr, sizeof(float), nOuts, fp)) != nOuts){
         fprintf(stderr, "ERROR : write_bin_nnpats : ");
         fprintf(stderr, "target vector fwrite\n");
         return(-19);
      }

#ifdef __NBISLE__
      for(j = 0; j < nOuts; j++)
         swap_float_bytes(targsptr[j]);
#endif

      featsptr += nInps;
      targsptr += nOuts;
                       
      tuint = wosize;
#ifdef __NBISLE__
      swap_uint_bytes(tuint);
#endif
      if ((n = fwrite(&tuint, sizeof(unsigned int), 1, fp)) != 1){
         fprintf(stderr, "ERROR : write_bin_nnpats : wsize4b fwrite\n");
         return(-20);
      }
   }
   fclose(fp);

   return(0);
}


/***********************************************************************
************************************************************************
#cat: write_text_nnpats - Routine writes feature vectors and their classes
#cat:                     to an ASCII file.

   Input:
      ofile       - name of output patterns file
   Output:
      feats       - feature vectors to be written out
      targs       - target vectors to be written out
      class_set   - the symbol (string) for each class
      nPats       - number of feature vectors
      nInps       - length of each feature vector (number of coefs)
      nOuts       - length of each target vector (number of classes)
   Return Code:
      Zero        - successful completion
      Negative    - system error
************************************************************************/
int write_text_nnpats(char *ofile, float *feats, float *targs,
          char **class_set, const int nPats, const int nInps, const int nOuts)
{
   float *featsptr, *targsptr;
   int   i, j;
   FILE  *fp;

   if ((fp = fopen(ofile, "wb")) == NULL){
      fprintf(stderr, "ERROR : write_text_nnpats : fopen : %s\n", ofile);
      return(-2);
   }

   fprintf(fp, "%d %d %d\n", nPats, nInps, nOuts);
   for (i = 0; i < nOuts; i++)
     fprintf(fp, "%s ", class_set[i]);
   fprintf(fp, "\n");

   featsptr = feats;
   targsptr = targs;

   for(i=0; i<nPats ; i++)
   {
      for(j=0; j<nInps; j++)
        fprintf(fp, "%f%c", *featsptr++,
                j%FMT_ITEMS==FMT_ITEMS-1 ? '\n' : ' ');

      if (nInps%FMT_ITEMS != 0)
        fprintf(fp, "\n");

      for(j=0; j<nOuts; j++)
         fprintf(fp, "%f ", *targs++);

      fprintf(fp, "\n");
   }
   fclose(fp);

   return(0);
}

