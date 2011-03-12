#include <QDir>

#include "ArealEstimationFile.h"   
#include "BorderFileProjector.h"
#include "BorderProjectionUnprojector.h"
#include "BrainSet.h"
#include "BrainModelSurfaceDeformDataFile.h"
#include "BrainModelSurfacePointProjector.h"
#include "CellFile.h"
#include "CellFileProjector.h"
#include "CellProjectionFile.h"
#include "CellProjectionUnprojector.h"
#include "CoordinateFile.h"
#include "DateAndTime.h"
#include "DeformationMapFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "LatLonFile.h"
#include "FileUtilities.h"
#include "MetricFile.h"   
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "RgbPaintFile.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopographyFile.h"
#include "TopologyFile.h"

/*
#include "SpecFile.h"
*/

/**
 * create a deformed spec file name
 */
QString
BrainModelSurfaceDeformDataFile::createDeformedSpecFileName(
                                        const QString& deformedFileNamePrefix,
                                        const QString& srcSpecFileName,
                                        const QString& tgtSpecFileName)
{
   QString outputName;

   QString srcDirectory, srcSpecies, srcCasename, srcAnatomy, srcHemisphere,
           srcDescription, srcDescriptionNoType, srcTheDate, srcNumNodes,
           srcExtension;

   //
   // Parse the source spec file name
   //
   bool srcSpecFileNameValid = false;
   if (srcSpecFileName.isEmpty() == false) {
      srcSpecFileNameValid =
       FileUtilities::parseCaretDataFileName(srcSpecFileName,
                                             srcDirectory,
                                             srcSpecies,
                                             srcCasename,
                                             srcAnatomy,
                                             srcHemisphere,
                                             srcDescription,
                                             srcDescriptionNoType,
                                             srcTheDate,
                                             srcNumNodes,
                                             srcExtension);
   }

   QString tgtDirectory, tgtSpecies, tgtCasename, tgtAnatomy, tgtHemisphere,
           tgtDescription, tgtDescriptionNoType, tgtTheDate, tgtNumNodes,
           tgtExtension;

   //
   // Parse the target spec file name
   //
   bool tgtSpecFileNameValid = false;
   if (tgtSpecFileName.isEmpty() == false) {
      tgtSpecFileNameValid =
       FileUtilities::parseCaretDataFileName(tgtSpecFileName,
                                             tgtDirectory,
                                             tgtSpecies,
                                             tgtCasename,
                                             tgtAnatomy,
                                             tgtHemisphere,
                                             tgtDescription,
                                             tgtDescriptionNoType,
                                             tgtTheDate,
                                             tgtNumNodes,
                                             tgtExtension);
   }

   //
   // If both files are parsed successfully
   //
   if (srcSpecFileNameValid && tgtSpecFileNameValid) {
      //
      // Assemble the spec file name
      //
      std::vector<QString> nameComponents;
      nameComponents.push_back(deformedFileNamePrefix);
      nameComponents.push_back(srcSpecies);
      nameComponents.push_back(srcCasename);
      nameComponents.push_back(srcAnatomy);
      nameComponents.push_back(srcHemisphere);
      nameComponents.push_back(srcDescription);
      nameComponents.push_back(tgtNumNodes);
      nameComponents.push_back(SpecFile::getDeformationMapFileExtension());

      outputName = "";
      if (tgtDirectory.isEmpty() == false) {
         //outputName.append(tgtDirectory);
         //outputName.append("/");
      }
      for (unsigned int i = 0; i < nameComponents.size(); i++) {
         QString s = nameComponents[i];
         if (s.isEmpty() == false) {
            if (s.startsWith(".") == false) {
               if (outputName.isEmpty() == false) {
                  if (i == 1) {
                     if (outputName.endsWith("_") == false) {
                        outputName.append(".");
                     }
                  }
                  else {
                     outputName.append(".");
                  }
               }
            }
            outputName.append(s);
         }
      }
   }
   else {
      //
      // just prepend destination directory and deformed file prefix
      //
      outputName = "";
      //outputName = FileUtilities::dirname(tgtSpecFileName);
      //if (outputName.isEmpty() == false) {
      //   outputName.append("/");
      //}
      outputName.append(deformedFileNamePrefix);
      if (srcSpecFileName.isEmpty() == false) {
         QString specName = FileUtilities::basename(srcSpecFileName);
         if (specName.endsWith(SpecFile::getSpecFileExtension())) {
         outputName.append(specName.left(specName.length() -
                                    SpecFile::getSpecFileExtension().length()));
         }
         else {
            outputName.append(srcSpecFileName);
         }
      }
      outputName.append(SpecFile::getDeformationMapFileExtension());
   }

   return outputName;
}

/**
 * Create the deformation map file name.
 */
QString
BrainModelSurfaceDeformDataFile::createDeformedFileName(const QString& deformingFromFileName,
                                                        const QString& mySpecFileName,
                                                        const QString& deformedFilePrefix,
                                                        const int numberOfNodes,
                                                        const bool deformationMapFileNameFlag)
{
   QString outputName;
   
   QString directory, species, casename, anatomy, hemisphere, description,
               descriptionNoType, theDate, numNodes, extension;
  
   //
   // Parse the deforming from file
   //
   const bool deformingFromFileNameValid = 
       FileUtilities::parseCaretDataFileName(deformingFromFileName,
                                             directory,
                                             species,
                                             casename,
                                             anatomy,
                                             hemisphere,
                                             description,
                                             descriptionNoType,
                                             theDate,
                                             numNodes,
                                             extension);      
   
   QString myDirectory, mySpecies, myCasename, myAnatomy, myHemisphere, myDescription,
               myDescriptionNoType, myTheDate, myNumNodes, myExtension;
      
   //
   // Parse the deforming to spec file
   //
   const bool mySpecFileNameValid = 
       FileUtilities::parseCaretDataFileName(mySpecFileName,
                                             myDirectory,
                                             mySpecies,
                                             myCasename,
                                             myAnatomy,
                                             myHemisphere,
                                             myDescription,
                                             myDescriptionNoType,
                                             myTheDate,
                                             myNumNodes,
                                             myExtension);
   
   //
   // Sometimes the output spec file does not contain the number of nodes
   //
   if (myNumNodes.isEmpty()) {
      myNumNodes = QString::number(numberOfNodes);
   }
   
   //
   // If both files are parsed successfully
   //
   if (deformingFromFileNameValid && mySpecFileNameValid) {
      //
      // Deformation Map File get todays date and time
      //
      QString dateString;
      if (deformationMapFileNameFlag) {
         //dateString = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm");
         dateString = DateAndTime::getDateAndTimeForNaming();
         extension = SpecFile::getDeformationMapFileExtension();
      }
      else {
         dateString = theDate;
      }
      
      //
      // Prepend the deformed file prefix to the species
      //
      QString prefixAndSpecies(deformedFilePrefix);
      prefixAndSpecies.append(species);
      
      //
      // Reassemble the file name
      //
      outputName = FileUtilities::reassembleCaretDataFileName(myDirectory,
                                                              prefixAndSpecies,
                                                              casename,
                                                              anatomy,
                                                              hemisphere,
                                                              description,
                                                              dateString,
                                                              myNumNodes,
                                                              extension);
   }
   else {
      //
      // just prepend destination directory and deformed file prefix
      //
      outputName = FileUtilities::dirname(mySpecFileName);
      if (outputName.isEmpty() == false) {
         outputName.append("/");
      }
      outputName.append(deformedFilePrefix);
      outputName.append(FileUtilities::basename(deformingFromFileName));
      //outputName.append(SpecFile::getDeformationMapFileExtension());
      if (deformationMapFileNameFlag) {
         outputName.append(SpecFile::getDeformationMapFileExtension());
      }
   }
   
   return outputName;
}

/**
 * deform node attribute data files
 */
void 
BrainModelSurfaceDeformDataFile::deformNodeAttributeFiles(const DeformationMapFile* dmf,
                                             const DATA_FILE_TYPE dataFileType,
                                             const SpecFile::Entry& dataFiles,
                                             QString& deformErrorsMessage)
                                       throw (BrainModelAlgorithmException)
{
   for (unsigned int i = 0; i < dataFiles.files.size(); i++) {
      try {
         deformNodeAttributeFile(dmf, dataFileType, true, dataFiles.files[i].filename);
      }
      catch (BrainModelAlgorithmException& e) {
         deformErrorsMessage.append(e.whatQString());
         deformErrorsMessage.append("\n");
      }
   }
}

/**
 * deform node attribute data files
 */
