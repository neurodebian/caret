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



#include <qglobal.h>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <iostream>
#include <limits>
#include <sstream>

#include <qdir.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>

#include "AtlasSurfaceDirectoryFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "ParamsFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"

#include "vtkImageFlip.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTriangleFilter.h"

#define __FMRI_DATA_MAPPER_MAIN__
#include "FMRIDataMapper.h"
#undef __FMRI_DATA_MAPPER_MAIN__

#include "map_fmri_prototypes.h"

/**
 * Constructor.
 */
FMRIDataMapper::FMRIDataMapper() {
   
   errorMessageCallback = NULL;
   
   mappingAlgorithm = ALGORITHM_AVERAGE_NODES;
   volumeType = VOLUME_TYPE_AFNI;
   surfaceFileType = SURFACE_FILE_TYPE_VTK_POLYDATA;
   mappingSurfaceType = MAPPING_SURFACE_ATLAS;
   
   volumeOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
   volumeOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
   volumeOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
   
   indivSurfaceOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
   indivSurfaceOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
   indivSurfaceOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;

   neighborLayers = 0;
   
   rawVolumeDimensionX = 0;
   rawVolumeDimensionY = 0;
   rawVolumeDimensionZ = 0;
      
   gaussianSigmaNorm = 2.0;
   gaussianSigmaTang = 1.0;
   gaussianNormBelowCutoff = 2.0;
   gaussianNormAboveCutoff = 2.0;
   gaussianTangCutoff      = 3.0; 
   
   brainfishMaxDistance = 1.0;
   brainfishSplatFactor = 1;
   
   indivCroppingOffsetX = 0;
   indivCroppingOffsetY = 0;
   indivCroppingOffsetZ = 0;
   
   indivPaddingEnabled = false;
   indivPaddingNegX = 0;
   indivPaddingNegY = 0;
   indivPaddingNegZ = 0;
   indivPaddingPosX = 0;
   indivPaddingPosY = 0;
   indivPaddingPosZ = 0;
   
//    indivStructVolDimX = 0;
//    indivStructVolDimY = 0;
//    indivStructVolDimZ = 0;
   
   volumeByteSwapping = false;
   
   selectedAtlasDirectory = 0;
   lastSelectedAtlas.assign("");
}

/**
 * Destructor.
 */
FMRIDataMapper::~FMRIDataMapper() {
}

/**
 * process an error message.
 */
void 
FMRIDataMapper::processErrorMessage(const std::string& s) const
{
   if (errorMessageCallback != NULL) {
      (*errorMessageCallback)(s);
   }
   else {
      std::cout << "ERROR: " << s << std::endl;
   }
}

/**
 * Set the error message callback.
 */
void 
FMRIDataMapper::setErrorMessageCallback(void (*callback)(const std::string&))
{
   errorMessageCallback = callback;
}

/**
 * Add a surface.
 */
void
FMRIDataMapper::addSurface(const std::string& subdir,
                           const std::string& surfaceName,
                           const std::string& descriptiveName,
                           const std::string& anatomyName,
                           const std::string& specFileName)
{
   FMRISurfaceInfo fsi(subdir, surfaceName, descriptiveName, 
                       anatomyName, specFileName);
   
   for (int i = 0; i < getNumberOfVolumes(); i++) {
      FMRIVolumeInfo* volInfo = getVolume(i);
      const std::string volumeName(volInfo->getVolumeName());
      std::vector<std::string> subVolNames;
      volInfo->getSubVolumeNames(subVolNames);
      //
      // Special case for AFNI volume with 2 subvolumes
      //
      if ((volumeType == VOLUME_TYPE_AFNI) &&
          (subVolNames.size() > 0)) {
         if (volInfo->getNumberOfSubVolumes() == 2) {
            std::string v1name("view-");
            v1name.append(FileUtilities::basename(volumeName));
            fsi.addMetricColumnInfo(v1name, i, 0);
            std::string v2name("threshold-");
            v2name.append(FileUtilities::basename(volumeName));
            fsi.addMetricColumnInfo(v2name, i, 1);
         }
         else {
            for  (int j = 0; j < volInfo->getNumberOfSubVolumes(); j++) {
               if (j < static_cast<int>(subVolNames.size())) {
                  std::string name(subVolNames[j]);
                  name.append(" - ");
                  name.append(FileUtilities::basename(volumeName));
                  fsi.addMetricColumnInfo(name, i, j);
               }
               else {
                  fsi.addMetricColumnInfo(FileUtilities::basename(volumeName), i, j);
               }
            }
         }
      }
      else if ((volumeType == VOLUME_TYPE_AFNI) &&
               (volInfo->getNumberOfSubVolumes() == 2)) {
         std::string v1name("view-");
         v1name.append(FileUtilities::basename(volumeName));
         fsi.addMetricColumnInfo(v1name, i, 0);
         std::string v2name("threshold-");
         v2name.append(FileUtilities::basename(volumeName));
         fsi.addMetricColumnInfo(v2name, i, 1);
      }
      else {
         for (int j = 0; j < volInfo->getNumberOfSubVolumes(); j++) {
            std::string name(QString::number(j + 1).latin1());
            name.append(" - ");
            name.append(FileUtilities::basename(volumeName));
            fsi.addMetricColumnInfo(name, i, j);
         }
      }
   }
   
   const int surfaceIndex = getNumberOfSurfaces();
   if (surfaceIndex < static_cast<int>(defaultMetricFileNames.size())) {
      fsi.setMetricFileName(defaultMetricFileNames[surfaceIndex]);
   }
   if (surfaceIndex < static_cast<int>(defaultSpecFileNames.size())) {
      fsi.setSpecFileName(defaultSpecFileNames[surfaceIndex]);
   }
   
   surfaces.push_back(fsi);
}

/**
 * Delete a surface.
 */
void
FMRIDataMapper::deleteSurface(const int index)
{
   if ((index < 0) || (index >= getNumberOfSurfaces())) {
      std::cerr << "bad fMRI surface info index " << index << std::endl;
      return;
   }
   std::vector<FMRISurfaceInfo>::iterator iter = surfaces.begin();
   iter += index;
   surfaces.erase(iter);
}

/**
 * Get a surface.
 */
FMRISurfaceInfo*
FMRIDataMapper::getSurface(const int index)
{
   if ((index < 0) || (index >= getNumberOfSurfaces())) {
      std::cerr << "bad fMRI surface info index " << index << std::endl;
      return NULL;
   }
   return &surfaces[index];
}

/**
 * Get the number of surfaces
 */
int
FMRIDataMapper::getNumberOfSurfaces() const
{
   return (int)surfaces.size();
}

/**
 * Input may be either the full path to the map_fmri_atlas.directory file
 * or the top level atlas directory.  If the input is the top level atlas
 * directory, it must contain a file named map_fmri_atlas.directory in the
 * subdirectory MAP_MASTER_FILES.
 */
bool 
FMRIDataMapper::addAtlasDirectory(const std::string& atlasDirNameIn,
                                  const bool saveInPreferences, 
                                  std::string& errorMessage)
{
   std::string atlasDirName(atlasDirNameIn);
   
   errorMessage.assign("");
   
   AtlasDirectoryInfo adi(atlasDirName, saveInPreferences);
   if (adi.isValid() == false ) {
      errorMessage.assign("Atlas mapping list list named \"");
      errorMessage.append(adi.getMappingSurfaceListFile());
      errorMessage.append("\" not found.");
      return true;
   }
   
   AtlasSurfaceDirectoryFile asdf;
   try {
      asdf.readFile(adi.getMappingSurfaceListFile());
   }
   catch (FileIOException& e) {
      errorMessage.assign(atlasDirName);
      errorMessage.append(" does not appear to be an Atlas Directory File");
      return true;
   }
   
   for (unsigned int i = 0; i < atlasDirectories.size(); i++) {
      if (atlasDirectories[i].getAtlasDirectory() == adi.getAtlasDirectory()) {
         return false;
      }
   }
   
   atlasDirectories.push_back(adi);
   
   return false;
}

/** 
 * Get the number of atlas directories.
 */
int 
FMRIDataMapper::getNumberOfAtlasDirectories() const
{
   return (int)atlasDirectories.size();
}

/**
 * Get the selected atlas directory.
 */
int
FMRIDataMapper::getSelectedAtlasDirectory() const
{
   if(atlasDirectories.size() > 0) {
      return selectedAtlasDirectory;
   }
   return -1;
}

/**
 * Set the selected atlas directory
 */
void
FMRIDataMapper::setSelectedAtlasDirectory(const int num)
{
   selectedAtlasDirectory = num;
}

/**
 * Get info about an atlas directory.
 */
AtlasDirectoryInfo*
FMRIDataMapper::getAtlasDirectoryInfo(const int num)
{
   if ((num >= 0) && (num < getNumberOfAtlasDirectories())) {
      return &atlasDirectories[num];
   }
   return NULL;
}

/**
 * Add a volume.
 */
bool
FMRIDataMapper::addVolume(const std::string& volumeName, 
                          std::string& errorMessage)
{
   errorMessage.assign("");
   
   int numberOfSubVolumes = -1;
   std::vector<std::string> subVolumeNames;
   
   VolumeFile::ORIENTATION orientation[3] = { VolumeFile::ORIENTATION_UNKNOWN,
                                              VolumeFile::ORIENTATION_UNKNOWN,
                                              VolumeFile::ORIENTATION_UNKNOWN };
   switch(getVolumeType()) {
      case VOLUME_TYPE_AFNI:
      case VOLUME_TYPE_ANALYZE:
      case VOLUME_TYPE_SPM_LPI:
      case VOLUME_TYPE_SPM_RPI:
      case VOLUME_TYPE_WUNIL:
         {
            VolumeFile volume;
            try {
               switch(getVolumeType()) {
                  case VOLUME_TYPE_AFNI:
                     volume.readVolumeHeader(volumeName, VolumeFile::FILE_READ_WRITE_TYPE_AFNI);
                     break;
                  case VOLUME_TYPE_ANALYZE:
                     volume.readVolumeHeader(volumeName, VolumeFile::FILE_READ_WRITE_TYPE_ANALYZE);
                     break;
                  case VOLUME_TYPE_SPM_LPI:
                  case VOLUME_TYPE_SPM_RPI:
                     volume.readVolumeHeader(volumeName, VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX);
                     break;
                  case VOLUME_TYPE_WUNIL:
                     volume.readVolumeHeader(volumeName, VolumeFile::FILE_READ_WRITE_TYPE_WUNIL);
                     break;
                  case VOLUME_TYPE_RAW_8_BIT_SIGNED:
                  case VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
                  case VOLUME_TYPE_RAW_16_BIT_SIGNED:
                  case VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
                  case VOLUME_TYPE_RAW_32_BIT_SIGNED:
                  case VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
                  case VOLUME_TYPE_RAW_64_BIT_SIGNED:
                  case VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
                  case VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
                  case VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
                     break;
               }
            }
            catch (FileIOException& e) {
               errorMessage.assign(e.what());
               return true;
            }
            numberOfSubVolumes = volume.getNumberOfSubVolumes();
            volume.getSubVolumeNames(subVolumeNames);
            volume.getOrientation(orientation);
         }
         break;
      case VOLUME_TYPE_RAW_8_BIT_SIGNED:
      case VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_16_BIT_SIGNED:
      case VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_32_BIT_SIGNED:
      case VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_64_BIT_SIGNED:
      case VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
      case VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
         numberOfSubVolumes = 1;
         break;
   }
   if (numberOfSubVolumes <= 0) {
      errorMessage.assign("Error getting number of sub volumes for ");
      errorMessage.append(volumeName);
      errorMessage.append(".  Is it a valid file?");
      return true;
   }

   //
   // Make sure AFNI and WU-NIL volumes have acceptable orientations
   //
   switch(getVolumeType()) {
      case VOLUME_TYPE_AFNI:
      case VOLUME_TYPE_WUNIL:
         {
            std::string msg;
            if ((orientation[0] != VolumeFile::ORIENTATION_RIGHT_TO_LEFT) &&
               (orientation[0] != VolumeFile::ORIENTATION_LEFT_TO_RIGHT)) {
               msg.append("X orientation must be left-to-right or right-to-left\n");
               msg.append("X orientation is ");
               msg.append(VolumeFile::getOrientationLabel(orientation[0]));
            }
            if ((orientation[1] != VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR) &&
               (orientation[1] != VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR)) {
               msg.append("Y orientation must be posterior-to-anterior or anterior-to-posterior\n");
               msg.append("Y orientation is ");
               msg.append(VolumeFile::getOrientationLabel(orientation[1]));
            }
            if ((orientation[2] != VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR) &&
               (orientation[2] != VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR)) {
               msg.append("Z orientation must be inferior-to-superior or superior-to-inferior\n");
               msg.append("Z orientation is ");
               msg.append(VolumeFile::getOrientationLabel(orientation[2]));
            }
            if (msg.empty() == false) {
               errorMessage.assign("Volume ");
               errorMessage.append(FileUtilities::basename(volumeName));
               errorMessage.append("\nhas unacceptable orientation(s):\n");
               errorMessage.append(msg);
               return true;
            }
         }
         break;
      case VOLUME_TYPE_ANALYZE:
      case VOLUME_TYPE_SPM_LPI:
      case VOLUME_TYPE_SPM_RPI:
      case VOLUME_TYPE_RAW_8_BIT_SIGNED:
      case VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_16_BIT_SIGNED:
      case VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_32_BIT_SIGNED:
      case VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_64_BIT_SIGNED:
      case VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
      case VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
         break;
   }
   
   FMRIVolumeInfo volInfo(volumeName, numberOfSubVolumes, subVolumeNames);
   volumes.push_back(volInfo);
   const int volumeIndex = static_cast<int>(volumes.size()) - 1;
   
   //volumeNames.push_back(volumeName);
   //volumesNumberOfSubVolumes.push_back(numberOfSubVolumes);
   //const int volumeIndex = static_cast<int>(volumeNames.size()) - 1;
   
   for (int i = 0; i < getNumberOfSurfaces(); i++) {
      FMRISurfaceInfo* fsi = getSurface(i);
      if ((volumeType == VOLUME_TYPE_AFNI) &&
          (subVolumeNames.size() > 0)) {
         for  (int j = 0; j < numberOfSubVolumes; j++) {
            if (j < static_cast<int>(subVolumeNames.size())) {
               std::string name(subVolumeNames[j]);
               name.append(" - ");
               name.append(FileUtilities::basename(volumeName));
               fsi->addMetricColumnInfo(name, volumeIndex, j);
            }
            else {
               fsi->addMetricColumnInfo(FileUtilities::basename(volumeName), volumeIndex, j);
            }
         }
      }
      else if ((volumeType == VOLUME_TYPE_AFNI) &&
          (numberOfSubVolumes == 2)) {
         std::string v1name("view-");
         v1name.append(FileUtilities::basename(volumeName));
         fsi->addMetricColumnInfo(v1name, volumeIndex, 0);
         std::string v2name("threshold-");
         v2name.append(FileUtilities::basename(volumeName));
         fsi->addMetricColumnInfo(v2name, volumeIndex, 1);
      }
      else {
         for (int j = 0; j < numberOfSubVolumes; j++) {
            std::string name(QString::number(j).latin1());
            name.append(FileUtilities::basename(volumeName));
            fsi->addMetricColumnInfo(name, volumeIndex, j);
         }
      }
   }
   
   return false;
}

