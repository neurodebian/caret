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
#include <fstream>

#include "CommandMetricROIMask.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StringUtilities.h"
#include "FileUtilities.h"
#include "MetricFile.h"

/**
 * constructor.
 */
CommandMetricROIMask::CommandMetricROIMask()
   : CommandBase("-metric-roi-mask",
                 "METRIC ROI MASK")
{
}

/**
 * destructor.
 */
CommandMetricROIMask::~CommandMetricROIMask()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricROIMask::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addString("Input Metric Column");
   paramsOut.addFile("Input Surface ROI File", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Metric File", FileFilters::getMetricFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandMetricROIMask::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<metric>\n"
       + indent9 + "<metric-col>\n"     
	    + indent9 + "<surface-roi>\n"
       + indent9 + "<output-metric>\n"
       + indent9 + "\n"
       + indent9 + "Applies a mask to the input metric file, and zeros out any values\n"
       + indent9 + "where the mask is 0.0f.\n"
       + indent9 + "\n"
       + indent9 + "      metric             the metric file\n"
       + indent9 + "\n"
       + indent9 + "      metric-col         which column to apply the mask to\n"
       + indent9 + "\n"
	    + indent9 + "      surface-roi        the surface roi file\n"
       + indent9 + "\n"
       + indent9 + "      output-metric      output metric file for mask\n"
       + indent9 + "\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricROIMask::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString metricInName =
      parameters->getNextParameterAsString("Input Metric File");
   const QString metricInColName =
      parameters->getNextParameterAsString("Input Metric Column");
   const QString roiInName =
	  parameters->getNextParameterAsString("Input ROI File");
   const QString metricOutName =
      parameters->getNextParameterAsString("Output Metric File");  

   MetricFile metricIn;
   metricIn.readFile(metricInName);
   if ((metricIn.getNumberOfColumns() <= 0) || 
       (metricIn.getNumberOfNodes() <= 0)) {
      throw CommandException("Metric file contains no data.");
   }


   MetricFile roiIn;
   roiIn.readFile(roiInName);
   if ((roiIn.getNumberOfColumns() <= 0) || 
       (roiIn.getNumberOfNodes() <= 0)) {
      throw CommandException("Roi file contains no data.");
   }

   if(roiIn.getNumberOfNodes() != metricIn.getNumberOfNodes())
   {
      throw CommandException("Input Metric File and Input ROI File contain different numbers of nodes.");
   }

   int metricInCol = metricIn.getColumnFromNameOrNumber(metricInColName, false);
   MetricFile metricOut;
   if (metricOutName != QString("NULL"))
   {
      metricOut.setFileName(metricOutName);
      metricOut.setNumberOfNodesAndColumns(metricIn.getNumberOfNodes(),1);
      metricOut.setColumnName(0,"Masked Column");
   }

   float *roiValues = NULL; 
   int numberOfNodes = 0;
   numberOfNodes = roiIn.getNumberOfNodes();

   //
   // Get array of ROI values for all nodes
   //

   roiValues = new float[numberOfNodes];
   roiIn.getColumnForAllNodes(0,roiValues);
   
   MaskSingleColumn(&metricIn,metricInCol, roiValues, &metricOut);
   if(metricOutName != QString("NULL")) metricOut.writeFile(metricOutName);
}

void 
CommandMetricROIMask::MaskSingleColumn(MetricFile * metricIn, int metricInCol, float *roiValues, MetricFile * metricOut)
{
   float *metricInValues = NULL;
   float *metricOutValues = NULL;
   int numberOfNodes = 0;
   numberOfNodes = metricIn->getNumberOfNodes();
   metricInValues = new float[numberOfNodes];
   metricOutValues = new float[numberOfNodes];
   metricIn->getColumnForAllNodes(metricInCol, metricInValues);

   for(int i = 0;i<numberOfNodes;i++)
   {
      if(roiValues[i] == 0.0f)
         metricOutValues[i] = 0.0f;
      else
         metricOutValues[i] = metricInValues[i];
   }
   metricOut->setColumnForAllNodes(0,metricOutValues);   
   delete metricInValues;
   delete metricOutValues;
   return;
}


