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

#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "GuiMorphingMeasurementsDialog.h"

/**
 * constructor
 */
GuiMorphingMeasurementsDialog::GuiMorphingMeasurementsDialog(
                        const std::vector<MorphingMeasurements>& mm, 
                        const float totalTime,
                        const BrainModelSurfaceMorphing::MORPHING_SURFACE_TYPE morphingType,
                        const bool deleteDialogWhenClosedFlag,
                        QWidget* parent)
   : WuQDialog(parent)
{
   if (morphingType == BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL) {
      setWindowTitle("Spherical Morphing Measurements");
   }
   else {
      setWindowTitle("Flat Morphing Measurements");
   }
   
   if (deleteDialogWhenClosedFlag) {
      setAttribute(Qt::WA_DeleteOnClose);
   }
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // Grid layout
   //
   const int numItems = static_cast<int>(mm.size());
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   int rowNumber = 0;
  
   //
   // column titles for 100% values
   //
   gridLayout->addWidget(new QLabel("Statistics for All"), rowNumber, 0,
                                  1, 11, Qt::AlignHCenter);
   rowNumber++;
   gridLayout->addWidget(new QLabel(" "), rowNumber, 0, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Node "), rowNumber, 1, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Tile "), rowNumber, 2, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 3, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 4, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 5, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 6, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 7, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 8, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 9, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 10, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Total "), rowNumber, 11, Qt::AlignRight);
   rowNumber++;
   gridLayout->addWidget(new QLabel("Cycle "), rowNumber, 0, Qt::AlignLeft);
   gridLayout->addWidget(new QLabel("Crossovers "), rowNumber, 1, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Crossovers "), rowNumber, 2, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Average "), rowNumber, 3, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Deviation "), rowNumber, 4, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Minimum "), rowNumber, 5, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Maximum "), rowNumber, 6, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Average "), rowNumber, 7, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Deviation "), rowNumber, 8, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Minimum "), rowNumber, 9, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Maximum "), rowNumber, 10, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Time "), rowNumber, 11, Qt::AlignRight);
   rowNumber++;
   
   //
   // Add in the data with stats for all values
   //
   for (int i = 0; i < numItems; i++) {
      QString name;
      int nodeCrossovers;
      int tileCrossovers;
      StatisticsUtilities::DescriptiveStatistics adStats, ldStats;
      float et;
      mm[i].get(name, adStats, ldStats,
                nodeCrossovers, tileCrossovers, et);
                
      gridLayout->addWidget(new QLabel(name),
                            rowNumber, 0, Qt::AlignLeft);
      gridLayout->addWidget(new QLabel(QString::number(nodeCrossovers)),
                            rowNumber, 1, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(tileCrossovers)),
                            rowNumber, 2, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.average, 'f', 3)),
                            rowNumber, 3, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.standardDeviation, 'f', 3)),
                            rowNumber, 4, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.minValue, 'f', 3)),
                            rowNumber, 5, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.maxValue, 'f', 3)),
                            rowNumber, 6, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.average, 'f', 3)),
                            rowNumber, 7, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.standardDeviation, 'f', 3)),
                            rowNumber, 8, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.minValue, 'f', 3)),
                            rowNumber, 9, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.maxValue, 'f', 3)),
                            rowNumber, 10, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(et, 'f', 3)),
                            rowNumber, 11, Qt::AlignRight);
      rowNumber++;
   }
   
   //
   // couple of blank rows
   //
   gridLayout->addWidget(new QLabel(" "), rowNumber, 0, 1, 11);
   rowNumber++;
   gridLayout->addWidget(new QLabel(" "), rowNumber, 0, 1, 11);
   rowNumber++;
   
   //
   // column titles for 100% values
   //
   gridLayout->addWidget(new QLabel("Statistics With Top and Bottom 2% Excluded"), rowNumber, 0,
                                  1, 11, Qt::AlignHCenter);
   rowNumber++;
   gridLayout->addWidget(new QLabel(" "), rowNumber, 0, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Node "), rowNumber, 1, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Tile "), rowNumber, 2, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 3, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 4, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 5, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Areal Dist "), rowNumber, 6, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 7, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 8, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 9, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Linear Dist "), rowNumber, 10, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Total "), rowNumber, 11, Qt::AlignRight);
   rowNumber++;
   gridLayout->addWidget(new QLabel("Cycle "), rowNumber, 0, Qt::AlignLeft);
   gridLayout->addWidget(new QLabel("Crossovers "), rowNumber, 1, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Crossovers "), rowNumber, 2, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Average "), rowNumber, 3, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Deviation "), rowNumber, 4, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Minimum "), rowNumber, 5, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Maximum "), rowNumber, 6, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Average "), rowNumber, 7, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Deviation "), rowNumber, 8, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Minimum "), rowNumber, 9, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Maximum "), rowNumber, 10, Qt::AlignRight);
   gridLayout->addWidget(new QLabel("Time "), rowNumber, 11, Qt::AlignRight);
   rowNumber++;
   
   //
   // Add in the data with stats for all values
   //
   for (int i = 0; i < numItems; i++) {
      QString name;
      int nodeCrossovers;
      int tileCrossovers;
      StatisticsUtilities::DescriptiveStatistics adStats, ldStats;
      float et;
      mm[i].get(name, adStats, ldStats,
                nodeCrossovers, tileCrossovers, et);
                
      gridLayout->addWidget(new QLabel(name),
                            rowNumber, 0, Qt::AlignLeft);
      gridLayout->addWidget(new QLabel(QString::number(nodeCrossovers)),
                            rowNumber, 1, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(tileCrossovers)),
                            rowNumber, 2, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.average96, 'f', 3)),
                            rowNumber, 3, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.standardDeviation96, 'f', 3)),
                            rowNumber, 4, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.minValue96, 'f', 3)),
                            rowNumber, 5, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(adStats.maxValue96, 'f', 3)),
                            rowNumber, 6, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.average96, 'f', 3)),
                            rowNumber, 7, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.standardDeviation96, 'f', 3)),
                            rowNumber, 8, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.minValue96, 'f', 3)),
                            rowNumber, 9, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(ldStats.maxValue96, 'f', 3)),
                            rowNumber, 10, Qt::AlignRight);
      gridLayout->addWidget(new QLabel(QString::number(et, 'f', 3)),
                            rowNumber, 11, Qt::AlignRight);
      rowNumber++;
   }
   
   //
   // couple of blank rows
   //
   gridLayout->addWidget(new QLabel(" "), rowNumber, 0, 1, 11);
   rowNumber++;
   gridLayout->addWidget(new QLabel(" "), rowNumber, 0, 1, 11);
   rowNumber++;
   
   //
   // Display the total execution time
   //
   QString totalTimeText("Total Execution Time (Minutes): ");
   totalTimeText += QString::number((totalTime / 60.0), 'f', 1);
   dialogLayout->addWidget(new QLabel(totalTimeText));
   
   //
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setFixedSize(closeButton->sizeHint());
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
}

/**
 * destructor
 */
GuiMorphingMeasurementsDialog::~GuiMorphingMeasurementsDialog()
{
}

