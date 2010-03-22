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

#include "BrainSet.h"
#include "CommandPaintDilation.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPaintDilation::CommandPaintDilation()
   : CommandBase("-paint-dilation",
                 "PAINT DILATION")
{
}

/**
 * destructor.
 */
CommandPaintDilation::~CommandPaintDilation()
{
}

/**
 * get the script builder parameters.
 */
void
CommandPaintDilation::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("AN");      descriptions.push_back("Average Neighbors");
   values.push_back("DILATE");  descriptions.push_back("Dilation");
   values.push_back("FWHM");   descriptions.push_back("Full Width Half Maximum");
   values.push_back("GAUSS");   descriptions.push_back("Gaussian");
   values.push_back("WAN");     descriptions.push_back("Weighted Average Neighbors");

   paramsOut.clear();
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addFile("Output Paint File Name", FileFilters::getPaintFileFilter());
   paramsOut.addInt("Number of Dilation Iterations", 5, 0, 100000);
}

/**
 * get full help information.
 */
QString
CommandPaintDilation::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<input-paint-file-name>\n"
       + indent9 + "<output-paint-file-name>\n"
       + indent9 + "<number-of-dilation-iterations>\n"
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandPaintDilation::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString inputPaintFileName =
      parameters->getNextParameterAsString("Input Paint File Name");
   const QString outputPaintFileName =
      parameters->getNextParameterAsString("Output Paint File Name");
   const int numberOfDilationIterations =
      parameters->getNextParameterAsInt("Number of Dilation Iterations");
   checkForExcessiveParameters();


   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     NULL,
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
   // Read input paint file
   //
   PaintFile paintFile;
   paintFile.readFile(inputPaintFileName);

   //
   // Perform dilation
   //
   const int numberOfColumns = paintFile.getNumberOfColumns();
   for (int i = 0; i < numberOfColumns; i++) {
      paintFile.dilateColumn(tf, i, numberOfDilationIterations);
   }

   //
   // Write the p file
   //
   paintFile.writeFile(outputPaintFileName);
}




