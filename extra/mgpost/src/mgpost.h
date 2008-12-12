/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#define MG_TRUE   1
#define MG_FALSE  0

#ifdef _MACOSX
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
#endif

#include "gl2ps/gl2ps.h"

#include <math.h>
#include "nrutil.h"
#include "post.h"
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>

#ifdef _LIBXML2
# include <libxml/tree.h>
#else
# include <gnome-xml/tree.h>
#endif

#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MGP_VERSION     "0.5.3" 
// to remove...

#define DISTANCE_INIT   15.0f
#define DISTANCE_MAX    300.0f
#define THETA_INIT      -30
#define PHI_INIT        300

#define MG_PI        3.14159265358979323846
#define MG_2PI       6.28318530717958647692
#define MG_DEG2RAD   0.01745329251994329576
#define MG_RAD2DEG   57.2957795130823208772

#ifndef M_PI
# define M_PI       3.14159265358979323846
#endif

#define GL2PS_TERMINAL 3
#define POV_TERMINAL   2
#define PS_TERMINAL    1
#define GL_TERMINAL    0

#ifndef F_OK
# define F_OK 0
#endif

#ifndef _GNU_SOURCE
extern char   **environ;
#endif

typedef unsigned char MGuchar;
typedef unsigned int  MGuint;

#include "colors.h"
#include "section.h"
#include "display_funcs.h"
#include "dialog.h"
#include "tapio-K_export.h"

char mgpost_editor[256];
char mgpost_home[256];
char mgpost_string[256];
char num_file_format[20];

static const char delim[] = " ;,()=\t\n";

MGuchar   affFuncname    = MG_TRUE;
MGuchar   afficheRepere  = MG_FALSE;
MGuchar   middle_rep     = MG_TRUE;
MGuchar   sectionActive  = MG_FALSE;
MGuchar   sauve_anim     = MG_FALSE;
MGuchar   jy_mode        = MG_FALSE; /* a supprimer ? */
MGuchar   cin_mode       = MG_FALSE;
MGuchar   his_mode       = MG_FALSE;
MGuchar   mgp_mode       = MG_TRUE;
MGuchar   mode2D         = MG_FALSE;
MGuchar   multifiles     = MG_FALSE;
MGuchar   play_again     = MG_FALSE;
MGuchar   fgziped        = MG_FALSE;
MGuchar   orient         = MG_FALSE;
MGuchar   bodies_numbers = MG_FALSE;
MGuchar   sep_networks   = MG_FALSE;
MGuchar   shade_polye    = MG_FALSE;
MGuchar   modDirCon      = 1;
MGuchar   modeTrace      = MG_FALSE;

MGuchar   disp_only_text   = MG_FALSE;
MGuchar   afftime          = MG_TRUE;
MGuchar   affgradcolor     = MG_FALSE;
MGuchar   affgradlinkcolor = MG_FALSE;
MGuchar   affgraphic       = MG_FALSE;
MGuchar   dynamic_scale    = MG_TRUE;
MGuchar   mgterminal       = GL_TERMINAL;

int             repere = 0;
int             stickpoint = 1;

char            b_gauche = 0, b_middle = 0;
int             theta = THETA_INIT, phi = PHI_INIT;
int             xprec, yprec;
float           distance = DISTANCE_INIT;
float           cut_distance = 0.1;
float           inc_cut = 0.0;
static int      W = 600, H = 600;
static int      Xviewp = 0, Yviewp = 0;
int             modifiers;
double          myfactor = 0.5;

const char     *datatitre = "no title";
char            pstfilename[256];
char            datafilename[256];
char            datanewfilename[256];
const char     *numfile[10];
int             nfile = 1;
char            funcname[100] = "Bodies position";
char            supfuncname[100] = "None           ";
char            quick_text[31] = "";

