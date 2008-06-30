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

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QTextBrowser>
#include <QPixmap>
#include <QSplitter>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiHelpViewerWindow.h"
#include "GuiMainWindow.h"
#include "GuiTextBrowser.h"

#include "global_variables.h"

/**
 * Constructor (non-modal full navigation controls)
 */
GuiHelpViewerWindow::GuiHelpViewerWindow(QWidget* parent)
   : WuQDialog(parent)
{
   setModal(false);
   setWindowTitle("Caret Help");
   
   //
   // Layout for dialog.
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setSpacing(3);
   dialogLayout->setMargin(3);
   
   //
   // main window containing text browser
   //
   dialogLayout->addWidget(createHelpBrowser(true));
   
   //
   // Buttons layout.
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   buttonsLayout->addWidget(closeButton);
   
   loadPage();

/*   
   QVector<QPair<QString,QString> > pages;
   getAllWebPages(pages);
   for (int i = 0; i < pages.count(); i++) {
      std::cout << "Page: "
                << pages[i].first.toAscii().constData()
                << ", "
                << pages[i].second.toAscii().constData()
                << std::endl;
   }
*/
}

/**
 * Contructor (modal with a modal parent).
 */
GuiHelpViewerWindow::GuiHelpViewerWindow(QDialog* parent,
                                         const QString& helpPageIn)
   : WuQDialog(parent)
{
   setModal(true);
   setWindowTitle("Caret Help");
   
   //
   // Layout for dialog.
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setSpacing(3);
   dialogLayout->setMargin(3);
   
   //
   // main window containing text browser
   //
   dialogLayout->addWidget(createHelpBrowser(false));
   
   //
   // Buttons layout.
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* cancelButton = new QPushButton("Cancel");
   cancelButton->setFixedSize(cancelButton->sizeHint());
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   buttonsLayout->addWidget(cancelButton);
   
   loadPage(helpPageIn);
}
                          
/**
 * Destructor
 */
GuiHelpViewerWindow::~GuiHelpViewerWindow()
{
}

/**
 * Main Window Constructor
 */
