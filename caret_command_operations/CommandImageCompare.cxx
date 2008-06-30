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

#include "CommandImageCompare.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "ImageFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandImageCompare::CommandImageCompare()
   : CommandBase("-image-compare",
                 "IMAGE COMPARE")
{
}

/**
 * destructor.
 */
CommandImageCompare::~CommandImageCompare()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandImageCompare::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   QStringList fileFilters, fileExtensions;
   FileFilters::getImageSaveFileFilters(fileFilters,
                                        fileExtensions);
   paramsOut.clear();
   paramsOut.addFile("Image File Name 1", fileFilters);
   paramsOut.addFile("Image File Name 2", fileFilters);
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandImageCompare::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<image-file-name-1>  \n"
       + indent9 + "<image-file-name-2> \n"
       + indent9 + "[-tol  pixel-tolerance] \n"
       + indent9 + "\n"
       + indent9 + "Compare two image files to determine if the pixels are\n"
       + indent9 + "different.  The default value for \"pixel-tolerance\" \n"
       + indent9 + "is zero, in which cse the pixels must match exactly.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandImageCompare::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString imageFileName1 =
      parameters->getNextParameterAsString("Image File Name 1");
   const QString imageFileName2 =
      parameters->getNextParameterAsString("Image File Name 2");
   float pixelTolerance = 0.0;
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString("Optional parameter");
      if (paramName == "-tol") {
         pixelTolerance = parameters->getNextParameterAsFloat("Pixel Tolerance");
      }
      else {
         throw CommandException("Unrecognized parameter = \""
                                + paramName
                                + "\".");      
      }
   }
      
   //
   // Make sure that are no more parameters
   //
   checkForExcessiveParameters();

   //
   // Read the images
   //
   ImageFile imageFile1, imageFile2;
   imageFile1.readFile(imageFileName1);
   imageFile2.readFile(imageFileName2);
   
   //
   // Compare the images
   //
   QString comparisonMessage;
   const bool theSame = imageFile1.compareFileForUnitTesting(&imageFile2,
                                                             pixelTolerance,
                                                             comparisonMessage);
   std::cout << "IMAGE COMPARISON for "
             << FileUtilities::basename(imageFileName1).toAscii().constData()
             << " and "
             << FileUtilities::basename(imageFileName2).toAscii().constData()
             << " ";

   if (theSame) {
      std::cout << "successful." << std::endl;
   }
   else {
      std::cout << "FAILED." << std::endl;
      std::cout << "   " << comparisonMessage.toAscii().constData() << std::endl;
      throw CommandException("");
   }
}

      

