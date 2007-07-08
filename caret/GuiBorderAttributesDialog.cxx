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

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "GuiBorderAttributesDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiBorderAttributesDialog::GuiBorderAttributesDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   showVarianceTopographyUncertaintyFlag = true;
   
   borderFileType = BORDER_FILE_TYPE_BORDER_SET;

   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const int numBorders = bmbs->getNumberOfBorders();
   for (int j = 0; j < numBorders; j++) {
      BrainModelBorder* b = bmbs->getBorder(j);
      BorderAttributes ba(b->getName(), 
                          j,
                          b->getNumberOfBorderLinks(),
                          b->getSamplingDensity(), 
                          b->getVariance(), 
                          b->getTopography(), 
                          b->getArealUncertainty());
      
      attributes.push_back(ba);
   }
   
   createDialog();
}

/**
 * Constructor for use with borders in a border or border projection file.
 */
GuiBorderAttributesDialog::GuiBorderAttributesDialog(QWidget* parent,
                                                     const QString fileNameIn,
                                                     const BORDER_FILE_TYPE borderFileTypeIn,
                                                     const bool showVarianceTopographyUncertaintyFlagIn)
   : QtDialog(parent, true)
{
   showVarianceTopographyUncertaintyFlag = showVarianceTopographyUncertaintyFlagIn;
   borderFileType = borderFileTypeIn;
   fileName = fileNameIn;
   
   switch (borderFileType) {
      case BORDER_FILE_TYPE_BORDER:
         {
            try {
               borderFile.readFile(fileName);
               
               const int numBorders = borderFile.getNumberOfBorders();
               for (int i = 0; i < numBorders; i++) {
                  Border* b = borderFile.getBorder(i);
                  BorderAttributes ba(b->getName(), 
                                      i,
                                      b->getNumberOfLinks(),
                                      b->getSamplingDensity(), 
                                      b->getVariance(), 
                                      b->getTopographyValue(), 
                                      b->getArealUncertainty());
                  
                  attributes.push_back(ba);
               }
            }
            catch (FileException& e) {
            }
         }
         break;
      case BORDER_FILE_TYPE_BORDER_PROJECTION:
         try {
            borderProjectionFile.readFile(fileName);
            
            const int numBorders = borderProjectionFile.getNumberOfBorderProjections();
            for (int i = 0; i < numBorders; i++) {
               BorderProjection* b = borderProjectionFile.getBorderProjection(i);
               QString name;
               float center[3];
               float samplingDensity;
               float variance;
               float topography;
               float arealUncertainty;
               b->getData(name,
                          center,
                          samplingDensity,
                          variance,
                          topography,
                          arealUncertainty);
               BorderAttributes ba(name, 
                                   i,
                                   b->getNumberOfLinks(),
                                   samplingDensity, 
                                   variance, 
                                   topography, 
                                   arealUncertainty);
               
               attributes.push_back(ba);
            }
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
            return;
         }
         break;
      case BORDER_FILE_TYPE_BORDER_SET:
         break;
   }
   
   createDialog();
}

/**
 * create the dialog.
 */
