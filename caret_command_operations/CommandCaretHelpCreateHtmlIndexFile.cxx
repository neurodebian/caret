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

#include <QDate>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QTextStream>

#include "CommandCaretHelpCreateHtmlIndexFile.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandCaretHelpCreateHtmlIndexFile::CommandCaretHelpCreateHtmlIndexFile()
   : CommandBase("-caret-help-create-html-index-file",
                 "CARET CREATE HTML INDEX FILE")
{

}

/**
 * destructor.
 */
CommandCaretHelpCreateHtmlIndexFile::~CommandCaretHelpCreateHtmlIndexFile()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandCaretHelpCreateHtmlIndexFile::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   
   paramsOut.addFile("Output HTML File",
                     "HTML Files (*.htm *.html)",
                     "index.html");
   paramsOut.addString("Page Title", "Title Goes Here");
}

/**
 * get full help information.
 */
QString 
CommandCaretHelpCreateHtmlIndexFile::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-html-file-name>\n"
       + indent9 + "<page-title> \n"
       + indent9 + "\n"
       + indent9 + "This command should be run from the top level Caret Help \n"
       + indent9 + "directory.  It generates an HTML file that links to all of\n"
       + indent9 + "HTML files in the current directory's subdirectories.\n"
       + indent9 + "\n"
       + indent9 + "Each subdirectory will become a topic.  Each HTML file found \n"
       + indent9 + "in the subdirectories will be searched for a title element.\n"
       + indent9 + "The title element will be the name of the page listed in the\n"
       + indent9 + "output file.  If a title element is not found, the name of\n"
       + indent9 + "the HTML file will be used.\n"
       + indent9 + "\n"
       + indent9 + "If the title contains spaces, it must be enclosed in \n"
       + indent9 + "double quotes. \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandCaretHelpCreateHtmlIndexFile::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString outputHtmlFileName = 
      parameters->getNextParameterAsString("Output HTML File Name");
   const QString theTitle =   
      parameters->getNextParameterAsString("Title");
      
   //
   // Open the output file
   //
   QFile fileOut(outputHtmlFileName);
   if (fileOut.open(QFile::WriteOnly) == false) {
      throw CommandException("Unable to open "
                             + outputHtmlFileName
                             + " for writing.");
   }
   QTextStream streamOut(&fileOut);
   
   //
   // Output start of HTML tags
   //
   streamOut << "<html> \n";
   streamOut << "<head> \n";
   streamOut << "   <title>" << theTitle << "</title> \n";
   streamOut << "   <meta http-equiv=\"content-type\""
             << " content=\"text/html; charset=ISO-8859-1\"> \n";
   streamOut << "</head> \n";
   streamOut << "<body> \n";
   streamOut << "<center><h1>" << theTitle << "</h1></center> \n";
   streamOut << "<center>" << "Generated " << QDate::currentDate().toString("dd MMM yyyy") << "</center><br> \n";
   
   //
   // Get all subdirectories of the current directory
   //
   QDir topDir(".");
   QStringList subDirList = topDir.entryList((QDir::AllDirs | QDir::NoDotAndDotDot),
                                             QDir::Name);
   //
   // HTML filters
   //
   QStringList htmlFilters;
   htmlFilters << "*.html";
   htmlFilters << "*.htm";
             
   //
   // Loop through the subdirectories
   //
   for (int i = 0; i < subDirList.count(); i++) {
      //
      // Name of a directory
      //
      const QString dirName(subDirList.at(i));
      if (DebugControl::getDebugOn()) {
         std::cout << "SubDir: " << dirName.toAscii().constData() << std::endl;
      }
         
      //
      // Get the HTML files
      //
      QDir subDir(dirName);
      QStringList htmlFileList = subDir.entryList(htmlFilters,
                                                  (QDir::Files
                                                   | QDir::NoSymLinks),
                                                  QDir::Name);
      
      //
      // If HTML files in directory
      //
      if (htmlFileList.count() > 0) {
         //
         // Place titles and URLs into multimap with title being the key
         //
         QMap<QString,QString> pagesMap;
         for (int j = 0; j < htmlFileList.count(); j++) {
            //
            // Name of html file and its title
            //
            const QString fileName(htmlFileList.at(j));
            const QString title = getHtmlPageTitle(dirName, fileName);
            
            //
            // Output List and Link HTML
            //
            const QString hrefTag("<li> <a href=\""
                                  + dirName
                                  + "/"
                                  + fileName
                                  + "\">");
                                  
            //
            // Add to list of pages
            //
            if (pagesMap.find(title) != pagesMap.end()) {
               std::cout << "ERROR: more than one page in the directory "
                         << dirName.toAscii().constData()
                         << " has the same title \""
                         << title.toAscii().constData()
                         << "\""
                         << std::endl
                         << "   Page not output: "
                         << fileName.toAscii().constData()
                         << std::endl;
            }
            else {
               pagesMap[title] = hrefTag;
            }

            if (DebugControl::getDebugOn()) {
               std::cout << "   HTML: "
                         << fileName.toAscii().constData()
                         << std::endl;
               std::cout << "   "
                         << "   TITLE: "
                         << title.toAscii().constData()
                         << std::endl;
            }
         }

         if (pagesMap.isEmpty() == false) {
            //
            // Add Directory Name as H2 to HTML files
            //
            addHtml(streamOut,
                    "<h2>",
                    "</h2>",
                    convertDirectoryNameToTitle(dirName),
                    3);
            
            //
            // Start an unordered list
            //
            addHtml(streamOut,
                    "<ul>",
                    "",
                    "",
                    3);
                    
            //
            // Output the pages
            //
            QStringList pageNames = pagesMap.keys();
            pageNames.sort();
            for (int k = 0; k < pageNames.count(); k++) {
               const QString pageTitle = pageNames.at(k);
               const QString pageHREF  = pagesMap[pageTitle];
               
               addHtml(streamOut,
                       pageHREF,
                       "</a>",
                       pageTitle,
                       6, 
                       true);            
            }
                    
            //
            // Close the unordered list
            //
            addHtml(streamOut,
                    "</ul>",
                    "",
                    "",
                    3);
         }
      }
   }
   
   //
   // End of HTML tags
   //
   streamOut << "</body> \n";
   streamOut << "</html> \n";
   
   //
   // Close output file
   //
   fileOut.close();
}

