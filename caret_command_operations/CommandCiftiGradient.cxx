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
#include <QTime>

#include "BrainModelCiftiGradient.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandCiftiGradient.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "DebugControl.h"

/**
 * constructor.
 */
CommandCiftiGradient::CommandCiftiGradient()
   : CommandBase("-cifti-gradient",
                 "CIFTI GRADIENT")
{
}

/**
 * get the script builder parameters.
 */
void 
CommandCiftiGradient::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Cifti File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Cifti File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Left Surface Topology", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Left Surface Coordinates", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Right Surface Topology", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Right Surface Coordinates", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFloat("Surface Presmoothing Kernel", 2.0f);
   paramsOut.addFloat("Volume Gradient Kernel", 2.0f, 0.01f);
   paramsOut.addBoolean("IndividualOutput", false);
   paramsOut.addBoolean("AverageNormals", false);
   paramsOut.addBoolean("Parallel flag", true);
   //paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandCiftiGradient::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-cifti>\n"
       + indent9 + "<output-cifti>\n"
       + indent9 + "<left-coordinate-file>\n"
       + indent9 + "<left-topology-file>\n"
       + indent9 + "<right-coordinate-file>\n"
       + indent9 + "<right-topology-file>\n"
       + indent9 + "<surface-presmooth-kernel>\n"
       + indent9 + "<volume-gradient-kernel>\n"
       + indent9 + "[<individual-output>]\n"
       + indent9 + "[<average-normals>]\n"
       + indent9 + "[<parallel-flag>]\n"
       + indent9 + "\n"
       + indent9 + "Compute the gradient of the input cifti file.\n"
       + indent9 + "Takes the gradient of each column (metric) in the cifti\n"
       + indent9 + "file separated by mapping info used as ROIs, that is, each\n"
       + indent9 + "structure in the cifti file uses an ROI of its extent during\n"
       + indent9 + "smoothing, if applicable, and gradient.\n"
       + indent9 + "\n"
       + indent9 + "The output file has the same number of rows as the input,\n"
       + indent9 + "with the same mapping.  If the optional argument\n"
       + indent9 + "<individual-output> is \"true\", this will also be true of\n"
       + indent9 + "the columns, otherwise only one column is output, with no\n"
       + indent9 + "mapping information, which is the average across the\n"
       + indent9 + "columns of their respective gradients.\n"
       + indent9 + "\n"
       + indent9 + "If <average-normals> is \"true\", surface gradient will\n"
       + indent9 + "average surface normals with those of its neighbors to\n"
       + indent9 + "obtain a smoother set of normals.\n"
       + indent9 + "\n"
       + indent9 + "If <parallel-flag> is \"false\", the algorithm will not execute\n"
       + indent9 + "in parallel (number of threads equal to number of cores), by\n"
       + indent9 + "default it will run in parallel.\n"
       + indent9 + "\n"
       + indent9 + "When specifying an optional argument, all previous optional\n"
       + indent9 + "arguments must also have a value.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandCiftiGradient::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the input and output file names
   //
   const QString inputCiftiFileName =
      parameters->getNextParameterAsString("Input Cifti File Name");
   const QString outputCiftiFileName =
      parameters->getNextParameterAsString("Output Cifti File Name");
   const QString leftCoordName =
      parameters->getNextParameterAsString("Left Coordinate File Name");
   const QString leftTopoName =
      parameters->getNextParameterAsString("Left Topology File Name");
   const QString rightCoordName =
      parameters->getNextParameterAsString("Right Coordinate File Name");
   const QString rightTopoName =
      parameters->getNextParameterAsString("Right Topology File Name");
   float surfaceKernel =
      parameters->getNextParameterAsFloat("Surface Presmoothing Kernel");
   float volumeKernel =
      parameters->getNextParameterAsFloat("Volume Gradient Kernel");
   bool indOutput = false;
   bool avgNormals = false;
   bool parallelFlag = true;
   //
   // Process optional parameters
   //
   if (parameters->getParametersAvailable())
   {
      indOutput = parameters->getNextParameterAsBoolean("Individual Gradients Output");
   }
   if (parameters->getParametersAvailable())
   {
      avgNormals = parameters->getNextParameterAsBoolean("Average Surface Normals");
   }
   if (parameters->getParametersAvailable())
   {
      parallelFlag = parameters->getNextParameterAsBoolean("Parallel Flag");
   }
   
   //
   // Read the cifti file
   //
   CiftiFile cf;
 
   QTime readTimer;
   readTimer.start();
   cf.openFile(inputCiftiFileName);
   BrainSet leftSet(leftTopoName, leftCoordName);
   BrainSet rightSet(rightTopoName, rightCoordName);
   if(DebugControl::getDebugOn()) 
      std::cout << "Time to read file "
                << (readTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;


   QTime algTimer;
   algTimer.start();
   BrainModelCiftiGradient alg(NULL,
                               leftSet.getBrainModelSurface(0),
                               rightSet.getBrainModelSurface(0),
                               &cf,
                               surfaceKernel,
                               volumeKernel,
                               avgNormals,
                               indOutput,
                               parallelFlag);
   alg.execute();
   if (DebugControl::getDebugOn())
   {
      std::cout << "Time to run algorithm " << (algTimer.elapsed() * 0.001) << " seconds." << std::endl;
   }
   
   //
   // Write cifti file
   //
   
   QTime writeTimer;
   writeTimer.start();
   
   try
   {
      cf.writeFile(outputCiftiFileName);
   } catch (CiftiFileException e) {
      std::cout << e.what() << std::endl;
   }
   if(DebugControl::getDebugOn()) 
      std::cout << "Time to write file "
                << (writeTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   
}
