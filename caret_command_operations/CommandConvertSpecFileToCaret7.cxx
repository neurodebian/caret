
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
#include "CommandConvertSpecFileToCaret7.h"
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
CommandConvertSpecFileToCaret7::CommandConvertSpecFileToCaret7()
   : CommandBase("-spec-file",
                 "CONVERT SPEC FILE AND ITS DATA FILES TO WORKBENCH FORMAT")
{
}

/**
 * destructor.
 */
CommandConvertSpecFileToCaret7::~CommandConvertSpecFileToCaret7()
{
}

/**
 * get the script builder parameters.
 */
void
CommandConvertSpecFileToCaret7::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString
CommandConvertSpecFileToCaret7::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<structure-name>\n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "[-allow-overwrite]\n"
       + indent9 + "[-write-color-files]\n"
       + indent9 + "\n"
       + indent9 + "\"structure-name\" is one of:\n"
       + indent9 + "   right\n"
       + indent9 + "   left\n"
       + indent9 + "   cerebellum\n"
       + indent9 + "\n"
       + indent9 + "Coordinate files are converted into a Surface file using\n"
       + indent9 + "the best available topology.\n"
       + indent9 + "\n"
       + indent9 + "Topology files are converted but are NOT added to the\n"
       + indent9 + "output spec file.\n"
       + indent9 + "\n"
       + indent9 + "By default, color files are not converted.  This is\n"
       + indent9 + "because this command will add the needed colors to\n"
       + indent9 + "border, foci, label, and volume files as needed.  If\n"
       + indent9 + "overwrite of color files is desired, add the \n"
       + indent9 + "\"-write-color-files\" option to the command.\n"
       + indent9 + "\n"
       + indent9 + "By default, files are not overwritten.  To allow \n"
       + indent9 + "the overwrite of exisiting files, the \"-allow-overwrite\"\n"
       + indent9 + "option must be added to the command.\n"
       + indent9 + "\n"
       + indent9 + "This command ALWAYS writes files to the current directory.\n"
       + indent9 + "IT IS BEST TO RUN THIS COMMAND FROM AN EMPTY DIRECTORY\n"
       + indent9 + "AND USE, AS INPUT, A SPEC FILE FROM ANOTHER DIRECTORY.  \n"
       + indent9 + "Running the command in this manner, will prevent the \n"
       + indent9 + "original files from being corrupted if the command fails.\n"
       + indent9 + "\n");

   return helpInfo;
}

/**
 * execute the command.
 */
void
CommandConvertSpecFileToCaret7::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   AbstractFile::setOverwriteExistingFilesAllowed(false);
   Structure structure;
   QString structureName =
           parameters->getNextParameterAsString("Structure Name").toLower();
   if (structureName == "left") {
      structure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
   }
   else if (structureName == "right") {
      structure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
   }
   else if (structureName == "cerebellum") {
      structure.setType(Structure::STRUCTURE_TYPE_CEREBELLUM);
   }
   else if (structureName == "invalid") {
      structure.setType(Structure::STRUCTURE_TYPE_INVALID);
   }
   else {
      throw CommandException("Invalid structure name: " + structureName);
   }
   QString specFileName = parameters->getNextParameterAsString("Spec File Name");
   this->writeColorFilesFlag = false;

   while (parameters->getParametersAvailable()) {
      const QString paramName =
        parameters->getNextParameterAsString("Convert Spec File to Workbench Parameter");
      if (paramName == "-allow-overwrite") {
         AbstractFile::setOverwriteExistingFilesAllowed(true);
      }
      else if (paramName == "-write-color-files") {
         this->writeColorFilesFlag = true;
      }
      else {
         throw CommandException("ERROR: Unrecognized parameter: \""
                                + paramName
                                + "\"");
      }
   }  

   SpecFile sf;
   sf.readFile(specFileName);
   if (structure.getType() != Structure::STRUCTURE_TYPE_INVALID) {
      sf.setStructure(structure);
   }
   QString path = FileUtilities::dirname(specFileName);
   if (path.length() > 0) {
      if (path != ".") {
         sf.prependPathsToAllFiles(path, true);
      }
   }

   //
   // Get topology files
   //
   QString closedTopoFileName;
   if (sf.closedTopoFile.getNumberOfFiles() > 0) {
      closedTopoFileName = sf.closedTopoFile.getFileName(0);
   }
   QString openTopoFileName;
   if (sf.openTopoFile.getNumberOfFiles() > 0) {
      openTopoFileName = sf.openTopoFile.getFileName(0);
   }
   QString cutTopoFileName;
   if (sf.cutTopoFile.getNumberOfFiles() > 0) {
      cutTopoFileName = sf.cutTopoFile.getFileName(0);
   }
   QString lobarCutTopoFileName;
   if (sf.lobarCutTopoFile.getNumberOfFiles() > 0) {
      lobarCutTopoFileName = sf.lobarCutTopoFile.getFileName(0);
   }

   readColorFiles(sf);

   std::vector<SpecFile::Entry> allEntries;
   sf.getAllEntries(allEntries);

   std::vector<QString> newSurfaceFileTags;
   std::vector<QString> newSurfaceFileNames;

   SpecFile outputSpecFile;

   int numEntries = sf.getNumberOfEntries();
   for (int i = 0; i < numEntries; i++) {
      SpecFile::Entry* sfe = sf.getEntry(i);

      int numFiles = sfe->getNumberOfFiles();
      for (int j = 0; j < numFiles; j++) {
            QString name = sfe->getFileName(j);
         if (sfe == &sf.rawCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getRawSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.fiducialCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getFiducialSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);

            if (openTopoFileName.isEmpty() == false) {
               QString outputSurfaceFileName =
                       this->convertCoordTopoToSurfaceFile(name,
                                                          openTopoFileName,
                                                          structure,
                                                          ".OPEN");
               newSurfaceFileTags.push_back(SpecFile::getFiducialSurfaceFileTag());
               newSurfaceFileNames.push_back(outputSurfaceFileName);
            }
         }
         else if (sfe == &sf.inflatedCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getInflatedSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.veryInflatedCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getVeryInflatedSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.sphericalCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getSphericalSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.ellipsoidCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getEllipsoidSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.compressedCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getCompressedSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.flatCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       cutTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getFlatSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.lobarFlatCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       lobarCutTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getLobarFlatSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else if (sfe == &sf.hullCoordFile) {
            QString outputSurfaceFileName =
                    this->convertCoordTopoToSurfaceFile(name,
                                                       closedTopoFileName,
                                                       structure,
                                                       "");
            newSurfaceFileTags.push_back(SpecFile::getHullSurfaceFileTag());
            newSurfaceFileNames.push_back(outputSurfaceFileName);
         }
         else {
            QString outputFileName = this->convertFile(name, structure);
            if (outputFileName.isEmpty() == false) {
               //
               // DO NOT add topology file to the spec file
               //
               if (outputFileName.endsWith(".topo.gii") == false) {
                  outputSpecFile.addToSpecFile(sfe->specFileTag, outputFileName, "", false);
               }
            }
         }
      }
   }

   for (unsigned int i = 0; i < newSurfaceFileTags.size(); i++) {
      outputSpecFile.addToSpecFile(newSurfaceFileTags[i],
                       newSurfaceFileNames[i],
                       "",
                       false);
   }

   outputSpecFile.setCategory(sf.getCategory());
   outputSpecFile.setFileComment(sf.getFileComment());
   outputSpecFile.setSpace(sf.getSpace());
   outputSpecFile.setSpecies(sf.getSpecies());
   outputSpecFile.setStructure(sf.getStructure());
   outputSpecFile.setSubject(sf.getSubject());
   outputSpecFile.writeFileInCaret7Format(FileUtilities::basename(specFileName), structure, NULL, true);
}

