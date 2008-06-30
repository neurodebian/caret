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

#include "CommandFileSubstitution.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandFileSubstitution::CommandFileSubstitution()
   : CommandBase("-file-substitution",
                 "FILE SUBSTITUTION")
{
}

/**
 * destructor.
 */
CommandFileSubstitution::~CommandFileSubstitution()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandFileSubstitution::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input File Name", FileFilters::getAnyFileFilter());
   paramsOut.addFile("Output File Name", FileFilters::getAnyFileFilter());
   paramsOut.addVariableListOfParameters("Substitutions");
}

/**
 * get full help information.
 */
QString 
CommandFileSubstitution::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-file-name>\n"
       + indent9 + "<output-file-name>\n"
       + indent9 + "-s find-text replace-with-text\n"
       + indent9 + "\n"
       + indent9 + "Substitute text in a file.  Every occurrance of \n"
       + indent9 + "\"find-text\" is replaced with \"replace-with-text\".\n"
       + indent9 + "\n"
       + indent9 + "If either of the text expressions contain spaces, the \n"
       + indent9 + "text must be enclosed in double quotes.\n"
       + indent9 + "\n"
       + indent9 + "Multiple \"-s\" substitution parameters are allowed\n"
       + indent9 + "\n"
       + indent9 + "The input and output file names may be the same.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandFileSubstitution::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get file names
   //
   const QString inputFileName =
      parameters->getNextParameterAsString("Input File Name");
   const QString outputFileName =
      parameters->getNextParameterAsString("Output File Name");

   //
   // Open the text file
   //
   TextFile tf;
   tf.readFile(inputFileName);      
   QString text = tf.getText();
   
   //
   // Loop through parameters
   //
   while (parameters->getParametersAvailable()) {
      const QString param = 
         parameters->getNextParameterAsString("Option");
      if (param == "-s") {
         const QString findText =
            parameters->getNextParameterAsString("Find Text");
         const QString replaceWithText =
            parameters->getNextParameterAsString("Replace With Text");
         
         text = text.replace(findText, replaceWithText);
      }
      else {
         throw CommandException("Unrecognized parameter "
                                + param);
      }
   }
   
   //
   // Add text to file and write the file
   //
   tf.setText(text);
   tf.writeFile(outputFileName);
}

      

