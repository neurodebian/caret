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
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stack>

#include <QGlobalStatic>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>

#include "Basename.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "StringUtilities.h"

#ifdef CARET_FLAG
#include "HttpFileDownload.h"
#include "zlib.h"
#endif // CARET_FLAG

/**
 * Find files in the directory that match the regular expression.
 * To match more than one file type, place a semi-colon between
 * the regular expressions.
 */
void
FileUtilities::findFilesInDirectory(const QString& dirName, 
                                    const QStringList& regExpMatchList,
                                    std::vector<QString>& matchingFiles)
{
   QDir dir(dirName);
   QStringList files = dir.entryList(regExpMatchList,
                                     QDir::Files | QDir::NoSymLinks);
   
   matchingFiles.clear();
   for (QStringList::Iterator it = files.begin(); it != files.end(); it++) {
      matchingFiles.push_back((*it));
   }
}

/**
 * like "basename" for a C-string
 */
QString
FileUtilities::basename(const QString& s)
{
   char *name = new char[s.length() + 1];
   strcpy(name, s.toAscii().constData());
   QString sout(Basename(name));
   delete[] name;
   return sout;
}

/**
 * like "dirname" for a C-string
 */
QString
FileUtilities::dirname(const QString& s)
{
   char *name = new char[s.length() + 1];
   strcpy(name, s.toAscii().constData());
   QString sout(Dirname(name));
   delete[] name;
   return sout;
}

/**
 * Return the filename's extension
 */
QString
FileUtilities::filenameExtension(const QString& s)
{
   QString ext;
   
   const int period = s.lastIndexOf('.');
   if (period >= 0) {
      ext = s.mid(period + 1);
   }
   return ext;
}

/**
 * Return the filename without the extension
 */
QString
FileUtilities::filenameWithoutExtension(const QString& s)
{
   QString name(s);
   
   const int period = s.lastIndexOf('.');
   if (period >= 0) {
      name = s.mid(0, period);
   }
   return name;
}

/** 
 * Rearrange a file name so that /usr/local/file.data becomes
 * file.data (/usr/local).
 */
QString
FileUtilities::rearrangeFileName(const QString& fileName,
                                 const int maxPathLength)
{
   QString name(basename(fileName));
   QString path(dirname(fileName));
   if ((path != ".") && (maxPathLength >= 0)) {
      name.append(" (");
      const int pathLength = static_cast<int>(path.length());
      if (pathLength > maxPathLength) {
         name.append("...");
         const int offset = pathLength - maxPathLength;
         name.append(path.mid(offset));
      }
      else {
         name.append(path);
      }
      name.append(")");
   } 
   return name;
}

/**
 * Given the directory "mypath", determine the relative path to "otherpath".
 *
 * Examples:
 *    mypath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM"
 *    otherpath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS"
 *    result - "..";
 *         
 *    mypath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM"
 *    otherpath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM/subdir"
 *    result - "subdir";
 *         
 *    mypath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM"
 *    otherpath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/LEFT_HEM/subdir"
 *    result - "../LEFT_HEM/subdir";
 */
void
FileUtilities::relativePath(const QString& otherPathIn,
                   const QString& myPathIn,
                   QString& result)
{
   result = otherPathIn;
   
   //
   // Check for either path being empty
   //
   if (otherPathIn.isEmpty() || myPathIn.isEmpty()) {
       return;
   }
   
#ifdef Q_OS_WIN32
   //
   // Both paths must be absolute paths
   //
   if (otherPathIn.indexOf(":") < 0) {
      return;
   }
   if (myPathIn.indexOf(":") < 0) {
      return;
   }
#else
   //
   // Both paths must be absolute paths
   //
   if ((otherPathIn[0] != '/') || (myPathIn[0] != '/')) {
      return;
   }
#endif
   
   std::vector<QString> otherPath;
   StringUtilities::token(QDir::cleanPath(otherPathIn),
               "/\\",
               otherPath);
               
   std::vector<QString> myPath;
   StringUtilities::token(QDir::cleanPath(myPathIn),
               "/\\",
               myPath);
   
   const unsigned int minLength = std::min(myPath.size(), otherPath.size());
   
   unsigned int sameCount = 0;
   for (unsigned int i = 0; i < minLength; i++) {
      if (myPath[i] == otherPath[i]) {
         //cout << "Match: |" << myPath[i] << "|" << std::endl;
         sameCount++;
      }
      else {
         break;
      }
   }
   //cout << "same count: " << sameCount << std::endl;
   
   //
   // Is root of both paths different
   //
   if (sameCount == 0) {
       result = otherPathIn;
   }
   
   //const char separator[2] = { QDir::separator(), '\0' };
   
   //
   // Is other path a subdirectory of mypath
   //
   if (sameCount == myPath.size()) {
      result = "";
      for (unsigned int j = sameCount; j < otherPath.size(); j++) {
         result.append(otherPath[j]);
         if (j < (otherPath.size() - 1)) {
            result.append(QDir::separator());
         }
      }
   }
   
   //
   // otherpath is above this one
   //
   result = "";
   for (unsigned int j = sameCount; j < myPath.size(); j++) {
      result.append("..");
      if (j < (myPath.size() - 1)) {
         result.append(QDir::separator());
      }
   }
   for (unsigned int k = sameCount; k < otherPath.size(); k++) {
      if (result.isEmpty() == false) {
         result.append(QDir::separator());
      }
      result.append(otherPath[k]);
   }   
}

