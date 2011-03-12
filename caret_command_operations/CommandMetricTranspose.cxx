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

#include "CommandMetricTranspose.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StatisticDescriptiveStatistics.h"

/**
 * constructor.
 */
CommandMetricTranspose::CommandMetricTranspose()
   : CommandBase("-metric-transpose",
                 "METRIC FILE TRANPOSE")
{
}

/**
 * destructor.
 */
CommandMetricTranspose::~CommandMetricTranspose()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricTranspose::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
    paramsOut.clear();
    paramsOut.addFile("Metric File Name", 
                     FileFilters::getMetricFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricTranspose::getHelpInformation() const
{
    QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Transpose a Metric File's Matrix.\n"
       + indent9 + "\n");
      
    return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricTranspose::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
    //
    // Get the name of the metric file
    //
    const QString metricFileName = 
      parameters->getNextParameterAsString("Input Metric File Name");
    const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
    //
    // Should not be any more parameters
    //
    checkForExcessiveParameters();

    MetricFile metricFile;
    MetricFile outputMetricFile;
    metricFile.readFile(metricFileName);
    std::cout << "Input Filename: " 
             << FileUtilities::basename(metricFileName).toAscii().constData()
             << std::endl;

    std::cout << "Output Filename: " 
             << FileUtilities::basename(outputMetricFileName).toAscii().constData()
             << std::endl;
    const int numNodes = metricFile.getNumberOfNodes();
    const int numCols  = metricFile.getNumberOfColumns();
    std::cout << "Number of Nodes: " << numNodes << std::endl;
    std::cout << "Number of Columns: " << numCols << std::endl;

    outputMetricFile.setNumberOfNodesAndColumns(numCols,numNodes);    

    for (int i=0;i<numNodes;i++)
        for (int j=0;j<numCols;j++)
        {
            outputMetricFile.setValue(j,i,metricFile.getValue(i,j));
        }
                
    outputMetricFile.writeFile(outputMetricFileName);
}

      

