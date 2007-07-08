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


//
// This file contains the main function
//

#include <QGlobalStatic>

#ifdef Q_OS_WIN32
//#include <crtdbg.h>
#endif

#include <vector.h>
#include <exception>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDir>
#include <QGLWidget>
#include <QImageIOPlugin>
#include <QPluginLoader>
#include <QStringList>
#include <QStyleFactory>
#include <QDateTime>

#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GuiDataFileDialog.h"
#include "GuiMacOSXApplication.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowFileActions.h"
#include "GuiMainWindowFileMenu.h"
#include "GuiMessageBox.h"
#include "GiftiDataArrayFile.h"
#include "SpecFile.h"
#include "SpecFileUtilities.h"
#include "StringUtilities.h"

#define CARET_MAIN_FLAG
#include "global_variables.h"
#undef  CARET_MAIN_FLAG

enum INITIAL_SPEC_MODE {
   INITIAL_SPEC_MODE_NONE,
   INITIAL_SPEC_MODE_LOAD,
   INITIAL_SPEC_MODE_DIALOG
};

static bool debugFlag = false;
static QString initialSpecFile;
static INITIAL_SPEC_MODE initialSpecMode = INITIAL_SPEC_MODE_NONE;
static bool glTimingFlag = false;

static int openGLsizeX = -1;
static int openGLsizeY = -1;

/**
 * List the available QT GUI styles.
 */
static void
listGuiStyles()
{
   QStringList styles = QStyleFactory::keys();
   if (styles.count() > 0) {
      std::cout << "      -style  style-name" << std::endl
                << "         Set the user-interface style where \"style-name\" is one of:" << std::endl;
      for (QStringList::Iterator it = styles.begin(); it != styles.end(); it++) {
         QString s = *it;
         std::cout << "            " << s.toAscii().constData() << std::endl;
      }
      std::cout << "" << std::endl;
   }
}

/**
 * Print command line option help.
 */
static void
printHelp(const QString& programNameIn)
{
   const QString programName(FileUtilities::basename(programNameIn));
   
   std::cout 
   << "NAME" << std::endl
   << "   " << programName.toAscii().constData() << std::endl
   << "" << std::endl
   << "SYNOPSIS" << std::endl
   << "   " << programName.toAscii().constData() << " [options]" << std::endl
   << "" << std::endl
   << "DESCRIPTION" << std::endl
   << "   Visualize cerebral and cerebellar cortical maps." << std::endl
   << "" << std::endl
   << "   If spec-file-name is specified, the spec file by that name will" << std::endl
   << "   be loaded into a spec file dialog." << std::endl
   << "" << std::endl
   << "   " << programName.toAscii().constData() << " has the following options:" << std::endl
   << "" << std::endl
   << "      -debug" << std::endl
   << "         Enable the printing of debugging information to the console." << std::endl
   << "" << std::endl
   << "      -gl-timing" << std::endl
   << "         Adds a menu item to the view menu to enable the timing" << std::endl
   << "         of OpenGL graphics." << std::endl
   << "" << std::endl
   << "      -h  or  -help" << std::endl
   << "         Displays this help information." << std::endl
   << "" << std::endl
   << "      -loadspec  <spec-file-name>" << std::endl
   << "         Automatically loads all of the data files in \"spec-file-name\" at" << std::endl
   << "         startup." << std::endl
   << "" << std::endl
   << "      -giftiXML" << std::endl
   << "         Enable prototype GIFTI Node Attribute files." << std::endl
   << "" << std::endl
   << "      -notext" << std::endl
   << "         Inhibits use of text in OpenGL drawing such as the text" << std::endl
   << "         shown on volume slices indicating stereotaxic coordinates." << std::endl
   << "         This option may prevent crashes on Linux systems with " << std::endl
   << "         FreeType library problems." << std::endl
   << "" << std::endl
   << "      -read-time" << std::endl
   << "         Enable printing of time required to read each file." << std::endl
   << "" << std::endl
   << "      -spec  <spec-file-name>" << std::endl
   << "         Automatically loads \"spec-file-name\" into the spec file dialog at" << std::endl
   << "         startup." << std::endl
   << "" << std::endl;
   
   listGuiStyles();

   std::cout
   << "      -xy  X  Y" << std::endl
   << "         Set the main window graphics to be \"X by Y\" size." << std::endl
   << "         This is typically used when recording to MPEG so that" << std::endl
   << "         the video is a specific size." << std::endl
   << std::endl;
   
   exit(0);
}

/**
 * process the command line options
 */
