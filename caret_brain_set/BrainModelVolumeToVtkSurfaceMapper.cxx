
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

#include <cmath>

#include "BrainModelVolumeToVtkSurfaceMapper.h"
#include "PaletteFile.h"
#include "VolumeFile.h"
#include "VtkModelFile.h"

/**
 * constructor.
 */
BrainModelVolumeToVtkSurfaceMapper::BrainModelVolumeToVtkSurfaceMapper(
                       BrainSet* bs,
                       VtkModelFile* vtkModelFileIn,
                       const VolumeFile* volumeFileIn,
                       const PaletteFile* paletteFileIn,
                       const int paletteNumberIn)
   : BrainModelAlgorithm(bs),
     vtkModelFile(vtkModelFileIn),
     volumeFile(volumeFileIn),
     paletteFile(paletteFileIn),
     paletteNumber(paletteNumberIn)
{
}

/**
 * destructor.
 */
BrainModelVolumeToVtkSurfaceMapper::~BrainModelVolumeToVtkSurfaceMapper()
{
}


/**
 * execute the algorithm.
 */
void 
BrainModelVolumeToVtkSurfaceMapper::execute() throw (BrainModelAlgorithmException)
{
   //
   // Check inputs
   //
   if (vtkModelFile == NULL) {
      throw BrainModelAlgorithmException("VTK model file is invalid.");
   }
   if (volumeFile == NULL) {
      throw BrainModelAlgorithmException("Volume file is invalid.");
   }
   if (paletteFile == NULL) {
      throw BrainModelAlgorithmException("Palette file is invalid.");
   }
   if ((paletteNumber < 0) ||
       (paletteNumber >= paletteFile->getNumberOfPalettes())) {
      throw BrainModelAlgorithmException("Palette number is invalid.");
   }

   //
   // Get the palette
   //
   const Palette* palette = paletteFile->getPalette(paletteNumber);
   
   //
   // Get volume minimum and maximum values
   //
   float minValue, maxValue;
   VolumeFile mappingVolume(*volumeFile);
   mappingVolume.getMinMaxVoxelValues(minValue, maxValue);
   
   //
   // Loop through the coordinates of the model
   //   
   const CoordinateFile* cf = vtkModelFile->getCoordinateFile();
   const int numCoords = cf->getNumberOfCoordinates();
   for (int i = 0; i < numCoords; i++) {
      const float* xyz = cf->getCoordinate(i);
      int ijk[3];
      if (mappingVolume.convertCoordinatesToVoxelIJK(xyz, ijk)) {
         //
         // Normalize the data value
         //
         const float voxelValue = volumeFile->getVoxel(ijk, 0);
         float normalized = 0.0;
         if (voxelValue > 0.0) {
            normalized = voxelValue / maxValue;
         }
         else if (voxelValue < 0.0) {
            normalized = -std::fabs(voxelValue / minValue);
         }
         
         //
         // Get the coloring
         //
         const bool interpolateColor = false;
         bool isNoneColor = false; 
         unsigned char colors[4]; 
         palette->getColor(normalized, interpolateColor,
                           isNoneColor, colors);
         colors[3] = 255;
                                    
         //
         // Set the point's color
         //
         vtkModelFile->setPointColor(i, colors);
      }      
   } 
}
