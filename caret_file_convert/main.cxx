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

#include <cstdlib>
#include <iostream>
 
#include <QGlobalStatic>
#include <QApplication>
#include <QDateTime>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CaretVersion.h"
#include "CommandLineUtilities.h"
#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "FreeSurferCurvatureFile.h"
#include "FreeSurferFunctionalFile.h"
#include "FreeSurferSurfaceFile.h"
#include "MetricFile.h"
#include "GiftiDataArrayFile.h"
#include "PaintFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"

enum MODE { 
   MODE_NONE, 
   MODE_BINARY, 
   MODE_TEXT, 
   MODE_XML,
   MODE_XML64,
   MODE_XML64GZ,
   MODE_INFO,
   MODE_SPEC_BINARY,
   MODE_SPEC_TEXT,
   MODE_SURFACE_CONVERT,
   MODE_FREE_SURFER_CURVATURE_TO_CARET,
   MODE_FREE_SURFER_FUNCTIONAL_TO_CARET,
   MODE_FREE_SURFER_LABEL_TO_CARET,
   MODE_CARET_PAINT_TO_FREE_SURFER_LABEL,
   MODE_VOLUME
};

enum SURFACE_FILE_TYPE {
   SURFACE_TYPE_UNKNOWN,
   SURFACE_TYPE_BYU,
   SURFACE_TYPE_CARET,
   SURFACE_TYPE_FREE_SURFER,
   SURFACE_TYPE_FREE_SURFER_PATCH,
   SURFACE_TYPE_STL,
   SURFACE_TYPE_VTK,
   SURFACE_TYPE_XML_VTK
};

static SURFACE_FILE_TYPE inputSurfaceType;
static SURFACE_FILE_TYPE outputSurfaceType;

static MODE mode = MODE_NONE;
static AbstractFile::FILE_FORMAT fileWriteType = 
                                       AbstractFile::FILE_FORMAT_ASCII;
static QString specFileName;
static QString structureName;
static QString inputSurfaceName;
static QString inputSurfaceName2;
static QString outputSurfaceName;
static QString outputSurfaceName2;
static QString outputCoordTypeName;
static QString outputTopoTypeName;

static BrainSet* brainSet = NULL;

/**
 * Print help information.
 */