/**
 * Delete a volume.
 */
void
FMRIDataMapper::deleteVolume(const int index)
{
   if ((index < 0) || (index >= getNumberOfVolumes())) {
      std::cerr << "bad fMRI surface index " << index << std::endl;
      return;
   }
   
   std::vector<FMRIVolumeInfo>::iterator iter = volumes.begin();
   iter += index;
   volumes.erase(iter);
   
   for (int i = 0; i < getNumberOfSurfaces(); i++) {
      FMRISurfaceInfo* fsi = getSurface(i);
      fsi->deleteMetricColumnName(index);
   }
}

/**
 * Get the number of volumes.
 */
int
FMRIDataMapper::getNumberOfVolumes() const
{
   return volumes.size();
}

/**
 * Get a volume.
 */
FMRIVolumeInfo*
FMRIDataMapper::getVolume(const int index)
{
   return &volumes[index];
}

/**
 * Remove all surfaces.
 */
void
FMRIDataMapper::clearSurfaces()
{
   surfaces.clear();
}

/**
 * Remove all volumes.
 */
void
FMRIDataMapper::clearVolumes()
{
   volumes.clear();
}

/**
 * Get the gaussian parameters.
 */
void 
FMRIDataMapper::getGaussianParameters(float& sigmaNorm,
                                      float& sigmaTang,
                                      float& normBelowCutoff,
                                      float& normAboveCutoff,
                                      float& tangCutoff) const
{
   sigmaNorm = gaussianSigmaNorm;
   sigmaTang = gaussianSigmaTang;
   normBelowCutoff = gaussianNormBelowCutoff;
   normAboveCutoff = gaussianNormAboveCutoff;
   tangCutoff = gaussianTangCutoff;
}
                                 
/**
 * Set the gaussian parameters.
 */
void
FMRIDataMapper::setGaussianParameters(const float sigmaNorm,
                                      const float sigmaTang,
                                      const float normBelowCutoff,
                                      const float normAboveCutoff,
                                      const float tangCutoff)
{
   gaussianSigmaNorm = sigmaNorm;
   gaussianSigmaTang = sigmaTang;
   gaussianNormBelowCutoff = normBelowCutoff;
   gaussianNormAboveCutoff = normAboveCutoff;
   gaussianTangCutoff = tangCutoff;
}

/**
 * Get the brain fish parameters.
 */
void 
FMRIDataMapper::getBrainfishParameters(float& maxDistance,
                                       int& splatFactor) const
{
   maxDistance = brainfishMaxDistance;
   splatFactor = brainfishSplatFactor;
}

/**
 * Set the brainfish parameters.
 */
void 
FMRIDataMapper::setBrainfishParameters(const float maxDistance,
                                       const int splatFactor)
{
   brainfishMaxDistance = maxDistance;
   brainfishSplatFactor = splatFactor;
}
                            
/**
 * Get the neighbor layers mapping parameter.
 */
int 
FMRIDataMapper::getNeighborLayers() const
{
   return neighborLayers;
}
                           
/**
 * Set the neighbor layers mapping parameter.
 */
void
FMRIDataMapper::setNeighborLayers(const int nl)
{
   neighborLayers = nl;
}

/**
 * Get the surface file type.
 */
FMRIDataMapper::SURFACE_FILE_TYPE 
FMRIDataMapper::getSurfaceFileType() const
{
   return surfaceFileType;
}

/**
 * Set the surface file type.
 */
void 
FMRIDataMapper::setSurfaceFileType(const FMRIDataMapper::SURFACE_FILE_TYPE st)
{
   surfaceFileType = st;
}

/**
 *  Get the surface file types and names.
 */
void 
FMRIDataMapper::getSurfaceFileTypesAndNames(
                              std::vector<FMRIDataMapper::SURFACE_FILE_TYPE>& types,
                              std::vector<std::string>& names) const
{
   types.clear();
   names.clear();
   
   types.push_back(SURFACE_FILE_TYPE_VTK_POLYDATA);  
      names.push_back("VTK Polydata");
}

/**
 * Get the volume types and names.
 */
void 
FMRIDataMapper::getVolumeTypesAndNames(std::vector<FMRIDataMapper::VOLUME_TYPE>& types,
                                       std::vector<std::string>& names) const
{
   types.clear();
   names.clear();
   
   types.push_back(VOLUME_TYPE_AFNI);  
      names.push_back("AFNI (.HEAD)");
   types.push_back(VOLUME_TYPE_ANALYZE);  
      names.push_back("Analyze (.hdr)");
   types.push_back(VOLUME_TYPE_SPM_LPI);  
      names.push_back("SPM/MEDx LPI Orientation (.hdr)");
   types.push_back(VOLUME_TYPE_SPM_RPI);  
      names.push_back("SPM/MEDx RPI Orientation (.hdr)");
   types.push_back(VOLUME_TYPE_WUNIL);
      names.push_back("Wash U. NIL (.ifh)");
   types.push_back(VOLUME_TYPE_RAW_8_BIT_SIGNED);  
      names.push_back("RAW - 8 Bit Signed");
   types.push_back(VOLUME_TYPE_RAW_8_BIT_UNSIGNED);
      names.push_back("RAW - 8 Bit Unsigned");
   types.push_back(VOLUME_TYPE_RAW_16_BIT_SIGNED);
      names.push_back("RAW - 16 Bit Signed");
   types.push_back(VOLUME_TYPE_RAW_16_BIT_UNSIGNED);
      names.push_back("RAW - 16 Bit Unsigned");
   types.push_back(VOLUME_TYPE_RAW_32_BIT_SIGNED);  
      names.push_back("RAW - 32 Bit Signed");
   types.push_back(VOLUME_TYPE_RAW_32_BIT_UNSIGNED);
       names.push_back("RAW - 32 Bit Unsigned");
   types.push_back(VOLUME_TYPE_RAW_64_BIT_SIGNED);
       names.push_back("RAW - 64 Bit Signed");
   types.push_back(VOLUME_TYPE_RAW_64_BIT_UNSIGNED);
       names.push_back("RAW - 64 Bit Unsigned");
   types.push_back(VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT);
       names.push_back("RAW - 32 Bit Floating Point");
   types.push_back(VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT);
       names.push_back("RAW - 64 Bit Floating Point");
}

/**
 * Get the orientation of the volume.
 */
void
FMRIDataMapper::getVolumeOrientation(X_NEGATIVE_ORIENTATION_TYPE& x,
                                     Y_NEGATIVE_ORIENTATION_TYPE& y,
                                     Z_NEGATIVE_ORIENTATION_TYPE& z)
{
   x = volumeOrientationX;
   y = volumeOrientationY;
   z = volumeOrientationZ;
}

/**
 * Get the orientation labels of the volume.
 */
void
FMRIDataMapper::getVolumeOrientationLabels(std::string& x,
                                           std::string& y,
                                           std::string& z)
{
   switch(volumeOrientationX) {
      case X_NEGATIVE_ORIENTATION_LEFT:
         x.assign(preferencesOrientationValueLeft);
         break;
      case X_NEGATIVE_ORIENTATION_RIGHT:
         x.assign(preferencesOrientationValueRight);
         break;
      default:
         x.assign("Unknown");
         break;
   }
   
   switch(volumeOrientationY) {
      case Y_NEGATIVE_ORIENTATION_ANTERIOR:
         y.assign(preferencesOrientationValueAnterior);
         break;
      case Y_NEGATIVE_ORIENTATION_POSTERIOR:
         y.assign(preferencesOrientationValuePosterior);
         break;
      default:
         y.assign("Unknown");
         break;
   }
   
   switch(volumeOrientationZ) {
      case Z_NEGATIVE_ORIENTATION_INFERIOR:
         z.assign(preferencesOrientationValueInferior);
         break;
      case Z_NEGATIVE_ORIENTATION_SUPERIOR:
         z.assign(preferencesOrientationValueSuperior);
         break;
      default:
         z.assign("Unknown");
         break;
   }
   
}

/**
 * Get the individual surface orientation labels.
 */
void
FMRIDataMapper::getIndivSurfaceOrientationLabels(std::string& x,
                                                 std::string& y,
                                                 std::string& z)
{
   switch(indivSurfaceOrientationX) {
      case X_NEGATIVE_ORIENTATION_LEFT:
         x.assign(preferencesOrientationValueLeft);
         break;
      case X_NEGATIVE_ORIENTATION_RIGHT:
         x.assign(preferencesOrientationValueRight);
         break;
      default:
         x.assign("Unknown");
         break;
   }
   
   switch(indivSurfaceOrientationY) {
      case Y_NEGATIVE_ORIENTATION_ANTERIOR:
         y.assign(preferencesOrientationValueAnterior);
         break;
      case Y_NEGATIVE_ORIENTATION_POSTERIOR:
         y.assign(preferencesOrientationValuePosterior);
         break;
      default:
         y.assign("Unknown");
         break;
   }
   
   switch(indivSurfaceOrientationZ) {
      case Z_NEGATIVE_ORIENTATION_INFERIOR:
         z.assign(preferencesOrientationValueInferior);
         break;
      case Z_NEGATIVE_ORIENTATION_SUPERIOR:
         z.assign(preferencesOrientationValueSuperior);
         break;
      default:
         z.assign("Unknown");
         break;
   }
   
}

/**
 * Set the volume orientation.
 */
void
FMRIDataMapper::setVolumeOrientation(const X_NEGATIVE_ORIENTATION_TYPE x,
                                     const Y_NEGATIVE_ORIENTATION_TYPE y,
                                     const Z_NEGATIVE_ORIENTATION_TYPE z)
{
   volumeOrientationX = x;
   volumeOrientationY = y;
   volumeOrientationZ = z;
}

/**
 * Get the individual surface orientation.
 */
