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

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

#include "CommandSpecFileDirectoryClean.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSpecFileDirectoryClean::CommandSpecFileDirectoryClean()
   : CommandBase("-spec-file-directory-clean",
                 "SPEC FILE DIRECTORY CLEAN")
{
}

/**
 * destructor.
 */
CommandSpecFileDirectoryClean::~CommandSpecFileDirectoryClean()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSpecFileDirectoryClean::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> operations;
   operations.push_back("DELETE");
   operations.push_back("LIST");
   operations.push_back("MOVE");

   paramsOut.clear();
   paramsOut.addListOfItems("Operation", operations, operations);
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandSpecFileDirectoryClean::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + " \n"
       + indent9 + "< DELETE | LIST | MOVE <directory-name> > \n"
       + indent9 + "\n"
       + indent9 + "This command first identifies all of the spec files in the\n"
       + indent9 + "current directory.  Next, all other files (that are not spec\n"
       + indent9 + "files) in the current directory are examined to see if the  \n"
       + indent9 + "file is listed in any of the spec files.  If a file is not\n"
       + indent9 + "listed in any of the spec files, one of the desired\n"
       + indent9 + "operations listed below is performed on the data file.\n"
       + indent9 + "\n"
       + indent9 + "OPERATIONS\n"
       + indent9 + "   \"DELETE\"   Any file not listed in a spec file is\n"
       + indent9 + "               deleted.\n"
       + indent9 + "   \"LIST\"     The name of any file not listed in a spec\n"
       + indent9 + "               file is printed.\n"
       + indent9 + "   \"MOVE\"     Any files not listed in the spec file are\n"
       + indent9 + "               moved to the specified directory.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSpecFileDirectoryClean::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   enum OPERATION {
      OPERATION_NONE,
      OPERATION_DELETE,
      OPERATION_LIST,
      OPERATION_MOVE
   };
   OPERATION operation = OPERATION_NONE;
   QString moveToDirectoryName;
   
   const QString operationString = parameters->getNextParameterAsString("Operation");
   if (operationString == "DELETE") {
      operation = OPERATION_DELETE;
   }
   else if (operationString == "LIST") {
      operation = OPERATION_LIST;
   }
   else if (operationString == "MOVE") {
      operation = OPERATION_MOVE;
      moveToDirectoryName = 
         parameters->getNextParameterAsString("Directory Name");
      QFileInfo dirInfo(moveToDirectoryName);
      if (dirInfo.exists() == false) {
         throw CommandException("Directory named \""
                                + moveToDirectoryName
                                + "\" does not exist.");
      }
      else if (dirInfo.isDir() == false) {
         throw CommandException("\"" 
                                +moveToDirectoryName
                                + "\" is not a directory.");
      }
   }
   else {
      throw CommandException("Invalid operation \"" + operationString + "\"");
   }
   checkForExcessiveParameters();
   
   //
   // Keep track of spec files non-spec files
   //
   std::vector<QString> specFileNames;
   std::vector<QString> dataFileNames;
   const QString specFileExtension(SpecFile::getSpecFileExtension());
   
   //
   // Get all spec files and data files in the current directory
   //
   const QString directoryName(QDir::currentPath());
   QDirIterator it(directoryName);
   while (it.hasNext()) {
      QFileInfo fi(it.next());
      const QString name(fi.fileName());
      if (fi.isFile() &&
          (fi.isSymLink() == false)) {
         if (name.endsWith(specFileExtension)) {
            specFileNames.push_back(name);
         }
         else {
            dataFileNames.push_back(name);
         }
      }
   }
    
   //
   // Verify spec files exist
   //
   const int numSpecFiles = static_cast<int>(specFileNames.size());
   if (numSpecFiles <= 0) {
      std::cout << "There are no spec files in the directory named \""
                << directoryName.toAscii().constData()
                << "\""
                << std::endl;
      return;
   }

   //
   // Get all of the files listed in the spec files
   //
   std::set<QString> specFileDataFiles;
   for (int i = 0; i < numSpecFiles; i++) {
      //
      // Read the spec file
      //
      SpecFile sf;
      sf.readFile(specFileNames[i]);

      //
      // Get all files in the spec file
      //
      std::vector<QString> allFiles;
      sf.getAllDataFilesInSpecFile(allFiles, true);
      specFileDataFiles.insert(allFiles.begin(), allFiles.end());
   }
   
   for (std::set<QString>::iterator iter = specFileDataFiles.begin();
        iter != specFileDataFiles.end();
        iter++) {
      const QString name(*iter);
   }
   
   //
   // Determine files that should be processed
   //
   const int numFiles = static_cast<int>(dataFileNames.size());
   for (int i = 0; i < numFiles; i++) {
      const QString name(dataFileNames[i]);
      if (seeIfFileIsInSpecFile(specFileDataFiles, name) == false) {
         switch (operation) {                   
            case OPERATION_NONE:
               break;
            case OPERATION_DELETE:
               if (QFile::remove(name) == false) {
                  std::cout << "ERROR: Unable to delete "
                            << name.toAscii().constData()
                            << std::endl;
               }
               break;
            case OPERATION_LIST:
               std::cout << name.toAscii().constData() << std::endl;
               break;
            case OPERATION_MOVE:
               {
                  const QString newName = (moveToDirectoryName
                                           + "/"
                                           + name);
                  if (QFile::rename(name, newName) == false) {
                     std::cout << "ERROR: Unable to rename "
                               << name.toAscii().constData()
                               << " to "
                               << newName.toAscii().constData()
                               << std::endl;
                  }
                  const QFile::Permissions permissions = 
                     AbstractFile::getFileWritePermissions();
                  if (permissions != 0) {
                     QFile::setPermissions(newName, 
                                           permissions);
                  }
               }
               break;
         }
      }
   }
}

