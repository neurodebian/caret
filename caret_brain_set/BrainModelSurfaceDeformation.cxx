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

#include <algorithm>

#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "vtkMath.h"

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformation.h"
#include "BrainModelSurfaceDeformDataFile.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainSet.h"
#include "DebugControl.h"
#include "DeformationFieldFile.h"
#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "TransformationMatrixFile.h"

/**
 * Constructor
 */
BrainModelSurfaceDeformation::BrainModelSurfaceDeformation(BrainSet* brainSetIn,
                                                  DeformationMapFile* deformationMapFileIn)
   : BrainModelAlgorithm(brainSetIn)
{
   deformationMapFile = deformationMapFileIn;
   sourceBrainSet = NULL;
   targetBrainSet = NULL;
   sourceBorderFlippedForDifferentHemispheres = false;
   deformSourceFiducialCoordFiles = true;
   deformSourceInflatedCoordFiles = true;
   deformSourceVeryInflatedCoordFiles = true;
   deformSourceSphericalCoordFiles = true;
   deformSourceFlatCoordFiles = true;
   //deformSourceFlatLobarCoordFiles = true;
   defaultViewValid = false;
   deformDataFilesFlag = true;
}

/**
 * Destructor
 */
BrainModelSurfaceDeformation::~BrainModelSurfaceDeformation()
{
   if (sourceBrainSet != NULL) {
      delete sourceBrainSet;
      sourceBrainSet = NULL;
   }
   if (targetBrainSet != NULL) {
      delete targetBrainSet;
      targetBrainSet = NULL;
   }
}

/**
 * set the surface that has viewing transformations that should be used.
 */
void BrainModelSurfaceDeformation::setsurfaceWithViewingTransformations(BrainModelSurface* bms) {
   this->defaultViewValid = false;
   if (bms != NULL) {
      this->defaultViewValid = true;
      bms->getTranslation(0, this->defaultViewTranslation);
      bms->getScaling(0, this->defaultViewScaling);
      bms->getRotationMatrix(0, this->defaultViewRotation);
   }
}

/**
 * update the viewing transformation for surface.
 */
void
BrainModelSurfaceDeformation::updateViewingTransformation(BrainModelSurface* bms)
{
   if (defaultViewValid) {
      bms->setTranslation(0, this->defaultViewTranslation);
      bms->setScaling(0, this->defaultViewScaling);
      bms->setRotationMatrix(0, this->defaultViewRotation);
   }
}

/**
 * update the viewing transformation for surfaces in brain set.
 */
void
BrainModelSurfaceDeformation::updateViewingTransformation(BrainSet* bs)
{
   for (int i = 0; i < bs->getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = bs->getBrainModelSurface(i);
      if (bms != NULL) {
         this->updateViewingTransformation(bms);
      }
   }
}

/**
 * Read in the source spec file and create a BrainSet for it.
 */
