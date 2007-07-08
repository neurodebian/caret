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

#include <algorithm>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QCheckBox>
#include <QButtonGroup>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolTip>
#include <QUrl>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "GuiFileDialogWithInstructions.h"
#include "GuiMessageBox.h"
#include "GuiSumsDialog.h"
#include "HttpFileDownload.h"
#include "PreferencesFile.h"
#include "QtUtilities.h"
#include "SceneFile.h"
#include "SpecFile.h"
#include "Species.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "Structure.h"
#include "SystemUtilities.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
GuiSumsDialog::GuiSumsDialog(QWidget* parent, PreferencesFile* preferencesFileIn)
   : QDialog(parent)
{
   setWindowTitle("SumsDB Database");
   dialogMode = DIALOG_MODE_NONE;
   preferencesFile = preferencesFileIn;

   //
   // Label for page title
   //
   pageTitleLabel = new QLabel("");
   
   //
   // Widget stack for pages
   //
   pagesStackedWidget = new QStackedWidget;

   //
   // Create the database login page
   //
   pageDatabaseLogin = createDatabaseLoginPage();
   addPage(pageDatabaseLogin, "<B>Database Login</B>");
   
   //
   // User information page
   //
   pageDatabaseUserInformation = createDatabaseUserInformation();
   addPage(pageDatabaseUserInformation, "<B>Database User Information</B>");
   
   //
   // Create the mode page
   //
   pageDatabaseMode = createDatabaseModePage();
   addPage(pageDatabaseMode, "<B>Database Mode</B>");
   
   //
   // Create the download atlas
   //
   pageDownloadAtlas = createDownloadAtlasPage();
   addPage(pageDownloadAtlas, "<B>Download Atlas</B>");
   
   //
   // Create the download search page
   //
   pageDownloadSearch = createDownloadSearchPage();
   addPage(pageDownloadSearch, "<B>Download Search</B>");
   
   //
   // Create the refresh spec file page
   //
   pageRefreshSpecFile = createRefereshSpecFilePage();
   addPage(pageRefreshSpecFile, "<B>Refresh Spec File</B>");
   
   //
   // Create the upload files page
   //
   pageUploadFiles = createUploadFilesPage();
   addPage(pageUploadFiles, "<B>Upload Files</B>");
   
   //
   // Create the spec file list section
   //
   pageSpecFileSelection = createSpecFileListSection();
   addPage(pageSpecFileSelection, "<B>Spec Files Selections</B>");
   
   //
   // Create the data file list section
   //
   pageDataFileSelection = createDataFileListSection();
   addPage(pageDataFileSelection, "<B>Data File Selection</B>");
   
   //
   // Create the output directory section
   //
   pageOutputDirectory = createOutputDirectorySection();
   addPage(pageOutputDirectory, "<B>Output Directory Selection</B>");

   //
   // push button
   // 
   QPushButton* logoutPushButton = new QPushButton("Logout");
   logoutPushButton->setAutoDefault(false);
   QObject::connect(logoutPushButton, SIGNAL(clicked()),
                    this, SLOT(slotLogoutButton()));                 
   
   //
   // push button
   // 
   backPushButton = new QPushButton("Back");
   backPushButton->setAutoDefault(false);
   QObject::connect(backPushButton, SIGNAL(clicked()),
                    this, SLOT(slotBackPushButton()));                 
   
   //
   // push button
   // 
   nextPushButton = new QPushButton("Next");
   nextPushButton->setAutoDefault(false);
   QObject::connect(nextPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNextPushButton()));                 
   
   //
   // push button
   // 
   finishPushButton = new QPushButton("Finish");
   finishPushButton->setAutoDefault(false);
   QObject::connect(finishPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFinishPushButton()));                 
   
   //
   // push button
   // 
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseButton()));                 
   
   //
   // Make buttons same size
   //
   QtUtilities::makeButtonsSameSize(logoutPushButton,
                                    backPushButton,
                                    nextPushButton,
                                    finishPushButton,
                                    closePushButton);
   //
   // Layout for push buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(backPushButton);
   buttonsLayout->addWidget(nextPushButton);
   buttonsLayout->addWidget(finishPushButton);
   //buttonsLayout->insertStretch();
   buttonsLayout->addWidget(logoutPushButton);
   buttonsLayout->addWidget(closePushButton);

   //
   // Layout for dialog
   //   
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(pageTitleLabel);
   dialogLayout->addWidget(pagesStackedWidget);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Initialize the dialog
   //
   initializeDialog();   
}

/**
 * Destructor.
 */
GuiSumsDialog::~GuiSumsDialog()
{
}

/**
 * initialize the dialog.
 */
void 
GuiSumsDialog::initializeDialog()
{
   dialogMode = DIALOG_MODE_NONE;
   
   //
   // Load default hosts
   //
   availableDatabaseHostNames.clear();
   availableDatabaseHostNames.push_back("http://sumsdb.wustl.edu:8081");
   
   //
   // Get hosts from  preferences file
   //
   if (preferencesFile != NULL) {
      const QString hosts = preferencesFile->getSumsDatabaseHosts();
      if (hosts.isEmpty() == false) {
         std::vector<QString> tokens;
         StringUtilities::token(hosts, ",", tokens);
         for (int i = 0; i < static_cast<int>(tokens.size()); i++) {
            const QString name = StringUtilities::trimWhitespace(tokens[i]);
            if (name.isEmpty() == false) {
               availableDatabaseHostNames.push_back(name);                  
            }
         }
      }
   }
         
   //
   // Find currently selected host and load database host combo box
   //
   databaseSelectionComboBox->clear();
   for (int i = 0; i < static_cast<int>(availableDatabaseHostNames.size()); i++) {
      databaseSelectionComboBox->addItem(availableDatabaseHostNames[i]);
   }

   if (preferencesFile != NULL) {
      QString userName;
      QString passWord;
      QString hostName;
      bool useLoginName;
      bool loginDataValid;
      preferencesFile->getSumsLoginInformation(userName,
                                               passWord,
                                               hostName,
                                               useLoginName,
                                               loginDataValid);
      if (loginDataValid) {
         databaseUserNameLineEdit->setText(userName);
         databasePasswordLineEdit->setText(passWord);
         for (int i = 0; i < databaseSelectionComboBox->count(); i++) {
            if (hostName == databaseSelectionComboBox->itemText(i)) {
               databaseSelectionComboBox->setCurrentIndex(i);
               break;
            }
         }

         if (useLoginName) {
            loginUserNameRadioButton->setChecked(true);
         }
         else {
            loginVisitorRadioButton->setChecked(true);
         }
      }
      
      saveLoginCheckBox->setChecked(loginDataValid);
   }

   showPage(pageDatabaseLogin);
}
      
/**
 * add a  page.
 */
void 
GuiSumsDialog::addPage(QWidget* newPage, const QString& title)
{
   pagesStackedWidget->addWidget(newPage);
   pageTitles.push_back(title);
}

/**
 * Called when the logout button is pressed.
 */
void 
GuiSumsDialog::slotLogoutButton()
{
   //
   // URL for logging off
   //
   QString url(databaseHostName);
   url.append("/sums/dispatch.do?forward=logoff");
   
   //
   // Send the request
   //
   QString contents;
   QString errorMessage;
   std::map<QString,QString> headerTags;
   if (FileUtilities::downloadFileWithHttpGet(url, searchTimeoutSpinBox->value(), 
                                              contents, errorMessage, &headerTags)) {
      //std::cout << "Logoff contents: " << contents.toAscii().constData() << std::endl;
   }
   
   showPage(pageDatabaseLogin);
}
      
/**
 * Called when close button pressed.
 */
void
GuiSumsDialog::slotCloseButton()
{
   QDialog::close();
}

/**
 * Show the dialog.
 */
void
GuiSumsDialog::show()
{
   slotEnableDisablePushButtons();
   
   QDialog::show();
}

/**
 * Create the database Mode page
 */
QWidget*
GuiSumsDialog::createDatabaseModePage()
{
   //
   // Download Archive button
   //
   modeDownloadArchiveRadioButton = new QRadioButton("Download Archive");
   downloadArchiveNumberLineEdit = new QLineEdit;
   downloadArchiveNumberLineEdit->setText("568300");
   downloadArchiveNumberLineEdit->setMaximumWidth(200);
   downloadArchiveNumberLineEdit->setEnabled(false);
                            
   //
   // Download Spec button
   //
   modeDownloadSpecRadioButton = new QRadioButton("Download Spec");
   downloadSpecNumberLineEdit = new QLineEdit;
   downloadSpecNumberLineEdit->setText("701006");
   downloadSpecNumberLineEdit->setMaximumWidth(200);
   downloadSpecNumberLineEdit->setEnabled(false);
   
   //
   // Download Atlas button
   //
   modeDownloadAtlasRadioButton = new QRadioButton("Download Atlas");
                                                   
   //
   // Download Search button
   //
   modeDownloadSearchRadioButton = new QRadioButton("Download Search");
                                                   
   //
   // Refresh Spec button
   //
   modeRefreshSpecRadioButton = new QRadioButton("Refresh Spec File");
   modeRefreshSpecRadioButton->setEnabled(false);  // no refresh at this time
                                                   
   //
   // Upload file button
   //
   modeUploadFilesRadioButton = new QRadioButton("Upload Files");
   
   //
   // Group box and layout for database mode
   //
   QGroupBox* modeGroupBox = new QGroupBox("Database Mode");
   QGridLayout* modeLayout = new QGridLayout(modeGroupBox);
   modeLayout->addWidget(modeDownloadArchiveRadioButton, 0, 0);
   modeLayout->addWidget(downloadArchiveNumberLineEdit, 0, 1);
   modeLayout->addWidget(modeDownloadSpecRadioButton, 1, 0);
   modeLayout->addWidget(downloadSpecNumberLineEdit, 1, 1);
   modeLayout->addWidget(modeDownloadAtlasRadioButton, 2, 0);
   modeLayout->addWidget(modeDownloadSearchRadioButton, 3, 0);
   modeLayout->addWidget(modeRefreshSpecRadioButton, 4, 0);
   modeLayout->addWidget(modeUploadFilesRadioButton, 5, 0);
   modeGroupBox->setFixedSize(modeGroupBox->sizeHint());
   
   //
   // Button group for database mode selection
   //
   QButtonGroup* dbModeButtonGroup = new QButtonGroup(this);
   QObject::connect(dbModeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
   dbModeButtonGroup->addButton(modeDownloadArchiveRadioButton);
   dbModeButtonGroup->addButton(modeDownloadSpecRadioButton);
   dbModeButtonGroup->addButton(modeDownloadAtlasRadioButton);
   dbModeButtonGroup->addButton(modeDownloadSearchRadioButton);
   dbModeButtonGroup->addButton(modeRefreshSpecRadioButton);
   dbModeButtonGroup->addButton(modeUploadFilesRadioButton);
                    
   //
   // Add help information
   //
   QWidget* infoPage = addPageInformation(
                      "Database Mode Information",
                      "Download Archive - Choose this item and enter the id number of the "
                      "archive to get a listing of the archive's files.\n\n"
                      "Download Spec - Choose this item and enter the id number of the spec file "
                      "to get a listing of the spec file's data files.\n\n"
                      "Download Atlas - Choose this item to download an atlas data set.\n\n"
                      "Download Search - Choose this to search the SumsDB database for files that "
                      "are to be downloaded.\n\n"
                      "Refresh Spec File - Choose this to refresh a spec file and "
                      "its data files that have already been downloaded.\n\n"
                      "Upload Files - Choose this to upload files to the SumsDB database.");
   
   QWidget* w = new QWidget;
   QVBoxLayout* pageLayout = new QVBoxLayout(w);
   pageLayout->addWidget(modeGroupBox);
   pageLayout->addWidget(infoPage);
   
   return w;   
}

/**
 * Create the database login page
 */
QWidget*
GuiSumsDialog::createDatabaseLoginPage()
{
   //
   // SuMS database selection
   //
   QLabel* databaseLabel = new QLabel("Database  ");
   databaseSelectionComboBox = new QComboBox;
   
   //
   // spacing between rows
   //
   QLabel* dummyRowLabel = new QLabel(" ");

   //
   // Visitor radio button and stuff
   //
   loginVisitorRadioButton = new QRadioButton("");
   QLabel* visitorLabel = new QLabel("Visitor");
   
   //
   // User name
   //
   loginUserNameRadioButton = new QRadioButton("");
   QLabel* usernameLabel = new QLabel("Username  ");
   databaseUserNameLineEdit = new QLineEdit;
   databaseUserNameLineEdit->setText("");
   
   //
   // Password
   //
   QLabel* passwordLabel = new QLabel("Password   ");
   databasePasswordLineEdit = new QLineEdit;
   databasePasswordLineEdit->setEchoMode(QLineEdit::Password);
   databasePasswordLineEdit->setText("");

   //
   // Save login parameters check box
   //
   saveLoginCheckBox = new QCheckBox("");
   QLabel* saveLoginLabel = new QLabel("Save Login");
   
   //
   // Button group for visitor and username radio buttons
   //
   QButtonGroup* loginButtonGroup = new QButtonGroup(this);
   QObject::connect(loginButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotEnableDisablePushButtons()));
   loginButtonGroup->addButton(loginVisitorRadioButton);
   loginButtonGroup->addButton(loginUserNameRadioButton);
                    
   //
   // Group box and layout for database login
   //   
   QGroupBox* databaseLoginGroupBox = new QGroupBox("Database Login");
   QGridLayout* databaseLayout = new QGridLayout(databaseLoginGroupBox);
   databaseLayout->addWidget(databaseLabel, 0, 1);
   databaseLayout->addWidget(databaseSelectionComboBox, 0, 2);
   databaseLayout->addWidget(dummyRowLabel, 1, 0);
   databaseLayout->addWidget(loginVisitorRadioButton, 2, 0);
   databaseLayout->addWidget(visitorLabel, 2, 1);
   databaseLayout->addWidget(loginUserNameRadioButton, 3, 0);
   databaseLayout->addWidget(usernameLabel, 3, 1);
   databaseLayout->addWidget(databaseUserNameLineEdit, 3, 2);
   databaseLayout->addWidget(passwordLabel, 4, 1);
   databaseLayout->addWidget(databasePasswordLineEdit, 4, 2);
   databaseLayout->addWidget(saveLoginCheckBox, 5, 0);
   databaseLayout->addWidget(saveLoginLabel, 5, 1);
   databaseLoginGroupBox->setFixedSize(databaseLoginGroupBox->sizeHint());
   
   //
   // search timeout
   //
   QLabel* searchTimeoutLabel = new QLabel("Search Timeout (seconds)  ");
   searchTimeoutSpinBox = new QSpinBox;
   searchTimeoutSpinBox->setMinimum(1);
   searchTimeoutSpinBox->setMaximum(10000000);
   searchTimeoutSpinBox->setSingleStep(10);
   searchTimeoutSpinBox->setValue(60);
   searchTimeoutSpinBox->setToolTip(
                 "This value is the maximum time a database\n"
                 "search may run before it is stopped.");
   
   //
   // data file timeout
   //
   QLabel* dataTimeoutLabel = new QLabel("Data File Timeout (seconds)  ");
   dataFileTimeoutSpinBox = new QSpinBox;
   dataFileTimeoutSpinBox->setMinimum(1);
   dataFileTimeoutSpinBox->setMaximum(10000000);
   dataFileTimeoutSpinBox->setSingleStep(10);
   dataFileTimeoutSpinBox->setValue(preferencesFile->getSumsDatabaseDataFileTimeout());
   dataFileTimeoutSpinBox->setToolTip(
                 "This value is the maximum time\n"
                 "allowed for downloading a file.");
   
   //
   // Max retries if file fails
   //
   QLabel* maxRetriesLabel = new QLabel("Max Retries ");
   downloadNumTimesSpinBox = new QSpinBox;
   downloadNumTimesSpinBox->setMinimum(1);
   downloadNumTimesSpinBox->setMaximum(100);
   downloadNumTimesSpinBox->setSingleStep(1);
   downloadNumTimesSpinBox->setValue(3);
   downloadNumTimesSpinBox->setToolTip(
                 "This value is the number of attempts that\n"
                 "will be made to download a file.  Sometimes\n"
                 "file downloads fail for no apparent reason, so\n"
                 "keep this value greater than 1.");
   
   //
   // Login before each operation check box
   //
   loginBeforeOperationCheckBox = new QCheckBox("Login Prior to Each Operation");
   loginBeforeOperationCheckBox->setChecked(true);
   
   //
   // Parameters group box and layout
   //
   QGridLayout* paramsGrid = new QGridLayout;
   paramsGrid->addWidget(searchTimeoutLabel, 0, 0);
   paramsGrid->addWidget(searchTimeoutSpinBox, 0, 1);
   paramsGrid->addWidget(dataTimeoutLabel, 1, 0);
   paramsGrid->addWidget(dataFileTimeoutSpinBox, 1, 1);
   paramsGrid->addWidget(maxRetriesLabel, 2, 0);
   paramsGrid->addWidget(downloadNumTimesSpinBox, 2, 1);
   QGroupBox* parametersGroupBox = new QGroupBox("Parameters");
   QVBoxLayout* paramsLayout = new QVBoxLayout(parametersGroupBox);
   paramsLayout->addLayout(paramsGrid);
   paramsLayout->addWidget(loginBeforeOperationCheckBox);
   parametersGroupBox->setFixedSize(parametersGroupBox->sizeHint());
   
   //
   // Add help information
   //
   QWidget* pageInfoWidget = addPageInformation(
                      "Database Information",
                      "If you have a username and password, select the radio (round) button "
                      "next to \"Username\" and enter your username and password.  Otherwise, "
                      "select the radio (round) button next to \"Visitor\".  Click the "
                      "checkbox (square) button next to \"Save Login\" to save your login "
                      "information so that you will not need to enter it next time.");
            
   //
   // Widget and layouts for page
   //
   QHBoxLayout* horizLayout = new QHBoxLayout;
   horizLayout->addWidget(databaseLoginGroupBox);
   horizLayout->addWidget(parametersGroupBox);
   QWidget* w = new QWidget;
   QVBoxLayout* pageLayout = new QVBoxLayout(w);
   pageLayout->addLayout(horizLayout);
   pageLayout->addWidget(pageInfoWidget);
   return w;;   
}

