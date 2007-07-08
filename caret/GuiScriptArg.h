
#ifndef __GUI_SCRIPT_ARG_H__
#define __GUI_SCRIPT_ARG_H__

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

#include <vector>

#include <QObject>
#include <QString>

class QComboBox;
class QGridLayout;
class QLineEdit;
class QString;

/// abstract class for script arguments
class GuiScriptArg : public QObject {
   public:
      // Constructor
      GuiScriptArg(QGridLayout* parentLayout,
                   const QString& labelString);
      
      // Destructor
      virtual ~GuiScriptArg();
      
      // get the value of the arg as text
      virtual QString getValue() const = 0;
      
      // see if a value of an arg is valid
      virtual bool getValueValid(QString& errorMessage) const;
      
      /// get the label
      QString getLabel() const { return argLabel; }
      
   protected:
      /// the label for the arg
      QString argLabel;
      
      /// row number for grid
      int rowNumber;
};

/// file name argument
class GuiScriptFileNameArg : public GuiScriptArg {
   Q_OBJECT
   
   public:
      /// type of file
      enum FILE_TYPE {
         /// generic file
         FILE_TYPE_GENERIC,
         /// limits file
         FILE_TYPE_LIMITS,
         /// spec file
         FILE_TYPE_SPEC,
         /// vector file
         FILE_TYPE_VECTOR,
         /// input volume file
         FILE_TYPE_VOLUME_INPUT,
         /// output volume file
         FILE_TYPE_VOLUME_OUTPUT
      };
      
      // constructor
      GuiScriptFileNameArg(QGridLayout* parentLayout,
                           const QString& labelString,
                           const FILE_TYPE ft,
                           const bool existingFilesOnlyFlagIn,
                           const bool fileNameIsOptionalIn = false);
      
      // constructor
      GuiScriptFileNameArg(QGridLayout* parentLayout,
                           const QString& labelString,
                           const QString& fileFilter,
                           const bool existingFilesOnlyFlagIn,
                           const bool fileNameIsOptionalIn);
      
      // destructor
      ~GuiScriptFileNameArg();
      
      // get the value of the arg as text
      virtual QString getValue() const;
      
      // see if a value of an arg is valid
      virtual bool getValueValid(QString& errorMessage) const;
      
      // get the argument's value
      void getValue(QString& fileName) const;
      
      // set the value
      void setValue(const QString& filenameIn, const QString& labelNameIn);
      
   protected slots:
      // called when file name push button is selected
      void slotFileNamePushButton();
      
   protected:
      /// type of file
      FILE_TYPE fileType;
      
      /// filename line edit
      QLineEdit* nameLineEdit;
      
      /// generic file filter
      QString genericFileFilter;
      
      /// only allow selection of existing files
      bool existingFilesOnlyFlag;
      
      /// label line edit
      QLineEdit* labelLineEdit;
      
      /// bool file is optional
      bool fileNameIsOptional;
};

/// directory argument
class GuiScriptDirectoryArg : public GuiScriptArg {
   Q_OBJECT
   
   public:
      // constructor
      GuiScriptDirectoryArg(QGridLayout* parentLayout,
                           const QString& labelString);
      
      // destructor
      ~GuiScriptDirectoryArg();
      
      // get the value of the arg as text
      virtual QString getValue() const;
      
      // see if a value of an arg is valid
      virtual bool getValueValid(QString& errorMessage) const;
      
      // get the argument's value
      void getValue(QString& directoryName) const;
      
      // set the value
      void setValue(const QString& directoryNameIn);
      
   protected slots:
      // called when directory name push button is selected
      void slotDirectoryNamePushButton();
      
   protected:
      /// filename line edit
      QLineEdit* nameLineEdit;
};

/// float argument
class GuiScriptFloatArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptFloatArg(QGridLayout* parentLayout,
                        const QString& labelString);
                        
      // destructor
      ~GuiScriptFloatArg();

      // get the value of the arg as text
      virtual QString getValue() const;

      // set the value of the arg
      void setValue(const float f);
      
   protected:      
      /// value line edit
      QLineEdit* valueLineEdit;
};

