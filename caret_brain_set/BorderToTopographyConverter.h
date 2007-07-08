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

#ifndef __BORDER_TO_TOPOGRAPHY_CONVERTER_H__
#define __BORDER_TO_TOPOGRAPHY_CONVERTER_H__

#include "BrainModelAlgorithm.h"

class Border;
class BorderFile;
class BrainModelSurface;
class PaintFile;
class QRegExp;
class TopographyFile;

/// class to convert borders into topography
class BorderToTopographyConverter : public BrainModelAlgorithm {
   public:
      /// constructor
      BorderToTopographyConverter(BrainSet* bs,
                                  BrainModelSurface* surfaceIn,
                                  BorderFile* borderFileIn,
                                  PaintFile* paintFileIn,
                                  TopographyFile* topographyFileIn,
                                  const int topographyFileColumnIn,
                                  const QString topographyFileColumnNameIn);
      /// destructor
      ~BorderToTopographyConverter();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   private:
      enum TOPOGRAPHY_TYPES { TYPE_EMEAN = 0,
                              TYPE_ELOW  = 1,
                              TYPE_EHIGH = 2,
                              TYPE_PMEAN = 3,
                              TYPE_PLOW  = 4,
                              TYPE_PHIGH = 5,
                              TYPE_NONE  = 6 };

      void getBorderAreaAndType(
                     const QString borderName, QRegExp& regExpr,
                     QString& areaNameOut, TOPOGRAPHY_TYPES& topographyTypeOut,
                     int& areaNumberOut);
                     
      void getNodeAreaTypeAndBorders(
                      const QString& nodeName, QRegExp& regExpr,
                      QString& areaNameOut, TOPOGRAPHY_TYPES& topographyTypeOut,
                      int& areaNumber1Out, int& areaNumber2Out);
                       
      /// Interpolate the topography values between two points.
      float interpolateBorderTopoValues(
                            const float nearDist, const float farDist,
                            const float nearValue, const float farValue) const;

      /// Find the distance to the link that is closest to the nodes' position
      float getClosestBorderPointDistance(const Border* b, 
                                          const float nodeXYZ[3]) const;

      /// the surface
      BrainModelSurface* surface;
      
      /// the border file
      BorderFile* borderFile;
      
      /// the paint file
      PaintFile* paintFile;
      
      /// the topography file
      TopographyFile* topographyFile;
      
      /// the topography file column
      int topographyFileColumn;
      
      /// the topography file column name
      QString topographyFileColumnName;
};

#endif // __BORDER_TO_TOPOGRAPHY_CONVERTER_H__
