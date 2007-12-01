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

#include "CommandPaintFileCreate.h"
#include "CoordinateFile.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPaintFileCreate::CommandPaintFileCreate()
   : CommandBase("-paint-file-create",
                 "PAINT FILE CREATION")
{
}

/**
 * destructor.
 */
CommandPaintFileCreate::~CommandPaintFileCreate()
{
}

/**
 * get full help information.
 */
QString 
CommandPaintFileCreate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<paint-file-name>  \n"
       + indent9 + "<number-of-columns> \n"
       + indent9 + "[-number-of-nodes  number-of-nodes] \n"
       + indent9 + "[-coordinate-file  coordinate-file-for-number-of-nodes] \n"
       + indent9 + "[-set-column-name  column-number  column-name]\n"
       + indent9 + "\n"
       + indent9 + "Create a paint file with the specified number of nodes\n"
       + indent9 + "and columns with all values initialized to zero.  \n"
       + indent9 + "\n"
       + indent9 + "The number of columns MUST be specified using either the\n"
       + indent9 + "\"-number-of-nodes\" option or the \"-coordinate-file\"\n"
       + indent9 + "in which case the number of nodes is set to the number\n"
       + indent9 + "of nodes in the coordinate file.\n"
       + indent9 + "\n"
       + indent9 + "When setting column names, the \"column-number\" starts at one.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandPaintFileCreate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get name, columns
   //
   const QString paintFileName =
      parameters->getNextParameterAsString("Paint File Name");
   const int numberOfColumns =
      parameters->getNextParameterAsInt("Number of Columns");
      
   if (numberOfColumns <= 0) {
      throw CommandException("Number of columns must be "
                             "greater than zero.");
   }
   int numberOfNodes = -1;
   
   //
   // Process optional parameters
   //
   std::vector<QString> paintColumnNames(numberOfColumns, "");
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Paint Create Parameter");
      if (paramName == "-number-of-nodes") {
         numberOfNodes = parameters->getNextParameterAsInt("Number of Nodes");
      }
      else if (paramName == "-coordinate-file") {
         const QString coordinateFileName =
            parameters->getNextParameterAsString("Coordinate File Name");
         CoordinateFile coordinateFile;
         coordinateFile.readFile(coordinateFileName);
         numberOfNodes = coordinateFile.getNumberOfCoordinates();
      }
      else if (paramName == "-set-column-name") {
         int paintColumnNumber = parameters->getNextParameterAsInt("Column Number");
         if ((paintColumnNumber < 1) ||
             (paintColumnNumber > numberOfColumns)) {
            throw CommandException("Invalid paint column number"
                                   + QString::number(paintColumnNumber));
         }
         paintColumnNumber--; // index starting at zero
         
         paintColumnNames[paintColumnNumber] = parameters->getNextParameterAsString("Column Name");
      }
      else {
         throw CommandException("Unrecognized parameter: "
                                + paramName);
      }
   }
   
   //
   // Error check
   //
   if (numberOfNodes <= 0) {
      throw CommandException("Number of nodes must be "
                             "greater than zero.");
   }
   
   //
   // Create the file
   //
   PaintFile paintFile;
   paintFile.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns);
   paintFile.addPaintName("???");

   for (int i = 0; i < numberOfColumns; i++) {
      if (paintColumnNames[i].isEmpty() == false) {
         //
         // Set the column name
         //
         paintFile.setColumnName(i,
                                 paintColumnNames[i]);
      }
   }
   
   //
   // Write the file
   //
   paintFile.writeFile(paintFileName);
}

/**
 * get the script builder parameters.
 */
void 
CommandPaintFileCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Paint File Name", 
                     FileFilters::getPaintFileFilter());
   paramsOut.addInt("Number of Columns", 1, 1, 100000);
   paramsOut.addVariableListOfParameters("Input Paint Files/Columns");
}


      

