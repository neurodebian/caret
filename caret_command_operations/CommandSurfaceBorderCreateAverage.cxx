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

#include "BorderFile.h"
#include "CommandSurfaceBorderCreateAverage.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderCreateAverage::CommandSurfaceBorderCreateAverage()
   : CommandBase("-surface-border-create-average",
                 "SURFACE BORDER CREATE AVERAGE")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderCreateAverage::~CommandSurfaceBorderCreateAverage()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderCreateAverage::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFloat("Resampling Distance", 10.0);
   paramsOut.addBoolean("Project to Sphere", false);
   paramsOut.addFile("Output Border File Name", FileFilters::getBorderGenericFileFilter());
   paramsOut.addMultipleFiles("Input Border Files", FileFilters::getBorderGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderCreateAverage::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<resampling-distance>\n"
       + indent9 + "<project-to-sphere>\n"
       + indent9 + "<output-border-file>\n"
       + indent9 + "<input-border-file-1>\n"
       + indent9 + "<input-border-file-2>\n"
       + indent9 + "[additional-input-border-files]\n"
       + indent9 + "\n"
       + indent9 + "Create an average border file from two or more border files.\n"
       + indent9 + "\n"
       + indent9 + "<project-to-sphere> is one of:\n"
       + indent9 + "   true\n"
       + indent9 + "   false\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderCreateAverage::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const float resamplingDistance = 
      parameters->getNextParameterAsFloat("Resampling Distance");
   const bool projectToSphereFlag =
      parameters->getNextParameterAsBoolean("Project to Sphere");
   const QString outputBorderFileName = 
      parameters->getNextParameterAsString("Output Border File Name");
   std::vector<QString> inputBorderFileNames;
   inputBorderFileNames.push_back(
      parameters->getNextParameterAsString("Input Border File Name 1"));
   inputBorderFileNames.push_back(
      parameters->getNextParameterAsString("Input Border File Name 2"));
   while (parameters->getParametersAvailable()) {
      inputBorderFileNames.push_back(
         parameters->getNextParameterAsString("Input Border File Name"));
   }

   //
   // Read the input border files
   //
   const int numBorderFiles = static_cast<int>(inputBorderFileNames.size());
   std::vector<BorderFile*> inputBorderFiles;
   for (int i = 0; i < numBorderFiles; i++) {
      BorderFile* bf = new BorderFile;
      try {
         bf->readFile(inputBorderFileNames[i]);
         inputBorderFiles.push_back(bf);
      }
      catch (FileException& e) {
         for (unsigned int j = 0; j < inputBorderFiles.size(); j++) {
            delete inputBorderFiles[j];
         }
         throw e;
      }
   }
   
   //
   // Create the average border
   //
   BorderFile averageBorderFile;
   BorderFile::createAverageBorderFile(inputBorderFiles,
                                       resamplingDistance,
                                       projectToSphereFlag,
                                       averageBorderFile);
   
   //
   // Save output border file
   //
   averageBorderFile.writeFile(outputBorderFileName);
   
   //
   // Free memory
   //
   for (unsigned int j = 0; j < inputBorderFiles.size(); j++) {
      delete inputBorderFiles[j];
   }
}

      

