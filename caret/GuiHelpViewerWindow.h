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


#ifndef __GUI_HELP_VIEWER_DIALOG_H__
#define __GUI_HELP_VIEWER_DIALOG_H__

#include <QMainWindow>

#include "QtDialog.h"

class GuiTextBrowser;
class QSplitter;
class QTreeWidget;
class QTreeWidgetItem;

class GuiHelpViewerMainWindow;

/// dialog for showing help in a browser window
class GuiHelpViewerWindow : public QtDialog {
   Q_OBJECT
   
   public:
      // Constructor (non-modal full navigation controls)
      GuiHelpViewerWindow(QWidget* parent);
      
      // Contructor (modal with a modal parent, use exec())
      GuiHelpViewerWindow(QDialog* parent,
                          const QString& helpPageIn);
                          
      // Destructor
      ~GuiHelpViewerWindow();
      
      // load a page into the help browser
      void loadPage(const QString& page = "");
      
   private slots:
   
   private:
      GuiHelpViewerMainWindow* helpMainWindow;
};

/// main window that is placed inside of help viewer dialog
class GuiHelpViewerMainWindow : public QMainWindow {
   Q_OBJECT
   
   public:
      // Constructor
      GuiHelpViewerMainWindow(const bool showNavigationControlsFlag);
      
      // Destructor
      ~GuiHelpViewerMainWindow();
      
      // load a page into the help browser
      void loadPage(const QString& page = "");
      
   private slots:
      // called when an tree index item is clicked
      void treeIndexItemSelected(QTreeWidgetItem* item, int column);
      
      /// called to print currently displayed page
      void slotPrint();
      
   private:
      // load the index tree
      void loadIndexTree();
      
      // create a tree widget item
      QTreeWidgetItem* createTreeItem(const QString& label, const QString& helpPage = "");
      
      /// the help browser
      GuiTextBrowser* helpBrowser;
      
      /// the splitter
      QSplitter* splitter;
      
      /// the tree widget
      QTreeWidget* indexTreeWidget;
};

#endif // __GUI_HELP_VIEWER_DIALOG_H__

