/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <cmath>
#include <iostream>

#include "BrainModelVolumeNearToPlane.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "VectorFile.h"
#include "VolumeFile.h"
#include "vtkMath.h"

static float Coeff[BrainModelVolumeNearToPlane::NALPHA][3][3];
static float Fnormal[BrainModelVolumeNearToPlane::NALPHA][3];
static float phis[6] = {
                           0.0,
                           0.0,
                           72.0,
                           144.0,
                           216.0,
                           288.0
                       };
static float thetas[6] = {
                              0.0,
                              63.59,	
                              63.59,	
                              63.59,	
                              63.59,	
                              63.59,	
                         };


/**
 * Constructor.
 */
BrainModelVolumeNearToPlane::BrainModelVolumeNearToPlane(
                                 BrainSet* bs,
                                 VectorFile* vecFileIn,
                                 const float sigmaNIn, 
                                 const float sigmaWIn, 
                                 const float offsetIn, 
                                 const bool  downflagIn, 
                                 const int gradsignIn,
                                 const bool maskingFlagIn, 
                                 VolumeFile* maskVolumeIn,
                                 VolumeFile* outputVolumeIn)
   : BrainModelAlgorithm(bs)
{
      vecFile = vecFileIn;
      sigmaN = sigmaNIn;
      sigmaW = sigmaWIn; 
      offset = offsetIn; 
      downflag = downflagIn;
      gradsign = gradsignIn;
      maskingFlag = maskingFlagIn; 
      maskVolume = maskVolumeIn;
      outputVolume = outputVolumeIn;
}

/**
 * Destructor.
 */
