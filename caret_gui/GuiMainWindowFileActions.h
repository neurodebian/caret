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


#ifndef __GUI_MAIN_WINDOW_FILE_ACTIONS_H__
#define __GUI_MAIN_WINDOW_FILE_ACTIONS_H__

#include <QObject>

class GuiMainWindow;
class QAction;
class QWidget;

/// Actions for the file menu.
class GuiMainWindowFileActions : public QObject {
   Q_OBJECT
   
   public:
      /// constructor
      GuiMainWindowFileActions(GuiMainWindow* parent);
      
      /// destructor
      ~GuiMainWindowFileActions();
   
      /// fast open data file dialog action
      QAction* getFastOpenDataFileAction() { return fastOpenDataFileAction; }
      
      /// preferences dialog action
      QAction* getShowPreferencesDialogAction() { return showPreferencesDialogAction; }
      
      /// set current directory action
      QAction* getSetCurrentDirectoryAction() { return setCurrentDirectoryAction; }
      
      /// convert data file action
      QAction* getConvertDataFileAction() { return convertDataFileAction; }
      
      /// copy spec file action
      QAction* getCopySpecFileAction() { return copySpecFileAction; }
      
      /// validate spec file action
      QAction* getValidateSpecFileAction() { return validateSpecFileAction; }
      
      /// zip spec file action
      QAction* getZipSpecFileAction() { return zipSpecFileAction; }
      
      /// record as mpeg action
      QAction* getRecordAsMpegAction() { return recordAsMpegAction; }
      
      /// data file open action
      QAction* getDataFileOpenAction() { return dataFileOpenAction; }
      
      /// data file save action
      QAction* getDataFileSaveAction() { return dataFileSaveAction; }
      
      /// copy main window to clipboard action
      QAction* getCopyMainWindowToClipboardAction() { return copyMainWindowToClipboardAction; }
      
      /// print main window action
      QAction* getPrintMainWindowAction() { return printMainWindowAction; }
      
      /// open spec file action
      QAction* getOpenSpecFileAction() { return openSpecFileAction; }
      
      /// close spec file action
      QAction* getCloseSpecFileAction() { return closeSpecFileAction; }
      
      /// add document to spec file action
      QAction* getAddDocumentToSpecFileAction() { return addDocumentToSpecFileAction; }
      
      ///loaded file management action
      QAction* getLoadedFileManagementAction() { return loadedFileManagementAction; }

      /// exit caret action
      QAction* getExitCaretAction() { return exitCaretAction; }
      
      /// capture image of main window action
      QAction* getCaptureMainWindowImageAction() { return captureMainWindowImageAction; }

public slots:
      /// update the actions (typically called when menu is about to show)
      void updateActions();

      /// add document to spec file action
      void addDocumentToSpecFileSlot();
      
      /// data file open slot
      void dataFileOpenSlot();
      
      /// data file save slot
      void dataFileSaveSlot();
      
      /// open spec file action
      void openSpecFileSlot();
         
      /// set current directory action
      void slotSetCurrentDirectory();
      
      /// copy main window to clipboard action
      void copyMainWindowToClipboard();
      
      /// print main window action
      void printMainWindowImage();
      
      /// loaded file management action
      void loadedFileManagementSlot();
      
      /// record as mpeg action
      void recordAsMpegSlot();
      
      /// copy spec file action
      void copySpecFileSlot();
      
      /// zip spec file action
      void zipSpecFileSlot();
      
      /// validate spec file action
      void validateSpecFileSlot();
      
      /// convert data files action
      void convertDataFileSlot();
      
   protected:
      /// exit caret action
      QAction* exitCaretAction;
      
      /// add document to spec file action
      QAction* addDocumentToSpecFileAction;
      
      /// close spec file action
      QAction* closeSpecFileAction;
      
      /// fast open data file dialog action
      QAction* fastOpenDataFileAction;
      
      /// preferences dialog action
      QAction* showPreferencesDialogAction;
      
      /// set current directory action
      QAction* setCurrentDirectoryAction;
      
      /// convert data file action
      QAction* convertDataFileAction;
      
      /// copy spec file action
      QAction* copySpecFileAction;
      
      /// validate spec file action
      QAction* validateSpecFileAction;
      
      /// zip spec file action
      QAction* zipSpecFileAction;
      
      /// record as mpeg action
      QAction* recordAsMpegAction;
      
      /// data file open action
      QAction* dataFileOpenAction;
      
      /// data file save action
      QAction* dataFileSaveAction;
      
      /// copy main window to clipboard action
      QAction* copyMainWindowToClipboardAction;
      
      /// capture image of main window action
      QAction* captureMainWindowImageAction;
      
      /// print main window action
      QAction* printMainWindowAction;
      
      /// open spec file action
      QAction* openSpecFileAction;
      
      /// loaded file management action
      QAction* loadedFileManagementAction;
};

#endif  // __GUI_MAIN_WINDOW_FILE_ACTIONS_H__