/**
 * Create the database user information page
 */
QWidget*
GuiSumsDialog::createDatabaseUserInformation()
{
   //
   // String with sixty blanks
   //
   QString emptyItemStr;
   emptyItemStr.fill(' ', 60);
   const QString emptyItem(emptyItemStr);
   
   //
   // User name
   //
   QLabel* usernameLabel = new QLabel("Username ");
   dbInfoUserNameLabel = new QLabel(emptyItem);
   QLabel* usageLabel = new QLabel("Usage/Quota (Mb)");
   dbInfoUsageQuotaLabel = new QLabel(emptyItem);
   QLabel* privilegesLabel = new QLabel("Privileges ");
   dbInfoPrivilegesLabel = new QLabel(emptyItem);
   QLabel* uploadLabel = new QLabel("Upload Human Data ");
   dbInfoHumanLabel = new QLabel(emptyItem);
   QLabel* sessionLabel = new QLabel("Session ID ");
   dbInfoSessionIdLabel = new QLabel(emptyItem);
   
   //
   // Group Box and label for items
   //
   QGroupBox* infoGroupBox = new QGroupBox("User Information");
   QGridLayout* infoLayout = new QGridLayout(infoGroupBox);
   infoLayout->addWidget(usernameLabel, 0, 0);
   infoLayout->addWidget(dbInfoUserNameLabel, 0, 1);
   infoLayout->addWidget(usageLabel, 1, 0);
   infoLayout->addWidget(dbInfoUsageQuotaLabel, 1, 1);
   infoLayout->addWidget(privilegesLabel, 2, 0);
   infoLayout->addWidget(dbInfoPrivilegesLabel, 2, 1);
   infoLayout->addWidget(uploadLabel, 3, 0);
   infoLayout->addWidget(dbInfoHumanLabel, 3, 1);
   infoLayout->addWidget(sessionLabel, 4, 0);
   infoLayout->addWidget(dbInfoSessionIdLabel, 4, 1);
   infoLayout->setColumnStretch(0, 0);
   infoLayout->setColumnStretch(1, 100);
   infoGroupBox->setFixedHeight(infoGroupBox->sizeHint().height());

   //
   // widget for page
   //
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(infoGroupBox);
   
   return w;   
}

/**
 * Load the database user information page.
 */
void
GuiSumsDialog::loadDatabaseUserInformationPage()
{
   dbInfoUserNameLabel->setText(loginInfoFile.getValue("sums:login:user:username"));
   const QString usage(loginInfoFile.getValue("sums:login:user:current"));
   const QString quota(loginInfoFile.getValue("sums:login:user:quota"));
   dbInfoUsageQuotaLabel->setText(usage + "/" + quota);
   dbInfoHumanLabel->setText("No");
   if ((loginInfoFile.getValue("sums:login:user:human") == "true") &&
       (loginInfoFile.getValue("sums:login:user:code_agreement") == "true")) {
      dbInfoHumanLabel->setText("Yes");
   }
   
   std::vector<QString> privs;
   loginInfoFile.getValue("sums:login:user:roles:role", privs);
   for (int i = 0; i < static_cast<int>(privs.size()); i++) {
      if (privs[i] == "upload") {
         uploadButtonValid = true;
      }
   }
   dbInfoPrivilegesLabel->setText(StringUtilities::combine(privs, "  "));
   dbInfoSessionIdLabel->setText(sumsSessionID);
   
   if (loginVisitorRadioButton->isChecked()) {
      dbInfoUserNameLabel->setText("Visitor");
      dbInfoPrivilegesLabel->setText("search");
   }
}

/**
 * Adds help information in a read only text widget.
 */
QGroupBox*
GuiSumsDialog::addPageInformation(const QString& title,
                                  const QString& text)
{
   //
   // Create the text editor
   //
   const int maxHeight = 125;
   QTextEdit* textEditor = new QTextEdit;
   textEditor->setReadOnly(true);
   textEditor->setPlainText(text);
   int height = textEditor->sizeHint().height();
   if (height > maxHeight) {
      height = maxHeight;
   }
   textEditor->setMaximumHeight(maxHeight);

   QGroupBox* group = new QGroupBox(title);
   QVBoxLayout* groupLayout = new QVBoxLayout(group);
   groupLayout->addWidget(textEditor);
   group->setFixedHeight(group->sizeHint().height());
   
   return group;
}

/**
 * Create the download atlas page
 */
QWidget*
GuiSumsDialog::createDownloadAtlasPage()
{
   //
   // Atlas species
   // 
   QLabel* speciesLabel = new QLabel("Species ");
   downloadAtlasSpeciesComboBox = new QComboBox;
   std::vector<QString> species;
   Species::getAllSpecies(species);
   downloadAtlasSpeciesComboBox->addItem("Any");
   for (int i = 0; i < static_cast<int>(species.size()); i++) {
      downloadAtlasSpeciesComboBox->addItem(species[i]);
   }
   
   //
   // Atlas hemisphere
   // 
   QLabel* structLabel = new QLabel("Structure ");
   downloadAtlasHemisphereComboBox = new QComboBox;
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes,
                                  structureNames,
                                  false);
   downloadAtlasHemisphereComboBox->addItem("Any");
   downloadAtlasHemisphereComboBox->addItem("left");
   downloadAtlasHemisphereComboBox->addItem("right");
   downloadAtlasHemisphereComboBox->addItem("both");
   for (int i = 0; i < static_cast<int>(structureNames.size()); i++) {
      downloadAtlasHemisphereComboBox->addItem(structureNames[i]);
   }

   //
   // Atlas space
   // 
   QLabel* spaceLabel = new QLabel("Space ");
   downloadAtlasSpaceComboBox = new QComboBox;
   std::vector<StereotaxicSpace> spaces;
   StereotaxicSpace::getAllStereotaxicSpaces(spaces);
   downloadAtlasSpaceComboBox->addItem("Any");
   for (unsigned int i = 0; i < spaces.size(); i++) {
      downloadAtlasSpaceComboBox->addItem(spaces[i].getName());
   }

   //
   // Download atlas group box and layout
   //
   QGroupBox* atlasGroupBox = new QGroupBox("Download Atlas");
   QGridLayout* atlasGrid = new QGridLayout(atlasGroupBox);
   atlasGrid->addWidget(speciesLabel, 0, 0);
   atlasGrid->addWidget(downloadAtlasSpeciesComboBox, 0, 1);
   atlasGrid->addWidget(structLabel, 1, 0);
   atlasGrid->addWidget(downloadAtlasHemisphereComboBox, 1, 1);
   atlasGrid->addWidget(spaceLabel, 2, 0);
   atlasGrid->addWidget(downloadAtlasSpaceComboBox, 2, 1);
   atlasGrid->setColumnStretch(0, 0);
   atlasGrid->setColumnStretch(1, 100);
   atlasGroupBox->setFixedHeight(atlasGroupBox->sizeHint().height());

   //
   // Add help information
   //
   QWidget* infoPage = addPageInformation(
                      "Download Atlas Information",
                      "Use the Species, Hemisphere, and Space selection to describe "
                      "the atlas you are seeking.  The next page will list the atlases "
                      "that meet your criteria.");
    
   QWidget* w = new QWidget;
   QVBoxLayout* pageLayout = new QVBoxLayout(w);
   pageLayout->addWidget(atlasGroupBox);
   pageLayout->addWidget(infoPage);
   return w;   
}

/**
 * Create the upload files page
 */
QWidget*
GuiSumsDialog::createUploadFilesPage()
{
   //
   // the list box containing the files for uploading
   //
   uploadFilesListBox = new QListWidget;
   uploadFilesListBox->setSelectionMode(QListWidget::ExtendedSelection);
   
   //
   // Add upload file button
   //
   QPushButton* uploadAddPushButton = new QPushButton("Add Files...");
   uploadAddPushButton->setAutoDefault(false);
   QObject::connect(uploadAddPushButton, SIGNAL(clicked()),
                    this, SLOT(slotUploadAddPushButton()));
                    
   //
   // Remove upload file button
   //
   QPushButton* uploadRemovePushButton = new QPushButton("Remove Selected Files");
   uploadRemovePushButton->setAutoDefault(false);
   QObject::connect(uploadRemovePushButton, SIGNAL(clicked()),
                    this, SLOT(slotUploadRemovePushButton()));
                    
   QtUtilities::makeButtonsSameSize(uploadAddPushButton, uploadRemovePushButton);
   
   //
   // Layouts for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(uploadAddPushButton);
   buttonsLayout->addWidget(uploadRemovePushButton);
   
   //
   // Comment line edit
   //
   QLabel* commentLabel = new QLabel("Comment  ");
   uploadCommentLineEdit = new QLineEdit;
   QHBoxLayout* commentLayout = new QHBoxLayout;
   commentLayout->addWidget(commentLabel);
   commentLayout->addWidget(uploadCommentLineEdit);
   
   //
   // Group box and layout for upload files section
   //
   QGroupBox* uploadGroupBox = new QGroupBox("Upload Files");
   QVBoxLayout* uploadLayout = new QVBoxLayout(uploadGroupBox);
   uploadLayout->addWidget(uploadFilesListBox);
   uploadLayout->addLayout(buttonsLayout);
   uploadLayout->addLayout(commentLayout);
   
   //
   // Add help information
   //
   QWidget* infoWidget = addPageInformation(
                      "Upload Files Information",
                      "Press the \"Finish\" button to upload the files.\n\n"
                      "Press the \"Add Files\" button to choose files for uploading.\n\n"
                      "To remove files, select them with the mouse and press the "
                      "\"Remove Selected Files\" button.");
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(uploadGroupBox);
   layout->addWidget(infoWidget);

   return w;   
}

/**
 * Called when upload file add button is pressed.
 */
void 
GuiSumsDialog::slotUploadAddPushButton()
{
   const QString allFilesFilter("All Files (*)");
   const QString specFilesFilter(QString("Spec Files (*%1)").arg(SpecFile::getSpecFileExtension()));
   QString instructions = "To select files, hold down the CTRL key while selecting "
                          "border file names with the mouse (Macintosh users should "
                          "hold downthe Apple key).";
   GuiFileDialogWithInstructions addUploadFileDialog(this, instructions, "chooseUploadFile", true);
   addUploadFileDialog.setWindowTitle("Choose Files For Uploading");
   addUploadFileDialog.setMode(GuiFileDialogWithInstructions::ExistingFiles);
   addUploadFileDialog.setFilter(specFilesFilter);
   addUploadFileDialog.addFilter(allFilesFilter);
   addUploadFileDialog.setSelectedFilter(specFilesFilter);
   if (addUploadFileDialog.exec() == QDialog::Accepted) {
      QStringList list = addUploadFileDialog.selectedFiles();
      QStringList::Iterator it = list.begin();
      while( it != list.end() ) {
         QString filename((*it));
         uploadFileNames.push_back(filename);
         ++it;
      }
   }
   
   loadUploadFilesListBox();
}

/**
 * load the upload files list box.
 */
void 
GuiSumsDialog::loadUploadFilesListBox()
{
   uploadFilesListBox->clear();
   for (int i = 0; i < static_cast<int>(uploadFileNames.size()); i++) {
      uploadFilesListBox->addItem(uploadFileNames[i]);
   }
   slotEnableDisablePushButtons();
}

/**
 * Called when upload file remove button is pressed.
 */
void 
GuiSumsDialog::slotUploadRemovePushButton()
{
   std::vector<QString> names;
   
   for (int i = 0; i < static_cast<int>(uploadFileNames.size()); i++) {
      QListWidgetItem* item = uploadFilesListBox->item(i);
      if (uploadFilesListBox->isItemSelected(item) == false) {
         names.push_back(uploadFileNames[i]);
      }
   }
   
   uploadFileNames = names;
   
   loadUploadFilesListBox();
}
      
/**
 * Create the refresh spec page
 */
QWidget*
GuiSumsDialog::createRefereshSpecFilePage()
{
   
   QLabel* notImpLabel = new QLabel("Not Implemented.");
   
   //
   // refresh group box
   //
   QGroupBox* refreshGroupBox = new QGroupBox("Refresh Spec File");
   QVBoxLayout* refreshLayout = new QVBoxLayout(refreshGroupBox);
   refreshLayout->addWidget(notImpLabel);
   
   //
   // Add help information
   //
   QWidget* infoWidget = addPageInformation(
                      "Refresh Spec File Information",
                      "");
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(refreshGroupBox);
   layout->addWidget(infoWidget);

   return w;   
}

/**
 * Create the download search page
 */
