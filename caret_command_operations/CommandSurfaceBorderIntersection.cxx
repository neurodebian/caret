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

#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CellFileProjector.h"
#include "CommandSurfaceBorderIntersection.h"
#include "FileFilters.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderIntersection::CommandSurfaceBorderIntersection()
   : CommandBase("-surface-border-intersection",
                 "SURFACE BORDER INTERSECTION")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderIntersection::~CommandSurfaceBorderIntersection()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderIntersection::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Border Projection File", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Input Foci Projection File", FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Output Foci Projection File", FileFilters::getFociProjectionFileFilter());
   paramsOut.addString("Border 1 Name");
   paramsOut.addString("Border 2 Name");
   paramsOut.addString("Focus Name");
   paramsOut.addFloat("Intersection Tolerance", 3.0, 0.0, 100000.0);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderIntersection::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<border-projection-file-name>\n"
       + indent9 + "<input-foci-projection-file-name>\n"
       + indent9 + "<output-foci-projection-file-name>\n"
       + indent9 + "<border-1-name>\n"
       + indent9 + "<border-2-name>\n"
       + indent9 + "<focus-name>\n"
       + indent9 + "<intersection-tolerance>\n"
       + indent9 + "\n"
       + indent9 + "Starting at the first border point in border 1, find the closest\n"
       + indent9 + "point in border 2.  If the distance between these two border\n"
       + indent9 + "points is less than \"intersection-tolerance\", place a focus\n"
       + indent9 + "at this location.  Otherwise, continue use successive points\n"
       + indent9 + "in border 1.\n"
       + indent9 + "\n"
       + indent9 + "The input foci projection file does not need to exist.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderIntersection::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString coordinateFileName = 
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName = 
      parameters->getNextParameterAsString("Topology File Name");
   const QString borderProjectionFileName = 
      parameters->getNextParameterAsString("Border Projection File Name");
   const QString inputFociProjectionFileName = 
      parameters->getNextParameterAsString("Input Foci Projection File Name");
   const QString outputFociProjectionFileName = 
      parameters->getNextParameterAsString("Output Foci Projection File Name");
   const QString border1Name = 
      parameters->getNextParameterAsString("Border 1 Name");
   const QString border2Name = 
      parameters->getNextParameterAsString("Border 2 Name");
   const QString focusName = 
      parameters->getNextParameterAsString("Focus Name");
   const float intersectionTolerance = 
      parameters->getNextParameterAsFloat("Intersection Tolerance");
   checkForExcessiveParameters();
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     "",
                     true);
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("unable to find surface.");
   }
   bms->setSurfaceType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL); // need for projection to work
   
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(borderProjectionFileName);
   
   //
   // Find the borders
   //
   BorderProjection* bp1 = borderProjectionFile.getLastBorderProjectionByName(border1Name);
   if (bp1 == NULL) {
      throw CommandException("unable to find border named "
                             + border1Name);
   }
   BorderProjection* bp2 = borderProjectionFile.getLastBorderProjectionByName(border2Name);
   if (bp2 == NULL) {
      throw CommandException("unable to find border named "
                             + border2Name);
   }
   
   //
   // Create another border projection file containing just the two borders
   //
   BorderProjectionFile tempBorderProjFile;
   tempBorderProjFile.addBorderProjection(*bp1);
   tempBorderProjFile.addBorderProjection(*bp2);
   
   //
   // Unproject the two borders
   //
   BorderFile borderFile;
   BorderProjectionUnprojector unprojector;
   unprojector.unprojectBorderProjections(*(bms->getCoordinateFile()),
                                          tempBorderProjFile,
                                          borderFile);
   if (borderFile.getNumberOfBorders() != 2) {
      throw CommandException("unprojection of borders failed.");
   }
   
   //
   // Find intersection of two borders
   //
   const Border* border1 = borderFile.getBorder(0);
   const Border* border2 = borderFile.getBorder(1);
   int border1IntersectionLink, border2IntersectionLink;
   if (border1->intersection3D(border2, 
                               intersectionTolerance,
                               border1IntersectionLink,
                               border2IntersectionLink) == false) {
      throw CommandException("no intersection within tolerance.");
   }
   
   //
   // Read input foci projection file
   //
   FociProjectionFile fociProjectionFile;
   if (QFile::exists(inputFociProjectionFileName)) {
      fociProjectionFile.readFile(inputFociProjectionFileName);
   }
   
   //
   // Add a focus at border intersection
   //
   const float* focusXYZ = border1->getLinkXYZ(border1IntersectionLink);
   CellData focus(focusName, focusXYZ[0], focusXYZ[1], focusXYZ[2]);
   FociFile fociFile;
   fociFile.addCell(focus);
   
   //
   // Project the focus
   //
   CellProjectionFile tempFociProjFile;
   tempFociProjFile.append(fociFile);
   CellFileProjector fociProjector(bms);
   fociProjector.projectFile(&tempFociProjFile,
                             0,
                             CellFileProjector::PROJECTION_TYPE_ALL,
                             0.0,
                             false,
                             NULL);
   
   //
   // Get projected focus
   //
   fociProjectionFile.append(tempFociProjFile);
   
   //
   // Write the foci projection file
   //
   fociProjectionFile.writeFile(outputFociProjectionFileName);
}

      

