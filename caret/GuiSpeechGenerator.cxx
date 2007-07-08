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

#include "GuiSpeechGenerator.h"
#include "PreferencesFile.h"
#include "SpeechGenerator.h"

/**
 * Constructor.
 */
GuiSpeechGenerator::GuiSpeechGenerator(PreferencesFile* pf)
{
   preferencesFile = pf;
   speechGenerator = NULL;
}

/**
 * Destructor.
 */
GuiSpeechGenerator::~GuiSpeechGenerator()
{
   delete speechGenerator;
}

/**
 * Speak some text and possibly some verbose text.
 */
void
GuiSpeechGenerator::speakText(const QString& text, const QString& verboseText)
{
   QString msg(text);
   if (preferencesFile != NULL) {
      if (preferencesFile->getSpeechType() == PreferencesFile::SPEECH_TYPE_VERBOSE) {
         msg.append(verboseText);
      }
   }
   speakText(msg, false);
}

/**
 * speak some text.
 */
void 
GuiSpeechGenerator::speakText(const QString& text, const bool verboseSpeech)
{
   bool speakIt = false;
   if (preferencesFile != NULL) {
      switch (preferencesFile->getSpeechType()) {
         case PreferencesFile::SPEECH_TYPE_OFF:
            speakIt = false;
            break;
         case PreferencesFile::SPEECH_TYPE_NORMAL:
            if (verboseSpeech) {
               speakIt = false;
            }
            else {
               speakIt = true;
            }
            break;
         case PreferencesFile::SPEECH_TYPE_VERBOSE:
            speakIt = true;
            break;
      }
   }
   
   if (speakIt) {
      if (speechGenerator == NULL) {
         speechGenerator = new SpeechGenerator;
      }
      if (speechGenerator != NULL) {
         if (speechGenerator->getSpeechGeneratorValid()) {
            speechGenerator->speakText(text);
         }
      }
   }
}

      
