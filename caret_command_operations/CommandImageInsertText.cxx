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

#include <QFont>
#include <QImage>
#include <QPainter>
#include <QPen>

#include "CommandImageInsertText.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandImageInsertText::CommandImageInsertText()
   : CommandBase("-image-insert-text",
                 "IMAGE INSERT TEXT")
{
}

/**
 * destructor.
 */
CommandImageInsertText::~CommandImageInsertText()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandImageInsertText::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Image File Name",
                     FileFilters::getImageOpenFileFilter());
   paramsOut.addFile("Output Image File Name",
                     FileFilters::getImageSaveFileFilter());
   paramsOut.addInt("Text X Position", 20);
   paramsOut.addInt("Text Y Position", 20);
   paramsOut.addString("Text");
}

/**
 * get full help information.
 */
QString 
CommandImageInsertText::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-image-file-name>\n"
       + indent9 + "<output-image-file-name>\n"
       + indent9 + "<text-x-position>  <text-y-position>\n"
       + indent9 + "<text-red> <text-green> <text-blue>\n"
       + indent9 + "<text>\n"
       + indent9 + "\n"
       + indent9 + "Place the text into an image at the specified location.\n"
       + indent9 + "The origin is at the bottom left corner of the image.\n"
       + indent9 + "\n"
       + indent9 + "The red, green, and blue color components for the text\n"
       + indent9 + "range from 0 to 255.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandImageInsertText::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Read the parameters
   //
   const QString inputImageFileName =
      parameters->getNextParameterAsString("Input Image File Name");
   const QString outputImageFileName =
      parameters->getNextParameterAsString("Output Image File Name");
   const int textPositionX =
      parameters->getNextParameterAsInt("Text X Position");
   const int textPositionY =
      parameters->getNextParameterAsInt("Text Y Position");
   const int textRed =
      parameters->getNextParameterAsInt("Text Red");
   const int textGreen =
      parameters->getNextParameterAsInt("Text Green");
   const int textBlue =
      parameters->getNextParameterAsInt("Text Blue");
   const QString theText =
      parameters->getNextParameterAsString("Text");
      
   //
   // Read the image
   //
   QImage image;
   if (image.load(inputImageFileName) == false) {
      throw CommandException("ERROR reading: " + inputImageFileName);
   }
   
   //
   // Create a painter and add the text
   //
   QPen pen(QColor(textRed, textGreen, textBlue));
   QPainter painter(&image);
   painter.setPen(pen);
   QFont font;
   font.setBold(true);
   font.setPointSize(16);
   painter.setFont(font);
   painter.drawText(textPositionX,
                    image.size().height() - textPositionY,
                    theText);

   //
   // Save the image
   //
   if (image.save(outputImageFileName, 0, 100) == false) {
      throw CommandException("ERROR writing: " + inputImageFileName);
   }
}

      

