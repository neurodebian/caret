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


#ifndef __DEFORMATION_MAP_FILE__CLASS_H__
#define __DEFORMATION_MAP_FILE__CLASS_H__

#include "AbstractFile.h"

/// Deformation data for a node
class DeformMapNodeData {
   public:
      /// tile's nodes
      int tileNodes[3];
      
      /// tile's node's barycentric areas
      float tileBarycentric[3];
      
      // constructor
      DeformMapNodeData();
      
};

/// class for file describing a deformation
class DeformationMapFile  : public AbstractFile{
   public:
      /// type of deformation
      enum DEFORMATION_TYPE {
         DEFORMATION_TYPE_FLAT,
         DEFORMATION_TYPE_SPHERE
      };
      
      /// border resampling type
      enum BORDER_RESAMPLING_TYPE {
         BORDER_RESAMPLING_NONE,
         BORDER_RESAMPLING_FROM_BORDER_FILE,
         BORDER_RESAMPLING_VALUE
      };
      
      /// border file types
      enum BORDER_FILE_TYPE {
         BORDER_FILE_UNKNOWN,
         BORDER_FILE_FLAT,
         BORDER_FILE_FLAT_LOBAR,
         BORDER_FILE_SPHERICAL,
         BORDER_FILE_PROJECTION
      };

      /// metric deform type
      enum METRIC_DEFORM_TYPE {
         METRIC_DEFORM_NEAREST_NODE,
         METRIC_DEFORM_AVERAGE_TILE_NODES
      };
      
      enum {
         MAX_SPHERICAL_CYCLES = 10
      };
      
   private:
     
      static const QString deformMapFileVersion;
      
      static const QString deformedFileNamePrefixTag;
      static const QString deformedColumnNamePrefixTag;
      static const QString sphereResolutionTag;
      static const QString borderResampleTag;
      static const QString sphericalNumberOfCyclesTag;
      static const QString smoothingParamtersTag;
      static const QString morphingParametersTag;
      static const QString flatParametersTag;
      static const QString sphereFiducialSphereRatioTag;
      static const QString smoothDeformedSurfaceTag;
      
      static const QString sourceDirectoryTag;
      static const QString sourceSpecTag;
      static const QString sourceBorderTag;
      static const QString sourceClosedTopoTag;
      static const QString sourceCutTopoTag;
      static const QString sourceFiducialCoordTag;
      static const QString sourceSphericalCoordTag;
      static const QString sourceDeformedSphericalCoordTag;
      static const QString sourceDeformedFlatCoordTag;
      static const QString sourceFlatCoordTag;
      static const QString sourceResampledFlatCoordTag;
      static const QString sourceResampledDeformedFlatCoordTag;
      static const QString sourceResampledCutTopoTag;

      static const QString targetDirectoryTag;
      static const QString targetSpecTag;
      static const QString targetBorderTag;
      static const QString targetClosedTopoTag;
      static const QString targetCutTopoTag;
      static const QString targetFiducialCoordTag;
      static const QString targetSphericalCoordTag;
      static const QString targetFlatCoordTag;
      
      static const QString flatOrSphereSelectionTag;
      static const QString DeformationFlatValue;
      static const QString DeformationSphereValue;      
      static const QString outputSpecFileTag;
      
      static const QString inverseDeformationFlagTag;
      
      static const QString startOfDataTag;
      
      /// read deformation files data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// write deformation files data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// Write a tag/value pair and make it relative to a directory
      void writeFileTagRelative(QTextStream& stream,
                                const QString& directory,
                                const QString& tag,
                                const QString& valueIn);
                                         
      /// deformation data for each node
      std::vector<DeformMapNodeData> deformData;
      
      /// deformation type
      DEFORMATION_TYPE flatOrSphereSelection;
      
      /// name of source spec file
      QString sourceSpecFileName;
      
      /// name of source border file
      QString sourceBorderFileName;
      
      /// type of source border file
      BORDER_FILE_TYPE sourceBorderFileType;

      /// name of source closed topo file
      QString sourceClosedTopoFileName;
      
