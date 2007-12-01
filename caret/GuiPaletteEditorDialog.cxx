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

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QSpinBox>
#include <QToolTip>

#include "AreaColorFile.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorFileEditorDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiPaletteColorSelectionDialog.h"
#include "GuiPaletteEditorDialog.h"
#include "PaletteFile.h"
#include <QDoubleSpinBox>
#include "QtRadioButtonSelectionDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiPaletteEditorDialog::GuiPaletteEditorDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   paletteBeingEdited = NULL;
   loadingPalette = false;
   
   setWindowTitle("Palette Editor");

   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Palette Colors group box
   //
   QGroupBox* colorsGroupBox = new QGroupBox("Palette Colors");
   dialogLayout->addWidget(colorsGroupBox);
   QVBoxLayout* colorsGroupBoxLayout = new QVBoxLayout(colorsGroupBox);
   
   //
   // Edit colors button
   //
   QPushButton* editColorsPushButton = new QPushButton("Add, Delete, and Edit Colors...");
   colorsGroupBoxLayout->addWidget(editColorsPushButton);
   editColorsPushButton->setAutoDefault(false);
   QObject::connect(editColorsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotEditColorsPushButton()));
   
   //
   // Palette select, create, delete group box
   //
   QGroupBox* selectGroupBox = new QGroupBox("Palette");
   dialogLayout->addWidget(selectGroupBox);
   QVBoxLayout* selectGroupBoxLayout = new QVBoxLayout(selectGroupBox);
   
   //
   // Palette selection
   //
   paletteSelectionComboBox = new QComboBox;
   selectGroupBoxLayout->addWidget(paletteSelectionComboBox);
   QObject::connect(paletteSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPaletteSelectionComboBox(int)));
   
   //
   // Create new palette button
   //
   QPushButton* createNewPaletteButton = new QPushButton("New...");
   createNewPaletteButton->setAutoDefault(false);
   QObject::connect(createNewPaletteButton, SIGNAL(clicked()),
                    this, SLOT(slotCreateNewPaletteButton()));
                    
   //
   // Rename palette button
   //
   QPushButton* renamePaletteButton = new QPushButton("Rename...");
   renamePaletteButton->setAutoDefault(false);
   QObject::connect(renamePaletteButton, SIGNAL(clicked()),
                    this, SLOT(slotRenamePaletteButton()));
    
   //
   // Delete palette button
   //
   QPushButton* deletePaletteButton = new QPushButton("Delete...");
   deletePaletteButton->setAutoDefault(false);
   QObject::connect(deletePaletteButton, SIGNAL(clicked()),
                    this, SLOT(slotDeletePaletteButton()));
    
   //
   // Create new and delete palette box
   //
   QHBoxLayout* cdHBoxLayout = new QHBoxLayout;
   selectGroupBoxLayout->addLayout(cdHBoxLayout);
   cdHBoxLayout->addWidget(createNewPaletteButton);
   cdHBoxLayout->addWidget(renamePaletteButton);
   cdHBoxLayout->addWidget(deletePaletteButton);
   
   //
   // Make new and delete same size
   //
   QtUtilities::makeButtonsSameSize(createNewPaletteButton, deletePaletteButton);

   //
   // Limit size of palette selection group box
   //
   selectGroupBox->setFixedSize(selectGroupBox->sizeHint());
   
   //
   // Group box for editing palettes
   //
   QGroupBox* editPaletteGroup = new QGroupBox("Edit Selected Palette");
   QVBoxLayout* editPaletteGroupLayout = new QVBoxLayout(editPaletteGroup);
   dialogLayout->addWidget(editPaletteGroup);
   
   //
   // Grid for palette entries
   //
   paletteEntryWidget = new QWidget;
   paletteEntryGridLayout = new QGridLayout(paletteEntryWidget);
   paletteEntryGridLayout->setMargin(5);
   paletteEntryGridLayout->setSpacing(3);
   
   //
   // Scrolling for palette entries
   //
   QScrollArea* paletteEntryScrollView = new QScrollArea;
   paletteEntryScrollView->setWidget(paletteEntryWidget);
   editPaletteGroupLayout->addWidget(paletteEntryScrollView);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(3);
   
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
   
   //
   // Create the signal mappers for the various palette entry buttons
   //
   addPaletteEntrySignalMapper = new QSignalMapper(this);
   QObject::connect(addPaletteEntrySignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotAddPaletteEntry(int)));
                    
   colorPaletteEntrySignalMapper = new QSignalMapper(this);
   QObject::connect(colorPaletteEntrySignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotColorPaletteEntry(int)));
                    
   removePaletteEntrySignalMapper = new QSignalMapper(this);
   QObject::connect(removePaletteEntrySignalMapper, SIGNAL(mapped(int)),
                    this, SLOT(slotRemovePaletteEntry(int)));
   
   //
   // Load palettes into the dialog
   //
   updateDialog();
}

