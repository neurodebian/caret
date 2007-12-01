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

#include "BorderFile.h"
#include "CommandSurfaceBorderVariability.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderVariability::CommandSurfaceBorderVariability()
   : CommandBase("-surface-border-variability",
                 "SURFACE BORDER VARIABILITY")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderVariability::~CommandSurfaceBorderVariability()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderVariability::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addFile("Input Landmark Average Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addFile("Output Border File",
                     FileFilters::getBorderGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderVariability::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-border-file>\n"
       + indent9 + "<input-landmark-average-border-file>\n"
       + indent9 + "<output-border-file>\n"
       + indent9 + "\n"
       + indent9 + "Resample the input borders so that they have the same number\n"
       + indent9 + "of links as the border with the corresponding name in the \n"
       + indent9 + "input landmark average border file.  In addition, the \n"
       + indent9 + "variability radius for the borders will also be set.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderVariability::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputBorderFileName =
      parameters->getNextParameterAsString("Input Border File Name");
   const QString inputLandmarkAverageBorderFileName =
      parameters->getNextParameterAsString("Input Landmark Average Border File Name");
   const QString outputBorderFileName =
      parameters->getNextParameterAsString("Output Border File Name");
   checkForExcessiveParameters();
      
   BorderFile inputBorderFile;
   inputBorderFile.readFile(inputBorderFileName);

   BorderFile inputLandmarkAverageBorderFile;
   inputLandmarkAverageBorderFile.readFile(inputLandmarkAverageBorderFileName);
   
   inputBorderFile.resampleToMatchLandmarkBorders(inputLandmarkAverageBorderFile);
   
   inputBorderFile.writeFile(outputBorderFileName);
}

      

