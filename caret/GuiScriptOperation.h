
#ifndef __GUI_SCRIPT_OPERATION_H__
#define __GUI_SCRIPT_OPERATION_H__

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

#include <QString>
#include <QStringList>
#include <vector>

class GuiScriptArg;
class QGridLayout;
class QLineEdit;
class QWidget;

/// class for creating a script operation
class GuiScriptOperation {
   public:
      // constructor
      GuiScriptOperation(QWidget* parentWidget,
                         const QString& commandDescriptiveNameIn,
                         const QString& explanationTextIn,
                         const QString& programNameIn,
                         const QString& programArgsIn);
                        
      // destructor
      ~GuiScriptOperation();
      
      /// get the desciptive name
      QString getDescriptiveName() const { return commandDescriptiveName; }
      
      /// get the explanation text
      QString getExplanationText() const { return explanationText; }
      
      /// get operation comment
      QString getOperationComment() const;
      
      /// get my top level widget
      QWidget* getWidget() { return gridWidget; }
      
      /// get my top level layout
      QGridLayout* getGridLayout() { return gridLayout; }
      
      // get the command for execution as a string (returns true if valid)
      bool getCommand(QString& programNameOut,
                      QStringList& programArgsOut,
                      QString& errorMessageOut) const;
      
      // add an axis pparameter
      GuiScriptAxisArg* addAxisParameter(const QString& label);
      
      // add a boolean parameter
      GuiScriptBoolArg* addBoolParameter(const QString& label);
      
      // add an int parameter
      GuiScriptIntArg* addIntParameter(const QString& label);
      
      // add a float parameter
      GuiScriptFloatArg* addFloatParameter(const QString& label);
      
      // add a limits file name parameter
      GuiScriptFileNameArg* addLimitsFileParameter(const QString& label);
      
      // add a directory name parameter
      GuiScriptDirectoryArg* addDirectoryParameter(const QString& label);
      
      // add a generic file name parameter
      GuiScriptFileNameArg* addGenericFileParameter(const QString& label,
                                                    const QString& fileFilter,
                                                    const bool existingFilesOnlyFlag = false,
                                                    const bool fileIsOptionalFlag = false);
                                                    
      // add a spec file name parameter
      GuiScriptFileNameArg* addSpecFileParameter(const QString& label,
                                                 const bool fileIsOptional = false);
      
      // add an input volume file name parameter
      GuiScriptFileNameArg* addInputVolumeFileParameter(const QString& label = "Input Volume",
                                                        const bool optionalFileNameIn = false);
      
      // add an output volume file name parameter
      GuiScriptFileNameArg* addOutputVolumeFileParameter(const QString& label = "Output Volume");
      
      // add a vector file parameter
      GuiScriptFileNameArg* addVectorFileParameter(const QString& label);
      
      // add a seed parameter
      GuiScriptSeedArg* addSeedParameter(const QString& label);
      
      // add an extent parameter
      GuiScriptExtentArg* addExtentParameter(const QString& label);
      
      // add a combo box parameter
      GuiScriptComboBoxArg* addComboBoxParameter(const QString& label,
                                const std::vector<QString>& comboBoxLabels,
                                const std::vector<QString>& comboBoxValues,
                                const int defaultItem = 0);
      
      // add a string parameter
      GuiScriptStringArg* addStringParameter(const QString& label,
                                             const QString& defaultValue = "");
      
   protected:
      /// the top level widget
      QWidget* gridWidget;
      
      /// the top level layout
      QGridLayout* gridLayout;
      
      /// the comment line edit
      QLineEdit* commentLineEdit;
      
      /// descriptive name of command
      QString commandDescriptiveName;

      /// program that is to be run
      QString programName;
   
      /// program's initial arguments
      QString programArgs;
      
      /// explanation of command
      QString explanationText;
      
      /// operation's parameters
      std::vector<GuiScriptArg*> operationParameters;
};

#endif // __GUI_SCRIPT_OPERATION_H__

