#include	<stdio.h>
#include 	<stdlib.h>
#include 	<math.h>

#ifdef MAC_OSX_SYSTEM
#include <vecLib/vecLib.h>
#endif

#ifdef HAVE_IRISGL
#include        <gl.h>
#include        <device.h>
#endif  /* HAVE_IRISGL */

static float SCALE_FACTOR = 1.0;  // JWH 14 Mar 2006

#define		FLUID_METHOD
#define		FLUID	1

#define 	IMAGE_SCALE
#ifdef IMAGE_SCALE
  #define	EXTENT	200.0
  double  left = 0, right = EXTENT,
	bottom = 0, top = EXTENT,
	near = 100.0, far = -100.0;
  float	DIM = 0.25; 
#else
#define	EXTENT	300.0
  double  left = -EXTENT, right = EXTENT,
  	bottom = -EXTENT, top = EXTENT,
	near = 150.0, far = -150.0;
  float	DIM = 0.25;
#endif

int     xsize = 200, ysize = 200;

static const float DEFAULT_SAMPLING_DENSITY = 20.0;
static const float DEFAULT_VARIANCE = 1.0;

#define	 		NUM_POINTS 		20
#define			LINES

#ifdef HAVE_IRISGL
#define			DISPLAY
#endif

/* #define 		MAX_FIDUCIAL_POINTS	5000 */
#define 		MAX_FIDUCIAL_POINTS	500
/* #define 		MAX_SULCI		1000 */
#define 		MAX_SULCI		10000
#define 		NEW_BORDERS
/*#define			READ_CELL_FILE  */
/* comment out IMAGE_SCALE */
/*#define			READ_COORD_FILE*/
#define			READ_BORDER_FILE
#ifdef READ_CELL_FILE
#define			MAX_NUM_CELLS	50000
#endif
#define			MAXLINE		256

#include	"surface.h"

void 	DisplaySurface (short new, int snum);
void 	DisplayDeform (short new, int snum, int flag);
void 	DisplayTotalDeform (short new, int snum, int flag);
void 	DisplaySulci (short new, int snum, int flag);
void 	DisplaySulci2 (short new, int snum, int flag);
int     get_lut_index (int snum, float value, float dmin, float dmax);
void    read_lut_file ();
void 	determine_min_max (int num, float *array, float *min, float *max);
float	DetermineVariance (char *name, float var);
void	DisplayImage (short, int, unsigned char*);
void 	DisplayDeformedImage (short new, unsigned char *image);
void	LAPACK_stuff (double *mat, double *x1, double *y1, double *iwtx, double *iwty);
void	ReadCellFile (char *file);
void	WriteCellFile (char *file);
void	CreateSurface (int width, int height, int spacing);
void	ImageInitStuff (char *filename, int xsize, int ysize);
void	ComputeDeformationFLUID (int step, double x0, double y0, double *defx, double *defy);
void	ResampleData ();
void	CreateGridImage (char *image, int xsize, int ysize);
void	WriteImage (char *file, char *inimage, int xsize, int ysize);
void	DeformImage (int ixsize, int iysize, int deform_flag);
double	ImageX2Transform (double val);
double	ImageY2Transform (double val);

int	num_surfaces = 1;

#define byte unsigned char
#define ABS(x)	((x) >= 0 ? (x) : -(x))
#define max(A, B) ((A) > (B) ? (A) : (B))
#define min(A, B) ((A) > (B) ? (B) : (A))

int 	totpoints;
int 	dim;
double 	origX1 [MAX_FIDUCIAL_POINTS],
	origY1 [MAX_FIDUCIAL_POINTS]; 
double 	origX2 [MAX_FIDUCIAL_POINTS],
	origY2 [MAX_FIDUCIAL_POINTS]; 

#ifdef READ_CELL_FILE
typedef struct{
	int     section, point;
        char    area1 [MAXLINE], area2 [MAXLINE];
        float   pos [3];
}Cells;

int	NumCells;
Cells 	cells [MAX_NUM_CELLS];
#endif

typedef struct{
	double	x [MAX_FIDUCIAL_POINTS], 
		y [MAX_FIDUCIAL_POINTS];
        float   ddd;
        float   variance;
	int	np;
	int	hold_np;
	char	name [MAXLINE];
}Sulcus;
Sulcus	sulci [4][MAX_SULCI];

typedef struct{ 
	char	name [MAXLINE];
	float	var_factor;
}SulciVar;
#define	 NUM_SULCI_TYPE 	21
SulciVar sulcivar [NUM_SULCI_TYPE] = {
	{"MORPH.A", 0.01}, 
	{"MORPH.B", 0.01}, 
	{"MORPH.C", 0.01}, 
	{"MORPH.D", 0.01}, 
	{"MORPH.E", 0.01}, 
	{"MORPH.F", 0.01}, 
	{"MORPH.G", 0.01}, 
	{"MORPH.H", 0.01}, 
	{"MORPH.I", 0.01}, 
	{"MORPH.Cis", 0.25},
	{"MORPH.Ces", 0.25},
	{"MORPH.IPS", 0.25},
	{"MORPH.POS", 0.25},
	{"MORPH.IOS", 0.25},
	{"MORPH.OTS", 0.25},
	{"MORPH.SF1", 0.25},
	{"MORPH.SF2", 0.25},
	{"MORPH.AS", 0.25},
	{"MORPH.PS", 0.25},
	{"MORPH.STS", 0.25},
	{"MORPH.MOS", 0.25}
};

double 	newX1 [MAX_FIDUCIAL_POINTS],
	newY1 [MAX_FIDUCIAL_POINTS]; 
char	name [MAX_FIDUCIAL_POINTS][MAXLINE];

float	displace [2][MAX_POINTS];
float	total_displace [2][MAX_POINTS];

char    *inimage,
	*outimage,
	*def_out;
float 	*image_xdeform;
float 	*image_ydeform;
int     ixsize, iysize; 

void	ReadPoints (char *file, int *num_sulci, int idx); 
void	WritePoints (char *file, int num_sulci, int idx); 
int	find_sulcus (int num, char *name, int idx);
double 	dist (double x1, double y1, double x2, double y2);

#define NUMT 10 
#define graphics 0

#ifdef HAVE_IRISGL
Matrix Identity = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
#endif  /* HAVE_IRISGL */

double 	X1 [MAX_FIDUCIAL_POINTS],
	Y1 [MAX_FIDUCIAL_POINTS]; 
double 	X2 [MAX_FIDUCIAL_POINTS],
	Y2 [MAX_FIDUCIAL_POINTS],
	VAR [MAX_FIDUCIAL_POINTS];
double 	pathx [NUMT][5000];
double 	pathy [NUMT][5000];
double *deformx;
double *deformy;

void 	findpath (double *X1,double *Y1,double *X2,double *Y2, int totpoints);
void 	calculate_weightsFLUID (int totpoints, int dim, double *mat,
		double *X1, double *Y1, double *X2, double *Y2);
double	TransformY2Image (double val);
double	TransformX2Image (double val);

/* The weights Of each of the greens functions at the points */
double 	wtx [MAX_FIDUCIAL_POINTS],
	wty [MAX_FIDUCIAL_POINTS];
int	n1, n2, n3 = 0, n4;
double 	beta;
float	var_mult; 
double 	alpha = 0.001;
/*double 	sigma = 1.0;*/
int	numiter;

        short   val;
        long    device, done;
FILE	*fd_iter;
double *mat;	    /* the matrix of linear equation */

/* Surface stuff. HAD 5.9.95 */
int	snum = 0;