/**
 * see if file is in a spec file (special case for volume data files).
 */
bool 
CommandSpecFileDirectoryClean::seeIfFileIsInSpecFile(
                              const std::set<QString>& dataFilesInSpecFile,
                              const QString& name) const
{
   const bool FILE_IS_IN_SPEC_FILE_FLAG = true;
   const bool FILE_IS_NOT_IN_SPEC_FILE_FLAG = false;
   
   //
   // File in spec file ?
   //
   if (dataFilesInSpecFile.find(name) != dataFilesInSpecFile.end()) {
      //
      // file is in the spec file
      //
      return FILE_IS_IN_SPEC_FILE_FLAG;
   }

   //
   // Some volume's are stored a pair of files with the "header" in one file
   // and the "data" in another file.  Plus, some spec files only contain
   // the header.  So, if a volume data file is found that is not listed in
   // a spec file, see if the corresponding header is in a spec file, and, 
   // if so, indicate that the data file is in a spec file.
   //   
   
   
   const QString afniDataExt(".BRIK");
   const QString afniDataZipExt(".BRIK.gz");
   const QString analyzeSpmWuNilDataExt(".img");
   const QString analyzeSpmWuNilDataZipExt(".img.gz");
   
   QString headerFileName;
   if (name.endsWith(afniDataExt) ||
       name.endsWith(afniDataZipExt)) {
      QString temp = name;
      if (name.endsWith(afniDataZipExt)) {
         temp.chop(afniDataZipExt.length());
      }
      else {
         temp.chop(afniDataExt.length());
      }
      temp.append(".HEAD");
      if (QFile::exists(temp)) {
         headerFileName = temp;
      }
   }
   else if (name.endsWith(analyzeSpmWuNilDataExt) ||
            name.endsWith(analyzeSpmWuNilDataZipExt)) {
      QString temp = name;
      if (name.endsWith(analyzeSpmWuNilDataZipExt)) {
         temp.chop(analyzeSpmWuNilDataZipExt.length());
      }
      else {
         temp.chop(analyzeSpmWuNilDataExt.length());
      }
      temp.append(".ifh");
      if (QFile::exists(temp)) {
         headerFileName = temp;
      }
      else {
         QString temp = name;
         if (name.endsWith(analyzeSpmWuNilDataZipExt)) {
            temp.chop(analyzeSpmWuNilDataZipExt.length());
         }
         else {
            temp.chop(analyzeSpmWuNilDataExt.length());
         }
         temp.append(".hdr");
         if (QFile::exists(temp)) {
            headerFileName = temp;
         }
      }
   }
   
   //
   // If header exists, do not delete data file
   //
   if (headerFileName.isEmpty() == false) {
      if (dataFilesInSpecFile.find(headerFileName) != dataFilesInSpecFile.end()) {
         return FILE_IS_IN_SPEC_FILE_FLAG;
      }
   }
   
   return FILE_IS_NOT_IN_SPEC_FILE_FLAG;
}


      