      /// name of source cut topo file
      QString sourceCutTopoFileName;
      
      /// name of source fiducial coord file
      QString sourceFiducialCoordFileName;
      
      /// name of source spherical coord file
      QString sourceSphericalCoordFileName;
      
      /// name of source deformed spherical coord file
      QString sourceDeformedSphericalCoordFileName;
      
      /// name of source deformed flat coord file
      QString sourceDeformedFlatCoordFileName;
      
      /// name of source flat coord file
      QString sourceFlatCoordFileName;
      
      /// name of source flat resampled coord file
      QString sourceResampledFlatCoordFileName;
      
      /// name of source resampled deformed flat file
      QString sourceResampledDeformedFlatCoordFileName;
      
      /// name of source resampled cut topo file
      QString sourceResampledCutTopoFileName;
      
      /// name of source directory
      QString sourceDirectoryName;
      
      /// name of target spec file
      QString targetSpecFileName;
      
      /// name of target border file
      QString targetBorderFileName;
      
      /// type of target border file
      BORDER_FILE_TYPE targetBorderFileType;

      /// name of target closed topo file
      QString targetClosedTopoFileName;
      
      /// name of target cut topo file
      QString targetCutTopoFileName;
      
      /// name of target fiducial coord file
      QString targetFiducialCoordFileName;
      
      /// name of target spherical coord file
      QString targetSphericalCoordFileName;
      
      /// name of target flat coord file
      QString targetFlatCoordFileName;
      
      /// name of target directory
      QString targetDirectoryName;
      
      /// name of output spec file
      QString outputSpecFileName;
      
      /// deformed file name prefix
      QString deformedFileNamePrefix;
      
      /// deformed column name prefix
      QString deformedColumnNamePrefix;
      
      /// index to sphere file's for different resolution spheres
      int sphereResolution;
      
      /// border resampling type
      BORDER_RESAMPLING_TYPE borderResampleType;
      
      /// border resampling value
      float borderResampleValue;
      
      /// number of spherical cycles
      int   sphericalNumberOfCycles;
      
      /// smoothing strength at each cycle
      float smoothingStrength[MAX_SPHERICAL_CYCLES];
      
      /// number of smoothing cycles
      int   smoothingCycles[MAX_SPHERICAL_CYCLES];
      
      /// number of smoothing iterations at each cycle
      int   smoothingIterations[MAX_SPHERICAL_CYCLES];
      
      /// number of neighbor smoothing iterations at each cycle
      int   smoothingNeighborIterations[MAX_SPHERICAL_CYCLES];
      
      /// number of final smoothing iterations at each cycle
      int   smoothingFinalIterations[MAX_SPHERICAL_CYCLES];
      
      /// number of morphing cycles
      int   morphingCycles[MAX_SPHERICAL_CYCLES];
      
      /// morphing linear force at each cycle
      float morphingLinearForce[MAX_SPHERICAL_CYCLES];
      
      /// morphing angular force at each cycle
      float morphingAngularForce[MAX_SPHERICAL_CYCLES];
      
      /// morphing step size at each cycle
      float morphingStepSize[MAX_SPHERICAL_CYCLES];
      
      /// morphing landmark step size at each cycle
      float morphingLandmarkStepSize[MAX_SPHERICAL_CYCLES];
      
      /// morphing iterations at each cycle
      int   morphingIterations[MAX_SPHERICAL_CYCLES];
      
      /// morphing smoothing iterations at each cycle
      int   morphingSmoothIterations[MAX_SPHERICAL_CYCLES];
      
      /// flat deformation sub sampling tiles
      int   flatSubSamplingTiles;
      
      /// flat deformation beta parameter
      float flatBeta;
      
      /// flat deformation variance multiplier
      float flatVarMult;
      
      /// flat deformation number of iterations
      int   flatNumIters;
      
      /// version of this deformation map file
      int   fileVersion;
      
      /// inverse deformation flag
      bool inverseDeformationFlag;
      
      /// delete intermediate files flag
      bool deleteIntermediateFilesFlag;
      