int	main (int argc, char *argv[])
{
	int	iter;
    	double l1,l2,l3,l4;
    	double tmp1, tmp2;
    	int flag,i,ret_val,j,k,l;
        int doImageWarping = 0;        /* JWH 12/05/00 */
        
    	/* Now the stuff to do the actual deformation */
    	double x,y,z;
    	double ux,uy,uz;  /* Displacement FIELD  */
    	int tmpux,tmpuy;  /* Displacement FIELD  */
    	double xdelta,ydelta,tx,ty;
    	FILE *fp, *fopen();  	/* File pointer for the corespondance file */

    	FILE 	*fpin,*fpout;
	char	out [100];
	FILE	*fdx, *fdy, *fdnew;
	float	errorx = 0, errory = 0, t1, t2;
	int	step;
	float 	min_ux, max_ux, min_uy, max_uy;

	float	x0, y0;
	char	s1 [10];
	char	imagefile [256], filename [256], outfile [256];

        printf("Size of surface: %d\n", sizeof(surface));
        printf("Size of sulci: %d\n", sizeof(sulci));
        
    	if (argc != 10){ 
           printf ("USAGE: %s file1 file2 area_file beta var_mult numiter image xsize ysize\n", 
		argv [0]); 
           printf("If xsize or ysize is zero, no image will be displayed.\n");
           exit(-1);
    	}

	beta = atof (argv [4]);
	var_mult = atof (argv [5]);
	numiter = atoi (argv [6]);
	printf ("beta is %f\n", beta);	
	printf ("var_mult is %f\n", var_mult);	
	printf ("numiter is %d\n", numiter);
#ifdef READ_COORD_FILE
	read_coord_file (argv [3], snum);
	/*read_sort_file (argv [4], snum);*/
#else
#ifdef READ_CELL_FILE
	ReadCellFile (argv [3]);
#else
	ReadPoints (argv [3], &n3, 2);
#endif
#endif
	/* n3 is areal borders */
	ReadPoints (argv [1], &n1, 0); 
	ReadPoints (argv [2], &n2, 1); 
	
	ResampleData ();

	ixsize = atoi (argv [8]);
	iysize = atoi (argv [9]);
        if (argc >= 11) {
           SCALE_FACTOR = atof(argv[10]);
        }

	if ((ixsize > 0) && (iysize > 0)) {
           doImageWarping = 1;
        }
        
        if (doImageWarping) {
	   ImageInitStuff ((char *)argv [7], ixsize, iysize); 
        }
        
	/* Add 4 corner points */
	/*X1 [totpoints] = 0;
	Y1 [totpoints] = 0;
	totpoints++;
	X1 [totpoints] = EXTENT;
	Y1 [totpoints] = EXTENT;
	totpoints++;
	X1 [totpoints] = EXTENT;
	Y1 [totpoints] = 0;
	totpoints++;
	X1 [totpoints] = 0;
	Y1 [totpoints] = EXTENT;
	totpoints++;*/

	/* Alocate the memory for the mat */
	dim = totpoints+3;
	mat = (double *)calloc(dim*dim,sizeof(double));

#ifdef DISPLAY
	read_lut_file ();
	DisplaySulci (True, snum, 0);
	DisplaySulci2 (True, snum, 0);
        if (doImageWarping) {
           DisplayDeformedImage (True, inimage);
        }
#endif

        /*for (i = 0; i < totpoints; i++)
           printf ("DEFORM %.1f %.1f -> %.1f %.1f, variance %f\n", 
			X1 [i], Y1 [i], X2 [i], Y2 [i], VAR [i]);*/

	printf ("\n****FLUID Deformation method...\n\n");
        deformx = (double *)calloc(totpoints,sizeof(double));
        deformy = (double *)calloc(totpoints,sizeof(double));

        findpath (X1,Y1,X2,Y2,totpoints);

        for(j=0;j<totpoints;j++){
           for(i=0;i<NUMT;i++)
	      printf ("%lf,%lf\n", pathx[i][j], pathy[i][j]);
           printf("========\n");
	}

        /* Initialize the deformation field to identity */

	for (i = 0; i < totpoints; i++){
	   deformx [i] = X1 [i];
           deformy [i] = Y1 [i];
        }

	for (step = NUMT-2; step >= 0; step--){
	   printf ("Step %2d\n", step);
	   calculate_weightsFLUID (totpoints, dim, mat, 
		pathx[step+1], pathy[step+1], pathx[step], pathy[step]);

	   /* Now deform the actual surface */
           printf ("\tDeforming Surface %d points...\n", surface [snum].num_points);
	   for (j = 0; j < surface [snum].num_points; j++){
	      if ((j % 10000) == 0)
 	         printf ("\t%5d of %5d\n", j, surface [snum].num_points);
	      x0 = surface [snum].points [j].pos [0];
	      y0 = surface [snum].points [j].pos [1];
	      ComputeDeformationFLUID (step, x0, y0, &ux, &uy); 

	      surface [snum].points [j].pos [0] = x0 + ux;
	      surface [snum].points [j].pos [1] = y0 + uy;
	      /*printf ("DISPLACE: %d %f %d %f\n", j, ux, j, uy);*/
	   }
#ifdef READ_COORD_FILE
	   {
	   char 	fname [256];
	   sprintf (fname, "surface.fluid.%d", step);	
	   write_coord_file (fname, snum);
	   }
#endif

#ifdef READ_CELL_FILE
	   /* Deform the cells */
	   printf ("\tDeforming %d cells...\n", NumCells);
	   for (j = 0; j < NumCells; j++){
	      if ((j % 10000) == 0)
	         printf ("\t%d of %d\n", j, NumCells);
              x0 = cells [j].pos [0]; 
              y0 = cells [j].pos [1]; 
	      ComputeDeformationFLUID (step, x0, y0, &ux, &uy); 
              cells [j].pos [0] = x0 + ux;
              cells [j].pos [1] = y0 + uy;
	   }
#else
	   /* Deform the areal borders */
	   printf ("\tDeforming %d areal borders...\n", n3);
	   for (j = 0; j < n3; j++){
	      /*printf ("Deforming areal border %d %s\n", j, sulci [2][j].name); */
              for (i = 0; i < sulci [2][j].np; i++){
                 x0 = sulci [2][j].x [i];
                 y0 = sulci [2][j].y [i];
	         ComputeDeformationFLUID (step, x0, y0, &ux, &uy); 
                 sulci [2][j].x [i] = x0 + ux;
                 sulci [2][j].y [i] = y0 + uy;
	 	 /*printf ("\tAreal %.2f %.2f -> %.2f %.2f\n",
			x0, y0, x0 + ux, y0 + uy);*/
	      }
	   }
#endif
           /* Now test it on known points */
           for (j = 0; j < totpoints; j++){
              x0 = X2 [j];
              y0 = Y2 [j];
              ComputeDeformationFLUID (step, x0, y0, &ux, &uy);
              X2 [j] = x0 + ux;
              Y2 [j] = y0 + uy;
           }

	   /* This is stupid, but it works. redundant (these are known points) */
	   /* HAD 6.13.96 */ 
	   for (i = 0; i < n4; i++){
              /*printf ("Deform Sulcus %d %s, %d points\n",
			i, sulci [3][i].name, sulci [3][i].np);  */
	      for (j = 0; j < sulci [3][i].np; j++){
	         x0 = sulci [3][i].x [j];
	         y0 = sulci [3][i].y [j];
                 ComputeDeformationFLUID (step, x0, y0, &ux, &uy);
                 sulci [3][i].x [j] = x0 + ux;
                 sulci [3][i].y [j] = y0 + uy;
	      }
	   }
/*&&&*/
#ifdef DISPLAY
	   DisplaySulci (False, snum, 0);
	   DisplaySulci2 (False, snum, 0);
	   sleep (1);
#endif

	}
	/*printf("PathX PathY \n");
	for(i=0;i<NUMT;i++){
	   for(k=0;k<totpoints;k++){
	      printf("%lf %lf\n",pathx[i][k],pathy[i][k]);
	   }
	}*/


        WritePoints ("target.deform.dat", n1, 0);
        WritePoints ("source.deform.dat", n4, 3);
#ifdef READ_CELL_FILE
        WriteCellFile ("cell.deform.dat");
#else
        WritePoints ("arch.deform.dat", n3, 2);
#endif
        {
        FILE *fd1, *fd2;

        fd1 = fopen ("source.points.deform.dat", "w");
        fd2 = fopen ("target.points.deform.dat", "w");
        for (i = 0; i < totpoints; i++){
	   fprintf (fd1, "%f %f\n", X1 [i], Y1 [i]);
	   fprintf (fd2, "%f %f\n", X2 [i], Y2 [i]);
        }
        fclose (fd1);
        fclose (fd2);
	}

	/* Deform the image... */
        if (doImageWarping) {
   	   DeformImage (ixsize, iysize, FLUID);
        }
        else {
           exit(0);
        }

#ifdef HAVE_IRISGL
	done = False;
	while (done == False){
           if (qtest ()){
              device = qread (&val);
              if (device == ESCKEY){ 
	         done = TRUE;
	      }
	      printf ("draw...\n");
	      DisplaySulci (False, snum, 0);
	      DisplaySulci2 (False, snum, 0);
              if (doImageWarping) {
                 DisplayDeformedImage (False, inimage);
              }
	      sleep (5);
              exit(0);
           }
	}
#endif
	exit(0);
}

void	LAPACK_stuff (double *mat, double *x1, double *y1, double *iwtx, double *iwty)
{
	/* Variables for LAPACK */
	int 	INFO,LDA,LDB,LWORK,N,NRHS;
	char 	UPLO;
	int 	IPIV[MAX_FIDUCIAL_POINTS];
	double 	work[MAX_FIDUCIAL_POINTS];
	double 	tot1,tot2,tot3;
	int	i;

	/* Set up the stuff for LAPACK routines */
	UPLO = 'U';
	INFO = 1;
	LDA = dim;
	LDB = dim;
	LWORK = 8;
	N = dim;
	NRHS = 1;

	/*printf("Calling SSYTRS \n");*/
	dsytrf_(&UPLO,&N,mat,&LDA,IPIV,work,&LWORK,&INFO);
	/*printf("Info after ssytrf =%d\n",INFO);*/
	dsytrs_(&UPLO,&N,&NRHS,mat,&LDA,IPIV,iwtx,&LDA,&INFO);
	/*printf("Info after ssytrs =%d\n",INFO);*/
	dsytrs_(&UPLO,&N,&NRHS,mat,&LDA,IPIV,iwty,&LDA,&INFO);
	/*printf("Info after ssytrs =%d\n",INFO);*/

	/* Now make sure all the constraints are meet */
	tot1 =0;
	tot2 =0;
	tot3 =0;
	for(i=0;i<totpoints;i++){
	   tot1 = tot1+iwtx[i];
	   tot2 = tot2+x1[i]*iwtx[i];
	   tot3 = tot3+x1[i]*iwty[i];
	}
	/*printf("Sum of weights = %lf\n",tot1);
	printf("Sum of X1*weights = %lf\n",tot2);
	printf("Sum of X1*wy = %lf\n",tot3);*/
 	if ((fabsf (tot1) > 0.001) ||
 	    (fabsf (tot2) > 0.001) ||
 	    (fabsf (tot3) > 0.001)){
	   printf ("WARNING: Constraints not met!!\n"); 
	   printf("\tSum of weights = %lf\n",tot1);
	   printf("\tSum of X1*weights = %lf\n",tot2);
	   printf("\tSum of X1*wy = %lf\n",tot3);
	   exit (-1);
	}
}

#ifdef HAVE_IRISGL
Object  SURFACE = 1;
Object  DEFORMX = 2;
Object  DEFORMY = 3;
Object	TARGET = 4;
Object	TEMPLATE = 5;
Object  TOTAL_DEFORMX = 6;
Object  TOTAL_DEFORMY = 7;

#define max(a,b)        ((a) > (b) ? (a) : (b))
#define min(a,b)        ((a) < (b) ? (a) : (b))
#define abs(a)          ((a) > 0.0 ? (a) : -(a))
#define sgn(a)          ((a) < 0.0 ? -1.0 : 1.0)        /* NB 0.0 -> 1.0 */

long	surface_win, 
	deformx_win, deformy_win,
	total_deformx_win, total_deformy_win,
	target_win, template_win, target_win2,
	winid1, winid2;

char    lut [256][4];
short	white [3] = {200, 200, 200};
short	black [3] = {0, 0, 0};
short	red [3] = {255, 0, 0};
short	green [3] = {0, 255, 0};
short	blue [3] = {0, 0, 255};
short	gray [3] = {100, 100, 100};

void DisplaySurface (short new, int snum)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 880, y_init = 0;
  	int	x_size = xsize*2.0, y_size = ysize*2.0;

#ifdef CALC
  	left = bottom = surface [snum].points [0].pos [0];
	right = top = surface [snum].points [0].pos [1];
  	for (i = 0; i < surface [snum].num_points; i++) {
    	   left   = min(left, surface [snum].points [i].pos [0]);
    	   right  = max(right, surface [snum].points [i].pos [0]);
    	   bottom = min(bottom, surface [snum].points [i].pos [1]);
    	   top    = max(top, surface [snum].points[i].pos [1]);
  	}
  	/*left = left - left*0.10; 
  	right = right + right*0.10;
  	bottom = bottom - bottom*0.10; 
  	top = top + top*0.10; */
	printf ("Extent: X %.2f %.2f, Y %.2f %.2f\n",
		left, right, top, bottom);
  	near = 100.0;
  	far = -100.0;
