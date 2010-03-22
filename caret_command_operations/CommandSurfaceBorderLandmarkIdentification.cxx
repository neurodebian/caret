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

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderLandmarkIdentification.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderLandmarkIdentification.h"
#include "FileFilters.h"
#include "FociColorFile.h"
#include "FociProjectionFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SurfaceShapeFile.h"
#include "VocabularyFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandSurfaceBorderLandmarkIdentification::CommandSurfaceBorderLandmarkIdentification()
   : CommandBase("-surface-border-landmark-identification",
                 "SURFACE BORDER LANDMARK IDENTIFICATION")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderLandmarkIdentification::~CommandSurfaceBorderLandmarkIdentification()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderLandmarkIdentification::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<StereotaxicSpace> spaces;
   BrainModelSurfaceBorderLandmarkIdentification::getSupportedStereotaxicSpaces(spaces);
   std::vector<QString> spaceNames;
   for (unsigned int i = 0; i < spaces.size(); i++) {
      spaceNames.push_back(spaces[i].getName());
   }
   
   paramsOut.clear();
   paramsOut.addListOfItems("Stereotaxic Space", spaceNames, spaceNames);
   paramsOut.addFile("Anatomy Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Inflated Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Very Inflated Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Ellipsoid Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Closed Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addString("Input Paint File Geography Column Name or Number", "Geography");
   paramsOut.addFile("Surface Shape File Name", FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addString("Surface Shape File Depth Column Name or Number", "Depth");
   paramsOut.addFile("Input Area Color File Name", FileFilters::getAreaColorFileFilter());
   paramsOut.addFile("Output Area Color File Name", FileFilters::getAreaColorFileFilter());
   paramsOut.addFile("Input Vocabulary File Name", FileFilters::getVocabularyFileFilter());
   paramsOut.addFile("Output Vocabulary File Name", FileFilters::getVocabularyFileFilter());
   paramsOut.addFile("Input Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File Name", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Input Border Color File Name", FileFilters::getBorderColorFileFilter());
   paramsOut.addFile("Output Border Color File Name", FileFilters::getBorderColorFileFilter());
//   paramsOut.addFile("Output Foci Projection File Name", FileFilters::getFociProjectionFileFilter());
//   paramsOut.addFile("Output Foci Color File Name", FileFilters::getFociColorFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderLandmarkIdentification::getHelpInformation() const
{
   std::vector<StereotaxicSpace> spaces;
   BrainModelSurfaceBorderLandmarkIdentification::getSupportedStereotaxicSpaces(spaces);
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<stereotaxic-space-name> \n"
       + indent9 + "<input-anatomical-volume-file-name>\n"
       + indent9 + "<input-fiducial-coordinate-file-name>\n"
       + indent9 + "<input-inflated-coord-file-name> \n"
       + indent9 + "<input-very-inflated-coord-file-name> \n"
       + indent9 + "<input-ellipsoid-coord-file-name> \n"
       + indent9 + "<input-closed-topo-file-name>  \n"
       + indent9 + "<input-paint-file-name>  \n"
       + indent9 + "<output-paint-file-name>  \n"
       + indent9 + "<input-paint-file-geography-column-name-or-number>  \n"
       + indent9 + "<input-surface-shape-file-name>  \n"
       + indent9 + "<input-surface-shape-file-depth-column-name-or-number>  \n"
       + indent9 + "<input-area-color-file-name>  \n"
       + indent9 + "<output-area-color-file-name>  \n"
       + indent9 + "<input-vocabulary-file-name>  \n"
       + indent9 + "<output-vocabulary-file-name>  \n"
       + indent9 + "<input-border-projection-file> \n"
       + indent9 + "<output-border-projection-file> \n"
       + indent9 + "<input-border-color-file> \n"
       + indent9 + "<output-border-color-file> \n"
//       + indent9 + "<output-foci-projection-file> \n"
//       + indent9 + "<output-foci-color-file> \n"
       + indent9 + "\n"
       + indent9 + "Draw borders on landmarks for flattening and registration.\n"
       + indent9 + "The landmarks identified are:\n"
       + indent9 + "   Calcarine Sulcus\n"
       + indent9 + "   Central Sulcus\n"
       + indent9 + "   Medial Wall\n"
       + indent9 + "   Superior Temporal Gyrus\n"
       + indent9 + "   Sylvian Fissure\n"
       + indent9 + "\n"
       + indent9 + "The anatomical volume is used to generate the corpus\n"
       + indent9 + "callosum.  If desired, you may provide an anatomical \n"
       + indent9 + "volume that contains ONLY the corpus callosum.  If so,\n"
       + indent9 + "the anatomical volume's filename MUST contain the\n"
       + indent9 + "case insensitive words \"corpus\" and \"callosum\".\n"
       + indent9 + "\n"
       + indent9 + "Supported stereotaxic spaces are: \n");
      for (unsigned int i = 0; i < spaces.size(); i++) {
         helpInfo += (indent9 + "   " + spaces[i].getName() + "\n");
      }
      helpInfo += (
         indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderLandmarkIdentification::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   bool doLandmarksFlag = true;
   bool doFlattenFlag   = true;
   
   const QString stereotaxicSpaceName = 
      parameters->getNextParameterAsString("Stereotaxic Space Name");
   const QString anatomicalVolumeFileName = 
      parameters->getNextParameterAsString("Input Anatomical Volume File Name");
   const QString fiducialCoordFileName = 
      parameters->getNextParameterAsString("Input Fiducial Coord File Name");
   const QString inflatedCoordFileName =
      parameters->getNextParameterAsString("Input Inflated Coord File Name");
   const QString veryInflatedCoordFileName =
      parameters->getNextParameterAsString("Input Very Inflated Coord File Name");
   const QString ellipsoidCoordFileName =
      parameters->getNextParameterAsString("Input Ellipsoid Coord File Name");
   const QString inputTopoFileName =
      parameters->getNextParameterAsString("Input Closed Topology File Name");
   const QString inputPaintFileName = 
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName = 
      parameters->getNextParameterAsString("Output Paint File Name");
   const QString inputPaintFileGeographyColumnNameOrNumber = 
      parameters->getNextParameterAsString("Input Paint File Geography Column Name or Number");
   const QString inputSurfaceShapeFileName = 
      parameters->getNextParameterAsString("Input Surface Shape File Name");
   const QString inputSurfaceShapeFileDepthColumnNameOrNumber = 
      parameters->getNextParameterAsString("Input Surface Shape File Depth Column Name or Number");
   const QString inputAreaColorFileName = 
      parameters->getNextParameterAsString("Input Area Color File Name");
   const QString outputAreaColorFileName = 
      parameters->getNextParameterAsString("Output Area Color File Name");
   const QString inputVocabularyFileName = 
      parameters->getNextParameterAsString("Input Vocabulary File Name");
   const QString outputVocabularyFileName = 
      parameters->getNextParameterAsString("Output Vocabulary File Name");
   const QString inputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Output Border Projection File Name");
   const QString inputBorderColorFileName = 
      parameters->getNextParameterAsString("Input Border Color File Name");
   const QString outputBorderColorFileName = 
      parameters->getNextParameterAsString("Output Border Color File Name");
   //const QString outputFociProjectionFileName =
   //   parameters->getNextParameterAsString("Output Foci Projection File Name");
   //const QString outputFociColorFileName =
   //   parameters->getNextParameterAsString("Output Foci Color File Name");
      
   //
   // Optional parameters
   //
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Optional Param");
      if (paramName == "-no-landmarks") {
         doLandmarksFlag = false;
      }
      else if (paramName == "-no-flatten") {
         doFlattenFlag = false;
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }

   //
   // Check name of stereotaxic space
   //
   if (StereotaxicSpace::validStereotaxicSpaceName(stereotaxicSpaceName) == false) {
      throw CommandException(stereotaxicSpaceName
                             + " is not the name of a valid stereotaxic space.");
   }
   StereotaxicSpace stereotaxicSpace(stereotaxicSpaceName);

   //
   // Create a spec file
   //
   SpecFile specFile;
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getVolumeAnatomyFileTag(), 
                          anatomicalVolumeFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getFiducialCoordFileTag(), 
                          fiducialCoordFileName,
                          "", 
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getInflatedCoordFileTag(),
                          inflatedCoordFileName,
                          "",
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getVeryInflatedCoordFileTag(),
                          veryInflatedCoordFileName,
                          "",
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getEllipsoidCoordFileTag(),
                          ellipsoidCoordFileName,
                          "",
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getClosedTopoFileTag(), 
                          inputTopoFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getPaintFileTag(), 
                          inputPaintFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getSurfaceShapeFileTag(), 
                          inputSurfaceShapeFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   if (inputAreaColorFileName.isEmpty() == false) {
      if (QFile::exists(inputAreaColorFileName)) {
         specFile.addToSpecFile(SpecFile::getAreaColorFileTag(), 
                                inputAreaColorFileName, 
                                "", 
                                SpecFile::SPEC_FALSE);
      }
   }
   if (inputVocabularyFileName.isEmpty() == false) {
      if (QFile::exists(inputVocabularyFileName)) {
         specFile.addToSpecFile(SpecFile::getVocabularyFileTag(), 
                                inputVocabularyFileName, 
                                "", 
                                SpecFile::SPEC_FALSE);
      }
   }
   if (inputBorderProjectionFileName.isEmpty() == false) {
      if (QFile::exists(inputBorderProjectionFileName)) {
         specFile.addToSpecFile(SpecFile::getBorderProjectionFileTag(), 
                                inputBorderProjectionFileName, 
                                "", 
                                SpecFile::SPEC_FALSE);
      }
   }
   if (inputBorderColorFileName.isEmpty() == false) {
      if (QFile::exists(inputBorderColorFileName)) {
         specFile.addToSpecFile(SpecFile::getBorderColorFileTag(), 
                                inputBorderColorFileName, 
                                "", 
                                SpecFile::SPEC_FALSE);
      }
   }
   
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, "temp", errorMessage)) {
      const QString msg("ERROR: Reading spec file data files: \n"
                        + errorMessage);
      throw CommandException(msg);
   }
   
   //
   // Find the fiducial and very inflated surfaces
   //
   BrainModelSurface* fiducialSurface = NULL;
   BrainModelSurface* inflatedSurface = NULL;
   BrainModelSurface* veryInflatedSurface = NULL;
   BrainModelSurface* ellipsoidSurface = NULL;
   for (int i = 0; i < brainSet.getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
            fiducialSurface = bms;
         }
         else if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_INFLATED) {
            inflatedSurface = bms;
         }
         else if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_VERY_INFLATED) {
            veryInflatedSurface = bms;
         }
         else if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL) {
            ellipsoidSurface = bms;
         }
      }
   }
   if (fiducialSurface == NULL) {
      throw CommandException("Unable to find fiducial surface.");
   }
   if (inflatedSurface == NULL) {
      throw CommandException("Unable to find inflated surface.");
   }
   if (veryInflatedSurface == NULL) {
      throw CommandException("Unable to find very inflated surface.");
   }
   if (ellipsoidSurface == NULL) {
      throw CommandException("Unable to find ellipsoid surface.");
   }
   
   //
   // Get the paint and shape column numbers
   //
   const int paintFileGeographyColumnNumber = 
      brainSet.getPaintFile()->getColumnFromNameOrNumber(inputPaintFileGeographyColumnNameOrNumber, false);
   const int surfaceShapeFileDepthColumnNumber = 
      brainSet.getSurfaceShapeFile()->getColumnFromNameOrNumber(inputSurfaceShapeFileDepthColumnNameOrNumber, false);

   //
   // Get any existing borders
   //
   BorderProjectionFile borderProjectionFile;
   brainSet.getBorderSet()->copyBordersToBorderProjectionFile(borderProjectionFile);
   
   int operation = 0;
   if (doLandmarksFlag) {
      operation |= 
         BrainModelSurfaceBorderLandmarkIdentification::OPERATION_ID_REGISTRATION_LANDMARKS;
   }
   if (doFlattenFlag) {
      operation |= 
         BrainModelSurfaceBorderLandmarkIdentification::OPERATION_ID_FLATTENING_LANDMARKS;
   }
   
   //
   // Identify the sulci
   //
   BrainModelSurfaceBorderLandmarkIdentification bmslg(&brainSet,
                                            stereotaxicSpace,
                                            brainSet.getVolumeAnatomyFile(0),
                                            fiducialSurface,
                                            inflatedSurface,
                                            veryInflatedSurface,
                                            ellipsoidSurface,
                                            brainSet.getSurfaceShapeFile(),
                                            surfaceShapeFileDepthColumnNumber,
                                            brainSet.getPaintFile(),
                                            paintFileGeographyColumnNumber,
                                            brainSet.getAreaColorFile(),
                                            &borderProjectionFile,
                                            brainSet.getBorderColorFile(),
                                            brainSet.getVocabularyFile(),
                                            operation);
   errorMessage = "";
   try {
      bmslg.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      errorMessage = ("identifying landmarks: " 
                             + e.whatQString());
   }
      
   //
   // Save the borders
   //
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
   brainSet.getBorderColorFile()->writeFile(outputBorderColorFileName);

   //
   // Write the paint file
   //
   if (outputPaintFileName.isEmpty() == false) {
      brainSet.getPaintFile()->writeFile(outputPaintFileName);
   }
   
   //
   // Write the area color file
   //
   if (outputAreaColorFileName.isEmpty() == false) {
      brainSet.getAreaColorFile()->writeFile(outputAreaColorFileName);
   }
   
   //
   // Write the vocabulary file
   //
   if (outputVocabularyFileName.isEmpty() == false) {
      brainSet.getVocabularyFile()->writeFile(outputVocabularyFileName);
   }

/*
   //
   // Write the foci and foci projection files
   //
   FociProjectionFile* fpf = (FociProjectionFile*)bmslg.getFociProjectionFile();
   fpf->writeFile(outputFociProjectionFileName);
   brainSet.addToSpecFile(SpecFile::fociProjectionFileTag,
                          outputFociProjectionFileName);
   FociColorFile* fcf = (FociColorFile*)bmslg.getFociColorFile();
   fcf->writeFile(outputFociColorFileName);
   brainSet.addToSpecFile(SpecFile::fociColorFileTag,
                          outputFociColorFileName);
*/
   
   if (errorMessage.isEmpty() == false) {
      throw CommandException(errorMessage);
   }
}

      

