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

#include <QTextBrowser>

class QKeyEvent;

#ifndef __GUI_HYPER_LINK_TEXT_BROWSER_H__
#define __GUI_HYPER_LINK_TEXT_BROWSER_H__

/// Derivative of QTextBrowser with setSource overridden to display clicked hyperlinks in browser
class GuiHyperLinkTextBrowser : public QTextBrowser {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiHyperLinkTextBrowser(QWidget* parent);
      
      /// Destructor
      ~GuiHyperLinkTextBrowser();
     
      /// set the text
      void setText(const QString& text);
      
   signals:
      /// called if a key is pressed
      void keyPressed();
      
   public slots:      
      /// append text and convert any URLs to hyperlinks
      void append(const QString& text);

      /// append html
      void appendHtml(const QString& html);
      
   private slots:
      /// override to get document to be loaded
      void setSource(const QUrl& url);
      
   private:
      /// called if a key is pressed over the text browser
      void keyPressEvent(QKeyEvent* e);
      
      /// the text for display
      //QString displayText;
};


#endif // __GUI_HYPER_LINK_TEXT_BROWSER_H__

