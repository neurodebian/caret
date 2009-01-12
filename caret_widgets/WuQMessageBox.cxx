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

#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QDialogButtonBox>
#include <QImage>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>

#include "WuQMessageBox.h"

/**
 * constructor.
 */
WuQMessageBox::WuQMessageBox(QWidget* parent)
   : QMessageBox(parent)
{
}

/**
 * constructor.
 */
WuQMessageBox::WuQMessageBox(Icon icon,
                             const QString& title,
                             const QString& text,
                             StandardButtons buttons,
                             QWidget* parent,
                             Qt::WindowFlags f)
   : QMessageBox(icon,
                 title,
                 text,
                 buttons,
                 parent,
                 f)
{
   setTheWindowTitle(title);
}

/**
 * destructor.
 */
WuQMessageBox::~WuQMessageBox()
{
}

/**
 * Set the window title.
 * Mac Guidelines say that a pop-up should not have a title so Qt's
 * implementation of QMessageBox has its own setWindowTitle() method
 * that overrides QDialog::setWindowTitle() to prevent the title
 * from showing up on a Mac.  But, we want the title on Macs, and
 * this method does it!
 */
void 
WuQMessageBox::setTheWindowTitle(const QString& title)
{
   QDialog::setWindowTitle(title);
}

/**
 * called to capture image after timeout so nothing obscures window. */
void
WuQMessageBox::slotCaptureImageAfterTimeOut()
{
   QImage image = QPixmap::grabWindow(this->winId()).toImage();
   if (image.isNull() == false) {
      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setImage(image);

      QMessageBox::information(this,
          "Information",
          "An image of this dialog has been placed onto the computer's clipboard.");
   }
}

/**
 * called to capture image of window and place it on the clipboard
 */
void
WuQMessageBox::slotMenuCaptureImageOfWindowToClipboard()
{
   //
   // Need to delay capture so that the context sensistive
   // menu closes or else the menu will be in the captured image.
   //
   QApplication::processEvents();
   QTimer::singleShot(1000, this, SLOT(slotCaptureImageAfterTimeOut()));
}

/**
 * called by parent when context menu event occurs.
 */
void
WuQMessageBox::contextMenuEvent(QContextMenuEvent* cme)
{
   //
   // Popup menu for selection of pages
   // 
   QMenu menu(this);
   
   // 
   // Add menu item for image capture
   //
   menu.addAction("Capture Image to Clipboard",
                  this,
                  SLOT(slotMenuCaptureImageOfWindowToClipboard()));
   
   // 
   // Popup the menu
   //
   menu.exec(cme->globalPos());
}

QMessageBox::StandardButton 
WuQMessageBox::information(QWidget *parent, const QString &title,
                         const QString& text, QMessageBox::StandardButtons buttons,
                         QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, Information, title, text, buttons,
                             defaultButton);
}
QMessageBox::StandardButton 
WuQMessageBox::question(QWidget *parent, const QString &title,
                      const QString& text, QMessageBox::StandardButtons buttons,
                      QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, Question, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton 
WuQMessageBox::warning(QWidget *parent, const QString &title,
                     const QString& text, QMessageBox::StandardButtons buttons,
                     QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, Warning, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton 
WuQMessageBox::critical(QWidget *parent, const QString &title,
                      const QString& text, QMessageBox::StandardButtons buttons,
                      QMessageBox::StandardButton defaultButton)
{
    return showNewMessageBox(parent, Critical, title, text, buttons, defaultButton);
}

/**
 * used by other static methods to create the dialog.
 */
QMessageBox::StandardButton 
WuQMessageBox::showNewMessageBox(QWidget *parent,
                               QMessageBox::Icon icon, 
                               const QString& title, 
                               const QString& text,
                               QMessageBox::StandardButtons buttons, 
                               QMessageBox::StandardButton defaultButton)
{
    // necessary for source compatibility with Qt 4.0 and 4.1
    // handles (Yes, No) and (Yes|Default, No)
    if (defaultButton && !(buttons & defaultButton)) {
        return (QMessageBox::StandardButton)
                    showOldMessageBox(parent, icon, title,
                                      text, int(buttons),
                                      int(defaultButton), 0);
    }

    WuQMessageBox msgBox(icon, title, text, QMessageBox::NoButton, parent);
    msgBox.setTheWindowTitle(title);
    QDialogButtonBox *buttonBox = qFindChild<QDialogButtonBox*>(&msgBox);
    Q_ASSERT(buttonBox != 0);

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb)
            continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);
        // Choose the first accept role as the default
        if (msgBox.defaultButton())
            continue;
        if ((defaultButton == QMessageBox::NoButton && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
            || (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

/** 
 * Show the old message box
 */
int 
WuQMessageBox::showOldMessageBox(QWidget *parent, QMessageBox::Icon icon,
                                          const QString &title, const QString &text,
                                          int button0, int button1, int button2)
{
    int butts[3] = { button0, button1, button2 };
    QMessageBox::StandardButtons buttons;
    for (int i = 0; i < 3; i++) {
       switch (butts[i]) {
          case QMessageBox::YesAll:
             buttons |= QMessageBox::YesToAll;
             break;
          case QMessageBox::NoAll:
             buttons |= QMessageBox::NoToAll;
             break;
          default:
             buttons |= static_cast<QMessageBox::StandardButton>(butts[i]);
             break;
       }
    }
    
    WuQMessageBox messageBox(icon, title, text, buttons, parent);
    return messageBox.exec();
}

