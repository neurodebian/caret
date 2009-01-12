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

#include <QDir>
#include <QFile>
#include <QTextStream>

#include "CommandImageToWebPage.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandImageToWebPage::CommandImageToWebPage()
   : CommandBase("-image-to-web-page",
                 "IMAGE INTO WEB PAGE")
{
}

/**
 * destructor.
 */
CommandImageToWebPage::~CommandImageToWebPage()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandImageToWebPage::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString
CommandImageToWebPage::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<mode> <image-filename-extension> [page-name-prefix]\n"
       + indent9 + "\n"
       + indent9 + "Creates a single web page (.html file) or a set of web pages  \n"
       + indent9 + "for each of the images.\n"
       + indent9 + "\n"
       + indent9 + "\"<mode>\" is one of:\n"
       + indent9 + "   ALL\n"
       + indent9 + "   SINGLE\n"
       + indent9 + "\n"
       + indent9 + "If \"<mode>\" is \"ALL\", all of the images are placed \n"
       + indent9 + "vertically in a single web page.\n"
       + indent9 + "\n"
       + indent9 + "If \"<mode>\" is \"SINGLE\", each image is placed in its own\n"
       + indent9 + "web page and each of the web pages is linked to allow\n"
       + indent9 + "stepping through the images.\n"
       + indent9 + "\n"
       + indent9 + "\"<image-filename-extension>\" is not a wild card but is the\n"
       + indent9 + "last characters of the image filenames.\n"
       + indent9 + "   ie: jpg   implies all files ending in \"jpg\"\n"
       + indent9 + "\n"
       + indent9 + "\"[page-name-prefix]\" is optional, and, if not specified,\n"
       + indent9 + "each of the web pages matches the name of its image file.\n"
       + indent9 + "If \"[page-name-prefix]\" is specified, it is the prefix\n"
       + indent9 + "for all web page names.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandImageToWebPage::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get parameters
   //
   const QString modeName = 
      parameters->getNextParameterAsString("Mode");
   const QString imageFileExtension = 
      parameters->getNextParameterAsString("Image Filename Extension");
   QString pageNamePrefix;
   if (parameters->getParametersAvailable()) {
      pageNamePrefix = parameters->getNextParameterAsString("Page Name Prefix");
   }
   checkForExcessiveParameters();
   
   //
   // Check mode
   //
   enum MODE { MODE_ALL, MODE_SINGLE } mode;
   if (modeName == "ALL") {
      mode = MODE_ALL;
   }
   else if (modeName == "SINGLE") {
      mode = MODE_SINGLE;
   }
   else {
      throw CommandException("Invalid mode: " + modeName);
   }

   //
   // Find all of the image files
   //
   QStringList imageFileNames;
   QDir dir(QString::null, "*");
   for (unsigned int i = 0; i < dir.count(); i++) {
      if (dir[i].endsWith(imageFileExtension)) {
         imageFileNames.push_back(dir[i]);
      }
   }
   
   //
   // Verify that there are images
   //
   const int numImages = imageFileNames.size();
   if (numImages <= 0) {
      throw CommandException("No files ending with " 
                             + imageFileExtension + " were found.");
   }
   
   //
   // If only one image, then use ALL mode
   //
   if (numImages == 1) {
      mode = MODE_ALL;
   }
   
   //
   // Create the web-pages
   //
   const QString space("&nbsp;");
   switch (mode) {
      case MODE_ALL:
         {
            const QString htmlPageName(pageNamePrefix + imageFileNames[0] + ".html");
            QFile file(htmlPageName);
            if (file.open(QFile::WriteOnly)) {
               QTextStream stream(&file);

               stream << "<html>" << "\n";
               stream << "<head></head> " << "\n";
               stream << "<body>" << "\n";
               
               for (int i = 0; i < numImages; i++) {
                  stream << "<img src=" << imageFileNames[i] << " align=\"center\"><br>" << "\n";
                  stream << space << space << imageFileNames[i] << "\n";
                  stream << "<P>" << "\n";
               }
                  
               stream << "</body>" << "\n";
               stream << "</html>" << "\n";
               file.close();

               std::cout << "HTML web page is " 
                         << htmlPageName.toAscii().constData()
                         << std::endl;
            }
            else {
               throw CommandException(htmlPageName + " " + file.errorString());
            }
         }
         break;
      case MODE_SINGLE:
         {
            const QString firstPageName(pageNamePrefix + imageFileNames[0] + ".html");
            const QString firstPageLink("<a href=\"" + firstPageName + "\">first</a>");
            const QString lastPageLink("<a href=\"" + pageNamePrefix + imageFileNames[numImages - 1] + ".html\">last</a>");
            
            for (int i = 0; i < numImages; i++) {
               QString prevPageLink;
               if (i > 0) {
                  prevPageLink = "<a href=\"" + pageNamePrefix + imageFileNames[i - 1] + ".html\">prev</a>";
               }

               QString nextPageLink;
               if (i < static_cast<int>((numImages- 1))) {
                  nextPageLink = "<a href=\"" + pageNamePrefix + imageFileNames[i + 1] + ".html\">next</a>";
               }

               QString name(pageNamePrefix + imageFileNames[i]);
               name += ".html";
               QFile file(name);
               if (file.open(QFile::WriteOnly)) {
                  QTextStream stream(&file);

                  stream << "<html>" << "\n";
                  stream << "<head></head> " << "\n";
                  stream << "<body>" << "\n";

                  stream << firstPageLink << "\n";
                  stream << space << space << "\n";
                  if (prevPageLink.isEmpty() == false) {
                     stream << prevPageLink << "\n";
                  }
                  stream << space << space << "\n";
                  if (nextPageLink.isEmpty() == false) {
                     stream << nextPageLink << "\n";
                  }
                  stream << space << space << "\n";
                  stream << lastPageLink << "\n";
                  stream << "<P> Image Name:" << space << space << imageFileNames[i] << "\n";

                  stream << "<P>" << "\n";
                  stream << "<img src=" << imageFileNames[i] << " align=\"center\">" << "\n";
                  stream << "<P>" << "\n";

                  stream << firstPageLink << "\n";
                  stream << space << space << "\n";
                  if (prevPageLink.isEmpty() == false) {
                     stream << prevPageLink << "\n";
                  }
                  stream << space << space << "\n";
                  if (nextPageLink.isEmpty() == false) {
                     stream << nextPageLink << "\n";
                  }
                  stream << space << space << "\n";
                  stream << lastPageLink << "\n";

                  stream << "<P> Image Name:" << space << space << imageFileNames[i] << "\n";
                  
                  stream << "</body>" << "\n";
                  stream << "</html>" << "\n";
                  file.close();
               }
               else {
                  throw CommandException(name + " " + file.errorString());
               }
            }
            
            std::cout << "First web page is " 
                      << firstPageName.toAscii().constData()
                      << std::endl;
         }
         break;
   }
   //
   // Loop through the images
   //
   for (int i = 0; i < numImages; i++) {
   }
         
}

      

