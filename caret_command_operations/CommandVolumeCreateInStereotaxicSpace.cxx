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

#include "CommandVolumeCreateInStereotaxicSpace.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "StereotaxicSpace.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandVolumeCreateInStereotaxicSpace::CommandVolumeCreateInStereotaxicSpace()
   : CommandBase("-volume-create-in-stereotaxic-space",
                 "VOLUME CREATE IN STEREOTAXIC SPACE")
{
}

/**
 * destructor.
 */
CommandVolumeCreateInStereotaxicSpace::~CommandVolumeCreateInStereotaxicSpace()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeCreateInStereotaxicSpace::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);
   std::vector<QString> spaceNames;
   for (unsigned int i = 0; i < spaces.size(); i++) {
      if ((spaces[i].getSpace() != StereotaxicSpace::SPACE_UNKNOWN) &&
          (spaces[i].getSpace() != StereotaxicSpace::SPACE_OTHER)) {
         const QString name = spaces[i].getName();
         spaceNames.push_back(name);
      }
   }
   
   paramsOut.clear();
   paramsOut.addListOfItems("Stereotaxic Space",
                            spaceNames,
                            spaceNames);
   paramsOut.addFile("Output Volume File", 
                         FileFilters::getVolumeGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeCreateInStereotaxicSpace::getHelpInformation() const
{
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<STEREOTAXIC-SPACE>\n"
       + indent9 + "<output-volume-file-name-and-label>\n"
       + indent9 + "\n"
       + indent9 + "Create a volume in the specified stereotaxic space.\n"
       + indent9 + "Valid spaces are:\n");
   for (unsigned int i = 0; i < spaces.size(); i++) {
      if ((spaces[i].getSpace() != StereotaxicSpace::SPACE_UNKNOWN) &&
          (spaces[i].getSpace() != StereotaxicSpace::SPACE_OTHER)) {
         const QString name = spaces[i].getName();
         helpInfo += (indent9 + "   " + name + "\n");
      }
   }
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeCreateInStereotaxicSpace::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);

   const QString stereotaxicSpaceName = 
      parameters->getNextParameterAsString("Stereotaxic Space");
   QString outputVolumeFileName, outputVolumeFileLabel;
   parameters->getNextParameterAsVolumeFileNameAndLabel("Output Volume File Name",
                                                        outputVolumeFileName,
                                                        outputVolumeFileLabel);

   StereotaxicSpace space = StereotaxicSpace::getStereotaxicSpace(stereotaxicSpaceName);
   if ((space.getSpace() == StereotaxicSpace::SPACE_UNKNOWN) ||
       (space.getSpace() == StereotaxicSpace::SPACE_OTHER)) {
      throw CommandException("ERROR: Stereotaxic space name not recognized.");
   }

   int dimensions[3];
   space.getDimensions(dimensions);
   float spacing[3];
   space.getVoxelSize(spacing);
   float origin[3];
   space.getOrigin(origin);
   const VolumeFile::ORIENTATION orient[3] = {
      VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
      VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
      VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR
   };
   
   VolumeFile volume;
   volume.initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT,
                     dimensions,
                     orient,
                     origin,
                     spacing);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeFileName, outputVolumeFileLabel);
}

      

