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



//
// This class is used to map fMRI data from one or more volumes to 
// one or more surfaces.
//
#ifndef __VE_FMRI_DATA_MAPPER__
#define __VE_FMRI_DATA_MAPPER__

#include <string>
#include <vector>

#include "vtkFloatArray.h"
#include "vtkStructuredPoints.h"
#include "vtkPolyData.h"

#include "AtlasDirectoryInfo.h"
#include "FMRIVolumeInfo.h"
#include "FMRISurfaceInfo.h"
#include "MetricFile.h"
#include "StringUtilities.h"
#include "TopologyHelper.h"

/// Class that maps a Volume (typically fMRI) to a Surface
class FMRIDataMapper {
   public:
      /// algorithm type
      enum ALGORITHM_TYPE {
         ALGORITHM_AVERAGE_NODES = 0,
         ALGORITHM_AVERAGE_VOXEL,
         ALGORITHM_GAUSSIAN,
         ALGORITHM_MAX_VOXEL,
         ALGORITHM_MCW_BRAINFISH
      };
     
      /// type of volume
      enum VOLUME_TYPE {
         VOLUME_TYPE_AFNI = 0,
         VOLUME_TYPE_ANALYZE,
         VOLUME_TYPE_SPM_LPI,
         VOLUME_TYPE_SPM_RPI,
         VOLUME_TYPE_WUNIL,
         VOLUME_TYPE_RAW_8_BIT_SIGNED,
         VOLUME_TYPE_RAW_8_BIT_UNSIGNED,
         VOLUME_TYPE_RAW_16_BIT_SIGNED,
         VOLUME_TYPE_RAW_16_BIT_UNSIGNED,
         VOLUME_TYPE_RAW_32_BIT_SIGNED,
         VOLUME_TYPE_RAW_32_BIT_UNSIGNED,
         VOLUME_TYPE_RAW_64_BIT_SIGNED,
         VOLUME_TYPE_RAW_64_BIT_UNSIGNED,
         VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT,
         VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT
      };
      
      /// type of surface file
      enum SURFACE_FILE_TYPE {
         SURFACE_FILE_TYPE_VTK_POLYDATA = 0
      };
      
      /// destination mapping surface
      enum MAPPING_SURFACE_TYPE {
         MAPPING_SURFACE_ATLAS = 0,
         MAPPING_SURFACE_INDIVIDUAL
      };

      /// X Orientation
      enum X_NEGATIVE_ORIENTATION_TYPE {
         X_NEGATIVE_ORIENTATION_LEFT = 0,
         X_NEGATIVE_ORIENTATION_RIGHT
      };
      
      /// Y Orientation
      enum Y_NEGATIVE_ORIENTATION_TYPE {
         Y_NEGATIVE_ORIENTATION_POSTERIOR = 0,
         Y_NEGATIVE_ORIENTATION_ANTERIOR
      };
      
      /// Z Orientation
      enum Z_NEGATIVE_ORIENTATION_TYPE {
         Z_NEGATIVE_ORIENTATION_INFERIOR = 0,
         Z_NEGATIVE_ORIENTATION_SUPERIOR
      };
      
   private: 
      /// algorithm method
      ALGORITHM_TYPE mappingAlgorithm;
      
      /// volume type
      VOLUME_TYPE volumeType;
      
      /// surface type
      SURFACE_FILE_TYPE surfaceFileType;
      
      /// destination surface type
      MAPPING_SURFACE_TYPE mappingSurfaceType;
      
      /// volume X Orientation
      X_NEGATIVE_ORIENTATION_TYPE volumeOrientationX;
      
      /// volume Y Orientation
      Y_NEGATIVE_ORIENTATION_TYPE volumeOrientationY;
      
      /// volume Z Orientation
      Z_NEGATIVE_ORIENTATION_TYPE volumeOrientationZ;
      
      /// individual surface X orientation
      X_NEGATIVE_ORIENTATION_TYPE indivSurfaceOrientationX;
      
      /// individual surface Y orientation
      Y_NEGATIVE_ORIENTATION_TYPE indivSurfaceOrientationY;
      
      /// individual surface Z orientation
      Z_NEGATIVE_ORIENTATION_TYPE indivSurfaceOrientationZ;
      
      /// surfaces being mapped
      std::vector<FMRISurfaceInfo> surfaces;
      
      /// atlas directories available
      std::vector<AtlasDirectoryInfo> atlasDirectories;
      
      /// selected atlas directory
      int selectedAtlasDirectory;
      
      /// name of last selected atlas
      std::string lastSelectedAtlas;
      
      /// volumes being used
      std::vector<FMRIVolumeInfo> volumes;
      
      /// X Dimension of raw volume
      int rawVolumeDimensionX;
      
      /// Y Dimension of raw volume
      int rawVolumeDimensionY;
      
      /// Z Dimension of raw volume
      int rawVolumeDimensionZ;
      
      /// neighbor layers used in some algorithms
      int neighborLayers;
      
      /// Gaussian algorithm paramter
      float gaussianSigmaNorm;
      