void
FMRIDataMapper::getIndivSurfaceOrientation(X_NEGATIVE_ORIENTATION_TYPE& x,
                                           Y_NEGATIVE_ORIENTATION_TYPE& y,
                                           Z_NEGATIVE_ORIENTATION_TYPE& z)
{
   x = indivSurfaceOrientationX;
   y = indivSurfaceOrientationY;
   z = indivSurfaceOrientationZ;
}

/**
 * Set the individual surface orientation.
 */
void
FMRIDataMapper::setIndivSurfaceOrientation(const X_NEGATIVE_ORIENTATION_TYPE x,
                                           const Y_NEGATIVE_ORIENTATION_TYPE y,
                                           const Z_NEGATIVE_ORIENTATION_TYPE z)
{
   indivSurfaceOrientationX = x;
   indivSurfaceOrientationY = y;
   indivSurfaceOrientationZ = z;
}

/**
 * Get the mapping method.
 */
FMRIDataMapper::ALGORITHM_TYPE 
FMRIDataMapper::getMappingAlgorithm() const
{
   return mappingAlgorithm;
}

/**
 * Set the mapping method.
 */
void
FMRIDataMapper::setMappingAlgorithm(FMRIDataMapper::ALGORITHM_TYPE mmt)
{
   mappingAlgorithm = mmt;
}

/**
 * Get the dimensions for a raw volume.
 */
void 
FMRIDataMapper::getRawVolumeDimensions(int& x, int& y, int& z) const
{
   x = rawVolumeDimensionX;
   y = rawVolumeDimensionY;
   z = rawVolumeDimensionZ;
}

/**
 * Set the raw volume dimensions.
 */
void 
FMRIDataMapper::setRawVolumeDimensions(const int x, const int y, const int z)
{
   rawVolumeDimensionX = x;
   rawVolumeDimensionY = y;
   rawVolumeDimensionZ = z;
}

/**
 * Get the Volume byte swapping flag.
 */
bool 
FMRIDataMapper::getVolumeByteSwapping() const
{
   return volumeByteSwapping;
}

/**
 * Set the volume byte swapping flag.
 */
void 
FMRIDataMapper::setVolumeByteSwapping(const bool byteSwap)
{
   volumeByteSwapping = byteSwap;
}

/**
 * Get the individual's cropping offset.
 */
void 
FMRIDataMapper::getIndivCroppingOffset(bool& croppingEnabled,
                                       int& x, int& y, int& z) const
{
   croppingEnabled = indivCroppingEnabled;
   x = indivCroppingOffsetX;
   y = indivCroppingOffsetY;
   z = indivCroppingOffsetZ;
}

/**
 * Set the individuals cropping offset.
 */
void 
FMRIDataMapper::setIndivCroppingOffset(const bool croppingEnabled,
                                       const int x, const int y, const int z)
{
   indivCroppingEnabled = croppingEnabled;
   indivCroppingOffsetX = x;
   indivCroppingOffsetY = y;
   indivCroppingOffsetZ = z;
}

/**
 * Get the volume's type.
 */
FMRIDataMapper::VOLUME_TYPE 
FMRIDataMapper::getVolumeType() const
{
   return volumeType;
}

/**
 * Find out if the volume type is a raw type.
 */
bool 
FMRIDataMapper::getVolumeTypeIsRaw() const
{
   bool rawTypeSelected = false;
   
   switch(getVolumeType()) {
      case VOLUME_TYPE_RAW_8_BIT_SIGNED:
      case VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_16_BIT_SIGNED:
      case VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_32_BIT_SIGNED:
      case VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_64_BIT_SIGNED:
      case VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
      case VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
      case VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
         rawTypeSelected = true;
         break;
      case VOLUME_TYPE_AFNI:
      case VOLUME_TYPE_ANALYZE:
      case VOLUME_TYPE_SPM_LPI:
      case VOLUME_TYPE_SPM_RPI:
      case VOLUME_TYPE_WUNIL:
         rawTypeSelected = false;
         break;
   }
   
   return rawTypeSelected;
}

/**
 * Set the volume type.
 */
void 
FMRIDataMapper::setVolumeType(const FMRIDataMapper::VOLUME_TYPE vt)
{
   volumeType = vt;
}

/**
 * Get the individual's padding values.
 */
void 
FMRIDataMapper::getIndivPadding(bool& paddingEnabled, 
                                int& negX, int& negY, int& negZ,
                                int& posX, int& posY, int& posZ) const
{
   paddingEnabled = indivPaddingEnabled;
   negX = indivPaddingNegX;
   negY = indivPaddingNegY;
   negZ = indivPaddingNegZ;
   posX = indivPaddingPosX;
   posY = indivPaddingPosY;
   posZ = indivPaddingPosZ;
}

/**
 * Set the individual's padding values.
 */
void 
FMRIDataMapper::setIndivPadding(const bool paddingEnabled,
                                const int negX, const int negY, const int negZ,
                                const int posX, const int posY, const int posZ)
{
   indivPaddingEnabled = paddingEnabled;
   indivPaddingNegX = negX;
   indivPaddingNegY = negY;
   indivPaddingNegZ = negZ;
   indivPaddingPosX = posX;
   indivPaddingPosY = posY;
   indivPaddingPosZ = posZ;
}

/**
 * Apply a transformation to the individual's surface.
 */
vtkPolyData*
FMRIDataMapper::applyTransformToIndividual(vtkPolyData* polyData)
{
   vtkPolyData* output = polyData;
   
   //
   // Translate to account for cropping
   //
   if (indivCroppingEnabled) {
      vtkTransform* transform = vtkTransform::New();
      transform->Translate(static_cast<double>(indivCroppingOffsetX),
                           static_cast<double>(indivCroppingOffsetY),
                           static_cast<double>(indivCroppingOffsetZ));

      vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
      filter->SetInput(polyData);
      filter->SetTransform(transform);
      filter->Update();
      output = filter->GetOutput();
   }
   
   return output;
}

/**
 * Mask the padded node's so functional data not mapped to them.
 */
void
FMRIDataMapper::maskPaddedNodes(vtkPolyData* polyData) 
{
   const int numPoints = polyData->GetNumberOfPoints();
   paddedNodeFlag = new bool[numPoints];
   for (int j = 0; j < numPoints; j++) {
      paddedNodeFlag[j] = false;
   }
   
   //
   // Only do padding for individual surfaces with padding enabled
   //
   if (mappingSurfaceType != MAPPING_SURFACE_INDIVIDUAL) {
      return;
   }
   if (indivPaddingEnabled == false) {
      return;
   }
   
   //
   // Get bounds of surface
   //
   float bounds[6];
   polyData->GetBounds(bounds);
   
   //
   // Determine padding around surface
   //
   float xMinPad = -std::numeric_limits<float>::max();
   if (indivPaddingNegX > 0) {
      xMinPad = bounds[0] + indivPaddingNegX;
   }
   
   float xMaxPad = std::numeric_limits<float>::max();;
   if (indivPaddingPosX > 0) {
      xMaxPad = bounds[1] - indivPaddingPosX;
   }
   
   float yMinPad = -std::numeric_limits<float>::max();;
   if (indivPaddingNegY > 0) {
      yMinPad = bounds[2] + indivPaddingNegY;
   }
   
   float yMaxPad = std::numeric_limits<float>::max();;
   if (indivPaddingPosY > 0) {
      yMaxPad = bounds[3] - indivPaddingPosY;
   }
   
   float zMinPad = -std::numeric_limits<float>::max();;
   if (indivPaddingNegZ > 0) {
      zMinPad = bounds[4] + indivPaddingNegZ;
   }
   
   float zMaxPad = std::numeric_limits<float>::max();;
   if (indivPaddingPosZ > 0) {
      zMaxPad = bounds[5] - indivPaddingPosZ;
   }
   
   //
   // Mask all nodes that are in the padded area
   //
   int cnt = 0;
   for (int i = 0; i < numPoints; i++) {      
      float p[3];
      polyData->GetPoint(i, p);
      
      if (p[0] < xMinPad) {
         paddedNodeFlag[i] = true;
      }
      if (p[0] > xMaxPad) {
         paddedNodeFlag[i] = true;
      }
      if (p[1] < yMinPad) {
         paddedNodeFlag[i] = true;
      }
      if (p[1] > yMaxPad) {
         paddedNodeFlag[i] = true;
      }
      if (p[2] < zMinPad) {
         paddedNodeFlag[i] = true;
      }
      if (p[2] > zMaxPad) {
         paddedNodeFlag[i] = true;
      }
      
      if (paddedNodeFlag[i]) {
         cnt++;
      }
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << cnt << " of " << numPoints 
                << " nodes are in the padded region" << std::endl;
   }
}

/**
 * Show progress information.  Returns true if execution should be aborted.
 */
bool
FMRIDataMapper::showProgressInformation(const int surfaceNum, const int volumeNum)
{
   int totalNumberOfVolumes = 0;
   for (int i = 0; i < getNumberOfVolumes(); i++) {
      totalNumberOfVolumes += getVolume(i)->getNumberOfSubVolumes();
   }
   int totalSteps = getNumberOfSurfaces() * totalNumberOfVolumes;
   int step = surfaceNum * getNumberOfVolumes() + volumeNum;
   if ((surfaceNum < 0) || (volumeNum < 0)) {
      step = totalSteps;
   }
   return displayProgressMessage(step, totalSteps);
}

/**
 * Map volume data to the surfaces.
 */