      /// deform both ways flag (indiv-to-atlas and atlas-to-indiv)
      bool deformBothWaysFlag;
      
      /// metric deformation type
      METRIC_DEFORM_TYPE metricDeformationType;
      
      /// fiducial sphere ratio
      float fiducialSphereRatio;
      
      /// fiducial sphere ratio enabled
      bool fiducialSphereRatioEnabled;
      
      /// smoothe deformed surfaces flag
      bool smoothDeformedSurfacesFlag;
      
   public:
   
      
      /// constructor
      DeformationMapFile();
      
      /// destructor
      ~DeformationMapFile();
      
      /// clear the data
      void clear();

      /// see if the file is isEmpty
      bool empty() const { return deformData.empty(); }

      /// compare a file for unit testing (returns true if "within tolerance")
      virtual bool compareFileForUnitTesting(const AbstractFile* af,
                                             const float tolerance,
                                             QString& messageOut) const;
      /// get the version of the file
      int getFileVersion() const { return fileVersion; }

      /// get deform data for a node
      void getDeformDataForNode(const int nodeNumber, int tileNodesOut[3],
                                float tileBarycentricOut[3]) const;
      
      /// get the deformed file name prefix
      QString getDeformedFileNamePrefix() const { return deformedFileNamePrefix; }
      
      /// get the deformed column name prefix
      QString getDeformedColumnNamePrefix() const { return deformedColumnNamePrefix; }
      
      /// get the delete intermediate files flag
      bool getDeleteIntermediateFiles() const { return deleteIntermediateFilesFlag; }
      
      /// get the deform both ways (I to A and A to I)
      bool getDeformBothWays() const { return deformBothWaysFlag; }
      
      /// get the flat/sphere deformation flag
      DEFORMATION_TYPE getFlatOrSphereSelection() const {
         return flatOrSphereSelection;
      }
      
      /// get the metric deformation type
      METRIC_DEFORM_TYPE getMetricDeformationType() const { return metricDeformationType; }
      
      /// get the number of nodes
      int getNumberOfNodes() const { return deformData.size(); }
      
      /// set deform data for a node
      void setDeformDataForNode(const int nodeNumber, 
                                const int tileNodesIn[3],
                                const float tileBarycentricIn[3]);
      
       
      // set the number of nodes
      void setNumberOfNodes(const int numNodes);
      
      /// make paths in the filenames relative to the specified directory
      void makePathsRelative(const QString& path);
      
      /// make the file "name" relative to the path "path"
      void makeFileRelative(const QString& path,
                            QString& name);
                            
      /// Make the source files relative to a path.
      void makeSourceFilesRelativeToPath(const QString& path);

      /// Make the target files relative to a path.
      void makeTargetFilesRelativeToPath(const QString& path);

      /// get the source directory
      QString getSourceDirectory() const { return sourceDirectoryName; }
      
      /// get the source spec file name
      QString getSourceSpecFileName() const {
         return sourceSpecFileName;
      }

      /// get the source border file name
      void getSourceBorderFileName(QString& name, 
                                   BORDER_FILE_TYPE& fileType) const {
         name = sourceBorderFileName;
         fileType = sourceBorderFileType;
      }  

      /// get the source closed topo file name
      QString getSourceClosedTopoFileName() const {
         return sourceClosedTopoFileName;
      }

      /// get the source cut topo file name
      QString getSourceCutTopoFileName() const {
         return sourceCutTopoFileName;
      }

      /// get the source fiducial coord file name
      QString getSourceFiducialCoordFileName() const {
         return sourceFiducialCoordFileName;
      }

      /// get the source spherical coord file name
      QString getSourceSphericalCoordFileName() const {
         return sourceSphericalCoordFileName;
      }

      /// get the source deformed spherical coord file name
      QString getSourceDeformedSphericalCoordFileName() const {
         return sourceDeformedSphericalCoordFileName;
      }

      /// get the source deformed flat coord file name
      QString getSourceDeformedFlatCoordFileName() const {
         return sourceDeformedFlatCoordFileName;
      }

