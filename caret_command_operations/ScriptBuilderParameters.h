
#ifndef __SCRIPT_BUILDER_PARAMTERS_H__
#define __SCRIPT_BUILDER_PARAMTERS_H__

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
#include <vector>

#include <QString>
#include <QStringList>

/// class for explaing a caret_commands operations to the script builder
class ScriptBuilderParameters {
   public:
      /// class for storing parameters
      class Parameter {
         public:
            /// parameter type
            enum TYPE {
               /// boolean
               TYPE_BOOLEAN,
               /// directory 
               TYPE_DIRECTORY,
               /// data file
               TYPE_FILE,
               /// multiple data files
               TYPE_FILE_MULTIPLE,
               /// flag
               //TYPE_FLAG,
               /// float 
               TYPE_FLOAT,
               /// integer
               TYPE_INT,
               /// list of items
               TYPE_LIST_OF_ITEMS,
               /// string 
               TYPE_STRING,   
               /// structure
               TYPE_STRUCTURE,            
               /// variable list
               TYPE_VARIABLE_LIST_OF_PARAMETERS
            };
      
            /// constructor
            Parameter(const TYPE typeIn,
                      const QString descriptionIn) {
               type        = typeIn;
               description = descriptionIn;
            }
            
            /// destructor
            ~Parameter() {};
            
            /// get the type
            TYPE getType() const { return type; }
            
            /// get the description
            QString getDescription() const { return description; }
            
            /// get the file filter 
            QStringList getFileFilters() const { return fileFilters; }
            
            /// set the file filter 
            void setFileFilters(const QStringList& ff) { fileFilters = ff; }
            
            /// get float parameters
            void getFloatParameters(float& defaultFloatValueOut,
                                    float& minimumFloatValueOut,
                                    float& maximumFloatValueOut) const {
               defaultFloatValueOut = defaultFloatValue;
               minimumFloatValueOut = minimumFloatValue;
               maximumFloatValueOut = maximumFloatValue;
            }
                                    
            /// set float parameters
            void setFloatParameters(const float defaultFloatValueIn,
                                    const float minimumFloatValueIn,
                                    const float maximumFloatValueIn) {
               defaultFloatValue = defaultFloatValueIn;
               minimumFloatValue = minimumFloatValueIn;
               maximumFloatValue = maximumFloatValueIn;
            }
                                    
            /// get int parameters
            void getIntParameters(int& defaultIntValueOut,
                                    int& minimumIntValueOut,
                                    int& maximumIntValueOut) const {
               defaultIntValueOut = defaultIntValue;
               minimumIntValueOut = minimumIntValue;
               maximumIntValueOut = maximumIntValue;
            }
                                    
            /// set int parameters
            void setIntParameters(const int defaultIntValueIn,
                                    const int minimumIntValueIn,
                                    const int maximumIntValueIn) {
               defaultIntValue = defaultIntValueIn;
               minimumIntValue = minimumIntValueIn;
               maximumIntValue = maximumIntValueIn;
            }
                                    
            /// get string parameters
            void getStringParameters(QString& defaultStringValueOut) const {
               defaultStringValueOut = defaultStringValue;
            }
                                    
            /// set string parameters
            void setStringParameters(const QString& defaultStringValueIn) {
               defaultStringValue = defaultStringValueIn;
            }
                                    
            /// get variable list parameters
            void getVariableListParameters(QString& defaultValueOut) const {
               defaultValueOut = defaultVariableListValue;
            }
                                    
            /// set variable list parameters
            void setVariableListParameters(const QString& defaultValueIn) {
               defaultVariableListValue = defaultValueIn;
            }
                                    
            /// get boolean parameters
            void getBooleanParameters(bool& defaultBooleanValueOut) const {
               defaultBooleanValueOut = defaultBooleanValue;
            }
                                    
