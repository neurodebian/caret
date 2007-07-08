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



#include "MetricFile.h"
#include "MetricsToRgbPaintConverter.h"
#include "RgbPaintFile.h"

/**
 * Constructor
 */
MetricsToRgbPaintConverter::MetricsToRgbPaintConverter()
{
}

/**
 * Destructor
 */
MetricsToRgbPaintConverter::~MetricsToRgbPaintConverter()
{
}

/**
 * set rgb paint file info.
 */
void
MetricsToRgbPaintConverter::setRgbPaintInfo(const int column, const QString& columnName,
                                            const QString& title)
{
   rgbPaintColumn = column;
   rgbPaintColumnName = columnName;
   rgbTitle = title;
}

/**
 * set red metric info.
 */
void
MetricsToRgbPaintConverter::setRedMetric(const int column, const float negMax, const float posMax)
{
   redMetricColumn = column;
   redMetricNegMax = negMax;
   redMetricPosMax = posMax;
}

/**
 * set green metric info.
 */
void
MetricsToRgbPaintConverter::setGreenMetric(const int column, const float negMax, const float posMax)
{
   greenMetricColumn = column;
   greenMetricNegMax = negMax;
   greenMetricPosMax = posMax;
}

/**
 * set blue metric info.
 */
void
MetricsToRgbPaintConverter::setBlueMetric(const int column, const float negMax, const float posMax)
{
   blueMetricColumn = column;
   blueMetricNegMax = negMax;
   blueMetricPosMax = posMax;
}

/**
 * set red threshold info.
 */
void
MetricsToRgbPaintConverter::setRedThresh(const int column, const float neg, const float pos)
{
   redThresholdColumn = column;
   redThresholdNeg = neg;
   redThresholdPos = pos;
}

/**
 * set green threshold info.
 */
void
MetricsToRgbPaintConverter::setGreenThresh(const int column, const float neg, const float pos)
{
   greenThresholdColumn = column;
   greenThresholdNeg = neg;
   greenThresholdPos = pos;
}

/**
 * set blue threshold info.
 */
void
MetricsToRgbPaintConverter::setBlueThresh(const int column, const float neg, const float pos)
{
   blueThresholdColumn = column;
   blueThresholdNeg = neg;
   blueThresholdPos = pos;
}

/**
 * convert the metric to rgb paint (returns true if an error occurs).
 */
