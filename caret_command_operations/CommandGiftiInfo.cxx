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

#include "CommandGiftiInfo.h"
#include "FileFilters.h"
#include "GiftiDataArrayFile.h"
#include "GiftiMetaData.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandGiftiInfo::CommandGiftiInfo()
   : CommandBase("-gifti-info",
                 "GIFTI INFO")
{
}

/**
 * destructor.
 */
CommandGiftiInfo::~CommandGiftiInfo()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandGiftiInfo::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addMultipleFiles("GIFTI Files", FileFilters::getAnyFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandGiftiInfo::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + " <one-of-more-gifti-file-names>\n"
       + indent9 + "\n"
       + indent9 + "Display the GIFTI file's information and metadata.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandGiftiInfo::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString indent3(3, QChar(' '));
   const QString indent6(6, QChar(' '));
   const QString dashes25(25, QChar('-'));
   const QString dashes75(75, QChar('-'));
   
   //
   // Get the names of the GIFTI files
   //
   std::vector<QString> fileNames;
   while (parameters->getParametersAvailable()) {
      fileNames.push_back(parameters->getNextParameterAsString("GIFTI File Name"));
   }
   const int numFiles = static_cast<int>(fileNames.size());
   if (numFiles <= 0) {
      throw CommandException("No GIFTI File Names provided.");
   }

   //
   // Print information about the files
   //
   for (int i = 0; i < numFiles; i++) {
      const QString name(fileNames[i]);

      //
      // Print file metadata
      //
      std::cout << dashes75.toAscii().constData() << std::endl;
      std::cout << "Filename: " 
                << fileNames[i].toAscii().constData()
                << std::endl;
      try {
         //
         // Read the file
         //
         GiftiDataArrayFile gifti;
         //gifti.setReadMetaDataOnlyFlag(true);
         gifti.readFileMetaDataOnly(name);
         
         std::cout << "File Metadata:"
                   << std::endl;
         printMetaData(std::cout, indent3, gifti.getMetaData());
         
         //
         // Print info about a data Array
         //
         const int numArrays = gifti.getNumberOfDataArrays();
         for (int j = 0; j < numArrays; j++) {
            std::cout << "Data Array "
                      << j + 1
                      << " "
                      << dashes25.toAscii().constData()
                      << std::endl;
            GiftiDataArray* gda = gifti.getDataArray(j);
            
            std::cout << indent3.toAscii().constData()
                      << "Array Indexing Order: "
                      << GiftiDataArray::getArraySubscriptingOrderName(
                            gda->getArraySubscriptingOrder()).toAscii().constData()
                      << std::endl;
            std::cout << indent3.toAscii().constData()
                      << "Data Type: "
                      << GiftiDataArray::getDataTypeName(
                            gda->getDataType()).toAscii().constData()
                      << std::endl;
            std::cout << indent3.toAscii().constData()
                      << "Dimensions: ";
            const int numDim = gda->getNumberOfDimensions();
            for (int id = 0; id < numDim; id++) {
               if (id > 0) {
                  std::cout << ", ";
               }
               std::cout << gda->getDimension(id);
            }
            std::cout << std::endl;
            std::cout << indent3.toAscii().constData()
                      << "Encoding: "
                      << GiftiDataArray::getEncodingName(
                            gda->getEncoding()).toAscii().constData()
                      << std::endl;
            std::cout << indent3.toAscii().constData()
                      << "Endian: "
                      << GiftiDataArray::getEndianName(
                            gda->getEndian()).toAscii().constData()
                      << std::endl;
            QString externalFileName;
            int externalFileOffset = 0;
            gda->getExternalFileInformation(externalFileName, externalFileOffset);
            std::cout << indent3.toAscii().constData()
                      << "External File Name: "
                      << externalFileName.toAscii().constData()
                      << std::endl;
            std::cout << indent3.toAscii().constData()
                      << "External File Offset: "
                      << externalFileOffset
                      << std::endl;
            std::cout << indent3.toAscii().constData()
                      << "Intent: "
                      << gda->getIntent().toAscii().constData()
                      << std::endl;
            std::cout << indent3.toAscii().constData()
                      << "Metadata:"
                      << std::endl;
            printMetaData(std::cout, indent6, gda->getMetaData());         
         }
      }
      catch (FileException& e) {
         std::cout << e.whatQString().toAscii().constData() << std::endl;
      }
      std::cout << std::endl;
   }
}

/**
 * print the metadata.
 */
void 
CommandGiftiInfo::printMetaData(std::ostream& outStream,
                                const QString& indent,
                                const GiftiMetaData* md)
{
   std::vector<QString> mdNames;
   md->getAllNames(mdNames);
   const int num = static_cast<int>(mdNames.size());
   
   for (int i = 0; i < num; i++) {
      QString value;
      md->get(mdNames[i], value);
      
      outStream << indent.toAscii().constData()
                << mdNames[i].toAscii().constData()
                << ":  "
                << value.toAscii().constData()
                << std::endl;
   }
}
      

