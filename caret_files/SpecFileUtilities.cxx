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


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGlobalStatic>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "SceneFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "VolumeFile.h"
#define __SPEC_FILE_UTILITIES_MAIN__
#include "SpecFileUtilities.h"
#undef __SPEC_FILE_UTILITIES_MAIN__

/**
 * Copy a spec file.  Return false if no errors occur.
 */
bool
SpecFileUtilities::copySpecFile(const QString& sourceSpecFileIn,
                                const QString& targetSpecFileIn,
                                const MODE_COPY_SPEC_FILE copySpecFileModeIn,
                                QString& errorMessage,
                                const bool oneMustBeCurrentDirectory,
                                const bool verboseMode)
{
   errorMessage = "";
   
   copySpecFileMode = copySpecFileModeIn;
   if (copySpecFileMode == MODE_COPY_SPEC_FILE_NONE) {
      errorMessage = "ERROR: Invalid copy mode";
      return true;
   }
 
   QString sourceSpecFile(sourceSpecFileIn);
   QString targetSpecFile(targetSpecFileIn);
   
   const QString sourceName(FileUtilities::basename(sourceSpecFile));
   copySpecSourcePath = FileUtilities::dirname(sourceSpecFile);
   bool sourceIsCurrentDirectory = false;
   if (copySpecSourcePath.compare(".") == 0) {
      sourceIsCurrentDirectory = true;
      copySpecSourcePath = QDir::currentPath();
   }
   
   QString targetName(FileUtilities::basename(targetSpecFile));
   copySpecTargetPath = FileUtilities::dirname(targetSpecFile);
   
   QFileInfo targetInfo(targetSpecFile);
   if (targetInfo.isDir()) {
      targetName = "";
      copySpecTargetPath = targetSpecFile;
   }
   
   if (targetName.length() == 0) {
      targetName = sourceName;
   }
   
   //
   // Can both be relative path, NOT
   //
   if (oneMustBeCurrentDirectory) {
      if ((sourceIsCurrentDirectory == false) &&
          (copySpecTargetPath.compare(".") != 0)) {
         errorMessage = "ERROR: Either the source or target spec file must be in the " 
                             "current directory.";
         return true;
      }
   }
      
   //cout << "source-spec-file: " << sourceSpecFile << endl;
   //cout << "source path: " << sourcePath << endl;
   //cout << "source name: " << sourceName << endl;
   
   //cout << "target-spec-file: " << targetSpecFile << endl;
   //cout << "target path: " << targetPath << endl;
   //cout << "target name: " << targetName << endl;
   
   SpecFile sf;
   try {
      sf.readFile(sourceSpecFile);
   }
   catch (FileException& e) {
      errorMessage.append("ERROR reading: ");
      errorMessage.append(sourceSpecFile);
      errorMessage.append(" ");
      errorMessage.append(e.whatQString());
      errorMessage.append("\n");
      return true;
   }
  
   //
   // process all files except scene
   // 
   for (unsigned int i = 0; i < sf.allEntries.size(); i++) {
      if (sf.allEntries[i] != &sf.sceneFile) {
         copySpecFileDataFiles(*(sf.allEntries[i]), verboseMode, errorMessage);
      }
   }
   
   //
   // Because the scene file contains paths, it MUST ALWAYS be copied
   //
   const MODE_COPY_SPEC_FILE savedCopyMode = copySpecFileMode;
   copySpecFileMode = MODE_COPY_SPEC_FILE_COPY_ALL;
   copySpecFileDataFiles(sf.sceneFile, verboseMode, errorMessage);
   copySpecFileMode = savedCopyMode;
   
   //
   // May need to alter paths in scene files
   //
   if (sf.sceneFile.files.empty() == false) {
      QString savedPath = QDir::currentPath();
      QDir::setCurrent(copySpecTargetPath);
      
      try {
         switch (copySpecFileMode) {
            case MODE_COPY_SPEC_FILE_NONE:
               break;
            case MODE_COPY_SPEC_FILE_COPY_ALL:
            case MODE_COPY_SPEC_FILE_MOVE_ALL:
               for (unsigned int i = 0; i < sf.sceneFile.files.size(); i++) {
                  SceneFile::removePathsFromAllSpecFileDataFileNames(sf.sceneFile.files[i].filename);
               }
               break;
            case MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES:
               for (unsigned int i = 0; i < sf.sceneFile.files.size(); i++) {
                  SceneFile::addPathToAllSpecFileDataFileNames(sf.sceneFile.files[i].filename,
                                                               copySpecSourcePath);
               }
               break;
         }
      }
      catch (FileException& e) {
         std::cout << "ERROR updating scene file " << e.whatQString().toAscii().constData() << std::endl;
      }
      
      if (savedPath.isEmpty() == false) {
         QDir::setCurrent(savedPath);
      }
   }
   
   QString outputName(copySpecTargetPath);
   if (outputName[outputName.length() - 1] != '/') {
      outputName.append("/");
   }
   outputName.append(targetName);
   if (DebugControl::getDebugOn()) {
      std::cout << "INFO: Writing Spec File: " << outputName.toAscii().constData() << std::endl;
   }
   try {
      sf.writeFile(outputName);
   }
   catch (FileException& e) {
      errorMessage.append("ERROR: writing spec file named: ");
      errorMessage.append(targetName);
      errorMessage.append(" ");
      errorMessage.append(e.whatQString());
      return true;
   }
   if (copySpecFileMode == MODE_COPY_SPEC_FILE_MOVE_ALL) {
      QFile file(sourceSpecFile);
      file.remove();
   }
   
   if (errorMessage.isEmpty() == false) {
      return true;
   }
   
   return false;
}