/**
 * Copy the file "inputName" to the file "outputName".  If
 * "moveFileFlag" is set, inputName will be deleted after
 * outputName has been written.  Returns true if an error
 * occurs.
 */
bool
FileUtilities::copyFile(const QString& inputName,
                        const QString& outputName,
                        const bool moveFileFlag,
                        const bool verboseMode)
{
   //
   // Make sure source file exists
   //
   QFileInfo srcInfo(inputName);
   if (srcInfo.exists() == false) {
      std::cerr << "ERROR: Unable to find source file " << inputName.toAscii().constData() << std::endl;
      return true;
   }
   
   //
   // If destination does not exist that is okay
   //
   QFileInfo destInfo(outputName);
   if (destInfo.exists() == false) {
   
      //
      // If destination is a symbolic link, remove it
      //
      if (destInfo.isSymLink()) {
         std::cout << "INFO: Destination " << outputName.toAscii().constData() << " is a symbolic link, "
                   << "deleting it." << std::endl;
         QFile file(outputName);
            if (file.remove() == false) {
            std::cout << "ERROR: Unable to delete symbolic link " 
                      << qPrintable(outputName) << std::endl;
            return true;
         }
      }

      //
      // Make sure that the source and destination are not the same file
      //
      if (srcInfo.absolutePath() == destInfo.absolutePath()) {
         std::cout << "INFO: Source (" << inputName.toAscii().constData() << ") and Destination (" 
                   << outputName.toAscii().constData()
                   << ") files are the same!  Not copied." << std::endl;
         return true;
      }
   }

   QFileInfo inputFileInfo(inputName);
   QFileInfo outputFileInfo(outputName);
   if (inputFileInfo.absolutePath() == outputFileInfo.absolutePath()) {
      std::cerr << "ERROR: Trying to copy file " 
                << inputName.toAscii().constData()
                << " to itself." 
                << std::endl;
      return true;
   }
   
   std::ifstream in(inputName.toAscii().constData(), std::ios::in | std::ios::binary);
   if (!in) {
      std::cerr << "ERROR: unable to open for reading " 
                << inputName.toAscii().constData() << std::endl;
      return true;
   }

   if (verboseMode) {
      std::cout << "INFO: Writing File: " << outputName.toAscii().constData() << std::endl;
   }
   std::ofstream out(outputName.toAscii().constData(), std::ios::out | std::ios::binary);
   if (!out) {
      std::cerr << "ERROR: unable to open for writing " 
                << outputName.toAscii().constData() << std::endl;
      in.close();
      return true;
   }

   //
   // This next line will copy the entire contents of the file
   //
   out << in.rdbuf();

   in.close();
   out.close();
   
   if (moveFileFlag) {
      QFile file(inputName);
      file.remove();
   }
   
   return false;
}

static void
removeTrailingPeriod(QString& s)
{
   if (s.isEmpty()) return;
   
   if (StringUtilities::endsWith(s, ".")) {
      s = s.left(s.length() - 1);
   }
}

/**
 * Parse a data file name to see if it is a valid Caret file name.
 * Returns true if the file name is a valid Caret file name.
 */
