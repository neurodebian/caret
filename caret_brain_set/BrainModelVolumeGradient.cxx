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

#include <QGlobalStatic>  // needed for Q_OS_WIN32
#ifdef Q_OS_WIN32     // required for M_PI in <cmath>
#define _USE_MATH_DEFINES
#define NOMINMAX
#endif

#include <cmath>
#include <iostream>
#include <sstream>

#include "BrainModelVolumeGradient.h"
#include "DebugControl.h"
#include "VectorFile.h"
#include "VolumeFile.h"


//
// Stuff for gradient operations
//
#define FIVE_TAP
#ifdef FIVE_TAP
  /* 5 tap kmag= 0.5 */
  double M01[6] = { 0.2500, 0.2638, 0.2684, 0.2760, 0.2760, 0.2684};

  double Mx1[6] = {0.0229,    0.9473,    0.2578,   -0.7316,   -0.7316,    0.2578};
  double My1[6] = {-0.0000,   -0.0000,    0.8901,    0.5082,   -0.5082,   -0.8901};
  double Mz1[6] = {1.1201,    0.4535,    0.4461,    0.4370,    0.4370,    0.4461};

  double Mxx1[6] = { 1.8665, 0.5017, 1.7378, 0.9013, 0.9013, 1.7378 };
  double Myy1[6] = { 1.8525, 1.8471, 0.5788, 1.3642, 1.3642, 0.5788 };
  double Mzz1[6] = { -0.1405, 1.1744, 1.1583, 1.1327, 1.1327, 1.1583 };
  double Mxy1[6] = { -0.0000,-0.0000,-0.6869, 1.2031,-1.2031, 0.6869 };
  double Mxz1[6] = { -0.3044,-1.4666,-0.5865, 0.8093, 0.8093,-0.5865 };
  double Myz1[6] = {  0.0000, 0.0000,-1.1971,-0.7241, 0.7241, 1.1971 }; 

  /* 5 tap kmag = 0.25 */

  double M02[6] = { 0.7286, 0.7305, 0.7312, 0.7323, 0.7323, 0.7312};

  double Mx2[6] = {0.0010,    0.7616,    0.2270,   -0.6077,   -0.6077,    0.2270};
  double My2[6] = {-0.0000,   -0.0000,    0.7216,    0.4361,   -0.4361,   -0.7216};
  double Mz2[6] = {0.8607,    0.3706,    0.3703,    0.3699,    0.3699,    0.3703};

  double Mxx2[6] = { 1.2048,-1.4741, 0.9749,-0.5593,-0.5593, 0.9749};
  double Myy2[6] = { 1.2043, 1.2167,-1.2321, 0.3024, 0.3024,-1.2321};
  double Mzz2[6] = { -2.1358, 0.5305, 0.5301, 0.5295, 0.5295, 0.5301};
  double Mxy2[6] = { -0.0000, 0.0000,-1.2283, 2.0323,-2.0323, 1.2283};
  double Mxz2[6] = { -0.0397,-2.1228,-0.6533, 1.7218, 1.7218,-0.6533};
  double Myz2[6] = { -0.0000, 0.0000,-2.0216,-1.2480, 1.2480, 2.0216};

  /* 5 tap kmag = 0.10 */
  double M05[6] = { 0.9517, 0.9517, 0.9517, 0.9518, 0.9518, 0.9517 };

  double Mx5[6] = {0.0000,    1.4907,    0.4579,   -1.2033,   -1.2033,    0.4579};
  double My5[6] = {-0.0000,    0.0000,    1.4169,    0.8725,   -0.8725,   -1.4169};
  double Mz5[6] = {1.6694,    0.7418,    0.7417,    0.7417,    0.7417,    0.7417};

  double Mxx5[6] = {  1.2048,-1.4741, 0.9749,-0.5593,-0.5593, 0.9749};
  double Myy5[6] = {  1.2043, 1.2167,-1.2321, 0.3024, 0.3024,-1.2321};
  double Mzz5[6] = { -2.1358, 0.5305, 0.5301, 0.5295, 0.5295, 0.5301};
  double Mxy5[6] = { -0.0000, 0.0000,-1.2283, 2.0323,-2.0323, 1.2283};
  double Mxz5[6] = { -0.0397,-2.1228,-0.6533, 1.7218, 1.7218,-0.6533};
  double Myz5[6] = { -0.0000, 0.0000,-2.0216,-1.2480, 1.2480, 2.0216};  
