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

#ifndef __GUI_BRAIN_MODEL_OPENGL_MOUSE_EVENT_H__
#define __GUI_BRAIN_MODEL_OPENGL_MOUSE_EVENT_H__

/// Class contains information about a mouse event
class GuiBrainModelOpenGLMouseEvent {
   public:
      /// event type  (cannot use name "MOUSE_EVENT" because it is a MFC type)
      enum BMO_MOUSE_EVENT {
         MOUSE_LEFT_CLICK,
         MOUSE_LEFT_PRESS,
         MOUSE_LEFT_RELEASE,
         MOUSE_LEFT_SHIFT_PRESS,
         MOUSE_LEFT_CONTROL_PRESS,
         MOUSE_LEFT_MOVE,
         MOUSE_LEFT_SHIFT_MOVE,
         MOUSE_LEFT_CONTROL_MOVE,
         MOUSE_LEFT_ALT_MOVE
      };
      
      /// Constructor
      GuiBrainModelOpenGLMouseEvent(const BMO_MOUSE_EVENT me,
                                    const int mx,
                                    const int my,
                                    const int dx,
                                    const int dy);
      
      /// Destructor
      ~GuiBrainModelOpenGLMouseEvent();
      
      /// the mouse event
      BMO_MOUSE_EVENT event;
      
      /// current mouse X position
      int x;

      /// current mouse Y position
      int y;
      
      /// change in mouse X position
      int dx;
      
      /// change in mouse Y position
      int dy;
};

#endif // __GUI_BRAIN_MODEL_OPENGL_MOUSE_EVENT_H__

