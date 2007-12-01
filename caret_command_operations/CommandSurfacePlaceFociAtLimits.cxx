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

#include <cmath>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "BrainSet.h"
#include "CellFileProjector.h"
#include "CommandSurfacePlaceFociAtLimits.h"
#include "FileFilters.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "NodeRegionOfInterestFile.h"
#include "ProgramParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfacePlaceFociAtLimits::CommandSurfacePlaceFociAtLimits()
   : CommandBase("-surface-place-foci-at-limits",
                 "SURFACE PLACE FOCI AT LIMITS")
{
}

/**
 * destructor.
 */
CommandSurfacePlaceFociAtLimits::~CommandSurfacePlaceFociAtLimits()
{
}

/**
 * get the script builder parameters.
 */
void
CommandSurfacePlaceFociAtLimits::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Limit Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Region of Interest File Name", FileFilters::getRegionOfInterestFileFilter());
   paramsOut.addFile("Input Foci Projection File Name", FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Output Foci Projection File Name", FileFilters::getFociProjectionFileFilter());
   paramsOut.addVariableListOfParameters("Place Foci at Limit Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfacePlaceFociAtLimits::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<fiducial-coord-file-name>  \n"
       + indent9 + "<limit-coord-file-name>  \n"
       + indent9 + "<input-topo-file-name>  \n"
       + indent9 + "<region-of-interest-file-name>  \n"
       + indent9 + "<input-foci-projection-file-name>  \n"
       + indent9 + "<output-foci-projection-file-name>  \n"
       + indent9 + "[-x-most-medial  focus-name]\n"
       + indent9 + "[-x-most-lateral  focus-name]\n"
       + indent9 + "[-x-min  focus-name] \n"
       + indent9 + "[-x-max  focus-name] \n"
       + indent9 + "[-x-abs-min  focus-name] \n"
       + indent9 + "[-x-abs-max  focus-name] \n"
       + indent9 + "[-y-min  focus-name] \n"
       + indent9 + "[-y-max  focus-name] \n"
       + indent9 + "[-y-abs-min  focus-name] \n"
       + indent9 + "[-y-abs-max  focus-name] \n"
       + indent9 + "[-z-min  focus-name] \n"
       + indent9 + "[-z-max  focus-name] \n"
       + indent9 + "[-z-abs-min  focus-name] \n"
       + indent9 + "[-z-abs-max  focus-name] \n"
       + indent9 + "[-cog  focus-name] \n"
       + indent9 + "\n"
       + indent9 + "Determine the limits of a surface and place foci at\n"
       + indent9 + "the specified limits.  The limits are determined on\n"
       + indent9 + "the \"limit-coord-file-name>\".  The fiducial coordinate\n"
       + indent9 + "file is used to correctly set the focus fiducial position.\n"
       + indent9 + "\n"
       + indent9 + "if append foci files is specified, new foci projections \n"
       + indent9 + "will be appended to the file.  \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfacePlaceFociAtLimits::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get coord, topo, and roi file names
   //
   const QString fiducialCoordFileName = 
      parameters->getNextParameterAsString("Input Fiducial Coordinate File Name");
   const QString limitCoordFileName = 
      parameters->getNextParameterAsString("Input Limit Coordinate File Name");
   const QString topoFileName = 
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString roiFileName = 
      parameters->getNextParameterAsString("Region of Interest File Name");
      
   //
   // Create a brain set
   //
   BrainSet brainSet(topoFileName,
                     fiducialCoordFileName,
                     limitCoordFileName,
                     true);
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurface(0);
   if (fiducialSurface == NULL) {
      throw CommandException("unable to find fiducial surface.");
   }
   BrainModelSurface* limitSurface = brainSet.getBrainModelSurface(1);
   if (limitSurface == NULL) {
      if (limitCoordFileName == fiducialCoordFileName) {
         limitSurface = fiducialSurface;
      }
      else {
         throw CommandException("unable to find limit surface.");
      }
   }
   const TopologyFile* tf = limitSurface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = limitSurface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read the region of interest file
   //
   NodeRegionOfInterestFile regionOfInterestFile;
   regionOfInterestFile.readFile(roiFileName);
   
   //
   // Get and set the region of interest
   //
   BrainModelSurfaceROINodeSelection* roi
      = brainSet.getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->getRegionOfInterestFromFile(regionOfInterestFile);
   
   //
   // Foci file names
   //
   const QString inputFociProjectionFileName = 
      parameters->getNextParameterAsString("Input Foci Projection File Name");
   const QString outputFociProjectionFileName = 
      parameters->getNextParameterAsString("Output Foci Projection File Name");
   
   //
   // Get bounds of the region of interest
   //
   float bounds[6];
   roi->getExtentOfSelectedNodes(limitSurface, bounds);
   if (roi->getNumberOfNodesSelected() <= 0) {
      throw CommandException("There are no nodes in the region of interest.");
   }
   
   //
   // Coordinate file
   //
   //const CoordinateFile* fiducialCoordFile = fiducialSurface->getCoordinateFile();
   //const CoordinateFile* limitCoordFile = limitSurface->getCoordinateFile();
   
   //
   // Get nodes at min/max values
   //
   int minXNode, maxXNode, minYNode, maxYNode, minZNode, maxZNode;
   int mostMedialXNode, mostLateralXNode;
   int absMinXNode, absMaxXNode, absMinYNode, absMaxYNode, absMinZNode, absMaxZNode;
   roi->getNodesWithMinMaxXYZValues(limitSurface,
                                    mostMedialXNode,
                                    mostLateralXNode,
                                    minXNode, 
                                    maxXNode, 
                                    minYNode, 
                                    maxYNode, 
                                    minZNode, 
                                    maxZNode,
                                    absMinXNode, 
                                    absMaxXNode, 
                                    absMinYNode, 
                                    absMaxYNode, 
                                    absMinZNode, 
                                    absMaxZNode);
                                    
   //
   // Foci projection file
   //
   FociProjectionFile fociProjectionFile;
   
   //
   // Process the parameters for node selection
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get the next parameter and process it
      //
      const QString paramName = parameters->getNextParameterAsString("Next Operation");
      if (paramName == "-x-most-medial") {
         if (mostMedialXNode < 0) {
            throw CommandException("most medial node unknown."
                                   "  Is surface structure set?");
         }
         const QString name = parameters->getNextParameterAsString("X-Most-Medial Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               mostMedialXNode);
      }
      else if (paramName == "-x-most-lateral") {
         if (mostMedialXNode < 0) {
            throw CommandException("most lateral node unknown."
                                   "  Is surface structure set?");
         }
         const QString name = parameters->getNextParameterAsString("X-Most-Lateral Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               mostLateralXNode);
      }
      else if (paramName == "-x-min") {
         const QString name = parameters->getNextParameterAsString("X-Min Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               minXNode);
      }
      else if (paramName == "-x-max") {
         const QString name = parameters->getNextParameterAsString("X-Max Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               maxXNode);
      }
      else if (paramName == "-x-abs-min") {
         const QString name = parameters->getNextParameterAsString("X-Abs-Min Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               absMinXNode);
      }
      else if (paramName == "-x-abs-max") {
         const QString name = parameters->getNextParameterAsString("X-Abs-Max Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               absMaxXNode);
      }
      else if (paramName == "-y-min") {
         const QString name = parameters->getNextParameterAsString("Y-Min Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               minYNode);
      }
      else if (paramName == "-y-max") {
         const QString name = parameters->getNextParameterAsString("Y-Max Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               maxYNode);
      }
      else if (paramName == "-y-abs-min") {
         const QString name = parameters->getNextParameterAsString("Y-Abs-Min Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               absMinYNode);
      }
      else if (paramName == "-y-abs-max") {
         const QString name = parameters->getNextParameterAsString("Y-Abs-Max Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               absMaxYNode);
      }
      else if (paramName == "-z-min") {
         const QString name = parameters->getNextParameterAsString("Z-Min Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               minZNode);
      }
      else if (paramName == "-z-max") {
         const QString name = parameters->getNextParameterAsString("Z-Max Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               maxZNode);
      }
      else if (paramName == "-z-abs-min") {
         const QString name = parameters->getNextParameterAsString("Z-Abs-Min Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               absMinZNode);
      }
      else if (paramName == "-z-abs-max") {
         const QString name = parameters->getNextParameterAsString("Z-Abs-Max Focus Name");
         createFocusProjection(fociProjectionFile,
                               name,
                               fiducialSurface,
                               absMaxZNode);
      }
      else if (paramName == "-cog") {
         const QString name = parameters->getNextParameterAsString("COG Focus Name");
         float xyz[3];
         roi->getCenterOfGravityOfSelectedNodes(limitSurface,
                                                xyz);
         FociFile fociFile;
         createFocus(fociFile,
                     name,
                     xyz);
                     
         fociProjectionFile.append(fociFile);
         
         CellFileProjector projector(fiducialSurface);
         projector.projectFile(&fociProjectionFile,
                               0,
                               CellFileProjector::PROJECTION_TYPE_ALL,
                               0.0,
                               false,
                               NULL);
      }
      else {
         throw CommandException("Unrecognized operation: "
                                + paramName);
      }
   }
   
   if (fociProjectionFile.getNumberOfCellProjections() <= 0) {
      throw CommandException("No foci were created.");
   }
   
   //
   // Append to existing foci projection file
   //
   FociProjectionFile outputFociProjectionFile;
   if (inputFociProjectionFileName.isEmpty() == false) {
      //
      // Read current foci projection file
      //
      if (QFile::exists(inputFociProjectionFileName)) {
         outputFociProjectionFile.readFile(inputFociProjectionFileName);
      }
   }
   outputFociProjectionFile.append(fociProjectionFile);

   //
   // Write the projection file
   //
   outputFociProjectionFile.writeFile(outputFociProjectionFileName);
   
}

/**
 * Create a focus projection.
 */
void 
CommandSurfacePlaceFociAtLimits::createFocusProjection(FociProjectionFile& fpf,
                                                       const QString& name,
                                                       const BrainModelSurface* fiducialSurface,
                                                       const int nodeNumber)
{
   //
   // Add the focus
   //
   fpf.addCellProjection(CellProjection(name,
                                        fiducialSurface->getCoordinateFile(),
                                        nodeNumber,
                                        fiducialSurface->getStructure()));
}

/**
 * Create a focus.
 */
void 
CommandSurfacePlaceFociAtLimits::createFocus(FociFile& ff,
                                             const QString& name,
                                             const float xyz[3])
{
   ff.addCell(CellData(name, xyz[0], xyz[1], xyz[2]));
}

