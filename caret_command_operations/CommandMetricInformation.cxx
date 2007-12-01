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

#include "CommandMetricInformation.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StatisticDescriptiveStatistics.h"

/**
 * constructor.
 */
CommandMetricInformation::CommandMetricInformation()
   : CommandBase("-metric-information",
                 "METRIC FILE INFORMATION")
{
}

/**
 * destructor.
 */
CommandMetricInformation::~CommandMetricInformation()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricInformation::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Metric File Name", 
                     FileFilters::getMetricFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricInformation::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<metric-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Display Information about a metric file's contents.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricInformation::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the name of the metric file
   //
   const QString metricFileName = 
      parameters->getNextParameterAsString("Metric File Name");
   
   //
   // Should not be any more parameters
   //
   checkForExcessiveParameters();
   
   MetricFile metricFile;
   metricFile.readFile(metricFileName);
   
   std::cout << "Filename: " 
             << FileUtilities::basename(metricFileName).toAscii().constData()
             << std::endl;

   const int numNodes = metricFile.getNumberOfNodes();
   const int numCols  = metricFile.getNumberOfColumns();
   std::cout << "Number of Nodes: " << numNodes << std::endl;
   std::cout << "Number of Columns: " << numCols << std::endl;
   
   std::cout << QString("Column").rightJustified(6).toAscii().constData() << " "
             << QString("Minimum").rightJustified(12).toAscii().constData() << " "
             << QString("Maximum").rightJustified(12).toAscii().constData() << "   "
             << QString("Mean").rightJustified(12).toAscii().constData() << "   "
             << QString("Sample Dev").rightJustified(12).toAscii().constData() << "   "
             << QString("% Positive").rightJustified(12).toAscii().constData() << "   "
             << QString("% Negative").rightJustified(12).toAscii().constData() << "   "
             << "Column Name"
             << std::endl;
   for (int j = 0; j < numCols; j++) {
      std::vector<float> values;
      metricFile.getColumnForAllNodes(j, values);
      
      StatisticDescriptiveStatistics stats;
      stats.addDataArray(&values[0], numNodes);
      stats.execute();
      
      float minValue, maxValue;
      stats.getMinimumAndMaximum(minValue, maxValue);
      
      float percentNegative = 0.0;
      float percentPositive = 0.0;
      for (int i = 0; i < numNodes; i++) {
         if (values[i] > 0.0) {
            percentPositive += 1.0;
         }
         else if (values[i] < 0.0) {
            percentNegative += 1.0;
         }
      }
      percentNegative = (percentNegative / numNodes) * 100.0;
      percentPositive = (percentPositive / numNodes) * 100.0;
      
      const QString colString = QString::number(j + 1).rightJustified(6);
      const QString minString = QString::number(minValue, 'f', 3).rightJustified(12);
      const QString maxString = QString::number(maxValue, 'f', 3).rightJustified(12);
      const QString meanString = QString::number(stats.getMean(), 'f', 3).rightJustified(12);
      const QString devString = QString::number(stats.getPopulationSampleStandardDeviation(), 'f', 3).rightJustified(12);
      const QString negString = QString::number(percentNegative, 'f', 3).rightJustified(12);
      const QString posString = QString::number(percentPositive, 'f', 3).rightJustified(12);
      std::cout << colString.toAscii().constData() << " "
                << minString.toAscii().constData() << " "
                << maxString.toAscii().constData() << "   "
                << meanString.toAscii().constData() << "   "
                << devString.toAscii().constData() << "   "
                << posString.toAscii().constData() << "   "
                << negString.toAscii().constData() << "   "
                << metricFile.getColumnName(j).toAscii().constData()
                << std::endl;
   }
}

      