/**
 * Copy a group os spec file data files
 */
void 
SpecFileUtilities::copySpecFileDataFiles(SpecFile::Entry& fileEntry,
                                         const bool verboseMode,
                                         QString& errorMessageOut) 
{
   for (unsigned int i = 0; i < fileEntry.files.size(); i++) {
      copySpecFileCopyDataFile(fileEntry.files[i].filename,
                               fileEntry.files[i].dataFileName,
                               fileEntry.fileType,
                               verboseMode, errorMessageOut);
   }
}

/**
 * Copy a spec file's data file.
 */
void 
SpecFileUtilities::copySpecFileCopyDataFile(QString& fileName,
                                            const QString& dataFileNameIn,
                                            const SpecFile::Entry::FILE_TYPE ft,
                                            const bool verboseMode,
                                            QString& errorMessageOut) 
{
   QString dataFileName = dataFileNameIn;
   if (fileName.isEmpty() == false) {
      QFileInfo fileInfo(fileName);

      //
      // See if file is NOT an absolute path and if not prepend source directory
      //
      QString inputName;
      if (fileInfo.isRelative()) {
         inputName = copySpecSourcePath;
         if (inputName.isEmpty() == false) {
            if (fileName[inputName.length() - 1] != '/') {
               inputName.append("/");
            }
         }
      }
      inputName.append(fileName);
      //cout << "Input  File: " << inputName << endl;

      if (copySpecFileMode == MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES) {
         fileName = inputName;
         return;
      }
      
      QString outputName(copySpecTargetPath);
      if (outputName.isEmpty() == false) {
         if (outputName[outputName.length() - 1] != '/') {
            outputName.append("/");
         }
      }
      outputName.append(FileUtilities::basename(fileName));
      
      const bool moveFileFlag = (copySpecFileMode == MODE_COPY_SPEC_FILE_MOVE_ALL);
      if (FileUtilities::copyFile(inputName, outputName, moveFileFlag, verboseMode)) {
         errorMessageOut.append("ERROR processing: ");
         errorMessageOut.append(FileUtilities::basename(inputName));
         errorMessageOut.append("\n");
         return;
      }
      
      
      switch (ft) {
         case SpecFile::Entry::FILE_TYPE_SURFACE:
            break;
         case SpecFile::Entry::FILE_TYPE_VOLUME:
            {
               //
               // Skip single file volume files
               //
               if ((inputName.right(4) == SpecFile::getNiftiVolumeFileExtension()) ||
                   (inputName.right(7) == SpecFile::getNiftiGzipVolumeFileExtension()) ||
                   (inputName.right(5) == SpecFile::getMincVolumeFileExtension())) {
                  // do nothing
               }
               else {
                  try {
                     if (dataFileName.isEmpty()) {
                        //
                        // Read in the volume' header
                        //
                        VolumeFile vf;
                        vf.readFile(fileName,
                                    VolumeFile::VOLUME_READ_HEADER_ONLY);
                        dataFileName = vf.getDataFileName();
                     }
                     
                     //
                     // Determine output file.
                     //
                     QString outputVolumeName(copySpecTargetPath);
                     if (outputVolumeName.isEmpty() == false) {
                        if (outputVolumeName[outputVolumeName.length() - 1] != '/') {
                           outputVolumeName.append("/");
                        }
                     }
                     outputVolumeName.append(FileUtilities::basename(dataFileName));
                     
                     //
                     // Determine input file
                     //
                     QString inputVolumeName(FileUtilities::dirname(inputName));
                     if (inputVolumeName.isEmpty() == false) {
                        if (inputVolumeName[inputVolumeName.length() - 1] != '/') {
                           inputVolumeName.append("/");
                        }
                     }
                     inputVolumeName.append(FileUtilities::basename(dataFileName));
                     
                     //
                     // See if the data does not file exist
                     //
                     if (QFile::exists(inputVolumeName) == false) {
                        //
                        // See if the gzipped version exists
                        //
                        QString temp(inputVolumeName);
                        temp.append(".gz");
                        if (QFile::exists(temp)) {
                           inputVolumeName.append(".gz");
                           outputVolumeName.append(".gz");
                        }
                     }
                     
                     //
                     // Process the volume's data file.
                     //
                     if (FileUtilities::copyFile(inputVolumeName, outputVolumeName, 
                                                 moveFileFlag, verboseMode)) {
                        //std::cout << "ERROR processing: " << inputVolumeName
                        //            << " with and without \".gz\" extension." << std::endl;
                        return;
                     }
                  }
                  catch (FileException& e) {
                     errorMessageOut.append("ERROR: Reading ");
                     errorMessageOut.append(FileUtilities::basename(inputName));
                     errorMessageOut.append(" to determine the volume's data file name.");
                     errorMessageOut.append("\n");
                  }
               }
            }
            break;
         case SpecFile::Entry::FILE_TYPE_OTHER:
            break;
      }
            
      //
      // Chop any path off the data file since it is copied to the same directory
      // as the new spec file.
      //
      fileName = FileUtilities::basename(fileName); 
   }     
}

