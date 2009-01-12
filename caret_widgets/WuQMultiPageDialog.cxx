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

#include <iostream>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>

#include "WuQMultiPageDialog.h"
#include "WuQSeparatorLine.h"

/**
 * constructor.
 */
WuQMultiPageDialog::WuQMultiPageDialog(const PAGE_CREATION pageCreationIn,
                                       const int numberOfToolBarsIn,
                                       QWidget* parent,
                                       Qt::WindowFlags flags)
   : WuQDialog(parent, flags),
     pageCreation(pageCreationIn)
{
   defaultPage = NULL;
   firstTimeDialogShown = true;
   
   //
   // Page Selection label
   //
   QLabel* pageSelectionLabel = new QLabel("Page Selection");
   
   //
   // Page Selection Backward tool button
   //
   pageBackToolButton = new QToolButton;
   pageBackToolButton->setToolTip("Go back to the \n"
                                  "previous page.");
   pageBackToolButton->setArrowType(Qt::LeftArrow);
   QObject::connect(pageBackToolButton, SIGNAL(clicked(bool)),
                    this, SLOT(slotPageBackToolButtonClicked()));
   
   //
   // Page Selection Forward tool button
   //
   pageForwardToolButton = new QToolButton;
   pageForwardToolButton->setToolTip("Go forward to the \n"
                                 "next page as a \n"
                                 "result of using \n"
                                 "the back arrow.");
   pageForwardToolButton->setArrowType(Qt::RightArrow);
   QObject::connect(pageForwardToolButton, SIGNAL(clicked(bool)),
                    this, SLOT(slotPageFwdToolButtonClicked()));
   
   
   
   //
   // Page Selection combo box
   // 
   pageSelectionComboBox = new QComboBox;
   QObject::connect(pageSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPageSelectionComboBox(int)));
                    
   //
   // Layout for page selection controls
   //
   QHBoxLayout* pageSelectionLayout = new QHBoxLayout;
   pageSelectionLayout->addWidget(pageSelectionLabel);
   pageSelectionLayout->addWidget(pageBackToolButton);
   pageSelectionLayout->addWidget(pageForwardToolButton);
   pageSelectionLayout->addWidget(pageSelectionComboBox);
   pageSelectionLayout->setStretchFactor(pageSelectionLabel, 0);
   pageSelectionLayout->setStretchFactor(pageBackToolButton, 0);
   pageSelectionLayout->setStretchFactor(pageForwardToolButton, 0);
   pageSelectionLayout->setStretchFactor(pageSelectionComboBox, 100);
   
   //
   // The ToolBar
   //
   for (int i = 0; i < numberOfToolBarsIn; i++) {
      toolBarLayouts.push_back(new QHBoxLayout);
   }
   
   //
   // Stacked widget for pages
   //
   pagesStackedWidget = new QStackedWidget;
   
   //
   // Dialog button box
   //
   dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Apply |
                                          QDialogButtonBox::Close);
   QObject::connect(dialogButtonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
                    this, SLOT(slotApplyButtonClicked()));
   QObject::connect(dialogButtonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
                    
   //
   // Layout for page controls and widgets
   //
   WuQSeparatorLine* sepLine = new WuQSeparatorLine(Qt::Horizontal, 3);
   pagesFrameWidget = new QFrame;
   pagesFrameWidget->setFrameStyle(QFrame::Box | QFrame::Plain);
   pagesFrameWidget->setLineWidth(1);
   QVBoxLayout* operationsLayout = new QVBoxLayout(pagesFrameWidget);
   operationsLayout->addLayout(pageSelectionLayout);
   operationsLayout->addWidget(sepLine);
   operationsLayout->addWidget(pagesStackedWidget);
   operationsLayout->setStretchFactor(pageSelectionLayout, 0);
   operationsLayout->setStretchFactor(sepLine, 0);
   operationsLayout->setStretchFactor(pagesStackedWidget, 100);
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   for (int i = 0; i < numberOfToolBarsIn; i++) {
      dialogLayout->addLayout(toolBarLayouts[i]);
   }
   dialogLayout->addWidget(pagesFrameWidget);
   dialogLayout->addWidget(dialogButtonBox);
   for (int i = 0; i < numberOfToolBarsIn; i++) {
      dialogLayout->setStretchFactor(toolBarLayouts[i], 0);
   }
   dialogLayout->setStretchFactor(pagesFrameWidget, 100);
   dialogLayout->setStretchFactor(dialogButtonBox, 0);

   pagesVisitedIndex = -1;
}
                   
/**
 * destructor.
 */ 
