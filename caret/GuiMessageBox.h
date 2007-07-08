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

#ifndef __GUI_MESSAGE_BOX_H__
#define __GUI_MESSAGE_BOX_H__

#include <QString>

#include "QtMessageBox.h"

class GuiSpeechGenerator;
class QPixmap;
class QString;
class QWidget;

/// class similar to QMessageBox but restores cursor, beeps, and speaks.
class GuiMessageBox {
   public:

    /// display information message
    static int information( QWidget *parent, const QString &caption,
                            const QString& text,
                            int button0, int button1=0, int button2=0 );
                            
    /// display information message
    static int information( QWidget *parent, const QString &caption,
                            const QString& text,
                            const QString& button0Text = QString::null,
                            const QString& button1Text = QString::null,
                            const QString& button2Text = QString::null,
                            int defaultButtonNumber = 0,
                            int escapeButtonNumber = -1 );

    /// display question message
    static int question( QWidget *parent, const QString &caption,
                         const QString& text,
                         int button0, int button1=0, int button2=0 );
                         
    /// display information message
    static int question( QWidget *parent, const QString &caption,
                         const QString& text,
                         const QString& button0Text = QString::null,
                         const QString& button1Text = QString::null,
                         const QString& button2Text = QString::null,
                         int defaultButtonNumber = 0,
                         int escapeButtonNumber = -1 );

    /// display warning message
    static int warning( QWidget *parent, const QString &caption,
                        const QString& text,
                        int button0, int button1, int button2=0 );
                        
    /// display warning message
    static int warning( QWidget *parent, const QString &caption,
                        const QString& text,
                        const QString& button0Text = QString::null,
                        const QString& button1Text = QString::null,
                        const QString& button2Text = QString::null,
                        int defaultButtonNumber = 0,
                        int escapeButtonNumber = -1 );

    /// display critical message
    static int critical( QWidget *parent, const QString &caption,
                         const QString& text,
                         int button0, int button1, int button2=0 );
                         
    /// display critical message
    static int critical( QWidget *parent, const QString &caption,
                         const QString& text,
                         const QString& button0Text = QString::null,
                         const QString& button1Text = QString::null,
                         const QString& button2Text = QString::null,
                         int defaultButtonNumber = 0,
                         int escapeButtonNumber = -1 );

      /// set the speech generator
      static void setSpeechGenerator(GuiSpeechGenerator* speechGeneratorIn);
      
      /// Load the icon.
      static void loadIcon(const QString& filename);
      
   private:
      /// Stuff common to each dialog.
      static void dialogCommon(const QString& dialogType, const QString& text);
      
      /// Displays the message box.
      static int displayMessageBox(QWidget *parent, QtMessageBox::Icon severity,
                            const QString& caption, const QString& text,
                            const QString& button0Text,
                            const QString& button1Text,
                            const QString& button2Text,
                            int defaultButtonNumber,
                            int escapeButtonNumber);
                    
      /// the speech generator
      static GuiSpeechGenerator* speechGenerator;
      
      /// the icon for display in the dialogs
      static QPixmap* dialogIcon;
      
      /// icon validity
      static bool dialogIconValid;
};

#ifdef __GUI_MESSAGE_BOX_H_MAIN__
GuiSpeechGenerator* GuiMessageBox::speechGenerator = NULL;
QPixmap* GuiMessageBox::dialogIcon = NULL;
bool    GuiMessageBox::dialogIconValid = false;
#endif //__GUI_MESSAGE_BOX_H_MAIN__

#endif // __GUI_MESSAGE_BOX_H__

