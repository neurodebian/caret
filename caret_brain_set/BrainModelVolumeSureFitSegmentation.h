
#ifndef __BRAIN_MODEL_VOLUME_SEGMENTATION_H__
#define __BRAIN_MODEL_VOLUME_SEGMENTATION_H__

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

#include <QString>

#include "BrainModelAlgorithm.h"
#include "BrainModelSurface.h"
#include "VectorFile.h"
#include "VolumeFile.h"

class PaintFile;

/// class for performing segmentation operations
class BrainModelVolumeSureFitSegmentation : public BrainModelAlgorithm {
   public:
      /// error correction method
      enum ERROR_CORRECTION_METHOD {
         /// No Error Correction
         ERROR_CORRECTION_METHOD_NONE,
         /// Graph-Based Error Correction
         ERROR_CORRECTION_METHOD_GRAPH,
         /// SureFit Error Correction
         ERROR_CORRECTION_METHOD_SUREFIT,
         /// SureFit Error Correction followed by Graph Correction
         ERROR_CORRECTION_METHOD_SUREFIT_AND_GRAPH,
         /// Graph Error Correction followed by SureFit Error
         ERROR_CORRECTION_METHOD_GRAPH_AND_SUREFIT
      };
      
      /// Constructor
      BrainModelVolumeSureFitSegmentation(BrainSet* bs,
                                   const VolumeFile* anatomyVolumeIn,
                                   const VolumeFile* segmentationVolumeIn,
                                   const VolumeFile::FILE_READ_WRITE_TYPE typeOfVolumeFilesToWriteIn,
                                   const int acIJKIn[3],
                                   const int paddingIn[6],
                                   const float wmPeakIn,
                                   const float gmPeakIn,
                                   const float midThreshOverrideIn,
                                   const Structure::STRUCTURE_TYPE structureIn,
                                   const bool disconnectEyeFlagIn,
                                   const bool disconnectHindBrainFlagIn,
                                   const bool disconnectHindBrainHiThreshFlagIn,
                                   const bool cutCorpusCallosumFlagIn,
                                   const bool segmentAnatomyFlagIn,
                                   const bool fillVentriclesFlagIn,
                                   const ERROR_CORRECTION_METHOD errorCorrectionMethodIn,
                                   const bool generateRawAndFidualSurfacesFlagIn,
                                   const bool maximumPolygonsFlagIn,
                                   const bool generateTopologicallyCorrectFiducialSurfaceFlagIn,
                                   const bool generateInflatedSurfaceFlagIn,
                                   const bool generateVeryInflatedSurfaceFlagIn,
                                   const bool generateEllipsoidSurfaceFlagIn,
                                   const bool generateSphericalSurfaceFlagIn,
                                   const bool generateCompressedMedialWallSurfaceFlagIn,
                                   const bool generateHullSurfaceFlagIn,
                                   const bool generateDepthCurvatureGeographyFlagIn,
                                   const bool identifyRegisterFlattenLandmarksFlagIn,
                                   const bool autoSaveFilesFlagIn);
                                   
      /// Constructor used for identifying sulci only.  
      BrainModelVolumeSureFitSegmentation(
                                   BrainSet* bs,
                                   const Structure::STRUCTURE_TYPE structureIn,
                                   const VolumeFile::FILE_READ_WRITE_TYPE typeOfVolumeFilesToWriteIn,
                                   const bool generateHullSurfaceFlagIn);
      
      /// Destructor
      ~BrainModelVolumeSureFitSegmentation();
      
      /// get error correction methods and names
      static void getErrorCorrectionMethodsAndNames(std::vector<QString>& namesOut,
                                  std::vector<ERROR_CORRECTION_METHOD>& methodsOut);
                                  
      /// set the volume mask applied prior to inner and outer boundary determination
      void setVolumeMask(const VolumeFile* volumeMaskIn);
      
      /// set white maximum (values larger than this are excluded prior to inner and outer boundary determination
      void setWhiteMatterMaximum(const float whiteMatterMaximumIn);
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// execute the algorithm for only identifying sulci
      void executeIdentifySulci() throw (BrainModelAlgorithmException);
      
      /// generate the corpus callosum slice (assumes AC at center)
      /// estimate white matter peak if invalid
      static void generateCorpusCallosumSlice(const VolumeFile& anatomyVolumeFileIn,
                                              VolumeFile& corpusCallosumVolumeFileOut,
                                              const Structure& structure,
                                              const float grayMatterPeakIn,
                                              const float whiteMatterPeakIn) throw (BrainModelAlgorithmException);
                                              
   protected:
      /// apply volume mask and white matter maximum
      void applyVolumeMaskAndWhiteMatterMaximum() throw (BrainModelAlgorithmException);
      
      /// disconnect the eye
      void disconnectEye() throw (BrainModelAlgorithmException);
      
      /// disconnect the hind brain
      void disconnectHindBrain() throw (BrainModelAlgorithmException);
      
      /// cut the corpus callossum
      void cutCorpusCallossum() throw (BrainModelAlgorithmException);
      
