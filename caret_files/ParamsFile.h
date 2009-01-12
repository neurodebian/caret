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


#ifndef __VE_PARAMS_FILE_H__
#define __VE_PARAMS_FILE_H__

#include <map>

#include <QString>

#include "AbstractFile.h"

/// Class for the Parameters File
class ParamsFile : public AbstractFile {

   public:
     
      /// parameter map for storing parameters
      typedef std::map<QString,QString> ParameterMap;

      /// iterator for access to ParameterMap
      typedef ParameterMap::const_iterator ParameterMapConstIterator;

      // NOTE: If you add more keys, be sure to update the clear() method
      // with initial values.
      static const QString keyComment;
      static const QString keyXdim;
      static const QString keyYdim;
      static const QString keyZdim;
      static const QString keyACx;
      static const QString keyACy;
      static const QString keyACz;
      static const QString keyPadded;
      static const QString keyCropped;
      static const QString keyCropMinX;
      static const QString keyCropMaxX;
      static const QString keyCropMinY;
      static const QString keyCropMaxY;
      static const QString keyCropMinZ;
      static const QString keyCropMaxZ;
      static const QString keyXmin;
      static const QString keyYmin;
      static const QString keyZmin;
      static const QString keyOldPadNegX;
      static const QString keyOldPadPosX;
      static const QString keyOldPadNegY;
      static const QString keyOldPadPosY;
      static const QString keyOldPadNegZ;
      static const QString keyOldPadPosZ;
      static const QString keyResolution;
      static const QString keyWholeXdim;
      static const QString keyWholeYdim;
      static const QString keyWholeZdim;
      static const QString keyWholeVolumeACx;
      static const QString keyWholeVolumeACy;
      static const QString keyWholeVolumeACz;
      static const QString keyCGMpeak;
      static const QString keyWMpeak;
      static const QString keyWMThreshSet;
      static const QString keyWMThresh;
      static const QString keyHem;
      
      static const QString keyValueYes;
      static const QString keyValueNo;

   private:
      /// storage of parameters
      ParameterMap params;
      
      /// get the parameter as a string
      bool getParameterAsString(const QString& parameterName, 
                                QString& parameterValue) const;
                               
      /// read parameter file data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// write parameter file data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
   public:
      
      /// constructor
      ParamsFile();
      
      /// destructor
      ~ParamsFile();
      
      /// append a parameters file to this one
      void append(ParamsFile& pf, QString errorMessage);
      
      /// clear the parameters
      void clear();
      
      /// see if file contains no data
      bool empty() const { return params.empty(); }
      
      /// method to get all parameters
      void getAllParameters(std::vector<QString>& keys, std::vector<QString>& values) const;
      
      /// get all of the parameters
      ParameterMap getAllParameters() const { return params; }
      
      /// set all parameters
      void setAllParameters(ParameterMap pm) { params = pm; setModified(); }
      
      /// methods to get parameters as string
      bool getParameter(const QString& parameterName, 
                        QString& parameterValue) const;
      
      /// methods to get parameters as int
      bool getParameter(const QString& parameterName, 
                        int& parameterValue) const;
      
      /// methods to get parameters as float
      bool getParameter(const QString& parameterName, 
                        float& parameterValue) const;
      
      // methods to set parameters with string
      void setParameter(const QString& parameterName, 
                        const QString& parameterValue);
      
      // methods to set parameters with int
      void setParameter(const QString& parameterName,  
                        const int& parameterValue);
      
      // methods to set parameters with float
      void setParameter(const QString& parameterName,  
                        const float& parameterValue);
};  


#endif // __VE_PARAMS_FILE_H__

#ifdef PARAMS_FILE_DEFINE

   const QString ParamsFile::keyComment = "comment";
   const QString ParamsFile::keyXdim = "xdim";
   const QString ParamsFile::keyYdim = "ydim";
   const QString ParamsFile::keyZdim = "zdim";
   const QString ParamsFile::keyACx  = "ACx";
   const QString ParamsFile::keyACy  = "ACy";
   const QString ParamsFile::keyACz  = "ACz";
   const QString ParamsFile::keyPadded = "padded";
   const QString ParamsFile::keyCropped = "cropped";
   const QString ParamsFile::keyCropMinX = "CropMinX";
   const QString ParamsFile::keyCropMaxX = "CropMaxX";
   const QString ParamsFile::keyCropMinY = "CropMinY";
   const QString ParamsFile::keyCropMaxY = "CropMaxY";
   const QString ParamsFile::keyCropMinZ = "CropMinZ";
   const QString ParamsFile::keyCropMaxZ = "CropMaxZ";
   const QString ParamsFile::keyXmin = "Xmin";
   const QString ParamsFile::keyYmin = "Ymin";
   const QString ParamsFile::keyZmin = "Zmin";
   const QString ParamsFile::keyOldPadNegX = "OldPadNegX";
   const QString ParamsFile::keyOldPadPosX = "OldPadPosX";
   const QString ParamsFile::keyOldPadNegY = "OldPadNegY";
   const QString ParamsFile::keyOldPadPosY = "OldPadPosY";
   const QString ParamsFile::keyOldPadNegZ = "OldPadNegZ";
   const QString ParamsFile::keyOldPadPosZ = "OldPadPosZ";
   const QString ParamsFile::keyResolution = "resolution";
   const QString ParamsFile::keyWholeXdim = "WholeXdim";
   const QString ParamsFile::keyWholeYdim = "WholeYdim";
   const QString ParamsFile::keyWholeZdim = "WholeZdim";
   const QString ParamsFile::keyWholeVolumeACx = "ACx_WholeVolume";
   const QString ParamsFile::keyWholeVolumeACy = "ACy_WholeVolume";
   const QString ParamsFile::keyWholeVolumeACz = "ACz_WholeVolume";
   const QString ParamsFile::keyCGMpeak = "CGMpeak";
   const QString ParamsFile::keyWMpeak = "WMpeak";
   const QString ParamsFile::keyWMThreshSet = "WMThreshSet";
   const QString ParamsFile::keyWMThresh = "WMThresh";
   const QString ParamsFile::keyHem = "Hem";
      
   const QString ParamsFile::keyValueYes = "yes";
   const QString ParamsFile::keyValueNo  = "no";
#endif // PARAMS_FILE_DEFINE
