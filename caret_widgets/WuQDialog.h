#ifndef __WU_QDIALOG_H__
#define __WU_QDIALOG_H__

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

#include <QDialog>

class QKeyEvent;
class QMenu;

/// QDialog with a few additions including copy of dialog as image to clipboard
class WuQDialog : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      WuQDialog(QWidget* parent = 0,
                Qt::WindowFlags f = 0);
                    
      // destructor
      virtual ~WuQDialog();
      
   protected slots:
      // called to capture image of window and place it on the clipboard
      void slotMenuCaptureImageOfWindowToClipboard();
      
      // called to capture image after timeout so nothing obscures window
      void slotCaptureImageAfterTimeOut();
      
   protected:
      // add a capture image of window menu item to the menu
      void addImageCaptureToMenu(QMenu* menu);
      
      // called by parent when context menu event occurs
      virtual void contextMenuEvent(QContextMenuEvent*);
      
      // ring the bell
      void beep();
      
      // show the wait cursor
      void showWaitCursor();
      
      // normal cursor
      void showNormalCursor();
};

#endif // __WU_QDIALOG_H__

