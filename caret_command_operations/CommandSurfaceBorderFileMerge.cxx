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
#include "CommandSurfaceBorderFileMerge.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderFileMerge::CommandSurfaceBorderFileMerge()
   : CommandBase("-surface-border-file-merge",
                 "SURFACE BORDER FILE MERGE")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderFileMerge::~CommandSurfaceBorderFileMerge()
{
}

/**
 * get the script builder parameters.
 */
void
CommandSurfaceBorderFileMerge::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();

   paramsOut.addFile("Output Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Input Border Projection File 1",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Input Border Projection File 2",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addVariableListOfParameters("Additional border projection files");
}

/**
 * get full help information.
 */
QString
CommandSurfaceBorderFileMerge::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-border-projection-file-name>\n"
       + indent9 + "<input-border-projection-file-1-name>\n"
       + indent9 + "<input-border-projection-file-2-name>\n"
       + indent9 + "[additional-input-border-projections-file-names]\n"
       + indent9 + "\n"
       + indent9 + "Merge two or more border projections files into a\n"
       + indent9 + "single border projection file.\n"
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandSurfaceBorderFileMerge::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get required parameters
   //
   const QString outputBorderProjectionFileName =
      parameters->getNextParameterAsString("Output Border Projection File");
   std::vector<QString> inputBorderProjectionFileNames;
   inputBorderProjectionFileNames.push_back(
      parameters->getNextParameterAsString("Input Border Projection File 1 Name"));
   inputBorderProjectionFileNames.push_back(
      parameters->getNextParameterAsString("Input Border Projection File 2 Name"));

   //
   // Process optional parameters
   //
   while (parameters->getParametersAvailable()) {
       inputBorderProjectionFileNames.push_back(
            parameters->getNextParameterAsString("Additional Border Projection File Names"));
   }

   //
   // Border projection file
   //
   BorderProjectionFile borderProjectionFile;

   //
   // loop through input border projections and append
   //
   std::vector<int> borderProjectionsForDeletion;
   const int numInputFiles = static_cast<int>(inputBorderProjectionFileNames.size());
   for (int i = 0; i < numInputFiles; i++) {
       BorderProjectionFile bpf;
       bpf.readFile(inputBorderProjectionFileNames[i]);
       borderProjectionFile.append(bpf);
   }

   //
   // Write the border projection file
   //
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
}