void            no_warranty();
int             compare (const void *v1, const void *v2);
void            mgp_delay(int t);
void            mgpost_init(int argc, char **argv);
void            mgp_buildmenu();
double        **AllocDouble(int dim1, int dim2);
int           **AllocInt(int dim1, int dim2);
double          mgMax(double a, double b);
double          mgMin(double a, double b);
double          findMax(double **T, int n, int state);
double          findMin(double **T, int n, int state);
double          findAbsMax(double **T, int n, int state);
double          findAbsMin(double **T, int n, int state);
double          findNegativeMax(double **T, int n, int state);
double          findPositiveMax(double **T, int n, int state);
void 
findBoundaries(int state, double *xminB, double *yminB, double *zminB,
	       double *xmaxB, double *ymaxB, double *zmaxB);
void            nullifyCumulatedTabs();
void            mgpallocs();
void            mgpfree();
void            make_mgpalloc_file();
void            make_mgconf_file();
void            param_init();
void            save_mgpview();
void            load_mgpview();
couleur         select_color(const char *col);
void            affichage();
void            traitmenu(int value);
void            traitsubmenu1(int value);
void            traitsubmenu2(int value);
void            traitsubmenu4(int value);
void            traitsubmenuFluid (int value);
void            clavier(MGuchar touche, int x, int y);
void            specialKey(int touche, int x, int y);
void            souris(int bouton, int etat, int x, int y);
void            mouvement(int x, int y);
void            redim(int l, int h);
void            creatDisplayLists();
void            affiche_aide();
void            affiche_infos();
void            info_gap();
void            display_infos_on_body(int i);
void            tool_surf_vol();
void            tool_output_positions();
void            tool_anisotropy(int cetat, int save, FILE * out);
void            charger_CINfile();
void            charger_HISfile();
void            charger_geometrie();
void            read_body_data(xmlNodePtr bdy_node, int bdy, int t);
void            centrer_repere();
void            precalculs();
void            calcul_adim();
void            count_contacts();
void            play();
void            play_filetofile();
void            next_state();
void            previous_state();
int             writetiff(char *filename, char *description, int x, int y, int width, int height, int compression);
void            exportCIN();
void            export_Rxy();
void            export_SSPX();
MGuchar         pres_du_plan(int i, double dist);

void            compil_pstcom();
void            pst_evol(MGuchar iscompressed);

void            processHits(GLint hits, GLuint buffer[]);

void            processDialogF1();
void            processDialogF2();
void            processDialogF3();
void            processDialogF4();
void            processDialogF5();

/* fichier de command pour post-traitement */
typedef struct 
{
	char            name[50];
	char            file[50];
	int             nbparam;
	double          param[10];
} PSTCOM;

PSTCOM         *pstcom;
int             nbcom = 0;

int             screenshot_counter = 0;

FILE           *psfile;
FILE           *povfile;
FILE           *outfile;
double          xminB, yminB, zminB, xmaxB, ymaxB, zmaxB;
double          maxmax, Lmax;
double          psZoom, psdim = 20.0;

GLfloat         L0pos[]    = {-6.0f, -6.0f, -6.0f};
GLfloat         whitedif[] = {1.0f, 1.0f, 1.0f};
GLfloat         L1pos[]    = {6.0f, 6.0f, 6.0f};

GLfloat         alpha_color = 1.0f;

double          grillX = 1.0e-2, grillY = 1.0e-2, grillZ = 1.0e-2;

int             nbbodies = 2000, nbtimes = 200, nbcontacts = 6, nbdatas = 2;
int             datapos = 0;

GLenum          renderMode = GL_RENDER;
#define BUFSIZE 1024
int             selectedBody = -1;

#define NB_SUBDIV_SPHERE 12
#define NB_SUBDIV_SPHERE_2 8
GLint           nb_subdiv_sphere = NB_SUBDIV_SPHERE;
GLint           nb_subdiv_sphere_2 = NB_SUBDIV_SPHERE_2;

int           **nbneighbors;
int           **neighbor;

int            *datadistrib;
MGuchar        *dataqty;
double         *datas;

MGuchar  *bdyty;

