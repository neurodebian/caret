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


#include <QLayout>
#include <QPushButton>

#include "BrainModel.h"
#include "GuiBrainModelViewingWindow.h"
#include "GuiMainWindow.h"
#include "GuiToolBar.h"
#include "global_variables.h"

/**
 * Viewing Window Constructor.
 */
GuiBrainModelViewingMainWindow::GuiBrainModelViewingMainWindow(QWidget* /*parent*/, 
                                     GuiMainWindow* mainWindowIn,
                                     const BrainModel::BRAIN_MODEL_VIEW_NUMBER svn,
                                     const char* /*name*/)
      : QtMainWindow(0)
{
   
   //
   // Create the openGL area
   // Pass the main window's OpenGL widget so that display lists are shared 
   //
   brainModelOpenGL = new GuiBrainModelOpenGL(this, 
                                              mainWindowIn->getBrainModelOpenGL(),
                                              "brainModelOpenGL", svn);
      
   setCentralWidget(brainModelOpenGL);

   //
   // Create the toolbar
   //
   toolBar = new GuiToolBar(this, mainWindowIn, brainModelOpenGL, false);
   addToolBar(toolBar);
}

/**
 * Main Window Destructor.
 */
GuiBrainModelViewingMainWindow::~GuiBrainModelViewingMainWindow()
{
}

/**
 * initialize the toolbar.  Performed after window is created so that the toolbar 
 * is created with a small model selection combo box that is expandable.
 */
void 
GuiBrainModelViewingMainWindow::initializeToolBar()
{
   toolBar->loadModelComboBox();
   toolBar->updateViewControls();
}

/*--------------------------------------------------------------------------------------*/
/**
 * The Constructor.
 */
GuiBrainModelViewingWindow::GuiBrainModelViewingWindow(QWidget* parent,
                           GuiMainWindow* caretMainWindowIn,
                           const BrainModel::BRAIN_MODEL_VIEW_NUMBER svn)
   : QtDialog(parent, false)
{
   setAttribute(Qt::WA_DeleteOnClose);
   
   mainWindow = caretMainWindowIn;
   windowNumber = svn;
   
   setWindowTitle(QString("Viewing Window %1").arg(svn+1));
   
   //
   // Layout for the dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setSpacing(5);
   
   //
   // Create the viewing main window in this dialog
   //
   viewingMainWindow = new GuiBrainModelViewingMainWindow(this,
                                                      caretMainWindowIn,
                                                      svn);
   this->resize(400, 300);
   viewingMainWindow->setMinimumSize(200, 200);
   dialogLayout->addWidget(viewingMainWindow);
   
   //
   // Close Button
   //
   QHBoxLayout* buttons = new QHBoxLayout;
   dialogLayout->addLayout(buttons);
   
   QPushButton* close = new QPushButton("Close", this);
   close->setAutoDefault(false);
   close->setFixedSize(close->sizeHint());
   QObject::connect(close, SIGNAL(clicked()),
                    this, SLOT(close()));
   buttons->addWidget(close);
}

/**
 * The Destructor.
 */
GuiBrainModelViewingWindow::~GuiBrainModelViewingWindow()
{
   mainWindow->removeViewingWindow(windowNumber);
}

/**
 * initialize the toolbar.  Performed after window is created so that the toolbar 
 * is created with a small model selection combo box that is expandable.
 */
void 
GuiBrainModelViewingWindow::initializeToolBar()
{
   viewingMainWindow->initializeToolBar();
}

/**
 * display the brain model in the window.
 */
void 
GuiBrainModelViewingWindow::displayBrainModelInWindow(BrainModel* bm)
{
   GuiToolBar* toolbar = viewingMainWindow->getToolBar();
   if (toolbar != NULL) {
      toolbar->setModelSelection(bm->getBrainModelIndex());
   }
}      
      