//----------------------------------------------------------------------------------------

/**
 * set the volume data file name if it is not already set.
 */
void 
SpecFileUtilities::setVolumeDataFileName(const QString& directoryName,
                                         const QString& headerFileName,
                                         QString& dataFileName)
{
   if (dataFileName.isEmpty()) {
      const QString savedDirectory = QDir::currentPath();
      if (directoryName.isEmpty() == false) {
         QDir::setCurrent(directoryName);
      }
      
      //
      // Read in the volume' header
      //
      VolumeFile vf;
      try {
         vf.readFile(headerFileName,
                     VolumeFile::VOLUME_READ_HEADER_ONLY);
      }
      catch (FileException&) {
         if (DebugControl::getDebugOn()) {
            std::cout << "INFO: Unable to read " << headerFileName.toAscii().constData()
                      << " while trying to determine data file name." << std::endl;
         }
         QDir::setCurrent(savedDirectory);
         return;
      }
      
      //
      // Determine the name of volume's data file.
      //
      QString outputVolumeName(FileUtilities::dirname(headerFileName));
      if (outputVolumeName.isEmpty() == false) {
         if (outputVolumeName == ".") {
            outputVolumeName = "";
         }
         else {
            outputVolumeName.append("/");
         }
      }
      QString inputVolumeName(vf.getDataFileName());
      outputVolumeName.append(FileUtilities::basename(inputVolumeName));
      
      //
      // See if the data does not file exist
      //
      if (QFile::exists(inputVolumeName) == false) {
         //
         // See if the gzipped version exists
         //
         QString temp(inputVolumeName);
         temp.append(".gz");
         if (QFile::exists(temp)) {
            outputVolumeName.append(".gz");
         }
      }
      
      //
      // Only add the volume data file name if it exists
      //
      if (QFile::exists(outputVolumeName)) {
         dataFileName = outputVolumeName;
      }
      
      QDir::setCurrent(savedDirectory);
   }
}
                                 