/**
 * Convert a file.
 */
QString
CommandConvertSpecFileToCaret7::convertCoordTopoToSurfaceFile(const QString& coordFileName,
                                                      const QString& topoFileName,
                                                      const Structure& structureIn,
                                                      const QString& preExt)
{
   if (topoFileName.isEmpty()) {
      throw CommandException("No topology file available for coordinate file: \""
                             + coordFileName + "\"");
   }
   BrainSet bs(topoFileName, coordFileName);
   BrainModelSurface* bms = bs.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Unable to create surface from coordinate file: \""
                             + coordFileName + "\"");
   }
   TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("Problems reading topology file coordinate file \""
                             + coordFileName + "\"");
   }
   
   Structure structure = structureIn;
   if (structure == Structure::STRUCTURE_TYPE_INVALID) {
      structure = Structure::convertStringToType(
              bms->getCoordinateFile()->getHeaderTag(
                      AbstractFile::headerTagStructure));
   }

   bms->orientNormalsOut();
    
   QString fileName;
   try {
      fileName = bms->writeSurfaceInCaret7Format(coordFileName, preExt, structure, true);
      std::cout << coordFileName.toAscii().constData()
             << " and "
             << topoFileName.toAscii().constData()
             << " into a surface file "
             << fileName.toAscii().constData()
             << " OK" << std::endl;
   }
   catch (FileException e) {
      std::cout << " ERROR CONVERTING "
                << coordFileName.toAscii().constData()
                << " and "
                << topoFileName.toAscii().constData()
                << " to surface: "
                << e.whatQString().toAscii().constData()
                << std::endl;
   }
   return fileName;
}

/**
 *  Read the color files.
 */
void
CommandConvertSpecFileToCaret7::readColorFiles(SpecFile& sf)
{
   std::vector<QString> areaColorFileNames;
   sf.areaColorFile.getAllFilesNoDataFile(areaColorFileNames);
   for (unsigned int i = 0; i < areaColorFileNames.size(); i++) {
      try {
         AreaColorFile acf;
         acf.readFile(areaColorFileNames[i]);
         this->areaColorFile.append(acf);
      }
      catch (FileException e) {
         throw CommandException(e.whatQString());
      }
   }

   std::vector<QString> borderColorFileNames;
   sf.borderColorFile.getAllFilesNoDataFile(borderColorFileNames);
   for (unsigned int i = 0; i < borderColorFileNames.size(); i++) {
      try {
         BorderColorFile bcf;
         bcf.readFile(borderColorFileNames[i]);
         this->borderColorFile.append(bcf);
      }
      catch (FileException e) {
         throw CommandException(e.whatQString());
      }
   }

   std::vector<QString> fociColorFileNames;
   sf.fociColorFile.getAllFilesNoDataFile(fociColorFileNames);
   for (unsigned int i = 0; i < fociColorFileNames.size(); i++) {
      try {
         FociColorFile fcf;
         fcf.readFile(fociColorFileNames[i]);
         this->fociColorFile.append(fcf);
      }
      catch (FileException e) {
         throw CommandException(e.whatQString());
      }
   }
}

/**
 *  Convert a file.
 */
QString
CommandConvertSpecFileToCaret7::convertFile(const QString& inputFileName,
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

      //
      // Should color files be skipped
      //
      if (this->writeColorFilesFlag == false) {
         if (dynamic_cast<ColorFile*>(af) != NULL) {
            return "";
         }
      }

      af->updateMetaDataForCaret7();
      QString dataFileName = FileUtilities::basename(inputFileName);
      try {
         outputFileName =
              af->writeFileInCaret7Format(dataFileName, structure, colorFile, true);
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




