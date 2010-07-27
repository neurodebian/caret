
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

#include "ColorFile.h"
#include "GiftiCommon.h"
#include "GiftiLabelTable.h"
#include "StringTable.h"

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
   
   labels.push_back(LabelData(labelName));
   return (labels.size() - 1);
}

/**
 * delete a label.
 */
void 
GiftiLabelTable::deleteLabel(const int labelIndex)
{
   labels.erase(labels.begin() + labelIndex);
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
   hadColorsWhenRead = false;
}

/**
 * get the index of a label (returns -1 if not found).
 */
int 
GiftiLabelTable::getLabelIndex(const QString& labelName) const
{
   int numLabels = getNumberOfLabels();
   for (int i = 0; i < numLabels; i++) {
      if (labels[i].getLabelName() == labelName) {
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
      return labels[indx].getLabelName();
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
      labels.resize(indx + 1, LabelData(""));
   }
   labels[indx].setLabelName(labelName);
}


/**
 * Get the color components as floats ranging 0.0 to 1.0
 */
void
GiftiLabelTable::getColorFloat(const int indx,
                   float& red,
                   float& green,
                   float& blue,
                   float& alpha) const
{
   if ((indx >= 0) && (indx < static_cast<int>(labels.size()))) {
      labels[indx].getColorFloat(red, green, blue, alpha);
   }
}

/**
 * Set the color components from floats ranging 0.0 to 1.0 .
 */
void
GiftiLabelTable::setColorFloat(const int indx,
                   float red,
                   float green,
                   float blue,
                   float alpha)
{
   //
   // Add space if needed
   //
   if (indx >= getNumberOfLabels()) {
      labels.resize(indx + 1, LabelData(""));
   }
   labels[indx].setColorFloat(red, green, blue, alpha);
}

/**
 * Get teh color components.
 */
void
GiftiLabelTable::getColor(const int indx,
              unsigned char& red,
              unsigned char& green,
              unsigned char& blue,
              unsigned char& alpha) const
{
   if ((indx >= 0) && (indx < static_cast<int>(labels.size()))) {
      labels[indx].getColor(red, green, blue, alpha);
   }
}
/**
 * Set the color components.
 */
void
GiftiLabelTable::setColor(const int indx,
              unsigned char red,
              unsigned char green,
              unsigned char blue,
              unsigned char alpha)
{
   //
   // Add space if needed
   //
   if (indx >= getNumberOfLabels()) {
      labels.resize(indx + 1, LabelData(""));
   }

   float r = ((float)red)   / 255.0;
   float g = ((float)green) / 255.0;
   float b = ((float)blue)  / 255.0;
   float a = ((float)alpha) / 255.0;

   labels[indx].setColorFloat(r, g, b, a);
}

/**
 * get label enabled using its index.
 */
bool 
GiftiLabelTable::getLabelEnabled(const int indx) const
{
   if ((indx >= 0) && (indx < static_cast<int>(labels.size()))) {
      return labels[indx].getLabelEnabled();
   }
   return false;
}

/**
 * set label enabled for a specified index (index must be >= 0).
 */
void 
GiftiLabelTable::setLabelEnabled(const int indx,
                                 const bool b)
{
   if ((indx >= 0) && (indx < static_cast<int>(labels.size()))) {
      labels[indx].setLabelEnabled(b);
   }
}
                           
/**
 * set all labels enabled.
 */
void 
GiftiLabelTable::setAllLabelsEnabled(const bool b)
{
   const int num = getNumberOfLabels();
   for (int i = 0; i < num; i++) {
      setLabelEnabled(i, b);
   }
}

/**
 * set a label's color file index.
 */
void 
GiftiLabelTable::setColorFileIndex(const int indx,
                                   const int colorFileIndex)
{
   if ((indx >= 0) &&
       (indx < getNumberOfLabels())) {
      labels[indx].setColorFileIndex(colorFileIndex);
   }
}

/**
 * get a labels color file index.
 */
int 
GiftiLabelTable::getColorFileIndex(const int indx) const
{
   if ((indx >= 0) &&
       (indx < getNumberOfLabels())) {
      return labels[indx].getColorFileIndex();
   }
   
   return -1;
}
      
/**
 * get all labels.
 */
void 
GiftiLabelTable::getAllLabels(std::vector<QString>& labelsOut) const
{
   const int num = getNumberOfLabels();
   labelsOut.clear();
   for (int i = 0; i < num; i++) {
      labelsOut.push_back(labels[i].getLabelName());
   }
}
                  
/**
 * copy helper used by copy constructor and assignement operators.
 */
void 
GiftiLabelTable::copyHelperGiftiLabelTable(const GiftiLabelTable& nlt)
{
   labels = nlt.labels;
   hadColorsWhenRead = nlt.hadColorsWhenRead;
}      

/**
 * Get colors from the labels.
 */
void
GiftiLabelTable::addColorsToColorFile(ColorFile& colorFile)
{
   unsigned char defRed, defGreen, defBlue, defAlpha;
   GiftiLabelTable::getDefaultColor(defRed, defGreen, defBlue, defAlpha);

   const int numLabels = getNumberOfLabels();
   for (int i = 0; i < numLabels; i++) {
      unsigned char r, g, b, a;
      labels[i].getColor(r, g, b, a);
      QString labelName = labels[i].getLabelName();

      bool haveColorInColorFile = colorFile.getColorExists(labelName);
      if (haveColorInColorFile == false) {
         //
         // Add color since it does not exist in color file
         //
         colorFile.addColor(labelName, r, g, b, a);
      }
      else {
         //
         // Color is in color file so override only if
         // label color is not the default color
         //
         if ((r != defRed) ||
             (g != defGreen) ||
             (b != defBlue) ||
             (a != defAlpha)) {
            colorFile.addColor(labelName, r, g, b, a);
         }
      }
   }

}

/**
 * Create a label from each of the colors in a color file.
 */
void
GiftiLabelTable::createLabelsFromColors(const ColorFile& colorFile)
{
   int numColors = colorFile.getNumberOfColors();
   for (int i = 0; i < numColors; i++) {
      const ColorFile::ColorStorage* cs = colorFile.getColor(i);
      QString name = cs->getName();
      unsigned char r, g, b, a;
      cs->getRgba(r, g, b, a);

      this->labels.push_back(LabelData(name, r, g, b, a));
   }
}

/**
 * assign colors to the labels.
 */
void 
GiftiLabelTable::assignColors(const ColorFile& colorFile)
{
   unsigned char r, g, b, a;
   GiftiLabelTable::getDefaultColor(r, g, b, a);
   bool exactMatch = false;
   const int numLabels = getNumberOfLabels();
   for (int i = 0; i < numLabels; i++) {
      QString labelName = labels[i].getLabelName();
      int indx = colorFile.getColorByName(labelName,
                                          exactMatch,
                                          r, g, b, a);
      labels[i].setColorFileIndex(indx);
      labels[i].setColor(r, g, b, a);
   }
   
   //
   // Use alpha of zero for ???
   //
   int unknownIndex = this->addLabel("???");
   if (unknownIndex >= 0) {
      float r, g, b, a;
      this->getColorFloat(unknownIndex, r, g, b, a);
      this->setColorFloat(unknownIndex, r, r, b, 0.0);
   }
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
         float red, green, blue, alpha;
         labels[i].getColorFloat(red, green, blue, alpha);
         GiftiCommon::writeIndentationXML(stream, indent);
         stream << "<" << GiftiCommon::tagLabel << " "
                << GiftiCommon::attKey << "=\"" << i << "\" "
                << GiftiCommon::attRed << "=\"" << QString::number(red, 'f', 3) << "\" "
                << GiftiCommon::attGreen << "=\"" << QString::number(green, 'f', 3) << "\" "
                << GiftiCommon::attBlue << "=\"" << QString::number(blue, 'f', 3) << "\" "
                << GiftiCommon::attAlpha << "=\"" << QString::number(alpha, 'f', 3) << "\""
                << ">";
         stream << "<![CDATA["
                << labels[i].getLabelName()
                << "]]>";
         stream << "</" << GiftiCommon::tagLabel << ">" << "\n";
      }
      
      indent--;
      GiftiCommon::writeIndentationXML(stream, indent);
      stream << "</" << GiftiCommon::tagLabelTable << ">" << "\n";
   }
}

