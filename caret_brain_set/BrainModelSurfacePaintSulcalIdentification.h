
#ifndef __BRAIN_MODEL_SURFACE_PAINT_SULCAL_IDENTIFICATION_H__
#define __BRAIN_MODEL_SURFACE_PAINT_SULCAL_IDENTIFICATION_H__

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

class AreaColorFile;
class BrainModelSurface;
class MetricFile;
class PaintFile;
class SurfaceShapeFile;
class VocabularyFile;
class VolumeFile;

/// generate paint sulcal identication 
class BrainModelSurfacePaintSulcalIdentification : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfacePaintSulcalIdentification(BrainSet* bs,
                                         const BrainModelSurface* fiducialSurfaceIn,
                                         const BrainModelSurface* inflatedSurfaceIn,
                                         const BrainModelSurface* veryInflatedSurfaceIn,
                                         PaintFile* paintFileInOut,
                                         const int paintFileGeographyColumnNumberIn,
                                         AreaColorFile* areaColorFileInOut,
                                         const SurfaceShapeFile* depthSurfaceShapeFileIn,
                                         const int depthSurfaceShapeFileColumnNumberIn,
                                         VocabularyFile* vocabularyFileOut);
                                         
      // destructor
      ~BrainModelSurfacePaintSulcalIdentification();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      /// get probabilistic metric file of sulci locations
      const MetricFile* getMetricFile() const { return metricFile; }
      
      /// get paint file containing the sulcal identification
      const PaintFile* getPaintFile() const { return paintFile; }
      
      /// get area color file containing paint sulcal identification
      const AreaColorFile* getAreaColorFile() const { return areaColorFile; }
      
      // get the name of the Sulcus ID paint column
      static QString getSulcusIdPaintColumnName();
            
   protected:
      /// fiducila surface
      const BrainModelSurface* fiducialSurface;
      
      /// inflated surface
      const BrainModelSurface* inflatedSurface;
      
      /// very inflated surface
      const BrainModelSurface* veryInflatedSurface;
      
      /// input paint file containing geography paint column
      PaintFile* paintFile;
      
      /// column number of geography in paint file
      const int paintFileGeographyColumnNumber;
      
      /// area color file
      AreaColorFile* areaColorFile;
      
      /// surface shape file containing depth information
      const SurfaceShapeFile* depthSurfaceShapeFile;
      
      /// surface shape file depth column number
      const int depthSurfaceShapeFileColumnNumber;      
      
      /// vocabulary file
      VocabularyFile* vocabularyFile;
      
      /// probabilistic metric file
      MetricFile* metricFile;
      
      /// left hemisphere flag
      bool leftHemisphereFlag;
};

#endif // __BRAIN_MODEL_SURFACE_PAINT_SULCAL_IDENTIFICATION_H__