void
BrainModelSurfaceDeformation::readSourceBrainSet() throw (BrainModelAlgorithmException)
{
   //
   // Get the source spec file name.
   //
   const QString specFileName(deformationMapFile->getSourceSpecFileName());
   
   //
   // Create the new brain set
   //
   sourceBrainSet = new BrainSet;
   
   //
   // Read in the source spec file and select the deformation files
   //
   SpecFile sf;
   QString borderName;
   DeformationMapFile::BORDER_FILE_TYPE borderType;
   deformationMapFile->getSourceBorderFileName(borderName, borderType);
   try {
      sf.readFile(specFileName);
   }
   catch(FileException& /*e*/) {
      QString msg("Error reading spec file ");
      msg.append(specFileName);
   }
   sf.setDeformationSelectedFiles(deformationMapFile->getSourceClosedTopoFileName(),
                                  deformationMapFile->getSourceCutTopoFileName(),
                                  deformationMapFile->getSourceFiducialCoordFileName(),
                                  deformationMapFile->getSourceSphericalCoordFileName(),
                                  deformationMapFile->getSourceFlatCoordFileName(),
                                  borderName,
                                  false,
                                  sf.getStructure());
   
   //
   // Read in the data files for the source brain set.
   // Note: BrainSet::readSpecFile()  will change the current directory so restore
   // current directory after files are read.
   //
   QString currentDirectory(QDir::currentPath());
   std::vector<QString> errorMessages;
   sourceBrainSet->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                sf, specFileName,
                                errorMessages, NULL, NULL);
   sourceDirectory = QDir::currentPath();
   QDir::setCurrent(currentDirectory);
   if (errorMessages.empty() == false) {
      const QString msg(StringUtilities::combine(errorMessages, "\n"));
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // See if border file needs to be projected
   //
   switch(deformationMapFile->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         switch (borderType) {
            case DeformationMapFile::BORDER_FILE_UNKNOWN:
               throw BrainModelAlgorithmException("Source border file is of unknown type.");
               break;
            case DeformationMapFile::BORDER_FILE_FLAT:
               break;
            case DeformationMapFile::BORDER_FILE_FLAT_LOBAR:
               break;
            case DeformationMapFile::BORDER_FILE_SPHERICAL:
               //
               // Project spherical border to create flat border
               //
               projectBorderFile(sourceBrainSet, BrainModelSurface::SURFACE_TYPE_SPHERICAL, "source");
               break;
            case DeformationMapFile::BORDER_FILE_PROJECTION:
               break;
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         switch (borderType) {
            case DeformationMapFile::BORDER_FILE_UNKNOWN:
               throw BrainModelAlgorithmException("Source border file is of unknown type.");
               break;
            case DeformationMapFile::BORDER_FILE_FLAT:
               //
               // Project flat border to create spherical border
               //
               projectBorderFile(sourceBrainSet, BrainModelSurface::SURFACE_TYPE_FLAT, "source");
               break;
            case DeformationMapFile::BORDER_FILE_FLAT_LOBAR:
               //
               // Project flat border to create spherical border
               //
               projectBorderFile(sourceBrainSet, BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR, "source");
               break;
            case DeformationMapFile::BORDER_FILE_SPHERICAL:
               break;
            case DeformationMapFile::BORDER_FILE_PROJECTION:
               break;
         }
         break;
   }
   
   //
   // Set file pointers for source
   //
   sourceFiducialSurface = 
      sourceBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (sourceFiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Unablet to find source fiducial coordinate file.");
   }
   switch(deformationMapFile->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         sourceSurface = 
            sourceBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
         sourceBorderFile = 
            sourceBrainSet->getBorderSet()->copyBordersOfSpecifiedType(BrainModelSurface::SURFACE_TYPE_FLAT);
         if (sourceSurface == NULL) {
            sourceSurface = 
               sourceBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR);
            sourceBorderFile = 
               sourceBrainSet->getBorderSet()->copyBordersOfSpecifiedType(BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR);
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         sourceSurface = 
            sourceBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
            sourceBorderFile = 
               sourceBrainSet->getBorderSet()->copyBordersOfSpecifiedType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
            checkSphericalBorder(sourceSurface, sourceBorderFile, "Individual");
         break;
   }
   if (sourceSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find source coordinate file.");
   }
   sourceTopologyFile = sourceSurface->getTopologyFile();
   if (sourceTopologyFile == NULL) {
      throw BrainModelAlgorithmException("Unable to find source topology file.");
   }
   if (sourceBorderFile == NULL) {
      throw BrainModelAlgorithmException("Unable to find source border file.");
   }
   else if (sourceBorderFile->getNumberOfBorders() <= 0) {
      throw BrainModelAlgorithmException("Source border file (" + borderName + ") contains no borders, was not found, or is not in the spec file.");
   }
}

/**
 * Check a border file to see if it might be screwed up
 */
void
BrainModelSurfaceDeformation::checkSphericalBorder(const BrainModelSurface* bms,
                                                   const BorderFile* bf,
                                                   const QString indivTargetName) const
{
   const float halfRadius = bms->getSphericalSurfaceRadius() * 0.5;
   
   bool warnFlag = false;
   
   const int numBorders = bf->getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      const Border* b = bf->getBorder(i);
      const int numLinks = b->getNumberOfLinks();
      for (int i = 0; i < (numLinks - 1); i++) {
         const float* b1 = b->getLinkXYZ(i);
         const float* b2 = b->getLinkXYZ(i + 1);
         const float dist = MathUtilities::distance3D(b1, b2);
         if (dist > halfRadius) {
            warnFlag = true;
            break;
         }
      }
      if (warnFlag) {
         break;
      }
   }
   
   if (warnFlag) {
      QString msg("Warning: Border file for ");
      msg.append(indivTargetName);
      msg.append(" has abnormally large spacing.\n");
      msg.append("Do you want to continue ?\n");
      msg.append("\n");
      msg.append("If you choose to cancel the deformation then\n");
      msg.append("   1) launch Caret5 in the approprate directory.\n");
      msg.append("   2) View the landmark borders on the spherical surface \n");
      msg.append("      with Display Control: Borders: Draw Borders as Points and Lines.\n");
      msg.append("   3) Look for misprojected border points (probably at the origin)");
      msg.append("      using Display Control: Surface Miscellaneous: Drawing Mode: Hide Surface.\n");
      msg.append("   4) Delete the misprojected border points with Layers: Borders\n");
      msg.append("      Delete Border Point With Mouse.\n");
      msg.append("   5) Save the modified border projection file.\n");
      
      QWidget* parentWidget = brainSet->getProgressDialogParent();
      if (parentWidget != NULL) {
         if (QMessageBox::warning(parentWidget, "Warning", msg, 
                                    "Continue", "Cancel") == 1) {
            throw BrainModelAlgorithmException("Canceled by user");
         }
      }
   }
}

/**
 * Read in the target spec file and create a BrainSet for it.
 */