QWidget*
GuiSumsDialog::createDownloadSearchPage()
{
   //
   // search file type
   //
   QLabel* fileTypeLabel = new QLabel("File Type ");
   downloadSearchFileTypeComboBox = new QComboBox;
   QObject::connect(downloadSearchFileTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotDownloadSearchFileTypeComboBox(int)));
                    
   //
   // Search filename
   //
   QLabel* fileNameLabel = new QLabel("File Name ");
   downloadSearchFileNameLineEdit = new QLineEdit;
   
   //
   // File comment
   //
   QLabel* fileCommentLabel = new QLabel("File Comment");
   downloadSearchFileCommentLineEdit = new QLineEdit;
   
   //
   // Keyword
   //
   QLabel* keywordLabel = new QLabel("Keyword");
   downloadSearchKeywordLineEdit = new QLineEdit;
   
   //
   // Layout for file information
   //
   QGridLayout* fileGridLayout = new QGridLayout;
   fileGridLayout->addWidget(fileTypeLabel, 0, 0);
   fileGridLayout->addWidget(downloadSearchFileTypeComboBox, 0, 1);
   fileGridLayout->addWidget(fileNameLabel, 1, 0);
   fileGridLayout->addWidget(downloadSearchFileNameLineEdit, 1, 1);
   fileGridLayout->addWidget(fileCommentLabel, 2, 0);
   fileGridLayout->addWidget(downloadSearchFileCommentLineEdit, 2, 1);
   fileGridLayout->addWidget(keywordLabel, 3, 0);
   fileGridLayout->addWidget(downloadSearchKeywordLineEdit, 3, 1);
   
   //
   // Date
   //
   const QString dateFormat("MMM dd YYYY");
   downloadSearchDateCheckBox = new QCheckBox("File Date");
   downloadSearchDateCheckBox->setFont(keywordLabel->font());
   QLabel* startLabel = new QLabel("Start ");
   downloadSearchStartDateEdit = new QDateTimeEdit(QDate::currentDate());
   downloadSearchStartDateEdit->setDisplayFormat(dateFormat);   
   QLabel* endLabel = new QLabel(" End ");   
   downloadSearchEndDateEdit = new QDateTimeEdit(QDate::currentDate());
   downloadSearchEndDateEdit->setDisplayFormat(dateFormat);
   QObject::connect(downloadSearchDateCheckBox, SIGNAL(toggled(bool)),
                    startLabel, SLOT(setEnabled(bool)));
   QObject::connect(downloadSearchDateCheckBox, SIGNAL(toggled(bool)),
                    downloadSearchStartDateEdit, SLOT(setEnabled(bool)));
   QObject::connect(downloadSearchDateCheckBox, SIGNAL(toggled(bool)),
                    endLabel, SLOT(setEnabled(bool)));
   QObject::connect(downloadSearchDateCheckBox, SIGNAL(toggled(bool)),
                    downloadSearchEndDateEdit, SLOT(setEnabled(bool)));
   startLabel->setEnabled(false);
   downloadSearchStartDateEdit->setEnabled(false);
   endLabel->setEnabled(false);
   downloadSearchEndDateEdit->setEnabled(false);
   downloadSearchDateCheckBox->setChecked(false);
   QHBoxLayout* dateLayout = new QHBoxLayout;
   dateLayout->addWidget(downloadSearchDateCheckBox);
   dateLayout->addWidget(startLabel);
   dateLayout->addWidget(downloadSearchStartDateEdit);
   dateLayout->addWidget(endLabel);
   dateLayout->addWidget(downloadSearchEndDateEdit);
   dateLayout->addStretch();
   
   //
   // Atlas hemisphere
   // 
   downloadSearchHemisphereLabel = new QLabel("Structure ");
   downloadSearchHemisphereComboBox = new QComboBox;
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes, structureNames, false);
   downloadSearchHemisphereComboBox->addItem("Any");
   for (int i = 0; i < static_cast<int>(structureNames.size()); i++) {
      downloadSearchHemisphereComboBox->addItem(structureNames[i]);
   }

   //
   // Atlas space
   // 
   downloadSearchSpaceLabel = new QLabel("Space ");
   downloadSearchSpaceComboBox = new QComboBox;
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
   downloadSearchSpaceComboBox->addItem("Any");
   for (int i = 0; i < static_cast<int>(allSpaces.size()); i++) {
      downloadSearchSpaceComboBox->addItem(allSpaces[i].getName());
   }

   //
   // Atlas species
   // 
   downloadSearchSpeciesLabel = new QLabel("Species ");
   downloadSearchSpeciesComboBox = new QComboBox;
   std::vector<QString> species;
   Species::getAllSpecies(species);
   downloadSearchSpeciesComboBox->addItem("Any");
   for (int i = 0; i < static_cast<int>(species.size()); i++) {
      downloadSearchSpeciesComboBox->addItem(species[i]);
   }
   
   //
   // Layout for attributes
   //
   QWidget* attWidget = new QWidget;
   QGridLayout* attributesGridLayout = new QGridLayout(attWidget);
   attributesGridLayout->addWidget(downloadSearchHemisphereLabel, 0, 0);
   attributesGridLayout->addWidget(downloadSearchHemisphereComboBox, 0, 1);
   attributesGridLayout->addWidget(downloadSearchSpaceLabel, 1, 0);
   attributesGridLayout->addWidget(downloadSearchSpaceComboBox, 1, 1);
   attributesGridLayout->addWidget(downloadSearchSpeciesLabel, 2, 0);
   attributesGridLayout->addWidget(downloadSearchSpeciesComboBox, 2, 1);
   attWidget->setFixedSize(attWidget->sizeHint());
   
   //
   // Download search group box
   //
   QGroupBox* downloadGroupBox = new QGroupBox("Download Search");
   QVBoxLayout* downloadGroupLayout = new QVBoxLayout(downloadGroupBox);
   downloadGroupLayout->addLayout(fileGridLayout);
   downloadGroupLayout->addLayout(dateLayout);
   downloadGroupLayout->addWidget(attWidget);
   
   //
   // Get file types from spec file and load into file type combo box
   //
   AbstractFile::getAllFileTypeNamesAndExtensions(downloadSearchFileTypeNames,
                                                  downloadSearchFileExtensions);
   
   //
   // Add anything but spec
   //
   downloadSearchFileTypeNames.insert(downloadSearchFileTypeNames.begin(), "All EXCEPT Spec Files");
   downloadSearchFileExtensions.insert(downloadSearchFileExtensions.begin(), "");

   //
   // Make spec file the first item
   //
   downloadSearchFileTypeNames.insert(downloadSearchFileTypeNames.begin(), "Spec Files (*.spec)");
   downloadSearchFileExtensions.insert(downloadSearchFileExtensions.begin(), "spec");
   
   //
   // Load file type combo box
   //
   for (unsigned int i = 0; i < downloadSearchFileTypeNames.size(); i++) {
      if (i > 0) {
         if (downloadSearchFileTypeNames[i] == "Spec File") {
            continue;
         }
      }
      downloadSearchFileTypeComboBox->addItem(downloadSearchFileTypeNames[i]);
   }
   slotDownloadSearchFileTypeComboBox(downloadSearchFileTypeComboBox->currentIndex());
   
   //
   // Add help information
   //
   QWidget* infoWidget = addPageInformation(
                      "Download Search Information",
                      "Enter information to search for file in the SumsDB database.\n\n"
                      "If the File Type is set to \"Spec Files\", the next page will be a "
                      "list of matching spec files.  Otherwise, the next page will be a "
                      "list of data files.");
       
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(downloadGroupBox);
   layout->addWidget(infoWidget);
   
   return w;   
}

/**
 * Called when a download search file type is selected.
 */
void 
GuiSumsDialog::slotDownloadSearchFileTypeComboBox(int item)
{
   bool showSpecSearchOptions = (item == SEARCH_FILE_INDEX_SPEC);
   
   downloadSearchSpeciesComboBox->setEnabled(showSpecSearchOptions);
   downloadSearchHemisphereComboBox->setEnabled(showSpecSearchOptions);
   downloadSearchSpaceComboBox->setEnabled(showSpecSearchOptions);
   downloadSearchSpeciesLabel->setEnabled(showSpecSearchOptions);
   downloadSearchHemisphereLabel->setEnabled(showSpecSearchOptions);
   downloadSearchSpaceLabel->setEnabled(showSpecSearchOptions);
}      

/**
 * Create the listing of data files.
 */
QWidget*
GuiSumsDialog::createDataFileListSection()
{
   //
   // Table containing list of files
   //
   dataFileTable = new QTableWidget;
   QObject::connect(dataFileTable, SIGNAL(cellChanged(int,int)),
                    this, SLOT(slotDataFileTableItemChanged(int,int)));
   dataFileTable->setColumnCount(DATA_TABLE_NUMBER_OF_COLUMNS);
//   dataFileTable->setColumnReadOnly(DATA_TABLE_FILE_DATE_COLUMN, true);
//   dataFileTable->setColumnReadOnly(DATA_TABLE_FILE_TYPE_COLUMN, true);
//   dataFileTable->setColumnReadOnly(DATA_TABLE_FILE_NAME_COLUMN, true);
//   dataFileTable->setColumnReadOnly(DATA_TABLE_FILE_SUBDIR_COLUMN, true);
//   dataFileTable->setColumnReadOnly(DATA_TABLE_FILE_STATE_COLUMN, true);
//   dataFileTable->setColumnReadOnly(DATA_TABLE_FILE_COMMENT_COLUMN, true);
   
   dataFileTable->setColumnWidth(DATA_TABLE_FILE_NAME_COLUMN, 500);
   dataFileTable->setColumnWidth(DATA_TABLE_FILE_COMMENT_COLUMN, 500);

   //
   // Data file table column headers
   //
   QHeaderView* header = dataFileTable->horizontalHeader();
   QObject::connect(header, SIGNAL(sectionClicked(int)),
                    this, SLOT(slotDataFileTableHeaderColumnClicked(int)));
                    
   //
   // Column labels
   //
   QStringList columnNames;
   columnNames << "Download";
   columnNames << "Date";
   columnNames << "File Type";
   columnNames << "File Name";
   columnNames << "Directory";
   columnNames << "State";
   columnNames << "Comment";
   dataFileTable->setHorizontalHeaderLabels(columnNames);
   
   //
   // Select all files button
   //
   QPushButton* selectAllFilesPushButton = new QPushButton("Select All Files");
   selectAllFilesPushButton->setFixedSize(selectAllFilesPushButton->sizeHint());
   selectAllFilesPushButton->setAutoDefault(false);
   QObject::connect(selectAllFilesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectAllDataFilesPushButton()));

   //
   // Deselect all files button
   //
   QPushButton* deselectAllFilesPushButton = new QPushButton("Deselect All Files");
   deselectAllFilesPushButton->setFixedSize(deselectAllFilesPushButton->sizeHint());
   deselectAllFilesPushButton->setAutoDefault(false);
   QObject::connect(deselectAllFilesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDeselectAllDataFilesPushButton()));
                    
   //
   // Assemble the buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(selectAllFilesPushButton);
   buttonsLayout->addWidget(deselectAllFilesPushButton);
   
   //
   // Group box for file selections
   //
   QGroupBox* dataFileGroupBox = new QGroupBox("Data Files");
   QVBoxLayout* dataFileLayout = new QVBoxLayout(dataFileGroupBox);
   dataFileLayout->addWidget(dataFileTable);
   dataFileLayout->addLayout(buttonsLayout);
   
   //
   // Add help information
   //
   QWidget* infoWidget = addPageInformation(
                      "Data Files Information",
                      "Files with a checkmark in the \"Download\" column will be "
                      "downloaded.  The files may be sorted by data, type, or "
                      "name by clicking on the appropriate column name header.");
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(dataFileGroupBox);
   layout->addWidget(infoWidget);
   
   return w;
}

/**
 * Called when a table item is changed.
 */
void 
GuiSumsDialog::slotDataFileTableItemChanged(int /*row*/, int col)
{
   if (col == DATA_TABLE_FILE_CHECK_BOX_COLUMN) {
      readDataFileTableSelectionCheckBoxes();
      slotEnableDisablePushButtons();
   }
}      

/**
 * Called when a header column is clicked for sorting.
 */
void 
GuiSumsDialog::slotDataFileTableHeaderColumnClicked(int item)
{
   switch (item) {
      case DATA_TABLE_FILE_CHECK_BOX_COLUMN:
      case DATA_TABLE_FILE_DATE_COLUMN:
         readDataFileTableSelectionCheckBoxes();
         sumsDataFileList.sort(SumsFileListFile::SORT_ORDER_DATE);
         loadDataFileTable();
         break;
      case DATA_TABLE_FILE_TYPE_COLUMN:
         readDataFileTableSelectionCheckBoxes();
         sumsDataFileList.sort(SumsFileListFile::SORT_ORDER_TYPE);
         loadDataFileTable();
         break;
      case DATA_TABLE_FILE_NAME_COLUMN:
         readDataFileTableSelectionCheckBoxes();
         sumsDataFileList.sort(SumsFileListFile::SORT_ORDER_NAME);
         loadDataFileTable();
         break;
      case DATA_TABLE_FILE_SUBDIR_COLUMN:
         break;
      case DATA_TABLE_FILE_STATE_COLUMN:
         break;
      case DATA_TABLE_FILE_COMMENT_COLUMN:
         break;
      case DATA_TABLE_NUMBER_OF_COLUMNS:
         break;
   }
}

/**
 * set the file selection check boxes
 */
void
GuiSumsDialog::setDataFileTableSelectionCheckBoxes()
{
   const int num = sumsDataFileList.getNumberOfSumsFiles();
   if (dataFileTable->rowCount() != num) {
      std::cout << "PROGRAM ERROR: GuiSumsDialog check boxes different size than list file."
                << std::endl;
      return;
   }

   dataFileTable->blockSignals(true);
   
   for (int i = 0; i < num; i++) {
      const SumsFileInfo* sfi = sumsDataFileList.getSumsFileInfo(i);

      QTableWidgetItem* item = dataFileTable->item(i, DATA_TABLE_FILE_CHECK_BOX_COLUMN);
      if (item != NULL) {
         if (sfi->getSelected()) {
            item->setCheckState(Qt::Checked);
         }
         else {
            item->setCheckState(Qt::Unchecked);
         }
      }
   }
   
   dataFileTable->blockSignals(false);
}

/**
 * read the file selection check boxes
 */
void
GuiSumsDialog::readDataFileTableSelectionCheckBoxes()
{
   const int num = sumsDataFileList.getNumberOfSumsFiles();
   if (dataFileTable->rowCount() != num) {
      std::cout << "PROGRAM ERROR: GuiSumsDialog check boxes different size than list file."
                << std::endl;
   }
   else {
      for (int i = 0; i < num; i++) {
         SumsFileInfo* sfi = sumsDataFileList.getSumsFileInfo(i);
         QTableWidgetItem* item = dataFileTable->item(i, DATA_TABLE_FILE_CHECK_BOX_COLUMN);
         sfi->setSelected(item->checkState() == Qt::Checked);
      }
   }
}

/**
 * called when select all files push button is pressed.
 */
void 
GuiSumsDialog::slotSelectAllDataFilesPushButton()
{
   sumsDataFileList.setAllFileSelectionStatus(true);
   setDataFileTableSelectionCheckBoxes();
   slotEnableDisablePushButtons();
}

/**
 * called when deselect all files push button is pressed.
 */
void 
GuiSumsDialog::slotDeselectAllDataFilesPushButton()
{
   sumsDataFileList.setAllFileSelectionStatus(false);
   setDataFileTableSelectionCheckBoxes();
   slotEnableDisablePushButtons();
}

/**
 * Set the number of rows in the table
 */