QWidget*
GuiHelpViewerWindow::createHelpBrowser(const bool showNavigationControlsFlag)
{
   //
   // create the help browser
   //
   helpBrowser = new GuiTextBrowser;
   helpBrowser->setMinimumWidth(400);
   helpBrowser->setMinimumHeight(200);

   //
   // Layout for widget
   //
   QWidget* theHelpWidget = new QWidget;
   QVBoxLayout* widgetLayout = new QVBoxLayout(theHelpWidget);
   
   if (showNavigationControlsFlag) {
      //
      // Create the tree widget for the indices
      //
      QStringList indexTreeHeaderLabels;
      indexTreeHeaderLabels << "Help Page Index" << "Location";
      indexTreeWidget = new QTreeWidget;
      indexTreeWidget->setColumnCount(2);
      indexTreeWidget->setHeaderLabels(indexTreeHeaderLabels);
      indexTreeWidget->setColumnHidden(0, false);
      indexTreeWidget->setColumnHidden(1, true);
      QObject::connect(indexTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                       this, SLOT(indexTreeItemSelected(QTreeWidgetItem*,int)));
      loadIndexTree();
      
      //
      // Search line edit and list widget
      //
      searchLineEdit = new QLineEdit;
      searchLineEdit->setText("Enter search here");
      QObject::connect(searchLineEdit, SIGNAL(returnPressed()),
                       this, SLOT(slotSearchLineEdit()));
      QStringList searchTreeHeaderLabels;
      searchTreeHeaderLabels << "Matching Help Pages" << "Location";
      searchTreeWidget = new QTreeWidget;
      searchTreeWidget->setColumnCount(2);
      searchTreeWidget->setHeaderLabels(searchTreeHeaderLabels);
      searchTreeWidget->setColumnHidden(0, false);
      searchTreeWidget->setColumnHidden(1, true);
      QObject::connect(searchTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                       this, SLOT(searchTreeItemSelected(QTreeWidgetItem*,int)));
      QWidget* searchWidget = new QWidget;
      QVBoxLayout* searchLayout = new QVBoxLayout(searchWidget);
      searchLayout->addWidget(searchLineEdit);
      searchLayout->addWidget(searchTreeWidget);
                       
      //
      // create the back toolbar button
      //
      QAction* backwardAction = new QAction("Back", this);
      connect(helpBrowser, SIGNAL(backwardAvailable(bool)), 
              backwardAction, SLOT(setEnabled(bool)));
      connect(backwardAction, SIGNAL(triggered()),
              helpBrowser, SLOT(backward()));
      QToolButton* backwardButton = new QToolButton;
      backwardButton->setDefaultAction(backwardAction);
      
      //
      // Create the forward toolbar button
      //
      QAction* forewardAction = new QAction("Fwd", this);
      connect(helpBrowser, SIGNAL(forwardAvailable(bool)), 
              forewardAction, SLOT(setEnabled(bool)));
      connect(forewardAction, SIGNAL(triggered()),
              helpBrowser, SLOT(forward()));
      QToolButton* forwardButton = new QToolButton;
      connect(helpBrowser, SIGNAL(forwardAvailable(bool)), 
              forwardButton, SLOT(setEnabled(bool)));
      forwardButton->setDefaultAction(forewardAction);
      
      //
      // Create the home toolbar button
      // 
      QAction* homeAction = new QAction("Home", this);
      connect(homeAction, SIGNAL(triggered()),
              helpBrowser, SLOT(home()));
      QToolButton* homeButton = new QToolButton;
      homeButton->setDefaultAction(homeAction);
      
      //
      // Create the print toolbar button
      //
      QAction* printAction = new QAction("Print", this);
      connect(printAction, SIGNAL(triggered()),
              this, SLOT(slotPrint()));
      QToolButton* printButton = new QToolButton;
      printButton->setDefaultAction(printAction);
      
      //
      // Find button
      //
      QToolButton* findPushButton = new QToolButton;
      findPushButton->setText("Find");
      QObject::connect(findPushButton, SIGNAL(clicked()),
                       this, SLOT(slotFindInBrowser()));
                       
      //
      // Next button
      //
      findNextPushButton = new QToolButton;
      findNextPushButton->setText("Next");
      findNextPushButton->setEnabled(false);
      QObject::connect(findNextPushButton, SIGNAL(clicked()),
                       this, SLOT(slotFindNextInBrowser()));
                       
      //
      // Layout for toolbuttons
      //
      QHBoxLayout* toolButtonLayout = new QHBoxLayout;
      toolButtonLayout->addWidget(homeButton);
      toolButtonLayout->addWidget(backwardButton);
      toolButtonLayout->addWidget(forwardButton);
      toolButtonLayout->addWidget(printButton);
      toolButtonLayout->addWidget(findPushButton);
      toolButtonLayout->addWidget(findNextPushButton);
      toolButtonLayout->addStretch();
      
      //
      // Layout for help browser and buttons
      //
      QWidget* helpBrowserWidgets = new QWidget;
      QVBoxLayout* helpBrowserLayout = new QVBoxLayout(helpBrowserWidgets);
      helpBrowserLayout->addLayout(toolButtonLayout);
      helpBrowserLayout->addWidget(helpBrowser);
      
      //
      // Tab widget for index and search
      //
      QTabWidget* indexSearchTabWidget = new QTabWidget;
      indexSearchTabWidget->addTab(indexTreeWidget, "Index");
      indexSearchTabWidget->addTab(searchWidget, "Search");
      
      //
      // Create the splitter and add the widgets to the splitter
      //
      splitter = new QSplitter;
      splitter->setOrientation(Qt::Horizontal);
      splitter->addWidget(indexSearchTabWidget);
      splitter->addWidget(helpBrowserWidgets);
      QList<int> sizeList;
      sizeList << 175 << 375;
      splitter->setSizes(sizeList);
      
      widgetLayout->addWidget(splitter);

   }
   else {
      widgetLayout->addWidget(helpBrowser);
   }
   
   //
   // Load the default page
   //
   loadPage();
   
   return theHelpWidget;
}

/**
 * called to find in browser window.
 */