void 
GuiBorderAttributesDialog::createDialog()      
{
   std::sort(attributes.begin(), attributes.end());

   setWindowTitle("Edit Border Attributes");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(5);
   
   //
   // Grid columns
   //
   const int numAttributes = static_cast<int>(attributes.size());
   int numColumns = 0;
   const int RETAIN_COLUMN      = numColumns++;
   const int NAME_COLUMN        = numColumns++;
   const int INDEX_COLUMN       = numColumns++;
   const int LINKS_COLUMN       = numColumns++;
   const int SAMPLING_COLUMN    = numColumns++;
   const int VARIANCE_COLUMN    = numColumns++;
   const int TOPOGRAPHY_COLUMN  = numColumns++;
   const int UNCERTAINTY_COLUMN = numColumns++;
   int rowNumber = 0;
   
   //
   // widget and layout in the scrollview
   //
   QWidget* svWidget = new QWidget;
   QGridLayout* gridLayout = new QGridLayout(svWidget);
   gridLayout->setMargin(3);
   gridLayout->setSpacing(3);
   
   //
   // column titles
   //
   gridLayout->addWidget(new QLabel("Keep"), rowNumber, RETAIN_COLUMN, Qt::AlignLeft);
   gridLayout->addWidget(new QLabel("Name"), rowNumber, NAME_COLUMN);//, Qt::AlignLeft);
   gridLayout->addWidget(new QLabel("File\nIndex"), rowNumber, INDEX_COLUMN, Qt::AlignLeft);
   gridLayout->addWidget(new QLabel("Number Of\nLinks"), rowNumber, LINKS_COLUMN, Qt::AlignLeft);   
   gridLayout->addWidget(new QLabel("Sampling"), rowNumber, SAMPLING_COLUMN, Qt::AlignLeft);
   if (showVarianceTopographyUncertaintyFlag) {
      gridLayout->addWidget(new QLabel("Variance"), rowNumber, VARIANCE_COLUMN, Qt::AlignLeft);
      gridLayout->addWidget(new QLabel("Topography"), rowNumber, TOPOGRAPHY_COLUMN, Qt::AlignLeft);
      gridLayout->addWidget(new QLabel("Uncertainty"), rowNumber, UNCERTAINTY_COLUMN, Qt::AlignLeft);
   }
   rowNumber++;
   
   gridLayout->setColumnStretch(RETAIN_COLUMN, 0);
   gridLayout->setColumnStretch(NAME_COLUMN, 1000);
   gridLayout->setColumnStretch(INDEX_COLUMN, 0);
   gridLayout->setColumnStretch(LINKS_COLUMN, 0);
   gridLayout->setColumnStretch(SAMPLING_COLUMN, 0);
   if (showVarianceTopographyUncertaintyFlag) {
      gridLayout->setColumnStretch(VARIANCE_COLUMN, 0);
      gridLayout->setColumnStretch(TOPOGRAPHY_COLUMN, 0);
      gridLayout->setColumnStretch(UNCERTAINTY_COLUMN, 0);
   }
   
   const int lineEditNumberWidth = 60;
   //
   // Border data
   //
   for (int i = 0; i < numAttributes; i++) {
      // 
      // Retain checkbox
      //
      QCheckBox* checkBox = new QCheckBox("");
      checkBox->setChecked(true);
      gridLayout->addWidget(checkBox, rowNumber, RETAIN_COLUMN, Qt::AlignLeft);
      retainCheckBoxes.push_back(checkBox);
      
      //
      // Name line edit
      //
      QLineEdit* nameLE = new QLineEdit;
      nameLE->setText(attributes[i].name);
      nameLE->setMinimumWidth(200);
      gridLayout->addWidget(nameLE, rowNumber, NAME_COLUMN); //, Qt::AlignLeft);
      nameLineEdits.push_back(nameLE);
      
      //
      // Index label
      //
      QLabel* indexLabel = new QLabel(QString::number(attributes[i].fileIndex));
      gridLayout->addWidget(indexLabel, rowNumber, INDEX_COLUMN, Qt::AlignLeft);

      //
      // Number of Links
      //      
      QLabel* linksLabel = new QLabel(QString::number(attributes[i].numberOfLinks));
      gridLayout->addWidget(linksLabel, rowNumber, LINKS_COLUMN, Qt::AlignLeft);
      
      //
      // Sampling line edit
      //
      QLineEdit* samplingLE = new QLineEdit;
      samplingLE->setMaximumWidth(lineEditNumberWidth);
      samplingLE->setText(QString::number(attributes[i].sampling, 'f', 2));
      gridLayout->addWidget(samplingLE, rowNumber, SAMPLING_COLUMN, Qt::AlignLeft);
      samplingLineEdits.push_back(samplingLE);
      
      if (showVarianceTopographyUncertaintyFlag) {
         //
         // Variance line edit
         //
         QLineEdit* varianceLE = new QLineEdit;
         varianceLE->setMaximumWidth(lineEditNumberWidth);
         varianceLE->setText(QString::number(attributes[i].variance, 'f', 2));
         gridLayout->addWidget(varianceLE, rowNumber, VARIANCE_COLUMN, Qt::AlignLeft);
         varianceLineEdits.push_back(varianceLE);
         
         //
         // Topography line edit
         //
         QLineEdit* topographyLE = new QLineEdit;
         topographyLE->setMaximumWidth(lineEditNumberWidth);
         topographyLE->setText(QString::number(attributes[i].topography, 'f', 2));
         gridLayout->addWidget(topographyLE, rowNumber, TOPOGRAPHY_COLUMN, Qt::AlignLeft);
         topographyLineEdits.push_back(topographyLE);
         
         //
         // Uncertainty line edit
         //
         QLineEdit* uncertaintyLE = new QLineEdit;
         uncertaintyLE->setMaximumWidth(lineEditNumberWidth);
         uncertaintyLE->setText(QString::number(attributes[i].uncertainty, 'f', 2));
         gridLayout->addWidget(uncertaintyLE, rowNumber, UNCERTAINTY_COLUMN, Qt::AlignLeft);
         uncertaintyLineEdits.push_back(uncertaintyLE);
      }
      
      rowNumber++;
   }
   
   //
   // Scroll View for all selections
   //
   QScrollArea* sv = new QScrollArea(this);
   sv->setWidget(svWidget);
   sv->setWidgetResizable(true);
   dialogLayout->addWidget(sv);

   //
   // Set the minimum size for the scroll area
   //
/*
   int minWidth = svWidget->sizeHint().width() + 10;
   if (minWidth > 600) minWidth = 600;
   sv->setMinimumWidth(minWidth);
   int minHeight = svWidget->sizeHint().height();
   if (minHeight > 200) minHeight = 200;
   sv->setMinimumHeight(minHeight);
*/   
   //
   // Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* okButton = new QPushButton("OK");
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   buttonsLayout->addWidget(okButton);
   
   QPushButton* cancelButton = new QPushButton("Cancel");
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   buttonsLayout->addWidget(cancelButton);
   
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);   
}