bool
FileUtilities::parseCaretDataFileName(const QString& filenameIn,
                                      QString& directory,
                                      QString& species,
                                      QString& casename,
                                      QString& anatomy,
                                      QString& hemisphere,
                                      QString& description,
                                      QString& descriptionNoTypeName,
                                      QString& theDate,
                                      QString& numNodes,
                                      QString& extension)

{
   //
   // set if file's name is a valid Caret file name.
   //
   bool validName = false;
   
   //
   // Get the directory
   //
   directory = FileUtilities::dirname(filenameIn);
   if (directory == ".") {
      directory = "";
   }
   
   //
   // Chop off any path on the file's name
   //
   const QString filename(FileUtilities::basename(filenameIn));
 
   //
   // clear the filename's components.
   //
   species = "";
   casename = "";
   anatomy = "";
   hemisphere = "";
   description = "";
   descriptionNoTypeName = "";
   theDate = "";
   numNodes = "";
   extension = "";
      
   //
   // species is a string containing one or more letters, numbers, underscores, or dashes 
   // followed by a period.
   //
   const QString speciesRE("([\\w_\\-]+\\.)");   
   
   //
   // case is a string containing one or more letters, numbers, undersores, or dashes 
   // followed by a period.
   //
   const QString caseRE("([\\w_\\-]+\\.)");
   
   //
   // brain part is a string containing one or more letters, numbers, undersores, or dashes
   // followed by a period.  OPTIONAL.
   //
   const QString brainPartRE("([\\w_\\-]+\\.)?");
   
   //
   // hemisphere is L, R, or LR followed by a period.
   //
   const QString hemisphereRE("(LR|L|R|BOTH)\\.");
   //const QString hemisphereRE("([LR|L|R|BOTH]\\.)");
   
   //
   // description is a anything ending with a period.  OPTIONAL.
   //
   const QString descriptionRE("(.+\\.)?");

   //
   // Date is of the form YYYY-MM-DD, YYYY-MM, YY-MM-DD, or YY-MM
   // followed by a period.
   //
   const QString dateRE("(\\d{2,4}\\-\\d{2}(\\-\\d{2})?\\.)");
   
   //
   // number of nodes is one or more numbers.
   //
   const QString numNodesRE("(\\d+\\.)");
   
   //
   // extension  is a string containing one or more letters, numbers, undersores, or dashes 
   //
   const QString extensionRE("([\\w_\\-]+)");
   
   //
   // Loop through the regular expressions.
   //
   const int numberOfRegularExpresions = 7;
   for (int i = 0; i < numberOfRegularExpresions; i++) {
      //
      // Create the regular expression
      //
      QString re;
      
      switch (i) {
         case 0:  
            re = "^"
               + speciesRE 
               + caseRE 
               + brainPartRE
               + hemisphereRE 
               + descriptionRE
               + dateRE
               + numNodesRE
               + extensionRE 
               + "$";
             break;
         case 1:  
            re = "^"
               + speciesRE 
               + caseRE 
               + brainPartRE
               + hemisphereRE 
               + descriptionRE
               + numNodesRE
               + extensionRE 
               + "$";
            break;
         case 2:  
            re = "^"
               + caseRE 
               + hemisphereRE 
               + descriptionRE
               + dateRE
               + numNodesRE
               + extensionRE 
               + "$";
            break;
         case 3:  
            re = "^"
               + caseRE 
               + hemisphereRE 
               + descriptionRE
               + numNodesRE
               + extensionRE 
               + "$";
            break;
         case 4:
            re = "^"
               + caseRE 
               + numNodesRE
               + extensionRE 
               + "$";
            break;
         case 5:
            re = "^"
               + caseRE 
               + extensionRE 
               + "$";
            break;
         case 6:
            re = "^"
               + speciesRE
               + caseRE
               + hemisphereRE
               + descriptionRE
               + extensionRE
               + "$";
            break;
      }
            
      
      //
      // Create the regular expression object.
      //
      QRegExp regexp(re);
      if (regexp.isValid() == false) {
         std::cout << "Program Error: "
                  << qPrintable(re)
                  << " is an invalid regular expression."
                  << std::endl;
         return false;
      }
      
      if (DebugControl::getDebugOn()) {
         std::cout << "-------------------------------------------------------------" << std::endl;
         std::cout << "Testing iter " << i << ": " << qPrintable(filename) << std::endl;
      }
      
      if (regexp.indexIn(filename) >= 0) {
         const int numCaptures = regexp.numCaptures();
         if (DebugControl::getDebugOn()) {
            std::cout << "num captures: " << numCaptures << std::endl;
            for (int j = 1; j <= numCaptures; j++) {
               std::cout << "   cap " << j << ": " << regexp.cap(j).toAscii().constData() << std::endl;
            }
         }
         
         switch (i) {
            case 0:
               if (numCaptures == 9) {
                  species = regexp.cap(1);
                  casename = regexp.cap(2);
                  anatomy = regexp.cap(3);
                  hemisphere = regexp.cap(4);
                  description = regexp.cap(5);
                  theDate = regexp.cap(6);
                  numNodes = regexp.cap(8);
                  extension = regexp.cap(9);
                  validName = true;
               }
               break;
            case 1:
               if (numCaptures == 7) {
                  species = regexp.cap(1);
                  casename = regexp.cap(2);
                  anatomy = regexp.cap(3);
                  hemisphere = regexp.cap(4);
                  description = regexp.cap(5);
                  numNodes = regexp.cap(6);
                  extension = regexp.cap(7);
                  validName = true;
               }
               break;
            case 2:
               if (numCaptures == 7) {
                  species = regexp.cap(1);
                  hemisphere = regexp.cap(2);
                  description = regexp.cap(3);
                  theDate = regexp.cap(4);
                  numNodes = regexp.cap(6);
                  extension = regexp.cap(7);
                  validName = true;
               }
               break;
            case 3:
               if (numCaptures == 5) {
                  species = regexp.cap(1);
                  hemisphere = regexp.cap(2);
                  description = regexp.cap(3);
                  numNodes = regexp.cap(4);
                  extension = regexp.cap(5);
                  validName = true;
               }
               break;
            case 4:
               if (numCaptures == 3) {
                  species = regexp.cap(1);
                  numNodes = regexp.cap(2);
                  extension = regexp.cap(3);
                  validName = true;
               }
               break;
            case 5:
               if (numCaptures == 2) {
                  species = regexp.cap(1);
                  extension = regexp.cap(2);
                  validName = true;
               }
               break;
            case 6:
               if (numCaptures == 5) {
                  species = regexp.cap(1);
                  casename = regexp.cap(2);
                  hemisphere = regexp.cap(3);
                  description = regexp.cap(4);
                  extension = regexp.cap(5);
                  validName = true;
               }
               break;
         }
         
         if (validName) {
            break;
         }         
      }
   }
   
   if (validName) {
      //
      // Remove periods on end of filename components
      //
      removeTrailingPeriod(species);
      removeTrailingPeriod(casename);
      removeTrailingPeriod(anatomy);
      removeTrailingPeriod(hemisphere);
      removeTrailingPeriod(description);
      removeTrailingPeriod(theDate);
      removeTrailingPeriod(numNodes);
      
      if (description.isEmpty() == false) {
         descriptionNoTypeName = description;
         const QString descriptionLowerCase(StringUtilities::makeLowerCase(descriptionNoTypeName));
         
         //
         // Different types for coordinate files
         //
         std::vector<QString> typeNames;
         typeNames.push_back("initialflat");
         typeNames.push_back("flat");
         typeNames.push_back("lobar");
         typeNames.push_back("sphere");
         typeNames.push_back("ellipsoid");
         typeNames.push_back("inflated");
         typeNames.push_back("veryinflated");
         typeNames.push_back("very_inflated");
         typeNames.push_back("fiducial");
         typeNames.push_back("raw");
         
         //
         // Different types for topo files
         //
         typeNames.push_back("closed");
         typeNames.push_back("open");
         typeNames.push_back("cut");
         typeNames.push_back("lobar");
         
         const int numTypeNames = static_cast<int>(typeNames.size());
         for (int i = 0; i < numTypeNames; i++) {
            const int pos = descriptionLowerCase.indexOf(typeNames[i]);
            if (pos != -1) {
               descriptionNoTypeName.resize(pos);
               if (StringUtilities::endsWith(descriptionNoTypeName, ".")) {
                  descriptionNoTypeName.resize(descriptionNoTypeName.length() - 1);
               }
               break;
            }
         }
      }

      if (DebugControl::getDebugOn()) {
         std::cout << qPrintable(filename) << " is a valid file name" << std::endl;
   
         std::cout << "species:             " << species.toAscii().constData() << std::endl;
         std::cout << "case:                " << casename.toAscii().constData() << std::endl;
         std::cout << "anatomy:             " << anatomy.toAscii().constData() << std::endl;
         std::cout << "hemisphere:          " << hemisphere.toAscii().constData() << std::endl;
         std::cout << "description:         " << description.toAscii().constData() << std::endl;
         std::cout << "description no type: " << descriptionNoTypeName.toAscii().constData() << std::endl;
         std::cout << "date:                " << theDate.toAscii().constData() << std::endl;
         std::cout << "num nodes:           " << numNodes.toAscii().constData() << std::endl;
         std::cout << "extension:           " << extension.toAscii().constData() << std::endl;
      }
   }
   else {
      if (DebugControl::getDebugOn()) {
         std::cout << filename.toAscii().constData() << " is NOT a valid file name" << std::endl;
      }
   }
   
   //
   // Date no longer used
   //
   theDate = "";
   
   return validName;
}