bool
FMRIDataMapper::execute()
{ 
   bool errorFlag = false;
   
   for (unsigned int is = 0; is < surfaces.size(); is++) {
   
      const std::string surfaceFileName = surfaces[is].getSurfaceFileName();
      
      std::cout << "Processing surface: " << FileUtilities::basename(surfaceFileName) << std::ends;
      if (showProgressInformation(is, 0)) {
         return true;
      }
      
      //
      // Read in the polygon file (just need the points)
      //
      vtkPolyDataReader* polyReader = vtkPolyDataReader::New();
      polyReader->SetFileName((char*)surfaceFileName.c_str());
      
      polyReader->Update();
      vtkPolyData* polyData = polyReader->GetOutput();
      
      //
      // Check to see if this is an individual surface that has padding.
      // Padding is typically used when only a single lobe is segmented
      // from the brain.
      //
      maskPaddedNodes(polyData);
      
      //
      // if mapping to an individual, transform surface to match volume
      //
      if (mappingSurfaceType == MAPPING_SURFACE_INDIVIDUAL) {
         polyData = applyTransformToIndividual(polyData);
      }
      
      //
      // Compute normals for the surface
      //
      vtkPolyDataNormals* polyNormals = vtkPolyDataNormals::New();
      polyNormals->SetSplitting(0);
      polyNormals->SetConsistency(0);
      polyNormals->SetComputePointNormals(1);
      polyNormals->SetComputeCellNormals(0);
      polyNormals->SetFlipNormals(0);
      polyNormals->SetInput(polyData);
      polyNormals->Update();
      
      vtkPolyData* polygons = polyNormals->GetOutput();
      if (polygons->GetNumberOfPoints() <= 0) {
         std::string msg("MAPPER ERROR: reading polygon data from "); 
         msg.append(surfaceFileName);
         processErrorMessage(msg);
         errorFlag = true;
         continue;
      }

      //
      // convert triangle strips to triangles if necessary
      //
      vtkTriangleFilter *triangleFilter = NULL;
      if (polygons->GetNumberOfStrips() > 0) {
         triangleFilter = vtkTriangleFilter::New();
         triangleFilter->SetInput(polygons);
         triangleFilter->Update();
         polygons = triangleFilter->GetOutput();
      }
      
//       //
//       // Write the surfaces used for mapping data
//       //
//       string outputName("surface_for_mapping_");
//       char numstr[256];
//       sprintf(numstr, "%d", is);
//       outputName.append(numstr);
//       outputName.append(".vtk");
//       vtkPolyDataWriter* polyWriter = vtkPolyDataWriter::New();
//       polyWriter->SetInput(polygons);
//       polyWriter->SetFileName(outputName.c_str());
//       polyWriter->Write();
//       polyWriter->Delete();
//       polyWriter = NULL;
      
      const int numPoints = polygons->GetNumberOfPoints();
      std::cout << "Number of points: " << numPoints << std::endl;

      if (numPoints <= 0) {
         std::string msg("MAPPER ERROR: reading polygon data from "); 
         msg.append(surfaceFileName);
         processErrorMessage(msg);
         errorFlag = true;
         continue;
      }

      TopologyHelper pointNeighbors(polygons, false, true, true);
      if (pointNeighbors.getNodeInfoValid() == false) {
         std::string msg("MAPPER ERROR: neighbors are invalid for "); 
         msg.append(surfaceFileName);
         processErrorMessage(msg);
         errorFlag = true;
         continue;
      }
      
      if (errorFlag) {
         showProgressInformation(-1, -1);
         return true;
      }
      
      //
      // Setup the metric file
      //
      MetricFile metricFile;
      metricFile.setNumberOfNodesAndColumns(polygons->GetNumberOfPoints(),
                              surfaces[is].getNumberOfMetricColumnNames());
      //for (int k = 0; k < (int)volumes.size(); k++) {
      for (int k = 0; k < surfaces[is].getNumberOfMetricColumnNames(); k++) {
         metricFile.setColumnName(k, surfaces[is].getMetricColumnName(k));
         metricFile.setColumnComment(k, surfaces[is].getMetricColumnComments(k));
      }
      metricFile.setFileTitle(surfaces[is].getMetricTitle());
       
      //for (unsigned int iv = 0; iv < volumeNames.size(); iv++) {
      for (int volIndex = 0; 
           volIndex < surfaces[is].getNumberOfMetricColumnNames(); volIndex++) {
           
         if (showProgressInformation(is, volIndex)) {
            return true;
         }
         
         const int iv = surfaces[is].getMetricColumnVolumeIndex(volIndex);
         FMRIVolumeInfo* volInfo = getVolume(iv);
         
         const int subVolumeIndex = 
                        surfaces[is].getMetricColumnSubVolumeIndex(volIndex);
         std::cout << "------------ processing volume: " 
                   << FileUtilities::basename(volInfo->getVolumeName())
                   << " subvolume: " << subVolumeIndex << std::endl;
         //
         // Setup for reading in the volume
         //
         VolumeFile volumeReader;
         
         //
         // read the volume
         //
         try {
            if (volumeType == VOLUME_TYPE_AFNI) {
               //afniReader->SetVolumeToRead(subVolumeIndex);
               volumeReader.readVolumeHeader(volInfo->getVolumeName(), 
                                             VolumeFile::FILE_READ_WRITE_TYPE_AFNI);
               //
               // This code that sets "subVolumeIndex" appears about 90 lines up.  However,
               // on the Sun workstation with gcc "subVolumeIndex" is getting trashed, so,
               // we reset it here, so the program will work correctly.
               //
               const int subVolumeIndex = 
                           surfaces[is].getMetricColumnSubVolumeIndex(volIndex);
               volumeReader.readVolumeData(subVolumeIndex);
            }
            else if (volumeType == VOLUME_TYPE_ANALYZE) {
               volumeReader.readVolumeHeader(volInfo->getVolumeName(),
                                             VolumeFile::FILE_READ_WRITE_TYPE_ANALYZE);
               //
               // This code that sets "subVolumeIndex" appears about 90 lines up.  However,
               // on the Sun workstation with gcc "subVolumeIndex" is getting trashed, so,
               // we reset it here, so the program will work correctly.
               //
               const int subVolumeIndex = 
                           surfaces[is].getMetricColumnSubVolumeIndex(volIndex);
               volumeReader.readVolumeData(subVolumeIndex);
            }
            else if ((volumeType == VOLUME_TYPE_SPM_LPI) ||
                     (volumeType == VOLUME_TYPE_SPM_RPI)) {
               volumeReader.readVolumeHeader(volInfo->getVolumeName(),
                                             VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX);
               //
               // This code that sets "subVolumeIndex" appears about 90 lines up.  However,
               // on the Sun workstation with gcc "subVolumeIndex" is getting trashed, so,
               // we reset it here, so the program will work correctly.
               //
               const int subVolumeIndex = 
                           surfaces[is].getMetricColumnSubVolumeIndex(volIndex);
               volumeReader.readVolumeData(subVolumeIndex);
            }
            else if (volumeType == VOLUME_TYPE_WUNIL) {
               volumeReader.readVolumeHeader(volInfo->getVolumeName(),
                                             VolumeFile::FILE_READ_WRITE_TYPE_WUNIL);
               //
               // This code that sets "subVolumeIndex" appears about 90 lines up.  However,
               // on the Sun workstation with gcc "subVolumeIndex" is getting trashed, so,
               // we reset it here, so the program will work correctly.
               //
               const int subVolumeIndex = 
                           surfaces[is].getMetricColumnSubVolumeIndex(volIndex);
               volumeReader.readVolumeData(subVolumeIndex);
            }
            else {   // Raw file
               volumeReader.clear();
               const int dim[3] = { rawVolumeDimensionX, 
                                    rawVolumeDimensionY,
                                    rawVolumeDimensionZ };
               const float spacing[3] = { 1.0, 1.0, 1.0 };
               const float origin[3] = { 0.0, 0.0, 0.0 };
   
               VolumeFile::VOXEL_DATA_TYPE vdt = VolumeFile::VOXEL_DATA_TYPE_CHAR;
               switch(volumeType) {
                  case VOLUME_TYPE_RAW_8_BIT_SIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_CHAR;
                     break;
                  case VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_CHAR_UNSIGNED;
                     break;
                  case VOLUME_TYPE_RAW_16_BIT_SIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_SHORT;
                     break;
                  case VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_SHORT_UNSIGNED;
                     break;
                  case VOLUME_TYPE_RAW_32_BIT_SIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_INT;
                     break;
                  case VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_INT_UNSIGNED;
                     break;
                  case VOLUME_TYPE_RAW_64_BIT_SIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_LONG;
                     break;
                  case VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_LONG_UNSIGNED;
                     break;
                  case VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_FLOAT;
                     break;
                  case VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
                     vdt = VolumeFile::VOXEL_DATA_TYPE_DOUBLE;
                     break;
                  case VOLUME_TYPE_AFNI:
                  case VOLUME_TYPE_ANALYZE:
                  case VOLUME_TYPE_SPM_LPI:
                  case VOLUME_TYPE_SPM_RPI:
                  case VOLUME_TYPE_WUNIL:
                     break;
               }
               
               VolumeFile::ORIENTATION orient[3] = {
                  VolumeFile::ORIENTATION_UNKNOWN,
                  VolumeFile::ORIENTATION_UNKNOWN,
                  VolumeFile::ORIENTATION_UNKNOWN
               };
               
               //
               // This code that sets "subVolumeIndex" appears about 90 lines up.  However,
               // on the Sun workstation with gcc "subVolumeIndex" is getting trashed, so,
               // we reset it here, so the program will work correctly.
               //
               const int subVolumeIndex = 
                           surfaces[is].getMetricColumnSubVolumeIndex(volIndex);
                           
               //
               // Read the raw volume file
               //
               volumeReader.readFileVolumeRaw(volInfo->getVolumeName(),
                                              subVolumeIndex,
                                              vdt,
                                              dim,
                                              orient,
                                              origin,
                                              spacing,
                                              volumeByteSwapping);
            }
         
         }
         catch (FileIOException& e) {
            processErrorMessage(e.what());
            showProgressInformation(-1, -1);
            return true;
         }

         //
         // volume orientation settings (do not override from preferences file)
         //
         X_NEGATIVE_ORIENTATION_TYPE currentVolumeOrientationX = 
                                                            volumeOrientationX;
         Y_NEGATIVE_ORIENTATION_TYPE currentVolumeOrientationY = 
                                                            volumeOrientationY;
         Z_NEGATIVE_ORIENTATION_TYPE currentVolumeOrientationZ = 
                                                            volumeOrientationZ;
         
         if (volumeType == VOLUME_TYPE_AFNI) {
            VolumeFile::ORIENTATION orientation[3];
            volumeReader.getOrientation(orientation);
            if (orientation[0] == 
                VolumeFile::ORIENTATION_RIGHT_TO_LEFT) {
               currentVolumeOrientationX = X_NEGATIVE_ORIENTATION_RIGHT;
            }
            else {
               currentVolumeOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
            }
            if (orientation[1] == 
                VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR) {
               currentVolumeOrientationY = Y_NEGATIVE_ORIENTATION_ANTERIOR;
            }
            else {
               currentVolumeOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
            }
            if (orientation[2] == 
                VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR) {
               currentVolumeOrientationZ = Z_NEGATIVE_ORIENTATION_SUPERIOR;
            }
            else {
               currentVolumeOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
            }
         }
         else if (volumeType == VOLUME_TYPE_SPM_LPI) {
            //
            // SPM volumes in LPI
            //
            currentVolumeOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
            currentVolumeOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
            currentVolumeOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
         }
         else if (volumeType == VOLUME_TYPE_SPM_RPI) {
            //
            // SPM volumes in RPI
            //
            currentVolumeOrientationX = X_NEGATIVE_ORIENTATION_RIGHT;
            currentVolumeOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
            currentVolumeOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
         }
         else if (volumeType == VOLUME_TYPE_WUNIL) {
            VolumeFile::ORIENTATION orientation[3];
            volumeReader.getOrientation(orientation);
            if (orientation[0] == 
                VolumeFile::ORIENTATION_RIGHT_TO_LEFT) {
               currentVolumeOrientationX = X_NEGATIVE_ORIENTATION_RIGHT;
            }
            else {
               currentVolumeOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
            }
            if (orientation[1] == 
                VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR) {
               currentVolumeOrientationY = Y_NEGATIVE_ORIENTATION_ANTERIOR;
            }
            else {
               currentVolumeOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
            }
            if (orientation[2] == 
                VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR) {
               currentVolumeOrientationZ = Z_NEGATIVE_ORIENTATION_SUPERIOR;
            }
            else {
               currentVolumeOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
            }
         }
            
         //
         // Make sure that the volume was read successfully
         //
         vtkStructuredPoints* sp = volumeReader.getVolumeData();
         vtkPointData* pd = sp->GetPointData();
         vtkDataArray* scalarsOrig = pd->GetScalars();
         if (scalarsOrig == NULL) {
            std::string msg("MAPPER ERROR: reading volume data from "); 
            //msg.append(volumeNames[iv]);
            msg.append(volInfo->getVolumeName());
            processErrorMessage(msg);
            
            std::string columnName("ERROR - ");
            //columnName.append(volumeNames[iv]);
            columnName.append(volInfo->getVolumeName());
            metricFile.setColumnName(volIndex, columnName);
            errorFlag = true;
            continue;
         }
         
         const int numScalars = scalarsOrig->GetNumberOfTuples();
         if (DebugControl::getDebugOn()) {
            std::cout << "Number of scalars: " << numScalars << std::endl;
         }
         
         if (numScalars <= 0) {
            std::string msg("ERROR: reading volume data from "); 
            //msg.append(volumeNames[iv]);
            msg.append(volInfo->getVolumeName());
            processErrorMessage(msg);
            std::string columnName("ERROR - ");
            columnName.append(volInfo->getVolumeName());
            //columnName.append(volumeNames[iv]);
            metricFile.setColumnName(volIndex, columnName);
            errorFlag = true;
            continue;
         }
         
         //
         // Atlas Surfaces are always LPI
         //
         X_NEGATIVE_ORIENTATION_TYPE surfaceOrientX = 
                                              X_NEGATIVE_ORIENTATION_LEFT;
         Y_NEGATIVE_ORIENTATION_TYPE surfaceOrientY = 
                                              Y_NEGATIVE_ORIENTATION_POSTERIOR;
         Z_NEGATIVE_ORIENTATION_TYPE surfaceOrientZ = 
                                              Z_NEGATIVE_ORIENTATION_INFERIOR;
         
         if (mappingSurfaceType == MAPPING_SURFACE_INDIVIDUAL) {
            surfaceOrientX = indivSurfaceOrientationX;
            surfaceOrientY = indivSurfaceOrientationY;
            surfaceOrientZ = indivSurfaceOrientationZ;
         }

         //
         // Perform any necessary flipping
         //
         performFlipping(sp, 
                         currentVolumeOrientationX,
                         currentVolumeOrientationY,
                         currentVolumeOrientationZ,
                         surfaceOrientX,
                         surfaceOrientY,
                         surfaceOrientZ);

         vtkFloatArray* scalars = vtkFloatArray::New();
         //scalars->SetNumberOfValues(numScalars);
         for (int ii = 0; ii < numScalars; ii++) {
            scalars->InsertNextValue(scalarsOrig->GetComponent(ii, 0));
         }

         //
         // Map the volume data to the surface points
         //
         mapVolumeToSurface(sp, scalars, polygons, pointNeighbors,
                            metricFile, volIndex);  //iv);
         
         //
         // Append to the column comment mapping information
         //
         std::string comment = metricFile.getColumnComment(volIndex);
         comment.append("\n");
         comment.append("Mapped to surface: ");
         comment.append(surfaceFileName);
         comment.append("\n");
         comment.append("Mapped from volume: ");
         //comment.append(volumeNames[iv]);
         comment.append(volInfo->getVolumeName());
         comment.append(", subvolume: ");
         char strsubvol[32];
         sprintf(strsubvol, "%d", subVolumeIndex);
         comment.append(strsubvol);
         metricFile.setColumnComment(volIndex, comment);
         
//          //set all non-zero voxels to 255 to see fMRI coverage
//          for (int j = 0; j < numScalars; j++) {
//             if (scalars->GetValue(j) != 0.0) {
//                scalars->SetValue(j, 255.0);
//             }
//          }         
//          vtkMincVolumeWriter* minWriter = vtkMincVolumeWriter::New();
//          minWriter->SetInput(volumeReader->GetOutput());
//          minWriter->SetFileName("output.mnc");
//          minWriter->Write();
//          minWriter->Delete();

         scalars->Delete();
      }
      
      //
      // Write the metric file to the same directory as the spec file.  If
      // there is no spec file use whatever path is with the metric file.
      //
      std::string metricName(surfaces[is].getMetricFileName());
      const std::string specFilePath(FileUtilities::dirname(surfaces[is].getSpecFileName()));
      if (specFilePath.empty() == false) {
         std::string tempName(FileUtilities::basename(metricName));
         metricName.assign(specFilePath);
         metricName.append("/");
         metricName.append(tempName);
      }
      if (DebugControl::getDebugOn()) {
         std::cout << "Writing metric file: " << metricName << std::endl;
      }
      
      try {
         metricFile.writeFile(metricName);
      }
      catch (FileIOException& e) {
         std::string msg("Error writing metric file: ");
         msg.append(metricName);
         msg.append(": ");
         msg.append(e.what());
         processErrorMessage(msg);
         errorFlag = true;
      }

      if (errorFlag == false) {
         //
         // Update the spec file with the metric file
         //      
         const std::string specFileName(surfaces[is].getSpecFileName());
         if (specFileName.length() > 0) {
            SpecFile sf;
            try {
               sf.readFile(specFileName);
            }
            catch (FileIOException& e) {
               std::string msg("Error updating spec file: ");
               msg.append(specFileName);
               msg.append(": ");
               msg.append(e.what());
               processErrorMessage(msg);
               errorFlag = true;
            }
            
            sf.addToSpecFile(SpecFile::metricFileTag,
                             FileUtilities::basename(surfaces[is].getMetricFileName()), "", false);
            try {
               sf.writeFile(specFileName);
            }
            catch (FileIOException& e) {
               std::string msg("Error writing spec file: ");
               msg.append(specFileName);
               msg.append(e.what());
               processErrorMessage(msg);
            }
         }
      }
      
      polyReader->Delete();
      
      if (triangleFilter != NULL) {
         triangleFilter->Delete();
      }
      
      delete[] paddedNodeFlag;
   }
   
   showProgressInformation(-1, -1);
   
   return errorFlag;
}

