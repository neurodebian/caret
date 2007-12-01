
#ifndef __BRAIN_MODEL_VOLUME_TO_SURFACE_MAPPER_ALGORITHM_PARAMETERS_H__
#define __BRAIN_MODEL_VOLUME_TO_SURFACE_MAPPER_ALGORITHM_PARAMETERS_H__

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

#include <vector>

#include <QString>

class PreferencesFile;

/// class for volume to surface algorithm parameters
class BrainModelVolumeToSurfaceMapperAlgorithmParameters {
   public:
      /// mapping algorithm
      enum ALGORITHM {
         /// metric average nodes mapping algorithm
         ALGORITHM_METRIC_AVERAGE_NODES,
         /// metric average voxel mapping algorithm
         ALGORITHM_METRIC_AVERAGE_VOXEL,
         /// metric enclosing voxel mapping algorithm
         ALGORITHM_METRIC_ENCLOSING_VOXEL,
         /// metric gaussian mapping algorithm
         ALGORITHM_METRIC_GAUSSIAN,
         /// metric interpolated voxel mapping algorithm
         ALGORITHM_METRIC_INTERPOLATED_VOXEL,
         /// metric maximum voxel mapping algorithm
         ALGORITHM_METRIC_MAXIMUM_VOXEL,
         /// metric MCW brainfish mapping algorithm
         ALGORITHM_METRIC_MCW_BRAINFISH,
         /// metric strongest voxel mapping algorithm
         ALGORITHM_METRIC_STRONGEST_VOXEL,
         /// paint enclosing voxel mapping algorithm
         ALGORITHM_PAINT_ENCLOSING_VOXEL
      };
      
      // constructor
      BrainModelVolumeToSurfaceMapperAlgorithmParameters();
      
      // destructor
      ~BrainModelVolumeToSurfaceMapperAlgorithmParameters();
      
      // transfer parameters from preferences file
      void transferParametersFromPreferencesFile(const PreferencesFile* pf);
      
      // transfer parameters to preferences file
      void transferParametersToPreferncesFile(PreferencesFile* pf,
                                              const bool writePreferencesFileFlag);
      
      // get algorithm
      ALGORITHM getAlgorithm() const;
      
      // set algorithm
      void setAlgorithm(const ALGORITHM alg);
      
      // get the algorithm names and values
      static void getAlgorithmNamesAndValues(std::vector<QString>& namesOut,
                                             std::vector<ALGORITHM>& valuesOut);
                                             
      // get the algorithm name
      static QString getAlgorithmName(const ALGORITHM alg);
      
      // get algorithm from string
      static ALGORITHM getAlgorithmFromNameString(const QString& s);
      
      // get algorithm parameter as string for comment usage
      QString getAlgorithmParametersForComment() const;
      
      // get the algorithm parameters as a equal and semicolon separated string
      QString getAlgorithmParametersAsString() const;
      
      // set the algorithm parameters from a string
      void setAlgorithmParametersFromString(const QString& s);
      
      // get the average voxel parameters
      void getAlgorithmMetricAverageVoxelParameters(float neighborsOut) const;
      
      // set the average voxel parameters
      void setAlgorithmMetricAverageVoxelParameters(const float neighborsIn);
      
      // get the maximum voxel parameters
      void getAlgorithmMetricMaximumVoxelParameters(float neighborsOut) const;
      
      // set the maximum voxel parameters
      void setAlgorithmMetricMaximumVoxelParameters(const float neighborsIn);
      
      // get the strongest voxel parameters
      void getAlgorithmMetricStrongestVoxelParameters(float neighborsOut) const;
      
      // set the strongest voxel parameters
      void setAlgorithmMetricStrongestVoxelParameters(const float neighborsIn);
      
      // set gaussian algorithm parameters
      void getAlgorithmMetricGaussianParameters(float& gaussianNeighborsOut,
                                          float& gaussianSigmaNormOut,
                                          float& gaussianSigmaTangOut,
                                          float& gaussianNormBelowCutoffOut,
                                          float& gaussianNormAboveCutoffOut,
                                          float& gaussianTangCutoffOut) const;
                                          