void
BrainModelSurfaceDeformation::readTargetBrainSet(int stageIndex) throw (BrainModelAlgorithmException)
{
   //
   // Get the target spec file name.
   //
   const QString specFileName(deformationMapFile->getTargetSpecFileName());
   
   //
   // Create the new brain set
   //
   targetBrainSet = new BrainSet;
   
   //
   // Read in the target spec file and select the deformation files
   //
   SpecFile sf;
   QString borderName;
   DeformationMapFile::BORDER_FILE_TYPE borderType;
   deformationMapFile->getTargetBorderFileName(stageIndex, borderName, borderType);
   try {
      sf.readFile(specFileName);
   }
   catch(FileException& /*e*/) {
      QString msg("Error reading spec file ");
      msg.append(specFileName);
   }
   sf.setDeformationSelectedFiles(deformationMapFile->getTargetClosedTopoFileName(),
                                  deformationMapFile->getTargetCutTopoFileName(),
                                  deformationMapFile->getTargetFiducialCoordFileName(),
                                  deformationMapFile->getTargetSphericalCoordFileName(),
                                  deformationMapFile->getTargetFlatCoordFileName(),
                                  borderName,
                                  false,
                                  sf.getStructure());
   
   //
   // Read in the data files for the target brain set.
   // Note: BrainSet::readSpecFile()  will change the current directory so restore
   // current directory after files are read.
   //
   QString currentDirectory(QDir::currentPath());
   std::vector<QString> errorMessages;
   targetBrainSet->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                sf, specFileName,
                                errorMessages, NULL, NULL);
   targetDirectory = QDir::currentPath();
   QDir::setCurrent(currentDirectory);
   if (errorMessages.empty() == false) {
      const QString msg(StringUtilities::combine(errorMessages, "\n"));
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // See if border file needs to be projected
   //
   switch(deformationMapFile->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         switch (borderType) {
            case DeformationMapFile::BORDER_FILE_UNKNOWN:
               throw BrainModelAlgorithmException("Target border file is of unknown type.");
               break;
            case DeformationMapFile::BORDER_FILE_FLAT:
               break;
            case DeformationMapFile::BORDER_FILE_FLAT_LOBAR:
               break;
            case DeformationMapFile::BORDER_FILE_SPHERICAL:
               //
               // Project spherical border to create flat border
               //
               projectBorderFile(targetBrainSet, BrainModelSurface::SURFACE_TYPE_SPHERICAL, "target");
               break;
            case DeformationMapFile::BORDER_FILE_PROJECTION:
               break;
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         switch (borderType) {
            case DeformationMapFile::BORDER_FILE_UNKNOWN:
               throw BrainModelAlgorithmException("Target border file is of unknown type.");
               break;
            case DeformationMapFile::BORDER_FILE_FLAT:
               //
               // Project flat border to create spherical border
               //
               projectBorderFile(targetBrainSet, BrainModelSurface::SURFACE_TYPE_FLAT, "target");
               break;
            case DeformationMapFile::BORDER_FILE_FLAT_LOBAR:
               //
               // Project flat border to create spherical border
               //
               projectBorderFile(targetBrainSet, BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR, "target");
               break;
            case DeformationMapFile::BORDER_FILE_SPHERICAL:
               break;
            case DeformationMapFile::BORDER_FILE_PROJECTION:
               break;
         }
         break;
   }
   
   //
   // Set file pointers for target
   //
   targetFiducialSurface = 
      targetBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (targetFiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Unablet to find target fiducial coordinate file.");
   }
   switch(deformationMapFile->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         targetSurface = 
            targetBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
         targetBorderFile = 
            targetBrainSet->getBorderSet()->copyBordersOfSpecifiedType(BrainModelSurface::SURFACE_TYPE_FLAT);
         if (targetSurface == NULL) {
            targetSurface = 
               targetBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR);
            targetBorderFile = 
               targetBrainSet->getBorderSet()->copyBordersOfSpecifiedType(BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR);
         }
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         targetSurface = 
            targetBrainSet->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
            targetBorderFile = 
               targetBrainSet->getBorderSet()->copyBordersOfSpecifiedType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
            checkSphericalBorder(targetSurface, targetBorderFile, "Target");
         break;
   }
   if (targetSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find target coordinate file.");
   }
   targetTopologyFile = targetSurface->getTopologyFile();
   if (targetTopologyFile == NULL) {
      throw BrainModelAlgorithmException("Unable to find target topology file.");
   }
   if (targetBorderFile == NULL) {
      throw BrainModelAlgorithmException("Unable to find target border file.");
   }
   else if (targetBorderFile->getNumberOfBorders() <= 0) {
      throw BrainModelAlgorithmException("Target border file (" + borderName + ") contains no borders, was not found, or is not in the spec file.");
   }
}

/**
 * Project the border file
 */
void
BrainModelSurfaceDeformation::projectBorderFile(BrainSet* theBrainSet,
                                                const BrainModelSurface::SURFACE_TYPES borderType,
                                                const QString& sourceTargetString)
                                                         throw (BrainModelAlgorithmException)
{
   //
   // Get the border file and make sure it has borders
   //
   BorderFile* bf = theBrainSet->getBorderSet()->copyBordersOfSpecifiedType(borderType);
   if (bf == NULL) {
      QString s("Unable to find ");
      s.append(sourceTargetString);
      s.append(" border file.");
      throw BrainModelAlgorithmException(s);
   }
   else if (bf->getNumberOfBorders() == 0) {
      QString s(sourceTargetString);
      s.append(" border file is empty.");
      throw BrainModelAlgorithmException(s);
   }
   
   //
   // Get the surface for projection
   //
   BrainModelSurface* bms = theBrainSet->getBrainModelSurfaceOfType(borderType);
   if (bms == NULL) {
      QString s("Unable to find surface for projecting the ");
      s.append(sourceTargetString);
      s.append(" border file.");
      throw BrainModelAlgorithmException(s);
   }
   
   //
   // Create a border file projector
   //
   BorderFileProjector projector(bms, true);
   
   //
   // Get and clear the border projection file
   //
   BorderProjectionFile bpf;
   
   //
   // Project the border file
   //
   projector.projectBorderFile(bf, &bpf, NULL);
   
   //
   // Clear the border files
   //
   theBrainSet->deleteAllBorders();
   
   //
   // Apply the border projection file to all surfaces
   //
   theBrainSet->getBorderSet()->copyBordersFromBorderProjectionFile(&bpf);
} 

/**
 * Resample border files as needed.
 */
