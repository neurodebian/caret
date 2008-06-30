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

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMetricsToRgbPaintDialog.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "MetricFile.h"
#include "MetricsToRgbPaintConverter.h"
#include "QtUtilities.h"
#include "RgbPaintFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiMetricsToRgbPaintDialog::GuiMetricsToRgbPaintDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Metrics to RGB Paint");
   inhibitUpdate = false;
   
   //
   // Vertical box layout of all items
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(3);
   rows->setSpacing(3);
   
   rows->addWidget(createRgbSection());
   rows->addWidget(createMetricSection());
   rows->addWidget(createThresholdSection());
      
   //
   //
   // Dialog Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   rows->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   buttonsLayout->addWidget(applyButton);
   
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   buttonsLayout->addWidget(closeButton);
   
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   updateDialog();
}

/** 
 * Destructor
 */ 
GuiMetricsToRgbPaintDialog::~GuiMetricsToRgbPaintDialog()
{
}

/**
 * Create the rgb paint section.
 */
QWidget*
GuiMetricsToRgbPaintDialog::createRgbSection()
{
   //
   // Group Box for Threshold Selections
   //
   QGroupBox* rgbPaintGB = new QGroupBox("RGB Paint");
   QVBoxLayout* rgbPaintLayout = new QVBoxLayout(rgbPaintGB);
   
   //
   // Horizontal box for column selection and name
   //
   QHBoxLayout* columnHBoxLayout = new QHBoxLayout;
   rgbPaintLayout->addLayout(columnHBoxLayout);
   columnHBoxLayout->addWidget(new QLabel("Column"));
   rgbColumnComboBox = new GuiNodeAttributeColumnSelectionComboBox( 
                                GUI_NODE_FILE_TYPE_RGB_PAINT, 
                                true, false, false);
   columnHBoxLayout->addWidget(rgbColumnComboBox);
   QObject::connect(rgbColumnComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotRgbColumnSelected(int)));
   columnNameLineEdit = new QLineEdit;
   columnHBoxLayout->addWidget(columnNameLineEdit);
   

   //
   // Horizontal box for comment
   //
   QHBoxLayout* commentHBoxLayout = new QHBoxLayout;
   rgbPaintLayout->addLayout(commentHBoxLayout);
   commentHBoxLayout->addWidget(new QLabel("Comment"));
   columnCommentLineEdit = new QLineEdit;
   commentHBoxLayout->addWidget(columnCommentLineEdit);
   
   //
   // initialize column selection
   //
   rgbColumnComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);

   return rgbPaintGB;
}

/**
 * Create the threshold section.
 */
