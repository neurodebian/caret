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

#include <iostream>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>

#include "BrainSet.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CellFileProjector.h"
#include "CellProjectionFile.h"
#include "ContourCellColorFile.h"
#include "ContourCellFile.h"
#include "DebugControl.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsContours.h"
#include "DisplaySettingsSection.h"
#include "GuiAddCellsDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorSelectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiNameSelectionDialog.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "SectionFile.h"

#include "global_variables.h"

/**
 * Constructor.
 */
GuiAddCellsDialog::GuiAddCellsDialog(const DIALOG_MODE dm, QWidget* parent, bool modal, Qt::WFlags f)
   : QtDialog(parent, modal, f)
{
   dialogMode = dm;
   
   cellNameChanged = true;
   cellClassNameChanged = true;
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   switch (dialogMode) {
      case DIALOG_MODE_ADD_CELLS:
         setWindowTitle("Add Cells");
         break;
      case DIALOG_MODE_ADD_CONTOUR_CELLS:
         setWindowTitle("Add Contour Cells");
         break;
   }
   
   //
   // Group box for cell data
   //
   QGroupBox* cellDataGroup = new QGroupBox("Cell Data");
   QVBoxLayout* cellDataGroupLayout = new QVBoxLayout(cellDataGroup);
   dialogLayout->addWidget(cellDataGroup);
   
   //
   // Grid for labels, line edits, and pushbuttons
   //
   QGridLayout* cellGridLayout = new QGridLayout;
   cellDataGroupLayout->addLayout(cellGridLayout);
   
   //
   // label, line edit, and push button for cell name
   //
   cellGridLayout->addWidget(new QLabel("Name"), 0, 0);
   nameLineEdit = new QLineEdit;
   cellGridLayout->addWidget(nameLineEdit, 0, 1);
   QObject::connect(nameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotCellNameChanged()));
   QPushButton* namePushButton = new QPushButton("Choose...");
   cellGridLayout->addWidget(namePushButton, 0, 2);
   namePushButton->setAutoDefault(false);
   namePushButton->setFixedSize(namePushButton->sizeHint());
   QObject::connect(namePushButton, SIGNAL(clicked()),
                    this, SLOT(slotNameButton()));
   
   //
   // label, line edit, and push button for cell class
   //
   cellGridLayout->addWidget(new QLabel("Class"), 1, 0);
   classLineEdit = new QLineEdit;
   cellGridLayout->addWidget(classLineEdit, 1, 1);
   QObject::connect(classLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotCellClassNameChanged()));
   QPushButton* classPushButton = new QPushButton("Choose...");
   cellGridLayout->addWidget(classPushButton, 1, 2);
   classPushButton->setAutoDefault(false);
   classPushButton->setFixedSize(namePushButton->sizeHint());
   QObject::connect(classPushButton, SIGNAL(clicked()),
                    this, SLOT(slotClassButton()));
   
   switch (dialogMode) {
      case DIALOG_MODE_ADD_CELLS:
         //
         // Auto project check box
         //
         autoProjectCellsCheckBox = new QCheckBox("Project Cells Automatically");
         cellDataGroupLayout->addWidget(autoProjectCellsCheckBox);
         break;
      case DIALOG_MODE_ADD_CONTOUR_CELLS:
         break;
   }
   
   //
   // Comment label and combo box
   //
   QHBoxLayout* commentBoxLayout = new QHBoxLayout;
   cellDataGroupLayout->addLayout(commentBoxLayout);
   QLabel* commentLabel = new QLabel("Comment ");
   commentBoxLayout->addWidget(commentLabel);
   commentComboBox = new QComboBox;
   commentBoxLayout->addWidget(commentComboBox);
   QObject::connect(commentComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotCommentComboBox(int)));
   commentBoxLayout->setStretchFactor(commentLabel, 0);
   commentBoxLayout->setStretchFactor(commentComboBox, 100);

   //
   // comment text edit
   //
   commentTextEdit = new QTextEdit;
   cellDataGroupLayout->addWidget(commentTextEdit);
   commentTextEdit->setReadOnly(false);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(2);
   
   //
   // Apply button 
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   updateDialog();
   slotCommentComboBox(commentComboBox->count() - 1);
}

