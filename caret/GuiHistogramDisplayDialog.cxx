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

#include <algorithm>
#include <iostream>
#include <set>

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QProgressDialog>
#include <QPushButton>

#include "DebugControl.h"
#include "GuiGraphWidget.h"
#include "GuiHistogramDisplayDialog.h"
#include "StatisticHistogram.h"
#include "WuQSaveWidgetAsImagePushButton.h"
#include "QtUtilities.h"
#include "StatisticDataGroup.h"

/**
 * Constructor  (dialog deleted when closed if non-modal).
 */
GuiHistogramDisplayDialog::GuiHistogramDisplayDialog(QWidget* parent, 
                                       const QString& titleCaption,
                                       const std::vector<float>& values,
                                       const bool showGrayWhitePeaks,
                                       const bool modalFlag, 
                                       Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   setModal(true);
   if (modalFlag == false) {
      //
      // Delete this dialog when user presses close button
      //
      setAttribute(Qt::WA_DeleteOnClose);
   }
   
   histogramGraphIndex = -1;
   histogram96GraphIndex = -1;
   histogramSelection = HISTO_SELECTION_ALL;
   
   enum {
      PROGRESS_STEPS_0 = 1,
      PROGRESS_STEPS_1 = 2,
      PROGRESS_STEPS_TOTAL = 3
   };
   QProgressDialog standbyDialog("Determining Histogram, please wait.",
                                 "",
                                 0,
                                 PROGRESS_STEPS_TOTAL,
                                 parent);
   standbyDialog.setCancelButton(0);
   standbyDialog.show();
   qApp->processEvents();  // note: qApp is global in QApplication
   
   //
   // Determine the normal histogram
   //
   histogram = new StatisticHistogram(256);
   StatisticDataGroup sdg(&values, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   histogram->addDataGroup(&sdg);
   try {
      histogram->execute();
   }
   catch (StatisticException&) {
   }
   standbyDialog.setValue(PROGRESS_STEPS_0);
   qApp->processEvents();  // note: qApp is global in QApplication  

   if (DebugControl::getDebugOn()) {
      for (int i = 0; i < 256; i++) {
         float val, cnt;
         histogram->getDataForBucket(i, val, cnt);
         std::cout << "New Hist: " << i << " " << val << " " << cnt << std::endl;
      }
   }
   
   //
   // Determine the 2% to 98% histogram
   //
   histogram96 = new StatisticHistogram(256, 2.0, 2.0);
   histogram96->addDataGroup(&sdg); // reuse data from normal histogram
   try {
      histogram96->execute();
   }
   catch (StatisticException&) {
   }
   standbyDialog.setValue(PROGRESS_STEPS_1);
   qApp->processEvents();  // note: qApp is global in QApplication
   
   QString caption("Histogram");
   if (titleCaption.isEmpty() == false) {
      caption.append(" - ");
      caption.append(titleCaption);
   }
   setWindowTitle(caption);
   
   //
   // Layout for histogram
   //
   QVBoxLayout* histogramLayoutWidgetLayout = new QVBoxLayout;
   histogramLayoutWidgetLayout->setSpacing(2);
   
   //
   // Widget showing histogram
   //
   histogramWidget = new GuiGraphWidget(0, "Histogram");
   histogramWidget->setFixedHeight(histogramWidget->sizeHint().height());
   
   //
   // Group box for display mode
   //
   QGroupBox* displayModeGroupBox = new QGroupBox("Display Mode");
   QVBoxLayout* displayModeLayout = new QVBoxLayout(displayModeGroupBox);
   displayModeComboBox = new QComboBox;
   displayModeLayout->addWidget(displayModeComboBox, Qt::AlignLeft);
   displayModeComboBox->insertItem(HISTO_SELECTION_ALL, "All");
   displayModeComboBox->insertItem(HISTO_SELECTION_MIDDLE_96, "Middle 96%");
   QObject::connect(displayModeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotDisplayModeComboBox(int)));
   displayModeGroupBox->setFixedSize(displayModeGroupBox->sizeHint());
   

/*
   //
   // Show hide stats push button
   //
   showHideStatsPushButton = new QPushButton("Hide Statisitics");
   modeStatsHBoxLayout->addWidget(showHideStatsPushButton);
   showHideStatsPushButton->setAutoDefault(false);
   showHideStatsPushButton->setFixedSize(showHideStatsPushButton->sizeHint());
   QObject::connect(showHideStatsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotShowHideStatsPushButton()));
*/
                    
   //
   // Display statistics for data 
   //
   const QString blanks("           ");
   meanLabel = new QLabel(blanks);
   mean96Label = new QLabel(blanks);
   stdDevLabel = new QLabel(blanks);
   stdDev96Label = new QLabel(blanks);
   minLabel = new QLabel(blanks);
   min96Label = new QLabel(blanks);
   maxLabel = new QLabel(blanks);
   max96Label = new QLabel(blanks);
   rangeLabel = new QLabel(blanks);
   range96Label = new QLabel(blanks);
   statsGroup = new QGroupBox("Statistics");
   histogramLayoutWidgetLayout->addWidget(statsGroup);
   QGridLayout* statsGroupLayout = new QGridLayout(statsGroup);
   statsGroupLayout->addWidget(new QLabel(" "), 0, 0);
   statsGroupLayout->addWidget(new QLabel("All "), 0, 1);
   statsGroupLayout->addWidget(new QLabel("   "), 0, 2);
   statsGroupLayout->addWidget(new QLabel("Middle 96%"), 0, 3);
   statsGroupLayout->addWidget(new QLabel("Average   "), 1, 0);
   statsGroupLayout->addWidget(meanLabel, 1, 1);;
   statsGroupLayout->addWidget(new QLabel("   "), 1, 2);
   statsGroupLayout->addWidget(mean96Label, 1, 3);
   statsGroupLayout->addWidget(new QLabel("Sample Deviation "), 2, 0);
   statsGroupLayout->addWidget(stdDevLabel, 2, 1);
   statsGroupLayout->addWidget(new QLabel("   "), 2, 2);
   statsGroupLayout->addWidget(stdDev96Label, 2, 3);
   statsGroupLayout->addWidget(new QLabel("Minimum   "), 4, 0);
   statsGroupLayout->addWidget(minLabel, 4, 1);
   statsGroupLayout->addWidget(new QLabel("   "), 4, 2);
   statsGroupLayout->addWidget(min96Label, 4, 3);
   statsGroupLayout->addWidget(new QLabel("Maximum   "), 5, 0);
   statsGroupLayout->addWidget(maxLabel, 5, 1);
   statsGroupLayout->addWidget(new QLabel("   "), 5, 2);
   statsGroupLayout->addWidget(max96Label, 5, 3);
   statsGroupLayout->addWidget(new QLabel("Range     "), 6, 0);
   statsGroupLayout->addWidget(rangeLabel, 6, 1);
   statsGroupLayout->addWidget(new QLabel("   "), 6, 2);
   statsGroupLayout->addWidget(range96Label, 6, 3);
   statsGroup->setFixedSize(statsGroup->sizeHint());
   
   //
   // Min/Max/Peak Gray/White Values
   //
   QGroupBox* whiteGrayGroupBox = NULL;
   csfPeakLabel = NULL;
   grayMinLabel = NULL;
   grayPeakLabel = NULL;
   grayWhiteLabel = NULL;
   whitePeakLabel = NULL;
   whiteMaxLabel = NULL;
   if (showGrayWhitePeaks) {
      QLabel* csfPeakTextLabel = new QLabel("CSF Peak");
      csfPeakLabel = new QLabel(blanks);
      QLabel* grayMinTextLabel = new QLabel("Gray Min");
      grayMinLabel = new QLabel(blanks);
      QLabel* grayPeakTextLabel = new QLabel("Gray Peak");
      grayPeakLabel = new QLabel(blanks);
      QLabel* grayWhiteTextLabel = new QLabel("Gray/White");
      grayWhiteLabel = new QLabel(blanks);
      QLabel* whitePeakTextLabel = new QLabel("White Peak");
      whitePeakLabel = new QLabel(blanks);
      QLabel* whiteMaxTextLabel = new QLabel("White Max");
      whiteMaxLabel = new QLabel(blanks);
      whiteGrayGroupBox = new QGroupBox("Gray/White Values");
      QGridLayout* whiteGrayGridLayout = new QGridLayout(whiteGrayGroupBox);
      whiteGrayGridLayout->addWidget(csfPeakTextLabel, 0, 0);
      whiteGrayGridLayout->addWidget(csfPeakLabel, 0, 1);
      whiteGrayGridLayout->addWidget(grayMinTextLabel, 1, 0);
      whiteGrayGridLayout->addWidget(grayMinLabel, 1, 1);
      whiteGrayGridLayout->addWidget(grayPeakTextLabel, 2, 0);
      whiteGrayGridLayout->addWidget(grayPeakLabel, 2, 1);
      whiteGrayGridLayout->addWidget(grayWhiteTextLabel, 3, 0);
      whiteGrayGridLayout->addWidget(grayWhiteLabel, 3, 1);
      whiteGrayGridLayout->addWidget(whitePeakTextLabel, 4, 0);
      whiteGrayGridLayout->addWidget(whitePeakLabel, 4, 1);
      whiteGrayGridLayout->addWidget(whiteMaxTextLabel, 5, 0);
      whiteGrayGridLayout->addWidget(whiteMaxLabel, 5, 1);
   }

   //
   // Load the histograms into the graph
   //
   loadDataIntoGraph(HISTO_SELECTION_ALL);
   loadDataIntoGraph(HISTO_SELECTION_MIDDLE_96);
   
   //
   // Histogram smoothing
   //
   QPushButton* smoothPushButton = NULL;
   if (DebugControl::getTestFlag1()) {
      smoothPushButton = new QPushButton("Smooth Histogram...");
      smoothPushButton->setFixedSize(smoothPushButton->sizeHint());
      smoothPushButton->setAutoDefault(false);
      QObject::connect(smoothPushButton, SIGNAL(clicked()),
                       this, SLOT(slotSmoothPushButton()));
   }
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   
   //
   // Save as Image button
   //
   WuQSaveWidgetAsImagePushButton* saveAsImageButton = 
              new WuQSaveWidgetAsImagePushButton("Save As Image...",
                                                histogramLayoutWidget);
   buttonsLayout->addWidget(saveAsImageButton);
   saveAsImageButton->setFixedSize(saveAsImageButton->sizeHint());
                    
   if (isModal()) {
      //
      // OK button
      //
      QPushButton* okButton = new QPushButton("OK");
      buttonsLayout->addWidget(okButton);
      QObject::connect(okButton, SIGNAL(clicked()),
                     this, SLOT(accept()));
                     
      //
      // Cancel button
      //
      QPushButton* cancelButton = new QPushButton("Cancel");
      cancelButton->setAutoDefault(false);
      buttonsLayout->addWidget(cancelButton);
      QObject::connect(cancelButton, SIGNAL(clicked()),
                     this, SLOT(reject()));
      
      QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   }
   else {
      //
      // Close button
      //
      QPushButton* closeButton = new QPushButton("Close");
      closeButton->setAutoDefault(false);
      closeButton->setFixedSize(closeButton->sizeHint());
      buttonsLayout->addWidget(closeButton);
      QObject::connect(closeButton, SIGNAL(clicked()),
                     this, SLOT(slotCloseButton()));
   }
   
   //
   // Stats and white Gray
   //
   QHBoxLayout* statsGrayWhiteLayout = new QHBoxLayout;
   statsGrayWhiteLayout->addWidget(statsGroup);
   if (whiteGrayGroupBox != NULL) {
      statsGrayWhiteLayout->addWidget(whiteGrayGroupBox);
   }
   statsGrayWhiteLayout->addStretch();
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   dialogLayout->addWidget(histogramWidget);
   dialogLayout->addWidget(displayModeGroupBox);
   if (smoothPushButton != NULL) {
      dialogLayout->addWidget(smoothPushButton);
   }
   dialogLayout->addLayout(statsGrayWhiteLayout);
   dialogLayout->addLayout(buttonsLayout);
   
   slotDisplayModeComboBox(displayModeComboBox->currentIndex());
   
   standbyDialog.close();
}

