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
#include "BrainModelSurfaceCellAttributeAssignment.h"
#include "BrainSet.h"
#include "FociProjectionFile.h"
#include "CommandSurfaceFociAttributeAssignment.h"
#include "FileFilters.h"
#include "PaintFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSurfaceFociAttributeAssignment::CommandSurfaceFociAttributeAssignment()
   : CommandBase("-surface-foci-attribute-assignment",
                 "SURFACE FOCI ATTRIBUTE ASSIGNMENT")
{
}

/**
 * destructor.
 */
CommandSurfaceFociAttributeAssignment::~CommandSurfaceFociAttributeAssignment()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceFociAttributeAssignment::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> attributeNames;
   std::vector<BrainModelSurfaceCellAttributeAssignment::ASSIGN_ATTRIBUTE> attributeValues;
   BrainModelSurfaceCellAttributeAssignment::getAttributeNamesAndValues(
                                    attributeNames, attributeValues);

   std::vector<QString> methodNames;
   std::vector<BrainModelSurfaceCellAttributeAssignment::ASSIGNMENT_METHOD> methodValues;
   BrainModelSurfaceCellAttributeAssignment::getAssignmentNamesAndValues(
                                    methodNames, methodValues);

   paramsOut.clear();
   paramsOut.addFile("Left Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Left Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Right Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Right Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Cerebellum Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Cerebellum Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Input Foci Projection File", FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Output Foci Projection File", FileFilters::getFociProjectionFileFilter());
   paramsOut.addFile("Paint File", FileFilters::getPaintFileFilter());
   paramsOut.addFloat("Maximum Distance From Surface", 1000.0);
   paramsOut.addListOfItems("Attribute to Assign", attributeNames, attributeNames);
   paramsOut.addListOfItems("Assignment Method", methodNames, methodNames);
   paramsOut.addInt("Attribute ID", -1);
   paramsOut.addBoolean("Ignore Paint ??? Names", true);
   paramsOut.addVariableListOfParameters("Paint Column Names/Numbers");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceFociAttributeAssignment::getHelpInformation() const
{
   std::vector<QString> attributeNames;
   std::vector<BrainModelSurfaceCellAttributeAssignment::ASSIGN_ATTRIBUTE> attributeValues;
   BrainModelSurfaceCellAttributeAssignment::getAttributeNamesAndValues(
                                    attributeNames, attributeValues);

   std::vector<QString> methodNames;
   std::vector<BrainModelSurfaceCellAttributeAssignment::ASSIGNMENT_METHOD> methodValues;
   BrainModelSurfaceCellAttributeAssignment::getAssignmentNamesAndValues(
                                    methodNames, methodValues);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<left-hemisphere-coordinate-file-name>\n"
       + indent9 + "<left-hemisphere-topology-file-name>\n"
       + indent9 + "<right-hemisphere-coordinate-file-name>\n"
       + indent9 + "<right-hemisphere-topology-file-name>\n"
       + indent9 + "<cerebellum-coordinate-file-name>\n"
       + indent9 + "<cerebellum-topology-file-name>\n"
       + indent9 + "<input-cell-projection-file-name>\n"
       + indent9 + "<output-cell-projection-file-name>\n"
       + indent9 + "<paint-file-name>\n"
       + indent9 + "<maximum-distance-from-surface>\n"
       + indent9 + "<assign-to-attribute-name>\n"
       + indent9 + "<assignment-method-name>\n"
       + indent9 + "<attribute-ID>\n"
       + indent9 + "<ignore-unknown-paint-names>\n"
       + indent9 + "[paint-column-name-or-number-1]\n"
       + indent9 + "[paint-column-name-or-number-2]\n"
       + indent9 + "...\n"
       + indent9 + "[paint-column-name-or-number-N]\n"
       + indent9 + "\n"
       + indent9 + "Assign paint names to foci attributes.\n"
       + indent9 + "\n"
       + indent9 + "\"assign-to-attribute-name\" is one of:\n");
    for (unsigned int i = 0; i < attributeNames.size(); i++) {
       helpInfo += (
         indent9 + "   " + attributeNames[i] + "\n");
    }
    helpInfo += ("\n"
       + indent9 + "\"assignment-method-name\" is one of:\n");
    for (unsigned int i = 0; i < methodNames.size(); i++) {
       helpInfo += (
         indent9 + "   " + methodNames[i] + "\n");
    }
    helpInfo += (
         indent9 + "\n"
       + indent9 + "attribute ID is an integer value:\n"
       + indent9 + "   -1   Indeterminate\n"
       + indent9 + "    0   Unassigned\n"
       + indent9 + "    1   \n"
       + indent9 + "\n"
       + indent9 + "If a surface is not available or you do not want to \n"
       + indent9 + "perform assignment to that surface, use a value of \n"
       + indent9 + "NONE for the coordinate and topology file.\n"
       + indent9 + "\n"
       + indent9 + "Paint columns are either the number of a paint column,\n"
       + indent9 + "which starts at one, or the name of a column.  If a name\n"
       + indent9 + "contains spaces, it must be enclosed in double quotes.\n"
       + indent9 + "Name has priority over number.\n"
       + indent9 + "\n"
       + indent9 + "\"ignore-unknown-paint-names\" is either\n"
       + indent9 + "   true\n"
       + indent9 + "   false\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceFociAttributeAssignment::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{

   const QString leftCoordinateFileName =
      parameters->getNextParameterAsString("Left Coordinate File Name");
   const QString leftTopologyFileName =
      parameters->getNextParameterAsString("Left Topology File Name");
   const QString rightCoordinateFileName =
      parameters->getNextParameterAsString("Right Coordinate File Name");
   const QString rightTopologyFileName =
      parameters->getNextParameterAsString("Right Topology File Name");
   const QString cerebellumCoordinateFileName =
      parameters->getNextParameterAsString("Cerebellum Coordinate File Name");
   const QString cerebellumTopologyFileName =
      parameters->getNextParameterAsString("Cerebellum Topology File Name");
   const QString inputFociProjectionFileName =
      parameters->getNextParameterAsString("Input Foci Projection File Name");
   const QString outputFociProjectionFileName =
      parameters->getNextParameterAsString("Output Foci Projection File Name");
   const QString paintFileName =
      parameters->getNextParameterAsString("Paint File Name");
   const float maximumDistanceFromSurface =
      parameters->getNextParameterAsFloat("Maximum Distance from Surface");
   const QString attributeName =
      parameters->getNextParameterAsString("Assign to Attribute Name");
   const QString assignmentMethodName = 
      parameters->getNextParameterAsString("Assignment Method Name");
   const int attributeID =
      parameters->getNextParameterAsInt("Attribute ID");
   const bool ignoreUnknownPaintNamesFlag = 
      parameters->getNextParameterAsBoolean("Ignore ??? Paint Names");
   std::vector<QString> paintColumnNamesAndNumbers;
   while (parameters->getParametersAvailable()) {
      paintColumnNamesAndNumbers.push_back(
         parameters->getNextParameterAsString("Paint Column Name or Number"));
   }
   
   bool doLeft = true;
   bool doRight = true;
   bool doCerebellum = true;
   const QString noneValue("NONE");
   if ((leftCoordinateFileName == noneValue) ||
       (leftTopologyFileName == noneValue)) {
      doLeft = false;
   }
   if ((rightCoordinateFileName == noneValue) ||
       (rightTopologyFileName == noneValue)) {
      doRight = false;
   }
   if ((cerebellumCoordinateFileName == noneValue) ||
       (cerebellumTopologyFileName == noneValue)) {
      doCerebellum = false;
   }
   
   //
   // Create a spec file and add coordinate and topology files to it
   //
   SpecFile sf;
   if (doLeft) {
      sf.addToSpecFile(SpecFile::getFiducialCoordFileTag(), leftCoordinateFileName, "", false);
      sf.addToSpecFile(SpecFile::getClosedTopoFileTag(), leftTopologyFileName, "", false);
   }
   if (doRight) {
      sf.addToSpecFile(SpecFile::getFiducialCoordFileTag(), rightCoordinateFileName, "", false);
      sf.addToSpecFile(SpecFile::getClosedTopoFileTag(), rightTopologyFileName, "", false);
   }
   if (doCerebellum) {
      sf.addToSpecFile(SpecFile::getFiducialCoordFileTag(), cerebellumCoordinateFileName, "", false);
      sf.addToSpecFile(SpecFile::getClosedTopoFileTag(), cerebellumTopologyFileName, "", false);
   }
   
   //
   // Read the spec file
   //
   BrainSet brainSet(true);
   QString errorMessage;
   brainSet.readSpecFile(sf,
                         "",
                         errorMessage);
   if (errorMessage.isEmpty() == false) {
      throw CommandException(errorMessage);
   }
   
   //
   // Find the topology files
   //
   TopologyFile* leftTopologyFile = NULL;
   TopologyFile* rightTopologyFile = NULL;
   TopologyFile* cerebellumTopologyFile = NULL;
   for (int i = 0; i < brainSet.getNumberOfTopologyFiles(); i++) {
      TopologyFile* tf = brainSet.getTopologyFile(i);
      if (tf->getFileName().endsWith(leftTopologyFileName)) {
         leftTopologyFile = tf;
      }
      if (tf->getFileName().endsWith(rightTopologyFileName)) {
         rightTopologyFile = tf;
      }
      if (tf->getFileName().endsWith(cerebellumTopologyFileName)) {
         cerebellumTopologyFile = tf;
      }
   }
   if (doLeft && (leftTopologyFile == NULL)) {
      throw CommandException("Unable to find left topology file after reading files.");
   }
   if (doRight && (rightTopologyFile == NULL)) {
      throw CommandException("Unable to find right topology file after reading files.");
   }
   if (doCerebellum && (cerebellumTopologyFile == NULL)) {
      throw CommandException("Unable to find cerebellum topology file after reading files.");
   }
   
   //
   // Find the surfaces and assign topology
   //
   BrainModelSurface* leftSurface = NULL;
   BrainModelSurface* rightSurface = NULL;
   BrainModelSurface* cerebellumSurface = NULL;
   for (int i = 0; i < brainSet.getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getCoordinateFile()->getFileName().endsWith(leftCoordinateFileName)) {
            leftSurface = bms;
            leftSurface->setTopologyFile(leftTopologyFile);
         }
         if (bms->getCoordinateFile()->getFileName().endsWith(rightCoordinateFileName)) {
            rightSurface = bms;
            rightSurface->setTopologyFile(rightTopologyFile);
         }
         if (bms->getCoordinateFile()->getFileName().endsWith(cerebellumCoordinateFileName)) {
            cerebellumSurface = bms;
            cerebellumSurface->setTopologyFile(cerebellumTopologyFile);
         }
      }
   }
   if (doLeft && (leftSurface == NULL)) {
      throw CommandException("Unable to find left surface file after reading files.");
   }
   if (doRight && (rightSurface == NULL)) {
      throw CommandException("Unable to find right surface file after reading files.");
   }
   if (doCerebellum && (cerebellumSurface == NULL)) {
      throw CommandException("Unable to find cerebellum surface file after reading files.");
   }
   
   //
   // Read the foci projection file
   //
   FociProjectionFile fociProjectionFile;
   fociProjectionFile.readFile(inputFociProjectionFileName);
   
   //
   // Read the paint file
   //
   PaintFile paintFile;
   paintFile.readFile(paintFileName);
   
   //
   // Select the paint columns
   //
   std::vector<bool> paintColumnsSelected;
   const int numCols = paintFile.getNumberOfColumns();
   if (numCols > 0) {
      paintColumnsSelected.resize(numCols, false);
   }
   for (unsigned int i = 0; i < paintColumnNamesAndNumbers.size(); i++) {
      const int colNum = paintFile.getColumnFromNameOrNumber(
                                           paintColumnNamesAndNumbers[i], false);
      if ((colNum >= 0) &&
          (colNum < paintFile.getNumberOfColumns())) {
         paintColumnsSelected[colNum] = true;
      }
      else {
         throw CommandException("Invalid paint column name or number " +
                                paintColumnNamesAndNumbers[i]);
      }
   }
   
   //
   // Get the attribute
   //
   std::vector<QString> attributeNames;
   std::vector<BrainModelSurfaceCellAttributeAssignment::ASSIGN_ATTRIBUTE> attributeValues;
   BrainModelSurfaceCellAttributeAssignment::ASSIGN_ATTRIBUTE attribute = 
      BrainModelSurfaceCellAttributeAssignment::ASSIGN_ATTRIBUTE_AREA;
   BrainModelSurfaceCellAttributeAssignment::getAttributeNamesAndValues(
                                    attributeNames, attributeValues);
   bool attributeFound = false;
   for (unsigned int i = 0; i < attributeNames.size(); i++) {
      if (attributeNames[i] == attributeName) {
         attribute = attributeValues[i];
         attributeFound = true;
      }
   }
   if (attributeFound == false) {
      throw CommandException("Invalid attribute: " + attributeName);
   }

   //
   // Get the assignemnt method
   //
   std::vector<QString> methodNames;
   std::vector<BrainModelSurfaceCellAttributeAssignment::ASSIGNMENT_METHOD> methodValues;
   BrainModelSurfaceCellAttributeAssignment::getAssignmentNamesAndValues(
                                    methodNames, methodValues);
   BrainModelSurfaceCellAttributeAssignment::ASSIGNMENT_METHOD assignmentMethod = 
      BrainModelSurfaceCellAttributeAssignment::ASSIGNMENT_METHOD_APPEND;
   bool methodFound = false;
   for (unsigned int i = 0; i < attributeNames.size(); i++) {
      if (methodNames[i] == assignmentMethodName) {
         assignmentMethod = methodValues[i];
         methodFound = true;
      }
   }
   if (methodFound == false) {
      throw CommandException("Invalid assignment method: " + assignmentMethodName);
   }

   //
   // Run the algorithm
   //
   BrainModelSurfaceCellAttributeAssignment
      assignFoci(&brainSet,
                 leftSurface,
                 rightSurface,
                 cerebellumSurface,
                 &fociProjectionFile,
                 &paintFile,
                 paintColumnsSelected,
                 maximumDistanceFromSurface,
                 attribute,
                 assignmentMethod,
                 QString::number(attributeID),
                 ignoreUnknownPaintNamesFlag);
   assignFoci.execute();
   
   fociProjectionFile.writeFile(outputFociProjectionFileName); 
}

      

