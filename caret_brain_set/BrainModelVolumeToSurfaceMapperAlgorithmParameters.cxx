
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

#include <QStringList>

#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"
#include "BrainSet.h"
#include "PreferencesFile.h"

/**
 * constructor.
 */
BrainModelVolumeToSurfaceMapperAlgorithmParameters::BrainModelVolumeToSurfaceMapperAlgorithmParameters()
{
   //
   // Create a brain set
   //
   BrainSet brainSet;
   
   //
   // Default the parameters
   //
   algorithm               = ALGORITHM_METRIC_ENCLOSING_VOXEL;
   gaussianNeighbors       = 6.0;
   averageVoxelNeighbors   = 0.0;
   maximumVoxelNeighbors   = 3.0;
   strongestVoxelNeighbors = 3.0;
   gaussianSigmaNorm       = 2.0;
   gaussianSigmaTang       = 1.0;
   gaussianNormBelowCutoff = 2.0;
   gaussianNormAboveCutoff = 2.0;
   gaussianTangCutoff      = 3.0;
   mcwBrainFishMaxDistance = 1.0;
   mcwBrainFishSplatFactor = 1;
   
   palsMetricAverageFiducialMappingEnabled = false;
   palsMetricMultiFiducialMappingEnabled = false;
   palsMetricMinimumEnabled = false;
   palsMetricMaximumEnabled = false;
   palsMetricStandardDeviationEnabled = false;
   palsMetricStandardErrorEnabled = false;
   palsMetricAllCasesEnabled = false;
   palsPaintAverageFiducialMappingEnabled = false;
   palsPaintMostCommonEnabled = false;
   palsPaintMostCommonExcludeUnidentifiedEnabled = false;
   palsPaintAllCasesEnabled = false;

   //
   // Get the preferences file which is read as part of BrainSet constructor
   //
   PreferencesFile* pf = brainSet.getPreferencesFile();
   transferParametersFromPreferencesFile(pf);
}

/**
 * destructor.
 */
BrainModelVolumeToSurfaceMapperAlgorithmParameters::~BrainModelVolumeToSurfaceMapperAlgorithmParameters()
{
}

/**
 * transfer parameters from preferences file.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::transferParametersFromPreferencesFile(const PreferencesFile* pf)
{
   const QString paramString = pf->getFmriAlgorithmParameters();
   setAlgorithmParametersFromString(paramString);
}

/**
 * transfer parameters to preferences file.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::transferParametersToPreferncesFile(
                                                       PreferencesFile* pf,
                                                       const bool writePreferencesFileFlag)
{
   pf->setFmriAlgorithmParameters(getAlgorithmParametersAsString());
   if (writePreferencesFileFlag) {
     try {
        if (pf->getFileName().isEmpty() == false) {
           pf->writeFile(pf->getFileName());
        }
     }
     catch (FileException&) {
     }
   }
}
      
/**
 * get the algorithm parameters as a equal and semicolon separated string.
 */
QString 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmParametersAsString() const
{
   QStringList sl;
   sl << "algorithm"
      << "="
      << getAlgorithmName(algorithm)
      << ";";

   sl << "averageVoxelNeighbors"
      << "="
      << QString::number(averageVoxelNeighbors, 'f', 3)
      << ";";

   sl << "maximumVoxelNeighbors"
      << "="
      << QString::number(maximumVoxelNeighbors, 'f', 3)
      << ";";

   sl << "strongestVoxelNeighbors"
      << "="
      << QString::number(strongestVoxelNeighbors, 'f', 3)
      << ";";

   sl << "gaussianNeighbors"
      << "="
      << QString::number(gaussianNeighbors, 'f', 3)
      << ";";

   sl << "gaussianSigmaNorm"
      << "="
      << QString::number(gaussianSigmaNorm, 'f', 3)
      << ";";

   sl << "gaussianSigmaTang"
      << "="
      << QString::number(gaussianSigmaTang, 'f', 3)
      << ";";

   sl << "gaussianNormBelowCutoff"
      << "="
      << QString::number(gaussianNormBelowCutoff, 'f', 3)
      << ";";

   sl << "gaussianNormAboveCutoff"
      << "="
      << QString::number(gaussianNormAboveCutoff, 'f', 3)
      << ";";

   sl << "gaussianTangCutoff"
      << "="
      << QString::number(gaussianTangCutoff, 'f', 3)
      << ";";

   sl << "mcwBrainFishMaxDistance"
      << "="
      << QString::number(mcwBrainFishMaxDistance, 'f', 3)
      << ";";

   sl << "mcwBrainFishSplatFactor"
      << "="
      << QString::number(mcwBrainFishSplatFactor)
      << ";";
      
   const QString s = sl.join("");
   return s;
}