      // set gaussian algorithm parameters
      void setAlgorithmMetricGaussianParameters(const float gaussianNeighborsIn,
                                          const float gaussianSigmaNormIn,
                                          const float gaussianSigmaTangIn,
                                          const float gaussianNormBelowCutoffIn,
                                          const float gaussianNormAboveCutoffIn,
                                          const float gaussianTangCutoffIn);
       
      // get the mcw brainfish algorithm parameters
      void getAlgorithmMetricMcwBrainFishParameters(float& mcwBrainFishMaxDistanceOut,
                                              int& mcwBrainFishSplatFactorOut) const;
                                              
      // set the mcw brainfish algorithm parameters
      void setAlgorithmMetricMcwBrainFishParameters(const float mcwBrainFishMaxDistanceIn,
                                              const int mcwBrainFishSplatFactorIn);
   
      /// get PALS metric average fiducial mapping enabled
      bool getPalsMetricAverageFiducialMappingEnabled() const { return palsMetricAverageFiducialMappingEnabled; }
      
      /// get PALS metric multi fiducial mapping enabled
      bool getPalsMetricMultiFiducialMappingEnabled() const { return palsMetricMultiFiducialMappingEnabled; }
      
      /// get PALS metric minimum enabled
      bool getPalsMetricMinimumEnabled() const { return palsMetricMinimumEnabled; }
      
      /// get PALS metric maximum enabled
      bool getPalsMetricMaximumEnabled() const { return palsMetricMaximumEnabled; }
      
      /// get PALS metric standard deviation enabled
      bool getPalsMetricStandardDeviationEnabled() const { return palsMetricStandardDeviationEnabled; }
      
      /// get PALS metric standard error enabled
      bool getPalsMetricStandardErrorEnabled() const { return palsMetricStandardErrorEnabled; }
      
      /// get PALS metric all cases enabled
      bool getPalsMetricAllCasesEnabled() const { return palsMetricAllCasesEnabled; }
      
      /// get PALS paint average fiducial mapping enabled
      bool getPalsPaintAverageFiducialMappingEnabled() const { return palsPaintAverageFiducialMappingEnabled; }
      
      /// get PALS paint most common enabled
      bool getPalsPaintMostCommonEnabled() const { return palsPaintMostCommonEnabled; }
      
      /// get PALS paint most common exlude unidentified enabled
      bool getPalsPaintMostCommonExcludeUnidentifiedEnabled() const { return palsPaintMostCommonExcludeUnidentifiedEnabled; }
      
      /// get PALS paint all cases enabled
      bool getPalsPaintAllCasesEnabled() const { return palsPaintAllCasesEnabled; }
      
      /// PALS metric average fiducial mapping enabled
      void setPalsMetricAverageFiducialMappingEnabled(const bool b) { palsMetricAverageFiducialMappingEnabled = b; }
      
      /// PALS metric multi fiducial mapping enabled
      void setPalsMetricMultiFiducialMappingEnabled(const bool b) { palsMetricMultiFiducialMappingEnabled = b; }
      
      /// PALS metric minimum enabled
      void setPalsMetricMinimumEnabled(const bool b) { palsMetricMinimumEnabled = b; }
      
      /// PALS metric maximum enabled
      void setPalsMetricMaximumEnabled(const bool b) { palsMetricMaximumEnabled = b; }
      
      /// PALS metric standard deviation enabled
      void setPalsMetricStandardDeviationEnabled(const bool b) { palsMetricStandardDeviationEnabled = b; }
      
      /// PALS metric standard error enabled
      void setPalsMetricStandardErrorEnabled(const bool b) { palsMetricStandardErrorEnabled = b; }
      
      /// PALS metric all cases enabled
      void setPalsMetricAllCasesEnabled(const bool b) { palsMetricAllCasesEnabled = b; }
      
      /// PALS paint average fiducial mapping enabled
      void setPalsPaintAverageFiducialMappingEnabled(const bool b) { palsPaintAverageFiducialMappingEnabled = b; }
      
      /// PALS paint most common enabled
      void setPalsPaintMostCommonEnabled(const bool b) { palsPaintMostCommonEnabled = b; }
      
