
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

#include "StereotaxicSpace.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
StereotaxicSpace::StereotaxicSpace(const QString& nameIn,
                                   const int dimensionsIn[3],
                                   const float originIn[3],
                                   const float voxelSizeIn[3])
{
   name = nameIn;
   for (int i = 0; i < 3; i++) {
      dimensions[i] = dimensionsIn[i];
      origin[i]     = originIn[i];
      voxelSize[i]  = voxelSizeIn[i];
   }
}
                 
/**
 * constructor space name.
 */
StereotaxicSpace::StereotaxicSpace(const QString& nameIn)
{
   setDataFromSpace(getSpaceFromName(nameIn));
}
      
/**
 * constructor.
 */
StereotaxicSpace::StereotaxicSpace(const SPACE spaceIn)
{
   setDataFromSpace(spaceIn);
}

/**
 * set data from space.
 */
void 
StereotaxicSpace::setDataFromSpace(const SPACE spaceIn)
{
   space = spaceIn;
   
   setData("UNKNOWN",
                         0, 0, 0,
                         0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0);

   switch (space) {
      case SPACE_UNKNOWN:
         setData("UNKNOWN",
                               0, 0, 0,
                               0.0, 0.0, 0.0,
                               0.0, 0.0, 0.0);
         break;
      case SPACE_OTHER:
         setData("OTHER",
                               0, 0, 0,
                               0.0, 0.0, 0.0,
                               0.0, 0.0, 0.0);
         break;
      case SPACE_AFNI_TALAIRACH:
         setData("AFNI",
                               161, 191, 151,
                               1.0, 1.0, 1.0,
                               -80.0, -110.0, -65.0);
         break;
      case SPACE_FLIRT:
         setData("FLIRT",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_MACAQUE_F99:
         setData("MACAQUE-F99",
                               143, 187, 118,
                               0.5, 0.5, 0.5,
                               -35.75, -54.75, -30.25);
         break;
      case SPACE_MACAQUE_F6:
         setData("MACAQUE-F6",
                               143, 187, 118,
                               0.5, 0.5, 0.5,
                               -35.75, -54.75, -30.25);
         break;
      case SPACE_MRITOTAL:
         setData("MRITOTAL",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_SPM:
         setData("SPM",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_SPM_95:
         setData("SPM95",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_SPM_96:
         setData("SPM96",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_SPM_99:
         setData("SPM99",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_SPM_2:
         setData("SPM2",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_SPM_5:
         setData("SPM5",
                               182, 217, 182,
                               1.0, 1.0, 1.0,
                               -90.0, -126.0, -72.0);
         break;
      case SPACE_T88:
         setData("T88",
                               161, 191, 151,
                               1.0, 1.0, 1.0,
                               -80.0, -110.0, -65.0);
         break;
      case SPACE_WU_7112B:
         setData("711-2B",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -88.0, -122.0, -73.0);
         break;
      case SPACE_WU_7112B_111:
         setData("711-2B-111",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -88.0, -122.0, -73.0);
         break;
      case SPACE_WU_7112B_222:
         setData("711-2B-222",
                               128, 128, 75,
                               2.0, 2.0, 2.0,
                               -129.0, -129.0, -68.0);
         break;
      case SPACE_WU_7112B_333:
         setData("711-2B-333",
                               48, 64, 48,
                               3.0, 3.0, 3.0,
                               -73.5, -108.0, -60.0);
         break;
      case SPACE_WU_7112C:
         setData("711-2C",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -88.0, -122.0, -73.0);
         break;
      case SPACE_WU_7112C_111:
         setData("711-2C-111",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -89.0, -122.0, -73.0);
         break;
      case SPACE_WU_7112C_222:
         setData("711-2C-222",
                               128, 128, 75,
                               2.0, 2.0, 2.0,
                               -129.0, -129.0, -68.0);
         break;
      case SPACE_WU_7112C_333:
         setData("711-2C-333",
                               48, 64, 48,
                               3.0, 3.0, 3.0,
                               -73.5, -108.0, -60.0);
         break;
      case SPACE_WU_7112O:
         setData("711-2O",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -89.0, -124.0, -75.0);
         break;
      case SPACE_WU_7112O_111:
         setData("711-2O-111",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -89.0, -124.0, -75.0);
         break;
      case SPACE_WU_7112O_222:
         setData("711-2O-222",
                               128, 128, 75,
                               2.0, 2.0, 2.0,
                               -129.0, -129.0, -68.0);
         break;
      case SPACE_WU_7112O_333:
         setData("711-2O-333",
                               48, 64, 48,
                               3.0, 3.0, 3.0,
                               -73.5, -108.0, -60.0);
         break;
      case SPACE_WU_7112Y:
         setData("711-2Y",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -88.0, -122.0, -73.0);
         break;
      case SPACE_WU_7112Y_111:
         setData("711-2Y-111",
                               176, 208, 176,
                               1.0, 1.0, 1.0,
                               -88.0, -122.0, -73.0);
         break;
      case SPACE_WU_7112Y_222:
         setData("711-2Y-222",
                               128, 128, 75,
                               2.0, 2.0, 2.0,
                               -129.0, -129.0, -68.0);
         break;
      case SPACE_WU_7112Y_333:
         setData("711-2Y-333",
                               48, 64, 48,
                               3.0, 3.0, 3.0,
                               -73.5, -108.0, -60.0);
         break;
      case SPACE_NUMBER_OF_SPACES:
         setData("UNKNOWN",
                               0, 0, 0,
                               0.0, 0.0, 0.0,
                               0.0, 0.0, 0.0);
         break;
   }
}
      
/**
 * constructor.
 */
void
StereotaxicSpace::setData(const QString& nameIn,
                             const int dimX, const int dimY, const int dimZ,
                             const float voxSizeX, const float voxSizeY, const float voxSizeZ,
                             const float originX, const float originY, const float originZ)
{
   name = nameIn;
   dimensions[0] = dimX;
   dimensions[1] = dimY;
   dimensions[2] = dimZ;
   origin[0]     = originX;
   origin[1]     = originY;
   origin[2]     = originZ;
   voxelSize[0]  = voxSizeX;
   voxelSize[1]  = voxSizeY;
   voxelSize[2]  = voxSizeZ;
}
                       
/**
 * constructor.
 */
StereotaxicSpace::StereotaxicSpace()
{
   name = "";
   dimensions[0] = 0;
   dimensions[1] = 0;
   dimensions[2] = 0;
   origin[0]     = 0.0;
   origin[1]     = 0.0;
   origin[2]     = 0.0;
   voxelSize[0]  = 0.0;
   voxelSize[1]  = 0.0;
   voxelSize[2]  = 0.0;
}
                       
/**
 * destructor.
 */
StereotaxicSpace::~StereotaxicSpace()
{
}

/**
 * get the dimensions of the space.
 */
void 
StereotaxicSpace::getDimensions(int dimensionsOut[3]) const
{
   for (int i = 0; i < 3; i++) {
      dimensionsOut[i] = dimensions[i];
   }
}

/**
 * get the origin of the space.
 */
void 
StereotaxicSpace::getOrigin(float originOut[3]) const
{
   for (int i = 0; i < 3; i++) {
      originOut[i] = origin[i];
   }
}

/**
 * get the voxel size of the space.
 */
void 
StereotaxicSpace::getVoxelSize(float voxelSizeOut[3]) const
{
   for (int i = 0; i < 3; i++) {
      voxelSizeOut[i] = voxelSize[i];
   }
}

/**
 * get information about a stereotaxic space.
 */
StereotaxicSpace 
StereotaxicSpace::getStereotaxicSpace(const SPACE space)
{
   StereotaxicSpace ss(space);
   return ss;
}

/**
 * get teh space from the name.
 */
QString 
StereotaxicSpace::cleanupSpaceName(const QString& spaceNameIn)
{
   QString spaceName(spaceNameIn.toUpper());
   if (spaceName == "7112B") {
      spaceName = "711-2B";
   }
   else if(spaceName == "7112C") {
      spaceName = "711-2C";
   }
   else if(spaceName == "7112O") {
      spaceName = "711-2O";
   }
   else if(spaceName == "7112Y") {
      spaceName = "711-2Y";
   }
   else if (spaceName == "MACAQUE") {
      spaceName = "MACAQUE-F99";
   }
   //else if (spaceName.left(5) == "711-2") {
   //   spaceName == "711-2B-111";
   //}
   else if (spaceName == "SPM_DEFAULT") {
      spaceName = "SPM99";
   }
   else if (spaceName == "SPM_TEMPLATE") {
      spaceName = "SPM99";
   }
   return spaceName;
}
      
/**
 * get a space from its name.
 */
StereotaxicSpace::SPACE 
StereotaxicSpace::getSpaceFromName(const QString& name)
{
   return getStereotaxicSpace(name).getSpace();
}
      
/**
 * get information about a stereotaxic space.
 * Returns the space SPACE_NON_STANDARD_OR_UNKNOWN if name is not recognized.
 */
StereotaxicSpace 
StereotaxicSpace::getStereotaxicSpace(const QString& spaceNameIn)
{
   const QString spaceName = cleanupSpaceName(spaceNameIn);
   
   for (int i = 0; i < SPACE_NUMBER_OF_SPACES; i++) {
      StereotaxicSpace ss = getStereotaxicSpace(static_cast<SPACE>(i));
      if (ss.getName() == StringUtilities::makeUpperCase(spaceName)) {
         return ss;
      }
   }
   
   return getStereotaxicSpace(SPACE_UNKNOWN);
}
      
/**
 * is the name that of a valid stereotaxic space.
 */
bool 
StereotaxicSpace::validStereotaxicSpaceName(const QString& name)
{
   StereotaxicSpace ss = getStereotaxicSpace(name);
   return (ss.getSpace() != SPACE_UNKNOWN);
}
      
/**
 * get all stereotaxic spaces.
 */
void 
StereotaxicSpace::getAllStereotaxicSpaces(std::vector<StereotaxicSpace>& allSpacesOut)
{
   allSpacesOut.clear();
   
   for (int i = 0; i < SPACE_NUMBER_OF_SPACES; i++) {
      allSpacesOut.push_back(getStereotaxicSpace(static_cast<SPACE>(i)));
   }
}
      
