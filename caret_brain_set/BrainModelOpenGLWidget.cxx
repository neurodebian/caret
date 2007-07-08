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

#include "BrainModel.h"
#include "BrainModelOpenGL.h"
#include "BrainModelOpenGLWidget.h"
#include "BrainSet.h"

/**
 * constructor.
 */
BrainModelOpenGLWidget::BrainModelOpenGLWidget(BrainSet* bsin,
                                               QWidget* parent, 
                                               const char* name)
   : QGLWidget(parent, name)
{
   brainModel = NULL;
   brainModelOpenGL = new BrainModelOpenGL(bsin);
}

/**
 * destructor.
 */
BrainModelOpenGLWidget::~BrainModelOpenGLWidget()
{
   delete brainModelOpenGL;
}

/**
 * draw a brain model.
 */
void 
BrainModelOpenGLWidget::drawBrainModel(BrainModel* bm)
{
   brainModel = bm;
   updateGL();
   brainModel = NULL;
}

/**
 * capture an image of a brain model
 */
bool 
BrainModelOpenGLWidget::captureBrainModelImage(BrainModel* bm,
                               const QString& imageFileName,
                               const PreferencesFile::IMAGE_CAPTURE_TYPE imageCaptureType,
                               const int imageQuality)
{
   brainModel = bm;
   
   updateGL();

   bool wroteImage = false;
   
   switch (imageCaptureType) {
      case PreferencesFile::IMAGE_CAPTURE_PIXMAP:
         {
            //
            // Save as a pixmap (mac/windows like this best)
            //
            QPixmap pix = renderPixmap();
            wroteImage = pix.save(imageFileName, "JPEG", imageQuality);
         }
         break;

      case PreferencesFile::IMAGE_CAPTURE_OPENGL_BUFFER:
         {
            //
            // Save with grabbing frame buffer (unix likes this best)
            //
            QImage image(grabFrameBuffer());
            wroteImage = image.save(imageFileName, "JPEG", imageQuality);
         }
   }

   brainModel = NULL;
   
   return wroteImage;
}
                                  
/**
 * initialize the OpenGL.
 */
void 
BrainModelOpenGLWidget::initializeGL()
{
   brainModelOpenGL->initializeOpenGL();
}

/**
 * widget is being resized.
 */
void 
BrainModelOpenGLWidget::resizeGL(int w, int h)
{
   glViewport(0, 0, w, h);
   brainModelOpenGL->updateOrthoSize(0, w, h);
}

/**
 * set the fixed size of the OpenGL widget.
 */
void 
BrainModelOpenGLWidget::setWidgetToFixedSize(const int width, const int height)
{
   setFixedSize(QSize(width, height));
   updateGL();
}      

/**
 * update the widget (draw with OpenGL).
 */
void 
BrainModelOpenGLWidget::paintGL()
{
   //
   // Get the size of the widget for use as viewport
   //
   QSize sz = size();
   const int viewport[4] = { 0, 0, sz.width(), sz.height() };
   
   if (brainModel != NULL) {
      brainModelOpenGL->drawBrainModel(brainModel,
                                       0,  // viewing window number
                                       viewport,
                                       this);
   }
}
