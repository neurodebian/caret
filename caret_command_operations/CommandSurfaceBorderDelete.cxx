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

#include <QFile>

#include "BorderProjectionFile.h"
#include "CommandSurfaceBorderDelete.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderDelete::CommandSurfaceBorderDelete()
   : CommandBase("-surface-border-delete",
                 "SURFACE BORDER DELETION")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderDelete::~CommandSurfaceBorderDelete()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderDelete::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Border Projection File", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File", FileFilters::getBorderProjectionFileFilter());
   paramsOut.addVariableListOfParameters("Border Names");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderDelete::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-border-projection-file-name> \n"
       + indent9 + "<output-border-projection-file-name> \n"
       + indent9 + "[border-names] \n"
       + indent9 + "\n"
       + indent9 + "Delete borders with the specified names from the border\n"
       + indent9 + "projection file.  If a border name contains spaces, it must\n"
       + indent9 + "be enclosed within double quotes.\n"
       + indent9 + "\n"
       + indent9 + "If the input border projection file name does not exist\n"
       + indent9 + "the command will exit without reporting an error but\n"
       + indent9 + "it will report a warning.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderDelete::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Input Border Projection File Name");
      
   if (QFile::exists(inputBorderProjectionFileName) == false) {
      std::cout << "WARNING "
                << getShortDescription().toAscii().constData()
                << " \""
                << inputBorderProjectionFileName.toAscii().constData()
                << "\" was not found."
                << std::endl;
      return;
   }
   
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(inputBorderProjectionFileName);
   
   while (parameters->getParametersAvailable()) {
      const QString borderName = parameters->getNextParameterAsString("Border Name");
      borderProjectionFile.removeBordersWithName(borderName);
   }
   
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
}

      

