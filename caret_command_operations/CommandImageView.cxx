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
#include <QGlobalStatic>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>

#include "CommandImageView.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandImageView::CommandImageView()
   : CommandBase("-image-view",
                 "IMAGE VIEW")
{
}

/**
 * destructor.
 */
CommandImageView::~CommandImageView()
{
}

/**
 * get full help information.
 */
QString 
CommandImageView::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<image-file-name>\n"
       + indent9 + "\n"
       + indent9 + "View an image file.\n"
       + indent9 + "\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandImageView::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString imageFileName =
      parameters->getNextParameterAsString("Image File Name");
                                          
   //
   // Make sure that are no more parameters
   //
   checkForExcessiveParameters();

   //
   // Read the image
   //
   QImage image;
   if (image.load(imageFileName) == false) {
      throw CommandException("Unable to load \"" + imageFileName + "\"");
   }
   
   //
   // Show the image
   //
   if (displayQImage(image) != 0) {
      throw CommandException("Image viewing failed.");
   }
}

/**
 * get the script builder parameters.
 */
void 
CommandImageView::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   QStringList fileFilters, fileExtensions;
   FileFilters::getImageSaveFileFilters(fileFilters,
                                        fileExtensions);
   paramsOut.clear();
   paramsOut.addFile("Image File Name", fileFilters);
}

/**
 * display a QImage.
 */
int 
CommandImageView::displayQImage(const QImage& qimage)
{
   //
   // Show the image
   //
   QLabel* label = new QLabel;
   label->setPixmap(QPixmap::fromImage(qimage));

   //
   // There is a limitation with the Mac OSX version of QT in that the program
   // must be built as an application for scrollbars to work correctly.  So, on
   // Mac OSX, simply show the image.
   //   
#ifdef Q_OS_MAC
   label->setFixedSize(qimage.width(), qimage.height());
   label->show();
#else // Q_OS_MAC
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidget(label);
   scrollArea->show();
#endif // Q_OS_MAC

   //
   // Allow the event loop to execute
   //
   return qApp->exec();
}      
      

