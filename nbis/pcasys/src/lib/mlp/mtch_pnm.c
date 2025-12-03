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
      LIBRARY: MLP - Multi-Layer Perceptron Neural Network

      FILE:    MTCH_PNM.C
      AUTHORS: Charles Wilson
               G. T. Candela
      DATE:    1992
      UPDATED: 03/21/2005 by MDG

      ROUTINES:
#cat: mtch_pnm - Finds out if a supposed parm name matches a specified
#cat:            legal parm name.
      
***********************************************************************/

/*
If the names match, and this parm has not already
been set, and (if this is a switch parm, with a defined set of legal
value-strings) if the value is legal, then this routine sets the value
member for this parm's member of the parms structure to the value
(converted into the appropriate form, if necessary), and it sets into
this parm's member's "ssl" member the specified line-number (of the
line in which the parm was set).  The routine turns on ssl's
"set_tried" member (and it stays on) if the parm names match, but it
turns on ssl's "set" member only if all the checks are passed, and it
turns "set" off if the parm names match but the checks are not all
passed.
*/
/*
Input args:
  a_namestr: The parm-name string of a "goal" parm, against which
    namestr will be compared and whose member of the parms structure,
    if the names match, will be affected.
  a_member: The goal member of the parms structure.
  a_type: The type of the goal parm.  Must be one of:
    MP_FILENAME (A filename.)
    MP_INT      (An int.)
    MP_FLOAT    (A float.)
    MP_SWITCH   (A parm that has a small number of legal values, each
                  of which can be referred to in the spec file by
                  either a name or a code number.)
  legal_values_str: If a_type is MP_FILENAME, this is ignored.  If
    a_type is MP_INT or MP_FLOAT, this should be a string
    showing the minimum and maximum legal values of this parm,
    separated by white space, with "-inf" or "-INF" in the first
    position meaning no minimum and "+inf" or "+INF" in the second
    position meaning no maximum; e.g., "10 50" or "-inf 2."; or,
    to set no limits on either end, this parm can be set to NULL,
    less verbose than "-inf +inf".  If a_type is MP_SWITCH, this
    should be a string showing each legal-value-name-string,
    legal-value-codenumber-string pair for this parm, all catenated
    together with whitespace between the words, e.g.
    "no_prune 0 abs_prune 2 square_prune 3" (the legal value strings
    and asociated code numbers for boltzmann).
*/
/*
Input/output args:
  nveol: (Address of) an NVEOL structure (typedef in mtch_pnm.h),
    containing the following members (bundled together to reduce
    verbosity of calls):
    linenum: Number of the specfile line in which this attempted
      parm-setting occurs.
    ok: A flag that starts out FALSE for this attempted parm-setting
      before any of the calls of this routine.  If this routine finds
      that the parm-names match and that all checks are passed, then
      it switches ok to TRUE.
    namestr: The (supposed) parm-name string of the parm-setting.
    valstr: The (supposed) parm-value string of the parm-setting.
    errstr: If this routine detects an error situation, it loads an
      error message into this buffer (which must be already allocated
      in nveol).

Return value:
  TRUE (FALSE): nveol->namestr does (does not) match a_namestr.
*/

#include <mlp.h>

