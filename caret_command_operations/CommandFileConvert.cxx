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

#include <QFileInfo>

#include "BrainSet.h"
#include "BrainModelContours.h"
#include "CommandFileConvert.h"
#include "ContourCellFile.h"
#include "ContourFile.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "FreeSurferCurvatureFile.h"
#include "FreeSurferFunctionalFile.h"
#include "FreeSurferSurfaceFile.h"
#include "GiftiDataArrayFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"

/**
 * constructor.
 */
CommandFileConvert::CommandFileConvert()
   : CommandBase("-file-convert",
                 "FILE CONVERSION")
{
   brainSet = NULL;
   fileWriteType = AbstractFile::FILE_FORMAT_ASCII;
   mode = MODE_NONE;
}

/**
 * destructor.
 */
CommandFileConvert::~CommandFileConvert()
{
   if (brainSet != NULL) {
      delete brainSet;
      brainSet = NULL;
   }
}

/**
 * get the script builder parameters.
 */
void 
CommandFileConvert::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addVariableListOfParameters("commands");
}

/**
 * get full help information.
 */
QString 
CommandFileConvert::getHelpInformation() const
{
   std::vector<AbstractFile::FILE_FORMAT> fileFormats;
   std::vector<QString> fileFormatNames;
   AbstractFile::getFileFormatTypesAndNames(fileFormats, fileFormatNames);
   
   const QString cmdSwitch(getOperationSwitch());
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "\n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "SYNOPSIS for Caret File Format Conversion \n"
       + indent9 + "   " + cmdSwitch + " -format-convert <file-format> \\ \n"
       + indent9 + "      <one-or-more-file-names>  \n"
       + indent9 + " \n"
       + indent9 + "DESCRIPTION for Caret File Format Conversion \n"
       + indent9 + "   Convert Caret data files to the specified formats. \n"
       + indent9 + " \n"
       + indent9 + "   REQUIRED PARAMETERS \n"
       + indent9 + "      <file-format> is any combination of the following\n"
       + indent9 + "      separated by a colon:\n");
   for (unsigned int i = 0; i < fileFormatNames.size(); i++) {
   helpInfo +=
         (indent9 + "         " + fileFormatNames[i] + "\n");
   }
   helpInfo += (""
       + indent9 + "            \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "SYNOPSIS for Caret File Format Information \n"
       + indent9 + "   " + cmdSwitch + " -format-info <one-or-more-file-names> \n"
       + indent9 + " \n"
       + indent9 + "DESCRIPTION for Caret File Format Information \n"
       + indent9 + "   Display the file format of each of the specified files. \n"
       + indent9 + " \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "SYNOPSIS for Converting Data Files in a Spec File \n"
       + indent9 + "   " + cmdSwitch + " -spec-convert <file-format>  spec-file-name \n"
       + indent9 + " \n"
       + indent9 + "DESCRIPTION for Converting Data Files in a Spec File \n"
       + indent9 + "   Converts all of the data files listed in a spec file to the \n"
       + indent9 + "   specified formats. \n"
       + indent9 + " \n"
       + indent9 + "   REQUIRED PARAMETERS \n"
       + indent9 + "      <file-format> is any combination of the following\n"
       + indent9 + "      separated by a colon:\n");
   for (unsigned int i = 0; i < fileFormatNames.size(); i++) {
   helpInfo +=
         (indent9 + "         " + fileFormatNames[i] + "\n");
   }
   helpInfo += (""
       + indent9 + "            \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "SYNOPSIS for Surface File Conversion \n"
       + indent9 + "   " + cmdSwitch + " -sc -is <type> <name> [name2]  \\ \n"
       + indent9 + "       -os <type> <name> [name2] [coord-type] [topo-type] \\ \n"
       + indent9 + "   [-outbin  -outtext] \\ \n"
       + indent9 + "   [-spec spec-file-name] [-struct structure] \n"
       + indent9 + " \n"
       + indent9 + "DESCRIPTION for Convert surface file formats. \n"
       + indent9 + "  \n"
       + indent9 + "   REQUIRED PARAMETERS \n"
       + indent9 + "      -sc  Specifies surface format conversion. \n"
       + indent9 + "       \n"
       + indent9 + "      -is <type> <name> [name2] Specifies the input surface. \n"
       + indent9 + "         type is one of: \n"
       + indent9 + "            BYU  input surface is a BYU surface file \n"
       + indent9 + "               This type is followed by the name of the BYU surface\n"
       + indent9 + "               file.\n"
       + indent9 + "       \n"
       + indent9 + "            CARET  input surface is Caret Coordinate and Topology \n"
       + indent9 + "               files.  This type is followed by the names of the   \n"
       + indent9 + "               Caret Coordinatefile, the Caret Topology file. \n"
       + indent9 + "       \n"
       + indent9 + "            FSS  input surface is a FreeSurfer surface surface file. \n"
       + indent9 + "               This type is followed by the name of the FreeSurfer  \n"
       + indent9 + "               surface file. \n"
       + indent9 + "             \n"
       + indent9 + "            FSP  input surface is a FreeSurfer surface patch file. \n"
       + indent9 + "               This type is followed by the name of the FreeSurfer  \n"
       + indent9 + "               patch file and the name of a corresponding FreeSurfer  \n"
       + indent9 + "               surface file. \n"
       + indent9 + "             \n"
       + indent9 + "            GS   input surface is a GIFTI XML surface file.\n"
       + indent9 + "               This type is followed by the name of the GIFTI\n"
       + indent9 + "               surfacde file.\n"
       + indent9 + "         \n"
       + indent9 + "            STL  input surface is a BYU surface file \n"
       + indent9 + "               This type is followed by the name of the STL surface \n"
       + indent9 + "               file.\n"
       + indent9 + "       \n"
       + indent9 + "            VTKP  input surface is a VTK PolyData file. \n"
       + indent9 + "               This type is followed by the name of the VTK PolyData \n"
       + indent9 + "               file.\n"
       + indent9 + "             \n"
       + indent9 + "            VTKXP  input surface is an XML VTK PolyData file. \n"
       + indent9 + "               This type is followed by the name of the XML VTK \n"
       + indent9 + "               PolyData file.\n"
       + indent9 + "       \n"
       + indent9 + "      -os <type> <name> [name2] [coord-type] [topo-type]    \n"
       + indent9 + "         Specifies the output surface. \n"
       + indent9 + "       \n"
       + indent9 + "         This option's type name and name2 parameters are the same as\n"
       + indent9 + "         for the \"-is\" option.  \n"
       + indent9 + "       \n"
       + indent9 + "         The third and forth parameters are the \n"
       + indent9 + "         coordinate file type and the topology file type. \n"
       + indent9 + "             \n"
       + indent9 + "         In addition to the types listed for input, an\n"
       + indent9 + "         output only type is one of: \n"
       + indent9 + "            OI  Output surface is an Open Inventor file. \n"
       + indent9 + "                 \n"
       + indent9 + "                  Valid topology file types:  \n"
       + indent9 + "                     CLOSED \n"
       + indent9 + "                     OPEN \n"
       + indent9 + "                     CUT \n"
       + indent9 + "                     LOBAR_CUT \n"
       + indent9 + "                     UNKNOWN \n"
       + indent9 + "       \n"
       + indent9 + "                  Valid coordinate file types \n"
       + indent9 + "                     RAW \n"
       + indent9 + "                     FIDUCIAL \n"
       + indent9 + "                     INFLATED \n"
       + indent9 + "                     VERY_INFLATED \n"
       + indent9 + "                     SPHERICAL \n"
       + indent9 + "                     ELLIPSOIDAL \n"
       + indent9 + "                     CMW \n"
       + indent9 + "                     FLAT \n"
       + indent9 + "                     FLAT_LOBAR \n"
       + indent9 + "                     UNKNOWN \n"       
       + indent9 + "       \n"
       + indent9 + "   OPTIONAL PARAMETERS \n"
       + indent9 + "      -outbin  Indicates that output Caret files are to be written in \n"
       + indent9 + "         binary format. \n"
       + indent9 + "       \n"
       + indent9 + "      -outtext  Indicates that output Caret files are to be written in \n"
       + indent9 + "         text (ascii) format. \n"
       + indent9 + "       \n"
       + indent9 + "      -spec spec-file-name   Indicates that output Caret files are to \n"
       + indent9 + "         be written to the spec file <spec-file-name>.  If the spec \n"
       + indent9 + "         file does not exist, it will be created. \n"
       + indent9 + "       \n"
       + indent9 + "      -struct  structure   Indicates the structure that should be \n"
       + indent9 + "         placed into the spec file when writing Caret files. \n"
       + indent9 + "            Valid structure types \n"
       + indent9 + "               right \n"
       + indent9 + "               left \n"
       + indent9 + "               cerebellum \n"
       + indent9 + "       \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "SYNOPSIS for FreeSurfer Data File / Caret Conversion \n"
       + indent9 + "   " + cmdSwitch + " -fsc2c free-surfer-curvature \\ \n"
       + indent9 + "       free-surfer-surface output-surface-shape \\ \n"
       + indent9 + "       [-outbin  -outtext]  \\ \n"
       + indent9 + "       [-spec spec-file-name] [-struct structure]  \n"
       + indent9 + " \n"
       + indent9 + "   " + cmdSwitch + " -fsf2c free-surfer-functional \\ \n"
       + indent9 + "       free-surfer-surface output-metric \\ \n"
       + indent9 + "       [-outbin  -outtext]  \\ \n"
       + indent9 + "       [-spec spec-file-name] [-struct structure]  \n"
       + indent9 + " \n"
       + indent9 + "   " + cmdSwitch + " -cp2fsl caret-paint-file \\ \n"
       + indent9 + "       caret-fiducial-coord-file  \n"
       + indent9 + " \n"
       + indent9 + "   " + cmdSwitch + " -cs2fsc caret-shape-file \\ \n"
       + indent9 + "       shape-column \\ \n"
       + indent9 + "       caret-fiducial-coord-file  \\ \n"
       + indent9 + "       free-surfer-curvature-file\n"
       + indent9 + " \n"
       + indent9 + "DESCRIPTION for FreeSurfer Data File / Caret Conversion \n"
       + indent9 + "   Convert free surfer data files to Caret format. \n"
       + indent9 + " \n"
       + indent9 + "   REQUIRED PARAMETERS \n"
       + indent9 + "       \n"
       + indent9 + "      Note: You must specify one of the following: \n"
       + indent9 + " \n"
       + indent9 + "         " + cmdSwitch + " -fsc2c   \n"
       + indent9 + "            Converts a free surfer curvature file into a Caret surface\n"
       + indent9 + "            shape file.  This option is followed by the name of the  \n"
       + indent9 + "            Free Surfer curvature file, the name of a Free Surfer   \n"
       + indent9 + "            surfacefile (used to get number of nodes), and the output  \n"
       + indent9 + "            Caret surface shape file. \n"
       + indent9 + "             \n"
       + indent9 + "         " + cmdSwitch + " -fsf2c   \n"
       + indent9 + "            Converts a free surfer functional file into a Caret metric\n"
       + indent9 + "            file.  This option is followed by the name of the Free \n"
       + indent9 + "            Surfer functional file, the name of a Free Surfer surface  \n"
       + indent9 + "            file (used to get number of nodes), and the output Caret \n"
       + indent9 + "            metric file. \n"
       + indent9 + "             \n"
       + indent9 + "         " + cmdSwitch + " -fsl2c \n"
       + indent9 + "            Converts all free surfer label files in the specified  \n"
       + indent9 + "            directory into a paint file.  This option is followed by \n"
       + indent9 + "            the directory containing the label files, the name of a \n"
       + indent9 + "            Free Surfer surface file (used to get number of nodes), \n"
       + indent9 + "            and the output Caret paint file. \n"
       + indent9 + "              \n"
       + indent9 + "         " + cmdSwitch + " -cp2fsl \n"
       + indent9 + "            Convert a paint file into free surfer label files.  No  \n"
       + indent9 + "            output name is needed since free surfer uses on file per  \n"
       + indent9 + "            paint ID with the name of the file being \"paintID\" \n"
       + indent9 + "            label.  \n"
       + indent9 + " \n"
       + indent9 + "         " + cmdSwitch + " -cs2fsc \n"
       + indent9 + "            Convert a Caret shape file column into a free\n"
       + indent9 + "            surfer curvature file.\n"
       + indent9 + " \n"
       + indent9 + "   OPTIONAL PARAMETERS for FreeSurfer Data File to Caret Conversion \n"
       + indent9 + " \n"
       + indent9 + "      See the surface conversion options for explanation. \n"
       + indent9 + " \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "SYNOPSIS for Converting Volume Files \n"
       + indent9 + "   " + cmdSwitch + " -vc  <input-volume> <output-volume> \n"
       + indent9 + " \n"
       + indent9 + "DESCRIPTION for Converting Volume Files \n"
       + indent9 + "   The extension of the output volume file name determines the type of \n"
       + indent9 + "    of volume file. \n"
       + indent9 + " \n"
       + indent9 + "   Extension       Type of Volume File \n"
       + indent9 + "   ---------       ------------------- \n"
       + indent9 + "      HEAD         AFNI Volume File \n"
       + indent9 + "      ifh          Washington University Volume File \n"
       + indent9 + "      nii          NIFTI Volume File \n"
       + indent9 + "      nii.gz       Compressed NIFTI Volume File \n"
       + indent9 + " \n"
       + indent9 + " \n"
       + indent9 + "For best results, run this program from the directory containing \n"
       + indent9 + "the files \n"
       + indent9 + " \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "SYNOPSIS for importing contour files \n"
       + indent9 + "   " + cmdSwitch + " -ic  <CONTOUR_TYPE> <input-file-name \n"
       + indent9 + "      <caret-contour-file-name> [caret-contour-cell-file-name] \n"
       + indent9 + " \n"
       + indent9 + "DESCRIPTION for Importing Contour Files \n"
       + indent9 + " \n"
       + indent9 + "   CONTOUR_TYPE    Type of Contour File\n"
       + indent9 + "   ------------    --------------------\n"
       + indent9 + "      MDPLOT       MDPLOT .mdo contour file\n"
       + indent9 + "      NEURO        Neurolucida XML contour file\n"
       + indent9 + " \n"
       + indent9 + "   The \"caret-contour-cell-file-name\" is optional.  If \n"
       + indent9 + "   there are no contour cells, it is not written.\n"
       + indent9 + " \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "---------------------------------------------------------------------- \n"
       + indent9 + "EXAMPLES \n"
       + indent9 + " \n"
       + indent9 + "   Convert all Caret data files in the current directory that support  \n"
       + indent9 + "   binary format into the binary format.  If the file does not \n"
       + indent9 + "   support binary, convert the file to XML format.  If the file \n"
       + indent9 + "   supports neither binary nor XML format, do not change the file:\n"
       + indent9 + "      " + cmdSwitch + " -format-convert BINARY:XML * \n"
       + indent9 + " \n"
       + indent9 + "   Convert the two Caret files into ascii format Caret files: \n"
       + indent9 + "       " + cmdSwitch + " -format-convert ASCII file1.top \\ \n"
       + indent9 + "           file3.coord \n"
       + indent9 + " \n"
       + indent9 + "   List the data type (text or binary) of all Caret files in the  \n"
       + indent9 + "   current directory: \n"
       + indent9 + "       " + cmdSwitch + " -format-info * \n"
       + indent9 + " \n"
       + indent9 + "   Convert the free surfer surface into closed topology and fiducial \n"
       + indent9 + "   coordinate files.  Create a spec file for a left cortex. \n"
       + indent9 + "       " + cmdSwitch + " -sc -is FSS lh.orig.asc -os CARET fiducial.coord \\\n"
       + indent9 + "            closed.topo FIDUCIAL CLOSED -spec file.spec -struct LEFT \n"
       + indent9 + " \n"
       + indent9 + "   Convert the free surfer patch into cut topology and flat \n"
       + indent9 + "   coordinate files.  Create or add to the spec file as left cortex. \n"
       + indent9 + "   Write the coord and topo files in text format. \n"
       + indent9 + "       " + cmdSwitch + " -sc -is FSP lh.oc.flat.asc lh.orig.asc \\ \n"
       + indent9 + "            -os CARET flat.coord cut.topo FLAT CUT -spec file.spec \\ \n"
       + indent9 + "            -struct LEFT -outtext \n"
       + indent9 + " \n"
       + indent9 + "   Convert the free surfer curvature file to a Caret surface shape \n"
       + indent9 + "   file.\n"
       + indent9 + "       " + cmdSwitch + " -fsc2c lh.curv.asc lh.pial.asc \\ \n"
       + indent9 + "            shape.surface_shape -spec file.spec \n"
       + indent9 + "    \n"
       + indent9 + "   Convert the free surfer functional file to a Caret metric file. \n"
       + indent9 + "       " + cmdSwitch + " -fsf2c pho-lh.5.w.asc lh.pial.asc \\ \n"
       + indent9 + "            func.metric -spec file.spec \n"
       + indent9 + "    \n"
       + indent9 + "   Convert the free surfer label files in current directory to a Caret \n"
       + indent9 + "   paint file. \n"
       + indent9 + "       " + cmdSwitch + " -fsl2c  .  lh.pial.asc \\ \n"
       + indent9 + "            label.paint -spec file.spec \n"
       + indent9 + "    \n"
       + indent9 + "   Convert the caret paint file into free surfer label files \n"
       + indent9 + "       " + cmdSwitch + " -cp2fsl  file.paint \\ \n"
       + indent9 + "            fiducial.coord \n"
       + indent9 + "    \n"
       + indent9 + "   Convert the first column in the caret surface shape file \n"
       + indent9 + "   into a Free Surfer curvature file. \n"
       + indent9 + "       " + cmdSwitch + " -cs2fsc Human.surface_shape 1  \\ \n"
       + indent9 + "            Human.FIDUCIAL.coord freesurfer.curv.asc  \n"
       + indent9 + "    \n"
       + indent9 + "   Convert the data files listed in the spec file to binary format. \n"
       + indent9 + "   If binary is not supported for a file, try ASCII.\n"
       + indent9 + "       " + cmdSwitch + " -spec-convert BINARY:ASCII  human.spec \n"
       + indent9 + "    \n"
       + indent9 + "   Convert the volume file from AFNI to compressed NIFTI format. \n"
       + indent9 + "       " + cmdSwitch + " -vc anat+orig.HEAD anat+orig.nii.gz \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandFileConvert::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   brainSet = new BrainSet(true);
   
   QString inputVolumeName;
   QString outputVolumeName;
   
   QString contourType;
   QString contourFileName;
   QString caretContourFileName;
   QString caretContourCellFileName;
   
   //
   // This gets the preferred write type from the preferences file
   //
   fileWriteType = AbstractFile::FILE_FORMAT_OTHER;
   
   //
   // Process the command line options
   //
   std::vector<QString> dataFileNames;
   QString dataFileFormatList;
   while (parameters->getParametersAvailable()) {
      const QString arg(parameters->getNextParameterAsString("Parameter"));
      if (arg == "-format-convert") {
         if (mode == MODE_NONE) {
            mode = MODE_FORMAT_CONVERT;
         }
         else {
            throw CommandException("More than one mode parameter specified.");
            std::cout << std::endl;
         }
         
         dataFileFormatList = parameters->getNextParameterAsString("File Format");
         
         while (parameters->getParametersAvailable()) {
            dataFileNames.push_back(parameters->getNextParameterAsString("Data File Name"));
         }
      }
      else if (arg == "-fsc2c") {
         mode = MODE_FREE_SURFER_CURVATURE_TO_CARET;
         inputSurfaceName = parameters->getNextParameterAsString("Input Surface Name");
         inputSurfaceName2 = parameters->getNextParameterAsString("Input Surface Name 2");
         outputSurfaceName = parameters->getNextParameterAsString("Output Surface Name");
      }
      else if (arg == "-fsf2c") {
         mode = MODE_FREE_SURFER_FUNCTIONAL_TO_CARET;
         inputSurfaceName = parameters->getNextParameterAsString("Input Surface Name");
         inputSurfaceName2 = parameters->getNextParameterAsString("Input Surface Name 2");
         outputSurfaceName = parameters->getNextParameterAsString("Output Surface Name");
      }
      else if (arg == "-fsl2c") {
         mode = MODE_FREE_SURFER_LABEL_TO_CARET;
         inputSurfaceName = parameters->getNextParameterAsString("Input Surface Name");
         inputSurfaceName2 = parameters->getNextParameterAsString("Input Surface Name 2");
         outputSurfaceName = parameters->getNextParameterAsString("Output Surface Name");
      }
      else if (arg == "-cp2fsl") {
         mode = MODE_CARET_PAINT_TO_FREE_SURFER_LABEL;
         inputSurfaceName = parameters->getNextParameterAsString("Input Surface Name");
         inputSurfaceName2 = parameters->getNextParameterAsString("Input Surface Name 2");
      }
      else if (arg == "-cs2fsc") {
         mode = MODE_CARET_SHAPE_TO_FREE_SURFER_CURVATURE;
         inputShapeName = parameters->getNextParameterAsString("Input Caret Shape File");
         inputShapeColumn = parameters->getNextParameterAsString("Input Caret Shape File Column");
         inputSurfaceName = parameters->getNextParameterAsString("Input Fiducial Coordinate Name");
         outputCurvatureName = parameters->getNextParameterAsString("Output Free Surfer Curvature Name");

      }
      else if (arg == "-sc") {
         mode = MODE_SURFACE_CONVERT;
      }
      else if (arg == "-format-info") {
         if (mode == MODE_NONE) {
            mode = MODE_FORMAT_INFO;
         }
         else {
            throw CommandException("More than one mode parameter specified.");
         }
         while (parameters->getParametersAvailable()) {
            dataFileNames.push_back(parameters->getNextParameterAsString("Data File Name"));
         }
      }
      else if (arg == "-ic") {
         if (mode == MODE_NONE) {
            mode = MODE_CONTOUR_CONVERT;
         }
         else {
            throw CommandException("More than one mode parameter specified.");
         }
         contourType = parameters->getNextParameterAsString("Contour Type");
         contourFileName = parameters->getNextParameterAsString("Contour File Name");
         caretContourFileName = parameters->getNextParameterAsString("Caret Contour File Name");
         caretContourCellFileName = parameters->getNextParameterAsString("Caret Contour Cell File Name");
      }
      else if (arg == "-is") {
         QString inputTypeName;
         inputTypeName = parameters->getNextParameterAsString("Input Type Name");
         inputSurfaceType = getSurfaceFileType(inputTypeName, "input");
         inputSurfaceName = parameters->getNextParameterAsString("Input Surface Name");
         switch(inputSurfaceType) {
            case SURFACE_TYPE_BYU:
               break;
            case SURFACE_TYPE_CARET:
               inputSurfaceName2 = parameters->getNextParameterAsString("Input Surface Name 2");
               break;
            case SURFACE_TYPE_FREE_SURFER:
               break;
            case SURFACE_TYPE_FREE_SURFER_PATCH:
               inputSurfaceName2 = parameters->getNextParameterAsString("Input Surface Name 2");
               break;
            case SURFACE_TYPE_GIFTI:
               break;
            case SURFACE_TYPE_OPEN_INVENTOR:
               throw CommandException("Open Inventor not supported for input.");
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
         outputTypeName = parameters->getNextParameterAsString("Output Type Name");
         outputSurfaceType = getSurfaceFileType(outputTypeName, "output");

         outputSurfaceName = parameters->getNextParameterAsString("Output Surface Name");
         switch(outputSurfaceType) {
            case SURFACE_TYPE_BYU:
               break;
            case SURFACE_TYPE_CARET:
               outputSurfaceName2 = parameters->getNextParameterAsString("Output Surface Name 2");
               outputCoordTypeName = parameters->getNextParameterAsString("Output Coord Type Name");
               outputTopoTypeName = parameters->getNextParameterAsString("Output Topo Type Name");
               break;
            case SURFACE_TYPE_FREE_SURFER:
               break;
            case SURFACE_TYPE_FREE_SURFER_PATCH:
               outputSurfaceName2 = parameters->getNextParameterAsString("Output Surface Name 2");
               break;
            case SURFACE_TYPE_GIFTI:
               break;
            case SURFACE_TYPE_OPEN_INVENTOR:
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
         specFileName = parameters->getNextParameterAsString("Spec File Name");
      }
      else if (arg == "-spec-convert") {
         if (mode == MODE_NONE) {
            mode = MODE_SPEC_CONVERT;
         }
         else {
            throw CommandException("More than one mode parameter specified.");
         }
         
         dataFileFormatList = parameters->getNextParameterAsString("File Format");
         specFileName = parameters->getNextParameterAsString("Spec File Name");
      }
      else if (arg == "-struct") {
         structureName = parameters->getNextParameterAsString("Structure").toLower();
      }
      else if (arg == "-outbin") {
         fileWriteType = AbstractFile::FILE_FORMAT_BINARY;
      }
      else if (arg == "-outtext") {
         fileWriteType = AbstractFile::FILE_FORMAT_ASCII;
      }
      else if (arg == "-outxml") {
         fileWriteType = AbstractFile::FILE_FORMAT_XML;
      }
      else if (arg == "-vc") {
         if (mode == MODE_NONE) {
            mode = MODE_VOLUME;
            inputVolumeName = parameters->getNextParameterAsString("Input Volume Name");
            outputVolumeName = parameters->getNextParameterAsString("Input Volume Name");
         }
         else {
            throw CommandException("More than one mode parameter specified.");
         }
      }
      else {
         throw CommandException("Unrecognized parameter: " + arg);
      }
   }
   
   if (mode == MODE_NONE) {
      throw CommandException("You must specify a mode parameter.");
   }
   
   const int numberOfDataFiles = static_cast<int>(dataFileNames.size());
   
   switch (mode) {
      case MODE_NONE:
         break; 
      case MODE_CONTOUR_CONVERT:
         contourConversion(contourType,
                           contourFileName,
                           caretContourFileName,
                           caretContourCellFileName);
         break;
      case MODE_FORMAT_CONVERT:
         if (numberOfDataFiles <= 0) {
            throw CommandException("No files specified.");
         }
         fileFormatConvert(dataFileNames, dataFileFormatList);
         break;
      case MODE_FORMAT_INFO:
         if (numberOfDataFiles <= 0) {
            throw CommandException("No files specified.");
         }
         fileFormatConvert(dataFileNames, "INFO");
         break;
      case MODE_SPEC_CONVERT:
         specFileConvert(dataFileFormatList, specFileName);
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
      case MODE_CARET_SHAPE_TO_FREE_SURFER_CURVATURE:
         caretShapeToFreeSurferCurvature();
         break;
      case MODE_VOLUME:
         volumeConversion(inputVolumeName, outputVolumeName);
         break;
   }
}

/**
 * Convert a contour file.
 */
void 
CommandFileConvert::contourConversion(const QString& contourType,
                                      const QString& contourFileName,
                                      const QString& caretContourFileName,
                                      const QString& caretContourCellFileName) throw (CommandException)
{
   if (caretContourFileName.isEmpty()) {
      throw CommandException("Caret contour file name is empty.");
   }
   
   try {
      //
      // Import contours
      //
      BrainSet bs;
      if (contourType == "MDPLOT") {
         bs.importMDPlotFile(contourFileName,
                             true,
                             true,
                             false,
                             false);
      }
      else if (contourType == "NEURO") {
         bs.importNeurolucidaFile(contourFileName,
                                  true,
                                  true,
                                  false,
                                  false);
      }
      
      //
      // Write contours
      //
      BrainModelContours* bmc = bs.getBrainModelContours();
      if (bmc == NULL) {
         throw CommandException("Contours import failed.");
      }
      ContourFile* cf = bmc->getContourFile();
      if (cf->getNumberOfContours() <= 0) {
         throw CommandException("File read but no contours found.");
      }
      bs.writeContourFile(caretContourFileName, cf);
      
      //
      // Write contour cells
      //
      if (caretContourCellFileName.isEmpty() == false) {
         ContourCellFile* cells = bs.getContourCellFile();
         if (cells->getNumberOfCells() > 0) {
            bs.writeContourCellFile(caretContourCellFileName);
         }
      }
   }
   catch (FileException& e) {
      throw (CommandException(e));
   }
}

/**
 * Convert caret files between binary and text format or display 
 * info about the file.
 */
void 
CommandFileConvert::fileFormatConvert(const std::vector<QString>& dataFileNames,
                                      const QString& dataFileFormatList) throw (CommandException)
{
   //
   // Convert the data format list to file types
   //
   const QStringList sl = dataFileFormatList.split(':');
   const int numFormats = sl.count();
   if (numFormats < 1) {
      throw CommandException("No file formats provided for file format conversion.");
   }
   
   //
   // Just Info ?
   //
   std::vector<AbstractFile::FILE_FORMAT> conversionFormats;
   bool infoFlag = false;
   if ((numFormats == 1) &&
       (sl.at(0) == "INFO")) {
      infoFlag = true;
   }
   else {
      for (int i = 0; i < numFormats; i++) {
         const QString formatName(sl.at(i));
         bool valid = false;
         const AbstractFile::FILE_FORMAT format = 
            AbstractFile::convertFormatNameToType(formatName, &valid);
         if (valid) {
            conversionFormats.push_back(format);
         }
         else {
            throw CommandException("Invalid file format name \""
                                   + formatName
                                   + "\"");
         }
      }
   }

   //
   // Process the files
   //
   const int numberOfDataFiles = static_cast<int>(dataFileNames.size());
   if (numberOfDataFiles < 1) {
      throw CommandException("No files provided for conversion.");
   }
   for (int i = 0; i < numberOfDataFiles; i++) {
      const QString filename(dataFileNames[i]);
      QString errorMessage;
      
      //
      // Skip directories and symbolic links
      //
      QFileInfo fileInfo(filename);
      if (fileInfo.isDir() ||
          fileInfo.isSymLink()) {
         continue;
      }
      
      //
      // Read the header of the file
      //
      AbstractFile* af = AbstractFile::readAnySubClassDataFile(filename, true, errorMessage);
      
      std::cout << FileUtilities::basename(filename).toAscii().constData() << ": ";
      if (af == NULL) {
         std::cout << "unable to read file or not a caret data file.\n"
                   << "error: " 
                   << errorMessage.toAscii().constData();
      }
      else if (af->getFileHasHeader() == false) {
         std::cout << "file does not have header.";
      }
      else {
         //
         // Get files current encoding
         //
         const QString formatString(af->getHeaderTag(AbstractFile::headerTagEncoding));
         bool validFormatFlag = false;
         const AbstractFile::FILE_FORMAT dataFileFormat = 
            AbstractFile::convertFormatNameToType(formatString, &validFormatFlag);
            
         if (infoFlag) {
            if (validFormatFlag == false) {
               std::cout << "unrecognized format: "
                         << formatString.toAscii().constData(); 
            }
            else {
               std::cout << formatString.toAscii().constData() << ".";
            }
         }
         else {
            bool fileProcessedFlag = false;
            
            for (unsigned int i = 0; i < conversionFormats.size(); i++) {
               if (validFormatFlag &&
                   (dataFileFormat == conversionFormats[i])) {
                  std::cout << " already in "
                            << formatString.toAscii().constData()
                            << " format";
                  fileProcessedFlag = true;
               }
               else if (af->getCanWrite(conversionFormats[i])) {
                  try {
                     af->readFile(filename);
                     af->setFileWriteType(conversionFormats[i]);
                     af->writeFile(filename);
                     std::cout << "converted to "
                               << AbstractFile::convertFormatTypeToName(conversionFormats[i]).toAscii().constData()
                               << ".";
                  }
                  catch (FileException& e) {
                     std::cout << "error converting or writing.";
                  }
                  fileProcessedFlag = true;
               }
               if (fileProcessedFlag) {
                  break;
               }
            }
            
            if (fileProcessedFlag == false) {
               std::cout << " does not support specified formats.";
            }
         }
      }
      
      if (af != NULL) {
        delete af;  // can't delete ?? compiler bug ??
      }
      
      std::cout << std::endl;
   }
}
      
/**
 * Write/Update a spec file.
 */
void 
CommandFileConvert::updateSpecFile(const std::vector<QString>& tags, 
                                   const std::vector<QString>& values) throw (CommandException)
{
   try {
      //
      // If the user specified a spec file
      //
      if (specFileName.isEmpty() == false) {
         //
         // Try reading the spec file but ignore errors since it may not exist
         //
         SpecFile specFile;
         specFile.readFile(specFileName);
         
         //
         // Add the spec file tags
         //
         for (int i = 0; i < static_cast<int>(tags.size()); i++) {
            specFile.addToSpecFile(tags[i], values[i], "", false);
         }
         
         //
         // Set the structure
         //
         if (structureName.isEmpty() == false) {
            specFile.setStructure(structureName);
         }
         
         //
         // Write the spec file
         //
         specFile.writeFile(specFileName);
      }
   }
   catch (FileException& e) {
      throw CommandException(e);
   }
}

/**
 * Convert a free surfer curvature file to caret surface shape.
 */
void 
CommandFileConvert::freeSurferCurvatureToCaretConvert() throw (CommandException)
{
   try {
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
      freeSurferSurfaceFile.setFileReadType(format);
      freeSurferSurfaceFile.readFile(freeSurfaceSurfaceName);

      //
      // convert the free surface curvature file to the surface shape file.
      //   
      SurfaceShapeFile shapeFile;
      format = AbstractFile::FILE_FORMAT_BINARY;
      if (freeSurfaceCurvatureName.right(3) == "asc") {
         format = AbstractFile::FILE_FORMAT_ASCII;
      }
      shapeFile.importFreeSurferCurvatureFile(freeSurferSurfaceFile.getNumberOfVertices(),
                                              freeSurfaceCurvatureName,
                                              format);
      
      //
      // Write the surface shape file
      //
      if (fileWriteType != AbstractFile::FILE_FORMAT_OTHER) {
         shapeFile.setFileWriteType(fileWriteType);
      }
      shapeFile.writeFile(shapeName);
      
      //
      // Add the surface shape files to the spec file
      //
      std::vector<QString> tags;
      std::vector<QString> values;
      tags.push_back(SpecFile::surfaceShapeFileTag);
      values.push_back(shapeName);
      updateSpecFile(tags, values);
   }
   catch (FileException& e) {
      throw CommandException(e);
   }
}

/**
 * Convert a caret shape column to free surfer curvature file.
 */
void 
CommandFileConvert::caretShapeToFreeSurferCurvature() throw (CommandException)
{
   try {
      //
      // Load the shape file
      //
      SurfaceShapeFile shapeFile;
      shapeFile.readFile(inputShapeName);
      const int columnNumber = shapeFile.getColumnFromNameOrNumber(inputShapeColumn, false);

      //
      // Load the coordinate file
      //
      CoordinateFile coordFile;
      coordFile.readFile(inputSurfaceName);
      
      //
      // Export to free surfer
      //
      shapeFile.exportFreeSurferAsciiCurvatureFile(columnNumber,
                                                   &coordFile,
                                                   outputCurvatureName);
   }
   catch (FileException& e) {
      throw CommandException(e.whatQString());
   }   
}
      
/**
 * Convert a caret paint file to free surfer label file.
 */
void
CommandFileConvert::caretPaintToFreeSurferLabel() throw (CommandException)
{
   try {
      //
      // Load the paint file
      //
      PaintFile paintFile;
      paintFile.readFile(inputSurfaceName);

      //
      // Load the coordinate file
      //
      CoordinateFile coordFile;
      coordFile.readFile(inputSurfaceName2);
      
      //
      // Export to label files
      //
      for (int i = 0; i < paintFile.getNumberOfColumns(); i++) {
         paintFile.exportFreeSurferAsciiLabelFile(i,
                                                  "",
                                                  &coordFile);
      }
   }
   catch (FileException& e) {
      throw CommandException(e);
   }
}

/**
 * Convert a free surfer functional file to caret metric.
 */
void 
CommandFileConvert::freeSurferLabelToCaretConvert() throw (CommandException)
{
   try {
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
      freeSurferSurfaceFile.setFileReadType(format);
      freeSurferSurfaceFile.readFile(freeSurfaceSurfaceName);

      //
      // convert the free surface label files to the paint file.
      //   
      PaintFile paintFile;
      paintFile.importFreeSurferAsciiLabelFile(freeSurferSurfaceFile.getNumberOfVertices(),
                                                freeSurfaceLabelDirName,
                                                NULL,
                                                true);

      //
      // Write the paint file
      //
      if (fileWriteType != AbstractFile::FILE_FORMAT_OTHER) {
         paintFile.setFileWriteType(fileWriteType);
      }
      paintFile.writeFile(paintName);

      //
      // Add the surface shape files to the spec file
      //
      std::vector<QString> tags;
      std::vector<QString> values;
      tags.push_back(SpecFile::paintFileTag);
      values.push_back(paintName);
      updateSpecFile(tags, values);
   }
   catch (FileException& e) {
      throw CommandException(e);
   }
}

/**
 * Convert a free surfer functional file to caret metric.
 */
void 
CommandFileConvert::freeSurferFunctionalToCaretConvert() throw (CommandException)
{
   try {
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
      freeSurferSurfaceFile.setFileReadType(format);
      freeSurferSurfaceFile.readFile(freeSurfaceSurfaceName);

      //
      // convert the free surface functional file to the metric file.
      //   
      MetricFile metricFile;
      format = AbstractFile::FILE_FORMAT_BINARY;
      if (freeSurfaceFunctionalName.right(3) == "asc") {
         format = AbstractFile::FILE_FORMAT_ASCII;
      }
      metricFile.importFreeSurferFunctionalFile(freeSurferSurfaceFile.getNumberOfVertices(),
                                                freeSurfaceFunctionalName,
                                                format);
      
      //
      // Write the surface shape file
      //
      if (fileWriteType != AbstractFile::FILE_FORMAT_OTHER) {
         metricFile.setFileWriteType(fileWriteType);
      }
      metricFile.writeFile(metricName);
      
      //
      // Add the surface shape files to the spec file
      //
      std::vector<QString> tags;
      std::vector<QString> values;
      tags.push_back(SpecFile::metricFileTag);
      values.push_back(metricName);
      updateSpecFile(tags, values);
   }
   catch (FileException& e) {
      throw CommandException(e);
   }
}

/**
 * Convert a surface file to another type.
 */
void 
CommandFileConvert::surfaceFileConversion() throw (CommandException)
{
   try {
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
            brainSet->importByuSurfaceFile(inputSurfaceName, true, true,
                                              surfaceType, topoType);
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
                  throw CommandException("Reading coordinate and topology files:"  
                                         + errorMessages);
               }   
            }
            break;
         case SURFACE_TYPE_FREE_SURFER:
            {
               AbstractFile::FILE_FORMAT format = AbstractFile::FILE_FORMAT_BINARY;
               if (inputSurfaceName.right(3) == "asc") {
                  format = AbstractFile::FILE_FORMAT_ASCII;
               }
               brainSet->importFreeSurferSurfaceFile(inputSurfaceName, true, true, format,
                                                     surfaceType, topoType);
            }
            break;
         case SURFACE_TYPE_FREE_SURFER_PATCH:
            {
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
            break;
         case SURFACE_TYPE_GIFTI:
            brainSet->readSurfaceFile(inputSurfaceName,
                                      surfaceType,
                                      false,
                                      true,
                                      false);
            break;
         case SURFACE_TYPE_OPEN_INVENTOR:
            throw CommandException("Open Inventor not supported for input.");
            break;
         case SURFACE_TYPE_STL:
            brainSet->importStlSurfaceFile(inputSurfaceName, true, true,
                                              surfaceType, topoType);
            break;
         case SURFACE_TYPE_VTK:
            brainSet->importVtkSurfaceFile(inputSurfaceName, true, true, false,
                                              surfaceType, topoType);
            break;
         case SURFACE_TYPE_XML_VTK:
            brainSet->importVtkXmlSurfaceFile(inputSurfaceName, true, true, false,
                                                 surfaceType, topoType);
            break;
         case SURFACE_TYPE_UNKNOWN:
            break;
      }
      
      //
      // Make sure the surface is available
      //
      BrainModelSurface* bms = brainSet->getBrainModelSurface(brainModelOfInterest);
      if (bms == NULL) {
         throw CommandException("problems reading surface, brain model not found.");
      }
      
      //
      // Write out the appropriate surface
      //
      switch(outputSurfaceType) {
         case SURFACE_TYPE_BYU:
            brainSet->exportByuSurfaceFile(bms, outputSurfaceName);
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
               if (fileWriteType != AbstractFile::FILE_FORMAT_OTHER) {
                  coordFile->setFileWriteType(fileWriteType);
               }
               coordFile->writeFile(outputSurfaceName);
               
               //
               // Write the topology file
               //
               if (fileWriteType != AbstractFile::FILE_FORMAT_OTHER) {
                  topoFile->setFileWriteType(fileWriteType);
               }
               topoFile->writeFile(outputSurfaceName2);
               
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
            brainSet->exportFreeSurferAsciiSurfaceFile(bms, outputSurfaceName);
            break;
         case SURFACE_TYPE_FREE_SURFER_PATCH:
            brainSet->exportFreeSurferAsciiSurfaceFile(bms, outputSurfaceName);
            brainModelOfInterest = 1;
            break;
         case SURFACE_TYPE_GIFTI:
            brainSet->writeSurfaceFile(outputSurfaceName,
                                       surfaceType,
                                       bms,
                                       false);
            break;
         case SURFACE_TYPE_OPEN_INVENTOR:
            brainSet->exportInventorSurfaceFile(bms, outputSurfaceName);
            break;
         case SURFACE_TYPE_STL:
            brainSet->exportStlSurfaceFile(bms, outputSurfaceName);
            break;
         case SURFACE_TYPE_VTK:
            brainSet->exportVtkSurfaceFile(bms, outputSurfaceName, false);
            break;
         case SURFACE_TYPE_XML_VTK:
            brainSet->exportVtkXmlSurfaceFile(bms, outputSurfaceName, false);
            break;
         case SURFACE_TYPE_UNKNOWN:
            break;
      }   
   }
   catch (FileException& e) {
      throw CommandException(e);
   }
}

