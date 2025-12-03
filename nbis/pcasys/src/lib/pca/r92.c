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
      LIBRARY: PCASYS - Pattern Classification System

      FILE:    R92.C
      AUTHORS: 
      DATE:    1995
      UPDATED: 04/19/2005 by MDG

      ROUTINES:
#cat: r92 - Wegstein's R92 registration algorithm.  Given an array of ridge
#cat:       angles, this routine finds the core, or a core-like feature,
#cat:       of the fingerprint.
#cat: build_k_table - (Internal to r92)
#cat: compute_arch_core - (Internal to r92)
#cat: compute_core - (Internal to r92)
#cat: compute_non_arch_core - (Internal to r92)
#cat: is_it_a_core - (Internal to r92)
#cat: ridge_dir_stats - (Internal to r92)
#cat: score_arch - (Internal to r92)
#cat: set_index - (Internal to r92)

***********************************************************************/

/* Wegstein's R92 registration algorithm.  Given an array of ridge
angles, this routine finds the core, or a core-like feature, of the
fingerprint. */

/* Changed each __( and _( to plain (.  GTC 14 July 1995. */

/* r92.f -- translated by f2c (version of 15 October 1990  19:58:17).
   You must link the resulting object file with the libraries:
	-lF77 -lI77 -lm -lc   (in that order)
*/

#include "f2c.h"


static int build_k_table(real *, int *, int *, int *);
static int compute_arch_core(int *, int *, int *, int *,
                 real *, int *);
static int compute_core(int *, int *, int *, int *, int *,
                 int *, real *, int *, int *, int *,
                 int *, int *, real *, real *, real *, real *,
                 int *, int *, int *, real *);
static int compute_non_arch_core(int *, int *, real *, real *,
                 int *, real *, real *, int *, int *);
static int is_it_a_core(int *, int *, real *, real *, real *, real *,
                 real *, real *, real *, int *, int *, int *,
                 int *, int *, int *, int *, int *,
                 int *);
static int ridge_dir_stats(int *, int *, real *, real *, real *);
static int score_arch(real *, real *, real *, real *, real *, real *,
                 real *, int *);
static int set_index(int *, int *, int *, int *, int *,
                 int *, int *, int *, real *);


/* Common Block Declarations */

struct {
    int k, k_max__, t_core_cand__[32], t_row__[32], t_col__[32];
    real t_sum_bc__[32], t_sum_ad__[32];
    int t_arch_score__[32];
    real t_sum_dir_diff__[32], t_sum_ridge_dir_high__[32], 
	    sum_ridge_dir_low__;
    int ttc[32];
    real t_sum_ridge_dir_low__[32];
    int tsw[32];
    real sum_ridge_dir_high__;
    int ttg[32], tt[32], ttk[32], tts[32];
    real tsa[32];
    int ttf[32], t_not_core_flag__[32], tjl[32];
} k_table__;

#define k_table__1 k_table__

struct {
    real rk1, rk2, rk3, rk5, rk6, rk7, rk13, rk14, rk15, rk19, rk20, rk21, 
	    rk22, rk23, rk24, rk30, rk31, rk32, rk33, rk34;
} param_real__;

#define param_real__1 param_real__

struct {
    int rk4, rk8, rk9, rk10, rk11, rk12, rk16, rk17, rk18, rk25, rk26, 
	    rk27, rk28, rk29, rk35;
} param_int__;

#define param_int__1 param_int__