      /// generate the inner boundary
      void generateInnerBoundary() throw (BrainModelAlgorithmException);
      
      /// generate the outer boundary
      void generateOuterBoundary() throw (BrainModelAlgorithmException);
      
      /// generate the segmentation
      void generateSegmentation() throw (BrainModelAlgorithmException);
      
      /// generate the raw and fiducial surfaces
      void generateRawAndFiducialSurfaces(VolumeFile* vf) throw (BrainModelAlgorithmException);
      
      /// create a fiducial surface that is topologically correct
      void generateTopologicallyCorrectFiducialSurface();
      
      /// generate the inflated and ellipsoid surfaces
      void generateInflatedAndEllipsoidSurfaces() throw (BrainModelAlgorithmException);
      
      /// fill ventricles
      void fillVentricles() throw (BrainModelAlgorithmException);
      
      /// do graph-based automatic error correction
      VolumeFile* graphBasedErrorCorrection(VolumeFile* vf);
      
      /// do SureFit automatic error correction
      VolumeFile* sureFitAutomaticErrorCorrection(VolumeFile* vf);
      
      /// generate depth, curvature, and geography
      void generateDepthCurvatureGeography(const VolumeFile* vf);
      
      /// generate landmarks borders for flattening and registration
      void generateRegistrationFlatteningLandmarkBorders() throw (BrainModelAlgorithmException);
      
      /// generate default scenes
      void generateDefaultScenes() throw (BrainModelAlgorithmException);
      
      /// get parameters from the parameters file
      void getParameters() throw (BrainModelAlgorithmException);
      
      /// write the volume for debugging
      void writeDebugVolume(VolumeFile* vf, const QString& name) throw (BrainModelAlgorithmException);
      
      /// write the volume for debugging
      void writeDebugVolume(VolumeFile& vf, const QString& name) throw (BrainModelAlgorithmException);
      
      /// write the vector file for debugging
      void writeDebugVector(VectorFile& vf, const QString& name) throw (BrainModelAlgorithmException);
      
      /// Free a volume file.
      void freeVolumeInMemory(VolumeFile* &vf);

      /// Free a vector file.
      void freeVectorInMemory(VectorFile* &vf);

      /// free all volumes and vector files in memory.
      void freeAllFilesInMemory();

      /// assign paint for padded CUT.FACE nodes
      void assignPaddedCutFaceNodePainting(const CoordinateFile* cf,
                                           const VolumeFile* segmentVol,
                                           PaintFile* pf,
                                           const int columnToAssign);
                                           
      /// the fiducial surface
      BrainModelSurface* fiducialSurface;
      
      /// the input volume
      VolumeFile* anatomyVolume;
      
      /// resulting segmentation volume
      VolumeFile* segmentationVolume;
      
      /// resulting segmentation with ventricles filled
      VolumeFile* segmentationVentriclesFilledVolume;
      
      /// white matter threshold no eye volume
      VolumeFile* whiteMatterThreshNoEyeVolume;
      
      /// white matter threshold no eye flood volume
      VolumeFile* whiteMatterThreshNoEyeFloodVolume;
      
      /// cerebral white matter no brain stem filled
      VolumeFile* cerebralWmNoBstemFill;
      
      /// the inner mask volume
      VolumeFile* innerMask1Volume;
      
      /// intensity gradient volume
      VolumeFile* gradIntensityVolume;
      
      /// eye fat sculpt volume
      VolumeFile* eyeFatSculptVolume;
      
      /// gray matter level volume
      VolumeFile* gmILevelVolume;
      
      /// outer mask
      VolumeFile* outerMaskVolume;
      
      /// hindbrain flood volume
      VolumeFile* hindbrainFloodVolume;
      
      /// volume
      VolumeFile* wmThreshFloodVolume;
      
      /// volume
      VolumeFile* inTotalVolume;
      
      /// wm volume
      VolumeFile* inTotalThinWMVolume;
      
      /// out total volume
      VolumeFile* outTotalVolume;
      
      /// wm near ventricle volume
      VolumeFile* thinWMOrNearVentricleHCMask;
      
      /// gradient blur
      VolumeFile* ventGradLevelBlurVolume;
      
      /// inner boundary blur
      VolumeFile* inTotalBlur1Volume;
      
      /// outer boundary blur
      VolumeFile* outTotalBlur1Volume;
      
      /// cerebral wm erode volume
      VolumeFile* cerebralWMErodeVolume;
      
      /// pia vector
      VectorFile* gradPiaLevelVec;
      
      /// the gradient thin white matter vector
      VectorFile* gradThinWMlevelVecFile;
      
      /// the intensity gradient file
      VectorFile* gradIntensityVecFile;
      
      /// total thin WM gradient file
      VectorFile* gradInTotalThinWMVecFile;
      
      /// gray/white gradient
      VectorFile* gradGWlevelVecFile;
      
      /// gradient pial vector
      VectorFile* outGradPialLevelGMGradOutITMagVecFile;
      
