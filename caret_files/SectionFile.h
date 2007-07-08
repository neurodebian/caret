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


#ifndef __VE_SECTIONS_FILE__
#define __VE_SECTIONS_FILE__

#include "NodeAttributeFile.h"

/// This file contains section information for a surface.
class SectionFile : public NodeAttributeFile {
   public:
   
      /// constructor
      SectionFile();
      
      /// destructor
      ~SectionFile();
      
      /// append a node attribute  file to this one
      void append(NodeAttributeFile& naf) throw (FileException);

      /// append a node attribute file to this one but selectively load/overwrite columns
      /// columnDestination is where naf's columns should be (-1=new, -2=do not load)
      void append(NodeAttributeFile& naf, 
                          std::vector<int> columnDestination,
                          const FILE_COMMENT_MODE fcm) throw (FileException);
      
      /// add columns to this sections file
      void addColumns(const int numberOfNewColumns);
      
      /// Add nodes to the file
      void addNodes(const int numberOfNodesToAdd);
      
      /// clears sections memory
      void clear();

      /// deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                      NodeAttributeFile& deformedFile,
                      const DEFORM_TYPE dt) const throw (FileException);

      /// reset a column of data
      void resetColumn(const int columnNumber);
      
      /// remove a column of data
      void removeColumn(const int columnNumber);
      
      /// get the minimum section
      int getMinimumSection(const int col) const;
      
      /// get the maximum section
      int getMaximumSection(const int col) const;
      
      /// set number of nodes
      void setNumberOfNodesAndColumns(const int numNodes, const int numCols);
      
      /// get section for a node
      int getSection(const int nodeNumber, const int columnNumber) const;
      
      /// set section for a node
      void setSection(const int nodeNumber, const int columnNumber, const int sectionNum);
      
      /// should be called after a new column is created and the sections are set
      void postColumnCreation(const int columnNumber);
   
   private:
      /// sections storage
      std::vector<int> sections;
      
      /// minimum section for each column
      std::vector<int> minimumSection;
      
      /// maximum section for each column
      std::vector<int> maximumSection;
      
      /// read file version 0 data
      void readFileDataVersion0(QTextStream& stream, QDataStream& binStream) throw (FileException);

      /// read an sections file data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// write an setctions file data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
   public:      
};

#ifdef _SECTION_FILE_MAIN_

#endif // _SECTION_FILE_MAIN_

#endif // __VE_SECTIONS_FILE__