/***********************************************************************/
/* Removed the following local variables from subroutine r92, */
/* because never used (as warned by f77): n, sum_ridge_dir, tlcp, */
/* t_largest_small_angle_cnt */
int r92(real *angles, int *core_x__, int *core_y__,
         int *r92class)
{
    /* System generated locals */
    int i_1;
    real r_1;

    /* Local variables */
    static int largest_small_angle_cnt__, tick;
    static real rask, ratk;
    static int col_left__;
    static real abs_sec_diff_bot__;
    static int tifx;
    static real ratx, abs_sec_diff_top__;
    static int col_check__;
    static int tickx, row_check__;
    static real raskx;
    static int col_right__;
    static real ratkx;
    static int not_core_flag__;
    static int jc;
    static real hk;
    static int jl, kk;
    static real ud;
    static int ir, kt;
    static real ul;
    static int sr;
    static real ts;
    static int sw;
    static real ut;
    static int cnt_large_for_arch__, sw1, sw2, sw4, row_bottom__, jlf, 
	    row, col, sww, jrt, clx, cnt_angle_large__, ktx;
    static real psl, psr;
    static int row_bot__;
    static real psp, trf;
    static int cnt_angle_small__, row_top__, row_dir_low__;

    /* Parameter adjustments */
    angles -= 33;

    /* Function Body */
    param_real__1.rk1 = (float)111.;
    param_real__1.rk2 = (float)45.;
    param_real__1.rk3 = (float)0.;
    param_int__1.rk4 = 8;
    param_real__1.rk5 = (float)73.;
    param_real__1.rk6 = (float)76.;
    param_real__1.rk7 = (float)176.;
    param_int__1.rk8 = 3;
    param_int__1.rk9 = 29;
    param_int__1.rk10 = 1;
    param_int__1.rk11 = 30;
    param_int__1.rk12 = 4;
    param_real__1.rk13 = (float)50.;
    param_real__1.rk14 = (float)100.;
    param_real__1.rk15 = (float)75.;
    param_int__1.rk16 = 3;
    param_int__1.rk17 = 9;
    param_int__1.rk18 = 3;
    param_real__1.rk19 = (float)465.;
    param_real__1.rk20 = (float)379.;
    param_real__1.rk21 = (float)379.;
    param_real__1.rk22 = (float)210.;
    param_real__1.rk23 = (float)90.;
    param_real__1.rk24 = (float)7.;
    param_int__1.rk25 = 3;
    param_int__1.rk26 = 4;
    param_int__1.rk27 = 3;
    param_int__1.rk28 = 10;
    param_int__1.rk29 = 2;
    param_real__1.rk30 = (float)76.;
    param_real__1.rk31 = (float)45.;
    param_real__1.rk32 = (float)30.;
    param_real__1.rk33 = (float)180.;
    param_real__1.rk34 = (float)0.;
    param_int__1.rk35 = 2;
/* L1: */
    k_table__1.k = 0;
    row = param_int__1.rk10;
    *core_x__ = (float)200.;
    *core_y__ = (float)200.;
    hk = (param_real__1.rk23 - param_real__1.rk31) / (float)34.9128;
    row_bottom__ = param_int__1.rk11;
    row_dir_low__ = 30;
    build_k_table(&angles[33], &row, &row_bottom__, &row_dir_low__);
/* L83: */
    clx = 0;
    sw4 = 0;
    sww = 0;
    trf = (float)0.;
L84:
    if (k_table__1.k <= 0) {
	goto L155;
    }
/* L85: */
    if (k_table__1.t_core_cand__[k_table__1.k - 1] > 0) {
	clx = 1;
	sw1 = 0;
	goto L88;
    }
L86:
    --k_table__1.k;
    goto L84;
L88:
    col_check__ = k_table__1.t_col__[k_table__1.k - 1];
    row_check__ = k_table__1.t_row__[k_table__1.k - 1];
    largest_small_angle_cnt__ = 0;
    cnt_large_for_arch__ = 0;
    cnt_angle_small__ = 0;
    row_top__ = row_check__ - 3;
    col_left__ = col_check__ - 4;
    col_right__ = col_check__ + 4;
    row_bot__ = row_check__ + 2;
    ir = 0;
    sr = 0;
    cnt_angle_large__ = 0;
    sw2 = 0;
    row = row_bot__;
L91:
    if (row > 29) {
	goto L93;
    }
/* L92: */
    if (row <= row_dir_low__) {
	goto L94;
    }
L93:
    --row;
    goto L91;
L94:
    col = col_right__;
    sw = 0;
    psp = (float)99.;
L95:
    if (col < 29) {
	goto L128;
    }
L96:
    if (col > col_left__) {
/* L97: */
	if (col > 1) {
/* L98: */
	    --col;
	    goto L95;
	}
    }
/* L99: */
    if (sw2 > 0 || row > row_check__) {
	goto L101;
    }
    if (ir > 0) {
	abs_sec_diff_bot__ = sr * (float)10. / ir;
	ir = 0;
	sr = 0;
    }
    sw2 = 1;
L101:
    if (row <= 1) {
	is_it_a_core(&ir, &sr, &abs_sec_diff_top__, &abs_sec_diff_bot__, &
		ratk, &rask, &ratkx, &raskx, &ratx, &tickx, &
		cnt_angle_large__, &largest_small_angle_cnt__, &
		cnt_large_for_arch__, &sw1, &sw4, &tifx, &ktx, &
		not_core_flag__);
	goto L126;
    }
/* L102: */
    if (cnt_angle_small__ > largest_small_angle_cnt__) {
	largest_small_angle_cnt__ = cnt_angle_small__;
    }
/* L104: */
    if (row <= row_top__) {
	is_it_a_core(&ir, &sr, &abs_sec_diff_top__, &abs_sec_diff_bot__, &
		ratk, &rask, &ratkx, &raskx, &ratx, &tickx, &
		cnt_angle_large__, &largest_small_angle_cnt__, &
		cnt_large_for_arch__, &sw1, &sw4, &tifx, &ktx, &
		not_core_flag__);
	goto L126;
    }
/* L106: */
    cnt_angle_small__ = 0;
    --row;
    goto L94;
L126:
    if (sw1 > 0) {
/* L173: */
	if (not_core_flag__ > 0) {
	    goto L176;
	} else {
	    goto L174;
	}
    }
/* L127: */
    if (not_core_flag__ <= 0) {
	goto L197;
    } else {
	goto L86;
    }
L128:
    if (angles[row + ((col + 1) << 5)] < (float)99.) {
	psp = angles[row + ((col + 1) << 5)];
    }
    if (psp >= (float)99.) {
	goto L96;
    }
/* L129: */
    if (angles[row + (col << 5)] >= (float)99.) {
	goto L96;
    }
/* L130: */
    if (sw2 <= 0) {
	goto L144;
    }
/* L131: */
    if (col <= col_left__) {
	goto L144;
    }
/* L132: */
    if ((r_1 = angles[row + (col << 5)], dabs(r_1)) >= param_real__1.rk30) {
	++cnt_angle_large__;
    }
/* L134: */
    if (col > col_check__) {
	goto L138;
    }
/* L135: */
    if (angles[row + (col << 5)] < -(doublereal)param_real__1.rk32) {
	goto L141;
    }
/* L136: */
    psl = hk * (col_check__ - col) * (float)11.6376 + param_real__1.rk31;
/* L137: */
    if (angles[row + (col << 5)] > psl) {
	goto L141;
    }
    goto L142;
L138:
    if (angles[row + (col << 5)] > param_real__1.rk32) {
	goto L141;
    }
/* L139: */
    psr = hk * (col_check__ + 1 - col) * (float)11.6376 - param_real__1.rk31;
/* L140: */
    if (angles[row + (col << 5)] > psr) {
	goto L142;
    }
L141:
    ++cnt_large_for_arch__;
L142:
    if ((r_1 = angles[row + (col << 5)], dabs(r_1)) > param_real__1.rk24) {
	goto L144;
    }
    ++cnt_angle_small__;
L144:
    ut = psp - angles[row + (col << 5)];
/* L145: */
    if (ut <= (float)90.) {
	if (ut < (float)-90.) {
	    ut += (float)180.;
	}
    } else {
	ut += (float)-180.;
    }
/* L149: */
    if (sw > 0) {
	goto L151;
    }
    ul = ut;
    sw = 1;
    goto L96;
L151:
    ud = (r_1 = ut - ul, dabs(r_1));
    ul = ut;
/* L152: */
    if (ud > (float)90.) {
	ud = (float)180. - ud;
    }
/* L154: */
    ++ir;
    sr += ud;
    goto L96;
L155:
    if (k_table__1.k_max__ < 2) {
	goto L196;
    }
    trf = (float)0.;
    k_table__1.k = 2;
L157:
    if ((i_1 = k_table__1.t_col__[k_table__1.k - 2] - k_table__1.t_col__[
	    k_table__1.k - 1], abs(i_1)) > 1) {
	goto L159;
    }
/* L158: */
    if (k_table__1.t_row__[k_table__1.k - 1] - k_table__1.t_row__[
	    k_table__1.k - 2] <= 1) {
	goto L162;
    }
L159:
    if (k_table__1.k >= k_table__1.k_max__) {
	goto L176;
    }
/* L160: */
    if (k_table__1.t_row__[k_table__1.k] - k_table__1.t_row__[k_table__1.k - 
	    1] > 1) {
	goto L176;
    }
/* L161: */
    if ((i_1 = k_table__1.t_col__[k_table__1.k] - k_table__1.t_col__[
	    k_table__1.k - 1], abs(i_1)) > 1) {
	goto L176;
    }
L162:
    if (k_table__1.t_sum_ridge_dir_low__[k_table__1.k - 1] < 
	    param_real__1.rk13) {
	goto L176;
    }
/* L163: */
    if (k_table__1.t_sum_ridge_dir_high__[k_table__1.k - 1] < 
	    param_real__1.rk14) {
	goto L176;
    }
    col = k_table__1.t_col__[k_table__1.k - 1];
    jlf = col - param_int__1.rk12;
    jrt = col + 1 + param_int__1.rk12;
/* L165: */
    if (jlf < 1) {
	goto L176;
    }
/* L166: */
    if (jrt > 29) {
	goto L176;
    }
    col = jlf;
    row = k_table__1.t_row__[k_table__1.k - 1];
    ts = (float)0.;
L168:
    if (angles[row + (col << 5)] >= (float)99.) {
	goto L176;
    }
    ts += (r_1 = angles[row + (col << 5)], dabs(r_1));
/* L170: */
    if (col < jrt) {
	++col;
	goto L168;
    }
/* L172: */
    k_table__1.tsa[k_table__1.k - 1] = ts;
    sw1 = 1;
    if (k_table__1.t_not_core_flag__[k_table__1.k - 1] > 0) {
	goto L176;
    }
    if (k_table__1.t_core_cand__[k_table__1.k - 1] <= 0) {
	goto L88;
    }
L174:
    if (ts >= trf) {
	trf = ts;
	kt = k_table__1.k;
    }
L176:
    if (k_table__1.k < k_table__1.k_max__) {
/* L177: */
	++k_table__1.k;
	goto L157;
    }
/* L178: */
    if (trf == (float)0.) {
	goto L196;
    }
/* L179: */
    set_index(&col_left__, &col_right__, &row_bot__, &row, &jc, &
	    row_dir_low__, &col, &kt, &angles[33]);
/* L194: */
    if (jc <= param_int__1.rk18) {
	*r92class = 2;
	col_check__ = k_table__1.t_col__[kt - 1];
	row_check__ = k_table__1.t_row__[kt - 1];
	goto L206;
    }
L196:
    *r92class = 3;
    kt = 0;
    goto L206;
L197:
    if (k_table__1.k + 2 > k_table__1.k_max__) {
	goto L205;
    }
/* L198: */
    if ((i_1 = k_table__1.t_col__[k_table__1.k] - k_table__1.t_col__[
	    k_table__1.k - 1], abs(i_1)) > 1) {
	goto L205;
    }
/* L199: */
    if ((i_1 = k_table__1.t_col__[k_table__1.k + 1] - k_table__1.t_col__[
	    k_table__1.k], abs(i_1)) > 1) {
	goto L205;
    }
/* L200: */
    if (k_table__1.t_row__[k_table__1.k] - k_table__1.t_row__[k_table__1.k - 
	    1] > 1) {
	goto L205;
    }
/* L201: */
    if (k_table__1.t_row__[k_table__1.k + 1] - k_table__1.t_row__[
	    k_table__1.k] > 1) {
	goto L205;
    }
/* L202: */
    if (k_table__1.t_sum_ad__[k_table__1.k] >= param_real__1.rk33) {
	goto L205;
    }
/* L203: */
    if (k_table__1.t_sum_ad__[k_table__1.k + 1] >= param_real__1.rk33) {
	goto L205;
    }
/* L204: */
    k_table__1.k = 0;
    goto L155;
L205:
    *r92class = 1;
    kt = k_table__1.k;
    trf = k_table__1.t_sum_dir_diff__[k_table__1.k - 1];
L206:
    compute_core(core_x__, core_y__, r92class, &clx, &ktx, &kt, &ratk, &
	    col_check__, &row_check__, &row, &col, &jl, &ratx, &rask, &ratkx, 
	    &raskx, &tick, &tickx, &tifx, &angles[33]);
/* L233: */
    if (kt <= 0) {
	goto L300;
    }
/* L234: */
    kk = kt;
L235:
    if (kk <= 1) {
	goto L300;
    }
/* L236: */
    if (k_table__1.t_row__[kt - 1] - k_table__1.t_row__[kk - 1] > 
	    param_int__1.rk35) {
	goto L300;
    }
/* L237: */
    if (k_table__1.t_row__[kk - 1] - k_table__1.t_row__[kk - 2] > 1) {
	goto L300;
    }
/* L238: */
    if ((i_1 = k_table__1.t_col__[kk - 1] - k_table__1.t_col__[kk - 2], abs(
	    i_1)) > 1) {
	ratk = (float)600.;
	goto L300;
    } else {
	--kk;
	goto L235;
    }
L300:
    return 0;
} /* r92_ */