#else  // FIVE_TAP
	// Matrix values for 7-tap filters
	//kmag = 0.5
	double M01[VolumeFile::NALPHA] = {0.1250,    0.1355,    0.1391,    0.1450,    0.1450,    0.1391};
	double Mx1[VolumeFile::NALPHA] = {0.0460,    1.2072,    0.3367,   -0.9407,   -0.9407,    0.3367};
	double My1[VolumeFile::NALPHA] = {0.0000,   -0.0000,    1.1375,    0.6586,   -0.6586,   -1.1375};
	double Mz1[VolumeFile::NALPHA] = {1.4419,    0.6148,    0.5990,    0.5789,    0.5789,    0.5990};
	double Mxx1[VolumeFile::NALPHA] = {-0.4021,   -1.4259,   -0.4345,   -1.0266,   -1.0266,   -0.4345};
	double Myy1[VolumeFile::NALPHA] = {-0.3991,   -0.3644,   -1.3160,   -0.6614,   -0.6614,   -1.3160};
	double Mzz1[VolumeFile::NALPHA] = {-1.5336,   -0.4476,   -0.4376,   -0.4220,   -0.4220,   -0.4376};
	double Mxy1[VolumeFile::NALPHA] = {0.0000,    0.0000,   -0.5893,    1.0177,   -1.0177,    0.5893};
	double Mxz1[VolumeFile::NALPHA] = {0.0729,   -0.9400,   -0.1813,    0.9950,    0.9950,   -0.1813};
	double Myz1[VolumeFile::NALPHA] = {0.0000,   -0.0000,   -1.0269,   -0.6126,    0.6126,    1.0269};

	//kmag = 0.25
	double Mx2[VolumeFile::NALPHA] = {0.0012,    0.5935,    0.1771,   -0.4738,   -0.4738,    0.1771};
	double My2[VolumeFile::NALPHA] = {-0.0000,   -0.0000,    0.5623,    0.3401,   -0.3401,   -0.5623};
	double Mz2[VolumeFile::NALPHA] = {0.6709,    0.2897,    0.2893,    0.2888,    0.2888,    0.2893};
	double M02[VolumeFile::NALPHA] = {0.6219,    0.6244,    0.6252,    0.6266,    0.6266,    0.6252};
	double Mxx2[VolumeFile::NALPHA] = {0.6046,   -0.6499,    0.4959,   -0.2216,   -0.2216,    0.4959};
	double Myy2[VolumeFile::NALPHA] = {0.6044,    0.6093,   -0.5365,    0.1811,    0.1811,   -0.5365};
	double Mzz2[VolumeFile::NALPHA] = {-0.9634,    0.2857,    0.2854,    0.2848,    0.2848,    0.2854};
	double Mxy2[VolumeFile::NALPHA] = {0.0000,    0.0000,   -0.6384,    1.0555,   -1.0555,    0.6384};
	double Mxz2[VolumeFile::NALPHA] = {-0.0221,   -1.1053,   -0.3410,    0.8928,    0.8928,   -0.3410};
	double Myz2[VolumeFile::NALPHA] = {-0.0000,    0.0000,   -1.0508,   -0.6482,    0.6482,    1.0508};

	//kmag = 0.1
	double M05[VolumeFile::NALPHA] = {0.9284,    0.9285,    0.9285,    0.9285,    0.9285,    0.9285};
	double Mx5[VolumeFile::NALPHA] = {0.0001,    1.0187,    0.3129,   -0.8223,   -0.8223,    0.3129};
	double My5[VolumeFile::NALPHA] = {-0.0000,   -0.0000,    0.9682,    0.5962,   -0.5962,   -0.9682};
	double Mz5[VolumeFile::NALPHA] = {1.1408,    0.5069,    0.5069,    0.5069,    0.5069,    0.5069};
	double Mxx5[VolumeFile::NALPHA] = {2.4273,   -3.3025,    1.8895,   -1.3265,   -1.3265,    1.8895};
	double Myy5[VolumeFile::NALPHA] = {2.4273,    2.4319,   -2.7601,    0.4560,    0.4560,   -2.7601};
	double Mzz5[VolumeFile::NALPHA] = {-4.7297,    0.9955,    0.9954,    0.9954,    0.9954,    0.9954};
	double Mxy5[VolumeFile::NALPHA] = {-0.0000,    0.0000,   -2.8244,    4.5867,   -4.5867,    2.8244};
	double Mxz5[VolumeFile::NALPHA] = {-0.0134,   -4.8158,   -1.4865,    3.9001,    3.9001,   -1.4865};
	double Myz5[VolumeFile::NALPHA] = {-0.0000,    0.0000,   -4.5823,   -2.8319,    2.8319,    4.5823};
#endif  // FIVE_TAP

#ifdef FIVE_TAP
#define		FILTSIZE 	5
#else
#define		FILTSIZE 	7
#endif

/**
 * Constructor.
 */
BrainModelVolumeGradient::BrainModelVolumeGradient(BrainSet* bs,
                                                   const int lambdaIn,
                                                   const bool gradFlagIn, 
                                                   const bool maskingFlagIn, 
                                                   VolumeFile* volumeFileIn,
                                                   VolumeFile* wholeMaskVolumeIn,
                                                   VectorFile* gradFileIn)
   : BrainModelAlgorithm(bs)
{
   lambda = lambdaIn;
   gradFlag = gradFlagIn;
   maskingFlag = maskingFlagIn;
   volumeFile = new VolumeFile(*volumeFileIn);
   wholeMaskVolume = new VolumeFile(*wholeMaskVolumeIn);
   gradFile = gradFileIn;
}
                         