            /// set boolean parameters
            void setBooleanParameters(const bool defaultBooleanValueIn) {
               defaultBooleanValue = defaultBooleanValueIn;
            }
                   
            /// get list of item parameters
            void getListOfItemParameters(std::vector<QString>& listOfItemValuesOut,
                                         std::vector<QString>& listOfItemDescriptionsOut) const {
               listOfItemValuesOut = listOfItemValues;
               listOfItemDescriptionsOut = listOfItemDescriptions;
            }

            /// set list of item parameters
            void setListOfItemParameters(const std::vector<QString>& listOfItemValuesIn,
                                         const std::vector<QString>& listOfItemDescriptionsIn) {
               listOfItemValues = listOfItemValuesIn;
               listOfItemDescriptions = listOfItemDescriptionsIn;
            }

         protected:  
            /// type of parameter
            TYPE type;
                      
            /// description of parameter
            QString description;
            
            /// file filter
            QStringList fileFilters;
            
            /// default float value
            float defaultFloatValue;
            
            /// minimum float value
            float minimumFloatValue;
            
            /// maximum float value
            float maximumFloatValue;
            
            /// default int value
            int defaultIntValue;
            
            /// minimum int value
            int minimumIntValue;
            
            /// maximum int value
            int  maximumIntValue;
            
            /// default string value
            QString defaultStringValue;
            
            /// variable list default value
            QString defaultVariableListValue;
            
            /// default boolean value
            bool defaultBooleanValue;
            
            /// list of item values
            std::vector<QString> listOfItemValues;
            
            /// list of item descriptions
            std::vector<QString> listOfItemDescriptions;
      };
      
      // constructor
      ScriptBuilderParameters();
      
      // destructor
      ~ScriptBuilderParameters();
      
      // add a directory
      void addDirectory(const QString& descriptionIn);
      
      // add a float parameters
      void addFloat(const QString& descriptionIn,
                    const float defaultValueIn = 0.0,
                    const float minimumValueIn = -std::numeric_limits<float>::max(),
                    const float maximumValueIn = std::numeric_limits<float>::max());
      
      // add an int parameter
      void addInt(const QString& descriptionIn,
                  const int defaultValueIn = 0,
                  const int minimumValueIn = std::numeric_limits<int>::min(),
                  const int maximumValueIn = std::numeric_limits<int>::max());
      
      // add a file
      void addFile(const QString& descriptionIn,
                   const QStringList& fileFiltersIn);
                       
      // add a file
      void addFile(const QString& descriptionIn,
                   const QString& fileFilterIn);
                       
      // add multiple files
      void addMultipleFiles(const QString& descriptionIn,
                            const QString& fileFilterIn);
                            
      // add multiple files
      void addMultipleFiles(const QString& descriptionIn,
                            const QStringList& fileFiltersIn);
                            
      // add a flag
      //void addFlag(const QString& descriptionIn);
      
      // add a list of items
      void addListOfItems(const QString& descriptionIn,
                          const std::vector<QString>& listOfItemValuesIn,
                          const std::vector<QString>& listOfItemDescriptionsIn);
                          
      // add a string parameter
      void addString(const QString& descriptionIn,
                     const QString& defaultValueIn = "");
                     
      // add variable list of parameters
      void addVariableListOfParameters(const QString& descriptionIn,
                                       const QString& defaultValueIn = "");
        
      // add a boolean parameter
      void addBoolean(const QString& descriptionIn,
                      const bool defaultValueIn = false);
            
      // add structure names
      void addStructure(const QString& descriptionIn);
                        
      // clear current parameters
      void clear();

      // get the number of parameters
      int getNumberOfParameters() const { return parameters.size(); }
      
      // get a parameter
      const Parameter* getParameter(const int indx) const { return &parameters[indx]; }
      
   protected:
      /// the parameters
      std::vector<Parameter> parameters;
};

#endif // __SCRIPT_BUILDER_PARAMTERS_H__