/***********************************************************************/
static int build_k_table(real *angles, int *row, int *row_bottom__,
         int *row_dir_low__)
{
    /* System generated locals */
    int i_1;

    /* Local variables */
    static int sum_dir_diff__, ib, jb, ii, jj, jl, sw;
    static real sum_bc__, sum_ad__;
    static int arch_score__;
    static int sw1;
    static real angle_a__, angle_b__, angle_c__;
    static int jlc;
    static real angle_d__;
    static int col;
    static real angle_m__, angle_n__;

    /* Parameter adjustments */
    angles -= 33;

    /* Function Body */
L2:
    if (*row > *row_bottom__) {
	goto L4;
    }
/* L3: */
    if (*row < *row_dir_low__) {
	goto L5;
    }
L4:
    k_table__1.k_max__ = k_table__1.k;
    goto L83;
L5:
    ++(*row);
    col = param_int__1.rk8;
L7:
    if (angles[*row + (col << 5)] >= (float)0.) {
	goto L10;
    }
L8:
    if (col >= param_int__1.rk9) {
	goto L2;
    } else {
	++col;
	goto L7;
    }
L10:
    angle_m__ = angles[*row + ((col - 2) << 5)];
    angle_a__ = angles[*row + ((col - 1) << 5)];
    angle_b__ = angles[*row + (col << 5)];
    angle_c__ = angles[*row + ((col + 1) << 5)];
    angle_d__ = angles[*row + ((col + 2) << 5)];
    angle_n__ = angles[*row + ((col + 3) << 5)];
    ib = *row;
    jb = col;
    if (angle_b__ >= (float)99. || angle_c__ >= (float)0. || angle_a__ >= (
	    float)99. || angle_d__ >= (float)99. || angle_n__ >= (float)99. ||
	     angle_m__ >= (float)99.) {
	goto L8;
    }
    sum_bc__ = dabs(angle_b__) + dabs(angle_c__);
    if (sum_bc__ > param_real__1.rk1) {
	goto L8;
    }
    sum_ad__ = sum_bc__ + dabs(angle_a__) + dabs(angle_d__);
    if (sum_ad__ < param_real__1.rk2) {
	goto L8;
    }
    score_arch(&angle_m__, &angle_a__, &angle_b__, &angle_c__, &angle_d__, &
	    angle_n__, &param_real__1.rk3, &arch_score__);
/* L42: */
    if (arch_score__ < param_int__1.rk4) {
	goto L8;
    }
/* L43: */
    sw = 0;
    jlc = 0;
/* L44: */
    if (*row - 1 <= 0) {
	goto L8;
    }
/* L45: */
    ii = *row - 1;
    sw1 = 0;
L46:
    jj = col - 1;
L47:
    if (angles[ii + (jj << 5)] < (float)0.) {
	goto L53;
    }
/* L48: */
    if (angles[ii + (jj << 5)] >= (float)99.) {
	goto L52;
    }
/* L49: */
    if (angles[ii + ((jj + 1) << 5)] < (float)0.) {
/* L50: */
	++sw;
	if (sw1 <= 0) {
	    goto L56;
	}
	jlc = jj;
	goto L59;
    } else {
/* L51: */
	if (angles[ii + ((jj + 1) << 5)] < (float)99.) {
	    goto L53;
	}
    }
L52:
    if (sw1 > 0) {
	jl = 0;
	goto L59;
    }
L53:
    if (jj < col + 1) {
	++jj;
	goto L47;
    }
/* L55: */
    if (sw1 > 0) {
	goto L58;
    }
L56:
    if (*row + 1 <= 29) {
	ii = *row + 1;
	sw1 = 1;
	jl = 1;
	goto L46;
    } else {
	goto L8;
    }
L58:
    if (sw <= 0) {
	goto L8;
    }
L59:
    ++k_table__1.k;
    k_table__1.t_core_cand__[k_table__1.k - 1] = 0;
    k_table__1.t_row__[k_table__1.k - 1] = *row;
    k_table__1.t_col__[k_table__1.k - 1] = col;
    k_table__1.t_sum_bc__[k_table__1.k - 1] = sum_bc__;
    k_table__1.t_sum_ad__[k_table__1.k - 1] = sum_ad__;
    k_table__1.t_arch_score__[k_table__1.k - 1] = arch_score__;
    k_table__1.t_sum_dir_diff__[k_table__1.k - 1] = (float)0.;
    k_table__1.t_sum_ridge_dir_high__[k_table__1.k - 1] = (float)0.;
    k_table__1.t_sum_ridge_dir_low__[k_table__1.k - 1] = (float)0.;
    k_table__1.tsw[k_table__1.k - 1] = sw;
    k_table__1.sum_ridge_dir_high__ = (float)0.;
    k_table__1.sum_ridge_dir_low__ = (float)360.;
    k_table__1.ttc[k_table__1.k - 1] = 0;
    k_table__1.ttg[k_table__1.k - 1] = 0;
    k_table__1.tt[k_table__1.k - 1] = 0;
    k_table__1.ttk[k_table__1.k - 1] = (float)0.;
    k_table__1.tts[k_table__1.k - 1] = (float)0.;
    k_table__1.tsa[k_table__1.k - 1] = (float)0.;
    k_table__1.tjl[k_table__1.k - 1] = jlc;
    k_table__1.t_not_core_flag__[k_table__1.k - 1] = 0;
    if (jl <= 0) {
	goto L8;
    }
    if (*row_bottom__ != param_int__1.rk11 || k_table__1.k < 2) {
	goto L60;
    }
    if (k_table__1.t_row__[k_table__1.k - 1] - k_table__1.t_row__[
	    k_table__1.k - 2] != 1 || (i_1 = k_table__1.t_col__[k_table__1.k 
	    - 1] - k_table__1.t_col__[k_table__1.k - 2], abs(i_1)) > 1) {
	goto L60;
    }
    *row_bottom__ = k_table__1.t_row__[k_table__1.k - 1] + param_int__1.rk28;
    if (*row_bottom__ > param_int__1.rk11) {
	*row_bottom__ = param_int__1.rk11;
    }
L60:
    ridge_dir_stats(row, &col, &angles[33], &
	    k_table__1.sum_ridge_dir_high__, &k_table__1.sum_ridge_dir_low__);

/* L72: */
    if (k_table__1.sum_ridge_dir_high__ <= (float)0.) {
	goto L8;
    }
/* L73: */
    sum_dir_diff__ = k_table__1.sum_ridge_dir_high__ - sum_ad__;
/* L74: */
    if (k_table__1.sum_ridge_dir_low__ >= param_real__1.rk6) {
/* L75: */
	if ((real) sum_dir_diff__ >= param_real__1.rk5) {
/* L76: */
	    if (k_table__1.sum_ridge_dir_high__ >= param_real__1.rk7) {
		k_table__1.t_core_cand__[k_table__1.k - 1] = 1;
	    }
	}
    }
/* L78: */
    k_table__1.t_sum_dir_diff__[k_table__1.k - 1] = (real) sum_dir_diff__;
    k_table__1.t_sum_ridge_dir_high__[k_table__1.k - 1] = 
	    k_table__1.sum_ridge_dir_high__;
    k_table__1.t_sum_ridge_dir_low__[k_table__1.k - 1] = 
	    k_table__1.sum_ridge_dir_low__;
    goto L8;
L83:
    return 0;
} /* build_k_table__ */