void 
GuiHelpViewerWindow::slotFindInBrowser()
{
   bool ok = false;
   const QString txt = QInputDialog::getText(this,
                                             "Find",
                                             "Find Text",
                                             QLineEdit::Normal,
                                             findInBrowserText,
                                             &ok);
   if (ok) {
      findNextPushButton->setEnabled(false);
      findInBrowserText = txt.trimmed();
      if (findInBrowserText.isEmpty() == false) {
         helpBrowser->moveCursor(QTextCursor::Start);
         if (helpBrowser->find(findInBrowserText)) {
            findNextPushButton->setEnabled(true);
         }
      }
   }
}

/**
 * called to find next in browser window.
 */
void 
GuiHelpViewerWindow::slotFindNextInBrowser()
{
   if (helpBrowser->find(findInBrowserText) == false) {
      helpBrowser->moveCursor(QTextCursor::Start);
      helpBrowser->find(findInBrowserText);
   }
}

/**
 * called to print currently displayed page.
 */
void 
GuiHelpViewerWindow::slotPrint()
{
   QPrinter printer;
   QPrintDialog* printDialog = new QPrintDialog(&printer, this);
   if (printDialog->exec() == QPrintDialog::Accepted) {
      helpBrowser->document()->print(&printer);
   }
}

/**
 * load the index tree.
 */
