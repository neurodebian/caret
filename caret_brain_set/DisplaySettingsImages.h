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

#ifndef __DISPLAY_SETTINGS_IMAGES_H__
#define __DISPLAY_SETTINGS_IMAGES_H__

#include <QImage>

#include "DisplaySettings.h"

/// Display settings for images
class DisplaySettingsImages : public DisplaySettings {
   public:
      /// image position mode
      enum IMAGE_POSITION_MODE {
         /// place center of image in center of window
         IMAGE_POSITION_MODE_CENTER_OF_WINDOW,
         /// scale image to fit within the window
         IMAGE_POSITION_MODE_SCALE_TO_WINDOW
      };
      
      /// Constructor
      DisplaySettingsImages(BrainSet* bsIn);
      
      /// Destructor
      ~DisplaySettingsImages();

      /// reinitialize all display settings
      void reset();
      
      /// update any selections due to changes with loaded data files
      void update();
   
      /// get the main window image number
      int getMainWindowImageNumber() const { return mainWindowImageNumber; }
      
      /// set the main window image number
      void setMainWindowImageNumber(const int n);
      
      /// get the OpenGL image for the main window (returns NULL if invalid)
      QImage* getMainWindowImage();
      
      /// get the show image in main window
      bool getShowImageInMainWindow() const { return showImageInMainWindowFlag; }
      
      /// set the show image in main window
      void setShowImageInMainWindow(const bool b) { showImageInMainWindowFlag = b; }
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       
      /// get the image position mode
      IMAGE_POSITION_MODE getImagePositionMode() const { return imagePositionMode; }
      
      /// set the image position mode
      void setImagePositionMode(const IMAGE_POSITION_MODE ipm) { imagePositionMode = ipm; }
      
   private:
      /// show image in main window flag
      bool showImageInMainWindowFlag;
      
      /// the main window image number
      int mainWindowImageNumber;
      
      /// the main window image formatted for OpenGL
      QImage openGLFormattedImage;
      
      /// the image position mode
      IMAGE_POSITION_MODE imagePositionMode;
};

#endif // __DISPLAY_SETTINGS_IMAGES_H__

