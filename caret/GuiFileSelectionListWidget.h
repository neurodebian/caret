
#ifndef __GUI_FILE_SELECTION_LIST_WIDGET_H__
#define __GUI_FILE_SELECTION_LIST_WIDGET_H__

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

#include <vector>

#include <QGroupBox>
#include <QStringList>

class QListWidget;

/// widget that contains a list widget and buttons for selecting a group of files
class GuiFileSelectionListWidget : public QGroupBox {
   Q_OBJECT
   
   public:
      // constructor
      GuiFileSelectionListWidget(const QString& groupBoxTitle,
                                 QWidget* parent = 0);
                  
      // destructor
      ~GuiFileSelectionListWidget();
      
      // set the file selection filters
      void setFileSelectionFilters(const QStringList& fileSelectionFiltersIn);
      
      // add files
      void addFiles(const QStringList& sl);
      
      // remove all files
      void removeAllFiles();
      
      // get the files from the list widget
      void getFileNames(QStringList& fileNames) const;
      
      // get the files from the list widget
      void getFileNames(std::vector<QString>& fileNames) const;
      
      // see if there are files in the list widget
      bool containsFiles() const;
      
   signals:
      // emitted when a file is add/removed
      void signalFilesChanged();
      
   protected slots:
      // slot for adding files
      void slotAddFilesPushButton();
      
      // slot for removing files
      void slotRemoveFilesPushButton();
      
   protected:
      /// the list widget
      QListWidget* listWidget;
      
      /// the file selection filters
      QStringList fileSelectionFilters;
};

#endif // __GUI_FILE_SELECTION_LIST_WIDGET_H__

