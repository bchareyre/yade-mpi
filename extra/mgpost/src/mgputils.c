/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

void no_warranty()
{
  fprintf(stdout, "There is NO WARRANTY on the results of this function!\n");
  fflush(stdout);
}

int compare (const void *v1, const void *v2)
{
  if (*((double*)v1) > *((double*)v2)) return 1;
  return -1;
}

void mgp_delay(int t)
{
  unsigned int i;
  while (t) {
    for (i = 65000; i > 0; i--);
    t--;
  }
}

double findMin (double **T, int n, int state)
{
  double min;
  int i;

  min = T[0][state];
  for (i = 0; i < n; i++)
    if (min > T[i][state])
      min = T[i][state];

  return min;
}

double findMax (double **T, int n, int state)
{
  double max;
  int i;

  max = T[0][state];
  for (i = 0; i < n; i++)
    if (max < T[i][state])
      max = T[i][state];

  return max;
}

double findAbsMin (double **T, int n, int state)
{
  double min;
  int i;

  min = fabs (T[0][state]);
  for (i = 0; i < n; i++)
    if (min > fabs (T[i][state]))
      min = fabs (T[i][state]);

  return min;
}

double findAbsMax (double **T, int n, int state)
{
  double max;
  int i;

  max = fabs (T[0][state]);
  for (i = 0; i < n; i++)
    if (max < fabs (T[i][state]))
      max = fabs (T[i][state]);

  return max;
}

double mgMax (double a, double b)
{
  return (a > b) ? a : b;
}

double mgMin (double a, double b)
{
  return (a < b) ? a : b;
}

void findBoundaries (int state, double *xminB, double *yminB, double *zminB,
                                double *xmaxB, double *ymaxB, double *zmaxB)
{
  int i;
  double xjonc, yjonc;
  double angleOrient;
  double Lmaxtmp;
  
  *xmaxB = x[0][0];
  *xminB = x[0][0];
  *ymaxB = y[0][0];
  *yminB = y[0][0];
  *zmaxB = z[0][0];
  *zminB = z[0][0];
  
  for (i = 0; i < nbel; i++)
    {
    angleOrient = rot[i][state];
    if (angleOrient < 0.0)
      angleOrient = angleOrient + 6.283185e00;
    if (angleOrient > 6.283185e0)
      angleOrient = angleOrient - 6.283185e00;
    
    if (((bdyty[i] == 1)||(bdyty[i] == 0))
      ||((bdyty[i] == 2)||(bdyty[i] == 5))) /* DISK ou SPHER ou POLYG ou POLYE */
      {
      if ((x[i][state] + radius[i][state]) > *xmaxB)
        *xmaxB = x[i][state] + radius[i][state];
      
      if ((x[i][state] - radius[i][state]) < *xminB)
        *xminB = x[i][state] - radius[i][state];
      
      if ((y[i][state] + radius[i][state]) > *ymaxB)
        *ymaxB = y[i][state] + radius[i][state];
      
      if ((y[i][state] - radius[i][state]) < *yminB)
        *yminB = y[i][state] - radius[i][state];
      
      if (!mode2D)
        {
        if ((z[i][state] + radius[i][state]) > *zmaxB)
          *zmaxB = z[i][state] + radius[i][state];
        
        if ((z[i][state] - radius[i][state]) < *zminB)
          *zminB = z[i][state] - radius[i][state];
        }
      
      }
    
    if (bdyty[i] == 4)	/* JONCx */
      {
      xjonc = datas[datadistrib[i]];
      yjonc = datas[datadistrib[i] + 1];
      
      if (x[i][state] + fabs (xjonc * cos (angleOrient)) + yjonc > *xmaxB)
        *xmaxB = x[i][state] + fabs (xjonc * cos (angleOrient)) + yjonc;
      
      if (x[i][state] - fabs (xjonc * cos (angleOrient)) - yjonc < *xminB)
        *xminB = x[i][state] - fabs (xjonc * cos (angleOrient)) - yjonc;
      
      if (y[i][state] + fabs (xjonc * sin (angleOrient)) + yjonc > *ymaxB)
        *ymaxB = y[i][state] + fabs (xjonc * sin (angleOrient)) + yjonc;
      
      if (y[i][state] - fabs (xjonc * sin (angleOrient)) - yjonc < *yminB)
        *yminB = y[i][state] - fabs (xjonc * sin (angleOrient)) - yjonc;
      }
    
    if ((bdyty[i] == 100)||(bdyty[i] == 101))	/* WALLX */
      {
      double pos = datas[datadistrib[i]] - xvec;
      
      if (pos < *xminB)
        *xminB = pos;
      
      if (pos > *xmaxB)
        *xmaxB = pos;
      }
    
    if ((bdyty[i] == 102)||(bdyty[i] == 103)) /* WALLY */
      {
      double pos = datas[datadistrib[i]] - yvec;
      
      if (pos < *yminB)
        *yminB = pos;
      
      if (pos > *ymaxB)
        *ymaxB = pos;
      }
    
    if (bdyty[i] == MGP_CYLYx)
      {
      double Rcyl = datas[datadistrib[i]];
      double pos = datas[datadistrib[i+3]];
      
      if ((pos + Rcyl) > *xmaxB)
        *xmaxB = pos + Rcyl;
      
      if ((pos - Rcyl) < *xminB)
        *xminB = pos - Rcyl;
      
      if ( Rcyl > *zmaxB)
        *zmaxB = Rcyl;
      
      if ( -Rcyl < *zminB)
        *zminB = -Rcyl;
      }
    
    
    /*       if ((bdyty[i] == 104)||(bdyty[i] == 105))	/\* WALLZ *\/ */
    /* 	{ */
    /* 	  double pos = datas[datadistrib[i]]; */
    
    /* 	  if (pos < *zminB) */
    /* 	    *zminB = pos; */
    
    /* 	  if (pos > *zmaxB) */
    /* 	    *zmaxB = pos; */
    /* 	} */
    
    if (mode2D) *zminB = *zmaxB = 0.0;
    
    }
  
  maxmax = *xmaxB;
  if (maxmax>*ymaxB) maxmax = *ymaxB;
  if (maxmax>*zmaxB) maxmax = *zmaxB;
  
  Lmax = (*xmaxB-*xminB);
  Lmaxtmp = (*ymaxB-*yminB);
  if (Lmaxtmp>Lmax) Lmax=Lmaxtmp;
  Lmaxtmp = (*zmaxB-*zminB);
  if (Lmaxtmp>Lmax) Lmax=Lmaxtmp;
  
}

