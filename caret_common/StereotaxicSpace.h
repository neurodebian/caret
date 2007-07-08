
#ifndef __STEREOTAXIC_SPACE_H__
#define __STEREOTAXIC_SPACE_H__

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

#include <QString>
#include <vector>

/// class for storing a stereotaxic space
class StereotaxicSpace {
   public:
      /// stereotaxic spaces available
      enum SPACE {
         /// unknown space
         SPACE_UNKNOWN,
         /// other space
         SPACE_OTHER,
         /// AFNI Talairach space
         SPACE_AFNI_TALAIRACH,
         /// FLIRT space
         SPACE_FLIRT,
         /// macaque atlas
         SPACE_MACAQUE_F6,
         /// Macaque F99
         SPACE_MACAQUE_F99,
         /// MRITOTAL space
         SPACE_MRITOTAL,
         /// SPM space
         SPACE_SPM,
         /// SPM 95 space
         SPACE_SPM_95,
         /// SPM 95 space
         SPACE_SPM_96,
         /// SPM 99 Template space
         SPACE_SPM_99,
         /// SPM 2 Template space
         SPACE_SPM_2,
         /// SPM 5 space
         SPACE_SPM_5,
         /// Talairach 88 space (same as AFNI)
         SPACE_T88,
         /// Washington University 711-2B space
         SPACE_WU_7112B,
         /// Washington University 711-2B 1mm voxelspace
         SPACE_WU_7112B_111,
         /// Washington University 711-2B 2mm voxelspace
         SPACE_WU_7112B_222,
         /// Washington University 711-2B 3mm voxelspace
         SPACE_WU_7112B_333,
         /// Washington University 711-2C space
         SPACE_WU_7112C,
         /// Washington University 711-2C 1mm voxelspace
         SPACE_WU_7112C_111,
         /// Washington University 711-2C 2mm voxelspace
         SPACE_WU_7112C_222,
         /// Washington University 711-2C 3mm voxelspace
         SPACE_WU_7112C_333,
         /// Washington University 711-2O space
         SPACE_WU_7112O,
         /// Washington University 711-2O 1mm voxelspace
         SPACE_WU_7112O_111,
         /// Washington University 711-2O 2mm voxelspace
         SPACE_WU_7112O_222,
         /// Washington University 711-2O 3mm voxelspace
         SPACE_WU_7112O_333,
         /// Washington University 711-2Y space
         SPACE_WU_7112Y,
         /// Washington University 711-2Y 1mm voxelspace
         SPACE_WU_7112Y_111,
         /// Washington University 711-2Y 2mm voxelspace
         SPACE_WU_7112Y_222,
         /// Washington University 711-2Y 3mm voxelspace
         SPACE_WU_7112Y_333,
         /// always last
         SPACE_NUMBER_OF_SPACES
      };
      
      /// constructor
      StereotaxicSpace(const SPACE spaceIn);
      
      /// constructor
      StereotaxicSpace(const QString& nameIn,
                       const int dimensionsIn[3],
                       const float originIn[3],
                       const float voxelSizeIn[3]);
                       
      /// constructor
      StereotaxicSpace();
      
      /// destructor
      ~StereotaxicSpace();
      
      /// equality operator (just checkes that names are the same)
      bool operator==(const StereotaxicSpace& ss) { return (name == ss.name); }
      
      /// get the space
      SPACE getSpace() const { return space; }
      
      /// get the name of the space
      QString getName() const { return name; }
      
      /// get the dimensions of the space
      void getDimensions(int dimensionsOut[3]) const;
      
      /// get the origin of the space
      void getOrigin(float originOut[3]) const;
      
      /// get the voxel size of the space
      void getVoxelSize(float voxelSizeOut[3]) const;
      
      /// get information about a stereotaxic space
      static StereotaxicSpace getStereotaxicSpace(const SPACE space);
      
      /// get information about a stereotaxic space
      static StereotaxicSpace getStereotaxicSpace(const QString& spaceName);
      
      /// get all stereotaxic spaces
      static void getAllStereotaxicSpaces(std::vector<StereotaxicSpace>& allSpacesOut);
      
      /// is the name that of a valid stereotaxic space
      static bool validStereotaxicSpaceName(const QString& name);
      
   protected:
      /// set the space data
      void setData(const QString& nameIn,
                  const int dimX, const int dimY, const int dimZ,
                  const float voxSizeX, const float voxSizeY, const float voxSizeZ,
                  const float originX, const float originY, const float originZ);
                       
      /// the space
      SPACE space;
      
      /// name of the space
      QString name;
      
      /// dimensions of the space
      int dimensions[3];
      
      /// origin of the spcae
      float origin[3];
      
      /// voxel size of the space
      float voxelSize[3];
};

#endif // __STEREOTAXIC_SPACE_H__