/**
 * Destructor.
 */
GuiBorderAttributesDialog::~GuiBorderAttributesDialog()
{
}

/**
 * process the border file.
 */
void 
GuiBorderAttributesDialog::processBorderFile()
{
   const int numAtt = static_cast<int>(attributes.size());
   const int numBorders = borderFile.getNumberOfBorders();
   std::vector<bool> deleteBorderFlag(numBorders, false);
   
   for (int i = 0; i < numAtt; i++) {
      const int borderIndex = attributes[i].fileIndex;
      
      if (retainCheckBoxes[i]->isChecked()) {
         deleteBorderFlag[borderIndex] = false;
         
         QString name;
         float center[3];
         float samplingDensity;
         float variance;
         float topography;
         float arealUncertainty;
         
         Border* b = borderFile.getBorder(borderIndex);
         b->getData(name,
                    center,
                    samplingDensity,
                    variance,
                    topography,
                    arealUncertainty);
         
         name = nameLineEdits[i]->text();
         samplingDensity = samplingLineEdits[i]->text().toFloat();
         if (showVarianceTopographyUncertaintyFlag) {
            variance = varianceLineEdits[i]->text().toFloat();
            topography = topographyLineEdits[i]->text().toFloat();
            arealUncertainty = uncertaintyLineEdits[i]->text().toFloat();
         }
         
         b->setData(name,
                    center,
                    samplingDensity,
                    variance,
                    topography,
                    arealUncertainty);
      }
      else {
         deleteBorderFlag[borderIndex] = true;
      }
   }
   
   for (int i = (numBorders - 1); i >= 0; i--) {
      if (deleteBorderFlag[i]) {
         borderFile.removeBorder(i);
      }
   }
   
   try {
      borderFile.writeFile(fileName);
   }
   catch (FileException& e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
   }
}

