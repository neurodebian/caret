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

#include <iostream>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceInformation.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandSurfaceInformation::CommandSurfaceInformation()
   : CommandBase("-surface-information",
                 "SURFACE INFORMATION")
{
}

/**
 * destructor.
 */
CommandSurfaceInformation::~CommandSurfaceInformation()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceInformation::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addVariableListOfParameters("Information Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceInformation::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name> \n"
       + indent9 + "<topology-file-name> \n"
       + indent9 + "[-show-volume] \n"
       + indent9 + "[-text-file  output-text-file-name] \n"
       + indent9 + "\n"
       + indent9 + "Display information about a surface.\n"
       + indent9 + "\n"
       + indent9 + "If \"-show-volume\" is specified, the volume displaced in\n"
       + indent9 + "cubic millimeters is displayed.  The supplied topology file \n"
       + indent9 + "MUST be CLOSED if this option is used.  Use of this option\n"
       + indent9 + "may significantly increase the execution time of this  \n"
       + indent9 + "command.\n"
       + indent9 + "\n"
       + indent9 + "The information is printed to the terminal unless a\n"
       + indent9 + "text file is specified with the \"-text-file\" option.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceInformation::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");

   //
   // Check optional parameters
   //
   QString textFileName;
   bool showVolumeFlag = false;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("SURFACE INFORMATION parameter");
      if ("-show-volume") {
         showVolumeFlag = true;
      }
      else if (paramName == "-text-file") {
         textFileName = parameters->getNextParameterAsString("Text File Name");
      }
      else {
         throw CommandException("unknown parameter: "
                                + paramName);
      }
   }
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
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes == 0) {      
      throw CommandException("surface contains not nodes.");  
   }

   //
   // Get the surface information
   //
   std::vector<QString> labels, values;
   bms->getSurfaceInformation(labels,
                              values);

   if (showVolumeFlag) {
      const float volumeMM = bms->getSurfaceVolumeDisplacement();
      labels.push_back("Volume Displacement");
      if (volumeMM < 0.0) {
         values.push_back("Volume computation failed.  Open Topology?");
      }
      else {
         values.push_back(QString::number(volumeMM, 'f', 3) + " cubic millimeters");
      }
   }
   
   //
   // Determine minimum width for labels
   //
   const int numItems = static_cast<int>(labels.size());
   int minWidth = 1;
   for (int i = 0; i < numItems; i++) {
      minWidth = std::max(minWidth, labels[i].length());
   }
   minWidth += 2;

   QString s;
   for (int i = 0; i < numItems; i++) {
      const QString space((minWidth - labels[i].size()), QChar(' '));
      s += (labels[i]
            + ":"
            + space
            + values[i]
            + "\n");
   }

   if (textFileName.isEmpty() == false) {
      TextFile textFile;
      textFile.setText(s);
      textFile.writeFile(textFileName);
   }
   else {
      std::cout << s.toAscii().constData()
                << std::endl;
   }
}

      