      /// get the source flat coord file name
      QString getSourceFlatCoordFileName() const {
         return sourceFlatCoordFileName;
      }

      /// get the source resampled flat coord file name
      QString getSourceResampledFlatCoordFileName() const {
         return sourceResampledFlatCoordFileName;
      }

      /// get the source deformed resampled float coord file name
      QString getSourceResampledDeformedFlatCoordFileName() const {
         return sourceResampledDeformedFlatCoordFileName;
      }

      /// get the source resampled cut file name
      QString getSourceResampledCutTopoFileName() const {
         return sourceResampledCutTopoFileName;
      }
      
      /// get the target directory
      QString getTargetDirectory() const { return targetDirectoryName; }
      
      /// get the target spec file name
      QString getTargetSpecFileName() const {
         return targetSpecFileName;
      }

      /// get the target border file name
      void getTargetBorderFileName(QString& name,
                                   BORDER_FILE_TYPE& fileType) const {
         name = targetBorderFileName;
         fileType = targetBorderFileType;
      }  

      /// get the target closed topo file name
      QString getTargetClosedTopoFileName() const {
         return targetClosedTopoFileName;
      }

      /// get the target cut topo file name
      QString getTargetCutTopoFileName() const {
         return targetCutTopoFileName;
      }

      /// get the target fiducial coord file name
      QString getTargetFiducialCoordFileName() const {
         return targetFiducialCoordFileName;
      }

      /// get the target spherical file name
      QString getTargetSphericalCoordFileName() const {
         return targetSphericalCoordFileName;
      }

      /// get the target flat coord file name
      QString getTargetFlatCoordFileName() const {
         return targetFlatCoordFileName;
      }
      
      /// get the output spec file name
      QString getOutputSpecFileName() const {
         return outputSpecFileName;
      }

      /// set the deformed file name prefix
      void setDeformedFileNamePrefix(const QString& s) { deformedFileNamePrefix = s; }

      /// set the deformed column name prefix
      void setDeformedColumnNamePrefix(const QString& s) { deformedColumnNamePrefix = s; }
            
      /// set the delete intermediate files flag
      void setDeleteIntermediateFiles(const bool f) { deleteIntermediateFilesFlag = f; }
      
      /// set the deform both ways flag
      void setDeformBothWays(const bool f) { deformBothWaysFlag = f; }
      
      /// set the metric deformation type
      void setMetricDeformationType(const METRIC_DEFORM_TYPE mdt) { metricDeformationType = mdt; }
      
      /// set the source directory
      void setSourceDirectory(const QString& name) {
         sourceDirectoryName = name;
         setModified();
      }
      
      /// set the source spec file name      
      void setSourceSpecFileName(const QString& name) {
         sourceSpecFileName = name;
         setModified();
      }

      /// set the source border file name      
      void setSourceBorderFileName(const QString& name,
                                   const BORDER_FILE_TYPE fileType) {
         sourceBorderFileName = name;
         sourceBorderFileType = fileType;
         setModified();
      }

      /// set the source closed topo file name      
      void setSourceClosedTopoFileName(const QString& name) {
         sourceClosedTopoFileName = name;
         setModified();
      }

      /// set the source cut topo file name      
      void setSourceCutTopoFileName(const QString& name) {
         sourceCutTopoFileName = name;
         setModified();
      }

      /// set the source fiducial coord file name      
      void setSourceFiducialCoordFileName(const QString& name) {
         sourceFiducialCoordFileName = name;
         setModified();
      }

      /// set the source spherical coord file name      
      void setSourceSphericalCoordFileName(const QString& name) {
         sourceSphericalCoordFileName = name;
         setModified();
      }

      /// set the source deformed spherical coord file name      
      void setSourceDeformedSphericalCoordFileName(const QString& name) {
         sourceDeformedSphericalCoordFileName = name;
         setModified();
      }

      /// set the source deformed flat coord file name      
      void setSourceDeformedFlatCoordFileName(const QString& name) {
         sourceDeformedFlatCoordFileName = name;
         setModified();
      }