/**
 * set the algorithm parameters from a string.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::setAlgorithmParametersFromString(const QString& s)
{
   const QStringList sl = s.split(';');
   for (int i = 0; i < sl.count(); i++) {
      const QStringList paramSL = sl.at(i).split('=');
      if (paramSL.count() == 2) {
         const QString name = paramSL.at(0);
         const QString value = paramSL.at(1);
         
         if (name == "algorithm") {
            algorithm = getAlgorithmFromNameString(value);
         }
         else if (name == "averageVoxelNeighbors") {
            averageVoxelNeighbors = value.toFloat();
         }
         else if (name == "maximumVoxelNeighbors") {
            maximumVoxelNeighbors = value.toFloat();
         }
         else if (name == "strongestVoxelNeighbors") {
            strongestVoxelNeighbors = value.toFloat();
         }
         else if (name == "gaussianNeighbors") {
            gaussianNeighbors = value.toFloat();
         }
         else if (name == "gaussianSigmaNorm") {
            gaussianSigmaNorm = value.toFloat();
         }
         else if (name == "gaussianSigmaTang") {
            gaussianSigmaTang = value.toFloat();
         }
         else if (name == "gaussianNormBelowCutoff") {
            gaussianNormBelowCutoff = value.toFloat();
         }
         else if (name == "gaussianNormAboveCutoff") {
            gaussianNormAboveCutoff = value.toFloat();
         }
         else if (name == "gaussianTangCutoff") {
            gaussianTangCutoff = value.toFloat();
         }
         else if (name == "mcwBrainFishMaxDistance") {
            mcwBrainFishMaxDistance = value.toFloat();
         }
         else if (name == "mcwBrainFishSplatFactor") {
            mcwBrainFishSplatFactor = value.toInt();
         }
      }
   }
}
      
/**
 * get algorithm parameters as string for comment usage.
 */
QString 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmParametersForComment() const
{
   QString s;
   
   switch (algorithm) {
      case ALGORITHM_METRIC_AVERAGE_NODES:
         s.append("Algorithm: Metric Average Nodes\n");
         break;
      case ALGORITHM_METRIC_AVERAGE_VOXEL:
         s.append("Algorithm: Metric Average Voxel\n");
         s.append("   neighbors=");
         s.append(QString::number(averageVoxelNeighbors, 'f', 3));
         s.append("mm\n");
         break;
      case ALGORITHM_METRIC_ENCLOSING_VOXEL:
         s.append("Algorithm: Metric Enclosing Voxel\n");
         break;
      case ALGORITHM_METRIC_GAUSSIAN:
         s.append("Algorithm: Metric Gaussian\n");
         s.append("   neighbors=");
         s.append(QString::number(gaussianNeighbors, 'f', 3));
         s.append("mm   sigma-norm=");
         s.append(QString::number(gaussianSigmaNorm, 'f', 3));
         s.append("   sigma-tang=");
         s.append(QString::number(gaussianSigmaTang, 'f', 3));
         s.append("   norm-below-cutoff=");
         s.append(QString::number(gaussianNormBelowCutoff, 'f', 3));
         s.append("   norm-above-cutoff=");
         s.append(QString::number(gaussianNormAboveCutoff, 'f', 3));
         s.append("   tang-cutoff=");
         s.append(QString::number(gaussianTangCutoff, 'f', 3));
         s.append("\n");
         break;
      case ALGORITHM_METRIC_INTERPOLATED_VOXEL:
         s.append("Algorithm: Metric Interpolated Voxel\n");
         break;
      case ALGORITHM_METRIC_MAXIMUM_VOXEL:
         s.append("Algorithm: Metric Maximum Voxel\n");
         s.append("   neighbors=");
         s.append(QString::number(maximumVoxelNeighbors, 'f', 3));
         s.append("mm\n");
         break;
      case ALGORITHM_METRIC_MCW_BRAINFISH:
         s.append("Algorithm: Metric MCW Brain Fish\n");
         s.append("   max-distance=");
         s.append(QString::number(mcwBrainFishMaxDistance, 'f', 3));
         s.append("   splat-factor=");
         s.append(QString::number(mcwBrainFishSplatFactor));
         s.append("\n");
         break;
      case ALGORITHM_METRIC_STRONGEST_VOXEL:
         s.append("Algorithm: Metric Strongest Voxel\n");
         s.append("   neighbors=");
         s.append(QString::number(strongestVoxelNeighbors, 'f', 3));
         s.append("mm\n");
         break;
      case ALGORITHM_PAINT_ENCLOSING_VOXEL:
         s.append("Algorithm: Paint Enclosing Voxel\n");
         break;
   }

   return s;
}
      