double findPositiveMax (double **T, int n, int state)
{
  double max;
  int i;

  max = 0.0/* fabs (T[0][0]) */;
  for (i = 0; i < n; i++)
    if ((T[i][state] > 0.0) && (max < fabs (T[i][state])))
      max = fabs (T[i][state]);

  return max;
}


double findNegativeMax (double **T, int n, int state)
{
  double max;
  int i;

  max = 0.0 /* fabs (T[0][0]) */;
  for (i = 0; i < n; i++)
    if ((T[i][state] < 0.0) && (max < fabs (T[i][state])))
      max = fabs (T[i][state]);

  return max;
}

MGuchar pres_du_plan (int i, double dist)
{
  static double dst;

  if ((sectionActive) && (bdyclass[i] == MGP_GRAIN))
    {
      dst = section.a * x[i][state] 
          + section.b * y[i][state] 
          + section.c * z[i][state] 
          + section.d; 

      return (fabs(dst) < dist) ? 1 : 0;
    }
  else
    return 1;
}

void nullifyCumulatedTabs ()
{
  int i, j;

  for (i = 0; i < nbbodies; i++)
    for (j = 0; j < nbtimes; j++)
      nbneighbors[i][j] = 0;

  for (i = 0; i < nbbodies; i++)
    dataqty[i] = 0;
}

void centrer_repere ()
{
  double xmax,ymax,zmax;
  double xmin,ymin,zmin;
  int i, j;
  
  xmax = x[0][0];
  ymax = y[0][0];
  zmax = z[0][0];
  xmin = x[0][0];
  ymin = y[0][0];
  zmin = z[0][0];
  
  for (i = 0; i < nbel; i++)
    {
    if (x[i][0] > xmax)
      xmax = x[i][0];
    if (y[i][0] > ymax)
      ymax = y[i][0];
    if (z[i][0] > zmax)
      zmax = z[i][0];
    
    if (x[i][0] < xmin)
      xmin = x[i][0];
    if (y[i][0] < ymin)
      ymin = y[i][0];
    if (z[i][0] < zmin)
      zmin = z[i][0];
    }
  
  if (middle_rep)
    {
    xvec = xmin + 0.5 * (xmax - xmin);
    yvec = ymin + 0.5 * (ymax - ymin);
    zvec = zmin + 0.5 * (zmax - zmin);
    
    xmin -= xvec;
    ymin -= yvec;
    zmin -= zvec;
    xmax -= xvec;
    ymax -= yvec;
    zmax -= zvec;
    
    for (i = 0; i < nbel; i++)
      for (j = 0; j < nb_state; j++)
        {
        x[i][j] -= xvec;
        y[i][j] -= yvec;
        z[i][j] -= zvec;
        }
        
        if(more_forces)
          {
          for (i = 0; i < nbContacts; i++)
              {
              Contact[i].x -= xvec;
              Contact[i].y -= yvec;
              Contact[i].z -= zvec;
              }
          }
    }
}

