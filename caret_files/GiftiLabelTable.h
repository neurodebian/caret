
#ifndef __GIFTI_LABEL_TABLE_H__
#define __GIFTI_LABEL_TABLE_H__

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

#include <QString>
#include <vector>

#include "FileException.h"

class ColorFile;
class QTextStream;
class StringTable;

/// table for label indices and names
class GiftiLabelTable {
   public:
      // constructor
      GiftiLabelTable();
      
      // copy constructor
      GiftiLabelTable(const GiftiLabelTable& nlt);
      
      // destructor
      ~GiftiLabelTable();

      // assignment operator
      GiftiLabelTable& operator=(const GiftiLabelTable& nlt);
      
      // add a label (returns the label's index)
      int addLabel(const QString& labelName);
      
      // append a label table to this one
      void append(const GiftiLabelTable& nlt,
                  const std::vector<int>* labelsWithTheseIndicesOnly = NULL);
      
      // assign colors to the labels
      void assignColors(const ColorFile& colorFile);
      
      // clear out the labels
      void clear();
      
      // are there labels ?
      bool empty() const { return labels.empty(); }
      
      // get the index of a label
      int getLabelIndex(const QString& labelName) const;
      
      // get label using its index
      QString getLabel(const int indx) const;
      
      // get all labels
      void getAllLabels(std::vector<QString>& labelsOut) const;
           
      /// get the number of labels
      int getNumberOfLabels() const { return labels.size(); }
      
      // set the label for a specified index (index must by >= 0)
      void setLabel(const int indx,
                    const QString& labelName);
        
      // set a label's color file index
      void setColorFileIndex(const int indx,
                             const int colorFileIndex);
      
      // get a labels color file index
      int getColorFileIndex(const int indx) const;
      
      // write the label table
      void writeAsXML(QTextStream& stream,
                      const int indentOffset) const;
                      
      /// write the data into a StringTable
      void writeDataIntoStringTable(StringTable& table) const;
      
      /// read the data from a StringTable
      void readDataFromStringTable(const StringTable& table) throw (FileException);
      
   protected:
      /// class used for storing a label and its corresponding color index
      class LabelData {
         public:
            /// constructor
            LabelData(const QString& labelNameIn) {
               labelName = labelNameIn;
               colorFileIndex = -1;
            };
            
            /// destructor
            ~LabelData() { }
            
            /// get the label name
            QString getLabelName() const { return labelName; }
            
            /// set the label name
            void setLabelName(const QString& labelNameIn) { labelName = labelNameIn; }
            
            /// get the color file index
            int getColorFileIndex() const { return colorFileIndex; }
            
            /// set the color file index
            void setColorFileIndex(const int indx) { colorFileIndex = indx; }
            
         protected:
            /// name of label
            QString labelName;
         
            /// index in color file
            int colorFileIndex;
      };
      
      // copy helper used by copy constructor and assignement operators
      void copyHelperGiftiLabelTable(const GiftiLabelTable& nlt);
      
      /// container for labels
      typedef std::vector<LabelData> LabelContainer;
      
      /// iterator for label container
      typedef LabelContainer::iterator LabelContainerIterator;
      
      /// iterator for label container
      typedef LabelContainer::const_iterator ConstLabelContainerIterator;
      
      // the labels
      LabelContainer labels;
      
      /*** IF ADDITIONAL MEMBERS ARE ADDED UPDATE copyHelperGiftiLabelTable() ***/
};

#endif // __GIFTI_LABEL_TABLE_H__
