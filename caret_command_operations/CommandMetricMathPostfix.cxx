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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>

#include <QStringList>

#include "CommandMetricMathPostfix.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricMathPostfix::CommandMetricMathPostfix(const QString& operationSwitchIn,
                                                   const QString& shortDescriptionIn)
   : CommandBase(operationSwitchIn,
                 shortDescriptionIn)
{
   whitespace = " \t\n\r";
   metricColumnIdentifierName = "at-sign";
   metricColumnIdentifierCharacter = "@";
   metricFileColumnSeparatorName = "colon";
   metricFileColumnSeparatorCharacter = ":";
}

/**
 * destructor.
 */
CommandMetricMathPostfix::~CommandMetricMathPostfix()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricMathPostfix::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
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
CommandMetricMathPostfix::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "<output-column-name-number>\n"
       + indent9 + "<postfix-expression-within-double-quotes>\n"
       + indent9 + "\n"
       + indent9 + "Perform mathematical operations on a metric file.  The \n"
       + indent9 + "mathematical expression must be in post-fix (reverse \n"
       + indent9 + "polish notation).  See \n"
       + indent9 + "   http://en.wikipedia.org/wiki/Reverse_Polish_notation\n"
       + indent9 + "\n"
       + indent9 + "The postfix expression must be in double quotes.  Otherwise,\n"
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
       + indent9 + "Binary operators supported are:\n"
       + indent9 + "   +     addition\n"
       + indent9 + "   -     subtraction\n"
       + indent9 + "   *     multiplication\n"
       + indent9 + "   /     division\n"
       + indent9 + "   ^     exponention\n"
       + indent9 + "   max2   maximum-value\n"
       + indent9 + "   min2   minimum-value\n"
       + indent9 + "\n"
       + indent9 + "Unary operations supported are:\n"
       + indent9 + "   abs    absolute-value\n"
       + indent9 + "   exp    exponential function\n"
       + indent9 + "   flipsign  flip the sign\n"
       + indent9 + "   log    natural log\n"
       + indent9 + "   log2   base 2 logarithm\n"
       + indent9 + "   log10  base 10 logarithm\n"
       + indent9 + "   sqrt   square root\n"
       + indent9 + "\n"
       + indent9 + "Predefined values from each nodes metric values\n"
       + indent9 + "   nodeavg  Average value at each node\n"
       + indent9 + "   nodemax  Maximum value at each node  \n"
       + indent9 + "   nodemin  Minimum value at each node  \n"
       + indent9 + "   nodesum  Sum of values at each node  \n"
       + indent9 + "\n"
       + indent9 + "Example:   \"5 1 2 + 4 * + 3 -\" \n"
       + indent9 + "          Infix => ((1 + 2) * 4 + 5 - 3)\n"
       + indent9 + "   evaluates to 14.\n"
       + indent9 + "Example:   \"2 3 * 2 3 + min2\" \n"
       + indent9 + "          Infix => min2(2 * 3, 2 + 3)\n"
       + indent9 + "   evaluates to 5.\n"
       + indent9 + "Example:   \"" + metricColumnIdentifierCharacter + "one" + metricColumnIdentifierCharacter 
       + indent9 + "             " + metricColumnIdentifierCharacter + "two" + metricColumnIdentifierCharacter + " add\"  \n"
       + indent9 + "   adds the columns named \"one\" and \"two\"."
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricMathPostfix::executeCommand() throw (BrainModelAlgorithmException,
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
   const QString inputExpression =   
      parameters->getNextParameterAsString("Postfix Expression");
   checkForExcessiveParameters();
   
   //
   // Get the mathematical expression
   //
   std::queue<QString> expression;
   convertInputToQueueOfTokens(inputExpression,
                               expression);

   if (expression.empty()) {
      throw CommandException("No mathematical expression provided.");
   }
    
   //
   // Process the postfix expression
   //
   processPostFixExpression(inputMetricFileName,
                            outputMetricFileName,
                            outputColumnNameNumber,
                            expression);
}

