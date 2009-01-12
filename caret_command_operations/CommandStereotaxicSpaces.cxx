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

#include "CommandStereotaxicSpaces.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StereotaxicSpace.h"

/**
 * constructor.
 */
CommandStereotaxicSpaces::CommandStereotaxicSpaces()
   : CommandBase("-stereotaxic-spaces",
                 "STEREOTAXIC SPACE INFORMATION")
{
}

/**
 * destructor.
 */
CommandStereotaxicSpaces::~CommandStereotaxicSpaces()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandStereotaxicSpaces::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandStereotaxicSpaces::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "\n"
       + indent9 + "List information about stereotaxic spaces.\n"
       + indent9 + "\n"
       + indent9 + "This information listed is for an LPI orientation \n"
       + indent9 + "LPI means:\n"
       + indent9 + "   negative-X => LEFT\n"
       + indent9 + "   negative-Y => POSTERIOR\n"
       + indent9 + "   negative-Z => INFERIOR\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandStereotaxicSpaces::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);

   QString nameTitle = "NAME";
   int nameLength      = nameTitle.length();
   QString dimTitle = "DIMENSIONS";
   int dimLength       = dimTitle.length();
   QString originTitle = "ORIGIN";
   int originLength    = originTitle.length();
   QString voxelSizeTitle = "VOXEL-SIZE";
   int voxelSizeLength = voxelSizeTitle.length();
      
   std::vector<QString> nameStrings,
                        dimStrings,
                        originStrings,
                        voxelSizeStrings;
                        
   for (unsigned int i = 0; i < spaces.size(); i++) {
      const StereotaxicSpace& ss = spaces[i];
      if ((ss.getSpace() == StereotaxicSpace::SPACE_UNKNOWN) ||
          (ss.getSpace() == StereotaxicSpace::SPACE_OTHER)) {
         continue;
      }
      
      QString name = ss.getName();
      nameLength = std::max(nameLength, name.length());
      nameStrings.push_back(name);
      
      int dim[3];
      ss.getDimensions(dim);
      QString dimString =
         (QString::number(dim[0])
          + "x"
          + QString::number(dim[1])
          + "x"
          + QString::number(dim[2]));
      dimLength = std::max(dimLength, dimString.length());
      dimStrings.push_back(dimString);
      
      float origin[3];
      ss.getOrigin(origin);
      QString originString =
         (QString::number(origin[0], 'f', 1)
          + "x"
          + QString::number(origin[1], 'f', 1)
          + "x"
          + QString::number(origin[2], 'f', 1));
      originLength = std::max(originLength, originString.length());
      originStrings.push_back(originString);
      
      float voxelSizes[3];
      ss.getVoxelSize(voxelSizes);
      QString voxelSizeString =
         (QString::number(voxelSizes[0], 'f', 1)
          + "x"
          + QString::number(voxelSizes[1], 'f', 1)
          + "x"
          + QString::number(voxelSizes[2], 'f', 1));
      voxelSizeLength = std::max(voxelSizeLength, voxelSizeString.length());
      voxelSizeStrings.push_back(voxelSizeString);
   }
   
   const int spacing = 5;
   nameLength       += spacing;
   dimLength        += spacing;
   originLength     += spacing;
   voxelSizeLength  += spacing;
   
   const QString titleLine = 
      (nameTitle.leftJustified(nameLength)
       + dimTitle.leftJustified(dimLength)
       + originTitle.leftJustified(originLength)
       + voxelSizeTitle.leftJustified(voxelSizeLength));
   std::cout << titleLine.toAscii().constData() << std::endl;
   std::cout << QString(titleLine).replace(QRegExp("[A-Z]"), "-").toAscii().constData() << std::endl;

   for (unsigned int i = 0; i < nameStrings.size(); i++) {
      const QString dataLine = 
         (nameStrings[i].leftJustified(nameLength)
          + dimStrings[i].leftJustified(dimLength)
          + originStrings[i].leftJustified(originLength)
          + voxelSizeStrings[i].leftJustified(voxelSizeLength));
      std::cout << dataLine.toAscii().constData() << std::endl;
   }
}

      