/**
 * Destructor.
 */
GuiHistogramDisplayDialog::~GuiHistogramDisplayDialog()
{
   if (histogram != NULL) {
      delete histogram;
      histogram = NULL;
   }
   if (histogram96 != NULL) {
      delete histogram96;
      histogram96 = NULL;
   }
}

/**
 * load gray/white peak information.
 */
void 
GuiHistogramDisplayDialog::loadGrayWhitePeakInfo(const HISTO_SELECTION selectedHistogram)
{
   if (grayMinLabel != NULL) {
      int grayPeakBucketNumber;
      int whitePeakBucketNumber;
      int grayMinimumBucketNumber;
      int whiteMaximumBucketNumber;
      int grayWhiteBoundaryBucketNumber;
      int csfPeakBucketNumber;
      switch (selectedHistogram) {
         case HISTO_SELECTION_ALL:
            histogram->getGrayWhitePeakEstimates(grayPeakBucketNumber,
                                                 whitePeakBucketNumber,
                                                 grayMinimumBucketNumber,
                                                 whiteMaximumBucketNumber,
                                                 grayWhiteBoundaryBucketNumber,
                                                 csfPeakBucketNumber);
            break;
         case HISTO_SELECTION_MIDDLE_96:
            histogram96->getGrayWhitePeakEstimates(grayPeakBucketNumber,
                                                   whitePeakBucketNumber,
                                                   grayMinimumBucketNumber,
                                                   whiteMaximumBucketNumber,
                                                   grayWhiteBoundaryBucketNumber,
                                                   csfPeakBucketNumber);
            break;
      }
      csfPeakLabel->setText(QString::number(histogram->getDataValueForBucket(csfPeakBucketNumber), 'f', 1));
      grayMinLabel->setText(QString::number(histogram->getDataValueForBucket(grayMinimumBucketNumber), 'f', 1));
      grayPeakLabel->setText(QString::number(histogram->getDataValueForBucket(grayPeakBucketNumber), 'f', 1));
      grayWhiteLabel->setText(QString::number(histogram->getDataValueForBucket(grayWhiteBoundaryBucketNumber), 'f', 1));
      whitePeakLabel->setText(QString::number(histogram->getDataValueForBucket(whitePeakBucketNumber), 'f', 1));
      whiteMaxLabel->setText(QString::number(histogram->getDataValueForBucket(whiteMaximumBucketNumber), 'f', 1));
   }
}      

