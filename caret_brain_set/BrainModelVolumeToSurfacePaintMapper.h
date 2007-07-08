
#ifndef __BRAIN_MODE_VOLUME_TO_SURFACE_PAINT_MAPPER_H__
#define __BRAIN_MODE_VOLUME_TO_SURFACE_PAINT_MAPPER_H__

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

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class PaintFile;
class VolumeFile;

/// class that maps a paint volume to a surface's paint file
class BrainModelVolumeToSurfacePaintMapper : public BrainModelAlgorithm {
   public:
      /// Constructor for a volume file in memory.
      BrainModelVolumeToSurfacePaintMapper(BrainSet* bs,
                                           BrainModelSurface* surfaceIn,
                                           VolumeFile* volumeFileIn,
                                           PaintFile* paintFileIn,
                                           const int paintColumnIn,
                                           const QString& paintColumnNameIn);

      /// Constructor for a volume file that needs to be read.
      BrainModelVolumeToSurfacePaintMapper(BrainSet* bs,
                                           BrainModelSurface* surfaceIn,
                                           const QString& volumeFileNameIn,
                                           PaintFile* paintFileIn,
                                           const int paintColumnIn,
                                           const QString& paintColumnNameIn);
                                                                                            
      /// Destructor
      ~BrainModelVolumeToSurfacePaintMapper();
      
      /// execute the algorithm
      virtual void execute() throw (BrainModelAlgorithmException);
   
   protected:
      /// volume source type
      enum MODE_VOLUME {
         MODE_VOLUME_IN_MEMORY,
         MODE_VOLUME_ON_DISK
      };
      
      /// surface for mapping
      BrainModelSurface* surface;
      
      /// volume to map
      VolumeFile* volumeFile;
      
      /// paint file to update
      PaintFile* paintFile;
      
      /// the paint file column
      int paintColumn;
      
      /// name for paint column
      QString paintColumnName;
      
      /// volume type mode
      MODE_VOLUME volumeMode;
      
      /// volume file name
      QString volumeFileName;
};

#endif  // __BRAIN_MODE_VOLUME_TO_SURFACE_PAINT_MAPPER_H__

