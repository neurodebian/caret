/*****************************************************************************
   Major portions of this software are copyrighted by the Medical College
   of Wisconsin, 1994-2001, and are released under the Gnu General Public
   License, Version 2.  See the file README.Copyright for details.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*
  Program to correct for image intensity non-uniformity.

  File:    3dUniformize.c
  Author:  B. Douglas Ward
  Date:    28 January 2000

  Mod:     Added call to AFNI_logger.
  Date:    15 August 2001

*/

/*---------------------------------------------------------------------------*/

#define PROGRAM_NAME "3dUniformize"                  /* name of this program */
#define PROGRAM_AUTHOR "B. D. Ward"                        /* program author */
#define PROGRAM_INITIAL "28 January 2000" /* date of initial program release */
#define PROGRAM_LATEST  "26 September 2005 [rickr, zss]"   /* date of latest program revision */

/*---------------------------------------------------------------------------*/
/*
  Include header files.
*/
#include <cmath>   // CARET
#include <cstdio>  // CARET
#include <cstdlib> // CARET
#include <iostream> // CARET

#include <QApplication>

#include "caret_uniformize.h"
#include "DebugControl.h"
#include "VolumeFile.h"  // CARET
//CARET #include "mrilib.h"
#include "matrix.h"

#define IJK_TO_THREE(ijk,i,j,k,nx,nxy) \
  ( (k) = (ijk)/(nxy) , (j)=((ijk)%(nxy))/(nx) , (i)=(ijk)%(nx) )

/*---------------------------------------------------------------------------*/
/*
  Global variables, constants, and data structures.
*/

#define MAX_STRING_LENGTH 80

static short* inputData = NULL;  // CARET

static VolumeFile* inputVolume;

// CARET static THD_3dim_dataset * anat_dset = NULL;     /* input anatomical dataset  */
//CARET char * commandline = NULL ;                /* command line for history notes */

// CARET int input_datum = MRI_short ;              /* 16 Apr 2003 - RWCox */
static int quiet       = 0 ;                      /* ditto */
#define USE_QUIET
 
typedef bool Boolean;  // CARET

typedef struct UN_options
{ 
  char * anat_filename;       /* file name for input anat dataset */
  char * prefix_filename;     /* prefix name for output dataset */
  Boolean quiet;              /* flag for suppress screen output */
  int lower_limit;    /* lower limit for voxel intensity */
  int upper_limit;    /* upper limit for voxel intensity 0 for ignoring this parameter*/
  int rpts;           /* #voxels in sub-sampled image (for pdf) */
  int spts;           /* #voxels in subsub-sampled image (for field poly.) */
  int nbin;           /* #bins for pdf estimation */
  int npar;           /* #parameters for field polynomial */
  int niter;          /* #number of iterations */
// CARET  THD_3dim_dataset * new_dset;   /* output afni data set pointer */
} UN_options;


/*---------------------------------------------------------------------------*/
/*
  Include source code files.
*/

//CARET #include "matrix.c"
#include "estpdf3.c"


/*---------------------------------------------------------------------------*/
/*
   Print error message and stop.
*/

static void UN_error (char * message)
{
  fprintf (stderr, "%s Error: %s \n", PROGRAM_NAME, message);
  //CARET exit(1);
}


/*---------------------------------------------------------------------------*/

/** macro to test a malloc-ed pointer for validity **/

#define MTEST(ptr) \
if((ptr)==NULL) \
( UN_error ("Cannot allocate memory") )
     

/*---------------------------------------------------------------------------*/
/*
  Routine to initialize the input options.
*/
 
static void initialize_options 
(
  UN_options * option_data    /* uniformization program options */
)
 
{

  /*----- initialize default values -----*/
  option_data->anat_filename = NULL;    /* file name for input anat dataset */
  option_data->prefix_filename = NULL;  /* prefix name for output dataset */
  option_data->quiet = FALSE;           /* flag for suppress screen output */
  option_data->lower_limit = 0;        /* voxel intensity lower limit, used to be 25 always ZSS Sept. 36 05 */
  option_data->upper_limit = 0;
  option_data->rpts = 200000;   /* #voxels in sub-sampled image (for pdf) */
  option_data->spts = 10000;    /* #voxels in subsub-sampled image 
				   (for field polynomial estimation) */
  option_data->nbin = 250;      /* #bins for pdf estimation */
  option_data->npar = 35;       /* #parameters for field polynomial */
  option_data->niter = 5;       /* #number of iterations  */
//CARET  option_data->new_dset = NULL;
}

