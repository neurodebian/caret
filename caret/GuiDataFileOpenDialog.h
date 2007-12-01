#ifndef __GUI_DATA_FILE_OPEN_DIALOG_H__
#define __GUI_DATA_FILE_OPEN_DIALOG_H__

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

#include "WuQFileDialog.h"

class QCheckBox;
class QPushButton;

/// dialog for opening Caret data files.
class GuiDataFileOpenDialog : public WuQFileDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiDataFileOpenDialog(QWidget* parent = 0,
                            Qt::WindowFlags flags = 0);
                        
      // destructor
      ~GuiDataFileOpenDialog();
      
      /// static method to open a data file and make necessary updates as a result.
      /// returns true if an error occurred.
      static bool openDataFile(QWidget* parentWidget, const QString specFileTag,
                               const QString& name, const bool append, const bool updateSpecFile,
                               QString& errorMessage,
                               bool& relatedFileWarningFlag);
   protected slots:
      // called when view/edit comment button pressed
      void slotViewEditCommentPushButton();
      
      // called when the file selection is changed
      void slotFilesSelected(const QStringList&);
      
   protected:
      // called when dialog is closed
      void done(int r);
      
      // returns file filters that match the name
      virtual QStringList matchingFilters(const QString& name);
      
      // copy a file to the current directory, returns true if successful
      bool copyFile(const QString& name);
      
      // process import files (returns true if filter matched an import file)
      bool importFile(const QString& fileName,
                      const QString& filterName);
                      
      // read the file
      void readFile(const QString& fileName,
                    const QString& filterName,
                    const bool addToSpecFileFlag,
                    const bool appendToCurrentFileFlag);
                    
      /// add to specification file check box
      QCheckBox* addToSpecFileCheckBox;
      
      /// append to current data flag
      QCheckBox* appendToCurrentFileCheckBox;
      
      /// view edit comment push button
      QPushButton* viewEditCommentPushButton;
};

#endif // __GUI_DATA_FILE_OPEN_DIALOG_H__

