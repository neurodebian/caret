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
#include <fstream>

#include "CommandSurfaceAffineRegression.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelSurfaceAffineRegression.h"
#include "BrainSet.h"
#include "StringUtilities.h"
#include "FileUtilities.h"

/**
 * constructor.
 */
CommandSurfaceAffineRegression::CommandSurfaceAffineRegression()
   : CommandBase("-surface-affine-regression",
                 "SURFACE TO SURFACE LINEAR REGRESSION AFFINE REGISTRATION")
{
}

/**
 * destructor.
 */
CommandSurfaceAffineRegression::~CommandSurfaceAffineRegression()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceAffineRegression::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Target Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Source Coordinate File", FileFilters::getCoordinateGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceAffineRegression::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<target-coord>\n"
       + indent9 + "<source-coord-1>\n"
       + indent9 + "[<source-coord-2>...]\n"
       + indent9 + "\n"
       + indent9 + "Use linear regression to compute and execute an affine transormation,\n"
       + indent9 + "attempting to minimize the variability from source to target.\n"
       + indent9 + "File names will be automatically generated by adding \".afftrans.\"\n"
       + indent9 + "as the third section of the filename, eg, \"test.case5.73730.coord\"\n"
       + indent9 + "will be output as \"test.case5.afftrans.73730.coord\", in the same\n"
       + indent9 + "directory.\n"
       + indent9 + "\n"
       + indent9 + "      target-coord  the target to minimize the variability to\n"
       + indent9 + "\n"
       + indent9 + "      source-coord-1  a source coordinate file to transform\n"
       + indent9 + "\n"
       + indent9 + "      [source-coord-2...]  same as above, but optional.  You may\n"
       + indent9 + "                            specify as many sources as you wish.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceAffineRegression::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString target =
      parameters->getNextParameterAsString("Target Coordinate File");
   BrainSet brainSet;
   CoordinateFile targetCoord, sourceCoord, outCoord;
   targetCoord.readFile(target);
   QString source, out, sourceFile, sourcePath, outMat;
   int i = 1, index;
   BrainModelSurfaceAffineRegression* affineObject;
   while (parameters->getParametersAvailable())
   {
      try
      {
         source = parameters->getNextParameterAsString("Source Coordinate File " + StringUtilities::fromNumber(i));
         sourceFile = FileUtilities::basename(source);
         sourcePath = FileUtilities::dirname(source);
         
         //
         // Add .afftrans. as 2nd field before extension
         //
         index = sourceFile.lastIndexOf('.');
         index = sourceFile.lastIndexOf('.', index - 1) + 1;
         if (index == -1)
         {
            index = 0;
         }
         outMat = source + ".mat";
         out = sourceFile;
         out.insert(index, "afftrans.");
         if (sourcePath.right(1) == FileUtilities::directorySeparator() || sourcePath == "")
         {
            out = sourcePath + out;
         } else {
            out = sourcePath + FileUtilities::directorySeparator() + out;
         }
         sourceCoord.readFile(source);
         outCoord = sourceCoord;
      
         //
         // Create the affine object
         //
         affineObject = new BrainModelSurfaceAffineRegression(&brainSet,
                                                        &sourceCoord,
                                                        &targetCoord,
                                                        &outCoord,
                                                        out);
            
         //
         // Execute regression
         //
         affineObject->execute();
         outCoord.writeFile(out);
         QFile affineFile(outMat);
         if (!affineFile.open(QIODevice::WriteOnly))
         {
            std::cerr << "Error opening output file \"" << outMat.toAscii().constData() << '"' << std::endl;
         } else {
            for (short j = 0; j < 3; ++j)
            {
               for (short k = 0; k < 4; ++k, affineFile.write("  ", 2))
               {
                  affineFile.write(StringUtilities::fromNumber(affineObject->getAffine(j, k)).toAscii());
               }
               affineFile.write("\n", 1);
               //affineFile.write(StringUtilities::fromStdString(std::endl).toAscii());
            }
            affineFile.write("0  0  0  1  \n", 13);
            affineFile.close();
            //affineFile.write(StringUtilities::fromStdString(std::endl).toAscii());
            const QString warningMessages = affineObject->getWarningMessages();
            if (warningMessages.isEmpty() == false) {
               std::cout << "Transformation Warnings: " << warningMessages.toAscii().constData() << std::endl;
            }
         }
         ++i;
         delete affineObject;
      } catch(FileException e) {
         std::cerr << e.whatQString().toAscii().constData() << std::endl;
      } catch(BrainModelAlgorithmException e) {
         std::cerr << e.whatQString().toAscii().constData() << std::endl;
      }
   }
}

      