/*---------------------------------------------------------------------------*/
/*
  Program initialization.
*/

static void initialize_program 
(
  UN_options ** option_data,       /* uniformization program options */
  short ** sfim                    /* output image volume */
)

{
  int nxyz;                        /* #voxels in input dataset */


  /*----- Save command line for history notes -----*/
  //CARET commandline = tross_commandline( PROGRAM_NAME , argc,argv ) ;


  /*----- Allocate memory for input options -----*/
  *option_data = (UN_options *) malloc (sizeof(UN_options));
  MTEST (*option_data);

  
  /*----- Initialize the input options -----*/
  initialize_options (*option_data); 


  /*----- Get operator inputs -----*/
  //CARET get_options (argc, argv, *option_data);


  /*----- Verify that inputs are acceptable -----*/
  //CARET verify_inputs (*option_data);


  /*----- Initialize random number generator -----*/
  rand_initialize (1234567);


  /*----- Allocate memory for output volume -----*/
  //CARET nxyz = DSET_NX(anat_dset) * DSET_NY(anat_dset) * DSET_NZ(anat_dset);
  int dim[3]; //CARET 
  inputVolume->getDimensions(dim); //CARET 
  nxyz = dim[0] * dim[1] * dim[2]; //CARET 
  *sfim = (short *) malloc (sizeof(short) * nxyz);
  MTEST (*sfim);
}


/*---------------------------------------------------------------------------*/
/*
  Write time series data to specified file.
*/

static void ts_write (char * filename, int ts_length, float * data)
{
  int it;
  FILE * outfile = NULL;

  outfile = fopen (filename, "w");


  for (it = 0;  it < ts_length;  it++)
    {
      fprintf (outfile, "%f ", data[it]);
      fprintf (outfile, " \n");
    }

  fclose (outfile);
}


/*---------------------------------------------------------------------------*/
/*
  Resample the original image at randomly selected voxels (whose intensity
  value is greater than the specified lower limit, to exclude voxels outside
  the brain).  Take the logarithm of the intensity values for the selected 
  voxels.
*/

static void resample 
(
  UN_options * option_data,
  int * ir,                         /* voxel indices for resampled image */
  float * vr                        /* resampled image data (logarithms) */
)

{
  short * anat_data = NULL;
  int nxyz;
  int rpts;
  int lower_limit;
  int it, k;


  /*----- Initialize local variables -----*/
  //CARET nxyz = DSET_NX(anat_dset) * DSET_NY(anat_dset) * DSET_NZ(anat_dset);
  int dim[3]; //CARET 
  inputVolume->getDimensions(dim); //CARET 
  nxyz = dim[0] * dim[1] * dim[2]; //CARET 
  
  //CARET anat_data = (short *) DSET_BRICK_ARRAY(anat_dset,0);
  anat_data = inputData; //CARET 
  lower_limit = option_data->lower_limit;
  rpts = option_data->rpts;

  it = 0;
  while (it < rpts)
    {
      k = (int)rand_uniform (0, nxyz);
      /* okay if no upper_limit, or data < upper_limit   16 Dec 2005 [rickr] */
      if ( (k >= 0) && (k < nxyz) && (anat_data[k] > lower_limit) &&
           ( ! option_data->upper_limit ||
               (anat_data[k] < option_data->upper_limit) ) )
	{
	  ir[it] = k;
	  vr[it] = log (anat_data[k] + rand_uniform (0.0,1.0));
	  it++;
	}
    }

  return;
}


/*---------------------------------------------------------------------------*/
/*
  Create intensity map that will tend to concentrate values around the
  means of the gray and white matter distributions.
*/

static void create_map (pdf vpdf, float * pars, float * vtou)

{
  int ibin;
  float v;

  for (ibin = 0;  ibin < vpdf.nbin;  ibin++)
    {
      v = PDF_ibin_to_xvalue (vpdf, ibin);
          
      if ((v > pars[4]-2.0*pars[5]) && (v < 0.5*(pars[4]+pars[7])))
	vtou[ibin] = pars[4];
      else if ((v > 0.5*(pars[4]+pars[7])) && (v < pars[7]+2.0*pars[8]))
	vtou[ibin] = pars[7];
      else
	vtou[ibin] = v;

    }
  
}


/*---------------------------------------------------------------------------*/
/*
  Use the intensity map to transform values of voxel intensities.
*/

static void map_vtou (pdf vpdf, int rpts, float * vr, float * vtou, float * ur)

