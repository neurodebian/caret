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

#include <QDir>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformDataFile.h"
#include "BrainSet.h"
#include "CommandDeformationMapApply.h"
#include "DeformationMapFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
CommandDeformationMapApply::CommandDeformationMapApply()
   : CommandBase("-deformation-map-apply",
                 "DEFORMATION MAP APPLY")
{
}

/**
 * destructor.
 */
CommandDeformationMapApply::~CommandDeformationMapApply()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandDeformationMapApply::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> names;
   names.push_back("AREAL_ESTIMATION");
   names.push_back("BORDER_FLAT");
   names.push_back("BORDER_PROJECTION");
   names.push_back("BORDER_SPHERICAL");
   names.push_back("CELL");
   names.push_back("CELL_PROJECTION");
   names.push_back("COORDINATE");
   names.push_back("COORDINATE_FLAT");
   names.push_back("FOCI");
   names.push_back("FOCI_PROJECTION");
   names.push_back("LAT_LON");
   names.push_back("METRIC");
   names.push_back("PAINT");
   names.push_back("PROB_ATLAS");
   names.push_back("RGB_PAINT");
   names.push_back("SURFACE_SHAPE");
   names.push_back("TOPOGRAPHY");
   
   paramsOut.clear();
   paramsOut.addFile("Deformation Map File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addListOfItems("Data File Type", names, names);
   paramsOut.addFile("Input Data File Name", 
                     FileFilters::getAnyFileFilter());
   paramsOut.addFile("Output Data File Name", 
                     FileFilters::getAnyFileFilter());
   paramsOut.addVariableListOfParameters("Deformation Options");
}

/**
 * get full help information.
 */