void
GuiSumsDialog::setDataFileTableNumberOfRows(const int num)
{
   dataFileTable->setRowCount(num);
   
   if (num > 0) {
      dataFileTable->blockSignals(true);
      
      for (int i = 0; i < num; i++) {
         QTableWidgetItem* checkItem = new QTableWidgetItem;
         checkItem->setFlags(Qt::ItemIsSelectable |
                             Qt::ItemIsUserCheckable |
                             Qt::ItemIsEnabled);
         checkItem->setCheckState(Qt::Checked);
         dataFileTable->setItem(i, DATA_TABLE_FILE_CHECK_BOX_COLUMN,
                                checkItem);
                                
         const Qt::ItemFlags flags = (Qt::ItemIsSelectable
                                      | Qt::ItemIsEnabled);
         QTableWidgetItem* dateItem = new QTableWidgetItem;
         dateItem->setFlags(flags);
         dataFileTable->setItem(i, DATA_TABLE_FILE_DATE_COLUMN, dateItem);

         QTableWidgetItem* typeItem = new QTableWidgetItem;
         typeItem->setFlags(flags);
         dataFileTable->setItem(i, DATA_TABLE_FILE_TYPE_COLUMN, typeItem);

         QTableWidgetItem* nameItem = new QTableWidgetItem;
         nameItem->setFlags(flags);
         dataFileTable->setItem(i, DATA_TABLE_FILE_NAME_COLUMN, nameItem);

         QTableWidgetItem* subDirItem = new QTableWidgetItem;
         subDirItem->setFlags(flags);
         dataFileTable->setItem(i, DATA_TABLE_FILE_SUBDIR_COLUMN, subDirItem);

         QTableWidgetItem* stateItem = new QTableWidgetItem;
         stateItem->setFlags(flags);
         dataFileTable->setItem(i, DATA_TABLE_FILE_STATE_COLUMN, stateItem);

         QTableWidgetItem* commentItem = new QTableWidgetItem;
         commentItem->setFlags(flags);
         dataFileTable->setItem(i, DATA_TABLE_FILE_COMMENT_COLUMN, commentItem);
      }
      
      dataFileTable->blockSignals(false);
   }
}

/**
 * Load the file list table
 */
void
GuiSumsDialog::loadDataFileTable()
{
   const int numFiles = sumsDataFileList.getNumberOfSumsFiles();
   if (numFiles < 0) {
      setDataFileTableNumberOfRows(0);
      return;
   }
   
   setDataFileTableNumberOfRows(numFiles);
   
   for (int i = 0; i < numFiles; i++) {
      const SumsFileInfo* sfi = sumsDataFileList.getSumsFileInfo(i);
      
      QTableWidgetItem* dateItem = dataFileTable->item(i, DATA_TABLE_FILE_DATE_COLUMN);
      dateItem->setText(sfi->getDate());

      QTableWidgetItem* typeItem = dataFileTable->item(i, DATA_TABLE_FILE_TYPE_COLUMN);
      typeItem->setText(sfi->getTypeName());

      QTableWidgetItem* nameItem = dataFileTable->item(i, DATA_TABLE_FILE_NAME_COLUMN);
      nameItem->setText(sfi->getNameWithoutPath());

      QTableWidgetItem* subDirItem = dataFileTable->item(i, DATA_TABLE_FILE_SUBDIR_COLUMN);
      subDirItem->setText(FileUtilities::dirname(sfi->getNameWithPath()));

      QTableWidgetItem* stateItem = dataFileTable->item(i, DATA_TABLE_FILE_STATE_COLUMN);
      stateItem->setText(sfi->getState());

      QTableWidgetItem* commentItem = dataFileTable->item(i, DATA_TABLE_FILE_COMMENT_COLUMN);
      commentItem->setText(sfi->getComment());
   }
   
   setDataFileTableSelectionCheckBoxes();
}

/**
 * Create the output directory section.
 */
QWidget*
GuiSumsDialog::createOutputDirectorySection()
{
   //
   // Pushbutton to select directory
   //
   QPushButton* directoryPushButton = new QPushButton("Select...");
   directoryPushButton->setFixedSize(directoryPushButton->sizeHint());
   directoryPushButton->setAutoDefault(false);
   QObject::connect(directoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDirectoryPushButton()));
                    
   //
   // Directory name line edit
   //
   directoryNameLineEdit = new QLineEdit;
   directoryNameLineEdit->setText(QDir::currentPath());
   QObject::connect(directoryNameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SLOT(slotEnableDisablePushButtons()));
   
   //
   // Group box and layout for output directory
   //
   QGroupBox* directoryGroupBox = new QGroupBox("Output Directory");
   QHBoxLayout* dirLayout = new QHBoxLayout(directoryGroupBox);
   dirLayout->addWidget(directoryPushButton);
   dirLayout->addWidget(directoryNameLineEdit);
   dirLayout->addStretch();
   
   //
   // Add help information
   //
   QWidget* infoWidget = addPageInformation(
                      "Output Directory Information",
                      "Press the \"Finish\" button to begin downloading files."
                      "\n\n"
                      "In the text box above, enter the name of the directory in which "
                      "the downloaded data files should be placed.  If the directory does "
                      "not exist, it will be created.  The \"Select\" button will display "
                      "a dialog for selecting existing directories.  Typically, when data "
                      "files are downloaded, they are placed in directories named "
                      "\"RIGHT_HEM\", \"LEFT_HEM\", or \"CEREBELLUM\".");
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(directoryGroupBox);
   layout->addWidget(infoWidget);
   
   return w;
}

/**
 * called when output directory select push button is pressed.
 */
void 
GuiSumsDialog::slotDirectoryPushButton()
{
   //
   // Pop up a file dialog for selecting a directory
   //
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::current());
   fd.setWindowTitle("Select Directory");
   fd.setFileMode(QFileDialog::Directory);
  
   //
   // Popup the dialog
   //
   if (fd.exec() == QDialog::Accepted) {   
      directoryNameLineEdit->setText(fd.selectedFiles().at(0));
   }
}

/**
 * Insert the session ID into the URL.
 */
QString
GuiSumsDialog::insertSessionIdIntoURL(const QString urlIn)
{
   QString urlString(urlIn);

   if (sumsSessionID.isEmpty() == false) {
      if (urlString.isEmpty() == false) {
      
         if (DebugControl::getDebugOn()) {
            std::cout << "URL: " << urlString.toAscii().constData() << std::endl;
         }
         
         int pos = urlString.indexOf('?');
         if (pos != -1) {
            urlString.insert(pos, sumsSessionID);
         }
         else {
            pos = urlString.indexOf(".do");
            if (pos != -1) {
               urlString.insert(pos + 3, sumsSessionID);
            }
         }
         
         if (DebugControl::getDebugOn()) {
            std::cout << "URL with session ID: " << urlString.toAscii().constData() << std::endl;
         }
/*
         if (DebugControl::getDebugOn()) {
            std::cout << std::endl;
            std::cout << "URL: " << urlString << std::endl;
         }
         QUrl url(urlString);
         QString fileName(url.fileName());
         fileName.append(";");
         fileName.append(sumsSessionID);
         url.setFileName(fileName);
         
         QString theURL(url);
         if (DebugControl::getDebugOn()) {
            std::cout << "Before decode: " << theURL << std::endl;
         }
         QUrl::decode(theURL);
         if (DebugControl::getDebugOn()) {
            std::cout << "After decode: " << theURL << std::endl;
         }
         urlString = theURL);
         
         if (DebugControl::getDebugOn()) {
            std::cout << "URL with session id: " << urlString << std::endl;
         }
*/
      }
   }
   
   return urlString;
}

/**
 * called when the download push button is pressed.
 */
