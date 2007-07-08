
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
#include <sstream>

#include <QRegExp>
#include <QStringList>

#include "DebugControl.h"
#include "FileUtilities.h"

#define __MDPLOT_FILE_MAIN__
#include "MDPlotFile.h"
#undef __MDPLOT_FILE_MAIN__

#include "SpecFile.h"

/**
 * get the color name.
 */
QString 
MDPlotColor::getColorName(const COLOR color) 
{
   if (colorsValid == false) {
      initializeColors();
   }
   
   if ((color >= 0) && (color < NUMBER_OF)) {
      return colorNames[color];
   }
   return "";
}

/**
 * get the color components (red, green, blue, alpha).
 */
const unsigned char* 
MDPlotColor::getColorComponents(const COLOR color) 
{
   static const unsigned char gray[3] = { 127, 127, 127 };
   
   if (colorsValid == false) {
      initializeColors();
   }

   if ((color >= 0) && (color < NUMBER_OF)) {
      return colors[color];
   }
   
   return gray;
}

/**
 * get the color components.
 */
void 
MDPlotColor::getColorComponents(const COLOR color,
                                unsigned char& red,
                                unsigned char& green,
                                unsigned char& blue) 
{
   if (colorsValid == false) {
      initializeColors();
   }

   if ((color >= 0) && (color < NUMBER_OF)) {
      red   = colors[color][0];
      green = colors[color][1];
      blue  = colors[color][2];
      return;
   }
   
   red   = 127;
   green = 127;
   blue  = 127;
}
                        
/**
 * get the color components (red, green, blue).
 */
void 
MDPlotColor::getColorComponents(const COLOR color,
                                unsigned char rgb[3]) 
{
   if (colorsValid == false) {
      initializeColors();
   }

   if ((color >= 0) && (color < NUMBER_OF)) {
      rgb[0] = colors[color][0];
      rgb[1] = colors[color][1];
      rgb[2] = colors[color][2];
      return;
   }
   
   rgb[0] = 127;
   rgb[1] = 127;
   rgb[2] = 127;
}

/**
 * initialize the colors.
 */
