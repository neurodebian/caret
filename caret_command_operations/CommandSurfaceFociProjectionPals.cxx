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
#include "CommandSurfaceFociProjectionPals.h"
#include "FileFilters.h"
#include "FociFile.h"
#include "FociFileToPalsProjector.h"
#include "FociProjectionFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StudyMetaDataFile.h"

/**
 * constructor.
 */
CommandSurfaceFociProjectionPals::CommandSurfaceFociProjectionPals()
   : CommandBase("-surface-foci-projection-pals",
                 "SURFACE FOCI PROJECTION TO PALS ATLAS")
{
}

/**
 * destructor.
 */
CommandSurfaceFociProjectionPals::~CommandSurfaceFociProjectionPals()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceFociProjectionPals::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   QStringList inputFilters;
   inputFilters << FileFilters::getFociFileFilter();
   inputFilters << FileFilters::getFociProjectionFileFilter();
   
   paramsOut.clear();
   paramsOut.addFile("Input Foci or Foci Projection File",
                     inputFilters);
   paramsOut.addFile("Output Foci Projection File",
                     FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Study Metadata File",
                     FileFilters::getStudyMetaDataFileFilter());
   paramsOut.addVariableListOfParameters("");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceFociProjectionPals::getHelpInformation() const
{
   const QString cerebralCutoffValue(
      QString::number(FociFileToPalsProjector::getDefaultCerebralCutoffDistance(), 'f', 2));
   const QString cerebellumCutoffValue(
      QString::number(FociFileToPalsProjector::getDefaultCerebellumCutoffDistance(), 'f', 2));
      

   QString s = "cell";
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-foci-or-foci-projection-file-name>\n"
       + indent9 + "<output-foci-projection-file-name>\n"
       + indent9 + "<input-study-metadata-file-name>\n"
       + indent9 + "[-cutoffs   cerebral-cutoff  cerebellum-cutoff]\n"
       + indent9 + "[-no-cerebellum]\n"
       + indent9 + "[-project-onto-surface  onto-surface-above-distance]\n"
       + indent9 + "\n"
       + indent9 + "Project foci to the PALS Atlas.\n"
       + indent9 + "\n"
       + indent9 + "The input file may be either a foci file or a foci\n"
       + indent9 + "projection file.  If the input file is a foci file, its\n"
       + indent9 + "name must end with \"" + SpecFile::getFociFileExtension() + "\".\n"
       + indent9 + "\n"
       + indent9 + "Add the \"-no-cerebellum\" option if you DO NOT want foci\n"
       + indent9 + "projected using the cerebellum.\n"
       + indent9 + "\n"
       + indent9 + "Use the \"-project-onto-surface\" option to project the\n"
       + indent9 + "foci so that they are a specified distance on or above\n"
       + indent9 + "the surfaces.\n"
       + indent9 + "\n"
       + indent9 + "Use the \"-cutoffs\" to control how foci are projected \n"
       + indent9 + "using a cerebral hemisphere and/or the cerebellum.\n"
       + indent9 + "   A ratio of the distance to the cerebral cortex relative\n"
       + indent9 + "   to the distance to the cerebellum is computed.\n"
       + indent9 + "   If this ratio is less than the \"cerebral-cutoff\", \n"
       + indent9 + "   the focus is assigned to the cerebral cortex.  If this\n"
       + indent9 + "   ratio is greater than \"cerebellum-cutoff\", the focus\n"
       + indent9 + "   is assigned to the cerebellum.  If the ratio is between\n"
       + indent9 + "   the two cutoffs, the focus is assigned to both a \n"
       + indent9 + "   cerebral hemisphere and the cerebellum.\n"
       + indent9 + "\n"
       + indent9 + "   Default \"cerebral-cutoff\": " + cerebralCutoffValue + "\n"
       + indent9 + "   Default \"cerebellum-cutoff\": " + cerebellumCutoffValue + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceFociProjectionPals::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputFociProjectionFileName =
      parameters->getNextParameterAsString("Input Foci or Foci Projection File Name");
   const QString outputFociProjectionFileName =
      parameters->getNextParameterAsString("Output Foci Projection File Name");
   const QString inputStudyMetaDataFileName =
      parameters->getNextParameterAsString("Input Study Metadata File Name");
      
   //
   // Process optional parameters
   //
   float cerebralCutoffDistance = 
      FociFileToPalsProjector::getDefaultCerebralCutoffDistance();
   float cerebellumCutoffDistance = 
      FociFileToPalsProjector::getDefaultCerebellumCutoffDistance();
   bool projectToCerebellumFlag = true;
   bool projectToSurfaceFlag = false;
   float surfaceAboveDistance = 0.0;
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString("Foci Projection Parameter");
      if (paramName == "-cutoffs") {
         cerebralCutoffDistance =
            parameters->getNextParameterAsFloat("Cerebral Cutoff Distance");
         cerebellumCutoffDistance =
            parameters->getNextParameterAsFloat("Cerebellum Cutoff Distance");
      }
      else if (paramName == "-no-cerebellum") {
         projectToCerebellumFlag = false;
      }
      else if (paramName == "-project-onto-surface") {
         projectToSurfaceFlag = true;
         surfaceAboveDistance = 
            parameters->getNextParameterAsFloat("Foci Projection Parameter: Above surface distance");
      }
      else {
         throw CommandException("unrecognized option");
      }
   }
   
   // 
   // Create a brain set
   //
   BrainSet brainSet; 
   
   //
   // Read the foci projection file
   //
   FociProjectionFile fociProjectionFile;
   if (inputFociProjectionFileName.endsWith(SpecFile::getFociFileExtension())) {
      FociFile ff;
      ff.readFile(inputFociProjectionFileName);
      fociProjectionFile.appendFiducialCellFile(ff);
   }
   else {
      fociProjectionFile.readFile(inputFociProjectionFileName);
   }
   
   //
   // Read the study metadata file
   //
   StudyMetaDataFile studyMetaDataFile;
   studyMetaDataFile.readFile(inputStudyMetaDataFileName);
   
   //
   // Project the foci file to the PALS atlas
   //
   FociFileToPalsProjector palsProjector(&brainSet,
                                         &fociProjectionFile,
                                         &studyMetaDataFile,
                                         0,
                                         fociProjectionFile.getNumberOfCellProjections() - 1,
                                         surfaceAboveDistance,
                                         projectToSurfaceFlag,
                                         projectToCerebellumFlag,
                                         cerebralCutoffDistance,
                                         cerebellumCutoffDistance);
   palsProjector.execute();
   
   //
   // Write the foci projection file
   //
   fociProjectionFile.writeFile(outputFociProjectionFileName);
}

      