/**
 * Flip the volume about the desired axis using the vtkImageFlip class.
 */
void
FMRIDataMapper::performFlipping(vtkStructuredPoints* sp,
                               const X_NEGATIVE_ORIENTATION_TYPE volumeOrientX,
                               const Y_NEGATIVE_ORIENTATION_TYPE volumeOrientY,
                               const Z_NEGATIVE_ORIENTATION_TYPE volumeOrientZ,
                               const X_NEGATIVE_ORIENTATION_TYPE surfaceOrientX,
                               const Y_NEGATIVE_ORIENTATION_TYPE surfaceOrientY,
                               const Z_NEGATIVE_ORIENTATION_TYPE surfaceOrientZ)
{
   vtkPointData* pd = sp->GetPointData();
   vtkDataArray* scalars = pd->GetScalars();
   const int numScalars = scalars->GetNumberOfTuples();

   //
   // Note: Number of threads is set to 1 for vtkImageFlip.   If this is not
   // done it may not run properly on some Sun workstations.
   //
   if (volumeOrientX != surfaceOrientX) {
      vtkImageFlip* flip = vtkImageFlip::New();
      flip->SetNumberOfThreads(1);
      flip->SetInput(sp);
      flip->SetFilteredAxis(0);
      flip->Update();
      vtkImageData* flipOut = flip->GetOutput();
      vtkPointData* fpd = flipOut->GetPointData();
      vtkDataArray* flipScalars = fpd->GetScalars();
      
      for (int i = 0; i < numScalars; i++) {
         scalars->SetComponent(i, 0, flipScalars->GetComponent(i, 0));
      }
      flip->Delete();
   }
   
   if (volumeOrientY != surfaceOrientY) {
      vtkImageFlip* flip = vtkImageFlip::New();
      flip->SetNumberOfThreads(1);
      flip->SetInput(sp);
      flip->SetFilteredAxis(1);
      flip->Update();
      vtkImageData* flipOut = flip->GetOutput();
      vtkPointData* fpd = flipOut->GetPointData();
      vtkDataArray* flipScalars = fpd->GetScalars();
      
      for (int i = 0; i < numScalars; i++) {
         scalars->SetComponent(i, 0, flipScalars->GetComponent(i, 0));
      }
      flip->Delete();
   }
   
   if (volumeOrientZ != surfaceOrientZ) {
      vtkImageFlip* flip = vtkImageFlip::New();
      flip->SetNumberOfThreads(1);
      flip->SetInput(sp);
      flip->SetFilteredAxis(2);
      flip->Update();
      vtkImageData* flipOut = flip->GetOutput();
      vtkPointData* fpd = flipOut->GetPointData();
      vtkDataArray* flipScalars = fpd->GetScalars();
      
      for (int i = 0; i < numScalars; i++) {
         scalars->SetComponent(i, 0, flipScalars->GetComponent(i, 0));
      }
      flip->Delete();
   }
}

/**
 * Map the data from the volume to the surface storing the results in
 * the proper column of the metric file.
 */
void 
FMRIDataMapper::mapVolumeToSurface(vtkStructuredPoints* volume,
                   vtkFloatArray* scalars,
                   vtkPolyData* surface,
                   TopologyHelper& pointNeighbors,
                   MetricFile& metricFile,
                   const int columnNumber)
{
   //
   // Metric value for masked nodes is zero.
   //
   const int numPoints = surface->GetNumberOfPoints();
   for (int i = 0; i < numPoints; i++) {
      if (paddedNodeFlag[i]) {
         metricFile.setValue(i, columnNumber, 0.0);
      }
   }
   
   switch(mappingAlgorithm) {
      case ALGORITHM_AVERAGE_NODES:
         mapVolumeToSurfaceAverageNodes(volume,
                                        scalars,
                                        surface,
                                        pointNeighbors,
                                        metricFile,
                                        columnNumber);
         break;
      case ALGORITHM_AVERAGE_VOXEL:
      case ALGORITHM_GAUSSIAN:
      case ALGORITHM_MAX_VOXEL:
         mapVolumeToSurfaceNeighborLayers(volume,
                                          scalars,
                                          surface,
                                          pointNeighbors,
                                          metricFile,
                                          columnNumber);
         break;
      case ALGORITHM_MCW_BRAINFISH:
         mapVolumeToSurfaceBrainfish(volume,
                                     scalars,
                                     surface,
                                     pointNeighbors,
                                     metricFile,
                                     columnNumber);
         break;
   }
} 
  
/**
 * Average Nodes Mapping Algorithm.
 */
void 
FMRIDataMapper::mapVolumeToSurfaceAverageNodes(
                                      vtkStructuredPoints* volume,
                                      vtkFloatArray* scalars,
                                      vtkPolyData* surface,
                                      TopologyHelper& pointNeighbors,
                                      MetricFile& metricFile,
                                      const int columnNumber)
{
   for (int i = 0; i < surface->GetNumberOfPoints(); i++) {
   
      //
      // Ignore nodes that are in the padded regions
      //
      if (paddedNodeFlag[i]) {
         continue;
      }
      
      float pos[3];
      surface->GetPoint(i, pos);
      
      //
      // Get this surface point's neighbors
      //
      std::vector<int> neighbors;
      pointNeighbors.getNodeNeighbors(i, neighbors);
      
      //
      // Given the surface points find the index into the volume
      //
      float pcoord[3];
      int   ijk[3];
      const int insideVolume = volume->ComputeStructuredCoordinates(pos, ijk, 
                                                                    pcoord);
                                                                    
      float metric = 0.0;
      float numContribute = 0.0;
      
      if (insideVolume) {
         //
         // Get the offset from the voxel storage for the voxel "ijk"
         //
         const int pointID = volume->ComputePointId(ijk);
         
//          printf("%d: pos(%.2f, %.2f, %.2f)  ijk(%d, %d, %d), pointID %d\n",
//                 i, pos[0], pos[1], pos[2],
//                 ijk[0], ijk[1], ijk[2], pointID);
            
             
               metric = scalars->GetValue(pointID);
               if (metric != 0.0) {
                  //printf("Node %d has metric data = %f\n", i, metric);
               }
               numContribute += 1.0;
               for (unsigned int j = 0; j < neighbors.size(); j++) {
                  float neighPos[3];
                  if (paddedNodeFlag[neighbors[j]]) {
                     continue;
                  }
                  surface->GetPoint(neighbors[j], neighPos);
                  const int neighborInsideVolume = 
                     volume->ComputeStructuredCoordinates(neighPos, ijk,pcoord);
                  if (neighborInsideVolume) {
                     const int neighborID = volume->ComputePointId(ijk);
                     metric += scalars->GetValue(neighborID);
                     numContribute += 1.0;
                  }
               }
      }
      
      if (numContribute > 0.0) {
         metric /= numContribute;
      }
      metricFile.setValue(i, columnNumber, metric);
   }
   
   std::string comment = metricFile.getColumnComment(columnNumber);
   if (comment.length() > 0) {
      comment.append("\n");
   }
   comment.append("Algorithm: Average Nodes");
   metricFile.setColumnComment(columnNumber, comment);
}

/**
 * MCW Brainfish Mapping Algorithm.
 */