#endif

  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
    	   prefposition (x_init, x_init+x_size, y_init, y_init+y_size); 
    	   surface_win = winopen ("Sulcal Warping Engine");
	   /*printf ("surface_win %x\n", surface_win);*/
	   RGBmode ();
    	   doublebuffer();
    	   gconfig();
  	}

	/*printf ("set win to surface win %x\n", surface_win);*/
  	winset (surface_win);
  	ortho (left, right, bottom, top, near, far);

  	setmap (0);
  	/*color (254);*/
	c3s (white);
  	clear (); 
  	makeobj (SURFACE);

  	for (i = 0; i < surface [snum].num_points; i++){
	   c3s (black);
	   /*if (i < 10)
	      printf ("DISPLAY %d %.2f %.2f\n", i,
			surface [snum].points [i].pos [0],
                        surface [snum].points [i].pos [1]);*/
#ifdef LINES
     	   for (j = 0; j < surface [snum].points [i].num_neighs; j++){
              n = surface [snum].points [i].neighs [j];
	      if (i > j){
	         /*printf ("line %d -> %d\n", i, n);*/
                 bgnline();
                    v3f (surface [snum].points [i].pos);
                    v3f (surface [snum].points [n].pos);
		    /*printf ("Point %.2f %.2f %.2f to %.2f %.2f %.2f\n",
			surface [snum].points [i].pos [0],
			surface [snum].points [i].pos [1],
			surface [snum].points [i].pos [2],
			surface [snum].points [n].pos [0],
			surface [snum].points [n].pos [1],
			surface [snum].points [n].pos [2]);*/
                 endline();
              }
           }
#else
	   rectf (surface [snum].points [i].pos [0]-DIM,
	  	  surface [snum].points [i].pos [1]-DIM,
    	  	  surface [snum].points [i].pos [0]+DIM,
	  	  surface [snum].points [i].pos [1]+DIM);
#endif
  	}
  	closeobj ();
  	callobj (SURFACE);
  	swapbuffers ();
}  

void DisplayDeform (short new, int snum, int flag)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 480, y_init = 0;
	float	dmin, dmax;
	int	color_index;
	short	current_color [3];
  	int	x_size = xsize*2.0, y_size = ysize*2.0;

  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
	   if (flag == 0)
	      x_init = 480;
	   else
	      x_init = 80;
    	   prefposition (x_init, x_init+x_size, y_init, y_init+y_size); 
	   if (flag == 0)
    	      deformx_win = winopen ("Deformation in X");
	   else
    	      deformy_win = winopen ("Deformation in Y");
    	   doublebuffer();
    	   RGBmode ();
    	   gconfig();
  	}
	
	if (flag == 0)
  	   winset (deformx_win);
	else
  	   winset (deformy_win);
  	ortho (left, right, bottom, top, near, far);

   	c3s (white);	
  	clear (); 
	if (flag == 0)
  	   makeobj (DEFORMX);
	else
  	   makeobj (DEFORMY);

	if (flag == 0)
	   determine_min_max (surface [snum].num_points, 
		displace [0], &dmin, &dmax);
 	else	
	   determine_min_max (surface [snum].num_points, 
		displace [1], &dmin, &dmax);
	printf ("flag %d %f %f\n", flag, dmin, dmax);
	/*dmin = -0.025;
	dmax = 0.025;*/
  	for (i = 0; i < surface [snum].num_points; i++){
	   if (flag == 0)
              color_index = get_lut_index (snum, displace [0][i], dmin, dmax);
	   else
              color_index = get_lut_index (snum, displace [1][i], dmin, dmax);
	   for (j = 0; j < 3; j++)
	      current_color [j] = lut [color_index][j+1];
	   c3s (current_color); 
           /*if ((flag == 0) && ((i % 1000) == 0))
	      printf ("color is %d %d %d\n", 
		current_color [0],
		current_color [1],
		current_color [2]);*/
#ifdef LINES
     	   for (j = 0; j < surface [snum].points [i].num_neighs; j++){
              n = surface [snum].points [i].neighs [j];
	      if (i > j){
                 bgnline();
                    v3f (surface [snum].points [i].pos);
                    v3f (surface [snum].points [n].pos);
                 endline();
              }
           }
#endif
	   rectf (surface [snum].points [i].pos [0]-DIM,
	  	  surface [snum].points [i].pos [1]-DIM,
    	  	  surface [snum].points [i].pos [0]+DIM,
	  	  surface [snum].points [i].pos [1]+DIM);
  	}
  	closeobj ();
	if (flag == 0)
  	   callobj (DEFORMX);
	else
  	   callobj (DEFORMY);
  	swapbuffers ();
}

void DisplayTotalDeform (short new, int snum, int flag)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 480, y_init = 400;
	float	dmin, dmax;
	int	color_index;
	short	current_color [3];
  	int	x_size = xsize*2.0, y_size = ysize*2.0;

  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
	   if (flag == 0)
	      x_init = 480;
	   else
	      x_init = 80;
    	   prefposition (x_init, x_init+x_size, y_init, y_init+y_size); 
	   if (flag == 0)
    	      total_deformx_win = winopen ("Aggregate Deformation in X");
	   else
    	      total_deformy_win = winopen ("Aggregate Deformation in Y");
    	   doublebuffer();
    	   RGBmode ();
    	   gconfig();
  	}
	
	if (flag == 0)
  	   winset (total_deformx_win);
	else
  	   winset (total_deformy_win);
  	ortho (left, right, bottom, top, near, far);

   	c3s (white);	
  	clear (); 
	if (flag == 0)
  	   makeobj (TOTAL_DEFORMX);
	else
  	   makeobj (TOTAL_DEFORMY);

	if (flag == 0)
	   determine_min_max (surface [snum].num_points, 
		total_displace [0], &dmin, &dmax);
 	else	
	   determine_min_max (surface [snum].num_points, 
		total_displace [1], &dmin, &dmax);
	/*dmin = -0.25;
	dmax = 0.25;*/
  	for (i = 0; i < surface [snum].num_points; i++){
	   if (flag == 0)
              color_index = get_lut_index (snum, 
			total_displace [0][i], dmin, dmax);
	   else
              color_index = get_lut_index (snum, 
			total_displace [1][i], dmin, dmax);
	   for (j = 0; j < 3; j++)
	      current_color [j] = lut [color_index][j+1];
	   c3s (current_color); 
#ifdef LINES
     	   for (j = 0; j < surface [snum].points [i].num_neighs; j++){
              n = surface [snum].points [i].neighs [j];
	      if (i > j){
                 bgnline();
                    v3f (surface [snum].points [i].pos);
                    v3f (surface [snum].points [n].pos);
                 endline();
              }
           }
#else
	   rectf (surface [snum].points [i].pos [0]-DIM,
	  	  surface [snum].points [i].pos [1]-DIM,
    	  	  surface [snum].points [i].pos [0]+DIM,
	  	  surface [snum].points [i].pos [1]+DIM);
#endif
  	}
  	closeobj ();
	if (flag == 0)
  	   callobj (TOTAL_DEFORMX);
	else
  	   callobj (TOTAL_DEFORMY);
  	swapbuffers ();
}

void DisplaySulci (short new, int snum, int flag)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 0, y_init = 800;
  	int	x_size = xsize*2.0, y_size = ysize*2.0;

	if (flag == 1){
	   printf ("DisplaySulci return...\n");
	   return;
	}
  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
    	   prefposition (x_init, x_init+x_size, y_init, y_init+y_size); 
    	   printf ("prefposition %d %d %d %d\n", x_init, x_init+x_size, y_init, y_init+y_size); 
	   if (flag == 0)
    	      target_win = winopen ("Target");
	   else
    	      template_win = winopen ("Tempate");
    	   doublebuffer();
    	   RGBmode ();
    	   gconfig();
  	}
	
	if (flag == 0)
  	   winset (target_win);
	else
  	   winset (template_win);
  	ortho (left, right, bottom, top, near, far);

   	c3s (white);	
  	clear (); 
	if (flag == 0)
  	   makeobj (TARGET);
	else
  	   makeobj (TEMPLATE);

  	for (i = 0; i < totpoints; i++){ 

	   c3s (red); 
	   rectf (X1 [i]-DIM*2,
		  Y1 [i]-DIM*2,
	    	  X1 [i]+DIM*2,
		  Y1 [i]+DIM*2);
	   c3s (blue); 
	   rectf (X2 [i]-DIM*2,
		  Y2 [i]-DIM*2,
	    	  X2 [i]+DIM*2,
		  Y2 [i]+DIM*2);
  	}
	/*{
	float	vert1 [2], vert2 [2];
  	for (i = 1; i < totpoints; i++){
	   c3s (red); 
	   vert1 [0] = X1 [i-1];
	   vert1 [1] = Y1 [i-1];
	   vert2 [0] = X1 [i];
	   vert2 [1] = Y1 [i];
	   bgnline ();
	      v2f (vert1);
	      v2f (vert2);
	   endline ();
	}
	}*/
	c3s (gray); 
  	/*for (i = 0; i < surface [snum].num_points; i++)
	   rectf (surface [snum].points [i].pos [0]-DIM,
	  	  surface [snum].points [i].pos [1]-DIM,
    	  	  surface [snum].points [i].pos [0]+DIM,
	  	  surface [snum].points [i].pos [1]+DIM);*/
  	closeobj ();
	if (flag == 0)
  	   callobj (TARGET);
	else
  	   callobj (TEMPLATE);
  	swapbuffers ();
}

void DisplaySulci2 (short new, int snum, int flag)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 700, y_init = 800;
  	int	x_size = xsize*2.0, y_size = ysize*2.0;
	float	x0, y0;

	if (flag == 1) return;
  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
    	   prefposition (x_init, x_init+x_size, y_init, y_init+y_size); 
	   if (flag == 0)
    	      target_win2 = winopen ("Target2");
	   else
    	      template_win = winopen ("Tempate");
    	   doublebuffer();
    	   RGBmode ();
    	   gconfig();
  	}
	
	if (flag == 0)
  	   winset (target_win2);
	else
  	   winset (template_win);
  	ortho (left, right, bottom, top, near, far);

   	c3s (white);	
  	clear (); 
	if (flag == 0)
  	   makeobj (TARGET);
	else
  	   makeobj (TEMPLATE);

  	for (i = 0; i < totpoints; i++){ 
	   c3s (red); 
	   rectf (X1 [i]-DIM*2,
		  Y1 [i]-DIM*2,
	    	  X1 [i]+DIM*2,
		  Y1 [i]+DIM*2);
	   c3s (blue); 
	   rectf (X2 [i]-DIM*2,
		  Y2 [i]-DIM*2,
	    	  X2 [i]+DIM*2,
		  Y2 [i]+DIM*2);
  	}
#ifdef READ_CELL_FILE
	for (i = 0; i < NumCells; i++){
	   if ((i % 100) == 0){
	      x0 = cells [i].pos [0];
	      y0 = cells [i].pos [1];
	      c3s (black); 
	      rectf (x0-DIM*2,
		  y0-DIM*2,
	    	  x0+DIM*2,
		  y0+DIM*2);
	   }
	}
#endif
        for (j = 0; j < n3; j++){
           for (i = 0; i < sulci [2][j].np; i++){
              x0 = sulci [2][j].x [i];
              y0 = sulci [2][j].y [i];
	      c3s (green); 
	      rectf (x0-DIM*2,
		  y0-DIM*2,
	    	  x0+DIM*2,
		  y0+DIM*2);
	   }
	}
  	closeobj ();
	if (flag == 0)
  	   callobj (TARGET);
	else
  	   callobj (TEMPLATE);
  	swapbuffers ();
}