/**
 * Destructor.
 */
BrainModelVolumeGradient::~BrainModelVolumeGradient()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeGradient::execute() throw (BrainModelAlgorithmException)      
{
/*
	float	N[NALPHA][3];
	int	i, j, k;
	float	pi = 4*std::atan(1.0);
	float	phi = 2*pi/5.0;
        float   *Grad[4];
	float	*wholevolume, *volume, kmag;
        double  R[NALPHA];
        float   Wnot;
        int     jj;
	int	CHUNK;
	float	*WholeSinVolume[NALPHA];
	char	file2[256];
        int     cnt = 0;
*/
   int nncol, nnrow, nnslices;
   volumeFile->getDimensions(nncol, nnrow, nnslices);
   
	//kmag = 0.5
	double WMx1[NALPHA], WMy1[NALPHA], WMz1[NALPHA];
	double WMxx1[NALPHA], WMyy1[NALPHA], WMzz1[NALPHA];
	double WMxy1[NALPHA], WMxz1[NALPHA], WMyz1[NALPHA];

	//kmag = 0.25
	double WMx2[NALPHA], WMy2[NALPHA], WMz2[NALPHA];
	double WMxx2[NALPHA], WMyy2[NALPHA], WMzz2[NALPHA];
	double WMxy2[NALPHA], WMxz2[NALPHA], WMyz2[NALPHA];

	//kmag = 0.1
	double WMx5[NALPHA], WMy5[NALPHA], WMz5[NALPHA];
	double WMxx5[NALPHA], WMyy5[NALPHA], WMzz5[NALPHA];
	double WMxy5[NALPHA], WMxz5[NALPHA], WMyz5[NALPHA];

	for (int i = 0; i < NALPHA; i++) {
       WMx1[i] = Mx1[i];
       WMx2[i] = Mx2[i];
       WMx5[i] = Mx5[i];
       WMxx1[i] = Mxx1[i];
       WMxx2[i] = Mxx2[i];
       WMxx5[i] = Mxx5[i];
       WMxy1[i] = Mxy1[i];
       WMxy2[i] = Mxy2[i];
       WMxy5[i] = Mxy5[i];
       WMxz1[i] = Mxz1[i];
       WMxz2[i] = Mxz2[i];
       WMxz5[i] = Mxz5[i];
       WMy1[i] = My1[i];
       WMy2[i] = My2[i];
       WMy5[i] = My5[i];
       WMyy1[i] = Myy1[i];
       WMyy2[i] = Myy2[i];
       WMyy5[i] = Myy5[i];
       WMyz1[i] = Myz1[i];
       WMyz2[i] = Myz2[i];
       WMyz5[i] = Myz5[i];
       WMz1[i] = Mz1[i];
       WMz2[i] = Mz2[i];
       WMz5[i] = Mz5[i];
       WMzz1[i] = Mzz1[i];
       WMzz2[i] = Mzz2[i];
       WMzz5[i] = Mzz5[i];
	}

   if (DebugControl::getDebugOn()) {
      std::cout << "\t\tGrad " << maskingFlag << std::endl;
   }

   const int numVoxels = nncol * nnrow * nnslices;
   int cnt = 0;
   for (int i = 0; i < numVoxels; i++) {
     if (wholeMaskVolume->getVoxelWithFlatIndex(i) != 0) {
        cnt++;
      }
   }
   if (DebugControl::getDebugOn()) {
      std::cout << "\t\t\t" << cnt << " voxels turned on in mask" << std::endl;
   }

	const float kmag = M_PI / (2.0*lambda); 
	if ((lambda != 1) && (lambda != 2) && (lambda != 5)){
	   std::cout << "Unrecognized lambda " << lambda << std::endl;
	   throw BrainModelAlgorithmException("Unrecognized lambda");
	}
	
	float Wnot = 0.0;
	switch (lambda){
	   case 1:
         for (int i = 0; i < NALPHA; i++) {
            Wnot += M01[i]; 
         }
         break;
	   case 2:
         for (int i = 0; i < NALPHA; i++) {
            Wnot += M02[i]; 
         }
         break;
	   case 5:
   		for (int i = 0; i < NALPHA; i++) {
	   	   Wnot += M05[i]; 
         }
		   break;
	  default:
	   throw BrainModelAlgorithmException("Unrecognized lambda");
	}
	Wnot = Wnot/(float)NALPHA;
   if (DebugControl::getDebugOn()) {
      std::cout << "lambda " << lambda 
                << ", kmag " << kmag 
                << ", Wnot " << Wnot 
                << ", FILTER " << FILTSIZE << std::endl;
   }

	// Apply to matrices
	switch (lambda){
	   case 1:
         for (int i = 0; i < NALPHA; i++){
            WMx1[i] *= Wnot;
            WMy1[i] *= Wnot;
            WMz1[i] *= Wnot;
            WMxx1[i] *= Wnot;
            WMyy1[i] *= Wnot;
            WMzz1[i] *= Wnot;
            WMxy1[i] *= Wnot;
            WMxz1[i] *= Wnot;
            WMyz1[i] *= Wnot;
         }
         break;
	   case 2:
         for (int i = 0; i < NALPHA; i++){
            WMx2[i] *= Wnot;
            WMy2[i] *= Wnot;
            WMz2[i] *= Wnot;
            WMxx2[i] *= Wnot;
            WMyy2[i] *= Wnot;
            WMzz2[i] *= Wnot;
            WMxy2[i] *= Wnot;
            WMxz2[i] *= Wnot;
            WMyz2[i] *= Wnot;
         }
         break;
	   case 5:
         for (int i = 0; i < NALPHA; i++){
            WMx5[i] *= Wnot;
            WMy5[i] *= Wnot;
            WMz5[i] *= Wnot;
            WMxx5[i] *= Wnot;
            WMyy5[i] *= Wnot;
            WMzz5[i] *= Wnot;
            WMxy5[i] *= Wnot;
            WMxz5[i] *= Wnot;
            WMyz5[i] *= Wnot;
         }
         break;
	  default:
		   throw BrainModelAlgorithmException("Invalid lambda in VolumeFile::performGradient");
	}

   //look_idx = 417;
   const int look_idx = -1;
   if (DebugControl::getDebugOn()) {
      std::cout << "Looking for " << look_idx << " in " << numVoxels << std::endl;
      for (int k = 0; k < nnslices; k++){
         for (int j = 0; j < nnrow; j++){
            for (int i = 0; i < nncol; i++){
               const int idx1 = volumeFile->getVoxelDataIndex(i, j, k);
               if (look_idx == idx1) {
                  std::cout << "here it is: " << i << " " << j << " " 
                             << k << "; " << idx1 << std::endl;
               }
            }
         }
      }
   }

	VolumeFile wholeVolume(*volumeFile);

	float	N[NALPHA][3];
	float	phi = 2.0 * M_PI / 5.0;
	computeWaveVectors(N, kmag, phi);	

	float* wholeSinVolume[NALPHA];
   for (int i = 0; i < NALPHA; i++) {
      wholeSinVolume[i] = new float[numVoxels];
      for (int j = 0; j < numVoxels; j++) {
	      wholeSinVolume[i][j] = 0.0;
      }
	}

	for (int CHUNK = 0; CHUNK < 7; CHUNK++) {
      float   *SinVolume[NALPHA];
      float   *CosVolume[NALPHA];
      //%unsigned char maskvolume;
      const int ncol = nncol;
      const int nrow = nnrow;

#define CHUNKLIKE
#ifdef CHUNKLIKE
      int slice_range[2] = { 0, 0 };
      if (CHUNK == 0){
           slice_range[0] = 0;
           slice_range[1] = (1*nnslices)/7+FILTSIZE/2;
      }
      else if (CHUNK == 1){
           slice_range[0] = (1*nnslices)/7-FILTSIZE/2;
           slice_range[1] = (2*nnslices)/7+FILTSIZE/2;
      }
      else if (CHUNK == 2){
           slice_range[0] = (2*nnslices)/7-FILTSIZE/2;
           slice_range[1] = (3*nnslices)/7+FILTSIZE/2;
      }
      else if (CHUNK == 3){ 
           slice_range[0] = (3*nnslices)/7-FILTSIZE/2;
           slice_range[1] = (4*nnslices)/7+FILTSIZE/2;
      }
      else if (CHUNK == 4){ 
           slice_range[0] = (4*nnslices)/7-FILTSIZE/2;
           slice_range[1] = (5*nnslices)/7+FILTSIZE/2;
      }
      else if (CHUNK == 5){ 
           slice_range[0] = (5*nnslices)/7-FILTSIZE/2;
           slice_range[1] = (6*nnslices)/7+FILTSIZE/2;
      }
      else{
           slice_range[0] = (6*nnslices)/7-FILTSIZE/2;
           slice_range[1] = nnslices; 
      }
      if ((slice_range[0] < 0) || (slice_range[0] > nnslices) ||
          (slice_range[1] < 0) || (slice_range[1] > nnslices)) {
         std::ostringstream str;
         str << "SLICE RANGE OUT OF WHACK! "
             << slice_range[0] << " " << slice_range[1] << std::endl;
         throw BrainModelAlgorithmException(str.str().c_str());
      }
#else // CHUNKLIKE
      slice_range[0] = 0;
      slice_range[1] = nnslices;
#endif // CHUNKLIKE

      const int nslices = slice_range[1] - slice_range[0] + 1;
      //%volume = new float[ncol*nrow*nslices];
      //%if (masking == 1)
      //%   maskvolume = new unsigned char[ncol*nrow*nslices];
      VolumeFile volume(*volumeFile);
      VolumeFile maskVolume(*volumeFile);
      if (DebugControl::getDebugOn()) {
         std::cout << "\n**** Analyzing "
                   << CHUNK << ": "
                   << 0 << " " << ncol << "; "
                   << 0 << " " << nrow << "; "
                   << slice_range[0] << " " << slice_range[1]
                   << std::endl;
      }
                
      for (int k = slice_range[0]; k < slice_range[1]; k++) {
         for (int j = 0; j < nnrow; j++) {
            for (int i = 0; i < nncol; i++) {
/*
               int idx1 = volume.getVoxelDataIndex(i, j, (k-slice_range[0]));
               int idx2 = wholeVolume.getVoxelDataIndex (i, j, k);
               volume.setVoxelWithFlatIndex(idx1, wholeVolume.getVoxelWithFlatIndex(idx2)); 
               if (maskingFlag) {
                  maskVolume.setVoxelWithFlatIndex(idx1,  
                                                  wholeMaskVolume->getVoxelWithFlatIndex(idx2)); 
               }
*/
               volume.setVoxel(i, j, (k - slice_range[0]), 0,
                               wholeVolume.getVoxel(i, j, k));
               if (maskingFlag) {
                  maskVolume.setVoxel(i, j, (k - slice_range[0]), 0,
                                      wholeMaskVolume->getVoxel(i, j, k));
               }
            }
         }
      }
      gradientCosTable[0] = new float[ncol];
      gradientCosTable[1] = new float[nrow];
      gradientCosTable[2] = new float[nslices];
      gradientSinTable[0] = new float[ncol];
      gradientSinTable[1] = new float[nrow];
      gradientSinTable[2] = new float[nslices];

      for (int i = 0; i < NALPHA; i++) {
         SinVolume[i] = new float[ncol*nrow*nslices];
         CosVolume[i] = new float[ncol*nrow*nslices];
         for (int j = 0; j < ncol*nrow*nslices; j++){
            CosVolume[i][j] = 0.0; 
            SinVolume[i][j] = 0.0; 
         }
      }

      // compute vector convolution field (or apply filter bank) 
      // output is 6 volumes for cos and 6 volumes for sin 
      for (int i = 0; i < NALPHA; i++) {
         const float	t1 = std::pow(Wnot, 0.3333f);
         if (DebugControl::getDebugOn()) {
            std::cout << "\tCompute dodecahedral filtering for alpha = " << i << std::endl;
         }
         computeTables(N[i], ncol, nrow, nslices);
         mod3d(volume.getVoxelData(), CosVolume[i], SinVolume[i], ncol, nrow, nslices);
#ifdef FIVE_TAP
         LPF_5(CosVolume[i], ncol, nrow, nslices, t1);
         LPF_5(SinVolume[i], ncol, nrow, nslices, t1);
         //printf ("LPF_5 Cos %f; %f %f %f\n", CosVolume[i][look_idx], N[i][0], N[i][1], N[i][2]);
         //printf ("LPF_5 Sin %f\n", SinVolume[i][look_idx]);
              //exit (-1);
#else
         NormLPF_7(CosVolume[i], ncol, nrow, nslices, t1);
         NormLPF_7(SinVolume[i], ncol, nrow, nslices, t1);
#endif
         demod3d(CosVolume[i], SinVolume[i], ncol, nrow, nslices);
      }

	   {
         int	kstart, kend;
#ifdef CHUNKLIKE
         if (CHUNK == 0){
           kstart = 0;
           kend = (1*nnslices)/7;
         }
         else if (CHUNK == 1){
           kstart = (1*nnslices)/7;
           kend = (2*nnslices)/7;
         }
         else if (CHUNK == 2){
           kstart = (2*nnslices)/7;
           kend = (3*nnslices)/7;
         }
         else if (CHUNK == 3){ 
           kstart = (3*nnslices)/7;
           kend = (4*nnslices)/7;
         }
         else if (CHUNK == 4){ 
           kstart = (4*nnslices)/7;
           kend = (5*nnslices)/7;
         }
         else if (CHUNK == 5){ 
           kstart = (5*nnslices)/7;
           kend = (6*nnslices)/7;
         }
         else{
           kstart = (6*nnslices)/7;
           kend = nnslices; 
         }
#else // CHUNKLIKE
         kstart = 0;
         kend = nnslices;
#endif // CHUNKLIKE
         if (DebugControl::getDebugOn()) {
            std::cout << "Copy slab sine volume back into large volume: "
                      << kstart << " to " << kend << "..." << std::endl;
         }
         for (int k = kstart; k < kend; k++) {
            if (DebugControl::getDebugOn()) {
               if ((k % 10) == 0) {
                  std::cout << "\t" << k << " of " << slice_range[1] << std::endl;
               }
            }
            for (int j = 0; j < nnrow; j++) {
               for(int i = 0; i < nncol; i++) {
                  for (int jj = 0; jj < NALPHA; jj++) {
                     const int idx1 = volumeFile->getVoxelDataIndex(i, j, (k-slice_range[0]));
                     const int idx2 = volumeFile->getVoxelDataIndex(i, j, k);
                     wholeSinVolume[jj][idx2] = SinVolume[jj][idx1];
                     if (DebugControl::getDebugOn()) {
                        if (idx2 == look_idx) {
                           std::cout << "\t\there " << jj << " " << idx2 << ": "
                                     << i << " " << j << " " << k << " gets "
                                     << idx1 << " "
                                     << i << " " << j << " " << (k - slice_range[0])
                                     << SinVolume[jj][idx1] << std::endl;
                        }
                     }
                  } 
               }
            }
         }
      }
      for (int i = 0; i < 3; i++){
         delete[] gradientCosTable[i]; 
         delete[] gradientSinTable[i]; 
      }
      for (int i = 0; i < NALPHA; i++){
         delete[] SinVolume[i];
         delete[] CosVolume[i];
      }
      //%if (masking == 1)
      //%   delete[] maskvolume;
      //%delete[] volume;
	} // CHUNK

	//%delete[] wholevolume;

	if (gradFlag) {
      if (DebugControl::getDebugOn()) {
	      std::cout << "Compute GRADIENT..." << std::endl;
      }
      const int numVox = nncol*nnrow*nnslices;
 	   for (int j = 0; j < numVox; j++) {
         gradFile->setVectorWithFlatIndex(j, 0.0, 0.0, 0.0);
         gradFile->setMagnitudeWithFlatIndex(j, 0.0);
	      //%GradX[j] = 0.0;
	      //%GradY[j] = 0.0;
	      //%GradZ[j] = 0.0;
	      //%GradMag[j] = 0.0;
	   }
      
      float* Grad[4];
      for (int i = 0; i < 3; i++){
         Grad[i] = new float[numVox];
         for (int j = 0; j < numVox; j++) {
            Grad[i][j] = 0.0;
         }
	   }

 	   for (int i = 0; i < numVox; i++) {
         if (DebugControl::getDebugOn()) {
	         if ((i % 500000) == 0) {
               std::cout << "\tGradient for node " << i << " of "
                         << numVox << "; "
                         << 100.0*((float)i/(float)(numVox)) << std::endl;
            }
	      }
	      if (((maskingFlag) && (wholeMaskVolume->getVoxelWithFlatIndex(i) != 0)) ||
             (maskingFlag == false)) {
            double  R[NALPHA];
	         for (int jj = 0; jj < NALPHA; jj++) {
	            R[jj] = wholeSinVolume[jj][i];
            }
            switch (lambda){
               case 1:
                  applyOddMatrix(i, R, Grad, WMx1, WMy1, WMz1);
                  break;
               case 2:
                  applyOddMatrix(i, R, Grad, WMx2, WMy2, WMz2);
                  break;
               case 5:
                  applyOddMatrix(i, R, Grad, WMx5, WMy5, WMz5);
                  break;
               default:
                  throw BrainModelAlgorithmException("Invalid lambda value.");
            }
         } // masking
	   }
      for (int i = 0; i < NALPHA; i++) {
         delete[] wholeSinVolume[i]; 
      }
      
      Grad[3] = new float[numVox];
 	   for (int i = 0; i < numVox; i++) {
	      float	mag = Grad[0][i]*Grad[0][i] + Grad[1][i]*Grad[1][i] +
                     Grad[2][i]*Grad[2][i];
	      if (mag > 0.0) {
	         Grad[3][i] = std::sqrt(mag);
            for (int k = 0; k < 3; k++) {
               Grad[k][i] = Grad[k][i]/Grad[3][i];
            }
	      }
	      else {
	         Grad[3][i] = 0.0; 
         }
	   }
      
      //sprintf (file2, "%s.grad.vec", rootfile);
      //%sprintf (file2, "%s.vec", outfile);
	   //%printf ("Write data to %s\n", file2);
      
      //%WriteRawVectorData (file2, Grad[0], Grad[1], Grad[2],
      //%          Grad[3], nncol, nnrow, nnslices);
	   for (int i = 0; i < numVox; i++) {
         gradFile->setVectorWithFlatIndex(i, Grad[0][i], Grad[1][i], Grad[2][i]);
         gradFile->setMagnitudeWithFlatIndex(i, Grad[3][i]);
	      //%GradX[i] = Grad[0][i];
	      //%GradY[i] = Grad[1][i];
	      //%GradZ[i] = Grad[2][i];
	      //%GradMag[i] = Grad[3][i];
	   }
      for (int i = 0; i < 4; i++) {
         delete[] Grad[i];
      }
	} // (grad_flag == 1)

   if (DebugControl::getDebugOn()) {
   	std::cout << "Grad done..." << std::endl;
   }
}

