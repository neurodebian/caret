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

#include <iostream>
#include <QGlobalStatic>

#include "SpeechGenerator.h"

/**
 * Constructor.
 */
SpeechGenerator::SpeechGenerator()
{
   speechGeneratorValid = false;
   
#ifdef Q_OS_MACX
   if (NewSpeechChannel(NULL, &speechChannel) == 0) {
      speechGeneratorValid = true;
   }
   else {
      std::cout << "ERROR: Unable to create speech generator."<< std::endl;
   }
#endif // Q_OS_MACX

#ifdef HAVE_MS_SPEECH
   HRESULT hr = cpVoice.CoCreateInstance(CLSID_SpVoice);
   if (SUCCEEDED(hr)) {
      speechGeneratorValid = true;
   }
   else {
      std::cout << "ERROR: Unable to create speech generator: " << hr << std::endl;
   }
#endif
}

/**
 * Constructor.
 */
SpeechGenerator::~SpeechGenerator()
{
   if (speechGeneratorValid) {
#ifdef Q_OS_MACX
      DisposeSpeechChannel(speechChannel);
#endif // Q_OS_MACX

#ifdef HAVE_MS_SPEECH
      cpVoice.Release();
#endif
   }
}

/**
 * speak a text string.
 */
void 
SpeechGenerator::speakText(const QString& text)
{
   if (speechGeneratorValid) {
      if (text.isEmpty() == false) {
#ifdef Q_OS_MACX
         SpeakText(speechChannel, text.toAscii().constData(), text.length());
#endif // Q_OS_MACX

#ifdef HAVE_MS_SPEECH
         const int len = text.length();
         WCHAR* wcharText = new WCHAR[len + 1];
         for (int i = 0; i < len; i++) {
            wcharText[i] = text[i];
         }
         wcharText[len] = '\0';
         cpVoice->Speak(wcharText, 0, NULL);
#endif
      }
   }
}