void    read_lut_file ()
{
        FILE    *fd;
	int	i;

        if ((fd = fopen ("/v1d/heather/ANATOMY/CARET2/bb1.lut", "r")) == NULL){;
           printf ("ERROR: could not open file %s...\n", 
		"/v1d/heather/ANATOMY/CARET2/bb1.lut");
           exit (-1);
        }
        fread (lut, 1, 256*4, fd);
	/*for (i = 0; i < 256; i++)
	   printf ("LUT %2d: %d %d %d %d\n",
		i, lut [0], lut [1], lut [2], lut [3]);*/
	printf ("BB lut read in ...\n");
}

int     get_lut_index (int snum, float value, float dmin, float dmax)
{
        float   lut_scale;
        int     color_index;

        if (value < dmin)
           color_index = 0;
        else if (value > dmax)
           color_index = 255;
        else{
           lut_scale = 255.0/(dmax-dmin);
           color_index = (int)(lut_scale * (value - dmin));
        }
        return (color_index);
}
#endif /* HAVE_IRISGL */

void 	determine_min_max (int num, float *array, float *min, float *max)
{
	int	i;

	*min = *max = array [0];
	for (i = 0; i < totpoints; i++){
	   if (array [i] < *min) *min = array [i]; 
	   if (array [i] > *max) *max = array [i]; 
	}
	/*printf ("Min %.2f, Max %.2f\n", *min, *max);*/
}

/* BORDER 2 FILE FORMAT */
void	ReadPoints (char *file, int *num_sulci, int idx) 
{
	FILE	*fp;
	int	i, j;
	char	name [MAXLINE];
	int	t1, t2, t3;
	float	f1, f2, f3;
        char lineIn[MAXLINE];

	printf ("Opening file %s...\n", file);
    	if ((fp = fopen (file, "r")) == NULL){
           printf ("ERROR: Could not open file %s\n", file);
           return; 
	}
        
        /*
         * (JWH 12/05/00)
         * skip over header if present.
         * if a header is present the first line is "BeginHeader".  The
         * header is concluded with "EndHeader".
         */
        if (fgets(lineIn, MAXLINE, fp) == NULL) {
           printf("ERROR: Unable to read file %s\n", file);
           return;
        }
        if (strncmp(lineIn, "BeginHeader", 11) == 0) {
           int done = 0;
           while (done == 0) {
              if (fgets(lineIn, MAXLINE, fp) == NULL) {
                 printf("ERROR: Error while reading file %s\n", file);
                 return;
              }
              if (strncmp(lineIn, "EndHeader", 9) == 0) {
                 done = 1;
              }
           }
           /* read number of borders */
           /* fscanf (fp, "%d", &t1); */
           fgets(lineIn, MAXLINE, fp);
           sscanf(lineIn, "%d", &t1);
        }
        else {
           /* no header, 1st line is number of borders */
           sscanf(lineIn, "%d", &t1);
        }

	/* JWH 12/05/00 fscanf (fp, "%d", &t1); */
        
	*num_sulci = t1;
	printf ("\t%d sulci to read...\n", *num_sulci);

        if (t1 > MAX_SULCI) {
           printf("\n\n MAX_SULCI = %d exceeded\n", MAX_SULCI);
           printf("Number of borders from file %s is %d\n\n\n", file, t1);
           exit(-1);
        }
        
	for (j = 0; j < *num_sulci; j++){
           float sd, vr, numScanned;
	   /* fscanf (fp, "%d %d %s", &t1, &t2, name);  */
           fgets(lineIn, MAXLINE, fp);
           numScanned = sscanf(lineIn, "%d %d %s %f %f", 
                               &t1, &t2, name, &sd, &vr);
           if (numScanned != 5) {
              sd = DEFAULT_SAMPLING_DENSITY;
              vr = DEFAULT_VARIANCE;
           }
	   /*printf ("\tsulcus %d %d %s\n", j, t2, name);*/
	   /*if (t2 > 0)
	      printf ("\t%d %d %s\n", j, t2, name);*/
	   /* fscanf (fp, "%f %f %f", &f1, &f2, &f3); */
           fgets(lineIn, MAXLINE, fp);
           sscanf (lineIn, "%f %f %f", &f1, &f2, &f3);
	   strcpy (sulci [idx][j].name, name);
           sulci[idx][j].ddd = sd;
           sulci[idx][j].variance = vr;
	   sulci [idx][j].np = t2;
	   if (idx != 2){
#define FILT
#ifdef FILT
	   if ( (strncmp(name, "morph.", 6) == 0) ||
                (strncmp(name, "MORPH.", 6) == 0) ||
                (strncmp(name, "LANDMARK", 8) == 0) ||
                (strncmp(name, "landmark", 8) == 0)){
	      printf ("\t***USING %s; %d\n", name, t2); 
	      sulci [idx][j].np = t2;
	   }
	   else {
	      printf ("\tNOT USING %s\n", name); 
	      sulci [idx][j].np = 0;
	   }
#endif
	   }
	   else{
	      /*sulci [idx][j].np = t2;
	      printf ("\t%d %d %s\n", j, t2, name);*/
	   }
	   if (t1 > MAX_SULCI){
	      printf ("ERROR: Exceeded max # of sulci %d, max %d\n", t2, MAX_SULCI);
	      exit (-1);
	   }
           if (t2 > MAX_FIDUCIAL_POINTS) {
              printf("\n\n  MAX_FIDUCIAL_POINTS = %d exceeded\n", MAX_SULCI);
              printf("Number of border links in bordr %s is %d\n\n\n", name, t2);
              exit(-1);
           }
	   for (i = 0; i < t2; i++){
#ifdef NEW_BORDERS
	      /* fscanf (fp, "%d %d %f %f %f", &t1, &t3, &f1, &f2, &f3); */
              fgets(lineIn, MAXLINE, fp);
              sscanf (lineIn, "%d %d %f %f %f", &t1, &t3, &f1, &f2, &f3);
#else
	      /*if (idx == 2) 
	         fscanf (fp, "%d %d %f %f %f", &t1, &t3, &f1, &f2, &f3);
	      else
	         fscanf (fp, "%d %f %f %f", &t1, &f1, &f2, &f3);*/
	      /* fscanf (fp, "%d %f %f %f", &t1, &f1, &f2, &f3); */
              fgets(lineIn, MAXLINE, fp);
              sscanf (lineIn, "%d %f %f %f", &t1, &f1, &f2, &f3);
#endif
	      /*printf ("\t\t%d %f %f\n", i, f1, f2);*/
#ifdef IMAGE_SCALE
	      /*if ((idx == 0) || (idx == 1) || (idx == 2)){
	         f1 *= 0.3;
	         f2 *= 0.3;
	      }*/
#endif
	      /*if (idx == 1){
	         f1 *= 3;
	         f2 *= 3;
	      }*/
	      /*if (idx == 1){
	         f1 += 80;
	         f2 += 80;
	      }*/
	      /*f1 *= 2;
	      f2 *= 2;*/
	      sulci [idx][j].x [i] = f1 * SCALE_FACTOR;
	      sulci [idx][j].y [i] = f2 * SCALE_FACTOR;
	   }
	}
	fclose (fp);
}

int	find_sulcus (int num, char *name, int idx)
{
	int	i, n;
	
	for (i = 0; i < num; i++){
	   if (sulci [idx][i].np > 0){
	      n = strcmp (sulci [idx][i].name, name);
	      /*printf ("\t %s %s\n", sulci [idx][i].name, name);*/
	      if (n == 0){
	         return (i);
	      }
	   }
	}
	printf ("ERROR: Corresponding sulcus for %s not found!\n", name);
	/*exit (-1);	*/
	return (-1);
}

/* BORDER 2 FILE FORMAT */
void	WritePoints (char *file, int num_sulci, int idx) 
{
	FILE	*fp;
	int	i, j;
	char	name [MAXLINE];
	int	t1, t2;
	float	f1, f2, f3;

	printf ("Opening file %s...\n", file);
    	if ((fp = fopen (file, "w")) == NULL){
           printf ("ERROR: Could not open file %s\n", file);
           return; 
	}
	fprintf (fp, "%d\n", num_sulci);
	for (j = 0; j < num_sulci; j++){
	   fprintf (fp, "%d %d %s\n", j, sulci [idx][j].np, sulci [idx][j].name); 
	   /*printf ("WRITE %d %d %s\n", j, sulci [idx][j].np, sulci [idx][j].name); */
	   fprintf (fp, "%f %f %f\n", 0.0, 0.0, 0.0); 
	   for (i = 0; i < sulci [idx][j].np; i++){
#ifdef IMAGE_SCALE
	      f1 = ImageX2Transform (sulci [idx][j].x [i]);
	      f2 = ImageX2Transform (sulci [idx][j].y [i]);
#else
	      f1 = sulci [idx][j].x [i];
	      f2 = sulci [idx][j].y [i];
#endif
              f1 = f1 / SCALE_FACTOR;
              f2 = f2 / SCALE_FACTOR;
	      fprintf (fp, "%d %d %f %f %f\n", i, -1, f1, f2, 0.0);
	   }
	}
	fclose (fp);
}

float	DetermineVariance (char *name, float var)
{
	int	i, tst;
	float	varout;

	for (i = 0; i <  NUM_SULCI_TYPE; i++){
	   tst = strncmp (name, sulcivar [i].name, strlen (sulcivar [i].name));
	   if (tst == 0){
	      /*printf ("var is %f times %f = %f\n", var, sulcivar [i].var_factor, varout);*/
	      varout = var * sulcivar [i].var_factor;
	      return (varout);
	   }
	}
	printf ("Error: Can't find variance for sulci type %s\n", name);
        return (1.0);
	return (0.01);
	varout = var * 1.0; 
	exit (-1);
	return (varout);
}

#ifdef HAVE_IRISGL

void DisplayImage (short new, int snum, unsigned char *image)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 0, y_init = 0;
	unsigned	long *parray;

        parray = (unsigned long *)malloc (ixsize*iysize*sizeof (unsigned long));
	for (j = 0; j < iysize; j++)
	   for (i = 0; i < ixsize; i++)
	      parray [(j*ixsize)+i] = image [(j*ixsize)+i]; 
  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
    	   prefposition (x_init, x_init+ixsize, y_init, y_init+iysize); 
    	   printf ("prefposition %d %d %d %d\n", x_init, x_init+ixsize, y_init, y_init+iysize); 
    	   winid1 = winopen ("Original Image");
	   RGBmode ();
    	   doublebuffer();
    	   gconfig();
  	}
  	winset (winid1);
    	RGBcolor (0, 0, 0);
    	clear ();
  	/*ortho (left, right, bottom, top, near, far);*/
  	ortho (0, ixsize, 0, iysize, -10000, 10000);

	/*rectzoom (2.0, 2.0);*/
	lrectwrite (0, 0, ixsize-1, iysize-1, parray);

        for (i = 0; i < totpoints; i++){
           c3s (white);
	   /*printf ("OrigImage: %d %.2f %.2f\n", i, X1 [i], Y1 [i]); */
           rectf (X1 [i]-DIM*2,
                  Y1 [i]-DIM*2,
                  X1 [i]+DIM*2,
                  Y1 [i]+DIM*2);
        }
  	swapbuffers ();
}

