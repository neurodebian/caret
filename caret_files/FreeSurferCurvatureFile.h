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

#ifndef __FREE_SURFER_CURVATURE_FILE_H__
#define __FREE_SURFER_CURVATURE_FILE_H__

#include "AbstractFile.h"

/// class for reading/writing free surfer curvature file
class FreeSurferCurvatureFile : public AbstractFile {
   public:
      /// Constructor
      FreeSurferCurvatureFile();
      
      /// Destructor
      ~FreeSurferCurvatureFile();
      
      /// clear the file
      void clear();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      /// get the number of vertices
      int getNumberOfVertices() const;
      
      /// set number of vertices
      void setNumberOfVertices(const int numVertices);
      
      /// get curvature value
      void getCurvature(const int vertexNumber, float xyz[3],
                        float& curve) const;
      
      /// set curvature value
      void setCurvature(const int vertexNumber, const float xyz[3],
                        const float curve);
      
      /// read the specified file 
      //void readFile(const QString& filenameIn)  throw (FileException);
      
      /// Write the current file's memory to the specified name
      //void writeFile(const QString& filenameIn) throw (FileException);
      
   private:
      /// read  file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write  file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
            
      /// class storage of curvature
      class Curvature {
         public:
            Curvature() {
               xyz[0] = 0.0;
               xyz[1] = 0.0;
               xyz[2] = 0.0;
               curve  = 0.0;
            }
            Curvature(const float xyzIn[3], const float c) {
               xyz[0] = xyzIn[0];
               xyz[1] = xyzIn[1];
               xyz[2] = xyzIn[2];
               curve  = c;
            }
            float xyz[3];
            float curve;
      };
      
      /// the curvature
      std::vector<Curvature> curvature;
};

#endif // __FREE_SURFER_CURVATURE_FILE_H__

