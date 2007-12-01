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

#include "CommandPaintComposite.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"

/**
 * constructor.
 */
CommandPaintComposite::CommandPaintComposite()
   : CommandBase("-paint-composite",
                 "PAINT FILE COMPOSITE ALL COLUMNS")
{
}

/**
 * destructor.
 */
CommandPaintComposite::~CommandPaintComposite()
{
}


/**
 * get the script builder parameters.
 */
void
CommandPaintComposite::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.clear();
   paramsOut.addFile("Output Paint File Name", 
                     FileFilters::getPaintFileFilter());
   paramsOut.addMultipleFiles("Input Paint File Name(s)", 
                               FileFilters::getMetricFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandPaintComposite::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-paint-file>  \n"
       + indent9 + "<input-paint-files...>\n"
       + indent9 + "\n"
       + indent9 + "Concatenate all columns from the input paint files and place\n"
       + indent9 + "them into the output paint file\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandPaintComposite::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString outputPaintFileName = 
      parameters->getNextParameterAsString("Output Paint File Name");

   std::vector<QString> inputPaintFileNames;   
   parameters->getRemainingParametersAsStrings("Input Paint Files",
                                               inputPaintFileNames);
   const int numPaintFiles = static_cast<int>(inputPaintFileNames.size());
   if (numPaintFiles <= 0) {
      throw CommandException("Names of input paint files are missing.");
   }

   PaintFile outputPaintFile;
   
   for (int i = 0; i < numPaintFiles; i++) {
      if (outputPaintFile.getNumberOfColumns() == 0) {
         outputPaintFile.readFile(inputPaintFileNames[i]);
      }
      else {
         PaintFile inputPaintFile;
         inputPaintFile.readFile(inputPaintFileNames[i]);
         const int numCols = inputPaintFile.getNumberOfColumns();
         if (numCols > 0) {
            outputPaintFile.append(inputPaintFile);
         }
      }
   }
   outputPaintFile.writeFile(outputPaintFileName);
}
