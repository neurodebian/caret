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
#include "CommandSurfaceRegionOfInterestSelection.h"
#include "CoordinateFile.h"
#include "FileFilters.h"
#include "FociProjectionFile.h"
#include "LatLonFile.h"
#include "MetricFile.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceRegionOfInterestSelection::CommandSurfaceRegionOfInterestSelection()
   : CommandBase("-surface-region-of-interest-selection",
                 "SURFACE REGION OF INTEREST SELECTION")
{
}

/**
 * destructor.
 */
CommandSurfaceRegionOfInterestSelection::~CommandSurfaceRegionOfInterestSelection()
{
}

/**
 * get the script builder parameters.
 */
void
CommandSurfaceRegionOfInterestSelection::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Region of Interest File Name", FileFilters::getRegionOfInterestFileFilter());
   paramsOut.addFile("Output Region of Interest File Name", FileFilters::getRegionOfInterestFileFilter());
   paramsOut.addVariableListOfParameters("ROI Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceRegionOfInterestSelection::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coord-file-name>  \n"
       + indent9 + "<input-topology-file-name>  \n"
       + indent9 + "<input-region-of-interest-file-name.roi>  \n"
       + indent9 + "<output-region-of-interest-file-name.roi>  \n"
       + indent9 + "[-all-nodes] \n"
       + indent9 + "[-border-projection \n"
       + indent9 + "   border-projection-file-name \n"
       + indent9 + "   border-projection-name \n"
       + indent9 + "   surface-view \n"
       + indent9 + "   dimension \n"
       + indent9 + "   z-minimum \n"
       + indent9 + "   SEL-TYPE] \n"
       + indent9 + "[-boundary-only] \n"
       + indent9 + "[-dilate  iterations] \n"
       + indent9 + "[-dilate-paint  paint-file-name column  paint-name  iterations]\n"
       + indent9 + "[-erode   iterations] \n"
       + indent9 + "[-limit-x-lateral   x-value] \n"
       + indent9 + "[-limit-x-medial    x-value] \n"
       + indent9 + "[-limit-x-min       x-value] \n"
       + indent9 + "[-limit-x-max       x-value] \n"
       + indent9 + "[-limit-y-min       y-value] \n"
       + indent9 + "[-limit-y-max       y-value] \n"
       + indent9 + "[-limit-z-min       z-value] \n"
       + indent9 + "[-limit-z-max       z-value] \n"
       + indent9 + "[-limit-x-min-focus  fociProjectionFile  focusName] \n"
       + indent9 + "[-limit-x-max-focus  fociProjectionFile  focusName] \n"
       + indent9 + "[-limit-y-min-focus  fociProjectionFile  focusName] \n"
       + indent9 + "[-limit-y-max-focus  fociProjectionFile  focusName] \n"
       + indent9 + "[-limit-z-min-focus  fociProjectionFile  focusName] \n"
       + indent9 + "[-limit-z-max-focus  fociProjectionFile  focusName] \n"
       + indent9 + "[-metric  metric-file-name  column  min  max SEL-TYPE]\n"
       + indent9 + "[-invert-selection] \n"
       + indent9 + "[-latlon file-name column min-lat max-lat min-lon max-lon SEL_TYPE]\n"
       + indent9 + "[-paint  paint-file-name  column  paint-name SEL-TYPE]\n"
       + indent9 + "[-shape  shape-file-name  column  min max SEL-TYPE]\n"
       + indent9 + "\n"
       + indent9 + "The input region of interest file does not need to exist.\n"
       + indent9 + "\n"
       + indent9 + "Perform a surface node region of interest selection\n"
       + indent9 + "\n"
       + indent9 + "SEL-TYPE is one of \n"
       + indent9 + "   NORMAL    Applies selection to all nodes.\n"
       + indent9 + "   AND       And (intersection) new selection with the\n"
       + indent9 + "             existing selection.\n"
       + indent9 + "   OR        Or (union) new selection with the\n"
       + indent9 + "             existing selection.\n"
       + indent9 + "   ANDNOT    And the inverse of the new selection with\n"
       + indent9 + "             existing selection.\n"
       + indent9 + "\n"
       + indent9 + "OPERATIONS\n"
       + indent9 + "\"-all-nodes\" will place all connected nodes into the ROI.\n"
       + indent9 + "\n"
       + indent9 + "\"-border-projection\" will place nodes that are inside of\n"
       + indent9 + "   the border into the ROI.  When \"dimension\" is 3D, nodes\n"
       + indent9 + "   within the border and whose screen Z-coordinate is greater\n"
       + indent9 + "   than \"z-minimum\" are assigned to the ROI.  In most cases,\n"
       + indent9 + "   a \"z-minimum\" of zero is sufficient.  If areas within the\n"
       + indent9 + "   border are not assigned, load the surface, click the mouse\n"
       + indent9 + "   on the unassigned nodes to find the value needed for \n"
       + indent9 + "   \"z-minimum\" and create the ROI again.\n"
       + indent9 + "\n"
       + indent9 + "      \"surface-view\" is one of these single characters:\n"
       + indent9 + "         A - Anterior\n"
       + indent9 + "         D - Dorsal\n"
       + indent9 + "         L - Lateral\n"
       + indent9 + "         M - Medial\n"
       + indent9 + "         P - Posterior\n"
       + indent9 + "         R - Reset (default view)\n"
       + indent9 + "         V - Ventral\n"
       + indent9 + "      \"dimension\" is one of:\n"
       + indent9 + "         2D \n"
       + indent9 + "         3D \n"
       + indent9 + "\n"
       + indent9 + "\"-boundary-only\" will retain only those nodes that have\n"
       + indent9 + "   at least one neighbor that is NOT in the ROI (interior\n"
       + indent9 + "   nodes are removed).\n"
       + indent9 + "\n"
       + indent9 + "\"-dilate\" will dilate the ROI for the specified iterations.\n"
       + indent9 + "\n"
       + indent9 + "\"-dilate-paint\" will dilate the ROI but only nodes that have\n"
       + indent9 + "   the specified paint are added to the ROI.\n"
       + indent9 + "\n"
       + indent9 + "\"-erode\" will erode the ROI for the specified iterations.\n"
       + indent9 + "\n"
       + indent9 + "\"-invert-selection\" will invert the node selection.\n"
       + indent9 + "\n"
       + indent9 + "\"-latlon\" will add nodes to the ROI if the nodes lat/long values\n"
       + indent9 + "   are within the specified range of the column.\n"
       + indent9 + "\n"
       + indent9 + "\"-metric\" will add nodes to the ROI if the nodes metric value\n"
       + indent9 + "   is within the specified range of the column.\n"
       + indent9 + "\n"
       + indent9 + "\"-paint\" will add nodes to the ROI if the nodes paint name\n"
       + indent9 + "   is the specified paint name for the column.\n"
       + indent9 + "\n"
       + indent9 + "\"-shape\" will add nodes to the ROI if the nodes shape value\n"
       + indent9 + "   is within the specified range of the column.\n"
       + indent9 + "\n"
       + indent9 + "NOTES:\n"
       + indent9 + "    The optional parameters (those enclosed in square\n"
       + indent9 + "    brackets) are performed in the order they are\n"
       + indent9 + "    listed on the command line.\n"
       + indent9 + "\n"
       + indent9 + "    \"column\" is either the number of the column, which\n"
       + indent9 + "    starts at one, or the name of the column.  If a\n"
       + indent9 + "    a name contains spaces, it must be enclosed in double \n" 
       + indent9 + "    quotes.  Name has priority over number.\n"
       + indent9 + "\n"
       + indent9 + "    When finding nodes enclosed inside a border projection\n"
       + indent9 + "    use a dimension of 2D for flat surfaces.  When the \n"
       + indent9 + "    dimension is 2D, the surface view is ignored.\n"
       + indent9 + "    \n"
       + indent9 + "    For three dimensional surfaces (fiducial, inflated,\n"
       + indent9 + "    spherical, etc.), set the dimension to 3D.  When determining \n"
       + indent9 + "    nodes within a border projection on a 3D surface, the \n"
       + indent9 + "    surface-view is applied and those nodes with a Z-coordinate\n"
       + indent9 + "    that is greater than Z-coordinate of the center of gravity\n"
       + indent9 + "    and within the border projection are added to the region\n"
       + indent9 + "    of interest.\n"
       + indent9 + "\n"
       + indent9 + "    When limits are specified with foci, the last focus \n"
       + indent9 + "    with \"focus-name\" found in the file is used.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceRegionOfInterestSelection::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get coord, topo, and roi file names
   //
   const QString coordFileName = 
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString topoFileName = 
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString inputRoiFileName = 
      parameters->getNextParameterAsString("Input Region of Interest File Name");
   const QString outputRoiFileName = 
      parameters->getNextParameterAsString("Output Region of Interest File Name");
      
   //
   // Create a brain set
   //
   BrainSet brainSet(topoFileName,
                     coordFileName,
                     "",
                     true);
   const BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains not nodes.");
   }
   
   //
   // Get the region of interest node selection
   //
   BrainModelSurfaceROINodeSelection* roi = 
      brainSet.getBrainModelSurfaceRegionOfInterestNodeSelection();
   roi->deselectAllNodes();
   
   //
   // Read input ROI file
   //
   if (inputRoiFileName.isEmpty() == false) {
      if (QFile::exists(inputRoiFileName)) {
         NodeRegionOfInterestFile inputROIFile;
         inputROIFile.readFile(inputRoiFileName);
         roi->getRegionOfInterestFromFile(inputROIFile);
      }
   }
   
   //
   // Process the parameters for node selection
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get the parameter
      //
      const QString parameterName =
         parameters->getNextParameterAsString("ROI Operation");
         
      if (parameterName == "-all-nodes") {
         const QString msg = roi->selectAllNodes(bms);
         if (msg.isEmpty() == false) {
            throw CommandException(msg);
         }
      }
      else if (parameterName == "-border-projection") {
         const QString borderProjectionFileName =
            parameters->getNextParameterAsString("Border Projection File Name");
         const QString borderName =
            parameters->getNextParameterAsString("Border Projection Name");
         const QString surfaceViewString =
            parameters->getNextParameterAsString("Surface View");
         const QString dimensionString =
            parameters->getNextParameterAsString("Dimension");
         const float zMinimum =
            parameters->getNextParameterAsFloat("Z-Minimum");
         const BrainModelSurfaceROINodeSelection::SELECTION_LOGIC
            selectionType = getSelectionType(
               parameters->getNextParameterAsString("Lat Lon Selection Type"));
            
         BrainModel::STANDARD_VIEWS surfaceView = BrainModel::VIEW_NONE;
         if (surfaceViewString == "A") {
            surfaceView = BrainModel::VIEW_ANTERIOR;
         }
         else if (surfaceViewString == "D") {
            surfaceView = BrainModel::VIEW_DORSAL;
         }
         else if (surfaceViewString == "L") {
            surfaceView = BrainModel::VIEW_LATERAL;
         }
         else if (surfaceViewString == "M") {
            surfaceView = BrainModel::VIEW_MEDIAL;
         }
         else if (surfaceViewString == "P") {
            surfaceView = BrainModel::VIEW_POSTERIOR;
         }
         else if (surfaceViewString == "R") {
            surfaceView = BrainModel::VIEW_RESET;
         }
         else if (surfaceViewString == "V") {
            surfaceView = BrainModel::VIEW_VENTRAL;
         }
         else {
            throw CommandException("invalid surface view: "
                                   + surfaceViewString);
         }
         
         bool dim3D = false;
         if (dimensionString == "2D") {
            dim3D = false;
         }
         else if (dimensionString == "3D") {
            dim3D = true;
         }
         else {
            throw CommandException("invalid dimension: "
                                   + dimensionString);
         }
         
         //
         // Read the border projection file
         //
         BorderProjectionFile borderProjectionFile;
         borderProjectionFile.readFile(borderProjectionFileName);
         
         //
         // Find the border
         //
         BorderProjection* borderProjection = 
            borderProjectionFile.getLastBorderProjectionByName(borderName);
         if (borderProjection == NULL) {
            throw CommandException("no border projection with name: "
                                   + borderName);
         }
         
         //
         // Copy the surface
         //
         BrainModelSurface borderSurface(*bms);
         
         //
         // 3D
         //
         if (dim3D) {
            //
            // Set view 
            //
            if ((bms->getStructure().getType() != Structure::STRUCTURE_TYPE_CORTEX_LEFT) &&
                (bms->getStructure().getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT)) {
               throw CommandException("Structure is neither right nor left for for 3D"
                                      "within border test.");
            }
            borderSurface.applyViewToCoordinates(surfaceView);

            //
            // Translate to center of mass
            //
            borderSurface.translateToCenterOfMass();
         }
         const CoordinateFile* borderCoordinateFile = borderSurface.getCoordinateFile();
         
         //
         // Create a border projection file with the single projection
         //
         BorderProjectionFile tempBorderProjectionFile;
         tempBorderProjectionFile.addBorderProjection(*borderProjection);
         
         // 
         // Unproject the border
         //
         BorderFile borderFile;
         BorderProjectionUnprojector unprojector;
         unprojector.unprojectBorderProjections(*borderCoordinateFile,
                                                tempBorderProjectionFile,
                                                borderFile);
         if (borderFile.getNumberOfBorders() <= 0) {
            throw CommandException("unprojecting border projection: "
                                   + borderName);
         }
         
         //
         // Get the border from unprojecting
         //
         const Border* border = borderFile.getBorder(0);
         
         //
         // select the nodes
         //
         const QString msg = roi->selectNodesWithinBorder(selectionType,
                                                           &borderSurface,
                                                           border,
                                                           dim3D,
                                                           zMinimum);
         if (msg.isEmpty() == false) {
            throw CommandException(msg);
         }
      }
      else if (parameterName == "-boundary-only") {
         roi->boundaryOnly(bms);
      }
      else if (parameterName == "-dilate") {
         const int numberOfIterations =
            parameters->getNextParameterAsInt("Dilation Iterations");
         roi->dilate(bms, numberOfIterations);
      }
      else if (parameterName == "-dilate-paint") {
         const QString paintFileName = 
            parameters->getNextParameterAsString("Dilate Paint File Name");
         const QString paintColumn =
            parameters->getNextParameterAsString("Dilate Paint Column Name or Number");
         const QString paintName = 
            parameters->getNextParameterAsString("Dilate Paint Name");
         const int iterations =
            parameters->getNextParameterAsInt("Dilate Paint Iterations");

         //
         // Read the paint file
         //
         PaintFile paintFile;
         paintFile.readFile(paintFileName);
         
         //
         // Get the column number
         //
         const int paintColumnNumber = paintFile.getColumnFromNameOrNumber(paintColumn, false);
         
         //
         // Select the nodes
         //
         roi->dilatePaintConstrained(bms,
                                     &paintFile,
                                     paintColumnNumber,
                                     paintName,
                                     iterations);
      }
      else if (parameterName == "-erode") {
         const int numberOfIterations =
            parameters->getNextParameterAsInt("Erosion Iterations");
         roi->erode(bms, numberOfIterations);
      }
      else if (parameterName == "-invert-selection") {
         roi->invertSelectedNodes(bms);
      }
      else if (parameterName == "-latlon") {
         //
         // Get the parameters
         //
         const QString latLonFileName = 
            parameters->getNextParameterAsString("Lat Lon File Name");
         const QString latLonColumn =
            parameters->getNextParameterAsString("Lat Lon Column Name or Number");
         const float minLatValue = 
            parameters->getNextParameterAsFloat("Lat Min Value");
         const float maxLatValue = 
            parameters->getNextParameterAsFloat("Lat Max Value");
         const float minLonValue = 
            parameters->getNextParameterAsFloat("Lon Min Value");
         const float maxLonValue = 
            parameters->getNextParameterAsFloat("Lon Max Value");
         const BrainModelSurfaceROINodeSelection::SELECTION_LOGIC
            selectionType = getSelectionType(
               parameters->getNextParameterAsString("Lat Lon Selection Type"));
               
         //
         // Read the metric file
         //
         LatLonFile latLonFile;
         latLonFile.readFile(latLonFileName);
         
         //
         // Get the column number
         //
         const int latLonColumnNumber = latLonFile.getColumnFromNameOrNumber(latLonColumn, false);
        
          //
         // Select the nodes
         //
         const QString msg = roi->selectNodesWithLatLong(selectionType,
                                                          bms,
                                                          &latLonFile,
                                                          latLonColumnNumber,
                                                          minLatValue,
                                                          maxLatValue,
                                                          minLonValue,
                                                          maxLonValue);
         if (msg.isEmpty() == false) {
            throw CommandException(msg);
         }
      }
      else if ((parameterName == "-limit-x-lateral") ||
               (parameterName == "-limit-x-medial") ||
               (parameterName == "-limit-x-min") ||
               (parameterName == "-limit-x-max") ||
               (parameterName == "-limit-y-min") ||
               (parameterName == "-limit-y-max") ||
               (parameterName == "-limit-z-min") ||
               (parameterName == "-limit-z-max")) {
         const float value = parameters->getNextParameterAsFloat("ROI "
                                                                 + parameterName
                                                                 + " value");
         extentLimit(bms,
                     roi,
                     parameterName,
                     value);
      }
      else if ((parameterName == "-limit-x-min-focus") ||
               (parameterName == "-limit-x-max-focus") ||
               (parameterName == "-limit-y-min-focus") ||
               (parameterName == "-limit-y-max-focus") ||
               (parameterName == "-limit-z-min-focus") ||
               (parameterName == "-limit-z-max-focus")) {
         //
         // limit the ROI
         //
         focusLimit(bms,
                    roi,
                    parameterName);
      }
      else if (parameterName == "-metric") {
         //
         // Get the parameters
         //
         const QString metricFileName = 
            parameters->getNextParameterAsString("Metric File Name");
         const QString metricColumn =
            parameters->getNextParameterAsString("Metric Column Name or Number");
         const float minValue = 
            parameters->getNextParameterAsFloat("Metric Min Value");
         const float maxValue = 
            parameters->getNextParameterAsFloat("Metric Max Value");
         const BrainModelSurfaceROINodeSelection::SELECTION_LOGIC
            selectionType = getSelectionType(
               parameters->getNextParameterAsString("Metric Selection Type"));
               
         //
         // Read the metric file
         //
         MetricFile metricFile;
         metricFile.readFile(metricFileName);
         
         //
         // Get the column number
         // 
         const int metricColumnNumber = metricFile.getColumnFromNameOrNumber(metricColumn, false);
         
         //
         // Select the nodes
         //
         const QString msg = roi->selectNodesWithMetric(selectionType,
                                                         bms,
                                                         &metricFile,
                                                         metricColumnNumber,
                                                         minValue,
                                                         maxValue);
         if (msg.isEmpty() == false) {
            throw CommandException(msg);
         }
      }
      else if (parameterName == "-paint") {
         //
         // Get the parameters
         //
         const QString paintFileName = 
            parameters->getNextParameterAsString("Paint File Name");
         const QString paintColumn =
            parameters->getNextParameterAsString("Paint Column Name or Number");
         const QString paintName = 
            parameters->getNextParameterAsString("Paint Name");
         const BrainModelSurfaceROINodeSelection::SELECTION_LOGIC
            selectionType = getSelectionType(
               parameters->getNextParameterAsString("Paint Selection Type"));
               
         //
         // Read the paint file
         //
         PaintFile paintFile;
         paintFile.readFile(paintFileName);
         
         //
         // Get the column number
         //
         const int paintColumnNumber = paintFile.getColumnFromNameOrNumber(paintColumn, false);
         
         //
         // Select the nodes
         //
         const QString msg = roi->selectNodesWithPaint(selectionType,
                                                        bms,
                                                        &paintFile,
                                                        paintColumnNumber,
                                                        paintName);
         if (msg.isEmpty() == false) {
            throw CommandException(msg);
         }
      }
      else if (parameterName == "-shape") {
         //
         // Get the parameters
         //
         const QString shapeFileName = 
            parameters->getNextParameterAsString("Shape File Name");
         const QString shapeColumn =
            parameters->getNextParameterAsString("Shape Column Name or Number");
         const float minValue = 
            parameters->getNextParameterAsFloat("Shape Min Value");
         const float maxValue = 
            parameters->getNextParameterAsFloat("Shape Max Value");
         const BrainModelSurfaceROINodeSelection::SELECTION_LOGIC
            selectionType = getSelectionType(
               parameters->getNextParameterAsString("Shape Selection Type"));
               
         //
         // Read the surface shape file
         //
         SurfaceShapeFile shapeFile;
         shapeFile.readFile(shapeFileName);
         
         //
         // Get the column number
         //
         const int shapeColumnNumber = shapeFile.getColumnFromNameOrNumber(shapeColumn, false);
        
         //
         // Select the nodes
         //
         const QString msg = roi->selectNodesWithSurfaceShape(selectionType,
                                                               bms,
                                                               &shapeFile,
                                                               shapeColumnNumber,
                                                               minValue,
                                                               maxValue);
         if (msg.isEmpty() == false) {
            throw CommandException(msg);
         }
      }
      else {
         throw CommandException("Unrecognized operation = \""
                                + parameterName
                                + "\".");
      }
   }
   
   //
   // Place the selections in a file and write it
   //
   NodeRegionOfInterestFile roiFile;
   roi->setRegionOfInterestIntoFile(roiFile);
   roiFile.writeFile(outputRoiFileName);
}