/**
 * Reassemble the Caret data file name from the components.
 */
QString 
FileUtilities::reassembleCaretDataFileName(const QString& directory,
                                           const QString& species,
                                           const QString& casename,
                                           const QString& anatomy,
                                           const QString& hemisphere,
                                           const QString& description,
                                           const QString& theDateIn,
                                           const QString& numNodes,
                                           const QString& extension)
{
   //
   // Date is no longer used
   //
   QString theDate = theDateIn;
   theDate = "";
   
   std::vector<QString> components;
   
   if (species.isEmpty() == false) {
      components.push_back(species);
   }
   if (casename.isEmpty() == false) {
      components.push_back(casename);
   }
   if (anatomy.isEmpty() == false) {
      components.push_back(anatomy);
   }
   if (hemisphere.isEmpty() == false) {
      components.push_back(hemisphere);
   }
   if (description.isEmpty() == false) {
      components.push_back(description);
   }
   if (theDate.isEmpty() == false) {
      components.push_back(theDate);
   }
   if (numNodes.isEmpty() == false) {
      components.push_back(numNodes);
   }
   if (extension.isEmpty() == false) {
      if (extension[0] == '.') {
         if (extension.length() > 1) {
            components.push_back(extension.mid(1));
         } 
      }
      else {
         components.push_back(extension);
      }
   }
   
   QString name;
   
   //
   // Add the directory if it is specified
   //
   if (directory.isEmpty() == false) {
      name.append(directory);
      if (StringUtilities::endsWith(name, "/") == false) {
         name.append("/");
      }
   }
   
   name.append(StringUtilities::combine(components, "."));
   
   return name;
}   