/**
 * compute wave vectors.
 */
void	
BrainModelVolumeGradient::computeWaveVectors(float N[NALPHA][3], const float kmag, const float phi)
{
	N[0][0] = 0.0;
	N[0][1] = 0.0;
	N[0][2] = kmag; 
	for (int i = 0; i < 5; i++) {	
	   N[i+1][0] = 2*kmag* std::cos(i*phi)/ std::sqrt (5.0);
	   N[i+1][1] = 2*kmag* std::sin(i*phi)/ std::sqrt (5.0);
	   N[i+1][2] = kmag/std::sqrt (5.0); 
	}
   if (DebugControl::getDebugOn()) {
      for (int i = 0; i < NALPHA; i++) {	
         std::cout << "Direction cosine " << i 
                   << ": " << N[i][0] << " " << N[i][1] << " " << N[i][2]
                   << std::endl;
      }
   }
}

/**
 * Compute some sin/cos tables
 */
void	
BrainModelVolumeGradient::computeTables(const float *N, const int ncol, const int nrow, const int nslices)
{
	for (int i = 0; i < ncol; i++){
	   gradientCosTable[0][i] = std::cos(i*N[0]);
	   gradientSinTable[0][i] = std::sin(i*N[0]);
	}
	for (int j = 0; j < nrow; j++){
	   gradientCosTable[1][j] = std::cos(j*N[1]);
	   gradientSinTable[1][j] = std::sin(j*N[1]);
	}
	for (int k = 0; k < nslices; k++){
	   gradientCosTable[2][k] = std::cos(k*N[2]);
	   //printf 2"TZ %d %f\n", k, cos (k*N[2]));
	   gradientSinTable[2][k] = std::sin(k*N[2]);
	}
}