char mtch_pnm(NVEOL *nveol, char *a_namestr, void *a_member,
              char a_type, char *legal_values_str)
{
  char legal_valname_str[10][50], *legal_valname_p[10],
    legal_valcode_str[10][2], str[500], legal_min_str[20],
    legal_max_str[20], value_is_legal;
  int n_legal, i, n_scanned;
  float a;
  SSL *sslp;

  if(strcmp(a_namestr, nveol->namestr))
    return FALSE;
  if(a_type == MP_FILENAME)
    sslp = &(((PARM_FILENAME *)a_member)->ssl);
  else if(a_type == MP_INT)
    sslp = &(((PARM_INT *)a_member)->ssl);
  else if(a_type == MP_FLOAT)
    sslp = &(((PARM_FLOAT *)a_member)->ssl);
  else /* a_type == MP_SWITCH */
    sslp = &(((PARM_SWITCH *)a_member)->ssl);
  if(sslp->set_tried) {
    sprintf(str, "ERROR, line %d: %s set, but it was already set in \
line %d", nveol->linenum, a_namestr, sslp->linenum);
    strm_fmt(str, nveol->errstr);
  }
  else {
    sslp->set_tried = TRUE;
    sslp->linenum = nveol->linenum;
    if(a_type == MP_FILENAME) {
      nveol->ok = TRUE;
      strcpy(((PARM_FILENAME *)a_member)->val, nveol->valstr);
    }
    else if(a_type == MP_INT) {
      n_scanned = sscanf(nveol->valstr, "%d", &i);
      if(n_scanned != 1) {
	sprintf(str, "ERROR, line %d: value, %s, for %s, is not an \
integer", nveol->linenum, nveol->valstr, a_namestr);
	strm_fmt(str, nveol->errstr);
      }
      else if(legal_values_str == (char *)NULL)
	nveol->ok = TRUE;
      else {
	sscanf(legal_values_str, "%s %s", legal_min_str,
          legal_max_str);
	if(strcmp(legal_min_str, "-inf") &&
          strcmp(legal_min_str, "-INF") &&
	  (i < atoi(legal_min_str))) {
	  sprintf(str, "ERROR, line %d: value, %s, for %s, is \
smaller than the minimum allowed value, %s", nveol->linenum,
            nveol->valstr, a_namestr, legal_min_str);
	  strm_fmt(str, nveol->errstr);
	}
	else if(strcmp(legal_max_str, "+inf") &&
          strcmp(legal_max_str, "+INF") &&
	  (i > atoi(legal_max_str))) {
	  sprintf(str, "ERROR, line %d: value, %s, for %s, is \
greater than the maximum allowed value, %s", nveol->linenum,
            nveol->valstr, a_namestr, legal_max_str);
	  strm_fmt(str, nveol->errstr);
	}
	else
	  nveol->ok = TRUE;
      }
      if(nveol->ok)
	((PARM_INT *)a_member)->val = i;
    }
    else if(a_type == MP_FLOAT) {
      n_scanned = sscanf(nveol->valstr, "%f", &a);
      if(n_scanned != 1) {
	sprintf(str, "ERROR, line %d: value, %s, for %s, is not a \
floating-point number", nveol->linenum, nveol->valstr, a_namestr);
	strm_fmt(str, nveol->errstr);
      }
      else if(legal_values_str == (char *)NULL)
	nveol->ok = TRUE;
      else {
	sscanf(legal_values_str, "%s %s", legal_min_str,
          legal_max_str);
	if(strcmp(legal_min_str, "-inf") &&
          strcmp(legal_min_str, "-INF") &&
	  (a < atof(legal_min_str))) {
	  sprintf(str, "ERROR, line %d: value, %s, for %s, is \
smaller than the minimum allowed value, %s", nveol->linenum,
            nveol->valstr, a_namestr, legal_min_str);
	  strm_fmt(str, nveol->errstr);
	}
	else if(strcmp(legal_max_str, "+inf") &&
          strcmp(legal_max_str, "+INF") &&
	  (a > atof(legal_max_str))) {
	  sprintf(str, "ERROR, line %d: value, %s, for %s, is \
greater than the maximum allowed value, %s", nveol->linenum,
            nveol->valstr, a_namestr, legal_max_str);
	  strm_fmt(str, nveol->errstr);
	}
	else
	  nveol->ok = TRUE;
      }
      if(nveol->ok)
	((PARM_FLOAT *)a_member)->val = a;
    }
    else { /* a_type == MP_SWITCH */
      n_legal = sscanf(legal_values_str, "%s %s %s %s %s %s %s %s \
%s %s %s %s %s %s %s %s %s %s %s %s",
        legal_valname_str[0], legal_valcode_str[0],
        legal_valname_str[1], legal_valcode_str[1],
        legal_valname_str[2], legal_valcode_str[2],
        legal_valname_str[3], legal_valcode_str[3],
        legal_valname_str[4], legal_valcode_str[4],
        legal_valname_str[5], legal_valcode_str[5],
        legal_valname_str[6], legal_valcode_str[6],
        legal_valname_str[7], legal_valcode_str[7],
        legal_valname_str[8], legal_valcode_str[8],
        legal_valname_str[9], legal_valcode_str[9]) / 2;
      for(i = 0, value_is_legal = FALSE; i < n_legal; i++)
	if(!strcmp(nveol->valstr, legal_valname_str[i]) ||
	  !strcmp(nveol->valstr, legal_valcode_str[i])) {
	  ((PARM_SWITCH *)a_member)->val =
            (char)atoi(legal_valcode_str[i]);
	  value_is_legal = nveol->ok = TRUE;
	  break;
	}
      if(!value_is_legal) {
	sprintf(str, "ERROR, line %d: illegal value, %s, for %s.  \
Legal values are these strings or code numbers:", nveol->linenum,
          nveol->valstr, a_namestr);
	strm_fmt(str, nveol->errstr);
	for(i = 0; i < n_legal; i++)
	  legal_valname_p[i] = (char *)(legal_valname_str[i]);
	lgl_tbl(n_legal, legal_valname_p, legal_valcode_str,
          nveol->errstr + strlen(nveol->errstr));
      }
    }
  }
  sslp->set = nveol->ok;
  return TRUE;
}
