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

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "CellColorFile.h"
#include "ContourCellColorFile.h"
#include "FileFilters.h"
#include "FociColorFile.h"
#include "CommandColorFileAddColor.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandColorFileAddColor::CommandColorFileAddColor()
   : CommandBase("-color-file-add-color",
                 "COLOR FILE ADD COLOR")
{
}

/**
 * destructor.
 */
CommandColorFileAddColor::~CommandColorFileAddColor()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandColorFileAddColor::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   QStringList filters;
   filters << FileFilters::getAreaColorFileFilter();
   filters << FileFilters::getBorderColorFileFilter();
   filters << FileFilters::getCellColorFileFilter();
   filters << FileFilters::getContourCellColorFileFilter();
   filters << FileFilters::getFociColorFileFilter();
   
   paramsOut.clear();
   paramsOut.addFile("Input Color File Name",
                     filters);
   paramsOut.addFile("Output Color File Name",
                     filters);
   paramsOut.addString("Color Name");
   paramsOut.addInt("Red", 0, 0, 255);
   paramsOut.addInt("Green", 0, 0, 255);
   paramsOut.addInt("Blue", 0, 0, 255);
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandColorFileAddColor::getHelpInformation() const
{  
   ColorFile::ColorStorage color;
   unsigned char r, g, b, a;
   color.getRgba(r, g, b, a);
   const QString lineSize(QString::number(color.getLineSize()));
   const QString pointSize(QString::number(color.getPointSize()));
   const QString alpha(QString::number(a));
   const QString symbol(ColorFile::ColorStorage::symbolToText(color.getSymbol()));
   std::vector<QString> symbolTypes;
   ColorFile::ColorStorage::getAllSymbolTypesAsStrings(symbolTypes);
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <input-color-file-name>\n"
       + indent9 + "   <output-color-file-name>\n"
       + indent9 + "   <color-name>\n"
       + indent9 + "   <red-color-component>\n"
       + indent9 + "   <green-color-component>\n"
       + indent9 + "   <blue-color-component>\n"
       + indent9 + "   [-alpha      alpha-color-component]\n"
       + indent9 + "   [-line-size  size-value]\n"
       + indent9 + "   [-point-size size-value]\n"
       + indent9 + "   [-symbol     symbol-type]\n"
       + indent9 + "\n"
       + indent9 + "Add a color to the color file.  The input color file\n"
       + indent9 + "does not need to exist.  \n"
       + indent9 + "\n"
       + indent9 + "Color components range (0, 255).\n"
       + indent9 + "\n"
       + indent9 + "Defaults:\n"
       + indent9 + "   alpha:      " + alpha + "\n"
       + indent9 + "   line-size:  " + lineSize + "\n"
       + indent9 + "   point-size: " + pointSize + "\n"
       + indent9 + "   symbol:     " + symbol + "\n"
       + indent9 + "\n"
       + indent9 + "Value symbol types are:\n");
   for (int i = 0; i < static_cast<int>(symbolTypes.size()); i++) {
      helpInfo += (indent9 + "   " + symbolTypes[i] + "\n");
   }
   helpInfo += (indent9 + "\n");

      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandColorFileAddColor::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   ColorFile::ColorStorage color;
   unsigned char red, green, blue, alpha;
   color.getRgba(red, green, blue, alpha);
   float lineSize = color.getLineSize();
   float pointSize = color.getPointSize();
   ColorFile::ColorStorage::SYMBOL symbol = color.getSymbol();
   std::vector<QString> symbolTypes;
   ColorFile::ColorStorage::getAllSymbolTypesAsStrings(symbolTypes);
   
   //
   // Get the parameters
   //
   const QString inputColorFileName =
      parameters->getNextParameterAsString("Input Color File Name");
   const QString outputColorFileName =
      parameters->getNextParameterAsString("Output Color File Name");
   const QString colorName =
      parameters->getNextParameterAsString("Color Name");
   red =
      parameters->getNextParameterAsInt("Red Color Component");
   green =
      parameters->getNextParameterAsInt("Green Color Component");
   blue =
      parameters->getNextParameterAsInt("Blue Color Component");
   while (parameters->getParametersAvailable()) {
      const QString paramName = 
        parameters->getNextParameterAsString("Optional Color Parameter");
      if (paramName == "-alpha") {
         alpha = parameters->getNextParameterAsInt("Alpha");
      }
      else if (paramName == "-line-size") {
         lineSize = parameters->getNextParameterAsFloat("Line Size");
      }
      else if (paramName == "-point-size") {
         pointSize = parameters->getNextParameterAsFloat("Point Size");
      }
      else if (paramName == "-symbol") {
        const QString symbolString  = parameters->getNextParameterAsString("Symbol");
        bool found = false;
        for (unsigned int i = 0; i < symbolTypes.size(); i++) {
           if (symbolString == symbolTypes[i]) {
              found = true;
              break;
           }
        }
        if (found == false) {
           throw CommandException("invalid symbol type: "
                                  + symbolString);
        }
        symbol = ColorFile::ColorStorage::textToSymbol(symbolString);
      }
      else {
         throw CommandException("color parameter: \""
                                + paramName + "\"");
      }
   }
   
   ColorFile* colorFile = NULL;
   
   if (inputColorFileName.isEmpty() == false) {
      colorFile = ColorFile::getColorFileFromFileNameExtension(inputColorFileName);
      if (QFile::exists(inputColorFileName)) {
         colorFile->readFile(inputColorFileName);
      }
   }
   
   if (colorFile == NULL) {
      colorFile = ColorFile::getColorFileFromFileNameExtension(outputColorFileName);
   }
   
   colorFile->addColor(colorName,
                      red,
                      green,
                      blue,
                      alpha,
                      pointSize,
                      lineSize,
                      symbol);
                      
   colorFile->writeFile(outputColorFileName);

   delete colorFile;
}

      