/// int argument
class GuiScriptIntArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptIntArg(QGridLayout* parentLayout,
                        const QString& labelString);
                        
      // destructor
      ~GuiScriptIntArg();

      // get the value of the arg as text
      virtual QString getValue() const;

      // set the value
      void setValue(const int v);
      
   protected:      
      /// value line edit
      QLineEdit* valueLineEdit;
};

/// bool argument
class GuiScriptBoolArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptBoolArg(QGridLayout* parentLayout,
                        const QString& labelString);
                        
      // destructor
      ~GuiScriptBoolArg();

      // get the value of the arg as text
      virtual QString getValue() const;
      
      // set the value
      void setValue(const bool b);

   protected:      
      /// value combo box
      QComboBox* valueComboBox;
};

/// string argument
class GuiScriptStringArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptStringArg(QGridLayout* parentLayout,
                        const QString& labelString,
                        const QString& defaultValue);
                        
      // destructor
      ~GuiScriptStringArg();

      // get the value of the arg as text
      virtual QString getValue() const;

      // set the value of the arg
      void setValue(const QString& s);
      
   protected:      
      /// value line edit
      QLineEdit* valueLineEdit;
};

/// seed (3 int) argument
class GuiScriptSeedArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptSeedArg(QGridLayout* parentLayout,
                        const QString& labelString);
                        
      // destructor
      ~GuiScriptSeedArg();

      // get the value of the arg as text
      virtual QString getValue() const;

      // set the seed
      void setValue(const int x, const int y, const int z);
      
   protected:      
      /// value line edit X
      QLineEdit* valueLineEditX;

      /// value line edit Y
      QLineEdit* valueLineEditY;

      /// value line edit Z
      QLineEdit* valueLineEditZ;      
};

/// extent (6 int) argument
class GuiScriptExtentArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptExtentArg(QGridLayout* parentLayout,
                        const QString& labelString);
                        
      // destructor
      ~GuiScriptExtentArg();

      // get the value of the arg as text
      virtual QString getValue() const;

      // set the value
      void setValue(const int x1, const int x2,
                    const int y1, const int y2,
                    const int z1, const int z2);
                    
   protected:      
      /// value line edit min X
      QLineEdit* valueLineEditMinX;

      /// value line edit max X
      QLineEdit* valueLineEditMaxX;

      /// value line edit min Y
      QLineEdit* valueLineEditMinY;

      /// value line edit max Y
      QLineEdit* valueLineEditMaxY;

      /// value line edit min Z
      QLineEdit* valueLineEditMinZ;      

      /// value line edit max Z
      QLineEdit* valueLineEditMaxZ;      
};

/// combo box argument
class GuiScriptComboBoxArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptComboBoxArg(QGridLayout* parentLayout,
                        const QString& labelString,
                        const std::vector<QString>& comboBoxLabels,
                        const std::vector<QString>& comboBoxValuesIn,
                        const int defaultItem);
                        
      // destructor
      ~GuiScriptComboBoxArg();

      // get the value of the arg as text
      virtual QString getValue() const;

      // set the value
      void setValue(const int indx);
      
   protected:      
      /// value line edit
      QComboBox* valueComboBox;
      
      /// values associated with combo box labels
      std::vector<QString> comboBoxValues;
};

/// axis argument
class GuiScriptAxisArg : public GuiScriptArg {
   public:
      // constructor
      GuiScriptAxisArg(QGridLayout* parentLayout,
                        const QString& labelString);
                        
      // destructor
      ~GuiScriptAxisArg();

      // get the value of the arg as text
      virtual QString getValue() const;

      // set the value
      void setValue(const int indx);
      
   protected:      
      /// value line edit
      QComboBox* valueComboBox;
};



#endif // __GUI_SCRIPT_ARG_H__