void DisplayDeformedImage (short new, unsigned char *image)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 400, y_init = 0;
	unsigned	long	*larray;
	float	zfactor = 3.0;

	if ((ixsize > 0) && (iysize > 0)){
	printf ("Display deformed image %d %d...\n", ixsize, iysize);
        larray = (unsigned long *)malloc (ixsize*iysize*sizeof (unsigned long));
	for (j = 0; j < iysize; j++){
	   for (i = 0; i < ixsize; i++){
	      larray [(j*ixsize)+i] = (image [(j*ixsize)+i] | (image [(j*ixsize)+i] << 8) | (image [(j*ixsize)+i] << 16));
	      /*larray [(j*ixsize)+i] = (image [(j*ixsize)+i]); */
	   }
	}
  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
    	   prefposition (x_init, x_init+ixsize*zfactor, y_init, y_init+iysize*zfactor); 
    	   winid2 = winopen ("Image");
	   RGBmode ();
    	   doublebuffer();
    	   gconfig();
  	}
  	winset (winid2);
    	clear ();
  	ortho (left, right, bottom, top, -10000, 10000); 
        rectzoom (zfactor, zfactor);
	lrectwrite (0, 0, ixsize-1, iysize-1, larray);

        for (i = 0; i < totpoints; i++){
	   c3s (red); 
           rectf (X1 [i]-DIM*2,
                  Y1 [i]-DIM*2,
                  X1 [i]+DIM*2,
                  Y1 [i]+DIM*2);
	   /*c3s (blue); 
           rectf (X2 [i]-DIM*2,
                  Y2 [i]-DIM*2,
                  X2 [i]+DIM*2,
                  Y2 [i]+DIM*2);*/
        }
  	swapbuffers ();
	}
}

void cmodeDisplayDeformedImage (short new, unsigned char *image)
{
  	int 	i, j, n;
  	short	val;
  	int	x_init = 400, y_init = 0;
	unsigned	short	*sarray;

	printf ("Display deformed image %d %d...\n", ixsize, iysize);
        sarray = (unsigned short *)malloc (ixsize*iysize*sizeof (unsigned short));
	for (j = 0; j < iysize; j++){
	   for (i = 0; i < ixsize; i++){
	      /*larray [(j*ixsize)+i] = i % 255;*/
	      sarray [(j*ixsize)+i] = image [(j*ixsize)+i]; 
	   }
	}
  	if (new){
    	   foreground ();
    	   keepaspect ((right-left),(top-bottom));
    	   prefposition (x_init, x_init+ixsize, y_init, y_init+iysize); 
    	   winid2 = winopen ("Image");
	   cmode ();
    	   doublebuffer();
    	   gconfig();
  	}
  	winset (winid2);
	for (i = 0; i < 256; i++)
	   mapcolor (i, i, i, i);
    	clear ();
  	ortho (left, right, bottom, top, -10000, 10000); 
	rectwrite (0, 0, ixsize-1, iysize-1, sarray);

        for (i = 0; i < totpoints; i++){
	   color (255);
           rectf (X1 [i]-DIM*2,
                  Y1 [i]-DIM*2,
                  X1 [i]+DIM*2,
                  Y1 [i]+DIM*2);
        }
  	swapbuffers ();
}
#endif /* HAVE_IRISGL */

#ifdef READ_CELL_FILE
void	ReadCellFile (char *file)
{
	int	i, j;
	char	line [MAXLINE];
	int	t1, t2, t3;
	char	s1 [MAXLINE], s2 [MAXLINE];
	float	f1, f2, f3;
	FILE	*fp;
	int	num;

  	if ((fp = fopen (file, "r")) == NULL){;
     	   printf ("ERROR: (ReadCellFile) could not open file %s\n", file);
     	   return; 
  	}
  	fscanf (fp, "%d", &NumCells); 

	printf ("%d cells to read from %s...\n", NumCells, file); 
	if (NumCells > MAX_NUM_CELLS){
     	   printf ("ERROR: %d > MAX_NUM_CELLS %d...\n", NumCells, MAX_NUM_CELLS);	
           exit (-1); 
	}
	for (i = 0; i < NumCells; i++){
  	   fgets (line, MAXLINE, fp);
  	   num = sscanf (line, "%d %d %d %s %s %f %f %f", 
		&t1, &t2, &t3, s1, s2, &f1, &f2, &f3);
	   if ((i % 10000) == 0)
	      printf ("\t%5d of %5d\n", i, NumCells);

	   strcpy (cells [i].area1, s1);
	   strcpy (cells [i].area2, s2);
	   cells [i].section = t2;
	   cells [i].point = t3;
	   cells [i].pos [0] = f1;
	   cells [i].pos [1] = f2;
	   cells [i].pos [2] = f3;
	}
	fclose (fp);
}

void	WriteCellFile (char *file)
{
	int	i, j;
	char	line [MAXLINE];
	int	t1, t2, t3;
	char	s1 [MAXLINE], s2 [MAXLINE];
	float	f1, f2, f3;
	FILE	*fp;
	int	num;

  	if ((fp = fopen (file, "w")) == NULL){;
     	   printf ("ERROR: (WriteCellFile) could not open file %s\n", file);
     	   return; 
  	}
  	fprintf (fp, "%d\n", NumCells); 

	printf ("%d cells to write to %s...\n", NumCells, file); 
	for (i = 0; i < NumCells; i++){
	   if ((i % 10000) == 0)
	      printf ("\t%5d of %5d\n", i, NumCells);
	   fprintf (fp, "%d %d %d %s %s %f %f %f\n",
		i, cells [i].section, cells [i].point, cells [i].area1, cells [i].area2,
		cells [i].pos [0], cells [i].pos [1], cells [i].pos [2]);
	}
	fclose (fp);
}
#endif  /* READ_CELL_FILE */

void calculate_weightsFLUID (int totpoints, int dim, double *mat,
	double *X1, double *Y1, double *X2, double *Y2)
	{
int INFO,LDA,LDB,LWORK,N,NRHS;
char UPLO;
int IPIV[MAX_FIDUCIAL_POINTS];
double work[MAX_FIDUCIAL_POINTS];
int     i, j;
float   t1;

	/* Now calculate the weights  */
	/* This is done by solving a liner equation */
	/* First fill the part of the covariance function */

	/*for(j=0;j<totpoints;j++) {
		printf("%lf %lf %lf %lf\n",X1[j],Y1[j],X2[j],Y2[j]);
		}*/
	for(i=0;i<totpoints;i++)
	{
		for(j=0;j<totpoints;j++)
	{ 
	/* The matrix is just the distance between the points i,j */

	t1 = sqrt((X1[i] -X1[j])*(X1[i]-X1[j])+
                    (Y1[i]-Y1[j])*(Y1[i]-Y1[j]));

	*(mat+j+i*dim) = exp(-alpha*t1);
	}
	*(mat+totpoints+i*dim) = X1[i];
	*(mat+totpoints+1+i*dim) = Y1[i];
	*(mat+totpoints+2+i*dim) = 1; 
	*(mat+totpoints*dim+i) = X1[i];
	*(mat+(totpoints+1)*dim+i) = Y1[i];
	*(mat+(totpoints+2)*dim+i) = 1;
	}

	/* Print the matrix  */

/*	for (i=0;i<dim;i++)
	{
		for (j=0;j<dim;j++)
		{
		printf("%lf ",*(mat+(i*dim+j)));
		}
	printf("\n");
	}
*/

	/* Set the weights to the given displacement
		field and than solve the equation */

	for(i=0;i<totpoints;i++){
	wtx[i] = X2[i]-X1[i];
	wty[i] = Y2[i]-Y1[i];
	}
	wtx[totpoints] = 0;
	wtx[totpoints+1] = 0;
	wtx[totpoints+2] = 0;

	wty[totpoints] = 0;
	wty[totpoints+1] = 0;
	wty[totpoints+2] = 0;

	/* Set up the stuff for LAPACK routines */
	
	UPLO = 'U';
	INFO = 1;
	LDA = dim;
	LDB = dim;
	LWORK = 8;
	N = dim;
	NRHS = 1;

	/*printf("Calling SSYTRS \n");*/

	dsytrf_(&UPLO,&N,mat,&LDA,IPIV,work,&LWORK,&INFO);
	
	/*printf("Info after ssytrf =%d\n",INFO);*/

	dsytrs_(&UPLO,&N,&NRHS,mat,&LDA,IPIV,wtx,&LDA,&INFO);
	/*printf("Info after ssytrs =%d\n",INFO);*/
	dsytrs_(&UPLO,&N,&NRHS,mat,&LDA,IPIV,wty,&LDA,&INFO);
	/*printf("Info after ssytrs =%d\n",INFO);*/

	/* Now print the weights 
	for(i=0;i<dim;i++) {
		printf("Wtx[%d] = %lf \n",i,wtx[i]);
		printf("Wty[%d] = %lf \n",i,wty[i]);
	}

	tot1 =0;
	tot2 =0;
	tot3 =0;

	for(i=0;i<totpoints;i++)
	{
		tot1 = tot1+wtx[i];
		tot2 = tot2+X1[i]*wtx[i];
		tot3 = tot3+X1[i]*wty[i];
	}

	printf("Sum of weights = %lf\n",tot1);
	printf("Sum of X1*weights = %lf\n",tot2);
	printf("Sum of X1*wy = %lf\n",tot3);

*/
		printf("\tCalculated Weights \n");
}

