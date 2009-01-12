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
#include <set>
#include <vector>

#include <QFile>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "CommandColorFileCreateMissingColors.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandColorFileCreateMissingColors::CommandColorFileCreateMissingColors()
   : CommandBase("-color-file-create-missing-colors",
                 "COLOR FILE CREATE MISSING COLORS")
{
}

/**
 * destructor.
 */
CommandColorFileCreateMissingColors::~CommandColorFileCreateMissingColors()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandColorFileCreateMissingColors::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> modeNames;
   modeNames.push_back("EXACT");
   modeNames.push_back("PARTIAL");
   
   QStringList colorFileFilters;
   colorFileFilters << FileFilters::getAreaColorFileFilter()
                    << FileFilters::getBorderColorFileFilter()
                    << FileFilters::getCellColorFileFilter()
                    << FileFilters::getFociColorFileFilter();
                    
   QStringList dataFileFilters;
   dataFileFilters << FileFilters::getBorderGenericFileFilter()
                   << FileFilters::getBorderProjectionFileFilter()
                   << FileFilters::getCellFileFilter()
                   << FileFilters::getCellProjectionFileFilter()
                   << FileFilters::getFociFileFilter()
                   << FileFilters::getFociProjectionFileFilter()
                   << FileFilters::getPaintFileFilter()
                   << FileFilters::getVolumePaintFileFilter();
   paramsOut.clear();
   paramsOut.addListOfItems("Mode", modeNames, modeNames);
   paramsOut.addMultipleFiles("Input Color File", colorFileFilters);
   paramsOut.addMultipleFiles("Output Color File", colorFileFilters);
   paramsOut.addMultipleFiles("Data File", dataFileFilters);
}

/**
 * get full help information.
 */