void 
GuiSumsDialog::slotFinishPushButton()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   switch (dialogMode) { 
      case DIALOG_MODE_NONE:
         break;
      case DIALOG_MODE_ARCHIVE_ID:
         downloadDataFiles();
         break;
      case DIALOG_MODE_SPEC_FILE_ID:
         downloadDataFiles();
         break;
      case DIALOG_MODE_DOWNLOAD_ATLAS:
         downloadDataFiles();
         break;
      case DIALOG_MODE_DOWNLOAD_SEARCH:
         downloadDataFiles();
         break;
      case DIALOG_MODE_REFRESH_SPEC_FILE:
         break;
      case DIALOG_MODE_UPLOAD_FILES:
         uploadDataFiles();
         break;
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * upload data files.
 */
void
GuiSumsDialog::uploadDataFiles()
{
   //
   // Make sure we're still logged in
   //
   if (reloginToSums() == false) {
      return;
   }
   
   //
   // Copy list of files to upload since it may be modified when spec files are being uploaded
   //
   std::vector<QString> filesToUpload;
   std::vector<QString> filesNameForSums;
   std::vector<UPLOAD_FILE_TYPE> filesType;
   
   //
   // Files that will need to be deleted, typically spec files since paths are removed
   //
   std::vector<QString> filesToDelete;
   
   for (int i = 0; i < static_cast<int>(uploadFileNames.size()); i++) {
       const QString& name = uploadFileNames[i];
       
       //
       // Is this a spec file ?
       //
       if (StringUtilities::endsWith(name, SpecFile::getSpecFileExtension())) {
          //
          // See if the user also wants to upload the spec file' data files
          //
          QString msg(FileUtilities::basename(name));
          msg.append(" is a spec file.\n"
                     "Do you also want to upload its associated data files ?");
          const bool uploadDataFilesFlag =
              (GuiMessageBox::question(this, "Question", msg, "Yes", "No") == 0);
          
          QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
          
          if (uploadDataFilesFlag) {
             //
             // Read in the spec file
             //
             SpecFile sf;
             try {
                sf.readFile(name);
             }
             catch (FileException&) {
                continue;
             }
             
             //
             // Path of spec file
             //
             const QString specFilePath(FileUtilities::dirname(name));
             
             //
             // Get all of the files in the spec file
             //
             std::vector<QString> dataFiles;
             sf.getAllDataFilesInSpecFile(dataFiles, false);
             
             //
             // Temp name for spec file since paths of data files need to be removed
             //
             QString tempSpecFileName("temp_spec_file_for_upload_");
             tempSpecFileName.append(QString::number(i));
             tempSpecFileName.append(".spec");
             
             //
             // Remove the paths from all spec file data file entries
             //
             sf.removePathsFromAllFiles();
             
             //
             // Write the temp spec file
             //
             try {
                sf.writeFile(tempSpecFileName);
             }
             catch (FileException&) {
             }
             
             //
             // Add the spec file to the list of file to update
             //
             filesToUpload.push_back(tempSpecFileName);
             filesNameForSums.push_back(FileUtilities::basename(name));
             filesType.push_back(UPLOAD_FILE_TYPE_SPEC_FILE);
             filesToDelete.push_back(tempSpecFileName);
             
             //
             // Add path to data files' name and add to list of files to upload
             //
             for (int j = 0; j < static_cast<int>(dataFiles.size()); j++) {
                const QString dataFileName(dataFiles[j]);
                if (dataFileName.isEmpty() == false) {
                   QString dataFileNameFullPath(dataFileName);
                   QFileInfo fi(dataFileName);
                   if (fi.isRelative()) {
                      dataFileNameFullPath = specFilePath;
                      dataFileNameFullPath.append(QDir::separator());
                      dataFileNameFullPath.append(dataFileName);
                   }
                   filesToUpload.push_back(dataFileNameFullPath);
                   filesNameForSums.push_back(FileUtilities::basename(dataFileNameFullPath));
                   filesType.push_back(UPLOAD_FILE_TYPE_SPEC_FILE_DATA_FILE);
                }
             }
          }
          else {
             filesToUpload.push_back(name);
             filesNameForSums.push_back(FileUtilities::basename(name));
             filesType.push_back(UPLOAD_FILE_TYPE_OTHER_FILE);
          }
       }
       else if (name.isEmpty() == false) {
          filesToUpload.push_back(name);
          filesNameForSums.push_back(FileUtilities::basename(name));
          filesType.push_back(UPLOAD_FILE_TYPE_OTHER_FILE);
       }
   }
   
   //
   // See if any of the data files are volume files
   //
   std::vector<QString> volumeDataFiles;
   std::vector<UPLOAD_FILE_TYPE> volumeDataFileTypes;
   for (int i = 0; i < static_cast<int>(filesToUpload.size()); i++) {
      //
      // Get the extension
      //
      QString ext(".");
      ext.append(FileUtilities::filenameExtension(filesToUpload[i]));
      
      //
      // If a volume file, NOTE: ignore NIFTI/MINC volumes since they are in a single file.
      //
      if ((ext == SpecFile::getAfniVolumeFileExtension()) ||
          (ext == SpecFile::getWustlVolumeFileExtension()) ||
          (ext == SpecFile::getAnalyzeVolumeFileExtension())) {
         try {
            //
            // Read in the volume' header
            //
            VolumeFile vf;
            vf.readFile(filesToUpload[i], VolumeFile::VOLUME_READ_HEADER_ONLY);

            //
            // Determine the name of volume's data file.
            //
            QString inputVolumeName(vf.getDataFileName());

            //
            // See if the data does not file exist
            //
            if (QFile::exists(inputVolumeName) == false) {
               //
               // See if the gzipped version exists
               //
               QString temp(inputVolumeName);
               temp.append(".gz");
               if (QFile::exists(temp)) {
                  inputVolumeName.append(".gz");
                  volumeDataFiles.push_back(inputVolumeName);
                  volumeDataFileTypes.push_back(filesType[i]);
               }
               else {
                  //
                  // Even though it is missing, add it to the list of files so that
                  // it will be found as "missing" with all other files
                  //
                  volumeDataFiles.push_back(inputVolumeName);
                  volumeDataFileTypes.push_back(filesType[i]);
               }
            }
            else {
               volumeDataFiles.push_back(inputVolumeName);
               volumeDataFileTypes.push_back(filesType[i]);
            }
         }
         catch (FileException&) {
         }
      }
   }
   
   //
   // Loop through the list of volume data files that need to be uploaded
   //
   for (unsigned int mm = 0; mm < volumeDataFiles.size(); mm++) {
      //
      // See if the volume data file is already in the list of files to upload
      //
      std::vector<QString>::iterator iter = std::find(filesToUpload.begin(), 
                                                      filesToUpload.end(),
                                                      volumeDataFiles[mm]);
      if (iter != filesToUpload.end()) {
         //
         // It is already there so do not add it to the list of files
         //
      }
      //
      // See if volume data file is "gzipped"
      //
      else if (volumeDataFiles[mm].endsWith(".gz")) {
         //
         // Take off the ".gz" extension
         //
         QString volDataNameNoGZ(volumeDataFiles[mm]);
         
         //
         // If it exists without the ".gz" extension
         //
         iter = std::find(filesToUpload.begin(), 
                          filesToUpload.end(),
                          volDataNameNoGZ); 
         if (iter != filesToUpload.end()) {
            //
            // Find its offset and replace it (replacing non-gzip-name with gzip-name)
            //
            const int offset = iter - filesToUpload.begin();
            filesToUpload[offset] = volumeDataFiles[mm];
            filesType[offset] = volumeDataFileTypes[mm];
            filesNameForSums[offset] = FileUtilities::basename(volumeDataFiles[mm]);
         }
         else {
            filesToUpload.push_back(volumeDataFiles[mm]);
            filesType.push_back(volumeDataFileTypes[mm]);
            filesNameForSums.push_back(FileUtilities::basename(volumeDataFiles[mm]));
         }
      }
      else {
         filesToUpload.push_back(volumeDataFiles[mm]);
         filesType.push_back(volumeDataFileTypes[mm]);
         filesNameForSums.push_back(FileUtilities::basename(volumeDataFiles[mm]));
      }
   }
   
   //
   // Make sure all of the files exist
   //
   QString missingMessage;
   for (unsigned int nn = 0; nn < filesToUpload.size(); nn++) {
      if (QFile::exists(filesToUpload[nn]) == false) {
         if (missingMessage.isEmpty()) {
            missingMessage = 
               "The following files listed below are selected for upload but do not exist.\n"
               "If you are uploading a spec file, remove these non-existant files from\n"
               "the spec file prior to continuing.\n"
               "\n";
         }
         missingMessage.append(filesNameForSums[nn]);
         
         const QString path(FileUtilities::dirname(filesToUpload[nn]));
         if (path.length() > 1) {
            missingMessage.append(" (");
            missingMessage.append(path);
            missingMessage.append(")");
         }
         missingMessage.append("\n");
      }
   }
   if (missingMessage.isEmpty() == false) {
      //
      // Remove temporary files
      //
      for (int i = 0; i < static_cast<int>(filesToDelete.size()); i++) {
         QFile::remove(filesToDelete[i]);
      }

      //
      // display error messages
      //
      GuiMessageBox::critical(this, "ERROR", missingMessage, "OK");
      return;
   }
   
   //
   // Hide the dialog while uploading
   //
   hide();
   
   const int numFilesToUpload = static_cast<int>(filesToUpload.size());
   
   //
   // Create a progress dialog 
   //
   QProgressDialog progressDialog(this);
   progressDialog.setMaximum(numFilesToUpload + 1);
   progressDialog.setValue(0);
   progressDialog.setWindowTitle("Upload to SumsDB");
   progressDialog.show();
   
   QString errorMessages;
   QString successMessages;
   
   QTime timer;
   timer.start();

   //
   // parent ID for data files associated with a spec file
   //
   QString lastSpecFileParentID;
   
   for (int i = 0; i < numFilesToUpload; i++) {
   
      //
      // File name without path
      //
      const QString shortFileName(filesNameForSums[i]);
      if (DebugControl::getDebugOn()) {
         std::cout << "Uploading File: " << shortFileName.toAscii().constData()
                   << " =============================================" << std::endl;
      }
      
      //
      // Update the progress dialog
      //
      qApp->processEvents();  // note: qApp is global in QApplication
      if (progressDialog.wasCanceled()) {
         break;
      }
      const int progressFileNumber = i + 1;
      progressDialog.setValue(progressFileNumber);
      QString progressMessage("Uploading ");
      progressMessage.append(QString::number(progressFileNumber));
      progressMessage.append(" of ");
      progressMessage.append(QString::number(numFilesToUpload));
      progressMessage.append(" files\n");
      progressMessage.append(shortFileName);
      progressMessage.append("\n");
      progressMessage.append("Elapsed time: ");
      progressMessage.append(QString::number(timer.elapsed() * 0.001, 'f', 0));
      progressMessage.append(" seconds.");
      progressDialog.setLabelText(progressMessage);
        
      //
      // Is this a scene file
      //
      QString tempSceneFileName;
      QString ext(".");
      ext.append(FileUtilities::filenameExtension(filesToUpload[i]));
      if (ext == SpecFile::getSceneFileExtension()) {
         try {
            SceneFile sceneFile;
            sceneFile.readFile(filesToUpload[i]);
            tempSceneFileName = "temp_scene_file_for_sums.scene";
            sceneFile.removePathsFromAllSpecFileDataFileNames();
            sceneFile.writeFile(tempSceneFileName);
            filesToUpload[i] = tempSceneFileName;
         }
         catch (FileException& e) {
            tempSceneFileName = "";
         }
      }
      
      //
      // URL for uploading
      //
      QString url(databaseHostName);
      url.append("/sums/caretupload.do");
      url.append(sumsSessionID);
      //url.append("&caret_xml=yes");
      HttpFileDownload http(url, dataFileTimeoutSpinBox->value());
      http.setUploadFileName(filesToUpload[i], filesNameForSums[i]);
      http.setUploadFileComment(uploadCommentLineEdit->text());
      std::map<QString, QString> headerElements;
      if (sumsCookie.isEmpty() == false) {
         headerElements["Cookie"] = sumsCookie;
      }
      http.addToRequestHeader(headerElements);
      
      //
      // Additional items in multipart file
      //
      std::vector<QString> additionalBodyTags, additionalBodyValues;

      //
      // Is this file a data file associated with a spec file ?
      //
      if (filesType[i] == UPLOAD_FILE_TYPE_SPEC_FILE_DATA_FILE) {
         if (lastSpecFileParentID.isEmpty() == false) {
            additionalBodyTags.push_back("parent_id");
            additionalBodyValues.push_back(lastSpecFileParentID);
         }
      }
      
      //
      // Upload the file
      //
      http.uploadFileToSums(additionalBodyTags, additionalBodyValues);
      
      if (DebugControl::getDebugOn()) {
         std::cout << "------------------- received -------------------------------" << std::endl;
         std::cout << "Response code: " << http.getResponseCode() << std::endl;
         std::cout << "Error message: " << http.getErrorMessage().toAscii().constData() << std::endl;
         std::map<QString,QString> headerTagsOut = http.getResponseHeader();
         for (std::map<QString,QString>::iterator pos = headerTagsOut.begin();
              pos != headerTagsOut.end(); pos++) {
            std::cout << "Upload tag/value:  "
                      << pos->first.toAscii().constData() << "  " << pos->second.toAscii().constData() << std::endl;
         }
      }
      if (http.getDownloadSuccessful()) {
         //
         // Response content is a Sums File List File with info about file just uploaded
         //
         QString responseContent;
         http.getContent(responseContent);
         if (DebugControl::getDebugOn()) {
            std::cout << "Response content: " << responseContent.toAscii().constData() << std::endl;
            std::cout << std::endl;
         }
         SumsFileListFile flf;
       //  flf.readFileFromString(responseContent);
         try {
            flf.readFileFromArray(responseContent.toAscii().constData(), responseContent.length(),
                                  "sums_upload_file.dat");
         }
         catch (FileException& e) {
            errorMessages.append(e.whatQString());
         }
         if (flf.getNumberOfSumsFiles() != 1) {
            QString msg("Failed (response content != 1): ");
            msg.append(shortFileName);
            msg.append("\n");
            std::cout << "ERROR " << msg.toAscii().constData();
            errorMessages.append(msg);
         }
         else {
            const SumsFileInfo* sfi = flf.getSumsFileInfo(0);
            switch (filesType[i]) {
               case UPLOAD_FILE_TYPE_SPEC_FILE:
                  {
                     lastSpecFileParentID = sfi->getID();
                     successMessages.append("Spec File: ");
                     successMessages.append(shortFileName);
                     successMessages.append(" ID = ");
                     successMessages.append(lastSpecFileParentID);
                     successMessages.append("\n");
                  }
                  break;
               case UPLOAD_FILE_TYPE_OTHER_FILE:
                  successMessages.append("File: ");
                  successMessages.append(shortFileName);
                  successMessages.append(" ID = ");
                  successMessages.append(sfi->getID());
                  successMessages.append("\n");
                  lastSpecFileParentID = "";
                  break;
               case UPLOAD_FILE_TYPE_SPEC_FILE_DATA_FILE:
                  break;
            }
         }
      }
      else {
         QString msg("Failed (download unsuccessful): ");
         msg.append(shortFileName);
         msg.append("\n");
         std::cout << "ERROR " << msg.toAscii().constData();
         errorMessages.append(msg);
      }
      
      if (tempSceneFileName.isEmpty() == false) {
         QFile::remove(tempSceneFileName);
      }
   }
   
   QString timeMsg("Total upload time: ");
   timeMsg.append(StringUtilities::fromNumber(timer.elapsed() * 0.001));
   timeMsg.append("\n");

   //
   // Show the dialog since it was hidden
   //
   show();
      
   //
   // Remove the progress dialog
   //
   progressDialog.cancel();
   
   if (errorMessages.isEmpty()) {
      timeMsg.append("All files were successfully uploaded.\n");
      timeMsg.append(successMessages);
      GuiMessageBox::information(this, "SUCCESS", timeMsg, "OK");
   }
   else {
      QString msg(errorMessages);
      if (successMessages.isEmpty() == false) {
         successMessages.append("\n");
         successMessages.append("The following files were successfully uploaded\n");
         successMessages.append("(does not include data files in a spec file):\n");
         successMessages.append(successMessages);
      }
      GuiMessageBox::critical(this, "ERROR", errorMessages, "OK");
   }
   
   //
   // Remove temporary files
   //
   for (int i = 0; i < static_cast<int>(filesToDelete.size()); i++) {
      QFile::remove(filesToDelete[i]);
   }
}

/**
 * download data files.
 */
void
GuiSumsDialog::downloadDataFiles()
{
   //
   // Make sure we're still logged in
   //
   if (reloginToSums() == false) {
      return;
   }
   
   //
   // Number of items in table and in sums list file must match
   //
   const int numFiles = sumsDataFileList.getNumberOfSumsFiles();
   if (numFiles != dataFileTable->rowCount()) {
      GuiMessageBox::critical(this, "ERROR", 
         "PROGRAM ERROR: number of rows in table does not match number in sums list file",
         "OK");
      return;
   }
   
   //
   // Determine number of files that are to be downloaded
   //
   readDataFileTableSelectionCheckBoxes();
   int numFilesToDownload = 0;
   for (int i = 0; i < numFiles; i++) {
      if (sumsDataFileList.getSumsFileInfo(i)->getSelected()) {
         numFilesToDownload++;
      }
   }
   
   //
   // Save the current directory
   //
   const QString savedDirectory(QDir::currentPath());
   
   //
   // Create the output directory if necessary
   //
   const QString outputDirectoryName = directoryNameLineEdit->text();
   if (QFile::exists(outputDirectoryName) == false) {
      QString msg("Create Directory ");
      msg.append(outputDirectoryName);
      if (GuiMessageBox::question(this, "Create Directory", msg, "Yes", "Cancel") != 0) {
         return;
      }
      if (FileUtilities::createDirectory(outputDirectoryName) == false) {
         QString msg("Unable to create directory: \n");
         msg.append(outputDirectoryName);
         GuiMessageBox::critical(this, "ERROR", msg, "OK");
         return;
      }
   }
   
   //
   // Hide the dialog while uploading
   //
   hide();
   
   //
   // Set to the output directory
   //
   QDir::setCurrent(outputDirectoryName);
   
   //
   // Create a progress dialog 
   //
   QProgressDialog progressDialog(this);
   progressDialog.setMaximum(numFilesToDownload + 1);
   progressDialog.setValue(0);
   progressDialog.setWindowTitle("Download from SumsDB");
   progressDialog.show();
   
   QString errorMessages;
   
   QTime timer;
   timer.start();

   //
   // loop through each file
   //
   int progressFileNumber = 0;
   for (int i = 0; i < numFiles; i++) {
      //
      // Is file selected for download
      //
      if (sumsDataFileList.getSumsFileInfo(i)->getSelected()) {
         const SumsFileInfo* sfi = sumsDataFileList.getSumsFileInfo(i);
         const QString name(sfi->getNameWithPath());
         QString url(sfi->getURL());
        
         //
         // Add the session ID to the URL
         //
         url = insertSessionIdIntoURL(url);
         
         //
         // Update the progress dialog
         //
         qApp->processEvents();  // note: qApp is global in QApplication
         if (progressDialog.wasCanceled()) {
            break;
         }
         progressDialog.setValue(progressFileNumber);
         progressFileNumber++;
         QString progressMessage("Downloading ");
         progressMessage.append(QString::number(progressFileNumber));
         progressMessage.append(" of ");
         progressMessage.append(QString::number(numFilesToDownload));
         progressMessage.append(" files\n");
         progressMessage.append(name);
         progressMessage.append("\n");
         progressMessage.append("Elapsed time: ");
         progressMessage.append(QString::number(timer.elapsed() * 0.001, 'f', 0));
         progressMessage.append(" seconds.");
         progressDialog.setLabelText(progressMessage);
         
         //
         // See if files will be gzipped
         //
         QString downloadName(name);
         bool gzipFlag = false;
         if (url.indexOf("downloadgzip") != -1) {
            gzipFlag = true;
            downloadName.append(".gz");
         }
         
         //
         // See if a subdirectory needs to be created, and, if so, create it
         //
         const QString subDir(FileUtilities::dirname(downloadName));
         if (DebugControl::getDebugOn()) {
            std::cout << "download name: " << downloadName.toAscii().constData()
                      << " path: " << subDir.toAscii().constData() << std::endl;
         }
         if (subDir.isEmpty() == false) {
            if ((subDir != ".") && (subDir != "..")) {
               QString subDirPath(outputDirectoryName);
               subDirPath.append(QDir::separator());
               subDirPath.append(subDir);
               if (QFile::exists(subDirPath) == false) {
                  if (FileUtilities::createDirectory(subDirPath) == false) {
                     QString msg("Unable to create directory: \n");
                     msg.append(subDirPath);
                     GuiMessageBox::critical(this, "ERROR", msg, "OK");
                     show();
                     return;
                  }
               }
            }
         }         
         
         //
         // If the file fails to download the first time, try it again
         //
         const int maxTimes = downloadNumTimesSpinBox->value();
         for (int k = 0; k < maxTimes; k++) {
            //
            // Download the file
            //
            QString downloadErrorMessage;
            const bool fileOK = FileUtilities::downloadFileWithHttpGet(url,
                                                            downloadName,
                                                            dataFileTimeoutSpinBox->value(),
                                                            downloadErrorMessage);
                                            
            //
            // File failed to download
            //                                
            if (fileOK == false) {
               if (errorMessages.isEmpty() == false) {
                  errorMessages.append("\n");
               }
               QString msg(FileUtilities::basename(name));
               msg.append("  ");
               msg.append(downloadErrorMessage);
               errorMessages.append(msg);
             }
             else {
                //
                // Was a gzip file requested
                //
                if (gzipFlag) {
                   //
                   // gunzip the file
                   //
                   if (FileUtilities::gunzipFile(downloadName, name)) {
                      //
                      // Remove the gzipped file
                      //
                      QFile::remove(downloadName);
                      
                      //
                      // Is gunzipped file the correct size ?
                      //
                      QFileInfo fi(name);
                      if (fi.size() == static_cast<unsigned int>(sfi->getSize())) {
                         //
                         // File OK, get out of loop
                         //
                         k = maxTimes;
                         
                         //
                         // See if a spec file
                         //
                         if (StringUtilities::endsWith(name, SpecFile::getSpecFileExtension())) {
                            if (preferencesFile != NULL) {
                               QString s(outputDirectoryName);
                               s.append("/");
                               s.append(name);
                               preferencesFile->addToRecentSpecFiles(s, true);
                            }
                         }
                      }
                      //
                      // Was this the last try
                      //
                      else if (k == (maxTimes - 1)) {
                         //
                         // File failed
                         //
                         if (errorMessages.isEmpty() == false) {
                            errorMessages.append("\n");
                         }
                         QString msg("Download error - incorrect size: ");
                         msg.append(sfi->getNameWithoutPath());
                         errorMessages.append(msg);
                      }
                      else {
                         std::cout << "Info: download of " << sfi->getNameWithoutPath().toAscii().constData()
                                   << " failed.  Will try again." << std::endl;
                         std::cout << "   Size: " << fi.size() 
                                   << "   SumsDB size: " << sfi->getSize() << std::endl;
                      }
                   }
                }
                //
                // File is not gzipped
                //
                else {
                   //
                   // File OK, get out of loop
                   //
                   k = maxTimes;
                   
                   //
                   // See if a spec file
                   //
                   if (StringUtilities::endsWith(name, SpecFile::getSpecFileExtension())) {
                      if (preferencesFile != NULL) {
                         QString s(outputDirectoryName);
                         s.append("/");
                         s.append(name);
                         preferencesFile->addToRecentSpecFiles(s, true);
                      }
                   }
                }
             }
          }
      }
   }
   
   //
   // Hide the dialog while uploading
   //
   show();
   
   QString timeMsg("Total download time: ");
   timeMsg.append(StringUtilities::fromNumber(timer.elapsed() * 0.001));
   timeMsg.append("\n");
   
   //
   // Restore the saved directory
   //
   QDir::setCurrent(savedDirectory);
    
   //
   // Remove the progress dialog
   //
   progressDialog.cancel();
   
   if (errorMessages.isEmpty()) {
      timeMsg.append("All files were successfully downloaded.");
      GuiMessageBox::information(this, "SUCCESS", timeMsg, "OK");
   }
   else {
      timeMsg.append(errorMessages);
      GuiMessageBox::critical(this, "ERROR", timeMsg, "OK");
   }
}

/**
 * Get a list of spec files
 */
bool
GuiSumsDialog::getSumsSpecFileListing()
{
   //
   // Make sure we're still logged in
   //
   if (reloginToSums() == false) {
      return false;
   }
   
   //
   // Clear the file list and the data file table
   //
   sumsSpecFileList.clear(); 
   setSpecFileTableNumberOfRows(0);
   
   QString url;
   switch (dialogMode) {
      case DIALOG_MODE_NONE:
         break;
      case DIALOG_MODE_ARCHIVE_ID:
         break;
      case DIALOG_MODE_SPEC_FILE_ID:
         break;
      case DIALOG_MODE_DOWNLOAD_ATLAS:
         url = databaseHostName;
         url.append("/sums/advancedsearch.do");
         url.append(sumsSessionID);
         url.append("?property_label=category&property_value=ATLAS");
         
         if (downloadAtlasSpeciesComboBox->currentIndex() > 0) {
            appendParameterToURL(url, "property_label", "species");
            appendParameterToURL(url, "property_value",
                                 downloadAtlasSpeciesComboBox->currentText());
         }
         if (downloadAtlasHemisphereComboBox->currentIndex() > 0) {
            appendParameterToURL(url, "property_label", "hem_flag");
            appendParameterToURL(url, "property_value",
                                 downloadAtlasHemisphereComboBox->currentText());
         }
         if (downloadAtlasSpaceComboBox->currentIndex() > 0) {
            appendParameterToURL(url, "property_label", "space");
            appendParameterToURL(url, "property_value",
                                 downloadAtlasSpaceComboBox->currentText());
         }
         appendParameterToURL(url, "caret_xml", "yes");
         break;
      case DIALOG_MODE_DOWNLOAD_SEARCH:
         {
            url = databaseHostName;
            url.append("/sums/advancedsearch.do");
            url.append(sumsSessionID);
            url.append("?filetype=spec");
            appendSearchParametersToURL(url, true);
         /*
            const QString searchName(downloadSearchFileNameLineEdit->text());
            if (searchName.empty() == false) {
               appendParameterToURL(url, "filename", searchName);
            }
            if (downloadSearchSpeciesComboBox->currentIndex() > 0) {
               appendParameterToURL(url, "property_label", "species");
               appendParameterToURL(url, "property_value", 
                                    downloadSearchSpeciesComboBox->currentText());
            }
            if (downloadSearchHemisphereComboBox->currentIndex() > 0) {
               appendParameterToURL(url, "property_label", "hem_flag");
               appendParameterToURL(url, "property_value", 
                                    downloadSearchHemisphereComboBox->currentText());
            }
            if (downloadSearchSpaceComboBox->currentIndex() > 0) {
               appendParameterToURL(url, "property_label", "space");
               appendParameterToURL(url, "property_value",
                                    downloadSearchSpaceComboBox->currentText());
            }
            appendParameterToURL(url, "caret_xml", "yes");
         */
         }
         break;
      case DIALOG_MODE_REFRESH_SPEC_FILE:
         break;
      case DIALOG_MODE_UPLOAD_FILES:
         break;
   }

   if (url.isEmpty()) {
      GuiMessageBox::critical(this, "Program Error", "Spec File Listing URL is empty.", "OK");     
      return false;
   }     
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Spec File URL: " << url.toAscii().constData() << std::endl;
   }
   
   //
   // Get the archives list of files
   //
   QString contents;
   QString errorMessage;
   std::map<QString,QString> headerTags;
   if (FileUtilities::downloadFileWithHttpGet(url, searchTimeoutSpinBox->value(), 
                                   contents, errorMessage, &headerTags) == false) {
      GuiMessageBox::critical(this, "ERROR", errorMessage, "OK");
      return false;
   }
   
   // Parse the file listing
   //
   try {
      //sumsSpecFileList.readFileFromString(contents);
      sumsSpecFileList.readFileFromArray(contents.toAscii().constData(), contents.length(), 
                                         "sums_file_list.dat");
   }
   catch (FileException& e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      return false;
   }

   if (sumsSpecFileList.getNumberOfSumsFiles() <= 0) {
      GuiMessageBox::critical(this, "ERROR", "No matches found.", "OK");
      return false;
   }
   
   //
   // Default to no file selected
   //
   sumsSpecFileList.setAllFileSelectionStatus(false);
   
   //
   // Load the data file table
   //  
   loadSpecFileTable();
  
   return true;
}

