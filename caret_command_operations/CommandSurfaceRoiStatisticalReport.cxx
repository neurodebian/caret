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
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainModelSurfaceROITextReport.h"
#include "BrainSet.h"
#include "CommandSurfaceRoiStatisticalReport.h"
#include "FileFilters.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "TextFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceRoiStatisticalReport::CommandSurfaceRoiStatisticalReport()
   : CommandBase("-surface-roi-statistical-report",
                 "SURFACE ROI STATISTICAL REPORT")
{
}

/**
 * destructor.
 */
CommandSurfaceRoiStatisticalReport::~CommandSurfaceRoiStatisticalReport()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceRoiStatisticalReport::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Latitude/Longitude File Name", FileFilters::getLatitudeLongitudeFileFilter());
   paramsOut.addFile("Metric File Name", FileFilters::getPaintFileFilter());
   paramsOut.addString("Metric Distortion Column Name or Number");
   paramsOut.addFile("Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addFile("Surface Shape File Name", FileFilters::getSurfaceShapeFileFilter());
   paramsOut.addFile("Region Of Interest File Name", FileFilters::getRegionOfInterestFileFilter());
   paramsOut.addFile("Output Text Report File Name", FileFilters::getTextFileFilter());
   paramsOut.addBoolean("Tab Separate Report", false);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRoiStatisticalReport::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name> \n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<latitude-longitude-file-name> \n"
       + indent9 + "<metric-file-name>\n"
       + indent9 + "<metric-distortion-column-name-or-number>\n"
       + indent9 + "<paint-file-name>\n"
       + indent9 + "<surface-shape-file-name>\n"
       + indent9 + "<region-of-interest-file-name> \n"
       + indent9 + "<output-text-report-file-name> \n"
       + indent9 + "<tab-separate-report-flag>\n"
       + indent9 + "\n"
       + indent9 + "Generate a staistical report.  If any of the data files \n"
       + indent9 + "(lat/long, paint, metric, or shape) are not available, \n"
       + indent9 + "specify its name two consecutive double quotes (\"\"). \n"
       + indent9 + "\n"
       + indent9 + "If there is not a metric distortion column, specify its\n"
       + indent9 + "value with two consecutive double quotes (\"\").\n"
       + indent9 + "\n"
       + indent9 + "\"tab-separate-report-flag\" value is either \"true\" or \n"
       + indent9 + "\"false\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceRoiStatisticalReport::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString latLonFileName = 
      parameters->getNextParameterAsString("Lat/Lon File Name");
   const QString metricFileName = 
      parameters->getNextParameterAsString("Metric File Name");
   const QString metricColumnNameOrNumber = 
      parameters->getNextParameterAsString("Metric Distortion Column Name or Number");
   const QString paintFileName = 
      parameters->getNextParameterAsString("Paint File Name");
   const QString surfaceShapeFileName = 
      parameters->getNextParameterAsString("Surface Shape File Name");   
   const QString roiFileName = 
      parameters->getNextParameterAsString("Region of Interest File Name");
   const QString textReportFileName = 
      parameters->getNextParameterAsString("Text Report File Name");
   const bool tabSeparateReportFlag = 
      parameters->getNextParameterAsBoolean("Tab Separate Report Flag");

   //
   // Read the spec file into a brain set
   //
   SpecFile specFile;
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag,
                          coordinateFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::closedTopoFileTag,
                          topoFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::latLonFileTag,
                          latLonFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::metricFileTag,
                          metricFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::paintFileTag,
                          paintFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::surfaceShapeFileTag,
                          surfaceShapeFileName,
                          "",
                          false);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   brainSet.setIgnoreTopologyFileInCoordinateFileHeaderFlag(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      throw CommandException("Reading spec file: " +
                             errorMessage);
   }

   //
   // Find the surface
   //
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Unable to find surface after reading files.");
   }
   
   //
   // Read the roi
   //
   BrainModelSurfaceROINodeSelection roi(&brainSet);
   NodeRegionOfInterestFile roiFile;
   roiFile.readFile(roiFileName);
   roi.getRegionOfInterestFromFile(roiFile);
   
   //
   // Find the metric distortion column number and set selected columns
   //
   MetricFile* metricFile = brainSet.getMetricFile();
   int metricDistortionColumnNumber = -1;
   if ((metricFile->empty() == false) &&
       (metricColumnNameOrNumber.isEmpty() == false)) {
      metricDistortionColumnNumber = metricFile->getColumnFromNameOrNumber(metricColumnNameOrNumber, false);
   }
   std::vector<bool> metricSelections(metricFile->getNumberOfColumns(), true);
   
   //
   // Paint selections
   //
   PaintFile* paintFile = brainSet.getPaintFile();
   std::vector<bool> paintSelections(paintFile->getNumberOfColumns(), true);
   
   //
   // Shape selections
   //
   SurfaceShapeFile* shapeFile = brainSet.getSurfaceShapeFile();
   std::vector<bool> shapeSelections(shapeFile->getNumberOfColumns(), true);
   
   //
   // Verify surface has nodes
   //
   const int numNodes = brainSet.getNumberOfNodes();
   if (numNodes <= 0) {
      throw CommandException("The surfaces contain no nodes.");
   }
   
   //
   // Run the statistical report
   //
   BrainModelSurfaceROITextReport bmsri(&brainSet,
                                        bms,
                                        &roi,
                                        metricFile,
                                        metricSelections,
                                        shapeFile,
                                        shapeSelections,
                                        paintFile,
                                        paintSelections,
                                        brainSet.getLatLonFile(),
                                        0,  // lat/lon file column
                                        "",
                                        metricFile,
                                        metricDistortionColumnNumber,
                                        tabSeparateReportFlag);
   try {
      bmsri.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      throw CommandException(e.whatQString());
      return;
   }

   
   //
   // File for text report
   //
   TextFile textReportFile;
   textReportFile.setText(bmsri.getReportText());

   //
   // Write the text file
   //
   textReportFile.writeFile(textReportFileName);
}

      