void 
FMRIDataMapper::mapVolumeToSurfaceBrainfish(
                                      vtkStructuredPoints* volume,
                                      vtkFloatArray* scalars,
                                      vtkPolyData* surface,
                                      TopologyHelper& pointNeighbors,
                                      MetricFile& metricFile,
                                      const int columnNumber)
{
   int dim[3];
   volume->GetDimensions(dim);
   float bounds[6];
   surface->GetBounds(bounds);
   //printf("Bounds: %f %f %f %f %f %f\n", bounds[0], bounds[1],
   //       bounds[2], bounds[3], bounds[4], bounds[5]);

   //
   // Limit search of volume to voxels around surface's bounds
   //
   int xmin = static_cast<int>(bounds[0] - brainfishMaxDistance - 1);
   int xmax = static_cast<int>(bounds[1] + brainfishMaxDistance + 1);
   if (xmin < 0) xmin = 0;
   if (xmax >= dim[0]) xmax = dim[0] - 1;
   int ymin = static_cast<int>(bounds[2] - brainfishMaxDistance - 1);
   int ymax = static_cast<int>(bounds[3] + brainfishMaxDistance + 1);
   if (ymin < 0) ymin = 0;
   if (ymax >= dim[1]) ymax = dim[1] - 1;
   int zmin = static_cast<int>(bounds[4] - brainfishMaxDistance - 1);
   int zmax = static_cast<int>(bounds[5] + brainfishMaxDistance + 1);
   if (zmin < 0) zmin = 0;
   if (zmax >= dim[2]) zmax = dim[2] - 1;
   
   if (DebugControl::getDebugOn()) {
      printf("ROI: xmin=%d; xmax=%d; ymin=%d; ymax=%d; zmin=%d; zmax=%d\n",
             xmin, xmax, ymin, ymax, zmin, zmax);
   }
     
   float* activity = new float[surface->GetNumberOfPoints()];
   bool*  assigned = new bool[surface->GetNumberOfPoints()];
   bool*  assignedToo = new bool[surface->GetNumberOfPoints()];
   for (int m = 0; m < surface->GetNumberOfPoints(); m++) {
      assigned[m] = false;
      assignedToo[m] = false;
      activity[m] = 0.0;
   }
   
   //
   // loop through the voxels
   //
   for (int k = zmin; k < zmax; k++) {
      for (int j = ymin; j < ymax; j++) {
         for (int i = xmin; i < xmax; i++) {
            int ijk[3] = { i, j, k };
            
            //
            // one-dimensional voxel offset from beginning of volume
            //
            const int voxelOffset = volume->ComputePointId(ijk);
         
            //
            // voxel at (i, j, k)
            //
            const float voxel = scalars->GetValue(voxelOffset);
            //printf("voxel (%d,%d,%d) = %f\n", i, j, k, voxel);
            
            //
            // Only do voxels that have some activity
            //
            if (voxel != 0.0) {
               int nearestNode = -1;
               float minDistance = std::numeric_limits<float>::max();;

               //
               // Find the node that is nearest to the voxel
               //
               for (int m = 0; m < surface->GetNumberOfPoints(); m++) {
                  //
                  // Ignore nodes that are in the padded regions
                  //
                  if (paddedNodeFlag[m]) {
                     continue;
                  }
                  
                  float xyz[3];
                  surface->GetPoint(m, xyz);

                  if ((xyz[0] > (i - brainfishMaxDistance)) &&
                      (xyz[0] < (i + brainfishMaxDistance))) {
                     if ((xyz[1] > (j - brainfishMaxDistance)) &&
                         (xyz[1] < (j + brainfishMaxDistance))) {
                        if ((xyz[2] > (k - brainfishMaxDistance)) &&
                            (xyz[2] < (k + brainfishMaxDistance))) {
                           const float dist = ((xyz[0] - i) * (xyz[0] - i) +
                                               (xyz[1] - j) * (xyz[1] - j) +
                                               (xyz[2] - k) * (xyz[2] - k));
                           if (dist < minDistance) {
                              nearestNode = m;
                              minDistance = dist;
                           }
                           else if (nearestNode < 0) {
                              nearestNode = m;
                              minDistance = dist;
                           }
                        }
                     }
                  }
               }

               //
               // Allow positive activity to override negative activity
               // Negative only overrides "less negative" 
               //
               if (nearestNode >= 0) {
                  assigned[nearestNode] = true;
                  const float nodeValue = activity[nearestNode];
                  if (voxel > 0.0) {
                     if (voxel > nodeValue) {
                        activity[nearestNode] = voxel;
                     }
                  }
                  else if (nodeValue < 0.0) {
                     if (voxel < nodeValue) {
                        activity[nearestNode] = voxel;
                     }
                  }
               }
            }
         }
      }
   }
   
   //
   // Process the splat factor.  For each node that is assigned, check any
   // of its neighbors that are not assigned.  If the neighbor is not assigned,
   // assign the neighbor the average of its assigned neighbors.
   //
   if (brainfishSplatFactor > 0) {
      for (int m = 0; m < surface->GetNumberOfPoints(); m++) {
         assignedToo[m] = assigned[m];
      }
      
      //
      // Number of Splat levels
      //
      for (int k = 0; k < brainfishSplatFactor; k++) {
         for (int m = 0; m < surface->GetNumberOfPoints(); m++) {
            //
            // Is this node assigned ?
            //
            if (assigned[m]) {
            
               //
               // Get this node's neighbors
               //
               std::vector<int> neighbors;
               pointNeighbors.getNodeNeighbors(m, neighbors);
               for (unsigned int j = 0; j < neighbors.size(); j++) {
                  const int neighbor = neighbors[j];
                  
                  //
                  // Ignore nodes that are in the padded regions
                  //
                  if (paddedNodeFlag[j]) {
                     continue;
                  }
                  
                  //
                  // Is neighbor unassigned ?
                  //
                  if (assigned[neighbor] == false) {
                     float sum = 0.0;
                     int cnt = 0;
                     
                     //
                     // Get this neighbor's neighbors
                     //
                     std::vector<int> neighbors2;
                     pointNeighbors.getNodeNeighbors(neighbor, neighbors2);
                     for (unsigned int n = 0; n < neighbors2.size(); n++) {
                     
                        //
                        // Is the neighbor's neighbor assigned ?
                        //
                        const int neighbor2 = neighbors2[n];
                        if (assigned[neighbor2]) {
                           sum += activity[neighbor2];
                           cnt++;
                        }
                     }
                     
                     //
                     // Set this neighbors activity, if any from its neighbors
                     //
                     if (cnt > 0) {
                        activity[neighbor] = sum/cnt;
                        assignedToo[neighbor] = true;
                     }
                  }
               }
            }
         }
         for (int mm = 0; mm < surface->GetNumberOfPoints(); mm++) {
            assigned[mm] = assignedToo[mm];
         }
      }
   }
   
   //
   // Update the metric file with the activity
   //
   for (int mm = 0; mm < surface->GetNumberOfPoints(); mm++) {
      metricFile.setValue(mm, columnNumber, activity[mm]);
   }
   
   std::string comment = metricFile.getColumnComment(columnNumber);
   comment.append("\n");
   comment.append("Algorithm: Brainfish");
   comment.append("\n");
   
   std::ostringstream str;
   str << "Parameters: Max Dist=" << brainfishMaxDistance
       << ", Splat Factor=" << brainfishSplatFactor << std::ends;
   comment.append(str.str());
   metricFile.setColumnComment(columnNumber, comment);
   
   delete[] assigned;
   delete[] assignedToo;
   delete[] activity;
}

/**
 * Mapping Algorithm.
 */
void 
FMRIDataMapper::mapVolumeToSurfaceNeighborLayers(
                                      vtkStructuredPoints* volume,
                                      vtkFloatArray* scalars,
                                      vtkPolyData* surface,
                                      TopologyHelper& pointNeighbors,
                                      MetricFile& metricFile,
                                      const int columnNumber)
{
   vtkPointData* pd = surface->GetPointData();
   vtkDataArray* normals = pd->GetNormals();
   float pointNormal[3];
   
   float origin[3], spacing[3];
   volume->GetOrigin(origin);
   volume->GetSpacing(spacing);
   
   for (int i = 0; i < surface->GetNumberOfPoints(); i++) {
      //
      // Ignore nodes that are in the padded regions
      //
      if (paddedNodeFlag[i]) {
         continue;
      }
      float pos[3];
      surface->GetPoint(i, pos);
      normals->GetTuple(i, pointNormal);
         
      //
      // Get this surface point's neighbors
      //
      std::vector<int> neighbors;
      pointNeighbors.getNodeNeighbors(i, neighbors);
      
      //
      // Given the surface points find the index into the volume
      //
      float pcoord[3];
      int   ijk[3];
      const int insideVolume = volume->ComputeStructuredCoordinates(pos, ijk, 
                                                                    pcoord);
                                                                    
      float metric = 0.0;
      
      if (insideVolume) {
      
         if (mappingAlgorithm == ALGORITHM_MAX_VOXEL) {
            metric = -std::numeric_limits<float>::max();
         }
         
         int voxelMinX = 0, voxelMaxX = 0;
         int voxelMinY = 0, voxelMaxY = 0;
         int voxelMinZ = 0, voxelMaxZ = 0;
         getNeighborsSubVolume(volume, ijk, 
                               voxelMinX, voxelMaxX,
                               voxelMinY, voxelMaxY,
                               voxelMinZ, voxelMaxZ);
                              
         float voxelSum = 0.0;
         float weightSum = 0.0;
         
         for (int ix = voxelMinX; ix <= voxelMaxX; ix++) {
            for (int iy = voxelMinY; iy <= voxelMaxY; iy++) {
               for (int iz = voxelMinZ; iz <= voxelMaxZ; iz++) {
                  ijk[0] = ix;
                  ijk[1] = iy;
                  ijk[2] = iz;
                  
                  //
                  // Get the offset from the voxel storage for the voxel "ijk"
                  //
                  const int pointID = volume->ComputePointId(ijk);
                  
                  switch(mappingAlgorithm) {
                     case ALGORITHM_AVERAGE_NODES:
                     case ALGORITHM_MCW_BRAINFISH:
                        // performed in another method
                        break;
                     case ALGORITHM_GAUSSIAN:
                        {
                           const float distance[3] = {
                              (ix*spacing[0] + origin[0]) - pos[0],
                              (iy*spacing[1] + origin[1]) - pos[1],
                              (iz*spacing[2] + origin[2]) - pos[2]
                           };
                           gaussian(pointNormal,
                                    scalars->GetValue(pointID),
                                    distance,
                                    voxelSum,
                                    weightSum);
                        }
                        break;
                     case ALGORITHM_AVERAGE_VOXEL:
                        voxelSum += scalars->GetValue(pointID);
                        weightSum += 1.0;
                        break;
                     case ALGORITHM_MAX_VOXEL:
                        {
                           const float temp = scalars->GetValue(pointID);
                           if (temp > metric) {
                              metric = temp;
                           }
                        }
                        break;
                  }
               }
            }
         }
         
         switch (mappingAlgorithm) {
            case ALGORITHM_AVERAGE_NODES:
            case ALGORITHM_MAX_VOXEL:
            case ALGORITHM_MCW_BRAINFISH:
               break;
            case ALGORITHM_GAUSSIAN:
            case ALGORITHM_AVERAGE_VOXEL:
               if (weightSum != 0.0) {
                  metric = voxelSum / weightSum;
               }
               break;
         }
      }
      
      metricFile.setValue(i, columnNumber, metric);
   }
   std::string comment = metricFile.getColumnComment(columnNumber);
   if (comment.length() > 0) {
      comment.append("\n");
   }
   switch (mappingAlgorithm) {
      case ALGORITHM_MCW_BRAINFISH:
      case ALGORITHM_AVERAGE_NODES:
         break;
      case ALGORITHM_MAX_VOXEL:
         {
            comment.append("Algorithm: Max Voxel\n");
            std::ostringstream str;
            str << "Parameters: neighbors=" << neighborLayers;
            comment.append(str.str());
         }
         break;
      case ALGORITHM_GAUSSIAN:
         {
            comment.append("Algorithm: Gaussian\n");
            std::ostringstream str;
            str << "Parameters: sigma-norm=" << gaussianSigmaNorm
               << ", sigma-tang=" << gaussianSigmaTang
               << ", norm-below= " << gaussianNormBelowCutoff
               << ", norm-above= " << gaussianNormAboveCutoff
               << ", tan-cutoff=" << gaussianTangCutoff
               << ", neighbors=" << neighborLayers;
            comment.append(str.str());
         }
         break;
      case ALGORITHM_AVERAGE_VOXEL:
         {
            comment.append("Algorithm: Average Voxel\n");
            std::ostringstream str;
            str << "Parameters: neighbors=" << neighborLayers;
            comment.append(str.str());
         }
         break;
   }
   metricFile.setColumnComment(columnNumber, comment);
}

/**
 *  get subvolume indices centered around a voxel
 */
void 
FMRIDataMapper::gaussian(const float normal[3],
                         const float voxelValue,
                         const float distanceVector[3],
                         float& metricSum,
                         float& weightSum)
{
   const float dNorm = vtkMath::Dot((float*)normal, (float*)distanceVector);
   float Wnorm = 0.0;
   if ((dNorm > -gaussianNormBelowCutoff) && 
       (dNorm < gaussianNormAboveCutoff)) {
      Wnorm = exp (-(dNorm*dNorm)/(gaussianSigmaNorm*gaussianSigmaNorm));
   }
   
   if (Wnorm > 0.0)  {
      float Tang[3];
      for (int i = 0; i < 3; i++) {
         Tang[i] = distanceVector[i] - dNorm*normal[i];
      }
      const float dTang = sqrt(Tang[0]*Tang[0] + Tang[1]*Tang[1] + 
                               Tang[2]*Tang[2]);
      float Wtang = 0.0;
      if (dTang < gaussianTangCutoff) {
         Wtang = exp (-((dTang*dTang)
                       / (gaussianSigmaTang * gaussianSigmaTang)));
      }
      if (Wtang > 0.0){
	 const float weight = Wnorm * Wtang;
	 metricSum += voxelValue * weight;
	 weightSum += weight;
      }
   }
}



/**
 * get subvolume indices centered around a voxel
 */
void 
FMRIDataMapper::getNeighborsSubVolume(vtkStructuredPoints* volume, 
                                      const int ijk[3],
                                      int& voxelMinX, int& voxelMaxX,
                                      int& voxelMinY, int& voxelMaxY,
                                      int& voxelMinZ, int& voxelMaxZ)
{
   int dims[3];
   volume->GetDimensions(dims);
   
   voxelMinX = ijk[0] - neighborLayers;
   if (voxelMinX < 0) {
      voxelMinX = 0;
   }
   voxelMaxX = ijk[0] + neighborLayers;
   if (voxelMaxX >= dims[0]) {
      voxelMaxX = dims[0] - 1;
   }

   voxelMinY = ijk[1] - neighborLayers;
   if (voxelMinY < 0) {
      voxelMinY = 0;
   }
   voxelMaxY = ijk[1] + neighborLayers;
   if (voxelMaxY >= dims[1]) {
      voxelMaxY = dims[1] - 1;
   }

   voxelMinZ = ijk[2] - neighborLayers;
   if (voxelMinZ < 0) {
      voxelMinZ = 0;
   }
   voxelMaxZ = ijk[2] + neighborLayers;
   if (voxelMaxZ >= dims[2]) {
      voxelMaxZ = dims[2] - 1;
   }
}

