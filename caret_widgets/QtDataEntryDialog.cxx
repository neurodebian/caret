
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

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>

#include "QtDataEntryDialog.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
QtDataEntryDialog::QtDataEntryDialog(const QString& dialogTitle,
                                     const bool addScrollBarsForParameters,
                                     QWidget* parent,
                                     Qt::WFlags w)
   : QDialog(parent, w)
{
   setWindowTitle(dialogTitle);
   
   //
   // Grid layout and optional scrollbars for user's widgets
   //
   QWidget* parametersWidget = NULL;
   if (addScrollBarsForParameters) {
      QWidget* w = new QWidget;
      parametersGridLayout = new QGridLayout(w);
      
      QScrollArea* sa = new QScrollArea;
      sa->setWidgetResizable(true);
      sa->setWidget(w);
      parametersWidget = sa;
   }
   else {
      parametersWidget = new QWidget;
      parametersGridLayout = new QGridLayout(parametersWidget);
   }

   //
   // buttons and layout
   //   
   QPushButton* okPushButton = new QPushButton("OK");
   okPushButton->setAutoDefault(false);
   QObject::connect(okPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOkPushButton()));
   QPushButton* cancelPushButton = new QPushButton("Cancel");
   cancelPushButton->setAutoDefault(false);
   QObject::connect(cancelPushButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   QtUtilities::makeButtonsSameSize(okPushButton,
                                    cancelPushButton);
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(okPushButton);
   buttonsLayout->addWidget(cancelPushButton);
                    
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(parametersWidget);
   dialogLayout->addLayout(buttonsLayout);
}
                  
/**
 * constructor.
 */
QtDataEntryDialog::~QtDataEntryDialog()
{
}

/**
 * called when OK button pressed.
 */
void 
QtDataEntryDialog::slotOkPushButton()
{
   QString msg;
   const int numParams = static_cast<int>(parameterWidgets.size());
   for (int i = 0; i < numParams; i++) {
      if (parameterValueMustNotBeEmpty[i]) {
         const QLineEdit* le = dynamic_cast<QLineEdit*>(parameterWidgets[i]);
         if (le != NULL) {
            if (le->text().isEmpty()) {
               msg.append(parameterLabels[i]);
            }
         }
      }
   }
   if (msg.isEmpty() == false) {
      QString s("The following parameter(s) must contain text\n");
      s.append(msg);
      QMessageBox::critical(this, "ERROR", s, "OK");
      return;
   }
   
   //
   // Allow dialog to close and tell user that values accepted
   //
   accept();
}
      
/**
 * add a float parameter (returns index of parameter).
 */
int 
QtDataEntryDialog::addParameterFloat(const QString& labelText,
                                     const float initialValue,
                                     const float stepValue,
                                     const float minValue,
                                     const float maxValue)
{   
   QDoubleSpinBox* dsb = new QDoubleSpinBox;
   dsb->setMinimum(minValue);
   dsb->setMaximum(maxValue);
   dsb->setSingleStep(stepValue);
   dsb->setValue(initialValue);
   
   return addToDialog(labelText, dsb);
}
                       
/**
 * add an integer parameter (returns index of parameter)
 */
int 
QtDataEntryDialog::addParameterInt(const QString& labelText,
                                   const int initialValue,
                                   const int stepValue,
                                   const int minValue,
                                   const int maxValue)
{
   QDoubleSpinBox* sb = new QDoubleSpinBox;
   sb->setMinimum(minValue);
   sb->setMaximum(maxValue);
   sb->setSingleStep(stepValue);
   sb->setValue(initialValue);
   
   return addToDialog(labelText, sb);
}
                       
/**
 * add a string parameter (returns index of parameter).
 */
int 
QtDataEntryDialog::addParameterString(const QString& labelText,
                                      const bool valueMustNotBeEmpty,
                                      const QString& initialValue)
{
   QLineEdit* le = new QLineEdit;
   le->setText(initialValue);
 
   return addToDialog(labelText, le, valueMustNotBeEmpty);
}
        
/**
 * add label and data widget to the dialog.
 */
int 
QtDataEntryDialog::addToDialog(const QString& labelText,
                               QWidget* dataWidget,
                               const bool valueMustNotBeEmpty)
{
   //
   // Create label and add label and widget to grid layout
   //
   QLabel* label = new QLabel(labelText);
   const int rowNum = parametersGridLayout->rowCount();
   parametersGridLayout->addWidget(label, rowNum, 0);
   parametersGridLayout->addWidget(dataWidget, rowNum, 1);

   //
   // Index of data widget
   //
   const int indx = static_cast<int>(parameterLabels.size());
   
   //
   // Keep track of widget
   //
   parameterLabels.push_back(labelText);
   parameterWidgets.push_back(dataWidget);
   parameterValueMustNotBeEmpty.push_back(valueMustNotBeEmpty);
   
   //
   // return index of data widget
   //
   return indx;
}
                       
/**
 * get a float parameter's value.
 */
float 
QtDataEntryDialog::getParameterFloat(const int indx) const
{
   const QDoubleSpinBox* dsb = dynamic_cast<QDoubleSpinBox*>(parameterWidgets[indx]);
   if (dsb != NULL) {
      return dsb->value();
   }
   
   std::cout << "PROGRAM ERROR: parameter " << parameterLabels[indx].toAscii().constData()
             << " is not a float parameter." << std::endl;
   return 0.0;
}
        
/**
 * get an int parameter's value.
 */
int 
QtDataEntryDialog::getParameterInt(const int indx) const
{
   const QSpinBox* sb = dynamic_cast<QSpinBox*>(parameterWidgets[indx]);
   if (sb != NULL) {
      return sb->value();
   }
   
   std::cout << "PROGRAM ERROR: parameter " << parameterLabels[indx].toAscii().constData()
             << " is not an int parameter." << std::endl;
   return 0;
}
        
/**
 * get a string parameter's value.
 */
QString 
QtDataEntryDialog::getParameterString(const int indx) const
{
   const QLineEdit* le = dynamic_cast<QLineEdit*>(parameterWidgets[indx]);
   if (le != NULL) {
      return le->text();
   }
   
   std::cout << "PROGRAM ERROR: parameter " << parameterLabels[indx].toAscii().constData()
             << " is not a QString parameter." << std::endl;
   return "";
}
