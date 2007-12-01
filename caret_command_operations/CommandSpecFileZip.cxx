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

#include <QFile>

#include "CommandSpecFileZip.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFileUtilities.h"

/**
 * constructor.
 */
CommandSpecFileZip::CommandSpecFileZip()
   : CommandBase("-spec-file-zip",
                 "SPEC FILE ZIP")
{
}

/**
 * destructor.
 */
CommandSpecFileZip::~CommandSpecFileZip()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSpecFileZip::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Zip File Name", FileFilters::getZipFileFilter());
   paramsOut.addString("Unzip Directory Name");
   paramsOut.addFile("Input Spec File Name", FileFilters::getSpecFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSpecFileZip::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-zip-file-name>\n"
       + indent9 + "<unzip-directory-name>\n"
       + indent9 + "<input-spec-file-name>\n"
       + indent9 + "\n"
       + indent9 + "The spec file \"spec-file-name\" and its data files are placed \n"
       + indent9 + "into the zip \"zip-file-name\".  When unzipped by the user, the \n"
       + indent9 + "subdirectory \"unzip-dir-name\" will be created and contain the \n"
       + indent9 + "contents of the zip file.  \n"
       + indent9 + "  \n"
       + indent9 + "EXAMPLE  \n"
       + indent9 + "Zip the spec file \"Human.spec\" and place into the zip file \n"
       + indent9 + "\"Human.zip\".  When unzipped by the user, the subdirectory\n"
       + indent9 + "\"human_case_1\" will be created and contain the files from \n"
       + indent9 + "the zip file. \n"
       + indent9 + "  \n"
       + indent9 + "   " + getOperationSwitch() + " Human.zip human_case_1 Human.spec \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSpecFileZip::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString zipFileName =
      parameters->getNextParameterAsString("Output Zip File Name");
   const QString unzipDirectoryName =
      parameters->getNextParameterAsString("Unzip Directory Name");
   const QString specFileName =
      parameters->getNextParameterAsString("Input Spec File Name");

   //
   // Make sure the zip file does not exist
   //
   if (QFile::exists(zipFileName)) {
      throw CommandException("The zip file " 
                             + zipFileName
                             + " already exists.\n"
                             + "   Delete the zip file and rerun program.");
   }
   
   //
   // Zip the spec file
   //
   QString errorMessage;
   const bool result = SpecFileUtilities::zipSpecFile(specFileName, 
                                                     zipFileName,
                                                     unzipDirectoryName,
                                                     errorMessage);
   
   if (result) {
      throw CommandException(errorMessage);
   }
}

      

