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
#include "BrainSet.h"
#include "CoordinateFile.h"
#include "CommandSurfaceTransformToStandardView.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceTransformToStandardView::CommandSurfaceTransformToStandardView()
   : CommandBase("-surface-transform-to-standard-view",
                 "SURFACE TRANSFORM TO STANDARD VIEW")
{
}

/**
 * destructor.
 */
CommandSurfaceTransformToStandardView::~CommandSurfaceTransformToStandardView()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceTransformToStandardView::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("A");   descriptions.push_back("Anterior");
   values.push_back("D");   descriptions.push_back("Dorsal");
   values.push_back("L");   descriptions.push_back("Lateral");
   values.push_back("M");   descriptions.push_back("Medial");
   values.push_back("P");   descriptions.push_back("Posterior");
   values.push_back("R");   descriptions.push_back("Reset (default view)");
   values.push_back("V");   descriptions.push_back("Ventral");
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addListOfItems("Standard View", values, descriptions);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceTransformToStandardView::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coordinate-file-name>\n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<output-coordinate-file-name>\n"
       + indent9 + "<standard-view>\n"
       + indent9 + "\n"
       + indent9 + "Transform a surface in a default (dorsal) view to another\n"
       + indent9 + "view.\n"
       + indent9 + "\n"
       + indent9 + "      \"standard-view\" is one of these single characters:\n"
       + indent9 + "         A - Anterior\n"
       + indent9 + "         D - Dorsal\n"
       + indent9 + "         L - Lateral\n"
       + indent9 + "         M - Medial\n"
       + indent9 + "         P - Posterior\n"
       + indent9 + "         R - Reset (default view)\n"
       + indent9 + "         V - Ventral\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceTransformToStandardView::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputCoordinateFileName = 
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString topologyFileName = 
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString outputCoordinateFileName = 
      parameters->getNextParameterAsString("Output Coordinate File Name");
   const QString surfaceViewString =
      parameters->getNextParameterAsString("Standard View");
      
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
      throw CommandException("invalid standard view: "
                             + surfaceViewString);
   }

   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     inputCoordinateFileName,
                     "",
                     true);
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
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
   
   if ((bms->getStructure().getType() != Structure::STRUCTURE_TYPE_CORTEX_RIGHT) &&
       (bms->getStructure().getType() != Structure::STRUCTURE_TYPE_CORTEX_LEFT)) {
      throw CommandException("Structure must be in the header of the coordinate "
                             "file and be either \"left\" or \"right\"");
   }

   bms->applyViewToCoordinates(surfaceView);
   
   CoordinateFile* cf = bms->getCoordinateFile();
   cf->writeFile(outputCoordinateFileName);
}

      

