
#ifndef __BRAIN_MODEL_OPENGL_WIDGET_H__
#define __BRAIN_MODEL_OPENGL_WIDGET_H__

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

#include <QString>

#include <QGLWidget>

#include "PreferencesFile.h"

class BrainModel;
class BrainModelOpenGL;
class BrainSet;

/// class used for offscreen opengl rendering
class BrainModelOpenGLWidget : public QGLWidget {
   public:
      /// constructor
      BrainModelOpenGLWidget(BrainSet* bsin,
                             QWidget* parent = 0,
                             const char* name = 0);
      
      
      /// destructor
      ~BrainModelOpenGLWidget();
      
      /// set the brain model that is to be drawn
      void drawBrainModel(BrainModel* bm);
      
      /// capture an image of a brain model (returns true if successful)
      bool captureBrainModelImage(BrainModel* bm,
                                  const QString& imageFileName,
                                  const PreferencesFile::IMAGE_CAPTURE_TYPE imageCaptureType,
                                  const int imageQuality = 75);
                                  
      /// get the renderer for OpenGL
      BrainModelOpenGL* getBrainModelOpenGL() { return brainModelOpenGL; }
      
      /// set the fixed size of the OpenGL widget
      void setWidgetToFixedSize(const int width, const int height);
      
   protected:
      /// rendering for OpenGL
      BrainModelOpenGL* brainModelOpenGL;
      
      /// brain model that is to be drawn
      BrainModel* brainModel;

      /// initialize the OpenGL
      void initializeGL();
      
      /// widget is being resized
      void resizeGL( int, int );
      
      /// update the widget (draw with OpenGL)
      void paintGL();
      
};

#endif // __BRAIN_MODEL_OPENGL_WIDGET_H__
