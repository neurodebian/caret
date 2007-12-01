
#ifndef __COMMAND_HELP_PDF_H__
#define __COMMAND_HELP_PDF_H__

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

#include <QStringList>

#include "CommandBase.h"

class QFont;
class QPainter;
class QPrinter;

/// class for printing help as PDF
/// taken from C++ GUI Programming with QT by Blanchette and Summerfield
class CommandHelpPDF : public CommandBase {
   public:
      // constructor 
      CommandHelpPDF();
      
      // destructor
      ~CommandHelpPDF();
      
      // get full help information
      QString getHelpInformation() const;
      
      // get the script builder parameters
      virtual void getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const;
      
      /// command requires GUI flag sent to QApplication to be true
      virtual bool getHasGUI() const { return true; }

   protected:
      // info about command
      class CommandInfo {
         public:
            // the commands help pages
            QList<QStringList> helpPages;
            
            // the page number
            int pageNumber;
            
            // the command
            CommandBase* command;
      };
      
      // execute the command
      void executeCommand() throw (BrainModelAlgorithmException,
                                   CommandException,
                                   FileException,
                                   ProgramParametersException,
                                   StatisticException);
      
      // convert lines of text to pages
      void createPages(QPainter& painter,
                       QFont& font,
                       const QStringList& textLines,
                       QList<QStringList>& pagesOut) const;
                                              
      // create the command table of contents page
      void createTableOfContentsPages(QPainter& painter,
                                      QFont& font,
                                      QList<QStringList>& pagesOut);
                                      
      // create the title page
      void createTitlePage(QPainter& painter,
                           QFont& font,
                           QList<QStringList>& pagesOut) const;
      
      // create the info page
      void createInfoPage(QPainter& painter,
                           QFont& font,
                           QList<QStringList>& pagesOut) const;
      
      // print pages of text
      void printPages(QPrinter& printer,
                      QPainter& painter,
                      QFont& font,
                      const QList<QStringList>& pages,
                      int& pageNumberInOut,
                      const bool alignCenterFlag = false) const;
      
      // get height of text
      int getTextHeight(QPainter& painter,
                        const QString& text) const;

      // get width of text
      int getTextWidth(QPainter& painter,
                        const QString& text) const;

      /// the command info
      std::vector<CommandInfo> commandInfo;
      
      /// large gap for pages
      int largeGapSize;
      
      /// small gap for pages
      int smallGapSize;
      
      /// width of page
      int pageWidth;
      
      /// height of page
      int pageHeight;
      
};

#endif // __COMMAND_HELP_PDF_H__