void 
BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFiles(const DeformationMapFile* dmf,
                                             const DATA_FILE_TYPE dataFileType,
                                             const SpecFile::Entry& dataFiles,
                                             QString& deformErrorsMessage)
                                       throw (BrainModelAlgorithmException)
{
   for (unsigned int i = 0; i < dataFiles.files.size(); i++) {
      try {
         deformGiftiNodeDataFile(dmf, dataFileType, true, dataFiles.files[i].filename);
      }
      catch (BrainModelAlgorithmException& e) {
         deformErrorsMessage.append(e.whatQString());
         deformErrorsMessage.append("\n");
      }
   }
}

/**
 * deform border files
 */
void
BrainModelSurfaceDeformDataFile::deformBorderFiles(BrainSet* sourceBrainSet,
                                                   BrainSet* targetBrainSet,
                                                   const DeformationMapFile* dmf,
                                                   const DATA_FILE_TYPE dataFileType,
                                                   const SpecFile::Entry& dataFiles,
                                                   QString& deformErrorsMessage)
                                       throw (BrainModelAlgorithmException)
{
   for (unsigned int i = 0; i < dataFiles.files.size(); i++) {
      try {
         deformBorderFile(sourceBrainSet, targetBrainSet, dmf, true, dataFileType, dataFiles.files[i].filename);
      }
      catch (BrainModelAlgorithmException& e) {
         deformErrorsMessage.append(e.whatQString());
         deformErrorsMessage.append("\n");
      }
   }
}

/**
 * Add comment information about deformation.
 */
void
BrainModelSurfaceDeformDataFile::addCommentAboutDeformation(const DeformationMapFile& dmf,
                                                            const AbstractFile* inputFile,
                                                            AbstractFile* outputFile)
{
   if ((inputFile != NULL) && (outputFile != NULL)) {
      outputFile->setFileTitle(inputFile->getFileTitle());
      QString comment("Deformed from: ");
      comment.append(FileUtilities::basename(inputFile->getFileName()));
      comment.append("\n");
      comment.append("Deformed with: ");
      comment.append(FileUtilities::basename(dmf.getFileName()));
      comment.append("\n");
      comment.append(inputFile->getFileComment());
      outputFile->setFileComment(comment);
   }
}
 
/**
 * Get a string containing names of surfaces loaded.
 */
QString 
BrainModelSurfaceDeformDataFile::getLoadedSurfaces(BrainSet* bs) 
{
   QString namesOut;
   
   int numModels = bs->getNumberOfBrainModels();
   for (int i = 0; i < numModels; i++) {
      BrainModelSurface* bms = bs->getBrainModelSurface(i);
      if (bms != NULL) {
         if (namesOut.isEmpty()) {
            namesOut += "\nSurfaces Loaded: ";
         }
         else {
            namesOut += "\n";
         }
         namesOut += bms->getCoordinateFile()->getFileName();
      }
   }
   return namesOut;
}

/**
 * deform a border file
 */
void
BrainModelSurfaceDeformDataFile::deformBorderFile(BrainSet* sourceBrainSet,
                                                  BrainSet* targetBrainSet,
                                                  const DeformationMapFile* dmf,
                                                  const bool useSourceTargetPathsFlag,
                                                  const DATA_FILE_TYPE dataFileType,
                                                  const QString& dataFileName,
                                                  const QString& outputFileNameIn)
                                       throw (BrainModelAlgorithmException)
{
   AbstractFile* inputFile = NULL;
   
   //
   // Save current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to source directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getSourceDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getSourceDirectory());
      }
      else if (dmf->getSourceSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getSourceSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getSourceSpecFileName()));
         }
      }
   }
   
   //
   // Find the source surface's
   //
   BrainModelSurface* sourceFlatSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceFlatCoordFileName());
   BrainModelSurface* sourceSphericalSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceSphericalCoordFileName());
   //if (sourceSurface == NULL) {
   //   throw BrainModelAlgorithmException("Unable to find source surface for deforming border file.");
   //}
   
   //
   // Read in the border or border projection file that is to be deformed.
   //
   BorderFile sourceBorderFile;
   BorderProjectionFile sourceBorderProjectionFile;
   BrainModelSurface* sourceBorderSurface = NULL;
   bool haveBorderFile = false;
   bool haveBorderProjectionFile = false;
   try {
      switch(dataFileType) {
         case DATA_FILE_AREAL_ESTIMATION:
            break;
         case DATA_FILE_ATLAS:
            break;
         case DATA_FILE_BORDER_FLAT:
            sourceBorderFile.readFile(dataFileName);
            sourceBorderSurface = sourceFlatSurface;
            haveBorderFile = true;
            inputFile = &sourceBorderFile;
            break;
         case DATA_FILE_BORDER_SPHERICAL:
            sourceBorderFile.readFile(dataFileName);
            sourceBorderSurface = sourceSphericalSurface;
            haveBorderFile = true;
            inputFile = &sourceBorderFile;
            break;
         case DATA_FILE_BORDER_PROJECTION:
            sourceBorderProjectionFile.readFile(dataFileName);
            haveBorderProjectionFile = true;
            inputFile = &sourceBorderProjectionFile;
            break;
         case DATA_FILE_CELL:
            break;
         case DATA_FILE_CELL_PROJECTION:
            break;
         case DATA_FILE_COORDINATE:
            break;
         case DATA_FILE_COORDINATE_FLAT:
            break;
         case DATA_FILE_FOCI:
            break;
         case DATA_FILE_FOCI_PROJECTION:
            break;
         case DATA_FILE_LAT_LON:
            break;
         case DATA_FILE_METRIC:
            break;
         case DATA_FILE_SHAPE:
            break;
         case DATA_FILE_PAINT:
            break;
         case DATA_FILE_RGB_PAINT:
            break;
         case DATA_FILE_TOPOGRAPHY:
            break;
      }
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   const QString sourceSurfaceNames = getLoadedSurfaces(sourceBrainSet);
   const QString targetSurfaceNames = getLoadedSurfaces(targetBrainSet);

   //
   // If the input file is a border file, make it into a border projection file
   //
   if (haveBorderFile) {
      if (sourceBorderSurface == NULL) {
         if (dataFileType == DATA_FILE_BORDER_FLAT) {
            QString msg("Unable to find flat surface for flat border file ");
            msg.append(dmf->getSourceFlatCoordFileName());
            throw BrainModelAlgorithmException(msg);
         }
         else {
            QString msg("Unable to find spherical surface for spherical border file ");
            msg.append(dmf->getSourceSphericalCoordFileName());
            throw BrainModelAlgorithmException(msg);
         }
      }
      //
      // Create a border file projector and project the border file
      //
      BorderFileProjector bfp(sourceBorderSurface, true);
      bfp.projectBorderFile(&sourceBorderFile, &sourceBorderProjectionFile, NULL);
   }
   else if (haveBorderProjectionFile == false) {
      throw BrainModelAlgorithmException("Unsupported file type for border file deformation.");
   }
   
   //
   // Find the deformed source surface which will exist if this was an indvidual to atlas deformation
   //
   QString unprojectSourceSurfaceName;
   BrainModelSurface* unprojectSourceSurface = NULL;
   if (dmf->getInverseDeformationFlag() == false) {
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            unprojectSourceSurfaceName = dmf->getSourceDeformedFlatCoordFileName();
            unprojectSourceSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getSourceDeformedFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            unprojectSourceSurfaceName = dmf->getSourceDeformedSphericalCoordFileName();
            unprojectSourceSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceDeformedSphericalCoordFileName());
            break;
      }
   }
   else {
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            unprojectSourceSurfaceName = dmf->getSourceFlatCoordFileName();
            unprojectSourceSurface = sourceFlatSurface;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            unprojectSourceSurfaceName = dmf->getSourceSphericalCoordFileName();
            unprojectSourceSurface = sourceSphericalSurface;
            break;
      }
   }
   if (unprojectSourceSurface == NULL) {
      QString msg("Missing source surface for border file deformation unprojection: ");
      msg.append(unprojectSourceSurfaceName);
      msg.append(sourceSurfaceNames);
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Unproject source border projection file onto the flat or spherical source surface
   //
   sourceBorderFile.clear();
   BorderProjectionUnprojector sbfu;
   sbfu.unprojectBorderProjections(*(unprojectSourceSurface->getCoordinateFile()),
                                   sourceBorderProjectionFile,
                                   sourceBorderFile);

   //
   // Target's border projection file
   //
   BorderProjectionFile targetBorderProjectionFile;
   
   //
   // if Atlas to Individual deformation
   //
   BrainModelSurface* targetProjectSurface = NULL;
   QString targetProjectSurfaceName;
   QString msg1 = "";
   if (dmf->getInverseDeformationFlag()) {
      //
      // Use the deformed surface for projection
      //
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            targetProjectSurfaceName = dmf->getSourceDeformedFlatCoordFileName();
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getSourceDeformedFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            targetProjectSurfaceName = dmf->getSourceDeformedSphericalCoordFileName();
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceDeformedSphericalCoordFileName());
            break;
      }
      msg1 = sourceSurfaceNames;
   }
   else {
      //
      // Use the non-deformed surface for projection
      //
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            targetProjectSurfaceName = dmf->getTargetFlatCoordFileName();
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getTargetFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            targetProjectSurfaceName = dmf->getTargetSphericalCoordFileName();
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getTargetSphericalCoordFileName());
            break;
      }
      msg1 = targetSurfaceNames;
   }
   if (targetProjectSurface == NULL) {
      QString msg("Unable to find target surface for projection of borders: ");
      msg.append(targetProjectSurfaceName);
      msg.append(msg1);
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Project the border onto the target surface
   //
   BorderFileProjector bfp(targetProjectSurface, true);
   bfp.projectBorderFile(&sourceBorderFile, &targetBorderProjectionFile, NULL);
   
   //
   // Set to the target directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getTargetDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getTargetDirectory());
      }
      else if (dmf->getOutputSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getOutputSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getOutputSpecFileName()));
         }
      }
   }
   
   //
   // Create the deformed file name
   //
   QString outputFileName;
   if (outputFileNameIn.isEmpty()) {
      outputFileName =
         FileUtilities::basename(
            createDeformedFileName(dataFileName, 
                                 dmf->getOutputSpecFileName(),
                                 dmf->getDeformedFileNamePrefix(),
                                 dmf->getNumberOfNodes(),
                                 false));
   }
   else {
      outputFileName = outputFileNameIn;
   }

   //
   // Use the border projection file as needed
   //
   QString specFileTag;   
   switch(dataFileType) {
      case DATA_FILE_AREAL_ESTIMATION:
         break;
      case DATA_FILE_ATLAS:
         break;
      case DATA_FILE_BORDER_FLAT:
         {
            //
            // Unproject onto the flat surface
            //
            BrainModelSurface* bms = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                     dmf->getTargetFlatCoordFileName());
            if (bms == NULL) {
               QString msg("Unable to find target flat coord file for border unprojection: ");
               msg.append(dmf->getTargetFlatCoordFileName());
               msg.append(targetSurfaceNames);
               throw BrainModelAlgorithmException(msg);
            }
            BorderProjectionUnprojector tbfu;
            BorderFile targetFlatBorderFile;
            sbfu.unprojectBorderProjections(*(bms->getCoordinateFile()),
                                          targetBorderProjectionFile,
                                          targetFlatBorderFile);
            addCommentAboutDeformation(*dmf, inputFile, &targetFlatBorderFile);
            targetFlatBorderFile.writeFile(outputFileName);
            specFileTag = SpecFile::getFlatBorderFileTag();
         }
         break;
      case DATA_FILE_BORDER_SPHERICAL:
         {
            //
            // Unproject onto the spherical surface
            //
            BrainModelSurface* bms = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                     dmf->getTargetSphericalCoordFileName());
            if (bms == NULL) {
               QString msg("Unable to find target spherical coord file for border unprojection: ");
               msg.append(dmf->getTargetSphericalCoordFileName());
               msg.append(targetSurfaceNames);
               throw BrainModelAlgorithmException(msg);
            }
            BorderProjectionUnprojector tbfu;
            BorderFile targetSphericalBorderFile;
            sbfu.unprojectBorderProjections(*(bms->getCoordinateFile()),
                                          targetBorderProjectionFile,
                                          targetSphericalBorderFile);
            addCommentAboutDeformation(*dmf, inputFile, &targetSphericalBorderFile);
            targetSphericalBorderFile.writeFile(outputFileName);
            specFileTag = SpecFile::getSphericalBorderFileTag();
         }
         break;
      case DATA_FILE_BORDER_PROJECTION:
         //
         // Use the projection file as it is
         //
         addCommentAboutDeformation(*dmf, inputFile, &targetBorderProjectionFile);
         targetBorderProjectionFile.writeFile(outputFileName);
         specFileTag = SpecFile::getBorderProjectionFileTag();
         break;
      case DATA_FILE_CELL:
         break;
      case DATA_FILE_CELL_PROJECTION:
         break;
      case DATA_FILE_COORDINATE:
         break;
      case DATA_FILE_COORDINATE_FLAT:
         break;
      case DATA_FILE_FOCI:
         break;
      case DATA_FILE_FOCI_PROJECTION:
         break;
      case DATA_FILE_LAT_LON:
         break;
      case DATA_FILE_METRIC:
         break;
      case DATA_FILE_SHAPE:
         break;
      case DATA_FILE_PAINT:
         break;
      case DATA_FILE_RGB_PAINT:
         break;
      case DATA_FILE_TOPOGRAPHY:
         break;
   }
   
   if (useSourceTargetPathsFlag) {
      if (specFileTag.isEmpty() == false) {
         //
         // Update the target spec file
         //
         QFileInfo specFileInfo(dmf->getOutputSpecFileName());
         if (specFileInfo.isFile()) {
            SpecFile sf;
            try {
               sf.readFile(dmf->getOutputSpecFileName());
               //
               // file will be written if file is new to spec file
               //
               sf.addToSpecFile(specFileTag, outputFileName, "", true);
            }
            catch(FileException& e) {
               QDir::setCurrent(savedDirectory);
               QString msg("File successfully deformed.  However, unable to update spec file:\n");
               msg.append(dmf->getOutputSpecFileName());
               throw BrainModelAlgorithmException(msg);
            }
         }
      }
   }
   
   //
   // Reset to the original directory
   //
   QDir::setCurrent(savedDirectory);
}
   
