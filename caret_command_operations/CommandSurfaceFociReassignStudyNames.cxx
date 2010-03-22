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

#include "BrainSet.h"
#include "FociProjectionFile.h"
#include "CommandSurfaceFociReassignStudyNames.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StudyMetaDataFile.h"

/**
 * constructor.
 */
CommandSurfaceFociReassignStudyNames::CommandSurfaceFociReassignStudyNames()
   : CommandBase("-surface-foci-reassign-study-names",
                 "SURFACE FOCI REASSIGN STUDY NAMES")
{
}

/**
 * destructor.
 */
CommandSurfaceFociReassignStudyNames::~CommandSurfaceFociReassignStudyNames()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceFociReassignStudyNames::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.addFile("Study Metadata File", FileFilters::getStudyMetaDataFileFilter());
   paramsOut.addFile("Input Foci Projection File", FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Output Foci Projection File", FileFilters::getFociProjectionFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceFociReassignStudyNames::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<study-metadata-file-name>\n"
       + indent9 + "<input-foci-projection-file-name>\n"
       + indent9 + "<output-foci-projection-file-name>\n"
       + indent9 + "\n"
       + indent9 + "If a focus is linked to a study via a PubMed ID, set \n"
       + indent9 + "the name of the focus to the name of the study. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceFociReassignStudyNames::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{

   const QString studyMetaDataFileName =
      parameters->getNextParameterAsString("Study Metadata File Name");
   const QString inputFociProjectionFileName =
      parameters->getNextParameterAsString("Input Foci Projection File Name");
   const QString outputFociProjectionFileName =
      parameters->getNextParameterAsString("Output Foci Projection File Name");
   checkForExcessiveParameters();
   
   //
   // Read the study metadata file
   //
   StudyMetaDataFile studyMetaDataFile;
   studyMetaDataFile.readFile(studyMetaDataFileName);
   
   //
   // Read the foci projection file
   //
   FociProjectionFile fociProjectionFile;
   fociProjectionFile.readFile(inputFociProjectionFileName);
   
   //
   // Rename foci
   //
   fociProjectionFile.updateCellNameWithStudyNameForMatchingPubMedIDs(&studyMetaDataFile);
   
   //
   // Write the foci projection file name
   //
   fociProjectionFile.writeFile(outputFociProjectionFileName); 
}

      

