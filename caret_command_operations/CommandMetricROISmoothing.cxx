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

#include "BrainModelSurfaceROIMetricSmoothing.h"
#include "BrainSet.h"
#include "CommandMetricROISmoothing.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricROISmoothing::CommandMetricROISmoothing()
   : CommandBase("-metric-roi-smoothing",
                 "METRIC ROI SMOOTHING")
{
}

/**
 * destructor.
 */
CommandMetricROISmoothing::~CommandMetricROISmoothing()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricROISmoothing::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("GEOGAUSS"); descriptions.push_back("Geodesic Gaussian");

   paramsOut.clear();
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input ROI File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addInt("Smoothing Number of Iterations", 50, 1, 100000);
   paramsOut.addFloat("Smoothing Strength", 1.0f);
   paramsOut.addVariableListOfParameters("Options");
   //TODO: add geodesic Gaussian to parameters...john?
}

/**
 * get full help information.
 */
QString 
CommandMetricROISmoothing::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-roi-file-name>\n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<smoothing-number-of-iterations>\n"
       + indent9 + "<smoothing-strength>\n"
       + indent9 + " \n"
       + indent9 + "[-sigma sigma-value]\n"
       + indent9 + " \n"
       + indent9 + "[-smooth-col column]\n"
       + indent9 + " \n"
       + indent9 + "Smooth metric data.\n"
       + indent9 + "\n"
       + indent9 + "Currently only Geodesic Gaussian is supported for ROI smoothing.\n"
       + indent9 + "\n"
       
       + indent9 + "\n"
       + indent9 + "   NOTE: The amount of smoothing is controlled solely by sigma\n"       
       + indent9 + "      (default 2.0), and iterations.  The intent is to do one\n"
       + indent9 + "       iteration of smoothing, with the sigma specifying how\n"
       + indent9 + "       much smoother the metric is desired to be.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricROISmoothing::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString roiInName =
      parameters->getNextParameterAsString("Input ROI File Name");
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   const int smoothingNumberOfIterations =
      parameters->getNextParameterAsInt("Smoothing Number of Iterations");
   const float smoothingStrength =
      parameters->getNextParameterAsFloat("Smoothing Strength");

   float geoGaussSigma = 2.0;
   bool parallelFlag = true;
   QString smoothColumnName;
   int smoothColumnNumber = 0;
   while (parameters->getParametersAvailable()) {
      const QString paramValue = parameters->getNextParameterAsString("Smoothing Parameter");
      if (paramValue == "-sigma") { 
         geoGaussSigma = 
            parameters->getNextParameterAsFloat("Geodesic Gaussian Sigma");
      }
      else if (paramValue == "-no-parallel") {
         parallelFlag = true;
      }
      else if (paramValue == "-smooth-col") {
         smoothColumnName = parameters->getNextParameterAsString("Column To Smooth");        
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramValue);
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     "",
                     true);
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Read input metric file
   //
   MetricFile metricFile;
   metricFile.readFile(inputMetricFileName);

   //
   // If SmoothColumnName exists, then convert it to 
   //
   
   if(!smoothColumnName.isEmpty())
   {
      //cout << "single column name is" << (char *)(smoothColumnName.toAscii().data()) << endl;
      smoothColumnNumber = metricFile.getColumnFromNameOrNumber(smoothColumnName, false);
   }
   //
   // Read input roi file
   //
   MetricFile roiIn;
   roiIn.readFile(roiInName);

   
   
   //
   // Perform smoothing
   //
   float gaussSigmaNorm  = 2.0;
   float gaussSigmaTang  = 2.0;
   float gaussNormBelow  = 2.0;
   float gaussNormAbove  = 2.0;
   float gaussTangCutoff = 3.0;

   
   if(!smoothColumnName.isEmpty())
   {
      //std::cout << "single column smoothing selected" << std::endl;
      MetricFile metricOut;
      int metricOutIndex = 0;
      metricOut.setNumberOfNodesAndColumns(metricFile.getNumberOfNodes(), 1);    
      float *valuesToSmooth;
      const int numberOfNodes = metricFile.getNumberOfNodes();
      valuesToSmooth = new float[numberOfNodes];
      metricFile.getColumnForAllNodes(smoothColumnNumber,valuesToSmooth);
      metricOut.setColumnForAllNodes(metricOutIndex,valuesToSmooth);
      //std::cout << "before getting column name" << std::endl;
      metricOut.setColumnName(metricOutIndex, metricFile.getColumnName(smoothColumnNumber));
      metricOut.setFileName(outputMetricFileName);

      BrainModelSurfaceROIMetricSmoothing smoothing(&brainSet,
                                                   surface,                                                   
                                                   &metricOut,
                                                   &roiIn,
                                                   metricOutIndex,
                                                   metricOutIndex,
                                                   metricOut.getColumnName(metricOutIndex),
                                                   smoothingStrength,
                                                   smoothingNumberOfIterations,
                                                   gaussNormBelow,
                                                   gaussNormAbove,
                                                   gaussSigmaNorm,
                                                   gaussSigmaTang,
                                                   gaussTangCutoff,
                                                   geoGaussSigma);
      smoothing.execute();

      metricOut.writeFile(outputMetricFileName);
   }
   else if(parallelFlag)
   {
      //
      // Perform parallel smoothing
      //
      const int numberOfColumns = metricFile.getNumberOfColumns();
      cout << "smoothing " << numberOfColumns << " columns." << endl;
      BrainModelSurfaceROIMetricSmoothing smoothing(&brainSet,
                                                      surface,
                                                      &metricFile,
                                                      &roiIn,
                                                      smoothingStrength,
                                                      smoothingNumberOfIterations,
                                                      gaussNormBelow,
                                                      gaussNormAbove,
                                                      gaussSigmaNorm,
                                                      gaussSigmaTang,
                                                      gaussTangCutoff,
                                                      geoGaussSigma,
                                                      true
                                                   );
      smoothing.execute();
      
      //
      // Write the metric file
      //
      metricFile.writeFile(outputMetricFileName);
   
      
      
   }
   else
   {
      //
      // Perform smoothing
      //
      const int numberOfColumns = metricFile.getNumberOfColumns();
      cout << "smoothing " << numberOfColumns << " columns." << endl;
      
      BrainModelSurfaceROIMetricSmoothing smoothing(&brainSet,
                                                   surface,                                                   
                                                   &metricFile,
                                                   &roiIn,                                                   
                                                   smoothingStrength,
                                                   smoothingNumberOfIterations,
                                                   gaussNormBelow,
                                                   gaussNormAbove,
                                                   gaussSigmaNorm,
                                                   gaussSigmaTang,
                                                   gaussTangCutoff,
                                                   geoGaussSigma,
                                                   false
                                                   );
         smoothing.execute();
      
      //
      // Write the metric file
      //
      metricFile.writeFile(outputMetricFileName);
   }
   
   
}

      

