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

#include "BrainModelVolumeSegmentationStereotaxic.h"
#include "BrainSet.h"
#include "CommandVolumeSegmentationStereotaxicSpace.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SegmentationMaskListFile.h"
#include "SpecFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeSegmentationStereotaxicSpace::CommandVolumeSegmentationStereotaxicSpace()
   : CommandBase("-volume-segment-stereo-space",
                 "VOLUME SEGMENTATION STEREOTAXIC SPACE")
{
   const QString maskVolumesDirectory(BrainSet::getCaretHomeDirectory()
                                      + "/data_files/segmentation_masks/");
   maskVolumeListFileName = maskVolumesDirectory
                            + "mask_list.txt.csv";
}

/**
 * destructor.
 */
CommandVolumeSegmentationStereotaxicSpace::~CommandVolumeSegmentationStereotaxicSpace()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeSegmentationStereotaxicSpace::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Anatomical Volume File Name", FileFilters::getVolumeAnatomyFileFilter());
   paramsOut.addFile("Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandVolumeSegmentationStereotaxicSpace::getHelpInformation() const
{
   SegmentationMaskListFile maskVolumeListFile;
   try {
      maskVolumeListFile.readFile(maskVolumeListFileName);
   }
   catch (FileException&) {
   }
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-anatomical-volume-file-name>\n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "[-ecv]\n"
       + indent9 + "[-ecs]\n"
       + indent9 + "[-flat]\n"
       + indent9 + "[-mp]\n"
       + indent9 + "[-noeye]\n"
       + indent9 + "\n"
       + indent9 + "Perform automatic segmentation of an anatomical volume\n"
       + indent9 + "in a known stereotaxic space.\n"
       + indent9 + "\n"
       + indent9 + " \n"
       + indent9 + "      THIS IS EXPERIMENTAL BUT IT WILL PRODUCE A SURFACE \n"
       + indent9 + " \n"
       + indent9 + "      The input spec file must contain the stereotaxic space and the  \n"
       + indent9 + "      structure (LEFT or RIGHT) for this command to operate.  A spec \n"
       + indent9 + "      is created by running this program with the \"-spec-file-create\" \n"
       + indent9 + "      command. \n"
       + indent9 + " \n"
       + indent9 + "      This input volume must be in one of the supported stereotaxic \n"
       + indent9 + "      spaces with the origin at the anterior commissure.  The volume \n"
       + indent9 + "      should not have non-uniformity (bias) problems.  The input \n"
       + indent9 + "      volume is cropped to a single hemisphere, the voxels are \n"
       + indent9 + "      mapped to the range 0 to 255, the gray and white matter \n"
       + indent9 + "      histogram peaks are estimated, the volume is segmented, and \n"
       + indent9 + "      fiducial and inflated surfaces are generated. \n"
       + indent9 + " \n"
       + indent9 + "      Stereotaxic Spaces Supported \n"
       +    maskVolumeListFile.getAvailableMasks(indent9 +"         ")
       + indent9 + "       \n"
       + indent9 + "      OPTIONS \n"
       + indent9 + "         -ecv   Perform error correction of the segmentation volume. \n"
       + indent9 + "       \n"
       + indent9 + "         -ecs   Perform error correction of the surface. \n"
       + indent9 + "       \n"
       + indent9 + "         -flat  Generate files for flattening (very inflated, ellipsoidal, \n"
       + indent9 + "                paint, and surface shape. \n"
       + indent9 + "       \n"
       + indent9 + "         -mp    Generate surface with maxmimum number of polygons. \n"
       + indent9 + "       \n"
       + indent9 + "         -noeye  Do NOT disconnect eye and strip skull \n"
       + indent9 + "       \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeSegmentationStereotaxicSpace::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Anatomical Volume File Name");
   const QString specFileName =
      parameters->getNextParameterAsString("Spec File Name");

   bool disconnectEyeAndSkull = true;
   bool errorCorrectVolumeFlag = false;
   bool errorCorrectSurfaceFlag = false;
   bool maxPolygonsFlag = false;
   bool flatteningFlag = false;
   int uniformityIterations = BrainModelVolumeSegmentationStereotaxic::getDefaultUniformityIterations();
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Segment Option");
      if (paramName == "-ecv") {
         errorCorrectVolumeFlag = true;
      }
      else if (paramName == "-ecs") {
         errorCorrectSurfaceFlag = true;
      }
      else if (paramName == "-flat") {
         flatteningFlag = true;
      }
      else if (paramName == "-mp") {
         maxPolygonsFlag = true;
      }
      else if (paramName == "-noeye") {
         disconnectEyeAndSkull = false;
      }
      else if (paramName == "-uniform") {
         uniformityIterations = parameters->getNextParameterAsInt("Uniformity Iterations");
      }
      else {
         throw CommandException("Unexpected parameter "
                                + paramName);
      }
   }
   
   //
   // Read in segmentation masks list file
   //
   SegmentationMaskListFile maskVolumeListFile;
   maskVolumeListFile.readFile(maskVolumeListFileName);
   
   
                                          
   
   //
   // Setup spec file with anatomy volume file name and params file
   //
   SpecFile specFile;
   try {
      specFile.readFile(specFileName);
   }
   catch (FileException& e) {
      throw CommandException("Unable to read spec file: "
                             + e.whatQString());
   }

   //
   // Clear surface files from spec file and disk
   //
   specFile.clearFiles(false, // volume files
                       true,  // surface files,
                       false, // other files
                       true); // remove the files
                       

   //
   // Select anatomy and params
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::volumeAnatomyFileTag,
                          inputVolumeFileName,
                          "",
                          false);
   if (specFile.paramsFile.getNumberOfFiles() > 0) {
      specFile.paramsFile.setSelected(specFile.paramsFile.getFileName(0),
                                      true,
                                      Structure::STRUCTURE_TYPE_CORTEX_BOTH);
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(true); 
   QString specReadMsg;
   brainSet.readSpecFile(specFile,
                         specFileName,
                         specReadMsg);
   if (specReadMsg.isEmpty() == false) {
      throw CommandException("Reading spec file: "
                             + specReadMsg);
   }

   //
   // Perform the segmentation operations
   //
   BrainModelVolumeSegmentationStereotaxic segment(&brainSet,
                                                   brainSet.getVolumeAnatomyFile(0),
                                                   uniformityIterations,
                                                   disconnectEyeAndSkull,
                                                   errorCorrectVolumeFlag,
                                                   errorCorrectSurfaceFlag,
                                                   maxPolygonsFlag,
                                                   flatteningFlag);

   //
   // Execute the segmentation
   //
   segment.execute();
}

      

