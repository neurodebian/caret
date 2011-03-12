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

#include <QTime>

#include "CommandMetricGradientAll.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceMetricGradient.h"
#include "BrainModelSurfaceMetricSmoothingAll.h"
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
CommandMetricGradientAll::CommandMetricGradientAll()
   : CommandBase("-metric-gradient-all",
                 "SURFACE GRADIENT OF ALL COLUMNS IN METRIC FILE")
{
}

/**
 * destructor.
 */
CommandMetricGradientAll::~CommandMetricGradientAll()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricGradientAll::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addBoolean("Average Normals", false);
   paramsOut.addFloat("Smoothing Kernel", -1.0, -1.0);
}

/**
 * get full help information.
 */
QString 
CommandMetricGradientAll::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<surface-coord>\n"
       + indent9 + "<surface-topo>\n"
       + indent9 + "<input-metric>\n"
       + indent9 + "<output-metric>\n"
       + indent9 + "<average-normals>\n"
       + indent9 + "<smooth-kernel>\n"
       + indent9 + "<parallel>\n"
       + indent9 + "\n"
       + indent9 + "Generate the surface gradient of a metric file.  Uses a linear\n"
       + indent9 + "regression on a projection of the neighbor positions to a plane\n"
       + indent9 + "perpendicular to the surface normal, for each node.  \n"
       + indent9 + "\n"
       + indent9 + "      surface-coord      the surface coord file\n"
       + indent9 + "\n"
       + indent9 + "      surface-topo       the surface topo file\n"
       + indent9 + "\n"
       + indent9 + "      input-metric       the input-metric file\n"
       + indent9 + "\n"
       + indent9 + "      output-metric      the output-metric file\n"
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
       + indent9 + "      parallel         Use true to run in parallel if code was compiled\n"
       + indent9 + "                       with OpenMP, otherwise false.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricGradientAll::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coord =
      parameters->getNextParameterAsString("Input Coordinate File");
   const QString topo =
      parameters->getNextParameterAsString("Input Topology File");
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric File");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File");
   bool avgNormals =
      parameters->getNextParameterAsBoolean("Average Surface Normals");
   float smoothing = 
      parameters->getNextParameterAsFloat("Smoothing Kernel");
   bool parallelFlag =
      parameters->getNextParameterAsBoolean("Parallel");
      
   QTime readTimer;
   readTimer.start();
   BrainSet mybs(topo, coord);
   BrainModelSurface* mysurf = mybs.getBrainModelSurface(0);
   MetricFile mymetric;
   mymetric.readFile(inputMetricFileName);
   std::cout << "Time to read files: " 
             << (readTimer.elapsed() * 0.001)
             << " seconds."
             << std::endl;
   if (smoothing > 0.0f)
   {
      QTime smoothTimer;
      smoothTimer.start();
      BrainModelSurfaceMetricSmoothingAll mysmooth(&mybs, mysurf, mysurf, &mymetric,
                                                   BrainModelSurfaceMetricSmoothingAll::SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN,
                                                   1.0f, 1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, smoothing, parallelFlag);                                 
      mysmooth.execute();
      const float elapsedTime = (static_cast<float>(smoothTimer.elapsed()) * 0.001);
      std::cout << "Smooth time: " << elapsedTime << std::endl;
   }
   int numCols = mymetric.getNumberOfColumns();
   
   QTime gradientTimer;
   gradientTimer.start();
   bool newFlag = true;
   if (newFlag) {
          BrainModelSurfaceMetricGradient myobject(
             &mybs, 0, &mymetric, &mymetric, avgNormals, parallelFlag);
          myobject.execute();
   }
   else {
      for (int i = 0; i < numCols; i++) {
          BrainModelSurfaceMetricGradient myobject(
             &mybs, 0, &mymetric, i, NULL, &mymetric, i, avgNormals);
          myobject.execute();
      }
   }
   const float elapsedTime = (static_cast<float>(gradientTimer.elapsed()) * 0.001);
   std::cout << "Gradient time: " << elapsedTime << std::endl;
   
   QTime writeTimer;
   writeTimer.start();
   mymetric.writeFile(outputMetricFileName);
   std::cout << "Time to write files: " 
             << (writeTimer.elapsed() * 0.001)
             << " seconds."
             << std::endl;
}