/**
 * deform a node attribute data file
 */
void 
BrainModelSurfaceDeformDataFile::deformNodeAttributeFile(const DeformationMapFile* dmf,
                                                         const DATA_FILE_TYPE dataFileType,
                                                         const bool useSourceTargetPathsFlag,
                                                         const QString& dataFileName,
                                                         const QString& outputFileNameIn)
                                       throw (BrainModelAlgorithmException)
{
   //
   // Save current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to source directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getSourceDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getSourceDirectory());
      }
      else if (dmf->getSourceSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getSourceSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getSourceSpecFileName()));
         }
      }
   }
   
   //
   // Default deformation type to nearest node
   //
   NodeAttributeFile::DEFORM_TYPE dt = NodeAttributeFile::DEFORM_NEAREST_NODE;
     
   //
   // Read input file and create output file
   //
   NodeAttributeFile* inputFile  = NULL;
   NodeAttributeFile* outputFile = NULL;
   QString specFileTag;
   
   switch(dataFileType) {
      case DATA_FILE_AREAL_ESTIMATION:
         inputFile  = new ArealEstimationFile;
         outputFile = new ArealEstimationFile;
         specFileTag = SpecFile::getArealEstimationFileTag();
         break;
      case DATA_FILE_ATLAS:
         break;
      case DATA_FILE_BORDER_FLAT:
         break;
      case DATA_FILE_BORDER_SPHERICAL:
         break;
      case DATA_FILE_BORDER_PROJECTION:
         break;
      case DATA_FILE_CELL:
         break;
      case DATA_FILE_CELL_PROJECTION:
         break;
      case DATA_FILE_COORDINATE:
         break;
      case DATA_FILE_COORDINATE_FLAT:
         break;
      case DATA_FILE_FOCI:
         break;
      case DATA_FILE_FOCI_PROJECTION:
         break;
      case DATA_FILE_LAT_LON:
         inputFile = new LatLonFile;
         outputFile = new LatLonFile;
         specFileTag = SpecFile::getLatLonFileTag();
         break;
      case DATA_FILE_METRIC:
         break;
      case DATA_FILE_SHAPE:
         break;
      case DATA_FILE_PAINT:
         break;
      case DATA_FILE_RGB_PAINT:
         inputFile  = new RgbPaintFile;
         outputFile = new RgbPaintFile;
         specFileTag = SpecFile::getRgbPaintFileTag();
         break;
      case DATA_FILE_TOPOGRAPHY:
         inputFile  = new TopographyFile;
         outputFile = new TopographyFile;
         specFileTag = SpecFile::getTopographyFileTag();
         break;
   }
   if ((inputFile != NULL) && (outputFile != NULL)) {
      try {
         inputFile->readFile(dataFileName);
      }
      catch (FileException& e) {
         if (inputFile != NULL) delete inputFile;
         if (outputFile != NULL) delete outputFile;
         QDir::setCurrent(savedDirectory);
         throw BrainModelAlgorithmException(e.whatQString());
      }
   }
   else {
      if (inputFile != NULL) delete inputFile;
      if (outputFile != NULL) delete outputFile;
      QDir::setCurrent(savedDirectory);
      QString msg("Data file type not supported for node deformation for file ");
      msg.append(dataFileName);
      throw BrainModelAlgorithmException(msg);
   }

   //
   // Deform the data file
   //
   try {
      inputFile->deform(*dmf, *outputFile, dt);
   }
   catch (FileException& e) {
      if (inputFile != NULL) delete inputFile;
      if (outputFile != NULL) delete outputFile;
      QDir::setCurrent(savedDirectory);
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Update file comments
   //
   addCommentAboutDeformation(*dmf, inputFile, outputFile);
   
   //
   // Set to the target directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getTargetDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getTargetDirectory());
      }
      else if (dmf->getOutputSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getOutputSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getOutputSpecFileName()));
         }
      }
   }
   
   //
   // Create the deformed file name
   //
   QString outputFileName;
   if (outputFileNameIn.isEmpty()) {
      outputFileName =
         FileUtilities::basename(
            createDeformedFileName(dataFileName, 
                                dmf->getOutputSpecFileName(),
                                dmf->getDeformedFileNamePrefix(),
                                dmf->getNumberOfNodes(),
                                false));
   }
   else {
      outputFileName = outputFileNameIn;
   }

   //
   // Write the data file
   //
   try {
      outputFile->writeFile(outputFileName);
   }
   catch(FileException& e) {
      if (inputFile != NULL) delete inputFile;
      if (outputFile != NULL) delete outputFile;
      QDir::setCurrent(savedDirectory);
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Update the target spec file
   //
   if (useSourceTargetPathsFlag) {
      QFileInfo specFileInfo(dmf->getOutputSpecFileName());
      if (specFileInfo.isFile()) {
         SpecFile sf;
         try {
            sf.readFile(dmf->getOutputSpecFileName());
            //
            // file will be written if file is new to spec file
            //
            sf.addToSpecFile(specFileTag, outputFileName, "", true);
         }
         catch(FileException& e) {
            if (inputFile != NULL) delete inputFile;
            if (outputFile != NULL) delete outputFile;
            QDir::setCurrent(savedDirectory);
            QString msg("File successfully deformed.  However, unable to update spec file:\n");
            msg.append(dmf->getOutputSpecFileName());
            throw BrainModelAlgorithmException(msg);
         }
      }
   }
   
   //
   // Free memory
   //
   if (inputFile != NULL) delete inputFile;
   if (outputFile != NULL) delete outputFile;
   
   //
   // Reset to the original directory
   //
   QDir::setCurrent(savedDirectory);
}

