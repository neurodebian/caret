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

#include "BrainModelCiftiDenseConnectomeGradient.h"
#include "BrainModelCiftiROIDenseConnectomeGradient.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandCiftiDenseConnectomeGradient.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "DebugControl.h"

/**
 * constructor.
 */
CommandCiftiDenseConnectomeGradient::CommandCiftiDenseConnectomeGradient()
   : CommandBase("-cifti-dense-connectome-gradient",
                 "CIFTI DENSE CONNECTOME GRADIENT")
{
}

/**
 * get the script builder parameters.
 */
void 
CommandCiftiDenseConnectomeGradient::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
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
   paramsOut.addBoolean("AverageNormals", false);
   //paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandCiftiDenseConnectomeGradient::getHelpInformation() const
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
       + indent9 + "[-average-normals <average-normals>]\n"
       + indent9 + "[-left-roi-override <left-roi>]\n"
       + indent9 + "[-right-roi-override <right-roi>]\n"
       + indent9 + "[-debug-output <debug>]\n"
       + indent9 + "\n"
       + indent9 + "Compute the correlation of the input cifti file rows within\n"
       + indent9 + "a structure, then take the gradient of the correlation and\n"
       + indent9 + "average the resulting gradients.\n"
       + indent9 + "\n"
       + indent9 + "If <average-normals> is \"true\", surface gradient will\n"
       + indent9 + "average surface normals with those of its neighbors to\n"
       + indent9 + "obtain a smoother set of normals.\n"
       + indent9 + "\n"
       + indent9 + "If <left-roi> or <right-roi> are present, correlation will\n"
       + indent9 + "only be done on the nodes within the ROI(s), and output will\n"
       + indent9 + "not have other nodes in the file.\n"
       + indent9 + "\n"
       + indent9 + "If <debug> is true, it will additionally output some metric\n"
       + indent9 + "files containing the raw correlation, smoothed correlation,\n"
       + indent9 + "and gradient.  Currently there is no such debug output for\n"
       + indent9 + "volumes.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandCiftiDenseConnectomeGradient::executeCommand() throw (BrainModelAlgorithmException,
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
   bool avgNormals = false, overrideMode = false, debug = false;
   MetricFile* leftROI = NULL, *rightROI = NULL;
   //
   // Process optional parameters
   //
   while (parameters->getParametersAvailable())
   {
      QString identifier = parameters->getNextParameterAsString("option identifier");
      if (identifier == QString("-average-normals"))
      {
         avgNormals = parameters->getNextParameterAsBoolean("Average Surface Normals");
      } else if (identifier == QString("-left-roi-override")) {
         overrideMode = true;
         leftROI = new MetricFile();
         leftROI->readFile(parameters->getNextParameterAsString("Left ROI Override"));
      } else if (identifier == QString("-right-roi-override")) {
         overrideMode = true;
         rightROI = new MetricFile();
         rightROI->readFile(parameters->getNextParameterAsString("Right ROI Override"));
      } else if (identifier == QString("-debug-output")) {
         debug = parameters->getNextParameterAsBoolean("Debug Output");
      } else {
         throw CommandException(QString("Unrecognized optional argument: ") + identifier);
      }
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
   if (overrideMode)
   {
      BrainModelCiftiROIDenseConnectomeGradient alg(NULL,
                               leftSet.getBrainModelSurface(0),
                               rightSet.getBrainModelSurface(0),
                               &cf,
                               leftROI,
                               rightROI,
                               surfaceKernel,
                               volumeKernel,
                               avgNormals,
                               debug);
      alg.execute();
   } else {
      BrainModelCiftiDenseConnectomeGradient alg(NULL,
                               leftSet.getBrainModelSurface(0),
                               rightSet.getBrainModelSurface(0),
                               &cf,
                               surfaceKernel,
                               volumeKernel,
                               avgNormals,
                               debug);
      alg.execute();
   }
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
