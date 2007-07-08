
#ifndef __GUI_MAP_FMRI_METRIC_INFO_H__
#define __GUI_MAP_FMRI_METRIC_INFO_H__

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

class GuiMapFmriMappingSet;
class GuiMapFmriVolume;

/// This class is used to store metric naming information for a single volume
/// that may be mapped to multiple surfaces.
class GuiMapFmriMetricInfo {
   public:
      /// Constructor
      GuiMapFmriMetricInfo(const GuiMapFmriMappingSet* surfaceInfo,
                           const GuiMapFmriVolume* volumeInfo,
                           const int volumeIndexIn,
                           const int subVolumeIndexIn);
      
      /// Destructor
      ~GuiMapFmriMetricInfo();
      
      /// get the number of metric columns
      int getNumberOfMetricColumns() const { return metricColumnNames.size(); }
      
      /// get the surface name for a metric column
      QString getSurfaceNameForMetricColumn(const int indx) const
                                           { return surfaceNameForMetricColumn[indx]; }
           
      /// get the name of a metric column
      QString getMetricColumnName(const int indx) const { return metricColumnNames[indx]; }
      
      /// set the name of a metric column
      void setMetricColumnName(const int indx, const QString& name)
                                          { metricColumnNames[indx] = name; }
      
      /// get the row number in the metric naming table for a metric column name
      int getMetricColumnNameRowNumber(const int indx) const 
                                     { return metricColumnNamesRowNumber[indx]; }
                                     
      /// set the row number in the metric naming table for a metric column name
      void setMetricColumnNameRowNumber(const int indx, const int rowNumber) 
                                     { metricColumnNamesRowNumber[indx] = rowNumber; }
                                     
      /// get a comment for a metric column
      QString getMetricColumnComment(const int indx) const
                                { return metricColumnComments[indx]; }
      
      /// set a comment for a metric column
      void setMetricColumnComment(const int indx, const QString& comment)
                                { metricColumnComments[indx] = comment; }
                                
      /// get the metric column thresholds
      void getMetricColumnThresholds(const int indx, float& negThresh, float& posThresh) const;
      
      /// set the metric column thresholds
      void setMetricColumnThresholds(const int indx, const float negThresh, const float posThresh);
      
      /// get the average fiducial coord file metric column name
      QString getMetricAverageFiducialCoordColumnName() const { return metricAverageFiducialCoordColumnName; }
      
      /// set the average fiducial coord file metric column name
      void setMetricAverageFiducialCoordColumnName(const QString& name)
                                             { metricAverageFiducialCoordColumnName = name; }
                                             
      /// get the average fiducial coord metric column name row number for metric naming table
      int getMetricAverageFiducialCoordNameRowNumber() const
                                { return  metricAverageFiducialCoordColumnNameRowNumber; }
                                
      /// set the average fiducial coord metric column name row number for metric naming table
      void setMetricAverageFiducialCoordNameRowNumber(const int rowNumber) 
                                { metricAverageFiducialCoordColumnNameRowNumber = rowNumber; }
                                
      /// get the metric average fiducial coord comment
      QString getMetricAverageFiducialCoordComment() const { return metricAverageFiducialCoordComment; }
      
      /// set the metric average fiducial coord comment
      void setMetricAverageFiducialCoordComment(const QString& comment)
                                 { metricAverageFiducialCoordComment = comment; }
                                 
      /// get the metric average fiducial coord thresholds
      void getMetricAverageFiducialCoordThresholds(float& negThresh, float& posThresh) const;
      
      /// set the metric average fiducial coord thresholds
      void setMetricAverageFiducialCoordThresholds(const float negThresh, const float posThresh);
      
      /// get the average of all metric column name
      QString getMetricAverageOfAllColumnName() const { return metricAverageOfAllColumnName; }
      
      /// set the average of all metric column name
      void setMetricAverageOfAllColumnName(const QString& name)
                                            { metricAverageOfAllColumnName = name; }
                                            
      /// get the average of all metric column name row number for metric naming table
      int getMetricAverageOfAllColumnNameRowNumber() const 
                                { return metricAverageOfAllColumnNameRowNumber; }
      
      /// set the average of all metric column name row number for metric naming table
      void setMetricAverageOfAllColumnNameRowNumber(const int rowNumber)
                                            { metricAverageOfAllColumnNameRowNumber = rowNumber; }
                                            
      /// get the  metric average of all comment
      QString getMetricAverageOfAllComment() const { return metricAverageOfAllComment; }
      