void
BrainModelSurfaceDeformation::resampleBorderFiles(const int stageNumber,
                                                  const int cycleNumber,
                                                  float sphericalRadius) throw (BrainModelAlgorithmException)
{
   //
   // Make sure source and target border files have same number of borders
   //
   if (sourceBorderFile->getNumberOfBorders() != targetBorderFile->getNumberOfBorders()) {
      throw BrainModelAlgorithmException(
         "Source and target border files have different numbers of borders.");
   }
   
   //
   // Get the type of resampling
   //
   DeformationMapFile::BORDER_RESAMPLING_TYPE resampleType;
   float resampleValue;
   deformationMapFile->getBorderResampling(resampleType, resampleValue);
   
   switch(resampleType) {
      case DeformationMapFile::BORDER_RESAMPLING_NONE:
         break;
      case DeformationMapFile::BORDER_RESAMPLING_FROM_BORDER_FILE:
      case DeformationMapFile::BORDER_RESAMPLING_VALUE:
         {
            //
            //  Resample the borders
            //
            const int numBorders = sourceBorderFile->getNumberOfBorders();
            for (int i = 0; i < numBorders; i++) {
               Border* sourceBorder = sourceBorderFile->getBorder(i);
               const int sourceBorderNumLinks = sourceBorder->getNumberOfLinks();
               if (sourceBorderNumLinks > 1) {
                  for (int j = 0; j < sourceBorderNumLinks; j++) {
                  
                     //
                     // Get the sampling density
                     //
                     float resampling = resampleValue;
                     switch(resampleType) {
                        case DeformationMapFile::BORDER_RESAMPLING_NONE:
                           break;
                        case DeformationMapFile::BORDER_RESAMPLING_FROM_BORDER_FILE:
                           resampling = sourceBorder->getSamplingDensity();
                           break;
                        case DeformationMapFile::BORDER_RESAMPLING_VALUE:
                           break;
                     }
                     
                     //
                     // Resample the source border
                     //
                     int newNumLinks = 2;
                     sourceBorder->resampleBorderToDensity(resampling, 2, newNumLinks);
                     
                     //
                     // Get the name of the border
                     //
                     const QString borderName = sourceBorder->getName();
                     
                     //
                     // Find target border with same name
                     //
                     Border* targetBorder = targetBorderFile->getBorderByName(borderName);
                     if (targetBorder == NULL) {
                        QString msg("Border named ");
                        msg.append(borderName);
                        msg.append(" not found in target borders.");
                        throw BrainModelAlgorithmException(msg);
                     }
                     
                     //
                     // Resample target border to match source border
                     //
                     targetBorder->resampleBorderToNumberOfLinks(newNumLinks);
                  }
               }
            }
         }
         break;
   }
   
   //
   // Make target borders have the same order as the source borders
   //
   BorderFile* tempBorderFile = new BorderFile;
   const int numBorders = sourceBorderFile->getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      const Border* sb = sourceBorderFile->getBorder(i);
      const QString name = sb->getName();
      Border* tb = targetBorderFile->getBorderByName(name);
      if (tb == NULL) {
         QString msg("Border named ");
         msg.append(name);
         msg.append(" not found in target borders.");
         throw BrainModelAlgorithmException(msg);
      }
      tempBorderFile->addBorder(*tb);
   }
   delete targetBorderFile;
   targetBorderFile = tempBorderFile;
   
   //
   // If source and target hemispheres are different, flip X coord of source border file
   //
   if (stageNumber <= 1) {
       if (sourceBrainSet->getStructure() != targetBrainSet->getStructure()) {
          if (sourceBorderFlippedForDifferentHemispheres == false) {
             sourceBorderFlippedForDifferentHemispheres = true;
             TransformationMatrix tm;
             tm.scale(-1.0, 1.0, 1.0);
             sourceBorderFile->applyTransformationMatrix(tm);
             std::cout << "Src/Tgt Different Hemispheres" << std::endl;
          }
       }
   }

   //
   //  set the resampled border types
   //
   switch(deformationMapFile->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         targetBorderFile->setHeaderTag(AbstractFile::headerTagConfigurationID,
                                        "FLAT");
         sourceBorderFile->setHeaderTag(AbstractFile::headerTagConfigurationID,
                                        "FLAT");
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         targetBorderFile->setHeaderTag(AbstractFile::headerTagConfigurationID,
                                        "SPHERICAL");
         sourceBorderFile->setHeaderTag(AbstractFile::headerTagConfigurationID,
                                        "SPHERICAL");
         break;
   }

   //
   // Apply spherical radius
   //
   if (sphericalRadius > 0.0) {
      sourceBorderFile->setSphericalBorderRadius(sphericalRadius);
      targetBorderFile->setSphericalBorderRadius(sphericalRadius);
   }

   //
   // Save the resampled source and target border files
   //
   sourceBorderResampledName = "source_after_resample_";
   if (stageNumber >= 0) {
      sourceBorderResampledName += ("stage_" + QString::number(stageNumber));
   }
   if (cycleNumber >= 0) {
      sourceBorderResampledName += ("_cycle_" + QString::number(cycleNumber));
   }
   sourceBorderResampledName += ".border";
   targetBorderResampledName = "target_after_resample_";
   if (stageNumber >= 0) {
      targetBorderResampledName += ("stage_" + QString::number(stageNumber));
   }
   if (cycleNumber >= 0) {
      targetBorderResampledName += ("_cycle_" + QString::number(cycleNumber));
   }
   targetBorderResampledName += ".border";
   sourceBorderFile->writeFile(sourceBorderResampledName);
   targetBorderFile->writeFile(targetBorderResampledName);
   intermediateFiles.push_back(sourceBorderResampledName);
   intermediateFiles.push_back(targetBorderResampledName);
}

/**
 * Create the output spec and deformation file names.
 */
void 
BrainModelSurfaceDeformation::createOutputSpecAndDeformationFileNames()
{
   sourceToTargetSpecFileName = 
      BrainModelSurfaceDeformDataFile::createDeformedFileName(
         deformationMapFile->getSourceSpecFileName(),
         deformationMapFile->getTargetSpecFileName(),
         deformationMapFile->getDeformedFileNamePrefix(),
         deformationMapFile->getNumberOfNodes(),
         false);

   if (sourceToTargetDeformationMapFileName.isEmpty()) {
      sourceToTargetDeformationMapFileName =
              BrainModelSurfaceDeformDataFile::createDeformedSpecFileName(
                 deformationMapFile->getDeformedFileNamePrefix(),
                 deformationMapFile->getSourceSpecFileName(),
                 deformationMapFile->getTargetSpecFileName());
       //sourceToTargetDeformationMapFileName =
       //   BrainModelSurfaceDeformDataFile::createDeformedFileName(
       //      deformationMapFile->getSourceSpecFileName(),
       //      deformationMapFile->getTargetSpecFileName(),
       //      deformationMapFile->getDeformedFileNamePrefix(),
       //      deformationMapFile->getNumberOfNodes(),
       //      true);
   }
   
   targetToSourceSpecFileName = 
      BrainModelSurfaceDeformDataFile::createDeformedFileName(
         deformationMapFile->getTargetSpecFileName(),
         deformationMapFile->getSourceSpecFileName(),
         deformationMapFile->getDeformedFileNamePrefix(),
         deformationMapFile->getNumberOfNodes(),
         false);

   if (targetToSourceDeformationMapFileName.isEmpty()) {
      targetToSourceDeformationMapFileName =
              BrainModelSurfaceDeformDataFile::createDeformedSpecFileName(
                 deformationMapFile->getDeformedFileNamePrefix(),
                 deformationMapFile->getTargetSpecFileName(),
                 deformationMapFile->getSourceSpecFileName());

       //targetToSourceDeformationMapFileName =
       //   BrainModelSurfaceDeformDataFile::createDeformedFileName(
       //      deformationMapFile->getTargetSpecFileName(),
       //      deformationMapFile->getSourceSpecFileName(),
       //      deformationMapFile->getDeformedFileNamePrefix(),
       //      deformationMapFile->getNumberOfNodes(),
       //      true);
   }
}      