/**
 * Append a parameter and its value to a URL.
 */
void
GuiSumsDialog::appendParameterToURL(QString& url, 
                                    const QString& paramName,
                                    const QString& paramValue)
{
   if (paramName.isEmpty() || paramValue.isEmpty()) {
      return;
   }
   
   const int len = url.length();
   if (len > 0) {
      const QCharRef lastChar = url[len - 1];
      if (lastChar != '?') {
         url.append("&");
      }
      url.append(paramName);
      url.append("=");
      url.append(paramValue);
   }
}

/**
 * Append search parameters onto the URL.
 */
void
GuiSumsDialog::appendSearchParametersToURL(QString& url, const bool specFileSearch)
{
   //
   // add on file name search
   //
   appendParameterToURL(url, "filename", downloadSearchFileNameLineEdit->text());

   //
   // add on keyword search
   //
   appendParameterToURL(url, "keyword", downloadSearchKeywordLineEdit->text());
   
   //
   // add on comment search
   //
   const QString comment(downloadSearchFileCommentLineEdit->text());
   if (comment.isEmpty() == false) {
      appendParameterToURL(url, "property_name", "comment");
      appendParameterToURL(url, "property_value", comment);
   }
   
   //
   // Are we searching for something OTHER than a spec file
   //
   if (specFileSearch == false) {
      const int item = downloadSearchFileTypeComboBox->currentIndex();
      if (item > SEARCH_FILE_INDEX_ALL_EXCEPT_SPEC) {
         if ((item >= 0) && (item < downloadSearchFileTypeComboBox->count())) {
            appendParameterToURL(url, "filetype", downloadSearchFileExtensions[item]);
         }
      }
   }
   
   //
   // Area we searching for spec files
   //
   if (specFileSearch) {
      //
      // Add species search
      //
      if (downloadSearchSpeciesComboBox->currentIndex() > 0) {
         appendParameterToURL(url, "property_label", "species");
         appendParameterToURL(url, "property_value", 
                              downloadSearchSpeciesComboBox->currentText());
      }
      
      //
      // Add hemisphere search
      //
      if (downloadSearchHemisphereComboBox->currentIndex() > 0) {
         appendParameterToURL(url, "property_label", "hem_flag");
         appendParameterToURL(url, "property_value", 
                              downloadSearchHemisphereComboBox->currentText());
      }
      
      //
      // add space search
      //
      if (downloadSearchSpaceComboBox->currentIndex() > 0) {
         appendParameterToURL(url, "property_label", "space");
         appendParameterToURL(url, "property_value",
                              downloadSearchSpaceComboBox->currentText());
      }
   }
   
   //
   // Add date search
   //
   if (downloadSearchDateCheckBox->isChecked()) {
      appendParameterToURL(url, "useDate", "true");
      
      const QDate startDate = downloadSearchStartDateEdit->date();
      appendParameterToURL(url, "minMonth", startDate.toString("MM"));
      appendParameterToURL(url, "minDay",   startDate.toString("dd"));
      appendParameterToURL(url, "minYear",  startDate.toString("yyyy"));
      
      const QDate endDate = downloadSearchEndDateEdit->date();
      appendParameterToURL(url, "maxMonth", endDate.toString("MM"));
      appendParameterToURL(url, "maxDay",   endDate.toString("dd"));
      appendParameterToURL(url, "maxYear",  endDate.toString("yyyy"));
   }
   
   //
   // Want our results in XML format
   //
   appendParameterToURL(url, "caret_xml", "yes");
}

/**
 * Create the listing of spec files.
 */
QWidget* 
GuiSumsDialog::createSpecFileListSection()
{
   //
   // Table containing list of files
   //
   specFileTable = new QTableWidget;
   QObject::connect(specFileTable, SIGNAL(cellChanged(int,int)),
                    this, SLOT(slotSpecFileTableItemChanged(int,int)));
   specFileTable->setColumnCount(SPEC_TABLE_NUMBER_OF_COLUMNS);
// QT4  specFileTable->setColumnReadOnly(SPEC_TABLE_FILE_DATE_COLUMN, true);
//   specFileTable->setColumnReadOnly(SPEC_TABLE_FILE_NAME_COLUMN, true);
//   specFileTable->setColumnReadOnly(SPEC_TABLE_FILE_COMMENT_COLUMN, true);

   specFileTable->setColumnWidth(SPEC_TABLE_FILE_NAME_COLUMN, 500);
   specFileTable->setColumnWidth(SPEC_TABLE_FILE_COMMENT_COLUMN, 500);

   //
   // Spec file table headers
   //
   QHeaderView*  header = specFileTable->horizontalHeader();
   QObject::connect(header, SIGNAL(sectionClicked(int)),
                    this, SLOT(slotSpecFileTableHeaderColumnClicked(int)));
                    
   //
   // Set column header labels
   //
   QStringList headerLabels;
   headerLabels << "Download";
   headerLabels << "Date";
   headerLabels << "Spec File Name";
   headerLabels << "Comment";
   specFileTable->setHorizontalHeaderLabels(headerLabels);
   //header->setLabel(SPEC_TABLE_FILE_CHECK_BOX_COLUMN, "Download");
   //header->setLabel(SPEC_TABLE_FILE_DATE_COLUMN,      "Date");
   //header->setLabel(SPEC_TABLE_FILE_NAME_COLUMN,      "Spec File Name");
   //header->setLabel(SPEC_TABLE_FILE_COMMENT_COLUMN,   "Comment");

   //
   // Group box and layout for spec file selections
   //
   QGroupBox* specGroupBox = new QGroupBox("Spec File Selections");
   QVBoxLayout* specGroupLayout = new QVBoxLayout(specGroupBox);
   specGroupLayout->addWidget(specFileTable);
   
   //
   // Add help information
   //
   QWidget* infoWidget = addPageInformation(
                      "Spec Files Information",
                      "Select one spec file.  The next page will list the data files "
                      "associated with the selected spec file.");
   
   QWidget* w = new QWidget;
   QVBoxLayout* layout = new QVBoxLayout(w);
   layout->addWidget(specGroupBox);
   layout->addWidget(infoWidget);
   
   return w;                                   
}
      
/**
 * Called when a spec file header column is clicked for sorting.
 */
void 
GuiSumsDialog::slotSpecFileTableHeaderColumnClicked(int item)
{
   switch (item) {
      case SPEC_TABLE_FILE_CHECK_BOX_COLUMN:
         break;
      case SPEC_TABLE_FILE_DATE_COLUMN:
         sumsSpecFileList.sort(SumsFileListFile::SORT_ORDER_DATE);
         loadSpecFileTable();
         break;
      case SPEC_TABLE_FILE_NAME_COLUMN:
         sumsSpecFileList.sort(SumsFileListFile::SORT_ORDER_NAME);
         loadSpecFileTable();
         break;
      case SPEC_TABLE_FILE_COMMENT_COLUMN:
         break;
      case SPEC_TABLE_NUMBER_OF_COLUMNS:
         break;
   }
}

/**
 * Called when a spec file table item is changed.
 */
void 
GuiSumsDialog::slotSpecFileTableItemChanged(int row, int col)
{
   if (col == SPEC_TABLE_FILE_CHECK_BOX_COLUMN) {
      //
      // Make the check boxes behave like radio buttons (mutually exclusive)
      //
      sumsSpecFileList.setAllFileSelectionStatus(false);
      SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(row);
      sfi->setSelected(true);
      setSpecFileTableSelectionCheckBoxes();
   }
   slotEnableDisablePushButtons();
}
      
/**
 * Set the number of rows in the spec file table.
 */
void 
GuiSumsDialog::setSpecFileTableNumberOfRows(const int num)
{
   specFileTable->setRowCount(num);
   specFileTable->blockSignals(true);
   
   if (num > 0) {
      for (int i = 0; i < num; i++) {
         QTableWidgetItem* checkItem = new QTableWidgetItem;
         checkItem->setFlags(Qt::ItemIsSelectable |
                             Qt::ItemIsUserCheckable |
                             Qt::ItemIsEnabled);
         checkItem->setCheckState(Qt::Checked);
         specFileTable->setItem(i, SPEC_TABLE_FILE_CHECK_BOX_COLUMN,
                                checkItem);
                                
         const Qt::ItemFlags flags = (Qt::ItemIsSelectable
                                      | Qt::ItemIsEnabled);
         QTableWidgetItem* dateItem = new QTableWidgetItem;
         dateItem->setFlags(flags);
         specFileTable->setItem(i, SPEC_TABLE_FILE_DATE_COLUMN, dateItem);

         QTableWidgetItem* nameItem = new QTableWidgetItem;
         nameItem->setFlags(flags);
         specFileTable->setItem(i, SPEC_TABLE_FILE_NAME_COLUMN, nameItem);

         QTableWidgetItem* commentItem = new QTableWidgetItem;
         commentItem->setFlags(flags);
         specFileTable->setItem(i, SPEC_TABLE_FILE_COMMENT_COLUMN, commentItem);
      }
   }
   
   specFileTable->blockSignals(false);
}

/**
 * load the spec file table.
 */
void 
GuiSumsDialog::loadSpecFileTable()
{
   const int numFiles = sumsSpecFileList.getNumberOfSumsFiles();
   if (numFiles < 0) {
      setSpecFileTableNumberOfRows(0);
      return;
   }

   setSpecFileTableNumberOfRows(numFiles);

   specFileTable->blockSignals(true);
   
   for (int i = 0; i < numFiles; i++) {
      const SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(i);
      QTableWidgetItem* dateItem = specFileTable->item(i, SPEC_TABLE_FILE_DATE_COLUMN);
      dateItem->setText(sfi->getDate());
      
      QTableWidgetItem* nameItem = specFileTable->item(i, SPEC_TABLE_FILE_NAME_COLUMN);
      nameItem->setText(sfi->getNameWithoutPath());
      
      QTableWidgetItem* commentItem = specFileTable->item(i, SPEC_TABLE_FILE_COMMENT_COLUMN);
      commentItem->setText(sfi->getComment());
   }

   specFileTable->blockSignals(false);

   setSpecFileTableSelectionCheckBoxes();
}      

