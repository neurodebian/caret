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

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderCutter.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderCutter.h"
#include "CoordinateFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceBorderCutter::CommandSurfaceBorderCutter()
   : CommandBase("-surface-border-cutter",
                 "SURFACE BORDER CUTTER")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderCutter::~CommandSurfaceBorderCutter()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderCutter::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> cutModes, cutModeDescriptions;
   cutModes.push_back("FLAT");    cutModeDescriptions.push_back("Cut Flat Surface");
   cutModes.push_back("POS_Z");   cutModeDescriptions.push_back("Cut Only Parts of Surface with Positive Z-Coordinates");
   cutModes.push_back("SPHERE");  cutModeDescriptions.push_back("Cut Anywhere on a Sphere");
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Output Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Border Projection File", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addListOfItems("Cut Mode", cutModes, cutModeDescriptions);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderCutter::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coordinate-file-name>\n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<output-coordinate-file-name>\n"
       + indent9 + "<output-topology-file-name>\n"
       + indent9 + "<border-projection-file-name>\n"
       + indent9 + "<cut-mode>\n"
       + indent9 + "\n"
       + indent9 + "Cut the surface using as cuts all borders in the border\n"
       + indent9 + "projection file.\n"
       + indent9 + "\n"
       + indent9 + "\"cut-mode\" is one of:\n"
       + indent9 + "  FLAT\n"
       + indent9 + "  POS_Z\n"
       + indent9 + "  SPHERE\n"
       + indent9 + "Use \"FLAT\" for cutting flat surfaces.  Use \"POS_Z\" to\n"
       + indent9 + "apply the cuts only to nodes with positive Z-coordinates.\n"
       + indent9 + "Use \"SPHERE\" to process cuts anywhere on a sphere.\n"
       + indent9 + "\n"
       + indent9 + "If the \"output-coordinate-file-name\" or the\n"
       + indent9 + "\"output-topology-file-name\" is empty (consecutive \n"
       + indent9 + "double quotes), the respective file is not written.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderCutter::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString inputCoordinateFileName =
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString outputCoordinateFileName =
      parameters->getNextParameterAsString("Output Coordinate File Name");
   const QString outputTopologyFileName =
      parameters->getNextParameterAsString("Output Topology File Name");
   const QString inputBorderProjectionFileName =
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString cutModeName =
      parameters->getNextParameterAsString("Cut Mode");
   checkForExcessiveParameters();
   
   //
   // Get cut mode
   //
   BrainModelSurfaceBorderCutter::CUTTING_MODE cutMode;
   if (cutModeName == "FLAT") {
      cutMode = BrainModelSurfaceBorderCutter::CUTTING_MODE_FLAT_SURFACE;
   }
   else if (cutModeName == "POS_Z") {
      cutMode = BrainModelSurfaceBorderCutter::CUTTING_MODE_NON_NEGATIVE_Z_ONLY;
   }
   else if (cutModeName == "SPHERE") {
      cutMode = BrainModelSurfaceBorderCutter::CUTTING_MODE_SPHERICAL_SURFACE;
   }
   else  {
      throw CommandException("Invalid cut mode: " + cutModeName);
   }
   
   //
   // Create a brain set and find the surface
   //
   BrainSet brainSet(inputTopologyFileName,
                     inputCoordinateFileName,
                     "",
                     true);
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Problem reading coordinate file.");
   }                  
   TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("Problem reading topology file.");
   }
   
   //
   // read the border projections
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(inputBorderProjectionFileName);

   //
   // Cut the surface
   //
   BrainModelSurfaceBorderCutter cmsbc(&brainSet,
                                       bms,
                                       &borderProjectionFile,
                                       cutMode,
                                       false);
   cmsbc.execute();
   
   //
   // Write the topology file before the coordinate file
   //
   if (outputTopologyFileName.isEmpty() == false) {
      brainSet.writeTopologyFile(outputTopologyFileName,
                                 tf->getTopologyType(),
                                 tf);
   }
   
   //
   // Write the coordinate file
   //
   if (outputCoordinateFileName.isEmpty() == false) {
      brainSet.writeCoordinateFile(outputCoordinateFileName,
                                   bms->getSurfaceType(),
                                   bms->getCoordinateFile(),
                                   true);
   }                     
}

      