/**
 * called when edit colors button is pressed.
 */
void 
GuiPaletteEditorDialog::slotEditColorsPushButton()
{
   PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
   //
   // Place palette colors into a color file
   //
   AreaColorFile cf;
   pf->getPaletteColorsUsingColorFile(cf);   
   
   //
   // Show colors in the color file editor dialog
   //
   GuiColorFileEditorDialog fed(this, 
                                &cf, 
                                false, 
                                false,
                                false,
                                false, 
                                true, 
                                0);
   fed.exec();
   
   //
   // Update colors
   //
   pf->replacePaletteColorsUsingColorFile(cf);
   
   //
   // Update the dialog
   //
   slotPaletteSelectionComboBox(paletteSelectionComboBox->currentIndex());
   slotApplyButton();
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiPaletteEditorDialog::slotApplyButton()
{
   if (loadingPalette) {
      return;
   }
   if (paletteBeingEdited != NULL) {
      const int fileNE = paletteBeingEdited->getNumberOfPaletteEntries();
      const int numSpin = static_cast<int>(paletteEntryScalarSpinBoxes.size());
      int numEntries = 0;
      for (int j = 0; j < numSpin; j++) {
         if (paletteEntryScalarSpinBoxes[j]->isHidden() == false) {
            numEntries++;
         }
      }
      if (fileNE != numEntries) {
         std::cout << "PROGRAM ERROR: number of entries in palette does not match that "
                   << "in the palette editor." << std::endl;
         return;
      }      
      
      for (int i = 0; i < numEntries; i++) {
         PaletteEntry* pe = paletteBeingEdited->getPaletteEntry(i);
         pe->setValue(paletteEntryScalarSpinBoxes[i]->value());
      }
   }

   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   bsnc->assignColors();
   BrainModelVolumeVoxelColoring* bmvvc = theMainWindow->getBrainSet()->getVoxelColoring();
   bmvvc->setVolumeFunctionalColoringInvalid();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called when a palette is selected.
 */
void 
GuiPaletteEditorDialog::slotPaletteSelectionComboBox(int item)
{
   loadingPalette = true;
   
   const int spinBoxWidth = 100;
   
   paletteBeingEdited = NULL;
   int numPaletteEntries = 0;
   
   PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
   if ((item >= 0) && (item < pf->getNumberOfPalettes())) {
      paletteBeingEdited = pf->getPalette(item);
      numPaletteEntries = paletteBeingEdited->getNumberOfPaletteEntries();
   }
   
   const int numValidRows = static_cast<int>(paletteEntryScalarSpinBoxes.size());
   
   //
   // Create new items as needed
   //
   for (int i = 0; i < numPaletteEntries; i++) {
      if (i >= numValidRows) {
         //
         // Insert push button
         //
         QPushButton* insertButton = new QPushButton("+");
         insertButton->setAutoDefault(false);
         insertButton->setFixedSize(insertButton->sizeHint());
         paletteEntryInsertPushButtons.push_back(insertButton);
         addPaletteEntrySignalMapper->setMapping(insertButton, i);
         QObject::connect(insertButton, SIGNAL(clicked()),
                          addPaletteEntrySignalMapper, SLOT(map()));
         insertButton->setToolTip(
                       "Press this button to insert\n"
                       "a row below this row.");
         paletteEntryGridLayout->addWidget(insertButton, i, 0);
         
         //
         // Delete push button
         //
         QPushButton* deleteButton = new QPushButton("-");
         deleteButton->setAutoDefault(false);
         deleteButton->setFixedSize(deleteButton->sizeHint());
         paletteEntryDeletePushButtons.push_back(deleteButton);
         removePaletteEntrySignalMapper->setMapping(deleteButton, i);
         QObject::connect(deleteButton, SIGNAL(clicked()),
                          removePaletteEntrySignalMapper, SLOT(map()));
         deleteButton->setToolTip( 
                       "Press this button\n"
                       "to delete this row.");
         paletteEntryGridLayout->addWidget(deleteButton, i, 1);
         
         //
         // Spin box for palette scalar
         //
         QDoubleSpinBox* fsb = new QDoubleSpinBox;
         fsb->setMinimum(-1.0);
         fsb->setMaximum(1.0);
         fsb->setSingleStep(0.01);
         fsb->setDecimals(3);
         fsb->setFixedWidth(spinBoxWidth);
         QObject::connect(fsb, SIGNAL(valueChanged(double)),
                          this, SLOT(slotApplyButton()));
         paletteEntryScalarSpinBoxes.push_back(fsb);
         paletteEntryGridLayout->addWidget(fsb, i, 2);
         
         //
         // Color button
         //
         QPushButton* fpb = new QPushButton(" ");
         fpb->setAutoDefault(false);
         paletteEntryColorPushButtons.push_back(fpb);
         colorPaletteEntrySignalMapper->setMapping(fpb, i);
         QObject::connect(fpb, SIGNAL(clicked()),
                          colorPaletteEntrySignalMapper, SLOT(map()));
         fpb->setToolTip( 
                       "Press this button to change\n"
                       "the color for this row.");
         paletteEntryGridLayout->addWidget(fpb, i, 3);
         
         //
         // Color swatch
         //
         QWidget* swatch = new QWidget;
         swatch->setFixedSize(QSize(20, 20));
         paletteEntryColorSwatch.push_back(swatch);
         paletteEntryGridLayout->addWidget(swatch, i, 4);
      }
   }
   
   //
   // Update all items
   //
   for (int i = 0; i < numPaletteEntries; i++) {
      const PaletteEntry* pe = paletteBeingEdited->getPaletteEntry(i);
      const float value = pe->getValue();
      const int colorIndex = pe->getColorIndex();
      const PaletteColor* pc = pf->getPaletteColor(colorIndex);
      const QString name = pc->getName();
      unsigned char rgb[3];
      pc->getRGB(rgb);
      paletteEntryInsertPushButtons[i]->show();
      paletteEntryDeletePushButtons[i]->show();
      paletteEntryScalarSpinBoxes[i]->setValue(value);
      paletteEntryColorPushButtons[i]->setText(name);
      //paletteEntryColorSwatch[i]->setPaletteBackgroundColor(QColor(rgb[0], rgb[1], rgb[2]));
      QPalette pal;
      pal.setColor(QPalette::Window, QColor(rgb[0], rgb[1], rgb[2]));
      paletteEntryColorSwatch[i]->setAutoFillBackground(true);
      paletteEntryColorSwatch[i]->setBackgroundRole(QPalette::Window);
      paletteEntryColorSwatch[i]->setPalette(pal);
      paletteEntryScalarSpinBoxes[i]->show();
      paletteEntryColorPushButtons[i]->show();
      paletteEntryColorSwatch[i]->show();
   }
   
   //
   // Hide unneeded items
   //
   for (int i = numPaletteEntries; i < numValidRows; i++) {
      paletteEntryInsertPushButtons[i]->hide();
      paletteEntryDeletePushButtons[i]->hide();
      paletteEntryScalarSpinBoxes[i]->hide();
      paletteEntryColorPushButtons[i]->hide();
      paletteEntryColorSwatch[i]->hide();
   }
   
   loadingPalette = false;
   
   paletteEntryWidget->adjustSize();
}
  
/**
 * called when an add palette entry button is pressed.
 */
void 
GuiPaletteEditorDialog::slotAddPaletteEntry(int item)
{
   if (paletteBeingEdited != NULL) {
      PaletteEntry pe = *(paletteBeingEdited->getPaletteEntry(item));
      paletteBeingEdited->insertPaletteEntry(item, pe);
      slotPaletteSelectionComboBox(paletteSelectionComboBox->currentIndex());
      slotApplyButton();
   }
}

/**
 * called when a remove palette entry button is pressed.
 */
void 
GuiPaletteEditorDialog::slotRemovePaletteEntry(int item)
{
   if (paletteBeingEdited != NULL) {
      paletteBeingEdited->removePaletteEntry(item);
      slotPaletteSelectionComboBox(paletteSelectionComboBox->currentIndex());
      slotApplyButton();
   }
}

/**
 * called when palette entry color button is pressed.
 */
void 
GuiPaletteEditorDialog::slotColorPaletteEntry(int item)
{
   PaletteEntry* pe = paletteBeingEdited->getPaletteEntry(item);
   GuiPaletteColorSelectionDialog pcsd(this, theMainWindow->getBrainSet()->getPaletteFile(),
                                       pe->getColorIndex());
   if (pcsd.exec() == QDialog::Accepted) {
      const int indx = pcsd.getSelectedColorIndex();
      pe->setColorIndex(indx);
   }
   slotPaletteSelectionComboBox(paletteSelectionComboBox->currentIndex());
   slotApplyButton();
}
      
/**
 * called when new palette button pressed.
 */
void
GuiPaletteEditorDialog::slotCreateNewPaletteButton()
{
   bool ok = false;
   const QString name = QInputDialog::getText(this, "New Palette Name",
                                     "New Palette Name",
                                     QLineEdit::Normal,
                                     QString::null, &ok);
   if (ok) {
      if (name.isEmpty()) {
         QMessageBox::critical(this, "ERROR", "Palette name blank.   No palette created.");
         return;
      }
      
      PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
      
      Palette* copyThisPalette = NULL;
      const int palIndex = paletteSelectionComboBox->currentIndex();
      if (palIndex >= 0) {
         Palette* pal = pf->getPalette(palIndex);
         
         std::vector<QString> labels;
         labels.push_back("New Empty Palette");
         QString s("Copy Palette ");
         s.append(pal->getName());
         labels.push_back(s);
         QtRadioButtonSelectionDialog rbd(this,
                                          "Choose New Palette",
                                          "Choose initial colors in new palette",
                                          labels,
                                          0);
         if (rbd.exec() == QDialog::Accepted) {
            if (rbd.getSelectedItemIndex() == 1) {
               copyThisPalette = pal;
            }
         }
         else {
            return;
         }
      }
      
      if (copyThisPalette != NULL) {
         Palette p = *copyThisPalette;
         p.setName(name);
         pf->addPalette(p);
      }
      else {
         //
         // Add the missing color
         //
         const unsigned char rgb[3] = { 0, 0, 0 };
         pf->addPaletteColor(PaletteColor::missingColorName, rgb);
         
         //
         // Create the palette with two "missing" colors
         //
         Palette pal(pf);
         pal.addPaletteEntry(1.0, PaletteColor::missingColorName);
         pal.addPaletteEntry(-1.0, PaletteColor::missingColorName);
         pal.setName(name);
         
         //
         // Add the palette to the palette file
         //
         pf->addPalette(pal);
      }
      
      //
      // Note theMainWindow->fileModificationUpdate(fm); will call GuiPaletteEditor::updateDialog()
      //
      GuiFilesModified fm;
      fm.setPaletteModified();
      theMainWindow->fileModificationUpdate(fm);
      
      const int num = pf->getNumberOfPalettes();
      
      slotPaletteSelectionComboBox(num - 1);
      paletteSelectionComboBox->setCurrentIndex(num - 1);      
   }
}

/**
 * called when rename palette button pressed.
 */
void 
GuiPaletteEditorDialog::slotRenamePaletteButton()
{
   const int palIndex = paletteSelectionComboBox->currentIndex();
   if (palIndex >= 0) {
      PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
      Palette* pal = pf->getPalette(palIndex);
      bool ok = false;
      const QString newName = QInputDialog::getText(this, "Change Palette Name",
                                              "Palette Name",
                                              QLineEdit::Normal,
                                              pal->getName(),
                                              &ok);
      if (ok) {
         pal->setName(newName);
         //
         // Note theMainWindow->fileModificationUpdate(fm); will call GuiPaletteEditor::updateDialog()
         //
         GuiFilesModified fm;
         fm.setPaletteModified();
         theMainWindow->fileModificationUpdate(fm);
      }
   }
}
      
/**
 * called when delete palette button pressed.
 */
void 
GuiPaletteEditorDialog::slotDeletePaletteButton()
{
   const int palIndex = paletteSelectionComboBox->currentIndex();
   if (palIndex >= 0) {
      PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
      QString msg("Are you sure you want to delete palette named ");
      const Palette* pal = pf->getPalette(palIndex);
      msg.append(pal->getName());
      msg.append(" ?");
      if (QMessageBox::question(this, 
                                "Are You Sure", 
                                msg, 
                                (QMessageBox::Yes | QMessageBox::No),
                                QMessageBox::Yes)
                                   == QMessageBox::Yes) {
          pf->removePalette(palIndex);
        
          //
          // Note theMainWindow->fileModificationUpdate(fm); will call GuiPaletteEditor::updateDialog()
          //
          GuiFilesModified fm;
          fm.setPaletteModified();
          theMainWindow->fileModificationUpdate(fm);
       }
       slotApplyButton();
   }
}

/**
 * destructor.
 */
GuiPaletteEditorDialog::~GuiPaletteEditorDialog()
{
}

/**
 * update the dialog.
 */
void 
GuiPaletteEditorDialog::updateDialog()
{
   paletteSelectionComboBox->clear();
   PaletteFile* pf = theMainWindow->getBrainSet()->getPaletteFile();
   
   int currentItem = 0;
   
   //
   // Load the combo box
   //
   const int num = pf->getNumberOfPalettes();
   for (int i = 0; i < num; i++) {
      const Palette* pal = pf->getPalette(i);
      paletteSelectionComboBox->addItem(pal->getName());
      if (paletteBeingEdited == pal) {
         currentItem = i;
      }
   }
   
   paletteSelectionComboBox->setCurrentIndex(currentItem);
   slotPaletteSelectionComboBox(currentItem);
}