/**
 * get algorithm.
 */
BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithm() const
{
   return algorithm;
}

/**
 * set algorithm.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::setAlgorithm(const ALGORITHM alg)
{
   algorithm = alg;
}
      
/**
 * get the algorithm name.
 */
QString 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmName(const ALGORITHM alg)
{
   QString s("Unknown");
   
   switch (alg) {
      case ALGORITHM_METRIC_AVERAGE_NODES:
         s = getAlgorithmMetricAverageNodesName();
         break;
      case ALGORITHM_METRIC_AVERAGE_VOXEL:
         s = getAlgorithmMetricAverageVoxelName();
         break;
      case ALGORITHM_METRIC_ENCLOSING_VOXEL:
         s = getAlgorithmMetricEnclosingVoxelName();
         break;
      case ALGORITHM_METRIC_GAUSSIAN:
         s = getAlgorithmMetricGaussianName();
         break;
      case ALGORITHM_METRIC_INTERPOLATED_VOXEL:
         s = getAlgorithmMetricInterpolatedVoxelName();
         break;
      case ALGORITHM_METRIC_MAXIMUM_VOXEL:
         s = getAlgorithmMetricMaximumVoxelName();
         break;
      case ALGORITHM_METRIC_MCW_BRAINFISH:
         s = getAlgorithmMetricMcwBrainFishName();
         break;
      case ALGORITHM_METRIC_STRONGEST_VOXEL:
         s = getAlgorithmMetricStrongestVoxelName();
         break;
      case ALGORITHM_PAINT_ENCLOSING_VOXEL:
         s = getAlgorithmPaintEnclosingVoxel();
         break;
   }
   
   return s;
}
      
/**
 * get algorithm from string.
 */
BrainModelVolumeToSurfaceMapperAlgorithmParameters::ALGORITHM 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmFromNameString(const QString& s)
{
   ALGORITHM alg = ALGORITHM_METRIC_ENCLOSING_VOXEL;
   
   if (s == getAlgorithmMetricAverageNodesName()) {
      alg = ALGORITHM_METRIC_AVERAGE_NODES;
   }
   else if (s == getAlgorithmMetricAverageVoxelName()) {
      alg = ALGORITHM_METRIC_AVERAGE_VOXEL;
   }
   else if (s == getAlgorithmMetricEnclosingVoxelName()) {
      alg = ALGORITHM_METRIC_ENCLOSING_VOXEL;
   }
   else if (s == getAlgorithmMetricGaussianName()) {
      alg = ALGORITHM_METRIC_GAUSSIAN;
   }
   else if (s == getAlgorithmMetricInterpolatedVoxelName()) {
      alg = ALGORITHM_METRIC_INTERPOLATED_VOXEL;
   }
   else if (s == getAlgorithmMetricMaximumVoxelName()) {
      alg = ALGORITHM_METRIC_MAXIMUM_VOXEL;
   }
   else if (s == getAlgorithmMetricMcwBrainFishName()) {
      alg = ALGORITHM_METRIC_MCW_BRAINFISH;
   }
   else if (s == getAlgorithmMetricStrongestVoxelName()) {
      alg = ALGORITHM_METRIC_STRONGEST_VOXEL;
   }
   else if (s == getAlgorithmPaintEnclosingVoxel()) {
      alg = ALGORITHM_PAINT_ENCLOSING_VOXEL;
   }
   
   return alg;
}
      