/**
 * load the data into graph widget.
 */
void 
GuiHistogramDisplayDialog::loadDataIntoGraph(const HISTO_SELECTION selectedHistogram)
{
   switch (selectedHistogram) {
      case HISTO_SELECTION_ALL:
         {
            //
            // Remove if already in graph
            //
            if (histogramGraphIndex >= 0) {
               histogramWidget->removeData(histogramGraphIndex);
            }
            
            //
            // Get stats about the histograms
            //
            float minValue, maxValue, range, mean, deviation;
            histogram->getDataStatistics(minValue, maxValue, range, mean, deviation);
            meanLabel->setText(QString::number(mean, 'f', 6));
            stdDevLabel->setText(QString::number(deviation, 'f', 6));
            minLabel->setText(QString::number(minValue, 'f', 6));
            maxLabel->setText(QString::number(maxValue, 'f', 6));
            rangeLabel->setText(QString::number(range, 'f', 6));                  

            //
            // Add the first histogram data to the graph
            //
            std::vector<double> dataX, dataY;
            for (int i = 0; i < histogram->getNumberOfBuckets(); i++) {
               float x, y;
               histogram->getDataForBucket(i, x, y);
               dataX.push_back(x);
               dataY.push_back(y);
               
               //
               // Need to get max value since bucket X-Value is at left side of bucket
               // so maximum value is at right side of last bucket
               //
               if ((histogram->getNumberOfBuckets() - 1) == i) {
                  dataX.push_back(maxValue);
                  dataY.push_back(0.0);
                  
               }
            }
            histogramGraphIndex = histogramWidget->addData(dataX, dataY, QColor(255, 0, 0),
                                     GuiGraphWidget::DRAW_DATA_TYPE_BARS);
         }
         break;
      case HISTO_SELECTION_MIDDLE_96:
         {
            //
            // Remove if already in graph
            //
            if (histogram96GraphIndex >= 0) {
               histogramWidget->removeData(histogram96GraphIndex);
            }
            
            //
            // Get stats about the histograms
            //
            float minValue96, maxValue96, range96, mean96, deviation96;
            histogram96->getDataStatistics(minValue96, maxValue96, range96, mean96, deviation96);

                  
            mean96Label->setText(QString::number(mean96, 'f', 6));
            stdDev96Label->setText(QString::number(deviation96, 'f', 6));
            min96Label->setText(QString::number(minValue96, 'f', 6));
            max96Label->setText(QString::number(maxValue96, 'f', 6));
            range96Label->setText(QString::number(range96, 'f', 6));

            //
            // Add the 96% histogram data to the graph
            //
            std::vector<double> dataX96, dataY96;
            for (int i = 0; i < histogram96->getNumberOfBuckets(); i++) {
               float x, y;
               histogram96->getDataForBucket(i, x, y);
               dataX96.push_back(x);
               dataY96.push_back(y);
            }
            histogram96GraphIndex = histogramWidget->addData(dataX96, dataY96, QColor(255, 0, 0),
                                     GuiGraphWidget::DRAW_DATA_TYPE_BARS);
         }
         break;
   }
}
      
