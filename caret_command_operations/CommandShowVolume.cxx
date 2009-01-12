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

#include <QImage>

#include "BrainModelOpenGL.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "CommandImageView.h"
#include "CommandShowVolume.h"
#include "DisplaySettingsVolume.h"
#include "FileFilters.h"
#include "OffScreenOpenGLWidget.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandShowVolume::CommandShowVolume()
   : CommandBase("-show-volume",
                 "SHOW VOLUME")
{
}

/**
 * destructor.
 */
CommandShowVolume::~CommandShowVolume()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandShowVolume::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> axisValues, axisNames;
   axisValues.push_back("X");   axisNames.push_back("Parasagittal View");
   axisValues.push_back("Y");   axisNames.push_back("Coronal View");
   axisValues.push_back("Z");   axisNames.push_back("Horizontal (axial) View");
   
   paramsOut.clear();
   paramsOut.addListOfItems("Axis", axisValues, axisNames);
   paramsOut.addInt("Slice", 0, 10000000, 0);
   paramsOut.addFile("Volume File Name", 
                     FileFilters::getVolumeGenericFileFilter());
   paramsOut.addInt("Image Width", 512, 0, 10000000);
   paramsOut.addInt("Image Height", 512, 0, 10000000);
   paramsOut.addVariableListOfParameters("Show Volume Options");
}

/**
 * get full help information.
 */
QString 
CommandShowVolume::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<axis>\n"
       + indent9 + "<slice>\n"
       + indent9 + "<volume-file-name>\n"
       + indent9 + "<image-width>\n"
       + indent9 + "<image-height>\n"
       + indent9 + "[image-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Render an image of the volume to an image file for the\n"
       + indent9 + "user's display.\n"
       + indent9 + "\n"
       + indent9 + "      axis is one of:  \n"
       + indent9 + "         X    => x-axis (parasagittal view)\n"
       + indent9 + "         Y    => y-axis (coronal view)\n"
       + indent9 + "         Z    => z-axis (horizontal/axial view)\n"
       + indent9 + "\n"
       + indent9 + "      NOTE: If the image file name is not specified, the image of \n"
       + indent9 + "      volume the will be shown in a window on the user's display.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandShowVolume::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const int MAIN_WINDOW_INDEX = 0;
   
   //
   // Get the parameters
   //
   VolumeFile::VOLUME_AXIS axis = 
      VolumeFile::getAxisFromString(parameters->getNextParameterAsString("AXIS"));
   
   const int sliceNumber =
      parameters->getNextParameterAsInt("Slice Number");

   QString volumeFileName =
      parameters->getNextParameterAsString("Volume File Name");
   
      
   //
   // Get the image X size
   //
   const int imageWidth =
      parameters->getNextParameterAsInt("Image Width");
   if (imageWidth <= 0) {
      throw CommandException("Invalid image width = "
                             + QString::number(imageWidth));
   }
   
   
   //
   // Get the image Y size
   //
   const int imageHeight =
      parameters->getNextParameterAsInt("Image Height");
   if (imageHeight <= 0) {
      throw CommandException("Invalid image height = "
                             + QString::number(imageHeight));
   }

   bool saveImageToFile = false;
   QString imageFileName;
   if (parameters->getParametersAvailable()) {
      saveImageToFile = true;
      
      //
      // Image file name
      //
      imageFileName = parameters->getNextParameterAsString("Image File Name");      
   }
   
   //
   // Add the volume to a spec file
   //
   SpecFile sf;
   sf.addToSpecFile(SpecFile::getVolumeAnatomyFileTag(),
                    volumeFileName,
                    "",
                    false);
                    
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet;
   QString errorMessage;
   if (brainSet.readSpecFile(sf,
                             "",
                             errorMessage)) {
      throw CommandException("reading volume file: " 
                             + errorMessage);
   }
   BrainModelVolume* bmv = brainSet.getBrainModelVolume();
   if (bmv == NULL) {
      throw CommandException("Cannot find volume in BrainSet.  "
                             "Problem with volume file? ");
   }   
   
   //
   // Set the underlay to anatomy
   //
   brainSet.getVoxelColoring()->setUnderlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY);
   
   //
   // Set the view
   //
   bmv->setSelectedAxis(MAIN_WINDOW_INDEX, axis);
   
   //
   // Set the slices
   //
   int slices[3];
   bmv->setToStandardView(MAIN_WINDOW_INDEX, BrainModelVolume::VIEW_RESET);
   bmv->getSelectedOrthogonalSlices(MAIN_WINDOW_INDEX, slices);
   switch (axis) {
      case VolumeFile::VOLUME_AXIS_X:
         slices[0] = sliceNumber;
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         slices[1] = sliceNumber;
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         slices[2] = sliceNumber;
         break;
      default:
         break;
   }
   bmv->setSelectedOrthogonalSlices(MAIN_WINDOW_INDEX, slices);
   
   //
   // Remove crosshairs
   //
   DisplaySettingsVolume* dsv = brainSet.getDisplaySettingsVolume();
   dsv->setDisplayCrosshairCoordinates(false);
   dsv->setDisplayCrosshairs(false);
   dsv->setDisplayOrientationLabels(false);
   
   //
   // setup the off screen renderer
   //
   QImage image;
   OffScreenOpenGLWidget opengl;
   
   opengl.setFixedSize(imageWidth, imageHeight);

   //
   // Render the image
   //
   opengl.drawToImage(&brainSet,
                      bmv,
                      image);
                      
   //
   // Write the image file
   //
   if (saveImageToFile) {
      if (image.save(imageFileName, "jpg") == false) {
         throw CommandException("Unable to write image file: " + imageFileName);
      }
   }
   else {
      CommandImageView::displayQImage(image);
   }
}

      