/**
 * Gradient function
 */
void	
BrainModelVolumeGradient::mod3d(float* voxels, float *Cos, float *Sin, const int ncol, const int nrow, const int nslices)
{
	for (int k = 0; k < nslices; k++){
	   const float cz = gradientCosTable[2][k];
	   const float sz = gradientSinTable[2][k];
	   for (int j = 0; j < nrow; j++){
	      const float cy = gradientCosTable[1][j];
	      const float sy = gradientSinTable[1][j];
	      const float cyz = cy*cz - sy*sz;
	      const float syz = sy*cz + cy*sz;
	      for (int i = 0; i < ncol; i++){
	         const float cx = gradientCosTable[0][i];
	         const float sx = gradientSinTable[0][i];
            const float cxyz = cx*cyz - sx*syz;	 
            const float sxyz = sx*cyz + cx*syz;	 
            const int idx = (j*ncol)+i+(k*ncol*nrow);
            Cos[idx] = cxyz * voxels[idx];
            Sin[idx] = sxyz * voxels[idx];
/*
            if (idx == look_idx) 
               printf ("idx %d: cxyz %f, sxyz %f, cos %f, sin %f, vol %f\n",
                        idx, cxyz, sxyz, Cos[idx], Sin[idx], volume[idx]);
*/
         }
	   }
	}
}