      /// Gaussian algorithm paramter
      float gaussianSigmaTang;
      
      /// Gaussian algorithm paramter
      float gaussianNormBelowCutoff;
      
      /// Gaussian algorithm paramter
      float gaussianNormAboveCutoff;
      
      /// Gaussian algorithm paramter
      float gaussianTangCutoff;
      
      /// Brainfish algorithm parameter
      float brainfishMaxDistance;
      
      /// Brainfish algorithm parameter
      int   brainfishSplatFactor;
      
      /// individual cropping enabled flag
      bool indivCroppingEnabled;
      
      /// individual cropping X offset
      int indivCroppingOffsetX;
      
      /// individual cropping Y offset
      int indivCroppingOffsetY;
      
      /// individual cropping Z offset
      int indivCroppingOffsetZ;
      
      /// individual padding enabled
      bool indivPaddingEnabled;
      
      /// Individual padding on positive X
      int indivPaddingPosX;
      
      /// Individual padding on positive Y
      int indivPaddingPosY;
      
      /// Individual padding on positive Z
      int indivPaddingPosZ;
      
      /// Individual passing on Negative X
      int indivPaddingNegX;
      
      /// Individual passing on Negative Y
      int indivPaddingNegY;
      
      /// Individual passing on Negative Z
      int indivPaddingNegZ;
      
      /// volume byte swapping flag
      bool volumeByteSwapping;
      
      /// keeps track of nodes in padded regions
      bool* paddedNodeFlag;
      
      /// Error message callback
      void (*errorMessageCallback)(const std::string&);
      
#ifdef OSX_COMPILER_BUG
      static std::string preferencesValueYes;
      static std::string preferencesValueNo;
      static std::string preferencesRawDimensionTag;
      static std::string preferencesByteSwapTag;
      
      static std::string preferencesAlgorithmTag;
      static std::string preferencesValueAlgorithmAverageNodes;
      static std::string preferencesValueAlgorithmAverageVoxel;
      static std::string preferencesValueAlgorithmGaussian;
      static std::string preferencesValueAlgorithmMaximumVoxel;
      static std::string preferencesValueAlgorithmMCWBrainfish;
      
      static std::string preferencesAlgorithmNeighborsTag;
      static std::string preferencesAlgorithmSigmaNormTag;
      static std::string preferencesAlgorithmSigmaTangTag;
      static std::string preferencesAlgorithmNormBelowTag;
      static std::string preferencesAlgorithmNormAboveTag;
      static std::string preferencesAlgorithmTangTag;
      static std::string preferencesAlgorithmMaxDistanceTag;
      static std::string preferencesAlgorithmSplatFactorTag;
      
      static std::string preferencesAtlasDirectoryTag;
      static std::string preferencesAtlasLastSelected;
      
      static std::string preferencesVolumeTypeTag;
      static std::string preferencesVolumeValueTypeAFNI;
      static std::string preferencesVolumeValueTypeAnalyze;
      static std::string preferencesVolumeValueTypeSPM_LPI;
      static std::string preferencesVolumeValueTypeSPM_RPI;
      static std::string preferencesVolumeValueTypeWUNIL;
      static std::string preferencesVolumeValueTypeSigned8;
      static std::string preferencesVolumeValueTypeUnsigned8;
      static std::string preferencesVolumeValueTypeSigned16;
      static std::string preferencesVolumeValueTypeUnsigned16;
      static std::string preferencesVolumeValueTypeSigned32;
      static std::string preferencesVolumeValueTypeUnsigned32;
      static std::string preferencesVolumeValueTypeSigned64;
      static std::string preferencesVolumeValueTypeUnsigned64;
      static std::string preferencesVolumeValueTypeFloat32;
      static std::string preferencesVolumeValueTypeFloat64;
      
      static std::string preferencesOrientationValueLeft;
      static std::string preferencesOrientationValueRight;
      static std::string preferencesOrientationValueAnterior;
      static std::string preferencesOrientationValuePosterior;
      static std::string preferencesOrientationValueInferior;
      static std::string preferencesOrientationValueSuperior;
      
      static std::string preferencesVolumeOrientationTag;
      
      static std::string preferencesIndivSurfaceOrientationTag;
      
      static std::string preferencesSurfaceTypeTag;
      static std::string preferencesSurfaceValueTypeVTK;
      
      static std::string preferencesIndivPaddingTag;
      static std::string preferencesIndivCroppingOffsetTag;
#else
      static const std::string preferencesValueYes;
      static const std::string preferencesValueNo;
      static const std::string preferencesRawDimensionTag;
      static const std::string preferencesByteSwapTag;
      
      static const std::string preferencesAlgorithmTag;
      static const std::string preferencesValueAlgorithmAverageNodes;
      static const std::string preferencesValueAlgorithmAverageVoxel;
      static const std::string preferencesValueAlgorithmGaussian;
      static const std::string preferencesValueAlgorithmMaximumVoxel;
      static const std::string preferencesValueAlgorithmMCWBrainfish;
      
