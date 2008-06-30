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

#include "BorderFile.h"
#include "CommandSurfaceBorderVariability.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StatisticDataGroup.h"
#include "StatisticDescriptiveStatistics.h"

/**
 * constructor.
 */
CommandSurfaceBorderVariability::CommandSurfaceBorderVariability()
   : CommandBase("-surface-border-variability",
                 "SURFACE BORDER VARIABILITY")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderVariability::~CommandSurfaceBorderVariability()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderVariability::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addFile("Input Landmark Average Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addFile("Output Border File",
                     FileFilters::getBorderGenericFileFilter());
   paramsOut.addBoolean("Do Report", false);
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderVariability::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-border-file>\n"
       + indent9 + "<input-landmark-average-border-file>\n"
       + indent9 + "<output-border-file>\n"
       + indent9 + "[-border-report] \n"
       + indent9 + "[-border-point-report] \n"
       + indent9 + "\n"
       + indent9 + "Print reports about variability between borders.\n"
       + indent9 + "\n"
       + indent9 + "Resample the input borders so that they have the same number\n"
       + indent9 + "of links as the border with the corresponding name in the \n"
       + indent9 + "input landmark average border file.  In addition, the \n"
       + indent9 + "variability radius for the borders will also be set.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderVariability::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputBorderFileName =
      parameters->getNextParameterAsString("Input Border File Name");
   const QString inputLandmarkAverageBorderFileName =
      parameters->getNextParameterAsString("Input Landmark Average Border File Name");
   const QString outputBorderFileName =
      parameters->getNextParameterAsString("Output Border File Name");
   bool borderReportFlag = false;
   bool borderPointReportFlag = false;
   while (parameters->getParametersAvailable()) {
      const QString paramName = 
         parameters->getNextParameterAsString("Optional Parameter");
      if (paramName == "-border-report") {
         borderReportFlag = true;
      }
      else if (paramName == "-border-point-report") {
         borderPointReportFlag = true;
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }
      
   BorderFile inputBorderFile;
   inputBorderFile.readFile(inputBorderFileName);

   BorderFile inputLandmarkAverageBorderFile;
   inputLandmarkAverageBorderFile.readFile(inputLandmarkAverageBorderFileName);
   
   inputBorderFile.resampleToMatchLandmarkBorders(inputLandmarkAverageBorderFile);
   
   inputBorderFile.writeFile(outputBorderFileName);
   
   if (borderReportFlag) {
      doBorderReport(inputBorderFile);
      doBorderReport(inputLandmarkAverageBorderFile);
   }
   
   if (borderPointReportFlag) {
      doBorderPointReport(inputBorderFile, inputLandmarkAverageBorderFile);
   }
}

/**
 * do the border report.
 */