void 
GuiHelpViewerWindow::loadIndexTree()
{
   QTreeWidgetItem* mainItem = createTreeItem("Main Page", "index.html");
   indexTreeWidget->addTopLevelItem(mainItem);
   
   QTreeWidgetItem* installationItem = createTreeItem("Installation", "installation/caret5_installation.html");
   indexTreeWidget->addTopLevelItem(installationItem);
   
   QTreeWidgetItem* sourceCodeItem = createTreeItem("Source Code", "source_code/source_code.html");
   indexTreeWidget->addTopLevelItem(sourceCodeItem);
   
   QTreeWidgetItem* menusItem = createTreeItem("Menus");
   indexTreeWidget->addTopLevelItem(menusItem);
   menusItem->addChild(createTreeItem("caret5 (Macintosh Only)", "menus/caret5_menu.html"));
   menusItem->addChild(createTreeItem("File", "menus/file_menu.html"));
   menusItem->addChild(createTreeItem("Attributes", "menus/attributes_menu.html"));
   menusItem->addChild(createTreeItem("Layers", "menus/layers_menu.html"));
   menusItem->addChild(createTreeItem("Surface", "menus/surface_menu.html"));
   menusItem->addChild(createTreeItem("Volume", "menus/volume_menu.html"));
   menusItem->addChild(createTreeItem("Comm", "menus/comm_menu.html"));
   menusItem->addChild(createTreeItem("Window", "menus/window_menu.html"));
   menusItem->addChild(createTreeItem("Help", "menus/help_menu.html"));
   
   QTreeWidgetItem* toolbarItem = createTreeItem("Toolbars");
   indexTreeWidget->addTopLevelItem(toolbarItem);
   toolbarItem->addChild(createTreeItem("Main Window", "toolbars/main_window_toolbar.html"));
   toolbarItem->addChild(createTreeItem("Viewing Window", "toolbars/viewing_window_toolbar.html"));

   QTreeWidgetItem* dialogsItem = createTreeItem("Dialogs");
   indexTreeWidget->addTopLevelItem(dialogsItem);
   dialogsItem->addChild(createTreeItem("Capture Image of Window", "dialogs/capture_image_of_window_dialog.html"));
   dialogsItem->addChild(createTreeItem("Display Control", "dialogs/display_control_dialog.html"));
   dialogsItem->addChild(createTreeItem("Draw Border", "dialogs/draw_border_dialog.html"));
   dialogsItem->addChild(createTreeItem("File Dialogs", "dialogs/file_dialogs.html"));
   dialogsItem->addChild(createTreeItem("Metric Smoothing", "dialogs/metric_smoothing_dialog.html"));
   dialogsItem->addChild(createTreeItem("Project Cells/Foci", "dialogs/project_cells_foci_dialog.html"));
   dialogsItem->addChild(createTreeItem("Project Foci to PALS", "dialogs/project_foci_to_pals_dialog.html"));
   dialogsItem->addChild(createTreeItem("Record Main Window Images as Movie", "dialogs/record_as_mpeg_dialog.html"));
   dialogsItem->addChild(createTreeItem("Study Metadata", "dialogs/study_metadata.html"));
   dialogsItem->addChild(createTreeItem("Surface Region of Interest", "dialogs/surface_roi_dialog.html"));
   dialogsItem->addChild(createTreeItem("Surface to Volume", "dialogs/Surface_To_Volume_Dialog.html"));

   
   QTreeWidgetItem* statsItem = createTreeItem("Statistics");
   indexTreeWidget->addTopLevelItem(statsItem);
   statsItem->addChild(createTreeItem("Interhemispheric Clusters", "statistics/interhemispheric_clusters.html"));
   statsItem->addChild(createTreeItem("Normalize Data", "statistics/normalization.html"));
   statsItem->addChild(createTreeItem("One-Sample and Paired T-Test", "statistics/one_sample_t_test.html"));
   statsItem->addChild(createTreeItem("Shuffled Cross-Correlation Maps", "statistics/shuffled_cross_correlation_maps.html"));
   statsItem->addChild(createTreeItem("Subtract Group Average", "statistics/subtract_group_average.html"));
   statsItem->addChild(createTreeItem("Two Sample T-Test", "statistics/two_sample_t_test.html"));
   statsItem->addChild(createTreeItem("T-Map Shuffled", "statistics/t_map_shuffled_columns.html"));
   statsItem->addChild(createTreeItem("T-Map", "statistics/tmap.html"));
   statsItem->addChild(createTreeItem("Wilcoxon Rank-Sum", "statistics/wilcoxon_rank_sum.html"));
   statsItem->addChild(createTreeItem("Z-Map", "statistics/z_map.html"));
   
   QTreeWidgetItem* studyItem = createTreeItem("Study Metadata", "dialogs/study_metadata.html");
   indexTreeWidget->addTopLevelItem(studyItem);
   
   QTreeWidgetItem* faqsItem = createTreeItem("FAQs");
   indexTreeWidget->addTopLevelItem(faqsItem);
   faqsItem->addChild(createTreeItem("Images", "faqs/images.html"));
   faqsItem->addChild(createTreeItem("Scenes", "faqs/scenes.html"));
   
   QTreeWidgetItem* filesItem = createTreeItem("Files");
   indexTreeWidget->addTopLevelItem(filesItem);
   filesItem->addChild(createTreeItem("File Formats", "file_formats/file_formats.html"));
   filesItem->addChild(createTreeItem("File Types", "file_formats/files.html"));
   filesItem->addChild(createTreeItem("Spec File Tags", "file_formats/spec_file_tags.html"));

   QTreeWidgetItem* keysItem = createTreeItem("Keyboard Commands", "misc/keyboard_commands.html");
   indexTreeWidget->addTopLevelItem(keysItem);
   
   QTreeWidgetItem* changesItem = createTreeItem("Changes Log", "misc/change_log.html");
   indexTreeWidget->addTopLevelItem(changesItem);
   
   QTreeWidgetItem* tutorialItem = createTreeItem("Caret 5.5 Basics Tutorial", "tutorials/Caret_Tutorial_5.5.html");
   indexTreeWidget->addTopLevelItem(tutorialItem);
   QTreeWidgetItem* introGuideItem = createTreeItem("Caret 5.5 Analysis Tutorial", 
                                                       "tutorials/Caret_Analysis_5.5.html");
   indexTreeWidget->addTopLevelItem(introGuideItem);
   QTreeWidgetItem* segmentGuideItem = createTreeItem("Caret 5.5 Segmentation, Flattening, and Registration", 
                                                       "tutorials/Caret_5.5_Tutorial_Segment.html");
   indexTreeWidget->addTopLevelItem(segmentGuideItem);
}

/**
 * Create an list tree widget item
 */