      static const std::string preferencesAlgorithmNeighborsTag;
      static const std::string preferencesAlgorithmSigmaNormTag;
      static const std::string preferencesAlgorithmSigmaTangTag;
      static const std::string preferencesAlgorithmNormBelowTag;
      static const std::string preferencesAlgorithmNormAboveTag;
      static const std::string preferencesAlgorithmTangTag;
      static const std::string preferencesAlgorithmMaxDistanceTag;
      static const std::string preferencesAlgorithmSplatFactorTag;
      
      static const std::string preferencesAtlasDirectoryTag;
      static const std::string preferencesAtlasLastSelected;
      
      static const std::string preferencesVolumeTypeTag;
      static const std::string preferencesVolumeValueTypeAFNI;
      static const std::string preferencesVolumeValueTypeAnalyze;
      static const std::string preferencesVolumeValueTypeSPM_LPI;
      static const std::string preferencesVolumeValueTypeSPM_RPI;
      static const std::string preferencesVolumeValueTypeWUNIL;
      static const std::string preferencesVolumeValueTypeSigned8;
      static const std::string preferencesVolumeValueTypeUnsigned8;
      static const std::string preferencesVolumeValueTypeSigned16;
      static const std::string preferencesVolumeValueTypeUnsigned16;
      static const std::string preferencesVolumeValueTypeSigned32;
      static const std::string preferencesVolumeValueTypeUnsigned32;
      static const std::string preferencesVolumeValueTypeSigned64;
      static const std::string preferencesVolumeValueTypeUnsigned64;
      static const std::string preferencesVolumeValueTypeFloat32;
      static const std::string preferencesVolumeValueTypeFloat64;
      
      static const std::string preferencesOrientationValueLeft;
      static const std::string preferencesOrientationValueRight;
      static const std::string preferencesOrientationValueAnterior;
      static const std::string preferencesOrientationValuePosterior;
      static const std::string preferencesOrientationValueInferior;
      static const std::string preferencesOrientationValueSuperior;
      
      static const std::string preferencesVolumeOrientationTag;
      
      static const std::string preferencesIndivSurfaceOrientationTag;
      
      static const std::string preferencesSurfaceTypeTag;
      static const std::string preferencesSurfaceValueTypeVTK;
      
      static const std::string preferencesIndivPaddingTag;
      static const std::string preferencesIndivCroppingOffsetTag;
#endif

      /// default names for metric files
      std::vector<std::string> defaultMetricFileNames;

      /// default names for spec files
      std::vector<std::string> defaultSpecFileNames;
      
      /// gaussian algorithm method
      void gaussian(const float normal[3],
                    const float voxelValue,
                    const float distanceVector[3],
                    float& metricSum,
                    float& weightSum);
                    
      /// Get the neighbors extent for a voxel
      void getNeighborsSubVolume(vtkStructuredPoints* volume, 
                                 const int ijk[3],
                                 int& voxelMinX, int& voxelMaxX,
                                 int& voxelMinY, int& voxelMaxY,
                                 int& voxelMinZ, int& voxelMaxZ);

      /// Get the name of the preferences file
      std::string getUserPreferenceFileName() const;
      
      /// Map a volume to a surface
      void  mapVolumeToSurface(vtkStructuredPoints* volume,
                                vtkFloatArray* scalars,
                                vtkPolyData* surface,
                                TopologyHelper& pointNeighbors,
                                MetricFile& metricFile,
                                const int columnNumber);
      
      /// Map a volume to a surface using average nodes
      void  mapVolumeToSurfaceAverageNodes(vtkStructuredPoints* volume,
                                vtkFloatArray* scalars,
                                vtkPolyData* surface,
                                TopologyHelper& pointNeighbors,
                                MetricFile& metricFile,
                                const int columnNumber);
      
      /// Map a volume to a surface using brainfish
      void mapVolumeToSurfaceBrainfish(vtkStructuredPoints* volume,
                                       vtkFloatArray* scalars,
                                       vtkPolyData* surface,
                                       TopologyHelper& pointNeighbors,
                                       MetricFile& metricFile,
                                       const int columnNumber);
      
      /// Map a volume to a surface for average voxel and others
      void  mapVolumeToSurfaceNeighborLayers(vtkStructuredPoints* volume,
                                       vtkFloatArray* scalars,
                                       vtkPolyData* surface,
                                       TopologyHelper& pointNeighbors,
                                       MetricFile& metricFile,
                                       const int columnNumber);
                                       
      /// mask the nodes that are in padded regions
      void maskPaddedNodes(vtkPolyData* polyData);
      
      /// Flip volumes as needed
      void performFlipping(vtkStructuredPoints* s,
                           const X_NEGATIVE_ORIENTATION_TYPE volumeOrientX,
                           const Y_NEGATIVE_ORIENTATION_TYPE volumeOrientY,
                           const Z_NEGATIVE_ORIENTATION_TYPE volumeOrientZ,
                           const X_NEGATIVE_ORIENTATION_TYPE surfaceOrientX,
                           const Y_NEGATIVE_ORIENTATION_TYPE surfaceOrientY,
                           const Z_NEGATIVE_ORIENTATION_TYPE surfaceOrientZ);
      