/**
 * set the deforomation map file names (overrides defaults).
 */
void
BrainModelSurfaceDeformation::setDeformationMapFileNames(
                              const QString& indivToAtlasDefMapFileName,
                              const QString& atlasToIndivDefMapFileName)
{
   sourceToTargetDeformationMapFileName = indivToAtlasDefMapFileName;
   targetToSourceDeformationMapFileName = atlasToIndivDefMapFileName;
}

/**
 * Create the deformation that maps target surface nodes into the deformed source surface.
 */
void
BrainModelSurfaceDeformation::createNodeDeformation(const BrainModelSurface* theSourceSurfaceDeformed,
                                                    const BrainModelSurface* theTargetSurface,
                                                    DeformationMapFile* theDeformationMap)
{
   //
   // Create a Point Projector for the deformed surface.
   //
   BrainModelSurfacePointProjector::SURFACE_TYPE_HINT surfaceTypeHint = 
      BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_FLAT;
   switch (deformationMapFile->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         surfaceTypeHint = BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_SPHERE;
         break;
   }
   BrainModelSurfacePointProjector bspp(theSourceSurfaceDeformed,
                                        surfaceTypeHint,
                                        false);
   
   //
   // Check for spherical deformation
   //
   float deformedRadius = 1.0;
   bool sphericalDeformationFlag = false;
   switch (deformationMapFile->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         sphericalDeformationFlag = true;
         deformedRadius = theSourceSurfaceDeformed->getSphericalSurfaceRadius();
         break;
   }
   
   //
   // Get the source coordinates
   //
   const CoordinateFile* theSourceCoords = theSourceSurfaceDeformed->getCoordinateFile();
   
   //
   // Get the target coordinates
   //
   const CoordinateFile* coords = theTargetSurface->getCoordinateFile();
   const int numCoords = coords->getNumberOfCoordinates();
   
   //
   // Set the number of nodes in the deformation file
   //
   theDeformationMap->setNumberOfNodes(numCoords);
   
   //
   // Project each target node into the source deformed surface
   //
   for (int i = 0; i < numCoords; i++) {
      float xyz[3];
      coords->getCoordinate(i, xyz);
      
      //
      // Push nodes onto sphere if spherical deformation
      //
      if (sphericalDeformationFlag) {
         const float radius = MathUtilities::vectorLength(xyz);
         if (radius != 0.0) {
            const float scale = deformedRadius / radius;
            xyz[0] *= scale;
            xyz[1] *= scale;
            xyz[2] *= scale;
         }
      }
      
      //
      // Project target node onto source deformed surface
      //
      int nearestNode = -1;
      int tileNodes[3];
      float tileAreas[3];
      const int tile = bspp.projectBarycentric(xyz, nearestNode,
                                               tileNodes, tileAreas, true);
                                               
      //
      // Unproject using the deformed source coordinate file
      //
      if (tile >= 0) {
         //
         // First node should be closest one
         //
         const float distToNode[3] = {
            vtkMath::Distance2BetweenPoints(xyz, 
                                          (float*)theSourceCoords->getCoordinate(tileNodes[0])),
            vtkMath::Distance2BetweenPoints(xyz, 
                                          (float*)theSourceCoords->getCoordinate(tileNodes[1])),
            vtkMath::Distance2BetweenPoints(xyz, 
                                          (float*)theSourceCoords->getCoordinate(tileNodes[2]))
         };
/*
         if ((distToNode[1] < distToNode[0]) &&
             (distToNode[1] < distToNode[2])) {
            std::swap(tileNodes[0], tileNodes[1]);
            std::swap(tileAreas[0], tileAreas[1]);
         }
         else if ((distToNode[2] < distToNode[0]) &&
                  (distToNode[2] < distToNode[1])) {
            std::swap(tileNodes[0], tileNodes[2]);
            std::swap(tileAreas[0], tileAreas[2]);
         }
*/
         //
         // Note: nodes and areas do not correspond by index due to an incorrect, early 
         // implementation in caret
         // See BrainModelSurfacePointProjector for implementation
         //    Node   Area
         //    Index  Index
         //     0       1
         //     1       2
         //     2       0
         //
         if ((distToNode[1] < distToNode[0]) &&
             (distToNode[1] < distToNode[2])) {
            std::swap(tileNodes[0], tileNodes[1]);
            std::swap(tileAreas[2], tileAreas[1]);  // this is correct 
         }
         else if ((distToNode[2] < distToNode[0]) &&
                  (distToNode[2] < distToNode[1])) {
            std::swap(tileNodes[0], tileNodes[2]);
            std::swap(tileAreas[0], tileAreas[1]);  // this is correct
         }
      }
      else if (nearestNode >= 0) {
         tileNodes[0] = nearestNode;
         tileNodes[1] = nearestNode;
         tileNodes[2] = nearestNode;
         tileAreas[0] = 1.0;
         tileAreas[1] = 0.0;
         tileAreas[2] = 0.0;
      }
      else {
         tileNodes[0] = -1;
         tileNodes[1] = -1;
         tileNodes[2] = -1;
         tileAreas[0] = 0.0;
         tileAreas[1] = 0.0;
         tileAreas[2] = 0.0;
      }
      
      //
      // Set the node for the deformation file
      //
      theDeformationMap->setDeformDataForNode(i, tileNodes, tileAreas);
   }
}

/**
 * Deform the data files.
 */
