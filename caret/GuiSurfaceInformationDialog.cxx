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

#include <sstream>

#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiStructureComboBox.h"
#include "GuiMainWindow.h"
#include "GuiSurfaceInformationDialog.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiSurfaceInformationDialog::GuiSurfaceInformationDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Surface Information");
   
   //
   // layout for entire dialog
   //
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(3);
   rows->setSpacing(5);
   
   //
   // Surface selection
   //
   QLabel* surfaceLabel = new QLabel("Surface");
   surfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                        true,
                                                        false,
                                                        "",
                                                        0);
                                                        
   surfaceComboBox->setMaximumWidth(400);
   QObject::connect(surfaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSurfaceSelection()));
   QHBoxLayout* surfaceLayout = new QHBoxLayout;
   surfaceLayout->addWidget(surfaceLabel);
   surfaceLayout->addWidget(surfaceComboBox);
   surfaceLayout->addStretch();
   rows->addLayout(surfaceLayout);
   
   //
   // Grid layout for coord and topology
   //
   namesAndValuesGridLayout = new QGridLayout;
   namesAndValuesGridLayout->setMargin(2);
   namesAndValuesGridLayout->setSpacing(5);
   rows->addLayout(namesAndValuesGridLayout);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(5);
   rows->addLayout(buttonsLayout);
   
   //
   // OK pushbutton
   //
   QPushButton* okButton = new QPushButton("OK");
   okButton->setAutoDefault(false);
   okButton->setFixedSize(okButton->sizeHint());
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
   buttonsLayout->addWidget(okButton);
   
   surfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainModelSurface());
   slotSurfaceSelection();
}

/**
 * called when a hemisphere is selected.
 */
void 
GuiSurfaceInformationDialog::slotHemisphereComboBox()
{
   //
   // Set topology file to one used by this surface.
   //
   BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      bms->setStructure(hemisphereComboBox->getSelectedStructure());
   }
}      

/**
 * Called when a surface is selected.
 */
void
GuiSurfaceInformationDialog::slotSurfaceSelection()
{
   //
   // Set topology file to one used by this surface.
   //
   BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      std::vector<QString> labels, values;
      bms->getSurfaceInformation(labels, values);
      
      for (unsigned int i = namesLabels.size(); i < labels.size(); i++) {
         QLabel* nl = new QLabel;
         QLabel* vl = new QLabel;
         namesLabels.push_back(nl);
         valuesLabels.push_back(vl);
         namesAndValuesGridLayout->addWidget(nl, i, 0, 1, 1);
         namesAndValuesGridLayout->addWidget(vl, i, 1, 1, 1);
      }
      
      for (unsigned int i = labels.size(); i < namesLabels.size(); i++) {
         namesLabels[i]->hide();
         valuesLabels[i]->hide();
      }
      
      for (unsigned int i = 0; i < labels.size(); i++) {
         namesLabels[i]->setText(labels[i]);
         namesLabels[i]->show();
         valuesLabels[i]->setText(values[i]);
         valuesLabels[i]->show();
      }
   }
}

/**
 * Destructor.
 */
GuiSurfaceInformationDialog::~GuiSurfaceInformationDialog()
{
}