/**
 * Change the description in the name of a caret file name.
 */
QString 
FileUtilities::changeCaretDataFileDescription(const QString& filename,
                                              const QString& newDescription)
{
   QString directory, species, casename, anatomy, hemisphere, description, descriptionNoType;
   QString theDate, numNodes, extension;
   
   QString outputName;
   
   //
   // See if a valid caret data file name
   //
   if (FileUtilities::parseCaretDataFileName(filename,
                                             directory,
                                             species,
                                             casename,
                                             anatomy,
                                             hemisphere,
                                             description,
                                             descriptionNoType,
                                             theDate,
                                             numNodes,
                                             extension)) {
      
      //
      // Set the new file name
      //
      outputName = reassembleCaretDataFileName(directory,
                                                    species,
                                                    casename,
                                                    anatomy,
                                                    hemisphere,
                                                    newDescription,
                                                    theDate,
                                                    numNodes,
                                                    extension);
   }
   else {
      //
      // Not a caret data file name so just add description before extension
      //
      outputName = filenameWithoutExtension(filename);
      outputName += newDescription;
      outputName += filenameExtension(filename);
   }

   return outputName;
}

/**
 * Change the type name in the description string of the caret file name.
 */
QString
FileUtilities::changeCaretDataFileDescriptionType(const QString& filename,
                                                  const QString& newDescription)
{
   QString directory, species, casename, anatomy, hemisphere, description, descriptionNoType;
   QString theDate, numNodes, extension;
   
   QString outputName;
   
   //
   // See if a valid caret data file name
   //
   if (FileUtilities::parseCaretDataFileName(filename,
                                             directory,
                                             species,
                                             casename,
                                             anatomy,
                                             hemisphere,
                                             description,
                                             descriptionNoType,
                                             theDate,
                                             numNodes,
                                             extension)) {
      
      //
      // Add the new description
      //
      if (descriptionNoType.isEmpty() == false) {
         descriptionNoType.append(".");
      }
      descriptionNoType.append(newDescription);
      
      //
      // Set the new file name
      //
      outputName = reassembleCaretDataFileName(directory,
                                                    species,
                                                    casename,
                                                    anatomy,
                                                    hemisphere,
                                                    descriptionNoType,
                                                    theDate,
                                                    numNodes,
                                                    extension);
   }
   else {
      //
      // Not a caret data file name so just add description before extension
      //
      outputName = filenameWithoutExtension(filename);
      outputName += newDescription;
      outputName += filenameExtension(filename);
   }

   return outputName;
}