/**
 * deform a nifti node data file
 */
void 
BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFile(const DeformationMapFile* dmf,
                                                         const DATA_FILE_TYPE dataFileType,
                                                         const bool useSourceTargetPathsFlag,
                                                         const QString& dataFileName,
                                                         const QString& outputFileNameIn)
                                       throw (BrainModelAlgorithmException)
{
   //
   // Save current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to source directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getSourceDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getSourceDirectory());
      }
      else if (dmf->getSourceSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getSourceSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getSourceSpecFileName()));
         }
      }
   }
   
   //
   // Default deformation type to nearest node
   //
   GiftiNodeDataFile::DEFORM_TYPE dt = GiftiNodeDataFile::DEFORM_NEAREST_NODE;
     
   //
   // Read input file and create output file
   //
   GiftiNodeDataFile* inputFile  = NULL;
   GiftiNodeDataFile* outputFile = NULL;
   QString specFileTag;
   
   switch(dataFileType) {
      case DATA_FILE_AREAL_ESTIMATION:
         break;
      case DATA_FILE_ATLAS:
         inputFile  = new ProbabilisticAtlasFile;
         outputFile = new ProbabilisticAtlasFile;
         specFileTag = SpecFile::getAtlasFileTag();
         break;
      case DATA_FILE_BORDER_FLAT:
         break;
      case DATA_FILE_BORDER_SPHERICAL:
         break;
      case DATA_FILE_BORDER_PROJECTION:
         break;
      case DATA_FILE_CELL:
         break;
      case DATA_FILE_CELL_PROJECTION:
         break;
      case DATA_FILE_COORDINATE:
         break;
      case DATA_FILE_COORDINATE_FLAT:
         break;
      case DATA_FILE_FOCI:
         break;
      case DATA_FILE_FOCI_PROJECTION:
         break;
      case DATA_FILE_LAT_LON:
         break;
      case DATA_FILE_METRIC:
         inputFile  = new MetricFile;
         outputFile = new MetricFile;
         specFileTag = SpecFile::getMetricFileTag();
         switch(dmf->getMetricDeformationType()) {
            case DeformationMapFile::METRIC_DEFORM_NEAREST_NODE:
               dt = GiftiNodeDataFile::DEFORM_NEAREST_NODE;
               break;
            case DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES:
               dt = GiftiNodeDataFile::DEFORM_TILE_AVERAGE;
               break;
         }
         break;
      case DATA_FILE_SHAPE:
         inputFile  = new SurfaceShapeFile;
         outputFile = new SurfaceShapeFile;
         specFileTag = SpecFile::getSurfaceShapeFileTag();
         break;
      case DATA_FILE_PAINT:
         inputFile  = new PaintFile;
         outputFile = new PaintFile;
         specFileTag = SpecFile::getPaintFileTag();
         break;
      case DATA_FILE_RGB_PAINT:
         break;
      case DATA_FILE_TOPOGRAPHY:
         break;
   }
   if ((inputFile != NULL) && (outputFile != NULL)) {
      try {
         inputFile->readFile(dataFileName);
      }
      catch (FileException& e) {
         if (inputFile != NULL) delete inputFile;
         if (outputFile != NULL) delete outputFile;
         QDir::setCurrent(savedDirectory);
         throw BrainModelAlgorithmException(e.whatQString());
      }
   }
   else {
      if (inputFile != NULL) delete inputFile;
      if (outputFile != NULL) delete outputFile;
      QDir::setCurrent(savedDirectory);
      QString msg("Data file type not supported for node deformation for file ");
      msg.append(dataFileName);
      throw BrainModelAlgorithmException(msg);
   }

   //
   // Deform the data file
   //
   try {
      inputFile->deform(*dmf, *outputFile, dt);
   }
   catch (FileException& e) {
      if (inputFile != NULL) delete inputFile;
      if (outputFile != NULL) delete outputFile;
      QDir::setCurrent(savedDirectory);
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Update file comments
   //
   addCommentAboutDeformation(*dmf, inputFile, outputFile);
   
   //
   // Set to the target directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getTargetDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getTargetDirectory());
      }
      else if (dmf->getOutputSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getOutputSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getOutputSpecFileName()));
         }
      }
   }
   
   //
   // Create the deformed file name
   //
   QString outputFileName;
   if (outputFileNameIn.isEmpty()) {
      outputFileName =
         FileUtilities::basename(
            createDeformedFileName(dataFileName, 
                                dmf->getOutputSpecFileName(),
                                dmf->getDeformedFileNamePrefix(),
                                dmf->getNumberOfNodes(),
                                false));
   }
   else {
      outputFileName = outputFileNameIn;
   }

   //
   // Write the data file
   //
   try {
      outputFile->writeFile(outputFileName);
   }
   catch(FileException& e) {
      if (inputFile != NULL) delete inputFile;
      if (outputFile != NULL) delete outputFile;
      QDir::setCurrent(savedDirectory);
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Update the target spec file
   //
   if (useSourceTargetPathsFlag) {
      QFileInfo specFileInfo(dmf->getOutputSpecFileName());
      if (specFileInfo.isFile()) {
         SpecFile sf;
         try {
            sf.readFile(dmf->getOutputSpecFileName());
            //
            // file will be written if file is new to spec file
            //
            sf.addToSpecFile(specFileTag, outputFileName, "", true);
         }
         catch(FileException& e) {
            if (inputFile != NULL) delete inputFile;
            if (outputFile != NULL) delete outputFile;
            QDir::setCurrent(savedDirectory);
            QString msg("File successfully deformed.  However, unable to update spec file:\n");
            msg.append(dmf->getOutputSpecFileName());
            throw BrainModelAlgorithmException(msg);
         }
      }
   }
   
   //
   // Free memory
   //
   if (inputFile != NULL) delete inputFile;
   if (outputFile != NULL) delete outputFile;
   
   //
   // Reset to the original directory
   //
   QDir::setCurrent(savedDirectory);
}

/**
 * deform cell or foci files
 */