void findpath (double *X1,double *Y1,double *X2,double *Y2, int totpoints)
{
int i,j,k,t;
int iter;
double dist;
double *K1;
double *K2;
/* Tmp variables for the finite differances  and gradiant calculations */
double *v1x,*v2x,*v3x;
double *v1y,*v2y,*v3y;
double tmp,tmpx,tmpy;
int dimK;
int totstep;
int INFO,LDA,LDB,LWORK,N,NRHS;
char UPLO;
int IPIV[MAX_FIDUCIAL_POINTS];
double work[MAX_FIDUCIAL_POINTS];
float u[3];
long xorigin, yorigin, xsize, ysize;
float rx, ry;
double prev_cost, cost;


/*beta = 0.001;*/
/*beta = 10.0; */


/* Estimate the initial path */

printf ("Estimate initial path...\n");
for (i=0;i<totpoints;i++){
	dist = sqrt((X1[i]-X2[i])*(X1[i]-X2[i])+(Y1[i]-Y2[i])*(Y1[i]-Y2[i]));	
	for(j=0;j<NUMT;j++){
/*#define CASE1*/
#ifdef CASE1
	pathx[j][i] = X2 [i]; 
	pathy[j][i] = Y2 [i];
#else
	pathx[j][i] = (j/(NUMT-1.0))*(X2[i]-X1[i])+X1[i]; 
	pathy[j][i] = (j/(NUMT-1.0))*(Y2[i]-Y1[i])+Y1[i]; 
	/*{
	float 	frac = 0.80;
	pathx[j][i] = (j/(NUMT-1.0))*(frac*(X2[i]-X1[i]))+X1[i]; 
	pathy[j][i] = (j/(NUMT-1.0))*(frac*(Y2[i]-Y1[i]))+Y1[i]; 
	}*/
#endif
		}
	/*pathx[0][i] = X1[i];
	pathy[0][i] = X1[i];*/
	
	}
printf ("\tdone...\n");

/* Now allocate the space for the matirx K */

dimK = totpoints+3;

totstep = NUMT;

printf ("allocate...\n");
K1 = (double *)calloc(dimK*dimK,sizeof(double));
K2 = (double *)calloc(dimK*dimK,sizeof(double));
v1x = (double *)calloc(dimK,sizeof(double));
v2x = (double *)calloc(dimK,sizeof(double));
v3x = (double *)calloc(totstep*dimK,sizeof(double));

v1y = (double *)calloc(dimK,sizeof(double));
v2y = (double *)calloc(dimK,sizeof(double));
v3y = (double *)calloc(totstep*dimK,sizeof(double));
printf ("\tdone...\n");

/* Now start the iterations */
/* Now do the graphics */

#ifdef HAVE_IRISGL
if (graphics == 1) {
printf("Graphics Enabled\n");

	foreground();
        winopen("Visulize Paths of points");

	getorigin(&xorigin, &yorigin);
        getsize(&xsize, &ysize);
        RGBmode();
        doublebuffer();
        gconfig();
        lsetdepth(getgdesc(GD_ZMIN), getgdesc(GD_ZMAX));
        zbuffer(1);
        mmode(MVIEWING);
        loadmatrix(Identity);
        /* perspective(900, xsize/(float)ysize, .25, 600.0); */

	/*ortho(-320,320,-240,240,-320,320);*/
	ortho(-160,160,-120,120,-320,320);

	/* translate(-0.5, -0.5, -300); */
}
#endif /* HAVE_IRISGL */

    	if ((fd_iter = fopen ("iter.dat", "w")) == NULL){
           printf ("ERROR: Could not open file %s\n", "iter.dat");
           exit (-1);; 
	}
	fprintf (fd_iter, "beta %f\n", beta);
cost = 0;
for (iter=0;iter<numiter;iter++){
	/* Given the path find the derivative at each time point */

	/* Now do this for each time step */
	/* Note that the initial and the final points are fixed */
	/* First for the X and than the Y */

	prev_cost = cost;		
	cost = 0;

	for(t=1;t<totstep-1;t++){
	   /*printf ("\t\titer %d, step %d of %d\n", iter, t, totstep);*/
	
		for(i=0;i<totpoints;i++){
		/* Fill v1 with the n^{t+1}-n^{t} */ 
			v1x[i] = pathx[t+1][i] - pathx[t][i];
			v1y[i] = pathy[t+1][i] - pathy[t][i];
		/* Fill v2 with the n^{t}-n^{t-1} */ 
			v2x[i] = pathx[t][i] - pathx[t-1][i];
			v2y[i] = pathy[t][i] - pathy[t-1][i];
			}
	
		v1x[totpoints] = 0;
	        v1x[totpoints+1] = 0;
       		v1x[totpoints+2] = 0;
        	v1y[totpoints] = 0;
        	v1y[totpoints+1] = 0;
        	v1y[totpoints+2] = 0;

		v2x[totpoints] = 0;
	        v2x[totpoints+1] = 0;
       		v2x[totpoints+2] = 0;
        	v2y[totpoints] = 0;
        	v2y[totpoints+1] = 0;
        	v2y[totpoints+2] = 0;

	/* Now fill the covariance matrix K(n_{i}^{t},n_{j}^{t})
	   and K(n_{i}^{t-1},n_{j}^{t-1})
	*/

	/* Clear the matrix K1 and K2 */

	for (i=0;i<dimK*dimK;i++){
		*(K1+i) = 0;
		*(K2+i) = 0;
	}

	for(i=0;i<totpoints;i++) {
		for(j=0;j<totpoints;j++){
	tmp = sqrt((pathx[t][i]-pathx[t][j])*(pathx[t][i]-pathx[t][j])+
	    (pathy[t][i]-pathy[t][j])*(pathy[t][i]-pathy[t][j]));
	
	if (i != j) *(K1+j+i*dimK) = exp(-alpha*tmp);

	if (i == j ) *(K1+j+i*dimK) = 1.0;
		
	tmp = sqrt((pathx[t-1][i]-pathx[t-1][j])*(pathx[t-1][i]-pathx[t-1][j])+
	    (pathy[t-1][i]-pathy[t-1][j])*(pathy[t-1][i]-pathy[t-1][j]));

	if (i != j ) *(K2+j+i*dimK) = exp(-alpha*tmp);

	if (i == j ) *(K2+j+i*dimK) = 1.0;

	}
	*(K1+totpoints+i*dimK) = pathx[t][i];
        *(K1+totpoints+1+i*dimK) = pathy[t][i];
        *(K1+totpoints+2+i*dimK) = 1;
        *(K1+totpoints*dimK+i) = pathx[t][i];
        *(K1+(totpoints+1)*dimK+i) = pathy[t][i];
        *(K1+(totpoints+2)*dimK+i) = 1;

	*(K2+totpoints+i*dimK) = pathx[t-1][i];
        *(K2+totpoints+1+i*dimK) = pathy[t-1][i];
        *(K2+totpoints+2+i*dimK) = 1;
        *(K2+totpoints*dimK+i) = pathx[t-1][i];
        *(K2+(totpoints+1)*dimK+i) = pathy[t-1][i];
        *(K2+(totpoints+2)*dimK+i) = 1;
        }

	/* print the MAtrix K1 */
	
/*for (i=0;i<dimK;i++)
        {
                for (j=0;j<dimK;j++)
                {
                printf("%lf ",*(K1+(i*dimK+j)));
                }
        printf("\n");
        }*/
	

	/* Now compute K1^{-1}v1x and K1^{-1}v1y  using LAPACK */

	UPLO = 'U';
        INFO = 1;
        LDA = dimK;
        LDB = dimK;
        LWORK = 8;
        N = dimK;
        NRHS = 1;

        /*printf("Calling SSYTRS 1\n");*/
        dsytrf_(&UPLO,&N,K1,&LDA,IPIV,work,&LWORK,&INFO);
        /*printf("Calling SSYTRS 2\n");*/
        dsytrs_(&UPLO,&N,&NRHS,K1,&LDA,IPIV,v1x,&LDA,&INFO);
        /*printf("Calling SSYTRS 3\n");*/
        dsytrs_(&UPLO,&N,&NRHS,K1,&LDA,IPIV,v1y,&LDA,&INFO);

	 /* Now compute K2^{-1}v2x and K2^{-1}v2y  using LAPACK */
	UPLO = 'U';
        INFO = 1;
        LDA = dimK;
        LDB = dimK;
        LWORK = 8;
        N = dimK;
        NRHS = 1;

        /*printf("Calling SSYTRS 4\n");*/
        dsytrf_(&UPLO,&N,K2,&LDA,IPIV,work,&LWORK,&INFO);

        /*printf("Info after ssytrf =%d\n",INFO); */
        dsytrs_(&UPLO,&N,&NRHS,K2,&LDA,IPIV,v2x,&LDA,&INFO);
        dsytrs_(&UPLO,&N,&NRHS,K2,&LDA,IPIV,v2y,&LDA,&INFO);

	/* Now fill the gradiant vector */
	for (i=0;i<dimK;i++){
	   v3x[i+t*dimK] = -2*(v1x[i] - v2x[i]);
	   v3y[i+t*dimK] = -2*(v1y[i] - v2y[i]);
	}

	if ( t == totstep-2 ) {
		for (i=0;i<dimK;i++){
           	v3x[i+(t+1)*dimK] = 2*v2x[i] + 2*(pathx[t+1][i]-X2[i])/VAR [i];
           	v3y[i+(t+1)*dimK] = 2*v2y[i] + 2*(pathy[t+1][i]-Y2[i])/VAR [i];
        	}
	}

	
		

	/* Now for the last term */
	for (i=0;i<totpoints;i++){
	   tmpx = 0;
	   tmpy = 0;
	   for(j=0;j<totpoints;j++) {
	      tmp = sqrt((pathx[t][i]-pathx[t][j])*(pathx[t][i]-pathx[t][j])+
            	     (pathy[t][i]-pathy[t][j])*(pathy[t][i]-pathy[t][j]));

	      if (i != j) {
	         tmpx = tmpx + v1x[j]*(+alpha*exp(-alpha*tmp)/(2*tmp)*
		(pathx[t][i]-pathx[t][j]));
		 tmpy = tmpy + v1y[j]*(+alpha*exp(-alpha*tmp)/(2*tmp)*
                (pathy[t][i]-pathy[t][j]));
	      }
	   }
	   tmpx = tmpx - v1x[totpoints];
	   tmpy = tmpy - v1y[totpoints+1];

	   /*printf("%d) tmpx = %lf; tmpy = %lf\n",i,tmpx,tmpy);
	   printf("%d) v1x = %lf; v1y = %lf\n",i,v1x[i],v1y[i]);
	   printf("%d) v2x = %lf; v2y = %lf\n",i,v2x[i],v2y[i]);*/

	   v3x[i+t*dimK] = v3x[i+t*dimK] + v1x[i]*tmpx;
	   v3y[i+t*dimK] = v3y[i+t*dimK] + v1y[i]*tmpy;

	   /*printf("%d) delx = %lf,dely = %lf\n",i, v3x[i+t*dimK],v3y[i+t*dimK]);*/
	}

	for (i=0;i<totpoints;i++){
	   float tmp;
	   cost = cost + (pathx[t+1][i] - pathx[t][i])*v1x[i]
	            + (pathy[t+1][i] - pathy[t][i])*v1y[i];
	   /*printf ("\tcost %d %.2lf %.2lf, %lf %lf %lf %lf\n", 
		i, tmp, cost, pathx [t+1][i], v1x [i], pathy [t+1][i], v1y [i]);*/
	}

	/* Now add the cost of the first point */

	if (t == 1) {
	for (i=0;i<totpoints;i++){
		cost = cost + (pathx[t][i] - pathx[t-1][i])*v2x[i]
		    + (pathy[t][i] - pathy[t-1][i])*v2y[i];
		    }
	}

	/* add the cost for the last point */

	if (t == totstep-2){
	for (i=0;i<totpoints;i++){
		cost = cost + (pathx[t+1][i] - X2[i])*(pathx[t+1][i] - X2[i])/VAR [i]+
			(pathy[t+1][i] - Y2[i])*(pathy[t+1][i] - Y2[i])/VAR [i];
		}
	}	
	
	}

	if ((iter % 1) == 0)
	   printf ("\tIteration %4d of %4d, cost %lf\n", iter, numiter, cost);
	fprintf (fd_iter, "%d %.3f %.3f %.2f\n", 
		iter, cost, prev_cost-cost, (prev_cost-cost)/cost*100.0);
 	fflush (fd_iter);	
	/*printf("(%d),Cost = %lf\n",t,cost);*/

	/* Now update the points */

	for(t=1;t<totstep;t++)
		for (i=0;i<totpoints;i++){
	pathx[t][i] = pathx[t][i] - beta*v3x[i+t*dimK];
	pathy[t][i] = pathy[t][i] - beta*v3y[i+t*dimK];
	}

#ifdef HAVE_IRISGL
	if (graphics == 1) {
		ry = 300 * (2.0*(getvaluator(MOUSEX)-512)/xsize-1.0);
                rx = -300 * (2.0*(getvaluator(MOUSEY)-512)/ysize-1.0);
                czclear(0x404040, getgdesc(GD_ZMAX));
                pushmatrix();
                rot(ry, 'y');
                rot(rx, 'x');
                translate(-160.0, -120.0, -50);
                /* Now draw the paths */

		for (i=0;i<totpoints;i++){
			bgnline();	
			for(t=0;t<totstep;t++){
			u[0] = pathx[t][i]*0.5;
			u[1] = pathy[t][i]*0.5;
			u[2] = t*10;
			v3f(u);
			}
			endline();
		}

                popmatrix();
                swapbuffers();
	}
#endif  /* HAVE_IRISGL */
	}
	fclose (fd_iter);
}