static void
processCommandLineOptions(int argc, char* argv[])
{
   GiftiDataArrayFile::setGiftiXMLEnabled(false);
   for (int i = 1; i < argc; i++) {
      const QString arg(argv[i]);
      if (arg == "-debug") {
         debugFlag = true;
      }
      else if ((arg == "-h") || (arg == "-help")) {
         printHelp(argv[0]);
      }
      else if (arg == "-loadspec") {
         i++;
         if (i >= argc) {
            std::cerr << "ERROR: missing name of spec file on \"-loadspec\" option." << std::endl;
            exit(-1);
         }
         initialSpecFile = argv[i];
         initialSpecMode = INITIAL_SPEC_MODE_LOAD;
      }
      else if (arg == "-spec") {
         i++;
         if (i >= argc) {
            std::cerr << "ERROR: missing name of spec file on \"-spec\" option." << std::endl;
            exit(-1);
         }
         initialSpecFile = argv[i];
         initialSpecMode = INITIAL_SPEC_MODE_DIALOG;
      }
      else if (arg == "-style") {
         i++;  // -style is used by QApplication
      }
      else if (arg == "-test") {
         DebugControl::setTestFlag(true);
      }
      else if (arg == "-gl-timing") {
         glTimingFlag = true;
      }
      else if (arg == "-giftiXML") {
         GiftiDataArrayFile::setGiftiXMLEnabled(true);
      }
      else if (arg == "-notext") {
         BrainModelOpenGL::setOpenGLTextEnabled(false);
      }
      else if (arg == "-read-time") {
         DebugControl::setFileReadTimingFlag(true);
      }
      else if (arg == "-xy") {
         i++;
         if (i >= argc) {
            std::cerr << "ERROR: Window size missing on \"-xy\" option." << std::endl;
            exit(-1);
         }
         openGLsizeX = QString(argv[i]).toInt();
         i++;
         if (i >= argc) {
            std::cerr << "ERROR: Window Y size missing on \"-xy\" option." << std::endl;
            exit(-1);
         }
         openGLsizeY = QString(argv[i]).toInt();
      }
      else if (arg.mid(0, 4) == "-psn") {
         // ignore this arg which occurs if a spec file is double clicked on in mac finder
      }
      else {
         if (StringUtilities::endsWith(arg, SpecFile::getSpecFileExtension())) {
            initialSpecFile = arg;
            initialSpecMode = INITIAL_SPEC_MODE_DIALOG;
         }
         else {
            std::cerr << "ERROR: Unrecognized command line option: " << arg.toAscii().constData() << std::endl;
            //exit(-1);
         }
      }
   }
}

/*
static void
testRE(const QString& filenameIn)
{
   QString directory,
   QString species;
   QString casename;  
   QString anatomy;
   QString hemisphere;   
   QString description;
   QString descriptionNoType;
   QString theDate;
   QString numNodes;
   QString extension; 
   
   if (FileUtilities::parseCaretDataFileName(filenameIn,
                                         directory,
                                         species,
                                         casename,
                                         anatomy,
                                         hemisphere,
                                         description,
                                         descriptionNoType,
                                         theDate,
                                         numNodes,
                                         extension)) {
      std::cout << "reassembled: "
         << FileUtilities::reassembleCaretDataFileName(
                                         directory,
                                         species,
                                         casename,
                                         anatomy,
                                         hemisphere,
                                         description,
                                         theDate,
                                         numNodes,
                                         extension) << std::endl << std::endl;
   }
}

static void
testRegularExpressions()
{
   
   std::cout << std::endl << std::endl;
   testRE("Human.colin.L.surface_shape");
   testRE("Human.colin.L.12345.surface_shape");
   testRE("Human.colin.L.2003-05-12.12345.surface_shape");
   testRE("Human.colin.L.03-05-12.12345.surface_shape");
   testRE("Human.colin.L.2003-05.12345.surface_shape");
   testRE("Human.colin.L.03-05.12345.surface_shape");
   testRE("Human.colin.Cerebral.R.71723.surface_shape");
   testRE("Human.colin.Cerebral.R.ATLAS.QuickStart.03-05.71723.spec");
   testRE("Human.colin.Cerebral.R.COMPOSITE.03-05-25.71723.paint");
   testRE("Human.colin.Cerebral.R.FIDUCIAL.TLRC.711-2B-afni.71723.coord");
   testRE("Human.colin.R.COMPOSITE_FUNCTIONAL_QuickStart.03-05-26.71723.metric");
   testRE("Human.colin.R.VISION.COMPOSITE.MapToSPM.03-05.fociproj");
   testRE("Human.colin.Cerebral.R.FLAT.CartSTD.71723.coord");
   testRE("Human.colin.Cerebral.R.SPHERE.STD.71723.coord");
   std::cout << std::endl << std::endl;
}
*/