      /// set the source flat coord file name      
      void setSourceFlatCoordFileName(const QString& name) {
         sourceFlatCoordFileName = name;
         setModified();
      }

      /// set the source resampled flat coord file name      
      void setSourceResampledFlatCoordFileName(const QString& name) {
         sourceResampledFlatCoordFileName = name;
         setModified();
      }

      /// set the source deformed resampled flat coord file name      
      void setSourceResampledDeformedFlatCoordFileName(const QString& name) {
         sourceResampledDeformedFlatCoordFileName = name;
         setModified();
      }

      /// set the file source resampled cut topo name      
      void setSourceResampledCutTopoFileName(const QString& name) {
         sourceResampledCutTopoFileName = name;
      }
      
      /// set the target directory
      void setTargetDirectory(const QString& name) {
         targetDirectoryName = name;
         setModified();
      }
      
      /// set the target spec file name      
      void setTargetSpecFileName(const QString& name) {
         targetSpecFileName = name;
         setModified();
      }

      /// set the target border file name      
      void setTargetBorderFileName(const QString& name,
                                   const BORDER_FILE_TYPE fileType) {
         targetBorderFileName = name;
         targetBorderFileType = fileType;
         setModified();
      }

      /// set the target closed topo file name      
      void setTargetClosedTopoFileName(const QString& name) {
         targetClosedTopoFileName = name;
         setModified();
      }

      /// set the target cut topo file name      
      void setTargetCutTopoFileName(const QString& name) {
         targetCutTopoFileName = name;
         setModified();
      }

      /// set the target fiducial coord file name      
      void setTargetFiducialCoordFileName(const QString& name) {
         targetFiducialCoordFileName = name;
         setModified();
      }

      /// set the target spherical coord file name      
      void setTargetSphericalCoordFileName(const QString& name) {
         targetSphericalCoordFileName = name;
         setModified();
      }

      /// set the target flat coord  file name      
      void setTargetFlatCoordFileName(const QString& name) {
         targetFlatCoordFileName = name;
         setModified();
      }
      
      /// set the output spec file name      
      void setOutputSpecFileName(const QString& name) {
         outputSpecFileName = name;
         setModified();
      }
      
      /// set the type of deformation
      void setFlatOrSphereSelection(const DEFORMATION_TYPE foss) {
         flatOrSphereSelection = foss;
         setModified();
      }
      
      /// get the sphere resolution index
      int getSphereResolution() const {
         return sphereResolution;
      }
      
      /// get the spherical number of cycles
      int getSphericalNumberOfCycles() const {
         return sphericalNumberOfCycles;
      }
      
      /// get the border resampling parameters
      void getBorderResampling(BORDER_RESAMPLING_TYPE&   resampleTypeOut, 
                               float& resampleValueOut) const {
         resampleTypeOut = borderResampleType;
         resampleValueOut = borderResampleValue;
      }
      
      /// get the smoothing parameters specific cycle
      void getSmoothingParameters(const int cycleNumber,
                                  float& strengthOut, 
                                  int& cyclesOut,
                                  int& iterationsOut,
                                  int& neighborIterationsOut,
                                  int& finalIterationsOut) const {
         strengthOut           = smoothingStrength[cycleNumber];
         cyclesOut             = smoothingCycles[cycleNumber];
         iterationsOut         = smoothingIterations[cycleNumber];
         neighborIterationsOut = smoothingNeighborIterations[cycleNumber];
         finalIterationsOut    = smoothingFinalIterations[cycleNumber];
      }
      
      /// get the morphing parameters for a specific cycle
      void getMorphingParameters(const int cycleNumber,
                                 int& cyclesOut,
                                 float& linearForceOut,
                                 float& angularForceOut,
                                 float& stepSizeOut,
                                 float& landmarkStepSizeOut,
                                 int&   iterationsOut,
                                 int&   smoothIterationsOut) const {
         cyclesOut       = morphingCycles[cycleNumber];
         linearForceOut  = morphingLinearForce[cycleNumber];
         angularForceOut = morphingAngularForce[cycleNumber];
         stepSizeOut     = morphingStepSize[cycleNumber];
         landmarkStepSizeOut = morphingLandmarkStepSize[cycleNumber];
         iterationsOut   = morphingIterations[cycleNumber];
         smoothIterationsOut = morphingSmoothIterations[cycleNumber];
      }
      