void calcul_adim ()
{
  int i;
  double rmax;

  rmax = radius[0][0];
  for (i = 0; i < nbel; i++)
    if (radius[i][0] > rmax)
      rmax = radius[i][0];

  if (rmax == 0.) adim = 1.;
  else adim = 0.5 / rmax;
}

void precalculs ()
{
  int i, j;

  fprintf (stdout, "pre-computation... ");
  fflush(stdout);

  nbint = 0;
  for (i = 0; i < nbel; i++)
    nbint += nbneighbors[i][0];

  r_moy = 0.0;
  for (i = 0; i < nbel; i++)
    r_moy += radius[i][0];
  
  if (nbel > 0) r_moy = r_moy / nbel;
  else r_moy = 1.;
  
  for (i = 0; i < nbel; i++)
    {
      if ((2.0 * r_moy / grillX) > fmod (fabs(x[i][0]), grillX) / grillX)
	discrim[i] = MG_TRUE;
      if ((2.0 * r_moy / grillY) > fmod (fabs(y[i][0]), grillY) / grillY)
	discrim[i] = MG_TRUE;
    }

  for (i = 0; i < nbel; i++)
    for (j = 0; j < nb_state; j++)
      {
	if (vx[i][j] > vxmax)
	  vxmax = vx[i][j];
	if (vy[i][j] > vymax)
	  vymax = vy[i][j];
	if (vz[i][j] > vzmax)
	  vzmax = vz[i][j];

	if (vx[i][j] < vxmin)
	  vxmin = vx[i][j];
	if (vy[i][j] < vymin)
	  vymin = vy[i][j];
	if (vz[i][j] < vzmin)
	  vzmin = vz[i][j];
      }

  if (multifiles)
    {
      for (i = 0; i < nbel; i++)
	{
	  x_0[i] = x[i][0];
	  y_0[i] = y[i][0];
	  z_0[i] = z[i][0];
	}
    }

  fprintf (stdout, "done\n");
}

/*
 * Resolution de polynome de degre 3 sous forme:
 *  a*X^3 + b*X^2 + c*X + d = 0
 *  x1,x2,x3 : solutions
 *  solty    : nombre de solutions
 */
void degre3(double a, double b, double c, double d, 
            double *x1, double *x2, double *x3, int *solty)
{
  double pi = 3.14159, p, q, del;
  double vt;
  double kos, r, z, alpha, x_k;
  double x[4], buf;	 
  int k;
  double uv1, uv2, sgn;

  vt = -b / 3 / a;
  p = c / a - b * b / 3 / a / a;
  q = b * b * b / a / a / a / 13.5 + d / a - b * c / 3 / a / a;

  if (fabs (p) < 1e-12)
    p = 0.0;
  if (fabs (q) < 1e-12)
    q = 0.0;

  del = q * q / 4.0 + p * p * p / 27.0;
  if (fabs (del) < 1e-12)
    del = 0.0;

  /* ------------------------------------------- */

  if (del <= 0.0)

    {
      if (p != 0.0)
	{
	  kos = -q / 2.0 / sqrt (-p * p * p / 27.0);
	  r = sqrt (-p / 3.0);
	}
      else  /* vu que del<=0, si p=0, alors del=0 */
	{
	  kos = 0.0;
	  r = 0.0;
	}

      if (fabs (kos == 1))
	  alpha = -pi * (kos - 1) / 2.0;
      else
	  alpha = acos (kos);

      for (k = 0; k <= 2; k++)
	{
	  x_k = 2.0 * r * cos ((alpha + 2.0 * k * pi) / 3.0) + vt;
	  sgn = ( (x_k > 0.0) - (x_k < 0.0) );
	  x[k] = sgn * floor (fabs (x_k) * 1e10 + .5) / 1.0e10;
	}

      /* on classe du plus grand au plus petit */
      if(x[2] > x[1])
	{
	  buf = x[1];
	  x[1] = x[2];
	  x[2] = buf;
	}

      if(x[1] > x[0])
	{
	  buf = x[0];
	  x[0] = x[1];
	  x[1] = buf;
	}

      if(x[2] > x[1])
	{
	  buf = x[1];
	  x[1] = x[2];
	  x[2] = buf;
	}

      *x1 = x[0];
      *x2 = x[1];
      *x3 = x[2];

      if (r == 0)
          *solty = 1;
      else
	  *solty = 3;

    }
  else
    {
      r = sqrt (del);
      z = -q / 2.0 + r;
      sgn = ((z > 0.0) - (z < 0.0));
      uv1 = sgn * pow(fabs(z),0.333333);
      z = -q / 2.0 - r;
      sgn = ((z > 0.0) - (z < 0.0)); 
      uv2 = sgn * pow(fabs(z), 0.333333);
      x_k = uv1 + uv2 + vt;
      sgn = ((x_k > 0.0) - (x_k < 0.0));
      x_k = sgn * floor (fabs (x_k) * 1.0e10 + 0.5) / 1.0e10;
      *x1 = x_k;
      *x2 = x_k;
      *x3 = x_k;
      *solty=1;
    }
}


