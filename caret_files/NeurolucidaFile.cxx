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

#include <iostream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include "DebugControl.h"
#include "HtmlColors.h"
#define __NEUROLUCIDA_MAIN__
#include "NeurolucidaFile.h"
#undef __NEUROLUCIDA_MAIN__
#include "SpecFile.h"

/**
 * constructor.
 */
NeurolucidaFile::NeurolucidaFile()
   : AbstractFile("Neurolucida File",
                  SpecFile::getNeurolucidaFileExtension(),
                  false,
                  FILE_FORMAT_XML,   // default format
                  FILE_IO_NONE,      // read/write ascii
                  FILE_IO_NONE,      // read/write binary
                  FILE_IO_READ_ONLY, // read/write xml
                  FILE_IO_NONE,      // read/write xml base64
                  FILE_IO_NONE,      // read/write xml gzip
                  FILE_IO_NONE,      // read/write other
                  FILE_IO_NONE)      // read/write csv
{
   clear();
}

/**
 * destructor.
 */
NeurolucidaFile::~NeurolucidaFile()
{
   clear();
}

/**
 * clear the file.
 */
void 
NeurolucidaFile::clear()
{
   AbstractFile::clearAbstractFile();
   setRootXmlElementTagName("mbf");
   contours.clear();
   markers.clear();
   markerColors.clear();
}

/**
 * get a contour.
 */
CaretContour* 
NeurolucidaFile::getContour(const int indx)
{
   if ((indx >= 0) &&
       (indx < getNumberOfContours())) {
      return &contours[indx];
   }
   
   return NULL;
}

/**
 * get a contour.
 */
const 
CaretContour* 
NeurolucidaFile::getContour(const int indx) const
{
   if ((indx >= 0) &&
       (indx < getNumberOfContours())) {
      return &contours[indx];
   }
   
   return NULL;
}
      
/**
 * get a marker.
 */
CellData* 
NeurolucidaFile::getMarker(const int indx)
{
   if ((indx >= 0) &&
       (indx < getNumberOfMarkers())) {
      return &markers[indx];
   }
   
   return NULL;
}

/**
 * get a marker.
 */
const CellData* 
NeurolucidaFile::getMarker(const int indx) const
{
   if ((indx >= 0) &&
       (indx < getNumberOfMarkers())) {
      return &markers[indx];
   }
   
   return NULL;
}

/**
 * parse the xml.
 */
void 
NeurolucidaFile::parseXML(QDomNode node) throw (FileException)
{
   if (node.hasChildNodes() == false) {
      const QString msg = "ERROR retrienving data (root element has no children for NeurolucidaFile).\n";
      throw FileException(msg);
   }
   
   if (DebugControl::getDebugOn()) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         std::cout << "Root element for NeurolucidaFile is: "
                   << elem.tagName().toAscii().constData()
                   << std::endl;
      }
   }
   
   //
   // Loop through children
   //
   node = node.firstChild();
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Element is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
         if (elem.tagName() == "contour") {
            processContourNode(node);
         }
         else if (elem.tagName() == "marker") {
            processMarkerNode(node);
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Number of contours in Neurolucida File: " 
                << getNumberOfContours() << std::endl;
   }
}
      
/**
 * process a contour node.
 */