      /// PALS paint most common exlude unidentified enabled
      void setPalsPaintMostCommonExcludeUnidentifiedEnabled(const bool b) { palsPaintMostCommonExcludeUnidentifiedEnabled = b; }
      
      /// PALS paint all cases enabled
      void setPalsPaintAllCasesEnabled(const bool b) { palsPaintAllCasesEnabled = b; }

      /// get the metric average nodes algorithm name
      static QString getAlgorithmMetricAverageNodesName() { return "METRIC_AVERAGE_NODES"; }
   
      /// get the metric average voxel algorithm name
      static QString getAlgorithmMetricAverageVoxelName() { return "METRIC_AVERAGE_VOXEL"; }
   
      /// get the metric enclosing voxel algorithm name
      static QString getAlgorithmMetricEnclosingVoxelName() { return "METRIC_ENCLOSING_VOXEL"; }
   
      /// get the metric gaussian algorithm name
      static QString getAlgorithmMetricGaussianName() { return "METRIC_GAUSSIAN"; }
   
      /// get the metric interpolated voxel algorithm name
      static QString getAlgorithmMetricInterpolatedVoxelName() { return "METRIC_INTERPOLATED_VOXEL"; }
   
      /// get the metric maximum voxel algorithm name
      static QString getAlgorithmMetricMaximumVoxelName() { return "METRIC_MAXIMUM_VOXEL"; }
   
      /// get the metric mcw brainfish algorithm name
      static QString getAlgorithmMetricMcwBrainFishName() { return "METRIC_MCW_BRAIN_FISH"; }
   
      /// get the metric strongest voxel algorithm name
      static QString getAlgorithmMetricStrongestVoxelName() { return "METRIC_STRONGEST_VOXEL"; }
   
      /// get the paint enclosing voxel algorithm name
      static QString getAlgorithmPaintEnclosingVoxel() { return "PAINT_ENCLOSING_VOXEL"; }
      
   protected:
      /// algorithm used for mapping
      ALGORITHM algorithm;
      
      /// average voxel neighbors
      float averageVoxelNeighbors;
      
      /// maximum voxel neighbors
      float maximumVoxelNeighbors;
      
      /// strongest voxel neighbors
      float strongestVoxelNeighbors;
      
      /// gaussian  neighbors
      float gaussianNeighbors;
      
      /// gaussian sigma norm
      float gaussianSigmaNorm;
      
      /// gaussian sigma tang
      float gaussianSigmaTang;
      
      /// gaussian norm below cutoff
      float gaussianNormBelowCutoff;
      
      /// gaussian norm above cutoff
      float gaussianNormAboveCutoff;
      
      /// gaussian tang cutoff
      float gaussianTangCutoff;
      
      /// mcw brain fish max distance
      float mcwBrainFishMaxDistance;
      
      /// mcw brain fish splat factor
      int mcwBrainFishSplatFactor;
      
      /// PALS metric average fiducial mapping enabled
      bool palsMetricAverageFiducialMappingEnabled;
      
      /// PALS metric multi fiducial mapping enabled
      bool palsMetricMultiFiducialMappingEnabled;
      
      /// PALS metric minimum enabled
      bool palsMetricMinimumEnabled;
      
      /// PALS metric maximum enabled
      bool palsMetricMaximumEnabled;
      
      /// PALS metric standard deviation enabled
      bool palsMetricStandardDeviationEnabled;
      
      /// PALS metric standard error enabled
      bool palsMetricStandardErrorEnabled;
      
      /// PALS metric all cases enabled
      bool palsMetricAllCasesEnabled;
      
      /// PALS paint average fiducial mapping enabled
      bool palsPaintAverageFiducialMappingEnabled;
      
      /// PALS paint most common enabled
      bool palsPaintMostCommonEnabled;
      
      /// PALS paint most common exlude unidentified enabled
      bool palsPaintMostCommonExcludeUnidentifiedEnabled;
      
      /// PALS paint all cases enabled
      bool palsPaintAllCasesEnabled;
      
};

#endif // __BRAIN_MODEL_VOLUME_TO_SURFACE_MAPPER_ALGORITHM_PARAMETERS_H__