WuQMultiPageDialog::~WuQMultiPageDialog()
{
   for (int i = 0; i < pagesVector.size(); i++) {
      delete pagesVector[i];
      pagesVector[i] = NULL;
   }
   pagesVector.clear();
}

/**
 * called when apply button is clicked.
 */
void 
WuQMultiPageDialog::slotApplyButtonClicked()
{
   PageInfo* p = getDisplayedPage();
   p->dataPage->applyPage();
}
      
/**
 * called to close dialog.
 */
bool 
WuQMultiPageDialog::close()
{
   return WuQDialog::close();
}
      
/**
 * get currently displayed page.
 */
WuQMultiPageDialog::PageInfo* 
WuQMultiPageDialog::getDisplayedPage()
{
   const QWidget* w = pagesStackedWidget->currentWidget();
   PageInfo* p = NULL;
   for (int i = 0; i < pagesVector.count(); i++) {
      if (pagesVector[i]->pageWidget == w) {
         p = pagesVector[i];
         break;
      }
   }
   return p;
}
      
/**
 * update the dialog.
 */
void 
WuQMultiPageDialog::updateDialog()
{
/*
   const int num = pagesVector.size();
   for (int i = 0; i < num; i++) {
      pagesVector[i]->dataPage->updatePage();
   }
*/
   PageInfo* currentPage = getDisplayedPage();
   if (currentPage != NULL) {
      currentPage->dataPage->updatePage();
   }
   updatePageSelectionComboBox();
}

/**
 * update the page selection combo box.
 */
void 
WuQMultiPageDialog::updatePageSelectionComboBox()
{
   PageInfo* currentPage = getDisplayedPage();
   
   pageSelectionComboBox->blockSignals(true);
   pageSelectionComboBox->clear();
   
   PageInfo* firstValidPage = NULL;
   for (int i = 0; i < pagesVector.size(); i++) {
      PageInfo* p = pagesVector[i];
      if (p->dataPage->getPageValid()) {
         p->pageComboBoxIndex = i;
         pageSelectionComboBox->addItem(p->name);
         if (firstValidPage == NULL) {
            firstValidPage = p;
         }
      }
      else if (p == currentPage) {
         currentPage = NULL;
      }
   }

   pageSelectionComboBox->blockSignals(false);
   
   if (currentPage == NULL) {
      currentPage = firstValidPage;
   }

   showPage(currentPage, true);
}
      
/**
 * called when page back tool button clicked.
 */
void 
WuQMultiPageDialog::slotPageBackToolButtonClicked()
{
   pagesVisitedIndex--;
   if ((pagesVisitedIndex >= 0) &&
       (pagesVisitedIndex < pagesVisited.size())) {
      showPage(pagesVisited[pagesVisitedIndex], false);
   }
}

/**
 * called when page forward tool button clicked.
 */
void 
WuQMultiPageDialog::slotPageFwdToolButtonClicked()
{
   pagesVisitedIndex++;
   if ((pagesVisitedIndex >= 0) &&
       (pagesVisitedIndex < pagesVisited.size())) {
      showPage(pagesVisited[pagesVisitedIndex], false);
   }
}

/**
 * called when page selection combo box selection is made.
 */
void 
WuQMultiPageDialog::slotPageSelectionComboBox(int item)
{
   if (item >= 0) {
      for (int i = 0; i < pagesVector.count(); i++) {
         if (item == pagesVector[i]->pageComboBoxIndex) {
            showPage(pagesVector[i], true);
         }
      }
   }
}

/**
 * create and add a tool button to the dialog.
 */
QToolButton* 
WuQMultiPageDialog::addToolButton(const int toolBarNumber,
                                  const QString& buttonText,
                                  const QString& toolTipText,
                                  const QObject* receiver,
                                  const char* member)
{
   if ((toolBarNumber < 0) ||
       (toolBarNumber >= toolBarLayouts.size())) {
      std::cout << "ERROR WuQMultiPageDialog::addToolButton: "
                << "ToolBar number invalid."
                << std::endl;
      return NULL;
   }

/*   
   QToolButton* button = new QToolButton;
   button->setText(buttonText);
   button->setToolTip(toolTipText);
   QObject::connect(button, SIGNAL(clicked()),
                    receiver, member);
*/
   QToolButton* button = createToolButton(buttonText,
                                          toolTipText,
                                          receiver,
                                          member);
   addWidgetToToolBar(toolBarNumber,
                      button);
   
   return button;
}                         

/**
 * Create a tool button.
 */