BrainModelVolumeNearToPlane::~BrainModelVolumeNearToPlane()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeNearToPlane::execute() throw (BrainModelAlgorithmException)
{
   const int cnt = maskVolume->getNumberOfNonZeroVoxels();
   if (DebugControl::getDebugOn()) {
	   std::cout << "\t\tNewNear2Planes " << maskingFlag << std::endl;
	   std::cout << "\t\t\t" << cnt << " voxels turned on in mask" << std::endl;
	   std::cout << "SIGMA: narrow " << sigmaN << ", wide " << sigmaW << std::endl;
   }

	//%printf ("%d %d %d; %f %f %f; %d %d\n", ncol, nrow, nslices, 
	//%	sigmaN, sigmaW, offset, downflag, gradsign);
		
   vecFile->multiplyXYZByMagnitude();

   const int numVoxels = outputVolume->getTotalNumberOfVoxels();
   outputVolume->setAllVoxels(0.0);
   float* Pmag = outputVolume->getVoxelData();

	float	filter1[NALPHA][FILTSIZE][FILTSIZE][FILTSIZE];
	float	filter2[NALPHA][FILTSIZE][FILTSIZE][FILTSIZE];
	generateEllipsoidFilter(sigmaW, sigmaW, sigmaN, offset, filter1); 
	generateEllipsoidFilter(sigmaW, sigmaW, sigmaN, -offset, filter2); 

	int startidx = 0;  //ComputeIndex (0, 0, 0, ncol, nrow);
	float* PMagInit = &Pmag[startidx];
	float* OrientVector = new float[numVoxels];
   float* OrientVectorForDeleting = OrientVector;
	float* OrientInit = &OrientVector[startidx];
   float* maskVoxels = maskVolume->getVoxelData();
	float* MaskInit = &maskVoxels[startidx];
	float* dotproduct = new float[numVoxels];
   float* dotproductForDeleting = dotproduct;
	float* dotinit = &dotproduct [startidx];
   float* vectorX = vecFile->getWithFlatIndexValueX(0);
   float* vectorY = vecFile->getWithFlatIndexValueY(0);
   float* vectorZ = vecFile->getWithFlatIndexValueZ(0);
   float* vectorInit[3];
	vectorInit[0] = &vectorX[startidx];
	vectorInit[1] = &vectorY[startidx];
	vectorInit[2] = &vectorZ[startidx];

   int ncol, nrow, nslices;
   outputVolume->getDimensions(ncol, nrow, nslices);

	for (int alpha = 0; alpha < NALPHA; alpha++){
	   float* Pmag = PMagInit; 
	   OrientVector = OrientInit; 
	   maskVoxels = MaskInit; 
	   dotproduct = dotinit;
	   vectorX = vectorInit [0];
	   vectorY = vectorInit [1];
	   vectorZ = vectorInit [2];
	   computeDotProduct(alpha, vectorX, vectorY, vectorZ, dotproduct); 

	   for (int k = 0; k < nslices; k++){
         if (DebugControl::getDebugOn()) {
	         if ((k % 10) == 0) {
	            printf ("\tALPHA %d; CONVOLUTION: Slice %d..%d\n", alpha, k, nslices);
            }
         }
	      for (int j = 0; j < nrow; j++) {
	         for (int i = 0; i < ncol; i++) {
               if (((maskingFlag) && (*maskVoxels != 0)) || (maskingFlag == false)) {
                  float nearAlphaPlus = 0.0;
                  float nearAlphaMinus = 0.0;
                  if (downflag == 0){
                     if (abs(gradsign) == 1) {
                        nearAlphaPlus = MathUtilities::limitToPositive(
                            newVectorConvolve(i, j, k,
                                              filter1[alpha], gradsign, 
                                              dotproduct, 0)); 
                        nearAlphaMinus = MathUtilities::limitToPositive(
                           newVectorConvolve(i, j, k,
                                             filter2[alpha], -gradsign, 
                                             dotproduct, 0));
                     }
                     else { 
                         nearAlphaPlus = newVectorConvolve(i, j, k,
                                                           filter1[alpha], 1, 
                                                           dotproduct, 1); 
                         nearAlphaMinus = newVectorConvolve(i, j, k,
                                                            filter2 [alpha], 1, 
                                                            dotproduct, 1); 
                     }
                  }
                  else if (downflag == 1) {
                     if (abs (gradsign) == 1) {
                        nearAlphaPlus = MathUtilities::limitToPositive(
                                       downVectorConvolve(alpha, 
                                          i, j, k, filter1[alpha], gradsign, 
                                          vectorInit, 0)); 
                        nearAlphaMinus = MathUtilities::limitToPositive(
                                       downVectorConvolve (alpha, 
                                          i, j, k, filter2[alpha], -gradsign, 
                                          vectorInit, 0)); 
                     }
                     else {
                         nearAlphaPlus = downVectorConvolve(alpha, 
                                             i, j, k, filter1[alpha], 1,
                                             vectorInit, 1); 
                         nearAlphaMinus = downVectorConvolve(alpha, 
                                             i, j, k, filter2[alpha], 1,
                                             vectorInit, 1); 
                     }
                  }
                  *OrientVector = std::sqrt(nearAlphaPlus*nearAlphaMinus); 
                  *Pmag += *OrientVector; 
               }
               maskVoxels++;
               OrientVector++;
               Pmag++;
            }
	      }
	   }
/*
	   char nfile [256];
	   sprintf (nfile, "%s.raw.float.c%d", directory, alpha);
	   printf ("Writing volume for alpha %d to %s\n", alpha, nfile); 
	   OrientVector = OrientInit; 
	   WriteFloatVolume (OrientVector, nfile, ncol, nrow, nslices); 
*/
	}
	Pmag = PMagInit; 
	delete[] OrientVectorForDeleting; //OrientVector;
	delete[] dotproductForDeleting;   //dotproduct;
}

