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

#include <algorithm>
#include <iostream>
#include <set>

#include <QImageReader>
#include <QImageWriter>

#include "CommandImageFormatConvert.h"
#include "FileFilters.h"
#include "ImageFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandImageFormatConvert::CommandImageFormatConvert()
   : CommandBase("-image-format-convert",
                 "IMAGE FORMAT CONVERT")
{
}

/**
 * destructor.
 */
CommandImageFormatConvert::~CommandImageFormatConvert()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandImageFormatConvert::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   QStringList fileFilters, fileExtensions;
   FileFilters::getImageSaveFileFilters(fileFilters,
                                        fileExtensions);
   paramsOut.clear();
   paramsOut.addFile("Input Image File Name", fileFilters);
   paramsOut.addFile("Output Image File Name", fileFilters);
}

/**
 * get full help information.
 */
QString 
CommandImageFormatConvert::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-image-file-name>  \n"
       + indent9 + "<output-image-file-name> \n"
       + indent9 + "\n"
       + indent9 + "Convert an image to a different image format.  This \n"
       + indent9 + "program determines the format of an image by the file \n"
       + indent9 + "name's extension.\n"
       + indent9 + "\n"
       + getSupportedImageFormatInformation(9)
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * get text displaying supported formats for use by getHelpInformation().
 */
QString 
CommandImageFormatConvert::getSupportedImageFormatInformation(const int indentation)
{
   //
   // Length of longest format name
   //
   int maxFormatLength = 0;
   
   //
   // Get formats writter
   //
   QList<QByteArray> writeByteArrayList = QImageWriter::supportedImageFormats();
   std::vector<QString> writeFormatList;
   QListIterator<QByteArray> writeByteArrayListIterator(writeByteArrayList);
   while (writeByteArrayListIterator.hasNext()) {
      const QString format(writeByteArrayListIterator.next());
      writeFormatList.push_back(format);
      maxFormatLength = std::max(maxFormatLength, format.length());
   }

   //
   // Get formats read
   //
   QList<QByteArray> readByteArrayList = QImageReader::supportedImageFormats();
   std::vector<QString> readFormatList;
   QListIterator<QByteArray> readByteArrayListIterator(readByteArrayList);
   while (readByteArrayListIterator.hasNext()) {
      const QString format(readByteArrayListIterator.next());
      readFormatList.push_back(format);
      maxFormatLength = std::max(maxFormatLength, format.length());
   }
   
   //
   // Get a set of all formats
   //
   std::set<QString> allFormats;
   allFormats.insert(readFormatList.begin(), readFormatList.end());
   allFormats.insert(writeFormatList.begin(), writeFormatList.end());

   //
   // Get read and write formats
   //
   const QString indentString(indentation, ' ');
   const QString indentString3(indentString + "   ");
   QString formatStringOut;
   formatStringOut += (indentString 
                       + "Supported Image Formats\n");
   for (std::set<QString>::const_iterator iter = allFormats.begin();
        iter != allFormats.end();
        iter++) {
      const QString formatName(*iter);
      
      const bool readFlag = 
         (std::find(readFormatList.begin(), readFormatList.end(), formatName) !=
                                                readFormatList.end());
      const bool writeFlag = 
         (std::find(writeFormatList.begin(), writeFormatList.end(), formatName) !=
                                                writeFormatList.end());
                                                
      QString s(formatName.leftJustified(maxFormatLength + 2, ' '));
      if (readFlag) {
         s += "READ  ";
      }
      else {
         s += "      ";
      }
      if (writeFlag) {
         s += "WRITE  ";
      }
      else {
         s += "       ";
      }
      s += "\n";
      
      formatStringOut += (indentString3 + s);
   }
   
   return formatStringOut;
}      

/**
 * execute the command.
 */
void 
CommandImageFormatConvert::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputImageFileName =
      parameters->getNextParameterAsString("Input Image File Name");
   const QString outputImageFileName =
      parameters->getNextParameterAsString("Output Image File Name");
   checkForExcessiveParameters();
      
   //
   // Read the image
   //
   ImageFile imageFile;
   imageFile.readFile(inputImageFileName);

   //
   // Write the image
   //
   imageFile.writeFile(outputImageFileName);
}

      