void
BrainModelSurfaceDeformation::deformDataFiles(BrainSet* sourceBrain,
                                              BrainSet* targetBrain,
                                              const QString& sourceSpecName,
                                              const DeformationMapFile* dmf,
                                              const bool sourceToTargetFlag,
                                              const bool deformSourceFiducialCoordFilesIn,
                                              const bool deformSourceInflatedCoordFilesIn,
                                              const bool deformSourceVeryInflatedCoordFilesIn,
                                              const bool deformSourceSphericalCoordFilesIn,
                                              const bool deformSourceFlatCoordFilesIn,
                                              QString& deformErrorsMessage)
                                       throw (BrainModelAlgorithmException)
{
   //
   // Read in the source spec file
   //
   SpecFile sf;
   try {
      sf.readFile(sourceSpecName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // deform areal estimation files 
   //
   BrainModelSurfaceDeformDataFile::deformNodeAttributeFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION,
                                      sf.arealEstimationFile,
                                      deformErrorsMessage);

   //
   // deform atlas files 
   //
   BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS,
                                      sf.atlasFile,
                                      deformErrorsMessage);

   // lat lon files
   //
   //
   BrainModelSurfaceDeformDataFile::deformNodeAttributeFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON,
                                      sf.latLonFile,
                                      deformErrorsMessage);
   //
   // deform metric files 
   //
   BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC,
                                      sf.metricFile,
                                      deformErrorsMessage);

   //
   // deform paint files 
   //
   BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT,
                                      sf.paintFile,
                                      deformErrorsMessage);

   //
   // deform rgb paint files 
   //
   BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT,
                                      sf.rgbPaintFile,
                                      deformErrorsMessage);

   //
   // deform surface shape files 
   //
   BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE,
                                      sf.surfaceShapeFile,
                                      deformErrorsMessage);

   //
   // deform topography files 
   //
   BrainModelSurfaceDeformDataFile::deformNodeAttributeFiles(
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY,
                                      sf.topographyFile,
                                      deformErrorsMessage);

   //
   // deform border files
   //
   BrainModelSurfaceDeformDataFile::deformBorderFiles(
                                      sourceBrain,
                                      targetBrain,
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT,
                                      sf.flatBorderFile,
                                      deformErrorsMessage);
                                      
   //
   // deform border files
   //
   BrainModelSurfaceDeformDataFile::deformBorderFiles(
                                      sourceBrain,
                                      targetBrain,
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL,
                                      sf.sphericalBorderFile,
                                      deformErrorsMessage);
                                      
   //
   // deform border files
   //
   BrainModelSurfaceDeformDataFile::deformBorderFiles(
                                      sourceBrain,
                                      targetBrain,
                                      dmf,
                                      BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION,
                                      sf.borderProjectionFile,
                                      deformErrorsMessage);
   
   //
   // deform cell files
   //
   BrainModelSurfaceDeformDataFile::deformCellOrFociFiles(
                                      sourceBrain,
                                      targetBrain,
                                      dmf,
                                      sf.cellFile,
                                      false,
                                      deformErrorsMessage);
                                      
   //
   // deform cell files
   //
   BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFiles(
                                      sourceBrain,
                                      targetBrain,
                                      dmf,
                                      sf.cellProjectionFile,
                                      false,
                                      deformErrorsMessage);
                                      
   //
   // deform foci files
   //
   BrainModelSurfaceDeformDataFile::deformCellOrFociFiles(
                                      sourceBrain,
                                      targetBrain,
                                      dmf,
                                      sf.fociFile,
                                      true,
                                      deformErrorsMessage);
  
   //
   // deform foci files
   //
   BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFiles(
                                      sourceBrain,
                                      targetBrain,
                                      dmf,
                                      sf.fociProjectionFile,
                                      true,
                                      deformErrorsMessage);
  
   //
   // Determine which colors files are needed
   // 
   const bool linkAreaColorFiles = (sf.arealEstimationFile.files.size() > 0) ||
                                   (sf.atlasFile.files.size() > 0) ||
                                   (sf.paintFile.files.size() > 0) ||
                                   (sf.topographyFile.files.size() > 0);
   const bool linkBorderColorFiles = (sf.flatBorderFile.files.size() > 0) ||
                                     (sf.sphericalBorderFile.files.size() > 0) ||
                                     (sf.borderProjectionFile.files.size() > 0);
   const bool linkCellColorFiles = ((sf.cellFile.files.size() > 0) ||
                                    (sf.cellProjectionFile.files.size() > 0));
   const bool linkFociColorFiles = ((sf.fociFile.files.size() > 0) ||
                                    (sf.fociProjectionFile.files.size() > 0));
   
   //
   // link the needed color files
   //
   BrainModelSurfaceDeformDataFile::linkColorFiles(dmf,
                                                   linkAreaColorFiles,
                                                   linkBorderColorFiles,
                                                   linkCellColorFiles,
                                                   linkFociColorFiles,
                                                   deformErrorsMessage);
                                                   
   //
   // Only spherical deformation deforms coord files
   //
   bool doCoordFiles = false;
   switch(dmf->getFlatOrSphereSelection()) {
      case DeformationMapFile::DEFORMATION_TYPE_FLAT:
         break;
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
      case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
         doCoordFiles = true;
         break;
   }
   
   //
   // If deforming source files
   //
   if (sourceToTargetFlag && doCoordFiles) {
      //
      // If fiducial coord files should be deformed
      //
      if (deformSourceFiducialCoordFilesIn) {
         BrainModelSurfaceDeformDataFile::deformCoordinateFiles(dmf,
                                                               sf.fiducialCoordFile,
                                                               deformErrorsMessage);
      }

      //
      // If inflated coord files should be deformed
      //
      if (deformSourceInflatedCoordFilesIn) {
         BrainModelSurfaceDeformDataFile::deformCoordinateFiles(dmf,
                                                               sf.inflatedCoordFile,
                                                               deformErrorsMessage);
      }

      //
      // If very inflated coord files should be deformed
      //
      if (deformSourceVeryInflatedCoordFilesIn) {
         BrainModelSurfaceDeformDataFile::deformCoordinateFiles(dmf,
                                                               sf.veryInflatedCoordFile,
                                                               deformErrorsMessage);
      }

      //
      // If spherical coord files should be deformed
      //
      if (deformSourceSphericalCoordFilesIn) {
         BrainModelSurfaceDeformDataFile::deformCoordinateFiles(dmf,
                                                               sf.sphericalCoordFile,
                                                               deformErrorsMessage);
      }

      //
      // If flat coord files should be deformed
      //
      if (deformSourceFlatCoordFilesIn) {
         BrainModelSurfaceDeformDataFile::deformFlatCoordinateFiles(dmf,
                                                               sf.flatCoordFile,
                                                               deformErrorsMessage);
      }
   }
}