/**
 * Destructor.
 */
GuiAddCellsDialog::~GuiAddCellsDialog()
{
}

/**
 * Called to add a contour cell at the specified position and section
 */
void
GuiAddCellsDialog::addContourCellAtXYZ(const float xyz[3], const int sectionNumber)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Adding cell at xyz " 
                << "(" << xyz[0] << ", " << xyz[1] << "," << xyz[2] << ")" << std::endl;
   }
   
   //
   // See if name needs to be obtained
   //
   if (cellNameChanged) {
      if (getNameAndColor()) {
         return;
      }
   }

   //
   // See if class name needs to be obtained
   //
   if (cellClassNameChanged) {
      if (getCellClass()) {
         return;
      }
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Get the cell file
   //   
   ContourCellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
   
   //
   // Get the comment number
   //
   int commentNumber = commentComboBox->currentIndex();
   
   //
   // Comment number greater than number of comments is "none"
   // Comment number equal to number of comments is "new comment"
   //
   bool newCommentFlag = false;
   if (commentNumber > cf->getNumberOfStudyInfo()) {
      commentNumber = -1;
    }
    else {
      if (commentNumber == cf->getNumberOfStudyInfo()) {
         CellStudyInfo csi;
         commentNumber = cf->addStudyInfo(csi);
         commentComboBox->setCurrentIndex(commentNumber);
         newCommentFlag = true;
      }   
      //
      // Get and store the comment text
      //
      CellStudyInfo csi;
      csi.setTitle(commentTextEdit->document()->toPlainText());
      cf->setStudyInfo(commentNumber, csi);
      
   }
   
   //
   // Create the cell
   //
   CellData cd(cellName, xyz[0], xyz[1], xyz[2], sectionNumber,  
               cellClassName, commentNumber, cellColorIndex);
   cd.setColorIndex(cellColorIndex);
   cf->addCell(cd);

   //
   // Display Cells and notify about cell changed
   //
   DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();
   dsc->setDisplayContourCells(true);
   GuiFilesModified fm;
   fm.setContourCellModified();
   fm.setContourCellColorModified();
   theMainWindow->fileModificationUpdate(fm);

   if (newCommentFlag) {
      updateDialog();
   }
   
   // Update all displayed surfaces
   //
   //
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
}   

/**
 * Called to add a cell at the specified node number
 */
