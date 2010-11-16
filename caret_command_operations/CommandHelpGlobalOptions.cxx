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

#include <cstdlib>
#include <iostream>

#include <QDir>

#include "BrainSet.h"
#include "CommandHelpGlobalOptions.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandHelpGlobalOptions::CommandHelpGlobalOptions()
   : CommandBase("-help-global-options",
                 "HELP GLOBAL OPTIONS")
{
}

/**
 * destructor.
 */
CommandHelpGlobalOptions::~CommandHelpGlobalOptions()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandHelpGlobalOptions::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandHelpGlobalOptions::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "\n"
       + indent9 + "These options may be specified anywhere AFTER the command's\n"
       + indent9 + "operation.  In other words, these options cannot immediately\n"
       + indent9 + "follow the program's name.\n"
       + indent9 + "\n"
       + indent9 + "Adding the parameter \"-CHDIR <directory-name>\" to \n"
       + indent9 + "the command will result in the command running from \n"
       + indent9 + "that directory.  This parameter does not affect the \n"
       + indent9 + "current directory in the shell that executes the \n"
       + indent9 + "command.\n"
       + indent9 + "\n"
       + indent9 + "Adding the parameter \"-CHMOD <permissions>\" to the \n"
       + indent9 + "command will result in all files written by the command\n"
       + indent9 + "receiving the specified permissions.  \"permissions\"\n"
       + indent9 + "is a list of one or more of the following values separated \n"
       + indent9 + "by commas:\n"
       + indent9 + "   UR   - user read\n"
       + indent9 + "   UW   - user write\n"
       + indent9 + "   UX   - user execute\n"
       + indent9 + "   GR   - group read\n"
       + indent9 + "   GW   - group write\n"
       + indent9 + "   GX   - group execute\n"
       + indent9 + "   AR   - all read\n"
       + indent9 + "   AW   - all write\n"
       + indent9 + "   AX   - all execute\n"
       + indent9 + "\n"
       + indent9 + "Adding the parameter \"-RANDOMSEED <seed-value>\" to \n"
       + indent9 + "the command will result in the random number generator's \n"
       + indent9 + "seed value being set to the provided value. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandHelpGlobalOptions::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   std::cout << getHelpInformation().toAscii().constData() << std::endl;
}

/**
 * process global options.
 */
void 
CommandHelpGlobalOptions::processGlobalOptions(ProgramParameters& params) throw (CommandException)
{
   processChangeDirectoryCommand(params);
   processSetPermissionsCommand(params);
   processSetRandomSeedCommand(params);
}

/**
 * process the change directory command.
 */
void 
CommandHelpGlobalOptions::processChangeDirectoryCommand(ProgramParameters& params) throw (CommandException)
{
   //
   // See if directory should be changed
   //
   const int chdirIndex = params.getIndexOfParameterWithValue("-CHDIR");
   if (chdirIndex >= 0) {
      const int dirNameIndex = chdirIndex + 1;
      if (dirNameIndex < params.getNumberOfParameters()) {
         const QString directoryName = params.getParameterAtIndex(dirNameIndex);
         if (directoryName.isEmpty() == false) {
            //
            // Is specified directory valid?
            //
            QDir dir(directoryName);
            if (dir.exists() == false) {
               throw CommandException("ERROR: Directory for -CHDIR \""
                                      + directoryName
                                      + "\" is invalid.");
            }

            //
            // Change to the specified directory
            //
            QDir::setCurrent(directoryName);
         }
            
         //
         // Remove the "-CHDIR" and directory name parameters
         // Remember, remove largest index first since array shrinks
         //
         params.removeParameterAtIndex(dirNameIndex);
         params.removeParameterAtIndex(chdirIndex);
      }
      else {
         throw CommandException("ERROR: Directory name missing for \"-CHDIR\" option.");
      }
   }
}

/**
 * process the change permissions command.
 */
void 
CommandHelpGlobalOptions::processSetPermissionsCommand(ProgramParameters& params) throw (CommandException)
{
   //
   // See if output file permissions should be set
   //
   const int chmodIndex = params.getIndexOfParameterWithValue("-CHMOD");
   if (chmodIndex >= 0) {
      const int modeIndex = chmodIndex + 1;
      if (modeIndex < params.getNumberOfParameters()) {
         const QString modeName = params.getParameterAtIndex(modeIndex);
         if (modeName.isEmpty() == false) {
            QFile::Permissions permissions;
            QStringList modes = modeName.split(',');
            for (int m = 0; m < modes.size(); m++) {
               const QString md = modes.at(m);
               if (md == "UR") {
                  permissions |= QFile::ReadOwner;
                  permissions |= QFile::ReadUser;
               }
               else if (md == "UW") {
                  permissions |= QFile::WriteOwner;
                  permissions |= QFile::WriteUser;
               }
               else if (md == "UX") {
                  permissions |= QFile::ExeOwner;
                  permissions |= QFile::ExeUser;
               }
               else if (md == "GR") {
                  permissions |= QFile::ReadGroup;
               }
               else if (md == "GW") {
                  permissions |= QFile::WriteGroup;
               }
               else if (md == "GX") {
                  permissions |= QFile::ExeGroup;
               }
               else if (md == "AR") {
                  permissions |= QFile::ReadOther;
               }
               else if (md == "AW") {
                  permissions |= QFile::WriteOther;
               }
               else if (md == "AX") {
                  permissions |= QFile::ExeOther;
               }
               else {
                  throw CommandException("ERROR: Unrecognzied permission \""
                                         + md
                                         + "\" for -CHMOD option.");
               }
            }
            
            AbstractFile::setFileWritePermissions(permissions);
         }
         //
         // Remove the "-CHMOD" and mode name parameters
         // Remember, remove largest index first since array shrinks
         //
         params.removeParameterAtIndex(modeIndex);
         params.removeParameterAtIndex(chmodIndex);
      }
      else {
         throw CommandException("ERROR: Mode missing for \"-CHMOD\" option.");
      }
   }
}
      
/**
 * process the set random seed.
 */
void 
CommandHelpGlobalOptions::processSetRandomSeedCommand(ProgramParameters& params) throw (CommandException)
{
   //
   // See if seed should be set
   //
   const int randomSeedIndex = params.getIndexOfParameterWithValue("-RANDOMSEED");
   if (randomSeedIndex >= 0) {
      const int seedIndex = randomSeedIndex + 1;
      if (seedIndex < params.getNumberOfParameters()) {
         const QString seedValueString = params.getParameterAtIndex(seedIndex);
         if (seedValueString.isEmpty() == false) {
            bool ok = false;
            const unsigned int seedValue = seedValueString.toUInt(&ok);
            if (ok == false) {
               throw CommandException("Invalid random seed value ("
                                      + seedValueString
                                      + ")");
            }
            
            BrainSet::setRandomSeed(seedValue);
         }
         //
         // Remove the "-RANDOMSEED" and mode name parameters
         // Remember, remove largest index first since array shrinks
         //
         params.removeParameterAtIndex(seedIndex);
         params.removeParameterAtIndex(randomSeedIndex);
      }
      else {
         throw CommandException("ERROR: Value missing for \"-RANDOMSEED\" option.");
      }
   }
}


