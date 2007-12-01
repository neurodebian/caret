
#ifndef __GUI_CARET_COMMAND_WIDGET_H__
#define __GUI_CARET_COMMAND_WIDGET_H__

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

#include <QGroupBox>
#include <QObject>

#include "ScriptBuilderParameters.h"

class CommandBase;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QLineEdit;
class QSpinBox;
class QTextEdit;

//=============================================================================
/// class for parameters
class GuiCaretCommandParameter : public QObject {
   Q_OBJECT 
   public:
      // constructor
      GuiCaretCommandParameter();
      
      // destructor
      virtual ~GuiCaretCommandParameter();
      
      // get parameter value as text
      virtual QString getParameterValueAsText() const = 0;
      
      // set parameter value from text
      virtual void setParameterValueFromText(const QString& s) = 0;
      
      // see if parameter can be blanks
      virtual bool getParameterAllowedToBeEmpty() const { return false; }
      
      // get description of parameter
      QString getParameterDescription() const { return parameterDescription; }
      
      // create a parameter
      static GuiCaretCommandParameter* createParameter(
                                    const ScriptBuilderParameters::Parameter* parameter,
                                    QGridLayout* gridLayout);
   signals:
      // emitted when data is modified
      void signalDataModified();
      
   protected:
      // set the parameter description
      void setParameterDescription(const QString& parameterDescriptionIn);
      
      /// description of parameter
      QString parameterDescription;
};

//=============================================================================
/// class for float parameter
class GuiCaretCommandParameterFloat : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterFloat(const ScriptBuilderParameters::Parameter* parameter,
                                    QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterFloat();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected:
      // double spin box for float value
      QDoubleSpinBox* doubleSpinBox;
};

//=============================================================================
/// class for int parameter
class GuiCaretCommandParameterInt : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterInt(const ScriptBuilderParameters::Parameter* parameter,
                                    QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterInt();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected:
      // spin box for int value
      QSpinBox* spinBox;
};

//=============================================================================
/// class for boolean parameter
class GuiCaretCommandParameterBoolean : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterBoolean(const ScriptBuilderParameters::Parameter* parameter,
                                    QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterBoolean();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected:
      // combo box for boolean value
      QComboBox* comboBox;
};

//=============================================================================
/// class for data file parameter
class GuiCaretCommandParameterFile : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterFile(const ScriptBuilderParameters::Parameter* parameter,
                                   QGridLayout* gridLayout,
                                   const bool multipleSelectionFlagIn);

      // destructor
      virtual ~GuiCaretCommandParameterFile();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected slots:
      // called when push button pressed
      void slotPushButtonPressed();
      
   protected:
      // line edit for file name 
      QLineEdit* fileNameLineEdit;
      
      /// the file filters
      QStringList fileFilters;
      
      /// allows multiple selections
      bool multipleSelectionFlag;
};

//=============================================================================
/// class for data directory parameter
class GuiCaretCommandParameterDirectory : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterDirectory(const ScriptBuilderParameters::Parameter* parameter,
                                        QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterDirectory();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected slots:
      // called when push button pressed
      void slotPushButtonPressed();
      
   protected:
      // line edit for directory name 
      QLineEdit* directoryNameLineEdit;      
};

//=============================================================================
/// class for data string parameter
class GuiCaretCommandParameterString : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterString(const ScriptBuilderParameters::Parameter* parameter,
                                     QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterString();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected:
      // line edit for text
      QLineEdit* lineEdit;
};

//=============================================================================
/// class for data variable parameter list
class GuiCaretCommandParameterVariableList : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterVariableList(const ScriptBuilderParameters::Parameter* parameter,
                                     QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterVariableList();

      // see if parameter can be blanks
      virtual bool getParameterAllowedToBeEmpty() const { return true; }
      
      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected slots:
      // called when add file button is selected
      void slotAddFileButton();
      
   protected:
      /// text edit for text
      QTextEdit* textEdit;
};

//=============================================================================
/// class for data item list
class GuiCaretCommandParameterDataItemList : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterDataItemList(const ScriptBuilderParameters::Parameter* parameter,
                                     QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterDataItemList();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected:
      /// combo box for items
      QComboBox* comboBox;
      
      /// values for combo box
      std::vector<QString> dataValues;
};

//=============================================================================
/// class for structure
class GuiCaretCommandParameterStructure : public GuiCaretCommandParameter {
   Q_OBJECT
   
   public:
      // constructor
      GuiCaretCommandParameterStructure(const ScriptBuilderParameters::Parameter* parameter,
                                     QGridLayout* gridLayout);

      // destructor
      virtual ~GuiCaretCommandParameterStructure();

      // get parameter value as text
      QString getParameterValueAsText() const;
   
      // set parameter value from text
      void setParameterValueFromText(const QString& s);
      
   protected:
      /// combo box for items
      QComboBox* comboBox;
      
      /// values for combo box
      std::vector<QString> comboBoxValues;
};
//=============================================================================
/// class for a single command in a widget
class GuiCaretCommandWidget : public QGroupBox {
   Q_OBJECT 
   
   public:
      // constructor
      GuiCaretCommandWidget(CommandBase* commandIn,
                            const bool showCommentOption);
      
      // destructor
      ~GuiCaretCommandWidget();
   
      // get the command switch and parameters for the GUI
      void getCommandLineForGUI(QString& commandSwitchOut,
                                QStringList& commandParametersOut,
                                QString& commandShortDescriptionOut,
                                bool& lastParameterIsVariableListFlag) const;
      
      // get the command switch and parameters for executing command
      void getCommandLineForCommandExecution(QString& commandSwitchOut,
                                             QStringList& commandParametersOut,
                                             QString& commandShortDescriptionOut,
                                             QString& errorMessageOut) const;
      
      /// get the comment
      QString getComment() const;
      
      /// get the command
      CommandBase* getCommand() { return command; }
      
      // set the comment
      void setComment(const QString& s);
      
      // set the parameters
      void setParameters(const QStringList& parametersIn);
      
   signals:
      // emitted when data is modified
      void signalDataModified();
         
   protected:
      /// the parameters
      std::vector<GuiCaretCommandParameter*> commandParameters;
      
      /// comment line edit
      QLineEdit* commentLineEdit;
      
      /// the command switch
      QString commandSwitch;
      
      /// the command
      CommandBase* command;
      
      /// the command short description
      QString commandShortDescription;
};
#endif // __GUI_CARET_COMMAND_WIDGET_H__
