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

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderToMetricConverter.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderToMetric.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderToMetric::CommandSurfaceBorderToMetric()
   : CommandBase("-surface-border-to-metric",
                 "SURFACE BORDER CONVERT TO METRIC")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderToMetric::~CommandSurfaceBorderToMetric()
{
}

/**
 * get the script builder parameters.
 */
void
CommandSurfaceBorderToMetric::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinates File",
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Topology File",
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Metric File",
                     FileFilters::getMetricFileFilter());
}

/**
 * get full help information.
 */
QString
CommandSurfaceBorderToMetric::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coordinate-file>\n"
       + indent9 + "<input-topology-file>\n"
       + indent9 + "<input-border-projection-file>\n"
       + indent9 + "<output-metric-file>\n"
       + indent9 + "\n"
       + indent9 + "Convert the border projection file to a metric file\n"
       + indent9 + "by assigning the border's variance to the surface \n"
       + indent9 + "nodes based upon the triangle which the border point\n"
       + indent9 + "falls within.\n"
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandSurfaceBorderToMetric::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the inputs
   //
   const QString coordinateFileName =
      parameters->getNextParameterAsString("Coordinate File");
   const QString topologyFileName =
      parameters->getNextParameterAsString("Topology File");
   const QString borderProjectionFileName =
      parameters->getNextParameterAsString("Border Projection File");
   const QString metricFileName =
      parameters->getNextParameterAsString("Metric File");

   //
   // Create a spec file to read in the data files
   //
   SpecFile sf;
   sf.addToSpecFile(SpecFile::getUnknownCoordFileMatchTag(),
           coordinateFileName,
           "",
           false);
   sf.addToSpecFile(SpecFile::getUnknownTopoFileMatchTag(),
           topologyFileName,
           "",
           false);
   sf.addToSpecFile(SpecFile::getBorderProjectionFileTag(),
           borderProjectionFileName,
           "",
           false);

   //
   // Read the files
   //
   QString errorMessage;
   BrainSet brainSet;
   brainSet.readSpecFile(sf,
                         "",
                         errorMessage);
   if (errorMessage.isEmpty() == false) {
      throw new CommandException(errorMessage);
   }
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("Surface is invalid.");
   }

   //
   // Get the border projections
   //
   BorderProjectionFile borderProjectionFile;
   BrainModelBorderSet* bmbs = brainSet.getBorderSet();
   bmbs->copyBordersToBorderProjectionFile(borderProjectionFile);
   
   //
   // Convert the borders to  metric
   //
   MetricFile mf;
   BrainModelSurfaceBorderToMetricConverter
           converter(&brainSet,
                     surface,
                     &borderProjectionFile,
                     &mf,
                     0,
                     FileUtilities::basename(borderProjectionFileName));
   converter.execute();

   //
   // Write the metric file
   //
   mf.writeFile(metricFileName);
}