void
BrainModelVolumeNearToPlane::generateCoefficientMatrix (const float sigmax, 
                                                        const float sigmay, 
                                                        const float sigmaz)
{
	for (int k = 0; k < NALPHA; k++) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				Coeff[k][i][j] = 0.0;
         }
      }
   }
	for (int k = 0; k < NALPHA; k++){
		Coeff[k][0][0] = 1.0/(sigmax*sigmax);
		Coeff[k][1][1] = 1.0/(sigmay*sigmay);
		Coeff[k][2][2] = 1.0/(sigmaz*sigmaz);
	}
	for (int i = 0; i < NALPHA; i++){
   	float	Ctheta[NALPHA][3][3];
		rotateTheta(Coeff[i], i, Ctheta[i]);
		rotatePhi(Ctheta[i], i, Coeff[i]);
      if (DebugControl::getDebugOn()) {
         std::cout << "Coefficients of Matrix: theta " << thetas[i]
                   << ", phi " << phis[i] << std::endl;
         for (int j = 0; j < 3; j++) {
            std::cout << "\t" << Coeff[i][j][0] << " " << Coeff[i][j][1]
                      << " " << Coeff[i][j][2] << std::endl;
         }
      }
	}
}

/**
 *
 */
void	
BrainModelVolumeNearToPlane::generateEllipsoidFilter(const float sigmax, 
                                        const float sigmay, 
                                        const float sigmaz,
		                                  const float delta, 
                                        float filter[NALPHA][FILTSIZE][FILTSIZE][FILTSIZE])
{
   const float DEG2RAD = MathUtilities::degreesToRadians();
	for (int i = 0; i < NALPHA; i++){
		Fnormal[i][0] = std::sin(DEG2RAD*thetas[i]) * std::cos(DEG2RAD * phis[i]);
		Fnormal[i][1] = std::sin(DEG2RAD*thetas[i]) * std::sin(DEG2RAD * phis[i]);
		Fnormal[i][2] = std::cos(DEG2RAD*thetas[i]);
      if (DebugControl::getDebugOn()) {
         std::cout << "Normal for " << i
                   << ": " << Fnormal[i][0]
                   << " " << Fnormal[i][1]
                   << " " << Fnormal[i][2]
                   << std::endl;
      }
	}	

	generateCoefficientMatrix(sigmax, sigmay, sigmaz);
	for (int alpha = 0; alpha < NALPHA; alpha++) {
      if (DebugControl::getDebugOn()) {
         std::cout << "Generate filter for nalpha " << alpha
                   << " (" << thetas[alpha]
                   << " " << phis[alpha]
                   << ")" << std::endl;
      }

		for (int k = 0; k < FILTSIZE; k++) {
			for (int i = 0; i < FILTSIZE; i++) {
				for (int j = 0; j < FILTSIZE; j++) {
               float R[3], Rout[3];
					R[0] = i - FILTSIZE/2 - (delta * Fnormal[alpha][0]);
					R[1] = j - FILTSIZE/2 - (delta * Fnormal[alpha][1]); 
					R[2] = k - FILTSIZE/2 - (delta * Fnormal[alpha][2]); 
					multMatrixRow(R, Coeff[alpha], Rout);
					const float answer = Rout[0] * R[0] + Rout[1] * R[1] + Rout[2] * R[2];
					filter[alpha][i][j][k] = exp(-answer); 
				}
			} 
		}
	}
}

void 
BrainModelVolumeNearToPlane::multMatrixRow(const float b[3], const float m[3][3], float out[3])
{
     for (int i = 0; i < 3; i++) {
        out [i] = 0.0;
     }
     for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
           out [i] += b [j] * m [j][i];
        }
     }
}

/**
 *
 */
