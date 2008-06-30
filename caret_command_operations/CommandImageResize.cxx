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

#include "CommandImageResize.h"
#include "FileFilters.h"
#include "ImageFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandImageResize::CommandImageResize()
   : CommandBase("-image-resize",
                 "IMAGE RESIZE")
{
}

/**
 * destructor.
 */
CommandImageResize::~CommandImageResize()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandImageResize::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> unitValues, unitNames;
   unitValues.push_back("CM");     unitNames.push_back("Centimeters");
   unitValues.push_back("INCH");   unitNames.push_back("Inches");
   unitValues.push_back("PIXEL");  unitNames.push_back("Pixels");
   paramsOut.clear();
   paramsOut.addFile("Input Image File Name", FileFilters::getImageOpenFileFilter());
   paramsOut.addFile("Input Image File Name", FileFilters::getImageSaveFileFilter());
   paramsOut.addListOfItems("New Image Units", unitValues, unitNames);
   paramsOut.addFloat("New Image Width", 512.0, -1);
   paramsOut.addFloat("New Image Height", 512.0, -1);
   paramsOut.addInt("Dots Per Meter/Inch", 72, 1);
   
}

/**
 * get full help information.
 */
QString 
CommandImageResize::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-image-file-name>\n"
       + indent9 + "<output-image-file-name>\n"
       + indent9 + "<new-image-width-and-height-units-type>\n"
       + indent9 + "<new-image-width>\n"
       + indent9 + "<new-image-height>\n"
       + indent9 + "[dots-per-meter-or-inch]\n"
       + indent9 + "\n"
       + indent9 + "Resize an image.\n"
       + indent9 + "\n"
       + indent9 + "If \"new-image-width\" (or \"new-image-height\") is zero  \n"
       + indent9 + "or a negative number, the new width (or height) is scaled\n"
       + indent9 + "so that the aspect ratio of the image is maintained.\n"
       + indent9 + "\n"
       + indent9 + "The units of \"new-image-width\" and \"new-image-height\"\n"
       + indent9 + "are specified by \"new-image-width-and-height-units-type\"\n"
       + indent9 + "which must be one of the following values:\n"
       + indent9 + "   CM\n"
       + indent9 + "   INCH\n"
       + indent9 + "   PIXEL\n"
       + indent9 + "\n"
       + indent9 + "\"If \"dots-per-meter-or-inch\" is not specified, the value \n"
       + indent9 + "from the input image is used.  The units of this value is\n"
       + indent9 + "is dots per meter if \"new-image-width-and-height-units-type\"\n"
       + indent9 + "is \"CM\" and this value is dots per inch if \n"
       + indent9 + "\"new-image-width-and-height-units-type\" is \"INCH\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandImageResize::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString imageUnitsTypeName =
      parameters->getNextParameterAsString("New Image Units Type");
   float outputImageWidth =
      parameters->getNextParameterAsFloat("New Image Width");
   float outputImageHeight =
      parameters->getNextParameterAsFloat("New Image Height");
   int outputDotsPerUnit = -1;
   if (parameters->getParametersAvailable()) {
      outputDotsPerUnit = parameters->getNextParameterAsInt("Dots Per Meter or Inch");
      if (outputDotsPerUnit <= 0) {
         throw CommandException("Dots per meter or inch must be positive.");
      }
   }
   checkForExcessiveParameters();
   
   //
   // Check parameters
   //
   if ((outputImageWidth <= 0) &&
       (outputImageHeight <= 0)) {
      throw CommandException("One of Image Width or Height must be positive.");
   }
   
   //
   // Read the input image
   //
   ImageFile imageFile;
   imageFile.readFile(inputImageFileName);

   //
   // Get a pointer to the image
   //
   QImage* image = imageFile.getImage();

   //
   // Get the input image width, height, and aspect ratio
   //
   const float inputImageWidth = image->width();
   const float inputImageHeight = image->height();
   if ((inputImageWidth <= 0.0) ||
       (inputImageHeight <= 0.0)) {
      throw CommandException("Input image contains no data.");
   }
   const float inputImageAspectRatio = inputImageHeight / inputImageWidth;
   
   //
   // Set output image width/height as needed
   //
   if (outputImageWidth <= 0) {
      outputImageWidth = outputImageHeight / inputImageAspectRatio;
   }
   else if (outputImageHeight <= 0) {
      outputImageHeight = outputImageWidth * inputImageAspectRatio;
   }
   
   //
   // Determine size of output image
   //
   int width = static_cast<int>(inputImageWidth);
   int height = static_cast<int>(inputImageHeight);
   int dotsPerMeter = image->dotsPerMeterX();
   
   if (imageUnitsTypeName == "CM") {
      float dotsPerCM = dotsPerMeter / 100.0;
      if (outputDotsPerUnit > 0) {
         dotsPerCM = outputDotsPerUnit / 100.0;
      }
      width = outputImageWidth * dotsPerCM;
      height = outputImageHeight * dotsPerCM;
      dotsPerMeter = dotsPerCM * 100;
   }
   else if (imageUnitsTypeName == "INCH") {
      const float INCHES_PER_METER = 39.37;
      float dotsPerInch = dotsPerMeter / INCHES_PER_METER;
      if (outputDotsPerUnit > 0) {
         dotsPerInch = outputDotsPerUnit;
      }
      width = outputImageWidth * dotsPerInch;
      height = outputImageHeight * dotsPerInch;
      dotsPerMeter = dotsPerInch * INCHES_PER_METER;
   }
   else if (imageUnitsTypeName == "PIXEL") {
      width = outputImageWidth;
      height = outputImageHeight;
      if (outputDotsPerUnit > 0) {
         dotsPerMeter = outputDotsPerUnit;
      }
   }
   else {
      throw CommandException("Image Units Type Name invalid: "
                             + imageUnitsTypeName);
   }
   
   //
   // Resize the image
   //
   *image = image->scaled(width, 
                          height,
                          Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation);
                 
   //
   // Set dots per meter
   //
   image->setDotsPerMeterX(dotsPerMeter);
   image->setDotsPerMeterY(dotsPerMeter);
   
   //
   // Set image size and dots per inch
   //
   //
   // Write the output image
   //
   imageFile.writeFile(outputImageFileName);
}

      

