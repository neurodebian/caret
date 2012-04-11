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

#include <vector>
#include <cmath>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceNormals.h"
#include "FileFilters.h"
#include "GiftiCommon.h"
#include "GiftiDataArray.h"
#include "GiftiDataArrayFile.h"
#include "ProgramParameters.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceNormals::CommandSurfaceNormals()
   : CommandBase("-surface-normals",
                 "SURFACE NORMALS")
{
}

/**
 * destructor.
 */
CommandSurfaceNormals::~CommandSurfaceNormals()
{
}

/**
 * get the script builder parameters.
 */
void
CommandSurfaceNormals::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name",
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name",
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output GIFTI Vector File Name",
                     FileFilters::getGiftiVectorFileFilter());
   paramsOut.addBoolean("Average Normals With Neighbors");
}

/**
 * get full help information.
 */
QString
CommandSurfaceNormals::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<fiducial-coordinate-file-name>\n"
       + indent9 + "<closed-topology-file-name>\n"
       + indent9 + "<output-gifti-vector-file-name>\n"
       + indent9 + "<average-normals>\n"
       + indent9 + "\n"
       + indent9 + "Write surface normals into a GIFTI Vector File.\n"
       + indent9 + "The GIFTI Vector File will contain a two-dimensional\n"
       + indent9 + "data array with dimensions (number-of-nodes, 3). If\n"
       + indent9 + "<average-normals> is \"true\", average the normals of\n"
       + indent9 + "each node with its neighbors before output.\n"
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandSurfaceNormals::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File Name");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File Name");
   const QString outputGiftiVectorFileName =
      parameters->getNextParameterAsString("Output GIFTI Vector File Name");
   const bool averageNormals =
      parameters->getNextParameterAsBoolean("AverageNormals");

   //
   // Create a brain set
   //
   BrainSet brainSet(topologyFileName,
                     coordinateFileName,
                     "",
                     true);
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("unable to find surface.");
   }
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains not nodes.");
   }

   //
   // Create the data array for the normals
   //
   std::vector<int> dim;
   dim.push_back(numNodes);
   dim.push_back(3);
   GiftiDataArrayFile vectorFile;
   GiftiDataArray* gda = new GiftiDataArray(&vectorFile,
                                            GiftiCommon::intentVectors,
                                            GiftiDataArray::DATA_TYPE_FLOAT32,
                                            dim);
   vectorFile.addDataArray(gda);
   gda->getMetaData()->set("Name", "Surface Normals");

   //
   // Add normals
   //
   if (averageNormals)
   {
	   bms->computeNormals(NULL, true);
   } else {
	   bms->computeNormals();
	}
   for (int i = 0; i < numNodes; i++) {
       const float* normal = bms->getNormal(i);

       for (int j = 0; j < 3; j++) {
           int indices[2] = { i, j };
           gda->setDataFloat32(indices, normal[j]);
       }
   }

   //
   // Write the vector file
   //
   vectorFile.writeFile(outputGiftiVectorFileName);
}