/**
 *
 */
void    
BrainModelVolumeGradient::LPF_5(float* voxels, 
                                const int ncol, const int nrow, const int nslices,
                                const float Wo)
{
   float   lpf_filter[5];
   lpf_filter[0] = 1.0/16.0;
   lpf_filter[1] = 1.0/4.0;
   lpf_filter[2] = 3.0/8.0;
   lpf_filter[3] = 1.0/4.0;
   lpf_filter[4] = 1.0/16.0;
   
   for (int i = 0; i < 5; i++) {
     //printf ("Filter %d %f\n", i, lpf_filter[i]);
     lpf_filter[i] = lpf_filter[i] / Wo;
     //printf ("Filter %d %f\n", i, lpf_filter[i]);
	}
	//printf ("before lpf %f %f\n", volume[417], Wo);
   VolumeFile::seperableConvolve(ncol, nrow, nslices, voxels, lpf_filter);
	//printf ("after lpf %f %f\n", volume[417], Wo);
}

/**
 *
 */
/*
void	
BrainModelVolumeGradient::seperableConvolve(int ncol, int nrow, int nslices, 
	                            	float *volume, float *filter)
{
	int	isize = ncol*nrow;

	//printf ("\tSeperableConvolve filtersize=%d\n", FSIZE); 
	float* Result = new float[ncol*nrow*nslices];
	float* TempSpace = new float[ncol*nrow*nslices];

	float* voxel = volume; 
	float* tempResult = Result;
	oneDimConvolve (voxel, tempResult, filter, 0, 1, ncol, nrow, nslices);	
	//printf ("%d %f %f\n", idx, voxel[idx], tempResult[idx]);

	voxel = Result; 
	tempResult = TempSpace;
	oneDimConvolve (voxel, tempResult, filter, 1, ncol, ncol, nrow, nslices);	
	//printf ("%d %f %f\n", idx, voxel[idx], tempResult[idx]);

	voxel = TempSpace; 
	tempResult = volume;
	oneDimConvolve (voxel, tempResult, filter, 2, isize, ncol, nrow, nslices);
	//printf ("%d %f %f\n", idx, voxel[idx], tempResult[idx]);

	delete[] Result; 
	delete[] TempSpace; 
	return;
}
*/
/**
 *
 */
