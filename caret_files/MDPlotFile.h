#ifndef __MDPLOT_FILE_H__
#define __MDPLOT_FILE_H__

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

#include "AbstractFile.h"

class MDPlotFile;

/// class for MDPlot Colors
class MDPlotColor {
   public:
      /// MDPlot Colors
      enum COLOR {
         /// dark red color
         COLOR_DARK_RED = 0,
         /// red color
         COLOR_RED = 1,
         /// dark green color
         COLOR_DARK_GREEN = 2,
         /// green color
         COLOR_GREEN = 3,
         /// dark blue color
         COLOR_DARK_BLUE = 4,
         /// blue color
         COLOR_BLUE = 5,
         /// dark magenta color
         COLOR_DARK_MAGENTA = 6,
         /// magenta color
         COLOR_MAGENTA = 7,
         /// dark yellow color
         COLOR_DARK_YELLOW = 8,
         /// yellow color
         COLOR_YELLOW = 9,
         /// dark cyan color
         COLOR_DARK_CYAN = 10,
         /// cyan color
         COLOR_CYAN = 11,
         /// black color
         COLOR_BLACK = 12,
         /// dark gray color
         COLOR_DARK_GRAY = 13,
         /// light gray color
         COLOR_LIGHT_GRAY = 14,
         /// white color
         COLOR_WHITE = 15,
         /// number of colors
         NUMBER_OF = 16
      };
      
      /// get the number of colors
      static int getNumberOfColors() { return NUMBER_OF; }
      
      /// get the color name
      static QString getColorName(const COLOR color);
      
      /// get the color components (red, green, blue, alpha)
      static const unsigned char* getColorComponents(const COLOR color);
      
      /// get the color components
      static void getColorComponents(const COLOR color,
                              unsigned char& red,
                              unsigned char& green,
                              unsigned char& blue);
                              
      /// get the color components (red, green, blue)
      static void getColorComponents(const COLOR color,
                              unsigned char rgb[3]);
                              
   protected:
      /// initialize the colors
      static void initializeColors();
      
      /// the actual colors
      static unsigned char colors[NUMBER_OF][3];
      
      /// the color names
      static QString colorNames[NUMBER_OF];
      
      /// colors valid flag
      static bool colorsValid;
};

#ifdef __MDPLOT_FILE_MAIN__
unsigned char MDPlotColor::colors[MDPlotColor::NUMBER_OF][3];
QString MDPlotColor::colorNames[MDPlotColor::NUMBER_OF];
bool MDPlotColor::colorsValid = false;
#endif // __MDPLOT_FILE_MAIN__

/// class for an MDPlot Vertex
class MDPlotVertex {
   public:
      // constructor
      MDPlotVertex();
      
      // constructor
      MDPlotVertex(const float x, const float y, const float z);
      
      // copy constructor
      MDPlotVertex(const MDPlotVertex& mv);
      
      // assignment operator
      MDPlotVertex& operator=(const MDPlotVertex& mv);
      
      // set the X/Y
      void setXYZ(const float x, const float y, const float z);
      
      // set the X/Y/Z
      void setXYZ(const float xyzIn[3]);
      
      // get the X/Y/Z
      const float* getXYZ() const { return xyz; }
      
      // get the X/Y/Z
      void getXYZ(float& x, float& y, float& z) const;
      
      // get the X/Y
      void getXYZ(float xyOut[3]) const;

      // set the MDPlot File in which this is an item
      void setMDPlotFile(MDPlotFile* mdpf);
         
   protected:
      /// copy helper
      void copyHelper(const MDPlotVertex& mv);
      
      /// the X, Y, & Z
      float xyz[3];
      
      /// MDPlot File in which this item is a member
      MDPlotFile* mdplotFile;
};

/// class for an MDPlot Point
class MDPlotPoint {
   public:
      /// point symbols
      enum SYMBOL {
         /// plus symbol
         SYMBOL_PLUS,
         /// X symbol
         SYMBOL_X,
         /// square symbol
         SYMBOL_SQUARE,
         /// circle symbol
         SYMBOL_CIRCLE,
         /// filled circle symbol
         SYMBOL_CIRCLE_FILLED,
         /// asterisk symbol
         SYMBOL_ASTERISK,
         /// dot symbol
         SYMBOL_DOT,
         /// filled square symbol
         SYMBOL_SQUARE_FILLED,
         /// triangle symbol
         SYMBOL_TRIANGLE,
         /// filled triangle symbol
         SYMBOL_TRIANGLE_FILLED
      };
      
      // constructor
      MDPlotPoint(const int vertexIn,
                  const SYMBOL symbolIn = SYMBOL_SQUARE,
                  const float sizeIn = 1,
                  const MDPlotColor::COLOR colorIn = MDPlotColor::COLOR_GREEN);
      
      // constructor
      MDPlotPoint();
      
      // destructor
      ~MDPlotPoint();
      