void
BrainModelSurfaceDeformDataFile::deformCellOrFociFiles(BrainSet* sourceBrainSet,
                                                       BrainSet* targetBrainSet,
                                                       const DeformationMapFile* dmf,
                                                       const SpecFile::Entry& dataFiles,
                                                       const bool fociFileFlag,
                                                       QString& deformErrorsMessage)
                                                  throw (BrainModelAlgorithmException)
{
   for (unsigned int i = 0; i < dataFiles.files.size(); i++) {
      try {
         deformCellOrFociFile(sourceBrainSet,
                              targetBrainSet,
                              dmf,
                              true,
                              dataFiles.files[i].filename,
                              fociFileFlag);
      }
      catch (BrainModelAlgorithmException& e) {
         deformErrorsMessage.append(e.whatQString());
         deformErrorsMessage.append("\n");
      }
   }
}

/**
 * deform a cell or foci file
 */
void
BrainModelSurfaceDeformDataFile::deformCellOrFociFile(BrainSet* sourceBrainSet,
                                                      BrainSet* targetBrainSet,
                                                      const DeformationMapFile* dmf,
                                                      const bool useSourceTargetPathsFlag,
                                                      const QString& dataFileName,
                                                      const bool fociFileFlag,
                                                      const QString& outputFileNameIn)
                                                  throw (BrainModelAlgorithmException)
{
   //
   // Save current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to source directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getSourceDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getSourceDirectory());
      }
      else if (dmf->getSourceSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getSourceSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getSourceSpecFileName()));
         }
      }
   }
   
   //
   // Find the source surface's
   //
   BrainModelSurface* sourceFiducialSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceFiducialCoordFileName());
   if (sourceFiducialSurface == NULL) {
      QDir::setCurrent(savedDirectory);
      throw BrainModelAlgorithmException("Unable to find source fiducial surface for cell deformation.");
   }
   BrainModelSurface* sourceFlatSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceFlatCoordFileName());
   BrainModelSurface* sourceSphericalSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceSphericalCoordFileName());
   
   //
   // Read in the cell file
   //
   CellFile* sourceCellFile = NULL;
   if (fociFileFlag) {
      sourceCellFile = new FociFile;
   }
   else {
      sourceCellFile = new CellFile;
   }
   try {
      sourceCellFile->readFile(dataFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
             
   //
   // Project the cell file using the fiducial surface
   //
   CellProjectionFile sourceCellsProjected;
   sourceCellsProjected.appendFiducialCellFile(*sourceCellFile);
   CellFileProjector cfp(sourceFiducialSurface);
   cfp.projectFile(&sourceCellsProjected,
                       0,
                       CellFileProjector::PROJECTION_TYPE_ALL,
                       0.0,
                       true,
                       NULL);
             
   //
   // Find the deformed source surface which will exist if this was an indvidual to atlas deformation
   //
   BrainModelSurface* unprojectSourceSurface = NULL;
   if (dmf->getInverseDeformationFlag() == false) {
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            unprojectSourceSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getSourceDeformedFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            unprojectSourceSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceDeformedSphericalCoordFileName());
            break;
      }
   }
   else {
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            unprojectSourceSurface = sourceFlatSurface;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            unprojectSourceSurface = sourceSphericalSurface;
            break;
      }
   }
   if (unprojectSourceSurface == NULL) {
      throw BrainModelAlgorithmException("Missing source surface for cell file deformation unprojection.");
   }

   //
   // Unproject source cell projection file onto the flat or spherical source surface
   //
   sourceCellFile->clear();
   CellProjectionUnprojector scpu;
   scpu.unprojectCellProjections(sourceCellsProjected,
                                 unprojectSourceSurface,
                                 *sourceCellFile,
                                 0);

   //
   // Target's cell projection file
   //
   CellProjectionFile targetCellProjectionFile;
   
   //
   // if Atlas to Individual deformation
   //
   BrainModelSurface* targetProjectSurface = NULL;
   if (dmf->getInverseDeformationFlag()) {
      //
      // Use the deformed surface for projection
      //
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getSourceDeformedFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceDeformedSphericalCoordFileName());
            break;
      }
   }
   else {
      //
      // Use the non-deformed surface for projection
      //
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getTargetFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getTargetSphericalCoordFileName());
            break;
      }
   }
   if (targetProjectSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find target's deformed surface for cell deformation.");
   }
   
   //
   // Get the  target fiducial surface
   //
   const BrainModelSurface* targetFiducialSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                             dmf->getTargetFiducialCoordFileName());
   if (targetFiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find target fiducial coord file for cell deformation.");
   }
   const CoordinateFile* targetFiducialCoordinateFile = targetFiducialSurface->getCoordinateFile();
   
   //
   // Project the cells onto the target surface CANNOT USE CELL PROJECTOR
   //
   BrainModelSurfacePointProjector tdspp(targetProjectSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                           false);
   for (int i = 0; i < sourceCellFile->getNumberOfCells(); i++) {
      CellData* cd = sourceCellFile->getCell(i);
      float xyz[3];
      cd->getXYZ(xyz);
      int nearestNode, tileNodes[3];
      float tileAreas[3];
      const int tile = tdspp.projectBarycentric(xyz,
                                                 nearestNode,
                                                 tileNodes,
                                                 tileAreas);
      //
      // Unproject onto target fiducial surface
      //
      if (tile >= 0) {
         BrainModelSurfacePointProjector::unprojectPoint(tileNodes,
                                                         tileAreas,
                                                         targetFiducialCoordinateFile,
                                                         xyz);
      }
      else if (nearestNode >= 0) {
         targetFiducialCoordinateFile->getCoordinate(nearestNode, xyz);
      }
      else {
         xyz[0] = 0.0;
         xyz[1] = 0.0;
         xyz[2] = 0.0;
      }
      cd->setXYZ(xyz);
   }
   
   //
   // Set to the target directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getTargetDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getTargetDirectory());
      }
      else if (dmf->getOutputSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getOutputSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getOutputSpecFileName()));
         }
      }
   }
   
   //
   // Create the deformed file name
   //
   QString outputFileName;
   if (outputFileNameIn.isEmpty()) {   
      outputFileName =
         FileUtilities::basename(
            createDeformedFileName(dataFileName, 
                                dmf->getOutputSpecFileName(),
                                dmf->getDeformedFileNamePrefix(),
                                dmf->getNumberOfNodes(),
                                false));
   }
   else {
      outputFileName = outputFileNameIn;
   }
   
   //
   // Write the target cell files
   //
   // Note: sourceCellFile is used for both source and output
   addCommentAboutDeformation(*dmf, sourceCellFile, sourceCellFile); 
   try {
      sourceCellFile->writeFile(outputFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Update the target spec file
   //
   if (useSourceTargetPathsFlag) {
      QFileInfo specFileInfo(dmf->getOutputSpecFileName());
      if (specFileInfo.isFile()) {
         SpecFile sf;
         try {
            sf.readFile(dmf->getOutputSpecFileName());
            //
            // file will be written if file is new to spec file
            //
            if (fociFileFlag) {
               sf.addToSpecFile(SpecFile::getFociFileTag(), outputFileName, "", true);
            }
            else {
               sf.addToSpecFile(SpecFile::getCellFileTag(), outputFileName, "", true);
            }
         }
         catch(FileException& e) {
            QDir::setCurrent(savedDirectory);
            QString msg("File was successfully deformed.  However, unable to "
                            "update Spec File: \n");
            msg.append(dmf->getOutputSpecFileName());
            throw BrainModelAlgorithmException(msg);
         }
      }
   }
   
   //
   // Reset to the original directory
   //
   QDir::setCurrent(savedDirectory);
   
   delete sourceCellFile;
}

/**
 * deform cell or foci files
 */
void
BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFiles(BrainSet* sourceBrainSet,
                                                       BrainSet* targetBrainSet,
                                                       const DeformationMapFile* dmf,
                                                       const SpecFile::Entry& dataFiles,
                                                       const bool fociFileFlag,
                                                       QString& deformErrorsMessage)
                                                  throw (BrainModelAlgorithmException)
{
   for (unsigned int i = 0; i < dataFiles.files.size(); i++) {
      try {
         deformCellOrFociProjectionFile(sourceBrainSet,
                              targetBrainSet,
                              dmf,
                              true,
                              dataFiles.files[i].filename,
                              fociFileFlag);
      }
      catch (BrainModelAlgorithmException& e) {
         deformErrorsMessage.append(e.whatQString());
         deformErrorsMessage.append("\n");
      }
   }
}

/**
 * deform a cell or foci projection file
 */
void
BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFile(BrainSet* sourceBrainSet,
                                                      BrainSet* targetBrainSet,
                                                      const DeformationMapFile* dmf,
                                                      const bool useSourceTargetPathsFlag,
                                                      const QString& dataFileName,
                                                      const bool fociFileFlag,
                                                      const QString& outputFileNameIn)
                                                  throw (BrainModelAlgorithmException)
{
   //
   // Save current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to source directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getSourceDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getSourceDirectory());
      }
      else if (dmf->getSourceSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getSourceSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getSourceSpecFileName()));
         }
      }
   }
   
   //
   // Find the source surface's
   //
   BrainModelSurface* sourceFiducialSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceFiducialCoordFileName());
   if (sourceFiducialSurface == NULL) {
      QDir::setCurrent(savedDirectory);
      throw BrainModelAlgorithmException("Unable to find source fiducial surface for cell deformation.");
   }
   BrainModelSurface* sourceFlatSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceFlatCoordFileName());
   BrainModelSurface* sourceSphericalSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceSphericalCoordFileName());
   
   //
   // Read in the cell projection file
   //
   CellProjectionFile* sourceCellProjectionFile = NULL;
   CellFile* sourceCellFile = NULL;
   if (fociFileFlag) {
      sourceCellFile = new FociFile;
      sourceCellProjectionFile = new FociProjectionFile;
   }
   else {
      sourceCellFile = new CellFile;
      sourceCellProjectionFile = new CellProjectionFile;
   }
   try {
      sourceCellProjectionFile->readFile(dataFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e);
   }
             
   //
   // Find the deformed source surface which will exist if this was an indvidual to atlas deformation
   //
   BrainModelSurface* unprojectSourceSurface = NULL;
   if (dmf->getInverseDeformationFlag() == false) {
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            unprojectSourceSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getSourceDeformedFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            unprojectSourceSurface = sourceBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceDeformedSphericalCoordFileName());
            break;
      }
   }
   else {
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            unprojectSourceSurface = sourceFlatSurface;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            unprojectSourceSurface = sourceSphericalSurface;
            break;
      }
   }
   if (unprojectSourceSurface == NULL) {
      throw BrainModelAlgorithmException("Missing source surface for cell file deformation unprojection.");
   }

   //
   // Unproject source cell projection file onto the flat or spherical source surface
   //
   sourceCellFile->clear();
   CellProjectionUnprojector scpu;
   scpu.unprojectCellProjections(*sourceCellProjectionFile,
                                 unprojectSourceSurface,
                                 *sourceCellFile,
                                 0);

   //
   // Target's cell projection file
   //
   CellProjectionFile targetCellProjectionFile;
   
   //
   // if Atlas to Individual deformation
   //
   BrainModelSurface* targetProjectSurface = NULL;
   if (dmf->getInverseDeformationFlag()) {
      //
      // Use the deformed surface for projection
      //
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getSourceDeformedFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getSourceDeformedSphericalCoordFileName());
            break;
      }
   }
   else {
      //
      // Use the non-deformed surface for projection
      //
      switch (dmf->getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName
                                                (dmf->getTargetFlatCoordFileName());
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_MULTI_STAGE_VECTOR:
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE_SINGLE_STAGE_VECTOR:
            targetProjectSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                                dmf->getTargetSphericalCoordFileName());
            break;
      }
   }
   if (targetProjectSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find target's deformed surface for cell deformation.");
   }
   
   //
   // Get the  target fiducial surface
   //
   const BrainModelSurface* targetFiducialSurface = targetBrainSet->getBrainModelSurfaceWithCoordinateFileName(
                                             dmf->getTargetFiducialCoordFileName());
   if (targetFiducialSurface == NULL) {
      throw BrainModelAlgorithmException("Unable to find target fiducial coord file for cell deformation.");
   }
   const CoordinateFile* targetFiducialCoordinateFile = targetFiducialSurface->getCoordinateFile();
   
   //
   // Project the cells onto the target surface CANNOT USE CELL PROJECTOR
   //
   BrainModelSurfacePointProjector tdspp(targetProjectSurface,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                           false);
   for (int i = 0; i < sourceCellFile->getNumberOfCells(); i++) {
      CellData* cd = sourceCellFile->getCell(i);
      float xyz[3];
      cd->getXYZ(xyz);
      int nearestNode, tileNodes[3];
      float tileAreas[3];
      const int tile = tdspp.projectBarycentric(xyz,
                                                 nearestNode,
                                                 tileNodes,
                                                 tileAreas);
      //
      // Unproject onto target fiducial surface
      //
      if (tile >= 0) {
         BrainModelSurfacePointProjector::unprojectPoint(tileNodes,
                                                         tileAreas,
                                                         targetFiducialCoordinateFile,
                                                         xyz);
      }
      else if (nearestNode >= 0) {
         targetFiducialCoordinateFile->getCoordinate(nearestNode, xyz);
      }
      else {
         xyz[0] = 0.0;
         xyz[1] = 0.0;
         xyz[2] = 0.0;
      }
      cd->setXYZ(xyz);
   }
   
   //
   // Replace cell projections with cells
   //
   sourceCellProjectionFile->clear();
   sourceCellProjectionFile->appendFiducialCellFile(*sourceCellFile);
   
   //
   // Project cells to target fiducial surface
   //
   CellFileProjector cellProj(targetFiducialSurface);
   cellProj.projectFile(sourceCellProjectionFile,
                        0,
                        CellFileProjector::PROJECTION_TYPE_ALL,
                        0.0,
                        false,
                        NULL);
                        
   //
   // Set to the target directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getTargetDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getTargetDirectory());
      }
      else if (dmf->getOutputSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getOutputSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getOutputSpecFileName()));
         }
      }   
   }
   
   //
   // Create the deformed file name
   //
   QString outputFileName;
   if (outputFileNameIn.isEmpty()) {   
      outputFileName =
         FileUtilities::basename(
            createDeformedFileName(dataFileName, 
                                dmf->getOutputSpecFileName(),
                                dmf->getDeformedFileNamePrefix(),
                                dmf->getNumberOfNodes(),
                                false));
   }
   else {
      outputFileName = outputFileNameIn;
   }
   
   //
   // Write the target cell files
   //
   // Note: sourceCellFile is used for both source and output
   addCommentAboutDeformation(*dmf, sourceCellProjectionFile, sourceCellProjectionFile); 
   try {
      sourceCellProjectionFile->writeFile(outputFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Update the target spec file
   //
   if (useSourceTargetPathsFlag) {
      QFileInfo specFileInfo(dmf->getOutputSpecFileName());
      if (specFileInfo.isFile()) {
         SpecFile sf;
         try {
            sf.readFile(dmf->getOutputSpecFileName());
            //
            // file will be written if file is new to spec file
            //
            if (fociFileFlag) {
               sf.addToSpecFile(SpecFile::getFociProjectionFileTag(), outputFileName, "", true);
            }
            else {
               sf.addToSpecFile(SpecFile::getCellProjectionFileTag(), outputFileName, "", true);
            }
         }
         catch(FileException& e) {
            QDir::setCurrent(savedDirectory);
            QString msg("File was successfully deformed.  However, unable to "
                            "update Spec File: \n");
            msg.append(dmf->getOutputSpecFileName());
            throw BrainModelAlgorithmException(msg);
         }
      }
   }
   
   //
   // Reset to the original directory
   //
   QDir::setCurrent(savedDirectory);
   
   delete sourceCellFile;
   delete sourceCellProjectionFile;
}

/**
 * Link color files to target from source.
 */
void 
BrainModelSurfaceDeformDataFile::linkColorFiles(const DeformationMapFile* dmf,
                                                const bool linkAreaColorFiles,
                                                const bool linkBorderColorFiles,
                                                const bool linkCellColorFiles,
                                                const bool linkFociColorFiles,
                                                QString& deformErrorsMessage)
                                                        throw (BrainModelAlgorithmException)
{
   //
   // Read in the source spec file
   //
   SpecFile sourceSpecFile;
   try {
      sourceSpecFile.readFile(dmf->getSourceSpecFileName());
   }
   catch (FileException& e) {
      deformErrorsMessage.append(e.whatQString());
      deformErrorsMessage.append("\n");
      return;
   }
   
   //
   // Get the path of the source spec file
   //
   const QString sourceSpecFilePath(FileUtilities::dirname(dmf->getSourceSpecFileName()));
   
   //
   // Read in the output spec file
   //
   SpecFile outputSpecFile;
   try {
      outputSpecFile.readFile(dmf->getOutputSpecFileName());
   }
   catch (FileException& e) {
      deformErrorsMessage.append(e.whatQString());
      deformErrorsMessage.append("\n");
      return;
   }
   
   //
   // Do area color files
   //
   if (linkAreaColorFiles) {
      linkColorFileHelper(sourceSpecFile.areaColorFile,
                          sourceSpecFilePath,
                          outputSpecFile);
   }
   
   //
   // Do border color files
   //
   if (linkBorderColorFiles) {
      linkColorFileHelper(sourceSpecFile.borderColorFile,
                          sourceSpecFilePath,
                          outputSpecFile);
   }
   
   //
   // Do cell color files
   //
   if (linkCellColorFiles) {
      linkColorFileHelper(sourceSpecFile.cellColorFile,
                          sourceSpecFilePath,
                          outputSpecFile);
   }
   
   //
   // Do foci color files
   //
   if (linkFociColorFiles) {
      linkColorFileHelper(sourceSpecFile.fociColorFile,
                          sourceSpecFilePath,
                          outputSpecFile);
   }
   
}

/**
 * Help out with linking one type of color files.
 */
void 
BrainModelSurfaceDeformDataFile::linkColorFileHelper(const SpecFile::Entry& colorFiles,
                                                     const QString& sourceSpecFilePath,
                                                     SpecFile& outputSpecFile)
{
   for (unsigned int i = 0; i < colorFiles.files.size(); i++) {
         QString filename(colorFiles.files[i].filename);
         if (filename.isEmpty() == false) {
            //
            // See if file does NOT have an absolute path
            //
            if (filename[0] != '/') {
               //
               // Prepend with the sourc spec file's path
               //
               QString s(sourceSpecFilePath);
               if (s.isEmpty() == false) {
                  s.append("/");
               }
               s.append(filename);
               filename = s;
            }
            //
            // Add to output spec file which will convert to a relative
            // path and write the spec file too.
            //
            outputSpecFile.addToSpecFile(colorFiles.specFileTag, filename, "", true);
         }
   }
}

/**
 * deform flat coordinate files.
 */
void 
BrainModelSurfaceDeformDataFile::deformFlatCoordinateFiles(const DeformationMapFile* dmf,
                          const SpecFile::Entry& dataFiles,
                          QString& deformErrorsMessage)
                                          throw (BrainModelAlgorithmException)
{
   for (int i = 0; i < static_cast<int>(dataFiles.files.size()); i++) {
      try {
         deformFlatCoordinateFile(dmf,
                                  dmf->getTargetClosedTopoFileName(),
                                  true,
                                  dataFiles.files[i].filename,
                                  dmf->getSourceCutTopoFileName(),
                                  ""
                                  "");
      }
      catch (BrainModelAlgorithmException& e) {
         deformErrorsMessage.append(e.whatQString());
         deformErrorsMessage.append("\n");
      }
   }
}
                                       
/**
 * deform a flat coordinate file.
 */
void 
BrainModelSurfaceDeformDataFile::deformFlatCoordinateFile(const DeformationMapFile* dmf,
                                 const QString& atlasTopoFileName,
                                 const bool useSourceTargetPathsFlag,
                                 const QString& coordFileNameIn,
                                 const QString& topoFileName,
                                 const QString& outputCoordFileNameIn,
                                 const QString& outputTopoFileNameIn,
                                 const float maxLength)
                                    throw (BrainModelAlgorithmException)
{
   //
   // Deform the coordinate file
   //
   QString deformedCoordFileName(outputCoordFileNameIn);
   deformCoordinateFile(dmf, coordFileNameIn, deformedCoordFileName, false, useSourceTargetPathsFlag);
   
   //
   // Save current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to the source directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getSourceDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getSourceDirectory());
      }
      else if (dmf->getSourceSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getSourceSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getSourceSpecFileName()));
         }
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet;
   
   //
   // Create a spec file consisting of coord and topo of individual
   //
   SpecFile sf;
   sf.setTopoAndCoordSelected(topoFileName, coordFileNameIn, brainSet.getStructure());
   
   //
   // Load the data files
   //
   std::vector<QString> errorMessages;
   brainSet.readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                        sf,
                        "spec file",
                        errorMessages,
                        NULL,
                        NULL);
   if (errorMessages.empty() == false) {
      throw BrainModelAlgorithmException(StringUtilities::combine(errorMessages, "\n"));
   }

   //
   // Make sure needed files present
   //
   if (brainSet.getNumberOfBrainModels() < 1) {
      throw BrainModelAlgorithmException("Unable to find brain model of indiv topo and coord for flat coord deformation");
   }
   const BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw BrainModelAlgorithmException("Unable to find brain model surface of indiv topo and coord for flat coord deformation");
   }
   
   //
   // Create a point projector
   //
   BrainModelSurfacePointProjector bspp(bms,
                                        BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_FLAT,
                                        false);

   //
   // Set to the target directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getTargetDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getTargetDirectory());
      }
      else if (dmf->getOutputSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getOutputSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getOutputSpecFileName()));
         }
      }
   }
   
   //
   // Read the deformed coordinate file
   //
   CoordinateFile cf;
   try {
      cf.readFile(deformedCoordFileName);
   }
   catch (FileException&) {
      throw BrainModelAlgorithmException("Unable to read deformed coordinate file for flat coord deformation.");
   }
   
   //
   // Flags nodes that are to be removed from topology
   //
   const int numCoords = cf.getNumberOfCoordinates();
   std::vector<bool> nodesToRemoveFromTopology(numCoords, false);
   
   //
   // Find any node's in deformed coord that are at the origin
   //
   for (int i = 0; i < numCoords; i++) {
      const float* xyz = cf.getCoordinate(i);
      if ((xyz[0] == 0.0) && (xyz[1] == 0.0) && (xyz[2] == 0.0)) {
         nodesToRemoveFromTopology[i] = true;
      }
   }
   
   //
   // See if nodes in deformed coordinate file project to original
   //
   const float* coords = cf.getCoordinate(0);
   for (int i = 0; i < numCoords; i++) {
      int nearestNode;
      int tileNodes[3];
      float barycentricCoords[3];
      const int tile = bspp.projectBarycentric(&coords[i*3],
                                               nearestNode,
                                               tileNodes,
                                               barycentricCoords);
                                               
      //
      // does deformed node FAIL to project to original surface ?
      //
      if (tile < 0) {
         nodesToRemoveFromTopology[i] = true;
      }
   }

   //
   // Get individiuals topology file information
   //
   TopologyFile* tf = bms->getTopologyFile();
   TopologyFile::TOPOLOGY_TYPES topoType = tf->getTopologyType();
   if ((topoType == TopologyFile::TOPOLOGY_TYPE_UNKNOWN) ||
       (topoType == TopologyFile::TOPOLOGY_TYPE_UNSPECIFIED)) {
      topoType = TopologyFile::TOPOLOGY_TYPE_CUT;
   }
   const QString specFileTag(TopologyFile::getSpecFileTagFromTopologyType(topoType));
   
   //
   // Read the atlas topo file
   //
   TopologyFile newTopoFile;
   try {
      newTopoFile.readFile(atlasTopoFileName);
      newTopoFile.setTopologyType(topoType);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException("Unable to read atlas topo file for flat coord deformation.");
   }
   
   //
   // Remove unneeded nodes
   //
   //newTopoFile.deleteTilesWithMarkedNodes(nodesToRemoveFromTopology);
   
   //
   // Find and disconnect long edges
   //
   const int numTiles = newTopoFile.getNumberOfTiles();
   for (int i = 0; i < numTiles; i++) {
      const int* n = newTopoFile.getTile(i);
      if ((cf.getDistanceBetweenCoordinates(n[0], n[1]) > maxLength) ||
          (cf.getDistanceBetweenCoordinates(n[1], n[2]) > maxLength) ||
          (cf.getDistanceBetweenCoordinates(n[2], n[0]) > maxLength)) {
         nodesToRemoveFromTopology[n[0]] = true;
         nodesToRemoveFromTopology[n[1]] = true;
         nodesToRemoveFromTopology[n[2]] = true;
      }
   }
   
   //
   // Remove unneeded nodes
   //
   newTopoFile.deleteTilesWithMarkedNodes(nodesToRemoveFromTopology);
   
   //
   // Move disconnected nodes to origin
   //
   for (int i = 0; i < numCoords; i++) {
      if (nodesToRemoveFromTopology[i]) {
         cf.setCoordinate(i, 0.0, 0.0, 0.0);
      }
   }
   
   //
   // Rewrite deformed coord file since nodes moved to origin
   //
   try {
      cf.writeFile(cf.getFileName());
   }
   catch (FileException& e) {
      QString msg("Unable to rewrite deformed coord file for flat coord deformation.\n");
      msg.append(e.whatQString());
      throw BrainModelAlgorithmException(msg);
   }

   //
   // Create output topo file name (if needed)
   //
   QString outputTopoFileName(outputTopoFileNameIn);
   if (outputTopoFileName.isEmpty()) {
      outputTopoFileName = FileUtilities::filenameWithoutExtension(deformedCoordFileName);
      outputTopoFileName.append(SpecFile::getTopoFileExtension());
   }
   
   //
   // Write the output topo file
   //
   try {
      newTopoFile.writeFile(outputTopoFileName);
   }
   catch (FileException& e) {
      QString msg("Unable to write output topo file for flat coord deformation\n");
      msg.append(e.whatQString());
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // Update the target spec file
   //
   if (useSourceTargetPathsFlag) {
      QFileInfo specFileInfo(dmf->getOutputSpecFileName());
      if (specFileInfo.isFile()) {
         SpecFile sfOut;
         const QString outputSpecFileName(dmf->getOutputSpecFileName());
         try {
            sfOut.readFile(outputSpecFileName);
            //
            // file will be written if file is new to spec file
            //
            sfOut.addToSpecFile(specFileTag, outputTopoFileName, "", true);
         }
         catch(FileException& e) {
            QDir::setCurrent(savedDirectory);
            QString msg("File successfully deformed.  However, unable to update spec file:\n");
            msg.append(outputSpecFileName);
            throw BrainModelAlgorithmException(msg);
         }
      }
   }
   
   //
   //
   // Reset to the original directory
   //
   QDir::setCurrent(savedDirectory);
}