/**
 * convert the input string into a queue of tokens.
 */
void 
CommandMetricMathPostfix::convertInputToQueueOfTokens(const QString& inputString,
                                          std::queue<QString>& tokensOut) const throw (CommandException)
{
   //
   // The input string should have each operand/operator separated by whitespace.
   // Metric column identifiers may contain spaces so they need to be handled specially.
   //
   
   const int len = inputString.length();
   QString token;
   for (int i = 0; i < len; i++) {
      const QString c(inputString[i]);
      
      if (isWhiteSpace(c)) {
         if (token.isEmpty() == false) {
            tokensOut.push(token);
            token.clear();
         }
      }
      else if (c == metricColumnIdentifierCharacter) {
         token += c;
         
         bool done = false;
         while (done == false) {
            i++;
            if (i >= len) {
               throw CommandException("Metric column identifier is missing closing \""
                                      + metricColumnIdentifierCharacter 
                                      + "\" \""
                                      + token
                                      + "\".");
            }
            const QString c(inputString[i]);
            token += c;
            if (c == metricColumnIdentifierCharacter) {
               tokensOut.push(token);
               token.clear();
               done = true;
            }
         }
      }
      else {
         token += c;
      }
   }
   
   if (token.isEmpty() == false) {
      tokensOut.push(token);
      token.clear();
   }
}
                                       
/**
 * process the postfix expression.
 * Algorithm from http://en.wikipedia.org/wiki/Reverse_Polish_notation
 */
