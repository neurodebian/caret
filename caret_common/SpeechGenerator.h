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

#include <QGlobalStatic>

#ifndef __SPEECH_GENERATOR_H__
#define __SPEECH_GENERATOR_H__

#include <QString>

#ifdef Q_OS_MACX
#include <SpeechSynthesis.h>
#endif // Q_OS_MACX

#ifdef HAVE_MS_SPEECH
#include <atlcomcli.h>
#include <sapi.h>
#endif

/// class for speech generation
class SpeechGenerator {
   public:
      /// Constructor
      SpeechGenerator();
      
      /// Constructor
      ~SpeechGenerator();
      
      /// speak a text string
      void speakText(const QString& text);
      
      /// get speech generator valid
      bool getSpeechGeneratorValid() const { return speechGeneratorValid; }
      
   private:
      /// speech generator valid
      bool speechGeneratorValid;
   
#ifdef Q_OS_MACX
      /// the speech channel
      SpeechChannel speechChannel;
#endif // Q_OS_MACX

#ifdef HAVE_MS_SPEECH
      /// MS voice
      CComPtr<ISpVoice> cpVoice;
#endif

};


#endif // __SPEECH_GENERATOR_H__