/**
 * Create the indiv to atlas deformation field
 */
void
BrainModelSurfaceDeformation::createIndivAtlasDeformationFieldFile(const BrainModelSurface* sphereSurface,
                                                      const BrainModelSurface* deformedSphereSurfaceIn)
{
   DeformationFieldFile dff;
   sphereSurface->createDeformationField(deformedSphereSurfaceIn,
                                         -1,
                                         "Spherical Deformation",
                                         dff);
   //
   // Write the deformation field file
   //
   QDir::setCurrent(sourceDirectory);
   const CoordinateFile* defCoordsIn = deformedSphereSurfaceIn->getCoordinateFile();
   QString name(FileUtilities::filenameWithoutExtension(defCoordsIn->getFileName()));
   name.append(SpecFile::getDeformationFieldFileExtension());
   try {
      dff.writeFile(name);
   }
   catch (FileException& e) {
      QDir::setCurrent(originalDirectory);   
      std::cout << "ERROR writing: " << e.whatQString().toAscii().constData() << std::endl;
   }
   QDir::setCurrent(originalDirectory);   
}

/**
 * Create the deformation field
 */
void
BrainModelSurfaceDeformation::createAtlasIndivDeformationFieldFile(
                                                      const BrainModelSurface* atlasSphereSurface,
                                                      const BrainModelSurface* sphereSurface,
                                                      const BrainModelSurface* deformedSphereSurfaceIn)
{
   DeformationFieldFile dff;
   atlasSphereSurface->createDeformationField(sphereSurface,
                                         deformedSphereSurfaceIn,
                                         -1,
                                         "Spherical Deformation",
                                         dff);
   //
   // Write the deformation field file
   //
   QDir::setCurrent(targetDirectory);
   const CoordinateFile* coords = atlasSphereSurface->getCoordinateFile();
   QString name(FileUtilities::filenameWithoutExtension(coords->getFileName()));
   name.append(SpecFile::getDeformationFieldFileExtension());
   try {
      dff.writeFile(name);
   }
   catch (FileException& e) {
      QDir::setCurrent(originalDirectory);   
      std::cout << "ERROR writing: " << e.whatQString().toAscii().constData() << std::endl;
   }
   QDir::setCurrent(originalDirectory);   
}

/**
 * Execute the deformation
 */
