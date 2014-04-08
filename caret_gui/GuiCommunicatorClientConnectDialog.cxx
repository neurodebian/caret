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


#include "GuiCommunicatorClientConnectDialog.h"
#include "QtUtilities.h"

#include <QLabel>
#include <QGridLayout>
#include <QLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

/**
 * Constructor
 */
GuiCommunicatorClientConnectDialog::GuiCommunicatorClientConnectDialog(QWidget* parent,
                                                           const QString& programName,
                                                           const QString& defaultHostName,
                                                           const int defaultControlPort,
                                                           const int defaultDataPort,
                                                           const bool defaultRequestUpdates,
                                                           const bool allowUserToEditDataPort,
                                                           const bool allowUserToEditRequestUpdates)
   : WuQDialog(parent)
{
   setModal(true);
   QString caption("Connect to ");
   caption.append(programName);
   setWindowTitle(caption);
   
   //
   // Layout for entire dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   
   int numRows = 0;
   const int HOST_NAME_ROW    = numRows++;
   const int CONTROL_PORT_ROW = numRows++;
   const int DATA_PORT_ROW    = numRows++;
   //
   // Grid Layout for line edits and labels
   //
   QGridLayout* gridLayout = new QGridLayout;
   dialogLayout->addLayout(gridLayout);
   gridLayout->setSpacing(3);
   
   //
   // Label and Line Edit for host name
   //
   gridLayout->addWidget(new QLabel("Host Name"), HOST_NAME_ROW, 0, Qt::AlignLeft);
   hostNameLineEdit = new QLineEdit;
   gridLayout->addWidget(hostNameLineEdit, HOST_NAME_ROW, 1, Qt::AlignLeft);
   hostNameLineEdit->setText(defaultHostName);
   
   //
   // Label and Spin Box for control port
   //
   gridLayout->addWidget(new QLabel("Control Port"), CONTROL_PORT_ROW, 0, Qt::AlignLeft);
   controlPortSpinBox = new QSpinBox;
   controlPortSpinBox->setMinimum(0);
   controlPortSpinBox->setMaximum(65536);
   controlPortSpinBox->setSingleStep(1);
   gridLayout->addWidget(controlPortSpinBox, CONTROL_PORT_ROW, 1, Qt::AlignLeft);
   controlPortSpinBox->setValue(defaultControlPort);
   
   //
   // Label and Line Edit for data port
   //
   if (allowUserToEditDataPort) {
      gridLayout->addWidget(new QLabel("Data Port"), DATA_PORT_ROW, 0, Qt::AlignLeft);
      dataPortSpinBox = new QSpinBox;
      dataPortSpinBox->setMinimum(0);
      dataPortSpinBox->setMaximum(65536);
      dataPortSpinBox->setSingleStep(1);
      gridLayout->addWidget(dataPortSpinBox, DATA_PORT_ROW, 1, Qt::AlignLeft);
      dataPortSpinBox->setValue(defaultDataPort);
   }
   else {
      dataPortSpinBox = NULL;
   }
   
   //
   // Request updates check box
   //
   if (allowUserToEditRequestUpdates) {
      requestUpdatesCheckBox = new QCheckBox("Request Updates");
      dialogLayout->addWidget(requestUpdatesCheckBox, Qt::AlignLeft);
      requestUpdatesCheckBox->setChecked(defaultRequestUpdates);
   }
   else {
      requestUpdatesCheckBox = NULL;
   }
   
   //
   // OK and Cancel Buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   dialogLayout->addLayout(buttonsLayout);
   
   QPushButton* ok = new QPushButton("OK");
   QObject::connect(ok, SIGNAL(clicked()),
                       this, SLOT(accept()));
   buttonsLayout->addWidget(ok);
   
   QPushButton* cancel = new QPushButton("Cancel");
   QObject::connect(cancel, SIGNAL(clicked()),
                    this, SLOT(reject()));
   buttonsLayout->addWidget(cancel);

   //
   // Make the OK and Cancel buttons the same size
   //
   QtUtilities::makeButtonsSameSize(ok, cancel);
}

/**
 * Destructor.
 */
GuiCommunicatorClientConnectDialog::~GuiCommunicatorClientConnectDialog()
{
}

/**
 * Get the hostname.
 */
QString
GuiCommunicatorClientConnectDialog::getHostName() const
{
   return hostNameLineEdit->text();
}

/**
 * Get the control port.
 */
int
GuiCommunicatorClientConnectDialog::getControlPort() const
{
   return controlPortSpinBox->value();
}

/**
 * Get the data port.
 */
int
GuiCommunicatorClientConnectDialog::getDataPort() const
{
   if (dataPortSpinBox != NULL) {
      return dataPortSpinBox->value();
   }
   return -1;
}

/**
 * Get the request updates selection.
 */
bool
GuiCommunicatorClientConnectDialog::getRequestUpdates() const
{
   if (requestUpdatesCheckBox != NULL) {
      return requestUpdatesCheckBox->isChecked();
   }
   return true;
}

