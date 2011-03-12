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
#include "CommandSurfaceSphere.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandSurfaceSphere::CommandSurfaceSphere()
   : CommandBase("-surface-sphere",
                 "SURFACE TO SPHERE")
{
}

/**
 * destructor.
 */
CommandSurfaceSphere::~CommandSurfaceSphere()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceSphere::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Output Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceSphere::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <input-coordinate-file-name>\n"
       + indent9 + "   <input-topology-file-name>\n"
       + indent9 + "   <output-coordinate-file-name>\n"
       + indent9 + "\n"
       + indent9 + "This command does the following:\n"
       + indent9 + "   1) All options are processed.\n"
       + indent9 + "   2) The surface is translated so that the midpoint\n"
       + indent9 + "      ((xmin+xmax)/2, (ymin+ymax)/2, (zmin+zmax)/2)\n"
       + indent9 + "      of the surface is at the origin.\n"
       + indent9 + "\n"
       + indent9 + "If the input surface file is a sphere and no\n"
       + indent9 + "options are provided, the spherical surface\n"
       + indent9 + "is centered at the origin \n"
       + indent9 + "abs(xmin)==abs(xmax), abs(ymin)==abs(ymax),\n"
       + indent9 + "abs(zmin)==abs(zmax).\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceSphere::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputCoordinateFileName =
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString outputCoordinateFileName =
      parameters->getNextParameterAsString("Output Coordinate File Name");
   checkForExcessiveParameters();
      
   //
   // Create a brain set
   //
   BrainSet brainSet(inputTopologyFileName,
                     inputCoordinateFileName,
                     "",
                     true);
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }

   //
   // Place midpoint at origin
   //   
   surface->translateMidpointToOrigin();
   
   //
   // Write the coordinate file
   //
   CoordinateFile* cf = surface->getCoordinateFile();
   cf->writeFile(outputCoordinateFileName);
}

      