double	ImageY2Transform (double val)
{
	double	answer = val;
	answer -= EXTENT/2;
	return (answer);
}

double	ImageX2Transform (double val)
{
	double	answer = val;
	answer -= EXTENT/2;
	return (answer);
}

double	TransformX2Image (double val)
{
	double	answer = val;
	/*answer = (val + 49.0)*(800.0/100.0);*/
	/*answer = (val + 49.0)*((float)EXTENT/100.0);*/
	answer += EXTENT/2;
	return (answer);
}

double	TransformY2Image (double val)
{
	double	answer = val;
	/*answer = (val + 45.5)*(800.0/100.0);*/
	/*answer = (val + 45.5)*((float)EXTENT/100.0);*/
	answer += EXTENT/2;
	return (answer);
}

void	CreateSurface (int width, int height, int spacing)
{
	int	i, j;
	int	npoints = 0;
	int	snum = 0, n;
	int	w, h;

#ifdef DEFORM_IMAGE
	for (j = 0; j < height; j+=spacing){
	   for (i = 0; i < width; i+=spacing){
     	      surface [snum].points [npoints].num_neighs = 0;
	      surface [snum].points [npoints].pos [0] = (float)i;
	      surface [snum].points [npoints].pos [1] = (float)j;
	      surface [snum].points [npoints].pos [2] = 0.0;
              /*surface [snum].points [npoints].neighs [0] = npoints-1;
     	      surface [snum].points [npoints].num_neighs++;
              surface [snum].points [npoints].neighs [1] = npoints+1;
     	      surface [snum].points [npoints].num_neighs++;*/
	      npoints++;
	   }
	}
	surface [snum].num_points = npoints;
	printf ("Created surface with %d points...\n", npoints);
#else
	/*printf ("%d %d %d %d\n", -height/2, height/2, spacing, height/spacing); */
 	w = width/spacing;
	h = height/spacing;	
	for (j = -height/2; j < height/2; j+=spacing){
	   for (i = -width/2; i < width/2; i+=spacing){
     	      /*surface [snum].points [npoints].num_neighs = 0;*/
	      surface [snum].points [npoints].pos [0] = (float)i;
	      surface [snum].points [npoints].pos [1] = (float)j;
	      surface [snum].points [npoints].pos [2] = 0.0;
	      surface [snum].points [npoints].section = i; 
	      surface [snum].points [npoints].point = j; 
	      if (i > -width/2){
	         n = surface [snum].points [npoints].num_neighs;
                 surface [snum].points [npoints].neighs [n] = npoints-1;
     	         surface [snum].points [npoints].num_neighs++;
	      }
	      if (i < width/2-spacing){
	         n = surface [snum].points [npoints].num_neighs;
                 surface [snum].points [npoints].neighs [n] = npoints+1;
     	         surface [snum].points [npoints].num_neighs++;
	      }
	      if (j > -height/2){
	         n = surface [snum].points [npoints].num_neighs;
                 surface [snum].points [npoints].neighs [n] = npoints-w;
     	         surface [snum].points [npoints].num_neighs++;
	         /*printf ("point %d %d %d, neigh %d gets %d\n", npoints, i, j, n, npoints-w);*/
	      }
	      if (j < height/2-spacing){ 
	         n = surface [snum].points [npoints].num_neighs;
                 surface [snum].points [npoints].neighs [n] = npoints+w;
     	         surface [snum].points [npoints].num_neighs++;
	         /*printf ("point %d %d %d, neigh %d gets %d\n\n", npoints, i, j, n, npoints+w);*/
	      }
	      npoints++;
	   }
	}
	surface [snum].num_points = npoints;
	printf ("Created surface with %d points...\n", npoints);
#endif
}

void	ImageInitStuff (char *filename, int xsize, int ysize)
{
	int	i;
	FILE	*fpin;
	int	answer;

           /*left = 0;
           right = xsize;
           bottom = 0;
           top = ysize;*/
           printf ("Deforming image %s also %d %d...\n", filename, xsize, ysize);

           /* alocate the memory */
           inimage = (char *)calloc (xsize*ysize,1);
           outimage = (char *)calloc (xsize*ysize,1);
           def_out = (char *)calloc (xsize*ysize,1);
           image_xdeform = (float *)malloc (xsize*ysize*sizeof(float));
           image_ydeform = (float *)malloc (xsize*ysize*sizeof(float));
           for (i = 0; i < xsize*ysize; i++){
              outimage [i] = 0;
              image_xdeform [i] = 0.0;
              image_ydeform [i] = 0.0;
           }
           if ((fpin = fopen (filename, "r")) == NULL){
              printf ("Could not open file %s\n", filename);
              for (i = 0; i < xsize*ysize; i++)
                 inimage [i] = 128;
	      CreateGridImage (inimage, xsize, ysize);
              /*DisplayDeformedImage (True, inimage);
	while (done == False){
           if (qtest ()){
              device = qread (&val);
              if (device == ESCKEY) 
	         done = TRUE;
           }
	}*/
           }
	   else{
              answer = fread (inimage, 1, xsize*ysize, fpin);
              printf ("after %d...\n", answer);
           }
}

void	ComputeDeformationFLUID (int step, double x0, double y0, double *defx, double *defy)
{
    	int 	tmpux,tmpuy;  /* Displacement FIELD  */
	int	k;
    	double 	tmp1, tmp2;
	double	ux, uy;

	/*Do the sumation of the Greens Functions */
	tmpux = ux = 0;
	tmpuy = uy = 0;
	for (k=0;k<totpoints;k++){
	   tmp1 = sqrt((pathx [step+1][k] - x0)*(pathx [step+1][k] - x0) +
	               (pathy [step+1][k] - y0)*(pathy [step+1][k] - y0));
	   if (tmp1 != 0){
	      tmp2 = exp(-alpha*tmp1);
	      ux = ux + wtx[k]*tmp2;
	      uy = uy + wty[k]*tmp2;
	   }
	}

	/* Now the scale and translation */
	ux = ux + wtx[k]*x0 + wtx[k+1]*y0 + wtx[k+2];
	uy = uy + wty[k]*x0 + wty[k+1]*y0 + wty[k+2];
	*defx = ux;
	*defy = uy;
}

