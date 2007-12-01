
#ifndef __COMMAND_METRIC_MATH_POSTFIX_H__
#define __COMMAND_METRIC_MATH_POSTFIX_H__

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

#include <queue>

#include "CommandBase.h"

/// class for metric postfix mathmatics
class CommandMetricMathPostfix : public CommandBase {
   public:
      // constructor 
      CommandMetricMathPostfix(const QString& operationSwitchIn  = "-metric-math-postfix",
                               const QString& shortDescriptionIn = "METRIC MATH POSTFIX");
      
      // destructor
      ~CommandMetricMathPostfix();
      
      // get full help information
      QString getHelpInformation() const;
      
      // get the script builder parameters
      virtual void getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const;
      
   protected:
      // execute the command
      void executeCommand() throw (BrainModelAlgorithmException,
                                   CommandException,
                                   FileException,
                                   ProgramParametersException,
                                   StatisticException);

      // convert the input string into a queue of tokens
      void convertInputToQueueOfTokens(const QString& inputString,
                                       std::queue<QString>& tokensOut) const throw (CommandException);
                                       
      // process the postfix expression
      void processPostFixExpression(const QString& inputMetricFileName,
                                    const QString& outputMetricFileName,
                                    const QString& outputMetricColumnNameOrNumber,
                                    std::queue<QString>& postFixExpression) throw (CommandException);

      // get an array with number of nodes elements
      float* getArray();
      
      // release an array
      void releaseArray(float* f);
      
      // see if whitespace
      bool isWhiteSpace(const QString& s) const;
      
      /// storage for data arrays to avoid extra memory allocations/frees
      std::queue<float*> arrayStorage;
      
      /// number of nodes in metric file
      int numberOfNodes;
      
      /// name of metric column identifier
      QString metricColumnIdentifierName;
      
      /// character that identifies a metric column
      QString metricColumnIdentifierCharacter;
      
      /// name of metric file/column separator
      QString metricFileColumnSeparatorName;
      
      /// characters that separate metric file/column
      QString metricFileColumnSeparatorCharacter;
      
      // the whitespace characters
      QString whitespace;
};

#endif // __COMMAND_METRIC_MATH_POSTFIX_H__

