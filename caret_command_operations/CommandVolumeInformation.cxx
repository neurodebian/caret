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

#include "CommandVolumeInformation.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "VolumeFile.h"
#include <iomanip>

/**
 * constructor.
 */
CommandVolumeInformation::CommandVolumeInformation()
   : CommandBase("-volume-information",
                 "VOLUME INFORMATION")
{
}

/**
 * destructor.
 */
CommandVolumeInformation::~CommandVolumeInformation()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandVolumeInformation::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Volume File Name", FileFilters::getVolumeGenericFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandVolumeInformation::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-volume-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Print information about the volume.  If possible, the\n"
       + indent9 + "volume data is transformated into LPI (neurological)"
       + indent9 + "orientation."
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandVolumeInformation::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Volume File Name");

   VolumeFile volume;
   volume.setVolumeSpace(VolumeFile::VOLUME_SPACE_COORD_LPI);
   volume.readFile(inputVolumeFileName);

   std::cout << "Volume File: " << FileUtilities::basename(inputVolumeFileName).toAscii().constData() << std::endl;
   
   int dim[3];
   volume.getDimensions(dim);
   std::cout << "   dimensions: " << std::endl;
   std::cout << indent9.toAscii().data() << "X: " << dim[0] << std::endl;
   std::cout << indent9.toAscii().data() << "Y: " << dim[1] << std::endl;
   std::cout << indent9.toAscii().data() << "Z: " << dim[2] << std::endl;
   if(volume.getNumberOfSubVolumes() > 1) std::cout << indent9.toAscii().data() << "Time: " << volume.getNumberOfSubVolumes() << std::endl;
   if(volume.getNumberOfComponentsPerVoxel() > 1) std::cout << indent9.toAscii().data() << "Components Per Voxel: " << volume.getNumberOfComponentsPerVoxel() << std::endl;
   
   float space[3];
   volume.getSpacing(space);
   std::ios_base::fmtflags flags = std::cout.flags( );
   std::cout << "   spacing: " << std::setprecision(std::numeric_limits<float>::digits10) << std::scientific <<space[0] << ", " << space[1] << ", " << space[2] << std::endl;
   
   float org[3];
   volume.getOrigin(org);
   std::cout << "   origin (center of first voxel): " << std::setprecision(std::numeric_limits< float >::digits10) << std::scientific<<org[0] << ", " << org[1] << ", " << org[2] << std::endl;
   
   std::cout.flags(flags);
   VolumeFile::ORIENTATION orient[3];
   volume.getOrientation(orient);
   std::cout << "   orientation: " 
             << VolumeFile::getOrientationLabel(orient[0]).toAscii().constData() << ", "
             << VolumeFile::getOrientationLabel(orient[1]).toAscii().constData() << ", "
             << VolumeFile::getOrientationLabel(orient[2]).toAscii().constData() << std::endl;
            
   std::cout << "   label: " << volume.getDescriptiveLabel().toAscii().constData() << std::endl;
   
   float minValue, maxValue;
   volume.getMinMaxVoxelValues(minValue, maxValue);
   std::cout << "   voxel range: " << minValue << ", " << maxValue << std::endl;
   
   const int numRegionNames = volume.getNumberOfRegionNames();
   if (numRegionNames > 0) {
      std::cout << "   Region Names: " << std::endl;
      for (int i = 0; i < numRegionNames; i++) {
         std::cout << "      " << i << " " << volume.getRegionNameFromIndex(i).toAscii().constData() << std::endl;
      }
   }
}

      