/**
 * download a file specified by a URL (returns true if successful)
 */
#ifdef CARET_FLAG
bool 
FileUtilities::downloadFileWithHttpGet(const QString& fileUrl, 
                            const QString& saveFileName,
                            const int maxTimeToWait,
                            QString& errorMessageOut,
                            std::map<QString,QString>* headerTags,
                            int* returnCode)
{ 
   HttpFileDownload hfd(fileUrl, saveFileName, maxTimeToWait);
   hfd.download();
   const bool success = hfd.getDownloadSuccessful();
   errorMessageOut = StringUtilities::fromNumber(hfd.getResponseCode());
   errorMessageOut.append(": ");
   errorMessageOut.append(hfd.getErrorMessage());
   
   if (headerTags != NULL) {
      *headerTags = hfd.getResponseHeader();
   }
   if (returnCode != NULL) {
      *returnCode = hfd.getResponseCode();
   }

   return success;
} 
#endif // CARET_FLAG

/**
 * download file to a QString
 */
#ifdef CARET_FLAG
bool
FileUtilities::downloadFileWithHttpGet(const QString& fileUrl,
                            const int maxTimeToWait,
                            QString& fileContents,
                            QString& errorMessageOut,
                            std::map<QString,QString>* headerTags,
                            int* returnCode)
{
   HttpFileDownload hfd(fileUrl, maxTimeToWait);
   hfd.download();
   const bool success = hfd.getDownloadSuccessful();
   hfd.getContent(fileContents);
   errorMessageOut = StringUtilities::fromNumber(hfd.getResponseCode());
   errorMessageOut.append(": ");
   errorMessageOut.append(hfd.getErrorMessage());
   
   if (headerTags != NULL) {
      *headerTags = hfd.getResponseHeader();
   }
   if (returnCode != NULL) {
      *returnCode = hfd.getResponseCode();
   }

   return success;
}
#endif // CARET_FLAG

/**
 * download file to a C++ string
 */   
#ifdef CARET_FLAG
bool
FileUtilities::downloadFileWithHttpPost(const QString& fileUrl,
                            const QString& postContentIn,
                            const int maxTimeToWait,
                            QString& fileContentsOut,
                            QString& errorMessageOut,
                            const std::map<QString,QString>* additionalHeaderTagsIn,
                            std::map<QString,QString>* headerTagsOut,
                            int* returnCode)
{
   HttpFileDownload hfd(fileUrl, maxTimeToWait);
   hfd.setHttpRequestType(HttpFileDownload::HTTP_REQUEST_TYPE_POST);
   hfd.setPostRequestContent(postContentIn);
   if (additionalHeaderTagsIn != NULL) {
      hfd.addToRequestHeader(*additionalHeaderTagsIn);
   }
   hfd.download();
   const bool success = hfd.getDownloadSuccessful();
   hfd.getContent(fileContentsOut);
   errorMessageOut = StringUtilities::fromNumber(hfd.getResponseCode());
   errorMessageOut.append(": ");
   errorMessageOut.append(hfd.getErrorMessage());
   
   if (headerTagsOut != NULL) {
      *headerTagsOut = hfd.getResponseHeader();
   }
   if (returnCode != NULL) {
      *returnCode = hfd.getResponseCode();
   }

   return success;
}
#endif // CARET_FLAG

/**
 * create a directory (returns true if successful).
 */