/***********************************************************************/
static int compute_arch_core(int *core_x__, int *core_y__, int *row,
         int *kt, real *xx1, int *row_check__)
{
    static real tdnm, tnum;

/* L214: */
    if (*kt >= k_table__1.k_max__) {
	tnum = (float)0.;
	tdnm = (float)0.;
    } else {
	tnum = (*row_check__ * (float)16. + (float)8.5) * k_table__1.tsa[*kt];

	tdnm = k_table__1.tsa[*kt];
    }
/* L216: */
    tnum = tnum + ((*row - 1) * (float)16. + (float)8.5) * k_table__1.tsa[*kt 
	    - 1] + ((*row - 2) * (float)16. + (float)8.5) * k_table__1.tsa[*
	    kt - 2];
    tdnm = tdnm + k_table__1.tsa[*kt - 1] + k_table__1.tsa[*kt - 2];
    *core_y__ = tnum / tdnm;
    *core_x__ = *xx1;
    return 0;
} /* compute_arch_core__ */

/***********************************************************************/
static int compute_core(int *core_x__, int *core_y__, int *r92class,
         int *clx, int *ktx, int *kt, real *ratk,
         int *col_check__, int *row_check__, int *row,
         int *col, int *jl, real *ratx, real *rask, real *ratkx, 
	 real *raskx, int *tick, int *tickx, int *tifx,
         real *angles)
{
    static real tlcp, abs_sec_diff_top__;
    static real ds;
    static real t_largest_small_angle_cnt__, xx1, dsp1;

    /* Parameter adjustments */
    angles -= 33;

    /* Function Body */
    if (*r92class <= 2) {
	goto L210;
    }
/* L207: */
    if (*clx > 0) {
	*kt = *ktx;
	*col_check__ = k_table__1.t_col__[*kt - 1];
	*row_check__ = k_table__1.t_row__[*kt - 1];
	goto L210;
    }
/* L208: */
    *core_x__ = (float)200.;
    *core_y__ = (float)200.;
    *ratk = (float)700.;
    *kt = k_table__1.k_max__;
L209:
    if (*kt <= 0) {
	goto L229;
    }
    if (k_table__1.t_sum_dir_diff__[*kt - 1] > (float)36.) {
	*col_check__ = k_table__1.t_col__[*kt - 1];
	*row_check__ = k_table__1.t_row__[*kt - 1];
    } else {
	--(*kt);
	goto L209;
    }
L210:
    *row = *row_check__;
    *col = *col_check__;
    dsp1 = angles[*row + (*col << 5)] - angles[*row + ((*col + 1) << 5)];
    ds = (float)16.;
    xx1 = ds * angles[*row + (*col << 5)] / dsp1 + (*col - 1) * (float)16. + (
	    float)8.5;
/* L213: */
    if (*r92class == 2) {
	compute_arch_core(core_x__, core_y__, row, kt, &xx1, row_check__);
    } else {
	compute_non_arch_core(core_x__, core_y__, &dsp1, &ds, kt, &xx1, &
		angles[33], row_check__, row);
    }
L229:
    if (*kt <= 0) {
/* this if statement was added to keep this thing from going into psam
 */
	*core_x__ = (float)256.;
	*core_y__ = (float)256.;
	*r92class = 3;
	goto L230;
    }
    tlcp = (real) k_table__1.t_arch_score__[*kt - 1];
L230:
    if (*r92class > 2) {
/* L231: */
	if (*clx > 0) {
	    abs_sec_diff_top__ = *ratx;
	    *ratk = *ratkx;
	    *rask = *raskx;
	    *tick = *tickx;
	    t_largest_small_angle_cnt__ = (real) (*tifx);
	}
    }
/* L233: */
    return 0;
} /* compute_core__ */