QString 
CommandColorFileCreateMissingColors::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<MODE> \n"
       + indent9 + "<input-color-file-name>\n"
       + indent9 + "<output-color-file-name>\n"
       + indent9 + "<data-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Create random colors for names from the data file that do\n"
       + indent9 + "not have matching colors in the color file.\n"
       + indent9 + "\n"
       + indent9 + "\"MODE\" is one of:\n"
       + indent9 + "   EXACT    \n"
       + indent9 + "   PARTIAL \n"
       + indent9 + "\n"
       + indent9 + "If \"MODE\" is \"EXACT\", a new color is created if there\n"
       + indent9 + "is not a color name that exactly matches a name from the \n"
       + indent9 + "data file.\n"
       + indent9 + "\n"
       + indent9 + "If \"MODE\" is \"PARTIAL\", a new color is created for a \n"
       + indent9 + "name only if the name does not start with the name of a \n"
       + indent9 + "color.  For example, if the name is \"SUL.CeS\" and there \n"
       + indent9 + "is a color named \"SUL\", a new color is NOT created.\n"
       + indent9 + "\n"
       + indent9 + "New colors are added to the input color file and written\n"
       + indent9 + "to the output color file.  The input color file does not \n"
       + indent9 + "need to exist.  If there is not an input color file its\n"
       + indent9 + "name may be specified by two consecutive double quotes (\"\").\n"
       + indent9 + "\n"
       + indent9 + "The data file may be a:\n"
       + indent9 + "   Border File\n"
       + indent9 + "   Border Projection File\n"
       + indent9 + "   Cell File\n"
       + indent9 + "   Cell Projection File\n"
       + indent9 + "   Foci File\n"
       + indent9 + "   Foci Projection File\n"
       + indent9 + "   Paint File\n"
       + indent9 + "   Volume Paint File\n"
       + indent9 + "\n"
       + indent9 + "The valid color file extensions are:\n"
       + indent9 + "   Area Color:   " + SpecFile::getAreaColorFileExtension() + "\n"
       + indent9 + "   Border Color: " + SpecFile::getBorderColorFileExtension() + "\n"
       + indent9 + "   Cell Color:   " + SpecFile::getCellColorFileExtension() + "\n"
       + indent9 + "   Foci Color:   " + SpecFile::getFociColorFileExtension() + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandColorFileCreateMissingColors::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString modeName = parameters->getNextParameterAsString("MODE");
   const QString inputColorFileName = 
      parameters->getNextParameterAsString("Input Color File Name");
   const QString outputColorFileName = 
      parameters->getNextParameterAsString("Output Color File Name");
   const QString dataFileName =
      parameters->getNextParameterAsString("Data File Name");
   checkForExcessiveParameters();
   
   //
   // Check mode
   //
   bool exactMatchFlag = false;
   if (modeName == "EXACT") {
      exactMatchFlag = true;
   }
   else if (modeName == "PARTIAL") {
      exactMatchFlag = false;
   }
   else {
      throw CommandException("\""
                             + modeName 
                             + " is not a valid mode.");
   }
   
   //
   // Check files
   //
   if (outputColorFileName.isEmpty()) {
      throw CommandException("Output Color File Name is empty.");
   }
   if (dataFileName.isEmpty()) {
      throw CommandException("Data File Name is empty.");
   }
   
   std::set<QString> names;
   
   //
   // Read the data file to get the names
   //
   if (dataFileName.endsWith(SpecFile::getBorderProjectionFileExtension())) {
      BorderProjectionFile bpf;
      bpf.readFile(dataFileName);
      const int numBorders = bpf.getNumberOfBorderProjections();
      for (int i = 0; i < numBorders; i++) {
         const BorderProjection* bp = bpf.getBorderProjection(i);
         names.insert(bp->getName());
      }
   }
   else if (dataFileName.endsWith(SpecFile::getBorderFileExtension())) {
      BorderFile bf;
      bf.readFile(dataFileName);
      const int numBorders = bf.getNumberOfBorders();
      for (int i = 0; i < numBorders; i++) {
         const Border* bp = bf.getBorder(i);
         names.insert(bp->getName());
      }
   }
   else if (dataFileName.endsWith(SpecFile::getCellFileExtension())) {
      CellFile cf;
      cf.readFile(dataFileName);
      const int numCells = cf.getNumberOfCells();
      for (int i = 0; i < numCells; i++) {
         const CellData* cd = cf.getCell(i);
         names.insert(cd->getName());
      }
   }
   else if (dataFileName.endsWith(SpecFile::getCellProjectionFileExtension())) {
      CellProjectionFile cpf;
      cpf.readFile(dataFileName);
      const int numCells = cpf.getNumberOfCellProjections();
      for (int i = 0; i < numCells; i++) {
         const CellProjection* cp = cpf.getCellProjection(i);
         names.insert(cp->getName());
      }
   }
   else if(dataFileName.endsWith(SpecFile::getFociProjectionFileExtension())) {
      FociProjectionFile fpf;
      fpf.readFile(dataFileName);
      const int numCells = fpf.getNumberOfCellProjections();
      for (int i = 0; i < numCells; i++) {
         const CellProjection* cp = fpf.getCellProjection(i);
         names.insert(cp->getName());
      }
   }
   else if(dataFileName.endsWith(SpecFile::getFociFileExtension())) {
      FociFile ff;
      ff.readFile(dataFileName);
      const int numCells = ff.getNumberOfCells();
      for (int i = 0; i < numCells; i++) {
         const CellData* cd = ff.getCell(i);
         names.insert(cd->getName());
      }
   }
   else if (dataFileName.endsWith(SpecFile::getPaintFileExtension())) {
      PaintFile pf;
      pf.readFile(dataFileName);
      const int num = pf.getNumberOfPaintNames();
      for (int i = 0; i < num; i++) {
         names.insert(pf.getPaintNameFromIndex(i));
      }
   }
   else if ((dataFileName.endsWith(SpecFile::getAnalyzeVolumeFileExtension())) ||
            (dataFileName.endsWith(SpecFile::getAfniVolumeFileExtension())) ||
            (dataFileName.endsWith(SpecFile::getWustlVolumeFileExtension())) ||
            (dataFileName.endsWith(SpecFile::getNiftiVolumeFileExtension())) ||
            (dataFileName.endsWith(SpecFile::getNiftiGzipVolumeFileExtension()))) {
      VolumeFile vf;
      vf.readFile(dataFileName);
      const int num = vf.getNumberOfRegionNames();
      for (int i = 0; i < num; i++) {
         names.insert(vf.getRegionNameFromIndex(i));
      }
   }
   else {
      throw CommandException(FileUtilities::basename(dataFileName)
                             + " does not end with a file extension matching "
                               " a supported data file type.");
   }
   
   //
   // Read the color file
   //
   ColorFile* colorFile = NULL;
   if (inputColorFileName.isEmpty() == false) {
      if (QFile::exists(inputColorFileName)) {
         if (inputColorFileName.endsWith(SpecFile::getAreaColorFileExtension())) {
            colorFile = new AreaColorFile;
         }
         else if (inputColorFileName.endsWith(SpecFile::getBorderColorFileExtension())) {
            colorFile = new BorderColorFile;
         }
         else if (inputColorFileName.endsWith(SpecFile::getCellColorFileExtension())) {
            colorFile = new CellColorFile;
         }
         else if (inputColorFileName.endsWith(SpecFile::getFociColorFileExtension())) {
            colorFile = new FociColorFile;
         }
         else {
            throw CommandException(FileUtilities::basename(inputColorFileName)
                                   + " does not end with a file extension matching "
                                     " a supported color file type.");
         }
         
         colorFile->readFile(inputColorFileName);
      }
   }
   
   //
   // Create color file if needed
   //
   if (colorFile == NULL) {
      if (outputColorFileName.endsWith(SpecFile::getAreaColorFileExtension())) {
         colorFile = new AreaColorFile;
      }
      else if (outputColorFileName.endsWith(SpecFile::getBorderColorFileExtension())) {
         colorFile = new BorderColorFile;
      }
      else if (outputColorFileName.endsWith(SpecFile::getCellColorFileExtension())) {
         colorFile = new CellColorFile;
      }
      else if (outputColorFileName.endsWith(SpecFile::getFociColorFileExtension())) {
         colorFile = new FociColorFile;
      }
      else {
         throw CommandException(FileUtilities::basename(outputColorFileName)
                                + " does not end with a file extension matching "
                                  " a supported color file type.");
      }
   }
   
   //
   // create the needed colors
   //
   const int numColors = colorFile->getNumberOfColors();
   std::vector<QString> nameVector(names.begin(), names.end());
   colorFile->generateColorsForNamesWithoutColors(nameVector, 
                                                  exactMatchFlag);
                                                  
   //
   // Write the color file
   //
   if (numColors != colorFile->getNumberOfColors()) {
      colorFile->writeFile(outputColorFileName);
   }
   else {
      std::cout << "No new colors were created, output color file not written." << std::endl;
   }
   
   delete colorFile;
}

      

