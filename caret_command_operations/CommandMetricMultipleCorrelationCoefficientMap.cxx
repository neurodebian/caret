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

#include "CommandMetricMultipleCorrelationCoefficientMap.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricMultipleCorrelationCoefficientMap::CommandMetricMultipleCorrelationCoefficientMap()
   : CommandBase("-metric-multiple-correlation-coefficient-map",
                 "METRIC MULTIPLE CORRELATION COEFFICIENT MAP")
{
}

/**
 * destructor.
 */
CommandMetricMultipleCorrelationCoefficientMap::~CommandMetricMultipleCorrelationCoefficientMap()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricMultipleCorrelationCoefficientMap::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("Dependent Metric File Name", FileFilters::getMetricShapeFileFilter());
   paramsOut.addMultipleFiles("Independent Metric File Names", FileFilters::getMetricShapeFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricMultipleCorrelationCoefficientMap::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<dependent-metric-file-name>\n"
       + indent9 + "<independent-metric-file-names>\n"
       + indent9 + "\n"
       + indent9 + "For each node, compute a correlation coefficient from the node's values\n"
       + indent9 + "in the input metric files.  The two input files must have the same\n"
       + indent9 + "number of columns and column 'j' in all files should contain data\n"
       + indent9 + "for the same subject.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricMultipleCorrelationCoefficientMap::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString outputMetricFileName = 
      parameters->getNextParameterAsString("Output Metric File Name");
   const QString dependentMetricFileName = 
      parameters->getNextParameterAsString("Dependent Metric File Name");
   std::vector<QString> independentMetricFileNames;
   while (parameters->getParametersAvailable()) {
      independentMetricFileNames.push_back( 
         parameters->getNextParameterAsString("Independent Metric File Name"));
   }
   
   MetricFile dependentMetricFile;
   dependentMetricFile.readFile(dependentMetricFileName);
   std::vector<MetricFile*> independentMetricFiles;
   for (unsigned int i = 0; i < independentMetricFileNames.size(); i++) {
      MetricFile* mf = new MetricFile;
      mf->readFile(independentMetricFileNames[i]);
      independentMetricFiles.push_back(mf);
   }
   MetricFile* outputMetricFile = MetricFile::computeMultipleCorrelationCoefficientMap(&dependentMetricFile,
                                                                               independentMetricFiles);
   outputMetricFile->writeFile(outputMetricFileName);
   delete outputMetricFile;
   outputMetricFile = NULL;
   for (unsigned int i = 0; i < independentMetricFileNames.size(); i++) {
      delete independentMetricFiles[i];
      independentMetricFiles[i] = NULL;
   }
}

      