void 
BrainModelSurfaceDeformation::execute() throw (BrainModelAlgorithmException)
{
   const QString originalPath(QDir::currentPath());
   
   enum {
      PROGRESS_READING_DATA_FILES = 0,
      PROGRESS_RUNNING_DEFORMATION,
      PROGRESS_CREATING_DEFORMAION_MAP,
      PROGRESS_DEFORMING_SOURCE_TO_TARGET,
      PROGRESS_DEFORMING_TARGET_TO_SOURCE,
      PROGRESS_TOTAL_STEPS
   };
   
   createProgressDialog("Deformation",
                        PROGRESS_TOTAL_STEPS,
                        "deformationProgressDialog");
                        
   try {
      originalDirectory = QDir::currentPath();

      //
      // Update progress
      //
      updateProgressDialog("Reading Data Files.",
                               PROGRESS_READING_DATA_FILES);
                                  
      //
      // Read the source data files in
      //
      readSourceBrainSet();
      
      //
      // Read the target data files in
      //
      readTargetBrainSet(0);
      
      //
      // Resample border files as needed so that they match.
      //
      resampleBorderFiles(-1, -1, -1.0);
      
      //
      // Create the spec and deformation map file names 
      //
      createOutputSpecAndDeformationFileNames();
      
      //
      // Set the source and target paths
      //
      deformationMapFile->setSourceDirectory(sourceDirectory);
      deformationMapFile->setTargetDirectory(targetDirectory);
      
      //
      // Create the deformed spec file name for the source data deformed to the target
      //
      deformationMapFile->setOutputSpecFileName(sourceToTargetSpecFileName);
            
      //
      // Update progress
      //
      updateProgressDialog("Running the Deformation.",
                               PROGRESS_RUNNING_DEFORMATION);
      
      //
      // Execute the subclass' deformation
      //
      executeDeformation();
    
      //
      // Update progress
      //
      updateProgressDialog("Creating the Deformation Map",
                               PROGRESS_CREATING_DEFORMAION_MAP);
      
      //
      // Add software version to deformation map file
      //
      deformationMapFile->appendSoftwareVersionToFileComment("Deformed with");
      
      //
      // set the inverse deformation flag for source to target
      //
      deformationMapFile->setInverseDeformationFlag(false);
      
      //
      // Create the node deformation part of the deformation map
      //
      createNodeDeformation(deformedSourceSurface, targetSurface, deformationMapFile);
      
      switch (deformationMapFile->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            //
            // Create the deformation field
            //
            createIndivAtlasDeformationFieldFile(sourceSurface, deformedSourceSurface);
            break;
      }
      
      //
      // Copy and set the name of the source to target spec file
      //
      SpecFile deformedToTargetSpecFile;
      QDir::setCurrent(targetDirectory);
      try {
         deformedToTargetSpecFile.readFile(deformationMapFile->getTargetSpecFileName());
      }
      catch (FileException& /*e*/) {
         throw BrainModelAlgorithmException(
                       "Error reading target spec file for creating deformed version.");
      }
      try {
         deformedToTargetSpecFile.writeFile(sourceToTargetSpecFileName);
      }
      catch (FileException& /*e*/) {
         throw BrainModelAlgorithmException(
                       "Error writing target spec file for creating deformed version.");
      }
      targetBrainSet->setSpecFileName(sourceToTargetSpecFileName);
      QDir::setCurrent(originalDirectory);
      
      //
      // Add to recent spec files
      //
      PreferencesFile* pf = brainSet->getPreferencesFile();
      pf->addToRecentSpecFiles(sourceToTargetSpecFileName, true);
      
      //
      // Write the deformation map file
      //
      QDir::setCurrent(targetDirectory);
      deformationMapFile->writeFile(sourceToTargetDeformationMapFileName);
      targetBrainSet->setDeformationMapFileName(sourceToTargetDeformationMapFileName, true);
      QDir::setCurrent(originalDirectory);
      
      //
      // Should data files be deformed?
      //
      if (deformDataFilesFlag) {
          //
          // Update progress
          //
          updateProgressDialog("Deforming Individual Files to Atlas.",
                                   PROGRESS_DEFORMING_SOURCE_TO_TARGET);

          //
          // Deform the source data files to the target
          //
          deformDataFiles(sourceBrainSet,
                          targetBrainSet,
                          deformationMapFile->getSourceSpecFileName(),
                          deformationMapFile,
                          true,
                          deformSourceFiducialCoordFiles,
                          deformSourceInflatedCoordFiles,
                          deformSourceVeryInflatedCoordFiles,
                          deformSourceSphericalCoordFiles,
                          deformSourceFlatCoordFiles,
                          sourceToTargetDeformDataFileErrors);

          //
          // Update progress
          //
          updateProgressDialog("Deforming Atlas Files to Individual.",
                                   PROGRESS_DEFORMING_TARGET_TO_SOURCE);

          //
          // if also deforming target to source
          //
          if (deformationMapFile->getDeformBothWays()) {
             //
             // set the inverse deformation flag for target to source
             //
             deformationMapFile->setInverseDeformationFlag(true);

             //
             // Create the deformed spec file for the target data deformed to the source
             //
             deformationMapFile->setOutputSpecFileName(targetToSourceSpecFileName);

             //
             // Create the node deformation part of the deformation map
             //
             createNodeDeformation(targetSurface, deformedSourceSurface, deformationMapFile);

             switch (deformationMapFile->getFlatOrSphereSelection()) {
                case DeformationMapFile::DEFORMATION_TYPE_FLAT:
                   break;
                case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
                case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
                case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
                   //
                   // Create the deformation field
                   //
                   createAtlasIndivDeformationFieldFile(targetSurface, sourceSurface, deformedSourceSurface);
                   break;
             }

             //
             // Copy and set the name of the target to source spec file
             //
             SpecFile deformedToSourceSpecFile;
             QDir::setCurrent(sourceDirectory);
             try {
                deformedToSourceSpecFile.readFile(deformationMapFile->getSourceSpecFileName());
             }
             catch (FileException& /*e*/) {
                throw BrainModelAlgorithmException(
                            "Error reading source spec file for creating deformed version.");
             }
             try {
                deformedToSourceSpecFile.writeFile(targetToSourceSpecFileName);
             }
             catch (FileException& /*e*/) {
                throw BrainModelAlgorithmException(
                            "Error writing source spec file for creating deformed version.");
             }
             sourceBrainSet->setSpecFileName(targetToSourceSpecFileName);
             QDir::setCurrent(originalDirectory);

             //
             // Add the deformed source coordinate file to the spec file
             //
             switch(deformationMapFile->getFlatOrSphereSelection()) {
                case DeformationMapFile::DEFORMATION_TYPE_FLAT:
                   deformedToSourceSpecFile.addToSpecFile(
                      SpecFile::getFlatCoordFileTag(),
                      deformationMapFile->getSourceDeformedFlatCoordFileName(),
                      "",
                      true);
                   break;
                case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
                case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
                case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
                   deformedToSourceSpecFile.addToSpecFile(
                      SpecFile::getSphericalCoordFileTag(),
                      deformationMapFile->getSourceDeformedSphericalCoordFileName(),
                      "",
                      true);
                   break;
             }

             //
             // Swap source and target in deformation map file since now doing target to source
             //
             deformationMapFile->swapSourceAndTargetFiles();

             //
             // Write the deformation map file
             //
             QDir::setCurrent(sourceDirectory);
             deformationMapFile->writeFile(targetToSourceDeformationMapFileName);
             sourceBrainSet->setDeformationMapFileName(targetToSourceDeformationMapFileName, true);
             QDir::setCurrent(originalDirectory);

             //
             // Add to recent spec files
             //
             PreferencesFile* pf = brainSet->getPreferencesFile();
             pf->addToRecentSpecFiles(targetToSourceSpecFileName, true);

             //
             // Deform the target data files to the source
             //
             deformDataFiles(targetBrainSet,
                             sourceBrainSet,
                             deformationMapFile->getSourceSpecFileName(),  // have source since
                             deformationMapFile,                     // swapSourceAndTarget was called
                             false,
                             deformSourceFiducialCoordFiles,
                             deformSourceInflatedCoordFiles,
                             deformSourceVeryInflatedCoordFiles,
                             deformSourceSphericalCoordFiles,
                             deformSourceFlatCoordFiles,
                             targetToSourceDeformDataFileErrors);
          }
      }

      //
      // Delete the intermediate files
      //
      if (DebugControl::getDebugOn()) {
         std::cout << "Current directory: " << QDir::currentPath().toAscii().constData() << std::endl;
      }
      QDir::setCurrent(originalPath);
      if (deformationMapFile->getDeleteIntermediateFiles()) {
         for (int i = 0; i < static_cast<int>(intermediateFiles.size()); i++) {
            QFile::remove(intermediateFiles[i]);
         }
      }
   
      //
      // Remove the progress dialog
      //
      removeProgressDialog();
   }
   catch (BrainModelAlgorithmException& e) {
      //
      // Remove the progress dialog
      //
      removeProgressDialog();
      
      throw e;
   }
}

      
