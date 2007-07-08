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
#include <QLayout>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QTextBrowser>
#include <QPixmap>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "BrainSet.h"
#include "GuiHelpViewerWindow.h"
#include "GuiMainWindow.h"
#include "GuiTextBrowser.h"

#include "global_variables.h"

/**
 * Constructor (non-modal full navigation controls)
 */
GuiHelpViewerWindow::GuiHelpViewerWindow(QWidget* parent)
   : QtDialog(parent, false)  // QT4, Qt::WGroupLeader)
{
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
   helpMainWindow = new GuiHelpViewerMainWindow(true);
   dialogLayout->addWidget(helpMainWindow);
   
   //
   // Buttons layout.
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setFixedSize(closeButton->sizeHint());
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   buttonsLayout->addWidget(closeButton);
   
   helpMainWindow->loadPage();
}

/**
 * Contructor (modal with a modal parent).
 */
GuiHelpViewerWindow::GuiHelpViewerWindow(QDialog* parent,
                                         const QString& helpPageIn)
   : QtDialog(parent, true)
{
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
   helpMainWindow = new GuiHelpViewerMainWindow(false);
   dialogLayout->addWidget(helpMainWindow);
   
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
   
   helpMainWindow->loadPage(helpPageIn);
}
                          
/**
 * Destructor
 */
GuiHelpViewerWindow::~GuiHelpViewerWindow()
{
}

/**
 * Load a page into the main window's help browser.
 */
void
GuiHelpViewerWindow::loadPage(const QString& page)
{
   helpMainWindow->loadPage(page);
}

//-------------------------------------------------------------------------------------

/**
 * Main Window Constructor
 */
GuiHelpViewerMainWindow::GuiHelpViewerMainWindow(const bool showNavigationControlsFlag)
                      : QMainWindow(0)
{
   //
   // Widget and layout for this main window
   //
   QWidget* mainWidget = new QWidget(this);
   QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
   setCentralWidget(mainWidget);
   
   //
   // create the help browser
   //
   helpBrowser = new GuiTextBrowser;
   helpBrowser->setMinimumWidth(400);
   helpBrowser->setMinimumHeight(200);
   if (showNavigationControlsFlag == false) {
      mainLayout->addWidget(helpBrowser);
   }
   
   if (showNavigationControlsFlag) {
      //
      // Create the toolbar
      //
      QToolBar* toolbar = new QToolBar(this);
      addToolBar(toolbar);
      
      //
      // Create the tree widget for the indices
      //
      QStringList headerLabels;
      headerLabels << "Help Page Index" << "Location";
      indexTreeWidget = new QTreeWidget;
      indexTreeWidget->setColumnCount(2);
      indexTreeWidget->setHeaderLabels(headerLabels);
      indexTreeWidget->setColumnHidden(0, false);
      indexTreeWidget->setColumnHidden(1, true);
      QObject::connect(indexTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                       this, SLOT(treeIndexItemSelected(QTreeWidgetItem*,int)));
      loadIndexTree();
      
      //
      // Create the splitter and add the widgets to the splitter
      //
      splitter = new QSplitter;
      splitter->setOrientation(Qt::Horizontal);
      splitter->addWidget(indexTreeWidget);
      splitter->addWidget(helpBrowser);
      QList<int> sizeList;
      sizeList << 125 << 375;
      splitter->setSizes(sizeList);
      mainLayout->addWidget(splitter);

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
      toolbar->addWidget(backwardButton);
      
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
      toolbar->addWidget(forwardButton);
      
      //
      // Create the home toolbar button
      // 
      QAction* homeAction = new QAction("Home", this);
      connect(homeAction, SIGNAL(triggered()),
              helpBrowser, SLOT(home()));
      QToolButton* homeButton = new QToolButton;
      homeButton->setDefaultAction(homeAction);
      toolbar->addWidget(homeButton);
      
      //
      // Create the print toolbar button
      //
      QAction* printAction = new QAction("Print", this);
      connect(printAction, SIGNAL(triggered()),
              this, SLOT(slotPrint()));
      QToolButton* printButton = new QToolButton;
      printButton->setDefaultAction(printAction);
      toolbar->addWidget(printButton);
   }
   
   //
   // Load the default page
   //
   loadPage();
}

/**
 * Main Window Destructor
 */
GuiHelpViewerMainWindow::~GuiHelpViewerMainWindow()
{
}

/**
 * called to print currently displayed page.
 */
void 
GuiHelpViewerMainWindow::slotPrint()
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
GuiHelpViewerMainWindow::loadIndexTree()
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
   dialogsItem->addChild(createTreeItem("Display Control", "dialogs/display_control_dialog.html"));
   dialogsItem->addChild(createTreeItem("Draw Border", "dialogs/draw_border_dialog.html"));
   dialogsItem->addChild(createTreeItem("Metric Smoothing", "dialogs/metric_smoothing_dialog.html"));
   dialogsItem->addChild(createTreeItem("Project Cells/Foci", "dialogs/project_cells_foci_dialog.html"));
   dialogsItem->addChild(createTreeItem("Record Main Window Images as Movie", "dialogs/record_as_mpeg_dialog.html"));
   dialogsItem->addChild(createTreeItem("Surface Region of Interest", "dialogs/surface_roi_dialog.html"));

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
GuiHelpViewerMainWindow::createTreeItem(const QString& label, const QString& helpPage)
{
   QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(label));
   item->setText(0, label);
   item->setText(1, helpPage);
   return item;
}

/**
 * called when an tree index item is clicked.
 */
void 
GuiHelpViewerMainWindow::treeIndexItemSelected(QTreeWidgetItem* item, int /*column*/)
{
   const QString webPage(item->text(1));
   
   //std::cout << "Item selected is: " << webPage.toAscii().constData() << std::endl;
   
   if (webPage.isEmpty() == false) {
      loadPage(webPage);
   }
}
      
/**
 * Load a page into the help browser.
 */
void
GuiHelpViewerMainWindow::loadPage(const QString& pageNameIn)
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

