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

#include "CommandSurfaceBankStraddling.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelSurfaceBankStraddling.h"
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
CommandSurfaceBankStraddling::CommandSurfaceBankStraddling()
   : CommandBase("-surface-bank-straddling",
                 "VOLUME TO SURFACE MAPPING DANGER POINTS")
{
}

/**
 * destructor.
 */
CommandSurfaceBankStraddling::~CommandSurfaceBankStraddling()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBankStraddling::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Metric File", FileFilters::getMetricFileFilter());
   paramsOut.addInt("Output Metric Column Number", 0, 0);
   paramsOut.addFloat("Volume Voxel Spacing (x)", 1.0f, 0.0f);
   paramsOut.addFloat("Volume Voxel Spacing (y)", 1.0f, 0.0f);
   paramsOut.addFloat("Volume Voxel Spacing (z)", 1.0f, 0.0f);
   paramsOut.addBoolean("Interpolation");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBankStraddling::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<surface-coord>\n"
       + indent9 + "<surface-topo>\n"
       + indent9 + "<output-metric>\n"
       + indent9 + "<out-metric-col>\n"
       + indent9 + "<x-spacing>\n"
       + indent9 + "<y-spacing>\n"
       + indent9 + "<z-spacing>\n"
       + indent9 + "<interpolated>\n"
       + indent9 + "\n"
       + indent9 + "Find areas of a surface prone to high correlation when mapping from\n"
       + indent9 + "volumes of a given voxel size.  Output is pessimistic, with value\n"
       + indent9 + "equal to maximum possible artificial correlation times geodesic\n"
       + indent9 + "distance.  This is computed for all nodes that can share information\n"
       + indent9 + "with the considered voxel, and the maximum of these values is used.\n"
       + indent9 + "Interpolated specifies whether mapping uses interpolation.  Trilinear\n"
       + indent9 + "interpolation is assumed.  If output metric exists, output is put into\n"
       + indent9 + "the specified column if possible, otherwise appended if possible, and\n"
       + indent9 + "if node numbers do not match, the existing metric file is overwritten.\n"
       + indent9 + "\n"
       + indent9 + "      surface-coord      the surface coord file\n"
       + indent9 + "\n"
       + indent9 + "      surface-topo       the surface topo file\n"
       + indent9 + "\n"
       + indent9 + "      output-metric      output metric file for min/max identification\n"
       + indent9 + "\n"
       + indent9 + "      out-metric-col     which column to put the output into\n"
       + indent9 + "\n"
       + indent9 + "      *-spacing          the dimensions of the voxels for the mapping volume\n"
       + indent9 + "\n"
       + indent9 + "      interpolated       TRUE or FALSE, specifies whether to assume interpolated\n"
       + indent9 + "                      mapping.  FALSE implies enclosing (nearest) voxel mapping.\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBankStraddling::executeCommand() throw (BrainModelAlgorithmException,
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
      parameters->getNextParameterAsString("Output Metric File");
   int metricCol =
      parameters->getNextParameterAsInt("Output Metric Column Number");
   float voxdim[3];
   voxdim[0] =
      parameters->getNextParameterAsFloat("Voxel X Dimension");
   voxdim[1] =
      parameters->getNextParameterAsFloat("Voxel Y Dimension");
   voxdim[2] =
      parameters->getNextParameterAsFloat("Voxel Z Dimension");
   bool interp =
      parameters->getNextParameterAsBoolean("Interpolated Mapping");
   BrainSet mybs(topo, coord);
   MetricFile mymetric;
   try
   {
      mymetric.readFile(metric);
   } catch (FileException e) {}
   BrainModelSurfaceBankStraddling myobject(&mybs, 0, &mymetric, metricCol - 1, voxdim, interp);
   myobject.execute();
   mymetric.writeFile(metric);
}