static void
printHelp(const QString& programNameIn)
{
   const QString programName(FileUtilities::basename(programNameIn));
   
   std::cout
   << std::endl
   << "NAME" << std::endl
   << "   " << programName.toAscii().constData() 
            << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
            << " (" << __DATE__ << ")" 
            << std::endl
   << "" << std::endl
   << "SYNOPSIS for Caret Binary/Text File Format Conversion" << std::endl;
   if (GiftiDataArrayFile::getGiftiXMLEnabled()) {
      std::cout << "   " << programName.toAscii().constData() << " [-text | -binary | -xml | -xml64 | -xml64GZ | -info] file(s)" << std::endl;
   }
   else {
      std::cout << "   " << programName.toAscii().constData() << " [-text | -binary | -info] file(s)" << std::endl;
   }
   std::cout
   << "" << std::endl
   << "DESCRIPTION for Caret Binary/Text File Format Conversion" << std::endl
   << "   Convert Caret data files to text or binary format." << std::endl
   << "" << std::endl
   << "   REQUIRED PARAMETERS" << std::endl
   << "      Note: You must specify one of the following:" << std::endl
   << "" << std::endl
   << "         -text  " << std::endl
   << "            convert the specified files to text (ascii) format." << std::endl
   << "" << std::endl
   << "         -binary  " << std::endl
   << "            convert the specified files to binary format." << std::endl
   << "" << std::endl
   << "         -info  " << std::endl
   << "            do no conversion but just list the data type of the recognized" << std::endl
   << "            files." << std::endl
   << "" << std::endl
   << "SYNOPSIS for Converting Data Files in a Spec File" << std::endl
   << "   " << programName.toAscii().constData() << " -spec-bin  spec-file-name" << std::endl
   << "   " << programName.toAscii().constData() << " -spec-text spec-file-name" << std::endl
   << "" << std::endl
   << "DESCRIPTION for Converting Data Files in a Spec File" << std::endl
   << "   Converts all of the data files listed in a spec file to either" << std::endl
   << "   binary or text format." << std::endl
   << "" << std::endl
   << "   REQUIRED PARAMETERS" << std::endl
   << "      Note: You must specify one of the following:" << std::endl
   << "         Use \"-spec-bin\" to convert the data files to binary format." << std::endl
   << "         Use \"-spec-text\" to convert the data files to text format." << std::endl
   << "" << std::endl
   << "SYNOPSIS for Surface File Conversion" << std::endl
   << "   " << programName.toAscii().constData() << " -sc -is <type> <name> [name2]  \\" << std::endl
   << "       -os <type> <name> [name2] [coord-type] [topo-type] \\" << std::endl
   << "       [-outbin  -outtext] \\" << std::endl
   << "       [-spec spec-file-name] [-struct structure]" << std::endl
   << "" << std::endl
   << "DESCRIPTION for Convert surface file formats." << std::endl
   << " " << std::endl
   << "   REQUIRED PARAMETERS" << std::endl
   << "      -sc  Specifies surface format conversion." << std::endl
   << "      " << std::endl
   << "      -is <type> <name> [name2] Specifies the input surface." << std::endl
   << "         type is one of:" << std::endl
   << "            BYU  input surface is a BYU surface file" << std::endl
   << "               This type is followed by the name of the BYU surface file." << std::endl
   << "      " << std::endl
   << "            CARET  input surface is Caret Coordinate and Topology files." << std::endl
   << "               This type is followed by the names of the Caret Coordinate" << std::endl
   << "               file, the Caret Topology file." << std::endl
   << "      " << std::endl
   << "            FSS  input surface is a FreeSurfer surface surface file." << std::endl
   << "               This type is followed by the name of the FreeSurfer surface" << std::endl
   << "               file." << std::endl
   << "            " << std::endl
   << "            FSP  input surface is a FreeSurfer surface patch file." << std::endl
   << "               This type is followed by the name of the FreeSurfer patch" << std::endl
   << "               file and the name of a corresponding FreeSurfer surface file." << std::endl
   << "            " << std::endl
   << "      " << std::endl
   << "            STL  input surface is a BYU surface file" << std::endl
   << "               This type is followed by the name of the STL surface file." << std::endl
   << "      " << std::endl
   << "            VTKP  input surface is a VTK PolyData file." << std::endl
   << "               This type is followed by the name of the VTK PolyData file." << std::endl
   << "            " << std::endl
   << "            VTKXP  input surface is an XML VTK PolyData file." << std::endl
   << "               This type is followed by the name of the XML VTK PolyData file." << std::endl
   << "      " << std::endl
   << "      -os <type> <name> [name2] [coord-type] [topo-type] Specifies the output " << std::endl
   << "         surface." << std::endl
   << "         This option's name and name2 parameters are the same as for the \"-is\"." << std::endl
   << "         option.  The third and forth parameters are the coordinate file type" << std::endl
   << "         and the topology file type." << std::endl
   << "            " << std::endl
   << "                  Valid topology file types: " << std::endl
   << "                     CLOSED" << std::endl
   << "                     OPEN" << std::endl
   << "                     CUT" << std::endl
   << "                     LOBAR_CUT" << std::endl
   << "                     UNKNOWN" << std::endl
   << "      " << std::endl
   << "                  Valid coordinate file types" << std::endl
   << "                     RAW" << std::endl
   << "                     FIDUCIAL" << std::endl
   << "                     INFLATED" << std::endl
   << "                     VERY_INFLATED" << std::endl
   << "                     SPHERICAL" << std::endl
   << "                     ELLIPSOIDAL" << std::endl
   << "                     CMW" << std::endl
   << "                     FLAT" << std::endl
   << "                     FLAT_LOBAR" << std::endl
   << "                     UNKNOWN" << std::endl   << "      " << std::endl
   << "   OPTIONAL PARAMETERS" << std::endl
   << "      -outbin  Indicates that output Caret files are to be written in" << std::endl
   << "         binary format." << std::endl
   << "      " << std::endl
   << "      -outtext  Indicates that output Caret files are to be written in" << std::endl
   << "         text (ascii) format." << std::endl
   << "      " << std::endl
   << "      -spec spec-file-name   Indicates that output Caret files are to be" << std::endl
   << "         written to the spec file <spec-file-name>.  If the spec file does" << std::endl
   << "         not exist, it will be created." << std::endl
   << "      " << std::endl
   << "      -struct  structure   Indicates the structure that should be placed into" << std::endl
   << "         the spec file when writing Caret files." << std::endl
   << "            Valid structure types" << std::endl
   << "               right" << std::endl
   << "               left" << std::endl
   << "               cerebellum" << std::endl
   << "      " << std::endl
   << "SYNOPSIS for FreeSurfer Data File / Caret Conversion" << std::endl
   << "   " << programName.toAscii().constData() << " -fsc2c free-surfer-curvature \\" << std::endl
   << "       free-surfer-surface output-surface-shape \\" << std::endl
   << "       [-outbin  -outtext]  \\" << std::endl
   << "       [-spec spec-file-name] [-struct structure] " << std::endl
   << "" << std::endl
   << "   " << programName.toAscii().constData() << " -fsf2c free-surfer-functional \\" << std::endl
   << "       free-surfer-surface output-metric \\" << std::endl
   << "       [-outbin  -outtext]  \\" << std::endl
   << "       [-spec spec-file-name] [-struct structure] " << std::endl
   << "" << std::endl
   << "   " << programName.toAscii().constData() << " -cp2fsl caret-paint-file \\" << std::endl
   << "       caret-fiducial-coord-file " << std::endl
   << "" << std::endl
   << "DESCRIPTION for FreeSurfer Data File / Caret Conversion" << std::endl
   << "   Convert free surfer data files to Caret format." << std::endl
   << "" << std::endl
   << "   REQUIRED PARAMETERS" << std::endl
   << "      " << std::endl
   << "      Note: You must specify one of the following:" << std::endl
   << "" << std::endl
   << "         -fsc2c  " << std::endl
   << "            Converts a free surfer curvature file into a Caret surface " << std::endl
   << "            shape file.  This option is followed by the name of the Free" << std::endl
   << "            Surfer curvature file, the name of a Free Surfer surface " << std::endl
   << "            file (used to get number of nodes), and the output Caret" << std::endl
   << "            surface shape file." << std::endl
   << "            " << std::endl
   << "         -fsf2c  " << std::endl
   << "            Converts a free surfer functional file into a Caret metric " << std::endl
   << "            file.  This option is followed by the name of the Free" << std::endl
   << "            Surfer functional file, the name of a Free Surfer surface " << std::endl
   << "            file (used to get number of nodes), and the output Caret" << std::endl
   << "            metric file." << std::endl
   << "            " << std::endl
   << "         -fsl2c" << std::endl
   << "            Converts all free surfer label files in the specified directory" << std::endl
   << "            into a paint file.  This option is followed by the directory" << std::endl
   << "            containing the label files, the name of a Free Surfer surface" << std::endl
   << "            file (used to get number of nodes), and the output Caret" << std::endl
   << "            paint file." << std::endl
   << "         -cp2fsl" << std::endl
   << "            Convert a paint file into free surfer label files.  No output" << std::endl
   << "            name is needed since free surfer uses on file per paint ID" << std::endl
   << "            with the name of the file being \"paintID\".label." << std::endl
   << "            " << std::endl
   << "" << std::endl
   << "   OPTIONAL PARAMETERS for FreeSurfer Data File to Caret Conversion" << std::endl
   << "" << std::endl
   << "      See the surface conversion options for explanation." << std::endl
   << "" << std::endl
   << "SYNOPSIS for Converting Volume Files" << std::endl
   << "   " << programName.toAscii().constData() << " -vc  input-volume output-volume" << std::endl
   << "" << std::endl
   << "DESCRIPTION for Converting Volume Files" << std::endl
   << "   The extension of the output volume file name determines the type of" << std::endl
   << "    of volume file." << std::endl
   << "" << std::endl
   << "   Extension       Type of Volume File" << std::endl
   << "   ---------       -------------------" << std::endl
   << "      HEAD         AFNI Volume File" << std::endl
   << "      ifh          Washington University Volume File" << std::endl
   << "      nii          NIFTI Volume File" << std::endl
   << "      nii.gz       Compressed NIFTI Volume File" << std::endl
   << "" << std::endl
   << "" << std::endl
   << "OPTIONAL PARAMETERS ALL MODES" << std::endl
   << "" << std::endl
   << "   -help" << std::endl
   << "      Prints this help information." << std::endl
   << "" << std::endl
   << "For best results, run this program from the directory containing the files." << std::endl
   << "" << std::endl
   << "EXAMPLES" << std::endl
   << "" << std::endl
   << "   Convert all Caret data files in the current directory that support " << std::endl
   << "   binary formatto the binary format:" << std::endl
   << "         " << programName.toAscii().constData() << " -binary *" << std::endl
   << "" << std::endl
   << "   Convert the two Caret files into text format Caret files:" << std::endl
   << "         " << programName.toAscii().constData() << " -text file1.top file3.coord" << std::endl
   << "" << std::endl
   << "   List the data type (text or binary) of all Caret files in the " << std::endl
   << "   current directory:" << std::endl
   << "         " << programName.toAscii().constData() << " -info *" << std::endl
   << "" << std::endl
   << "   Convert the free surfer surface into closed topology and fiducial" << std::endl
   << "   coordinate files.  Create a spec file for a left cortex." << std::endl
   << "         " << programName.toAscii().constData() << " -sc -is FSS lh.orig.asc -os CARET fiducial.coord \\" << std::endl
   << "            closed.topo FIDUCIAL CLOSED -spec file.spec -struct LEFT" << std::endl
   << "" << std::endl
   << "   Convert the free surfer patch into cut topology and flat" << std::endl
   << "   coordinate files.  Create or add to the spec file as left cortex." << std::endl
   << "   Write the coord and topo files in text format." << std::endl
   << "         " << programName.toAscii().constData() << " -sc -is FSP lh.oc.flat.asc lh.orig.asc \\" << std::endl
   << "            -os CARET flat.coord cut.topo FLAT CUT -spec file.spec -struct LEFT \\" << std::endl
   << "            -outtext" << std::endl
   << "" << std::endl
   << "   Convert the free surfer curvature file to a Caret surface shape file." << std::endl
   << "         " << programName.toAscii().constData() << " -fsc2c lh.curv.asc lh.pial.asc \\" << std::endl
   << "            shape.surface_shape -spec file.spec" << std::endl
   << "   " << std::endl
   << "   Convert the free surfer functional file to a Caret metric file." << std::endl
   << "         " << programName.toAscii().constData() << " -fsf2c pho-lh.5.w.asc lh.pial.asc \\" << std::endl
   << "            func.metric -spec file.spec" << std::endl
   << "   " << std::endl
   << "   Convert the free surfer label files in current directory to a Caret metric file." << std::endl
   << "         " << programName.toAscii().constData() << " -fsl2c  .  lh.pial.asc \\" << std::endl
   << "            label.paint -spec file.spec" << std::endl
   << "   " << std::endl
   << "   Convert the caret paint file into free surfer label files" << std::endl
   << "         " << programName.toAscii().constData() << " -cp2fsl  file.paint \\" << std::endl
   << "            fiducial.coord" << std::endl
   << "   " << std::endl
   << "   Convert the data files listed in the spec file to binary format." << std::endl
   << "         " << programName.toAscii().constData() << "-spec-bin  human.spec" << std::endl
   << "   " << std::endl
   << "   Convert the volume file from AFNI to compressed NIFTI format." << std::endl
   << "         " << programName.toAscii().constData() << " -vc anat+orig.HEAD anat+orig.nii.gz" << std::endl
   << "   " << std::endl
   << "   " << std::endl;
}

