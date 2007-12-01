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

#include "CommandSpecFileCopy.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFileUtilities.h"

/**
 * constructor.
 */
CommandSpecFileCopy::CommandSpecFileCopy()
   : CommandBase("-spec-file-copy",
                 "SPEC FILE COPY")
{
}

/**
 * destructor.
 */
CommandSpecFileCopy::~CommandSpecFileCopy()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSpecFileCopy::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   std::vector<QString> values, descriptions;
   values.push_back("COPY_ALL");   descriptions.push_back("Copy Spec and its Data Files");
   values.push_back("MOVE_ALL");   descriptions.push_back("Move Spec and its Data Files");
   values.push_back("COPY_SPEC_ONLY");   descriptions.push_back("Copy Spec File and Point to Data Files");

   paramsOut.addListOfItems("Copy Mode", values, descriptions);
   paramsOut.addFile("Input Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Target  Name", FileFilters::getSpecFileFilter());

}

/**
 * get full help information.
 */
QString 
CommandSpecFileCopy::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<copy-mode>\n"
       + indent9 + "<source-spec-file-name>\n"
       + indent9 + "<target-name>\n"
       + indent9 + "\n"
       + indent9 + "The source-spec-file and perhaps its data files are \n"
       + indent9 + "copied to the new location/spec file. \n"
       + indent9 + "  \n"
       + indent9 + "if \"target-name\" is a directory, the name of the  \n"
       + indent9 + "target spec file will be the same as the source spec \n"
       + indent9 + "file. \n"
       + indent9 + "  \n"
       + indent9 + "\"copy-mode\" is one of \n"
       + indent9 + "  \n"
       + indent9 + "   COPY_ALL   Copy the data files to the destination directory. \n"
       + indent9 + "   COPY_SPEC_ONLY   Copy only the spec file, the data files \n"
       + indent9 + "                    will not be copied.  Any absolute or  \n"
       + indent9 + "                    relative path on the specified spec file  \n"
       + indent9 + "                    is prepended to the data filenames in \n"
       + indent9 + "                    the new spec file. \n"
       + indent9 + "   MOVE ALL   Move all files from their old location to their \n"
       + indent9 + "              new location. \n"
       + indent9 + "  \n"
       + indent9 + "LIMITATIONS  \n"
       + indent9 + "   Either the source or target spec file must be in the  \n"
       + indent9 + "   current directory. \n"
       + indent9 + "  \n"
       + indent9 + "   If you are using the \"-s\" option to copy only the spec \n"
       + indent9 + "   file and want a relative path to the data files in the  \n"
       + indent9 + "   target spec file, you must run the command from the  \n"
       + indent9 + "   target directory as shown in the first example and use \n"
       + indent9 + "   a relative path to specify the spec file that is being \n"
       + indent9 + "   copied. \n"
       + indent9 + "  \n"
       + indent9 + "EXAMPLES  \n"
       + indent9 + "  \n"
       + indent9 + "   Copy the spec file brain.spec to the current directory. \n"
       + indent9 + "   The data files will not be copied but the path to each of the\n"
       + indent9 + "   data files in the spec file will be a RELATIVE path \n"
       + indent9 + "   since a relative path is used to specify the spec file. \n"
       + indent9 + "   Note that a \".\" is used to specify the current directory \n"
       + indent9 + "   for the target spec file. \n"
       + indent9 + "  \n"
       + indent9 + "      COPY_SPEC_ONLY ../../brain/brain.spec . \n"
       + indent9 + "  \n"
       + indent9 + "   Copy the spec file brain.spec to the current directory. \n"
       + indent9 + "   The data files will not be copied but the path to each of the\n"
       + indent9 + "   data files in the spec file will be an ABSOLUTE path \n"
       + indent9 + "   since an absolute path is used to specify the spec file. \n"
       + indent9 + "  \n"
       + indent9 + "      COPY_SPEC_ONLY /usr/data/brain/brain.spec . \n"
       + indent9 + "  \n"
       + indent9 + "   Copy the spec file occipital.spec located in the current \n"
       + indent9 + "   directory to the directory /usr/people/bob/brain_data. \n"
       + indent9 + "   The data files will also be copied. \n"
       + indent9 + "  \n"
       + indent9 + "      COPY_ALL  occipital.spec /usr/people/bob/brain_data \n"
       + indent9 + "  \n"
       + indent9 + "   Copy the spec file occipital.spec located in the current \n"
       + indent9 + "   directory to the directory /usr/people/bob/brain_data. \n"
       + indent9 + "   The data files will not be copied and the path to each of  \n"
       + indent9 + "   the data files in the spec file will be an absolute path \n"
       + indent9 + "   since the command is run from the directory containing \n"
       + indent9 + "   the spec file. \n"
       + indent9 + " \n"
       + indent9 + "      COPY_SPEC_ONLY occipital.spec /usr/people/bob/brain_data \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSpecFileCopy::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString copyModeName =
      parameters->getNextParameterAsString("Copy Mode");
   const QString sourceSpecFileName =
      parameters->getNextParameterAsString("Source Spec File Name");
   const QString targetName =
      parameters->getNextParameterAsString("Target Name");
   checkForExcessiveParameters();
   
   SpecFileUtilities::MODE_COPY_SPEC_FILE copyMode;
   if (copyModeName == "COPY_ALL") {
      copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_COPY_ALL;
   }
   else if (copyModeName == "COPY_SPEC_ONLY") {
      copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES;
   }
   else if (copyModeName == "MOVE_ALL") {
      copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_MOVE_ALL;
   }
   else {
      throw CommandException("Invalid copy mode \""
                             + copyModeName
                             + "\"");
   }
   
   QString errorMessage;
   const bool errorFlag = SpecFileUtilities::copySpecFile(sourceSpecFileName, 
                                                      targetName, 
                                                      copyMode, 
                                                      errorMessage,
                                                      true);
   if (errorFlag) {
      throw CommandException(errorMessage);
   }
}

      