/**
 * called when smooth histogram button pressed.
 */
void 
GuiHistogramDisplayDialog::slotSmoothPushButton()
{
   const float strength = 0.5;
   const int iterations = 5;
   const int neighborDepth = 5;
   
   showWaitCursor();
   const HISTO_SELECTION histSel = static_cast<HISTO_SELECTION>(displayModeComboBox->currentIndex());
   switch (histSel) {
      case HISTO_SELECTION_ALL:
         histogram->smoothHistogram(strength, iterations, neighborDepth);
         {
            int csf, gray, white, grayMin, whiteMax, grayWhite;
            histogram->getGrayWhitePeakEstimates(gray, white, grayMin, whiteMax, grayWhite, csf);
            //std::cout << "Gray peak: " << gray 
            //          << "  white peak: " << white << std::endl;
         }
         break;
      case HISTO_SELECTION_MIDDLE_96:
         histogram96->smoothHistogram(strength, iterations, neighborDepth);
         break;
   }
   loadDataIntoGraph(histSel);
   slotDisplayModeComboBox(histSel);
   showNormalCursor();
}
      
/**
 * called when show/hide stats push button is pressed.
 */
void 
GuiHistogramDisplayDialog::slotShowHideStatsPushButton()
{
   statsGroup->setHidden(! statsGroup->isHidden());
   if (statsGroup->isHidden()) {
      showHideStatsPushButton->setText("Show Statistics");
   }
   else {
      showHideStatsPushButton->setText("Hide Statistics");
   }
   setMinimumSize(sizeHint());
   setMaximumSize(sizeHint());
   adjustSize();
}
      
