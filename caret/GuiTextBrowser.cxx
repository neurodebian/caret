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

#include "GuiMainWindow.h"
#include "GuiTextBrowser.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiTextBrowser::GuiTextBrowser(QWidget* parent)
   : QTextBrowser(parent)
{
}

/**
 * destructor.
 */
GuiTextBrowser::~GuiTextBrowser()
{
}

/**
 * intercept http web pages.
 */
void 
GuiTextBrowser::setSource(const QUrl& name)
{
   const QString pageName(name.toString());
   if (pageName.startsWith("http://")) {
      setHtml("<HTML>The help page requested ("
              + pageName +
              ") is a web site and "
              "will be displayed in your web browser</HTML>");
      theMainWindow->displayWebPage(pageName);
      return;
   }
   
   QTextBrowser::setSource(name);
}

