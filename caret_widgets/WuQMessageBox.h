
#ifndef __GUI_MESSAGE_BOX_H__
#define __GUI_MESSAGE_BOX_H__

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

#include <QMessageBox>

/// class that extends QMessageBox to add enhancements
class WuQMessageBox : public QMessageBox {
   Q_OBJECT
   
   public:
      // constructor
      WuQMessageBox(QWidget* parent = 0);
      
      // constructor
      WuQMessageBox(Icon icon,
                    const QString& title,
                    const QString& text,
                    StandardButtons buttons = NoButton,
                    QWidget* parent = 0,
                    Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
      
      // destructor
      ~WuQMessageBox();

      // set the window title
      void setTheWindowTitle(const QString& title);
      
      // information dialog
      static QMessageBox::StandardButton information(QWidget *parent, const QString &title,
         const QString &text, QMessageBox::StandardButtons buttons = Ok,
         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
         
      // question dialog
      static QMessageBox::StandardButton question(QWidget *parent, const QString &title,
         const QString &text, QMessageBox::StandardButtons buttons = Ok,
         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
         
      // warning dialog
      static QMessageBox::StandardButton warning(QWidget *parent, const QString &title,
         const QString &text, QMessageBox::StandardButtons buttons = Ok,
         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
         
      // critical dialog
      static QMessageBox::StandardButton critical(QWidget *parent, const QString &title,
         const QString &text, QMessageBox::StandardButtons buttons = Ok,
         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
         
   protected slots:
      // called to capture image of window and place it on the clipboard
      void slotMenuCaptureImageOfWindowToClipboard();

      // called to capture image after timeout so nothing obscures window
      void slotCaptureImageAfterTimeOut();
      
   protected:
      // called by parent when context menu event occurs
      virtual void contextMenuEvent(QContextMenuEvent*);
      
      // used by other static methods to create the dialog
      static QMessageBox::StandardButton showNewMessageBox(QWidget *parent,
                                              QMessageBox::Icon icon, 
                                              const QString& title, 
                                              const QString& text,
                                              QMessageBox::StandardButtons buttons, 
                                              QMessageBox::StandardButton defaultButton);
      
      // Show the old message box
      static int showOldMessageBox(QWidget *parent, QMessageBox::Icon icon,
                            const QString &title, const QString &text,
                            int button0, int button1, int button2);
};

#endif // __GUI_MESSAGE_BOX_H__