/**
 * get the algorithm names and values.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmNamesAndValues(
                                                   std::vector<QString>& namesOut,
                                                   std::vector<ALGORITHM>& valuesOut)
{
   namesOut.clear();
   valuesOut.clear();
   
   namesOut.push_back(getAlgorithmMetricAverageNodesName());
      valuesOut.push_back(ALGORITHM_METRIC_AVERAGE_NODES);
   
   namesOut.push_back(getAlgorithmMetricAverageVoxelName());
      valuesOut.push_back(ALGORITHM_METRIC_AVERAGE_VOXEL);
   
   namesOut.push_back(getAlgorithmMetricEnclosingVoxelName());
      valuesOut.push_back(ALGORITHM_METRIC_ENCLOSING_VOXEL);
   
   namesOut.push_back(getAlgorithmMetricGaussianName());
      valuesOut.push_back(ALGORITHM_METRIC_GAUSSIAN);
   
   namesOut.push_back(getAlgorithmMetricInterpolatedVoxelName());
      valuesOut.push_back(ALGORITHM_METRIC_INTERPOLATED_VOXEL);
   
   namesOut.push_back(getAlgorithmMetricMaximumVoxelName());
      valuesOut.push_back(ALGORITHM_METRIC_MAXIMUM_VOXEL);
   
   namesOut.push_back(getAlgorithmMetricMcwBrainFishName());
      valuesOut.push_back(ALGORITHM_METRIC_MCW_BRAINFISH);
   
   namesOut.push_back(getAlgorithmMetricStrongestVoxelName());
      valuesOut.push_back(ALGORITHM_METRIC_STRONGEST_VOXEL);
   
   namesOut.push_back(getAlgorithmPaintEnclosingVoxel());
      valuesOut.push_back(ALGORITHM_PAINT_ENCLOSING_VOXEL);
}
                                             
/**
 * get the average voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmMetricAverageVoxelParameters(float& neighborsOut) const
{
   neighborsOut = averageVoxelNeighbors;
}

/**
 * set the average voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::setAlgorithmMetricAverageVoxelParameters(const float neighborsIn)
{
   averageVoxelNeighbors = neighborsIn;
}

/**
 * get the maximum voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmMetricMaximumVoxelParameters(float& neighborsOut) const
{
   neighborsOut = maximumVoxelNeighbors;
}

/**
 * set the maximum voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::setAlgorithmMetricMaximumVoxelParameters(const float neighborsIn)
{
  maximumVoxelNeighbors = neighborsIn;
}

/**
 * get the strongest voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmMetricStrongestVoxelParameters(float& neighborsOut) const
{
   neighborsOut = strongestVoxelNeighbors;
}

/**
 * set the maximum voxel parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::setAlgorithmMetricStrongestVoxelParameters(const float neighborsIn)
{
  strongestVoxelNeighbors = neighborsIn;
}

/**
 * set gaussian algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmMetricGaussianParameters(
                                                            float& gaussianNeighborsOut,
                                                            float& gaussianSigmaNormOut,
                                                            float& gaussianSigmaTangOut,
                                                            float& gaussianNormBelowCutoffOut,
                                                            float& gaussianNormAboveCutoffOut,
                                                            float& gaussianTangCutoffOut) const
{
   gaussianNeighborsOut       = gaussianNeighbors;
   gaussianSigmaNormOut       = gaussianSigmaNorm;
   gaussianSigmaTangOut       = gaussianSigmaTang;
   gaussianNormBelowCutoffOut = gaussianNormBelowCutoff;
   gaussianNormAboveCutoffOut = gaussianNormAboveCutoff;
   gaussianTangCutoffOut      = gaussianTangCutoff;
}
                                    
/**
 * set gaussian algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::setAlgorithmMetricGaussianParameters(
                                                         const float gaussianNeighborsIn,
                                                         const float gaussianSigmaNormIn,
                                                         const float gaussianSigmaTangIn,
                                                         const float gaussianNormBelowCutoffIn,
                                                         const float gaussianNormAboveCutoffIn,
                                                         const float gaussianTangCutoffIn)
{
   gaussianNeighbors       = gaussianNeighborsIn;
   gaussianSigmaNorm       = gaussianSigmaNormIn;
   gaussianSigmaTang       = gaussianSigmaTangIn;
   gaussianNormBelowCutoff = gaussianNormBelowCutoffIn;
   gaussianNormAboveCutoff = gaussianNormAboveCutoffIn;
   gaussianTangCutoff      = gaussianTangCutoffIn;
}
 
/**
 * get the mcw brainfish algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::getAlgorithmMetricMcwBrainFishParameters(
                                                    float& mcwBrainFishMaxDistanceOut,
                                                    int& mcwBrainFishSplatFactorOut) const
{
   mcwBrainFishMaxDistanceOut = mcwBrainFishMaxDistance;
   mcwBrainFishSplatFactorOut = mcwBrainFishSplatFactor;
}
                                        
/**
 * set the mcw brainfish algorithm parameters.
 */
void 
BrainModelVolumeToSurfaceMapperAlgorithmParameters::setAlgorithmMetricMcwBrainFishParameters(
                                        const float mcwBrainFishMaxDistanceIn,
                                        const int mcwBrainFishSplatFactorIn)
{
   mcwBrainFishMaxDistance = mcwBrainFishMaxDistanceIn;
   mcwBrainFishSplatFactor = mcwBrainFishSplatFactorIn;
}                                              
