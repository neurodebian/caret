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

#include "FociProjectionFile.h"
#include "CommandSurfaceFociDelete.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceFociDelete::CommandSurfaceFociDelete()
   : CommandBase("-surface-foci-delete",
                 "SURFACE FOCI DELETION")
{
}

/**
 * destructor.
 */
CommandSurfaceFociDelete::~CommandSurfaceFociDelete()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceFociDelete::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Foci Projection File", FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Output Foci Projection File", FileFilters::getFociProjectionFileFilter());
   paramsOut.addVariableListOfParameters("Foci Names");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceFociDelete::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-foci-projection-file-name> \n"
       + indent9 + "<output-foci-projection-file-name> \n"
       + indent9 + "[foci-names] \n"
       + indent9 + "\n"
       + indent9 + "Delete foci with the specified names from the foci\n"
       + indent9 + "projection file.  If a foci name contains spaces, it must\n"
       + indent9 + "be enclosed within double quotes.\n"
       + indent9 + "\n"
       + indent9 + "If the input foci projection file name does not exist\n"
       + indent9 + "the command will exit without reporting an error but\n"
       + indent9 + "it will report a warning.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceFociDelete::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputFociProjectionFileName = 
      parameters->getNextParameterAsString("Input Foci Projection File Name");
   const QString outputFociProjectionFileName = 
      parameters->getNextParameterAsString("Output Foci Projection File Name");
      
   if (QFile::exists(inputFociProjectionFileName) == false) {
      std::cout << "WARNING "
                << getShortDescription().toAscii().constData()
                << " \""
                << inputFociProjectionFileName.toAscii().constData()
                << "\" was not found."
                << std::endl;
      return;
   }
   
   FociProjectionFile fociProjectionFile;
   fociProjectionFile.readFile(inputFociProjectionFileName);
   
   while (parameters->getParametersAvailable()) {
      const QString focusName = parameters->getNextParameterAsString("Focus Name");
      fociProjectionFile.deleteCellProjectionsWithName(focusName);
   }
   
   fociProjectionFile.writeFile(outputFociProjectionFileName);
}

      

