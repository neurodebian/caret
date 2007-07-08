
#ifndef __GUI_BRAIN_MODEL_OPENGL_KEY_EVENT_H__
#define __GUI_BRAIN_MODEL_OPENGL_KEY_EVENT_H__

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
/// class for processing a key event
class GuiBrainModelOpenGLKeyEvent {
   public:
      /// Constructor
      GuiBrainModelOpenGLKeyEvent(const int keyIn,
                                  const bool shiftKeyDownIn,
                                  const bool altKeyDownIn,
                                  const bool controlKeyDownIn);
                                  
      /// Destructor
      ~GuiBrainModelOpenGLKeyEvent();
      
      /// debug print
      void debug() const;
      
      /// set the key event has been used
      void setKeyEventUsed(const bool b) { keyEventUsed = b; }
      
      /// get the key event has been used
      bool getKeyEventUsed() const { return keyEventUsed; }
      
      /// the key code
      int key;
      
      /// shift key is down flag
      bool shiftKeyDown;
      
      /// alt key is down flag
      bool altKeyDown;
      
      /// control key is down
      bool controlKeyDown;
      
      /// ke event used flag
      bool keyEventUsed;
};

#endif //  __GUI_BRAIN_MODEL_OPENGL_KEY_EVENT_H__

