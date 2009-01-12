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


#ifndef __GUI_HELP_ASSISTANT_WINDOW_H__
#define __GUI_HELP_ASSISTANT_WINDOW_H__

#include <QAssistantClient>

/// dialog for showing help in a qt assistant window
class GuiHelpAssistantWindow : public QAssistantClient {
   Q_OBJECT
   
   public:
      // conostructor
      GuiHelpAssistantWindow(const QString& path,
                             QObject* parent = 0);
                             
      // destructor
      ~GuiHelpAssistantWindow();
      
      // show help page
      virtual void showPage(const QString& pageNameIn);
      
   protected slots:
      // called if error
      void showError(const QString& message);
      
   protected:
};

#endif // __GUI_HELP_ASSISTANT_WINDOW_H__

