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

#include "CommandSpecFileClean.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSpecFileClean::CommandSpecFileClean()
   : CommandBase("-spec-file-clean",
                 "SPEC FILE CLEAN")
{
}

/**
 * destructor.
 */
CommandSpecFileClean::~CommandSpecFileClean()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSpecFileClean::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Spec File Name", FileFilters::getSpecFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSpecFileClean::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Remove spec file entries for files that do not exist.\n"
       + indent9 + "\n"
       + indent9 + "If a file listed in a spec file does not exists, the \n"
       + indent9 + "entry for the file is removed from the spec file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSpecFileClean::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString specFileName =
      parameters->getNextParameterAsString("Spec File Name");
   checkForExcessiveParameters();  
   
   //
   // Read the spec file
   //
   SpecFile specFile;
   specFile.readFile(specFileName);
   
   //
   // Clean the spec file
   //
   bool filesCleanedFlag = specFile.cleanSpecFile();
   
   //
   // Write the spec file
   //
   if (filesCleanedFlag) {
      specFile.writeFile(specFileName);
   }
}

      