      /// Process an error message
      void processErrorMessage(const std::string& s) const;

      /// Show progress information
      bool showProgressInformation(const int surfaceNum, const int volumeNum);

   public:
      /// constructor
      FMRIDataMapper();
      
      /// destructor
      ~FMRIDataMapper();
      
      /// error message callback
      void setErrorMessageCallback(void (*callback)(const std::string&));
      
      /// add an atlas directory file (return true if error)
      bool addAtlasDirectory(const std::string& atlasDirName, 
                             const bool saveInPreferences,
                             std::string& errorMessage);
      
      /// number of atlas directories
      int getNumberOfAtlasDirectories() const;
      
      /// get the name of an atlas directory 
      AtlasDirectoryInfo* getAtlasDirectoryInfo(const int num);
      
      /// get the selected atlas directory index
      int getSelectedAtlasDirectory() const;
      
      /// set the selected atlas directory index
      void setSelectedAtlasDirectory(const int num);
      
      /// apply transforms to surface so that it matches volume
      vtkPolyData* applyTransformToIndividual(vtkPolyData* polyData);
      
      /// add a volume for mapping (returns true if an error)
      bool addVolume(const std::string& volumeName, 
                     std::string& message);
      
      /// clear all volumes
      void clearVolumes();
      
      /// delete a volume
      void deleteVolume(const int index);
      
      /// get number of volumes
      int getNumberOfVolumes() const;
      
      /// get a volume
      FMRIVolumeInfo* getVolume(const int index);
      
      /// add a surface for mapping
      void addSurface(const std::string& subdir,
                      const std::string& surfaceName,
                      const std::string& descriptiveName,
                      const std::string& anatomyName = "",
                      const std::string& specFileName = "");
      
      /// clear all surfaces
      void clearSurfaces();
      
      /// delete a surface
      void deleteSurface(const int index);
      
      /// get a surface
      FMRISurfaceInfo* getSurface(const int index);
      
      /// get the number of surfaces
      int getNumberOfSurfaces() const;
      
      /// map volume data to the surfaces
      bool execute();
      
      /// load the parameters from a params file
      void loadParamsFile(const std::string& fileName);
      
      /// get the name of the last selected atlas
      std::string getLastSelectedAtlas() const { return lastSelectedAtlas; }
      
      /// set the last selected atlas
      void setLastSelectedAtlas(const std::string& name) { lastSelectedAtlas.assign(name); }
      
      /// get orientation of volume or indiv surface
      void getVolumeOrientation(X_NEGATIVE_ORIENTATION_TYPE& x,
                                Y_NEGATIVE_ORIENTATION_TYPE& y,
                                Z_NEGATIVE_ORIENTATION_TYPE& z);
                                
      /// get volume orientation labels
      void getVolumeOrientationLabels(std::string& x,
                                      std::string& y,
                                      std::string& z);
      
      /// get individual surface orientation
      void getIndivSurfaceOrientation(X_NEGATIVE_ORIENTATION_TYPE& x,
                                      Y_NEGATIVE_ORIENTATION_TYPE& y,
                                      Z_NEGATIVE_ORIENTATION_TYPE& z);
                                      
      /// get individual surface orientation labels
      void getIndivSurfaceOrientationLabels(std::string& x,
                                      std::string& y,
                                      std::string& z);
      
      /// set orientation of volume or indiv surface
      void setVolumeOrientation(const X_NEGATIVE_ORIENTATION_TYPE x,
                                const Y_NEGATIVE_ORIENTATION_TYPE y,
                                const Z_NEGATIVE_ORIENTATION_TYPE z);
                                
      /// set individual surface orientation
      void setIndivSurfaceOrientation(const X_NEGATIVE_ORIENTATION_TYPE x,
                                      const Y_NEGATIVE_ORIENTATION_TYPE y,
                                      const Z_NEGATIVE_ORIENTATION_TYPE z);
      
      /// get the mapping algorithm
      FMRIDataMapper::ALGORITHM_TYPE getMappingAlgorithm() const;
      
      /// set the mapping method
      void setMappingAlgorithm(FMRIDataMapper::ALGORITHM_TYPE mmt);
      
      /// get the mapping surface type (indiv or atlas)
      MAPPING_SURFACE_TYPE getMappingSurfaceType() const 
                                          { return mappingSurfaceType; }
                                                
      /// set the mapping surface type (indiv or atlas)
      void setMappingSurfaceType(const MAPPING_SURFACE_TYPE mst)
                                          { mappingSurfaceType = mst; }
            
      /// get the raw volume dimensions
      void getRawVolumeDimensions(int& x, int& y, int& z) const;
      
      /// set the raw volume dimensions
      void setRawVolumeDimensions(const int x, const int y, const int z);
      
      /// get the surface types and the names of the surface types
      void getSurfaceFileTypesAndNames(
                       std::vector<FMRIDataMapper::SURFACE_FILE_TYPE>& surfaceTypes,
                       std::vector<std::string>& surfaceTypeNames) const;
                             
