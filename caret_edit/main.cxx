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

#include <iostream>
 
#include <QApplication>

#include "BrainSet.h"
#include "CaretVersion.h"
#include "FileUtilities.h"
#include "QtTextFileEditorDialog.h"
#include "PreferencesFile.h"

/*----------------------------------------------------------------------------------------
 * Print help information.
 */
static void
printHelp(const QString progNameIn)
{
   const QString progName(progNameIn);
   std::cout
      << std::endl
      << "PROGRAM" << std::endl
      << "   " << progName.toAscii().constData()
      << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
      << " (" << __DATE__ << ")" << std::endl
      << std::endl
      << "SYNOPSIS" << std::endl
      << "   " << progName.toAscii().constData() << " [-help] [name-of-file-to-edit]" << std::endl 
      << "" << std::endl
      << "DESCRIPTION OF OPERATIONS" << std::endl
      << "   Edit a text file.  This program handles all types of" << std::endl
      << "   platform dependent line feeds." << std::endl
      << "" << std::endl
      << "   -help" << std::endl
      << "         Display usage information about this program." << std::endl
      << "" << std::endl
      << "   If \"name-of-file-to-edit\" is present, that file is " << std::endl
      << "   automatically loaded into the editor. " << std::endl
      << "" << std::endl;
}

/*----------------------------------------------------------------------------------------
 * The main function.
 */
int
main(int argc, char* argv[])
{
   std::cout << std::endl;
   std::cout << "INFO: To see command line options, Run with \"-help\"" << std::endl;
   std::cout << std::endl;
   
   int fileIndex = -1;
   for (int i = 1; i < argc; i++) {
      QString arg(argv[i]);
      if (arg[0] != '-') {
         fileIndex = i;
         break;
      }
      else if (arg == "-help") {
         printHelp(argv[0]);
         return 0;
      }
   }
   
   QString defaultFileName;
   if (fileIndex > 0) {
      defaultFileName = argv[fileIndex];
   }
   
   //
   // QT application
   //
   QApplication app(argc, argv, true);
   app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

   //
   // Get the preferences file
   //
   BrainSet bs;
   PreferencesFile* pf = bs.getPreferencesFile();
   
   //
   // Create the editor
   //
   QtTextFileEditorDialog* editor = new QtTextFileEditorDialog(0);
   //app.setMainWidget(editor);
   editor->setPreferencesFile(pf);
   editor->show();
   if (defaultFileName.isEmpty() == false) {
      editor->loadFile(defaultFileName);
   }
   int result = app.exec();
   
   return result;
}