/**
 * Create the file path and name for the user preferences file
 */
std::string
FMRIDataMapper::getUserPreferenceFileName() const
{
   std::string preferencesFileName(QDir::homeDirPath().latin1());
   if (preferencesFileName.empty() == false) {
      preferencesFileName.append("/");
   }
   preferencesFileName.append(".map_fmri_to_surface_preferences");
   return preferencesFileName;
}

/**
 * load user preferences 
 */
void 
FMRIDataMapper::loadUserPreferences()
{
   const std::string filename = getUserPreferenceFileName();
   if (filename.length() > 0) {
      QFile file(filename.c_str());
      if (file.open(IO_ReadOnly) == false) {
         std::cout << "INFO: Unable to open file " << filename 
                   << "for reading user preferences, possibly not created yet." << std::endl;
         return;
      }
      
      QTextStream stream(&file);
      while(stream.atEnd() == false) {
         QString qline = stream.readLine();
         if (qline.isNull()) {
            break;
         }
         
         //
         // Chop off any comment that begins with '#'
         //
         std::string line(qline.latin1());
         std::string::size_type commentCharPos = line.find('#');
         if (commentCharPos != std::string::npos) {
            line.resize(commentCharPos);
         }

         //
         // Split up the line along whitespace
         //
         std::vector<std::string> tokens;
         StringUtilities::token(line, " \t", tokens);
         const int numItems = static_cast<int>(tokens.size());
         
         if (numItems <= 0) {
            continue;
         }
         
         const std::string tag(tokens[0]);
         
         if (tag == preferencesRawDimensionTag) {
            if (numItems == 4) {
               rawVolumeDimensionX = atoi(tokens[1].c_str());
               rawVolumeDimensionY = atoi(tokens[2].c_str());
               rawVolumeDimensionZ = atoi(tokens[3].c_str());
            }
         }
         else if (preferencesIndivPaddingTag.compare(tag) == 0) {
            if (numItems == 8) {
               if (tokens[1].c_str() == preferencesValueYes) {
                  indivPaddingEnabled = true;
               }
               else {
                  indivPaddingEnabled = false;
               }
               indivPaddingNegX = atoi(tokens[2].c_str());
               indivPaddingNegY = atoi(tokens[3].c_str());
               indivPaddingNegZ = atoi(tokens[4].c_str());
               indivPaddingPosX = atoi(tokens[5].c_str());
               indivPaddingPosY = atoi(tokens[6].c_str());
               indivPaddingPosZ = atoi(tokens[7].c_str());
            }
         }
         else if (preferencesIndivCroppingOffsetTag.compare(tag) == 0) {
            if (numItems == 5) {
               if (tokens[1].c_str() == preferencesValueYes) {
                  indivCroppingEnabled = true;
               }
               else {
                  indivCroppingEnabled = false;
               }
               indivCroppingOffsetX = atoi(tokens[2].c_str());
               indivCroppingOffsetY = atoi(tokens[3].c_str());
               indivCroppingOffsetZ = atoi(tokens[4].c_str());
            }
         }
         else if (preferencesByteSwapTag.compare(tag) == 0) {
            if (numItems == 2) {
               if (preferencesValueYes.compare(tokens[1].c_str()) == 0) {
                  volumeByteSwapping = true;
               }
               else {
                  volumeByteSwapping = false;
               }
            }
         }
         else if (preferencesAlgorithmTag.compare(tag) == 0) {
            if (numItems == 2) {
               if (preferencesValueAlgorithmAverageNodes.compare(tokens[1].c_str()) == 0) {
                  mappingAlgorithm = ALGORITHM_AVERAGE_NODES;
               }
               else if (preferencesValueAlgorithmAverageVoxel.compare(tokens[1].c_str()) == 0) {
                  mappingAlgorithm = ALGORITHM_AVERAGE_VOXEL;
               }
               else if (preferencesValueAlgorithmGaussian.compare(tokens[1].c_str()) == 0) {
                  mappingAlgorithm = ALGORITHM_GAUSSIAN;
               }
               else if (preferencesValueAlgorithmMaximumVoxel.compare(tokens[1].c_str()) == 0) {
                  mappingAlgorithm = ALGORITHM_MAX_VOXEL;
               }
               else if (preferencesValueAlgorithmMCWBrainfish.compare(tokens[1].c_str()) == 0) {
                  mappingAlgorithm = ALGORITHM_MCW_BRAINFISH;
               }
               else {
                  std::cout << "WARNING: Unrecognized FMRI Mapper Algorithm: "
                            << tokens[1] << std::endl;
                  mappingAlgorithm = ALGORITHM_AVERAGE_VOXEL;
               }
            }
         }
         else if (preferencesAlgorithmNeighborsTag.compare(tag) == 0) {
            if (numItems == 2) {
               neighborLayers = atoi(tokens[1].c_str());
            }
         }
         else if (preferencesAlgorithmSigmaNormTag.compare(tag) == 0) {
            if (numItems == 2) {
               gaussianSigmaNorm = atof(tokens[1].c_str());
            }
         }
         else if (preferencesAlgorithmSigmaTangTag.compare(tag) == 0) {
            if (numItems == 2) {
               gaussianSigmaTang = atof(tokens[1].c_str());
            }
         }
         else if (preferencesAlgorithmNormBelowTag.compare(tag) == 0) {
            if (numItems == 2) {
               gaussianNormBelowCutoff = atof(tokens[1].c_str());
            }
         }
         else if (preferencesAlgorithmNormAboveTag.compare(tag) == 0) {
            if (numItems == 2) {
               gaussianNormAboveCutoff = atof(tokens[1].c_str());
            }
         }
         else if (preferencesAlgorithmTangTag.compare(tag) == 0) {
            if (numItems == 2) {
               gaussianTangCutoff = atof(tokens[1].c_str());
            }
         }
         else if (preferencesAlgorithmMaxDistanceTag.compare(tag) == 0) {
            if (numItems == 2) {
               brainfishMaxDistance = atof(tokens[1].c_str());
            }
         }
         else if (preferencesAlgorithmSplatFactorTag.compare(tag) == 0) {
            if (numItems == 2) {
               brainfishSplatFactor = atoi(tokens[1].c_str());
            }
         }
         else if (preferencesAtlasDirectoryTag.compare(tag) == 0) {
            if (numItems == 2) {
               std::string errorMessage;
               if (addAtlasDirectory(tokens[1].c_str(), true, errorMessage)) {
                  processErrorMessage(errorMessage);
               }
            }
         }
         else if (preferencesAtlasLastSelected.compare(tag) == 0) {
            if (numItems == 2) {
               lastSelectedAtlas.assign(StringUtilities::replace(tokens[1], '?', ' '));
            }
         }
         else if (preferencesVolumeTypeTag.compare(tag) == 0) {
            if (numItems == 2) {
               if (preferencesVolumeValueTypeAFNI.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_AFNI;
               }
               else if (preferencesVolumeValueTypeAnalyze.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_ANALYZE;
               }
               else if (preferencesVolumeValueTypeSPM_LPI.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_SPM_LPI;
               }
               else if (preferencesVolumeValueTypeSPM_RPI.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_SPM_RPI;
               }
               else if (preferencesVolumeValueTypeWUNIL.compare(tokens[1]) == 0) {
                  volumeType = VOLUME_TYPE_WUNIL;
               }
               else if (preferencesVolumeValueTypeSigned8.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_8_BIT_SIGNED;
               }
               else if (preferencesVolumeValueTypeUnsigned8.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_8_BIT_UNSIGNED;
               }
               else if (preferencesVolumeValueTypeSigned16.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_16_BIT_SIGNED;
               }
               else if (preferencesVolumeValueTypeUnsigned16.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_16_BIT_UNSIGNED;
               }
               else if (preferencesVolumeValueTypeSigned32.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_32_BIT_SIGNED;
               }
               else if (preferencesVolumeValueTypeUnsigned32.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_32_BIT_UNSIGNED;
               }
               else if (preferencesVolumeValueTypeSigned64.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_64_BIT_SIGNED;
               }
               else if (preferencesVolumeValueTypeUnsigned64.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_64_BIT_UNSIGNED;
               }
               else if (preferencesVolumeValueTypeFloat32.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT;
               }
               else if (preferencesVolumeValueTypeFloat64.compare(tokens[1].c_str()) == 0) {
                  volumeType = VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT;
               }
               else  {
                  volumeType = VOLUME_TYPE_AFNI;
                  std::cout << "WARNING: Unrecognized FMRI Mapper volume type: "
                            << tokens[1] << std::endl;
               }
            }
         }
         else if (preferencesSurfaceTypeTag.compare(tag) == 0) {
            if (numItems == 2) {
               if (preferencesSurfaceValueTypeVTK.compare(tokens[1].c_str()) == 0) {
                  surfaceFileType = SURFACE_FILE_TYPE_VTK_POLYDATA;
               }
               else {
                  surfaceFileType = SURFACE_FILE_TYPE_VTK_POLYDATA;
                  std::cout << "WARNING: Unrecognized FMRI Mapper surface type: "
                           << tokens[1] << std::endl;
               }
            }
         }
         else if (preferencesVolumeOrientationTag.compare(tag) == 0) {
            if (preferencesOrientationValueLeft.compare(tokens[1].c_str()) == 0) {
                volumeOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
            }
            else if (preferencesOrientationValueRight.compare(tokens[1].c_str()) == 0) {
                volumeOrientationX = X_NEGATIVE_ORIENTATION_RIGHT;
            }
            else {
                volumeOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
                std::cout << "WARNING: Unrecognized volume X orientation: "
                          << tokens[1] << std::endl;
            }
            if (preferencesOrientationValuePosterior.compare(tokens[2].c_str()) == 0) {
                volumeOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
            }
            else if (preferencesOrientationValueAnterior.compare(tokens[2].c_str()) == 0) {
                volumeOrientationY = Y_NEGATIVE_ORIENTATION_ANTERIOR;
            }
            else {
                volumeOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
                std::cout << "WARNING: Unrecognized volume Y orientation: "
                          << tokens[1] << std::endl;
            }
            if (preferencesOrientationValueInferior.compare(tokens[3].c_str()) == 0) {
                volumeOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
            }
            else if (preferencesOrientationValueSuperior.compare(tokens[3].c_str()) == 0) {
                volumeOrientationZ = Z_NEGATIVE_ORIENTATION_SUPERIOR;
            }
            else {
                volumeOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
                std::cout << "WARNING: Unrecognized volume Z orientation: "
                          << tokens[1] << std::endl;
            }
         }
         else if (preferencesIndivSurfaceOrientationTag.compare(tag) == 0) {
            if (preferencesOrientationValueLeft.compare(tokens[1].c_str()) == 0) {
                indivSurfaceOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
            }
            else if (preferencesOrientationValueRight.compare(tokens[1].c_str()) == 0) {
                indivSurfaceOrientationX = X_NEGATIVE_ORIENTATION_RIGHT;
            }
            else {
                indivSurfaceOrientationX = X_NEGATIVE_ORIENTATION_LEFT;
                std::cout << "WARNING: Unrecognized surface X orientation: "
                          << tokens[1] << std::endl;
            }
            if (preferencesOrientationValuePosterior.compare(tokens[2].c_str()) == 0) {
                indivSurfaceOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
            }
            else if (preferencesOrientationValueAnterior.compare(tokens[2].c_str()) == 0) {
                indivSurfaceOrientationY = Y_NEGATIVE_ORIENTATION_ANTERIOR;
            }
            else {
                indivSurfaceOrientationY = Y_NEGATIVE_ORIENTATION_POSTERIOR;
                std::cout << "WARNING: Unrecognized surface Y orientation: "
                          << tokens[1] << std::endl;
            }
            if (preferencesOrientationValueInferior.compare(tokens[3].c_str()) == 0) {
                indivSurfaceOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
            }
            else if (preferencesOrientationValueSuperior.compare(tokens[3].c_str()) == 0) {
                indivSurfaceOrientationZ = Z_NEGATIVE_ORIENTATION_SUPERIOR;
            }
            else {
                indivSurfaceOrientationZ = Z_NEGATIVE_ORIENTATION_INFERIOR;
                std::cout << "WARNING: Unrecognized surface Z orientation: "
                          << tokens[1] << std::endl;
            }
         }
         else {
            std::cout << "WARNING: Unrecognized FMRI Mapper preferences tag: "
                      << tag << std::endl;
         }
      }
      
      file.close();
   }
}

