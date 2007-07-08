
#ifndef __BRAIN_MODEL_SURFACE_REGION_OF_INTEREST_H__
#define __BRAIN_MODEL_SURFACE_REGION_OF_INTEREST_H__

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

#include <vector>

#include "BorderFile.h"
#include "BrainModelAlgorithm.h"

class BrainModelSurface;

class CoordinateFile;
class LatLonFile;
class MetricFile;
class PaintFile;
class SurfaceShapeFile;
class TopologyFile;

/// class for performing region of interest operations on a surface
class BrainModelSurfaceRegionOfInterest : public BrainModelAlgorithm {
   public:
      /// operation
      enum OPERATION {
         /// operation create border
         OPERATION_CREATE_BORDER,
         /// surface XYZ means report
         OPERATION_SURFACE_XYZ_MEANS_REPORT,
         /// text report
         OPERATION_TEXT_REPORT
      };
      
      // constructor
      BrainModelSurfaceRegionOfInterest(BrainSet* bs,
                                        BrainModelSurface* bmsIn,
                                        const OPERATION operationIn,
                                        const std::vector<bool> selectedNodeFlagsIn);
      
      // destructor
      ~BrainModelSurfaceRegionOfInterest();
      
      // execute the operation
      void execute() throw (BrainModelAlgorithmException);
      
      // get the report text
      QString getReportText() const { return reportText; }
      
      // set create border options
      void setCreateBorderControlsAndOptions(const QString& borderNameIn,
                                             const int startNodeIn,
                                             const int endNodeIn,
                                             const float samplingDensityIn);
                                             
      // get the border that was created by create border mode
      Border getBorder() const;
      
      // set surface means report controls and options
      void setSurfaceMeansReportControlsAndOptions(std::vector<CoordinateFile*>& coordFilesIn);
                                                    
      // set report controls and options
      void setTextReportControlsAndOptions(MetricFile* metricFileIn,
                                           const std::vector<bool>& selectedMetricColumnsForReportIn,
                                           MetricFile* shapeFileIn,
                                           const std::vector<bool>& selectedShapeColumnsForReportIn,
                                           PaintFile* paintFileIn,
                                           const std::vector<bool>& selectedPaintColumnsForReportIn,
                                           LatLonFile* latLonFileIn,
                                           const int latLonFileColumnIn,
                                           const QString& nodeSelectionTextIn,
                                           const QString& headerTextIn,
                                           MetricFile* metricCorrectionFileIn,
                                           const int metricCorrectionColumnIn,
                                           const bool tabSeparateReportFlagIn);

   protected:
      // create the text report
      void createTextReport() throw (BrainModelAlgorithmException);
      
      // create the surface means text report
      void createSurfaceMeansTextReport() throw (BrainModelAlgorithmException);
      
      // Create the report header
      void createReportHeader(float& roiAreaOut);
       
      // create the border
      void createBorder(const int numNodesInROI) throw (BrainModelAlgorithmException);
      
      // Peform Metric and Surface Shape Report.
      void metricAndSurfaceShapeReport(const bool metricFlag);

      // perform paint report
      void paintReport(const float roiArea);
      
      // set node selection information
      void setNodeSelectionInformation(const QString& nodeSelectionTextIn);
      
      /// create the report header
      /// surface on which to perform operation
      BrainModelSurface* bms;
      
      /// the operation
      OPERATION operation;
      
      /// the selected nodes for the operation
      std::vector<bool> selectedNodeFlags;
      
      /// selected metric columns
      std::vector<bool> selectedMetricColumnsForReport;
      
      /// selected surface shape columns
      std::vector<bool> selectedShapeColumnsForReport;
      
      /// selected paint columns
      std::vector<bool> selectedPaintColumnsForReport;
      
      /// lat lon file for report
      LatLonFile* reportLatLonFile;
      
      /// column for report lat lon file
      int reportLatLonFileColumn;
      
      /// metric file for report
      MetricFile* reportMetricFile;
      
      /// shape file for report
      MetricFile* reportShapeFile;
      
      /// paint file for report
      PaintFile* reportPaintFile;
      
      /// metric correction file for report
      MetricFile* reportMetricCorrectionFile;
      
      /// information about node selection
      QString nodeSelectionText;
      
      /// the header text
      QString headerText;
      
      /// tab separate report flag
      bool tabSeparateReportFlag;
      
      /// report text 
      QString reportText;
      
      /// area of each tile
      std::vector<float> tileArea;
      
      /// tile in ROI flags
      std::vector<bool> tileInROI;
      
      /// the metric correction column
      int metricCorrectionColumn;
      
      /// coord files for surface means 
      std::vector<CoordinateFile*> surfaceMeansCoordFiles;
      
      /// name for create border
      QString createBorderName;
      
      /// create border option border
      Border createBorderModeBorder;

      /// create border start node
      int createBorderStartNode;
      
      /// create border end node
      int createBorderEndNode;
      
      /// create border sampling density
      float createBorderSamplingDensity;
      
      /// the separator character
      static const QString separatorCharacter;
};

#ifdef __BRAIN_MODEL_SURFACE_REGION_MAIN__
   const QString BrainModelSurfaceRegionOfInterest::separatorCharacter = ";";
#endif // __BRAIN_MODEL_SURFACE_REGION_MAIN__

#endif // __BRAIN_MODEL_SURFACE_REGION_OF_INTEREST_H__