      /// get the flat deformation parameters
      void getFlatParameters(int& subSamplingTilesOut,
                             float& betaOut,
                             float& varMultOut,
                             int& numItersOut) const {
         subSamplingTilesOut = flatSubSamplingTiles;
         betaOut = flatBeta;
         varMultOut = flatVarMult;
         numItersOut = flatNumIters;
      }
      
      /// set the sphere resolution
      void setSphereResolution(const int sphereResolutionIn) {
         sphereResolution = sphereResolutionIn;
      }
      
      /// set the spherical number of cycles
      void setSphericalNumberOfCycles(const int sphericalNumberOfCyclesIn) {
         sphericalNumberOfCycles = sphericalNumberOfCyclesIn;
      }
      
      /// set the border resampling parameters
      void setBorderResampling(const BORDER_RESAMPLING_TYPE resampleTypeIn, 
                               const float resampleValueIn) {
         borderResampleType  = resampleTypeIn;
         borderResampleValue = resampleValueIn;
         setModified();
      }
      
      /// set the smoothing parameters for a specific cycle
      void setSmoothingParameters(const int cycleNumber, 
                                  const float strengthIn, 
                                  const int cyclesIn,
                                  const int iterationsIn,
                                  const int neighborIterationsIn,
                                  const int finalIterationsIn) {
         smoothingStrength[cycleNumber]           = strengthIn;
         smoothingCycles[cycleNumber]             = cyclesIn;
         smoothingIterations[cycleNumber]         = iterationsIn;
         smoothingNeighborIterations[cycleNumber] = neighborIterationsIn;
         smoothingFinalIterations[cycleNumber]    = finalIterationsIn;
         setModified();
      }
      
      /// set the morphing parameters for a specific cycle
      void setMorphingParameters(const int cycleNumber,
                                 const int cyclesIn,
                                 const float linearForceIn,
                                 const float angularForceIn,
                                 const float stepSizeIn,
                                 const float landmarkStepSizeIn,
                                 const int   iterationsIn,
                                 const int   smoothIterationsIn) {
         morphingCycles[cycleNumber]           = cyclesIn;
         morphingLinearForce[cycleNumber]      = linearForceIn;
         morphingAngularForce[cycleNumber]     = angularForceIn;
         morphingStepSize[cycleNumber]         = stepSizeIn;
         morphingLandmarkStepSize[cycleNumber] = landmarkStepSizeIn;
         morphingIterations[cycleNumber]       = iterationsIn;
         morphingSmoothIterations[cycleNumber] = smoothIterationsIn;
         setModified();
      }
      
      /// set the flat morphing parameters
      void setFlatParameters(const int subSamplingTilesIn,
                             const float betaIn,
                             const float varMultIn,
                             const int numItersIn) {
         flatSubSamplingTiles = subSamplingTilesIn;
         flatBeta = betaIn;
         flatVarMult = varMultIn;
         flatNumIters = numItersIn;
         setModified();
      }
                        
      /// get the inverse deformation flag
      bool getInverseDeformationFlag() const { return inverseDeformationFlag; }
      
      /// set the inverse deformation flag
      void setInverseDeformationFlag(const bool idf) {
         inverseDeformationFlag = idf;
         setModified();
      }
      
      /// get the sphere fiducial ratios
      void getSphereFiducialRatio(bool& on, float& ratio) const {
         ratio = fiducialSphereRatio;
         on    = fiducialSphereRatioEnabled;
      }
      
      /// set the sphere fiducial ratios
      void setSphereFiducialRatio(const bool on, const float ratio) {
         fiducialSphereRatio = ratio;
         fiducialSphereRatioEnabled = on;
         setModified();
      }
      
