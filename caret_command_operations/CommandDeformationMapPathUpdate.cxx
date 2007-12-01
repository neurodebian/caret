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

#include "CommandDeformationMapPathUpdate.h"
#include "DeformationMapFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandDeformationMapPathUpdate::CommandDeformationMapPathUpdate()
   : CommandBase("-deformation-map-path",
                 "DEFORMATION MAP FILE SOURCE/TARGET PATH UPDATE")
{
}

/**
 * destructor.
 */
CommandDeformationMapPathUpdate::~CommandDeformationMapPathUpdate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandDeformationMapPathUpdate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Deformation Map File Name",
                     FileFilters::getDeformationMapFileFilter());
   paramsOut.addDirectory("Source Path");
   paramsOut.addDirectory("Target Path");
}
/**
 * get full help information.
 */
QString 
CommandDeformationMapPathUpdate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<deformation-map-file-name>  \n"
       + indent9 + "<new-source-path>  \n"
       + indent9 + "<new-target-path> \n"
       + indent9 + "\n"
       + indent9 + "Update the source (individual) and target (atlas) paths\n"
       + indent9 + "in a deformation map file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandDeformationMapPathUpdate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString defMapFileName =
      parameters->getNextParameterAsString("Deformation Map File");
   const QString sourcePath =
      parameters->getNextParameterAsString("Source Path");
   const QString targetPath =
      parameters->getNextParameterAsString("Target Path");
                                 
   //
   // Make sure that are no more parameters
   //
   checkForExcessiveParameters();

   DeformationMapFile defMapFile;
   
   defMapFile.readFile(defMapFileName);
   
   defMapFile.setSourceDirectory(sourcePath);
   defMapFile.setTargetDirectory(targetPath);
   
   defMapFile.writeFile(defMapFileName);
}


      

