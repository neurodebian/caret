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
#include <stack>

#include "CommandMetricMath.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricMath::CommandMetricMath()
   : CommandMetricMathPostfix("-metric-math",
                              "METRIC MATH")
{
   delimiters = "(),[]";
   operators  = "+-*/^";
}

/**
 * destructor.
 */
CommandMetricMath::~CommandMetricMath()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricMath::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addString("Output Metric Column Name/Number");
   paramsOut.addVariableListOfParameters("Expression");
}

/**
 * get full help information.
 */
QString 
CommandMetricMath::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<mathematical-expression-within-double-qutoes>\n"
       + indent9 + "\n"
       + indent9 + "Perform mathematical operations on a metric file.\n"
       + indent9 + "\n"
       + indent9 + "The math expression must be in double quotes.  Otherwise,\n"
       + indent9 + "operators, such as \"*\" will match all files in the \n"
       + indent9 + "current directory.\n"
       + indent9 + "\n"
       + indent9 + "A metric column is identified by a sequence of characters\n"
       + indent9 + "the are immediately proceeded and immediately followed by\n"
       + indent9 + "an " + metricColumnIdentifierName + " (" + metricColumnIdentifierCharacter + ").   \n"
       + indent9 + "\n"
       + indent9 + "These characters are either the name of the metric column or\n"
       + indent9 + "the number of the metric column which starts at one.  To use\n"
       + indent9 + "a metric column that is in a metric file other than the input\n"
       + indent9 + "metric file, start with an " + metricColumnIdentifierName + ", followed by the name of\n"
       + indent9 + "the metric file, followed by a " + metricFileColumnSeparatorName + " (" + metricFileColumnSeparatorCharacter + "), \n"
       + indent9 + "followed by the identifier of the metric column, and lastly,\n"
       + indent9 + "an " + metricColumnIdentifierName + ".\n"
       + indent9 + "\n"
       + indent9 + "Spaces are allowed in file names or column identifiers:\n"
       + indent9 + "   Example \"@test file::subject 2@\"\n"
       + indent9 + "\n"
       + indent9 + "Examples (for the column named \"activation\" located in:\n"
       + indent9 + "the file named \"experiment.metric\"\n"
       + indent9 + "   " + metricColumnIdentifierCharacter + "activation" + metricColumnIdentifierCharacter + "\n"
       + indent9 + "   " + metricColumnIdentifierCharacter + "experiment.metric::activation" + metricColumnIdentifierCharacter + "\n"
       + indent9 + "\n"
       + indent9 + "If the output metric column is a name and it does not exist\n"
       + indent9 + "it will be created.\n"
       + indent9 + "\n"
       + indent9 + "Operators supported are:\n"
       + indent9 + "   +     addition\n"
       + indent9 + "   -     subtraction\n"
       + indent9 + "   *     multiplication\n"
       + indent9 + "   /     division\n"
       + indent9 + "   ^     exponention\n"
       + indent9 + "\n"
       + indent9 + "FUNCTIONS\n"
       + indent9 + "   A function's parameters must be contained in square\n"
       + indent9 + "   brackets (example max2[3, 4]).\n"
       + indent9 + "\n"
       + indent9 + "   Functions accepting two parameters:\n"
       + indent9 + "      max2   maximum-value\n"
       + indent9 + "      min2   minimum-value\n"
       + indent9 + "\n"
       + indent9 + "   Functions accepting a single parameter:\n"
       + indent9 + "      abs    absolute-value\n"
       + indent9 + "      exp    exponential function\n"
       + indent9 + "      flipsign  flip the sign\n"
       + indent9 + "      log    natural log\n"
       + indent9 + "      log2   base 2 logarithm\n"
       + indent9 + "      log10  base 10 logarithm\n"
       + indent9 + "      sqrt   square root\n"
       + indent9 + "\n"
       + indent9 + "Predefined values from each nodes metric values\n"
       + indent9 + "   nodeavg  Average value at each node\n"
       + indent9 + "   nodemax  Maximum value at each node  \n"
       + indent9 + "   nodemin  Minimum value at each node  \n"
       + indent9 + "   nodesum  Sum of values at each node  \n"
       + indent9 + "\n"
       + indent9 + "Example mathematical expressions"
       + indent9 + "   3 + max2[7, 2] \n"
       + indent9 + "      evaluates to 10.\n"
       + indent9 + "\n"
       + indent9 + "   abs[2 * (3 - 5)] \n"
       + indent9 + "      evaluates to 4.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricMath::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the input and output file names
   //
   const QString inputMetricFileName = 
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName = 
      parameters->getNextParameterAsString("Output Metric File Name");
   const QString outputColumnNameNumber = 
      parameters->getNextParameterAsString("Output Metric Column Name/Number");
   const QString expressionIn = parameters->getNextParameterAsString("Infix Expression");
   checkForExcessiveParameters();
   
   //
   // Parse the input
   //
   std::queue<QString> tokens;
   parseInputText(expressionIn, tokens);

