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

#include "CommandVolumeFslToVector.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TransformationMatrixFile.h"
#include "VectorFile.h"
#include "VolumeFile.h"
/**
 * constructor.
 */
CommandVolumeFslToVector::CommandVolumeFslToVector()
   : CommandBase("-volume-fsl-to-vector",
                 "VOLUME FSL TO VECTOR")
{
}

/**
 * destructor.
 */
CommandVolumeFslToVector::~CommandVolumeFslToVector()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeFslToVector::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input XYZ Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Input Magnitude Volume File Name", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Input FSL Diffusion to Structureal Matrix File Name", "*.mat");
   paramsOut.addFile("Volume in Output Stereotaxic Space", FileFilters::getVolumeGenericFileFilter());
   paramsOut.addFile("Output Vector File Name", FileFilters::getGiftiVectorFileFilter());
   paramsOut.addVariableListOfParameters("", "");
}

/**
 * get full help information
 */
QString 
CommandVolumeFslToVector::getHelpInformation() const
{
   float rgba[4];
   VectorFile::getDefaultColorRGBA(rgba);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-xyz-volume-file-name>\n"
       + indent9 + "<input-magnitude-volume-file-name>\n"
       + indent9 + "<input-fsl-diffusion-to-structural-matrix>\n"
       + indent9 + "<volume-in-output-stereotaxic-space-file-name>\n"
       + indent9 + "<output-vector-file-name>\n"
       + indent9 + "[-output-matrix  output-transformation-matrix-file-name]\n"
       + indent9 + "[-rgb  red  green  blue]\n"
       + indent9 + "\n"
       + indent9 + "Create a vector file from a vector volume file and .\n"
       + indent9 + "a magnitude volume file.  All of the volumes must have \n"
       + indent9 + "the same IJK dimensions.  A vector is created for each\n"
       + indent9 + "voxel whose magnitude is greater than zero.\n"
       + indent9 + "\n"
       + indent9 + "\"input-xyz-volume-file-name\" is a volume file that\n"
       + indent9 + "contains three sub-volumes containing the X, Y, and Z\n"
       + indent9 + "components of a unit vector.\n"
       + indent9 + "\n"
       + indent9 + "\"input-magnitude-volume-file-name\" is a volume \n"
       + indent9 + "containing the magnitude of the vectors.\n"
       + indent9 + "\n"
       + indent9 + "\"volume-in-output-stereotaxic-space-file-name\" is a \n"
       + indent9 + "volume whose stereotaxic coordinates are in the \n"
       + indent9 + "destination space.\n"
       + indent9 + "\n"
       + indent9 + "\"input-fsl-diffusion-to-structural-matrix\" is the FSL\n"
       + indent9 + "diffusion space to structural space matrix file, typically\n"
       + indent9 + "named \"diff2str.mat\".  It contains a 4x4 transformation\n"
       + indent9 + "matrix with each row of the matrix on a separate line.\n"
       + indent9 + "\n"
       + indent9 + "The file name extension for a vector file is \"\n"
       + indent9 + SpecFile::getGiftiVectorFileExtension() + "\".\n"
       + indent9 + "\n"
       + indent9 + "If \"-output-matrix\" is specified, the matrix used to \n"
       + indent9 + "transform from the input volume to output volume space\n"
       + indent9 + "is placed in the output transformation matrix file.\n"
       + indent9 + "\n"
       + indent9 + "Use the \"-rgb\" option to set the colors of the  \n"
       + indent9 + "vectors.  The red, green, and blue components are\n"
       + indent9 + "specified as floating point number swith a range of \n"
       + indent9 + "zero to one.  The default color is "
       + indent9 + "(" + QString::number(rgba[0])
                 + ", " + QString::number(rgba[1])
                 + ", " + QString::number(rgba[2]) + ")\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeFslToVector::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   float rgba[4];
   VectorFile::getDefaultColorRGBA(rgba);

   QString outputMatrixFileName;

   const QString inputXYZVolumeFileName =
      parameters->getNextParameterAsString("Input XYZ Volume File Name");
   const QString inputMagnitudeVolumeFileName =
      parameters->getNextParameterAsString("Input Magnitude Volume File Name");
   const QString fslTransformationMatrixFileName =
      parameters->getNextParameterAsString("FSL Diffusion to Structural Matrix File Name");
   const QString outputSpaceVolumeFileName =
      parameters->getNextParameterAsString("Volume in Output Stereotaxic Space File Name");
   const QString outputVectorFileName =
      parameters->getNextParameterAsString("Output Vector File Name");
   while (parameters->getParametersAvailable()) {
      const QString param = parameters->getNextParameterAsString("Optional Parameter");
      if (param == "-output-matrix") {
         outputMatrixFileName =
                 parameters->getNextParameterAsString("Output Matrix File Name");
      }
      else if (param == "-rgb") {
         rgba[0] = parameters->getNextParameterAsFloat("Red Color Component");
         rgba[1] = parameters->getNextParameterAsFloat("Green Color Component");
         rgba[2] = parameters->getNextParameterAsFloat("Blue Color Component");
      }
      else {
         throw CommandException("Unrecognized parameter: " + param);
      }
   }

   //
   // Read the input files
   //
   TransformationMatrixFile transformMatrixFile;
   transformMatrixFile.readFile(fslTransformationMatrixFileName);
   TransformationMatrix* fslMatrix = transformMatrixFile.getTransformationMatrix(0);
   if (fslMatrix == NULL) {
      throw CommandException("Unable to find a matrix in FSL Matrix File.");
   }

   std::vector<VolumeFile*> xyzVolumes;
   VolumeFile::setVolumeSpace(VolumeFile::VOLUME_SPACE_VOXEL_NATIVE);  // DO NOT make LPI
   VolumeFile::readFile(inputXYZVolumeFileName,
                        -1,
                        xyzVolumes);
   if (xyzVolumes.size() != 3) {
      throw CommandException(inputXYZVolumeFileName +
                             " must contain exactly three subvolumes.");
   }
   VolumeFile magnitudeVolume;
   VolumeFile::setVolumeSpace(VolumeFile::VOLUME_SPACE_VOXEL_NATIVE);  // DO NOT make LPI
   magnitudeVolume.readFile(inputMagnitudeVolumeFileName);

   VolumeFile outputSpaceVolumeFile;
   outputSpaceVolumeFile.readFile(outputSpaceVolumeFileName);

   TransformationMatrix outputTransformationMatrix;
   VectorFile* vf = VectorFile::createVectorFileFromFSLVectorVolumes(
                        *(xyzVolumes[0]),
                        *(xyzVolumes[1]),
                        *(xyzVolumes[2]),
                        magnitudeVolume,
                        outputSpaceVolumeFile,
                        *fslMatrix,
                        outputTransformationMatrix,
                        rgba);
   
   vf->writeFile(outputVectorFileName);

   if (outputMatrixFileName.isEmpty() == false) {
      TransformationMatrixFile tm;
      tm.addTransformationMatrix(outputTransformationMatrix);
      tm.writeFile(outputMatrixFileName);
   }
   
   delete vf;
}

      

