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

#include "CommandMetricExtrema.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelSurfaceMetricExtrema.h"
#include "BrainSet.h"
#include "StringUtilities.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "VectorFile.h"
#include "CoordinateFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandMetricExtrema::CommandMetricExtrema()
   : CommandBase("-metric-extrema",
                 "LOCAL EXTREMA OF A METRIC FILE")
{
}

/**
 * destructor.
 */
CommandMetricExtrema::~CommandMetricExtrema()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricExtrema::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addString("Input Metric Column");
   paramsOut.addFile("Output Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addInt("Output Metric Column Number", 0, 0);
   paramsOut.addInt("Neighbor Depth", 1, 1);
}

/**
 * get full help information.
 */
QString 
CommandMetricExtrema::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<surface-coord>\n"
       + indent9 + "<surface-topo>\n"
       + indent9 + "<metric>\n"
       + indent9 + "<metric-col>\n"
       + indent9 + "<output-metric>\n"
       + indent9 + "<out-metric-col-num>\n"
       + indent9 + "<neighbor-depth>\n"
       + indent9 + "\n"
       + indent9 + "Find local extrema of a metric file.  Nodes with greater value\n"
       + indent9 + "than all neighbors to <neighbor-depth> hops on the mesh will have a\n"
       + indent9 + "value of 1 in the output, and likewise nodes less than will have a\n"
       + indent9 + "value of -1.  All other nodes will have value 0.  If the output file\n"
       + indent9 + "exists, the column with number out-metric-col-num is replaced if it\n"
       + indent9 + "exists, otherwise output is appended as a new column.\n"
       + indent9 + "\n"
       + indent9 + "      surface-coord      the surface coord file\n"
       + indent9 + "\n"
       + indent9 + "      surface-topo       the surface topo file\n"
       + indent9 + "\n"
       + indent9 + "      metric             the metric file\n"
       + indent9 + "\n"
       + indent9 + "      metric-col         which column to find the extrema of\n"
       + indent9 + "\n"
       + indent9 + "      output-metric      output metric file for min/max identification\n"
       + indent9 + "\n"
       + indent9 + "      out-metric-col-num which column to put the output into\n"
       + indent9 + "\n"
       + indent9 + "      neighbor-depth     distance in hops to include neighbors for calculation\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricExtrema::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coord =
      parameters->getNextParameterAsString("Input Coordinate File");
   const QString topo =
      parameters->getNextParameterAsString("Input Topology File");
   const QString metric =
      parameters->getNextParameterAsString("Input Metric File");
   const QString metricColName =
      parameters->getNextParameterAsString("Input Metric Column");
   const QString extrema =
      parameters->getNextParameterAsString("Output Metric File");
   int exCol =
      parameters->getNextParameterAsInt("Output Metric Column Number");
   int depth =
      parameters->getNextParameterAsInt("Neighbor Depth");
   BrainSet mybs(topo, coord);
   MetricFile mymetric;
   mymetric.readFile(metric);
   MetricFile myex;
   try
   {
      myex.readFile(extrema);
   } catch (FileException e) {}
   int metricCol = mymetric.getColumnFromNameOrNumber(metricColName, false);
   BrainModelSurfaceMetricExtrema myobject(&mybs, 0, &mymetric, metricCol, &myex, exCol - 1, depth);
   myobject.execute();
   myex.writeFile(extrema);
}
