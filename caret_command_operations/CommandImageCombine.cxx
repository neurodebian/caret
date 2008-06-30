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

#include "BrainSet.h"
#include "CommandImageCombine.h"
#include "FileFilters.h"
#include "ImageFile.h"
#include "PreferencesFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandImageCombine::CommandImageCombine()
   : CommandBase("-image-combine",
                 "IMAGE COMBINE")
{
}

/**
 * destructor.
 */
CommandImageCombine::~CommandImageCombine()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandImageCombine::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addInt("Number of Images Per Row",
                     2);
   paramsOut.addFile("Output Image File Name",
                     FileFilters::getImageSaveFileFilter());
   paramsOut.addFile("Input Image 1 File Name",
                     FileFilters::getImageOpenFileFilter());
   paramsOut.addVariableListOfParameters("Additional Image Files");
}

/**
 * get full help information.
 */
QString 
CommandImageCombine::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<number-of-images-per-row>\n"
       + indent9 + "<output-image-file-name>\n"
       + indent9 + "<input-image-1-file-name>\n"
       + indent9 + "[additional-image-file-names]\n"
       + indent9 + "\n"
       + indent9 + "Combine images into a single image.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandImageCombine::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the input parameters
   //
   const int numberOfImagesPerRow =
      parameters->getNextParameterAsInt("Number of Images Per Row");
   const QString outputImageFileName =
      parameters->getNextParameterAsString("Output Image File Name");
   std::vector<QString> imageFileNames;
   imageFileNames.push_back(
      parameters->getNextParameterAsString("Input Image File 1 Name"));
   while (parameters->getParametersAvailable()) {
      imageFileNames.push_back(
         parameters->getNextParameterAsString("Input Image File Name"));
   }

   //
   // Read the images
   //   
   std::vector<QImage> images;
   for (unsigned int i = 0; i < imageFileNames.size(); i++) {
      QImage image;
      if (image.load(imageFileNames[i]) == false) {
         throw CommandException("ERROR reading: " +
                                imageFileNames[i]);
      }
      images.push_back(image);
   }
   
   //
   // Get background color
   //
   BrainSet brainSet;
   const PreferencesFile* pf = brainSet.getPreferencesFile();
   unsigned char r, g, b;
   pf->getSurfaceBackgroundColor(r, g, b);
   const int backgroundColor[3] = { r, b, b };

   //
   // Combine the images
   //
   QImage outputImage;
   ImageFile::combinePreservingAspectAndFillIfNeeded(images,
                                                     numberOfImagesPerRow,
                                                     backgroundColor,
                                                     outputImage);
                                                     
   //
   // Write the output image
   //
   if (outputImage.save(outputImageFileName, 0, 100) == false) {
      throw CommandException("ERROR writing: " +
                             outputImageFileName);
   }
}

      

