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

#ifndef __GUI_LOADED_FILE_MANAGEMENT_DIALOG_H__
#define __GUI_LOADED_FILE_MANAGEMENT_DIALOG_H__

#include <vector>

#include "WuQDialog.h"

#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "SpecFile.h"

class AbstractFile;
class BrainModelBorderFileInfo;
class BrainModelBorderSet;
class QButtonGroup;
class QCheckBox;
class QGridLayout;
class QLineEdit;
class QToolButton;
class VolumeFile;

/// Dialog to manage loaded files
class GuiLoadedFileManagementDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiLoadedFileManagementDialog(QWidget* parent);
      
      /// Destructor
      ~GuiLoadedFileManagementDialog();
      
   private slots:
      /// called when an info button is pressed
      void slotInfoButtonGroup(int item);
      
      /// called when a save button is pressed
      void slotSaveButtonGroup(int item);
      
      /// called when a view button is pressed
      void slotViewButtonGroup(int item);
      
      /// called when an border info button is pressed
      void slotBorderInfoButtonGroup(int item);
      
      /// called when a remove file button is pressed
      void slotRemoveFileButtonGroup(int item);
      
      /// called when a remove file column button is pressed
      void slotRemoveFileColumnButtonGroup(int item);
      
      /// called when a clear file column button is pressed
      void slotClearFileColumnButtonGroup(int item);
      
      /// called to delete border projections
      void slotDeleteBorderProjections();
      
      /// called to save border projections
      void slotSaveBorderProjections();
      
      /// save all checked files
      void slotSaveAllCheckFiles();
      
   private:
      /// class for storing data file and pointers to its labels
      class DataFile {
         public:
            /// constructor
            DataFile(AbstractFile* dataFileIn,
                     GuiFilesModified dataFileTypeMaskIn,
                     QWidget* fileTypeCheckBoxOrLabelIn,
                     QLineEdit* nameLineEditIn,
                     QLabel* modifiedLabelIn,
                     const QString& specFileTagIn);
            
            /// destructor
            ~DataFile();
            
            /// set modified
            void setModified();
            
            /// clear modified
            void clearModified();
                        
            /// save the file if it is checked
            QString saveFileIfChecked();
            
            ///  the data file
            AbstractFile* dataFile;
            
            /// file type checkbox or label
            QWidget* fileTypeCheckBoxOrLabel;
            
            /// name line edit
            QLineEdit* nameLineEdit;
            
            /// data file type mask
            GuiFilesModified dataFileTypeMask;
            
            /// modified label
            QLabel* modifiedLabel;
            
            /// the spec file tag
            QString specFileTag;
      };
      
      /// add a file to the list of files
      void addFileToGrid(AbstractFile* af,
                         const GuiFilesModified& fileMask,
                         const char* typeName,
                         const QString& specFileTagIn);
      
      /// add a file to the list of files
      void addFileToGrid(AbstractFile* af,
                         const GuiFilesModified& fileMask,
                         const SpecFile::Entry& dataFileInfo,
                         const char* typeName,
                         const QString& specFileTagIn);
                         
      /// add a border file to the list of files
      void addBorderFileToGrid(BrainModelBorderSet* bmbs,
                               const BrainModelSurface::SURFACE_TYPES surfaceType,
                               const char* typeName,
                               const QString& specFileTagIn);
      
      /// Add a border projection file to the grid.
      void addBorderProjectionFileToGrid(BrainModelBorderSet* bmbs);
      
      /// widget that contains files
      QWidget* fileGridWidget;
      
      /// layout for files
      QGridLayout* fileGridLayout;
      
      /// view button group
      QButtonGroup* viewButtonGroup;
      
      /// save button group
      QButtonGroup* saveButtonGroup;
      
      /// info button group
      QButtonGroup* infoButtonGroup;
      
      /// remove file button group
      QButtonGroup* removeFileButtonGroup;
      
      /// remove file column button group
      QButtonGroup* removeFileColumnButtonGroup;
      
      /// clear file column button group
      QButtonGroup* clearFileColumnButtonGroup;
      
      /// border info button group
      QButtonGroup* borderInfoButtonGroup;
      
      /// pointers to files
      std::vector<DataFile> dataFiles;
      
      /// pointers to border file info
      std::vector<BrainModelBorderFileInfo*> borderInfoFileInfo;
      
      /// border projection info button
      QToolButton* borderProjInfoPB;
      
      /// border projection clear button
      QToolButton* borderProjClearPB;
      
      /// border projection save button
      QToolButton* borderProjSavePB;
      
      /// border projection file index
      int borderProjectionDataFileIndex;
      
      /// column for file type
      int columnFileTypeIndex;
      
      /// column for modified
      int columnModifiedIndex;
      
      /// column for view file
      int columnViewFileIndex;
      
      /// column for save file
      int columnSaveFileIndex;
      
      /// column for comment info
      int columnCommentIndex;
      
      /// column for remove file
      int columnRemoveFileIndex;
      
      /// column for remove column
      int columnRemoveFileColumnIndex;
      
      /// column for clear file column
      int columnClearFileColumnIndex;
      
      /// column for file name
      int columnFileNameIndex;
      
      /// width of view/clear/info buttons
      int buttonWidth;
      
      /// text for remove file button
      QString removeFilePushButtonText;
      
      /// text for remove file column button
      QString removeFileColumnPushButtonText;
      
      /// text for clear file column button
      QString clearFileColumnPushButtonText;
      
      /// text for view file button
      QString viewFilePushButtonText;
      
      /// text for comment/header button
      QString commentHeaderPushButtonText;
      
      /// text for save file button
      QString saveFilePushButtonText;
      
      /// border projection file
      BorderProjectionFile borderProjectionFileForGrid;
};

#endif // __GUI_LOADED_FILE_MANAGEMENT_DIALOG_H__

