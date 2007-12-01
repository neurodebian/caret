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

#include "AbstractFile.h"
#include "CommandDataFileCompare.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "ProgramParameters.h"

/**
 * constructor.
 */
CommandDataFileCompare::CommandDataFileCompare()
   : CommandBase("-caret-data-file-compare",
                 "CARET DATA FILE COMPARISON")
{
}

/**
 * destructor.
 */
CommandDataFileCompare::~CommandDataFileCompare()
{
}

/**
 * get the script builder parameters.
 */
void
CommandDataFileCompare::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   
   paramsOut.addFile("Caret Data File 1",
                      FileFilters::getAnyFileFilter());
   paramsOut.addFile("Caret Data File 2",
                      FileFilters::getAnyFileFilter());
   paramsOut.addFloat("Tolerance",
                       0.01,
                       0.0);
}
/**
 * get full help information.
 */
QString 
CommandDataFileCompare::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<caret-data-file-1-name>  \n"
       + indent9 + "<caret-data-file-2-name> \n"
       + indent9 + "[tolerance]\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandDataFileCompare::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   // 
   const QString dataFileName1 = 
      parameters->getNextParameterAsString("Caret Data File 1 Name");
   const QString dataFileName2 = 
      parameters->getNextParameterAsString("Caret Data File 2 Name");
   float tolerance = 0.0;
   if (parameters->getParametersAvailable()) {
      tolerance = parameters->getNextParameterAsFloat("Tolerance");
   }
   
   QString errorMessage;
   AbstractFile* af1 = AbstractFile::readAnySubClassDataFile(dataFileName1, false, errorMessage);
   if (af1 == NULL) {
      throw CommandException(errorMessage);
   }

   errorMessage = "";
   AbstractFile* af2 = AbstractFile::readAnySubClassDataFile(dataFileName2, false, errorMessage);
   if (af2 == NULL) {
      delete af1;
      throw CommandException(errorMessage);
   }
   
   QString diffMessage;
   const bool theSame = af1->compareFileForUnitTesting(af2, tolerance, diffMessage);
   
   //std::cout << "---------------------------------------------------------------------" << std::endl;
   std::cout << "Comparison of " << FileUtilities::basename(dataFileName1).toAscii().constData() << " and " << std::endl
             << "              " << FileUtilities::basename(dataFileName2).toAscii().constData() << std::endl;
   if (theSame) {
      std::cout << "OK" << std::endl;
   }
   else {
      std::cout << "FAILED" << std::endl;
      std::cout << diffMessage.toAscii().constData() << std::endl;
      delete af1;
      delete af2;
      throw CommandException("Files did not match");
   }
   std::cout << std::endl;
   
   delete af1;
   delete af2;
}

