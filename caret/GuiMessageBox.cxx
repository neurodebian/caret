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

#include <QApplication>
#include <QImage>
#include <QPixmap>

#define __GUI_MESSAGE_BOX_H_MAIN__
#include "GuiMessageBox.h"
#undef __GUI_MESSAGE_BOX_H_MAIN__

//#include "GuiSpeechGenerator.h"
#include "StringUtilities.h"

/**
 * display information message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::information(QWidget *parent, const QString &caption,
                           const QString& text,
                           int button0, int button1, int button2)
{
   dialogCommon("Information.", text);
   return QtMessageBox::information(parent, caption, text, button0, button1, button2);
}
                        
/**
 * display information message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::information(QWidget *parent, const QString &caption,
                           const QString& text,
                           const QString& button0Text,
                           const QString& button1Text,
                           const QString& button2Text,
                           int defaultButtonNumber,
                           int escapeButtonNumber)
{
   dialogCommon("Information.", text);
   return displayMessageBox(parent, QMessageBox::Critical, caption, text, 
                                   button0Text, button1Text, button2Text,
                                   defaultButtonNumber, escapeButtonNumber);
}

/**
 * display question message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::question(QWidget *parent, const QString &caption,
                        const QString& text,
                        int button0, int button1, int button2)
{
   dialogCommon("Question.", text);
   return QtMessageBox::question(parent, caption, text, button0, button1, button2);
}
                     
/**
 * display information message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::question(QWidget *parent, const QString &caption,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text,
                        const QString& button2Text,
                        int defaultButtonNumber,
                        int escapeButtonNumber)
{
   dialogCommon("Question.", text);
   return displayMessageBox(parent, QMessageBox::Question, caption, text, 
                                button0Text, button1Text, button2Text,
                                defaultButtonNumber, escapeButtonNumber);
}

/**
 * display warning message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::warning(QWidget *parent, const QString &caption,
                       const QString& text,
                       int button0, int button1, int button2)
{
   dialogCommon("Warning.", text);
   return QtMessageBox::warning(parent, caption, text, button0, button1, button2);
}
                  
/**
 * display warning message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::warning(QWidget *parent, const QString &caption,
                       const QString& text,
                       const QString& button0Text,
                       const QString& button1Text,
                       const QString& button2Text,
                       int defaultButtonNumber,
                       int escapeButtonNumber)
{
   dialogCommon("Warning.", text);
   return displayMessageBox(parent, QMessageBox::Warning, caption, text, 
                               button0Text, button1Text, button2Text,
                               defaultButtonNumber, escapeButtonNumber);
}

/**
 * display critical message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::critical(QWidget *parent, const QString &caption,
                        const QString& text,
                        int button0, int button1, int button2)
{
   dialogCommon("Error.", text);
   return QtMessageBox::critical(parent, caption, text, button0, button1, button2);
}
                     
/**
 * display critical message, restore cursor, beep, and perhaps speak message.
 */
int 
GuiMessageBox::critical(QWidget *parent, const QString &caption,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text,
                        const QString& button2Text,
                        int defaultButtonNumber,
                        int escapeButtonNumber)
{
   dialogCommon("Error.", text);
   return displayMessageBox(parent, QMessageBox::Critical, caption, text, 
                                button0Text, button1Text, button2Text,
                                defaultButtonNumber, escapeButtonNumber);
}

/**
 * Stuff common to each dialog, restore cursor, beep, and perhaps speak message.
 */
void
GuiMessageBox::dialogCommon(const QString& /*dialogType*/, const QString& /*text*/)
{
   QApplication::beep();
   QApplication::restoreOverrideCursor();
   
   //if (speechGenerator != NULL) {
  //    speechGenerator->speakText(dialogType, QString(text));
  // }
}

/**
 * set the speech generator.
 */
/*
void 
GuiMessageBox::setSpeechGenerator(GuiSpeechGenerator* speechGeneratorIn)
{
   speechGenerator = speechGeneratorIn;
}
*/

/**
 * Load the icon.
 */
void
GuiMessageBox::loadIcon(const QString& filename)
{
   if (dialogIcon == NULL) {
      dialogIcon = new QPixmap;
   }
   QImage image;
   dialogIconValid = false;
   if (image.load(filename) == false) {
      std::cout << "INFO: Failed to dialog icon image: " << filename.toAscii().constData() << std::endl;
   }
   else {
      *dialogIcon = QPixmap::fromImage(image);
      dialogIconValid = true;
   }
}

/**
 * Displays the message box.
 * This code was taken from Qt's "qmessagebox.cpp".  The code has been modified 
 * to display the Caret icon.
 *
****************************************************************************
** $Id: GuiMessageBox.cxx,v 1.2 2007/11/01 15:18:54 john Exp $
**
** Implementation of QMessageBox class
**
** Created : 950503
**
** Copyright (C) 1992-2003 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
 */
int 
GuiMessageBox::displayMessageBox( QWidget *parent, QMessageBox::Icon severity,
                    const QString& caption, const QString& text,
                    const QString& button0Text,
                    const QString& button1Text,
                    const QString& button2Text,
                    int defaultButtonNumber,
                    int escapeButtonNumber )
{
    int b[3];
    b[0] = 1;
    b[1] = button1Text.isEmpty() ? 0 : 2;
    b[2] = button2Text.isEmpty() ? 0 : 3;

    int i;
    for( i=0; i<3; i++ ) {
        if ( b[i] && defaultButtonNumber == i )
            b[i] += QMessageBox::Default;
        if ( b[i] && escapeButtonNumber == i )
            b[i] += QMessageBox::Escape;
    }

    //
    // Fix for bug in QT 4.1 for OSX
    //
    const QString newText = StringUtilities::replace(text,
                                                     "\n",
                                                     "<BR>");
                                                     
    QtMessageBox *mb = new QtMessageBox( caption, newText, severity,
                                       b[0], b[1], b[2],
                                       parent);
    mb->setAttribute(Qt::WA_DeleteOnClose);
    if (dialogIconValid) {
       mb->setIconPixmap(*dialogIcon);
    }
    Q_CHECK_PTR( mb );
    if ( button0Text.isEmpty() )
        mb->setButtonText( 1, "OK");
    else
        mb->setButtonText( 1, button0Text );
    if ( b[1] )
        mb->setButtonText( 2, button1Text );
    if ( b[2] )
        mb->setButtonText( 3, button2Text );

#ifndef QT_NO_CURSOR
    mb->setCursor( Qt::ArrowCursor );
#endif
    return mb->exec() - 1;
}