void 
BrainModelVolumeNearToPlane::rotatePhi(const float C[DIM][DIM], const int alpha, 
                                       float Cout[DIM][DIM])
{
   const float DEG2RAD = MathUtilities::degreesToRadians();

	float	t1[DIM][DIM], t2[DIM][DIM], t3[DIM][DIM];
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < DIM; j++){
			t1[i][j] = 0.0;
			t2[i][j] = 0.0;
		}
	}
	t1[0][0] = std::cos(DEG2RAD * phis[alpha]);
	t1[0][1] = -std::sin(DEG2RAD * phis[alpha]);
	t1[1][0] = std::sin(DEG2RAD * phis[alpha]);
	t1[1][1] = std::cos(DEG2RAD * phis[alpha]);
	t1[2][2] = 1.0;

	t2[0][0] = std::cos(DEG2RAD * phis[alpha]);
	t2[0][1] = std::sin(DEG2RAD * phis[alpha]);
	t2[1][0] = -std::sin(DEG2RAD * phis[alpha]);
	t2[1][1] = std::cos(DEG2RAD * phis[alpha]);
	t2[2][2] = 1.0;

	multMatrixMatrix(t1, C, t3);
	multMatrixMatrix(t3, t2, Cout);
}
 
/**
 *
 */
void 
BrainModelVolumeNearToPlane::rotateTheta(const float C[DIM][DIM], const int alpha,
                                         float Cout[DIM][DIM])
{
   const float DEG2RAD = MathUtilities::degreesToRadians();

	float	t1[DIM][DIM], t2[DIM][DIM], t3[DIM][DIM];
	for (int i = 0; i < DIM; i++){
		for (int j = 0; j < DIM; j++){
			t1[i][j] = 0.0;
			t2[i][j] = 0.0;
		}
	}
	t1[0][0] = std::cos(DEG2RAD * thetas[alpha]);
	t1[0][2] = -std::sin(DEG2RAD * thetas[alpha]);
	t1[1][1] = 1.0;
	t1[2][0] = std::sin(DEG2RAD * thetas[alpha]);
	t1[2][2] = std::cos(DEG2RAD * thetas[alpha]);

	t2[0][0] = std::cos(DEG2RAD * thetas[alpha]);
	t2[0][2] = std::sin(DEG2RAD * thetas[alpha]);
	t2[1][1] = 1.0;
	t2[2][0] = -std::sin(DEG2RAD * thetas[alpha]);
	t2[2][2] = std::cos(DEG2RAD * thetas[alpha]);

	multMatrixMatrix(t1, C, t3);
	multMatrixMatrix(t3, t2, Cout);
}

/**
 *
 */
void 
BrainModelVolumeNearToPlane::multMatrixMatrix(const float A[DIM][DIM], 
                                              const float B[DIM][DIM],
                                              float out[DIM][DIM])
{
   for (int i = 0; i < DIM; i++) {
      for (int j = 0; j < DIM; j++) {
         out[i][j] = 0.0;
      }
   }
   for (int i = 0; i < DIM; i++) {
      for (int j = 0; j < DIM; j++) {
         for (int k = 0; k < DIM; k++) {
            out[i][j] += A[i][k] * B[k][j];
         }
      }
   }
}

/**
 *
 */