/**
 * Unexpected handler
 */
void unexpectedHandler()
{
  std::cout << "WARNING: unhandled exception." << std::endl;
  if (theMainWindow != NULL) {
     const QString msg("Caret will be terminating due to an unexpected exception.\n"
                       "abort() will be called and a core file may be created.");
     GuiMessageBox::critical(theMainWindow, "ERROR", msg, "OK");
  }
  
  abort();
}

/**
 * New handler
 */
void newHandler()
{
   std::ostringstream str;
   str << "\n"
       << "OUT OF MEMORY\n"
       << "\n"
       << "This means that Caret is unable to get memory that it needs.\n"
       << "Possible causes:\n"
       << "   (1) Your computer lacks sufficient RAM.\n"
       << "   (2) Swap space is too small (you might increase it)."
       << "\n";
   std::cout << str.str().c_str() << std::endl;
   
   if (theMainWindow != NULL) {
      QString msg(str.str().c_str());
      msg.append("\nIf you choose to continue, Caret may crash.\n");
      if (GuiMessageBox::critical(theMainWindow, "OUT OF MEMORY", msg, 
                                  "Continue", "Exit Caret") == 1) {
         exit(0);
      }
   }
}

/*
 * Can be used to print all events (and there are lots of them)
class CatchEvents : public QObject {
   protected:
      bool eventFilter(QObject* o, QEvent* e) {
         std::cout << "Class/Name: " << o->className() << ", " << o->name() << std::endl;
         return false;
      }
};
*/

/**
 * THE program's main.
 */
int
main(int argc, char* argv[])
{
#ifdef Q_OS_WIN32
   //
   // Disables some window debugging stuff that gets stuck in calls to new()
   //
   //_CrtSetDbgFlag(0);
#endif

   //
   // Set handlers in case sh*t happens
   //
   std::set_unexpected(unexpectedHandler);
   std::set_new_handler(newHandler);

/*
#ifdef Q_OS_LINUX
   //
   // Set style to Motif for Linux so that combo boxes use option menus
   //
   bool haveStyle = false;
   for (int i = 1; i < argc; i++) {
      if (QString(argv[i]) == "-style") {
         haveStyle = true;
         break;
      }
   }
   if (haveStyle == false) {
      //QT4 CONVERSION QApplication::setStyle(new QMotifStyle);
   }
#endif
*/

   QString allArgs;
   for (int i = 1; i < argc; i++) {
      allArgs.append(argv[i]);
      allArgs.append("\n");
   }

   //
   // needed for static linking to have JPEG support
   //
   Q_IMPORT_PLUGIN(QJpegPlugin)
   Q_IMPORT_PLUGIN(QGifPlugin)

#ifdef Q_OS_MACX
   GuiMacOSXApplication app(argc, argv);
#else   
   QApplication app(argc, argv);
#endif
   
   /// shows all events  app.installEventFilter(new CatchEvents);
   
   processCommandLineOptions(argc, argv);
      
   std::cout << "Set the environment variable CARET_DEBUG for debugging information." << std::endl;
   DebugControl::setDebugOnWithEnvironmentVariable("CARET_DEBUG");
   if (debugFlag) {
      DebugControl::setDebugOn(true);
   }

   if (getenv("CARET_TEST") != NULL) {
      DebugControl::setTestFlag(true);
   }
   
   //
   // Make sure OpenGL is available.
   //
   if (!QGLFormat::hasOpenGL()) {
      qWarning( "This system has no OpenGL support. Exiting." );
      return -1;
   }

   //
   // Setup OpenGL
   //
   QGLFormat glfmt;
   glfmt.setDoubleBuffer(true);
   glfmt.setDirectRendering(true);
   if ((getenv("CARET_TIMING") != NULL) || glTimingFlag) {
      glfmt.setDoubleBuffer(false);
      glTimingFlag = true;
   }
   QGLFormat::setDefaultFormat(glfmt);
   
   //
   // Get byte order
   //
   if (DebugControl::getDebugOn()) {
      if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
         std::cout << "big endian system" << std::endl;
      }
      else {
         std::cout << "little endian system" << std::endl;
      }
      
      QDesktopWidget* dt = QApplication::desktop();
      std::cout << "Screen size (" << dt->width() << ", " << dt->height() << ")" << std::endl;
   }
   
   //
   // Create and display the main window.
   //
   theMainWindow = new GuiMainWindow(glTimingFlag, openGLsizeX, openGLsizeY);
   //theMainWindow->resize(500, 500);
   //app.setMainWidget(theMainWindow);
   theMainWindow->show();
   app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
   
