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

#include <QDateTime>
#include <QFont>
#include <QPainter>
#include <QPrinter>

#include "CaretVersion.h"
#include "CommandHelpPDF.h"
#include "DateAndTime.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandHelpPDF::CommandHelpPDF()
   : CommandBase("-help-pdf",
                 "HELP WRITE TO PDF FILE")
{
   //
   // page gaps
   //
   largeGapSize = 12;
   smallGapSize = 4;
}

/**
 * destructor.
 */
CommandHelpPDF::~CommandHelpPDF()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandHelpPDF::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output PDF File Name", FileFilters::getPDFFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandHelpPDF::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-PDF-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Print the help information to a PDF file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandHelpPDF::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Store the commands
   //
   std::vector<CommandBase*> commands;
   CommandBase::getAllCommandsSortedBySwitch(commands);
   const int numCommands = static_cast<int>(commands.size());
   if (numCommands <= 0) {
      return;
   }
   
   //
   // Get name of output file
   // 
   const QString pdfFileExtension(".pdf");
   QString pdfFileName = parameters->getNextParameterAsString("PDF File Name");
   if (pdfFileName.endsWith(pdfFileExtension) == false) {
      pdfFileName += pdfFileExtension;
   }
   
   //
   // Create a printer and painter
   //
   QPrinter printer(QPrinter::HighResolution);
   printer.setOutputFileName(pdfFileName);
   printer.setOutputFormat(QPrinter::PdfFormat);
   QPainter painter;
   painter.begin(&printer);
   
   
   //
   // page sizes
   //
   pageHeight = painter.window().height() - 2 * largeGapSize;
   pageWidth  = painter.window().width() - 2 * smallGapSize;
   
   //
   // Title page font
   //
   QFont titlePageFont("Times", 24, QFont::Bold);
   painter.setFont(titlePageFont);
   
   //
   // Font for text
   //
   QFont font("Times", 14, QFont::Normal);
   painter.setFont(font);

   //
   // Create the command help pages and set each commands page offset
   //
   commandInfo.resize(numCommands);
   for (int i = 0; i < numCommands; i++) {
      createPages(painter,
                  font,
                  commands[i]->getHelpInformation().split('\n'),
                  commandInfo[i].helpPages);
      if (i == 0) {
         commandInfo[i].pageNumber = 0;
      }
      else {
         commandInfo[i].pageNumber = (commandInfo[i - 1].pageNumber
                                      + commandInfo[i - 1].helpPages.count());
      }
      commandInfo[i].command = commands[i];
   }
   
   //
   // page offset for commands
   //
   int pageOffset = 1;
   
   //
   // Create the title pages
   //
   QList<QStringList> titlePages;
   createTitlePage(painter, 
                   titlePageFont,
                   titlePages);
   pageOffset += titlePages.count();
     
   //
   // Create the info pages
   //
   QList<QStringList> infoPages;
   createInfoPage(painter,
                  font,
                  infoPages);
   pageOffset += infoPages.count();
                  
   //
   // Create the table of contents pages
   // only to determine the number of pages
   //
   QList<QStringList> tableOfContentsPages;
   createTableOfContentsPages(painter,
                              font,
                              tableOfContentsPages);
   pageOffset += tableOfContentsPages.count();
                              
   //
   // Update the page numbers of the command help pages
   //
   for (int i = 0; i < numCommands; i++) {
      commandInfo[i].pageNumber += pageOffset;
   }
   
   //
   // Create the table of contents pages AGAIN
   // but this time the page numbers of the commands
   // will be correct
   //
   createTableOfContentsPages(painter,
                              font,
                              tableOfContentsPages);
   //
   // Page number counter
   //
   int pageNumber = 1;
   
   //
   // Print the title pages
   //
   printPages(printer,
              painter,
              titlePageFont,
              titlePages,
              pageNumber,
              true);

   //
   // Print the info pages
   //
   printPages(printer,
              painter,
              font,
              infoPages,
              pageNumber);
              
   //
   // Print the table of contents
   //
   printPages(printer,
              painter,
              font,
              tableOfContentsPages,
              pageNumber);
              
   //
   // Print the command help pages
   //
   for (int i = 0; i < numCommands; i++) {
      printPages(printer,
                 painter,
                 font,
                 commandInfo[i].helpPages,
                 pageNumber);
   }

   painter.end();
}
      
/**
 * print pages of text.
 */
