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

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderReverse.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderReverse::CommandSurfaceBorderReverse()
   : CommandBase("-surface-border-reverse",
                 "SURFACE BORDER REVERSE")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderReverse::~CommandSurfaceBorderReverse()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderReverse::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addVariableListOfParameters("Border Reverse Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderReverse::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-border-projection-file-name>\n"
       + indent9 + "<output-border-projection-file-name>\n"
       + indent9 + "[-all]\n"
       + indent9 + "[-border-name  border-name]\n"
       + indent9 + "\n"
       + indent9 + "Reverse the order of links in borders.\n"
       + indent9 + "\n"
       + indent9 + "Either \"-all\", which reverses all borders in the file, or\n"
       + indent9 + "\"-border-name\", which reverses borders with specific names\n"
       + indent9 + "MUST be specified.  \"-border-name\" may be used multiple\n"
       + indent9 + "times to reverse additional borders.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderReverse::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputBorderProjectionFileName =
      parameters->getNextParameterAsString("Input Border Projection File Name");
   const QString outputBorderProjectionFileName =
      parameters->getNextParameterAsString("Output Border Projection File Name");
      
   //
   // Check optional parameters
   //
   bool reverseAllFlag = false;
   std::vector<QString> borderNames;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("parameter");
      if (paramName == "-all") {
         reverseAllFlag = true;;
      }
      else if (paramName == "-border-name") {
         const QString nameString = 
            parameters->getNextParameterAsString("border name");
         borderNames.push_back(nameString);
      }
      else {
         throw CommandException("unknown parameter: "
                                + paramName);
      }
   }

   const int numberOfBorderNames = static_cast<int>(borderNames.size());
   if ((reverseAllFlag == false) &&
       (numberOfBorderNames <= 0)) {
      throw CommandException("either \"-all\" or "
                             "\"-border-name\" must be specified to indicate "
                             "which borders should be reversed.");
   }
   
   // 
   // Read the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(inputBorderProjectionFileName);

   //
   // Loop through the border projections
   //
   const int num = borderProjectionFile.getNumberOfBorderProjections();
   for (int i = 0; i < num; i++) {
      //
      // Get the border projection
      //
      BorderProjection* bp = borderProjectionFile.getBorderProjection(i);
      
      //
      // Determine if the border projection should be reversed
      //
      bool reverseIt = false;
      if (reverseAllFlag) {
         reverseIt = true;
      }
      else {
         if (std::find(borderNames.begin(),
                       borderNames.end(),
                       bp->getName()) != borderNames.end()) {
            reverseIt = true;
         }
      }
      
      //
      // Should border projection be reversed
      //
      if (reverseIt) {
         //
         // Reverse border projection
         //
         bp->reverseOrderOfBorderProjectionLinks();
      }
   }
   
   //
   // Write output border projection file
   //
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
}

      

