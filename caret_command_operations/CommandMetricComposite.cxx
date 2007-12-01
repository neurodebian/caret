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

#include "CommandMetricComposite.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricComposite::CommandMetricComposite()
   : CommandBase("-metric-composite",
                 "METRIC FILE COMPOSITE ALL COLUMNS")
{
}

/**
 * destructor.
 */
CommandMetricComposite::~CommandMetricComposite()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricComposite::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Metric File Name", 
                     FileFilters::getMetricFileFilter());
   paramsOut.addMultipleFiles("Input Metric File Name(s)", 
                               FileFilters::getMetricFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricComposite::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-metric-file>  \n"
       + indent9 + "<input-metric-files...>\n"
       + indent9 + "\n"
       + indent9 + "Concatenate all columns from the input metric files and place\n"
       + indent9 + "them into the output metric file\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricComposite::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString outputMetricFileName = 
      parameters->getNextParameterAsString("Output Metric File Name");

   std::vector<QString> inputMetricFileNames;   
   parameters->getRemainingParametersAsStrings("Input Metric Files",
                                               inputMetricFileNames);
   const int numMetricFiles = static_cast<int>(inputMetricFileNames.size());
   if (numMetricFiles <= 0) {
      throw CommandException("Names of input metric files are missing.");
   }

   MetricFile outputMetricFile;
   
   for (int i = 0; i < numMetricFiles; i++) {
      if (outputMetricFile.getNumberOfColumns() == 0) {
         outputMetricFile.readFile(inputMetricFileNames[i]);
      }
      else {
         MetricFile inputMetricFile;
         inputMetricFile.readFile(inputMetricFileNames[i]);
         const int numCols = inputMetricFile.getNumberOfColumns();
         if (numCols > 0) {
            outputMetricFile.append(inputMetricFile);
         }
      }
   }
   outputMetricFile.writeFile(outputMetricFileName);
}

      

