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

#include <QStringList>

#include "FileUtilities.h"
#include "ProgramParameters.h"

/**
 * constructor.
 */
ProgramParameters::ProgramParameters(const int argc, char* argv[])
{
   parameters.clear();
   for (int i = 0; i < argc; i++) {
      parameters.push_back(argv[i]);
   }
   resetParametersIndex();
}

/**
 * constructor.
 */
ProgramParameters::ProgramParameters(const QString& programName,
                                     const QStringList& programParameters)
{
   parameters.clear();
   parameters.push_back(programName);
   for (int i = 0; i < programParameters.count(); i++) {
      parameters.push_back(programParameters.at(i));
   }
   resetParametersIndex();
}
      
/**
 * destructor.
 */
ProgramParameters::~ProgramParameters()
{
}

/**
 * get the number of parameters (value will change if splitUpRemainingParameters() called)
 */
int 
ProgramParameters::getNumberOfParameters() const throw (ProgramParametersException)
{
   return parameters.size();
}

/**
 * reset the parameters index.
 */
void 
ProgramParameters::resetParametersIndex() throw (ProgramParametersException)
{
   parametersIndex = 1;
}
      
/**
 * get the program name without the path.
 */
QString 
ProgramParameters::getProgramNameWithoutPath() const throw (ProgramParametersException)
{
   if (parameters.empty()) {
      throw ProgramParametersException("Parameters passed to constructor were empty.");
   }
   const QString name(FileUtilities::basename(parameters[0]));
   return name;
}

/**
 * get the program name with the path.
 */
QString 
ProgramParameters::getProgramNameWithPath() const throw (ProgramParametersException)
{
   if (parameters.empty()) {
      throw ProgramParametersException("Parameters passed to constructor were empty.");
   }
   const QString name(parameters[0]);
   return name;
}

/**
 * see if parameters are remaining.
 */
bool 
ProgramParameters::getParametersAvailable() const throw (ProgramParametersException)
{
   if (parametersIndex < static_cast<int>(parameters.size())) {
      return true;
   }
   return false;
}

/**
 * get next parameter as a string.
 */
QString 
ProgramParameters::getNextParameterAsString(const QString& parameterNameForErrorMessage) throw (ProgramParametersException)
{
   if (getParametersAvailable() == false) {
      const QString msg("Parameter named \""
                        + parameterNameForErrorMessage
                        + "\" is missing.");
      throw ProgramParametersException(msg);
   }
   
   const QString name(parameters[parametersIndex]);
   parametersIndex++;
   return name;
}

/**
 * get next parameter as an int.
 */
int 
ProgramParameters::getNextParameterAsInt(const QString& parameterNameForErrorMessage) throw (ProgramParametersException)
{
   const QString s(getNextParameterAsString(parameterNameForErrorMessage));
   bool ok = false;
   const int i = s.toInt(&ok);
   if (ok == false) {
      throw ProgramParametersException("Error converting parameter named \""
                                       + parameterNameForErrorMessage
                                       + "\" with value \""
                                       + s
                                       + "\" to an integer");
   }
   return i;
}

/**
 * get next parameter as a float.
 */
float 
ProgramParameters::getNextParameterAsFloat(const QString& parameterNameForErrorMessage) throw (ProgramParametersException)
{
   const QString s(getNextParameterAsString(parameterNameForErrorMessage));
   bool ok = false;
   const float f = s.toFloat(&ok);
   if (ok == false) {
      throw ProgramParametersException("Error converting parameter named \""
                                       + parameterNameForErrorMessage
                                       + "\" with value \""
                                       + s
                                       + "\" to a float");
   }
   return f;
}

/**
 * get next parameter as a double
 */
double 
ProgramParameters::getNextParameterAsDouble(const QString& parameterNameForErrorMessage) throw (ProgramParametersException)
{
   const QString s(getNextParameterAsString(parameterNameForErrorMessage));
   bool ok = false;
   const double d = s.toDouble(&ok);
   if (ok == false) {
      throw ProgramParametersException("Error converting parameter named \""
                                       + parameterNameForErrorMessage
                                       + "\" with value \""
                                       + s
                                       + "\" to an double");
   }
   return d;
}

/**
 * get next parameter as boolean.
 */
bool 
ProgramParameters::getNextParameterAsBoolean(const QString& parameterNameForErrorMessage) throw (ProgramParametersException)
{
   const QString s(getNextParameterAsString(parameterNameForErrorMessage).toLower());

   if (s == "true") return true;
   if (s == "false") return false;
   
   throw ProgramParametersException(parameterNameForErrorMessage 
                                    + " should have a value of \"true\" or \"false\".");
}
      
/**
 * unget the last parameter.
 */
void 
ProgramParameters::ungetLastParameter() throw (ProgramParametersException)
{
   if (parametersIndex <= 1) {
      throw ProgramParametersException("Cannot unget last parameter, already "
                                       "at first parameter");
   }
}

/**
 * get several parameters as strings.
 */
void 
ProgramParameters::getNextParametersAsStrings(const QString& parameterNameForErrorMessage,
                                              const int numberOfParametersToGet,
                                              std::vector<QString>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   for (int i = 0; i < numberOfParametersToGet; i++) {
      const QString paramName(parameterNameForErrorMessage
                              + "[" + QString::number(i) + "]");
      parametersOut.push_back(getNextParameterAsString(paramName));
   }
}

/**
 * get several parameters as ints.
 */