void 
NeurolucidaFile::processContourNode(QDomNode contourNode) throw (FileException)
{
   //
   // Create a contour
   //
   CaretContour contour;
   bool sectionHasBeenSet = false;
      
   int sectionNumber = 0;
   
   //
   // Loop through children
   //
   QDomNode node = contourNode.firstChild();
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "contour child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         } 
               
         if (elem.tagName() == "point") {
            float x, y, z, diameter;
            QString sid;
            bool valid;
            processPointNode(node, x, y, z, diameter, sid, valid);
            if (valid) {
               contour.addPoint(x, y, z);
               
               if (sectionHasBeenSet == false) {
                  if (sid.isEmpty() == false) {
                     if (sid.startsWith("S")) {
                        const QString sectionString(sid.mid(1));
                        bool ok = false;
                        const int num = sectionString.toInt(&ok);
                        if (ok) {
                           sectionNumber = num;
                           sectionHasBeenSet = true;
                        }
                     }
                  }
                  else {
                     sectionNumber = static_cast<int>(z);
                     sectionHasBeenSet = true;
                  }
               }
            }
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
   
   contour.setSectionNumber(sectionNumber);
   
   if (contour.getNumberOfPoints() > 0) {
      contours.push_back(contour);
   }
}
      
/**
 * process the point node.
 */
void 
NeurolucidaFile::processPointNode(QDomNode node,
                                  float& xOut,
                                  float& yOut,
                                  float& zOut,
                                  float& diameterOut,
                                  QString& sidOut,
                                  bool& pointValidOut) throw (FileException)
{
   pointValidOut = false;
   sidOut = "";
   
   QDomElement elem = node.toElement();
   if (elem.isNull() == false) {
      const QString xAttr = elem.attribute("x");
      const QString yAttr = elem.attribute("y");
      const QString zAttr = elem.attribute("z");
      if ((xAttr.isEmpty() == false) &&
          (yAttr.isEmpty() == false) &&
          (zAttr.isEmpty() == false)) {
         xOut = xAttr.toFloat() * micronsToMillimeters;
         yOut = yAttr.toFloat() * micronsToMillimeters;
         zOut = zAttr.toFloat() * micronsToMillimeters;
         pointValidOut = true;
         
         const QString diameterAttr = elem.attribute("d");
         diameterOut = 1.0;
         if (diameterAttr.isEmpty() == false) {
            diameterOut = diameterAttr.toFloat();
         }
         
         sidOut = elem.attribute("sid", "");
      }
   }
}

/**
 * process the marker node.
 */
void 
NeurolucidaFile::processMarkerNode(QDomNode markerNode) throw (FileException)
{
   QString name;
   QString colorString;
   
   //
   // Process the childrent
   //
   QDomElement markerElement = markerNode.toElement();
   if (markerElement.isNull() == false) {
      name = markerElement.attribute("name");
      colorString = markerElement.attribute("color");
   }
   if (name.isEmpty()) {
      return;
   }
 
   //
   // Create the marker color
   //
   if (colorString.isEmpty() == false) {
      unsigned char red = 127, green = 127, blue = 127, alpha = 255;
      const float pointSize = 1.0;
      const float lineSize = 1.0;
      const ColorFile::ColorStorage::SYMBOL symbol = ColorFile::ColorStorage::SYMBOL_OPENGL_POINT;
      if (colorString[0] == '#') {
         if (colorString.size() >= 7) {
            bool ok = false;
            red   = static_cast<unsigned char>(colorString.mid(1, 2).toInt(&ok, 16));
            green = static_cast<unsigned char>(colorString.mid(3, 2).toInt(&ok, 16));
            blue  = static_cast<unsigned char>(colorString.mid(5, 2).toInt(&ok, 16));
         }
      }
      else {
         const bool valid = HtmlColors::getRgbFromColorName(colorString, red, green, blue);
         if (valid == false) {
            std::cout << "WARNING: Neurolucida file import - no matching color for "
                      << colorString.toAscii().constData() 
                      << std::endl;
         }
      }
      
      //
      // Create a new color with the marker name and specified color
      //
      ColorFile::ColorStorage cs(name, red, green, blue, alpha,
                                 pointSize, lineSize, symbol);
      markerColors.push_back(cs);
   }
   
   //
   // Loop through children
   //
   QDomNode node = markerNode.firstChild();
   while (node.isNull() == false) {
      //
      // Convert to element
      // 
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            std::cout << "marker child is: " 
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         } 
               
         if (elem.tagName() == "point") {
            float x, y, z, diameter;
            QString sid;
            bool valid;
            processPointNode(node, x, y, z, diameter, sid, valid);
            if (valid) {
               CellData cd(name, x, y, z);
               markers.push_back(cd);
            }
         }
      }
      
      //
      // next sibling
      //
      node = node.nextSibling();
   }
}
      
/**
 * get a marker color.
 */
ColorFile::ColorStorage*
NeurolucidaFile::getMarkerColor(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfMarkers())) {
      return &markerColors[indx];
   }
   return NULL;
}

/**
 * get a marker color.
 */
const ColorFile::ColorStorage*
NeurolucidaFile::getMarkerColor(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfMarkers())) {
      return &markerColors[indx];
   }
   return NULL;
}
      
/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
NeurolucidaFile::empty() const 
{
   const bool isEmpty = ((getNumberOfContours() == 0) &&
                         (getNumberOfMarkers() == 0) &&
                         (getNumberOfMarkerColors() == 0));

   return isEmpty;
}
      
/**
 * read the file.
 */
void 
NeurolucidaFile::readFileData(QFile& /* file */, 
                                QTextStream& /* stream */, 
                                QDataStream& /* binStream */,     
                                QDomElement&  rootElement) throw (FileException)
{
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         throw FileException("Reading of Ascii format Neurolucida files not supported..");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException("Reading of Binary format Neurolucida files not supported..");
         break;
      case FILE_FORMAT_XML:
         {
            QDomNode node = rootElement.firstChild();
            if (node.isNull() == false) {
               parseXML(rootElement);
            }
         }
         break;   
      case FILE_FORMAT_XML_BASE64:
         throw FileException("Reading of XML Base64 format Neurolucida files not supported..");
         break;   
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException("Reading of XMK GZip Base64 format Neurolucida files not supported..");
         break;   
      case FILE_FORMAT_OTHER:
         throw FileException("Reading of other format Neurolucida files not supported..");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException("Reading of Comma Separated Value File format Neurolucida files not supported..");
         break;
   }
}

/**
 * write the file.
 */
void 
NeurolucidaFile::writeFileData(QTextStream& /* stream */, 
                                 QDataStream& /* binStream */,
                                 QDomDocument& /* xmlDoc */,
                                 QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException("Writing of Neurolucida not supported.");
}
