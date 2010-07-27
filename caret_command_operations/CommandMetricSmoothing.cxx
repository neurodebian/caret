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

#include "BrainModelSurfaceMetricSmoothing.h"
#include "BrainSet.h"
#include "CommandMetricSmoothing.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricSmoothing::CommandMetricSmoothing()
   : CommandBase("-metric-smoothing",
                 "METRIC SMOOTHING")
{
}

/**
 * destructor.
 */
CommandMetricSmoothing::~CommandMetricSmoothing()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricSmoothing::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("AN");      descriptions.push_back("Average Neighbors");
   values.push_back("DILATE");  descriptions.push_back("Dilation");
   values.push_back("FWHM");   descriptions.push_back("Full Width Half Maximum");
   values.push_back("GAUSS");   descriptions.push_back("Gaussian");
   values.push_back("GEOGAUSS"); descriptions.push_back("Geodesic Gaussian");
   values.push_back("WAN");     descriptions.push_back("Weighted Average Neighbors");

   paramsOut.clear();
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addListOfItems("Smoothing Algorithm", values, descriptions);
   paramsOut.addInt("Smoothing Number of Iterations", 50, 1, 100000);
   paramsOut.addFloat("Smoothing Strength", 1.0, 0.0, 1.0);
   paramsOut.addVariableListOfParameters("Options");
   //TODO: add geodesic Gaussian to parameters...john?
}

/**
 * get full help information.
 */
QString 
CommandMetricSmoothing::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<smoothing-algorithm>\n"
       + indent9 + "<smoothing-number-of-iterations>\n"
       + indent9 + "<smoothing-strength>\n"
       + indent9 + " \n"
       + indent9 + "[-geo-gauss sigma] \n"
       + indent9 + " \n"
       + indent9 + "[-fwhm  desired-full-width-half-maximum] \n"
       + indent9 + " \n"
       + indent9 + "[-gauss   spherical-coordinate-file-name\n"
       + indent9 + "          sigma-norm\n"
       + indent9 + "          sigma-tang\n"
       + indent9 + "          norm below cutoff (mm)\n"
       + indent9 + "          norm above cutoff (mm)\n"
       + indent9 + "          tang-cutoff (mm)]\n"
       + indent9 + "\n"
       + indent9 + "Smooth metric data.\n"
       + indent9 + "\n"
       + indent9 + "\"smoothing-algorithm\" is one of:\n"
       + indent9 + "   AN      Average Neighbors\n"
       + indent9 + "   DILATE  Dilation\n"
       + indent9 + "   FWHM    Full-Width Half-Maximum\n"
       + indent9 + "   GAUSS   Gaussian, requires -gauss\n"
       + indent9 + "   GEOGAUSS   Geodesic Gaussian, uses -geo-gauss, default 2.0\n"
       + indent9 + "   WAN     Weighted Average Neighbors\n"
       + indent9 + "\n"
       + indent9 + "   NOTE: Geodesic Gaussian IGNORES the strength parameter,\n"
       + indent9 + "      amount of smoothing is controlled solely by sigma and\n"
       + indent9 + "      iterations.  A very small sigma (around a fourth of\n"
       + indent9 + "      node spacing) may result in no smoothing on many nodes,\n"
       + indent9 + "      as the neighbor cutoff distance is 4 times the sigma.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricSmoothing::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputMetricFileName =
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName =
      parameters->getNextParameterAsString("Output Metric File Name");
   const QString algorithmName =
      parameters->getNextParameterAsString("Smoothing Algorithm");
   const int smoothingNumberOfIterations =
      parameters->getNextParameterAsInt("Smoothing Number of Iterations");
   const float smoothingStrength =
      parameters->getNextParameterAsFloat("Smoothing Strength");

   float desiredFullWidthHalfMaximum = 0.0;
   QString gaussianSphericalSurfaceName;
   float gaussSigmaNorm  = 2.0;
   float gaussSigmaTang  = 2.0;
   float gaussNormBelow  = 2.0;
   float gaussNormAbove  = 2.0;
   float gaussTangCutoff = 3.0;
   float geoGaussSigma = 2.0;
   while (parameters->getParametersAvailable()) {
      const QString paramValue = parameters->getNextParameterAsString("Gaussian Parameter");
      if (paramValue == "-fwhm") {
         desiredFullWidthHalfMaximum = 
            parameters->getNextParameterAsFloat("Desired Full Width Half Maximum");
      }
      else if (paramValue == "-gauss") { 
         gaussianSphericalSurfaceName = 
            parameters->getNextParameterAsString("Guassian Spherical Surface");
         gaussSigmaNorm = 
            parameters->getNextParameterAsFloat("Gaussian Sigma Norm");
         gaussSigmaTang = 
            parameters->getNextParameterAsFloat("Gaussian Sigma Tangent");
         gaussNormBelow = 
            parameters->getNextParameterAsFloat("Gaussina Norm Below Cutoff");
         gaussNormAbove = 
            parameters->getNextParameterAsFloat("Gaussian Norm Above Cutoff");
         gaussTangCutoff = 
            parameters->getNextParameterAsFloat("Gaussian Tangent Cutoff");
      }
      else if (paramValue == "-geo-gauss") { 
         geoGaussSigma = 
            parameters->getNextParameterAsFloat("Geodesic Gaussian Sigma");
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramValue);
      }
   }
   
   //
   // Get algorithm
   //
   BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM smoothingAlgorithm;
   if (algorithmName == "AN") {
      smoothingAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS;
   }
   else if (algorithmName == "DILATE") {
      smoothingAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_DILATE;
   }
   else if (algorithmName == "FWHM") {
      smoothingAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_FULL_WIDTH_HALF_MAXIMUM;
   }
   else if (algorithmName == "GAUSS") {
      smoothingAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_SURFACE_NORMAL_GAUSSIAN;
   }
   else if (algorithmName == "GEOGAUSS") {
      smoothingAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN;
   }
   else if (algorithmName == "WAN") {
      smoothingAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS;
   }
   else {
      throw CommandException("Invalid algorithm: \""
                             + algorithmName
                             + "\"");
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     gaussianSphericalSurfaceName,
                     true);
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   BrainModelSurface* gaussianSphericalSurface = NULL;
   if (gaussianSphericalSurfaceName.isEmpty() == false) {
      gaussianSphericalSurface = brainSet.getBrainModelSurface(1);
      if (gaussianSphericalSurface == NULL) {
         throw CommandException("unable to find spherical surface.");
      }
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
   // Perform smoothing
   //
   const int numberOfColumns = metricFile.getNumberOfColumns();
   for (int i = 0; i < numberOfColumns; i++) {
      BrainModelSurfaceMetricSmoothing smoothing(&brainSet,
                                                   surface,
                                                   gaussianSphericalSurface,
                                                   &metricFile,
                                                   smoothingAlgorithm,
                                                   i,
                                                   i,
                                                   metricFile.getColumnName(i),
                                                   smoothingStrength,
                                                   smoothingNumberOfIterations,
                                                   desiredFullWidthHalfMaximum,
                                                   gaussNormBelow,
                                                   gaussNormAbove,
                                                   gaussSigmaNorm,
                                                   gaussSigmaTang,
                                                   gaussTangCutoff,
                                                   geoGaussSigma);
      smoothing.execute();
   }
   
   //
   // Write the metric file
   //
   metricFile.writeFile(outputMetricFileName);
}

      