/**
 * process the border projection file.
 */
void 
GuiBorderAttributesDialog::processBorderProjectionFile()
{
   const int numAtt = static_cast<int>(attributes.size());
   const int numBorders = borderProjectionFile.getNumberOfBorderProjections();
   std::vector<bool> deleteBorderFlag(numBorders, false);
   
   for (int i = 0; i < numAtt; i++) {
      const int borderIndex = attributes[i].fileIndex;
      
      if (retainCheckBoxes[i]->isChecked()) {
         deleteBorderFlag[borderIndex] = false;
         
         QString name;
         float center[3];
         float samplingDensity;
         float variance;
         float topography;
         float arealUncertainty;
         
         BorderProjection* b = borderProjectionFile.getBorderProjection(borderIndex);
         b->getData(name,
                    center,
                    samplingDensity,
                    variance,
                    topography,
                    arealUncertainty);
         
         name = nameLineEdits[i]->text();
         samplingDensity = samplingLineEdits[i]->text().toFloat();
         if (showVarianceTopographyUncertaintyFlag) {
            variance = varianceLineEdits[i]->text().toFloat();
            topography = topographyLineEdits[i]->text().toFloat();
            arealUncertainty = uncertaintyLineEdits[i]->text().toFloat();
         }
         
         b->setData(name,
                    center,
                    samplingDensity,
                    variance,
                    topography,
                    arealUncertainty);
      }
      else {
         deleteBorderFlag[borderIndex] = true;
      }
   }
   
   for (int i = (numBorders - 1); i >= 0; i--) {
      if (deleteBorderFlag[i]) {
         borderProjectionFile.removeBorderProjection(i);
      }
   }
   
   try {
      borderProjectionFile.writeFile(fileName);
   }
   catch (FileException& e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
   }
}
      
/**
 * process the brain model border set borders.
 */
void 
GuiBorderAttributesDialog::processBrainModelBorderSetBorders()
{
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      
   const int numAtt = static_cast<int>(attributes.size());
   const int numBorders = bmbs->getNumberOfBorders();
   std::vector<bool> deleteBorderFlag(numBorders, false);
   
   for (int i = 0; i < numAtt; i++) {
      const int borderIndex = attributes[i].fileIndex;
      
      if (retainCheckBoxes[i]->isChecked()) {
         deleteBorderFlag[borderIndex] = false;
         
         BrainModelBorder* b = bmbs->getBorder(borderIndex);
         b->setName(nameLineEdits[i]->text());
         b->setSamplingDensity(samplingLineEdits[i]->text().toFloat());
         if (showVarianceTopographyUncertaintyFlag) {
            b->setVariance(varianceLineEdits[i]->text().toFloat());
            b->setTopography(topographyLineEdits[i]->text().toFloat());
            b->setArealUncertainty(uncertaintyLineEdits[i]->text().toFloat());
         }
      }
      else {
         deleteBorderFlag[borderIndex] = true;
      }
   }
   
   for (int i = (numBorders - 1); i >= 0; i--) {
      if (deleteBorderFlag[i]) {
         bmbs->deleteBorder(i);
      }
   }
   
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   
   GuiBrainModelOpenGL::updateAllGL(NULL);
}
         
/**
 * Apply button slot.
 */
void
GuiBorderAttributesDialog::done(int r)
{
   if (r == GuiBorderAttributesDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      switch (borderFileType) {
         case BORDER_FILE_TYPE_BORDER:
            processBorderFile();
            break;
         case BORDER_FILE_TYPE_BORDER_PROJECTION:
            processBorderProjectionFile();
            break;
         case BORDER_FILE_TYPE_BORDER_SET:
            processBrainModelBorderSetBorders();
            break;
      }
      
      QApplication::restoreOverrideCursor();
   }
   
   QtDialog::done(r);
}