{
  int i, ibin;
  float v;


  for (i = 0;  i < rpts;  i++)
    {
      v = vr[i];
      ibin = PDF_xvalue_to_ibin (vpdf, v);
      
      if ((ibin >= 0) && (ibin < vpdf.nbin))
	ur[i] = vtou[ibin];
      else
	ur[i] = v;
    }

}


/*---------------------------------------------------------------------------*/

static void subtract (int rpts, float * a, float * b, float * c)

{
  int i;


  for (i = 0;  i < rpts;  i++)
    {
      c[i] = a[i] - b[i];
    }

}


/*---------------------------------------------------------------------------*/
/*
  Create one row of the X matrix.
*/

static void create_row (int ixyz, int nx, int ny, int nz, float * xrow)

{
  int ix, jy, kz;
  float x, y, z, x2, y2, z2, x3, y3, z3, x4, y4, z4;


  IJK_TO_THREE (ixyz, ix, jy, kz, nx, nx*ny); 


  x = (float) ix / (float) nx - 0.5;
  y = (float) jy / (float) ny - 0.5;
  z = (float) kz / (float) nz - 0.5;

  x2 = x*x;   x3 = x*x2;   x4 = x2*x2;
  y2 = y*y;   y3 = y*y2;   y4 = y2*y2;
  z2 = z*z;   z3 = z*z2;   z4 = z2*z2;


  xrow[0] = 1.0;
  xrow[1] = x;
  xrow[2] = y;
  xrow[3] = z;
  xrow[4] = x*y;
  xrow[5] = x*z;
  xrow[6] = y*z;
  xrow[7] = x2;
  xrow[8] = y2;
  xrow[9] = z2;
  xrow[10] = x*y*z;
  xrow[11] = x2*y;
  xrow[12] = x2*z;
  xrow[13] = y2*x;
  xrow[14] = y2*z;
  xrow[15] = z2*x;
  xrow[16] = z2*y;
  xrow[17] = x3;
  xrow[18] = y3;
  xrow[19] = z3;
  xrow[20] = x2*y*z;
  xrow[21] = x*y2*z;
  xrow[22] = x*y*z2;
  xrow[23] = x2*y2;
  xrow[24] = x2*z2;
  xrow[25] = y2*z2;
  xrow[26] = x3*y;
  xrow[27] = x3*z;
  xrow[28] = x*y3;
  xrow[29] = y3*z;
  xrow[30] = x*z3;
  xrow[31] = y*z3;
  xrow[32] = x4;
  xrow[33] = y4;
  xrow[34] = z4;


  return;
}


/*---------------------------------------------------------------------------*/
/*
  Approximate the distortion field with a polynomial function in 3 dimensions.
*/

static void poly_field (int nx, int ny, int nz, int rpts, int * ir, float * fr, 
		 int spts, int npar, float * fpar)

{
  int p;                   /* number of parameters in the full model */ 
  int i, j, k;
  matrix x;                      /* independent variable matrix */
  matrix xtxinv;                 /* matrix:  1/(X'X)       */
  matrix xtxinvxt;               /* matrix:  (1/(X'X))X'   */
  vector y;
  vector coef;
  float * xrow = NULL;
  int ip;
  //int iter;
  //float f;


  p = npar;


  /*----- Initialize matrices and vectors -----*/
  matrix_initialize (&x);
  matrix_initialize (&xtxinv);
  matrix_initialize (&xtxinvxt);
  vector_initialize (&y);
  vector_initialize (&coef);


  /*----- Allocate memory -----*/
  matrix_create (spts, p, &x);
  vector_create (spts, &y);
  xrow = (float *) malloc (sizeof(float) * p); 
 

  /*----- Set up the X matrix and Y vector -----*/
  for (i = 0;  i < spts;  i++)
    {
      k = (int)rand_uniform (0, rpts);
      create_row (ir[k], nx, ny, nz, xrow);

      for (j = 0;  j < p;  j++)
	x.elts[i][j] = xrow[j];
      y.elts[i] = fr[k];
    }


  /*  
      matrix_sprint ("X matrix = ", x);
      vector_sprint ("Y vector = ", y);
  */


  {
    /*----- calculate various matrices which will be needed later -----*/
    matrix xt, xtx;            /* temporary matrix calculation results */
    int ok;                    /* flag for successful matrix inversion */
    
    
    /*----- initialize matrices -----*/
    matrix_initialize (&xt);
    matrix_initialize (&xtx);
    
	
    matrix_transpose (x, &xt);
    matrix_multiply (xt, x, &xtx);
    ok = matrix_inverse (xtx, &xtxinv);
    
    if (ok)
      matrix_multiply (xtxinv, xt, &xtxinvxt);
    else
      {
	matrix_sprint ("X matrix = ", x);
	matrix_sprint ("X'X matrix = ", xtx);
	UN_error ("Improper X matrix  (cannot invert X'X) ");
      }
    
    /*----- dispose of matrices -----*/
    matrix_destroy (&xtx);
    matrix_destroy (&xt);
    
  }


  /*
    matrix_sprint ("1/(X'X)     = ", xtxinv);
    matrix_sprint ("(1/(X'X))X' = ", xtxinvxt);
    vector_sprint ("Y data  = ", y);
  */
  
  vector_multiply (xtxinvxt, y, &coef);
  /*
    vector_sprint ("Coef    = ", coef);
  */
  

  for (ip = 0;  ip < p;  ip++)
    {
     fpar[ip] = coef.elts[ip];
    }
  

  /*----- Dispose of matrices and vectors -----*/
  matrix_destroy (&x);
  matrix_destroy (&xtxinv);
  matrix_destroy (&xtxinvxt);
  vector_destroy (&y);
  vector_destroy (&coef);

  free(xrow);
}


