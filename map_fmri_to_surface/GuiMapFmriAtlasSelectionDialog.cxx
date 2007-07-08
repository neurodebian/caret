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



#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>

#include "GuiMapFmriAtlasSelectionDialog.h"
#include "map_fmri_global_variables.h"

/** 
 * Constructor
 */
GuiMapFmriAtlasSelectionDialog::GuiMapFmriAtlasSelectionDialog(QWidget* parent, 
                                                               AtlasSurfaceDirectoryFile* asdf)
   : QDialog(parent, "GuiMapFmriAtlasSelectionDialog", true)
{
   atlasDirectory = asdf;
   
   setCaption("Select Atlas Surface(s) for Mapping");

   QVBoxLayout* layout = new QVBoxLayout(this, 5, 5, "layout");
   
   //
   // Vertical Groupbox for atlas selection
   //
   QVGroupBox* atlasGroupBox = new QVGroupBox("Atlas Selection", this, "atlasGroupBox");
   layout->addWidget(atlasGroupBox);
   
   //
   // Place the atlas information in a structure
   //
   for (int i = 0; i < asdf->getNumberOfAtlasSurfaces(); i++) {
      AtlasSurface* as = asdf->getAtlasSurface(i);
      const std::string atlasName = as->getAtlasName();

      int atlasIndex = -1;
      for (int j = 0; j < (int)availableAtlases.size(); j++) {
         if (availableAtlases[j].atlasName.compare(atlasName) == 0) {
            atlasIndex = j;
            break;
         }    
      }
      if (atlasIndex < 0) {
         AvailableAtlases aa(atlasName);
         availableAtlases.push_back(aa);
         atlasIndex = availableAtlases.size() - 1;
      }

      switch(as->getAnatomyType()) {
         case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
            availableAtlases[atlasIndex].cerebralLeftIndex = i;
            break;
         case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
            availableAtlases[atlasIndex].cerebralRightIndex = i;
            break;
         case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
            availableAtlases[atlasIndex].cerebellumIndex = i;
            break;
         default:
            break;
      }
   }
   
   int defaultAtlas = 0;
   
   //
   // Combo box for atlases
   //
   atlasSelectionComboBox = new QComboBox(atlasGroupBox, "atlasSelectionComboBox");
   QObject::connect(atlasSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotAtlasSelection(int)));
   for (unsigned int k = 0; k < availableAtlases.size(); k++) {
      if (availableAtlases[k].atlasName == fmriMapper.getLastSelectedAtlas()) {
         defaultAtlas = k;
      }
      atlasSelectionComboBox->insertItem(availableAtlases[k].atlasName.c_str());
   }
   
   //
   // Vertical Groupbox for anatomical surface selection
   //
   QVGroupBox* surfaceGroupBox = new QVGroupBox("Anatomical Surface", this, "surfaceGroupBox");
   layout->addWidget(surfaceGroupBox);
   
   //
   // surface selection check buttons
   //
   leftHemisphereCheckBox = new QCheckBox("Cerebral Left", surfaceGroupBox, 
                                          "leftHemisphereCheckBox");
   rightHemisphereCheckBox = new QCheckBox("Cerebral Right", surfaceGroupBox, 
                                          "rightHemisphereCheckBox");
   cerebellumCheckBox = new QCheckBox("Cerebellum", surfaceGroupBox, 
                                          "cerebellumCheckBox");
   
   //
   // Dialog button(s)
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout(layout, 5, "buttonsLayout");
   QPushButton* okButton = new QPushButton("OK", this);
   okButton->setFixedSize(okButton->sizeHint());
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   atlasSelectionComboBox->setCurrentItem(defaultAtlas);
   slotAtlasSelection(defaultAtlas);
}

/** 
 * Destructor
 */
GuiMapFmriAtlasSelectionDialog::~GuiMapFmriAtlasSelectionDialog()
{
}

/**
 * Called when users closes dialog.
 */
void
GuiMapFmriAtlasSelectionDialog::done(int r)
{
   if (r == Accepted) {
      std::vector<int> selections;
      getSelectedAtlas(selections);
      
      if (selections.size() == 0) {
         if (QMessageBox::warning(this, "No Atlas Selection",
                                  "You have not selected any surfaces!",
                                  "OK", "Change Selections") != 0) {
            return;
         }
      }
   }
   QDialog::done(r);
}

/**
 * Called when an atlas is selected.
 */
void
GuiMapFmriAtlasSelectionDialog::slotAtlasSelection(int item)
{
   leftHemisphereCheckBox->setEnabled(false);
   rightHemisphereCheckBox->setEnabled(false);
   cerebellumCheckBox->setEnabled(false);
   if (item < static_cast<int>(availableAtlases.size())) {
      const AvailableAtlases& aa = availableAtlases[item];
      if (aa.cerebralLeftIndex >= 0) {
         leftHemisphereCheckBox->setEnabled(true);
      }
      if (aa.cerebralRightIndex >= 0) {
         rightHemisphereCheckBox->setEnabled(true);
      }
      if (aa.cerebellumIndex >= 0) {
         cerebellumCheckBox->setEnabled(true);
      }
      fmriMapper.setLastSelectedAtlas(aa.atlasName);
   }
}

/*
 * Get the selected atlas index.
 */
void 
GuiMapFmriAtlasSelectionDialog::getSelectedAtlas(std::vector<int>& atlasesSelected) const
{
   atlasesSelected.clear();
   
   const int atlasIndex = atlasSelectionComboBox->currentItem();
   const AvailableAtlases& aa = availableAtlases[atlasIndex];
   if (leftHemisphereCheckBox->isEnabled()) {
      if (leftHemisphereCheckBox->isChecked()) {
         atlasesSelected.push_back(aa.cerebralLeftIndex);
      }
   }
   if (rightHemisphereCheckBox->isEnabled()) {
      if (rightHemisphereCheckBox->isChecked()) {
         atlasesSelected.push_back(aa.cerebralRightIndex);
      }
   }
   if (cerebellumCheckBox->isEnabled()) {
      if (cerebellumCheckBox->isChecked()) {
         atlasesSelected.push_back(aa.cerebellumIndex);
      }
   }
}