void
GuiAddCellsDialog::addCellAtNodeNumber(const int nodeNumber)
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Adding cell at node " << nodeNumber << std::endl;
   }
   
   QString errorMessage;
   
   //
   // Check to see that a fiducial surface is available.
   //   
   const BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      errorMessage.append("There is no fiducial surface in the main window.");
   }
   else if (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
      errorMessage.append("There is no fiducial surface in the main window.");
   }
   
   //
   // Is there an error ?
   //
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this, "Cell Error", errorMessage);
      return;
   }
   //
   // See if name needs to be obtained
   //
   if (cellNameChanged) {
      if (getNameAndColor()) {
         return;
      }
   }

   //
   // See if class name needs to be obtained
   //
   if (cellClassNameChanged) {
      if (getCellClass()) {
         return;
      }
   }

   //
   // Is there an error ?
   //
   if (errorMessage.isEmpty() == false) {
      QMessageBox::critical(this, "Cell Error", errorMessage);
      return;
   }

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // Get the position of the node
   //
   float x, y, z;
   const CoordinateFile* coordFile = bms->getCoordinateFile();
   coordFile->getCoordinate(nodeNumber, x, y, z);

   //
   // Get the cell file
   //   
   CellProjectionFile* cpf = theMainWindow->getBrainSet()->getCellProjectionFile();
   
   //
   // Get the comment number
   //
   int commentNumber = commentComboBox->currentIndex();
   
   //
   // Comment number greater than number of comments is "none"
   // Comment number equal to number of comments is "new comment"
   //
   bool newCommentFlag = false;
   if (commentNumber > cpf->getNumberOfStudyInfo()) {
      commentNumber = -1;
    }
    else {
      if (commentNumber == cpf->getNumberOfStudyInfo()) {
         CellStudyInfo csi;
         commentNumber = cpf->addStudyInfo(csi);
         commentComboBox->setCurrentIndex(commentNumber);
         newCommentFlag = true;
      }   
      //
      // Get and store the comment text
      //
      CellStudyInfo csi;
      csi.setTitle(commentTextEdit->document()->toPlainText());
      cpf->setStudyInfo(commentNumber, csi);
      
   }
   
   //
   // Get the displayed section number
   //
   int sectionNumber = 0;
   SectionFile* sectionFile = theMainWindow->getBrainSet()->getSectionFile();
   DisplaySettingsSection* dss = theMainWindow->getBrainSet()->getDisplaySettingsSection();
   if (sectionFile != NULL) {
      const int column = dss->getSelectedColumn();
      if ((column >= 0) && (column < sectionFile->getNumberOfColumns())) {
         sectionNumber = sectionFile->getSection(nodeNumber, column);
      }
   }
   
   int cellNumber = -1;
   
   //
   // Create the cell
   //
   CellData cd(cellName, x, y, z, sectionNumber,  
               cellClassName, commentNumber, cellColorIndex);
   cd.setColorIndex(cellColorIndex);
   CellProjection cp;
   cp.copyData(cd);
   cpf->addCellProjection(cp);
   cellNumber = cpf->getNumberOfCellProjections() - 1;

   //
   // If cells should be automatically projected
   //
   if (autoProjectCellsCheckBox->isChecked()) {
      //
      // Create the cell file projector (cell same as cells)
      //
      CellFileProjector projector(bms);
      
      //
      // Project the new cell
      //
      projector.projectFile(cpf,
                            cellNumber, 
                            CellFileProjector::PROJECTION_TYPE_FLIP_TO_MATCH_HEMISPHERE, 
                            0.0,
                            false,
                            NULL);
   }

   //
   // Display Cells and notify about cell changed
   //
   DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
   dsc->setDisplayCells(true);
   GuiFilesModified fm;
   fm.setCellModified();
   fm.setCellColorModified();
   theMainWindow->fileModificationUpdate(fm);

   if (newCommentFlag) {
      updateDialog();
   }
   
   // Update all displayed surfaces
   //
   //
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
}

/**
 * update for cell projections.
 */
void 
GuiAddCellsDialog::updateCellProjectionMode()
{
   CellProjectionFile* cpf = theMainWindow->getBrainSet()->getCellProjectionFile();
   
   int currentCommentNumber = commentComboBox->currentIndex();
   commentComboBox->clear();
   
   if (currentCommentNumber >= cpf->getNumberOfStudyInfo()) {
      currentCommentNumber = -1;
   }
   const int numComments = cpf->getNumberOfStudyInfo();
   for (int i = 0; i < numComments; i++) {
      const CellStudyInfo* cci = cpf->getStudyInfo(i);
      QString s(cci->getTitle());
      if (s.length() > 30) {
         s.resize(30);
      }
      QString qs(QString::number(i));
      qs.append(" - ");
      qs.append(s);
      commentComboBox->addItem(qs);
   }
   commentComboBox->addItem("New");
   commentComboBox->addItem("None");
   if ((commentComboBox->count() == 2) || (currentCommentNumber < 0)) {      
      currentCommentNumber = commentComboBox->count() - 1;
      commentTextEdit->clear();
   }
   commentTextEdit->setEnabled(false);

   //
   // load the comment's text
   //
   slotCommentComboBox(currentCommentNumber);
}

/**
 * update contour cells.
 */
