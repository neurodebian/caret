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


#ifndef __VE_BRAIN_SURFACE_DISTORTION_H__
#define __VE_BRAIN_SURFACE_DISTORTION_H__

#include <QString>

#include "BrainModelAlgorithm.h"
#include "StatisticsUtilities.h"

class BrainModelSurface;
class SurfaceShapeFile;
class TopologyFile;

/// Class to compute distortion between two surfaces
class BrainModelSurfaceDistortion : public BrainModelAlgorithm {
   public:
      enum DISTORTION_COLUMNS {
         DISTORTION_COLUMN_CREATE_NEW = -1,
         DISTORTION_COLUMN_DO_NOT_GENERATE = -2
      };

      /// Constructor
      BrainModelSurfaceDistortion(BrainSet* brainSetIn,
                             BrainModelSurface* surfaceIn,
                             BrainModelSurface* referenceSurfaceIn,
                             TopologyFile* topologyFileIn,
                             SurfaceShapeFile* surfaceShapeFileIn,
                             const int arealDistortionColumnIn,
                             const int linearDistortionColumnIn,
                             const QString& arealDistortionNameIn,
                             const QString& linearDistortionNameIn);
                             
      /// Destructor
      ~BrainModelSurfaceDistortion();
      
      /// add/update the distortions in the surface shape file
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// Get statistical measurements for areal distortion
      void getArealDistortionStatistics(StatisticsUtilities::DescriptiveStatistics& stats) const;
      
      /// Get statistical measurements for linear distortion
      void getLinearDistortionStatistics(StatisticsUtilities::DescriptiveStatistics& stats) const;
      
   private:
      /// surface
      BrainModelSurface* surface;
      
      /// reference surface
      BrainModelSurface* referenceSurface;
      
      /// topology file
      TopologyFile* topologyFile;
      
      /// surface shape file
      SurfaceShapeFile* surfaceShapeFile;
      
      /// name for areal distorition
      QString arealDistortionName;
      
      /// name for linear distortion
      QString linearDistortionName;
      
      /// linear distortion saved for statistical calculation
      std::vector<float> linearDistortionForStatistics;
      
      /// areal distortion saved for statistical calculation
      std::vector<float> arealDistortionForStatistics;
      
      /// areal distortion column number
      int arealDistortionColumn;
      
      /// linear distortion column number
      int linearDistortionColumn;
};

#endif  // __VE_BRAIN_SURFACE_DISTORTION_H__

