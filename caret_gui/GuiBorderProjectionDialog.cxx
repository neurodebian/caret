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
#include <QButtonGroup>
#include <QGroupBox>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>

#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBorderProjectionDialog.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "QtUtilities.h"

#include "global_variables.h"

/**
 * Constructor
 */
GuiBorderProjectionDialog::GuiBorderProjectionDialog(QWidget* parent) 
   : WuQDialog(parent)
{
   setModal(true);
   setWindowTitle("Border Projection");
   
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(5);
   
   //
   // Projection method buttons
   //
   QGroupBox* projGroupBox = new QGroupBox("Projection Method");
   dialogLayout->addWidget(projGroupBox);
   nearestNodeButton = new QRadioButton("Nearest Node");
   nearestTileButton = new QRadioButton("Nearest Tile");
   QVBoxLayout* projGroupLayout = new QVBoxLayout(projGroupBox);
   projGroupLayout->addWidget(nearestNodeButton);
   projGroupLayout->addWidget(nearestTileButton);
   
   QButtonGroup* buttGroup = new QButtonGroup(this);
   buttGroup->addButton(nearestNodeButton);
   buttGroup->addButton(nearestTileButton);
   nearestTileButton->setChecked(true);
                                        
   //
   // Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   buttonsLayout->setSpacing(5);
   
   QPushButton* ok = new QPushButton("OK");
   QObject::connect(ok, SIGNAL(clicked()),
                    this, SLOT(accept()));
   buttonsLayout->addWidget(ok);
   
   QPushButton* close = new QPushButton("Cancel");
   QObject::connect(close, SIGNAL(clicked()),
                    this, SLOT(reject()));
   buttonsLayout->addWidget(close);
   
   QtUtilities::makeButtonsSameSize(ok, close);   
}

/**
 * Destructor
 */
GuiBorderProjectionDialog::~GuiBorderProjectionDialog()
{
}

/**
 * called by QDialog when accept/reject signal is emitted.
 */
void
GuiBorderProjectionDialog::done(int r)
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   if (r == QDialog::Accepted) {
      GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
      BrainModelSurface* bms = openGL->getDisplayedBrainModelSurface();
      if (bms != NULL) {
         //
         // Project borders used by this surface
         //
         BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
         bmbs->projectBorders(bms, nearestTileButton->isChecked());
         
         //
         // Notify that borders have been changed.
         //
         GuiFilesModified fm;
         fm.setBorderModified();
         theMainWindow->fileModificationUpdate(fm);

         //
         // Update all displayed surfaces
         //
         GuiBrainModelOpenGL::updateAllGL(NULL);
      }
   }

   //
   // Call parent' method to close dialog.
   //
   QDialog::done(r);
   
   QApplication::restoreOverrideCursor();
}

