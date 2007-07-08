
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

#include <QTextStream>

#include "GiftiCommon.h"
#include "GiftiLabelTable.h"

/**
 * constructor.
 */
GiftiLabelTable::GiftiLabelTable()
{
   clear();
}

/**
 * copy constructor.
 */
GiftiLabelTable::GiftiLabelTable(const GiftiLabelTable& nlt)
{
   copyHelperGiftiLabelTable(nlt);
}

/**
 * destructor.
 */
GiftiLabelTable::~GiftiLabelTable()
{
   clear();
}

/** 
 * assignment operator.
 */
GiftiLabelTable& 
GiftiLabelTable::operator=(const GiftiLabelTable& nlt)
{
   if (this != &nlt) {
      copyHelperGiftiLabelTable(nlt);
   }
   return *this;
}

/**
 * add a label (returns the label's index).
 * If the label is already in the table its index is returned.
 */
int 
GiftiLabelTable::addLabel(const QString& labelName)
{
   const int indx = getLabelIndex(labelName);
   if (indx >= 0) {
      return indx;
   }
   
   labels.push_back(labelName);
   return (labels.size() - 1);
}

/**
 * append a label table to this one.
 */
void 
GiftiLabelTable::append(const GiftiLabelTable& nlt,
                        const std::vector<int>* labelsWithTheseIndicesOnly)
{
   const int numLabels = nlt.getNumberOfLabels();
   if (numLabels > 0) {
      //
      // Determine which labels should be appended
      //
      std::vector<bool> appendThisLabel(numLabels, false);
      if (labelsWithTheseIndicesOnly != NULL) {
         const int num = static_cast<int>(labelsWithTheseIndicesOnly->size());
         for (int i = 0; i < num; i++) {
            const int indx = (*labelsWithTheseIndicesOnly)[i];
            appendThisLabel[indx] = true;
         }
      }
      else {
         std::fill(appendThisLabel.begin(), appendThisLabel.end(), true);
      }
      
      //
      // Append the labels
      //
      for (int i = 0; i < numLabels; i++) {
         if (appendThisLabel[i]) {
            addLabel(nlt.getLabel(i));
         }
      }
   }
}

/**
 * clear out the labels.
 */
void 
GiftiLabelTable::clear()
{
   labels.clear();
}

/**
 * get the index of a label (returns -1 if not found).
 */
int 
GiftiLabelTable::getLabelIndex(const QString& labelName) const
{
   int numLabels = getNumberOfLabels();
   for (int i = 0; i < numLabels; i++) {
      if (labels[i] == labelName) {
         return i;
      }
   }
   return -1;
}

/**
 * get label using its index.
 */
QString 
GiftiLabelTable::getLabel(const int indx) const
{
   if ((indx >= 0) && (indx < static_cast<int>(labels.size()))) {
      return labels[indx];
   }
   return "";
}

/**
 * set the label for a specified index.
 */
void 
GiftiLabelTable::setLabel(const int indx,
                          const QString& labelName)
{
   //
   // Add space if needed
   // 
   if (indx >= getNumberOfLabels()) {
      labels.resize(indx + 1, "");
   }
   labels[indx] = labelName;
}

/**
 * get all labels.
 */
void 
GiftiLabelTable::getAllLabels(std::vector<QString>& labelsOut) const
{
   labelsOut = labels;
}
                  
/**
 * copy helper used by copy constructor and assignement operators.
 */
void 
GiftiLabelTable::copyHelperGiftiLabelTable(const GiftiLabelTable& nlt)
{
   labels = nlt.labels;
}      

/**
 * write metadata (used by other classes so static).
 */
void 
GiftiLabelTable::writeAsXML(QTextStream& stream,
                                const int indentOffset) const
{
   int indent = indentOffset;
   
   if (labels.empty()) {
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "<" << GiftiCommon::tagLabelTable << "/>" << "\n";
   }
   else {
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "<" << GiftiCommon::tagLabelTable << ">" << "\n";
      indent++;
      
      const int numLabels = getNumberOfLabels();
      for (int i = 0; i < numLabels; i++) {
         GiftiCommon::writeIndentationXML(stream, indent);
         stream << "<" << GiftiCommon::tagLabel << " "
                << GiftiCommon::attIndex << "=\"" << i << "\""
                << ">";
         stream << "<![CDATA["
                << labels[i]
                << "]]>";
         stream << "</" << GiftiCommon::tagLabel << ">" << "\n";
      }
      
      indent--;
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "</" << GiftiCommon::tagLabelTable << ">" << "\n";
   }
}