/**
 * set the spec file selection check boxes.
 */
void 
GuiSumsDialog::setSpecFileTableSelectionCheckBoxes()
{
   const int num = specFileTable->rowCount();
   if (num != sumsSpecFileList.getNumberOfSumsFiles()) {
      std::cout << "PROGRAM ERROR: spec file table not same as sumsSpecFileList" << std::endl;
      return;
   }
   
   specFileTable->blockSignals(true);
   
   for (int i = 0; i < num; i++) {
      SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(i);
      QTableWidgetItem* item = specFileTable->item(i, SPEC_TABLE_FILE_CHECK_BOX_COLUMN);
      if (item != NULL) {
         if (sfi->getSelected()) {
            item->setCheckState(Qt::Checked);
         }
         else {
            item->setCheckState(Qt::Unchecked);
         }
      }
   }
   
   specFileTable->blockSignals(false);
}

/**
 * Get a SuMS data file listing.  Returns true if listing is valid
 */
bool 
GuiSumsDialog::getSumsDataFileListing(const QString& specFileID)
{
   //
   // Make sure we're still logged in
   //
   if (reloginToSums() == false) {
      return false;
   }
   
   bool excludeSpecFiles = false;
   bool deselectAllDataFiles = false;
   bool removeAllPaths = false;
   
   //
   // Clear the file list and the data file table
   //
   sumsDataFileList.clear();
   setDataFileTableNumberOfRows(0);
      
   //
   // Set the archive's URL based upon the mode
   //
   QString archiveURL;
   switch (dialogMode) {
      case DIALOG_MODE_NONE:
         break;
      case DIALOG_MODE_ARCHIVE_ID:
         archiveURL = databaseHostName;
         archiveURL.append("/sums/archivelist.do");
         archiveURL.append(sumsSessionID);
         archiveURL.append("?");
         appendParameterToURL(archiveURL, "archive_id",
                              downloadArchiveNumberLineEdit->text());
         appendParameterToURL(archiveURL, "caret_xml", "yes");
         break;
      case DIALOG_MODE_SPEC_FILE_ID:
         archiveURL = databaseHostName;
         archiveURL.append("/sums/specfile.do");
         archiveURL.append(sumsSessionID);
         archiveURL.append("?");
         appendParameterToURL(archiveURL, "archive_id",
                              downloadSpecNumberLineEdit->text());
         appendParameterToURL(archiveURL, "caret_xml", "yes");
         break;
      case DIALOG_MODE_DOWNLOAD_ATLAS:
         archiveURL = databaseHostName;
         archiveURL.append("/sums/specfile.do");
         archiveURL.append(sumsSessionID);
         archiveURL.append("?");
         appendParameterToURL(archiveURL, "filetype", "spec");
         appendParameterToURL(archiveURL, "state", "active");
         appendParameterToURL(archiveURL, "archive_id", specFileID);
         appendParameterToURL(archiveURL, "caret_xml", "yes");
         break;
      case DIALOG_MODE_DOWNLOAD_SEARCH:
         {
            const int item = downloadSearchFileTypeComboBox->currentIndex();
            if (item == SEARCH_FILE_INDEX_SPEC) {
               //
               // Get a spec file's data files.
               //
               archiveURL = databaseHostName;
               archiveURL.append("/sums/specfile.do");
               archiveURL.append(sumsSessionID);
               archiveURL.append("?");
               appendParameterToURL(archiveURL, "filetype", "spec");
               appendParameterToURL(archiveURL, "state", "active");
               appendParameterToURL(archiveURL, "archive_id", specFileID);
               appendParameterToURL(archiveURL, "caret_xml", "yes");
            }
            else {
               excludeSpecFiles = true;
               deselectAllDataFiles = true;
               removeAllPaths = true;
               archiveURL = databaseHostName;
               archiveURL.append("/sums/advancedsearch.do");
               archiveURL.append(sumsSessionID);
               archiveURL.append("?");
               appendSearchParametersToURL(archiveURL, false);
            /*
               const QString searchName(downloadSearchFileNameLineEdit->text());
               if (searchName.empty() == false) {
                  appendParameterToURL(archiveURL, "filename", searchName);
               }
               if ((item >= 0) && (item < downloadSearchFileTypeComboBox->count())) {
                  const QString searchExt(downloadSearchFileExtensions[item]);
                  if (searchExt.empty() == false) {
                     appendParameterToURL(archiveURL, "filetype", searchExt);
                  }
               }
               appendParameterToURL(archiveURL, "caret_xml", "yes");
            */
            }
         }
         break;
      case DIALOG_MODE_REFRESH_SPEC_FILE:
         break;
      case DIALOG_MODE_UPLOAD_FILES:
         break;
   }
   
   if (archiveURL.isEmpty()) {
      GuiMessageBox::critical(this, "Program Error", "Archive URL is empty.", "OK");
      return false;
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Search URL: " << archiveURL.toAscii().constData() << std::endl;
   }
   
   //
   // Get the archives list of files
   //
   QString contents;
   QString errorMessage;
   std::map<QString,QString> headerTags;
   if (FileUtilities::downloadFileWithHttpGet(archiveURL, searchTimeoutSpinBox->value(), 
                                   contents, errorMessage, &headerTags) == false) {
      GuiMessageBox::critical(this, "ERROR", errorMessage, "OK");
      return false;
   }
   
   //
   // if debugging print the file received
   //
   if (DebugControl::getDebugOn()) {
      std::cout << std::endl;
      std::cout << "spec listing length " << contents.length() << std::endl;
      std::cout << contents.toAscii().constData() << std::endl;
      std::cout << std::endl;
   }
   

   //
   // Parse the file listing
   //
   try {
      if (excludeSpecFiles) {
         sumsDataFileList.setExcludeSpecFileFlag(excludeSpecFiles);
      }
      // sumsDataFileList.readFileFromString(contents);
      sumsDataFileList.readFileFromArray(contents.toAscii().constData(), contents.length(),
                                         "sums_data_file_list.dat");
      if (deselectAllDataFiles) {
         sumsDataFileList.setAllFileSelectionStatus(false);
      }
      if (removeAllPaths) {
         sumsDataFileList.removePathsFromAllFiles();
      }
   }
   catch (FileException& e) {
      GuiMessageBox::critical(this, "ERROR", e.whatQString(), "OK");
      return false;
   }
   
   if (sumsDataFileList.getNumberOfSumsFiles() <= 0) {
      GuiMessageBox::critical(this, "ERROR", "File does not exist in SumsDB", "OK");
      return false;
   }

   //
   // If the files the same directory prefix, remove it from the files and
   // add it onto the current directory and place in output directory line edit.
   //
   const QString prefix(sumsDataFileList.getCommonSubdirectoryPrefix());
   if (prefix.isEmpty() == false) {
      sumsDataFileList.removeSubdirectoryPrefix();
      QString outputDir = QDir::currentPath();
      outputDir.append(QDir::separator());
      outputDir.append(prefix);
      directoryNameLineEdit->blockSignals(true);
      directoryNameLineEdit->setText(outputDir);
      directoryNameLineEdit->blockSignals(false);
   }
   
   //
   // Load the data file table
   //   
   loadDataFileTable();
   
   return true;
}

/**
 * Overrides parents close method.
 */
void
GuiSumsDialog::done(int)
{
   //
   // Do nothing - overriding this method prevents the window from closing
   // when the Finish button is pressed.
   //
}

/**
 * Called to select the back page.
 */
void 
GuiSumsDialog::slotBackPushButton()
{
   QWidget* currentPage = pagesStackedWidget->currentWidget();
   
   QWidget* prevPage = NULL;
   
   if (currentPage == pageDatabaseLogin) {
   }
   else if (currentPage == pageDatabaseUserInformation) {
      prevPage = pageDatabaseLogin;
   }
   else if (currentPage == pageDatabaseMode) {
      prevPage = pageDatabaseUserInformation;
   }
   else if (currentPage == pageDownloadAtlas) {
      prevPage = pageDatabaseMode;
   }
   else if (currentPage == pageDownloadSearch) {
      prevPage = pageDatabaseMode;
   }
   else if (currentPage == pageRefreshSpecFile) {
      prevPage = pageDatabaseMode;
   }
   else if (currentPage == pageUploadFiles) {
      prevPage = pageDatabaseMode;
   }
   else if (currentPage == pageSpecFileSelection) {
      switch (dialogMode) {
         case DIALOG_MODE_NONE:
            break;
         case DIALOG_MODE_ARCHIVE_ID:
            break;
         case DIALOG_MODE_SPEC_FILE_ID:
            break;
         case DIALOG_MODE_DOWNLOAD_ATLAS:
            prevPage = pageDownloadAtlas;
            break;
         case DIALOG_MODE_DOWNLOAD_SEARCH:
            prevPage = pageDownloadSearch;
            break;
         case DIALOG_MODE_REFRESH_SPEC_FILE:
            break;
         case DIALOG_MODE_UPLOAD_FILES:
            break;
      }
   }
   else if (currentPage == pageDataFileSelection) {
      switch (dialogMode) {
         case DIALOG_MODE_NONE:
            break;
         case DIALOG_MODE_ARCHIVE_ID:
            prevPage = pageDatabaseMode;
            break;
         case DIALOG_MODE_SPEC_FILE_ID:
            prevPage = pageDatabaseMode;
            break;
         case DIALOG_MODE_DOWNLOAD_ATLAS:
            prevPage = pageSpecFileSelection;
            break;
         case DIALOG_MODE_DOWNLOAD_SEARCH:
            prevPage = pageDownloadSearch;
            break;
         case DIALOG_MODE_REFRESH_SPEC_FILE:
            break;
         case DIALOG_MODE_UPLOAD_FILES:
            break;
      }
   }
   else if (currentPage == pageOutputDirectory) {
      switch (dialogMode) {
         case DIALOG_MODE_NONE:
            break;
         case DIALOG_MODE_ARCHIVE_ID:
            prevPage = pageDataFileSelection;
            break;
         case DIALOG_MODE_SPEC_FILE_ID:
            prevPage = pageDataFileSelection;
            break;
         case DIALOG_MODE_DOWNLOAD_ATLAS:
            prevPage = pageDataFileSelection;
            break;
         case DIALOG_MODE_DOWNLOAD_SEARCH:
            prevPage = pageDataFileSelection;
            break;
         case DIALOG_MODE_REFRESH_SPEC_FILE:
            break;
         case DIALOG_MODE_UPLOAD_FILES:
            break;
      }
   }
   
   if (prevPage != NULL) {
      showPage(prevPage);
   }
   
   slotEnableDisablePushButtons();
}
      
/**
 * Called to select the next page.
 */
void
GuiSumsDialog::slotNextPushButton()
{   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   QWidget* currentPage = pagesStackedWidget->currentWidget();

   //
   // Determine the next page for display
   //
   QWidget* nextPage = NULL;
   
   if (currentPage == pageDatabaseLogin) {
      sumsSessionID = "";
      sumsCookie = "";
      uploadButtonValid = false;
      dialogMode = DIALOG_MODE_NONE;
      
      modeDownloadArchiveRadioButton->setChecked(false);
      modeDownloadSpecRadioButton->setChecked(false);
      modeDownloadAtlasRadioButton->setChecked(false);
      modeDownloadSearchRadioButton->setChecked(false);
      modeRefreshSpecRadioButton->setChecked(false);
      modeUploadFilesRadioButton->setChecked(false);
      
      databaseUserName = databaseUserNameLineEdit->text();
      databasePassword = databasePasswordLineEdit->text();

      databaseHostName = "";
      const int hostIndex = databaseSelectionComboBox->currentIndex();
      if (hostIndex >= 0) {
         databaseHostName = availableDatabaseHostNames[hostIndex];
      }
      if (databaseHostName.isEmpty()) {
         GuiMessageBox::critical(this, "ERROR", "No Hostname is selected.", "OK");
         return;
      }

      //
      // Login to SuMS successful ?
      //
      if (loginToSums()) {
         if (preferencesFile != NULL) {
            //
            // save login to preferences
            //
            preferencesFile->setSumsDatabaseDataFileTimeout(dataFileTimeoutSpinBox->value());
            preferencesFile->setSumsLoginInformation(databaseUserName,
                                                     databasePassword,
                                                     databaseHostName,
                                                     loginUserNameRadioButton->isChecked(),
                                                     saveLoginCheckBox->isChecked());
            try {
               preferencesFile->writeFile(preferencesFile->getFileName());
            }
            catch (FileException&) {
            }
            
            //
            // Set the next page
            //
            nextPage = pageDatabaseUserInformation;
         }
      } 
   }
   else if (currentPage == pageDatabaseUserInformation) {
      nextPage = pageDatabaseMode;
   }
   else if (currentPage == pageDatabaseMode) {
      switch (dialogMode) {
         case DIALOG_MODE_NONE:
            break;
         case DIALOG_MODE_ARCHIVE_ID:
            if (getSumsDataFileListing()) {
               nextPage = pageDataFileSelection;
            }
            break;
         case DIALOG_MODE_SPEC_FILE_ID:
            if (getSumsDataFileListing()) {
               nextPage = pageDataFileSelection;
            }
            break;
         case DIALOG_MODE_DOWNLOAD_ATLAS:
            nextPage = pageDownloadAtlas;
            break;
         case DIALOG_MODE_DOWNLOAD_SEARCH:
            nextPage = pageDownloadSearch;
            break;
         case DIALOG_MODE_REFRESH_SPEC_FILE:
            nextPage = pageRefreshSpecFile;
            break;
         case DIALOG_MODE_UPLOAD_FILES:
            nextPage = pageUploadFiles;
            break;
      }
   }
   else if (currentPage == pageDownloadAtlas) {
      if (getSumsSpecFileListing()) {
         nextPage = pageSpecFileSelection;
      }
   }
   else if (currentPage == pageDownloadSearch) {
      if (downloadSearchFileTypeComboBox->currentIndex() == SEARCH_FILE_INDEX_SPEC) {
         if (getSumsSpecFileListing()) {
            nextPage = pageSpecFileSelection;
         }
      }
      else { 
         if (getSumsDataFileListing()) {
            nextPage = pageDataFileSelection;
         }
      }
   }
   else if (currentPage == pageRefreshSpecFile) {
      nextPage = NULL;
   }
   else if (currentPage == pageUploadFiles) {
      //
      // last page for uploading files
      //
      nextPage = NULL;
   }
   else if (currentPage == pageSpecFileSelection) {
      for (int i = 0; i < sumsSpecFileList.getNumberOfSumsFiles(); i++) {
         SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(i);
         if (sfi->getSelected()) {
            if (getSumsDataFileListing(sfi->getID())) {
               nextPage = pageDataFileSelection;
            }
            break;
         }
      }
   }
   else if (currentPage == pageDataFileSelection) {
      nextPage = pageOutputDirectory;
   }
   else if (currentPage == pageOutputDirectory) {
      //
      // last page for downloading files
      //
      nextPage = NULL;
   }

/*
   
   
   if (currentPage == pageSpecFileSelection) {
      nextPageValid = false;
      for (int i = 0; i < sumsSpecFileList.getNumberOfSumsFiles(); i++) {
         SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(i);
         if (sfi->getSelected()) {
            nextPageValid = getSumsDataFileListing(sfi->getID());
            break;
         }
      }
   }
*/
  
   //
   // Should the next page be shown ?
   //
   if (nextPage != NULL) {
      showPage(nextPage);
   }
   
   slotEnableDisablePushButtons();
   
   QApplication::restoreOverrideCursor();
}

