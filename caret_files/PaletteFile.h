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

#ifndef __VE_PALETTE_FILE_H__
#define __VE_PALETTE_FILE_H__

#include "AbstractFile.h"

class ColorFile;

class PaletteFile;  // forward declaration

class Palette;

/// Class for storing a color in the palette.
class PaletteColor {
   private:
      /// rgb components of color
      unsigned char colorRGB[3];
      
      /// name of color
      QString colorName;
   
      /// palette file using this color
      PaletteFile* myPaletteFile;
      
      /// Set this palette color modified.
      void setModified();
      
   public:
      static const QString noneColorName;
      static const QString missingColorName;

      /// constructor
      PaletteColor(const QString& name, const unsigned char rgb[3]);
      
      /// get rgb color components
      void getRGB(unsigned char rgb[3]) const;
      
      /// get name of color
      QString getName() const;
      
      /// set rgb color components
      void setRGB(const unsigned char rgb[3]);
      
      /// set name of color
      void setName(const QString& name);
      
      /// color is the none color
      bool isNoneColor() const;
      
   friend class PaletteFile;
};

///
/// Class for storing an entry in the color palette.
///
class PaletteEntry {
   private:
      /// index of color
      int colorIndex;
      
      /// scalar value
      float  tableValue;
      
      /// Palette this entry is part of
      Palette* myPalette;
      
      /// set this palette entry modified
      void setModified();
      
   public:
      /// Constructor
      PaletteEntry(const float value, const int colorIndexIn = -1);
      
      /// Copy contructor
      PaletteEntry(const PaletteEntry& pe);
      
      /// get the index of the color
      int getColorIndex() const { return colorIndex; }
      
      /// set the color index
      void setColorIndex(const int indx) { colorIndex = indx; setModified(); }
      
      /// get the scalar value
      float getValue() const { return tableValue; }
      
      /// set the scalar value
      void setValue(const float f) { tableValue = f; setModified(); }
      
      friend class PaletteFile;
      friend class Palette;
};

/// Class for an AFNI palette.
class Palette {
   private:
      /// entries in the palette
      std::vector<PaletteEntry> paletteEntries;
      
      /// name of the palette
      QString paletteName;
      
      /// palette is positive only
      bool positiveOnly;
    
      /// PaletteFile this palette is part of
      PaletteFile* myPaletteFile;
      
   public:  
   
      /// Constructor
      Palette(PaletteFile* myPaletteFileIn);
      
      /// set the palette file
      void setPaletteFile(PaletteFile* myPaletteFileIn) 
             { myPaletteFile = myPaletteFileIn; }
             
      /// Copy constructor
      Palette(const Palette& p);
      
      /// palette has been modified
      void setModified();

      /// Add an entry to the palette
      void addPaletteEntry(const float tableScalar, const QString& colorName);
      
      /// insert a palette entry
      void insertPaletteEntry(const int afterPaletteEntryNumber,
                              const PaletteEntry& pe);
      
      /// remove a palette entry
      void removePaletteEntry(const int indx);
      
      /// get the appropriate color for a scalar value
      void getColor(const float scalar, 
                    const bool interpolateColorIn,
                    bool& noneColorFlagOut,
                    unsigned char colorOut[3]) const;
      
      /// Get the number of palette entries in this palette.
      int getNumberOfPaletteEntries() const { return paletteEntries.size(); }
      
      /// Get the name of this palette
      QString getName() const { return paletteName; }
      
      /// set the name of this palette
      void setName(const QString& name);
      
      /// Get an entry from this palette (const method)
      const PaletteEntry* getPaletteEntry(const int idx) const { return &paletteEntries[idx]; }
      
      /// Get an entry from this palette
      PaletteEntry* getPaletteEntry(const int idx) { return &paletteEntries[idx]; }
      
      /// Get the min and max values for this palette
      void getMinMax(float& minPalette, float& maxPalette) const;
      
      /// Get the positive only flag
      bool getPositiveOnly() const { return positiveOnly; }
      
      /// Set the positive only flag
      void setPositiveOnly(const bool posOnly);
      
      /// Write this palette to a file
      void writeFileData(QTextStream& stream) const throw (FileException);
      
   friend class paletteEntry;
   friend class PaletteFile;
};

/// This class represents an AFNI palette file.
class PaletteFile : public AbstractFile {
   private:
      /// name for gray interpolated palette
      static const QString grayInterpPaletteName;

      /// Colors from all palettes
      std::vector<PaletteColor> colors;

      /// storage for all palettes
      std::vector<Palette> palettes;

      /// convert an RGB format to X color format
      QString convertToXColor(unsigned char rgb[3]) const;

      /// get the color components for a color      
      int getColorComponents(const QString& colorName, 
                             bool& noneColorFlag,
                             unsigned char rgb[3]) const;
      
      /// Convert from X color format
      bool convertFromXColor(const char* colorString,
                               unsigned char rgb[3]) const;
                               
      /// Convert a hexadecimal number to base 10
      int hexToInt(char c) const;

      /// See if palette with specified name exists
      bool paletteNameExists(const QString& name);
      
      // read an PaletteFile
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);

      // write an PaletteFile
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
   public:
      /// constructor for the PaletteFile
      PaletteFile();

      /// destructor for the PaletteFile
      ~PaletteFile();

      /// add the default palettes to this palette file
      void addDefaultPalettes();

      /// add a palette to the palette file
      void addPalette(const Palette& pal);
      
      /// get the palette colors into a color file
      void getPaletteColorsUsingColorFile(ColorFile& cf) const;
      
      /// replace the palette colors using a ColorFile (update color assignments too)
      void replacePaletteColorsUsingColorFile(const ColorFile& cf);
      
      /// get the number of palette colors
      int getNumberOfPaletteColors() const { return colors.size(); }
      
      /// get a palette color
      PaletteColor* getPaletteColor(const int indx) { return &colors[indx]; }
      
      /// get a palette color (const method)
      const PaletteColor* getPaletteColor(const int indx) const { return &colors[indx]; }
      
      /// get a palette color index by name
      int getColorIndexFromName(const QString& name) const;
      
      /// add a color to the palette
      void addPaletteColor(const PaletteColor& pc);
      
      /// add a color to the palette file
      void addPaletteColor(const QString& name, const unsigned char rgb[3]);
      
      /// append a palette file to this one
      void append(PaletteFile& pf);
      
      /// clear memory
      void clear();

      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfPalettes() == 0); }
      
      /// get the number of palettes
      int getNumberOfPalettes() const { return palettes.size(); }
      
      /// get the index of the "gray interp" palette
      int getGrayInterPaletteIndex() const;
      
      /// get palette index from name or number (number ranges 1..N)
      int getPaletteIndexFromNameOrNumber(const QString& nameOrNumber) const throw (FileException);
      
      /// get a palette (const method)
      const Palette* getPalette(const int palNum) const { return &palettes[palNum]; }
      
      /// get a palette
      Palette* getPalette(const int palNum) { return &palettes[palNum]; }
      
      /// remove a palette
      void removePalette(const int palNum);

      /// write the file's memory in caret6 format to the specified name
      virtual QString writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException);

};

#endif // __VE_PALETTE_FILE_H__

#ifdef __PALETTE_FILE_DEFINED__
const QString PaletteColor::noneColorName = "none";
const QString PaletteColor::missingColorName = "missing";
const QString PaletteFile::grayInterpPaletteName = "Gray_Interp";
#endif // __PALETTE_FILE_DEFINED__

