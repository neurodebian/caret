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


#ifndef __VE_GUI_COMMUNICATOR_CONNECT_DIALOG_H__
#define __VE_GUI_COMMUNICATOR_CONNECT_DIALOG_H__

#include <QString>
#include "WuQDialog.h"

class QCheckBox;
class QLineEdit;
class QSpinBox;

/// This class is used to connect to other programs via sockets.
class GuiCommunicatorClientConnectDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiCommunicatorClientConnectDialog(QWidget* parent,
                                   const QString& programName,
                                   const QString& defaultHostName,
                                   const int defaultControlPort,
                                   const int defaultDataPort,
                                   const bool defaultRequestUpdates,
                                   const bool allowUserToEditDataPort,
                                   const bool allowUserToEditRequestUpdates);
      
      /// Destructor
      ~GuiCommunicatorClientConnectDialog();
      
      /// get the host name in the dialog
      QString getHostName() const;
      
      /// get the control port in the dialog
      int getControlPort() const;
      
      /// get the data port in the dialog
      int getDataPort() const;
      
      /// get the request updates checkbox
      bool getRequestUpdates() const;
      
   private:
      /// hostname line edit
      QLineEdit* hostNameLineEdit;
      
      /// control port spin box
      QSpinBox* controlPortSpinBox;
      
      /// data port spin box
      QSpinBox* dataPortSpinBox;
      
      /// request update from AFNI checkbox
      QCheckBox* requestUpdatesCheckBox;
};

#endif


