
#ifndef __QT_DATA_ENTRY_DIALOG_H__
#define __QT_DATA_ENTRY_DIALOG_H__

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

#include <limits>

#include <QDialog>
#include <QVector>

class QGridLayout;
class QLabel;

/// generic dialog for entering a list of data
class QtDataEntryDialog : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      QtDataEntryDialog(const QString& dialogTitle,
                        const bool addScrollAreaForParameters,
                        QWidget* parent  = 0,
                        Qt::WFlags w = 0);
                        
      // constructor
      ~QtDataEntryDialog();
      
      // add a float parameter (returns index of parameter)
      int addParameterFloat(const QString& labelText,
                            const float initialValue = 0.0,
                            const float stepValue = 1.0,
                            const float minValue = -std::numeric_limits<float>::max(),
                            const float maxValue = std::numeric_limits<float>::max());
                             
      // add an integer parameter (returns index of parameter)
      int addParameterInt(const QString& labelText,
                          const int initialValue = 0,
                          const int stepValue = 1,
                          const int minValue = std::numeric_limits<int>::min(),
                          const int maxValue = std::numeric_limits<int>::max());
                             
      // add a string parameter (returns index of parameter)
      int addParameterString(const QString& labelText,
                             const bool valueMustNotBeEmpty,
                             const QString& initialValue = "");
              
      // get a float parameter's value
      float getParameterFloat(const int indx) const;
              
      // get an int parameter's value
      int getParameterInt(const int indx) const;
              
      // get a string parameter's value
      QString getParameterString(const int indx) const;
      
   protected slots:
      // called when OK button pressed
      void slotOkPushButton();
      
   protected:
      /// add label and data widget to the dialog
      int addToDialog(const QString& labelText,
                       QWidget* dataWidget,
                       const bool valueMustNotBeEmpty = false);
                       
      /// the parameters
      QVector<QWidget*> parameterWidgets;
      
      /// the parameter labels
      QVector<QString> parameterLabels;
      
      /// parameter must not be empty (used for QStrings only)
      QVector<bool> parameterValueMustNotBeEmpty;
      
      /// layout for the widgets
      QGridLayout* parametersGridLayout;
};

#endif // __QT_DATA_ENTRY_DIALOG_H__
