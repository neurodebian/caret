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

#include "CommandMetricROIGradient.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceROIMetricGradient.h"
#include "BrainModelSurfaceROIMetricSmoothing.h"
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
CommandMetricROIGradient::CommandMetricROIGradient()
   : CommandBase("-metric-roi-gradient",
                 "METRIC ROI GRADIENT")
{
}

/**
 * destructor.
 */
CommandMetricROIGradient::~CommandMetricROIGradient()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricROIGradient::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Input Surface ROI File", FileFilters::getMetricFileFilter());
//   paramsOut.addFile("Output Vector File", FileFilters::getGiftiVectorFileFilter());
   paramsOut.addFile("Output Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addBoolean("Average Normals", false);
   paramsOut.addFloat("Smoothing Kernel", -1.0, -1.0);
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandMetricROIGradient::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<surface-coord>\n"
       + indent9 + "<surface-topo>\n"
	    + indent9 + "<surface-roi>\n"
       + indent9 + "<metric>\n"       
       + indent9 + "<output-metric>\n"
       + indent9 + "<average-normals>\n"
       + indent9 + "<smooth-kernel>\n"
       + indent9 + "\n"
       + indent9 + "[metric-col]\n"
       + indent9 + "[out-metric-col-num]\n"
       + indent9 + "[parallel-flag]\n"
       //+ indent9 + "[output-vector]\n"
       + indent9 + "\n"
       + indent9 + "Generate the surface gradient of a metric file on a ROI.  Uses a linear\n"
       + indent9 + "regression on a projection of the neighbor positions to a plane\n"
       + indent9 + "perpendicular to the surface normal, for each node.\n"  
       + indent9 + "\n"
       + indent9 + "Multi-Column computation is the default, and is run in parallel.  If\n"
       + indent9 + "the gradient of a single column is desired, then specify the input\n"
       + indent9 + "column name.\n"
       + indent9 + "\n"
       + indent9 + "For single column computations, the column specified by out-metric-col\n"
       + indent9 + "is replaced if it exists, otherwise output is appended as a new column.\n"
       + indent9 + "\n"
       + indent9 + "      surface-coord      the surface coord file\n"
       + indent9 + "\n"
       + indent9 + "      surface-topo       the surface topo file\n"
       + indent9 + "\n"
	    + indent9 + "      surface-roi       the surface roi file\n"
       + indent9 + "\n"
       + indent9 + "      metric             the metric file\n"
       + indent9 + "\n"
       + indent9 + "      output-metric      output metric file for gradient magnitude\n"
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
       + indent9 + "Optional parmeters:\n"
       + indent9 + "      metric-col         which column to take the gradient of, if none specified\n"
       + indent9 + "                       the command is run on all columns\n"
       + indent9 + "\n"
       + indent9 + "      out-metric-col-num which column to put the gradient magnitude into, if not\n"
       + indent9 + "                       specified, then it is appended to the output metric file.\n"
       + indent9 + "                       This parameter is only used for single column computations.\n"
       + indent9 + "\n"
       + indent9 + "      single-threaded    For multi-column gradients, this option disable parallel\n"
       + indent9 + "                       processing.  This is recommended for troubleshooting purposes\n"
       + indent9 + "                       only\n"
       //+ indent9 + "      output-vector      the output gradient vector file\n"
       //+ indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricROIGradient::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coord =
      parameters->getNextParameterAsString("Input Coordinate File");
   const QString topo =
      parameters->getNextParameterAsString("Input Topology File");
   const QString roi =
	  parameters->getNextParameterAsString("Input ROI File");
   const QString metric =
      parameters->getNextParameterAsString("Input Metric File"); 
   const QString vector = "NULL";
      //parameters->getNextParameterAsString("Output Vector File");
   const QString mag =
      parameters->getNextParameterAsString("Output Metric File");
   bool avgNormals =
      parameters->getNextParameterAsBoolean("Average Surface Normals");
   float smoothing = 
      parameters->getNextParameterAsFloat("Smoothing Kernel");
   
   QString metricColName;     
   int magCol = -1;//setting to -1 will append to the output metric file by default
   bool allColumnsFlag = true;
   bool parallelFlag = true;
      
   while (parameters->getParametersAvailable()) {
      const QString paramValue = parameters->getNextParameterAsString("ROI Gradient Parameter");
      if (paramValue == "-metric-col") { 
         metricColName = parameters->getNextParameterAsString("Input Metric Column");
         allColumnsFlag = false;
      }
      else if (paramValue == "-single-threaded") { 
         parallelFlag = false;      
      }
      else if (paramValue == "-out-metric-col-num") {
         magCol = parameters->getNextParameterAsInt("Output Metric Column Number");         
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramValue);
      }
   }
   BrainSet mybs(topo, coord);
   BrainModelSurface* mysurf = mybs.getBrainModelSurface(0);
   MetricFile mymetric;
   mymetric.readFile(metric);
   MetricFile myroi;
   myroi.readFile(roi);
   
   if(allColumnsFlag)
   {
      if (smoothing > 0.0f)
      {
         BrainModelSurfaceROIMetricSmoothing mysmooth(&mybs, mysurf, &mymetric, &myroi,                                                   
                                                      1.0f, 1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, smoothing, parallelFlag);
         mysmooth.execute();         
      }
      BrainModelSurfaceROIMetricGradient myobject(&mybs, 0, &myroi, &mymetric, avgNormals,parallelFlag);
      myobject.execute();
      if (mag != QString("NULL")) mymetric.writeFile(mag);
   }
   else
   {
      int metricCol = mymetric.getColumnFromNameOrNumber(metricColName, false);
      
      if (smoothing > 0.0f)
      {
         BrainModelSurfaceROIMetricSmoothing mysmooth(&mybs, mysurf, &mymetric, &myroi,                                                   
                                                      metricCol, metricCol, metricColName, 1.0f, 1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, smoothing);
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
      if ((vector != QString("NULL")))
      {
         myvec = new VectorFile();
         myvec->setFileName(vector);
      }
      BrainModelSurfaceROIMetricGradient myobject(&mybs, 0, &myroi, &mymetric, metricCol, myvec, mymag, magCol - 1, avgNormals);
      myobject.execute();
      if (myvec != NULL) myvec->writeFile(vector);
      if (mymag != NULL) mymag->writeFile(mag);
   }
   
}