void 
CommandHelpPDF::printPages(QPrinter& printer,
                           QPainter& painter,
                           QFont& font,
                           const QList<QStringList>& pages,
                           int& pageNumberInOut,
                           const bool alignCenterFlag) const
{
   //
   // Set the font
   //
   painter.setFont(font);
   
   //
   // Width of half a page
   //
   const int halfPageWidth = (pageWidth + smallGapSize * 2) / 2;
   
   //
   // Loop through the pages
   //
   for (int i = 0; i < pages.count(); i++) {
      //
      // Y coord of page
      //
      int pageY = largeGapSize;
      
      //
      // Get a page
      //
      const QStringList page = pages.at(i);
      
      //
      // Loop through lines of text in page
      //
      for (int j = 0; j < page.count(); j++) {
         //
         // Get the line of text
         //
         const QString text = page.at(j);
         
         //
         // Height of text
         //
         const int textHeight = getTextHeight(painter, text);
         
         //
         // is text being centered
         //
         int pageX = smallGapSize;
         if (alignCenterFlag) {
            const int textWidth = getTextWidth(painter, text);
            pageX = halfPageWidth - (textWidth / 2);
         }
         
         //
         // Print the text
         //
         painter.drawText(pageX, pageY, text);
         pageY += textHeight;
               
         
      }
      
      //
      // print the page number
      //
      if (pageNumberInOut > 1) {
         painter.drawText(painter.window(),
                          Qt::AlignHCenter | Qt::AlignBottom,
                          QString::number(pageNumberInOut));
      }
                       
      //
      // New page
      //
      printer.newPage();
      pageNumberInOut++;
   }
}
                      
                        
/**
 * create the title page.
 */
void 
CommandHelpPDF::createTitlePage(QPainter& painter,
                                QFont& font,
                                QList<QStringList>& pagesOut) const
{
   pagesOut.clear();
   
   QStringList titlePageText;
   titlePageText << "Caret Command Manual";
   titlePageText << ("Version " + CaretVersion::getCaretVersionAsString());
   titlePageText << ("Date Printed " + DateAndTime::getDateAndTimeAsString());
   titlePageText << "   ";
   titlePageText << "John Harwell, Heather A. Drury, Donna Dierker,";
   titlePageText << "and David C. Van Essen";
   titlePageText << "   ";
   titlePageText << "Washington University School of Medicine";
   titlePageText << "Department of Anatomy and Neurobiology";
   titlePageText << "660 S. Euclid Ave";
   titlePageText << "Saint Louis, Missouri  63110";
   titlePageText << ("Copyright 1995-"
                     + QDateTime::currentDateTime().toString("yyyy")
                     + " Washington University");
   titlePageText << "   ";
   titlePageText << "http://brainmap.wustl.edu/caret";
   
   //
   // Create the title pages
   //
   createPages(painter,
               font,
               titlePageText,
               pagesOut);
}
      
/**
 * create the info page.
 */
void 
CommandHelpPDF::createInfoPage(QPainter& painter,
                               QFont& font,
                               QList<QStringList>& pagesOut) const
{
   pagesOut.clear();
   
   QStringList infoPageText = getGeneralHelpInformation().split('\n');
   
   createPages(painter,
               font,
               infoPageText,
               pagesOut);
}
      
/**
 * create the command table of contents page.
 */
void 
CommandHelpPDF::createTableOfContentsPages(QPainter& painter,
                                           QFont& font,
                                           QList<QStringList>& pagesOut)
{
   //
   // Get the lines for the table of contents
   //
   QStringList tocLines;
   const int numCommands = static_cast<int>(commandInfo.size());
   for (int i = 0; i < numCommands; i++) {
      tocLines << (QString::number(commandInfo[i].pageNumber).leftJustified(7, '.')
                   + commandInfo[i].command->getShortDescription());
   }
   
   //
   // Create the table of contents pages
   //
   createPages(painter,
               font,
               tocLines,
               pagesOut);
}
                                      
/**
 * convert lines of text into pages
 */
void 
CommandHelpPDF::createPages(QPainter& painter,
                            QFont& font,
                            const QStringList& textLines,
                            QList<QStringList>& pagesOut) const
{
   pagesOut.clear();
   
   //
   // Font used for printing command information
   //   
   painter.setFont(font);

   //
   // Start a new page
   //
   int pageY = largeGapSize;
   QStringList currentPage;
   
   //
   // loop through the lines of text
   //
   for (int i = 0; i < textLines.count(); i++) {
      //
      // get a line of the text
      //
      const QString text = textLines.at(i);
      
      //
      // Get height of the line
      //
      const int textHeight = getTextHeight(painter, text);
      
      //
      // Is new page needed
      //
      if (((pageY + textHeight) > pageHeight) &&
          (currentPage.empty() == false)) {
         pagesOut.append(currentPage);
         currentPage.clear();
         pageY = largeGapSize;
      }
          
      //
      // Add text to current page
      //
      currentPage.append(text);
      pageY += textHeight;      
   }
   
   //
   // Don't forget last page
   //
   if (currentPage.empty() == false) {
      pagesOut.append(currentPage);
   }
}

/**
 * get height of text.
 */
int 
CommandHelpPDF::getTextHeight(QPainter& painter,
                              const QString& text) const
{
   //
   // Get height of the line
   //
   const QRect textSizeRect = painter.boundingRect(0, 0, pageWidth, pageHeight,
                                                   Qt::TextSingleLine,
                                                   text);
   const int textHeight = textSizeRect.height() + 2 * smallGapSize;
   
   return textHeight;
}                                              

/**
 * get width of text.
 */
int 
CommandHelpPDF::getTextWidth(QPainter& painter,
                              const QString& text) const
{
   //
   // Get height of the line
   //
   const QRect textSizeRect = painter.boundingRect(0, 0, pageWidth, pageHeight,
                                                   Qt::TextSingleLine,
                                                   text);
   const int textHeight = textSizeRect.width();
   
   return textHeight;
}                                              