QTreeWidgetItem* 
GuiHelpViewerWindow::createTreeItem(const QString& label, const QString& helpPage)
{
   QString pageName = helpPage;
   
   QFileInfo fileInfo(pageName);
   if (fileInfo.isAbsolute() == false) {
      QString defaultPage(theMainWindow->getBrainSet()->getCaretHomeDirectory());
      defaultPage.append("/");
      defaultPage.append("caret5_help");
      defaultPage.append("/");
      defaultPage.append(helpPage);
      pageName = defaultPage;
   }
      
   QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(label));
   item->setText(0, label);
   item->setText(1, pageName);
   return item;
}

/**
 * called when an index tree item is clicked.
 */
void 
GuiHelpViewerWindow::indexTreeItemSelected(QTreeWidgetItem* item, int /*column*/)
{
   const QString webPage(item->text(1));
   
   //std::cout << "Item selected is: " << webPage.toAscii().constData() << std::endl;
   
   if (webPage.isEmpty() == false) {
      loadPage(webPage);
   }
}
      
/**
 * called when a search tree item is clicked.
 */
void 
GuiHelpViewerWindow::searchTreeItemSelected(QTreeWidgetItem* item, int /*column*/)
{
   const QString webPage(item->text(1));
   
   //std::cout << "Item selected is: " << webPage.toAscii().constData() << std::endl;
   
   if (webPage.isEmpty() == false) {
      loadPage(webPage);
      slotFindNextInBrowser();
   }
}
      
/**
 * Load a page into the help browser.
 */
void
GuiHelpViewerWindow::loadPage(const QString& pageNameIn)
{
   //
   // If no page name then default to main page
   //
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
   
   static bool firstTime = true;
   
   if (firstTime && (QFile::exists(pageName) == false)) {
      QString msg;
      msg.append("The file ");
      msg.append(pageName);
      msg.append("\n");
      msg.append("cannot be found.");      
      QApplication::beep();
      QMessageBox::critical(this, "Caret Error", msg, "OK");
      return;
   }
   firstTime = false;
   
   helpBrowser->setSource(QUrl::fromLocalFile(pageName));
}

/**
 * get all web page names and titles.
 */
void 
GuiHelpViewerWindow::getAllWebPages(QVector<QPair<QString,QString> >& pagesOut) const
{
   pagesOut.clear();
   
   //
   // Search through the tree widget to find all items with URLs
   //
   const int numItems = indexTreeWidget->topLevelItemCount();
   for (int i = 0; i < numItems; i++) {
      const QTreeWidgetItem* topItem = indexTreeWidget->topLevelItem(i);
      if (topItem->text(1).isEmpty() == false) {
         const QString pageName = topItem->text(0);
         const QString pageURL  = topItem->text(1);
         pagesOut.push_back(qMakePair(pageName, pageURL));
      }
         
      //
      // Search children of this item
      //
      const int numSubItems = topItem->childCount();
      for (int j = 0; j < numSubItems; j++) {
         const QTreeWidgetItem* subItem = topItem->child(j);
         if (subItem->text(1).isEmpty() == false) {
            const QString pageName = subItem->text(0);
            const QString pageURL  = subItem->text(1);
            pagesOut.push_back(qMakePair(pageName, pageURL));
         }
      }
   }
}

/**
 * called to search all help pages.
 */
void 
GuiHelpViewerWindow::slotSearchLineEdit()
{
   searchTreeWidget->clear();
   
   const QString searchText = searchLineEdit->text();
   if (searchText.isEmpty() == false) {
      QVector<QPair<QString,QString> > pages;
      getAllWebPages(pages);
      for (int i = 0; i < pages.count(); i++) {
         const QString pageTitle = pages[i].first.toAscii().constData();
         const QString pageURL   = pages[i].second.toAscii().constData();
                   
         //std::cout << "Searching: "
         //          << pageTitle.toAscii().constData()
         //          << std::endl;
         if (FileUtilities::findTextInFile(pageURL, searchText, false)) {
            //std::cout << "   Page matches " 
            //          << std::endl;                      
            searchTreeWidget->addTopLevelItem(createTreeItem(pageTitle, pageURL));
         }      
      }
      
      findInBrowserText = searchText;
      findNextPushButton->setEnabled(true);
   }   
}