/*
 * Convert caret files between binary and text format or display info about the file.
 */
static void
textBinaryConvert(const int firstFileIndex,
                  const int argc,
                  char* argv[])
{
   //
   // Process the files
   //
   for (int i = firstFileIndex; i < argc; i++) {
      const QString filename(argv[i]);
      QString errorMessage;
      
      //
      // Read the header of the file
      //
      AbstractFile* af = AbstractFile::readAnySubClassDataFile(filename, true, errorMessage);
      
      std::cout << FileUtilities::basename(filename).toAscii().constData() << ": ";
      if (af == NULL) {
         std::cout << "unable to read file or not a caret data file.";
      }
      else if (af->getFileHasHeader() == false) {
         std::cout << "file does not have header.";
      }
      else {
         //
         // Get files current encoding
         //
         const QString formatString(af->getHeaderTag(AbstractFile::headerTagEncoding));
         
         switch (mode) {
            case MODE_NONE:
               break;
            case MODE_BINARY:
               if (formatString == AbstractFile::headerTagEncodingValueBinary) {
                  std::cout << "already in binary format";
               }
               else if (af->getCanWrite(AbstractFile::FILE_FORMAT_BINARY)) {
                  try {
                     af->readFile(filename);
                     af->setFileWriteType(AbstractFile::FILE_FORMAT_BINARY);
                     af->writeFile(filename);
                     std::cout << "converted to binary.";
                  }
                  catch (FileException& e) {
                     std::cout << "error converting or writing.";
                  }
               }
               else {
                  std::cout << "does not support binary format.";
               }
               break;
            case MODE_INFO:
               if (formatString.isEmpty()) {
                  std::cout << "unrecognized format."; 
               }
               else {
                  std::cout << formatString.toAscii().constData() << ".";
               }
               break;
            case MODE_TEXT:
               if (formatString == AbstractFile::headerTagEncodingValueAscii) {
                  std::cout << "already in text format";
               }
               else if (af->getCanWrite(AbstractFile::FILE_FORMAT_ASCII)) {
                  try {
                     af->readFile(filename);
                     af->setFileWriteType(AbstractFile::FILE_FORMAT_ASCII);
                     af->writeFile(filename);
                     std::cout << "converted to text.";
                  }
                  catch (FileException& e) {
                     std::cout << "error converting or writing.";
                  }
               }
               else {
                  std::cout << "does not support binary format.";
               }
               break;
            case MODE_XML:
               //if (formatString == AbstractFile::headerTagEncodingValueAscii) {
               //   std::cout << "already in XML format";
               //}
               //else 
               if (af->getCanWrite(AbstractFile::FILE_FORMAT_XML)) {
                  try {
                     af->readFile(filename);
                     af->setFileWriteType(AbstractFile::FILE_FORMAT_XML);
                     af->writeFile(filename);
                     std::cout << "converted to XML.";
                  }
                  catch (FileException& e) {
                     std::cout << "error converting or writing.";
                  }
               }
               else {
                  std::cout << "does not support XML format.";
               }
               break;
            case MODE_XML64:
               //if (formatString == AbstractFile::headerTagEncodingValueAscii) {
               //   std::cout << "already in text format";
               //}
               //else 
               if (af->getCanWrite(AbstractFile::FILE_FORMAT_XML_BASE64)) {
                  try {
                     af->readFile(filename);
                     af->setFileWriteType(AbstractFile::FILE_FORMAT_XML_BASE64);
                     af->writeFile(filename);
                     std::cout << "converted to XML Base64.";
                  }
                  catch (FileException& e) {
                     std::cout << "error converting or writing.";
                  }
               }
               else {
                  std::cout << "does not support XML Base64 format.";
               }
               break;
            case MODE_XML64GZ:
               //if (formatString == AbstractFile::headerTagEncodingValueAscii) {
               //   std::cout << "already in text format";
               //}
               //else 
               if (af->getCanWrite(AbstractFile::FILE_FORMAT_XML_GZIP_BASE64)) {
                  try {
                     af->readFile(filename);
                     af->setFileWriteType(AbstractFile::FILE_FORMAT_XML_GZIP_BASE64);
                     af->writeFile(filename);
                     std::cout << "converted to XML GZIP Base64.";
                  }
                  catch (FileException& e) {
                     std::cout << "error converting or writing.";
                  }
               }
               else {
                  std::cout << "does not support XML GZIP Base64 format.";
               }
               break;
            case MODE_SPEC_BINARY:
            case MODE_SPEC_TEXT:
            case MODE_SURFACE_CONVERT:
            case MODE_FREE_SURFER_CURVATURE_TO_CARET:
            case MODE_FREE_SURFER_FUNCTIONAL_TO_CARET:
            case MODE_FREE_SURFER_LABEL_TO_CARET:
            case MODE_CARET_PAINT_TO_FREE_SURFER_LABEL:
            case MODE_VOLUME:
               break;
         }
      }
      
      if (af != NULL) {
        delete af;  // can't delete ?? compiler bug ??
        //af->clear();   // so at least free up memory
      }
      
      std::cout << std::endl;
   }
}