/**
 * find spec files in specified directory.
 */
void 
SpecFileUtilities::findSpecFilesInDirectory(const QString& directory,
                                            std::vector<QString>& files)
{
   FileUtilities::findFilesInDirectory(directory, QStringList("*.spec"), files);
}

/**
 * zip a spec file (returns true if an error occurred).
 */
bool 
SpecFileUtilities::zipSpecFile(const QString& specFileToZipIn,
                               const QString& zipFileNameIn,
                               const QString& unzipDirName,
                               QString& errorMessage,
                               const QString& zipCommandIn,
                               const QString& temporaryDirectoryIn)
{
   errorMessage = "";
   
   try {
      SpecFile sf;
      sf.readFile(specFileToZipIn);
      QString msg1;
      if (sf.validate(msg1) == false) {
         QString msg("Missing data files so unable to zip ");
         msg.append(FileUtilities::basename(specFileToZipIn));
         msg.append(":\n");
         msg.append(msg1);
         errorMessage = msg;
         return true;
      }
   }
   catch (FileException&) {
      QString msg("Unable to read: ");
      msg.append(FileUtilities::basename(specFileToZipIn));
      errorMessage = msg;
      return true;
   }
   
   QString specFileToZip(specFileToZipIn);
   if (specFileToZip.isEmpty()) {
      errorMessage = "spec file name is isEmpty.";
      return true;
   }
   QString zipFileName(zipFileNameIn);
   if (zipFileName.isEmpty()) {
      errorMessage = "zip file name is isEmpty.";
      return true;
   }
   if (unzipDirName.isEmpty()) {
      errorMessage = "unzip directory name is isEmpty.";
      return true;
   }
   
   //
   // Prepend path to spec file (if needed)
   //
   if (QDir::isRelativePath(specFileToZip)) {
      QString s(QDir::currentPath());
      s.append("/");
      s.append(specFileToZip);
      specFileToZip = s;
   }
   
   //
   // Add zip file extension
   //
   if (StringUtilities::endsWith(StringUtilities::makeLowerCase(zipFileName),
                                 ".zip") == false) {
      zipFileName.append(".zip");
   }
   
   //
   // Prepend path to zip file (if needed)
   //
   if (QDir::isRelativePath(zipFileName)) {
      QString s(QDir::currentPath());
      s.append("/");
      s.append(zipFileName);
      zipFileName = s;
   }
   
   //
   // Create zip command (if needed)
   //
   QString zipCommand(zipCommandIn);
   if (zipCommand.isEmpty()) {
#ifdef Q_OS_WIN32
      zipCommand = "caret_zip -r";
#else  // Q_OS_WIN32
      zipCommand = "zip -r";
#endif // Q_OS_WIN32
   }
   
   //
   // Create temporary directory name (if needed)
   //
   QString temporaryDirectory(temporaryDirectoryIn);
   if (temporaryDirectory.isEmpty()) {
      temporaryDirectory = FileUtilities::temporaryDirectory();
      if (temporaryDirectory.isEmpty()) {
         errorMessage = "Do not know name of temorary directory.";
         return true;
      }
   }
   
   //
   // Save the current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Change to the temporary directory
   //
   if (DebugControl::getDebugOn()) {
      std::cout << "ZIP-SPEC: Changing to the temporary directory named: "
                << temporaryDirectory.toAscii().constData()
                << std::endl;
   }
   QDir::setCurrent(temporaryDirectory);
   if (DebugControl::getDebugOn()) {
      std::cout << "ZIP-SPEC: current directory is: "
                << QDir::currentPath().toAscii().constData()
                << std::endl;
   }
   
   //
   // Create the unzip subdirectory specified by user
   //
   if (FileUtilities::createDirectory(unzipDirName) == false) {
      errorMessage = "ERROR: Unable to create the unzip directory: ";
      errorMessage.append(unzipDirName);
      errorMessage.append("   You may need to delete the directory \"");
      errorMessage.append(unzipDirName);
      errorMessage.append("\" in ");
      errorMessage.append(QDir::currentPath());
      errorMessage.append("\n");
      QDir::setCurrent(savedDirectory);
      return true;
   }
   if (DebugControl::getDebugOn()) {
      std::cout << "ZIP-SPEC: Creating unzip directory: " 
                << unzipDirName.toAscii().constData() << std::endl;
   }
   
   //
   // Change to subdirectory specified by user
   //
   QDir::setCurrent(unzipDirName);
   if (DebugControl::getDebugOn()) {
      std::cout << "ZIP-SPEC: current directory is: "
                << QDir::currentPath().toAscii().constData()
                << std::endl;
   }
   
   //
   // Copy the spec file and its data files
   //
   if (DebugControl::getDebugOn()) {
      std::cout << "ZIP-SPEC: Copying the spec file " 
                << specFileToZip.toAscii().constData() << std::endl;
   }
   if (SpecFileUtilities::copySpecFile(specFileToZip,
                                       FileUtilities::basename(specFileToZip),
                                       SpecFileUtilities::MODE_COPY_SPEC_FILE_COPY_ALL,
                                       errorMessage,
                                       false,
                                       true)) {
      QDir::setCurrent(savedDirectory);
      return true;
   }
                                   
   
   //
   // Go up one directory
   //
   QDir::setCurrent("..");
   
   //
   // Zip the subdirectory into the zip file
   //
   if (DebugControl::getDebugOn()) {
      std::cout << "ZIP-SPEC: creating zip file " 
                << FileUtilities::basename(zipFileName).toAscii().constData() << std::endl;
   }
   QString command(zipCommand);
   command.append(" ");
   command.append(FileUtilities::basename(zipFileName));
   command.append(" ");
   command.append(unzipDirName);
   command.append("/*");
   const int result1 = std::system(command.toAscii().constData());
   const int result = ((result1 >> 8) & 0xff);
   if ((result != 0) || (result1 != 0)) {
      std::ostringstream str;
      str << "Execution of command "
          << command.toAscii().constData()
          << " failed with status: "
          << result
          << " and " 
          << result1;
      errorMessage = str.str().c_str();
      return true;
   }
   
   //
   // Move the zip file to the desired location
   //
   FileUtilities::copyFile(FileUtilities::basename(zipFileName),
                           zipFileName,
                           true,
                           true);
                           
   //
   // Remove the directory that was created and the files within it
   //
   QDir::setCurrent(unzipDirName);
   QDir unzipDir;
   for (unsigned int i = 0; i < unzipDir.count(); i++) {
      QFile::remove(unzipDir[i]);
   }
   QDir::setCurrent("..");
   QDir d1;
   d1.rmdir(unzipDirName);
   
   //
   // Return to the original directory
   //
   QDir::setCurrent(savedDirectory);
   
   return false;
}
      

