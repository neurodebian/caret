
#ifndef __WU_QMULTI_PAGE_DIALOG_H__
#define __WU_QMULTI_PAGE_DIALOG_H__

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

#include <QVector>

#include "WuQDialog.h"

class QComboBox;
class QDialogButtonBox;
class QFrame;
class QHBoxLayout;
class QStackedWidget;
class QToolBar;
class QToolButton;

/// page for generic multipage dialog
class WuQMultiPageDialogPage : public QWidget {
   Q_OBJECT
   
   public:
      // constructor
      WuQMultiPageDialogPage();
                         
      // destructor
      virtual ~WuQMultiPageDialogPage();
      
      // apply the pages settings
      virtual void applyPage() = 0;
      
      // create the page
      virtual QWidget* createPage() = 0;
                              
      // update the page
      virtual void updatePage() = 0;

      // see if the page is valid
      virtual bool getPageValid() const = 0;
      
   protected:
};

/// generic multi-page dialog
class WuQMultiPageDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// page creation time
      enum PAGE_CREATION {
         /// create when first displayed
         PAGE_CREATION_WHEN_DISPLAYED,
         /// create immediately
         PAGE_CREATION_IMMEDIATELY
      };
      
      // constructor
      WuQMultiPageDialog(const PAGE_CREATION pageCreationIn,
                         const int numberOfToolBarsIn,
                         QWidget* parent = 0,
                         Qt::WindowFlags flags = 0);
                         
      // destructor
      virtual ~WuQMultiPageDialog();
      
      // update the dialog
      void updateDialog();
      
      // add a page
      void addPage(const QString& pageName,
                   WuQMultiPageDialogPage* page);

      // create and add a tool button to the dialog
      QToolButton* addToolButton(const int toolBarNumber,
                                 const QString& buttonText,
                                 const QString& toolTipText,
                                 const QObject* receiver,
                                 const char* member);
                         
      // create a tool button
      QToolButton* createToolButton(const QString& buttonText,
                                    const QString& toolTipText,
                                    const QObject* receiver,
                                    const char* member);
                         
       // add a widget to a toolbar
      void addWidgetToToolBar(const int toolBarNumber,
                              QWidget* widget);
                              
      // show a page
      void showPage(WuQMultiPageDialogPage* page);
      
      // show the dialog
      virtual void show();
   
   public slots:
      /// called to close dialog
      virtual bool close();
      
   protected slots:
      /// called when page back tool button clicked
      void slotPageBackToolButtonClicked();
      
      /// called when page forward tool button clicked
      void slotPageFwdToolButtonClicked();
      
      /// called when page selection combo box selection is made
      void slotPageSelectionComboBox(int);
      
      /// called when apply button is clicked
      void slotApplyButtonClicked();
      
   protected:
      /// class for data on each page
      class PageInfo {
         public:
            /// constructor
            PageInfo(const QString& nameIn,
                     WuQMultiPageDialogPage* dataPageIn) {
               name = nameIn;
               dataPage = dataPageIn;
               pageComboBoxIndex = -1;
               pageWidget = NULL;
            }
            
            /// destructor
            ~PageInfo() { }
                 
            /// name of page
            QString name;
            
            /// index of page in combo box
            int pageComboBoxIndex;
            
            /// the page
            WuQMultiPageDialogPage* dataPage;
            
            /// the page's widget
            QWidget* pageWidget;
      };
      
      // set the default page
      void setDefaultPage(WuQMultiPageDialogPage* defaultPageIn);

      /// get currently displayed page
      PageInfo* getDisplayedPage();
      
      /// update the page selection combo box
      void updatePageSelectionComboBox();
      
      /// show a page
      void showPage(PageInfo* p,
                    const bool updatePagesVisited);
      
      /// when to create pages
      const PAGE_CREATION pageCreation;
      
      /// the pages
      QVector<PageInfo*> pagesVector; 
      
      /// pages that have been visited
      QVector<PageInfo*> pagesVisited;
      
      /// index into pages visited
      int pagesVisitedIndex;
      
      /// the toolbar layout
      QVector<QHBoxLayout*> toolBarLayouts;
      
      /// page selection combo box
      QComboBox* pageSelectionComboBox;
      
      /// page back tool button
      QToolButton* pageBackToolButton;
      
      /// page fwd tool button
      QToolButton* pageForwardToolButton;
      
      /// frame for operations pages
      QFrame* pagesFrameWidget;
      
      /// stacked widget containing pages
      QStackedWidget* pagesStackedWidget;
      
      /// dialog's button box
      QDialogButtonBox* dialogButtonBox;
      
      /// first time dialog shown
      bool firstTimeDialogShown;
      
      /// the default page
      WuQMultiPageDialogPage* defaultPage;
      
};

#endif // __WU_QMULTI_PAGE_DIALOG_H__