/*---------------------------------------------------------------------------*/
/*
  Use the 3-dimensional polynomial function to estimate the distortion field
  at each point.
*/

static float warp_image (int npar, float * fpar, int nx, int ny, int nz,
		  int rpts, int * ir, float * fs)
{
  int i, j;
  //float x;
  float * xrow;
  float max_warp;


  xrow = (float *) malloc (sizeof(float) * npar); 


  max_warp = 0.0;

  for (i = 0;  i < rpts;  i++)
    {
      create_row (ir[i], nx, ny, nz, xrow);

      fs[i] = 0.0;
            
      for (j = 1;  j < npar;  j++)
	fs[i] += fpar[j] * xrow[j];

      if (fabs(fs[i]) > max_warp)
	max_warp = fabs(fs[i]);
    }


  free (xrow);   xrow = NULL;


  return (max_warp);
}


/*---------------------------------------------------------------------------*/
/*
  Find polynomial approximation to the distortion field.
*/

static void estimate_field (UN_options * option_data, 
		     int * ir, float * vr, float * fpar)
{
  float * ur = NULL, * us = NULL, * fr = NULL, * fs = NULL, * wr = NULL;
  float * vtou = NULL;
  float * gpar;
  int iter = 0, itermax=5;
  int ip;
  //int it;
  int nx, ny, nz, nxy, nxyz;
  int rpts, spts, nbin, npar;
  float parameters [DIMENSION];    /* parameters for PDF estimation */
  //Boolean ok = TRUE;               /* flag for successful PDF estimation */
  char filename[MAX_STRING_LENGTH];


  /*----- Initialize local variables -----*/
  //CARET nx = DSET_NX(anat_dset);  ny = DSET_NY(anat_dset);  nz = DSET_NZ(anat_dset);
  int dim[3]; //CARET 
  inputVolume->getDimensions(dim); //CARET 
  nx = dim[0]; ny = dim[1]; nz = dim[2]; //CARET 
  nxy = nx*ny;   nxyz = nxy*nz;
  rpts = option_data->rpts;
  spts = option_data->spts;
  nbin = option_data->nbin;
  npar = option_data->npar;
  itermax = option_data->niter;


  /*----- Allocate memory -----*/
  ur   = (float *) malloc (sizeof(float) * rpts);   MTEST (ur);
  us   = (float *) malloc (sizeof(float) * rpts);   MTEST (us);
  fr   = (float *) malloc (sizeof(float) * rpts);   MTEST (fr);
  fs   = (float *) malloc (sizeof(float) * rpts);   MTEST (fs);
  wr   = (float *) malloc (sizeof(float) * rpts);   MTEST (wr);
  gpar = (float *) malloc (sizeof(float) * npar);   MTEST (gpar);
  vtou = (float *) malloc (sizeof(float) * nbin);   MTEST (vtou);


  /*----- Initialize polynomial coefficients -----*/
  for (ip = 0;  ip < npar;  ip++)
    {
      fpar[ip] = 0.0;
      gpar[ip] = 0.0;
    }


  /*----- Estimate pdf for resampled data -----*/
  if( 0 && !quiet ){
   fprintf (stderr,"       PDF_Initializing... \n");
  }
  PDF_initialize (&p);
  if( 0 && !quiet ){
   fprintf (stderr,"       float to pdf... \n");
  }
  PDF_float_to_pdf (rpts, vr, nbin, &p);

  if( !quiet ){
   sprintf (filename, "p%d.1D", iter);
   fprintf (stderr,"       Writing pdf output to %s... \n", filename);
   PDF_write_file (filename, p);
  }


  /*----- Estimate gross field distortion -----*/
  if( 0 && !quiet ){
   fprintf (stderr,"       Estimating gross distortions... \n");
  }
  poly_field (nx, ny, nz, rpts, ir, vr, spts, npar, fpar);
  warp_image (npar, fpar, nx, ny, nz, rpts, ir, fs);
  subtract (rpts, vr, fs, ur);
 
  
  for (ip = 0;  ip < rpts;  ip++)
    vr[ip] = ur[ip];


  /*----- Iterate over field distortion for concentrating the PDF -----*/
  for (iter = 1;  iter <= itermax;  iter++)
    {
      /*----- Estimate pdf for perturbed image ur -----*/
      estpdf_float (rpts, ur, nbin, parameters);
      PDF_sprint ("p", p);
      if( !quiet ){
       sprintf (filename, "p%d.1D", iter);
       PDF_write_file (filename, p);
      }

      /*----- Sharpen the pdf and produce modified image wr -----*/
      create_map (p, parameters, vtou);
      if( !quiet ){
       sprintf (filename, "vtou%d.1D", iter);
       ts_write (filename, p.nbin, vtou);
      }
      map_vtou (p, rpts, ur, vtou, wr);

      /*----- Estimate smooth distortion field fs -----*/
      subtract (rpts, vr, wr, fr);
      poly_field (nx, ny, nz, rpts, ir, fr, spts, npar, gpar);
      warp_image (npar, gpar, nx, ny, nz, rpts, ir, fs);

      /*----- Create perturbed image ur -----*/
      subtract (rpts, vr, fs, ur);
    }
  

  /*----- Accumulate distortion field polynomial coefficients -----*/
  for (ip = 0;  ip < npar;  ip++)
    fpar[ip] += gpar[ip];


  /*----- Deallocate memory -----*/
  free (ur);     ur = NULL;  
  free (us);     us = NULL;
  free (fr);     fr = NULL;
  free (fs);     fs = NULL;
  free (wr);     wr = NULL;
  free (gpar);   gpar = NULL;
  free (vtou);   vtou = NULL;


  return;
}