float	
BrainModelVolumeNearToPlane::newVectorConvolve(const int x, 
                                               const int y, 
                                               const int z, 
                                               const float filter[FILTSIZE][FILTSIZE][FILTSIZE], 
                                               const int signflag, 
                                               float *dotproduct, 
                                               const int absflag)
{
	const int offset = FILTSIZE/2;
	float answer = 0.0;

   int ncol, nrow, nslices;
   outputVolume->getDimensions(ncol, nrow, nslices);
   
	int xx = x-offset;
   int starti = 0;
	if (xx < 0)
	   starti = -xx;
	else
	   starti = 0;
	xx = x+offset-ncol;
   int stopi = 0;
	if (xx >= 0) 
	   stopi = (FILTSIZE-1)-xx;
	else
	   stopi = FILTSIZE;

	int yy = y-offset;
   int startj = 0;
	if (yy < 0)
	   startj = -yy;
	else
	   startj = 0;
	yy = y+offset-nrow;
   int stopj = 0;
	if (yy >= 0) 
	   stopj = (FILTSIZE-1)-yy;
	else
	   stopj = FILTSIZE;

	int zz = z-offset;
   int startk = 0;
	if (zz < 0)
	   startk = -zz;
	else
	   startk = 0;
	zz = z+offset-nslices;
   int stopk = 0;
	if (zz >= 0)
	   stopk = (FILTSIZE-1)-zz;
	else
	   stopk = FILTSIZE;

	xx = x+starti-offset;
	yy = y+startj-offset;
	zz = z+startk-offset;
	const int idx = outputVolume->getVoxelDataIndex(xx, yy, zz);

   const int isize = ncol * nrow;
	float* startdot = &dotproduct[idx];
   float* curdot = &dotproduct[idx]; 
	float val = 0.0;
	for (int k = startk; k < stopk; k++) {
	   for (int j = startj; j < stopj; j++) {
	      for (int i = starti; i < stopi; i++) {
            if (absflag == 0) {
               val = *curdot*signflag;
            }
            else {
               val = fabs(*curdot);
            }
            const float filter_val = filter[i][j][k];
            answer += val * filter_val;
            curdot++;
	      }
	      int valinc = (j+1)*ncol;
	      curdot = startdot + valinc; 
	   }
	   startdot = startdot + isize;
	   curdot = startdot;
	}
	return (answer);
}

/**
 *
 */
void 
BrainModelVolumeNearToPlane::computeDotProduct(const int alpha, 
                                               float *VectorVolumeX, 
                                               float *VectorVolumeY,
                                               float *VectorVolumeZ, 
                                               float *dotproduct)
{
   int ncol, nrow, nslices;
   outputVolume->getDimensions(ncol, nrow, nslices);
   
	for (int k = 0; k < nslices; k++) {
	   for (int j = 0; j < nrow; j++) {
	      for (int i = 0; i < ncol; i++) {
            float vector[3];
            vector[0] = *VectorVolumeX++; 
            vector[1] = *VectorVolumeY++; 
            vector[2] = *VectorVolumeZ++; 
            *dotproduct++ = MathUtilities::dotProduct(Fnormal[alpha], vector);
	      }
	   }
	}
}

float 
BrainModelVolumeNearToPlane::downVectorConvolve(const int alpha, 
                                     const int x, 
                                     const int y, 
                                     const int z, 
                                     const float filter[FILTSIZE][FILTSIZE][FILTSIZE], 
                                     const int signflag, 
                                     float *VectorVolume[3],
                                     const int absFlag)
{
   int ncol, nrow, nslices;
   outputVolume->getDimensions(ncol, nrow, nslices);

	float answer = 0.0;
	for (int k = 0; k < FILTSIZE; k++) {
		for (int j = 0; j < FILTSIZE; j++) {
			for (int i = 0; i < FILTSIZE; i++) {
				const float xx = x+(2*i)-(2*FILTSIZE/2);
				const float yy = y+(2*j)-(2*FILTSIZE/2);
				const float zz = z+(2*k)-(2*FILTSIZE/2);
				if ((xx >= 0) && (xx < ncol) && 
					 (yy >= 0) && (yy < nrow) && 
					 (zz >= 0) && (zz < nslices)) {
               const int idx = outputVolume->getVoxelDataIndex(static_cast<int>(xx), 
                                                               static_cast<int>(yy), 
                                                               static_cast<int>(zz));
               float vector[3];
               vector[0] = VectorVolume[0][idx];
               vector[1] = VectorVolume[1][idx];
               vector[2] = VectorVolume[2][idx];
               float val;
               if (absFlag == 0) {
                  val = signflag*MathUtilities::dotProduct(Fnormal[alpha], vector); 
               }
               else {
                  val = fabs(MathUtilities::dotProduct(Fnormal[alpha], vector));
               }
               const float filter_val = filter[i][j][k];
               answer += val * filter_val; 
				}
			}
		}
	}
	return (answer);
}