/**
 * Write as Caret6 XML.
 */
void
GiftiLabelTable::writeAsXML(XmlGenericWriter& xmlWriter) const
{
   xmlWriter.writeStartElement(GiftiCommon::tagLabelTable);

   const int numLabels = getNumberOfLabels();
   for (int i = 0; i < numLabels; i++) {
      float red, green, blue, alpha;
      labels[i].getColorFloat(red, green, blue, alpha);

      XmlGenericWriterAttributes attributes;
      attributes.addAttribute(GiftiCommon::attKey, i);
      attributes.addAttribute(GiftiCommon::attRed, red);
      attributes.addAttribute(GiftiCommon::attGreen, green);
      attributes.addAttribute(GiftiCommon::attBlue, blue);
      attributes.addAttribute(GiftiCommon::attAlpha, alpha);
      xmlWriter.writeElementCData(GiftiCommon::tagLabel,
                                  attributes,
                                  labels[i].getLabelName());
   }

   xmlWriter.writeEndElement();

}

/**
 * write the data into a StringTable.
 */
void 
GiftiLabelTable::writeDataIntoStringTable(StringTable& table) const
{
   const int numLabels = getNumberOfLabels();
   if (numLabels <= 0) {
      return;
   }
   table.setNumberOfRowsAndColumns(numLabels, 2, GiftiCommon::tagLabelTable);
   
   table.setTableTitle(GiftiCommon::tagLabelTable);
   table.setColumnTitle(0, "index");
   table.setColumnTitle(1, GiftiCommon::tagLabel);
   
   int rowCount = 0;
   for (int i = 0; i < numLabels; i++) {
      table.setElement(rowCount, 0, i);
      table.setElement(rowCount, 1, labels[i].getLabelName());
      rowCount++;
   }
}

/**
 * read the data from a StringTable.
 */
void 
GiftiLabelTable::readDataFromStringTable(const StringTable& table) throw (FileException)
{
   clear();
   
   int indexCol = -1;
   int labelCol = -1;
   for (int j = 0; j < table.getNumberOfColumns(); j++) {
      if (table.getColumnTitle(j) == "index") {
         indexCol = j;
      }
      else if (table.getColumnTitle(j) == GiftiCommon::tagLabel) {
         labelCol = j;
      }
   }
   
   if ((indexCol < 0) || (labelCol < 0)) {
      throw FileException("GiftiLabelTable: Unable to find index and label column titles.");
   }
   
   for (int i = 0; i < table.getNumberOfRows(); i++) {
      const int indx = table.getElementAsInt(i, indexCol);
      const QString label = table.getElement(i, labelCol);
      setLabel(indx, label);
   }
}
      
