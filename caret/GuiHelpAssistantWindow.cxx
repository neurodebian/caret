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

#include <QFileInfo>
#include <QMessageBox>

#include "BrainSet.h"
#include "GuiHelpAssistantWindow.h"
#include "GuiMainWindow.h"
#include "global_variables.h"

/**
 * conostructor.
 */
GuiHelpAssistantWindow::GuiHelpAssistantWindow(const QString& path,
                                               QObject* parent)
   : QAssistantClient(path, parent)
{
   QObject::connect(this, SIGNAL(error(const QString&)),
                    this, SLOT(showError(const QString&)));
}
                       
/**
 * destructor.
 */
GuiHelpAssistantWindow::~GuiHelpAssistantWindow()
{
}

/**
 * show help page.
 */
void 
GuiHelpAssistantWindow::showPage(const QString& pageNameIn)
{
   QString pageName(pageNameIn);   
   if (pageName.isEmpty()) {
      pageName = "index.html";
   }
   
   //
   // If not absolute page, assume that file is in caret help directory
   //    
   QFileInfo fileInfo(pageName);
   if (fileInfo.isAbsolute() == false) {
      QString defaultPage(theMainWindow->getBrainSet()->getCaretHomeDirectory());
      defaultPage.append("/");
      defaultPage.append("caret5_help");
      defaultPage.append("/");
      defaultPage.append(pageName);
      pageName = defaultPage;
   }

   if (isOpen() == false) {
      openAssistant();
   }
   QAssistantClient::showPage(pageName);
}

/**
 * called if error.
 */
void 
GuiHelpAssistantWindow::showError(const QString& message)
{
   QMessageBox::critical(theMainWindow, "ERROR", message);
}
      