/**
 * Convert a surface type name to an enum.
 */
CommandFileConvert::SURFACE_FILE_TYPE 
CommandFileConvert::getSurfaceFileType(const QString& surfaceTypeName,
                                       const QString& inputOutputName)
                                           throw (CommandException)
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
   else if (surfaceTypeName == "GS") {
      return SURFACE_TYPE_GIFTI;
   }
   else if (surfaceTypeName == "OI") {
      return SURFACE_TYPE_OPEN_INVENTOR;
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
      throw CommandException("Invalid " 
                             + inputOutputName
                             + " surface type: " 
                             + surfaceTypeName);
   }
   return SURFACE_TYPE_UNKNOWN;
}

/**
 * convert a volume.
 */
void 
CommandFileConvert::volumeConversion(const QString& inputVolumeName, 
                                     const QString& outputVolumeName) throw (CommandException)
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
         throw FileException("No volumes were read successfully.");
      }
   }
   catch (FileException& e) {
      throw CommandException(e);
   }

}

/**
 * Convert a spec file's data files.
 */
void 
CommandFileConvert::specFileConvert(const QString& dataFileFormatList,
                                    const QString& specFileName) throw (CommandException)
{
   std::vector<AbstractFile::FILE_FORMAT> conversionFormats;
   const QStringList sl = dataFileFormatList.split(':');
   for (int i = 0; i < sl.count(); i++) {
      const QString formatName(sl.at(i));
      bool valid = false;
      const AbstractFile::FILE_FORMAT format = 
         AbstractFile::convertFormatNameToType(formatName, &valid);
      if (valid) {
         conversionFormats.push_back(format);
      }
      else {
         throw CommandException("Invalid file format name \""
                                + formatName
                                + "\"");
      }
   }
   try {
      SpecFile sf;
      sf.readFile(specFileName);
      sf.convertAllDataFilesToType(conversionFormats, true);
   }
   catch (FileException& e) {
      throw CommandException(e);
   }
}      