/**
 * add to the output file.
 */
void 
CommandCaretHelpCreateHtmlIndexFile::addHtml(QTextStream& streamOut,
                                             const QString& htmlStartTag,
                                             const QString& htmlEndTag,
                                             const QString& text,
                                             const int indentation,
                                             const bool addLineBreakFlag)
{
   QString breakTag;
   if (addLineBreakFlag) {
      breakTag = " <br>";
   }
   
   streamOut << QString(indentation, ' ')
             << htmlStartTag
             << text
             << htmlEndTag
             << breakTag
             << "\n";
}

/**
 * get the title from an HTML page.
 */
QString 
CommandCaretHelpCreateHtmlIndexFile::getHtmlPageTitle(const QString& directoryName,
                                                      const QString& fileName)
{
   //
   // Default title to the name of the file
   //
   QString title(fileName);
   if (fileName.endsWith(".htm")) {
      title = fileName.left(fileName.length() - QString(".htm").length());
   }
   else if (fileName.endsWith(".html")) {
      title = fileName.left(fileName.length() - QString(".html").length());
   }
   
   //
   // Open the file
   //
   QFile file(directoryName + "/" + fileName);
   if (file.open(QFile::ReadOnly)) {
      //
      // Read the entire file
      //
      QTextStream stream(&file);
      const QString fileText = stream.readAll();
      
      //
      // Find the start and end title flags
      //
      const QString startTag("<title>");
      const QString endTag("</title>");
      const int startIndex = fileText.indexOf(startTag,
                                              Qt::CaseInsensitive);
      if (startIndex >= 0) {
         const int endIndex = fileText.indexOf(endTag,
                                               startIndex + 1,
                                               Qt::CaseInsensitive);
         if (endIndex > 0) {
            const int textStart = startIndex + startTag.length();
            const int textLength = endIndex - textStart;
            
            if ((textStart > 0) &&
                (textLength > 0)) {
               title = fileText.mid(textStart, textLength);
            }
         }
      }
      
      //
      // Close the file
      //
      file.close();
   }
   
   return title;
}

/**
 * convert directory name to title.
 */
QString 
CommandCaretHelpCreateHtmlIndexFile::convertDirectoryNameToTitle(const QString& dirNameIn)
{
   QString dirName(dirNameIn);
   QString title(dirName.replace('_', ' '));
   
   //
   // Make first character and first after blanks capitals
   //
   for (int i = 0; i < title.length(); i++) {
      QChar ch = title[i];
      if (i == 0) {
         ch = ch.toUpper();
      }
      else if (title[i - 1] == ' ') {
         ch = ch.toUpper();
      }
      title[i] = ch;
   }
   
   return title;
}