bool 
FileUtilities::createDirectory(const QString& dirPathIn)
{
   //
   // Just creating a relative path ?
   //
   if (StringUtilities::findFirstOf(dirPathIn,"/\\") == -1) {
      QDir dir;
      return dir.mkdir(dirPathIn);
   }
   
   //
   // Save the current directory
   //
   const QString savedDirectory = QDir::currentPath();

   //
   // Determine which directories need to be created
   //
   std::stack<QString> dirStack;
   QString dirPath(dirPathIn);
   bool done = false;
   while (done == false) {
      if (QFile::exists(dirPath)) {
         QDir::setCurrent(dirPath);
         done = true;
      }
      else {
         dirStack.push(QString(FileUtilities::basename(dirPath)));
         dirPath = FileUtilities::dirname(dirPath);
         if (dirPath.isEmpty()) {
            done = true;
         }
      }
   }
   
   
   //
   // Create any needed directories
   //
   while (dirStack.empty() == false) {
      const QString dirName = dirStack.top();
      dirStack.pop();
      
      QDir dir;
      if (dir.mkdir(dirName) == false) {
         std::cout << "Unable to create directory " << dirName.toAscii().constData() << " in " << QDir::currentPath().toAscii().constData()
                   << std::endl;
         return false;
      }
      QDir::setCurrent(dirName);
   }
   
   //
   // Go back to saved directory
   //
   QDir::setCurrent(savedDirectory);
   
   return true;
}

/**
 * see if a directory contains files.
 */
bool 
FileUtilities::directoryContainsFiles(const QString& dirPath)
{
   QDir d(dirPath, QString::null, QDir::Name, QDir::Files);
   return (d.count() > 0);
}

/**
 * gunzip a file (returns true if successful).
 */
#ifdef CARET_FLAG
bool 
FileUtilities::gunzipFile(const QString& inputName, const QString& outputName)
{
   //
   // Buffer for reading and writing
   //
   const unsigned int bufferSize = 4096;
   unsigned char buffer[bufferSize];
   
   //
   // Open the input file 
   //
   gzFile zf = gzopen(inputName.toAscii().constData(), "rb");
   if (zf == NULL) {
      std::cout << "Unable to open " << inputName.toAscii().constData() << " for reading." << std::endl;
      return false;
   }
   
   //
   // Open the output file (need binary flags so new lines not manipulated on windows).
   //
   std::ofstream outFile(outputName.toAscii().constData(), std::ios::out | std::ios::binary);
   if (!outFile) {
      std::cout << "Unable to open " << outputName.toAscii().constData() << " for writing." << std::endl;
      return false;
   }
   
   //
   // Copy data from the input to the output file
   //
   int numRead = gzread(zf, (void*)buffer, bufferSize);
   while (numRead > 0) {
      outFile.write((const char*)buffer, numRead);
      numRead = gzread(zf, (void*)buffer, bufferSize);
   }
   
   //
   // Close the input and output files
   //
   gzclose(zf);
   outFile.close();
   
   return true;
}
#endif // CARET_FLAG

/**
 * Get subdirectory prefix for a file specified with a relative path.
 * if input is "thedir/filename.txt" this returns "thedir".  If there is no subdirectory or
 * the path is absolute (begins with "/" on unix or "a:\" on windows an empty string is returned.
 */
QString
FileUtilities::getSubdirectoryPrefix(const QString& s)
{
   QFileInfo fi(s);
   if (fi.isRelative() == false) {
      return "";
   }
   
   std::vector<QString> components;
   StringUtilities::token(s, "/\\", components);
   if (components.size() > 1) {
      return components[0];
   }
   return "";
}

/**
 * Return the directory separator for the current platform.
 */
QString 
FileUtilities::directorySeparator()
{
//   const char sep[2] = { QDir::separator(), '\0' };
   const QString sepStr(QDir::separator());
   return sepStr;
}

/**
 * deterimine if a path is an absolute path.
 */
bool 
FileUtilities::isAbsolutePath(const QString& name)
{
   if (name.isEmpty()) {
      return false;
   }
#ifdef Q_OS_WIN32
   if (name.indexOf(":") != -1) {
      return true;
   }
#else
   if (name[0] == '/') {
      return true;
   }
#endif
   return false;
}

/**
 * return the name of a temporary directory (empty if invalid).
 */
QString 
FileUtilities::temporaryDirectory()
{
   return QDir::tempPath();
/*
   QString tempDir("");
   
#ifdef Q_OS_WIN32
   tempDir = std::getenv("TEMP");
   if (tempDir.isEmpty()) {
      tempDir = std::getenv("TMP");
   }
#else
    char tmp[L_tmpnam];
    tmpnam(tmp);
    tempDir = FileUtilities::dirname(tmp);
#endif

   return tempDir;
   
*/
}

