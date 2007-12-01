
#ifndef __PROGRAM_PARAMETERS_H__
#define __PROGRAM_PARAMETERS_H__

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

#include <vector>

#include <QString>

#include "ProgramParametersException.h"
#include "Structure.h"

/// class for handling and accessing a programs parameters
class ProgramParameters {
   public:
      // constructor
      ProgramParameters(const int argc, char* argv[]);
      
      // constructor
      ProgramParameters(const QString& programName,
                        const QStringList& programParameters);
      
      // destructor
      ~ProgramParameters();
   
      // get the number of parameters (value will change if splitUpRemainingParameters() called)
      int getNumberOfParameters() const throw (ProgramParametersException);
      
      // reset the parameters index
      void resetParametersIndex() throw (ProgramParametersException);
      
      // get the program name without the path
      QString getProgramNameWithoutPath() const throw (ProgramParametersException);
      
      // get the program name with the path
      QString getProgramNameWithPath() const throw (ProgramParametersException);
      
      // see if parameters are remaining
      bool getParametersAvailable() const throw (ProgramParametersException);
      
      // get next parameter as a string
      QString getNextParameterAsString(const QString& parameterNameForErrorMessage) throw (ProgramParametersException);
      
      // get next parameter as an int
      int getNextParameterAsInt(const QString& parameterNameForErrorMessage) throw (ProgramParametersException);
      
      // get next parameter as a float
      float getNextParameterAsFloat(const QString& parameterNameForErrorMessage) throw (ProgramParametersException);
      
      // get next parameter as a double
      double getNextParameterAsDouble(const QString& parameterNameForErrorMessage) throw (ProgramParametersException);
      
      // get next parameter as boolean
      bool getNextParameterAsBoolean(const QString& parameterNameForErrorMessage) throw (ProgramParametersException);
      
      // get several parameters as strings
      void getNextParametersAsStrings(const QString& parameterNameForErrorMessage,
                                      const int numberOfParametersToGet,
                                      std::vector<QString>& parametersOut) throw (ProgramParametersException);
      
      // get several parameters as ints
      void getNextParametersAsInts(const QString& parameterNameForErrorMessage,
                                   const int numberOfParametersToGet,
                                   std::vector<int>& parametersOut) throw (ProgramParametersException);
      
      // get several parameters as floats
      void getNextParametersAsFloats(const QString& parameterNameForErrorMessage,
                                     const int numberOfParametersToGet,
                                     std::vector<float>& parametersOut) throw (ProgramParametersException);
      
      // get several parameters as doubles
      void getNextParametersAsDoubles(const QString& parameterNameForErrorMessage,
                                      const int numberOfParametersToGet,
                                      std::vector<double>& parametersOut) throw (ProgramParametersException);
      
      // get remaining parameters as strings
      void getRemainingParametersAsStrings(const QString& parameterNameForErrorMessage,
                                           std::vector<QString>& parametersOut) throw (ProgramParametersException);

      // get remaining parameters as ints
      void getRemainingParametersAsInts(const QString& parameterNameForErrorMessage,
                                           std::vector<int>& parametersOut) throw (ProgramParametersException);

      // get remaining parameters as floats
      void getRemainingParametersAsFloats(const QString& parameterNameForErrorMessage,
                                           std::vector<float>& parametersOut) throw (ProgramParametersException);

      // get remaining parameters as doubles
      void getRemainingParametersAsDoubles(const QString& parameterNameForErrorMessage,
                                           std::vector<double>& parametersOut) throw (ProgramParametersException);

      // get the next parameter as structure
      Structure getNextParameterAsStructure(const QString& parameterNameForErrorMessage) throw (ProgramParametersException);
      
      // get the next parameter as a volume file name and label separated by a semicolon
      void getNextParameterAsVolumeFileNameAndLabel(const QString& parameterNameForErrorMessage,
                                                    QString& volumeFileNameOut,
                                                    QString& volumeFileLabelOut) throw (ProgramParametersException);
      
      // get the volume file name and label delimeter
      static QString getVolumeFileNameAndLabelDelimeter() { return ":::"; }
      
      // see if there is a parameter with specified name (does not affect "next parameter")
      bool getParameterWithValueExists(const QString& value) const;
      
      // unget the last parameter
      void ungetLastParameter() throw (ProgramParametersException);
      
      // split up remaining parameters (may have more than one in single string)
      void splitUpRemainingParameters();
      
   protected:
      // the list of parameters
      std::vector<QString> parameters;
      
      // index of current parameter
      int parametersIndex;
};

#endif // __PROGRAM_PARAMETERS_H__