//   if (DebugControl::getDebugOn()) {
      std::cout << "METRIC MATH TOKENS" << std::endl;
      std::queue<QString> q = tokens;
      while (q.empty() == false) {
         std::cout << "   "
                   << q.front().toAscii().constData()
                   << std::endl;
         q.pop();
      }
//   }
   
   if (tokens.empty()) {
      throw CommandException("No mathematical expression provided.");
   }
   
   std::queue<QString> postFixTokens;
   infixToPostfix(tokens, postFixTokens);
   
//   if (DebugControl::getDebugOn()) {
      std::cout << "POSTFIX: " << std::endl;
      std::queue<QString> qp = postFixTokens;
      while (qp.empty() == false) {
         std::cout << "   "
                   << qp.front().toAscii().constData();
         qp.pop();
      }
      std::cout << std::endl << std::endl;

//   }

   //throw CommandException("METRIC MATH IS IN DEVELOPMENT");
   
   //
   // Process the postfix expression
   //
   processPostFixExpression(inputMetricFileName,
                            outputMetricFileName,
                            outputColumnNameNumber,
                            postFixTokens);
}

// parse the input text
void 
CommandMetricMath::parseInputText(const QString& inputText,
                                  std::queue<QString>& tokensOut)
{                          
   //
   // Break the expression string into tokens
   //
   const int textLength = inputText.length();
   QString currentToken;
   for (int i = 0; i < textLength; i++) {
      //
      // Next character
      //
      const QString c(inputText[i]);
      
      //
      // Is this a delimiter or operator
      //
      if (isDelimiter(c) ||
          isOperator(c)) {
         //
         // Save current token
         //
         pushTokenOntoQueueAndClearToken(tokensOut, currentToken);
         
         //
         // put delimeter in queue
         //
         tokensOut.push(c);
      }
      else if (isWhiteSpace(c)) {  // white space
         //
         // Save current token
         //
         pushTokenOntoQueueAndClearToken(tokensOut, currentToken);         
      }
      else if (c == metricColumnIdentifierCharacter) {  // start of metric column
         //
         // Save current token
         //
         pushTokenOntoQueueAndClearToken(tokensOut, currentToken);
         
         //
         // Find closing metric column identifier to create column name
         //
         bool validName = false;
         QString columnIdentifier(c);
         i++;
         while (i < textLength) {
            const QString& c(inputText[i]);
            columnIdentifier += c;
            if (c == metricColumnIdentifierCharacter) {
               validName = true;
               break;  
            }
            else {
               i++;
            }
         }
         
         if (validName) {
            tokensOut.push(columnIdentifier);
         }
         else {
            throw CommandException("Metric column identifier \""
                                   + columnIdentifier
                                   + "\" missing concluding \""
                                   + metricColumnIdentifierCharacter
                                   + "\"");
         }
      }
      else {
         //
         // Add to current token
         //
         currentToken += c;
      }
   }

   //
   // Save last token
   //
   pushTokenOntoQueueAndClearToken(tokensOut, currentToken);   
}

/**
 * convert the infix tokens to postfix.
 * This algorithm is from:
 *   http://montcs.bloomu.edu/~bobmon/Information/RPN/infix2rpn.shtml
 */