bool
MetricsToRgbPaintConverter::convert(const MetricFile* metricFile, RgbPaintFile* rgbPaintFile, 
                                    QString& errorMessage)
{
   bool errorFlag = false;
   errorMessage = "";
   
   if ((redMetricColumn < 0) &&
       (greenMetricColumn < 0) &&
       (blueMetricColumn < 0)) {
      errorMessage = "You must select at least one metric column.";
      return true;
   }
   
   if (redMetricColumn >= 0) {
      if (redMetricNegMax > 0.0) {
         errorMessage.append("Red Neg Max must be less than or equal to zero.\n");
         errorFlag = true;
      }
      if (redMetricPosMax < 0.0) {
         errorMessage.append("Red Pos Max must be greater than or equal to zero.\n");
         errorFlag = true;
      }
      if (redThresholdColumn >= 0) {
         if (redThresholdNeg > 0.0) {
            errorMessage.append("Red Neg Threshold must be less than or equal to zero.\n");
            errorFlag = true;
         }
         if (redThresholdPos < 0.0) {
            errorMessage.append("Red Pos Threshold must be greater than or equal to zero.\n");
            errorFlag = true;
         }
      }
   }
   
   if (greenMetricColumn >= 0) {
      if (greenMetricNegMax > 0.0) {
         errorMessage.append("Green Neg Max must be less than or equal to zero.\n");
         errorFlag = true;
      }
      if (greenMetricPosMax < 0.0) {
         errorMessage.append("Green Pos Max must be greater than or equal to zero.\n");
         errorFlag = true;
      }
      if (greenThresholdColumn >= 0) {
         if (greenThresholdNeg > 0.0) {
            errorMessage.append("Green Neg Threshold must be less than or equal to zero.\n");
            errorFlag = true;
         }
         if (greenThresholdPos < 0.0) {
            errorMessage.append("Green Pos Threshold must be greater than or equal to zero.\n");
            errorFlag = true;
         }
      }
   }
   
   if (blueMetricColumn >= 0) {
      if (blueMetricNegMax > 0.0) {
         errorMessage.append("Blue Neg Max must be less than or equal to zero.\n");
         errorFlag = true;
      }
      if (blueMetricPosMax < 0.0) {
         errorMessage.append("Blue Pos Max must be greater than or equal to zero.\n");
         errorFlag = true;
      }
      if (blueThresholdColumn >= 0) {
         if (blueThresholdNeg > 0.0) {
            errorMessage.append("Blue Neg Threshold must be less than or equal to zero.\n");
            errorFlag = true;
         }
         if (blueThresholdPos < 0.0) {
            errorMessage.append("Blue Pos Threshold must be greater than or equal to zero.\n");
            errorFlag = true;
         }
      }
   }
   
   if (errorFlag) {
      return true;
   }
   
   //
   // Prepare the RGB Paint file
   //
   const int numNodes = metricFile->getNumberOfNodes();
   if (rgbPaintColumn < 0) {
      if (rgbPaintFile->getNumberOfColumns() == 0) {
         rgbPaintFile->setNumberOfNodesAndColumns(numNodes, 1);
         rgbPaintColumn = 0;
         rgbPaintFile->setFileTitle(rgbTitle);
      }
      else {
         rgbPaintColumn = rgbPaintFile->getNumberOfColumns();
         rgbPaintFile->addColumns(1);
      }
   }
   rgbPaintFile->setColumnName(rgbPaintColumn, rgbPaintColumnName);
   rgbPaintFile->setColumnComment(rgbPaintColumn, rgbTitle);
      
   //
   // Loop through nodes
   //
   for (int i = 0; i < numNodes; i++) {
      float r = 0, g = 0, b = 0;
      if (redMetricColumn >= 0) {
         r = metricFile->getValue(i, redMetricColumn);
         if (redThresholdColumn >= 0) {
            const float rt = metricFile->getValue(i, redThresholdColumn);
            if (rt >= 0.0) {
               if (rt < redThresholdPos) {
                  r = 0.0;
               }
            }
            else {
               if (rt > redThresholdNeg) {
                  r = 0.0;
               }
            }
         }
      }

      if (greenMetricColumn >= 0) {
         g = metricFile->getValue(i, greenMetricColumn);
         if (greenThresholdColumn >= 0) {
            const float gt = metricFile->getValue(i, greenThresholdColumn);
            if (gt >= 0.0) {
               if (gt < greenThresholdPos) {
                  g = 0.0;
               }
            }
            else {
               if (gt > greenThresholdNeg) {
                  g = 0.0;
               }
            }
         }
      }
      if (blueMetricColumn >= 0) {
         b = metricFile->getValue(i, blueMetricColumn);
         if (blueThresholdColumn >= 0) {
            const float bt = metricFile->getValue(i, blueThresholdColumn);
            if (bt >= 0.0) {
               if (bt < blueThresholdPos) {
                  b = 0.0;
               }
            }
            else {
               if (bt > blueThresholdNeg) {
                  b = 0.0;
               }
            }
         }
      }
      rgbPaintFile->setRgb(i, rgbPaintColumn, r, g, b);
   }
  
   if (redMetricColumn >= 0) {
      rgbPaintFile->setTitleRed(rgbPaintColumn, metricFile->getColumnName(redMetricColumn));
      rgbPaintFile->setCommentRed(rgbPaintColumn, metricFile->getColumnComment(redMetricColumn));
      rgbPaintFile->setScaleRed(rgbPaintColumn, redMetricNegMax, redMetricPosMax);
   }
   else {
      rgbPaintFile->setTitleRed(rgbPaintColumn, "Unused");
   }
  
   if (greenMetricColumn >= 0) {
      rgbPaintFile->setTitleGreen(rgbPaintColumn, metricFile->getColumnName(greenMetricColumn));
      rgbPaintFile->setCommentGreen(rgbPaintColumn, metricFile->getColumnComment(greenMetricColumn));
      rgbPaintFile->setScaleGreen(rgbPaintColumn, greenMetricNegMax, greenMetricPosMax);
   }
   else {
      rgbPaintFile->setTitleGreen(rgbPaintColumn, "Unused");
   }
  
   if (blueMetricColumn >= 0) {
      rgbPaintFile->setTitleBlue(rgbPaintColumn, metricFile->getColumnName(blueMetricColumn));
      rgbPaintFile->setCommentBlue(rgbPaintColumn, metricFile->getColumnComment(blueMetricColumn));
      rgbPaintFile->setScaleBlue(rgbPaintColumn, blueMetricNegMax, blueMetricPosMax);
   }
   else {
      rgbPaintFile->setTitleBlue(rgbPaintColumn, "Unused");
   }
   return false;
}

