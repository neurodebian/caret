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

#include "BrainSet.h"
#include "CaretVersion.h"
#include "FileUtilities.h"
#include "SpecFileUtilities.h"

static void usage(const QString& programName);

int
main(int argc, char* argv[])
{ 

   QApplication app(argc, argv, false);

   if (argc == 1) {
      usage(FileUtilities::basename(argv[0]));
      exit(0);
   }
   
   std::cout << "CARET copy spec v" << CaretVersion::getCaretVersionAsString().toAscii().constData()
             << " (" << __DATE__ << ")" << std::endl << std::endl;

   bool errorFlag = false;
   SpecFileUtilities::MODE_COPY_SPEC_FILE copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_NONE;
   int firstFileIndex = -1;
   
   for (int i = 1; i < argc; i++) {
      const QString arg(argv[i]);
      if ((arg == "-h") || (arg == "-help")) {
         usage(FileUtilities::basename(argv[0]));
         exit(0);
      }
      else if (arg == "-s") {
         copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES;
      }
      else if (arg == "-d") {
         copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_COPY_ALL;
      }
      else if (arg == "-m") {
         copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES;
      }
      else if (arg[0] != '-') {
         firstFileIndex = i;
         break;
      }
      else {
         errorFlag = true;
      }
   }
   
   if (copyMode == SpecFileUtilities::MODE_COPY_SPEC_FILE_NONE) {
      std::cout << "ERROR: one of \"-d\", \"-m\", or \"-s\" must be specified" << endl;
      exit(-1);
   }
   
   if (errorFlag ||
       (firstFileIndex < 0) ||
       (firstFileIndex != (argc - 2))) {
      std::cout << "ERROR: improper arguments" << endl;
      exit(-1);
   }
   
   const int sourceIndex = firstFileIndex;
   const int targetIndex = firstFileIndex + 1;
   
   const QString sourceSpecFile(argv[sourceIndex]);
   const QString targetSpecFile(argv[targetIndex]);

   QString errorMessage;
   const int result = SpecFileUtilities::copySpecFile(sourceSpecFile, targetSpecFile, copyMode, 
                                                      errorMessage, true);
   if (result != 0) {
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
   std::cout << "   " << programName.toAscii().constData() << " [-d] [-s] source-spec-file target-name" << endl;
   std::cout << " " << endl;
   std::cout << "DESCRIPTION" << endl;
   std::cout << "   The source-spec-file and perhaps its data files are" << endl;
   std::cout << "   copied to the new location/spec file." << endl;
   std::cout << " " << endl;
   std::cout << "   if \"target-name\" is a directory, the name of the " << endl;
   std::cout << "   target spec file will be the same as the source spec" << endl;
   std::cout << "   file." << endl;
   std::cout << " " << endl;
   std::cout << "   Options" << endl;
   std::cout << " " << endl;
   std::cout << "      -d   Copy the data files to the destination directory." << endl;
   std::cout << " " << endl;
   std::cout << "      -m   Move all files from their old location to their" << endl;
   std::cout << "           new location." << endl;
   std::cout << " " << endl;
   std::cout << "      -s   Copy only the spec file, the data files will not" << endl;
   std::cout << "           be copied.  Any absolute or relative path on the" << endl;
   std::cout << "           specified spec file is prepended to the data file" << endl;
   std::cout << "           names in the new spec file." << endl;
   std::cout << " " << endl;
   std::cout << "LIMITATIONS " << endl;
   std::cout << "   Either the source or target spec file must be in the " << endl;
   std::cout << "   current directory." << endl;
   std::cout << " " << endl;
   std::cout << "   If you are using the \"-s\" option to copy only the spec" << endl;
   std::cout << "   file and want a relative path to the data files in the " << endl;
   std::cout << "   target spec file, you must run the command from the " << endl;
   std::cout << "   target directory as shown in the first example and use" << endl;
   std::cout << "   a relative path to specify the spec file that is being" << endl;
   std::cout << "   copied." << endl;
   std::cout << " " << endl;
   std::cout << "EXAMPLES " << endl;
   std::cout << " " << endl;
   std::cout << "   Copy the spec file brain.spec to the current directory." << endl;
   std::cout << "   The data files will not be copied but the path to each of the"<< endl;
   std::cout << "   data files in the spec file will be a RELATIVE path" << endl;
   std::cout << "   since a relative path is used to specify the spec file." << endl;
   std::cout << "   Note that a \".\" is used to specify the current directory" << endl;
   std::cout << "   for the target spec file." << endl;
   std::cout << " " << endl;
   std::cout << "      " << programName.toAscii().constData() << " -s ../../brain/brain.spec ." << endl;
   std::cout << " " << endl;
   std::cout << "   Copy the spec file brain.spec to the current directory." << endl;
   std::cout << "   The data files will not be copied but the path to each of the"<< endl;
   std::cout << "   data files in the spec file will be an ABSOLUTE path" << endl;
   std::cout << "   since an absolute path is used to specify the spec file." << endl;
   std::cout << " " << endl;
   std::cout << "      " << programName.toAscii().constData() << " -s /usr/data/brain/brain.spec ." << endl;
   std::cout << " " << endl;
   std::cout << "   Copy the spec file occipital.spec located in the current" << endl;
   std::cout << "   directory to the directory /usr/people/bob/brain_data." << endl;
   std::cout << "   The data files will also be copied." << endl;
   std::cout << " " << endl;
   std::cout << "      " << programName.toAscii().constData() << " -d occipital.spec /usr/people/bob/brain_data" << endl;
   std::cout << " " << endl;
   std::cout << "   Copy the spec file occipital.spec located in the current" << endl;
   std::cout << "   directory to the directory /usr/people/bob/brain_data." << endl;
   std::cout << "   The data files will not be copied and the path to each of " << endl;
   std::cout << "   the data files in the spec file will be an absolute path" << endl;
   std::cout << "   since the command is run from the directory containing" << endl;
   std::cout << "   the spec file." << endl;
   std::cout << "   " << endl;
   std::cout << "      " << programName.toAscii().constData() << " -s occipital.spec /usr/people/bob/brain_data" << endl;
   std::cout << " " << endl;
}