/**
 * get the selection type.
 */
BrainModelSurfaceROINodeSelection::SELECTION_LOGIC
CommandSurfaceRegionOfInterestSelection::getSelectionType(const QString& selectionTypeName) const
                                                    throw (CommandException)
{
   BrainModelSurfaceROINodeSelection::SELECTION_LOGIC selType;
   if (selectionTypeName == "NORMAL") {
      selType = BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL; 
   }
   else if (selectionTypeName == "AND") {
      selType = BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND; 
   }
   else if (selectionTypeName == "OR") {
      selType = BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_OR; 
   }
   else if (selectionTypeName == "ANDNOT") {
      selType = BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_AND_NOT; 
   }
   else {
      throw CommandException("Invalid selection type: "
                             + selectionTypeName);
   }
   
   return selType;
}

/**
 * focus xyz limit.
 */
void 
CommandSurfaceRegionOfInterestSelection::focusLimit(const BrainModelSurface* bms,
                                                    BrainModelSurfaceROINodeSelection* roi,
                                                    const QString& operationName)
{
   //
   // Get parameters
   //
   const QString fociProjectionFileName = 
      parameters->getNextParameterAsString("Limit Foci Projection File Name");
   const QString focusName =
      parameters->getNextParameterAsString("Limit Focus Name");
            
   //
   // Get the position of the focus
   //
   FociProjectionFile fpf;
   fpf.readFile(fociProjectionFileName);
   const CellProjection* focus = fpf.getLastCellProjectionWithName(focusName);
   if (focus == NULL) {
      throw CommandException("ROI LIMIT: focus name "
                             + focusName
                             + " not found.");
   }
   const bool fiducialSurfaceFlag =
      (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   const bool flatSurfaceFlag =
      ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT) ||
       (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR));
   float focusXYZ[3];
   if (focus->getProjectedPosition(bms->getCoordinateFile(),
                                        bms->getTopologyFile(),
                                        fiducialSurfaceFlag,
                                        flatSurfaceFlag,
                                        false,
                                        focusXYZ) == false) {
      throw CommandException("ROI LIMIT: unable to get projected position for "
                             + focusName);
   }

   //
   // Set the extent for limiting
   //
   float extent[6] = {
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max()
   };
   
   if (operationName == "-limit-x-min-focus") {
      extent[0] = focusXYZ[0];
   }
   else if (operationName == "-limit-x-max-focus") {
      extent[1] = focusXYZ[0];
   }
   else if (operationName == "-limit-y-min-focus") {
      extent[2] = focusXYZ[1];
   }
   else if (operationName == "-limit-y-max-focus") {
      extent[3] = focusXYZ[1];
   }
   else if (operationName == "-limit-z-min-focus") {
      extent[4] = focusXYZ[2];
   }
   else if (operationName == "-limit-z-max-focus") {
      extent[5] = focusXYZ[2];
   }
   else {
      throw CommandException("Invalid limit command "
                             + operationName);
   }
   
   //
   // Limit the ROI
   //
   roi->limitExtent(bms, extent);
}

