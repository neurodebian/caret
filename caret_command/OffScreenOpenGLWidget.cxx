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

#include <QImage>
#include <QPixmap>

#include "BrainModelOpenGL.h"
#include "BrainSet.h"
#include "OffScreenOpenGLWidget.h"
#include "PreferencesFile.h"

/**
 * constructor.
 */
OffScreenOpenGLWidget::OffScreenOpenGLWidget(QWidget* parent,
                                             Qt::WFlags f)
   : QGLWidget(parent, NULL, f)
{
   openGL = new BrainModelOpenGL;
}
                      
/**
 * destructor.
 */
OffScreenOpenGLWidget::~OffScreenOpenGLWidget()
{
   delete openGL;
   openGL = NULL;
}

/**
 * draw to an image (returns true if successful).
 */
void 
OffScreenOpenGLWidget::drawToImage(BrainSet* bs,
                                   BrainModel* bm,
                                   QImage& imageOut)
{
   resizeGL(width(), height());
   
   brainSet = bs;
   brainModel = bm;
   imageOut.reset();
   
   PreferencesFile* pf = brainSet->getPreferencesFile();
  
   brainSet->setDisplaySplashImage(false);

   double orthoLeft, orthoRight, orthoBottom, orthoTop,
         orthoNear, orthoFar;
   openGL->getOrthographicBox(0, orthoLeft, orthoRight, orthoBottom, orthoTop,
                                 orthoNear, orthoFar);
   brainSet->setDefaultScaling(orthoRight, orthoTop);
   
   //
   // Display lists do not work during screen capture - do not know why.
   // So disable display lists, capture image, re-enable display lists.
   //
   const bool displayListsValid = pf->getDisplayListsEnabled();
   if (displayListsValid) {
      brainSet->clearAllDisplayLists();
      pf->setDisplayListsEnabled(false);
   }

   updateGL();

   switch(pf->getImageCaptureType()) {
      case PreferencesFile::IMAGE_CAPTURE_PIXMAP:
         {
            imageOut = renderPixmap().toImage();
         }
         break;
      case PreferencesFile::IMAGE_CAPTURE_OPENGL_BUFFER:
         {
            updateGL();
            imageOut = grabFrameBuffer();
         }
         break;
   }

   //
   // Re-enable display lists
   //
   if (displayListsValid) {
      pf->setDisplayListsEnabled(true);
   }
}

/**
 * initialize.
 */
void 
OffScreenOpenGLWidget::initializeGL()
{
   openGL->initializeOpenGL(true);
}

/**
 * handle resize event.
 */
void 
OffScreenOpenGLWidget::resizeGL(int width, int height)
{
   windowWidth = width;
   windowHeight = height;

   openGL->updateOrthoSize(0, windowWidth, windowHeight);

}


/**
 * draw the scene.
 */
void 
OffScreenOpenGLWidget::paintGL()
{
   int viewport[4] = { 0, 0, windowWidth, windowHeight };
   openGL->drawBrainModelWebCaret(brainSet,
                                  brainModel,
                                  0,
                                  viewport);
}

