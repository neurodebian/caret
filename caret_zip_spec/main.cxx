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
#include <QFile>

#include "BrainSet.h"
#include "CaretVersion.h"
#include "FileUtilities.h"  
#include "SpecFileUtilities.h"

static void usage(const QString& programName);

int
main(int argc, char* argv[])
{

   QApplication app(argc, argv, false);

   //
   // Use just to get debugging info 
   //
   BrainSet::initializeStaticStuff();
   
   if (argc == 1) {
      usage(FileUtilities::basename(argv[0]));
      exit(0);
   }
   else if (argc != 4) {
      std::cerr << " ERROR: Invalid parameters." << std::endl;
      std::cerr << "        Run with no parameters for usage information." << std::endl;
   }
   
   std::cout << "CARET zip spec v" << CaretVersion::getCaretVersionAsString().toAscii().constData()
             << " (" << __DATE__ << ")" << std::endl << std::endl;

   
   //
   // Get the parameter
   //
   const QString zipFileName(argv[1]);
   const QString unzipDirName(argv[2]);
   const QString specFileName(argv[3]);

   //
   // Make sure the zip file does not exist
   //
   if (QFile::exists(zipFileName)) {
      std::cerr << "ERROR: The zip file " << zipFileName.toAscii().constData() << " already exists." << std::endl;
      std::cerr << "   Delete the zip file and rerun program." << std::endl;
      exit(-1);
   }
   
   //
   // Zip the spec file
   //
   QString errorMessage;
   const bool result = SpecFileUtilities::zipSpecFile(specFileName, 
                                                     zipFileName,
                                                     unzipDirName,
                                                     errorMessage);
   if (result) {
      std::cout << errorMessage.toAscii().constData() << endl;
   }
   return result;
}

static void
usage(const QString& programName)
{
   std::cout << " " << endl;
   std::cout << "NAME" << endl;
   std::cout << "   " << programName.toAscii().constData() 
             << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
             << " (" << __DATE__ << ")" 
             << std::endl;
   std::cout << " " << endl;
   std::cout << "SYNOPSIS" << endl;
   std::cout << "   " << programName.toAscii().constData() << " zip-file-name unzip-dir-name spec-file-name" << endl;
   std::cout << " " << endl;
   std::cout << "DESCRIPTION" << endl;
   std::cout << "   The spec file \"spec-file-name\" and its data files are placed" << endl;
   std::cout << "   into the zip \"zip-file-name\".  When unzipped by the user, the" << endl;
   std::cout << "   subdirectory \"unzip-dir-name\" will be created and contain the" << endl;
   std::cout << "   contents of the zip file. " << endl;
   std::cout << " " << endl;
   std::cout << "EXAMPLE " << endl;
   std::cout << "   Zip the spec file \"Human.spec\" and place into the zip file" << endl;
   std::cout << "   \"Human.zip\".  When unzipped by the user, the subdirectory"<< endl;
   std::cout << "   \"human_case_1\" will be created and contain the files from" << endl;
   std::cout << "   the zip file." << endl;
   std::cout << " " << endl;
   std::cout << "      " << programName.toAscii().constData() << " Human.zip human_case_1 Human.spec" << endl;
   std::cout << " " << endl;
   std::cout << " " << endl;
}