void 
MDPlotColor::initializeColors()
{
   colorNames[COLOR_DARK_RED] = "Red - Dark";
   colors[COLOR_DARK_RED][0] = 128;
   colors[COLOR_DARK_RED][1] = 0;
   colors[COLOR_DARK_RED][2] = 0;

   colorNames[COLOR_RED] = "Red";
   colors[COLOR_RED][0] = 255;
   colors[COLOR_RED][1] = 0;
   colors[COLOR_RED][2] = 0;

   colorNames[COLOR_DARK_GREEN] = "Green - Dark";
   colors[COLOR_DARK_GREEN][0] = 0;
   colors[COLOR_DARK_GREEN][1] = 128;
   colors[COLOR_DARK_GREEN][2] = 0;

   colorNames[COLOR_GREEN] = "Green";
   colors[COLOR_GREEN][0] = 0;
   colors[COLOR_GREEN][1] = 255;
   colors[COLOR_GREEN][2] = 0;

   colorNames[COLOR_DARK_BLUE] = "Blue - Dark";
   colors[COLOR_DARK_BLUE][0] = 0;
   colors[COLOR_DARK_BLUE][1] = 0;
   colors[COLOR_DARK_BLUE][2] = 128;

   colorNames[COLOR_BLUE] = "Blue";
   colors[COLOR_BLUE][0] = 0;
   colors[COLOR_BLUE][1] = 0;
   colors[COLOR_BLUE][2] = 255;

   colorNames[COLOR_DARK_MAGENTA] = "Magenta - Dark";
   colors[COLOR_DARK_MAGENTA][0] = 128;
   colors[COLOR_DARK_MAGENTA][1] = 0;
   colors[COLOR_DARK_MAGENTA][2] = 128;

   colorNames[COLOR_MAGENTA] = "Magenta";
   colors[COLOR_MAGENTA][0] = 255;
   colors[COLOR_MAGENTA][1] = 0;
   colors[COLOR_MAGENTA][2] = 255;

   colorNames[COLOR_DARK_YELLOW] = "Yellow - Dark";
   colors[COLOR_DARK_YELLOW][0] = 128;
   colors[COLOR_DARK_YELLOW][1] = 128;
   colors[COLOR_DARK_YELLOW][2] = 0;

   colorNames[COLOR_YELLOW] = "Yellow";
   colors[COLOR_YELLOW][0] = 255;
   colors[COLOR_YELLOW][1] = 255;
   colors[COLOR_YELLOW][2] = 0;

   colorNames[COLOR_DARK_CYAN] = "Cyan - Dark";
   colors[COLOR_DARK_CYAN][0] = 0;
   colors[COLOR_DARK_CYAN][1] = 128;
   colors[COLOR_DARK_CYAN][2] = 128;

   colorNames[COLOR_CYAN] = "Cyan";
   colors[COLOR_CYAN][0] = 0;
   colors[COLOR_CYAN][1] = 255;
   colors[COLOR_CYAN][2] = 255;

   colorNames[COLOR_BLACK] = "Black";
   colors[COLOR_BLACK][0] = 0;
   colors[COLOR_BLACK][1] = 0;
   colors[COLOR_BLACK][2] = 0;

   colorNames[COLOR_DARK_GRAY] = "Gray - Dark";
   colors[COLOR_DARK_GRAY][0] = 128;
   colors[COLOR_DARK_GRAY][1] = 128;
   colors[COLOR_DARK_GRAY][2] = 128;

   colorNames[COLOR_LIGHT_GRAY] = "Gray - Light";
   colors[COLOR_LIGHT_GRAY][0] = 192;
   colors[COLOR_LIGHT_GRAY][1] = 192;
   colors[COLOR_LIGHT_GRAY][2] = 192;

   colorNames[COLOR_WHITE] = "White";
   colors[COLOR_WHITE][0] = 255;
   colors[COLOR_WHITE][1] = 255;
   colors[COLOR_WHITE][2] = 255;

   colorsValid = true;
}
 
//*******************************************************************************************
//*******************************************************************************************
//*
//*
//*******************************************************************************************
//*******************************************************************************************
/**
 * constructor.
 */
MDPlotVertex::MDPlotVertex()
{
   mdplotFile = NULL;
   setXYZ(0.0, 0.0, 0.0);
}

/**
 * constructor.
 */
MDPlotVertex::MDPlotVertex(const float x, const float y, const float z)
{
   mdplotFile = NULL;
   setXYZ(x, y, z);
}

/**
 * copy helper.
 */
void 
MDPlotVertex::copyHelper(const MDPlotVertex& mv)
{
   xyz[0] = mv.xyz[0];
   xyz[1] = mv.xyz[1];
   xyz[2] = mv.xyz[2];
   mdplotFile = NULL;
}

/**
 * copy constructor.
 */
MDPlotVertex::MDPlotVertex(const MDPlotVertex& mv)
{
   copyHelper(mv);
}

/**
 * assignment operator.
 */
MDPlotVertex& 
MDPlotVertex::operator=(const MDPlotVertex& mv)
{
   if (this != &mv) {
      copyHelper(mv);
   }
   
   return *this;
}
      
/**
 * set the X/Y.
 */