void 
CommandSurfaceBorderVariability::doBorderReport(const BorderFile& bf)
{
   std::cout << QString(60, '=').toAscii().constData() << std::endl;
   std::cout << "Border Summary: "
             << FileUtilities::basename(bf.getFileName()).toAscii().constData()
             << std::endl;
   std::cout << "BorderName Min Max Mean StdDev NumberOfLinks" << std::endl;
   std::cout << std::endl;
   
   std::vector<QString> namesWithVariabilityGreaterThanTwo;
   std::vector<int> countWithVariabilityGreaterThanTwo;
   
   const int numBorders = bf.getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      const Border* border = bf.getBorder(i);
      
      //
      // Get the variability from the links
      //
      int numLinksWithVariabilityGreaterThanTwo = 0;
      std::vector<float> variability;
      const int numLinks = border->getNumberOfLinks();
      if (numLinks > 0) {
         for (int j = 0; j < numLinks; j++) {
            const float radius = border->getLinkRadius(j);
            variability.push_back(radius);
            if (radius > 2.0) {
               numLinksWithVariabilityGreaterThanTwo++;
            }
         }
      }
      
      if (numLinksWithVariabilityGreaterThanTwo > 0) {
         namesWithVariabilityGreaterThanTwo.push_back(border->getName());
         countWithVariabilityGreaterThanTwo.push_back(numLinksWithVariabilityGreaterThanTwo);
      }
      
      //
      // Compute descriptive statistics
      //
      StatisticDataGroup sdg(&variability,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
      StatisticDescriptiveStatistics sds;
      sds.addDataGroup(&sdg);
      try {
         sds.execute();
      }
      catch (StatisticException&) {
      }
      
      //
      // Print results
      //
      float minValue, maxValue;
      sds.getMinimumAndMaximum(minValue, maxValue);
      std::cout << border->getName().toAscii().constData()
                << " "
                << minValue
                << " "
                << maxValue
                << " "
                << sds.getMean()
                << " "
                << sds.getStandardDeviation()
                << " "
                << numLinks
                << std::endl;
   }
   
   const int numGT2 = static_cast<int>(namesWithVariabilityGreaterThanTwo.size());
   if (numGT2 > 0) {
      std::cout << QString(60, '=').toAscii().constData() << std::endl;
      std::cout << "The following borders had at least one point whose ratio exceeded 2.0: " << std::endl;
      for (int i = 0; i < numGT2; i++) {
         std::cout << namesWithVariabilityGreaterThanTwo[i].toAscii().constData()
                   << " : " 
                   << countWithVariabilityGreaterThanTwo[i]
                   << " points"
                   << std::endl;
      }
   }
   
   std::cout << std::endl;
}
      
/**
 * do the border point report.
 */
void 
CommandSurfaceBorderVariability::doBorderPointReport(const BorderFile& bfIn,
                                               const BorderFile& avgBorderFileIn)
                                                           throw (CommandException)
{
   std::vector<QString> originalBorderNames;
   BorderFile bf = bfIn;
   const int numBorders = bf.getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      Border* b = bf.getBorder(i);
      originalBorderNames.push_back(b->getName());
      b->removeLandmarkRaterInfoFromBorderName();
   }
   
   BorderFile avgBorderFile = avgBorderFileIn;
   const int numAvgBorders = avgBorderFile.getNumberOfBorders();
   for (int i = 0; i < numAvgBorders; i++) {
      avgBorderFile.getBorder(i)->removeLandmarkRaterInfoFromBorderName();
   }
   
   std::cout << QString(60, '=').toAscii().constData() << std::endl;
   std::cout << "Source Border: "
             << FileUtilities::basename(bf.getFileName()).toAscii().constData()
             << std::endl;
   for (int i = 0; i < numBorders; i++) {
      const Border* border = bf.getBorder(i);
      const Border* avgBorder = avgBorderFile.getBorderByName(border->getName());
      if (avgBorder == NULL) {
         throw CommandException("Border named " 
                                + border->getName()
                                + " not found in average border file.");
      }      
      
      //
      // Get the variability from the links
      //
      const int numLinks = border->getNumberOfLinks();
      if (numLinks != avgBorder->getNumberOfLinks()) {
         throw CommandException("Border named "
                                + border->getName()
                                + " has different number of links in the border files.");
      }

      std::cout << QString(60, '=').toAscii().constData() << std::endl;
      std::cout << "Border Name: " 
                << originalBorderNames[i].toAscii().constData()
                << std::endl;
      std::cout << std::endl;
      std::cout << "PointIndex Distance TargetVar Ratio" << std::endl;
      if (numLinks > 0) {
         for (int j = 0; j < numLinks; j++) {
            const float avg3dVar = avgBorder->getLinkRadius(j);
            const float ratio    = border->getLinkRadius(j);
            const float d = ratio * avg3dVar; 
            std::cout << j 
                      << " " 
                      << d
                      << " "
                      << avg3dVar
                      << " "
                      << ratio;
            if (ratio > 2.0) {
               std::cout << " ***";
            }
            std::cout << std::endl;
         }
      }
   }
   std::cout << std::endl;
}