/**
 * Write/Update a spec file.
 */
static void
updateSpecFile(const std::vector<QString>& tags, const std::vector<QString>& values)
{
   //
   // If the user specified a spec file
   //
   if (specFileName.isEmpty() == false) {
      //
      // Try reading the spec file but ignore errors since it may not exist
      //
      SpecFile specFile;
      try {
         specFile.readFile(specFileName);
      }
      catch (FileException&) {
      }
      
      //
      // Add the spec file tags
      //
      for (int i = 0; i < static_cast<int>(tags.size()); i++) {
         specFile.addToSpecFile(tags[i], values[i], "", false);
      }
      
      //
      // Set the structure
      //
      specFile.setStructure(StringUtilities::makeLowerCase(structureName));
      
      //
      // Write the spec file
      //
      try {
         specFile.writeFile(specFileName);
      }
      catch (FileException&) {
         std::cout << "ERROR: writing spec file: " << specFileName.toAscii().constData() << std::endl;
         exit(-1);
      }
   }
}

/*
 * Convert a free surfer curvature file to caret surface shape.
 */
static void
freeSurferCurvatureToCaretConvert()
{
   QString freeSurfaceCurvatureName(inputSurfaceName);
   
   QString freeSurfaceSurfaceName(inputSurfaceName2);
   
   QString shapeName(outputSurfaceName);
   
   //
   // Read in the free surfer surface file
   //
   AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
   if (freeSurfaceSurfaceName.right(3) == "asc") {
      format = AbstractFile::FILE_FORMAT_ASCII;
   }
   FreeSurferSurfaceFile freeSurferSurfaceFile;
   try {
      freeSurferSurfaceFile.setFileReadType(format);
      freeSurferSurfaceFile.readFile(freeSurfaceSurfaceName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading " << freeSurfaceSurfaceName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // convert the free surface curvature file to the surface shape file.
   //   
   SurfaceShapeFile shapeFile;
   try {
      AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
      if (freeSurfaceCurvatureName.right(3) == "asc") {
         format = AbstractFile::FILE_FORMAT_ASCII;
      }
      shapeFile.importFreeSurferCurvatureFile(freeSurferSurfaceFile.getNumberOfVertices(),
                                              freeSurfaceCurvatureName,
                                              format);
   }
   catch (FileException&) {
      std::cout << "ERROR: reading " << freeSurfaceCurvatureName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the surface shape file
   //
   try {
      shapeFile.setFileWriteType(fileWriteType);
      shapeFile.writeFile(shapeName);
   }
   catch (FileException&) {
      std::cout << "Error writing surface shape file: " << shapeName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Add the surface shape files to the spec file
   //
   std::vector<QString> tags;
   std::vector<QString> values;
   tags.push_back(SpecFile::surfaceShapeFileTag);
   values.push_back(shapeName);
   updateSpecFile(tags, values);
}

/*
 * Convert a caret paint file to free surfer label file
 */
static void
caretPaintToFreeSurferLabel()
{
   //
   // Load the paint file
   //
   PaintFile paintFile;
   try {
      paintFile.readFile(inputSurfaceName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading " << inputSurfaceName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Load the coordinate file
   //
   CoordinateFile coordFile;
   try {
      coordFile.readFile(inputSurfaceName2);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading " << inputSurfaceName2.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Export to label files
   //
   try {
      for (int i = 0; i < paintFile.getNumberOfColumns(); i++) {
         paintFile.exportFreeSurferAsciiLabelFile(i,
                                                  "",
                                                  &coordFile);
      }
   }
   catch (FileException& e) {
      std::cout << "ERROR: exporting labels: " << e.whatQString().toAscii().constData() << std::endl;
   }
}

/*
 * Convert a free surfer functional file to caret metric.
 */
static void
freeSurferLabelToCaretConvert()
{
   QString freeSurfaceLabelDirName(inputSurfaceName);
   freeSurfaceLabelDirName.append("/junk");
   
   QString freeSurfaceSurfaceName(inputSurfaceName2);
   
   QString paintName(outputSurfaceName);
   
   //
   // Read in the free surfer surface file
   //
   AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
   if (freeSurfaceSurfaceName.right(3) == "asc") {
      format = AbstractFile::FILE_FORMAT_ASCII;
   }
   FreeSurferSurfaceFile freeSurferSurfaceFile;
   try {
      freeSurferSurfaceFile.setFileReadType(format);
      freeSurferSurfaceFile.readFile(freeSurfaceSurfaceName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading " << freeSurfaceSurfaceName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // convert the free surface label files to the paint file.
   //   
   PaintFile paintFile;
   try {
      paintFile.importFreeSurferAsciiLabelFile(freeSurferSurfaceFile.getNumberOfVertices(),
                                                freeSurfaceLabelDirName,
                                                NULL,
                                                true);
   }
   catch (FileException&) {
      std::cout << "ERROR: reading " << freeSurfaceLabelDirName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the paint file
   //
   try {
      paintFile.setFileWriteType(fileWriteType);
      paintFile.writeFile(paintName);
   }
   catch (FileException&) {
      std::cout << "Error writing paint file: " << paintName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Add the surface shape files to the spec file
   //
   std::vector<QString> tags;
   std::vector<QString> values;
   tags.push_back(SpecFile::paintFileTag);
   values.push_back(paintName);
   updateSpecFile(tags, values);
}

/*
 * Convert a free surfer functional file to caret metric.
 */
static void
freeSurferFunctionalToCaretConvert()
{
   QString freeSurfaceFunctionalName(inputSurfaceName);
   
   QString freeSurfaceSurfaceName(inputSurfaceName2);
   
   QString metricName(outputSurfaceName);
   
   //
   // Read in the free surfer surface file
   //
   AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
   if (freeSurfaceSurfaceName.right(3) == "asc") {
      format = AbstractFile::FILE_FORMAT_ASCII;
   }
   FreeSurferSurfaceFile freeSurferSurfaceFile;
   try {
      freeSurferSurfaceFile.setFileReadType(format);
      freeSurferSurfaceFile.readFile(freeSurfaceSurfaceName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading " << freeSurfaceSurfaceName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // convert the free surface functional file to the metric file.
   //   
   MetricFile metricFile;
   try {
      AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
      if (freeSurfaceFunctionalName.right(3) == "asc") {
         format = AbstractFile::FILE_FORMAT_ASCII;
      }
      metricFile.importFreeSurferFunctionalFile(freeSurferSurfaceFile.getNumberOfVertices(),
                                                freeSurfaceFunctionalName,
                                                format);
   }
   catch (FileException&) {
      std::cout << "ERROR: reading " << freeSurfaceFunctionalName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the surface shape file
   //
   try {
      metricFile.setFileWriteType(fileWriteType);
      metricFile.writeFile(metricName);
   }
   catch (FileException&) {
      std::cout << "Error writing metric file: " << metricName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Add the surface shape files to the spec file
   //
   std::vector<QString> tags;
   std::vector<QString> values;
   tags.push_back(SpecFile::metricFileTag);
   values.push_back(metricName);
   updateSpecFile(tags, values);
}

/**
 * Convert a surface file to another type.
 */
static void
surfaceFileConversion()
{
   //
   // Convert the coordinate type name to a surface type
   //
   const BrainModelSurface::SURFACE_TYPES surfaceType = 
         BrainModelSurface::getSurfaceTypeFromConfigurationID(outputCoordTypeName);
            
   //
   // Convert the topology type name to a topology type
   //
   const TopologyFile::TOPOLOGY_TYPES topoType = 
             TopologyFile::getTopologyTypeFromPerimeterID(outputTopoTypeName);
            
   int brainModelOfInterest = 0;
   
   //
   // Read in the appropriate surface
   //
   switch(inputSurfaceType) {
      case SURFACE_TYPE_BYU:
         try {
            brainSet->importByuSurfaceFile(inputSurfaceName, true, true,
                                           surfaceType, topoType);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_CARET:
         {
            SpecFile sf;
            sf.addToSpecFile(SpecFile::closedTopoFileTag, inputSurfaceName2, "", false);
            sf.addToSpecFile(SpecFile::fiducialCoordFileTag, inputSurfaceName, "", true);
            sf.setAllFileSelections(SpecFile::SPEC_TRUE);
            QString errorMessages;
            brainSet->readSpecFile(sf, "spec-name", errorMessages); 
            if (errorMessages.isEmpty() == false) {
               std::cout << "ERROR: reading coordinate and topology files:" << std::endl;
                  std::cout << "   " << errorMessages.toAscii().constData() << std::endl;
               exit(-1);
            }   
         }
         break;
      case SURFACE_TYPE_FREE_SURFER:
         try {
            AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
            if (inputSurfaceName.right(3) == "asc") {
               format = AbstractFile::FILE_FORMAT_ASCII;
            }
            brainSet->importFreeSurferSurfaceFile(inputSurfaceName, true, true, format,
                                                  surfaceType, topoType);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_FREE_SURFER_PATCH:
         try {
            AbstractFile::FILE_FORMAT format2 = AbstractFile::FILE_FORMAT_BINARY;
            if (inputSurfaceName2.right(3) == "asc") {
               format2 = AbstractFile::FILE_FORMAT_ASCII;
            }
            brainSet->importFreeSurferSurfaceFile(inputSurfaceName2, true, true, format2,
                                                  surfaceType, topoType);
            
            AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
            if (inputSurfaceName.right(3) == "asc") {
               format = AbstractFile::FILE_FORMAT_ASCII;
            }
            brainSet->importFreeSurferSurfaceFile(inputSurfaceName, true, true, format,
                                                  surfaceType, topoType);
            brainModelOfInterest = 1;
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_STL:
         try {
            brainSet->importStlSurfaceFile(inputSurfaceName, true, true,
                                           surfaceType, topoType);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_VTK:
         try {
            brainSet->importVtkSurfaceFile(inputSurfaceName, true, true, false,
                                           surfaceType, topoType);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_XML_VTK:
         try {
            brainSet->importVtkXmlSurfaceFile(inputSurfaceName, true, true, false,
                                              surfaceType, topoType);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_UNKNOWN:
         break;
   }
   
   //
   // Make sure the surface is available
   //
   BrainModelSurface* bms = brainSet->getBrainModelSurface(brainModelOfInterest);
   if (bms == NULL) {
      std::cout << "ERROR: problems reading surface, brain model not found." << std::endl;
      exit(-1);
   }
   
   //
   // Write out the appropriate surface
   //
   switch(outputSurfaceType) {
      case SURFACE_TYPE_BYU:
         try {
            brainSet->exportByuSurfaceFile(bms, outputSurfaceName);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_CARET:
         {
            
            //
            // Get the coordinate and topology files
            //
            CoordinateFile* coordFile = bms->getCoordinateFile();
            TopologyFile* topoFile = bms->getTopologyFile();
            
            //
            // Get the coordinate files spec file type tag
            //
            const QString coordSpecFileTag(
                        BrainModelSurface::getCoordSpecFileTagFromSurfaceType(surfaceType));
             
            //
            // Set the coordinate file's type
            //
            coordFile->setHeaderTag(AbstractFile::headerTagConfigurationID, 
                        BrainModelSurface::getSurfaceConfigurationIDFromType(surfaceType));
                        
            //
            // Get the topo file's spec file tag
            //
            const QString topoSpecFileTag(
                            TopologyFile::getSpecFileTagFromTopologyType(topoType));
             
            //
            // Set the topo file's type
            //
            topoFile->setTopologyType(topoType);
            
            //
            // Write the coordinate file
            //
            try {
               coordFile->setFileWriteType(fileWriteType);
               coordFile->writeFile(outputSurfaceName);
            }
            catch (FileException&) {
               std::cout << "Error writing coord file: " << outputSurfaceName.toAscii().constData() << std::endl;
               exit(-1);
            }
            
            //
            // Write the topology file
            //
            try {
               topoFile->setFileWriteType(fileWriteType);
               topoFile->writeFile(outputSurfaceName2);
            }
            catch (FileException&) {
               std::cout << "Error writing topo file: " << outputSurfaceName2.toAscii().constData() << std::endl;
               exit(-1);
            }
            
            //
            // Update the spec file
            //
            std::vector<QString> tags;
            std::vector<QString> values;
            tags.push_back(topoSpecFileTag);
            values.push_back(outputSurfaceName2);
            tags.push_back(coordSpecFileTag);
            values.push_back(outputSurfaceName);
            updateSpecFile(tags, values);
         }
         break;
      case SURFACE_TYPE_FREE_SURFER:
         try {
            brainSet->exportFreeSurferAsciiSurfaceFile(bms, outputSurfaceName);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_FREE_SURFER_PATCH:
         try {
            brainSet->exportFreeSurferAsciiSurfaceFile(bms, outputSurfaceName);
            brainModelOfInterest = 1;
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_STL:
         try {
            brainSet->exportStlSurfaceFile(bms, outputSurfaceName);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_VTK:
         try {
            brainSet->exportVtkSurfaceFile(bms, outputSurfaceName, false);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_XML_VTK:
         try {
            brainSet->exportVtkXmlSurfaceFile(bms, outputSurfaceName, false);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            exit(-1);
         }
         break;
      case SURFACE_TYPE_UNKNOWN:
         break;
   }   
}

/**
 * Convert a surface type name to an enum.
 */
static SURFACE_FILE_TYPE
getSurfaceFileType(const QString& surfaceTypeName,
                   const QString& inputOutputName)
{
   if (surfaceTypeName == "BYU") {
      return SURFACE_TYPE_BYU;
   }
   if (surfaceTypeName == "CARET") {
      return SURFACE_TYPE_CARET;
   }
   else if (surfaceTypeName == "FSS") {
      return SURFACE_TYPE_FREE_SURFER;
   }
   else if (surfaceTypeName == "FSP") {
      return SURFACE_TYPE_FREE_SURFER_PATCH;
   }
   else if (surfaceTypeName == "STL") {
      return SURFACE_TYPE_STL;
   }
   else if (surfaceTypeName == "VTKP") {
      return SURFACE_TYPE_VTK;
   }
   else if (surfaceTypeName == "VTKXP") {
      return SURFACE_TYPE_XML_VTK;
   }
   else {
      std::cout << "ERROR: invalid " << inputOutputName.toAscii().constData() << " surface type: " 
                << surfaceTypeName.toAscii().constData() << std::endl;
      exit(-1);
   }
   return SURFACE_TYPE_UNKNOWN;
}

static void
volumeConversion(const QString& inputVolumeName, 
                 const QString& outputVolumeName)
{
   try {
      //
      // Read the input file
      //
      std::vector<VolumeFile*> volumes;
      VolumeFile::readFile(inputVolumeName,
                           VolumeFile::VOLUME_READ_SELECTION_ALL,
                           volumes);
                           
      if (volumes.empty() == false) {
         //
         // Write the output file
         //
         VolumeFile::writeFile(outputVolumeName,
                               volumes[0]->getVoxelDataType(),
                               volumes);
                               
         //
         // Free memory
         //
         for (unsigned int i = 0; i < volumes.size(); i++) {
            delete volumes[i];
         }
      }
      else {
         std::cout << "ERROR: no volumes were read successfully." << std::endl;
         std::exit(-1);
      }
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
}

/*
 * Convert a spec file's data files
 */
static void
specFileConvert(const AbstractFile::FILE_FORMAT ft)
{
   SpecFile sf;
   try {
      sf.readFile(specFileName);
      sf.convertAllDataFilesToType(ft, true);
   }
   catch (FileException& e) {
      std::cout << "ERROR " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
}

/*
 * The main function.
 */
int
main(int argc, char* argv[])
{
   if (argc == 1) {
      printHelp(argv[0]);
      exit(0);
   }
   
   QApplication app(argc, argv, false);

   //
   // Create a brain set
   //
#ifdef Q_OS_MAC
   BrainSet::initializeDataFileStaticMembers();
#endif
   brainSet = new BrainSet(true);
   
   QString inputVolumeName;
   QString outputVolumeName;
   
   //
   // This gets the preferred write type from the preferences file
   //
   fileWriteType = AbstractFile::getPreferredWriteType();
   
   //
   // Process the command line options
   //
   int firstFileIndex = -1;
   for (int i = 1; i < argc; i++) {
      const QString arg(argv[i]);
      if (arg == "-info") {
         if (mode == MODE_NONE) {
            mode = MODE_INFO;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            exit(-1);
         }
      }
      else if ((arg == "-h") || (arg == "-help")) {
         printHelp(argv[0]);
         exit(0);
      }
      else if (arg == "-binary") {
         if (mode == MODE_NONE) {
            mode = MODE_BINARY;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
      }
      else if (arg == "-fsc2c") {
         mode = MODE_FREE_SURFER_CURVATURE_TO_CARET;
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName);
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName2);
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, outputSurfaceName);
      }
      else if (arg == "-fsf2c") {
         mode = MODE_FREE_SURFER_FUNCTIONAL_TO_CARET;
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName);
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName2);
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, outputSurfaceName);
      }
      else if (arg == "-fsl2c") {
         mode = MODE_FREE_SURFER_LABEL_TO_CARET;
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName);
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName2);
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, outputSurfaceName);
      }
      else if (arg == "-cp2fsl") {
         mode = MODE_CARET_PAINT_TO_FREE_SURFER_LABEL;
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName);
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName2);
      }
      else if (arg == "-sc") {
         mode = MODE_SURFACE_CONVERT;
      }
      else if (arg == "-is") {
         QString inputTypeName;
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputTypeName);
         inputSurfaceType = getSurfaceFileType(inputTypeName, "input");

         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputSurfaceName);
         switch(inputSurfaceType) {
            case SURFACE_TYPE_BYU:
               break;
            case SURFACE_TYPE_CARET:
               CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, 
                                                     inputSurfaceName2);
               break;
            case SURFACE_TYPE_FREE_SURFER:
               break;
            case SURFACE_TYPE_FREE_SURFER_PATCH:
               CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, 
                                                     inputSurfaceName2);
               break;
            case SURFACE_TYPE_STL:
               break;
            case SURFACE_TYPE_VTK:
               break;
            case SURFACE_TYPE_XML_VTK:
               break;
            case SURFACE_TYPE_UNKNOWN:
               break;
         }
      }
      else if (arg == "-os") {
         QString outputTypeName;
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, outputTypeName);
         outputSurfaceType = getSurfaceFileType(outputTypeName, "output");

         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, outputSurfaceName);
         switch(outputSurfaceType) {
            case SURFACE_TYPE_BYU:
               break;
            case SURFACE_TYPE_CARET:
               CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, 
                                                      outputSurfaceName2);
               CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, 
                                                      outputCoordTypeName);
               CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, 
                                                      outputTopoTypeName);
               break;
            case SURFACE_TYPE_FREE_SURFER:
               break;
            case SURFACE_TYPE_FREE_SURFER_PATCH:
               CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, 
                                                      outputSurfaceName2);
               break;
            case SURFACE_TYPE_STL:
               break;
            case SURFACE_TYPE_VTK:
               break;
            case SURFACE_TYPE_XML_VTK:
               break;
            case SURFACE_TYPE_UNKNOWN:
               break;
         }
      }
      else if (arg == "-spec") {
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, specFileName);
      }
      else if (arg == "-spec-bin") {
         if (mode == MODE_NONE) {
            mode = MODE_SPEC_BINARY;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, specFileName);
      }
      else if (arg == "-spec-text") {
         if (mode == MODE_NONE) {
            mode = MODE_SPEC_TEXT;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, specFileName);
      }
      else if (arg == "-struct") {
         CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, structureName);
      }
      else if (arg == "-outbin") {
         fileWriteType = AbstractFile::FILE_FORMAT_BINARY;
      }
      else if (arg == "-outtext") {
         fileWriteType = AbstractFile::FILE_FORMAT_ASCII;
      }
      else if (arg == "-text") {
         if (mode == MODE_NONE) {
            mode = MODE_TEXT;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
      }
      else if (arg == "-vc") {
         if (mode == MODE_NONE) {
            mode = MODE_VOLUME;
            CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, inputVolumeName);
            CommandLineUtilities::getNextParameter(arg, argc, argv, true, i, outputVolumeName);
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
      }
      else if (arg == "-xml") {
         if (mode == MODE_NONE) {
            mode = MODE_XML;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
      }
      else if (arg == "-xml64") {
         if (mode == MODE_NONE) {
            mode = MODE_XML64;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
      }
      else if (arg == "-xml64GZ") {
         if (mode == MODE_NONE) {
            mode = MODE_XML64GZ;
         }
         else {
            std::cout << "ERROR: More than one mode parameter specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
      }
      else if (arg[0] != '-') {
         firstFileIndex = i;
         break;
      }
      else {
         std::cout << "ERROR: Unrecognized parameter: " << arg.toAscii().constData() << std::endl;
         std::cout << std::endl;
         exit(-1);
      }
   }
   
   std::cout << std::endl;
   std::cout << "INFO: To see command line options, Run with \"-help\"" << std::endl;
   std::cout << std::endl;
   
   if (mode == MODE_NONE) {
      std::cout << "ERROR: You must specify a mode parameter." << std::endl;
      std::cout << std::endl;
      exit(-1);
   }
   
   //
   // Initialize debugging off
   //
   DebugControl::setDebugOn(false);

   //
   // Process any environment variables
   //
   DebugControl::setDebugOnWithEnvironmentVariable("CARET_DEBUG");

   switch (mode) {
      case MODE_NONE:
         break; 
      case MODE_BINARY:
         if (firstFileIndex < 0) {
            std::cout << "ERROR: No files specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         textBinaryConvert(firstFileIndex, argc, argv);
         break;
      case MODE_TEXT:
         if (firstFileIndex < 0) {
            std::cout << "ERROR: No files specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         textBinaryConvert(firstFileIndex, argc, argv);
         break; 
      case MODE_XML:
         if (firstFileIndex < 0) {
            std::cout << "ERROR: No files specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         textBinaryConvert(firstFileIndex, argc, argv);
         break; 
      case MODE_XML64:
         if (firstFileIndex < 0) {
            std::cout << "ERROR: No files specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         textBinaryConvert(firstFileIndex, argc, argv);
         break; 
      case MODE_XML64GZ:
         if (firstFileIndex < 0) {
            std::cout << "ERROR: No files specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         textBinaryConvert(firstFileIndex, argc, argv);
         break; 
      case MODE_INFO:
         if (firstFileIndex < 0) {
            std::cout << "ERROR: No files specified." << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         textBinaryConvert(firstFileIndex, argc, argv);
         break;
      case MODE_SPEC_BINARY:
         specFileConvert(AbstractFile::FILE_FORMAT_BINARY);
         break;
      case MODE_SPEC_TEXT:
         specFileConvert(AbstractFile::FILE_FORMAT_ASCII);
         break;
      case MODE_SURFACE_CONVERT:
         surfaceFileConversion();
         break;
      case MODE_FREE_SURFER_CURVATURE_TO_CARET:
         freeSurferCurvatureToCaretConvert();
         break;
      case MODE_FREE_SURFER_FUNCTIONAL_TO_CARET:
         freeSurferFunctionalToCaretConvert();
         break;
      case MODE_FREE_SURFER_LABEL_TO_CARET:
         freeSurferLabelToCaretConvert();
         break;
      case MODE_CARET_PAINT_TO_FREE_SURFER_LABEL:
         caretPaintToFreeSurferLabel();
         break;
      case MODE_VOLUME:
         volumeConversion(inputVolumeName, outputVolumeName);
         break;
   }
   
   delete brainSet;
   
   return 0;
}