      /// set the metric average of all comment
      void setMetricAverageOfAllComment(const QString& comment)
                                 { metricAverageOfAllComment = comment; }
                                 
      /// get the metric average of all thresholds
      void getMetricAverageOfAllThresholds(float& negThresh, float& posThresh) const;
      
      /// set the metric average of all thresholds
      void setMetricAverageOfAllThresholds(const float negThresh, const float posThresh);
      
      /// get the std dev metric column name
      QString getMetricStdDevColumnName() const { return metricStdDevColumnName; }
      
      /// set the std dev metric column name
      void setMetricStdDevColumnName(const QString& name)
                                            { metricStdDevColumnName = name; }
                                            
      /// get the std dev metric column name row number for metric naming table
      int getMetricStdDevColumnNameRowNumber() const 
                                { return metricStdDevColumnNameRowNumber; }
      
      /// set the std dev metric column name row number for metric naming table
      void setMetricStdDevColumnNameRowNumber(const int rowNumber)
                                            { metricStdDevColumnNameRowNumber = rowNumber; }
                                            
      /// get the  metric std dev comment
      QString getMetricStdDevComment() const { return metricStdDevComment; }
      
      /// set the metric std dev comment
      void setMetricStdDevComment(const QString& comment)
                                 { metricStdDevComment = comment; }
                                 
      /// get the metric std dev thresholds
      void getMetricStdDevThresholds(float& negThresh, float& posThresh) const;
      
      /// set the metric std dev thresholds
      void setMetricStdDevThresholds(const float negThresh, const float posThresh);
      
      /// get the std error metric column name
      QString getMetricStdErrorColumnName() const { return metricStdErrorColumnName; }
      
      /// set the std error metric column name
      void setMetricStdErrorColumnName(const QString& name)
                                            { metricStdErrorColumnName = name; }
       
      /// get the std error metric column name row number for metric naming table
      int getMetricStdErrorColumnNameRowNumber() const 
                                { return metricStdErrorColumnNameRowNumber; }
      
      /// set the std error metric column name row number for metric naming table
      void setMetricStdErrorColumnNameRowNumber(const int rowNumber)
                                            { metricStdErrorColumnNameRowNumber = rowNumber; }

      /// get the  metric std error comment
      QString getMetricStdErrorComment() const { return metricStdErrorComment; }
      
      /// set the metric std error comment
      void setMetricStdErrorComment(const QString& comment)
                                 { metricStdErrorComment = comment; }
      
      /// get the metric std error thresholds
      void getMetricStdErrorThresholds(float& negThresh, float& posThresh) const;
      
      /// set the metric std error thresholds
      void setMetricStdErrorThresholds(const float negThresh, const float posThresh);
      
      /// get the minimum value metric column name
      QString getMetricMinValueColumnName() const { return metricMinValueColumnName; }
      
      /// set the minimum value metric column name
      void setMetricMinValueColumnName(const QString& name)
                                            { metricMinValueColumnName = name; }
       
      /// get the minimum value metric column name row number for metric naming table
      int getMetricMinValueColumnNameRowNumber() const 
                                { return metricMinValueColumnNameRowNumber; }
      
      /// set the minimum value metric column name row number for metric naming table
      void setMetricMinValueColumnNameRowNumber(const int rowNumber)
                                            { metricMinValueColumnNameRowNumber = rowNumber; }

      /// get the  metric minimum value comment
      QString getMetricMinValueComment() const { return metricMinValueComment; }
      
      /// set the metric minimum value comment
      void setMetricMinValueComment(const QString& comment)
                                 { metricMinValueComment = comment; }
      
      /// get the metric minimum value thresholds
      void getMetricMinValueThresholds(float& negThresh, float& posThresh) const;
      
      /// set the metric minimum value thresholds
      void setMetricMinValueThresholds(const float negThresh, const float posThresh);
      
      /// get the maximum value metric column name
      QString getMetricMaxValueColumnName() const { return metricMaxValueColumnName; }
      
      /// set the maximum value metric column name
      void setMetricMaxValueColumnName(const QString& name)
                                            { metricMaxValueColumnName = name; }
       
      /// get the maximum value metric column name row number for metric naming table
      int getMetricMaxValueColumnNameRowNumber() const 
                                { return metricMaxValueColumnNameRowNumber; }
      
