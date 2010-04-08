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

#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QTime>

#include "BrainModelSurface.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowTimingActions.h"
#include "TopologyFile.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowTimingActions::GuiMainWindowTimingActions(GuiMainWindow* parent) :
   QObject(parent)
{
   viewTimingTestAction = new QAction(parent);
   viewTimingTestAction->setObjectName("viewTimingTestAction");
   viewTimingTestAction->setText("Timing Test");
   QObject::connect(viewTimingTestAction, SIGNAL(triggered(bool)),
                    this, SLOT(viewMenuTimingTest()));
}

/**
 * Destructor.
 */
GuiMainWindowTimingActions::~GuiMainWindowTimingActions()
{
}

/**
 * Called for running a timing test.
 */
void
GuiMainWindowTimingActions::viewMenuTimingTest()
{
   const int numTimes = 20;
   QTime timer;
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   timer.start();
   for (int i = 0; i < numTimes; i++) {
      GuiBrainModelOpenGL::updateAllGL(theMainWindow->getBrainModelOpenGL());
   }
   
   const float milliseconds = static_cast<float>(timer.elapsed())
                            / static_cast<float>(numTimes); 
   const float fps = 1000.0 / milliseconds;
   std::ostringstream str;
   str << "Average time: " << milliseconds << " milliseconds per frame\n"
       << "Frames per second: " << fps << "\n";

   const BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      const TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         str << "Number of triangles: " << tf->getNumberOfTiles() << "\n";
      }
   }
   
   str << std::ends;
   QApplication::restoreOverrideCursor();
   QMessageBox::information(theMainWindow, "Timing", str.str().c_str());
}

/**
 * update the actions (typically called when menu is about to show)
 */
void 
GuiMainWindowTimingActions::updateActions()
{
}