      /// get the selected surface type
      FMRIDataMapper::SURFACE_FILE_TYPE getSurfaceFileType() const;
      
      /// set the selected surface type
      void setSurfaceFileType(const FMRIDataMapper::SURFACE_FILE_TYPE st);
      
      /// get the volume types and the names of the volume types
      void getVolumeTypesAndNames(
                             std::vector<FMRIDataMapper::VOLUME_TYPE>& volumeTypes,
                             std::vector<std::string>& volumeTypeNames) const;
      
      /// get the volume byte swapping status
      bool getVolumeByteSwapping() const;
      
      
      /// set the volume byte swapping status
      void setVolumeByteSwapping(const bool byteSwap);
      
      /// get indiv cropping enabled
      bool getIndivCroppingEnabled() const { return indivCroppingEnabled; }
      
      /// set indiv cropping enabled
      void setIndivCroppingEnabled(const bool enable) { indivCroppingEnabled = enable; }
      
      /// get the individual cropping offset
      void getIndivCroppingOffset(bool& croppingEnabled,
                                  int& x, int& y, int& z) const;
      
      // set the individual cropping offset
      void setIndivCroppingOffset(const bool croppingEnabled,
                                  const int x, const int y, const int z);
      
      /// get the selected volume type
      FMRIDataMapper::VOLUME_TYPE getVolumeType() const;
      
      /// find out if selected volume type is a "raw type"
      bool getVolumeTypeIsRaw() const;
      
      /// set the selected volume type
      void setVolumeType(const FMRIDataMapper::VOLUME_TYPE vt);
      
      /// get indiv padding enabled
      bool getIndivPaddingEnabled() const { return indivPaddingEnabled; }
      
      /// set indiv padding enabled
      void setIndivPaddingEnabled(const bool enable) { indivPaddingEnabled = enable; }
      
      /// get the individual padding offset
      void getIndivPadding(bool& paddingEnabled, 
                           int& negX, int& negY, int& negZ,
                           int& posX, int& posY, int& posZ) const;
      
      /// set the individual padding offset
      void setIndivPadding(const bool paddingEnabled,
                           const int negX, const int negY, const int negZ,
                           const int posX, const int posY, const int posZ);
      
      /// get the gaussian mapping method parameters
      void getGaussianParameters(float& sigmaNorm,
                                 float& sigmaTang,
                                 float& normBelowCutoff,
                                 float& normAboveCutoff,
                                 float& tangCutoff) const;
                                 
      /// set the gaussian mapping method parameters
      void setGaussianParameters(const float sigmaNorm,
                                 const float sigmaTang,
                                 const float normBelowCutoff,
                                 const float normAboveCutoff,
                                 const float tangCutoff);
        
      /// get the brainfish mapping method parameters
      void getBrainfishParameters(float& maxDistance,
                                  int& splatFactor) const;
      
      /// set the brainfish mapping method parameters
      void setBrainfishParameters(const float maxDistance,
                                  const int splatFactor);
                                                           
      /// get the mapping neighbor layers
      int getNeighborLayers() const;
                       
      /// load user preferences 
      void loadUserPreferences();
      
      /// save user preferences
      void saveUserPreferences() const;

      /// preset the names of the output metric files                
      void setDefaultMetricFileNames(const std::vector<std::string> defaultNames);
      
      /// preset the names of the output spec files                
      void setDefaultSpecFileNames(const std::vector<std::string> defaultNames);
      
      /// set the mapping neighbor layers
      void setNeighborLayers(const int nl);
      
#ifdef OSX_COMPILER_BUG
      static void initializeStatic();
#endif

};

#ifdef __FMRI_DATA_MAPPER_MAIN__

int FMRISurfaceInfo::metricNameCounter = 0;