void 
GuiAddCellsDialog::updateContourCellMode()
{
   CellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
   if (cf == NULL) {
      std::cout << "PROGRAM ERROR: NULL Cell File in GuiAddCellsDialog::updateDialog()" << std::endl;
      return;
   }
   
   int currentCommentNumber = commentComboBox->currentIndex();
   commentComboBox->clear();
   
   if (cf != NULL) {
      if (currentCommentNumber >= cf->getNumberOfStudyInfo()) {
         currentCommentNumber = -1;
      }
      const int numComments = cf->getNumberOfStudyInfo();
      for (int i = 0; i < numComments; i++) {
         const CellStudyInfo* cci = cf->getStudyInfo(i);
         QString s(cci->getTitle());
         if (s.length() > 30) {
            s.resize(30);
         }
         QString qs(QString::number(i));
         qs.append(" - ");
         qs.append(s);
         commentComboBox->addItem(qs);
      }
   }
   commentComboBox->addItem("New");
   commentComboBox->addItem("None");
   if ((commentComboBox->count() == 2) || (currentCommentNumber < 0)) {      
      currentCommentNumber = commentComboBox->count() - 1;
      commentTextEdit->clear();
   }
   commentTextEdit->setEnabled(false);

   //
   // load the comment's text
   //
   slotCommentComboBox(currentCommentNumber);
}

/**
 * Update the dialog due to file changes.
 */
void
GuiAddCellsDialog::updateDialog()
{
   switch (dialogMode) {
      case DIALOG_MODE_ADD_CELLS:
         updateCellProjectionMode();
         break;
      case DIALOG_MODE_ADD_CONTOUR_CELLS:
         updateContourCellMode();
         break;
   }
}

/**
 * Get the class.  Returns true if missing.
 */
bool
GuiAddCellsDialog::getCellClass()
{
   //
   // Get the class name
   //
   cellClassName = classLineEdit->text();
   if (cellClassName.isEmpty()) {
      QMessageBox::critical(this, "Cell Error", "You must enter a class name.");
      return true;
   }
   cellClassNameChanged = false;
   
   return false;
}

/**
 * Get the name and the color.  Returns true if an item is missing.
 */
bool
GuiAddCellsDialog::getNameAndColor()
{
   QString errorMessage;
   
   //
   // Get the name of the cell
   //
   cellName = nameLineEdit->text();
   if (cellName.isEmpty()) {
      QMessageBox::critical(this, "Cell Error", "You must enter a cell name.");
      return true;
   }

   //
   // Find the matching color
   //
   bool colorMatch = false;
   ColorFile* colorFile = NULL;
   switch (dialogMode) {
      case DIALOG_MODE_ADD_CELLS:
         colorFile = theMainWindow->getBrainSet()->getCellColorFile();
         break;
      case DIALOG_MODE_ADD_CONTOUR_CELLS:
         colorFile = theMainWindow->getBrainSet()->getContourCellColorFile();
         break;
   }
   if (colorFile == NULL) {
      std::cout << "PROGRAM ERROR: NULL Color File in GuiAddCellsDialog::getNameAndColor()" << std::endl;
      return true;
   }
   
   cellColorIndex = colorFile->getColorIndexByName(cellName, colorMatch);
   
   //
   // Cell color may need to be created
   //
   bool createColor = false;
   if ((cellColorIndex >= 0) && (colorMatch == true)) {
      createColor = false;
   }
   else if ((cellColorIndex >= 0) && (colorMatch == false)) {
      QString msg("Use cell color \"");
      msg.append(colorFile->getColorNameByIndex(cellColorIndex));
      msg.append("\" for cell ");
      msg.append(cellName);
      msg.append(" ?");
      QString noButton("No, define color ");
      noButton.append(cellName);
      if (QMessageBox::question(this, 
                                  "Use Partially Matching Color",
                                  msg, 
                                  (QMessageBox::Yes | QMessageBox::No), 
                                  QMessageBox::No) 
                                     == QMessageBox::No) {
         createColor = true;
      }
   }
   else {
      createColor = true;
   }
   
   if (createColor) {
      QString title("Create Cell Color ");
      title.append(cellName);
      GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(this, 
                                                                 title, 
                                                                 true,
                                                                 true, 
                                                                 false,
                                                                 true);
      csd->exec();
      
      //
      // Add new cell color
      //
      float pointSize = 2.0, lineSize = 1.0;
      unsigned char r, g, b, a;
      ColorFile::ColorStorage::SYMBOL symbol;
      csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
      colorFile->addColor(cellName, r, g, b, a, pointSize, lineSize, symbol);
      cellColorIndex = colorFile->getNumberOfColors() - 1;
   }
   
   cellNameChanged = false;
   
   return false;
}