/*
void	
BrainModelVolumeGradient::oneDimConvolve (float *voxel, float *tempResult, float *filter, 
                const int dim, const int inc,
		          const int ncol, const int nrow, const int nslices)
{
	float	p[FILTSIZE];

	int cnt = 0;
	for (int k = 0; k < nslices; k++){
	   for (int j = 0; j < nrow; j++){
	      for (int i = 0; i < ncol; i++){
             float* ip = voxel;	 
             if (dim == 0){
                if (i == 0){
                   p[0] = *ip;
                   p[1] = *ip;
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                        }
                else if (i == 1) {
                   p[0] = *(ip-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                }
                else if (i == ncol-2) {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc);
                }
                else if (i == ncol-1) {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *ip;
                   p[4] = *ip;
                }
                else {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                }
             }
             else if (dim == 1) {
                if (j == 0){
                   p[0] = *ip;
                   p[1] = *ip;
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                        }
                else if (j == 1) {
                   p[0] = *(ip-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                }
                else if (j == nrow-2) {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc);
                }
                else if (j == nrow-1) {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *ip;
                   p[4] = *ip;
                }
	             else {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                }
             }
             else if (dim == 2) {
                if (k == 0){
                   p[0] = *ip;
                   p[1] = *ip;
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                        }
                else if (k == 1) {
                   p[0] = *(ip-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                }
                else if (k == nslices-2) {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc);
                }
                else if (k == nslices-1) {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *ip;
                   p[4] = *ip;
                }
                else {
                   p[0] = *(ip-inc-inc);
                   p[1] = *(ip-inc);
                   p[2] = *ip;
                   p[3] = *(ip+inc);
                   p[4] = *(ip+inc+inc);
                }
             }
             *tempResult = 0;
             for (int n = 0; n < FILTSIZE; n++){
               *tempResult += filter[n]*p[n];
             }
             tempResult++;
             voxel++;
             cnt++;
	      }
	   }
	}
}
*/

