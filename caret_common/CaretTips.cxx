
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

#include <QGlobalStatic>

#include "CaretTips.h"

// constructor
CaretTips::CaretTips()
{
   {
      QString tip("To capture an image of any of Caret's Dialog or Windows,  "
                  "hold down the CTRL and SHIFT keys and press the F1 key (on Apple "
                  "computers, hold down the APPLE and SHIFT key and press the F1 key).  "
                  "A dialog will appear that asking the user how the captured image "
                  "should be processed.");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("To change the model viewed in the Main or Viewing Windows, "
                  "hold down the CTRL key (Apple key on Apple Computers) and "
                  "press the F1, F2, ..., F15 key.  F1 corresponds to the first "
                  "model in the Toolbar's Model Selection Control, F2 to the "
                  "second model, etc.");
      tips.push_back(Tip(tip, "misc/keyboard_commands.html"));
   }
   
   {
      QString tip("The keyboard may be used to control the view of the "
                  "surface.  Use the Arrow Keys, the Page Up Key, and "
                  "the Page Down Key to rotate the surface.  While "
                  "holding down the Shift Key, use the Arrow Keys to "
                  "pan the surface.  While holding down the CTRL (Apple "
                  "Key on Apple Computers), use the Up or Down Arrow Keys "
                  "to Zoom the Surface.");
      tips.push_back(Tip(tip, "misc/keyboard_commands.html"));
   }
   
#ifdef Q_OS_WIN32
   {
      QString tip("The installation instructions contain instructions "
                  "for creating a Shortcut on the Desktop and how to "
                  "start Caret by double-clicking a Spec File in Windows "
                  "Explorer.");
      tips.push_back(Tip(tip, "installation/caret5_installation.html"));
   }
#endif // Q_OS_WIN32

   {
      QString tip("Keys may be used to select standard views of a surface. "
                  "The Home Key will reset the view.  A, D, L, M, P, and V "
                  "correspond to Anterior, Dorsal, Lateral, Medial, Posterior, "
                  "and Ventral Views.");
      tips.push_back(Tip(tip, "misc/keyboard_commands.html"));
   }
   
   {
      QString tip("Scenes save the current \"state\" of Caret (displayed "
                  "models and all Display Control settings) so that they "
                  "may be easily reproduced at a later time or by other "
                  "users.  More information about scenes is available on "
                  "the Caret Help Window selected from the Help Menu.");
      tips.push_back(Tip(tip, "faqs/scenes.html"));
   }
   
   {
      QString tip("Use the popup menu.  To display the popup menu, "
                  "click the Right Mouse Button (on Apple Computers "
                  "hold down the CTRL Key and click the mouse button).");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("Use File Menu->Capture Image of Main Window to get "
                  "an image of the graphics contents of the Main Window. "
                  "The captured image may be saved to a file or even "
                  "copied to the Computer's Clipboard so that the image "
                  "may be pasted into another program suchs as Photoshop, "
                  "Powerpoint, or Word.");
      tips.push_back(Tip(tip, "faqs/images.html"));
   }
   
   {
      QString tip("caret_command is a command line program for performing "
                  "an ever increasing number of operations in a command "
                  "line or shell window.  Run caret_command without any "
                  "arguments to see a list of its operations.");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("Use Help Menu->Caret Help for a reference to Caret's "
                  "Graphical User Interface (GUI).");
      tips.push_back(Tip(tip, "index.html"));
   }
   
   {
      QString tip("Use the Preferences Window (available on the File Menu "
                  "or the Apple Menu on Apple Computers) to change the "
                  "background or foreground color.  The foreground color "
                  "is used for text such as the text that lists the scale "
                  "on a color palette bar.");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("The status bar at the bottom of the Main Window lists "
                  "the Mouse Mode of Caret along the the functions performed "
                  "by Keyboard/Mouse operations.");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("Use \"Zip Spec File\" on the File Menu to create a ZIP "
                  "archive of a Spec File and all of its data files.");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("Just about any image file can be used as a background "
                  "for the Main Window.  Use File Menu->Open Image Files "
                  "to laod the image file.  On the Display Control Window, "
                  "select the Images page, select the appropriate image "
                  "file, and check the box next to Show Image in Main "
                  "Window.");
      tips.push_back(Tip(tip, "faqs/images.html"));
   }
   
   {
      QString tip("For editing text files, use the Text File Editor "
                  "available from the Window Menu.");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("Make a movie (MPEG1 or MPEG2 format) using File Menu->"
                  "Record Main Window Images as Movie.  Using Automatic "
                  "mode, adds an image of the Main Window's graphics area "
                  "to the movie each time the graphics area is redrawn.  "
                  "Automatic Rotation or "
                  "Interpolate Surfaces, both on the Surface Menu, are "
                  "useful for movie creation.  Caret can be started with "
                  "the \"-xy\" option to fix the size of the graphics area."
                  "For example, \"caret5 -xy 320 240\".");
      tips.push_back(Tip(tip, "dialogs/record_as_mpeg_dialog.html"));
   }
   
   {
      QString tip("NIFTI is a newer volume format supported by other "
                  "brain mapping software applications such as AFNI, "
                  "BrainVoyager, FreeSurfer, and SPM");
      tips.push_back(Tip(tip, "http://nifti.nimh.nih.gov/"));
   }
   
   {
      QString tip("AFNI is a useful program for processing volume data.");
      tips.push_back(Tip(tip, "http://afni.nimh.nih.gov/afni/"));
   }
   
   {
      QString tip("To automatically load a Spec File and all of its Data Files "
                  "at Caret start up, run \"caret5 -loadspec <spec-file-name>\""
                  "where <spec-file-name> is the name of your spec file.");
      tips.push_back(Tip(tip));
   }
   
   {
      QString tip("At the command line, run \"caret5 -help\" to see "
                  "command line options for caret5.");
      tips.push_back(Tip(tip));
   }

   {
      QString tip("\"Color Keys\" display names and corresponding colors "
                  "for many data types such as borders and paint areas.  "
                  "Color keys may be displayed by pressing the \"Color "
                  "Key\" button on the appropriate Display Control Dialog "
                  "page or by using the popup menu (press right mouse "
                  "button (control + mouse button on Macs) over the Main "
                  "Window's graphics area.");
      tips.push_back(Tip(tip));
   }
}

/**
 * destructor.
 */
CaretTips::~CaretTips()
{
}

/**
 * get the number of Caret tips.
 */
int 
CaretTips::getNumberOfCaretTips() const 
{ 
   return tips.size(); 
}

/**
 * get a tip.
 */
void 
CaretTips::getTip(const int tipNumber,
                  QString& tipText,
                  QString& helpPage) const 
{ 
   tipText  = tips[tipNumber].tip; 
   helpPage = tips[tipNumber].helpPage;
}

//====================================================================================

/**
 * constructor.
 */
CaretTips::Tip::Tip(const QString& tipIn, const QString& helpPageIn)
{
   tip = tipIn;
   helpPage = helpPageIn;
}

/**
 * destructor.
 */
CaretTips::Tip::~Tip()
{
}