#ifdef OSX_COMPILER_BUG
   std::string FMRIDataMapper::preferencesValueYes = "YES";
   std::string FMRIDataMapper::preferencesValueNo = "NO";
   std::string FMRIDataMapper::preferencesRawDimensionTag = "tag-raw-dimensions";
   std::string FMRIDataMapper::preferencesByteSwapTag = "tag-byte-swapping";

   std::string FMRIDataMapper::preferencesAlgorithmTag =
                                                "tag-algorithm";
   std::string FMRIDataMapper::preferencesValueAlgorithmAverageNodes =
                                                "ALGORITHM_AVERAGE_NODES";
   std::string FMRIDataMapper::preferencesValueAlgorithmAverageVoxel =
                                                "ALGORITHM_AVERAGE_VOXEL";
   std::string FMRIDataMapper::preferencesValueAlgorithmGaussian =
                                                "ALGORITHM_GAUSSIAN";
   std::string FMRIDataMapper::preferencesValueAlgorithmMaximumVoxel =
                                                "ALGORITHM_MAXIMUM_VOXEL";
   std::string FMRIDataMapper::preferencesValueAlgorithmMCWBrainfish =
                                                "ALGORITHM_MCW_BRAINFISH";

   std::string FMRIDataMapper::preferencesAlgorithmNeighborsTag =
                                                "tag-algorithm-neighbors";
   std::string FMRIDataMapper::preferencesAlgorithmSigmaNormTag =
                                                "tag-algorithm-sigma-norm";
   std::string FMRIDataMapper::preferencesAlgorithmSigmaTangTag =
                                                "tag-algorithm-sigma-tang";
   std::string FMRIDataMapper::preferencesAlgorithmNormBelowTag =
                                                "tag-algorithm-norm-below";
   std::string FMRIDataMapper::preferencesAlgorithmNormAboveTag =
                                                "tag-algorithm-norm-above";
   std::string FMRIDataMapper::preferencesAlgorithmTangTag =
                                                "tag-algorithm-tang";
   std::string FMRIDataMapper::preferencesAlgorithmMaxDistanceTag =
                                                "tag-algorithm-max-dist";
   std::string FMRIDataMapper::preferencesAlgorithmSplatFactorTag =
                                                "tag-algorithm-splat";

   std::string FMRIDataMapper::preferencesAtlasDirectoryTag = "tag-atlas-directory";
   std::string FMRIDataMapper::preferencesAtlasLastSelected = "tag-last-atlas-selected";
   std::string FMRIDataMapper::preferencesIndivPaddingTag = "tag-indiv-padding";
   std::string FMRIDataMapper::preferencesIndivCroppingOffsetTag = "tag-indiv-cropping-offset";
   
   std::string FMRIDataMapper::preferencesVolumeTypeTag =
                                                "tag-volume-type";
   std::string FMRIDataMapper::preferencesVolumeValueTypeAFNI =
                                                "VOLUME_AFNI";
   std::string FMRIDataMapper::preferencesVolumeValueTypeAnalyze =
                                                "VOLUME_ANALYZE";
   std::string FMRIDataMapper::preferencesVolumeValueTypeSPM_LPI =
                                                "VOLUME_SPM";
   std::string FMRIDataMapper::preferencesVolumeValueTypeSPM_RPI =
                                                "VOLUME_SPM_RPI";
   std::string FMRIDataMapper::preferencesVolumeValueTypeWUNIL =
                                                "VOLUME_WUNIL";
   std::string FMRIDataMapper::preferencesVolumeValueTypeSigned8 =
                                                "VOLUME_SIGNED_8";
   std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned8 =
                                                "VOLUME_UNSIGNED_8";
   std::string FMRIDataMapper::preferencesVolumeValueTypeSigned16 =
                                                "VOLUME_SIGNED_16";
   std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned16 =
                                                "VOLUME_UNSIGNED_16";
   std::string FMRIDataMapper::preferencesVolumeValueTypeSigned32 =
                                                "VOLUME_SIGNED_32";
   std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned32 =
                                                "VOLUME_UNSIGNED_32";
   std::string FMRIDataMapper::preferencesVolumeValueTypeSigned64 =
                                                "VOLUME_SIGNED_64";
   std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned64 =
                                                "VOLUME_UNSIGNED_64";
   std::string FMRIDataMapper::preferencesVolumeValueTypeFloat32 =
                                                "VOLUME_FLOAT_32";
   std::string FMRIDataMapper::preferencesVolumeValueTypeFloat64 =
                                                "VOLUME_FLOAT_64";

   std::string FMRIDataMapper::preferencesSurfaceTypeTag =
                                                "tag-surface-type";
   std::string FMRIDataMapper::preferencesSurfaceValueTypeVTK =
                                                "SURFACE_VTK";
                                                
   std::string FMRIDataMapper::preferencesOrientationValueLeft = "LEFT";
   std::string FMRIDataMapper::preferencesOrientationValueRight = "RIGHT";
   std::string FMRIDataMapper::preferencesOrientationValueAnterior  =
                                                         "ANTERIOR";
   std::string FMRIDataMapper::preferencesOrientationValuePosterior =
                                                         "POSTERIOR";
   std::string FMRIDataMapper::preferencesOrientationValueInferior =
                                                         "INFERIOR";
   std::string FMRIDataMapper::preferencesOrientationValueSuperior =
                                                         "SUPERIOR";
   
   std::string FMRIDataMapper::preferencesVolumeOrientationTag =
                                                "tag-volume-orientation";
   
   std::string FMRIDataMapper::preferencesIndivSurfaceOrientationTag =
                                             "tag-indiv-surface-orientation";
   
   void 
   FMRIDataMapper::initializeStatic()
   {
      FMRIDataMapper::preferencesValueYes = "YES";
      FMRIDataMapper::preferencesValueNo = "NO";
      FMRIDataMapper::preferencesRawDimensionTag = "tag-raw-dimensions";
      FMRIDataMapper::preferencesByteSwapTag = "tag-byte-swapping";

      FMRIDataMapper::preferencesAlgorithmTag =
                                            "tag-algorithm";
      FMRIDataMapper::preferencesValueAlgorithmAverageNodes =
                                            "ALGORITHM_AVERAGE_NODES";
      FMRIDataMapper::preferencesValueAlgorithmAverageVoxel =
                                            "ALGORITHM_AVERAGE_VOXEL";
      FMRIDataMapper::preferencesValueAlgorithmGaussian =
                                            "ALGORITHM_GAUSSIAN";
      FMRIDataMapper::preferencesValueAlgorithmMaximumVoxel =
                                            "ALGORITHM_MAXIMUM_VOXEL";
      FMRIDataMapper::preferencesValueAlgorithmMCWBrainfish =
                                            "ALGORITHM_MCW_BRAINFISH";

      FMRIDataMapper::preferencesAlgorithmNeighborsTag =
                                            "tag-algorithm-neighbors";
      FMRIDataMapper::preferencesAlgorithmSigmaNormTag =
                                            "tag-algorithm-sigma-norm";
      FMRIDataMapper::preferencesAlgorithmSigmaTangTag =
                                            "tag-algorithm-sigma-tang";
      FMRIDataMapper::preferencesAlgorithmNormBelowTag =
                                            "tag-algorithm-norm-below";
      FMRIDataMapper::preferencesAlgorithmNormAboveTag =
                                            "tag-algorithm-norm-above";
      FMRIDataMapper::preferencesAlgorithmTangTag =
                                            "tag-algorithm-tang";
      FMRIDataMapper::preferencesAlgorithmMaxDistanceTag =
                                            "tag-algorithm-max-dist";
      FMRIDataMapper::preferencesAlgorithmSplatFactorTag =
                                            "tag-algorithm-splat";
                                            
      FMRIDataMapper::preferencesIndivPaddingTag =
                                             "tag-indiv-padding";
      
      FMRIDataMapper::preferencesIndivCroppingOffsetTag =
                                             "tag-indiv-cropping-offset";

      FMRIDataMapper::preferencesAtlasDirectoryTag = "tag-atlas-directory";
      FMRIDataMapper::preferencesAtlasLastSelected = "tag-last-atlas-selected";
       
      FMRIDataMapper::preferencesVolumeTypeTag =
                                            "tag-volume-type";
      FMRIDataMapper::preferencesVolumeValueTypeAFNI =
                                            "VOLUME_AFNI";
      FMRIDataMapper::preferencesVolumeValueTypeAnalyze =
                                            "VOLUME_ANALYZE";
      FMRIDataMapper::preferencesVolumeValueTypeSPM_LPI =
                                            "VOLUME_SPM";
      FMRIDataMapper::preferencesVolumeValueTypeSPM_RPI =
                                            "VOLUME_SPM_RPI";
      FMRIDataMapper::preferencesVolumeValueTypeWUNIL =
                                            "VOLUME_WUNIL";
      FMRIDataMapper::preferencesVolumeValueTypeSigned8 =
                                            "VOLUME_SIGNED_8";
      FMRIDataMapper::preferencesVolumeValueTypeUnsigned8 =
                                            "VOLUME_UNSIGNED_8";
      FMRIDataMapper::preferencesVolumeValueTypeSigned16 =
                                            "VOLUME_SIGNED_16";
      FMRIDataMapper::preferencesVolumeValueTypeUnsigned16 =
                                            "VOLUME_UNSIGNED_16";
      FMRIDataMapper::preferencesVolumeValueTypeSigned32 =
                                            "VOLUME_SIGNED_32";
      FMRIDataMapper::preferencesVolumeValueTypeUnsigned32 =
                                            "VOLUME_UNSIGNED_32";
      FMRIDataMapper::preferencesVolumeValueTypeSigned64 =
                                            "VOLUME_SIGNED_64";
      FMRIDataMapper::preferencesVolumeValueTypeUnsigned64 =
                                            "VOLUME_UNSIGNED_64";
      FMRIDataMapper::preferencesVolumeValueTypeFloat32 =
                                            "VOLUME_FLOAT_32";
      FMRIDataMapper::preferencesVolumeValueTypeFloat64 =
                                            "VOLUME_FLOAT_64";

      FMRIDataMapper::preferencesSurfaceTypeTag =
                                            "tag-surface-type";
      FMRIDataMapper::preferencesSurfaceValueTypeVTK =
                                            "SURFACE_VTK";
      FMRIDataMapper::preferencesOrientationValueLeft = "LEFT";
      FMRIDataMapper::preferencesOrientationValueRight = "RIGHT";
      FMRIDataMapper::preferencesOrientationValueAnterior  =
                                                     "ANTERIOR";
      FMRIDataMapper::preferencesOrientationValuePosterior =
                                                     "POSTERIOR";
      FMRIDataMapper::preferencesOrientationValueInferior =
                                                     "INFERIOR";
      FMRIDataMapper::preferencesOrientationValueSuperior =
                                                     "SUPERIOR";

      FMRIDataMapper::preferencesVolumeOrientationTag =
                                            "tag-volume-orientation";

      FMRIDataMapper::preferencesIndivSurfaceOrientationTag =
                                                "tag-indiv-surface-orientation";
   }

