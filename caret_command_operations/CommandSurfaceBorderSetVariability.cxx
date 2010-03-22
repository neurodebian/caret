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

#include <algorithm>

#include "BorderFile.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderSetVariability.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderSetVariability::CommandSurfaceBorderSetVariability()
   : CommandBase("-surface-border-set-variability",
                 "SURFACE BORDER SET VARIABILITY")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderSetVariability::~CommandSurfaceBorderSetVariability()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderSetVariability::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.addFile("Input Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addFile("Output Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addFloat("New Variability", 1.0, 0.0, 100000.0);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderSetVariability::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-border-file-name>\n"
       + indent9 + "<output-border-file-name>\n"
       + indent9 + "<variability-value>\n"
       + indent9 + "\n"
       + indent9 + "Set the variability for all borders in a file..\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderSetVariability::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputBorderFileName =
      parameters->getNextParameterAsString("Input Border File Name");
   const QString outputBorderFileName =
      parameters->getNextParameterAsString("Output Border File Name");
   const float variability = 
      parameters->getNextParameterAsFloat("Variability");
   checkForExcessiveParameters();
   
   //
   // Read input file
   //
   BorderFile borderFile;
   borderFile.readFile(inputBorderFileName);
   
   //
   // Loop through the borders
   //
   const int num = borderFile.getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      //
      // Get the border projection
      //
      Border* bp = borderFile.getBorder(i);
      
      bp->setArealUncertainty(variability);
   }
   
   //
   // Write output border file
   //
   borderFile.writeFile(outputBorderFileName);
}

      

