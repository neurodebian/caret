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

#include "CommandVolumeImportRawFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeImportRawFile::CommandVolumeImportRawFile()
   : CommandBase("-volume-raw-import-file",
                 "VOLUME IMPORT RAW FILE")
{
}

/**
 * destructor.
 */
CommandVolumeImportRawFile::~CommandVolumeImportRawFile()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeImportRawFile::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> values, descriptions;
   values.push_back("INT8");     descriptions.push_back("8-bit integer");
   values.push_back("UINT8");    descriptions.push_back("8-bit unsigned integer");
   values.push_back("INT16");    descriptions.push_back("16-bit signed integer");
   values.push_back("UINT16");   descriptions.push_back("16-bit unsigned integer");
   values.push_back("INT32");    descriptions.push_back("32-bit signed integer");
   values.push_back("UINT32");   descriptions.push_back("32-bit unsigned integer");
   values.push_back("FLOAT");    descriptions.push_back("Single Precision Floating Point");
   values.push_back("DOUBLE");   descriptions.push_back("Double Precision Floating Point");
   values.push_back("VIRGB");    descriptions.push_back("Voxel-Interleaved RGB (red-green-blue)");
   values.push_back("SIRBB");    descriptions.push_back("Slice-Interleaved RGB (red-green-blue)");

   paramsOut.clear();
   paramsOut.addFile("Raw Data Volume File Name", FileFilters::getAnyFileFilter());
   paramsOut.addFile("Output Volume File Name", FileFilters::getAnyFileFilter());
   paramsOut.addInt("X-Dimension", 0, 0, 100000);
   paramsOut.addInt("Y-Dimension", 0, 0, 100000);
   paramsOut.addInt("Z-Dimension", 0, 0, 100000);
   paramsOut.addListOfItems("Data Type", values, descriptions);
   paramsOut.addBoolean("Byte Swap", false);
}

/**
 * get full help information.
 */
QString 
CommandVolumeImportRawFile::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-raw-volume-file-name>\n"
       + indent9 + "<output-volume-file-name>\n"
       + indent9 + "<x-dimension>\n"
       + indent9 + "<y-dimension>\n"
       + indent9 + "<z-dimension>\n"
       + indent9 + "<data-type>\n"
       + indent9 + "<byte-swap-flag>\n"
       + indent9 + "\n"
       + indent9 + "Import a raw data file into a volume file.\n"
       + indent9 + "\n"
       + indent9 + "\"data-type\" is one of:\n"
       + indent9 + "   INT8        8-bit integer\n"
       + indent9 + "   UINT8       8-bit unsigned integer\n"
       + indent9 + "   INT16       16-bit signed integer\n"
       + indent9 + "   UINT16      16-bit unsigned integer\n"
       + indent9 + "   INT32       32-bit signed integer\n"
       + indent9 + "   UINT32      32-bit unsigned integer\n"
       + indent9 + "   FLOAT       Single Precision Floating Point\n"
       + indent9 + "   DOUBLE      Double Precision Floating Point\n"
       + indent9 + "   VIRGB       Voxel-Interleaved RGB (red-green-blue)\n"
       + indent9 + "   SIRBB       Slice-Interleaved RGB (red-green-blue)\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeImportRawFile::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputDataFileName =
      parameters->getNextParameterAsString("Input Raw Volume File Name");
   const QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Volume File Name");
   const int dimensions[3] = {
      parameters->getNextParameterAsInt("X-Dimension"),
      parameters->getNextParameterAsInt("Y-Dimension"),
      parameters->getNextParameterAsInt("Z-Dimension")
   };
   const QString dataTypeName =
      parameters->getNextParameterAsString("Data Type Name");
   const bool byteSwapFlag =
      parameters->getNextParameterAsBoolean("Byte Swap");
   checkForExcessiveParameters();
   
   VolumeFile::VOXEL_DATA_TYPE dataType = VolumeFile::VOXEL_DATA_TYPE_UNKNOWN;
   if (dataTypeName == "INT8") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_CHAR;
   }
   else if (dataTypeName == "UINT8") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_CHAR_UNSIGNED;
   }
   else if (dataTypeName == "INT16") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_SHORT;
   }
   else if (dataTypeName == "UINT16") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_SHORT_UNSIGNED;
   }
   else if (dataTypeName == "INT32") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_INT;
   }
   else if (dataTypeName == "UINT32") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_INT_UNSIGNED;
   }
   else if (dataTypeName == "FLOAT") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_FLOAT;
   }
   else if (dataTypeName == "DOUBLE") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_DOUBLE;
   }
   else if (dataTypeName == "VIRGB") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED;
   }
   else if (dataTypeName == "SIRBB") {
      dataType = VolumeFile::VOXEL_DATA_TYPE_RGB_SLICE_INTERLEAVED;
   }
   else {
      throw CommandException("Invalid data type \""
                             + dataTypeName
                             + "\"");
   }
   
   VolumeFile volume;
   
   const VolumeFile::ORIENTATION orient[3] = {
      VolumeFile::ORIENTATION_UNKNOWN,
      VolumeFile::ORIENTATION_UNKNOWN,
      VolumeFile::ORIENTATION_UNKNOWN
   };
   const float org[3] = { 0.0, 0.0, 0.0 };
   const float space[3] = { 1.0, 1.0, 1.0 };
   volume.readFileVolumeRaw(inputDataFileName,
                            0,
                            dataType,
                            dimensions,
                            orient,
                            org,
                            space,
                            byteSwapFlag);
                            
   volume.writeFile(outputVolumeFileName);
}

      

