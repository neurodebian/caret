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



#ifndef __VE_COLOR_FILE_H__
#define __VE_COLOR_FILE_H__

#include "AbstractFile.h"

class CommaSeparatedValueFile;
class QDomNode;

/// Color File
class ColorFile : public AbstractFile {
   public: 
      /// Class for color's name and RGB components
      class ColorStorage {
         public:
            /// symbol
            enum SYMBOL {
               /// box
               SYMBOL_BOX,
               /// diamond
               SYMBOL_DIAMOND,
               /// disk symbol
               SYMBOL_DISK,
               /// point
               SYMBOL_OPENGL_POINT,
               /// ring
               SYMBOL_RING,
               /// sphere
               SYMBOL_SPHERE,
               /// square
               SYMBOL_SQUARE,
               /// no symbol
               SYMBOL_NONE
            };
            
            // constructor
            ColorStorage();
                        
            // constructor
            ColorStorage(const QString& nameIn,
                         const unsigned char red, const unsigned char green, 
                         const unsigned char blue,
                         const float pointSizeIn, const float lineSizeIn);
                                     
            // constructor
            ColorStorage(const QString& nameIn,
                         const unsigned char red, const unsigned char green, 
                         const unsigned char blue, const unsigned char alpha,
                         const float pointSizeIn, const float lineSizeIn,
                         const SYMBOL symmbolIn = SYMBOL_OPENGL_POINT);
                                     
            // destructor
            virtual ~ColorStorage();
            
            // is the color selected
            bool getSelected() const;
            
            // set the selection status 
            void setSelected(const bool b);
            
            // get the symbol
            SYMBOL getSymbol() const;
            
            // set the symbol
            void setSymbol(const SYMBOL s);
            
            // get the name
            QString getName() const;
            
            // set the name
            void setName(const QString& n);
         
            // get the line size
            float getLineSize() const;
            
            // set the line size
            void setLineSize(const float s);
            
            // get the point size
            float getPointSize() const;
            
            // set the point size
            void setPointSize(const float s);
            
            // get rgb colors
            const unsigned char* getRgb() const;
            
            // get the rgb colors
            void getRgb(unsigned char& r,
                        unsigned char& g,
                        unsigned char& b) const;

            // set the rgb colors
            void setRgb(const unsigned char rgbIn[3]);
            
            // set the rgb colors
            void setRgb(const unsigned char r,
                        const unsigned char g,
                        const unsigned char b);

            // get rgba colors
            const unsigned char* getRgba() const;
            
            // get the rgba colors
            void getRgba(unsigned char& r,
                        unsigned char& g,
                        unsigned char& b,
                        unsigned char& a) const;

            // set the rgba colors
            void setRgba(const unsigned char rgbaIn[4]);
            
            // set the rgba colors
            void setRgba(const unsigned char r,
                        const unsigned char g,
                        const unsigned char b,
                        const unsigned char a);
            
            // called to read from an XML structure.
            void readXML(QDomNode& nodeIn) throw (FileException);

            // called to write to an XML structure.
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement);
                          
            // symbol enum to text
            static QString symbolToText(const SYMBOL s);
            
            // text to symbol enum
            static SYMBOL textToSymbol(const QString& s);
            
            // get all symbol types as strings
            static void getAllSymbolTypesAsStrings(std::vector<QString>& symbolStrings);

         protected:
            /// name of color
            QString name;
            
            /// color components
            unsigned char rgba[4];
            
            /// point size
            float pointSize;
            
            /// line size
            float lineSize;
            
            /// color selected
            bool selected;       
            
            /// the symbol
            SYMBOL symbol;
      };   
      
   protected:    
      /// Constructor
      ColorFile(const QString& descriptiveName,
                const QString& defaultExtensionIn);
      
      /// storage of colors
      std::vector<ColorStorage> colors;
      
      /// read the color file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write the color file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);

      // write the file's data into a comma separated values file (throws exception if not supported)
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      // read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);
      
   public:
      /// Destructor
      virtual ~ColorFile();
   
      /// add a color
      int addColor(const QString& name,
                   const unsigned char r, const unsigned char g, const unsigned char b,
                   const unsigned char alpha = 255,
                   const float pointSize = 2.0, const float lineSize = 1.0,
                   ColorStorage::SYMBOL symbol = ColorStorage::SYMBOL_OPENGL_POINT);
                   
      /// append a color file to this one
      void append(const ColorFile& cf);
      
      /// clear the color file
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfColors() == 0); }
      
      /// get the number of colors
      int getNumberOfColors() const { return colors.size(); }
      
      /// Get a color
      ColorStorage* getColor(const int indx) { return &colors[indx]; }

      /// Get a color (const method)
      const ColorStorage* getColor(const int indx) const { return &colors[indx]; }

      /// get color by index
      void getColorByIndex(const int index,
                           unsigned char& red, unsigned char& green, unsigned char& blue) const;
                        
      /// get color by index
      void getColorByIndex(const int index,
                           unsigned char& red, unsigned char& green, unsigned char& blue,
                           unsigned char& alpha) const;
                        
      /// get line and point size by index
      void getPointLineSizeByIndex(const int index,
                                   float& pointSize, float& lineSize) const;
      
      /// get the symbol by the color index
      ColorStorage::SYMBOL getSymbolByIndex(const int indx) const;
      
      /// get color name by index
      QString getColorNameByIndex(const int index) const;
                           
      /// get color index by name
      int getColorIndexByName(const QString& name, bool& exactMatch) const;

      /// get color by name
      int getColorByName(const QString& name,
                          bool& exactMatch,
                          unsigned char& red, unsigned char& green, unsigned char& blue) const;
      
      /// get color by name
      int getColorByName(const QString& name,
                          bool& exactMatch,
                          unsigned char& red, unsigned char& green, unsigned char& blue,
                          unsigned char& alpha) const;
      
      /// get color selected
      bool getSelected(const int index) const { return colors[index].getSelected(); }
      
      /// set color selected
      void setSelected(const int index, const bool sel) { colors[index].setSelected(sel); }
      
      /// remove color at specified index
      void removeColorByIndex(const int index);

      /// set status of all colors selected
      void setAllSelectedStatus(const bool sel);
      
      /// set color by index
      void setColorByIndex(const int index,
                           const QString& name,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue);
       
      /// set color by index
      void setColorByIndex(const int index,
                           const QString& name,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue, const unsigned char alpha);
       
      /// set color by index
      void setColorByIndex(const int index,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue);
       
      /// set color by index
      void setColorByIndex(const int index,
                           const  unsigned char red, const unsigned char green, 
                           const unsigned char blue, const unsigned char alpha);
       
      /// set line and point size by index
      void setPointLineSizeByIndex(const int index,
                                   const float pointSize, const float lineSize);
      
      /// set symbol by index
      void setSymbolByIndex(const int indx, const ColorStorage::SYMBOL s);
      
      /// set the number of colors 
      void setNumberOfColors(const int numColors);

      // find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;
      
};

#endif
