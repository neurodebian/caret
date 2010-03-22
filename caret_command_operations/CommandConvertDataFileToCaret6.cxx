
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

#include <QDateTime>
#include <QDir>
#include <QVector>
#include <CaretVersion.h>

#include "AbstractFile.h"
#include "AreaColorFile.h"
#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandConvertDataFileToCaret6.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "Structure.h"
#include "TopologyFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandConvertDataFileToCaret6::CommandConvertDataFileToCaret6()
   : CommandBase("-caret6-convert-data-file",
                 "CONVERT DATA FILE TO CARET6 FORMAT")
{
}

/**
 * destructor.
 */
CommandConvertDataFileToCaret6::~CommandConvertDataFileToCaret6()
{
}

/**
 * get the script builder parameters.
 */
void
CommandConvertDataFileToCaret6::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString
CommandConvertDataFileToCaret6::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "[-allow-overwrite]\n"
       + indent9 + "[-structure  structure-name]\n"
       + indent9 + "[-area-color-file    area-color-file-name]\n"
       + indent9 + "[-border-color-file  border-color-file-name]\n"
       + indent9 + "[-foci-color-file    foci-color-file-name]\n"
       + indent9 + "<names-of-files-for-conversion>]\n"
       + indent9 + "\n"
       + indent9 + "Since Caret6 expects colors to be within the border, foci\n"
       + indent9 + ", label, and label volume files, the appropriate color\n"
       + indent9 + "file should be specified using its color file option. \n"
       + indent9 + "This results in the colors being added into the border,\n"
       + indent9 + "foci, label, or label volume file.\n"
       + indent9 + "\n"
       + indent9 + "It is highly recommended that the structure be specified\n"
       + indent9 + "as many Caret6 data files are structure dependent.\n"
       + indent9 + "\"structure-name\" is one of:\n"
       + indent9 + "   right\n"
       + indent9 + "   left\n"
       + indent9 + "   cerebellum\n"
       + indent9 + "\n"
       + indent9 + "By default, files are not overwritten.  To allow \n"
       + indent9 + "the overwrite of exisiting files, the \"-allow-overwrite\"\n"
       + indent9 + "option must be added to the command.\n"
       + indent9 + "\n"
       + indent9 + "This command ALWAYS writes files to the current directory.\n"
       + indent9 + "IT IS BEST TO RUN THIS COMMAND FROM A DIRECTORY DIFFERENT\n"
       + indent9 + "THAN THE DIRECTORY THE INPUT FILE IS LOCATED.  Running\n"
       + indent9 + "the command in this manner, will prevent the original files\n"
       + indent9 + "from being corrupted if the command fails.\n"
       + indent9 + "\n"
       + indent9 + "In most cases, you are better off converting a spec file\n"
       + indent9 + "and its data file to Caret6 format using \n"
       + indent9 + "   \"caret_command -convert-spec-file-to-caret6\" as it\n"
       + indent9 + "    will convert coordinate and topology files into \n"
       + indent9 + "   surface files and create a spec file that is Caret6 \n"
       + indent9 + "   compatible.\n"
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandConvertDataFileToCaret6::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   AbstractFile::setOverwriteExistingFilesAllowed(false);
   Structure structure;
   QVector<QString> filenames;
   QVector<QString> areaColorFileNames;
   QVector<QString> borderColorFileNames;
   QVector<QString> fociColorFileNames;

   while (parameters->getParametersAvailable()) {
      const QString paramName =
        parameters->getNextParameterAsString("Convert to Caret6 Parameter");

      if (paramName == "-allow-overwrite") {
         AbstractFile::setOverwriteExistingFilesAllowed(true);
      }
      else if (paramName == "-area-color-file") {
         areaColorFileNames.push_back(parameters->getNextParameterAsString("Area Color File Name"));
      }
      else if (paramName == "-border-color-file") {
         borderColorFileNames.push_back(parameters->getNextParameterAsString("Border Color File Name"));
      }
      else if (paramName == "-foci-color-file") {
         fociColorFileNames.push_back(parameters->getNextParameterAsString("Foci Color File Name"));
      }
      else if (paramName == "-structure") {
         QString structureName = parameters->getNextParameterAsString("Structure Name").toLower();
         if (structureName == "left") {
            structure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
         }
         else if (structureName == "right") {
            structure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
         }
         else if (structureName == "cerebellum") {
            structure.setType(Structure::STRUCTURE_TYPE_CEREBELLUM);
         }
         else {
            throw CommandException("Invalid structure name: " + structureName);
         }
      }
      else {
         filenames.push_back(paramName);
      }
   }

   int numDataFiles = static_cast<int>(filenames.size());

   if (numDataFiles <= 0) {
      throw CommandException("No files were specified for conversion");
   }

   if (areaColorFileNames.size() > 0) {
      for (int i = 0; i < areaColorFileNames.size(); i++) {
         AreaColorFile acf;
         acf.readFile(areaColorFileNames[i]);
         this->areaColorFile.append(acf);
      }
   }

   if (borderColorFileNames.size() > 0) {
      for (int i = 0; i < borderColorFileNames.size(); i++) {
         BorderColorFile bcf;
         bcf.readFile(borderColorFileNames[i]);
         this->borderColorFile.append(bcf);
      }
   }

   if (fociColorFileNames.size() > 0) {
      for (int i = 0; i < fociColorFileNames.size(); i++) {
         FociColorFile fcf;
         fcf.readFile(fociColorFileNames[i]);
         this->fociColorFile.append(fcf);
      }
   }

   for (int i = 0; i < numDataFiles; i++) {
      QString inputFileName = filenames.at(i);
      QString outputFileName = this->convertFile(inputFileName, structure);
   }
}

/**
 *  Convert a file.
 */
QString
CommandConvertDataFileToCaret6::convertFile(const QString& inputFileName,
                                    const Structure& structure)
{
   QString errorMessage;
   QString outputFileName;
   AbstractFile* af = AbstractFile::readAnySubClassDataFile(inputFileName, false, errorMessage);
   if (af != NULL) {
      ColorFile* colorFile = NULL;
      if ((dynamic_cast<PaintFile*>(af) != NULL) ||
          (dynamic_cast<VolumeFile*>(af) != NULL)) {
         colorFile = &this->areaColorFile;
      }
      else if ((dynamic_cast<BorderFile*>(af) != NULL) ||
               (dynamic_cast<BorderProjectionFile*>(af) != NULL)) {
         colorFile = &this->borderColorFile;
      }
      else if ((dynamic_cast<FociFile*>(af) != NULL) ||
               (dynamic_cast<FociProjectionFile*>(af) != NULL)) {
         colorFile = &this->fociColorFile;
      }
      else if (dynamic_cast<VolumeFile*>(af) != NULL) {
         colorFile = &this->areaColorFile;
      }

      af->updateMetaDataForCaret6();
      
      QString dataFileName = FileUtilities::basename(inputFileName);
      try {
         outputFileName =
              af->writeFileInCaret6Format(dataFileName, structure, colorFile, true);
         std::cout << inputFileName.toAscii().constData() << " OK" << std::endl;
      }
      catch (FileException e) {
         std::cout << " ERROR CONVERTING "
                   << dataFileName.toAscii().constData()
                   << ": "
                   << e.whatQString().toAscii().constData()
                   << std::endl;
      }
   }
   else {
      std::cout << " WARNING, UNRECOGNIZED file type for file: "
                << inputFileName.toAscii().constData()
                << std::endl;
   }
   
   return outputFileName;
}



