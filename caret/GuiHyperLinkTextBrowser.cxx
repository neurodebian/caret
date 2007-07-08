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

#include <QKeyEvent>

#include "BrainSet.h"
#include "GuiHyperLinkTextBrowser.h"
#include "GuiMainWindow.h"
#include "StringUtilities.h"
#include "VocabularyFile.h"
#include "global_variables.h"

/**
 * Constructor
 */ 
GuiHyperLinkTextBrowser::GuiHyperLinkTextBrowser(QWidget* parent)
   : QTextBrowser(parent)
{
   //QT4setTextFormat(QTextBrowser::RichText);
}

/**
 * Destructor
 */
GuiHyperLinkTextBrowser::~GuiHyperLinkTextBrowser()
{
}

/**
 * Override of QT's QTextBrowser method.
 * Called when the user clicks a link in this browser.
 */
void
GuiHyperLinkTextBrowser::setSource(const QUrl& url)
{
   const QString s(url.toString());
   
   if (s.startsWith("vocabulary://")) {
      const QString name = s.mid(13);
      VocabularyFile* vf = theMainWindow->getBrainSet()->getVocabularyFile();
      VocabularyFile::VocabularyEntry* ve = vf->getBestMatchingVocabularyEntry(name, false);
      if (ve != NULL) {
         append(ve->getFullDescriptionForDisplayToUser(true));
      }
   }
   else {
      theMainWindow->displayWebPage(s);
   }
}

/**
 * Override of QT's QTextBrowser method.  Find URLs and adds HTML tags to them.
 */
void
GuiHyperLinkTextBrowser::append(const QString& textIn)
{
   //
   // See if string contains a URL
   //
   QString text;
   if (textIn.indexOf("http://") >= 0) {
      //
      // Insert the string with hyperlinks into text browser
      //
      text = StringUtilities::convertURLsToHyperlinks(textIn); 
   }
   else {
      text = textIn;
   }
   text.replace("\n", "<br>");
   
   //QTextBrowser::append(text);
   QString displayText = toHtml();
   displayText.append("<br>");
   displayText.append(text);
   setHtml(displayText);
}

/**
 * append html.
 */
void 
GuiHyperLinkTextBrowser::appendHtml(const QString& html)
{
   QString displayText = toHtml();
   displayText.append("<br>");
   displayText.append(html);
   setHtml(displayText);
}
      
/**
 *
 */
void
GuiHyperLinkTextBrowser::setText(const QString& textIn)
{
   //
   // See if string contains a URL
   //
   QString displayText;
   if (textIn.indexOf("http://") != -1) {
      //
      // Insert the string with hyperlinks into text browser
      //
      displayText = StringUtilities::convertURLsToHyperlinks(textIn); 
   }
   else {
      displayText = textIn;
   }   
   displayText.replace("\n", "<br>");
   QTextBrowser::setHtml(displayText);
}

/**
 * called if a key is pressed over the text browser.
 */
void 
GuiHyperLinkTextBrowser::keyPressEvent(QKeyEvent* /*e*/)
{
   emit keyPressed();
}