#else
   const std::string FMRIDataMapper::preferencesValueYes = "YES";
   const std::string FMRIDataMapper::preferencesValueNo = "NO";
   const std::string FMRIDataMapper::preferencesRawDimensionTag = "tag-raw-dimensions";
   const std::string FMRIDataMapper::preferencesByteSwapTag = "tag-byte-swapping";

   const std::string FMRIDataMapper::preferencesAlgorithmTag =
                                                      "tag-algorithm";
   const std::string FMRIDataMapper::preferencesValueAlgorithmAverageNodes =
                                                      "ALGORITHM_AVERAGE_NODES";
   const std::string FMRIDataMapper::preferencesValueAlgorithmAverageVoxel =
                                                      "ALGORITHM_AVERAGE_VOXEL";
   const std::string FMRIDataMapper::preferencesValueAlgorithmGaussian =
                                                      "ALGORITHM_GAUSSIAN";
   const std::string FMRIDataMapper::preferencesValueAlgorithmMaximumVoxel =
                                                      "ALGORITHM_MAXIMUM_VOXEL";
   const std::string FMRIDataMapper::preferencesValueAlgorithmMCWBrainfish =
                                                      "ALGORITHM_MCW_BRAINFISH";

   const std::string FMRIDataMapper::preferencesAlgorithmNeighborsTag =
                                                      "tag-algorithm-neighbors";
   const std::string FMRIDataMapper::preferencesAlgorithmSigmaNormTag =
                                                      "tag-algorithm-sigma-norm";
   const std::string FMRIDataMapper::preferencesAlgorithmSigmaTangTag =
                                                      "tag-algorithm-sigma-tang";
   const std::string FMRIDataMapper::preferencesAlgorithmNormBelowTag =
                                                      "tag-algorithm-norm-below";
   const std::string FMRIDataMapper::preferencesAlgorithmNormAboveTag =
                                                      "tag-algorithm-norm-above";
   const std::string FMRIDataMapper::preferencesAlgorithmTangTag =
                                                      "tag-algorithm-tang";
   const std::string FMRIDataMapper::preferencesAlgorithmMaxDistanceTag =
                                                      "tag-algorithm-max-dist";
   const std::string FMRIDataMapper::preferencesAlgorithmSplatFactorTag =
                                                      "tag-algorithm-splat";

   const std::string FMRIDataMapper::preferencesAtlasDirectoryTag =
                                                      "tag-atlas-directory";
   const std::string FMRIDataMapper::preferencesAtlasLastSelected = "tag-last-atlas-selected";
   
   const std::string FMRIDataMapper::preferencesIndivPaddingTag =
                                             "tag-indiv-padding";
   const std::string FMRIDataMapper::preferencesIndivCroppingOffsetTag =
                                             "tag-indiv-cropping-offset";
