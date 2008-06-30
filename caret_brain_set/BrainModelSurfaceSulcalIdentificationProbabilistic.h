
#ifndef __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_PROBABILISTIC_H__
#define __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_PROBABILISTIC_H__

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

#include "BrainModelAlgorithm.h"

class AreaColorFile;
class BrainModelSurface;
class MetricFile;
class PaintFile;
class SurfaceShapeFile;
class VocabularyFile;

/// class for identifying sulci
class BrainModelSurfaceSulcalIdentificationProbabilistic : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceSulcalIdentificationProbabilistic(BrainSet* bs,
                                            const BrainModelSurface* fiducialSurfaceIn,
                                            const BrainModelSurface* inflatedSurfaceIn,
                                            const BrainModelSurface* veryInflatedSurfaceIn,
                                            const PaintFile* inputPaintFileIn,
                                            const int paintFileGeographyColumnNumberIn,
                                            const SurfaceShapeFile* surfaceShapeFileIn,
                                            const int surfaceShapeFileDepthColumnNumberIn,
                                            const QString& probabilisticDepthVolumeCSVFileNameIn,
                                            const float postCentralSulcusOffsetIn = 25.0,
                                            const float postCentralSulcusStdDevSquareIn = 100.0,
                                            const float postCentralSulcusSplitIn = 5.0);
      
      // destructor
      ~BrainModelSurfaceSulcalIdentificationProbabilistic();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      // get the output paint file (will be delete'd by this algorithm, so copy if needed)
      const PaintFile* getOutputPaintFile() const { return outputPaintFile; }
      
      // get the output area color file
      const AreaColorFile* getOutputAreaColorFile() const { return outputAreaColorFile; }
      
      // get the output metric file containing probabilistic mappings
      const MetricFile* getOutputMetricFile() const { return probabilisticMetricFile; }
       
      // get the output vocabulary file
      const VocabularyFile* getOutputVocabularyFile() const { return outputVocabularyFile; }
      
      /// get the name of the Sulcus ID paint column
      static QString getSulcusIdPaintColumnName() { return "Sulcal Identification"; }
      
   protected:
      /// class for sulcal names and matching volume file
      class SulcalNameAndVolume {
         public:
            /// constructor
            SulcalNameAndVolume(const QString& sulcusNameIn,
                                const float& depthThresholdIn,
                                const QString& volumeNameIn,
                                const int maximumClustersIn) {
               sulcusName = sulcusNameIn;
               depthThreshold = depthThresholdIn;
               volumeName = volumeNameIn;
               maximumClusters = maximumClustersIn;
            }
            
            /// get name of sulcus
            QString getSulcusName() const { return sulcusName; }
            
            /// get the depth threshold
            float getDepthThreshold() const { return depthThreshold; }
            
            /// get name of volume
            QString getVolumeName() const { return volumeName; }
         
            /// get the maximum number of clusters
            int getMaximumClusters() const { return maximumClusters; }
            
         protected:
            /// name of sulcus
            QString sulcusName;
            
            /// depth threshold 
            float depthThreshold;
            
            /// name of volume
            QString volumeName;
            
            /// maximum clusters
            int maximumClusters;
      };
      
      /// add an area color
      void addAreaColor(const QString& colorName);
      
      // load the colors
      void loadColors();
      
      // load vocabulary
      void loadVocabulary();
      
      // add casename to name
      QString addCaseNameToName(const QString& name) const;
      
      // add vocabulary
      void addVocabulary(const QString& name);
      
      // map the probabilistic functional volumes to the metric file
      void mapProbabilisticFunctionalVolumes() throw (BrainModelAlgorithmException);
      
      // create the initial sulcal identification
      void createInitialSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException);
      
      // dilate the sulcal identification
      void dilateSulcalIdentification(const int paintColumn) throw (BrainModelAlgorithmException);
      
      // read the probabilistic volume file list
      void readProbabilisticVolumeFileList() throw (BrainModelAlgorithmException);
      
      // multiply probabilistic volumes by depth
      void multiplyProbabilisticFunctionalDataByDepth() throw (BrainModelAlgorithmException);
      
      // rotate the very inflated surface
      void rotateVeryInflatedSurface() throw (BrainModelAlgorithmException);
      
      // special process for the hippocampal fissure
      void specialProcessingForHippocampalFissure(MetricFile* metricFile,
                                                  const int metricFileColumnNumber);
      
      /// sulcal names and volumes
      std::vector<SulcalNameAndVolume> sulcalNamesAndVolumes;
      
      /// the fiducial surface
      const BrainModelSurface* fiducialSurface;
      
      /// the inflated surface
      const BrainModelSurface* inflatedSurface;
      
      /// the very inflated surface
      const BrainModelSurface* veryInflatedSurface;
      
      /// the rotated very inflated surface
      BrainModelSurface* rotatedVeryInflatedSurface;
      
      /// the input paint file
      const PaintFile* inputPaintFile;
      
      /// the output paint file
      PaintFile* outputPaintFile;
      
      /// the metric file for mapping functional probabilistic volumes
      MetricFile* probabilisticMetricFile;
      
      /// the paint file geography column number
      int paintFileGeographyColumnNumber;

      /// the output area color file
      AreaColorFile* outputAreaColorFile;
      
      /// the default area colors
      AreaColorFile* defaultAreaColorFile;
      
      /// the surface shape file
      const SurfaceShapeFile* surfaceShapeFile;
      
      /// the surface shape file depth column number
      int surfaceShapeFileDepthColumnNumber;
      
      /// output vocabulary file
      VocabularyFile* outputVocabularyFile;
      
      /// default vocabulary file
      VocabularyFile* defaultVocabularyFile;
      
      /// name of file listing sulcal names, depth thresholds, and volume file names
      const QString probabilisticDepthVolumeCSVFileName;

      /// post central sulcus offset from central sulcus
      const float postCentralSulcusOffset;
      
      /// post central sulcus offset from central sulcus std dev squared
      const float postCentralSulcusStdDevSquare;
      
      /// post central sulcus split between two largest post central sulci
      const float postCentralSulcusSplit;
      
      /// flag to add some default colors
      bool addingFirstColorFlag;
};

#endif // __BRAIN_MODEL_SURFACE_SULCAL_IDENTIFICATION_PROBABILISTIC_H__