      /// optional mask used for removal of non-cortical material
      VolumeFile* volumeMask;
      
      /// optional value for excluding voxels larger than this value
      float whiteMatterMaximum;
      
      /// disconnect eye flag
      bool disconnectEyeFlag;
      
      /// disconnect the hind brain flag
      bool disconnectHindBrainFlag;
      
      /// disconnect hind brain using high thresholding flag
      bool disconnectHindBrainHiThreshFlag;
      
      /// cut the corpus callossum flag
      bool cutCorpusCallosumFlag;
      
      /// generate the inner boundary flag
      bool generateInnerBoundaryFlag;
      
      /// generate the outer boundary flag
      bool generateOuterBoundaryFlag;
      
      /// generate the segmentation flag
      bool generateSegmentationFlag;
      
      /// generate the surface flag
      bool generateRawAndFidualSurfacesFlag;
      
      /// generate a topologically correct fiducial surface
      bool generateTopologicallyCorrectFiducialSurfaceFlag;
      
      /// generate inflated surface
      bool generateInflatedSurfaceFlag;
      
      /// generate very inflated surface
      bool generateVeryInflatedSurfaceFlag;
      
      /// generate ellipsoid surfaces
      bool generateEllipsoidSurfaceFlag;
      
      /// generate spherical surface
      bool generateSphericalSurfaceFlag;
      
      /// generate compressed medial wall surface
      bool generateCompressedMedialWallSurfaceFlag;
      
      /// generate hull surface
      bool generateHullSurfaceFlag;
      
      /// fill ventricles flag
      bool fillVentriclesFlag;
      
      /// error correction method
      ERROR_CORRECTION_METHOD errorCorrectionMethod;
      
      /// generate depth, curvature, geography flag
      bool generateDepthCurvatureGeographyFlag;
      
      /// generate landmarks for registration and flattening
      bool identifyRegisterFlattenLandmarksFlag;
      
      /// auto-save files flag
      bool autoSaveFilesFlag;
      
      /// generate maximum polygons
      bool maximumPolygonsFlag;
      
      /// x dimensions of volume being segmented
      int xDim;
      
      /// y dimensions of volume being segmented
      int yDim;
      
      /// z dimensions of volume being segmented
      int zDim;
      
      /// Anterior Commissure voxel indices
      int acIJK[3];
      
      /// x/y/z minimums
      //int xyzMin[3];
      
      /// white matter peak
      float wmPeak;
      
      /// white matter threshold
      float wmThresh;
      
      ///
      float csfThresh;
      
      /// cgm peak
      float cgmPeak;
      
      /// 
      float cgmLow;
      
      /// 
      float cgmHigh;
      
      /// 
      float cgmSignum;
      
      /// 
      float inITPeak;
      
      /// 
      float inITLow;
      
      /// 
      float inITHigh;
      
      /// 
      float inITSignum;
      
      ///
      float outITPeak;
      
      /// 
      float outITLow;
      
      /// 
      float outITHigh;
      
      ///
      float outITSignum;
      
      /// padding voxel indices
      //int paddingIJK[6];
      
      /// structure
      Structure::STRUCTURE_TYPE structure;
      
      /// extract mask flag
      bool extractMaskFlag;
      
      /// 0=left, 1=right
      int Hem;
      
      /// ???
      int Hem1;
      
      /// ???
      int Hem2;
      
      /// ???
      int HemDbl;
      
      /// ??? 
      int Hem3;
      
      /// ??? 
      int xAClow;
      
      /// ??? 
      int xAChigh;
      
      /// ???
      int xAC_1;
      
      /// ???
      int xAC_1_low;
      
      /// ???
      int xAC_1_high;
      
      /// ???
      int xAC_10;
      
      /// ??? 
      int xAC_15;
      
      /// ???
      int xAC_20;
      
      /// ???
      int xAC_20_low;
      
      /// ???
      int xAC_20_high;
      
      /// ??? 
      int xAC_40;
      
      /// ??? 
      int xAC_15_40_low;
      
      /// ??? 
      int xAC_15_40_high;
      
      /// ??? 
      int xAC_50;
      
      /// ??? 
      int xMedLimit_50_low;
      
      /// ??? 
      int xMedLimit_50_high;
      
      /// ???
      int xMedLimit_20_low;
      
      /// ???
      int xMedLimit_20_high;
      
      /// ???
      int xMedLimit_low;
      
      /// ???
      int xMedLimit_high;
      
      /// Xmin from params file
      //int xMin;
      
      /// Ymin from params file
      //int yMin;
      
      /// Zmin from params file
      //int zMin;      
      
      /// segmentation debug files sub directory
      QString segmentationDebugFilesSubDirectory;
      
      /// partial hemisphere padding
      int partialHemispherePadding[6];
      
      /// type of volume files to write
      VolumeFile::FILE_READ_WRITE_TYPE typeOfVolumeFilesToWrite;
};

#endif // __BRAIN_MODEL_VOLUME_SEGMENTATION_H__