void 
CommandMetricMath::infixToPostfix(std::queue<QString>& infixTokens,
                                  std::queue<QString>& postfixTokens)
{
   //
   // Stack for operators
   //
   std::stack<QString> operatorStack;
   
   //
   // Stack for functions
   //
   std::stack<QString> functionStack;
   
   while (infixTokens.empty() == false) {
      //
      // Get and remove next token in input
      //
      const QString token = infixTokens.front();
      infixTokens.pop();
      const char tokenFirstChar = token[0].toLatin1();
      
      //
      // peek ahead to next token
      //
      QString nextToken;
      if (infixTokens.empty() == false) {
         nextToken = infixTokens.front();
      }
      
      const bool isOpenParenFlag    = (token == "(");
      const bool isClosingParenFlag = (token == ")");
      const bool isOpenBracketFlag    = (token == "[");
      const bool isClosingBracketFlag = (token == "]");
      const bool isCommaFlag      = (token == ",");
      const bool isOperatorFlag   = isOperator(token);
      const bool isOperandFlag    = (((tokenFirstChar >= 'a') &&
                                      (tokenFirstChar <= 'z')) ||
                                     ((tokenFirstChar >= 'A') &&
                                      (tokenFirstChar <= 'Z')) ||
                                     ((tokenFirstChar >= '0') &&
                                      (tokenFirstChar <= '9')) ||
                                     (tokenFirstChar == '_') ||
                                     (QString(tokenFirstChar) == metricColumnIdentifierCharacter));
      //
      // An operand
      //
      if (isOperandFlag) {
         //
         // If next token is an open bracket then this token is a function name
         //
         if (nextToken == "[") {
            functionStack.push(token);
         }
         else {
            postfixTokens.push(token);
         }
      }
      else if (isOperatorFlag) {  // operator ?
         //
         // If any operators in the operator stack are of equal or greater
         // precedence, place them into the output followed by this operator
         //
         bool done = false;
         while (done == false) {
            if (operatorStack.empty()) {
               done = true;
            }
            else {
               const QString stackOperator = operatorStack.top();
               if (getOperatorPrecedence(token) 
                   <= getOperatorPrecedence(stackOperator)) {
                  postfixTokens.push(stackOperator);
                  operatorStack.pop();
               }
               else {
                  done = true;
               }
            }
         }
         operatorStack.push(token);
      }
      else if (isOpenParenFlag) {
         //
         // Put open paren into operator stack
         //
         operatorStack.push(token);
      }
      else if (isClosingParenFlag) {
         //
         // while the top of the operator stack is not an open paren
         //
         bool done = false;
         while (done == false) {
            if (operatorStack.empty()) {
               throw CommandException("Unbalanced parenthesis in expression.");
            }
            const QString stackToken = operatorStack.top();
            
            //
            // Is an open paren on top of the stack ?
            //
            if (stackToken == "(") {
               //
               // ignore current token closed paren and done
               //
               done = true;
            }
            else {
               postfixTokens.push(stackToken);
            }
            operatorStack.pop();
         }
      }
      else if (isOpenBracketFlag) {
         //
         // Put open bracket into operator stack
         //
         operatorStack.push(token);
      }
      else if (isClosingBracketFlag) {
         //
         // while the top of the operator stack is not an open bracket
         //
         bool done = false;
         while (done == false) {
            if (operatorStack.empty()) {
               throw CommandException("Unbalanced brackets in expression.");
            }
            const QString stackToken = operatorStack.top();
            
            //
            // Is an open bracket on top of the stack ?
            //
            if (stackToken == "[") {
               //
               // ignore current token closed bracket and done
               //
               done = true;
            }
            else {
               postfixTokens.push(stackToken);
            }
            operatorStack.pop();
         }   
         
         if (functionStack.empty()) {
            throw CommandException("Function name missing before brackets.");
         }
         postfixTokens.push(functionStack.top());
         functionStack.pop();
      }
      else if (isCommaFlag) {
         //
         // while the top of the operator stack is not an open bracket
         //
         bool done = false;
         while (done == false) {
            if (operatorStack.empty()) {
               throw CommandException("Unbalanced brackets in expression.");
            }
            const QString stackToken = operatorStack.top();
            
            //
            // Is an open bracket on top of the stack ?
            //
            if (stackToken == "[") {
               //
               // ignore current token closed bracket and done
               // leave bracket on stack
               //
               done = true;
            }
            else {
               postfixTokens.push(stackToken);
               operatorStack.pop();
            }
         }   
      }
      else {
         throw CommandException("Invalid infix expression.");
      }
   }
   
   //
   // Output any remaining tokens from the function stack
   //
   while (functionStack.empty() == false) {
      const QString stackToken = functionStack.top();
      postfixTokens.push(stackToken);
      functionStack.pop();
   }
   
   //
   // Output any remaining tokens from the operator stack
   //
   while (operatorStack.empty() == false) {
      const QString stackToken = operatorStack.top();
      if (stackToken == "(") {
         throw CommandException("Unbalanced parenthesis in expression.");
      }
      postfixTokens.push(stackToken);
      operatorStack.pop();
   }
}
                          
/**
 * get the prcedence of an operator.
 */
int 
CommandMetricMath::getOperatorPrecedence(const QString& operatorToken) const
{
   int precedence = -10000;
   
   if (operatorToken == "^") {
      precedence = 9;
   }
   else if (operatorToken == "*") {
      precedence = 8;
   }
   else if (operatorToken == "/") {
      precedence = 8;
   }
   else if (operatorToken == "%") {
      precedence = 8;
   }
   else if (operatorToken == "+") {
      precedence = 6;
   }
   else if (operatorToken == "-") {
      precedence = 6;
   }
   else if (operatorToken == "(") {
      precedence = -1;
   }
   else if (operatorToken == ")") {
      precedence = -10;
   }
   else if (operatorToken == "[") {
      precedence = -1;
   }
   else if (operatorToken == "]") {
      precedence = -10;
   }
   else  {
      throw CommandException("Operator \"" 
                                + QString(operatorToken)
                                + "\" does not have a precedence.");
   }
   
   return precedence;
}
      
/**
 * push the current token into the queue.
 */
void 
CommandMetricMath::pushTokenOntoQueueAndClearToken(std::queue<QString>& queue,
                                                   QString& token)
{
   token = token.trimmed();
   if (token.isEmpty() == false) {
      queue.push(token);
   }
   token = "";
}
                              
/**
 * see if operator
 */
bool 
CommandMetricMath::isOperator(const QString& s) const
{
   if (operators.indexOf(s) >= 0) {
      return true;
   }
   return false;
}
      
/**
 * see if a character is a delimiter.
 */
bool 
CommandMetricMath::isDelimiter(const QString& s) const
{
   if (delimiters.indexOf(s) >= 0) {
      return true;
   }
   return false;
}      