/**
 * Called when non-modal dialog is closed.
 */
void
GuiHistogramDisplayDialog::slotCloseButton()
{
   close();
}

/**
 * Called when display mode changed
 */
void
GuiHistogramDisplayDialog::slotDisplayModeComboBox(int value)
{
   histogramSelection = static_cast<HISTO_SELECTION>(value);
   switch (histogramSelection) {
      case HISTO_SELECTION_ALL:
         histogramWidget->setDataDisplayed(histogramGraphIndex, true);
         histogramWidget->setDataDisplayed(histogram96GraphIndex, false);
         break;
      case HISTO_SELECTION_MIDDLE_96:
         histogramWidget->setDataDisplayed(histogramGraphIndex, false);
         histogramWidget->setDataDisplayed(histogram96GraphIndex, true);
         break;
   }
   
   loadGrayWhitePeakInfo(histogramSelection);
}

/**
 * get the peaks if "CHOOSE_PEAKS" mode.
 */
void 
GuiHistogramDisplayDialog::getPeaks(float& minPeak, float& maxPeak)
{
   minPeak = 0;
   maxPeak = 0;
}
      
/**
 * Slot for OK or Cancel button in modal mode.
 */
void
GuiHistogramDisplayDialog::done(int r) 
{
   if (r == QDialog::Accepted) {
   }
   QDialog::done(r);
}
 
/**
 * Slot for apply button in non-modal mode.
 */
void
GuiHistogramDisplayDialog::slotApply()
{
}


