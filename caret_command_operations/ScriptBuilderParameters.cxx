
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

#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
ScriptBuilderParameters::ScriptBuilderParameters()
{
}

/**
 * destructor.
 */
ScriptBuilderParameters::~ScriptBuilderParameters()
{
}

/**
 * add a float parameters.
 */
void 
ScriptBuilderParameters::addFloat(const QString& descriptionIn,
                                  const float defaultValueIn,
                                  const float minimumValueIn,
                                  const float maximumValueIn)
{
   Parameter p(Parameter::TYPE_FLOAT,
               descriptionIn);
   p.setFloatParameters(defaultValueIn,
                        minimumValueIn,
                        maximumValueIn);
   parameters.push_back(p);
}

/**
 * add an int parameter.
 */
void 
ScriptBuilderParameters::addInt(const QString& descriptionIn,
                                const int defaultValueIn,
                                const int minimumValueIn,
                                const int maximumValueIn)
{
   Parameter p(Parameter::TYPE_INT,
               descriptionIn);
   p.setIntParameters(defaultValueIn,
                        minimumValueIn,
                        maximumValueIn);
   parameters.push_back(p);
}

/**
 * add a string parameter.
 */
void 
ScriptBuilderParameters::addString(const QString& descriptionIn,
                                   const QString& defaultValueIn)
{
   Parameter p(Parameter::TYPE_STRING,
               descriptionIn);
   p.setStringParameters(defaultValueIn);
   parameters.push_back(p);
}
                     
/**
 * add a file.
 */
void 
ScriptBuilderParameters::addFile(const QString& descriptionIn,
                                 const QStringList& fileFiltersIn)
{
   Parameter p(Parameter::TYPE_FILE,
               descriptionIn);
   p.setFileFilters(fileFiltersIn);
   parameters.push_back(p);
}
                 
/**
 * add a directory.
 */
void 
ScriptBuilderParameters::addDirectory(const QString& descriptionIn)
{
   Parameter p(Parameter::TYPE_DIRECTORY,
               descriptionIn);
   parameters.push_back(p);
}
                 
/**
 * add a file.
 */
void 
ScriptBuilderParameters::addFile(const QString& descriptionIn,
                                 const QString& fileFilterIn)
{
   Parameter p(Parameter::TYPE_FILE,
               descriptionIn);
   p.setFileFilters(QStringList(fileFilterIn));
   parameters.push_back(p);
}
                 
/**
 * add multiple files.
 */
void 
ScriptBuilderParameters::addMultipleFiles(const QString& descriptionIn,
                                          const QStringList& fileFiltersIn)
{
   Parameter p(Parameter::TYPE_FILE_MULTIPLE,
               descriptionIn);
   p.setFileFilters(fileFiltersIn);
   parameters.push_back(p);
}
                      
/**
 * add multiple files.
 */
void 
ScriptBuilderParameters::addMultipleFiles(const QString& descriptionIn,
                                          const QString& fileFilterIn)
{
   Parameter p(Parameter::TYPE_FILE_MULTIPLE,
               descriptionIn);
   p.setFileFilters(QStringList(fileFilterIn));
   parameters.push_back(p);
}
                      
/**
 * add a boolean parameter.
 */
void 
ScriptBuilderParameters::addBoolean(const QString& descriptionIn,
                                    const bool defaultValueIn)
{
   Parameter p(Parameter::TYPE_BOOLEAN,
               descriptionIn);
   p.setBooleanParameters(defaultValueIn);
   parameters.push_back(p);
}
                      
/**
 * add a flag.
 */
/*
void 
ScriptBuilderParameters::addFlag(const QString& descriptionIn)
{
   Parameter p(Parameter::TYPE_FLAG,
               descriptionIn);
   parameters.push_back(p);
}
*/
             
/**
 * add a list of items.
 */
void 
ScriptBuilderParameters::addListOfItems(const QString& descriptionIn,
                                        const std::vector<QString>& listOfItemValuesIn,
                                        const std::vector<QString>& listOfItemDescriptionsIn)
{
   Parameter p(Parameter::TYPE_LIST_OF_ITEMS,
               descriptionIn);
   p.setListOfItemParameters(listOfItemValuesIn,
                             listOfItemDescriptionsIn);
   parameters.push_back(p);
}
                          
/**
 * add variable list of parameters.
 */
void 
ScriptBuilderParameters::addVariableListOfParameters(const QString& descriptionIn,
                                                     const QString& defaultValueIn)
{
   Parameter p(Parameter::TYPE_VARIABLE_LIST_OF_PARAMETERS,
               descriptionIn);
   p.setVariableListParameters(defaultValueIn);
   parameters.push_back(p);
}        

/**
 * add structure names.
 */
void 
ScriptBuilderParameters::addStructure(const QString& descriptionIn)
{
   Parameter p(Parameter::TYPE_STRUCTURE,
               descriptionIn);
   parameters.push_back(p);
}
                        
/**
 * clear current parameters.
 */
void 
ScriptBuilderParameters::clear()
{
   parameters.clear();
}

//===============================================================================