/***********************************************************************/
static int compute_non_arch_core(int *core_x__, int *core_y__, real *dsp1,
         real *ds, int *kt, real *xx1, real *angles, int *row_check__,
         int *row)
{
    static real dh;
    static int jl;
    static real dh1, dh2, xx2, dsp2;

    /* Parameter adjustments */
    angles -= 33;

    /* Function Body */
/* L217: */
    if (*dsp1 < (float)90.) {
	dh1 = *dsp1 * *ds / (float)90.;
    } else {
	dh1 = *ds;
    }
/* L220: */
    jl = k_table__1.tjl[*kt - 1];
/* L221: */
    if (jl <= 0) {
	xx2 = *xx1;
	dh2 = *ds;
    } else {
	dsp2 = angles[*row + 1 + (jl << 5)] - angles[*row + 1 +
                                                     ((jl + 1) << 5)];
	xx2 = *ds * angles[*row + 1 + (jl << 5)] / dsp2 + (jl - 1) * (float)
		16. + (float)8.5;
	if (dsp2 > (float)90.) {
	    dh2 = (dsp2 - (float)90.) * *ds / (float)90.;
	} else {
	    dh2 = (float)0.;
	}
    }
/* L227: */
    dh = (dh1 + dh2) / (float)2.;
    *core_x__ = dh * (*xx1 - xx2) / *ds + xx2;
    *core_y__ = (*row << 4) - dh;
    return 0;
} /* compute_non_arch_core__ */

