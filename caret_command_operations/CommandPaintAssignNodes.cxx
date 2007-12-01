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

#include "CommandPaintAssignNodes.h"
#include "FileFilters.h"
#include "NodeRegionOfInterestFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPaintAssignNodes::CommandPaintAssignNodes()
   : CommandBase("-paint-assign-to-nodes",
                 "PAINT ASSIGN TO NODES")
{
}

/**
 * destructor.
 */
CommandPaintAssignNodes::~CommandPaintAssignNodes()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandPaintAssignNodes::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Paint File Name", 
                     FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File Name", 
                     FileFilters::getPaintFileFilter());
   paramsOut.addString("Paint Column");
   paramsOut.addString("Paint Name");
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandPaintAssignNodes::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <input-paint-file-name>\n"
       + indent9 + "   <output-paint-file-name>\n"
       + indent9 + "   <paint-column>\n"
       + indent9 + "   <paint-name>\n"
       + indent9 + "[-assign-from-roi-file   roi-file-name]\n"
       + indent9 + "[-set-column-name   name]\n"
       + indent9 + "\n"
       + indent9 + "Assign paint values to nodes.  The input paint file must\n"
       + indent9 + "exist.  If the input paint column is not found and its\n"
       + indent9 + "name is non-numeric, a new column will be added to the paint\n"
       + indent9 + "file if the paint file contains nodes.\n"
       + indent9 + "\n"
       + indent9 + "    \"column\" is either the number of the column, which\n"
       + indent9 + "    starts at one, or the name of the column.  If a\n"
       + indent9 + "    a name contains spaces, it must be enclosed in double \n" 
       + indent9 + "    quotes.  Name has priority over number.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandPaintAssignNodes::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputPaintFileName =
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName =
      parameters->getNextParameterAsString("Output Paint File Name");
   const QString paintColumn =
      parameters->getNextParameterAsString("Paint Column");
   const QString paintName =
      parameters->getNextParameterAsString("Paint Name");
      
   //
   // Read the paint file
   //
   PaintFile paintFile;
   paintFile.readFile(inputPaintFileName);
   
   //
   // Get the column number
   //
   const int paintColumnNumber = paintFile.getColumnFromNameOrNumber(paintColumn, true);
         
   //
   // Get index of paint name
   //
   const int paintNameIndex = paintFile.addPaintName(paintName);
   
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString("Paint Parameter");
      if (paramName == "-assign-from-roi-file") {
         const QString roiFileName =
            parameters->getNextParameterAsString("ROI File Name");
            
         //
         // Read the ROI file
         //
         NodeRegionOfInterestFile nodeRoiFile;
         nodeRoiFile.readFile(roiFileName);
         
         //
         // Assign paints
         //
         const int numNodes = std::max(paintFile.getNumberOfNodes(),
                                       nodeRoiFile.getNumberOfNodes());
         for (int i = 0; i < numNodes; i++) {
            if (nodeRoiFile.getNodeSelected(i) != 0) {
               paintFile.setPaint(i, paintColumnNumber, paintNameIndex);
            }
         }
      }
      else if (paramName == "-set-column-name") {
         const QString columnName =
            parameters->getNextParameterAsString("Set Column Name");
         paintFile.setColumnName(paintColumnNumber, columnName);
      }
   }
   
   
   //
   // Write the paint file
   //
   paintFile.writeFile(outputPaintFileName);
}

      

