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


#ifndef __BRAIN_MODEL_SURFACE_DEFORMATION_H__
#define __BRAIN_MODEL_SURFACE_DEFORMATION_H__

#include "BrainModelAlgorithm.h"
#include "BrainModelSurface.h"

class DeformationMapFile;

/// class for deforming a surface to another surface
class BrainModelSurfaceDeformation : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceDeformation(BrainSet* brainSetIn,
                                   DeformationMapFile* deformationMapFileIn);
                                   
      /// Destructor
      virtual ~BrainModelSurfaceDeformation();
      
      /// Execute the deformation
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// get deform source fiducial coord files
      bool getDeformSourceFiducialCoordFiles() const { return deformSourceFiducialCoordFiles; }
      
      /// set deform source fiducial coord files
      void setDeformSourceFiducialCoordFiles(const bool b) { deformSourceFiducialCoordFiles = b; }
      
      /// get deform source inflated coord files
      bool getDeformSourceInflatedCoordFiles() const { return deformSourceInflatedCoordFiles; }
      
      /// set deform source inflated coord files
      void setDeformSourceInflatedCoordFiles(const bool b) { deformSourceInflatedCoordFiles = b; }
      
      /// get deform source very inflated coord files
      bool getDeformSourceVeryInflatedCoordFiles() const { return deformSourceVeryInflatedCoordFiles; }
      
      /// set deform source very inflated coord files
      void setDeformSourceVeryInflatedCoordFiles(const bool b) { deformSourceVeryInflatedCoordFiles = b; }
      
      /// get deform source spherical coord files
      bool getDeformSourceSphericalCoordFiles() const { return deformSourceSphericalCoordFiles; }
      
      /// set deform source spherical coord files
      void setDeformSourceSphericalCoordFiles(const bool b) { deformSourceSphericalCoordFiles = b; }
      
      /// get deform source flat coord files
      bool getDeformSourceFlatCoordFiles() const { return deformSourceFlatCoordFiles; }
      
      /// set deform source flat coord files
      void setDeformSourceFlatCoordFiles(const bool b) { deformSourceFlatCoordFiles = b; }
      
      // get deform source flat lobar coord files
      //bool getDeformSourceFlatLobarCoordFiles() const { return deformSourceFlatLobarCoordFiles; }
      
      // set deform source flat lobar coord files
      //void setDeformSourceFlatLobarCoordFiles(const bool b) { deformSourceFlatLobarCoordFiles = b; }
            
      /// get the source to target deform data file errors message
      QString getSourceToTargetDeformDataFileErrors() const 
                         { return sourceToTargetDeformDataFileErrors; }
      
      /// get the target to source deform data file errors message
      QString getTargetToSourceDeformDataFileErrors() const 
                         { return targetToSourceDeformDataFileErrors; }

      /// deform the data files
      static void deformDataFiles(BrainSet* sourceBrain,
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
                     throw (BrainModelAlgorithmException);

      /// set the surface that has viewing transformations that should be used
      void setsurfaceWithViewingTransformations(BrainModelSurface* bms);

      /// set the deforomation map file names (overrides defaults)
      void setDeformationMapFileNames(const QString& indivToAtlasDefMapFileName,
                                      const QString& atlasToIndivDefMapFileName);

      /// get the source brain set
      BrainSet* getSourceBrainSet() { return this->sourceBrainSet; }

      /// get the target brain set
      BrainSet* getTargetBrainSet() { return this->targetBrainSet; }

      /// get should data files be deformed
      bool getDeformDataFilesStatus() { return deformDataFilesFlag; }

      /// set data files be deformed
      void setDeformDataFilesStatus(bool status) { deformDataFilesFlag = status; }

   protected:
      /// Execute the subclass' deformation
      virtual void executeDeformation() throw (BrainModelAlgorithmException) = 0;

      /// update the viewing transformation for surface
      void updateViewingTransformation(BrainModelSurface* bms);

      /// update the viewing transformation for surfaces in brain set
      void updateViewingTransformation(BrainSet* bs);

      /// Create the deformation that maps target surface nodes into the deformed source surface.
      void createNodeDeformation(const BrainModelSurface* theSourceSurfaceDeformed,
                                 const BrainModelSurface* theTargetSurface,
                                 DeformationMapFile* theDeformationMap);
                                
      /// Check a border file to see if it might be screwed up
      void checkSphericalBorder(const BrainModelSurface* bms,
                                const BorderFile* bf,
                                const QString indivTargetName) const;
      
      /// Create the indiv-to-atlas deformation field
      void createIndivAtlasDeformationFieldFile(const BrainModelSurface* surface,
                                      const BrainModelSurface* deformedSurface);
                                                      
      /// Create the atlas-to-indiv deformation field
      void createAtlasIndivDeformationFieldFile(
                                      const BrainModelSurface* atlasSphereSurface,
                                      const BrainModelSurface* surface,
                                      const BrainModelSurface* deformedSurface);
                                                      
      /// create the output spec and deformation file names
      void createOutputSpecAndDeformationFileNames();
      
      /// resample the border files
      void resampleBorderFiles(const int stageNumber,
                               const int cycleNumber,
                               float sphericalRadius) throw (BrainModelAlgorithmException);
      
      /// project the border file for the brain set to create other types of border files
      void projectBorderFile(BrainSet* theBrainSet,
                             const BrainModelSurface::SURFACE_TYPES borderType,
                             const QString& sourceTargetString)
                                          throw (BrainModelAlgorithmException);
      /// read the source brain set
      void readSourceBrainSet() throw (BrainModelAlgorithmException);

      /// read the target brain set
      void readTargetBrainSet(int stageIndex) throw (BrainModelAlgorithmException);

      /// the deformation map file (do not delete)
      DeformationMapFile* deformationMapFile;
      
      /// directory deformation run from
      QString originalDirectory;
      
      /// the source brain set
      BrainSet* sourceBrainSet;
      
      /// the target brain set
      BrainSet* targetBrainSet;
      
      /// source fiducial coord file (do not delete)
      BrainModelSurface* sourceFiducialSurface;
      
      /// source coordinate file (do not delete)
      BrainModelSurface* sourceSurface;
      
      /// deformed source coordinate file (do not delete)
      BrainModelSurface* deformedSourceSurface;
      
      /// source topology file (do not delete)
      TopologyFile* sourceTopologyFile;
      
      /// source border file (do not delete)
      BorderFile* sourceBorderFile;

      /// directory containing source files
      QString sourceDirectory;
      
      /// name of source resampled border file
      QString sourceBorderResampledName;
      
      /// source to target deformation spec file name
      QString sourceToTargetSpecFileName;
      
      /// source to target deformation map file name
      QString sourceToTargetDeformationMapFileName;
      
      /// target fiducial coord file (do not delete)
      BrainModelSurface* targetFiducialSurface;
      
      /// target coordinate file (do not delete)
      BrainModelSurface* targetSurface;
      
      /// target topology file (do not delete)
      TopologyFile* targetTopologyFile;
      
      /// target border file (do not delete)
      BorderFile* targetBorderFile;
      
      /// directory containing target files
      QString targetDirectory;
      
      /// name of target resampled border file
      QString targetBorderResampledName;
      
      /// target to source deformation spec file name
      QString targetToSourceSpecFileName;
      
      /// target to source deformation map file name
      QString targetToSourceDeformationMapFileName;
      
      /// keeps track of intermediate files
      std::vector<QString> intermediateFiles;
      
      /// keeps source border from being flipped multiple times
      bool sourceBorderFlippedForDifferentHemispheres;
      
      /// deform source fiducial coord files
      bool deformSourceFiducialCoordFiles;
      
      /// deform source inflated coord files
      bool deformSourceInflatedCoordFiles;
      
      /// deform source very inflated coord files
      bool deformSourceVeryInflatedCoordFiles;
      
      /// deform source spherical coord files
      bool deformSourceSphericalCoordFiles;
      
      /// deform source flat coord files
      bool deformSourceFlatCoordFiles;
      
      // deform source flat lobar coord files
      //bool deformSourceFlatLobarCoordFiles;
      
      /// source to target deform data file errors message
      QString sourceToTargetDeformDataFileErrors;
      
      /// target to source deform data file errors message
      QString targetToSourceDeformDataFileErrors;

      /// translation for default view
      float defaultViewTranslation[3];

      /// scaling for default view
      float defaultViewScaling[3];

      /// rotation for default view
      float defaultViewRotation[16];

      /// default view valid
      bool defaultViewValid;

      /// should data files be deformed
      bool deformDataFilesFlag;
};

#endif // __BRAIN_MODEL_SURFACE_DEFORMATION_H__

