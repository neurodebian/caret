
#ifndef __BRAIN_MODEL_SURFACE_ROI_FOLDING_MEASUREMENT_REPORT_H__
#define __BRAIN_MODEL_SURFACE_ROI_FOLDING_MEASUREMENT_REPORT_H__

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

#include "BrainModelSurfaceROIOperation.h"

class BrainModelSurface;

/// class for measuring folding on the surface
class BrainModelSurfaceROIFoldingMeasurementReport : public BrainModelSurfaceROIOperation {
   public:
      // constructor
      BrainModelSurfaceROIFoldingMeasurementReport(BrainSet* bs,
                                           const BrainModelSurface* surfaceIn,
                                           const BrainModelSurfaceROINodeSelection* roiIn,
                                           const QString& headerTextIn,
                                           const bool semicolonSeparateReportFlagIn,
                                           const BrainModelSurface* hullSurfaceIn = NULL);
      
      // destructor
      ~BrainModelSurfaceROIFoldingMeasurementReport();
      
      // execute the operation
      void executeOperation() throw (BrainModelAlgorithmException);
      
   protected:
      /// curvatures measures on each node
      class NodeCurvatureMeasure {
         public:
            // constructor
            NodeCurvatureMeasure();
            
            // destructor
            ~NodeCurvatureMeasure();

            // set the measurements
            void setMeasurements(const float k1In, const float k2In);
            
            /// primary principal curvature
            float k1;

            /// secondary principal curvature
            float k2;

            /// mean curvature
            float H;

            /// gaussian curvature
            float K;

            /// positive mean curvature
            float Hplus;

            /// negative mean curvature
            float Hminus;

            /// positive gaussian curvature
            float Kplus;

            /// negative gaussian curvature 
            float Kminus;

            /// surface area measure of positive mean curvature 
            float HplusAreaMeasure;

            /// surface area measure of negative mean curvature
            float HminusAreaMeasure;

            /// surface area measure of positive gaussian curvature
            float KplusAreaMeasure;

            /// surface area measure of negative gaussian curvature 
            float KminusAreaMeasure;

            /// curvedness index
            float ci;

            /// shape index 
            float si;

            /// folding index
            float fi;
      };
      
      /// Folding Measurements 
      class FoldingMeasurements {
         public:
            // constructor
            FoldingMeasurements();
            
            // destructor
            ~FoldingMeasurements();
            
            // add folding measurements for a tile
            void addTileFolding(const BrainModelSurfaceROINodeSelection* roi,
                                const float tileFullArea,
                                const int tileNodes[3],
                                const std::vector<NodeCurvatureMeasure>& nodeCurvatures);
            
            // finalize the measurements
            void finalizeMeasurements(const float roiSurfaceArea);
            
            // Intrinsic Curvature Index 
            float ICI;
                                
            // Negative Intrinsic Curvature Index
            float NICI;
                                
            // Gaussian L2 Norm
            float GLN;
                                
            // Absolute ICI 
            float AICI;
                                
            // Mean Curvature Index
            float MCI;
                                
            // Negative Mean Curavture Index
            float NMCI;
                                
            // Mean L2 Norm
            float MLN;
                                
            // Absolute Mean Curvature Index 
            float AMCI;
                                
            // Folding Index
            float FI;
                                
            // Curvedness Index
            float CI;
                                
            // Shape Index
            float SI;
                                
            // Area Fraction of Intrinsic Curvature Index
            float FICI;
                                
            // Area Fraction of Negative Intrinsic Curvature Index
            float FNICI;
                                
            // Area Fraction of Mean Curvature Index
            float FMCI;
                                
            // Area Fraction of Negative Mean Curvature Index
            float FNMCI;
                                
            // SH2SH
            float SH2SH;
                                
            // SK2SK
            float SK2SK;
            
            // total area of the ROI
            float roiTotalArea;
      };
      
      // compute the node curvature measurements
      void computeNodeCurvatureMeasurements(std::vector<NodeCurvatureMeasure>& cm) 
         throw (BrainModelAlgorithmException);
         
      /// the cerebral hull surface
      const BrainModelSurface* hullSurface;       
      
      /// semicolon separate the report
      bool semicolonSeparateReportFlag;
};

#endif // __BRAIN_MODEL_SURFACE_ROI_FOLDING_MEASUREMENT_REPORT_H__