/**
 *
 */
void	
BrainModelVolumeGradient::demod3d(float *Cos, float *Sin, 
                       const int ncol, int const nrow, const int nslices)
{
	//%int	i, j, k, idx;
	//%float	cx, cy, cz;	
	//%float	sx, sy, sz;	
	//%float	cyz, syz, cxyz, sxyz;
	//%float	rval, ival;

	//printf ("\t\tdemod3d %d %d %d\n", ncol, nrow, nslices);
	for (int k = 0; k < nslices; k++){
	   const float cz = gradientCosTable[2][k];
	   const float sz = gradientSinTable[2][k];
	   for (int j = 0; j < nrow; j++){
	      const float cy = gradientCosTable[1][j];
	      const float sy = gradientSinTable[1][j];
	      const float cyz = cy*cz - sy*sz;
	      const float syz = sy*cz + cy*sz;
	      for (int i = 0; i < ncol; i++){
	         const float cx = gradientCosTable[0][i];
	         const float sx = gradientSinTable[0][i];
            const float cxyz = cx*cyz - sx*syz;	 
            const float sxyz = sx*cyz + cx*syz;	 
            const int idx = (j*ncol)+i+(k*ncol*nrow);
            const float rval = Cos[idx]; 
            const float ival = Sin[idx]; 
            Cos[idx] =  (cxyz*rval) + (sxyz*ival); 
            Sin[idx] = (-sxyz*rval) + (cxyz*ival); 
	      }
	   }
	}
}

/**
 *
 */
void 
BrainModelVolumeGradient::applyOddMatrix(int idx, const double R[NALPHA], float *Grad[4], 
	                   const double Mx[NALPHA], const double My[NALPHA], const double Mz[NALPHA])
{
	Grad[0][idx] = multRow(R, Mx);
	Grad[1][idx] = multRow(R, My);
	Grad[2][idx] = multRow(R, Mz);
	/*if (idx == 777534)
	   printf ("%d: %f %f %f, %f %f %f %f %f %f\n",
		idx, Grad [0][idx], Grad [1][idx], Grad [2][idx],
		R [0], R [1], R [2], R [3], R [4], R [5]);*/
}

/**
 *
 */
double 
BrainModelVolumeGradient::multRow(const double R[NALPHA], const double M[NALPHA])
{
	double ans = 0.0;

	for (int i = 0; i < NALPHA; i++) {
	   ans += R[i] * M[i];
   }

	return (ans);
}