/**
 * Login to SuMS.  Returns true if login is successful.
 */
bool
GuiSumsDialog::loginToSums()
{
   loginInfoFile.clear();
   loginInfoFile.setRootXmlElementTagName("sums");
   
   //
   // URL for initial login
   //
   QString url(databaseHostName);
   url.append("/sums/logon.do");
   
   //
   // Send the URL
   //
   QString contents;
   QString errorMessage;
   std::map<QString,QString> headerTagsOut;
   int responseCode = -1;
   FileUtilities::downloadFileWithHttpGet(url, searchTimeoutSpinBox->value(),
                                   contents, errorMessage, &headerTagsOut, &responseCode);
   if (responseCode != 302) {
      QString msg("Initial login attemp failed (login.do).  Try again.\n");
      msg.append(errorMessage);
      GuiMessageBox::critical(this, "ERROR", msg, "OK");
      return false;
   }
   else {
      //
      // Get the session ID
      //
      sumsSessionID = headerTagsOut["set-cookie"];
      sumsCookie = "";
      if (DebugControl::getDebugOn()) {
         std::cout << "Session ID tag: " << sumsSessionID.toAscii().constData() << std::endl;
      }
      if (sumsSessionID.isEmpty() == false) {
         std::vector<QString> tokens;
         StringUtilities::token(sumsSessionID, ";", tokens);
         if (tokens.empty() == false) {
            sumsCookie = tokens[0];
            sumsSessionID = tokens[0];
            StringUtilities::token(sumsSessionID, "=", tokens);
            if (tokens.size() >= 2) {
               sumsSessionID = ";";
               sumsSessionID.append(StringUtilities::makeLowerCase(tokens[0]));
               sumsSessionID.append("=");
               sumsSessionID.append(tokens[1]);
            }
            else {
               sumsSessionID = "";
            }
         }
         else {
            sumsSessionID = "";
         }
      }
      if (DebugControl::getDebugOn()) {
         std::cout << "SessionID: (" << sumsSessionID.toAscii().constData() << ")" << std::endl;
         std::cout << "Cookie: (" << sumsCookie.toAscii().constData() << ")" << std::endl;
      }
      
      //
      // Are we just a vistor that does not need to login ?
      // All that is needed is the session ID
      //
      if (loginVisitorRadioButton->isChecked()) {
         return true;
      }
      
      //
      // Next request is a post, so setup post data that contains login information
      //
      QString password(databasePassword);
      password = QUrl::toPercentEncoding(password);
      //Q3Url::encode(password);
      QString postContents;
      std::ostringstream str;
      str << "j_username="
          << databaseUserName.toAscii().constData()
          << "&"
          << "j_password="
          << password.toAscii().constData()
          << "&form=login";
      const QString postData(str.str().c_str());
      
      //
      // URL for second phase of login
      //
      url = databaseHostName;
      url.append("/sums/login/j_security_check");
      if (sumsSessionID.isEmpty() == false) {
         url.append(sumsSessionID);
      }
      
      //
      // Make the request
      //
      FileUtilities::downloadFileWithHttpPost(url, 
                                              postData,
                                              searchTimeoutSpinBox->value(),
                                              postContents, 
                                              errorMessage, 
                                              NULL,
                                              &headerTagsOut, 
                                              &responseCode);
      
      if (DebugControl::getDebugOn()) {
         std::cout << std::endl;
         std::cout << "Second part of request -------------------" << std::endl;
         std::cout << "URL: (" << url.toAscii().constData() << ")" << std::endl;
         std::cout << "Post data sent: (" << postData.toAscii().constData() << ")" << std::endl;
         std::cout << "Response Code: " << responseCode << std::endl;
         for (std::map<QString,QString>::iterator pos = headerTagsOut.begin();
              pos != headerTagsOut.end(); pos++) {
            std::cout << "Post tag/value:  "
                      << pos->first.toAscii().constData() << "  " << pos->second.toAscii().constData() << std::endl;
         }
         std::cout << "Post Content: " << postContents.toAscii().constData() << std::endl;
      }
      
      if (responseCode != 302) {
         QString msg("Second phase of login attempt failed (j_security_check).  Try again.\n");
         msg.append(errorMessage);
         GuiMessageBox::critical(this, "ERROR", msg, "OK");
         return false;
      }
      else {
         //
         // Third request
         //
         url = databaseHostName;
         url.append("/sums/logon.do");
         if (sumsSessionID.isEmpty() == false) {
            url.append(sumsSessionID);
         }
         url.append("?caret_xml=yes");
         FileUtilities::downloadFileWithHttpGet(url, searchTimeoutSpinBox->value(),
                                      contents, errorMessage, &headerTagsOut, &responseCode);
         if (DebugControl::getDebugOn()) {
            std::cout << "Third part of request --------------------" << std::endl;
            std::cout << "URL: (" << url.toAscii().constData() << ")" << std::endl;
            std::cout << "Response Code: " << responseCode << std::endl;
            for (std::map<QString,QString>::iterator pos = headerTagsOut.begin();
                 pos != headerTagsOut.end(); pos++) {
               std::cout << "Third-GET tag/value:  "
                         << pos->first.toAscii().constData() << "  " << pos->second.toAscii().constData() << std::endl;
            }
            std::cout << "GET Content: " << contents.toAscii().constData() << std::endl;
         }
         if (responseCode != 200) {
            QString msg("Third phase of login attempt failed (login.do).\n"
                            "Check username and password.\n");
            msg.append(errorMessage);
            GuiMessageBox::critical(this, "ERROR", msg, "OK");
            return false;
         }
         
         //
         // XML returned by SuMS with info about the user
         //
         try {
            // loginInfoFile.readFileFromString(contents);
            loginInfoFile.readFileFromArray(contents.toAscii().constData(), contents.length(),
                                            "sums_login_info.dat");
         }
         catch (FileException& e) {
            QString msg("Login successful, however, unable to parse\n"
                                    "user information.\n");
            msg.append(e.whatQString());
            GuiMessageBox::information(this, "INFO", 
                                       msg, "OK");
            return true;  // login successful
         }
         
         if (DebugControl::getDebugOn()) {
            std::vector<QString> roles;
            loginInfoFile.getValue("sums:login:user:roles:role", roles);
            std::cout << "Email: " << loginInfoFile.getValue("sums:login:user:email").toAscii().constData() << std::endl;
            for (unsigned int m = 0; m < roles.size(); m++) {
               std::cout << "Roles: " << roles[m].toAscii().constData() << std::endl;
            }
         }
      }
   }
         
   return true;
}

/**
 * Since the SuMS session may time out, this may be used to relogin
 * prior to an operation so that the user does not need to go to the 
 * starting page to login.
 * Returns true if login is successful, otherwise it will pop up an 
 * error Message dialog and return false.
 * Relogin is only performed if the "Login Prior to Operation" checkbox
 * is checked.
 */
bool
GuiSumsDialog::reloginToSums()
{
   if (loginBeforeOperationCheckBox->isChecked()) {
      return loginToSums();
   }
   return true;
}

/**
 * Prepare some pages when they are about to be shown
 */
void
GuiSumsDialog::showPage(QWidget* page)
{
   //
   // Show the page
   //
   pagesStackedWidget->setCurrentWidget(page);
   pageTitleLabel->setText(pageTitles[pagesStackedWidget->currentIndex()]);

   if (page == pageDatabaseUserInformation) {
      loadDatabaseUserInformationPage();
   }
   
   if (page == pageDatabaseMode) {
      modeUploadFilesRadioButton->setEnabled(uploadButtonValid);
   }
   
   slotEnableDisablePushButtons();
}

/**   
 * Set the pages that are valid for viewing according to the current selections.
 */
void
GuiSumsDialog::slotEnableDisablePushButtons()
{ 
   backPushButton->setEnabled(true);
   nextPushButton->setEnabled(false);
   finishPushButton->setEnabled(false);
   
   const QWidget* currentPage = pagesStackedWidget->currentWidget();
   
   bool finishValid = false;
   bool nextValid = false;
   
   //
   // Update the mode of the dialog
   //
   bool databaseModeNextValid = false;
   if (modeDownloadArchiveRadioButton->isChecked()) {
      dialogMode = DIALOG_MODE_ARCHIVE_ID;
      databaseModeNextValid = (downloadArchiveNumberLineEdit->text().isEmpty() == false);
   }
   else if (modeDownloadSpecRadioButton->isChecked()) {
      dialogMode = DIALOG_MODE_SPEC_FILE_ID;
      databaseModeNextValid = (downloadSpecNumberLineEdit->text().isEmpty() == false);
   }
   else if (modeDownloadAtlasRadioButton->isChecked()) {
      dialogMode = DIALOG_MODE_DOWNLOAD_ATLAS;
      databaseModeNextValid = true;
   }
   else if (modeDownloadSearchRadioButton->isChecked()) {
      dialogMode = DIALOG_MODE_DOWNLOAD_SEARCH;
      databaseModeNextValid = true;
   }
   else if (modeRefreshSpecRadioButton->isChecked()) {
      dialogMode = DIALOG_MODE_REFRESH_SPEC_FILE;
      databaseModeNextValid = true;
   }
   else if (modeUploadFilesRadioButton->isChecked()) {
      dialogMode = DIALOG_MODE_UPLOAD_FILES;
      databaseModeNextValid = true;
   }
   
   if (currentPage == pageDatabaseLogin) {
      databaseUserNameLineEdit->setEnabled(loginUserNameRadioButton->isChecked());
      databasePasswordLineEdit->setEnabled(loginUserNameRadioButton->isChecked());
      nextValid = (loginVisitorRadioButton->isChecked() ||
                   loginUserNameRadioButton->isChecked());
   }
   else if (currentPage == pageDatabaseUserInformation) {
      nextValid = true;
   }
   else if (currentPage == pageDatabaseMode) {
      switch (dialogMode) {
         case DIALOG_MODE_NONE:
            nextValid = false;
            break;
         case DIALOG_MODE_ARCHIVE_ID:
            nextValid = (downloadArchiveNumberLineEdit->text().isEmpty() == false);
            break;
         case DIALOG_MODE_SPEC_FILE_ID:
            nextValid = (downloadSpecNumberLineEdit->text().isEmpty() == false);
            break;
         case DIALOG_MODE_DOWNLOAD_ATLAS:
            nextValid = true;
            break;
         case DIALOG_MODE_DOWNLOAD_SEARCH:
            nextValid = true;
            break;
         case DIALOG_MODE_REFRESH_SPEC_FILE:
            nextValid = true;
            break;
         case DIALOG_MODE_UPLOAD_FILES:
            nextValid = true;
            break;
      }

   }
   else if (currentPage == pageDownloadAtlas) {
      nextValid = true;
   }
   else if (currentPage == pageDownloadSearch) {
      nextValid = true;
   }
   else if (currentPage == pageRefreshSpecFile) {
   }
   else if (currentPage == pageUploadFiles) {
      if (uploadFileNames.empty() == false) {
         finishValid = true;
      }
   }
   else if (currentPage == pageSpecFileSelection) {
      for (int i = 0; i < sumsSpecFileList.getNumberOfSumsFiles(); i++) {
         SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(i);
         if (sfi->getSelected()) {
            nextValid = true;
         }
      }
   }
   else if (currentPage == pageDataFileSelection) {
      for (int i = 0; i < sumsDataFileList.getNumberOfSumsFiles(); i++) {
         if (sumsDataFileList.getSumsFileInfo(i)->getSelected()) {
            nextValid = true;
            break;
         }
      }
   }
   else if (currentPage == pageOutputDirectory) {
      finishValid = true;
   }

   //
   // enable the next & finish buttons push button
   //
   nextPushButton->setEnabled(nextValid);
   finishPushButton->setEnabled(finishValid);
   
/*
   setAppropriate(pageDatabaseLogin, true);
   setAppropriate(pageDatabaseUserInformation, true);
   setAppropriate(pageDatabaseMode, true);
   setAppropriate(pageDownloadAtlas, false);
   setAppropriate(pageDownloadSearch, false);
   setAppropriate(pageRefreshSpecFile, false);
   setAppropriate(pageUploadFiles, false);
   setAppropriate(pageSpecFileSelection, false);
   setAppropriate(pageDataFileSelection, false);
   setAppropriate(pageOutputDirectory, false);
   
   
   downloadArchiveNumberLineEdit->setEnabled(dialogMode == DIALOG_MODE_ARCHIVE_ID);
   downloadSpecNumberLineEdit->setEnabled(dialogMode == DIALOG_MODE_SPEC_FILE_ID);
   
   setNextEnabled(pageDatabaseMode, databaseModeNextValid);
   
   if (databaseModeNextValid) {
      switch (dialogMode) {
         case DIALOG_MODE_NONE:
            break;
         case DIALOG_MODE_ARCHIVE_ID:
            break;
         case DIALOG_MODE_SPEC_FILE_ID:
            break;
         case DIALOG_MODE_DOWNLOAD_ATLAS:
            setAppropriate(pageDownloadAtlas, true);
            setNextEnabled(pageDownloadAtlas, true);
            if (sumsSpecFileList.empty() == false) {
               setAppropriate(pageSpecFileSelection, true);
               bool specFileSelected = false;
               for (int i = 0; i < sumsSpecFileList.getNumberOfSumsFiles(); i++) {
                  SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(i);
                  if (sfi->getSelected()) {
                     specFileSelected = true;
                  }
               }
               setNextEnabled(pageSpecFileSelection, specFileSelected);
            }
            break;
         case DIALOG_MODE_DOWNLOAD_SEARCH:
            setAppropriate(pageDownloadSearch, true);
            setNextEnabled(pageDownloadSearch, true);
            if (downloadSearchFileTypeComboBox->currentIndex() == SEARCH_FILE_INDEX_SPEC) {
               if (sumsSpecFileList.empty() == false) {
                  setAppropriate(pageSpecFileSelection, true);
                  bool specFileSelected = false;
                  for (int i = 0; i < sumsSpecFileList.getNumberOfSumsFiles(); i++) {
                     SumsFileInfo* sfi = sumsSpecFileList.getSumsFileInfo(i);
                     if (sfi->getSelected()) {
                        specFileSelected = true;
                     }
                  }
                  setNextEnabled(pageSpecFileSelection, specFileSelected);
               }
            }
            break;
         case DIALOG_MODE_REFRESH_SPEC_FILE:
            setAppropriate(pageRefreshSpecFile, true);
            setNextEnabled(pageRefreshSpecFile, false);
            break;
         case DIALOG_MODE_UPLOAD_FILES:
            setAppropriate(pageUploadFiles, true);
            setNextEnabled(pageUploadFiles, false);
            setFinishEnabled(pageUploadFiles, (uploadFileNames.empty() == false));
            break;
      }
   }
   
   if (sumsDataFileList.empty() == false) {
      setAppropriate(pageDataFileSelection, true);
      
      bool dataFilesSelected = false;
      for (int i = 0; i < sumsDataFileList.getNumberOfSumsFiles(); i++) {
         if (sumsDataFileList.getSumsFileInfo(i)->getSelected()) {
            dataFilesSelected = true;
            break;
         }
      }
      
      setNextEnabled(pageDataFileSelection, dataFilesSelected);
      if (dataFilesSelected) {
         setAppropriate(pageOutputDirectory, true);
         
         setFinishEnabled(pageOutputDirectory,
                          directoryNameLineEdit->text().isEmpty() == false);
      }
   }
   
   setNextEnabled(pageOutputDirectory, false);
*/
}