      /// get smoot the deformed surfaces flag
      bool getSmoothDeformedSurfacesFlag() const { return smoothDeformedSurfacesFlag; }

      /// set smooth the deformed surfaces flag
      void setSmoothDeformedSurfacesFlag(const bool b) {
         smoothDeformedSurfacesFlag = b;
         setModified();
      }
      
      /// switch the source files with the target files
      void swapSourceAndTargetFiles();
};


#endif // __DEFORMATION_MAP_FILE__CLASS_H__

#ifdef DEFORMATION_MAP_FILE_DEFINE
const QString DeformationMapFile::deformMapFileVersion = "deform-map-file-version";

const QString DeformationMapFile::deformedFileNamePrefixTag = "deformed-file-name-prefix";
const QString DeformationMapFile::deformedColumnNamePrefixTag = "deformed-column-name-prefix";
const QString DeformationMapFile::sphereResolutionTag = "sphere-resolution";
const QString DeformationMapFile::borderResampleTag = "border-resampling";
const QString DeformationMapFile::sphericalNumberOfCyclesTag = "spherical-number-of-cycles";
const QString DeformationMapFile::smoothingParamtersTag = "smoothing-parameters";
const QString DeformationMapFile::morphingParametersTag = "morphing-parameters";
const QString DeformationMapFile::flatParametersTag = "flat-parameters";
const QString DeformationMapFile::sphereFiducialSphereRatioTag = "sphere-fiducial-sphere-ratio";
const QString DeformationMapFile::smoothDeformedSurfaceTag = "smooth-deformed-surface-flag";
      
const QString DeformationMapFile::sourceDirectoryTag = "source-directory";
const QString DeformationMapFile::sourceSpecTag = "source-spec";
const QString DeformationMapFile::sourceBorderTag = "source-landmark-border";
const QString DeformationMapFile::sourceClosedTopoTag = "source-closed-topo";
const QString DeformationMapFile::sourceCutTopoTag = "source-cut-topo";
const QString DeformationMapFile::sourceFiducialCoordTag = "source-fiducial-coord";
const QString DeformationMapFile::sourceSphericalCoordTag = "source-sphere-coord";
const QString DeformationMapFile::sourceDeformedSphericalCoordTag =
                                                   "source-deform-sphere-coord";
const QString DeformationMapFile::sourceDeformedFlatCoordTag =
                                                   "source-deform-flat-coord";
const QString DeformationMapFile::sourceFlatCoordTag = "source-flat-coord";
const QString DeformationMapFile::sourceResampledFlatCoordTag = 
                                       "source-resampled-flat-coord";
const QString DeformationMapFile::sourceResampledDeformedFlatCoordTag = 
                                 "source-resampled-deformed-flat-coord";
const QString DeformationMapFile::sourceResampledCutTopoTag = 
                                 "source-resampled-cut-topo";

const QString DeformationMapFile::targetDirectoryTag = "target-directory";
const QString DeformationMapFile::targetSpecTag = "target-spec";
const QString DeformationMapFile::targetBorderTag = "target-landmark-border";
const QString DeformationMapFile::targetClosedTopoTag = "target-closed-topo";
const QString DeformationMapFile::targetCutTopoTag = "target-cut-topo";
const QString DeformationMapFile::targetFiducialCoordTag = "target-fiducial-coord";
const QString DeformationMapFile::targetSphericalCoordTag = "target-sphere-coord";
const QString DeformationMapFile::targetFlatCoordTag = "target-flat-coord";
      
const QString DeformationMapFile::flatOrSphereSelectionTag = "flat-or-sphere";
const QString DeformationMapFile::DeformationFlatValue = "DEFORM_FLAT";
const QString DeformationMapFile::DeformationSphereValue = "DEFORM_SPHERE";

const QString DeformationMapFile::outputSpecFileTag = "output-spec-file";

const QString DeformationMapFile::startOfDataTag = "DATA-START";

const QString DeformationMapFile::inverseDeformationFlagTag = "inverse-deformation";
#endif  // DEFORMATION_MAP_FILE_DEFINE