void 
MDPlotVertex::setXYZ(const float x, const float y, const float z)
{
   xyz[0] = x;
   xyz[1] = y;
   xyz[2] = z;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the X/Y/Z.
 */
void 
MDPlotVertex::setXYZ(const float xyzIn[3])
{
   xyz[0] = xyzIn[0];
   xyz[1] = xyzIn[1];
   xyz[2] = xyzIn[2];
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * get the X/Y/Z.
 */
void 
MDPlotVertex::getXYZ(float& x, float& y, float& z) const
{
   x = xyz[0];
   y = xyz[1];
   z = xyz[2];
}

/**
 * get the X/Y.
 */
void 
MDPlotVertex::getXYZ(float xyzOut[3]) const
{
   xyzOut[0] = xyz[0];
   xyzOut[1] = xyz[1];
   xyzOut[2] = xyz[2];
}
      
/**
 * set the MDPlot File in which this is an item.
 */
void 
MDPlotVertex::setMDPlotFile(MDPlotFile* mdpf)
{
   mdplotFile = mdpf;
}         

//*******************************************************************************************
//*******************************************************************************************
//*
//*
//*******************************************************************************************
//*******************************************************************************************
/**
 * constructor.
 */
MDPlotPoint::MDPlotPoint(const int vertexIn,
                         const SYMBOL symbolIn,
                         const float sizeIn,
                         const MDPlotColor::COLOR colorIn)
{
   mdplotFile = NULL;
   setVertex(vertexIn);
   setSymbol(symbolIn);
   setSize(sizeIn);
   setColor(colorIn);
}

/**
 * constructor.
 */
MDPlotPoint::MDPlotPoint()
{
   mdplotFile = NULL;
}
      
/**
 * destructor.
 */
MDPlotPoint::~MDPlotPoint()
{
}

/**
 * copy helper.
 */
void 
MDPlotPoint::copyHelper(const MDPlotPoint& mp)
{
   symbol = mp.symbol;
   color  = mp.color;
   vertexIndex = mp.vertexIndex;
   mdplotFile = NULL;
}

/**
 * copy constructor.
 */
MDPlotPoint::MDPlotPoint(const MDPlotPoint& mp)
{
   copyHelper(mp);
}

/**
 * assignment operator.
 */
MDPlotPoint& 
MDPlotPoint::operator=(const MDPlotPoint& mp)
{
   if (this != &mp) {
      copyHelper(mp);
   }
   
   return *this;
}
      
/**
 * set the symbol.
 */
void 
MDPlotPoint::setSymbol(const SYMBOL symbolIn)
{
   symbol = symbolIn;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the size.
 */
void 
MDPlotPoint::setSize(const float sizeIn)
{
   size = sizeIn;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the color.
 */
void 
MDPlotPoint::setColor(const MDPlotColor::COLOR colorIn)
{
   color = colorIn;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the vertex index.
 */
void 
MDPlotPoint::setVertex(const int vertexIn)
{
   vertexIndex = vertexIn;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the MDPlot File in which this is an item.
 */
void 
MDPlotPoint::setMDPlotFile(MDPlotFile* mdpf)
{
   mdplotFile = mdpf;
}         

//*******************************************************************************************
//*******************************************************************************************
//*
//*
//*******************************************************************************************
//*******************************************************************************************

/**
 * constructor.
 */
MDPlotLine::MDPlotLine()
{
   mdplotFile = NULL;
   setStyle(STYLE_SOLID);
   setColor(MDPlotColor::COLOR_RED);
   setWidth(1);
}

/**
 * constructor.
 */
MDPlotLine::MDPlotLine(const std::vector<int> verticesIn,
                       const STYLE styleIn,
                       MDPlotColor::COLOR colorIn,
                       const float widthIn)
{
   mdplotFile = NULL;
   vertices = verticesIn;
   setStyle(styleIn);
   setColor(colorIn);
   setWidth(widthIn);
}

/**
 * destructor.
 */
MDPlotLine::~MDPlotLine()
{
}

/**
 * copy constructor.
 */
MDPlotLine::MDPlotLine(const MDPlotLine& ml)
{
   copyHelper(ml);
}

/**
 * assignment operator.
 */
MDPlotLine& 
MDPlotLine::operator=(const MDPlotLine& ml)
{
   if (this != &ml) {
      copyHelper(ml);
   }
   
   return *this;
}

/**
 * copy helper.
 */
void 
MDPlotLine::copyHelper(const MDPlotLine& ml)
{
   style = ml.style;
   width = ml.width;
   color = ml.color;
   vertices = ml.vertices;
   mdplotFile = NULL;
}
      
/**
 * set the style.
 */
void 
MDPlotLine::setStyle(const STYLE styleIn)
{
   style = styleIn;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the width.
 */
void 
MDPlotLine::setWidth(const float widthIn)
{
   width = widthIn;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the color.
 */
void 
MDPlotLine::setColor(const MDPlotColor::COLOR colorIn)
{
   color = colorIn;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set a vertex index.
 */
void 
MDPlotLine::setVertexIndex(const int vertexNum, const int value)
{
   if ((vertexNum < 0) ||
       (vertexNum >= getNumberOfVertices())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " : Invalid vertex index in MDPlot::setVertexIndex(): " 
                << vertexNum << std::endl;
      return;
   }
   vertices[vertexNum] = value;
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * get a vertex index.
 */
int 
MDPlotLine::getVertexIndex(const int vertexNum) const 
{ 
   if ((vertexNum < 0) ||
       (vertexNum >= getNumberOfVertices())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid vertex index in MDPlot::getVertexIndex(): " 
                << vertexNum << std::endl;
      return -1;
   }

   return vertices[vertexNum]; 
}
      
/**
 * add a vertex index.
 */
void 
MDPlotLine::addVertexIndex(const int value)
{
   vertices.push_back(value);
   
   if (mdplotFile != NULL) {
      mdplotFile->setModified();
   }
}

/**
 * set the MDPlot File in which this is an item.
 */
void 
MDPlotLine::setMDPlotFile(MDPlotFile* mdpf)
{
   mdplotFile = mdpf;
}         

//*******************************************************************************************
//*******************************************************************************************
//*
//*
//*******************************************************************************************
//*******************************************************************************************

/**
 * constructor.
 */
MDPlotFile::MDPlotFile()
   : AbstractFile("MDPlot File",
                  SpecFile::getMDPlotFileExtension(),
                  false,
                  FILE_FORMAT_ASCII,
                  FILE_IO_READ_AND_WRITE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE)
{
   clear();
}

/**
 * destructor.
 */
MDPlotFile::~MDPlotFile()
{
   clear();
}

/**
 * clear the file.
 */
void 
MDPlotFile::clear()
{
   clearAbstractFile();
   
   vertices.clear();
   points.clear();
   lines.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
MDPlotFile::empty() const
{
   return (vertices.empty() &&
           points.empty() &&
           lines.empty());
}

/**
 * add a vertex.
 */
void 
MDPlotFile::addVertex(const MDPlotVertex& mv)
{
   vertices.push_back(mv);
   vertices[vertices.size() - 1].setMDPlotFile(this);
   setModified();
}

/**
 * get a vertex.
 */
MDPlotVertex* 
MDPlotFile::getVertex(const int indx) 
{ 
   if ((indx < 0) ||
       (indx >= getNumberOfVertices())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid index sent to MDPlotFile::getVertex(): "
                << indx << std::endl;
      return NULL;
   }
   return &vertices[indx]; 
}

/**
 * get a vertex.
 */
const MDPlotVertex* 
MDPlotFile::getVertex(const int indx) const 
{ 
   if ((indx < 0) ||
       (indx >= getNumberOfVertices())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid index sent to MDPlotFile::getVertex(): "
                << indx << std::endl;
      return NULL;
   }
   return &vertices[indx]; 
}
      
/**
 * add a point.
 */
void 
MDPlotFile::addPoint(const MDPlotPoint& mp)
{
   points.push_back(mp);
   points[points.size() - 1].setMDPlotFile(this);
   setModified();
}

/**
 * get a point.
 */
MDPlotPoint* 
MDPlotFile::getPoint(const int indx) 
{ 
   if ((indx < 0) ||
       (indx >= getNumberOfPoints())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid index sent to MDPlotFile::getPoint(): "
                << indx << std::endl;
      return NULL;
   }
   return &points[indx]; 
}

/**
 * get a point.
 */
const MDPlotPoint* 
MDPlotFile::getPoint(const int indx) const 
{ 
   if ((indx < 0) ||
       (indx >= getNumberOfPoints())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid index sent to MDPlotFile::getPoint(): "
                << indx << std::endl;
      return NULL;
   }
   return &points[indx]; 
}
      
/**
 * add a line.
 */
void 
MDPlotFile::addLine(const MDPlotLine& ml)
{
   lines.push_back(ml);
   lines[lines.size() - 1].setMDPlotFile(this);
   setModified();
}

/**
 * get a line.
 */
MDPlotLine* 
MDPlotFile::getLine(const int indx) 
{ 
   if ((indx < 0) ||
       (indx >= getNumberOfLines())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid index sent to MDPlotFile::getLine(): "
                << indx << std::endl;
      return NULL;
   }
   return &lines[indx]; 
}

/**
 * get a line.
 */
const MDPlotLine* 
MDPlotFile::getLine(const int indx) const 
{ 
   if ((indx < 0) ||
       (indx >= getNumberOfLines())) {
      std::cout << "PROGRAM ERROR line " << __LINE__ << " file " << __FILE__ << " :: Invalid index sent to MDPlotFile::getLine(): "
                << indx << std::endl;
      return NULL;
   }
   return &lines[indx]; 
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
MDPlotFile::readFileData(QFile& /*file*/,
                         QTextStream& stream,
                         QDataStream& /* binStream */,
                         QDomElement& /* rootElement */) throw (FileException)
{
   const QRegExp splitRegExp("\\s");
   
   int vertexOffset = 0;
   float zValue = 0.0;
   
   while (stream.atEnd() == false) {
      QString line = stream.readLine().trimmed();
      
      if (DebugControl::getDebugOn()) {
         //std::cout << "Line : " << line.toAscii().constData() << std::endl;
      }
      QString lineClean = line.section('#', 0, 0);
      if (lineClean.isNull() == false) {
         if (DebugControl::getDebugOn()) {
            //std::cout << "Clean: " << lineClean.toAscii().constData() << std::endl;
         }
      
         //
         // Is this line continued to the next line
         //
         if (lineClean.endsWith("\\")) {
            lineClean.truncate(lineClean.length() - 1);
            
            bool done = false;
            while (done == false) {
               done = true;
               
               if (stream.atEnd() == false) {
                  QString line2 = stream.readLine().trimmed();
                  QString line2Clean = line2.section('#', 0, 0);
                  if (line2Clean.isNull()) {
                     done = true;
                  }
                  else if (line2Clean.isEmpty()) {
                     done = true;
                  }
                  else {
                     if (line2Clean.endsWith("\\")) {
                        line2Clean.truncate(line2Clean.length() - 1);
                        done = false;
                     }
                     lineClean += " ";
                     lineClean += line2Clean;
                  }
               }
            }
         }
         
         //QStringList sl = QStringList::split(splitRegExp, lineClean);
         QStringList sl = lineClean.split(splitRegExp, QString::SkipEmptyParts);
         if (sl.count() > 1) {
            const QString key(sl.at(0));
      
            if (key =="SECTION") {
               if (DebugControl::getDebugOn()) {
                  std::cout << "Section: " << (sl.at(1).toAscii().constData()) << std::endl;
               }
               //
               // Each section has its vertice indices start at zero (I believe)
               //
               vertexOffset = getNumberOfVertices();
               if (DebugControl::getDebugOn()) {
                  std::cout << "Vertex Offset: " << vertexOffset << std::endl;
               }
               zValue = 0.0;
            }
            else if (lineClean.startsWith("ZVALUE")) {
               zValue = (sl.at(1)).toInt();
               if (DebugControl::getDebugOn()) {
                  std::cout << "ZVALUE: " << zValue << std::endl;
               }
            }
            else if (key == "V") {
               if (sl.count() >= 3) {
                  const float x = (sl.at(1)).toFloat();
                  const float y = (sl.at(2)).toFloat();
                  MDPlotVertex v(x, y, zValue);
                  addVertex(v);
               }
               else {
                  std::ostringstream str;
                  str << "MDPlot File Invalid vertex: " 
                      << line.toAscii().constData();
                  throw FileException(filename, str.str().c_str());
               }
            }
            else if (key == "P") {
               if (sl.count() >= 5) {
                  const int pointType   = (sl.at(1)).toInt();
                  const int pointSize   = (sl.at(2)).toInt();
                  const int pointColor  = (sl.at(3)).toInt();
                  int pointVertex = (sl.at(4)).toInt() + vertexOffset - 1;  // we index at 0
                  if ((pointVertex < 0) ||
                      (pointVertex >= getNumberOfVertices())) {
                     std::cout << "MDPlot File: Point (" << sl.join(" ").toAscii().constData()
                               << ") has invalid vertex index.  Total=" 
                               << getNumberOfVertices() << std::endl;
                  }
                  else {
                     MDPlotPoint p(pointVertex, 
                                   static_cast<MDPlotPoint::SYMBOL>(pointType), 
                                   pointSize, 
                                   static_cast<MDPlotColor::COLOR>(pointColor));
                     addPoint(p);
                  }
               }
               else {
                  std::ostringstream str;
                  str << "MDPlot File Invalid vertex: " 
                      << line.toAscii().constData();
                  throw FileException(filename, str.str().c_str());
               }
            }
            else if (key == "L") {
               if (sl.count() >= 5) {
                  if (DebugControl::getDebugOn()) {
                     std::cout << "LINE DATA: " << sl.join(" ").toAscii().constData() << std::endl;
                  }
                  const int lineType    = (sl.at(1)).toInt();
                  const int lineColor   = (sl.at(2)).toInt();
                  const float lineWidth = (sl.at(3)).toFloat();

                  std::vector<int> verts;
                  const int numItems = sl.count();
                  for (int m = 4; m < numItems; m++) {
                     const QString s(sl.at(m));
                     if (DebugControl::getDebugOn()) {
                        std::cout << "MDPlot Line Element (" << m << ") "
                                  << s.toAscii().constData() << std::endl;
                     }
                     int lineVertex = s.toInt() + vertexOffset - 1;  // we index at zero
                     if ((lineVertex < 0) ||
                         (lineVertex >= getNumberOfVertices())) {
                        std::cout << "MDPlot::readFileData() line has invalid vertex index=" << lineVertex 
                                  << "  Num-Vertices=" << getNumberOfVertices() << std::endl;
                     }
                     else {
                        verts.push_back(lineVertex);
                     }
                  }

                  MDPlotLine theLine(verts, 
                                     static_cast<MDPlotLine::STYLE>(lineType), 
                                     static_cast<MDPlotColor::COLOR>(lineColor),
                                     lineWidth);
                  addLine(theLine);
               }
               else {
                  std::ostringstream str;
                  str << "MDPlot File Invalid line: " 
                      << line.toAscii().constData();
                  throw FileException(filename, str.str().c_str());
               }
            }
            else if (key.isEmpty() == false) {
               if (DebugControl::getDebugOn()) {
                  std::cout << "MDPlot INFO: ignoring line with key: " << key.toAscii().constData() << std::endl;
               }
            }
         }
      }
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "MDPlot File: " << FileUtilities::basename(getFileName()).toAscii().constData() << std::endl;
      std::cout << "Number of Vertices: " << getNumberOfVertices() << std::endl;
      std::cout << "Number of Points: " << getNumberOfPoints() << std::endl;
      std::cout << "Number of Lines: " << getNumberOfLines() << std::endl;
      std::cout << std::endl;
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
MDPlotFile::writeFileData(QTextStream& /* stream */,
                          QDataStream& /* binStream */,
                          QDomDocument& /* xmlDoc */,
                          QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, "Writing of MD Plot files not supported.");
}
