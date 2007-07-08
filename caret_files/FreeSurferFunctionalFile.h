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

#ifndef __FREE_SURFER_FUNCTIONAL_FILE_H__
#define __FREE_SURFER_FUNCTIONAL_FILE_H__

#include "AbstractFile.h"

/// class for reading/writing free surfer functional file
class FreeSurferFunctionalFile : public AbstractFile {
   public:
      /// Constructor
      FreeSurferFunctionalFile();
      
      /// Destructor
      ~FreeSurferFunctionalFile();
      
      /// clear the file
      void clear();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      /// get the number of vertices
      int getNumberOfFunctionalItems() const;
      
      /// set number of vertices
      void setNumberOfFunctionalItems(const int numVertices);
      
      /// get functional value
      void getFunctionalData(const int index, int& vertexNumber, float& funcValue) const;
      
      /// add functional value
      void addFunctionalData(const int vertexNumber, const float funcValue);
      
      /// set functional value
      void setFunctionalData(const int index, const int vertexNumber, const float funcValue);
      
   private:
      /// read  file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write  file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
            
      /// class for storing functional items
      class FuncData {
         public:
            int vertexNumber;
            float funcValue;
            
            FuncData() {
               vertexNumber = 0;
               funcValue    = 0.0;
            }
            FuncData(const int vertexNumberIn, const float funcValueIn) {
               vertexNumber = vertexNumberIn;
               funcValue    = funcValueIn;
            }
      };
      
      /// the functional values
      std::vector<FuncData> functionalData;
};

#endif // __FREE_SURFER_FUNCTIONAL_FILE_H__

