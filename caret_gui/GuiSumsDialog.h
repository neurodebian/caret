#ifndef __GUI_SUMS_DIALOG_H__
#define __GUI_SUMS_DIALOG_H__

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

#include "GenericXmlFile.h"
#include "SumsFileListFile.h"
#include "WuQDialog.h"

class PreferencesFile;
class QCheckBox;
class QComboBox;
class QDateTimeEdit;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QStackedWidget;
class QString;
class QTableWidget;
class QTextEdit;

/// dialog for interacting with SuMS database
class GuiSumsDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiSumsDialog(QWidget* parent, PreferencesFile* preferencesFileIn);
      
      /// constructor
      ~GuiSumsDialog();
     
   public slots:
      /// Show the dialog.
      void show();
   
   protected slots:
      /// called when output directory select push button is pressed
      void slotDirectoryPushButton();
      
      /// called when the download push button is pressed
      void slotFinishPushButton();
      
      /// called when select all files push button is pressed
      void slotSelectAllDataFilesPushButton();
      
      /// called when deselect all files push button is pressed
      void slotDeselectAllDataFilesPushButton();
      
      /// Called to select the next page
      void slotNextPushButton();
      
      /// Called to select the back page
      void slotBackPushButton();
      
      /// enable/disable push buttons
      void slotEnableDisablePushButtons();

      /// Called when close button pressed.
      void slotCloseButton();

      /// Called when a header column is clicked for sorting
      void slotDataFileTableHeaderColumnClicked(int item);

      /// Called when a data file table item is changed
      void slotDataFileTableItemChanged(int row, int col);
      
      /// Called when a spec file header column is clicked for sorting
      void slotSpecFileTableHeaderColumnClicked(int item);

      /// Called when a spec file table item is changed
      void slotSpecFileTableItemChanged(int row, int col);
      
      /// Called when a download search file type is selected
      void slotDownloadSearchFileTypeComboBox(int item);
      
      /// Called when upload file add button is pressed
      void slotUploadAddPushButton();
      
      /// Called when upload file remove button is pressed
      void slotUploadRemovePushButton();
      
      /// Called when the logout button is pressed
      void slotLogoutButton();
      
   protected:
      
      /// column indices for data file table
      enum DATA_TABLE_COLUMNS {
         DATA_TABLE_FILE_CHECK_BOX_COLUMN = 0,
         DATA_TABLE_FILE_DATE_COLUMN      = 1,
         DATA_TABLE_FILE_TYPE_COLUMN      = 2,
         DATA_TABLE_FILE_NAME_COLUMN      = 3,
         DATA_TABLE_FILE_SUBDIR_COLUMN    = 4,
         DATA_TABLE_FILE_STATE_COLUMN     = 5,
         DATA_TABLE_FILE_COMMENT_COLUMN   = 6,
         DATA_TABLE_NUMBER_OF_COLUMNS     = 7  // this item must always be the last item
      };
      
      /// column indices for spec file table
      enum SPEC_TABLE_COLUMNS{
         SPEC_TABLE_FILE_CHECK_BOX_COLUMN = 0,
         SPEC_TABLE_FILE_DATE_COLUMN      = 1,
         SPEC_TABLE_FILE_NAME_COLUMN      = 2,
         SPEC_TABLE_FILE_COMMENT_COLUMN   = 3,
         SPEC_TABLE_NUMBER_OF_COLUMNS     = 4
      };
      
      /// dialog mode type
      enum DIALOG_MODE {
         DIALOG_MODE_NONE,
         DIALOG_MODE_ARCHIVE_ID,
         DIALOG_MODE_SPEC_FILE_ID,
         DIALOG_MODE_DOWNLOAD_ATLAS,
         DIALOG_MODE_DOWNLOAD_SEARCH,
         DIALOG_MODE_REFRESH_SPEC_FILE,
         DIALOG_MODE_UPLOAD_FILES
      };
      
      enum UPLOAD_FILE_TYPE {
         UPLOAD_FILE_TYPE_OTHER_FILE,
         UPLOAD_FILE_TYPE_SPEC_FILE,
         UPLOAD_FILE_TYPE_SPEC_FILE_DATA_FILE
      };
   
      /// add a  page
      void addPage(QWidget* newPage, const QString& title);
      
      /// initialize the dialog
      void initializeDialog();
      
      /// Create the database login page
      QWidget* createDatabaseLoginPage();
      
      /// Create the database user information
      QWidget* createDatabaseUserInformation();
      
      /// Create the database mode page
      QWidget* createDatabaseModePage();
      
      /// Create the download atlas page
      QWidget* createDownloadAtlasPage();

      /// Create the download search page
      QWidget* createDownloadSearchPage();
      
      /// Create the listing of spec files
      QWidget* createSpecFileListSection();
      
      /// Create the listing of data files.
      QWidget* createDataFileListSection();

      /// Create the output directory section.
      QWidget* createOutputDirectorySection();

      /// Create the refresh spec page
      QWidget* createRefereshSpecFilePage();

      /// Create the upload files page
      QWidget* createUploadFilesPage();

      /// Insert the session ID into the URL.
      QString insertSessionIdIntoURL(const QString urlIn);

      /// Prepare some pages when they are about to be shown
      void showPage(QWidget* page);
      
      /// Load the file list table
      void loadDataFileTable();

      /// Get a SuMS file listing (returns true if valid)
      bool getSumsDataFileListing(const QString& specFileID = "");

      /// Set the number of rows in the table
      void setDataFileTableNumberOfRows(const int num);

      /// Adds help information in a read only text widget.
      QGroupBox* addPageInformation(const QString& title,
                                    const QString& text);
                                  
      /// set the spec file selection check boxes
      void setSpecFileTableSelectionCheckBoxes();

      /// set the data file selection check boxes
      void setDataFileTableSelectionCheckBoxes();
      
      /// read the file selection check boxes
      void readDataFileTableSelectionCheckBoxes();

      /// Get the list of spec files (returns true if valid)
      bool getSumsSpecFileListing();

      /// set the number of rows in the spec file table
      void setSpecFileTableNumberOfRows(const int num);
      
      /// load the spec file table
      void loadSpecFileTable();
  
      /// Append a parameter and its value to a URL.
      void appendParameterToURL(QString& url, 
                                const QString& paramName,
                                const QString& paramValue);
                                
      /// Append search parameters onto the URL.
      void appendSearchParametersToURL(QString& url, const bool specFileSearch);

      /// Login to SuMS.  Returns true if login is successful
      bool loginToSums();
      
      /// load the upload files list box
      void loadUploadFilesListBox();
      
      /// upload data files.
      void uploadDataFiles();

      /// download data files.
      void downloadDataFiles();

      /// Load the database user information page.
      void loadDatabaseUserInformationPage();

      /// relogin to sums if login prior to operation check box is checked
      bool reloginToSums();

      /// table of spec files
      QTableWidget* specFileTable;
      
      /// table of data files
      QTableWidget* dataFileTable;
   
      /// data file listing produced by SuMS database
      SumsFileListFile sumsDataFileList;
      
      /// spec file listing produced by SuMS database
      SumsFileListFile sumsSpecFileList;
      
      /// directory name line edit
      QLineEdit* directoryNameLineEdit;
      
      /// download push button
      QPushButton* downloadPushButton;
      
      /// database login page
      QWidget* pageDatabaseLogin;
      
      /// database user information page
      QWidget* pageDatabaseUserInformation;
      
      /// database mode page
      QWidget* pageDatabaseMode;
      
      /// Download atlas page
      QWidget* pageDownloadAtlas;
      
      /// Download search page
      QWidget* pageDownloadSearch;
      
      /// Refresh spec file page
      QWidget* pageRefreshSpecFile;
      
      /// Upload files page
      QWidget* pageUploadFiles;
      
      /// spec file selection page
      QWidget* pageSpecFileSelection;
      
      /// data file selection page
      QWidget* pageDataFileSelection;
      
      /// output directory page
      QWidget* pageOutputDirectory;
      
      /// combo box for database selection
      QComboBox* databaseSelectionComboBox;
      
      /// search timeout spin box
      QSpinBox* searchTimeoutSpinBox;
      
      /// data file timeout spin box
      QSpinBox* dataFileTimeoutSpinBox;
      
      /// name of database host
      QString databaseHostName;
      
      /// database user name line edit
      QLineEdit* databaseUserNameLineEdit;
      
      /// database user name
      QString databaseUserName;
      
      /// database password line edit
      QLineEdit* databasePasswordLineEdit;
      
      /// database password
      QString databasePassword;
      
      /// mode download archive 
      QRadioButton* modeDownloadArchiveRadioButton;
      
      /// mode download spec 
      QRadioButton* modeDownloadSpecRadioButton;
      
      /// mode download atlas button
      QRadioButton* modeDownloadAtlasRadioButton;
      
      /// mode download search button
      QRadioButton* modeDownloadSearchRadioButton;
      
      /// mode refresh spec button
      QRadioButton* modeRefreshSpecRadioButton;
      
      /// mode upload files button
      QRadioButton* modeUploadFilesRadioButton;
      
      /// dialog mode
      DIALOG_MODE dialogMode;
      
      /// the cookie sent by SuMS
      QString sumsCookie;
      
      /// the session ID sent by SuMS
      QString sumsSessionID;
      
      // download archive number line edit
      QLineEdit* downloadArchiveNumberLineEdit;
      
      /// download spec number line edit
      QLineEdit* downloadSpecNumberLineEdit;
      
      /// number of times to try downloading a file
      QSpinBox* downloadNumTimesSpinBox;
      
      /// download atlas species combo box
      QComboBox* downloadAtlasSpeciesComboBox;
      
      /// download atlas hemisphere combo box
      QComboBox* downloadAtlasHemisphereComboBox;
      
      /// download atlas space combo box
      QComboBox* downloadAtlasSpaceComboBox;
      
      /// download search file name line edit
      QLineEdit* downloadSearchFileNameLineEdit;
      
      /// download search file type combo box
      QComboBox* downloadSearchFileTypeComboBox;
      
      /// download search list of file type names
      std::vector<QString> downloadSearchFileTypeNames;
      
      /// download search list of file extensions
      std::vector<QString> downloadSearchFileExtensions;
      
      /// download search species combo box
      QComboBox* downloadSearchSpeciesComboBox;
      
      /// download search hemisphere combo box
      QComboBox* downloadSearchHemisphereComboBox;
      
      /// download search space combo box
      QComboBox* downloadSearchSpaceComboBox;
      
      /// download search species label
      QLabel* downloadSearchSpeciesLabel;
      
      /// download search hemisphere label
      QLabel* downloadSearchHemisphereLabel;
      
      /// download search space label
      QLabel* downloadSearchSpaceLabel;
      
      /// download search keyword
      QLineEdit* downloadSearchKeywordLineEdit;
      
      /// download search file comment
      QLineEdit* downloadSearchFileCommentLineEdit;
      
      /// download search date check box
      QCheckBox* downloadSearchDateCheckBox;
      
      /// download search start date
      QDateTimeEdit* downloadSearchStartDateEdit;
      
      /// download search end date
      QDateTimeEdit* downloadSearchEndDateEdit;
      
      /// upload files list box listing files for upload
      QListWidget* uploadFilesListBox;
      
      /// upload comment line edit
      QLineEdit* uploadCommentLineEdit;
      
      /// files for uploading
      std::vector<QString> uploadFileNames;
      
      /// search file indices
      enum SEARCH_FILE_INDICES {
         SEARCH_FILE_INDEX_SPEC = 0,
         SEARCH_FILE_INDEX_ALL_EXCEPT_SPEC = 1
      };
      
      /// information about user return at time of successful login
      GenericXmlFile loginInfoFile;
      
      /// db info user name label
      QLabel* dbInfoUserNameLabel;
      
      /// db info usage/quota label
      QLabel* dbInfoUsageQuotaLabel;
      
      /// db info privileges label
      QLabel* dbInfoPrivilegesLabel;
      
      /// db human label
      QLabel* dbInfoHumanLabel;
      
      /// db info session id
      QLabel* dbInfoSessionIdLabel;
      
      /// upload button valid 
      bool uploadButtonValid;
      
      /// login visitor radio button
      QRadioButton* loginVisitorRadioButton;
      
      /// login user name radio button
      QRadioButton* loginUserNameRadioButton;
      
      /// save login check box
      QCheckBox* saveLoginCheckBox;
      
      /// the preferences file
      PreferencesFile* preferencesFile;
      
      /// the database host names
      std::vector<QString> availableDatabaseHostNames;
      
      /// the logout button
      QPushButton* logoutButton;
      
      /// login before each operation check box
      QCheckBox* loginBeforeOperationCheckBox;
      
      /// widget stack for pages
      QStackedWidget* pagesStackedWidget;
      
      /// label for page title
      QLabel* pageTitleLabel;
      
      /// labels for each of the pages
      std::vector<QString> pageTitles;
      
      /// next push button
      QPushButton* nextPushButton;
      
      /// back push button
      QPushButton* backPushButton;
      
      /// finish push button
      QPushButton* finishPushButton;
};

#endif // __GUI_SUMS_DIALOG_H__