/**
 * save user preferences
 */
void 
FMRIDataMapper::saveUserPreferences() const
{
   const std::string filename = getUserPreferenceFileName();
   
   if (filename.length() > 0) {
      QFile file(filename.c_str());
      if (file.open(IO_WriteOnly) == false) {
         std::string msg("MAPPER ERROR: Unable to open file ");
         msg.append(filename);
         msg.append(" for writing user preferences");
         processErrorMessage(msg);
         return;
      }
      
      QTextStream stream(&file);
      
      stream << "# Preferences for map_fmri_to_surface program" << "\n";
      stream << "# Do not edit this file" << "\n";
      stream << "\n";
      
      stream << preferencesAlgorithmTag.c_str() << " ";
      switch(getMappingAlgorithm()) {
         case ALGORITHM_AVERAGE_NODES:
            stream << preferencesValueAlgorithmAverageNodes.c_str();
            break;
         case ALGORITHM_GAUSSIAN:
            stream << preferencesValueAlgorithmGaussian.c_str();
            break;
         case ALGORITHM_AVERAGE_VOXEL:
            stream << preferencesValueAlgorithmAverageVoxel.c_str();
            break;
         case ALGORITHM_MAX_VOXEL:
            stream << preferencesValueAlgorithmMaximumVoxel.c_str();
            break;
         case ALGORITHM_MCW_BRAINFISH:
            stream << preferencesValueAlgorithmMCWBrainfish.c_str();
            break;
         default:
            stream << preferencesValueAlgorithmAverageVoxel.c_str();
            break;
      }
      
      stream << "\n\n";
   
      for (unsigned int i = 0; i < atlasDirectories.size(); i++) {
         if (atlasDirectories[i].getSaveInPreferences()) {
            stream << preferencesAtlasDirectoryTag.c_str() << " "
                   << atlasDirectories[i].getMappingSurfaceListFile().c_str() << "\n";
         }
      }
      
      stream << "\n";
      
      stream << preferencesAtlasLastSelected.c_str() << " " 
             << StringUtilities::replace(lastSelectedAtlas, ' ', '?').c_str() << "\n\n";
      
      stream << preferencesVolumeTypeTag.c_str() << " ";
      switch(getVolumeType()) {
         case VOLUME_TYPE_AFNI:
            stream << preferencesVolumeValueTypeAFNI.c_str();
            break;
         case VOLUME_TYPE_ANALYZE:
            stream << preferencesVolumeValueTypeAnalyze.c_str();
            break;
         case VOLUME_TYPE_SPM_LPI:
            stream << preferencesVolumeValueTypeSPM_LPI.c_str();
            break;
         case VOLUME_TYPE_SPM_RPI:
            stream << preferencesVolumeValueTypeSPM_RPI.c_str();
            break;
         case VOLUME_TYPE_WUNIL:
            stream << preferencesVolumeValueTypeWUNIL.c_str();
            break;
         case VOLUME_TYPE_RAW_8_BIT_SIGNED:
            stream << preferencesVolumeValueTypeSigned8.c_str();
            break;
         case VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
            stream << preferencesVolumeValueTypeUnsigned8.c_str();
            break;
         case VOLUME_TYPE_RAW_16_BIT_SIGNED:
            stream << preferencesVolumeValueTypeSigned16.c_str();
            break;
         case VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
            stream << preferencesVolumeValueTypeUnsigned16.c_str();
            break;
         case VOLUME_TYPE_RAW_32_BIT_SIGNED:
            stream << preferencesVolumeValueTypeSigned32.c_str();
            break;
         case VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
            stream << preferencesVolumeValueTypeUnsigned32.c_str();
            break;
         case VOLUME_TYPE_RAW_64_BIT_SIGNED:
            stream << preferencesVolumeValueTypeSigned64.c_str();
            break;
         case VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
            stream << preferencesVolumeValueTypeUnsigned64.c_str();
            break;
         case VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
            stream << preferencesVolumeValueTypeFloat32.c_str();
            break;
         case VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
            stream << preferencesVolumeValueTypeFloat64.c_str();
            break;
         default:
            stream << preferencesVolumeValueTypeAFNI.c_str();
            break;
      }
      stream << "\n\n";
      
      stream << preferencesSurfaceTypeTag.c_str() << " ";
      switch(getSurfaceFileType()) {
         case SURFACE_FILE_TYPE_VTK_POLYDATA:
            stream << preferencesSurfaceValueTypeVTK.c_str();
            break;
         default:
            stream << preferencesSurfaceValueTypeVTK.c_str();
            break;
      }
      stream << "\n" << "\n";
      
      stream << preferencesAlgorithmNeighborsTag.c_str() << " " 
             << neighborLayers << "\n";
      stream << preferencesAlgorithmSigmaNormTag.c_str() << " "  
             <<gaussianSigmaNorm << "\n";
      stream << preferencesAlgorithmSigmaTangTag.c_str() << " "
             << gaussianSigmaTang << "\n";
      stream << preferencesAlgorithmNormBelowTag.c_str() << " "
             << gaussianNormBelowCutoff << "\n";
      stream << preferencesAlgorithmNormAboveTag.c_str() << " "
             << gaussianNormAboveCutoff << "\n";
      stream << preferencesAlgorithmTangTag.c_str() << " "
             << gaussianTangCutoff << "\n";
      stream << preferencesAlgorithmMaxDistanceTag.c_str() << " "  
             << brainfishMaxDistance << "\n";
      stream << preferencesAlgorithmSplatFactorTag.c_str() << " "
             << brainfishSplatFactor << "\n";
      stream << "\n";
                             
      stream << preferencesRawDimensionTag.c_str() << " "
             << rawVolumeDimensionX << " "
             << rawVolumeDimensionY << " "
             << rawVolumeDimensionZ << "\n";
      stream << "\n";
      
      std::string yesno(preferencesValueNo);                       
      if (indivPaddingEnabled) {
         yesno.assign(preferencesValueYes);
      }
      else {
         yesno.assign(preferencesValueNo);
      }
            
      stream << preferencesIndivPaddingTag.c_str() << " "
             << yesno.c_str() << " "
             << indivPaddingNegX << " "
             << indivPaddingNegY << " "
             << indivPaddingNegZ << " "
             << indivPaddingPosX << " "
             << indivPaddingPosY << " "
             << indivPaddingPosZ << "\n";
                                                                      
      if (indivCroppingEnabled) {
         yesno.assign(preferencesValueYes);
      }
      else {
         yesno.assign(preferencesValueNo);
      }
      stream << preferencesIndivCroppingOffsetTag.c_str() << " "
             << yesno.c_str() << " "
             << indivCroppingOffsetX << " "
             << indivCroppingOffsetY << " "
             <<indivCroppingOffsetZ << "\n";
      stream << "\n";
      
      if (volumeByteSwapping) {
         stream << preferencesByteSwapTag.c_str() << " " 
                << preferencesValueYes.c_str() << "\n";
      }
      else {
         stream << preferencesByteSwapTag.c_str() << " " 
                << preferencesValueNo.c_str() << "\n";
      }
      stream << "\n";
      
      //
      // Volume orientation
      //
      stream << preferencesVolumeOrientationTag.c_str() << " ";
      
      switch(volumeOrientationX) {
         case X_NEGATIVE_ORIENTATION_LEFT:
            stream << preferencesOrientationValueLeft.c_str();
            break;
         case X_NEGATIVE_ORIENTATION_RIGHT:
            stream << preferencesOrientationValueRight.c_str();
            break;
         default:
            stream << preferencesOrientationValueLeft.c_str();
            break;
      }
      stream << " ";
      
      switch(volumeOrientationY) {
         case Y_NEGATIVE_ORIENTATION_POSTERIOR:
            stream << preferencesOrientationValuePosterior.c_str();
            break;
         case Y_NEGATIVE_ORIENTATION_ANTERIOR:
            stream << preferencesOrientationValueAnterior.c_str();
            break;
         default:
            stream << preferencesOrientationValuePosterior.c_str();
            break;
      }
      stream << " ";
      
      switch(volumeOrientationZ) {
         case Z_NEGATIVE_ORIENTATION_INFERIOR:
            stream << preferencesOrientationValueInferior.c_str();
            break;
         case Z_NEGATIVE_ORIENTATION_SUPERIOR:
            stream << preferencesOrientationValueSuperior.c_str();
            break;
         default:
            stream << preferencesOrientationValueInferior.c_str();
            break;
      }
      stream << "\n" << "\n";
      
      //
      // Indivd Surface orientation
      //
      stream << preferencesIndivSurfaceOrientationTag.c_str() << " ";
      
      switch(indivSurfaceOrientationX) {
         case X_NEGATIVE_ORIENTATION_LEFT:
            stream << preferencesOrientationValueLeft.c_str();
            break;
         case X_NEGATIVE_ORIENTATION_RIGHT:
            stream << preferencesOrientationValueRight.c_str();
            break;
         default:
            stream << preferencesOrientationValueLeft.c_str();
            break;
      }
      stream << " ";
      
      switch(indivSurfaceOrientationY) {
         case Y_NEGATIVE_ORIENTATION_POSTERIOR:
            stream << preferencesOrientationValuePosterior.c_str();
            break;
         case Y_NEGATIVE_ORIENTATION_ANTERIOR:
            stream << preferencesOrientationValueAnterior.c_str();
            break;
         default:
            stream << preferencesOrientationValuePosterior.c_str();
            break;
      }
      stream << " ";
      
      switch(indivSurfaceOrientationZ) {
         case Z_NEGATIVE_ORIENTATION_INFERIOR:
            stream << preferencesOrientationValueInferior.c_str();
            break;
         case Z_NEGATIVE_ORIENTATION_SUPERIOR:
            stream << preferencesOrientationValueSuperior.c_str();
            break;
         default:
            stream << preferencesOrientationValueInferior.c_str();
            break;
      }
      stream << "\n\n";
      
      file.close();
      
   }
}
                
/**
 * Set the default metric names.
 */
void 
FMRIDataMapper::setDefaultMetricFileNames(const std::vector<std::string> defaultNames)
{
   defaultMetricFileNames = defaultNames;
}

/**
 * Set the default spec file names.
 */
void
FMRIDataMapper::setDefaultSpecFileNames(const std::vector<std::string> defaultNames)
{
   defaultSpecFileNames = defaultNames;
}

/**
 * Load the parameters from a params file
 */
void
FMRIDataMapper::loadParamsFile(const std::string& fileName)
{
   ParamsFile pf;
   try {
      pf.readFile(fileName);
   }
   catch (FileIOException& e) {
      std::string msg("Error reading params file ");
      msg.append(fileName);
      msg.append(e.what());
      processErrorMessage(msg);
      return;
   }
   
   //
   // Padding
   //
   bool paddingEnabled = false;
   std::string padding;
   if (pf.getParameter(ParamsFile::keyPadded, padding)) {
       paddingEnabled = (padding == ParamsFile::keyValueYes);
   }
   int negX = 0, negY = 0, negZ = 0;
   int posX = 0, posY = 0, posZ = 0;
   pf.getParameter(ParamsFile::keyOldPadNegX, negX);
   pf.getParameter(ParamsFile::keyOldPadPosX, posX);
   pf.getParameter(ParamsFile::keyOldPadNegY, negY);
   pf.getParameter(ParamsFile::keyOldPadPosY, posY);
   pf.getParameter(ParamsFile::keyOldPadNegZ, negZ);
   pf.getParameter(ParamsFile::keyOldPadPosZ, posZ);
   setIndivPadding(paddingEnabled, negX, negY, negZ, posX, posY, posZ);
   
   //
   // Cropping
   //
   bool croppingEnabled = 0;
   int x = 0, y = 0, z = 0;
   std::string cropping;
   if (pf.getParameter(ParamsFile::keyCropped, cropping)) {
      croppingEnabled = (cropping == ParamsFile::keyValueYes);
   }
   pf.getParameter(ParamsFile::keyXmin, x);
   pf.getParameter(ParamsFile::keyYmin, y);
   pf.getParameter(ParamsFile::keyZmin, z);
   setIndivCroppingOffset(croppingEnabled, x, y, z);
}