QWidget*
GuiMetricsToRgbPaintDialog::createThresholdSection()
{
   //
   // Group Box for Threshold Selections
   //
   QGroupBox* thresholdSelectGB = new QGroupBox("Threshold Selection");
   
   //
   // grid for threshold selections
   //
   int numColumns = 0;
   const int NAME_COLUMN       = numColumns++;
   const int THRESHOLD_COLUMN  = numColumns++;
   const int NEG_MAX_COLUMN    = numColumns++;
   const int POS_MAX_COLUMN    = numColumns++;
   int numRows = 0;
   const int TITLE_ROW = numRows++;
   const int RED_ROW   = numRows++;
   const int GREEN_ROW = numRows++;
   const int BLUE_ROW  = numRows++;
   QGridLayout* thresholdSelectGrid = new QGridLayout(thresholdSelectGB);
   thresholdSelectGrid->setMargin(5);
   thresholdSelectGrid->setSpacing(5);
   
   //
   // column titles
   //
   thresholdSelectGrid->addWidget(new QLabel("Color"),
                               TITLE_ROW, NAME_COLUMN, Qt::AlignLeft);
   thresholdSelectGrid->addWidget(new QLabel("Threshold"),
                               TITLE_ROW, THRESHOLD_COLUMN, Qt::AlignHCenter);
   thresholdSelectGrid->addWidget(new QLabel("Negative"),
                               TITLE_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   thresholdSelectGrid->addWidget(new QLabel("Positive"),
                               TITLE_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
                               
   //
   // Red Row
   //
   thresholdSelectGrid->addWidget(new QLabel("Red"),
                               RED_ROW, NAME_COLUMN, Qt::AlignLeft);
   redThresholdComboBox = new GuiNodeAttributeColumnSelectionComboBox(
         GUI_NODE_FILE_TYPE_METRIC, false, true, false);
   QObject::connect(redThresholdComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotRedThresholdSelected(int)));
   thresholdSelectGrid->addWidget(redThresholdComboBox, RED_ROW, THRESHOLD_COLUMN, Qt::AlignLeft);
   redThresholdNegLineEdit = new QLineEdit;
   thresholdSelectGrid->addWidget(redThresholdNegLineEdit, RED_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   redThresholdPosLineEdit = new QLineEdit;
   thresholdSelectGrid->addWidget(redThresholdPosLineEdit, RED_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
   
   //
   // Green Row
   //
   thresholdSelectGrid->addWidget(new QLabel("Green"),
                               GREEN_ROW, NAME_COLUMN, Qt::AlignLeft);
   greenThresholdComboBox = new GuiNodeAttributeColumnSelectionComboBox(
         GUI_NODE_FILE_TYPE_METRIC, false, true, false);
   QObject::connect(greenThresholdComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotGreenThresholdSelected(int)));
   thresholdSelectGrid->addWidget(greenThresholdComboBox, GREEN_ROW, THRESHOLD_COLUMN, Qt::AlignLeft);
   greenThresholdNegLineEdit = new QLineEdit;
   thresholdSelectGrid->addWidget(greenThresholdNegLineEdit, GREEN_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   greenThresholdPosLineEdit = new QLineEdit;
   thresholdSelectGrid->addWidget(greenThresholdPosLineEdit, GREEN_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
   
   //
   // Blue Row
   //
   thresholdSelectGrid->addWidget(new QLabel("Blue"),
                               BLUE_ROW, NAME_COLUMN, Qt::AlignLeft);
   blueThresholdComboBox = new GuiNodeAttributeColumnSelectionComboBox(
         GUI_NODE_FILE_TYPE_METRIC, false, true, false);
   QObject::connect(blueThresholdComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotBlueThresholdSelected(int)));
   thresholdSelectGrid->addWidget(blueThresholdComboBox, BLUE_ROW, THRESHOLD_COLUMN, Qt::AlignLeft);
   blueThresholdNegLineEdit = new QLineEdit;
   thresholdSelectGrid->addWidget(blueThresholdNegLineEdit, BLUE_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   blueThresholdPosLineEdit = new QLineEdit;
   thresholdSelectGrid->addWidget(blueThresholdPosLineEdit, BLUE_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
   
   //
   // Initialize thresholds to NONE
   //
   redThresholdComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   greenThresholdComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   blueThresholdComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   redThresholdNegLineEdit->setText("0.0");
   redThresholdPosLineEdit->setText("0.0");
   greenThresholdNegLineEdit->setText("0.0");
   greenThresholdPosLineEdit->setText("0.0");
   blueThresholdNegLineEdit->setText("0.0");
   blueThresholdPosLineEdit->setText("0.0");
   
   return thresholdSelectGB;
}

/**
 * Create the metric section.
 */
QWidget*
GuiMetricsToRgbPaintDialog::createMetricSection()
{
   //
   // Group Box for Metric Selections
   //
   QGroupBox* metricSelectGB = new QGroupBox("Metric Selection");
   
   //
   // Widget and grid for metric selections
   //
   int numColumns = 0;
   const int NAME_COLUMN    = numColumns++;
   const int METRIC_COLUMN  = numColumns++;
   const int NEG_MAX_COLUMN = numColumns++;
   const int POS_MAX_COLUMN = numColumns++;
   int numRows = 0;
   const int TITLE_ROW = numRows++;
   const int RED_ROW   = numRows++;
   const int GREEN_ROW = numRows++;
   const int BLUE_ROW  = numRows++;
   QGridLayout* metricSelectGrid = new QGridLayout(metricSelectGB);
   metricSelectGrid->setMargin(5);
   metricSelectGrid->setSpacing(5);

   //
   // column titles
   //
   metricSelectGrid->addWidget(new QLabel("Color"),
                               TITLE_ROW, NAME_COLUMN, Qt::AlignLeft);
   metricSelectGrid->addWidget(new QLabel("Metric"),
                               TITLE_ROW, METRIC_COLUMN, Qt::AlignHCenter);
   metricSelectGrid->addWidget(new QLabel("Neg Max"),
                               TITLE_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   metricSelectGrid->addWidget(new QLabel("Pos Max"),
                               TITLE_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
                               
   //
   // Red Row
   //
   metricSelectGrid->addWidget(new QLabel("Red"),
                               RED_ROW, NAME_COLUMN, Qt::AlignLeft);
   redMetricComboBox = new GuiNodeAttributeColumnSelectionComboBox(
         GUI_NODE_FILE_TYPE_METRIC, false, true, false);
   QObject::connect(redMetricComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotRedMetricSelected(int)));
   metricSelectGrid->addWidget(redMetricComboBox, RED_ROW, METRIC_COLUMN, Qt::AlignLeft);
   redMetricNegMaxLineEdit = new QLineEdit;
   metricSelectGrid->addWidget(redMetricNegMaxLineEdit, RED_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   redMetricPosMaxLineEdit = new QLineEdit;
   metricSelectGrid->addWidget(redMetricPosMaxLineEdit, RED_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
   
   //
   // Green Row
   //
   metricSelectGrid->addWidget(new QLabel("Green"),
                               GREEN_ROW, NAME_COLUMN, Qt::AlignLeft);
   greenMetricComboBox = new GuiNodeAttributeColumnSelectionComboBox(
         GUI_NODE_FILE_TYPE_METRIC, false, true, false);
   QObject::connect(greenMetricComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotGreenMetricSelected(int)));
   metricSelectGrid->addWidget(greenMetricComboBox, GREEN_ROW, METRIC_COLUMN, Qt::AlignLeft);
   greenMetricNegMaxLineEdit = new QLineEdit;
   metricSelectGrid->addWidget(greenMetricNegMaxLineEdit, GREEN_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   greenMetricPosMaxLineEdit = new QLineEdit;
   metricSelectGrid->addWidget(greenMetricPosMaxLineEdit, GREEN_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
   
   //
   // Blue Row
   //
   metricSelectGrid->addWidget(new QLabel("Blue"),
                               BLUE_ROW, NAME_COLUMN, Qt::AlignLeft);
   blueMetricComboBox = new GuiNodeAttributeColumnSelectionComboBox(
         GUI_NODE_FILE_TYPE_METRIC, false, true, false);
   QObject::connect(blueMetricComboBox, SIGNAL(itemSelected(int)),
                    this, SLOT(slotBlueMetricSelected(int)));
   metricSelectGrid->addWidget(blueMetricComboBox, BLUE_ROW, METRIC_COLUMN, Qt::AlignLeft);
   blueMetricNegMaxLineEdit = new QLineEdit;
   metricSelectGrid->addWidget(blueMetricNegMaxLineEdit, BLUE_ROW, NEG_MAX_COLUMN, Qt::AlignLeft);
   blueMetricPosMaxLineEdit = new QLineEdit;
   metricSelectGrid->addWidget(blueMetricPosMaxLineEdit, BLUE_ROW, POS_MAX_COLUMN, Qt::AlignLeft);
   
   //
   // Initialize metrics to NONE
   //
   redMetricComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   greenMetricComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   blueMetricComboBox->setCurrentIndex(GuiNodeAttributeColumnSelectionComboBox::CURRENT_ITEM_NONE);
   
   return metricSelectGB;
}

/**
 * Called to update the dialog when rgb or metrics change.
 */
void
GuiMetricsToRgbPaintDialog::updateDialog()
{
   rgbColumnComboBox->updateComboBox(theMainWindow->getBrainSet()->getRgbPaintFile());

   redMetricComboBox->updateComboBox(theMainWindow->getBrainSet()->getMetricFile());
   greenMetricComboBox->updateComboBox(theMainWindow->getBrainSet()->getMetricFile());
   blueMetricComboBox->updateComboBox(theMainWindow->getBrainSet()->getMetricFile());

   redThresholdComboBox->updateComboBox(theMainWindow->getBrainSet()->getMetricFile());
   greenThresholdComboBox->updateComboBox(theMainWindow->getBrainSet()->getMetricFile());
   blueThresholdComboBox->updateComboBox(theMainWindow->getBrainSet()->getMetricFile());

   if (inhibitUpdate == false) {
      slotRgbColumnSelected(rgbColumnComboBox->currentIndex());
      slotRedMetricSelected(redMetricComboBox->currentIndex());
      slotGreenMetricSelected(greenMetricComboBox->currentIndex());
      slotBlueMetricSelected(blueMetricComboBox->currentIndex());
      slotRedThresholdSelected(redThresholdComboBox->currentIndex());
      slotGreenThresholdSelected(greenThresholdComboBox->currentIndex());
      slotBlueThresholdSelected(blueThresholdComboBox->currentIndex());
   }
}

void
GuiMetricsToRgbPaintDialog::slotRgbColumnSelected(int column)
{
   if (column >= 0) {
      RgbPaintFile* rpf = theMainWindow->getBrainSet()->getRgbPaintFile();
      columnNameLineEdit->setText(rpf->getColumnName(column));
      columnCommentLineEdit->setText(rpf->getColumnComment(column));
   }
   else {
      columnNameLineEdit->setText("New Column Name");
      columnCommentLineEdit->setText("Comment");
   }
}

/**
 *
 */
void
GuiMetricsToRgbPaintDialog::loadMetricMinMaxLineEdits(const int metricColumn,
                                                      QLineEdit* minLineEdit,
                                                      QLineEdit* maxLineEdit)
{
   float minValue = 0.0;
   float maxValue = 0.0;
   if (metricColumn >= 0) {
     MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
     mf->getDataColumnMinMax(metricColumn, minValue, maxValue);
   }

   minLineEdit->setText(QString::number(minValue, 'f', 6));
   maxLineEdit->setText(QString::number(maxValue, 'f', 6));
}

/**
 * Slot called when red metric is selected.
 */
void 
GuiMetricsToRgbPaintDialog::slotRedMetricSelected(int item)
{
   loadMetricMinMaxLineEdits(item, redMetricNegMaxLineEdit, redMetricPosMaxLineEdit);
}

/**
 * Slot called when green metric is selected.
 */
void 
GuiMetricsToRgbPaintDialog::slotGreenMetricSelected(int item)
{
   loadMetricMinMaxLineEdits(item, greenMetricNegMaxLineEdit, greenMetricPosMaxLineEdit);
}

/**
 * Slot called when blue metric is selected.
 */
void 
GuiMetricsToRgbPaintDialog::slotBlueMetricSelected(int item)
{
   loadMetricMinMaxLineEdits(item, blueMetricNegMaxLineEdit, blueMetricPosMaxLineEdit);
}

/**
 * Slot called when red threshold is selected.
 */
void 
GuiMetricsToRgbPaintDialog::slotRedThresholdSelected(int /*item*/)
{
   //loadMetricMinMaxLineEdits(item, redThresholdNegLineEdit, redThresholdPosLineEdit);
}

/**
 * Slot called when green threshold is selected.
 */
void 
GuiMetricsToRgbPaintDialog::slotGreenThresholdSelected(int /*item*/)
{
   //loadMetricMinMaxLineEdits(item, greenThresholdNegLineEdit, greenThresholdPosLineEdit);
}

/**
 * Slot called when blue threshold is selected.
 */
void 
GuiMetricsToRgbPaintDialog::slotBlueThresholdSelected(int /*item*/)
{
   //loadMetricMinMaxLineEdits(item, blueThresholdNegLineEdit, blueThresholdPosLineEdit);
}

/**
 * Called when apply button is pressed.
 */
void
GuiMetricsToRgbPaintDialog::slotApplyButton()
{
   MetricsToRgbPaintConverter converter;
   
   const int rgbColumn = rgbColumnComboBox->currentIndex();
   const QString columnComment(columnCommentLineEdit->text());
   const QString columnName(columnNameLineEdit->text());
   
   const int redMetric   = redMetricComboBox->currentIndex();
   const int greenMetric = greenMetricComboBox->currentIndex();
   const int blueMetric  = blueMetricComboBox->currentIndex();
   
   const int redThresh   = redThresholdComboBox->currentIndex();
   const int greenThresh = greenThresholdComboBox->currentIndex();
   const int blueThresh  = blueThresholdComboBox->currentIndex();
   
   const float redMetricNegMax   = QString(redMetricNegMaxLineEdit->text()).toFloat();
   const float greenMetricNegMax = QString(greenMetricNegMaxLineEdit->text()).toFloat();
   const float blueMetricNegMax  = QString(blueMetricNegMaxLineEdit->text()).toFloat();
   
   const float redMetricPosMax   = QString(redMetricPosMaxLineEdit->text()).toFloat();
   const float greenMetricPosMax = QString(greenMetricPosMaxLineEdit->text()).toFloat();
   const float blueMetricPosMax  = QString(blueMetricPosMaxLineEdit->text()).toFloat();
   
   const float redThreshNeg   = QString(redThresholdNegLineEdit->text()).toFloat();
   const float greenThreshNeg = QString(greenThresholdNegLineEdit->text()).toFloat();
   const float blueThreshNeg  = QString(blueThresholdNegLineEdit->text()).toFloat();
   
   const float redThreshPos   = QString(redThresholdPosLineEdit->text()).toFloat();
   const float greenThreshPos = QString(greenThresholdPosLineEdit->text()).toFloat();
   const float blueThreshPos  = QString(blueThresholdPosLineEdit->text()).toFloat();
   
   converter.setRgbPaintInfo(rgbColumn, columnName, columnComment);
   
   converter.setRedMetric(redMetric, redMetricNegMax, redMetricPosMax);
   converter.setGreenMetric(greenMetric, greenMetricNegMax, greenMetricPosMax);
   converter.setBlueMetric(blueMetric, blueMetricNegMax, blueMetricPosMax);
   
   converter.setRedThresh(redThresh, redThreshNeg, redThreshPos);
   converter.setGreenThresh(greenThresh, greenThreshNeg, greenThreshPos);
   converter.setBlueThresh(blueThresh, blueThreshNeg, blueThreshPos);
   
   QString errorMessage;
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   if (converter.convert(theMainWindow->getBrainSet()->getMetricFile(), theMainWindow->getBrainSet()->getRgbPaintFile(), errorMessage)) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "Error", errorMessage);
      return;
   }
   
   inhibitUpdate = true;
   GuiFilesModified fm;
   fm.setRgbPaintModified();
   theMainWindow->fileModificationUpdate(fm);
   inhibitUpdate = false;
   if (rgbColumn < 0) {
      //
      // Reset to newest rgb column 
      //
      rgbColumnComboBox->setCurrentIndex(theMainWindow->getBrainSet()->getRgbPaintFile()->getNumberOfColumns() - 1);
      slotRgbColumnSelected(theMainWindow->getBrainSet()->getRgbPaintFile()->getNumberOfColumns() - 1);
   }
   
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
}
