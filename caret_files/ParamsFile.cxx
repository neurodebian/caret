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

#include <sstream>

#define PARAMS_FILE_DEFINE
#include "ParamsFile.h"
#undef  PARAMS_FILE_DEFINE
#include "SpecFile.h"

/**
 * Constructor
 */
ParamsFile::ParamsFile()
   : AbstractFile("Params File", SpecFile::getParamsFileExtension())
{
   clear();
}

/**
 * Destructor
 */
ParamsFile::~ParamsFile()
{
   clear();
}

/**
 * Clear the parameters file.
 */
void
ParamsFile::clear()
{
   clearAbstractFile();
   params.clear();
   setParameter(keyComment, "");
   setParameter(keyXdim, "");
   setParameter(keyYdim, "");
   setParameter(keyZdim, "");
   setParameter(keyACx, "");
   setParameter(keyACy, "");
   setParameter(keyACz, "");
   setParameter(keyPadded, "");
   setParameter(keyCropped, "");
   setParameter(keyCropMinX, "");
   setParameter(keyCropMaxX, "");
   setParameter(keyCropMinY, "");
   setParameter(keyCropMaxY, "");
   setParameter(keyCropMinZ, "");
   setParameter(keyCropMaxZ, "");
   setParameter(keyXmin, "");
   setParameter(keyYmin, "");
   setParameter(keyZmin, "");
   setParameter(keyOldPadNegX, "");
   setParameter(keyOldPadPosX, "");
   setParameter(keyOldPadNegY, "");
   setParameter(keyOldPadPosY, "");
   setParameter(keyOldPadNegZ, "");
   setParameter(keyOldPadPosZ, "");
   setParameter(keyResolution, "");
   setParameter(keyWholeXdim, "");
   setParameter(keyWholeYdim, "");
   setParameter(keyWholeZdim, "");
   setParameter(keyWholeVolumeACx, "");
   setParameter(keyWholeVolumeACy, "");
   setParameter(keyWholeVolumeACz, "");
   setParameter(keyCGMpeak, "");
   setParameter(keyWMpeak, "");
   setParameter(keyWMThreshSet, "");
   
   clearModified();
}

/**
 * Append a parameters file to this one.
 */
void 
ParamsFile::append(ParamsFile& pf, QString errorMessage)
{
   errorMessage = "";
   std::vector<QString> keys, values;
   pf.getAllParameters(keys, values);
   
   for (unsigned int i = 0; i < keys.size(); i++) {
      setParameter(keys[i], values[i]);
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(pf);

   setModified();
}

/**
 * Get all parameters in the file.
 */
void 
ParamsFile::getAllParameters(std::vector<QString>& keys, std::vector<QString>& values) const
{
   keys.clear();
   values.clear();
   
   for (ParameterMapConstIterator pi = params.begin(); 
        pi != params.end(); pi++) {
      keys.push_back(pi->first);
      values.push_back(pi->second);
   }
}

/**
 * Get a parameter as a string.  Returns false if not found.
 */
bool
ParamsFile::getParameterAsString(const QString& parameterName,
                                 QString& parameterValue) const
{
   parameterValue = "";

   ParameterMap::const_iterator pos = params.find(parameterName);
   if (pos != params.end()) {
      parameterValue = pos->second;
      return true;
   }
   return false;
}

/**
 * Get a parameter as an string.  Returns false if not found.
 */
bool
ParamsFile::getParameter(const QString& parameterName, 
                         QString& parameterValue) const
{
   parameterValue = "";

   QString stringParam;
   if (getParameterAsString(parameterName, stringParam)) {
      if (stringParam.length() > 1) {
         if (stringParam[0] == '"') {
            // remove quotes
            stringParam = stringParam.mid(1);
            stringParam.resize(stringParam.length() - 1);
         }
      }
      parameterValue = stringParam;
      return true;
   }
   return false;
}

/**
 * Get a parameter as an int.  Return false if not found
 */
bool
ParamsFile::getParameter(const QString& parameterName, 
                         int& parameterValue) const
{
   parameterValue = 0;

   QString stringParam;
   if (getParameterAsString(parameterName, stringParam)) {
      parameterValue = stringParam.toInt();
      return true;
   }
   return false;
}

/**
 * Get a parameters as a float.  Returns false if not found.
 */
bool
ParamsFile::getParameter(const QString& parameterName, 
                         float& parameterValue) const
{
   parameterValue = 0.0;

   QString stringParam;
   if (getParameterAsString(parameterName, stringParam)) {
      parameterValue = stringParam.toFloat();
      return true;
   }
   return false;
}

/**
 * Set a parameter with a string.
 */
void
ParamsFile::setParameter(const QString& parameterName, 
                         const QString& parameterValue)
{
   setModified();
   ParameterMap::iterator pos = params.find(parameterName);
   if (pos != params.end()) {
      pos->second = parameterValue;
      return;
   }
   params.insert(ParameterMap::value_type(parameterName, parameterValue));
}

/**
 * Set a parameter with an int.
 */
void
ParamsFile::setParameter(const QString& parameterName,  
                         const int& parameterValue)
{
   setModified();
   std::ostringstream str;
   str << parameterValue;
   setParameter(parameterName, str.str().c_str());
}

/**
 * Set a parameter with a float.
 */
void
ParamsFile::setParameter(const QString& parameterName,  
                        const float& parameterValue)
{
   setModified();
   std::ostringstream str;
   str << parameterValue;
   setParameter(parameterName, str.str().c_str());
}

/**
 * read the parameters file's data.
 */
void
ParamsFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   while (!stream.atEnd()) {
      QString line;
      readLine(stream, line);
      
      int equalsPos = line.indexOf('=');
      if (equalsPos != -1) {
         const QString key = line.mid(0, equalsPos);
         const QString value = line.mid(equalsPos + 1);
         setParameter(key, value);
      }
   }
}

/**
 * Write the parameters file's data.
 */
void
ParamsFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   for (ParameterMap::iterator pos = params.begin();
        pos != params.end(); pos++) {
      stream << pos->first << "=" << pos->second << "\n";
   }
}