#ifdef Q_OS_MACX
   //
   // Enable getting the open file event
   //
   app.setGuiMainWindow(theMainWindow);
#endif

   if (initialSpecFile.isEmpty() == false) {
      if (QFile::exists(initialSpecFile) == false) {
         std::cerr << "The spec file " << initialSpecFile.toAscii().constData() 
                   << " specified on the command line not found." << std::endl;
         initialSpecFile = "";
      }
   }
   else if (initialSpecFile.isEmpty()) {
      std::vector<QString> specFiles;
      SpecFileUtilities::findSpecFilesInDirectory(QDir::currentPath(), specFiles);
      if (specFiles.size() == 1) {
         initialSpecFile = specFiles[0];
         initialSpecMode = INITIAL_SPEC_MODE_DIALOG;
      }
      else if (specFiles.size() > 1) {
         //
         // Show file dialog for choosing a spec file
         //
         theMainWindow->getFileActions()->openSpecFileSlot();
      }
   }
   
   //
   // Without this and if "-loadspec" option is used, the caret image will fail to be drawn
   //
   GuiBrainModelOpenGL::updateAllGL();
   
   QString welcomeMessage;
   const QTime timeNow(QTime::currentTime());
   if (timeNow.hour() < 12) {
      welcomeMessage.append("Good morning.");
   }
   else if (timeNow.hour() < 18) {
      welcomeMessage.append("Good afternoon.");
   }
   else {
      welcomeMessage.append("Good evening.");
   }
   welcomeMessage.append("  Welcome to carrot.");
   theMainWindow->speakText(welcomeMessage, false);
   
   //
   // Will initialize some file filters needed by GUI components
   //
   GuiDataFileDialog* dfd = new GuiDataFileDialog;
   delete dfd;

   if (initialSpecFile.isEmpty() == false) {
      QFileInfo fileInfo(initialSpecFile);
      switch (initialSpecMode) {
         case INITIAL_SPEC_MODE_NONE:
            break;
         case INITIAL_SPEC_MODE_LOAD:
            {
               SpecFile sf;
               try {
                  QString path(FileUtilities::dirname(initialSpecFile));
                  if (QDir::isRelativePath(path)) {
                     path = QDir::currentPath();
                     path.append("/");
                     path.append(initialSpecFile);
                     initialSpecFile = QDir::cleanPath(path);
                  }
                  sf.readFile(initialSpecFile);
                  sf.setAllFileSelections(SpecFile::SPEC_TRUE);
                  theMainWindow->loadSpecFilesDataFiles(sf, NULL);
               }
               catch (FileException& e) {
                  GuiMessageBox::warning(theMainWindow, "ERROR", e.whatQString(), "OK");
               }
            }
            break;
         case INITIAL_SPEC_MODE_DIALOG:
            theMainWindow->readSpecFile(fileInfo.absoluteFilePath());
            break;
      }
   }
   
   if (QFile::exists(theMainWindow->getBrainSet()->getCaretHomeDirectory()) == false) {
      QString msg("It appears that Caret is unable to determine its\n"
                      "installation directory.  Please set the environment\n"
                      "variable CARET5_HOME to the Caret installation\n"
                      "directory.  Until this is corrected, some Caret features\n"
                      "may not operate properly.");                      
       GuiMessageBox::warning(theMainWindow, "Install Warning", msg, "OK");
   }
   
   //
   // Show command line arguments in an information dialog
   //
   //GuiMessageBox::information(theMainWindow, "args", allArgs, "OK");
   
   int result = -1;
   try {
      result = app.exec();
   }
   catch (BrainModelAlgorithmException& e) {
      QString msg("PROGRAM ERROR: BrainModelAlgorithmException not caught (this should not happen):\n"
                  "Please copy the contents of this message and send to john@brainvis.wustl.edu.");
      msg.append(e.whatQString());
      std::cerr << msg.toAscii().constData() << std::endl;
   }
   catch (FileException& e) {
      QString msg("PROGRAM ERROR: FileException not caught (this should not happen):\n"
                  "Please copy the contents of this message and send to john@brainvis.wustl.edu.");
      msg.append(e.whatQString());
      std::cerr << msg.toAscii().constData() << std::endl;
   }
   catch (std::exception& e) {
      QString msg("PROGRAM ERROR: std::exception not caught (this should not happen):\n"
                  "Please copy the contents of this message and send to john@brainvis.wustl.edu.");
      msg.append(e.what());
      std::cerr << msg.toAscii().constData() << std::endl;
   }
   //catch (std::bad_alloc) {
   //   std::cout << "Out of memory (bad_alloc) " << std::endl;
   //}
      
   return result;
}