/**
 * extent xyz limit.
 */
void 
CommandSurfaceRegionOfInterestSelection::extentLimit(const BrainModelSurface* bms,
                                                     BrainModelSurfaceROINodeSelection* roi,
                                                     const QString& operationName,
                                                     const float extentValue)
{
   //
   // Set the extent for limiting
   //
   float extent[6] = {
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max(),
      -std::numeric_limits<float>::max(),
       std::numeric_limits<float>::max()
   };
   
   if (operationName == "-limit-x-lateral") {
      if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
         extent[0] = -extentValue;
      }
      else if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
         extent[1] = extentValue;
      }
      else {
         throw CommandException("Structure is neither right nor left for lateral x limit");
      }
   }
   else if (operationName == "-limit-x-medial") {
      if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) {
         extent[1] = extentValue;
      }
      else if (bms->getStructure().getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT) {
         extent[0] = -extentValue;
      }
      else {
         throw CommandException("Structure is neither right nor left for lateral x limit");
      }
   }
   else if (operationName == "-limit-x-min") {
      extent[0] = extentValue;
   }
   else if (operationName == "-limit-x-max") {
      extent[1] = extentValue;
   }
   else if (operationName == "-limit-y-min") {
      extent[2] = extentValue;
   }
   else if (operationName == "-limit-y-max") {
      extent[3] = extentValue;
   }
   else if (operationName == "-limit-z-min") {
      extent[4] = extentValue;
   }
   else if (operationName == "-limit-z-max") {
      extent[5] = extentValue;
   }
   //
   
   // Limit the ROI
   //
   roi->limitExtent(bms, extent);
}
                       
