
#ifndef __COMMAND_METRIC_MATH_H__
#define __COMMAND_METRIC_MATH_H__

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

#include "CommandMetricMathPostfix.h"

/// class for
class CommandMetricMath : public CommandMetricMathPostfix {
   public:
      // constructor 
      CommandMetricMath();
      
      // destructor
      ~CommandMetricMath();
      
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

      // see if a character is a delimiter
      bool isDelimiter(const QString& s) const;
      
      // see if operator
      bool isOperator(const QString& s) const;
      
      // get the prcedence of an operator
      int getOperatorPrecedence(const QString& operatorToken) const;
      
      // push the current token into the queue
      void pushTokenOntoQueueAndClearToken(std::queue<QString>& queue,
                                           QString& token);
                              
      // parse the input text
      void parseInputText(const QString& inputText,
                          std::queue<QString>& tokensOut);
                          
      // convert the infix tokens to postfix
      void infixToPostfix(std::queue<QString>& infixTokens,
                          std::queue<QString>& postfixTokens);
      
      // the operators
      QString operators;
      
      // the delimeters in a string
      QString delimiters;
};

#endif // __COMMAND_METRIC_MATH_H__

