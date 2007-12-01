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

#include "CommandMetricStatisticsLeveneMap.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricStatisticsLeveneMap::CommandMetricStatisticsLeveneMap()
   : CommandBase("-metric-statistics-levene-map",
                 "METRIC STATISTICS LEVENE MAP")
{
}

/**
 * destructor.
 */
CommandMetricStatisticsLeveneMap::~CommandMetricStatisticsLeveneMap()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricStatisticsLeveneMap::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addMultipleFiles("Input Metric File Names", FileFilters::getMetricShapeFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricStatisticsLeveneMap::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<input-metric-file-names>\n"
       + indent9 + "\n"
       + indent9 + "Compute a Levene Map.  The output metric file will contain\n"
       + indent9 + "the Levene F-Statistic, the numerator degrees of freedom,\n"
       + indent9 + "the denominator degrees of freedom, and a P-Value.\n"
       + indent9 + "\n"
       + indent9 + "References:\n"
       + indent9 + "   http://www.people.vcu.edu/~wsstreet/courses/314_20033/Handout.Levene.pdf\n"
       + indent9 + "   http://www.itl.nist.gov/div898/handbook/eda/section3/eda35a.htm\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricStatisticsLeveneMap::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   std::vector<MetricFile*> inputMetricFiles;
   while (parameters->getParametersAvailable()) {
      const QString inputMetricFileName =
         parameters->getNextParameterAsString("Input Metric File Name");
      MetricFile* mf = new MetricFile;
      mf->readFile(inputMetricFileName);
      inputMetricFiles.push_back(mf);
   }
   
   MetricFile* outputMetricFile = 
      MetricFile::computeStatisticalLeveneMap(inputMetricFiles);
      
   outputMetricFile->writeFile(outputMetricFileName);
   delete outputMetricFile;
}

      