/***********************************************************************/
static int is_it_a_core(int *ir, int *sr, real *diff_top__, real *diff_bot__,
         real *ratk, real *rask, real *ratkx, real *raskx, real *ratx,
         int *tickx, int *cnt_angle_large__,
         int *largest_small_angle_cnt__, int *cnt_large_for_arch__,
         int *sw1, int *sw4, int *tifx, int *ktx, 
	 int *not_core_flag__)
{
    static int tick;
    static real abs_sec_diff_top__;
    static int t_largest_small_angle_cnt__;

/* L109: */
    if (*ir == 0) {
	*diff_top__ = (float)999.;
    } else {
	*diff_top__ = *sr * (float)10. / *ir;
    }
/* L112: */
    *ratk = *diff_top__;
    *rask = *diff_bot__;
    *diff_top__ += *diff_bot__;
    t_largest_small_angle_cnt__ = *largest_small_angle_cnt__;
    tick = *cnt_angle_large__;
    *not_core_flag__ = 0;
    k_table__1.ttc[k_table__1.k - 1] = tick;
    k_table__1.ttf[k_table__1.k - 1] = t_largest_small_angle_cnt__;
    k_table__1.ttg[k_table__1.k - 1] = *cnt_large_for_arch__;
    k_table__1.tt[k_table__1.k - 1] = *diff_top__;
    k_table__1.ttk[k_table__1.k - 1] = *ratk;
    k_table__1.tts[k_table__1.k - 1] = *rask;
/* L114: */
    if (*sw1 <= 0) {
	if (*sw4 <= 0) {
	    *ratkx = *ratk;
	    *raskx = *rask;
	    *ratx = *diff_top__;
	    *tifx = t_largest_small_angle_cnt__;
	    *tickx = tick;
	    *sw4 = 1;
	    *ktx = k_table__1.k;
	}
    }
/* L117: */
    if (tick >= param_int__1.rk29) {
	if (abs_sec_diff_top__ > param_real__1.rk19) {
	    if (*ratk > param_real__1.rk20) {
		if (*cnt_large_for_arch__ >= param_int__1.rk25) {
		    if (*rask > param_real__1.rk21) {
			if (t_largest_small_angle_cnt__ >= param_int__1.rk26) 
				{
			    if (t_largest_small_angle_cnt__ >= 
				    param_int__1.rk27) {
				if (*rask > param_real__1.rk22) {
/* L125: */
				    *not_core_flag__ = 1;
				    k_table__1.t_not_core_flag__[k_table__1.k 
					    - 1] = 1;
				}
			    }
			}
		    }
		}
	    }
	}
    }
/* L126: */
    return 0;
} /* is_it_a_core__ */

