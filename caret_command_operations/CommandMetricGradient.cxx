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

#include "CommandMetricGradient.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricGradient.h"
#include "BrainModelSurfaceMetricSmoothing.h"
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
CommandMetricGradient::CommandMetricGradient()
   : CommandBase("-metric-gradient",
                 "SURFACE GRADIENT OF A METRIC FILE")
{
}

/**
 * destructor.
 */
CommandMetricGradient::~CommandMetricGradient()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricGradient::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addString("Input Metric Column");
   paramsOut.addFile("Output Vector File", FileFilters::getGiftiVectorFileFilter());
   paramsOut.addFile("Output Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addInt("Output Metric Column Number", 1, 1);
   paramsOut.addBoolean("Average Normals", false);
   paramsOut.addFloat("Smoothing Kernel", -1.0, -1.0);
}

/**
 * get full help information.
 */
QString 
CommandMetricGradient::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<surface-coord>\n"
       + indent9 + "<surface-topo>\n"
       + indent9 + "<metric>\n"
       + indent9 + "<metric-col>\n"
       + indent9 + "<output-vector>\n"
       + indent9 + "<output-metric>\n"
       + indent9 + "<out-metric-col-num>\n"
       + indent9 + "<average-normals>\n"
       + indent9 + "<smooth-kernel>\n"
       + indent9 + "\n"
       + indent9 + "Generate the surface gradient of a metric file.  Uses a linear\n"
       + indent9 + "regression on a projection of the neighbor positions to a plane\n"
       + indent9 + "perpendicular to the surface normal, for each node.  Use \"NULL\"\n"
       + indent9 + "for either output file to skip storing that output.  If output-metric\n"
       + indent9 + "exists, the column specified by out-metric-col is replaced if it\n"
       + indent9 + "exists, otherwise output is appended as a new column.\n"
       + indent9 + "\n"
       + indent9 + "      surface-coord      the surface coord file\n"
       + indent9 + "\n"
       + indent9 + "      surface-topo       the surface topo file\n"
       + indent9 + "\n"
       + indent9 + "      metric             the metric file\n"
       + indent9 + "\n"
       + indent9 + "      metric-col         which column to take the gradient of\n"
       + indent9 + "\n"
       + indent9 + "      output-vector      the output gradient vector file\n"
       + indent9 + "\n"
       + indent9 + "      output-metric      output metric file for gradient magnitude\n"
       + indent9 + "\n"
       + indent9 + "      out-metric-col-num which column to put the gradient magnitude into\n"
       + indent9 + "\n"
       + indent9 + "      average-normals    uses an average of the normals of the node and all\n"
       + indent9 + "                       neighbors, use 'true' if your surface is not smooth.\n"
       + indent9 + "\n"
       + indent9 + "      smooth-kernel      applies smoothing before computing gradient.  Uses\n"
       + indent9 + "                       geodesic gaussian smoothing with specified kernel.\n"
       + indent9 + "                       Give a negative number to skip smoothing.  Kernel\n"
       + indent9 + "                       specifies the geodesic distance where weight is\n"
       + indent9 + "                       0.607, center node has weight of 1.\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricGradient::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString vector =
      parameters->getNextParameterAsString("Output Vector File");
   const QString mag =
      parameters->getNextParameterAsString("Output Metric File");
   int magCol =
      parameters->getNextParameterAsInt("Output Metric Column Number");
   bool avgNormals =
      parameters->getNextParameterAsBoolean("Average Surface Normals");
   float smoothing = 
      parameters->getNextParameterAsFloat("Smoothing Kernel");
   BrainSet mybs(topo, coord);
   BrainModelSurface* mysurf = mybs.getBrainModelSurface(0);
   MetricFile mymetric;
   mymetric.readFile(metric);
   int metricCol = mymetric.getColumnFromNameOrNumber(metricColName, false);
   if (smoothing > 0.0f)
   {
      BrainModelSurfaceMetricSmoothing mysmooth(&mybs, mysurf, mysurf, &mymetric,
                                                   BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN,
                                                   metricCol, metricCol, metricColName, 1.0f, 1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, smoothing);
      mysmooth.execute();
   }
   MetricFile* mymag = NULL;
   if (mag != QString("NULL"))
   {
      mymag = new MetricFile();
      mymag->setFileName(mag);
      try
      {
         mymag->readFile(mag);
      } catch (FileException e) {}//fail silently on nonexistant or no permissions, gets written later
   }
   VectorFile* myvec = NULL;
   if (vector != QString("NULL"))
   {
      myvec = new VectorFile();
      myvec->setFileName(vector);
   }
   BrainModelSurfaceMetricGradient myobject(NULL, mysurf, &mymetric, metricCol, myvec, mymag, magCol - 1, avgNormals);
   myobject.execute();
   if (myvec != NULL) myvec->writeFile(vector);
   if (mymag != NULL) mymag->writeFile(mag);
}
