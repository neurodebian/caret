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

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "CommandSystemFileCopy.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSystemFileCopy::CommandSystemFileCopy()
   : CommandBase("-system-file-copy",
                 "SYSTEM FILE COPY")
{
}

/**
 * destructor.
 */
CommandSystemFileCopy::~CommandSystemFileCopy()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSystemFileCopy::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Source File Name", FileFilters::getAnyFileFilter());
   paramsOut.addFile("Target File Name", FileFilters::getAnyFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSystemFileCopy::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<source-file-name>\n"
       + indent9 + "<target-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Copy a file.\n"
       + indent9 + "\n"
       + indent9 + "If the target file name is an existing file, the file will\n"
       + indent9 + "not be copied and the command will fail.  If the target file\n"
       + indent9 + "name is a directory, the source file name is copied into the\n"
       + indent9 + "directory (as long as a file with that name in the directory\n"
       + indent9 + "does not exist).\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSystemFileCopy::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString sourceFileName =
      parameters->getNextParameterAsString("Source File Name");
   QString targetFileName =
      parameters->getNextParameterAsString("Target File Name");
      
   //
   // Is target a directory?
   //
   QFileInfo targetFileInfo(targetFileName);
   if (targetFileInfo.isDir()) {
      targetFileName += (QDir::separator()
                         + sourceFileName);
   }
   
   if (QFile::exists(targetFileName)) {
      throw CommandException("Target file exists, will not overwrite it.");
   }
   
   if (QFile::copy(sourceFileName, targetFileName) == false) {
      throw CommandException("Copy failed.");
   }
}

      

