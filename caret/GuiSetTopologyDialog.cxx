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

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiSetTopologyDialog.h"
#include "GuiTopologyFileComboBox.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiSetTopologyDialog::GuiSetTopologyDialog(QWidget* parent)
   : QtDialogNonModal(parent)
{
   setWindowTitle("Set Topology");
   
   //
   // Grid layout for coord and topology
   //
   QGridLayout* grid = new QGridLayout;
   grid->setSpacing(4);
   
   //
   // Surface selection
   //
   grid->addWidget(new QLabel("Surface"), 0, 0, Qt::AlignLeft);
   surfaceComboBox = new GuiBrainModelSelectionComboBox(false,
                                                        true,
                                                        false,
                                                        "",
                                                        0);
   surfaceComboBox->setMaximumWidth(400);
   grid->addWidget(surfaceComboBox, 0, 1, Qt::AlignLeft);
   QObject::connect(surfaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSurfaceSelection()));
   
   //
   // Topology selection
   //
   grid->addWidget(new QLabel("Topology"), 1, 0, Qt::AlignLeft);
   topologyComboBox = new GuiTopologyFileComboBox(0);
   topologyComboBox->setMaximumWidth(400);
   grid->addWidget(topologyComboBox, 1, 1, Qt::AlignLeft);
   QObject::connect(topologyComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotTopologySelection()));
   
   //
   // layout for entire dialog
   //
   QVBoxLayout* layout = getDialogLayout();
   layout->addLayout(grid);
   
   //
   // hookup close pushbutton
   //
   getApplyPushButton()->hide();
   QObject::connect(getClosePushButton(), SIGNAL(clicked()),
                    this, SLOT(close()));
   
   surfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainModelSurface());                                                        
   slotSurfaceSelection();
}

/**
 * update the dialog.
 */
void 
GuiSetTopologyDialog::updateDialog()
{
   surfaceComboBox->updateComboBox();
   topologyComboBox->updateComboBox();
}

/**
 * Called when a surface is selected.
 */
void
GuiSetTopologyDialog::slotSurfaceSelection()
{
   //
   // Set topology file to one used by this surface.
   //
   BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      const TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfTopologyFiles(); i++) {
            if (tf == theMainWindow->getBrainSet()->getTopologyFile(i)) {
               topologyComboBox->setCurrentIndex(i);
               break;
            }
         }
      }
   }
}

/**
 * Called when a topology is selected.
 */
void
GuiSetTopologyDialog::slotTopologySelection()
{
   //
   // Set the surface's topology file
   //
   BrainModelSurface* bms = surfaceComboBox->getSelectedBrainModelSurface();
   if (bms != NULL) {
      if (topologyComboBox->getSelectedTopologyFile() != bms->getTopologyFile()) {
         TopologyFile* oldTopologyFile = bms->getTopologyFile();
         if (bms->setTopologyFile(topologyComboBox->getSelectedTopologyFile())) {
            std::ostringstream str;
            str << "Topology File "
                << FileUtilities::basename(bms->getTopologyFile()->getFileName()).toAscii().constData()
                << "\n is not for use with coordinate file "
                << FileUtilities::basename(bms->getCoordinateFile()->getFileName()).toAscii().constData()
                << ".\n  Topo file has tiles with node numbers exceeding \n"
                << "the number of coordinates in the coordinate file.";
            bms->setTopologyFile(oldTopologyFile);
            GuiMessageBox::critical(this, "ERROR", str.str().c_str(), "OK");
            return;
         }
         GuiFilesModified fm;
         fm.setCoordinateModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL(NULL);
      }
   }
}

/**
 * Destructor.
 */
GuiSetTopologyDialog::~GuiSetTopologyDialog()
{
}

