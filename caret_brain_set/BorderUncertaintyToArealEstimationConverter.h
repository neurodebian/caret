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

#ifndef __BORDER_UNCERTAINTY_TO_AREAL_ESTIMATION_CONVERTER_H__
#define __BORDER_UNCERTAINTY_TO_AREAL_ESTIMATION_CONVERTER_H__

#include <set>

#include "BrainModelAlgorithm.h"

class ArealEstimationFile;
class BorderFile;
class BrainModelSurface;
class PaintFile;

/// Class that creates an areal estimation file from border uncertainty
class BorderUncertaintyToArealEstimationConverter : public BrainModelAlgorithm {
   public:
      enum MODE {
         MODE_ALL_NODES,
         MODE_NODES_WITH_PAINT
      };
      
      /// Constructor
      BorderUncertaintyToArealEstimationConverter(
                                       BrainSet* bs,
                                       BrainModelSurface* surfaceIn,
                                       ArealEstimationFile* arealEstimationFileIn,
                                       BorderFile* borderFileIn,
                                       PaintFile* paintFileIn,
                                       const MODE modeIn,
                                       const int arealEstimationFileColumnIn,
                                       const QString& arealEstimationFileColumnNameIn,
                                       const QString& longNameIn,
                                       const QString& commentIn,
                                       const int paintColumnIn,
                                       const QString paintMatchNameIn,
                                       const bool overrideBorderUncertaintyIn,
                                       const float overrideBorderUncertaintyValueIn);
      
      /// Destructor
      ~BorderUncertaintyToArealEstimationConverter();
      
      /// execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   private:
      /// class to help with finding the nearest borders
      class BorderDist {
         public:
            int borderNumber;
            int borderNearestLink;
            float distanceToBorder;
            
            BorderDist(const int borderNumberIn,
                     const int borderNearestLinkIn,
                     const float distanceToBorderIn) {
               borderNumber = borderNumberIn;
               borderNearestLink = borderNearestLinkIn;
               distanceToBorder = distanceToBorderIn;
            }
            bool operator<(const BorderDist& b) const {
               if (distanceToBorder < b.distanceToBorder) {
                  return true;
               }
               if (distanceToBorder > b.distanceToBorder) {
                  return false;
               }
               if (borderNumber < b.borderNumber) {
                  return true;
               }
               return false;
            }
      };
      
      /// Class for sorting 4 borders with normalized probabilities
      class ProbabilitySort {
         public:
            QString name;
            float probability;
            ProbabilitySort(const QString& n, const float p) {
               name = n;
               probability = p;
            }
            bool operator<(const ProbabilitySort& p) const {   
               if (probability > p.probability) {
                  return true;
               }
               if (probability < p.probability) {
                  return false;
               }
               if (name.compare(p.name) < 0) {
                  return true;
               }
               return false;
            }
      };
      
      float determineSignedDistance(const float* pos, const BorderDist& bd);
           
      void findClosestBorderLinks2D(const float* pos, std::set<BorderDist>& closestBorders);
      
                                                   
      /// the areal estimation file
      ArealEstimationFile* arealEstimationFile;
      
      /// column in the areal estimation file
      int arealEstimationFileColumn;
      
      /// name of areal estimation file column
      QString arealEstimationFileColumnName;
      
      /// the paint file
      PaintFile* paintFile;
      
      /// the paint column
      int paintColumn;
      
      /// the border file
      BorderFile* borderFile;
      
      /// paint name to match
      QString paintMatchName;
      
      /// long name for column
      QString longName;
      
      /// comment for column
      QString comment;
      
      /// the surface used 
      BrainModelSurface* surface;
      
      /// mode 
      MODE mode;
      
      /// override border file uncertainties
      bool overrideBorderUncertainty;
      
      /// border override uncertainty value
      float overrideBorderUncertaintyValue;
};

#endif // __BORDER_UNCERTAINTY_TO_AREAL_ESTIMATION_CONVERTER_H__

