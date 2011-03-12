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

#include "CommandMetricFileCreate.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricFileCreate::CommandMetricFileCreate()
   : CommandBase("-metric-file-create",
                 "METRIC FILE CREATION")
{
}

/**
 * destructor.
 */
CommandMetricFileCreate::~CommandMetricFileCreate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricFileCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Metric File Name", 
                     FileFilters::getMetricFileFilter());
   paramsOut.addInt("Number of Columns", 1);
   paramsOut.addVariableListOfParameters("Input Metric Files/Columns");
}

/**
 * get full help information.
 */
QString 
CommandMetricFileCreate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<metric-file-name>  \n"
       + indent9 + "<number-of-columns> \n"
       + indent9 + "[-number-of-nodes  number-of-nodes] \n"
       + indent9 + "[-coordinate-file  coordinate-file-for-number-of-nodes] \n"
       + indent9 + "[-set-column-name  column-number  column-name]\n"
       + indent9 + "[-random]\n"
       + indent9 + "\n"
       + indent9 + "Create a metric file with the specified number of nodes\n"
       + indent9 + "and columns with all values initialized to zero (default) \n"
       + indent9 + "or random numbers in the range zero to one if the \n"
       + indent9 + "\"-random\" option is specified.\n"
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
CommandMetricFileCreate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get name, columns
   //
   const QString metricFileName =
      parameters->getNextParameterAsString("Metric File Name");
   const int numberOfColumns =
      parameters->getNextParameterAsInt("Number of Columns");
      
   if (numberOfColumns <= 0) {
      throw CommandException("Number of columns must be "
                             "greater than zero.");
   }
   int numberOfNodes = -1;
   bool randomFlag = false;
   
   //
   // Process optional parameters
   //
   std::vector<QString> metricColumnNames(numberOfColumns, "");
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Metric Create Parameter");
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
      else if (paramName == "-random") {
         randomFlag = true;
      }
      else if (paramName == "-set-column-name") {
         int metricColumnNumber = parameters->getNextParameterAsInt("Column Number");
         if ((metricColumnNumber < 1) ||
             (metricColumnNumber > numberOfColumns)) {
            throw CommandException("Invalid metric column number"
                                   + QString::number(metricColumnNumber));
         }
         metricColumnNumber--; // index starting at zero
         
         metricColumnNames[metricColumnNumber] = parameters->getNextParameterAsString("Column Name");
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
   MetricFile metricFile;
   metricFile.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns);

   for (int i = 0; i < numberOfColumns; i++) {
      if (metricColumnNames[i].isEmpty() == false) {
         //
         // Set the column name
         //
         metricFile.setColumnName(i,
                                 metricColumnNames[i]);
      }
   }
   
   if (randomFlag) {
      const double divisor = RAND_MAX + 1.0f;
      for (int i = 0; i < numberOfColumns; i++) {
         GiftiDataArray* gda = metricFile.getDataArray(i);
         float* data = gda->getDataPointerFloat();
         for (int j = 0; j < numberOfNodes; j++) {
            float v = ::rand()/(divisor);
            if (v < 0.0) {
               v = 0.0;
            }
            else if (v > 1.0) {
               v = 1.0;
            }
            data[j] = v;
         }
      }
   }
   
   //
   // Write the file
   //
   metricFile.writeFile(metricFileName);
}

      