/**
 * Called when apply button is pressed.
 */
void 
GuiAddCellsDialog::slotApplyButton()
{
   if (getNameAndColor()) {
      return;
   }
   if (getCellClass()) {
      return;
   }
   
   switch (dialogMode) {
      case DIALOG_MODE_ADD_CELLS:
         theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CELL_ADD);
         break;
      case DIALOG_MODE_ADD_CONTOUR_CELLS:
         theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_ADD);
         break;
   }
}

/**
 * Called when button name choose is pressed.
 */
void 
GuiAddCellsDialog::slotNameButton()
{
   GuiNameSelectionDialog nsd(this);
   if (nsd.exec() == QDialog::Accepted) {
      const QString name(nsd.getName());
      if (name.isEmpty() == false) {
         nameLineEdit->setText(name);
         cellNameChanged = true;
      }
   }
}

/**
 * Called when button comment choose is pressed.
 */
void 
GuiAddCellsDialog::slotClassButton()
{
   int numClasses = 0;
   std::vector<QString> classNames;
   
   switch (dialogMode) {
      case DIALOG_MODE_ADD_CELLS:
         {
            CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
            numClasses = cf->getNumberOfCellClasses();
            for (int i = 0; i < numClasses; i++) {
               classNames.push_back(cf->getCellClassNameByIndex(i));
            }
         }
         break;
      case DIALOG_MODE_ADD_CONTOUR_CELLS:
         {
            CellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
            numClasses = cf->getNumberOfCellClasses();
            for (int i = 0; i < numClasses; i++) {
               classNames.push_back(cf->getCellClassNameByIndex(i));
            }
         }
         break;
   }
   
   if (classNames.size() > 0) {
      QtListBoxSelectionDialog lbsd(this, "Cell Classes", "", classNames, 0);
      if (lbsd.exec() == QDialog::Accepted) {
         classLineEdit->setText(lbsd.getSelectedText());
         cellClassNameChanged = true;
      }
   }
   else {
      QMessageBox::information(this, "No Classes", "Cell File has no classes");   
   }
}

/**
 * Called when comment combo box item is selected.
 */
void 
GuiAddCellsDialog::slotCommentComboBox(int itemNumber)
{
   commentComboBox->setCurrentIndex(itemNumber);

   commentTextEdit->setPlainText("");   
   
   switch (dialogMode) {
      case DIALOG_MODE_ADD_CELLS:
         {
            CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
            if (cf != NULL) {
               if (itemNumber < cf->getNumberOfStudyInfo()) {
                  const CellStudyInfo* csi = cf->getStudyInfo(itemNumber);
                  commentTextEdit->setPlainText(csi->getTitle());
               }
            }
         }
         break;
      case DIALOG_MODE_ADD_CONTOUR_CELLS:
         {
            CellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
            if (cf != NULL) {
               if (itemNumber < cf->getNumberOfStudyInfo()) {
                  const CellStudyInfo* csi = cf->getStudyInfo(itemNumber);
                  commentTextEdit->setPlainText(csi->getTitle());
               }
            }
         }
         break;
   }
   
   if (itemNumber < (commentComboBox->count() - 1)) {
      commentTextEdit->setEnabled(true);
   }
   else {
      commentTextEdit->setEnabled(false);
   }
}

/**
 * Called when cell name line edit is changed.
 */
void 
GuiAddCellsDialog::slotCellNameChanged()
{
   cellNameChanged = true;
}

/**
 * Called when cell class name line edit is changed.
 */
void 
GuiAddCellsDialog::slotCellClassNameChanged()
{
   cellClassNameChanged = true;
}
