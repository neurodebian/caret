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



#ifndef __VE_METRICS_TO_RGB_PAINT_CONVERTER_H__
#define __VE_METRICS_TO_RGB_PAINT_CONVERTER_H__

#include <QString>

class MetricFile;
class RgbPaintFile;

/// Class that converts metrics to RGB Paint
class MetricsToRgbPaintConverter {
   public:
      /// Constructor
      MetricsToRgbPaintConverter();
      
      /// Destructor
      ~MetricsToRgbPaintConverter();
      
      /// set rgb paint file info
      void setRgbPaintInfo(const int column, const QString& columnName,
                           const QString& title);
      
      /// set red metric info
      void setRedMetric(const int column, const float negMax, const float posMax);

      /// set green metric info
      void setGreenMetric(const int column, const float negMax, const float posMax);

      /// set blue metric info
      void setBlueMetric(const int column, const float negMax, const float posMax);

      /// set red threshold info
      void setRedThresh(const int column, const float negMax, const float posMax);

      /// set green threshold info
      void setGreenThresh(const int column, const float negMax, const float posMax);

      /// set blue threshold info
      void setBlueThresh(const int column, const float negMax, const float posMax);

      /// convert metric to rgb paint (returns true if an error occurs)
      bool convert(const MetricFile* mf, RgbPaintFile* rgbPaintFile, QString& errorMessage);
      
   private:
      /// rgb paint column
      int rgbPaintColumn;
      
      /// rgb paint column name
      QString rgbPaintColumnName;
      
      /// rgb paint title
      QString rgbTitle;
      
      /// red metric column
      int redMetricColumn;
      
      /// green metric column
      int greenMetricColumn;
      
      /// blue metric column
      int blueMetricColumn;
      
      /// red metric negative max
      float redMetricNegMax;
            
      /// red metric positive max
      float redMetricPosMax;
            
      /// green metric negative max
      float greenMetricNegMax;
            
      /// green metric positive max
      float greenMetricPosMax;
            
      /// blue metric negative max
      float blueMetricNegMax;
            
      /// blue metric positive max
      float blueMetricPosMax;
            
      /// red threshold column
      int redThresholdColumn;
      
      /// green threshold column
      int greenThresholdColumn;
      
      /// blue threshold column
      int blueThresholdColumn;
      
      /// red threshold negative 
      float redThresholdNeg;
            
      /// red threshold positive 
      float redThresholdPos;
            
      /// green threshold negative 
      float greenThresholdNeg;
            
      /// green threshold positive 
      float greenThresholdPos;
            
      /// blue threshold negative 
      float blueThresholdNeg;
            
      /// blue threshold positive 
      float blueThresholdPos;
            
};

#endif  // __VE_METRICS_TO_RGB_PAINT_CONVERTER_H__