#define MGP_DISKx 0
#define MGP_SPHER 1
#define MGP_POLYG 2
#define MGP_SEGMT 3
#define MGP_JONCx 4
#define MGP_POLYE 5
#define MGP_PICKT 10
#define MGP_MURX0 100
#define MGP_MURX1 101
#define MGP_MURY0 102
#define MGP_MURY1 103
#define MGP_MURZ0 104
#define MGP_MURZ1 105
#define MGP_MBR2D 200
#define MGP_CYLYx 250

MGuchar  *bdyclass;

#define MGP_GRAIN 0
#define MGP_OBSTACLE 1

double         *mgp_time;

MGuchar  *discrim; /* a suprimer lorsque color sera mis en place */
MGuchar       *color;
double        **x, **y, **z;
double        *x_0, *y_0, *z_0;
double        **rot;
double        **radius;
double        **Crot;
double        **Fn, **Ft, **Fs;
double        **Vn, **Vt;
double        **vx, **vy, **vz;
double        **vrot;
double        **Vliq;
int           **statut;

#define MGP_STT_NONE  0
#define MGP_STT_STICK 1

/* double ***rotMatrix; *//* TO BE FIXED */

int             nbint = 0;
int             nbel = 0;
int             nb_state = 0;
int             state = 0;
int             state_id = 0;
double          adim = 1.0;

double          vxrep = 0.0;
double          vyrep = 0.0;
double          vzrep = 0.0;

double          v_adi = 1.0;

double          xvec = 0.0, yvec = 0.0, zvec = 0.0;
/*
double          xmax, ymax, zmax;
double          xmin, ymin, zmin;
*/
double          vxmax = -10000.0, vymax = -10000.0, vzmax = -10000.0;
double          vxmin = 10000.0, vymin = 10000.0, vzmin = 10000.0;
double          r_moy;
double          Fcutlevel=1.;

/* antialiasing  */

MGuchar   antialiased = MG_FALSE;
#define ACSIZE	8

typedef struct {
	GLfloat         x, y;
}               jitter_point;

/* 2 jitter points */
jitter_point    j2[] =
{
	{0.246490f, 0.249999f},
	{-0.246490f, -0.249999f}
};

jitter_point    j8[] =
{
	{-0.334818f, 0.435331f},
	{0.286438f, -0.393495f},
	{0.459462f, 0.141540f},
	{-0.414498f, -0.192829f},
	{-0.183790f, 0.082102f},
	{-0.079263f, -0.317383f},
	{0.102254f , 0.299133f},
	{0.164216f, -0.054399f}
};

/* vectors */

typedef struct {
	double          x1;
	double          x2;
}               vect2d;

/* MEMO changer les me_ en mgp_ */

void            jacobi(float **a, int n, float d[], float **v, int *nrot); /* numerical recipes in C */
vect2d         *me_vect2d_new(double x1, double x2);
double          me_vect2d_angle_rad(vect2d * v1, vect2d * v2);
double          me_vect2d_scal(vect2d * v1, vect2d * v2);
double          me_vect2d_vecto(vect2d * v1, vect2d * v2);
double          me_vect2d_norm(vect2d * v);


/* pour la visu des forces entre les polyedres (bricolage) */

MGuchar more_forces = MG_FALSE;

typedef struct 
{
  double x, y, z;
  double nx,ny,nz;
  double fn;
  int rank;
  int i,j;
}TACT;

TACT * Contact;
unsigned int nbContacts;


/* pour la visu des vitesses et pression de fluide */

MGuchar with_fluid = MG_FALSE;

typedef struct
{
  double x, y;
  double delx,dely;
  double ux,uy,p;
}FLUIDCELL;

FLUIDCELL * FluidCell;
unsigned int nbFluidCells;

/* pour la visu des layers */

MGuchar with_layers = MG_FALSE;
MGuchar layers_defined = MG_FALSE;
unsigned int nbLayers = 5;
unsigned int * layer;

