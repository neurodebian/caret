
#ifndef __BRAIN_MODEL_VOLUME_NEAR_TO_PLANE_H__
#define __BRAIN_MODEL_VOLUME_NEAR_TO_PLANE_H__

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

/// class for volume near to planes calculation
class BrainModelVolumeNearToPlane : public BrainModelAlgorithm {
   public:
      /// some constants
      enum {
         DIM      = 3,
         NALPHA   = 6,
         FILTSIZE = 7
      };
      
      /// Constructor
      BrainModelVolumeNearToPlane(BrainSet* bs,
                                  SureFitVectorFile* vecFileIn,
                                  const float sigmaNIn, 
                                  const float sigmaWIn, 
                                  const float offsetIn, 
                                  const bool  downflagIn, 
                                  const int gradsignIn,
                                  const bool maskingFlagIn, 
                                  VolumeFile* maskVolumeIn,
                                  VolumeFile* outputVolumeIn);
   
      /// Destructor
      ~BrainModelVolumeNearToPlane();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      ///
      void generateCoefficientMatrix(float sigmax, float sigmay, float sigmaz);

      ///
      void generateEllipsoidFilter(const float sigmax, 
                                        const float sigmay, 
                                        const float sigmaz,
		                                  const float delta, 
                                        float filter[NALPHA][FILTSIZE][FILTSIZE][FILTSIZE]);
       
      ///
      void multMatrixRow(const float b[3], const float m[3][3], float out[3]);
      
      ///
      void rotateTheta (const float C[DIM][DIM], const int alpha, float Cout[DIM][DIM]);
      
      ///
      void rotatePhi(const float C[DIM][DIM], const int alpha, float Cout[DIM][DIM]);

      ///
      void multMatrixMatrix(const float A[DIM][DIM], const float B[DIM][DIM],
                            float out[DIM][DIM]);
      
      ///
      float	newVectorConvolve(const int x, const int y, const int z, 
                  const float filter[FILTSIZE][FILTSIZE][FILTSIZE], const int signflag, 
                  float *dotproduct, const int absflag);
      
      ///
      void computeDotProduct(const int alpha, float *VectorVolumeX, float *VectorVolumeY,
		                       float *VectorVolumeZ, float *dotproduct);
      
      /// 
      float downVectorConvolve(const int alpha, 
                         const int x, 
                         const int y, 
                         const int z, 
		                   const float filter[FILTSIZE][FILTSIZE][FILTSIZE], 
		                   const int signflag, 
                         float *VectorVolume[3],
                         const int absFlag);
                         
      ///
      SureFitVectorFile* vecFile;
      
      ///
      float sigmaN;
      
      ///
      float sigmaW; 
      
      ///
      float offset; 
      
      ///
      bool  downflag;
      
      ///
      int gradsign;
      
      ///
      bool maskingFlag; 
      
      ///
      VolumeFile* maskVolume;
      
      ///
      VolumeFile* outputVolume;
};

#endif // __BRAIN_MODEL_VOLUME_NEAR_TO_PLANE_H__

