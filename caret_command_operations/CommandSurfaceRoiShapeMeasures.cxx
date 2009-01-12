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

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROIIntegratedFoldingIndexReport.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CommandSurfaceRoiShapeMeasures.h"
#include "FileFilters.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandSurfaceRoiShapeMeasures::CommandSurfaceRoiShapeMeasures()
   : CommandBase("-surface-shape-measures",
                 "SURFACE SHAPE MEASURES")
{
}

/**
 * destructor.
 */
CommandSurfaceRoiShapeMeasures::~CommandSurfaceRoiShapeMeasures()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceRoiShapeMeasures::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Surface Shape File", FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addFile("Output Text File", FileFilters::getTextFileFilter(), "Folding.txt");
   paramsOut.addFile("Region Of Interest File", FileFilters::getRegionOfInterestFileFilter(), "", "-roi");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRoiShapeMeasures::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<surface-shape-file-name>\n"
       + indent9 + "<output-text-report-file-name>\n"
       + indent9 + "[-roi  region-of-interest-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Generate a report of shape measurements on a surface.\n"
       + indent9 + "\n"
       + indent9 + "If an ROI is provided, the shape measurements are \n"
       + indent9 + "limited to that region of the surface.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceRoiShapeMeasures::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString coordinateFileName = 
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topoFileName = 
      parameters->getNextParameterAsString("Topology File Name");
   const QString surfaceShapeFileName = 
      parameters->getNextParameterAsString("Surface Shape File Name");
   const QString textReportFileName = 
      parameters->getNextParameterAsString("Text Report File Name");
   
   QString roiFileName;
   while (parameters->getParametersAvailable()) {
      const QString paramName = 
         parameters->getNextParameterAsString("Optional parameter");
      if (paramName == "-roi") {
         roiFileName = 
            parameters->getNextParameterAsString("Region of Interest File Name");
         if (roiFileName.isEmpty()) {
            throw CommandException("Region of Interest File Name is missing.");
         }
      }
      else {
         throw CommandException("Unrecognized parameter \""
                                + paramName
                                + "\".");
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topoFileName,
                     coordinateFileName,
                     "");
                     
   //
   // Find the surface
   //
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Unable to find surface after reading files.");
   }
   if (bms->getTopologyFile() == NULL) {
      throw CommandException("Unable to find topology after reading files.");
   }
   
   brainSet.readSurfaceShapeFile(surfaceShapeFileName, false, false);

   //
   // Get the ROI
   //
   BrainModelSurfaceROINodeSelection* surfaceROI = 
      brainSet.getBrainModelSurfaceRegionOfInterestNodeSelection();
   if (roiFileName.isEmpty() == false) {
      NodeRegionOfInterestFile nroi;
      nroi.readFile(roiFileName);
      surfaceROI->getRegionOfInterestFromFile(nroi);
   }
   else {
      surfaceROI->selectAllNodes(bms);
   }
   
   //
   // Run the report
   //
   BrainModelSurfaceROIIntegratedFoldingIndexReport
      fmr(&brainSet,
          bms,
          surfaceROI,
          brainSet.getSurfaceShapeFile(),
          "",
          true);
   fmr.execute();
   
   //
   // File for text report
   //
   TextFile textReportFile;
   textReportFile.setText(fmr.getReportText());

   //
   // Write the text file
   //
   textReportFile.writeFile(textReportFileName);
}

      