//   const std::string FMRIDataMapper::preferencesIndivStructVolDimTag = 
//                                             "tag-indiv-struct-vol-dim";
                                             
   const std::string FMRIDataMapper::preferencesVolumeTypeTag =
                                                      "tag-volume-type";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeAFNI =
                                                      "VOLUME_AFNI";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeAnalyze =
                                                      "VOLUME_ANALYZE";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeSPM_LPI =
                                                      "VOLUME_SPM";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeSPM_RPI =
                                                      "VOLUME_SPM_RPI";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeWUNIL =
                                                      "VOLUME_WUNIL";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeSigned8 =
                                                      "VOLUME_SIGNED_8";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned8 =
                                                      "VOLUME_UNSIGNED_8";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeSigned16 =
                                                      "VOLUME_SIGNED_16";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned16 =
                                                      "VOLUME_UNSIGNED_16";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeSigned32 =
                                                      "VOLUME_SIGNED_32";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned32 =
                                                      "VOLUME_UNSIGNED_32";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeSigned64 =
                                                      "VOLUME_SIGNED_64";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeUnsigned64 =
                                                      "VOLUME_UNSIGNED_64";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeFloat32 =
                                                      "VOLUME_FLOAT_32";
   const std::string FMRIDataMapper::preferencesVolumeValueTypeFloat64 =
                                                      "VOLUME_FLOAT_64";

   const std::string FMRIDataMapper::preferencesSurfaceTypeTag =
                                                      "tag-surface-type";
   const std::string FMRIDataMapper::preferencesSurfaceValueTypeVTK =
                                                      "SURFACE_VTK";
                                                      
   const std::string FMRIDataMapper::preferencesOrientationValueLeft = "LEFT";
   const std::string FMRIDataMapper::preferencesOrientationValueRight = "RIGHT";
   const std::string FMRIDataMapper::preferencesOrientationValueAnterior  =
                                                               "ANTERIOR";
   const std::string FMRIDataMapper::preferencesOrientationValuePosterior =
                                                               "POSTERIOR";
   const std::string FMRIDataMapper::preferencesOrientationValueInferior =
                                                               "INFERIOR";
   const std::string FMRIDataMapper::preferencesOrientationValueSuperior =
                                                               "SUPERIOR";
   
   const std::string FMRIDataMapper::preferencesVolumeOrientationTag =
                                                      "tag-volume-orientation";
   
   const std::string FMRIDataMapper::preferencesIndivSurfaceOrientationTag =
                                              "tag-indiv-surface-orientation";
#endif

#endif

#endif