/* from Numerical Recipes: Eigenvalues and Eigen vectors */
#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau);

void jacobi(float **a, int n, float d[], float **v, int *nrot)
{
  int j,iq,ip,i;
  float tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

  b=vector(1,n);
  z=vector(1,n);
  for (ip=1;ip<=n;ip++) {
    for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
    v[ip][ip]=1.0;
  }
  for (ip=1;ip<=n;ip++) {
    b[ip]=d[ip]=a[ip][ip];
    z[ip]=0.0;
  }
  *nrot=0;
  for (i=1;i<=50;i++) {
    sm=0.0;
    for (ip=1;ip<=n-1;ip++) {
      for (iq=ip+1;iq<=n;iq++)
	sm += fabs(a[ip][iq]);
    }
    if (sm == 0.0) {
      free_vector(z,1,n);
      free_vector(b,1,n);
      return;
    }
    if (i < 4)
      tresh=0.2*sm/(n*n);
    else
      tresh=0.0;
    for (ip=1;ip<=n-1;ip++) {
      for (iq=ip+1;iq<=n;iq++) {
	g=100.0*fabs(a[ip][iq]);
	if (i > 4 && (float)(fabs(d[ip])+g) == (float)fabs(d[ip])
	    && (float)(fabs(d[iq])+g) == (float)fabs(d[iq]))
	  a[ip][iq]=0.0;
	else if (fabs(a[ip][iq]) > tresh) {
	  h=d[iq]-d[ip];
	  if ((float)(fabs(h)+g) == (float)fabs(h))
	    t=(a[ip][iq])/h;
	  else {
	    theta=0.5*h/(a[ip][iq]);
	    t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
	    if (theta < 0.0) t = -t;
	  }
	  c=1.0/sqrt(1+t*t);
	  s=t*c;
	  tau=s/(1.0+c);
	  h=t*a[ip][iq];
	  z[ip] -= h;
	  z[iq] += h;
	  d[ip] -= h;
	  d[iq] += h;
	  a[ip][iq]=0.0;
	  for (j=1;j<=ip-1;j++) {
	    ROTATE(a,j,ip,j,iq)
	      }
	  for (j=ip+1;j<=iq-1;j++) {
	    ROTATE(a,ip,j,j,iq)
	      }
	  for (j=iq+1;j<=n;j++) {
	    ROTATE(a,ip,j,iq,j)
	      }
	  for (j=1;j<=n;j++) {
	    ROTATE(v,j,ip,j,iq)
	      }
	  ++(*nrot);
	}
      }
    }
    for (ip=1;ip<=n;ip++) {
      b[ip] += z[ip];
      d[ip]=b[ip];
      z[ip]=0.0;
    }
  }
  fprintf(stderr, "Too many iterations in routine jacobi\n");
}
#undef ROTATE


vect2d *me_vect2d_new(double x1, double x2)
{
  vect2d *v;

  v = (vect2d*)malloc(sizeof(vect2d));

  v->x1 = x1;
  v->x2 = x2;

  return v;
}

double me_vect2d_angle_rad(vect2d *v1, vect2d *v2)
{
  double U, A = 0.0, S = 0.0;
  
  U = me_vect2d_norm(v1);
  U = U * me_vect2d_norm(v2);

  if (U > 1.0e-10)
    {
      A = me_vect2d_vecto(v1, v2);
      S = me_vect2d_scal(v1, v2);

      if (A >= 0.0)
	{
	  if (S >= 0.0)
	    A = asin(A / U);
	  else
	    A = M_PI - asin(A / U);
	}
      else
	{

	  if (S >= 0.0)
	    A = 1.5 * M_PI - asin(A / U);
	  else
	    A = M_PI - asin(A / U);
	}

      return A;
    }
  else
    return 0.0;
}

double me_vect2d_scal(vect2d *v1, vect2d *v2)
{
  return (v1->x1 * v2->x1 + v1->x2 * v2->x2);
}

double me_vect2d_vecto(vect2d *v1, vect2d *v2)
{
  return (v1->x1 * v2->x2 - v1->x2 * v2->x1);
}

double me_vect2d_norm(vect2d *v)
{
  return sqrt( v->x1 * v->x1 + v->x2 * v->x2);
}