/**
 * deform coordinate files
 */
void
BrainModelSurfaceDeformDataFile::deformCoordinateFiles(const DeformationMapFile* dmf,
                                                       const SpecFile::Entry& dataFiles,
                                                       QString& deformErrorsMessage)
                                          throw (BrainModelAlgorithmException)
{
   for (int i = 0; i < static_cast<int>(dataFiles.files.size()); i++) {
      try {
         QString emptyString;
         deformCoordinateFile(dmf, 
                              dataFiles.files[i].filename, 
                              emptyString, 
                              dmf->getSmoothDeformedSurfacesFlag(),
                              true);
      }
      catch (BrainModelAlgorithmException& e) {
         deformErrorsMessage.append(e.whatQString());
         deformErrorsMessage.append("\n");
      }
   }
}

/**
 * deform a coordinate file.
 */
void 
BrainModelSurfaceDeformDataFile::deformCoordinateFile(const DeformationMapFile* dmf,
                                                      const QString& dataFileName,
                                                      QString& outputFileNameInOut,
                                                      const bool smoothCoordFileOneIteration,
                                                      const bool useSourceTargetPathsFlag)
                                                throw (BrainModelAlgorithmException)
{
   //
   // Save current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Set to source directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getSourceDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getSourceDirectory());
      }
      else if (dmf->getSourceSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getSourceSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getSourceSpecFileName()));
         }
      }
   }
   
   //
   // Read input file
   //
   CoordinateFile inputFile;
   try {
      inputFile.readFile(dataFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException(e.whatQString());
   }
   
   //
   // Base spec file tag upon input file's configuration ID
   //
   const QString specFileTag = 
      BrainModelSurface::getCoordSpecFileTagFromSurfaceType(
         BrainModelSurface::getSurfaceTypeFromConfigurationID(
            inputFile.getHeaderTag(AbstractFile::headerTagConfigurationID)));
   
   //
   // Get the number of nodes
   //
   const int numNodes = dmf->getNumberOfNodes();
   
   //
   // Create the output file
   //
   CoordinateFile outputFile;
   outputFile.setNumberOfCoordinates(numNodes);

   //
   // Process each node
   //
   for (int i = 0; i < numNodes; i++) {
      int tileNodes[3];
      float tileAreas[3];
      dmf->getDeformDataForNode(i, tileNodes, tileAreas);
      
      float xyz[3] = { 0.0, 0.0, 0.0 };
      
      if (tileNodes[0] >= 0) {
         //
         // Unproject to input coordinate file
         //
         BrainModelSurfacePointProjector::unprojectPoint(tileNodes, tileAreas,
                                                         &inputFile, xyz);
      }
      
      outputFile.setCoordinate(i, xyz);
   }

   //
   // Copy some of the input file's coordinate metadata
   // IF TAGS ADDED ALSO ADD THEM AFTER SMOOTHING LATER IN THIS METHOD
   //
   const QString configID = inputFile.getHeaderTag(CoordinateFile::headerTagConfigurationID);
   outputFile.setHeaderTag(CoordinateFile::headerTagConfigurationID, 
                           configID);
   outputFile.setHeaderTag(CoordinateFile::headerTagCoordFrameID, 
                           inputFile.getHeaderTag(CoordinateFile::headerTagCoordFrameID));
   outputFile.setHeaderTag(CoordinateFile::headerTagOrientation, 
                           inputFile.getHeaderTag(CoordinateFile::headerTagOrientation));
   outputFile.setHeaderTag(CoordinateFile::headerTagStructure, 
                           inputFile.getHeaderTag(CoordinateFile::headerTagStructure));
   
   //
   // Update file comments
   //
   addCommentAboutDeformation(*dmf, &inputFile, &outputFile);
   
   //
   // Set to the target directory
   //
   if (useSourceTargetPathsFlag) {
      if (dmf->getTargetDirectory().isEmpty() == false) {
         QDir::setCurrent(dmf->getTargetDirectory());
      }
      else if (dmf->getOutputSpecFileName().isEmpty() == false) {
         QFileInfo fi(dmf->getOutputSpecFileName());
         if (fi.isFile()) {
            QDir::setCurrent(FileUtilities::dirname(dmf->getOutputSpecFileName()));
         }
      }
   }
   
   //
   // Create the deformed file name
   //
   if (outputFileNameInOut.isEmpty()) {
      outputFileNameInOut =
         FileUtilities::basename(
            createDeformedFileName(dataFileName, 
                                dmf->getOutputSpecFileName(),
                                dmf->getDeformedFileNamePrefix(),
                                dmf->getNumberOfNodes(),
                                false));
   }

   //
   // Write the data file
   //
   try {
      outputFile.writeFile(outputFileNameInOut);
   }
   catch(FileException& e) {
      QDir::setCurrent(savedDirectory);
      throw BrainModelAlgorithmException(e.whatQString());
   }

   //
   // If the coord file should be smoothed
   //   
   if (smoothCoordFileOneIteration) {
      QString topoFileName(dmf->getTargetClosedTopoFileName());
      if (FileUtilities::isAbsolutePath(topoFileName) == false) {
         topoFileName = dmf->getTargetDirectory();
         if (topoFileName.isEmpty() == false) {
            topoFileName.append("/");
         }
         topoFileName.append(dmf->getTargetClosedTopoFileName());
      }
      BrainSet bs(topoFileName, outputFileNameInOut);
      if (bs.getNumberOfBrainModels() > 0) {
         BrainModelSurface* bms = bs.getBrainModelSurface(0);
         if (bms != NULL) {
            bms->arealSmoothing(1.0, 1, 0);

            int numNodeCrossovers = 0;
            int numTileCrossovers = 0;
            bms->crossoverCheck(numTileCrossovers,
                                numNodeCrossovers,
                                bms->getSurfaceType());
            if (numNodeCrossovers > 0) {
               std::cout << "WARNING: coordinate file "
                         << outputFileNameInOut.toAscii().constData()
                         << " has "
                         << numNodeCrossovers
                         << " crossovers after deformation."
                         << std::endl;
            }
            
            CoordinateFile* cf = bms->getCoordinateFile();
            
            //
            // Copy some of the input file's coordinate metadata
            //
            cf->setHeaderTag(CoordinateFile::headerTagConfigurationID, 
                                    configID);
            cf->setHeaderTag(CoordinateFile::headerTagCoordFrameID, 
                                    inputFile.getHeaderTag(CoordinateFile::headerTagCoordFrameID));
            cf->setHeaderTag(CoordinateFile::headerTagOrientation, 
                                    inputFile.getHeaderTag(CoordinateFile::headerTagOrientation));
            cf->setHeaderTag(CoordinateFile::headerTagStructure, 
                                    inputFile.getHeaderTag(CoordinateFile::headerTagStructure));
   
            try {
               cf->writeFile(outputFileNameInOut);
            }
            catch(FileException& e) {
               QDir::setCurrent(savedDirectory);
               QString msg("Error writing smoothed coord file.\n");
               msg.append(e.whatQString());
               throw BrainModelAlgorithmException(msg);
            }
         }
      }
   }
   
   //
   // Update the target spec file
   //
   if (useSourceTargetPathsFlag) {
      QFileInfo specFileInfo(dmf->getOutputSpecFileName());
      if (specFileInfo.isFile()) {
         SpecFile sf;
         try {
            sf.readFile(dmf->getOutputSpecFileName());
            //
            // file will be written if file is new to spec file
            //
            sf.addToSpecFile(specFileTag, outputFileNameInOut, "", true);
         }
         catch(FileException& e) {
            QDir::setCurrent(savedDirectory);
            QString msg("File successfully deformed.  However, unable to update spec file:\n");
            msg.append(dmf->getOutputSpecFileName());
            throw BrainModelAlgorithmException(msg);
         }
      }
   }
   
   //
   // Reset to the original directory
   //
   QDir::setCurrent(savedDirectory);
}