QString 
CommandDeformationMapApply::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<deformation-map-file-name>\n"
       + indent9 + "<data-file-type>\n"
       + indent9 + "<input-data-file-name>\n"
       + indent9 + "<output-data-file-name>\n"
       + indent9 + "[source-topology-file-name\n"
       + indent9 + " source-deformed-topology-file-name\n"
       + indent9 + " target-topology-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Deform a data file.\n"
       + indent9 + "\n"
       + indent9 + "Note:  \"source-topology-file-name\", \n"
       + indent9 + "\"source-deformed-topology-file-name\",\n"
       + indent9 + "and \"target-topology-file-name\" are only required when\n"
       + indent9 + "deforming coordinate FLAT files.\n"
       + indent9 + "\n"
       + indent9 + "\"data-file-type\" is one of:\n"
       + indent9 + "   AREAL_ESTIMATION \n"
       + indent9 + "   BORDER_FLAT \n"
       + indent9 + "   BORDER_PROJECTION \n"
       + indent9 + "   BORDER_SPHERICAL \n"
       + indent9 + "   CELL \n"
       + indent9 + "   CELL_PROJECTION \n"
       + indent9 + "   COORDINATE \n"
       + indent9 + "   COORDINATE_FLAT \n"
       + indent9 + "   FOCI \n"
       + indent9 + "   FOCI_PROJECTION \n"
       + indent9 + "   LAT_LON \n"
       + indent9 + "   METRIC \n"
       + indent9 + "   PAINT \n"
       + indent9 + "   PROB_ATLAS \n"
       + indent9 + "   RGB_PAINT \n"
       + indent9 + "   SURFACE_SHAPE \n"
       + indent9 + "   TOPOGRAPHY \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandDeformationMapApply::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString deformationMapFileName =
      parameters->getNextParameterAsString("Deformation Map File Named");

   const QString fileType =
      parameters->getNextParameterAsString("Data File Type");
   
   const QString dataFileName =
      parameters->getNextParameterAsString("Input Data File Name");
   
   const QString deformedFileName =
      parameters->getNextParameterAsString("Output Data File Name");
  
   QString indivTopoFileName;
   QString indivDeformTopoFileName;
   QString atlasTopoFileName;
   if (fileType == "COORDINATE-FLAT") {
      indivTopoFileName =
         parameters->getNextParameterAsString("Source Topology File Name");
      indivDeformTopoFileName =
         parameters->getNextParameterAsString("Source Deformed Topology File Name");
      atlasTopoFileName =
         parameters->getNextParameterAsString("Target Topology File Name");
   }   
   
   //
   // Get file type
   //
   BrainModelSurfaceDeformDataFile::DATA_FILE_TYPE dft;
   if (fileType == "AREAL_ESTIMATION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION;
   }
   else if (fileType == "BORDER_FLAT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT;
   }
   else if (fileType == "BORDER_SPHERICAL") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL;
   }
   else if (fileType == "BORDER_PROJECTION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION;
   }
   else if (fileType == "CELL") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_CELL;
   }
   else if (fileType == "CELL_PROJECTION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION;
   }
   else if (fileType == "COORDINATE") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE;
   }
   else if (fileType == "COORDINATE_FLAT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT;
   }
   else if (fileType == "FOCI") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI;
   }
   else if (fileType == "FOCI_PROJECTION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION;
   }
   else if (fileType == "LAT_LON") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON;
   }
   else if (fileType == "METRIC") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC;
   }
   else if (fileType == "PAINT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT;
   }
   else if (fileType == "PROB_ATLAS") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS;
   }
   else if (fileType == "RGB_PAINT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT;
   }
   else if (fileType == "SURFACE_SHAPE") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE;
   }
   else if (fileType == "TOPOGRAPHY") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY;
   }
   else {
      QString msg("Invalid file type: \"");
      msg.append(fileType);
      msg.append("\"\n");
      throw CommandException(msg);
   }

   //
   // Read the deformation map file
   //
   DeformationMapFile deformationMapFile;
      deformationMapFile.readFile(deformationMapFileName);
   
   if (deformationMapFile.getFileVersion() >= 2) {      
      QString msg;
      if (QFile::exists(deformationMapFile.getSourceDirectory()) == false) {
         msg.append("Individual directory is invalid.  Change the\n"
                    "individual directory to the directory containing\n"
                    "the individual spec file.");
      }
      if (QFile::exists(deformationMapFile.getTargetDirectory()) == false) {
         msg.append("Atlas directory is invalid.  Change the\n"
                    "atlas directory to the directory containing\n"
                    "the atlas spec file.");
      }
      if (msg.isEmpty() == false) {
         throw CommandException(msg);
      }
   }
   
   //
   // Read the brain sets if needed
   //
   BrainSet sourceBrainSet, targetBrainSet;
   switch(dft) {
      case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
         readBrainSetsForDeformation(sourceBrainSet,
                                     targetBrainSet,
                                     deformationMapFile);
         break;
   }
 
   if (deformationMapFile.getFileVersion() >= 2) {
      //
      // Prepend with path
      //
      QString name(deformationMapFile.getTargetDirectory());
      name.append("/");
      name.append(deformationMapFile.getOutputSpecFileName());
      deformationMapFile.setOutputSpecFileName(name);
   }

   //
   // set metric deformation
   //
      deformationMapFile.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_NEAREST_NODE);
   //   deformationMapFile.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES);

   //
   // Deform the data file
   //
   switch(dft) {
      case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
         BrainModelSurfaceDeformDataFile::deformNodeAttributeFile(&deformationMapFile,
                                                                  dft,
                                                                  dataFileName,
                                                                  deformedFileName);
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
         BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFile(&deformationMapFile,
                                                                  dft,
                                                                  dataFileName,
                                                                  deformedFileName);
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
         {
            QString deformedFileName2(deformedFileName);
            BrainModelSurfaceDeformDataFile::deformCoordinateFile(&deformationMapFile,
                                                                  dataFileName,
                                                                  deformedFileName2,
                                                                  true);
         }
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
         BrainModelSurfaceDeformDataFile::deformFlatCoordinateFile(
                                 &deformationMapFile,
                                 atlasTopoFileName,
                                 dataFileName,
                                 indivTopoFileName,
                                 deformedFileName,
                                 indivDeformTopoFileName,
                                 10.0); //flatCoordMaxEdgeLengthFloatSpinBox->floatValue());
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
         BrainModelSurfaceDeformDataFile::deformBorderFile(
                          &sourceBrainSet,
                          &targetBrainSet,
                          &deformationMapFile,
                          dft,
                          dataFileName,
                          deformedFileName);
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
         BrainModelSurfaceDeformDataFile::deformCellOrFociFile(
                              &sourceBrainSet,
                              &targetBrainSet,
                              &deformationMapFile,
                              dataFileName,
                              false,
                              deformedFileName);
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
         BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFile(
                              &sourceBrainSet,
                              &targetBrainSet,
                              &deformationMapFile,
                              dataFileName,
                              false,
                              deformedFileName);
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
         BrainModelSurfaceDeformDataFile::deformCellOrFociFile(
                              &sourceBrainSet,
                              &targetBrainSet,
                              &deformationMapFile,
                              dataFileName,
                              true,
                              deformedFileName);
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
         BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFile(
                              &sourceBrainSet,
                              &targetBrainSet,
                              &deformationMapFile,
                              dataFileName,
                              true,
                              deformedFileName);
         break;
   }
}

/*----------------------------------------------------------------------------------------
 * Read in BrainSets for applying a deformation map to a data file.
 */
