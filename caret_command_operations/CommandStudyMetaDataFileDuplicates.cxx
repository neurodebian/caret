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

#include "CommandStudyMetaDataFileDuplicates.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StudyMetaDataFile.h"

/**
 * constructor.
 */
CommandStudyMetaDataFileDuplicates::CommandStudyMetaDataFileDuplicates()
   : CommandBase("-study-metadata-duplicates",
                 "STUDY METADATA DUPLICATES")
{
}

/**
 * destructor.
 */
CommandStudyMetaDataFileDuplicates::~CommandStudyMetaDataFileDuplicates()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandStudyMetaDataFileDuplicates::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addVariableListOfParameters("Study Metadata File Name(s)");
}

/**
 * get full help information.
 */
QString 
CommandStudyMetaDataFileDuplicates::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<study-metadata-file-names>\n"
       + indent9 + "\n"
       + indent9 + "Find duplicate studies (studies with the same PubMedID) in\n"
       + indent9 + "the provided Study Metadata Files.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandStudyMetaDataFileDuplicates::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the inputs
   //
   std::vector<QString> fileNames;
   while (parameters->getParametersAvailable()) {
      fileNames.push_back(parameters->getNextParameterAsString("Study Metadata File Name"));
   }
   
   //
   // Find the duplicates
   //
   std::multimap<QString,QString> duplicatesStudies;
   StudyMetaDataFile::findDuplicateStudies(fileNames, duplicatesStudies);
   
   //
   // Process results
   //
   if (duplicatesStudies.empty()) {
      std::cout << "No duplicate studies found." << std::endl;
   }
   else {
      std::cout << "Duplicate PubMed IDs and File Names" << std::endl;
      for (std::multimap<QString,QString>::iterator iter = duplicatesStudies.begin();
           iter != duplicatesStudies.end();
           iter++) {
         std::cout << iter->first.toAscii().constData()
                   << "    "
                   << iter->second.toAscii().constData()
                   << std::endl;
      }
   }
}

      