QToolButton* 
WuQMultiPageDialog::createToolButton(const QString& buttonText,
                                     const QString& toolTipText,
                                     const QObject* receiver,
                                     const char* member)
{
   QToolButton* button = new QToolButton;
   button->setText(buttonText);
   button->setToolTip(toolTipText);
   QObject::connect(button, SIGNAL(clicked()),
                    receiver, member);
   
   return button;
}                         

/**
 * add a widget to a toolbar.
 */
void 
WuQMultiPageDialog::addWidgetToToolBar(const int toolBarNumber,
                                       QWidget* widget)
{
   if ((toolBarNumber < 0) ||
       (toolBarNumber >= toolBarLayouts.size())) {
      std::cout << "ERROR WuQMultiPageDialog::addWidgetToToolBar: "
                << "ToolBar number invalid."
                << std::endl;
      return;
   }

   toolBarLayouts[toolBarNumber]->addWidget(widget);
}
                              
/**
 * add a page.
 */
void 
WuQMultiPageDialog::addPage(const QString& pageName,
                            WuQMultiPageDialogPage* page)
{

   
   PageInfo* p = new PageInfo(pageName, page);
   switch (pageCreation) {
      case PAGE_CREATION_WHEN_DISPLAYED:
         break;
      case PAGE_CREATION_IMMEDIATELY:
         p->pageWidget = page->createPage();
         pagesStackedWidget->addWidget(p->pageWidget);
         break;
   }
                              
   pagesVector.push_back(p);
}

/**
 * show a page.
 */
void 
WuQMultiPageDialog::showPage(WuQMultiPageDialogPage* page)
{
   const int num = pagesVector.size();
   for (int i = 0; i < num; i++) {
      if (pagesVector[i]->dataPage == page) {
         showPage(pagesVector[i], true);
         break;
      }
   }
}      

/**
 * show a page.
 */
void 
WuQMultiPageDialog::showPage(PageInfo* p,
                             const bool updatePagesVisited)
{
   if (p != NULL) {
      //
      // Create the page, if needed
      //
      if (p->pageWidget == NULL) {
         p->pageWidget = p->dataPage->createPage();
         pagesStackedWidget->addWidget(p->pageWidget);
      }
      
      //
      // Update the page
      //
      p->dataPage->updatePage();
      
      //
      // Show the page
      //
      pagesStackedWidget->setCurrentWidget(p->pageWidget);
      
      //
      // Set the page selection combo box to the current page
      //
      pageSelectionComboBox->blockSignals(true);
      pageSelectionComboBox->setCurrentIndex(p->pageComboBoxIndex);
      pageSelectionComboBox->blockSignals(false);   

      if (updatePagesVisited) {
         // 
         // Remove any pages "forward" of the current page index
         //    
         const int numPages = static_cast<int>(pagesVisited.size());
         if ((pagesVisitedIndex >= 0) &&
             (pagesVisitedIndex < (numPages - 1))) {
            pagesVisited.erase(pagesVisited.begin() + pagesVisitedIndex + 1,
                               pagesVisited.end());
         }

         //
         // Add new page
         //
         pagesVisited.push_back(p);
         pagesVisitedIndex = static_cast<int>(pagesVisited.size() - 1);
      }
      
      //
      // Make dialog its minimum size so that no space is empty
      //
      pagesStackedWidget->setFixedSize(p->pageWidget->sizeHint());
      pagesFrameWidget->setFixedSize(pagesFrameWidget->sizeHint());
      setFixedSize(sizeHint());
   }
   else {
      std::cout << "PROGRAM ERROR: No page to show in WuQMultiPageDialog::showPage" << std::endl;
   }
   
   //
   // Enable/Disable back and forward buttons
   //
   pageBackToolButton->setEnabled(pagesVisitedIndex > 0);
   pageForwardToolButton->setEnabled(pagesVisitedIndex
                                     < static_cast<int>((pagesVisited.size() - 1)));
}      
      
/**
 * show the dialog.
 */
void 
WuQMultiPageDialog::show()
{
   if (firstTimeDialogShown) {
      firstTimeDialogShown = false;
      for (int i = 0; i < toolBarLayouts.size(); i++) {
         toolBarLayouts[i]->addStretch();
      }
      
      if (defaultPage != NULL) {
         showPage(defaultPage);
      }
   }
   WuQDialog::show();
   updateDialog();
}

/**
 * set the default page.
 */
void 
WuQMultiPageDialog::setDefaultPage(WuQMultiPageDialogPage* defaultPageIn)
{
   defaultPage = defaultPageIn;
}

//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
WuQMultiPageDialogPage::WuQMultiPageDialogPage()
{
}

/**
 * destructor.
 */ 
WuQMultiPageDialogPage::~WuQMultiPageDialogPage()
{
}

