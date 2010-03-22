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

#include <algorithm>
#include <iostream>
#include <map>

#include "BorderFile.h"
#include "CommandSurfaceBorderLengths.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StatisticDataGroup.h"
#include "StatisticDescriptiveStatistics.h"

/**
 * constructor.
 */
CommandSurfaceBorderLengths::CommandSurfaceBorderLengths()
   : CommandBase("-surface-border-lengths",
                 "SURFACE BORDER LENGTHS")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderLengths::~CommandSurfaceBorderLengths()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderLengths::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addMultipleFiles("Input Border Files", FileFilters::getBorderGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderLengths::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-border-file-1>\n"
       + indent9 + "[additional-input-border-files]\n"
       + indent9 + "\n"
       + indent9 + "Get information about border lengths.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderLengths::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   std::vector<QString> inputBorderFileNames;
   inputBorderFileNames.push_back(
      parameters->getNextParameterAsString("Input Border File Name 1"));
   while (parameters->getParametersAvailable()) {
      inputBorderFileNames.push_back(
         parameters->getNextParameterAsString("Input Border File Name"));
   }

   //
   // Border lengths
   //
   std::vector<BorderLengths> borderLengths;
   
   //
   // Read the input border files
   //
   const int numBorderFiles = static_cast<int>(inputBorderFileNames.size());
   for (int i = 0; i < numBorderFiles; i++) {
      BorderFile bf;
      bf.readFile(inputBorderFileNames[i]);

      std::cout << inputBorderFileNames[i].toAscii().constData() 
                << "------------------"
                << std::endl;

      const int numBorders = bf.getNumberOfBorders();
      for (int j = 0; j < numBorders; j++) {
         Border* b = bf.getBorder(j);
         const QString name = b->getName();
         
         const float length = b->getBorderLength();
         std::cout << "   " << name.toAscii().constData() 
                   << ": " << length << std::endl;

         BorderLengths* bl = NULL;
         for (unsigned int i = 0; i < borderLengths.size(); i++) {
            if (borderLengths[i].name == name) {
               bl = &borderLengths[i];
            }
         }
         if (bl == NULL) {
            BorderLengths blNew;
            blNew.name = name;
            borderLengths.push_back(blNew);
            bl = &borderLengths[borderLengths.size() - 1];
         }
         
         bl->lengths.push_back(length);
      }
   }  

   std::cout << "------------------------------------------------" << std::endl;   
   for (std::vector<BorderLengths>::iterator iter = borderLengths.begin();
        iter != borderLengths.end(); 
        iter++) {
      QString name = iter->name;
      std::vector<float> lengths = iter->lengths;
      
      StatisticDataGroup sda(&lengths,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDescriptiveStatistics stats;
      stats.addDataGroup(&sda, false);
      stats.execute();
      float minValue, maxValue;
      stats.getMinimumAndMaximum(minValue, maxValue);
      
      std::cout << name.toAscii().constData() 
                << " avg=" << stats.getMean()
                << " dev="  << stats.getStandardDeviation()
                << " min=" << minValue 
                << " max=" << maxValue
                << std::endl;
   }
}

      