      // copy constructor
      MDPlotPoint(const MDPlotPoint& mv);
      
      // assignment operator
      MDPlotPoint& operator=(const MDPlotPoint& mv);
      
      /// get the symbol
      SYMBOL getSymbol() const { return symbol; }
      
      // set the symbol
      void setSymbol(const SYMBOL symbolIn);
      
      /// get the size
      float getSize() const { return size; }
      
      // set the size
      void setSize(const float sizeIn);
      
      /// get the color
      MDPlotColor::COLOR getColor() const { return color; }
      
      // set the color
      void setColor(const MDPlotColor::COLOR colorIn);
      
      /// get the vertex index
      int getVertex() const { return vertexIndex; }
      
      // set the vertex index
      void setVertex(const int vertexIn);
      
      // set the MDPlot File in which this is an item
      void setMDPlotFile(MDPlotFile* mdpf);
      
   protected:
      /// copy helper
      void copyHelper(const MDPlotPoint& mv);
      
      /// the symbol
      SYMBOL symbol;
      
      /// the size
      float size;
      
      /// the color
      MDPlotColor::COLOR color;
      
      /// the vertex index
      int vertexIndex;
            
      /// MDPlot File in which this item is a member
      MDPlotFile* mdplotFile;
};

/// class for an MDPlot line
class MDPlotLine {
   public:
      /// line style
      enum STYLE {
         /// solid style
         STYLE_SOLID,
         /// dashed style
         STYLE_DASHED,
         /// dotted style
         STYLE_DOTTED,
         /// dash-dot style
         STYLE_DASH_DOT,
         /// dash-dot-dot style
         STYLE_DASH_DOT_DOT
      };
      
      // constructor
      MDPlotLine();
      
      // constructor
      MDPlotLine(const std::vector<int> verticesIn,
                 const STYLE styleIn = STYLE_SOLID,
                 MDPlotColor::COLOR colorIn = MDPlotColor::COLOR_RED,
                 const float widthIn = 1);
      
      // destructor
      ~MDPlotLine();
      
      // copy constructor
      MDPlotLine(const MDPlotLine& mv);
      
      // assignment operator
      MDPlotLine& operator=(const MDPlotLine& mv);
      
      /// get the style
      STYLE getStyle() const { return style; }
      
      // set the style
      void setStyle(const STYLE styleIn);
      
      /// get the width
      float getWidth() const { return width; }
      
      // set the width
      void setWidth(const float widthIn);
      
      /// get the color
      MDPlotColor::COLOR getColor() const { return color; }
      
      // set the color
      void setColor(const MDPlotColor::COLOR colorIn);
      
      /// get the vertices
      int getNumberOfVertices() const { return vertices.size(); }
      
      /// get a vertex index
      int getVertexIndex(const int vertexNum) const;
      
      // set a vertex index
      void setVertexIndex(const int vertexNum, const int value);
      
      // add a vertex index
      void addVertexIndex(const int value);
      
      // set the MDPlot File in which this is an item
      void setMDPlotFile(MDPlotFile* mdpf);
      
   protected:
      /// copy helper
      void copyHelper(const MDPlotLine& mv);
      
      /// the style
      STYLE style;
      
      /// the color
      MDPlotColor::COLOR color;
      
      /// the width
      float width;
      
      /// vertices in the line
      std::vector<int> vertices;
      
      /// MDPlot File in which this item is a member
      MDPlotFile* mdplotFile;
};

/// class for reading and writing an MDPlot file
class MDPlotFile : public AbstractFile {
   public:
      // constructor
      MDPlotFile();
      
      // destructor
      ~MDPlotFile();
      
      /// clear the file
      void clear();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const;

      /// get the number of vertices
      int getNumberOfVertices() const { return vertices.size(); }
      
      /// get a vertex
      MDPlotVertex* getVertex(const int indx);
      
      /// get a vertex
      const MDPlotVertex* getVertex(const int indx) const;
      
      // add a vertex
      void addVertex(const MDPlotVertex& mv);
      
      /// get the number of points
      int getNumberOfPoints() const { return points.size(); }
      
      /// get a point
      MDPlotPoint* getPoint(const int indx);
      
      /// get a point
      const MDPlotPoint* getPoint(const int indx) const;
      
      // add a point
      void addPoint(const MDPlotPoint& mp);
      
      /// get the number of lines
      int getNumberOfLines() const { return lines.size(); }
      
      /// get a line
      MDPlotLine* getLine(const int indx);
      
      /// get a line
      const MDPlotLine* getLine(const int indx) const;
      
      // add a line
      void addLine(const MDPlotLine& ml);
      
   protected:
      /// Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                        QDomElement& rootElement) throw (FileException);

      /// Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
      
      /// the vertices
      std::vector<MDPlotVertex> vertices;
      
      /// the points
      std::vector<MDPlotPoint> points;
      
      /// the lines
      std::vector<MDPlotLine> lines;
};

#endif // __MDPLOT_FILE_H__
