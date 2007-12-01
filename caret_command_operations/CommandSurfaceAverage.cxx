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

#include "CommandSurfaceAverage.h"
#include "CoordinateFile.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SurfaceShapeFile.h"

/**
 * constructor.
 */
CommandSurfaceAverage::CommandSurfaceAverage()
   : CommandBase("-surface-average",
                 "SURFACE CREATE AVERAGE")
{
}

/**
 * destructor.
 */
CommandSurfaceAverage::~CommandSurfaceAverage()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceAverage::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addMultipleFiles("Input Coordinate File Name(s)", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addVariableListOfParameters("Coordinate Options");
}
      
/**
 * get full help information.
 */
QString 
CommandSurfaceAverage::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-coordinate-file-name>\n"
       + indent9 + "<input-coordinate-file-1-name>\n"
       + indent9 + "[input-coordinate-file-2-name]\n"
       + indent9 + "...\n"
       + indent9 + "[input-coordinate-file-N-name]\n"
       + indent9 + "[output-surface-shape-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Create an average surface.\n"
       + indent9 + "\n"
       + indent9 + "If a surface shape file name is specified, it will be created\n"
       + indent9 + "and contain the coordinate uncertainty.  The surface shape file\n"
       + indent9 + "name must end with \"" + SpecFile::getSurfaceShapeFileExtension() + "\".\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceAverage::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the pamameters
   //
   const QString outputCoordinateFileName = 
      parameters->getNextParameterAsString("Output Coordinate File Name");
   std::vector<QString> inputCoordinateFileNames;
   QString surfaceShapeFileName;
   while (parameters->getParametersAvailable()) {
      const QString name = parameters->getNextParameterAsString("Optional File Name");
      if (name.endsWith(SpecFile::getSurfaceShapeFileExtension())) {
         surfaceShapeFileName = name;
      }
      else {
         inputCoordinateFileNames.push_back(name);
      }
   }
   
   const int numCoordFiles = static_cast<int>(inputCoordinateFileNames.size());
   if (numCoordFiles < 1) {
      throw CommandException("There must be at least one input coordinate file.");
   }
   
   //
   // Read the input coordinate files
   //   
   std::vector<CoordinateFile*> coordinateFiles;
   for (int i = 0; i < static_cast<int>(inputCoordinateFileNames.size()); i++) {
      CoordinateFile* coordFile = new CoordinateFile;
      coordFile->readFile(inputCoordinateFileNames[i]);
      coordinateFiles.push_back(coordFile);
   }
   
   //
   // Should a surface shape file containing uncertainty be created?
   //
   SurfaceShapeFile* ssf = NULL;
   if (surfaceShapeFileName.isEmpty() == false) {
      ssf = new SurfaceShapeFile;
   }
   
   //
   // Create the average coordinate file
   //
   CoordinateFile outputCoordinateFile;
   CoordinateFile::createAverageCoordinateFile(coordinateFiles,
                                               outputCoordinateFile,
                                               ssf);
   
   //
   // Write the coordinate file
   //
   outputCoordinateFile.writeFile(outputCoordinateFileName);
   
   //
   // Write surface shape file if it was created
   //
   if (ssf != NULL) {
      ssf->writeFile(surfaceShapeFileName);
   }
   
   //
   // Free memory
   //
   for (unsigned int i = 0; i < coordinateFiles.size(); i++) {
      delete coordinateFiles[i];
   }
   if (ssf != NULL) {
      delete ssf;
   }
}


