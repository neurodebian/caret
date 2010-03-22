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

#include "CommandSurfaceFociStudyValidate.h"
#include "FileFilters.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StudyMetaDataFile.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandSurfaceFociStudyValidate::CommandSurfaceFociStudyValidate()
   : CommandBase("-surface-foci-study-validate",
                 "SURFACE FOCI STUDY VALIDATE")
{
}

/**
 * destructor.
 */
CommandSurfaceFociStudyValidate::~CommandSurfaceFociStudyValidate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceFociStudyValidate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{                     
   paramsOut.clear();

   QStringList fileFilters;
   fileFilters << FileFilters::getFociFileFilter();
   fileFilters << FileFilters::getFociProjectionFileFilter();
   paramsOut.addFile("Foci or Foci Projection File", fileFilters);
   paramsOut.addFile("Study Metadata File", FileFilters::getStudyMetaDataFileFilter());
   paramsOut.addVariableListOfParameters("Output Text File Name");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceFociStudyValidate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<innput-foci-or-foci-projection-file>\n"
       + indent9 + "<input-study-metatdata-file>\n"
       + indent9 + "[-output  output-text-file]\n"
       + indent9 + "\n"
       + indent9 + "Verify that all foci have valid study metadata links.\n"
       + indent9 + "Errors include links to invalid studies, foci names \n"
       + indent9 + "that do not match the study name, or links to invalid\n"
       + indent9 + "tables, table subheaders, figures, figure panels, \n"
       + indent9 + "page numbers, and page subheaders.\n"
       + indent9 + "\n"
       + indent9 + "If there are any errors, they are written to the output\n"
       + indent9 + "text file if it is provided.  Otherwise, results are \n"
       + indent9 + "written to the terminal.\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceFociStudyValidate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString fociFileName = 
      parameters->getNextParameterAsString("Input Foci or Foci Projection File");
   const QString studyFileName =
      parameters->getNextParameterAsString("Input Study Metadata File");
   QString textFileName;
   while (parameters->getParametersAvailable()) {
      QString paramName = parameters->getNextParameterAsString("Opt Param");
      if (paramName == "-output") {
         textFileName = parameters->getNextParameterAsString("Text File Name");
      }
      else {
         throw CommandException("Unexpected Parameter: " + paramName);
      }
   }
   
   FociFile ff;
   FociProjectionFile fpf;
   if (fociFileName.endsWith(SpecFile::getFociFileExtension()) ||
       fociFileName.endsWith(SpecFile::getFociFileExtension() + ".csv")) {
      ff.readFile(fociFileName);
   }
   else if (fociFileName.endsWith(SpecFile::getFociProjectionFileExtension())) {
      fpf.readFile(fociFileName);
   }
   else {
      throw CommandException("The foci or foci projection file must end with "
                             + SpecFile::getFociFileExtension()
                             + " or "
                             + SpecFile::getFociProjectionFileExtension());
   }
   
   StudyMetaDataFile smdf;
   smdf.readFile(studyFileName);
   
   QString msg;
   if (ff.getNumberOfCells() > 0) {
      const int num = ff.getNumberOfCells();
      for (int i = 0; i < num; i++) {
         CellData* cd = ff.getCell(i);
         QStringList sl = cd->validateStudyMetaDataLink(&smdf);
         for (int m = 0; m < sl.count(); m++) {
            msg += "Focus " + QString::number(i)
                   + " " + cd->getName() + ": "
                   + sl.at(m) + "\n";
         }
      }
   }
   if (fpf.getNumberOfCellProjections() > 0) {
      const int num = fpf.getNumberOfCellProjections();
      for (int i = 0; i < num; i++) {
         CellProjection* cd = fpf.getCellProjection(i);
         QStringList sl = cd->validateStudyMetaDataLink(&smdf);
         for (int m = 0; m < sl.count(); m++) {
            msg += "Focus " + QString::number(i)
                + " " + cd->getName() + ": "
                + sl.at(m) + "\n";
         }
      }
   }
   
   if (msg.isEmpty()) {
      std::cout << "No foci/study problems were found." << std::endl;
   }
   else {
      if (textFileName.isEmpty() == false) {
         TextFile tf;
         tf.setText(msg);
         tf.writeFile(textFileName);
      }
      else {
         std::cout << msg.toAscii().constData() << std::endl;
      }
   }
}

      

