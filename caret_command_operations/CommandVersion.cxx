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

#include <QGlobalStatic>

#include "CaretVersion.h"
#include "CommandVersion.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "vtkVersion.h"

/**
 * constructor.
 */
CommandVersion::CommandVersion()
   : CommandBase("-version",
                 "VERSION")
{
}

/**
 * destructor.
 */
CommandVersion::~CommandVersion()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVersion::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandVersion::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "\n"
       + indent9 + "Display the version of command operations.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVersion::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   std::cout << "Caret Version: "
             << CaretVersion::getCaretVersionAsString().toAscii().constData()
             << std::endl;
   std::cout << "QT Version: " 
             << qVersion()
             << std::endl;
   std::cout << "VTK Version: "
             << vtkVersion::GetVTKVersion()
             << std::endl;
   std::cout << "Date Compiled: "
             << __DATE__
             << std::endl;
}

      

