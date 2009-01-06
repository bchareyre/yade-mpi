/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef DISPLAY_FUNCS_H
#define DISPLAY_FUNCS_H 1

#define TRANS_CAM_FACTOR -0.01

int graph_xm, graph_xM, graph_ym, graph_yM;

void (*rendu)         (void);
void (*disp_current)  (void); 
void (*rendu_sup)     (void);
void (*rendu_fluid)   (void);
void (*affgraph_func) (void);

int nb_section1 = 17;
int nb_section2 = 7;

void disp_time ();
void disp_text (char *text);
void disp_grad_color ();
void disp_func_name ();
void disp_points ();
void disp_boundaries ();
void disp_obstacles ();
void disp_shapes ();
void disp_discrim ();
void disp_colors ();
void disp_sizes ();
void disp_outline ();
void disp_network ();
void disp_cracks ();
void disp_stick_links ();
void disp_networks_pos_neg();
void disp_positive_normal_strength_2d ();
void disp_positive_normal_strength_3d ();
void disp_positive_normal_strength_3d_v2 ();
void disp_negative_normal_strength_2d ();
void disp_negative_normal_strength_3d ();
void disp_force_colorlines ();
void disp_special_forces_3d ();
void disp_special_forces_lines_3d ();
void disp_tangential_strength_2d ();
void disp_tangential_strength_3d ();
void disp_strength_3d ();
void disp_torque_2d();
void disp_Vliq_3d ();
void disp_Fcoh ();
void disp_dep_relatif ();
void disp_shear_vlocy_2d ();
void disp_angular_vlocy_2d ();
void disp_vlocy_magnitude ();
void disp_vlocy_field ();
void disp_dist_ref ();
void disp_coord_number ();
void disp_geo_layers ();

void disp_fluid_ux ();
void disp_fluid_uy ();
void disp_fluid_normeu ();
void disp_fluid_p ();

void graph_position (int xm, int xM, int ym, int yM);
void graph_open ();
void graph_close ();
void graph_trace_buffer (double *xbuf, double *ybuf, int nbp, MGuchar axes, MGuchar opt);

void plot_granulo ();
void plot_hist_ny ();
void plot_hist_fn ();
void plot_distri_TCT (); 
void plot_distri_TCT_3d (); 
void plot_distri_TCT2_3d (); 
void plot_ncloud ();

void plot_fn_vs_ft();

void mgbodyselect (int i, int t);
void mgobstselect (int i, int t);
void mgbodyselectPS (int i, int t);
void mgbodyselectPOV (int i, int t);
void mgbodyselectPS_plein (int i, int t, int coul);
void mgbodyselect_plein (int i, int t);
void mglinkPS (int etat, double larg, int i, int j);
void mgdisk (double r);
void mgsphere (double r);
void mgorient (double r);
void mgdisk_plein (double r);
void mgjonc (double ax1, double ax2);
void mgjonc_plein (double ax1, double ax2);
void mgCylY (int datapos);
void mgPicket (int datapos);
void mgSegmt (double r);
void mgpolyg (int bdy);
void mgpolyg_plein (int bdy);
void mgpolye_plein (int bdy);
void mgmurX (double pos);
void mgmurY (double pos);
void mgmbr2D (int datapos);
void mgFluidCell (int num);
void bodies_number (int i, int t);
void pastille2d (int i, int j, double size);

#endif