void 
CommandMetricMathPostfix::processPostFixExpression(const QString& inputMetricFileName,
                                                   const QString& outputMetricFileName,
                                                   const QString& outputMetricColumnNameOrNumber,
                                                   std::queue<QString>& postFixExpression) throw (CommandException)
{  
   //
   // Read the metric file
   //
   MetricFile metricFile;
   metricFile.readFile(inputMetricFileName);
   
   //
   // Check the number of nodes
   // 
   numberOfNodes = metricFile.getNumberOfNodes();
   if (numberOfNodes <= 0) {
      throw CommandException("Input metric file contains no nodes.");
   }
   const int numberOfInputMetricColumns = metricFile.getNumberOfColumns();
      
   //
   // Find the output column (create if necessary)
   //
   const int outputColumnNumber = metricFile.getColumnFromNameOrNumber(outputMetricColumnNameOrNumber,
                                                                       true);
   
   //
   // Stack used for processing the postfix expression
   //
   std::stack<float*> valueStack;
   
   //
   // Loop through the queue until it is empty
   //
   while (postFixExpression.empty() == false) {
      const QString token = postFixExpression.front(); 
      const int tokenLength = token.length();
         
      postFixExpression.pop();
        
      std::cout << "Processing: \"" << token.toAscii().constData() << "\"" << std::endl;
      
      const QChar firstChar = token[0];
      
      //
      // Examing the first character
      //
      if (QString(firstChar) == metricColumnIdentifierCharacter) {  // column name/number
         //
         // Remove metric column identifier at beginning and end of name
         //
         if (token.endsWith(metricColumnIdentifierCharacter) == false) {
            throw CommandException("Invalid metric column identifier (missing closing "
                                   + metricColumnIdentifierCharacter
                                   + ")");
         } 
         QString columnID = token.mid(1, tokenLength - 2);
         
         //
         // Is this a file name and column ID
         //
         QString columnFileName;
         const int fileColumnSeparatorIndex = columnID.indexOf(metricFileColumnSeparatorCharacter);
         if (fileColumnSeparatorIndex >= 0) {
            columnFileName = columnID.left(fileColumnSeparatorIndex);
            columnID = columnID.mid(fileColumnSeparatorIndex + 1);
            if (columnFileName.isEmpty()) {
               throw CommandException("Invalid metric column ID filename \""
                                      + token
                                      + "\"");
            }
         }
         if (columnID.isEmpty()) {
            throw CommandException("Invalid metric column ID \""
                                   + token
                                   + "\"");
         }
         std::cout << "column ID \""
                   << columnID.toAscii().constData()
                   << "\""
                   << std::endl;
         
         //
         // Is data in a metric file that is not the input metric file
         //
         if (columnFileName.isEmpty() == false) {
            MetricFile columnMetricFile;
            columnMetricFile.readFile(columnFileName);
            if (columnMetricFile.getNumberOfNodes() == numberOfNodes) {
               //
               // Push column value onto stack
               //
               try {
                  float* array = getArray();
                  const int columnNumber = columnMetricFile.getColumnFromNameOrNumber(columnID, false);
                  columnMetricFile.getColumnForAllNodes(columnNumber, array);
                  valueStack.push(array);
               }
               catch (FileException& e) {
                  throw CommandException(e);
               }
            }
            else {
               throw CommandException(FileUtilities::basename(columnFileName)
                                      + " has a different number of nodes than "
                                      + inputMetricFileName);
            }
         }
         else {
            //
            // Push column value onto stack
            //
            try {
               float* array = getArray();
               const int columnNumber = metricFile.getColumnFromNameOrNumber(columnID, false);
               metricFile.getColumnForAllNodes(columnNumber, array);
               valueStack.push(array);
            }
            catch (FileException& e) {
               throw CommandException(e);
            }
         }
      }
      else if (firstChar.isDigit()) {     // a number
         //
         // Convert to a float
         //
         bool ok = false;
         const float f = token.toFloat(&ok);
         if (ok == false) {
            throw CommandException("Invalid number " + token);
         }
         
         //
         // Put the float in an array with size number of nodes
         //
         float* array = getArray();
         for (int i = 0; i < numberOfNodes; i++) {
            array[i] = f;
         }
         
         //
         // Push onto the values stack
         //
         valueStack.push(array);
      }
      else if ((token == "abs")   ||   // unary operators
               (token == "exp")   ||
               (token == "flipsign") ||
               (token == "log")   ||
               (token == "log2")  ||
               (token == "log10") ||
               (token == "sqrt")) {
         if (valueStack.size() < 1) {
            throw CommandException("Invalid expression (insufficient operands) at " + token);
         }
         
         //
         // Get the one column of data
         //
         float* f1 = valueStack.top();
         valueStack.pop();
         
         //
         // Evaluate the expression
         //
         float* value = getArray();
         if (token == "abs") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::fabs(f1[i]);
            }
         }
         else if (token == "exp") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::exp(f1[i]);
            }
         }
         else if (token == "flipsign") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = -f1[i];
            }
         }
         else if (token == "log") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::log(f1[i]);
            }
         }
         else if (token == "log2") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = MathUtilities::log(2.0, f1[i]);
            }
         }
         else if (token == "log10") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::log10(f1[i]);
            }
         }
         else if (token == "sqrt") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::sqrt(f1[i]);
            }
         }
         else {
            throw CommandException("PROGRAM ERROR unrecognized operator at line "
                                   + QString(__LINE__)
                                   + " in "
                                   + QString(__FILE__));
         }
         
         //
         // put result back onto stack
         //
         valueStack.push(value);
         
         //
         // Release arrays
         //
         releaseArray(f1);
      }
      else if ((token == "+") ||  // binary operators
               (token == "-") ||
               (token == "*") ||
               (token == "/") ||
               (token == "^") ||
               (token == "max2") ||
               (token == "min2")) {   
         if (valueStack.size() < 2) {
            throw CommandException("Invalid expression (insufficient operands) at " + token);
         }
         
         //
         // Get the two column of data
         //
         float* f1 = valueStack.top();
         valueStack.pop();
         float* f2 = valueStack.top();
         valueStack.pop();
         
         //
         // Evaluate the expression
         //
         float* value = getArray();
         if (token == "+") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = f1[i] + f2[i];
            }
         }
         else if (token == "-") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = f2[i] - f1[i];
            }
         }
         else if (token == "*") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = f1[i] * f2[i];
            }
         }
         else if (token == "/") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = f2[i] / f1[i];
            }
         }
         else if (token == "^") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::pow(f2[i], f1[i]);
            }
         }
         else if (token == "max2") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::max(f1[i], f2[i]);
            }
         }
         else if (token == "min2") {
            for (int i = 0; i < numberOfNodes; i++) {
               value[i] = std::min(f1[i], f2[i]);
            }
         }
         else {
            throw CommandException("PROGRAM ERROR unrecognized operator at line "
                                   + QString(__LINE__)
                                   + " in "
                                   + QString(__FILE__));
         }
         
         //
         // put result back onto stack
         //
         valueStack.push(value);
         
         //
         // Release arrays
         //
         releaseArray(f1);
         releaseArray(f2);
      }
      else if ((token == "nodeavg") ||
               (token == "nodemax") ||
               (token == "nodemin") ||
               (token == "nodesum")) {
         //
         // Evaluate the expression
         //
         float* value = getArray();
         if (token == "nodeavg") {
            for (int i = 0; i < numberOfNodes; i++) {
               float sum = 0.0;
               for (int j = 0; j < numberOfInputMetricColumns; j++) {
                  sum += metricFile.getValue(i, j);
               }
               value[i] = sum / static_cast<float>(numberOfInputMetricColumns);
            }
         }
         else if (token == "nodemax") {
            for (int i = 0; i < numberOfNodes; i++) {
               float nodeMax = -std::numeric_limits<float>::max();
               for (int j = 0; j < numberOfInputMetricColumns; j++) {
                  nodeMax = std::max(nodeMax, metricFile.getValue(i, j));
               }
               value[i] = nodeMax;
            }
         }
         else if (token == "nodemin") {
            for (int i = 0; i < numberOfNodes; i++) {
               float nodeMin = std::numeric_limits<float>::max();
               for (int j = 0; j < numberOfInputMetricColumns; j++) {
                  nodeMin = std::min(nodeMin, metricFile.getValue(i, j));
               }
               value[i] = nodeMin;
            }
         }
         else if (token == "nodesum") {
            for (int i = 0; i < numberOfNodes; i++) {
               float sum = 0.0;
               for (int j = 0; j < numberOfInputMetricColumns; j++) {
                  sum += metricFile.getValue(i, j);
               }
               value[i] = sum;
            }
         }
         else {
            throw CommandException("PROGRAM ERROR unrecognized operator at line "
                                   + QString(__LINE__)
                                   + " in "
                                   + QString(__FILE__));
         }
         
         //
         // put result back onto stack
         //
         valueStack.push(value);
      }
      else {
         throw CommandException("Invalid expression at " + token);
      }
   }
   
   if (valueStack.size() != 1) {
      throw CommandException("Invalid expression");
   }
   
   //
   // Get the result array
   //
   float* resultArray = valueStack.top();
   valueStack.pop();
   
   metricFile.setColumnForAllNodes(outputColumnNumber,
                                   resultArray);
                                   
   //
   // Free memory
   //
   releaseArray(resultArray);
   //std::cout << "Result: " << valueStack.top() << std::endl;
   
   //
   // Write the output metric file
   //
   metricFile.writeFile(outputMetricFileName);
}

/**
 * get an array with number of nodes elements.
 */
float* 
CommandMetricMathPostfix::getArray()
{
   //
   // Is a previously used array available
   //
   if (arrayStorage.empty()) {
      float* f = new float[numberOfNodes];
      return f;
   }
   
   //
   // Create a new array
   //
   float* f = arrayStorage.front();
   arrayStorage.pop();
   return f;
}

/**
 * release an array.
 */
void 
CommandMetricMathPostfix::releaseArray(float* f)
{
   arrayStorage.push(f);
}

/**
 * see if a character is a whitespace.
 */
bool 
CommandMetricMathPostfix::isWhiteSpace(const QString& s) const
{
   if (whitespace.indexOf(s) >= 0) {
      return true;
   }
   return false;
}      