/*---------------------------------------------------------------------------*/
/*
  Remove the nonuniformity field.
*/

static void remove_field (UN_options * option_data, float * fpar, short * sfim)
{
  short * anat_data = NULL;
  int rpts;
  int npar;
  int lower_limit;
  int nx, ny, nz, nxyz;
  int ixyz, jpar;
  //float x;
  float * xrow;
  float f;

  double d, dmax = 0.0;
  int    tcount  = 0;


  /*----- Initialize local variables -----*/
  //CARET nx = DSET_NX(anat_dset);  ny = DSET_NY(anat_dset);  nz = DSET_NZ(anat_dset);
  int dim[3]; //CARET 
  inputVolume->getDimensions(dim); //CARET 
  nx = dim[0]; ny = dim[1]; nz = dim[2]; //CARET 
  nxyz = nx*ny*nz;
  //CARET anat_data = (short *) DSET_BRICK_ARRAY(anat_dset,0);
  anat_data = inputData;
  rpts = option_data->rpts;
  npar = option_data->npar;
  lower_limit = option_data->lower_limit;

  xrow = (float *) malloc (sizeof(float) * npar); 


  for (ixyz = 0;  ixyz < nxyz;  ixyz++)
    {
      if (anat_data[ixyz] > lower_limit) 
	   {
	     create_row (ixyz, nx, ny, nz, xrow);

	     f = 0.0;
	     for (jpar = 1;  jpar < npar;  jpar++)
	       f += fpar[jpar] * xrow[jpar];

          /* monitor the results for short range (rickr) */
          {
            d = exp( log(anat_data[ixyz]) - f);
            if ( d > 32767.0 )
            {
                if ( d > dmax ) dmax = d;
                sfim[ixyz] = 32767;
                tcount++;
            } else sfim[ixyz] = (short)d;
          }
	   }
      else
	   sfim[ixyz] = anat_data[ixyz];
    }

  if ( dmax > 32767.0 && !option_data->quiet )  /* then report an overflow */
      fprintf(stderr,
        "\n"
        "** warning: %d values exceeded the maximum dataset value of %d\n"
        "            (max overflow value of %.1f)\n"
        "** such values were set to the maximum %d\n"
        "** check your results!\n",
        tcount, 32767, dmax, 32767);
  
  free(xrow);
  
  return;
}


