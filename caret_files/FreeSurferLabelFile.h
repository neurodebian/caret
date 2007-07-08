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

#ifndef __FREE_SURFER_LABEL_FILE_H__
#define __FREE_SURFER_LABEL_FILE_H__

#include "AbstractFile.h"

/// class for reading/writing free surfer label file
class FreeSurferLabelFile : public AbstractFile {
   public:
      /// Constructor
      FreeSurferLabelFile();
      
      /// Destructor
      ~FreeSurferLabelFile();
      
      /// clear the file
      void clear();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      /// get the number of label items
      int getNumberOfLabelItems() const;
      
      /// set number of vertices
      void setNumberOfLabelItems(const int numLabelItems);
      
      /// get label value
      void getLabelItem(const int index, int& vertexNumber, float xyz[3]) const;
      
      /// set label value
      void setLabelItem(const int index, const int vertexNumber, const float xyz[3]);
      
      /// add label value
      void addLabelItem(const int vertexNumber, const float xyz[3]);
      
   private:
      /// read  file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write  file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
            
      /// class storage of curvature
      class LabelData {
         public:
            LabelData() {
               xyz[0] = 0.0;
               xyz[1] = 0.0;
               xyz[2] = 0.0;
               vertexNumber = 0;
            }
            LabelData(const int vertexNumberIn, const float xyzIn[3]) {
               vertexNumber = vertexNumberIn;
               xyz[0] = xyzIn[0];
               xyz[1] = xyzIn[1];
               xyz[2] = xyzIn[2];
            }
            float xyz[3];
            int vertexNumber;
      };
      
      /// the curvature
      std::vector<LabelData> labelData;
};

#endif // __FREE_SURFER_LABEL_FILE_H__