      /// set the maximum value metric column name row number for metric naming table
      void setMetricMaxValueColumnNameRowNumber(const int rowNumber)
                                            { metricMaxValueColumnNameRowNumber = rowNumber; }

      /// get the  metric maximum value comment
      QString getMetricMaxValueComment() const { return metricMaxValueComment; }
      
      /// set the metric maximum value comment
      void setMetricMaxValueComment(const QString& comment)
                                 { metricMaxValueComment = comment; }
      
      /// get the metric maximum value thresholds
      void getMetricMaxValueThresholds(float& negThresh, float& posThresh) const;
      
      /// set the metric maximum value thresholds
      void setMetricMaxValueThresholds(const float negThresh, const float posThresh);
      
      /// get the volume's index
      int getVolumeIndex() const { return volumeIndex; }
      
      /// get the sub-volume's index
      int getSubVolumeIndex() const { return subVolumeIndex; }
      
   protected:
      /// name of metric columns (one per mapping surface)
      std::vector<QString> metricColumnNames;
      
      /// negative threshold (one per mapping surface)
      std::vector<float> metricColumnNegativeThreshold;
      
      /// positive threshold (one per mapping surface)
      std::vector<float> metricColumnPositiveThreshold;
      
      /// row number of metric column name in naming table
      std::vector<int> metricColumnNamesRowNumber;
      
      /// name of surface for the corresponding metric column
      std::vector<QString> surfaceNameForMetricColumn;
      
      /// comments for metric columns
      std::vector<QString> metricColumnComments;
      
      /// name of average fiducial coord metric column name
      QString metricAverageFiducialCoordColumnName;
      
      /// row number of average fiducial coord metric column in naming table
      int metricAverageFiducialCoordColumnNameRowNumber;
      
      /// comment for averge fiducialcoord metric
      QString metricAverageFiducialCoordComment;
      
      /// negative threshold for average fiducial coord
      float metricAverageFiducialCoordColumnNegativeThreshold;
      
      /// positive threshold for average fiducial coord
      float metricAverageFiducialCoordColumnPositiveThreshold;
      
      /// name of average of all metric column name
      QString metricAverageOfAllColumnName;
      
      /// row number of average of all metric column in naming table
      int metricAverageOfAllColumnNameRowNumber;
      
      /// comment for average of all metric
      QString metricAverageOfAllComment;
      
      /// negative threshold for average of all threshold
      float metricAverageOfAllColumnNegativeThreshold;
      
      /// positive threshold for average of all threshold
      float metricAverageOfAllColumnPositiveThreshold;
      
      /// name of std dev metric column name
      QString metricStdDevColumnName;
      
      /// row number of std dev metric column in naming table
      int metricStdDevColumnNameRowNumber;
      
      /// comment for std dev metric 
      QString metricStdDevComment;
      
      /// negative threshold for std dev threshold
      float metricStdDevColumnNegativeThreshold;
      
      /// positive threshold for std dev threshold
      float metricStdDevColumnPositiveThreshold;
      
      /// name of std error metric column name
      QString metricStdErrorColumnName;
      
      /// row number of std error metric column in naming table
      int metricStdErrorColumnNameRowNumber;
      
      /// comment for std error metric 
      QString metricStdErrorComment;
      
      /// negative threshold for std error threshold
      float metricStdErrorColumnNegativeThreshold;
      
      /// positive threshold for std error threshold
      float metricStdErrorColumnPositiveThreshold;
      
      /// name of minimum value metric column name
      QString metricMinValueColumnName;
      
      /// row number of minimum value metric column in naming table
      int metricMinValueColumnNameRowNumber;
      
      /// comment for minimum value metric 
      QString metricMinValueComment;
      
      /// negative threshold for minimum value threshold
      float metricMinValueColumnNegativeThreshold;
      
      /// positive threshold for minimum value threshold
      float metricMinValueColumnPositiveThreshold;
      
      /// name of maximum value metric column name
      QString metricMaxValueColumnName;
      
      /// row number of maximum value metric column in naming table
      int metricMaxValueColumnNameRowNumber;
      
      /// comment for maximum value metric 
      QString metricMaxValueComment;
      
      /// negative threshold for maximum value threshold
      float metricMaxValueColumnNegativeThreshold;
      
      /// positive threshold for maximum value threshold
      float metricMaxValueColumnPositiveThreshold;
      
      /// index into volumes
      int volumeIndex;
      
      /// sub volume index
      int subVolumeIndex;
};

#endif // __GUI_MAP_FMRI_METRIC_INFO_H__