/*---------------------------------------------------------------------------*/
/*
  Correct for image intensity nonuniformity.
*/

static void uniformize (UN_options * option_data, short * sfim)

{
  int * ir = NULL;
  float * vr = NULL;
  float * fpar = NULL;
  int rpts, npar;


  /*----- Initialize local variables -----*/
  rpts = option_data->rpts;
  npar = option_data->npar;


  /*----- Allocate memory -----*/
  ir = (int *) malloc (sizeof(int) * rpts);         MTEST(ir);
  vr = (float *) malloc (sizeof(float) * rpts);     MTEST(vr);
  fpar = (float *) malloc (sizeof(float) * npar);   MTEST(fpar);


  /*----- Resample the data -----*/
  if( 0 && !quiet ){
   fprintf (stderr,"     resampling... \n");
  }
  resample (option_data, ir, vr);


  /*----- Estimate the nonuniformity field -----*/
  if( 0 && !quiet ){
   fprintf (stderr,"     estimating field... \n");
  }
  estimate_field (option_data, ir, vr, fpar);


  /*----- Remove the nonuniformity field -----*/
  if( 0 && !quiet ){
   fprintf (stderr,"     removing field... \n");
  }
  remove_field (option_data, fpar, sfim);

 
  /*----- Deallocate memory -----*/
  free (ir);     ir = NULL;
  free (vr);     vr = NULL;
  free (fpar);   fpar = NULL;

}


/*---------------------------------------------------------------------------*/
/*
  Do the bias correction
*/
void
biasCorrectVolume(VolumeFile* inputVolumeIn,
                  const int lowLimitIn,
                  const int highLimitIn,
                  const int numberOfIterations)
{

  UN_options * option_data = NULL;     /* uniformization program options */
  short * sfim = NULL;                 /* output uniformized image */

  inputVolume = inputVolumeIn;
  
  int dim[3];
  inputVolume->getDimensions(dim);
  const int numVoxels = dim[0] * dim[1] * dim[2];
  inputData = new short[numVoxels];
  for (int i = 0; i < numVoxels; i++) {
     inputData[i] = static_cast<short>(inputVolume->getVoxelWithFlatIndex(i));
  }
  
  quiet = (DebugControl::getDebugOn() == false);

  /*----- Program initialization -----*/
  if( !quiet ){
   fprintf (stderr,"  Initializing... \n");
  }
  initialize_program (&option_data, &sfim);


  option_data->lower_limit = lowLimitIn;
  option_data->upper_limit = highLimitIn;
  option_data->niter = numberOfIterations;
  
  /*----- Perform uniformization -----*/
  
  if( !quiet ){
   fprintf (stderr,"  Uniformizing... \n");
  }
  uniformize (option_data, sfim);


  /*----- Write out the results -----*/
  if( !quiet ){
   fprintf (stderr,"  Writing results... \n");
  }
  for (int i = 0; i < numVoxels; i++) {
     inputVolume->setVoxelWithFlatIndex(i, 0, sfim[i]);
  }
  
  //write_afni_data (option_data, sfim);
  
  delete[] inputData;
  if (sfim != NULL) free(sfim);
  if (option_data != NULL) free(option_data);
}

/*---------------------------------------------------------------------------*/
/*
  This is the main routine for program 3dUniformize.
*/
/*
int main
(
  int argc,                
  char ** argv            
)

{
  QApplication app(argc, argv, false);
  
  VolumeFile volume;
  try {
     volume.readFile("anat_needs_correction.nii");
     //volume.readFile("Human.colin.LR.TLRC-711-2B.111+orig.HEAD");
  }
  catch (FileException& e) {
     std::cout << "ERROR reading input volume: " << e.whatQString().toAscii().constData() << std::endl;
     std::exit(-1);
  }
  
  biasCorrectVolume(&volume,
                    14,
                    132,
                    5);
  try {
     inputVolume->writeFile("anat_corrected.nii");
     //inputVolume->writeFile("colin_corrected.nii");
  }
  catch (FileException& e) {
     std::cout << "ERROR writing output volume: " << e.whatQString().toAscii().constData() << std::endl;
     std::exit(-1);
  }
  
  return 0;
}
*/
/*---------------------------------------------------------------------------*/






