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

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceApplyTransformationMatrix.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TransformationMatrixFile.h"

/**
 * constructor.
 */
CommandSurfaceApplyTransformationMatrix::CommandSurfaceApplyTransformationMatrix()
   : CommandBase("-surface-apply-transformation-matrix",
                 "SURFACE APPLY TRANSFORMATION MATRIX")
{
}

/**
 * destructor.
 */
CommandSurfaceApplyTransformationMatrix::~CommandSurfaceApplyTransformationMatrix()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceApplyTransformationMatrix::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Coordinate File Name", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addVariableListOfParameters("Matrix Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceApplyTransformationMatrix::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coordinate-file-name>\n"
       + indent9 + "<topology-file-name>\n"
       + indent9 + "<output-coordinate-file-name>\n"
       + indent9 + "[-matrix-file  matrix-file-name   matrix-name]\n"
       + indent9 + "[-matrix  matrix-values-left-to-right-and-top-to-bottom]\n"
       + indent9 + "\n"
       + indent9 + "Apply a transformation matrix to a surface.\n"
       + indent9 + "\n"
       + indent9 + "The matrix may be within a specified file or the matrix\n"
       + indent9 + "may be specified on the command line.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceApplyTransformationMatrix::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputCoordinateFileName =
      parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Topology File Name");
   const QString outputCoordinateFileName =
      parameters->getNextParameterAsString("Output Coordinate File Name");
   
   //
   // Get optional parameters
   //
   enum MATRIX_SOURCE {
      MATRIX_SOURCE_NONE,
      MATRIX_SOURCE_FROM_FILE,
      MATRIX_SOURCE_VALUES
   };
   MATRIX_SOURCE matrixSource = MATRIX_SOURCE_NONE;
   QString matrixFileName;
   QString matrixName;
   float matrixValues[4][4];
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Apply Transform Parameter");
      if (paramName == "-matrix-file") {
         matrixFileName = parameters->getNextParameterAsString("Matrix File Name");
         matrixName     = parameters->getNextParameterAsString("Matrix Name");
         matrixSource   = MATRIX_SOURCE_FROM_FILE;
      }
      else if (paramName == "-matrix") {
         int counter = 1;
         for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
               const QString text("Matrix value " + QString::number(counter));
               matrixValues[i][j] = parameters->getNextParameterAsFloat(text);
            }
            counter++;
         }
         matrixSource = MATRIX_SOURCE_VALUES;
      }
      else {
         throw CommandException("unrecognized parameter: "
                                + paramName);
      }
   }
   
   //
   // Did user forget to supply a matrix
   //
   if (matrixSource == MATRIX_SOURCE_NONE) {
      throw CommandException("A matrix must be supplied "
                             "with either \"-matrix-file\" or \"-matrix\".");
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(inputTopologyFileName,
                     inputCoordinateFileName,
                     "",
                     true);
   BrainModelSurface* surface = brainSet.getBrainModelSurface(0);
   if (surface == NULL) {
      throw CommandException("unable to find fiducial surface.");
   }
   const TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("unable to find topology.");
   }
   const int numNodes = surface->getNumberOfNodes();
   if (numNodes == 0) {
      throw CommandException("surface contains no nodes.");
   }
   
   //
   // Apply the transformation matrix
   //
   switch (matrixSource) {
      case MATRIX_SOURCE_NONE:
         break;
      case MATRIX_SOURCE_FROM_FILE:
         {
            TransformationMatrixFile tmf;
            tmf.readFile(matrixFileName);
            TransformationMatrix* matrix = tmf.getTransformationMatrixWithName(matrixName);
            surface->applyTransformationMatrix(*matrix);
         }
         break;
      case MATRIX_SOURCE_VALUES:
         {
            TransformationMatrix matrix;
            matrix.setMatrix(matrixValues);
            surface->applyTransformationMatrix(matrix);
         }
         break;
   }
   
   //
   // Write the coordinate file
   //
   CoordinateFile* cf = surface->getCoordinateFile();
   cf->writeFile(outputCoordinateFileName);
}

      

