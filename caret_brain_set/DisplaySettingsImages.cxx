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

#include <QGLWidget>
#include <QImage>

#include "BrainSet.h"
#include "DisplaySettingsImages.h"
#include "FileUtilities.h"
#include "ImageFile.h"

/**
 * Constructor.
 */
DisplaySettingsImages::DisplaySettingsImages(BrainSet* bsIn)
   : DisplaySettings(bsIn)
{
   setMainWindowImageNumber(-1);
   imagePositionMode = IMAGE_POSITION_MODE_CENTER_OF_WINDOW; //IMAGE_POSITION_MODE_SCALE_TO_WINDOW;
}

/**
 * Destructor.
 */
DisplaySettingsImages::~DisplaySettingsImages()
{
}

/**
 * reinitialize all display settings.
 */
void 
DisplaySettingsImages::reset()
{
   showImageInMainWindowFlag = false;
   setMainWindowImageNumber(-1);
}

/**
 * update any selections due to changes with loaded data files.
 */
void 
DisplaySettingsImages::update()
{
   if (mainWindowImageNumber >= brainSet->getNumberOfImageFiles()) {
      mainWindowImageNumber = -1;
   }
   
   //
   // Need to make this call so that the opengl image is properly updated
   //
   setMainWindowImageNumber(mainWindowImageNumber);
}

/**
 * set the main window image number.
 */
void 
DisplaySettingsImages::setMainWindowImageNumber(const int n) 
{ 
   openGLFormattedImage = QImage();
   mainWindowImageNumber = -1; 
   
   if ((n >= 0) && (n < brainSet->getNumberOfImageFiles())) {
      ImageFile* img = brainSet->getImageFile(n);
      openGLFormattedImage = QGLWidget::convertToGLFormat(*(img->getImage()));
      mainWindowImageNumber = n;
   }
}      

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsImages::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   mainWindowImageNumber = -1;
   
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsImages") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();

            if (infoName == "showImageInMainWindowFlag") {
               showImageInMainWindowFlag = si->getValueAsBool();
            }
            else if (infoName == "mainWindowImageNumber") {
               const QString imageName = si->getValueAsString();
               
               bool found = false;
               const int num = brainSet->getNumberOfImageFiles();
               for (int m = 0; m < num; m++) {
                  const ImageFile* img = brainSet->getImageFile(m);
                  if (imageName ==
                      FileUtilities::basename(img->getFileName())) {
                     setMainWindowImageNumber(m);
                     found = true;
                     break;
                  }
               }
               
               if (found == false) {
                  errorMessage.append("Image file named ");
                  errorMessage.append(imageName);
                  errorMessage.append(" not loaded");
               }
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsImages::saveScene(SceneFile::Scene& scene, const bool /*onlyIfSelected*/)
{
   const int num = brainSet->getNumberOfImageFiles();

   if ((mainWindowImageNumber >= 0) && (mainWindowImageNumber < num)) {
      SceneFile::SceneClass sc("DisplaySettingsImages");
   
      const ImageFile* img = brainSet->getImageFile(mainWindowImageNumber);
      sc.addSceneInfo(SceneFile::SceneInfo("mainWindowImageNumber",
                                           FileUtilities::basename(img->getFileName())));
      sc.addSceneInfo(SceneFile::SceneInfo("showImageInMainWindowFlag",
                                           showImageInMainWindowFlag));
      scene.addSceneClass(sc);
   }
}

/**
 * get the OpenGL image for the main window (returns NULL if invalid).
 */
QImage* 
DisplaySettingsImages::getMainWindowImage()
{
   if (openGLFormattedImage.width() <= 0) {
      return NULL;
   }
   return &openGLFormattedImage;
}      
                       