/***********************************************************************/
static int ridge_dir_stats(int *row, int *col, real *angles, real *high,
         real *low)
{
    /* System generated locals */
    real r_1;

    /* Local variables */
    static int m, n;
    static real ridge_dir__;
    static int n1, n2;

    /* Parameter adjustments */
    angles -= 33;

    /* Function Body */
    m = *row + 1;
    n1 = *col - 3;
    n2 = *col;
L60:
    if (n1 < 1) {
	++n1;
	++n2;
	goto L60;
    }
L61:
    n = n1;
    ridge_dir__ = (float)0.;
L62:
    if (angles[m + (n << 5)] >= (float)99.) {
	goto L70;
    }
/* L63: */
    ridge_dir__ += (r_1 = angles[m + (n << 5)], dabs(r_1));
/* L64: */
    if (n < n2) {
	++n;
	goto L62;
    }
/* L66: */
    if (ridge_dir__ < *low) {
	*low = ridge_dir__;
    }
/* L68: */
    if (ridge_dir__ > *high) {
	*high = ridge_dir__;
    }
L70:
    if (n1 >= *col + 1) {
	goto L72;
    }
    if (n2 >= 29) {
	goto L72;
    }
/* L71: */
    ++n1;
    ++n2;
    goto L61;
L72:
    return 0;
} /* ridge_dir_stats__ */

