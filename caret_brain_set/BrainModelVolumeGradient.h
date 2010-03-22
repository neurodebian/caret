
#ifndef __BRAIN_MODEL_VOLUME_GRADIENT_H__
#define __BRAIN_MODEL_VOLUME_GRADIENT_H__

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
#include "BrainModelAlgorithm.h"

class SureFitVectorFile;
class VolumeFile;

/// class for creating a gradient on a volume
class BrainModelVolumeGradient : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelVolumeGradient(BrainSet* bs,
                               const int lambdaIn,
                               const bool gradFlagIn, 
                               const bool maskingFlag, 
                               VolumeFile* volumeFileIn,
                               VolumeFile* wholeMaskVolumeIn,
                               SureFitVectorFile* gradFileIn);
                               
      /// Destructor
      ~BrainModelVolumeGradient();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// lambda
      int lambda;
      
      /// grad flag
      bool gradFlag;
      
      /// masking flag
      bool maskingFlag;
      
      /// volume file
      VolumeFile* volumeFile;
      
      /// mask volume
      VolumeFile* wholeMaskVolume;
      
      /// gradient file
      SureFitVectorFile* gradFile;
      
      /// misc enums
      enum { 
              NALPHA = 6 
           };
      
      /// cosine table
      float* gradientCosTable[3];
      
      /// sine table
      float* gradientSinTable[3];

      /// compute wave vectors.
      void computeWaveVectors(float N[NALPHA][3], const float kmag, const float phi);

      /// gradient function
      void mod3d(float* voxels, float *Cos, float *Sin, const int ncol, const int nrow, const int nslices);

      /// gradient function
      void LPF_5(float* voxels, 
                 const int ncol, const int nrow, const int nslices,
                 const float Wo);

      /// Compute some sin/cos tables
      void computeTables(const float *N, const int ncol, const int nrow, const int nslices);

      /// 
      void demod3d(float *Cos, float *Sin, 
                          const int ncol, int const nrow, const int nslices);
         
      ///
      //static void seperableConvolve(int ncol, int nrow, int nslices, 
	   //                         	float *volume, float *filter);
      /// 
      //static void oneDimConvolve (float *voxel, float *tempResult, float *filter, 
      //          const int dim, const int inc,
		//          const int ncol, const int nrow, const int nslices);
                
      ///
      void applyOddMatrix(int idx, const double R[NALPHA], float *Grad[4], 
	                   const double Mx[NALPHA], const double My[NALPHA], const double Mz[NALPHA]);
                      
      ///
      double multRow(const double R[NALPHA], const double M[NALPHA]);
};
                
#endif // __BRAIN_MODEL_VOLUME_GRADIENT_H__