void
CommandDeformationMapApply::readBrainSetsForDeformation(BrainSet& sourceBrainSet,
                                                            BrainSet& targetBrainSet,
                                                            DeformationMapFile& deformationMapFile)
                                          throw (BrainModelAlgorithmException,
                                                 CommandException,
                                                 FileException)
{
   const QString savedDirectory(QDir::currentPath());
   
   //
   // create source brain set
   //
   bool sourceSpecMissing = true;
   SpecFile sourceSpecFile;
   try {
      QString specFileName;
      if (deformationMapFile.getFileVersion() >= 2) {
         specFileName.append(deformationMapFile.getSourceDirectory());
         specFileName.append("/");
      }
      specFileName.append(deformationMapFile.getSourceSpecFileName());
      sourceSpecFile.readFile(specFileName);
      sourceSpecMissing = false;
   }
   catch (FileException& e) {
      //
      // David has a bad habit of renaming spec files, so just hope the
      // data files are still the same name and in the same location.
      //
      QDir::setCurrent(deformationMapFile.getSourceDirectory());
      //errorMessage = e.whatQString().toAscii().constData());
      //return true;
   }
   
   //
   // Select the deformation files
   //
   sourceSpecFile.setDeformationSelectedFiles(
         deformationMapFile.getSourceClosedTopoFileName(),
         deformationMapFile.getSourceCutTopoFileName(),
         deformationMapFile.getSourceFiducialCoordFileName(),
         deformationMapFile.getSourceSphericalCoordFileName(),
         deformationMapFile.getSourceFlatCoordFileName(),
         "",
         sourceSpecMissing,
         sourceSpecFile.getStructure());
   
   //
   // Read in the source brain set
   //
   std::vector<QString> errorMessages;
   sourceBrainSet.readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                sourceSpecFile,
                                deformationMapFile.getSourceSpecFileName(),
                                errorMessages, NULL, NULL);
   if (errorMessages.empty() == false) {
      if (sourceSpecMissing) {
         errorMessages.push_back("Source spec file was not found.\n"
                                 "Tried to load data files.\n");
      }
      const QString errorMessage = StringUtilities::combine(errorMessages, "\n");
      throw CommandException(errorMessage);
   }
   
   //
   // Read in the deformed coordinate file
   //
   if (deformationMapFile.getInverseDeformationFlag() == false) {
      QString coordFileName;
      BrainModelSurface::SURFACE_TYPES surfaceType =
                         BrainModelSurface::SURFACE_TYPE_UNKNOWN;
      switch(deformationMapFile.getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            coordFileName = 
               deformationMapFile.getSourceDeformedFlatCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
            coordFileName = 
               deformationMapFile.getSourceDeformedSphericalCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
            break;
      }
      if (coordFileName.isEmpty()) {
         throw CommandException("Deformed source coordinate file is missing.");
      }
      sourceBrainSet.readCoordinateFile(coordFileName,
                                         surfaceType,
                                         false,
                                         true,
                                         false);
   }
   
   QDir::setCurrent(savedDirectory);
   
   //
   // Read in the target spec file
   //
   bool targetSpecMissing = true;
   SpecFile targetSpecFile;
   try {
      QString specFileName;
      if (deformationMapFile.getFileVersion() >= 2) {
         specFileName.append(deformationMapFile.getTargetDirectory());
         specFileName.append("/");
      }
      specFileName.append(deformationMapFile.getTargetSpecFileName());
      targetSpecFile.readFile(specFileName);
      targetSpecMissing = false;
   }
   catch (FileException& e) {
      //
      // David has a bad habit of renaming spec files, so just hope the
      // data files are still the same name and in the same location.
      //
      QDir::setCurrent(deformationMapFile.getSourceDirectory());
      //errorMessage = e.whatQString().toAscii().constData());
      //return true;
   }
   
   //
   // Select the deformation files
   //
   targetSpecFile.setDeformationSelectedFiles(
         deformationMapFile.getTargetClosedTopoFileName(),
         deformationMapFile.getTargetCutTopoFileName(),
         deformationMapFile.getTargetFiducialCoordFileName(),
         deformationMapFile.getTargetSphericalCoordFileName(),
         deformationMapFile.getTargetFlatCoordFileName(),
         "",
         targetSpecMissing,
         targetSpecFile.getStructure());
   
   //
   // Read in the target brain set
   //
   errorMessages.clear();
   targetBrainSet.readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                targetSpecFile,
                                deformationMapFile.getTargetSpecFileName(),
                                errorMessages, NULL, NULL);
   if (errorMessages.empty() == false) {
      if (targetSpecMissing) {
         errorMessages.push_back("Target spec file was not found.\n"
                                 "Tried to load data files.\n");
      }
      const QString errorMessage = StringUtilities::combine(errorMessages, "\n");
      throw CommandException(errorMessage);
   }
   
   //
   // Read in the deformed coordinate file
   //
   if (deformationMapFile.getInverseDeformationFlag()) {
      QString coordFileName;
      BrainModelSurface::SURFACE_TYPES surfaceType =
                         BrainModelSurface::SURFACE_TYPE_UNKNOWN;
      switch(deformationMapFile.getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            coordFileName = 
               deformationMapFile.getSourceDeformedFlatCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
            coordFileName = 
               deformationMapFile.getSourceDeformedSphericalCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
            break;
      }
      if (coordFileName.isEmpty()) {
         throw CommandException("Deformed source coordinate file is missing.");
      }
      targetBrainSet.readCoordinateFile(coordFileName,
                                            surfaceType,
                                            false,
                                            true,
                                            false);
   }
   
   QDir::setCurrent(savedDirectory);
}



