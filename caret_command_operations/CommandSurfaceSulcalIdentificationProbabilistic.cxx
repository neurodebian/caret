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
#include "BrainModelSurface.h"
#include "BrainModelSurfaceSulcalIdentificationProbabilistic.h"
#include "BrainSet.h"
#include "CommandSurfaceSulcalIdentificationProbabilistic.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"

/**
 * constructor.
 */
CommandSurfaceSulcalIdentificationProbabilistic::CommandSurfaceSulcalIdentificationProbabilistic()
   : CommandBase("-surface-sulcal-identification-probabilistic",
                 "SURFACE SULCAL IDENTIFICATION PROBABILISTIC")
{
}

/**
 * destructor.
 */
CommandSurfaceSulcalIdentificationProbabilistic::~CommandSurfaceSulcalIdentificationProbabilistic()
{
}

/**
 * get the script builder parameters.
 */
void
CommandSurfaceSulcalIdentificationProbabilistic::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Fiducial Coord File Name",
                     FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Input Inflated Coord File Name",
                     FileFilters::getCoordinateInflatedFileFilter());
   paramsOut.addFile("Input Very Inflated Coord File Name",
                     FileFilters::getCoordinateVeryInflatedFileFilter());
   paramsOut.addFile("Input Closed Topology File Name",
                     FileFilters::getTopologyClosedFileFilter());
   paramsOut.addFile("Input Paint File Name",
                     FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File Name",
                     FileFilters::getPaintFileFilter());
   paramsOut.addFile("Input Area Color File Name",
                     FileFilters::getAreaColorFileFilter());
   paramsOut.addFile("Output Area Color File Name",
                     FileFilters::getAreaColorFileFilter());
   paramsOut.addString("Input Paint Geography Column");
   paramsOut.addFile("Input Surface Shape File Name",
                     FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addString("Input Surface Shape Depth Column");
   paramsOut.addFile("Probabilistic Sulcus Volume list",
                     "*.csv");
   paramsOut.addFloat("Post Central Sulcus Offset", 25.0);
   paramsOut.addFloat("Post Central Sulcus Std Dev Squared", 100.0);
   paramsOut.addFloat("Post Central Sulcus Split", 5.0);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceSulcalIdentificationProbabilistic::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-fiducial-coord-file-name>  \n"
       + indent9 + "<input-inflated-coord-file-name> \n"
       + indent9 + "<input-very-inflated-coord-file-name> \n"
       + indent9 + "<input-closed-topo-file-name>  \n"
       + indent9 + "<input-paint-file-name>  \n"
       + indent9 + "<output-paint-file-name> \n" 
       + indent9 + "<input-paint-file-geography-column-name-or-number>  \n"
       + indent9 + "<input-area-color-file-name>  \n"
       + indent9 + "<output-area-color-file-name> \n" 
       + indent9 + "<input-surface-shape-file-name>  \n"
       + indent9 + "<input-surface-shape-file-depth-column-name-or-number>  \n"
       + indent9 + "<input-probabilistic-suclus-volume-list-file.csv>  \n"
       + indent9 + "<input-post-central-sulcus-offset>  \n" 
       + indent9 + "<input-post-central-sulcus-std-dev-squared>  \n" 
       + indent9 + "<input-post-central-sulcus-split>  \n" 
       + indent9 + "\n" 
       + indent9 + "Identify sulci on a surface by mapping a probabilistic volume. \n" 
       + indent9 + "\n"
       + indent9 + "Note: Paint and Surface Shape file column numbers start at one. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceSulcalIdentificationProbabilistic::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters from those passed to base class
   //
   const QString fiducialCoordFileName = 
      parameters->getNextParameterAsString("Input Fiducial Coord File Name");
   const QString inflatedCoordFileName =
      parameters->getNextParameterAsString("Input Inflated Coord File Name");
   const QString veryInflatedCoordFileName =
      parameters->getNextParameterAsString("Input Very Inflated Coord File Name");
   const QString inputTopoFileName =
      parameters->getNextParameterAsString("Input Closed Topology File Name");
   const QString paintFileName = 
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName =
      parameters->getNextParameterAsString("Output Paint File Name");
   const QString paintFileGeographyColumnNameOrNumber = 
      parameters->getNextParameterAsString("Paint File Geography Column Name or Number");
   const QString inputAreaColorFileName = 
      parameters->getNextParameterAsString("Input Area Color File Name");
   const QString outputAreaColorFileName =
      parameters->getNextParameterAsString("Output Area Color File Name");
   const QString surfaceShapeFileName = 
      parameters->getNextParameterAsString("Surface Shape File Name");
   const QString surfaceShapeFileDepthColumnNameOrNumber = 
      parameters->getNextParameterAsString("Surface Shape File Depth Column Name or Number");
   const QString probabilisticSulcusVolumeListFileName = 
      parameters->getNextParameterAsString("Probabilistic Sulcus/Volume List File (CSV)");
   const float postCentralSulcusOffset = 
      parameters->getNextParameterAsFloat("Post Central Sulcus Offset");
   const float postCentralSulcusStdDevSquared = 
      parameters->getNextParameterAsFloat("Post Central Sulcus Std Dev Squared");
   const float postCentralSulcusSplit = 
      parameters->getNextParameterAsFloat("Post Central Sulcus Split");
   
   //
   // Make sure that are no more parameters
   //
   checkForExcessiveParameters();

   //
   // Create a spec file
   //
   SpecFile specFile;
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag, 
                          fiducialCoordFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::inflatedCoordFileTag,
                          inflatedCoordFileName,
                          "",
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::veryInflatedCoordFileTag,
                          veryInflatedCoordFileName,
                          "",
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::closedTopoFileTag, 
                          inputTopoFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::paintFileTag, 
                          paintFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   if (inputAreaColorFileName.isEmpty() == false) {
      if (QFile::exists(inputAreaColorFileName)) {
         specFile.addToSpecFile(SpecFile::areaColorFileTag, 
                                inputAreaColorFileName, 
                                "", 
                                SpecFile::SPEC_FALSE);
      }
   }
   specFile.addToSpecFile(SpecFile::surfaceShapeFileTag, 
                          surfaceShapeFileName, 
                          "", 
                          SpecFile::SPEC_FALSE);
   
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
      }
   }
   if (fiducialSurface == NULL) {
      throw CommandException("Unable to find fiducial surface.");
   }
   if (veryInflatedSurface == NULL) {
      throw CommandException("Unable to find very inflated surface.");
   }
   
   //
   // Get the paint and shape column numbers
   //
   const int paintFileGeographyColumnNumber = 
      brainSet.getPaintFile()->getColumnFromNameOrNumber(paintFileGeographyColumnNameOrNumber, false);
   const int surfaceShapeFileDepthColumnNumber = 
      brainSet.getSurfaceShapeFile()->getColumnFromNameOrNumber(surfaceShapeFileDepthColumnNameOrNumber, false);
   //
   // Identify the sulci
   //
   BrainModelSurfaceSulcalIdentificationProbabilistic bmssi(&brainSet,
                                                  fiducialSurface,
                                                  inflatedSurface,
                                                  veryInflatedSurface,
                                                  brainSet.getPaintFile(),
                                                  paintFileGeographyColumnNumber,
                                                  brainSet.getSurfaceShapeFile(),
                                                  surfaceShapeFileDepthColumnNumber,
                                                  probabilisticSulcusVolumeListFileName,
                                                  postCentralSulcusOffset,
                                                  postCentralSulcusStdDevSquared,
                                                  postCentralSulcusSplit);
   try {
      bmssi.execute();
      if (bmssi.getOutputPaintFile() == NULL) {
         throw CommandException("output paint file with Identified sulci is NULL.");
      }
      try {
         PaintFile outputPaintFile(*bmssi.getOutputPaintFile());
         outputPaintFile.writeFile(outputPaintFileName);
      }
      catch (FileException& e) {
         const QString msg("identifying sulci writing output paint file: " 
                           + e.whatQString());
         throw CommandException(msg);
      }
      
      if (outputAreaColorFileName.isEmpty() == false) {
         const AreaColorFile* acf = bmssi.getOutputAreaColorFile(); 
         if (acf != NULL) {
            brainSet.getAreaColorFile()->append(*acf);
            try {
               brainSet.getAreaColorFile()->writeFile(outputAreaColorFileName);
            }
            catch (FileException&) {
               std::cout << "WARNING: Unable to write "
                         << outputAreaColorFileName.toAscii().constData()
                         << std::endl;
            }
         }
      }
   }
   catch (BrainModelAlgorithmException& e) {
      throw CommandException("identifying sulci: " 
                             + e.whatQString());
   }
}


