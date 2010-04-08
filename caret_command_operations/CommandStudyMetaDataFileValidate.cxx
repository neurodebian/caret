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

#include "CommandStudyMetaDataFileValidate.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StudyMetaDataFile.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandStudyMetaDataFileValidate::CommandStudyMetaDataFileValidate()
   : CommandBase("-study-metadata-file-validate",
                 "STUDY METADATA FILE VALIDATION")
{
}

/**
 * destructor.
 */
CommandStudyMetaDataFileValidate::~CommandStudyMetaDataFileValidate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandStudyMetaDataFileValidate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{                     
   paramsOut.clear();

   paramsOut.addFile("Study Metadata File", FileFilters::getStudyMetaDataFileFilter());
   paramsOut.addVariableListOfParameters("Output Text File Name");
}

/**
 * get full help information.
 */
QString 
CommandStudyMetaDataFileValidate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-study-metatdata-file>\n"
       + indent9 + "[-output  output-text-file]\n"
       + indent9 + "\n"
       + indent9 + "Validate a study metadata file by searching for missing \n"
       + indent9 + "or duplicate PubMed ID's or Study Names, missing figure \n"
       + indent9 + "numbers, figure panel numbers, page reference numbers, \n"
       + indent9 + "page reference subheader numbers or short names, table\n"
       + indent9 + "numbers, and table subheader numbers or short names.\n"
       + indent9 + "\n"
       + indent9 + "If there are any errors, they are written to the output\n"
       + indent9 + "text file if it is provided.  Otherwise, results are \n"
       + indent9 + "written to the terminal.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandStudyMetaDataFileValidate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString studyFileName =
      parameters->getNextParameterAsString("Input Study Metadata File");
   QString textFileName;
   while (parameters->getParametersAvailable()) {
      QString paramName = parameters->getNextParameterAsString("Opt Param");
      if (paramName == "-output") {
         textFileName = parameters->getNextParameterAsString("Text File Name");
      }
      else {
         throw CommandException("Unexpected Parameter: " + paramName);
      }
   }
   
   StudyMetaDataFile smdf;
   smdf.readFile(studyFileName);
   QStringList sl = smdf.validStudyMetaDataFile();
   if (sl.count() <= 0) {
      std::cout << "No study problems were found." << std::endl;
   }
   else {
      const QString msg(sl.join("\n"));
      if (textFileName.isEmpty() == false) {
         TextFile tf;
         tf.setText(msg);
         tf.writeFile(textFileName);
      }
      else {
         std::cout << msg.toAscii().constData() << std::endl;
      }
   }
}

      