void 
ProgramParameters::getNextParametersAsInts(const QString& parameterNameForErrorMessage,
                                           const int numberOfParametersToGet,
                                           std::vector<int>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   for (int i = 0; i < numberOfParametersToGet; i++) {
      const QString paramName(parameterNameForErrorMessage
                              + "[" + QString::number(i) + "]");
      parametersOut.push_back(getNextParameterAsInt(paramName));
   }
}

/**
 * get several parameters as floats.
 */
void 
ProgramParameters::getNextParametersAsFloats(const QString& parameterNameForErrorMessage,
                                             const int numberOfParametersToGet,
                                             std::vector<float>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   for (int i = 0; i < numberOfParametersToGet; i++) {
      const QString paramName(parameterNameForErrorMessage
                              + "[" + QString::number(i) + "]");
      parametersOut.push_back(getNextParameterAsFloat(paramName));
   }
}

/**
 * get several parameters as doubles.
 */
void 
ProgramParameters::getNextParametersAsDoubles(const QString& parameterNameForErrorMessage,
                                              const int numberOfParametersToGet,
                                              std::vector<double>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   for (int i = 0; i < numberOfParametersToGet; i++) {
      const QString paramName(parameterNameForErrorMessage
                              + "[" + QString::number(i) + "]");
      parametersOut.push_back(getNextParameterAsDouble(paramName));
   }
}      

/**
 * get remaining parameters as strings.
 */
void 
ProgramParameters::getRemainingParametersAsStrings(const QString& parameterNameForErrorMessage,
                                     std::vector<QString>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   while (getParametersAvailable()) {
      parametersOut.push_back(getNextParameterAsString(parameterNameForErrorMessage));
   }
}

/**
 * get remaining parameters as ints.
 */
void 
ProgramParameters::getRemainingParametersAsInts(const QString& parameterNameForErrorMessage,
                                     std::vector<int>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   while (getParametersAvailable()) {
      parametersOut.push_back(getNextParameterAsInt(parameterNameForErrorMessage));
   }
}

/**
 * get remaining parameters as floats.
 */
void 
ProgramParameters::getRemainingParametersAsFloats(const QString& parameterNameForErrorMessage,
                                     std::vector<float>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   while (getParametersAvailable()) {
      parametersOut.push_back(getNextParameterAsFloat(parameterNameForErrorMessage));
   }
}

/**
 * get remaining parameters as doubles.
 */
void 
ProgramParameters::getRemainingParametersAsDoubles(const QString& parameterNameForErrorMessage,
                                     std::vector<double>& parametersOut) throw (ProgramParametersException)
{
   parametersOut.clear();
   while (getParametersAvailable()) {
      parametersOut.push_back(getNextParameterAsDouble(parameterNameForErrorMessage));
   }
}      

/**
 * get the next parameter as a volume file name and label separated by a semicolon.
 */
void 
ProgramParameters::getNextParameterAsVolumeFileNameAndLabel(const QString& parameterNameForErrorMessage,
                                                            QString& volumeFileNameOut,
                                                            QString& volumeFileLabelOut) throw (ProgramParametersException)
{
   volumeFileNameOut  = "";
   volumeFileLabelOut = "";
   
   volumeFileNameOut = getNextParameterAsString(parameterNameForErrorMessage);

   const QString delimeter = getVolumeFileNameAndLabelDelimeter();
   
   int colonPos = volumeFileNameOut.indexOf(delimeter);
   if (colonPos != -1) {
      volumeFileLabelOut = volumeFileNameOut.mid(0, colonPos);
      volumeFileNameOut = volumeFileNameOut.mid(colonPos + delimeter.length());
   }
   
   
}

/**
 * get the next parameter as structure.
 */
Structure 
ProgramParameters::getNextParameterAsStructure(const QString& parameterNameForErrorMessage) throw (ProgramParametersException)
{
   const QString name(getNextParameterAsString(parameterNameForErrorMessage));
   Structure structure;
   structure.setTypeFromString(name);
   if (structure.isInvalid()) {
      throw ProgramParametersException("Structure specified "
                                       + name
                                       + " is invalid.");
   }
   return structure;
}
      
/**
 * see if there is a parameter with specified name (does not affect "next parameter").
 */
bool 
ProgramParameters::getParameterWithValueExists(const QString& value) const
{
   const int num = getNumberOfParameters();
   for (int i = 0; i < num; i++) {
      if (parameters[i] == value) {
         return true;
      }
   }
   return false;
}

      
/**
 * split up remaining parameters (may have more than one in single string).
 */
void 
ProgramParameters::splitUpRemainingParameters()
{
   const int savedParameterIndex = parametersIndex;
   
   std::vector<QString> paramsSplitUp;
   
   while (getParametersAvailable()) {
      //
      // Get the next parameter and split it up using whitespace
      //
      const QStringList sl = 
         getNextParameterAsString("").split(QRegExp("\\s+"), QString::SkipEmptyParts);
      for (int i = 0; i < sl.count(); i++) {
         const QString s = sl.at(i).trimmed();
         if (s.isEmpty() == false) {
            paramsSplitUp.push_back(s);
         }
      }
   }
   
   //
   // Were parameters split up?
   //
   if (paramsSplitUp.empty() == false) {
      //
      // Remove end of existing parameters
      //
      parameters.erase(parameters.begin() + savedParameterIndex,
                       parameters.end());
                       
      //
      // Add on split up parameters
      //
      parameters.insert(parameters.end(), 
                        paramsSplitUp.begin(), paramsSplitUp.end());
   
      //
      // Reset index of next parameter
      //
      parametersIndex = savedParameterIndex;
   }
}
                                                    
