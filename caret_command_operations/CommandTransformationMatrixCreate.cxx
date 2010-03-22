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

#include <QFile>

#include "CommandTransformationMatrixCreate.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TransformationMatrixFile.h"

/**
 * constructor.
 */
CommandTransformationMatrixCreate::CommandTransformationMatrixCreate()
   : CommandBase("-transformation-matrix-create",
                 "TRANSFORMATION MATRIX CREATION")
{
}

/**
 * destructor.
 */
CommandTransformationMatrixCreate::~CommandTransformationMatrixCreate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandTransformationMatrixCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Transformation Matrix File Name", 
                     FileFilters::getTransformationMatrixFileFilter());
   paramsOut.addFile("Output Transformation Matrix File Name", 
                     FileFilters::getTransformationMatrixFileFilter());
   paramsOut.addString("Matrix Name");
   paramsOut.addVariableListOfParameters("Matrix Options");
}

/**
 * get full help information.
 */
QString 
CommandTransformationMatrixCreate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-transformation-matrix-file>\n"
       + indent9 + "<output-transformation-matrix-file>\n"
       + indent9 + "<matrix-name>\n"
       + indent9 + "[-matrix-comment  comment-text]\n"
       + indent9 + "[-delete-all-matrices-from-file]\n"
       + indent9 + "[-identity]\n"
       + indent9 + "[-inverse]\n"
       + indent9 + "[-pre-multiply  matrix-name]\n"
       + indent9 + "[-post-multiply  matrix-name]\n"
       + indent9 + "[-rotate X  Y  Z]\n"
       + indent9 + "[-scale  X  Y  Z]\n"
       + indent9 + "[-translate  X  Y  Z]\n"
       + indent9 + "[-transpose]\n"
       + indent9 + "\n"
       + indent9 + "Operations are applied in the order they are specified on the\n"
       + indent9 + "command line.\n"
       + indent9 + "\n"
       + indent9 + "The input transformation matrix file does not need to exist.\n"
       + indent9 + "If it does exist, new matrices are appended to the file.  If\n"
       + indent9 + "the name of the new matrix is the same as that of an existing\n"
       + indent9 + "matrix, the existing matrix is replaced.\n"
       + indent9 + "\n"
       + indent9 + "Use \"-delete-all-matrices-from-file\" to remove all the \n"
       + indent9 + "matrices from an existing file.\n"
       + indent9 + "\n"
       + indent9 + "If comment-text contains spaces, it must be enclosed in quotes.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandTransformationMatrixCreate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get required parameters
   //
   const QString inputMatrixFileName =
      parameters->getNextParameterAsString("Input Matrix File Name");
   const QString outputMatrixFileName =
      parameters->getNextParameterAsString("Output Matrix File Name");
   const QString matrixName =
      parameters->getNextParameterAsString("Matrix Name");
     
   //
   // Create the transformation matrix file and read it if it exists
   //
   TransformationMatrixFile tmf;
   if (inputMatrixFileName.isEmpty() == false) {
      if (QFile::exists(inputMatrixFileName)) {
         tmf.readFile(inputMatrixFileName);
      }
   }
   
   TransformationMatrix matrix;
   matrix.setMatrixName(matrixName);
   
   //
   // process optional parameters
   //
   while (parameters->getParametersAvailable()) {
      //
      // Get the next parameter
      //
      const QString paramName = parameters->getNextParameterAsString("Create Matrix Parameter");
      
      if (paramName == "-delete-all-matrices-from-file") {
         tmf.clear();
      }
      else if (paramName == "-matrix-comment") {
         matrix.setMatrixComment(parameters->getNextParameterAsString("Matrix Comment Text"));
      }
      else if (paramName == "-identity") {
         matrix.identity();
      }
      else if (paramName == "-inverse") {
         matrix.inverse();
      }
      else if (paramName == "-pre-multiply") {
         const QString multiplyMatrixName(parameters->getNextParameterAsString("Multiply Matrix Name"));
         const TransformationMatrix* tm = tmf.getTransformationMatrixWithName(multiplyMatrixName);
         if (tm == NULL) {
            throw CommandException("Unable to find matrix "
                                   "with name \""
                                   + multiplyMatrixName
                                   + "\" for pre-multiplication");
         }
         matrix.preMultiply(*tm);
      }
      else if (paramName == "-post-multiply") {
         const QString multiplyMatrixName(parameters->getNextParameterAsString("Multiply Matrix Name"));
         const TransformationMatrix* tm = tmf.getTransformationMatrixWithName(multiplyMatrixName);
         if (tm == NULL) {
            throw CommandException("Unable to find matrix "
                                   "with name \""
                                   + multiplyMatrixName
                                   + "\" for post-multiplication");
         }
         matrix.postMultiply(*tm);
      }
      else if (paramName == "-rotate") {
         const float x = parameters->getNextParameterAsFloat("Rotation X");
         const float y = parameters->getNextParameterAsFloat("Rotation Y");
         const float z = parameters->getNextParameterAsFloat("Rotation Z");
         matrix.rotateZ(z);
         matrix.rotateX(x);
         matrix.rotateY(y);
      }
      else if (paramName == "-scale") {
         const float x = parameters->getNextParameterAsFloat("Scale X");
         const float y = parameters->getNextParameterAsFloat("Scale Y");
         const float z = parameters->getNextParameterAsFloat("Scale Z");
         matrix.scale(x, y, z);
      }
      else if (paramName == "-translate") {
         const float x = parameters->getNextParameterAsFloat("Translation X");
         const float y = parameters->getNextParameterAsFloat("Translation Y");
         const float z = parameters->getNextParameterAsFloat("Translation Z");
         matrix.translate(x, y, z);
      }
      else if (paramName == "-transpose") {
         matrix.transpose();
      }
      else {
         throw CommandException("Unrecognized parameter: "
                                + paramName);
      }
   }
   
   //
   // Does the matrix exist
   //
   TransformationMatrix* oldMatrix = tmf.getTransformationMatrixWithName(matrixName);
   if (oldMatrix != NULL) {
      //
      // Replace old matrix
      //
      *oldMatrix = matrix;
   }
   else {
      //
      // Add new matrix
      //
      tmf.addTransformationMatrix(matrix);
   }
   
   //
   // Write the transformation matrix file
   //
   tmf.writeFile(outputMatrixFileName);
}

      