void	ResampleData ()
{
	int		i, j, flag; 
	int		idx, new;
	double		newx [MAX_FIDUCIAL_POINTS], 
			newy [MAX_FIDUCIAL_POINTS];
	double		totdist, ddd;
	int		sulcus_count = 0;

	/* Need to resample each sulcus so it's the same number of points...*/
	for (i = 0; i < n1; i++){

	   if (sulci [0][i].np > 0){
	      /* Find the corresponding sulcus in the other list */
	      idx = find_sulcus (n2, sulci [0][i].name, 1);
	      if (idx >= 0){
	   	 char 	name [256];
		 strcpy (name, sulci [1][idx].name);
	         if ( (strncmp(name, "morph.", 6) == 0) ||
                      (strncmp(name, "MORPH.", 6) == 0) ||
                      (strncmp(name, "LANDMARK", 8) == 0) ||
                      (strncmp(name, "landmark", 8) == 0) ) {
	            ddd = 5.00;
                 }
	         else {
	            ddd = 10.00;
                 }
	         printf ("Sulcus %3d: target %10s %4d, template %d %10s %4d, %.2f %s\n",
			i, sulci [0][i].name, sulci [0][i].np, 
			idx, sulci [1][idx].name, sulci [1][idx].np, ddd, name);
	         flag = 0;
                 resample_line (sulci [0][i].x, sulci [0][i].y, sulci [0][i].np,
			newx, newy, ddd, &new, flag);
	         for (j = 1; j < new; j++){
	            X1 [totpoints+j-1] = newx [j];
	            Y1 [totpoints+j-1] = newy [j];
		    VAR [totpoints+j-1] = DetermineVariance (sulci [0][i].name, var_mult);
                    sulci [0][sulcus_count].x [j-1] = X1 [totpoints+j-1];
                    sulci [0][sulcus_count].y [j-1] = Y1 [totpoints+j-1];
	         }
	         flag = 1;
                 resample_line (sulci [1][idx].x, sulci [1][idx].y, sulci [1][idx].np,
			newx, newy, ddd, &new, flag);
	         for (j = 1; j < new; j++){
	            X2 [totpoints+j-1] = newx [j]; 
	            Y2 [totpoints+j-1] = newy [j]; 
#ifdef PIN_VISIBLE_MAN_EDGES	
	            if ((strncmp (name, "93I.MORPH.P", 7)) == 0){ 
	               X2 [totpoints+j-1] = X1 [totpoints+j-1]; 
	               Y2 [totpoints+j-1] = Y1 [totpoints+j-1]; 
		    }
#endif
	            /*printf ("\t%s: %d %.2f %.2f -> %.2f %.2f\n", name,
			totpoints+j-1, X1 [totpoints+j-1], Y1 [totpoints+j-1],
			X2 [totpoints+j-1], Y2 [totpoints+j-1]);*/
                    sulci [3][sulcus_count].x [j-1] = X2 [totpoints+j-1];
                    sulci [3][sulcus_count].y [j-1] = Y2 [totpoints+j-1];
	         }
	         /*totpoints += (new-2);
	         sulci [0][sulcus_count].np = sulci [0][i].hold_np = (new-2); 
	         sulci [3][sulcus_count].np = sulci [1][idx].hold_np = (new-2);*/

	         totpoints += (new-1);
	         sulci [0][sulcus_count].np = sulci [0][i].hold_np = new-1; 
	         sulci [3][sulcus_count].np = sulci [1][idx].hold_np = new-1;

	         strcpy (sulci [0][sulcus_count].name, sulci [0][i].name);
	         strcpy (sulci [3][sulcus_count].name, sulci [0][i].name);
	         printf ("Sulcus %d, %d points, name %s\n",
			sulcus_count, sulci [0][sulcus_count].np,
			sulci [0][sulcus_count].name);
		 sulcus_count++;
	      }
	      else{
	         printf ("Sulcus %3d NOT FOUND: target %10s %4d\n",
			i, sulci [0][i].name, sulci [0][i].np);
	         sulci [0][i].hold_np = 0; 
	      }
	   }
	}
	{
	int	 temp = 0;;
	n1 = sulcus_count;
	for (i = 0; i < n1; i++){
           printf ("Sulcus %d %s, %d points\n",
			i, sulci [0][i].name, sulci [0][i].np);  
	   temp += sulci [0][i].np;
	}
	printf ("%d total points...\n\n", temp);
	n4 = sulcus_count;
	temp = 0;
	for (i = 0; i < n4; i++){
           printf ("Sulcus %d %s, %d points\n",
			i, sulci [3][i].name, sulci [3][i].np);  
	   temp += sulci [3][i].np;
	}
	printf ("%d total points...\n", temp);
	}

#ifdef IMAGE_SCALE
	/* Rescale the input points so they are back in image coordinate system */
	for (i = 0; i < totpoints; i++){
	   X1 [i] = TransformX2Image (X1 [i]);
	   Y1 [i] = TransformY2Image (Y1 [i]);
#define JUNK
#ifdef JUNK
	   X2 [i] = TransformX2Image (X2 [i]);
	   Y2 [i] = TransformY2Image (Y2 [i]);
#endif

	   /* X1,Y1 is the TARGET */
	   /*X1 [i] = ((X1 [i] - 392/2.0)*0.70)+392/2.0 - 50*0; 
	   Y1 [i] = ((Y1 [i] - 364/2.0)*0.70)+364/2.0 - 50*0; */
	   /* X2,Y2 is the SOURCE */
	   /*X2 [i] = ((X2 [i] - 392/2.0)*0.9)+392/2.0 - 50*0; 
	   Y2 [i] = ((Y2 [i] - 364/2.0)*0.9)+364/2.0 - 50*0;*/
	}
#ifdef JUNK
	for (i = 0; i < n3; i++){
	   /*printf ("\tAreal %d %d\n", i, sulci [2][i].np);*/
           for (j = 0; j < sulci [2][i].np; j++){
	      sulci [2][i].x [j] = TransformX2Image (sulci [2][i].x [j]);
	      sulci [2][i].y [j] = TransformY2Image (sulci [2][i].y [j]);
	      /* FVE map */
	      /*sulci [2][i].x [j] = ((sulci [2][i].x [j]  - 392/2.0)*0.7)+392/2.0 - 50*0; 
	      sulci [2][i].y [j] = ((sulci [2][i].y [j]  - 364/2.0)*0.7)+364/2.0 - 50*0; */
	      /* Brodmann map */
	      /*sulci [2][i].x [j] = ((sulci [2][i].x [j]  - 392/2.0)*0.9)+392/2.0 - 50*0; 
	      sulci [2][i].y [j] = ((sulci [2][i].y [j]  - 364/2.0)*0.9)+364/2.0 - 50*0; */
	   }
	}
#endif

#endif
	printf("Total number of corespondance points = %d\n",totpoints);
	for (i = 0; i < totpoints; i++){
	   origX1 [i] = X1 [i];
	   origY1 [i] = Y1 [i];
	   origX2 [i] = X2 [i];
	   origY2 [i] = Y2 [i];
	   /*printf ("%d: %.2f %.2f -> %.2f %.2f\n", i, X1 [i], Y1 [i], X2 [i], Y2 [i]);*/
	}

        /*{
	int	cnt = 0;
	printf ("n1 is %d, n2 is %d, n3 is %d\n", n1, n2, n3);
	for (i = 0; i < n1; i++){
           sulci [0][i].np = sulci [0][i].hold_np; 
           sulci [1][i].np = sulci [0][i].hold_np;
	   strcpy (sulci [1][i].name, sulci [0][i].name);
           for (j = 0; j < sulci [0][i].np; j++){
              sulci [0][i].x [j] = X1 [cnt];
              sulci [0][i].y [j] = Y1 [cnt];
              sulci [1][i].x [j] = X2 [cnt];
              sulci [1][i].y [j] = Y2 [cnt];
	      cnt++;
           }
        }
        }*/
        WritePoints ("target.orig.dat", n1, 0);
        WritePoints ("source.orig.dat", n4, 3);
#ifndef READ_CELL_FILE
        WritePoints ("arch.orig.dat", n3, 2);
#endif

        {
        FILE *fd1, *fd2;

        fd1 = fopen ("source.dat", "w");
        fd2 = fopen ("target.dat", "w");
        for (i = 0; i < totpoints; i++){
	   fprintf (fd1, "%.2f %.2f %.2f\n", 
		X1 [i], Y1 [i], VAR [i]);
	   fprintf (fd2, "%.2f %.2f %.2f\n", 
		X2 [i], Y2 [i], VAR [i]);
        }
        fclose (fd1);
        fclose (fd2);
	}
}

void	CreateGridImage (char *inimage, int xsize, int ysize)
{
	/*unsigned	char	*inimage;*/
	int		i, j, k;
	int		ii, jj;

	/*inimage = (unsigned char *)calloc (xsize*ysize, sizeof(unsigned char));*/
	for (j = 0; j < ysize; j++) 
	   for (i = 0; i < xsize; i++)
	      inimage [(j*ixsize)+i] = 255;
	for (j = 0; j < ysize; j++){
	   for (i = 0; i < xsize; i++){
	      if ((i % 10) == 0)
	         inimage [(j*ixsize)+i] = 0;
	      if ((j % 10) == 0)
	         inimage [(j*ixsize)+i] = 0;
              for (k = 0; k < totpoints; k++){
	         if ((i == (int)X2 [k]) && (j == (int)Y2 [k])){
	            for (jj = j-1; jj < j+1; jj++){
	               for (ii = i-1; ii < i+1; ii++){
	                  inimage [(jj*ixsize)+ii] = 0;
	               }
	            }
	         }
	      }
	   }
	}
}

void	WriteImage (char *file, char *inimage, int xsize, int ysize)
{
	FILE	*fd;
	int	answer;


        if ((fd = fopen (file, "w")) == NULL){;
           printf ("ERROR: could not open file %s...\n", file);
           return; 
        }
	answer = fwrite (inimage, 1, xsize*ysize, fd);
	printf ("write %d bytes, %d %d...\n", answer, xsize, ysize);
	fclose (fd);
}

void	DeformImage (int ixsize, int iysize, int deform_flag)
{
	int	i, j, k, step;
	float	x0, y0;
    	int 	tmpux,tmpuy; 
    	double 	xdelta,ydelta,tx,ty;
    	double 	l1,l2,l3,l4;
	double	ux, uy;
	double	tmp1, tmp2;
	double 	new_pathx [NUMT][5000];
	double	new_pathy [NUMT][5000];
 
	/*printf ("Deform image %d %d...\n", ixsize, iysize);*/
	if ((deform_flag == FLUID) && (ixsize > 0) && (iysize > 0)){
        for(j=0;j<totpoints;j++){
           for(i=0;i<NUMT;i++){
	      new_pathx [i][j] = pathx [i][j];
	      new_pathy [i][j] = pathy [i][j];
	   }
	}
        for(j=0;j<totpoints;j++){
           for(i=0;i<NUMT;i++){
	      pathx [i][j] = new_pathx [NUMT-i-1][j];
	      pathy [i][j] = new_pathy [NUMT-i-1][j];
	   }
	}
	for (step = NUMT-2; step >= 0; step--){
	   calculate_weightsFLUID (totpoints, dim, mat, 
		pathx[step+1], pathy[step+1], pathx[step], pathy[step]);

	   printf ("Deform the image, step %d of %d...\n", step, NUMT-1);
	   for (j = 0; j < iysize; j++){
	      if ((j % 100) == 0)
	         printf ("\t%d of %d\n", j, iysize);
	      for (i = 0; i < ixsize; i++){
	         x0 = i;
	         y0 = j;
	         ComputeDeformationFLUID (step, x0, y0, &ux, &uy); 
		 tmpux = (int)floor(ux);
		 tmpuy = (int)floor(uy);
		 xdelta = ux-(double)tmpux;
		 ydelta = uy-(double)tmpuy;
		 tx = 1.0-xdelta;
		 ty = 1.0-ydelta;

		 /* Now do the bi-linear interpolation
		 *       1--2
		 *       |  |
		 *       4--3
		 Only I and Gary Know what this is!! */


		 l1=l2=l3=l4=0.0;
		 k = ((j+tmpuy)*ixsize)+(i+tmpux);
		 if (((j+tmpuy)>=0)&((i+tmpux)>=0)&((i+tmpux)<ixsize)&((j+tmpuy)<iysize))
	   	    l1 = inimage[k];
	 	 if (((j+tmpuy)>=0)&((i+tmpux+1)>=0)&((i+tmpux+1)<ixsize)&((j+tmpuy)<iysize)) 
	   	    l2 = inimage [k+1];
		 if (((j+tmpuy+1)>=0)&((i+tmpux+1)>=0)&((i+tmpux+1)<ixsize)&((j+tmpuy+1)<iysize)) 
	            l3 = inimage [k+1+ixsize]; 
	         if (((j+tmpuy)>=0)&((i+tmpux+1)>=0)&((i+tmpux+1)<ixsize)&((j+tmpuy)<iysize)) 
	            l4 = inimage [k+ixsize]; 

	         outimage [(j*ixsize)+i] = (unsigned char)(tx*ty*l1 + tx*ydelta*l4 + 
				ty*xdelta*l2 + xdelta*ydelta*l3);
	         image_xdeform [(j*ixsize)+i] = ux; 
	         image_ydeform [(j*ixsize)+i] = uy; 
                 /*for (k = 0; k < totpoints; k++){
                    if ((i == (int)pathx[step][k]) && (j == (int)pathy[step][k])){
    printf ("\t\t%3d  %3d %3d -> %3d %3d (%lf %lf) \n",
                                k, i, j, i+tmpux, j+tmpuy,pathx[step+1][k],pathy[step+1][k]);
                    }
                 }*/
	      }
	   }
           /* Transfer the outimage into inimage */
           for(j=0;j<iysize;j++){
              for (i=0;i<ixsize;i++){
                 inimage[j*ixsize+i] = outimage[j*ixsize+i];
                 outimage[j*ixsize+i] = 0;
	      }
	   }
#ifdef HAVE_IRISGL
           DisplayDeformedImage (False, inimage);
#endif /* HAVE_IRISGL */
	   {
	   char 	fname [256];

	   sprintf (fname, "image.fluid.%d", step);	
	   WriteImage (fname, inimage, ixsize, iysize);
	   }
	}
	}
}
