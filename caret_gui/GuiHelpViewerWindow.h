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

#include <QPair>
#include <QVector>

#include "WuQDialog.h"

class GuiTextBrowser;
class QLineEdit;
class QSplitter;
class QToolButton;
class QTreeWidget;
class QTreeWidgetItem;

class GuiHelpViewerMainWindow;

/// dialog for showing help in a browser window
class GuiHelpViewerWindow : public WuQDialog {
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
      
   protected slots:
      // called when an index tree item is clicked
      void indexTreeItemSelected(QTreeWidgetItem* item, int column);
      
      // called when a search tree item is clicked
      void searchTreeItemSelected(QTreeWidgetItem* item, int column);
      
      // called to print currently displayed page
      void slotPrint();
      
      // called to find in browser window
      void slotFindInBrowser();
      
      // called to find next in browser window
      void slotFindNextInBrowser();
         
      // called to search all help pages
      void slotSearchLineEdit();
      
   protected:
      // create the help widget
      QWidget* createHelpBrowser(const bool showNavigationControlsFlag);
       
      // load the index tree
      void loadIndexTree();
      
      // create a tree widget item
      QTreeWidgetItem* createTreeItem(const QString& label, const QString& helpPage = "");
      
      // get all web page names and titles
      void getAllWebPages(QVector<QPair<QString,QString> >& pagesOut) const;
      
      /// the help browser
      GuiTextBrowser* helpBrowser;
      
      /// the splitter
      QSplitter* splitter;
      
      /// the index tree widget
      QTreeWidget* indexTreeWidget;
      
      /// the search tree widget
      QTreeWidget* searchTreeWidget;
      
      /// text when searching browser
      QString findInBrowserText;
      
      /// find next toolbutton
      QToolButton* findNextPushButton;
      
      /// line edit for searching web pages
      QLineEdit* searchLineEdit;
};

#endif // __GUI_HELP_VIEWER_DIALOG_H__

