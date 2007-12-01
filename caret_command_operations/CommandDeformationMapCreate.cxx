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
#include "BrainModelSurfaceDeformationMapCreate.h"
#include "BrainSet.h"
#include "CommandDeformationMapCreate.h"
#include "DeformationMapFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandDeformationMapCreate::CommandDeformationMapCreate()
   : CommandBase("-deformation-map-create",
                 "DEFORMATION MAP CREATE")
{
}

/**
 * destructor.
 */
CommandDeformationMapCreate::~CommandDeformationMapCreate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandDeformationMapCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();

   std::vector<QString> surfaceTypeNameAndValue;
   surfaceTypeNameAndValue.push_back("SPHERE");
   
   paramsOut.addListOfItems("Surface Type",
                            surfaceTypeNameAndValue,
                            surfaceTypeNameAndValue);
   paramsOut.addFile("Source Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Source Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Target Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Target Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Deformation Map File Name", 
                     FileFilters::getDeformationMapFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandDeformationMapCreate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<SURFACE_TYPE>\n"
       + indent9 + "<input-source-coordinate-file-name>\n"
       + indent9 + "<input-source-topology-file-name>\n"
       + indent9 + "<input-target-coordinate-file-name>\n"
       + indent9 + "<input-target-topology-file-name>\n"
       + indent9 + "<output-deformation-map-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Create a deformation map file that maps data from the \n"
       + indent9 + "source surface to the target surface.  The number of \n"
       + indent9 + "nodes in the output deformation map file is the number \n"
       + indent9 + "of nodes in the target surface.\n"
       + indent9 + "\n"
       + indent9 + "\"SURFACE_TYPE\" is one of:\n"
       + indent9 + "   SPHERE \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandDeformationMapCreate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the inputs
   //
   const QString surfaceTypeString =
      parameters->getNextParameterAsString("SURFACE TYPE");
   const QString sourceCoordinateFileName =
      parameters->getNextParameterAsString("Source Coordinate File Name");
   const QString sourceTopologyFileName =
      parameters->getNextParameterAsString("Source Topology File Name");
   const QString targetCoordinateFileName =
      parameters->getNextParameterAsString("Target Coordinate File Name");
   const QString targetTopologyFileName =
      parameters->getNextParameterAsString("Target Topology File Name");
   const QString deformationMapFileName =
      parameters->getNextParameterAsString("Deformation Map File Name");
   checkForExcessiveParameters();
   
   BrainModelSurfaceDeformationMapCreate::DEFORMATION_SURFACE_TYPE defType;
   if (surfaceTypeString == "SPHERE") {
      defType = BrainModelSurfaceDeformationMapCreate::DEFORMATION_SURFACE_TYPE_SPHERE;
   }
   else {
      throw CommandException("Invalid surface type: " + surfaceTypeString);
   }
   
   //
   // Create the source surface
   //
   BrainSet sourceBrainSet(sourceTopologyFileName,
                           sourceCoordinateFileName,
                           "",
                           true);
   BrainModelSurface* sourceSurface = sourceBrainSet.getBrainModelSurface(0);
   if (sourceSurface == NULL) {
      throw CommandException("Unable to create source surface.");
   }
   
   //
   // Create the target surface
   //
   BrainSet targetBrainSet(targetTopologyFileName,
                           targetCoordinateFileName);
   BrainModelSurface* targetSurface = targetBrainSet.getBrainModelSurface(0);
   if (targetSurface == NULL) {
      throw CommandException("Unable to create target surface.");
   }
   
   //
   // Deformation Map File
   //
   DeformationMapFile defMapFile;
   
   //
   // Execute the deformation 
   //
   BrainModelSurfaceDeformationMapCreate bmsdmc(&sourceBrainSet,
                                                sourceSurface,
                                                targetSurface,
                                                &defMapFile,
                                                defType);
   bmsdmc.execute();
   
   //
   // Write the deformation map file
   //
   defMapFile.writeFile(deformationMapFileName);
}

      

