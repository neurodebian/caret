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
#include "CommandPreferencesFileSettings.h"
#include "FileFilters.h"
#include "PreferencesFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandPreferencesFileSettings::CommandPreferencesFileSettings()
   : CommandBase("-preferences-file-settings",
                 "PREFERENCES FILES SETTINGS")
{
}

/**
 * destructor.
 */
CommandPreferencesFileSettings::~CommandPreferencesFileSettings()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandPreferencesFileSettings::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandPreferencesFileSettings::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "[-background-color  red  green  blue]\n"
       + indent9 + "[-foreground-color  red  green  blue]\n"
       + indent9 + "[-debug   debug-value]\n"
       + indent9 + "\n"
       + indent9 + "Set items in the preferences file.  The preferences file\n"
       + indent9 + "is located in the user's home directory and is named\n"
       + indent9 + "   " + BrainSet::getPreferencesFileName() + "\n"
       + indent9 + "\n"
       + indent9 + "\"debug-value\" must be one of \"true\" or \"false\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandPreferencesFileSettings::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   PreferencesFile* pf = BrainSet::getPreferencesFile();
   
   while (parameters->getParametersAvailable()) {
       const QString paramName = parameters->getNextParameterAsString("Param");
       if (paramName == "-background-color") {
          const int red   = parameters->getNextParameterAsInt("Red");
          const int green = parameters->getNextParameterAsInt("Green");
          const int blue  = parameters->getNextParameterAsInt("Blue");
          pf->setSurfaceBackgroundColor(red, green, blue);
       }
       else if (paramName == "-foreground-color") {
          const int red   = parameters->getNextParameterAsInt("Red");
          const int green = parameters->getNextParameterAsInt("Green");
          const int blue  = parameters->getNextParameterAsInt("Blue");
          pf->setSurfaceForegroundColor(red, green, blue);
       }
       else if (paramName == "-debug") {
          const bool debugOnFlag = parameters->getNextParameterAsBoolean("Debug Value");
          pf->setDebugOn(debugOnFlag);
       }
       else {
          throw CommandException("Unrecognized parameter: " + paramName);
       }
   }
   
   pf->writeFile(pf->getFileName());
}

      

