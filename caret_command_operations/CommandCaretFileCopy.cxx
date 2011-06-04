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
#include "CommandCaretFileCopy.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandCaretFileCopy::CommandCaretFileCopy()
   : CommandBase("-caret-file-copy",
                 "CARET FILE COPY")
{
}

/**
 * destructor.
 */
CommandCaretFileCopy::~CommandCaretFileCopy()
{
}

/**
 * get the script builder parameters.
 */
void
CommandCaretFileCopy::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Caret Data File Name", FileFilters::getAnyFileFilter());
   paramsOut.addFile("Output Caret Data File Name", FileFilters::getAnyFileFilter());
   paramsOut.addVariableListOfParameters("Optional Parameters");
}

/**
 * get full help information.
 */
QString
CommandCaretFileCopy::getHelpInformation() const
{
   std::vector<AbstractFile::FILE_FORMAT> fileFormats;
   std::vector<QString> fileFormatNames;
   AbstractFile::getFileFormatTypesAndNames(fileFormats, fileFormatNames);
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + " <input-caret-data-file-name>\n"
       + indent9 + " <output-caret-data-file-name>\n"
       + indent9 + " [-output-encoding  output-file-encoding]\n"
       + indent9 + "\n"
       + indent9 + "Copy a Caret Data File with optional output encoding.\n"
       + indent9 + "\n"
       + indent9 + "      <file-format> is any combination of the following\n"
       + indent9 + "      separated by a colon:\n");
   for (unsigned int i = 0; i < fileFormatNames.size(); i++) {
   helpInfo +=
         (indent9 + "         " + fileFormatNames[i] + "\n");
   }
   helpInfo += (""
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandCaretFileCopy::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString& inputFileName = parameters->getNextParameterAsString("Input Caret File Name");
   const QString& outputFileName = parameters->getNextParameterAsString("Output Caret File Name");
   if (inputFileName.isEmpty()) {
      throw CommandException("Output File Name is empty.");
   }
   if (outputFileName.isEmpty()) {
      throw CommandException("Output File Name is empty.");
   }

   QString outputEncodingName;
   while  (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Optional Parameter");
      if (paramName == "-output-encoding") {
         outputEncodingName = parameters->getNextParameterAsString("Output File Encoding");
      }
      else {
         throw CommandException("Unrecogized parameter: " + paramName);
      }
   }

   QString errorMessage;
   AbstractFile* dataFile = AbstractFile::readAnySubClassDataFile(inputFileName, false, errorMessage);
   if (dataFile == NULL) {
      throw CommandException(errorMessage);
   }

   AbstractFile::FILE_FORMAT fileFormat = dataFile->getFileWriteType();
   if (outputEncodingName.isEmpty() == false) {
      bool validEncodingFlag = true;
      fileFormat = AbstractFile::convertFormatNameToType(outputEncodingName, &validEncodingFlag);
      if (validEncodingFlag == false) {
         throw CommandException("Invalid encoding name: " + outputEncodingName);
      }
   }

   dataFile->setFileWriteType(fileFormat);

   dataFile->writeFile(outputFileName);
}



