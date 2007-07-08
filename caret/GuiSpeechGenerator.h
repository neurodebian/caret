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

#ifndef __GUI_SPEECH_GENERATOR_H__
#define __GUI_SPEECH_GENERATOR_H__

#include <QString>

class PreferencesFile;
class SpeechGenerator;

/// Class for interfacing speech generator with the user interface
class GuiSpeechGenerator {
   public:
      /// Constructor
      GuiSpeechGenerator(PreferencesFile* pf);
      
      /// Destructor
      ~GuiSpeechGenerator();
      
      /// speak some text
      void speakText(const QString& text, const bool verboseSpeech);
            
      /// Speak some text and possibly some verbose text.
      void speakText(const QString& text, const QString& verboseText);
      
   protected:
      /// preferences file associated with the speech generator interface
      PreferencesFile* preferencesFile;

      /// the speech generator
      SpeechGenerator *speechGenerator;
};

#endif // __GUI_SPEECH_GENERATOR_H__