/***********************************************************************/
/* Changed score_arch from function to subroutine. */
static int score_arch(real *angle_m__, real *angle_a__, real *angle_b__,
         real *angle_c__, real *angle_d__, real *angle_n__, real *rk3,
         int *arch_score__)
{
    *arch_score__ = 0;
    if (*angle_a__ > (float)0.) {
	++(*arch_score__);
	if (*angle_m__ - *angle_a__ > *rk3) {
	    ++(*arch_score__);
	}
    }
/* L26: */
    if (*angle_m__ > (float)0.) {
	++(*arch_score__);
    }
/* L28: */
    if (*angle_m__ - *angle_b__ > *rk3) {
	++(*arch_score__);
    }
/* L30: */
    if (*angle_a__ - *angle_b__ > *rk3) {
	++(*arch_score__);
    }
/* L32: */
    if (*angle_d__ < (float)0.) {
/* L33: */
	++(*arch_score__);
/* L34: */
	if (*angle_d__ - *angle_n__ > *rk3) {
	    ++(*arch_score__);
	}
    }
/* L36: */
    if (*angle_n__ < (float)0.) {
	++(*arch_score__);
    }
/* L38: */
    if (*angle_c__ - *angle_n__ > *rk3) {
	++(*arch_score__);
    }
/* L40: */
    if (*angle_c__ - *angle_d__ > *rk3) {
	++(*arch_score__);
    }
    return 0;
} /* score_arch__ */

/***********************************************************************/
static int set_index(int *col_left__, int *col_right__, int *row_bot__,
         int *row, int *jc, int *row_dir_low__, int *col,
         int *kt, real *angles)
{
    /* System generated locals */
    real r_1;

    /* Parameter adjustments */
    angles -= 33;

    /* Function Body */
/* L179: */
    *col_left__ = k_table__1.t_col__[*kt - 1] - param_int__1.rk16 + 1;
    *col_right__ = k_table__1.t_col__[*kt - 1] + param_int__1.rk16;
    *row_bot__ = k_table__1.t_row__[*kt - 1] + param_int__1.rk17;
    *row = k_table__1.t_row__[*kt - 1] + 1;
    *jc = 0;
/* L180: */
    if (*row_bot__ > *row_dir_low__) {
	*row_bot__ = *row_dir_low__;
    }
/* L182: */
    if (*col_left__ < 1) {
	*col_left__ = 1;
    }
/* L184: */
    if (*col_right__ > 29) {
	*col_right__ = 29;
    }
L186:
    *col = *col_left__;
L187:
    if ((r_1 = angles[*row + (*col << 5)], dabs(r_1)) < param_real__1.rk15) {
	goto L190;
    }
/* L188: */
    if (angles[*row + (*col << 5)] < (float)99.) {
	++(*jc);
    }
L190:
    if (*col < *col_right__) {
	++(*col);
	goto L187;
    }
/* L192: */
    if (*row < *row_bot__) {
	++(*row);
	goto L186;
    }
/* L194: */
    return 0;
} /* set_index__ */

