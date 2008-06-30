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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "ColorFile.h"
#include "FileUtilities.h"
#define __PALETTE_FILE_DEFINED__
#include "PaletteFile.h"
#undef  __PALETTE_FILE_DEFINED__
#include "SpecFile.h"

// 
// Example of a Palette File's contents:
//
// ***COLORS
//   yellow = yellow
//   oran-yell = #ff9900
//   oran-red = #ff4400
//   dk-blue = rgbi:0.0/0.0/1.0
//   lt-blue1 = #0069ff
//   blue-cyan = #00ccff
// 
// ***PALETTES palette7 [7]
//   1.000000 -> yellow
//   0.660000 -> oran-yell
//   0.330000 -> oran-red
//   0.050000 -> none
//   -0.050000 -> dk-blue
//   -0.398438 -> lt-blue1
//   -0.660000 -> blue-cyan
//
//---------------------------------------------------------------------------
/**
 * The constructor.
 */
PaletteColor::PaletteColor(const QString& name, const unsigned char rgb[3])
{
   myPaletteFile = NULL;
   setName(name);
   setRGB(rgb);
}

/**
 * Set this palette color modified.
 */
void
PaletteColor::setModified()
{
   if (myPaletteFile != NULL) {
      myPaletteFile->setModified();
   }
}

/**
 * Get the RGB components.
 */
void
PaletteColor::getRGB(unsigned char rgb[3]) const
{
   rgb[0] = colorRGB[0];
   rgb[1] = colorRGB[1];
   rgb[2] = colorRGB[2];
}

/**
 * Get the name.
 */
QString
PaletteColor::getName() const
{
   return colorName;
}

/**
 * set the RGB components.
 */
void
PaletteColor::setRGB(const unsigned char rgb[3])
{
   colorRGB[0] = rgb[0];
   colorRGB[1] = rgb[1];
   colorRGB[2] = rgb[2];
   setModified();
}

/**
 * color is the none color.
 */
bool 
PaletteColor::isNoneColor() const
{
   return (colorName == noneColorName);
}

/**
 * set the name.
 */
void
PaletteColor::setName(const QString& name)
{
   colorName = name;
   setModified();
}

//---------------------------------------------------------------------------
//
// Palette Entry Class Methods
//
/**
 * The constructor.
 */
PaletteEntry::PaletteEntry(const float value, const int colorIndexIn)
{
   tableValue = value;
   colorIndex = colorIndexIn;
   myPalette = NULL;
}

/**
 * Copy contructor.
 */
PaletteEntry::PaletteEntry(const PaletteEntry& pe)
{
   tableValue = pe.tableValue;
   colorIndex = pe.colorIndex;
   myPalette = NULL;
}
      
/**
 * set this palette entry modified.
 */
void 
PaletteEntry::setModified()
{
   if (myPalette != NULL) {
      myPalette->setModified();
   }
}

//---------------------------------------------------------------------------
//
// Palette methods
//
/**
 * Palette Constructor
 */
Palette::Palette(PaletteFile* myPaletteFileIn)
{
   myPaletteFile = myPaletteFileIn;
   positiveOnly = false;
}

/**
 * Copy constructor.
 */
Palette::Palette(const Palette& p)
{
   paletteEntries = p.paletteEntries;
   for (int i = 0; i < static_cast<int>(paletteEntries.size()); i++) {
      paletteEntries[i].myPalette = this;
   }
   paletteName = p.paletteName;
   positiveOnly = p.positiveOnly;
   myPaletteFile = p.myPaletteFile;
}
      
/**
 * palette has been modified.
 */
void 
Palette::setModified()
{
   if (myPaletteFile != NULL) {
      myPaletteFile->setModified();
   }
}

/**
 * Set the name of this palette.
 */
void 
Palette::setName(const QString& name)
{
   paletteName = name;
   setModified();
}

/**
 * Set the positive only flag for this palette.
 */
void 
Palette::setPositiveOnly(const bool posOnly) 
{ 
   positiveOnly = posOnly;
   setModified();
}
      
/**
 * Return the minimum and maximum values for the palette
 */
void 
Palette::getMinMax(float& minPalette, float& maxPalette) const
{
   if (paletteEntries.size() > 0) {
      maxPalette = paletteEntries[0].tableValue;
      minPalette = paletteEntries[paletteEntries.size() - 1].tableValue;
   }
   else {
      minPalette = 0.0;
      maxPalette = 0.0;
   }
}      

/**
 * Add a Palette entry to the palette.
 */
void 
Palette::addPaletteEntry(const float tableScalar, const QString& colorName)
{
   int colorIndex = -1;
   if (colorName.isEmpty() == false) {
      if (myPaletteFile != NULL) {
         colorIndex = myPaletteFile->getColorIndexFromName(colorName);
      }
   }
   if (colorIndex < 0) {
      if (colorName == "none") {
         const unsigned char none[3] = { 0xff, 0xff, 0xff };
         myPaletteFile->addPaletteColor(PaletteColor("none", none));
         colorIndex = myPaletteFile->getColorIndexFromName(colorName);
      }
      else {
         std::cout << "PALETTE FILE ERROR: color \"" << colorName.toAscii().constData() 
                   << "\" not found for palette \""  
                   << getName().toAscii().constData() << "\"" << std::endl;
         return; 
      }
   }
   PaletteEntry pe(tableScalar, colorIndex);
   paletteEntries.push_back(pe);
   const int num = getNumberOfPaletteEntries() - 1;
   paletteEntries[num].myPalette = this;
   setModified();
}

/**
 * insert a palette entry.
 */
void 
Palette::insertPaletteEntry(const int afterPaletteEntryNumber,
                            const PaletteEntry& pe)
{
   paletteEntries.insert(paletteEntries.begin() + afterPaletteEntryNumber, pe);
   paletteEntries[afterPaletteEntryNumber + 1].myPalette = this;
   setModified();
}

/**
 * remove a palette entry.
 */
void 
Palette::removePaletteEntry(const int indx)
{
   paletteEntries.erase(paletteEntries.begin() + indx);
   setModified();
}
      
/** 
 * The Palette colors are in a range -1.0 to 1.0.  "scalar" must be within
 *  the inclusive range -1.0 to 1.0
 */
void
Palette::getColor(const float scalar, 
                  const bool interpolateColorIn,
                  bool& noneColorFlagOut,
                  unsigned char colorOut[3]) const
{
   colorOut[0] = 0;
   colorOut[1] = 0;
   colorOut[2] = 0;

   if (paletteEntries.size() <= 0) return;
   if (myPaletteFile == NULL) return;
   
   bool interpolateColor = interpolateColorIn;
   if (paletteEntries.size() == 1) {
      interpolateColor = false;
   }
   int paletteIndex = -1;
   if (scalar >= paletteEntries[0].tableValue) {
      paletteIndex = 0;
      interpolateColor = false;
   }
   
   const int lastPalette = paletteEntries.size() - 1;
   if (scalar <= paletteEntries[lastPalette].tableValue) {
      paletteIndex = lastPalette;
      interpolateColor = false;
   }
   
   for (unsigned int i = 1; i < paletteEntries.size(); i++) {
      const PaletteEntry& pe = paletteEntries[i];
      if (scalar > pe.tableValue) {
         paletteIndex = i - 1;
         break;
      }
   }
   if (paletteIndex >= 0) {
      const int colorIndex = paletteEntries[paletteIndex].getColorIndex();
      if ((colorIndex < 0) && (colorIndex >= myPaletteFile->getNumberOfPaletteColors())) {
         return;
      }
      const PaletteColor* pc = myPaletteFile->getPaletteColor(colorIndex);
      
      noneColorFlagOut = pc->isNoneColor();
      
      if (noneColorFlagOut == false) {
         unsigned char rgbColor[3];
         pc->getRGB(rgbColor);
         if (interpolateColor) {
            float red = 0.0;
            float green = 0.0;
            float blue = 0.0;
            if (paletteEntries.size() == 2) {
               const int colorIndex1 = paletteEntries[0].getColorIndex();
               unsigned char rgbColor1[3];
               const PaletteColor* pc1 = myPaletteFile->getPaletteColor(colorIndex1);
               pc1->getRGB(rgbColor1);
               const int colorIndex2 = paletteEntries[1].getColorIndex();
               unsigned char rgbColor2[3];
               const PaletteColor* pc2 = myPaletteFile->getPaletteColor(colorIndex2);
               pc2->getRGB(rgbColor2);
               red   = scalar * rgbColor1[0]
                     + (1.0 - scalar) * rgbColor2[0];
               green = scalar * rgbColor1[1]
                     + (1.0 - scalar) * rgbColor2[1];
               blue  = scalar * rgbColor1[2]
                     + (1.0 - scalar) * rgbColor2[2];
            }
            else {
               const int paletteIndex2 = paletteIndex + 1;
               //
               // Cannot interpolate to the "none" color
               //
               const int colorIndex2 = paletteEntries[paletteIndex2].getColorIndex();
               const PaletteColor* pc2 = myPaletteFile->getPaletteColor(colorIndex2);
               if (pc2->isNoneColor()) {
                  red   = rgbColor[0];
                  green = rgbColor[1];
                  blue  = rgbColor[2];
               }
               else {
                  const float d2 = paletteEntries[paletteIndex].tableValue - scalar;
                  const float d1 = scalar - paletteEntries[paletteIndex2].tableValue;
                  float pct1 = 0.0;
                  float pct2 = 0.0;
                  const float dTable = paletteEntries[paletteIndex].tableValue
                                     - paletteEntries[paletteIndex2].tableValue;
                  if (dTable > 0.0) {
                     pct1 = d1 / dTable;
                     pct2 = d2 / dTable;
                  }
                  unsigned char rgbColor2[3];
                  pc2->getRGB(rgbColor2);
                  red   = pct1 * rgbColor[0]
                        + pct2 * rgbColor2[0];
                  green = pct1 * rgbColor[1]
                        + pct2 * rgbColor2[1];
                  blue  = pct1 * rgbColor[2]
                        + pct2 * rgbColor2[2];
               }
            }
            if (red > 255.0) red = 255.0;
            if (red < 0.0)   red = 0.0;
            if (green > 255.0) green = 255.0;
            if (green < 0.0)   green = 0.0;
            if (blue > 255.0) blue = 255.0;
            if (blue < 0.0)   blue = 0.0;
            colorOut[0] = static_cast<unsigned char>(red);
            colorOut[1] = static_cast<unsigned char>(green);
            colorOut[2] = static_cast<unsigned char>(blue);
         }
         else {
            colorOut[0] = rgbColor[0];
            colorOut[1] = rgbColor[1];
            colorOut[2] = rgbColor[2];
         }
      }
   }
}

/**
 * Write the palette's data.
 */
void
Palette::writeFileData(QTextStream& stream) const throw (FileException)
{
   if (positiveOnly) {
      stream << "***PALETTES " << paletteName 
             << " [" << paletteEntries.size() << "+]\n";
   }
   else {
      stream << "***PALETTES " << paletteName
             << " [" << paletteEntries.size() << "]\n";
   }
   for (unsigned int j = 0; j < paletteEntries.size(); j++) {
      const int indx = paletteEntries[j].getColorIndex();
      if ((indx >= 0) && (indx < myPaletteFile->getNumberOfPaletteColors())) {
         const PaletteColor* pc = myPaletteFile->getPaletteColor(indx);
         stream << " " << paletteEntries[j].getValue() << " -> " << pc->getName() << "\n";
      }
   }
   stream << "\n";
}

//---------------------------------------------------------------------------
//
// Palette File Class Methods
//
/**
 * The constructor.
 */
PaletteFile::PaletteFile()
   : AbstractFile("Palette File", SpecFile::getPaletteFileExtension())
{
   clear();
   addDefaultPalettes();
   clearModified();
}

/**
 * get a palette color index by name.
 */
int 
PaletteFile::getColorIndexFromName(const QString& name) const
{
   int indx = -1;
   
   const int num = getNumberOfPaletteColors();
   for (int i = 0; i < num; i++) {
      const PaletteColor* pc = getPaletteColor(i);
      if (pc->getName() == name) {
         return i;
      }
   }
   
   return indx;
}      

/**
 * Create the default palettes
 */
void
PaletteFile::addDefaultPalettes()
{
   const unsigned char none[3] = { 0xff, 0xff, 0xff };
   addPaletteColor(PaletteColor("none", none));

   const unsigned char yellow[3] = { 0xff, 0xff, 0x00 };
   addPaletteColor(PaletteColor("_yellow", yellow));
   const unsigned char black[3] = { 0x00, 0x00, 0x00 };
   addPaletteColor(PaletteColor("_black", black));
   const unsigned char orange[3] = { 0xff, 0x69, 0x00 };
   addPaletteColor(PaletteColor("_orange", orange));

   //----------------------------------------------------------------------
   // Psych palette
   //
   if (paletteNameExists("PSYCH") == false) {
      const unsigned char pyell_oran[3] = { 0xff, 0xcc, 0x00 };
      addPaletteColor(PaletteColor("_pyell-oran", pyell_oran));
      const unsigned char poran_red[3] = { 0xff, 0x44, 0x00 };
      addPaletteColor(PaletteColor("_poran-red", poran_red));
      const unsigned char pblue[3] = { 0x00, 0x44, 0xff };
      addPaletteColor(PaletteColor("_pblue", pblue));
      const unsigned char pltblue1[3] = { 0x00, 0x69, 0xff };
      addPaletteColor(PaletteColor("_pltblue1", pltblue1));
      const unsigned char pltblue2[3] = { 0x00, 0x99, 0xff };
      addPaletteColor(PaletteColor("_pltblue2", pltblue2));
      const unsigned char pbluecyan[3] = { 0x00, 0xcc, 0xff };
      addPaletteColor(PaletteColor("_pbluecyan", pbluecyan));
      
      Palette psych(this);
      psych.setName("PSYCH");
      psych.setPositiveOnly(false);
      
      psych.addPaletteEntry(1.00, "_yellow");
      psych.addPaletteEntry(0.75, "_pyell-oran");
      psych.addPaletteEntry(0.50, "_orange");
      psych.addPaletteEntry(0.25, "_poran-red");
      psych.addPaletteEntry(0.05, "none");
      psych.addPaletteEntry(-0.05, "_pblue");
      psych.addPaletteEntry(-0.25, "_pltblue1");
      psych.addPaletteEntry(-0.50, "_pltblue2");
      psych.addPaletteEntry(-0.75, "_pbluecyan");
      addPalette(psych);
   }
   
   //----------------------------------------------------------------------
   // Psych palette
   //
   if (paletteNameExists("PSYCH-NO-NONE") == false) {
      Palette psychNo(this);
      psychNo.setName("PSYCH-NO-NONE");
      psychNo.setPositiveOnly(false);
      
      psychNo.addPaletteEntry(1.00, "_yellow");
      psychNo.addPaletteEntry(0.75, "_pyell-oran");
      psychNo.addPaletteEntry(0.50, "_orange");
      psychNo.addPaletteEntry(0.25, "_poran-red");
      psychNo.addPaletteEntry(0.00, "_pblue");
      psychNo.addPaletteEntry(-0.25, "_pltblue1");
      psychNo.addPaletteEntry(-0.50, "_pltblue2");
      psychNo.addPaletteEntry(-0.75, "_pbluecyan");
      addPalette(psychNo);
   }
   
   //
   // Geodesic palette
   //
   if (paletteNameExists("Geodesic") == false) {
      Palette geodesic(this);
      geodesic.setName("Geodesic");
      geodesic.setPositiveOnly(true);
      
      const float geo_color2[3] = { 255, 255, 0 };
      const float geo_color1[3] = { 255,   0, 0 };
      float dx = 1.0;
      for (int colorIndex = 1; colorIndex <= 11; colorIndex++) {
         const float dx1 = 1.0 - dx;
         const float color[3] = {
                                  geo_color2[0]*dx + geo_color1[0]*dx1,
                                  geo_color2[1]*dx + geo_color1[1]*dx1,
                                  geo_color2[2]*dx + geo_color1[2]*dx1
                                };
         const unsigned char c255 = 255;
         const unsigned char c[3] = {
                                  std::min(static_cast<unsigned char>(color[0]), c255),
                                  std::min(static_cast<unsigned char>(color[1]), c255),
                                  std::min(static_cast<unsigned char>(color[2]), c255)
                                    };
         std::ostringstream name;
         name << "geo_" << colorIndex;
         addPaletteColor(PaletteColor(name.str().c_str(), c));
         if (dx < 0.05) {
            dx = 0.02;
         }
         geodesic.addPaletteEntry(dx, name.str().c_str());
         dx -= 0.1;
      }
      const unsigned char geo_blue[3] = { 0, 0, 0xaa };
      addPaletteColor(PaletteColor("geo_blue", geo_blue)); 
      geodesic.addPaletteEntry( 0.01,   "geo_blue");
      geodesic.addPaletteEntry(-0.01,   "none");
      geodesic.addPaletteEntry(-1.00,   "none");

      addPalette(geodesic);
   }
   
   //------------------------------------------------------------------------
   //
   // Palette by David Van Essen
   //
   const unsigned char oran_yell[3] = { 0xff, 0x99, 0x00 };
   addPaletteColor(PaletteColor("_oran-yell", oran_yell));
   const unsigned char red[3] = { 0xff, 0x00, 0x00 };
   addPaletteColor(PaletteColor("_red", red));
   const unsigned char cyan[3] = { 0x00, 0xff, 0xff };
   addPaletteColor(PaletteColor("_cyan", cyan));
   const unsigned char green[3] = { 0x00, 0xff, 0x00 };
   addPaletteColor(PaletteColor("_green", green));
   const unsigned char limegreen[3] = { 0x10, 0xb0, 0x10 };
   addPaletteColor(PaletteColor("_limegreen", limegreen));
   const unsigned char violet[3] = { 0xe2, 0x51, 0xe2 };
   addPaletteColor(PaletteColor("_violet", violet));
   const unsigned char hotpink[3] = { 0xff, 0x38, 0x8d };
   addPaletteColor(PaletteColor("_hotpink", hotpink));
   const unsigned char white[3] = { 0xff, 0xff, 0xff };
   addPaletteColor(PaletteColor("_white", white));
   const unsigned char gry_dd[3] = { 0xdd, 0xdd, 0xdd };
   addPaletteColor(PaletteColor("_gry-dd", gry_dd ));
   const unsigned char gry_bb[3] = { 0xbb, 0xbb, 0xbb };
   addPaletteColor(PaletteColor("_gry-bb", gry_bb));
   const unsigned char purple2[3] = { 0x66, 0x00, 0x33 };
   addPaletteColor(PaletteColor("_purple2", purple2));
   const unsigned char blue_videen11[3] = { 0x33, 0x33, 0x4c };
   addPaletteColor(PaletteColor("_blue_videen11", blue_videen11));
   const unsigned char blue_videen9[3] = { 0x4c, 0x4c, 0x7f };
   addPaletteColor(PaletteColor("_blue_videen9", blue_videen9));
   const unsigned char blue_videen7[3] = { 0x7f, 0x7f, 0xcc };
   addPaletteColor(PaletteColor("_blue_videen7", blue_videen7));
   
   if (paletteNameExists("clear_brain") == false) {
      Palette clearBrain(this);
      clearBrain.setName("clear_brain");
      clearBrain.addPaletteEntry(1.0 , "_red");
      clearBrain.addPaletteEntry(0.9 , "_orange");
      clearBrain.addPaletteEntry(0.8 , "_oran-yell");
      clearBrain.addPaletteEntry(0.7 , "_yellow");
      clearBrain.addPaletteEntry(0.6 , "_limegreen");
      clearBrain.addPaletteEntry(0.5 , "_green");
      clearBrain.addPaletteEntry(0.4 , "_blue_videen7");
      clearBrain.addPaletteEntry(0.3 , "_blue_videen9");
      clearBrain.addPaletteEntry(0.2 , "_blue_videen11");
      clearBrain.addPaletteEntry(0.1 , "_purple2");
      clearBrain.addPaletteEntry(0.0 , "none");
      clearBrain.addPaletteEntry(-0.1 , "_cyan");
      clearBrain.addPaletteEntry(-0.2 , "_green");
      clearBrain.addPaletteEntry(-0.3 , "_limegreen");
      clearBrain.addPaletteEntry(-0.4 , "_violet");
      clearBrain.addPaletteEntry(-0.5 , "_hotpink");
      clearBrain.addPaletteEntry(-0.6 , "_white");
      clearBrain.addPaletteEntry(-0.7 , "_gry-dd");
      clearBrain.addPaletteEntry(-0.8 , "_gry-bb");
      clearBrain.addPaletteEntry(-0.9 , "_black");
      addPalette(clearBrain);
   }
   
   if (paletteNameExists("videen_style") == false) {
      Palette videenStyle(this);
      videenStyle.setName("videen_style");
      videenStyle.addPaletteEntry(1.0 , "_red");
      videenStyle.addPaletteEntry(0.9 , "_orange");
      videenStyle.addPaletteEntry(0.8 , "_oran-yell");
      videenStyle.addPaletteEntry(0.7 , "_yellow");
      videenStyle.addPaletteEntry(0.6 , "_limegreen");
      videenStyle.addPaletteEntry(0.5 , "_green");
      videenStyle.addPaletteEntry(0.4 , "_blue_videen7");
      videenStyle.addPaletteEntry(0.3 , "_blue_videen9");
      videenStyle.addPaletteEntry(0.2 , "_blue_videen11");
      videenStyle.addPaletteEntry(0.1 , "_purple2");
      videenStyle.addPaletteEntry(0.0 , "_black");
      videenStyle.addPaletteEntry(-0.1 , "_cyan");
      videenStyle.addPaletteEntry(-0.2 , "_green");
      videenStyle.addPaletteEntry(-0.3 , "_limegreen");
      videenStyle.addPaletteEntry(-0.4 , "_violet");
      videenStyle.addPaletteEntry(-0.5 , "_hotpink");
      videenStyle.addPaletteEntry(-0.6 , "_white");
      videenStyle.addPaletteEntry(-0.7 , "_gry-dd");
      videenStyle.addPaletteEntry(-0.8 , "_gry-bb");
      videenStyle.addPaletteEntry(-0.9 , "_black");
      addPalette(videenStyle);
   }
   
   if (paletteNameExists("fidl") == false) {
      const unsigned char Bright_Yellow[3] = { 0xee, 0xee, 0x55 };
      addPaletteColor(PaletteColor("_Bright_Yellow", Bright_Yellow));
      const unsigned char Mustard[3] = { 0xdd, 0xdd, 0x66 };
      addPaletteColor(PaletteColor("_Mustard", Mustard));
      const unsigned char Brown_Mustard[3] = { 0xdd, 0x99, 0x00 };
      addPaletteColor(PaletteColor("_Brown_Mustard", Brown_Mustard));
      const unsigned char Bright_Red[3] = { 0xff, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_Bright_Red", Bright_Red));
      const unsigned char Fire_Engine_Red[3] = { 0xdd, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_Fire_Engine_Red", Fire_Engine_Red));
      const unsigned char Brick[3] = { 0xbb, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_Brick", Brick));
      const unsigned char Beet[3] = { 0x99, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_Beet", Beet));
      const unsigned char Beaujolais[3] = { 0x77, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_Beaujolais", Beaujolais));
      const unsigned char Burgundy[3] = { 0x55, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_Burgundy", Burgundy));
      const unsigned char Thrombin[3] = { 0x11, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_Thrombin", Thrombin));
      const unsigned char Deep_Green[3] = { 0x00, 0x11, 0x00 };
      addPaletteColor(PaletteColor("_Deep_Green", Deep_Green));
      const unsigned char British_Racing_Green[3] = { 0x00, 0x55, 0x00 };
      addPaletteColor(PaletteColor("_British_Racing_Green", British_Racing_Green));
      const unsigned char Kelp[3] = { 0x00, 0x77, 0x00 };
      addPaletteColor(PaletteColor("_Kelp", Kelp));
      const unsigned char Lime[3] = { 0x00, 0x99, 0x00 };
      addPaletteColor(PaletteColor("_Lime", Lime));
      const unsigned char Mint[3] = { 0x00, 0xbb, 0x00 };
      addPaletteColor(PaletteColor("_Mint", Mint));
      const unsigned char Brussell_Sprout[3] = { 0x00, 0xdd, 0x00 };
      addPaletteColor(PaletteColor("_Brussell_Sprout", Brussell_Sprout));
      const unsigned char Bright_Green[3] = { 0x00, 0xff, 0x00 };
      addPaletteColor(PaletteColor("_Bright_Green", Bright_Green));
      const unsigned char Periwinkle[3] = { 0x66, 0x66, 0xbb };
      addPaletteColor(PaletteColor("_Periwinkle", Periwinkle));
      const unsigned char Azure[3] = { 0x88, 0x88, 0xee };
      addPaletteColor(PaletteColor("_Azure", Azure));
      const unsigned char Turquoise[3] = { 0x00, 0xcc, 0xcc };
      addPaletteColor(PaletteColor("_Turquoise", Turquoise));
      
      Palette fidl(this);
      fidl.setName("fidl");
      fidl.addPaletteEntry(1.0, "_Bright_Yellow");
      fidl.addPaletteEntry(0.9, "_Mustard");
      fidl.addPaletteEntry(0.8, "_Brown_Mustard");
      fidl.addPaletteEntry(0.7, "_Bright_Red");
      fidl.addPaletteEntry(0.6, "_Fire_Engine_Red");
      fidl.addPaletteEntry(0.5, "_Brick");
      fidl.addPaletteEntry(0.4, "_Beet");
      fidl.addPaletteEntry(0.3, "_Beaujolais");
      fidl.addPaletteEntry(0.2, "_Burgundy");
      fidl.addPaletteEntry(0.1, "_Thrombin");
      fidl.addPaletteEntry(0.0, "none");
      fidl.addPaletteEntry(-0.1, "_Deep_Green");
      fidl.addPaletteEntry(-0.2, "_British_Racing_Green");
      fidl.addPaletteEntry(-0.3, "_Kelp");
      fidl.addPaletteEntry(-0.4, "_Lime");
      fidl.addPaletteEntry(-0.5, "_Mint");
      fidl.addPaletteEntry(-0.6, "_Brussell_Sprout");
      fidl.addPaletteEntry(-0.7, "_Bright_Green");
      fidl.addPaletteEntry(-0.8, "_Periwinkle");
      fidl.addPaletteEntry(-0.9, "_Azure");
      fidl.addPaletteEntry(-1.0, "_Turquoise");
      addPalette(fidl);
   }

   //------------------------------------------------------------------------
   //
   // Palette by Jon Wieser @ mcw
   //
   const unsigned char rbgyr20_01[3] = { 0xCC, 0x10, 0x33 };
   addPaletteColor(PaletteColor("_rbgyr20_01", rbgyr20_01));
   const unsigned char rbgyr20_02[3] = { 0x99, 0x20, 0x66 };
   addPaletteColor(PaletteColor("_rbgyr20_02", rbgyr20_02));
   const unsigned char rbgyr20_03[3] = { 0x66, 0x31, 0x99 };
   addPaletteColor(PaletteColor("_rbgyr20_03", rbgyr20_03));
   const unsigned char rbgyr20_04[3] = { 0x34, 0x41, 0xCC };
   addPaletteColor(PaletteColor("_rbgyr20_04", rbgyr20_04));
   const unsigned char rbgyr20_05[3] = { 0x00, 0x51, 0xFF };
   addPaletteColor(PaletteColor("_rbgyr20_05", rbgyr20_05));
   const unsigned char rbgyr20_06[3] = { 0x00, 0x74, 0xCC };
   addPaletteColor(PaletteColor("_rbgyr20_06", rbgyr20_06));
   const unsigned char rbgyr20_07[3] = { 0x00, 0x97, 0x99 };
   addPaletteColor(PaletteColor("_rbgyr20_07", rbgyr20_07));
   const unsigned char rbgyr20_08[3] = { 0x00, 0xB9, 0x66 };
   addPaletteColor(PaletteColor("_rbgyr20_08", rbgyr20_08));
   const unsigned char rbgyr20_09[3] = { 0x00, 0xDC, 0x33 };
   addPaletteColor(PaletteColor("_rbgyr20_09", rbgyr20_09));
   const unsigned char rbgyr20_10[3] = { 0x00, 0xFF, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_10", rbgyr20_10));
   const unsigned char rbgyr20_11[3] = { 0x33, 0xFF, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_11", rbgyr20_11));
   const unsigned char rbgyr20_12[3] = { 0x66, 0xFF, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_12", rbgyr20_12));
   const unsigned char rbgyr20_13[3] = { 0x99, 0xFF, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_13", rbgyr20_13));
   const unsigned char rbgyr20_14[3] = { 0xCC, 0xFF, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_14", rbgyr20_14));
   const unsigned char rbgyr20_15[3] = { 0xFF, 0xFF, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_15", rbgyr20_15));
   const unsigned char rbgyr20_16[3] = { 0xFF, 0xCC, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_16", rbgyr20_16));
   const unsigned char rbgyr20_17[3] = { 0xFF, 0x99, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_17", rbgyr20_17));
   const unsigned char rbgyr20_18[3] = { 0xFF, 0x66, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_18", rbgyr20_18));
   const unsigned char rbgyr20_19[3] = { 0xFF, 0x33, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_19", rbgyr20_19));
   const unsigned char rbgyr20_20[3] = { 0xFF, 0x00, 0x00 };
   addPaletteColor(PaletteColor("_rbgyr20_20", rbgyr20_20));
   
   if (paletteNameExists("RBGYR20") == false) {
      Palette pal2(this);
      pal2.setName("RBGYR20");
      pal2.addPaletteEntry( 1.0, "_rbgyr20_01");
      pal2.addPaletteEntry( 0.9, "_rbgyr20_02");
      pal2.addPaletteEntry( 0.8, "_rbgyr20_03");
      pal2.addPaletteEntry( 0.7, "_rbgyr20_04");
      pal2.addPaletteEntry( 0.6, "_rbgyr20_05");
      pal2.addPaletteEntry( 0.5, "_rbgyr20_06");
      pal2.addPaletteEntry( 0.4, "_rbgyr20_07");
      pal2.addPaletteEntry( 0.3, "_rbgyr20_08");
      pal2.addPaletteEntry( 0.2, "_rbgyr20_09");
      pal2.addPaletteEntry( 0.1, "_rbgyr20_10");
      pal2.addPaletteEntry( 0.0, "_rbgyr20_11");
      pal2.addPaletteEntry(-0.1, "_rbgyr20_12");
      pal2.addPaletteEntry(-0.2, "_rbgyr20_13");
      pal2.addPaletteEntry(-0.3, "_rbgyr20_14");
      pal2.addPaletteEntry(-0.4, "_rbgyr20_15");
      pal2.addPaletteEntry(-0.5, "_rbgyr20_16");
      pal2.addPaletteEntry(-0.6, "_rbgyr20_17");
      pal2.addPaletteEntry(-0.7, "_rbgyr20_18");
      pal2.addPaletteEntry(-0.8, "_rbgyr20_19");
      pal2.addPaletteEntry(-0.9, "_rbgyr20_20");
      addPalette(pal2);
 
      Palette pal3(this);
      pal3.setName("RBGYR20");
      pal3.setPositiveOnly(true);
      pal3.addPaletteEntry(1.00, "_rbgyr20_01");
      pal3.addPaletteEntry(0.95, "_rbgyr20_02");
      pal3.addPaletteEntry(0.90, "_rbgyr20_03");
      pal3.addPaletteEntry(0.85, "_rbgyr20_04");
      pal3.addPaletteEntry(0.80, "_rbgyr20_05");
      pal3.addPaletteEntry(0.75, "_rbgyr20_06");
      pal3.addPaletteEntry(0.70, "_rbgyr20_07");
      pal3.addPaletteEntry(0.65, "_rbgyr20_08");
      pal3.addPaletteEntry(0.60, "_rbgyr20_09");
      pal3.addPaletteEntry(0.55, "_rbgyr20_10");
      pal3.addPaletteEntry(0.50, "_rbgyr20_11");
      pal3.addPaletteEntry(0.45, "_rbgyr20_12");
      pal3.addPaletteEntry(0.40, "_rbgyr20_13");
      pal3.addPaletteEntry(0.35, "_rbgyr20_14");
      pal3.addPaletteEntry(0.30, "_rbgyr20_15");
      pal3.addPaletteEntry(0.25, "_rbgyr20_16");
      pal3.addPaletteEntry(0.20, "_rbgyr20_17");
      pal3.addPaletteEntry(0.15, "_rbgyr20_18");
      pal3.addPaletteEntry(0.10, "_rbgyr20_19");
      pal3.addPaletteEntry(0.05, "_rbgyr20_20");
      addPalette(pal3);
   }
  
   {
      //------------------------------------------------------------------------
      //
      // Colors by Russ H.
      //
      const unsigned char _rbgyr20_10[3] = { 0x00, 0xff, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_10", _rbgyr20_10));
      const unsigned char _rbgyr20_15[3] = { 0xff, 0xff, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_15", _rbgyr20_15));
      const unsigned char _rbgyr20_20[3] = { 0xff, 0x00, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_20", _rbgyr20_20));
   
      const unsigned char _rbgyr20_21[3] = { 0x9d, 0x22, 0xc1 };
      addPaletteColor(PaletteColor("_rbgyr20_21", _rbgyr20_21));
      const unsigned char _rbgyr20_22[3] = { 0x81, 0x06, 0xa5 };
      addPaletteColor(PaletteColor("_rbgyr20_22", _rbgyr20_22));
      const unsigned char _rbgyr20_23[3] = { 0xff, 0xec, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_23", _rbgyr20_23));
      const unsigned char _rbgyr20_24[3] = { 0xff, 0xd6, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_24", _rbgyr20_24));
      const unsigned char _rbgyr20_25[3] = { 0xff, 0xbc, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_25", _rbgyr20_25));
      const unsigned char _rbgyr20_26[3] = { 0xff, 0x9c, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_26", _rbgyr20_26));
      const unsigned char _rbgyr20_27[3] = { 0xff, 0x7c, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_27", _rbgyr20_27));
      const unsigned char _rbgyr20_28[3] = { 0xff, 0x5c, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_28", _rbgyr20_28));
      const unsigned char _rbgyr20_29[3] = { 0xff, 0x3d, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_29", _rbgyr20_29));
      const unsigned char _rbgyr20_30[3] = { 0xff, 0x23, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_30", _rbgyr20_30));
      const unsigned char _rbgyr20_31[3] = { 0x00, 0xed, 0x12 };
      addPaletteColor(PaletteColor("_rbgyr20_31", _rbgyr20_31));
      const unsigned char _rbgyr20_32[3] = { 0x00, 0xd5, 0x2a };
      addPaletteColor(PaletteColor("_rbgyr20_32", _rbgyr20_32));
      const unsigned char _rbgyr20_33[3] = { 0x00, 0xb9, 0x46 };
      addPaletteColor(PaletteColor("_rbgyr20_33", _rbgyr20_33));
      const unsigned char _rbgyr20_34[3] = { 0x00, 0x9b, 0x64 };
      addPaletteColor(PaletteColor("_rbgyr20_34", _rbgyr20_34));
      const unsigned char _rbgyr20_35[3] = { 0x00, 0x7b, 0x84 };
      addPaletteColor(PaletteColor("_rbgyr20_35", _rbgyr20_35));
      const unsigned char _rbgyr20_36[3] = { 0x00, 0x5b, 0xa4 };
      addPaletteColor(PaletteColor("_rbgyr20_36", _rbgyr20_36));
      const unsigned char _rbgyr20_37[3] = { 0x00, 0x44, 0xbb };
      addPaletteColor(PaletteColor("_rbgyr20_37", _rbgyr20_37));
      const unsigned char _rbgyr20_38[3] = { 0x00, 0x24, 0xdb };
      addPaletteColor(PaletteColor("_rbgyr20_38", _rbgyr20_38));
      const unsigned char _rbgyr20_39[3] = { 0x00, 0x00, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_39", _rbgyr20_39));
      
      const unsigned char _rbgyr20_40[3] = { 0xff, 0xf1, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_40", _rbgyr20_40));
      const unsigned char _rbgyr20_41[3] = { 0xff, 0xdc, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_41", _rbgyr20_41));
      const unsigned char _rbgyr20_42[3] = { 0xff, 0xcb, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_42", _rbgyr20_42));
      const unsigned char _rbgyr20_43[3] = { 0xff, 0xc2, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_43", _rbgyr20_43));
      const unsigned char _rbgyr20_44[3] = { 0xff, 0xae, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_44", _rbgyr20_44));
      const unsigned char _rbgyr20_45[3] = { 0xff, 0x9f, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_45", _rbgyr20_45));
      const unsigned char _rbgyr20_46[3] = { 0xff, 0x86, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_46", _rbgyr20_46));
      const unsigned char _rbgyr20_47[3] = { 0xff, 0x59, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_47", _rbgyr20_47));
      const unsigned char _rbgyr20_48[3] = { 0x00, 0xff, 0x2d };
      addPaletteColor(PaletteColor("_rbgyr20_48", _rbgyr20_48));
      const unsigned char _rbgyr20_49[3] = { 0x00, 0xff, 0x65 };
      addPaletteColor(PaletteColor("_rbgyr20_49", _rbgyr20_49));
      const unsigned char _rbgyr20_50[3] = { 0x00, 0xff, 0xa5 };
      addPaletteColor(PaletteColor("_rbgyr20_50", _rbgyr20_50));
      const unsigned char _rbgyr20_51[3] = { 0x00, 0xff, 0xdd };
      addPaletteColor(PaletteColor("_rbgyr20_51", _rbgyr20_51));
      const unsigned char _rbgyr20_52[3] = { 0x00, 0xff, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_52", _rbgyr20_52));
      const unsigned char _rbgyr20_53[3] = { 0x00, 0xe9, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_53", _rbgyr20_53));
      const unsigned char _rbgyr20_54[3] = { 0x00, 0xad, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_54", _rbgyr20_54));
      const unsigned char _rbgyr20_55[3] = { 0x00, 0x69, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_55", _rbgyr20_55));
      const unsigned char _rbgyr20_56[3] = { 0xff, 0x00, 0xb9 };
      addPaletteColor(PaletteColor("_rbgyr20_56", _rbgyr20_56));
      const unsigned char _rbgyr20_57[3] = { 0xff, 0x00, 0x63 };
      addPaletteColor(PaletteColor("_rbgyr20_57", _rbgyr20_57));
      const unsigned char _rbgyr20_58[3] = { 0xff, 0x05, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_58", _rbgyr20_58));
      const unsigned char _rbgyr20_59[3] = { 0xff, 0x32, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_59", _rbgyr20_59));
      const unsigned char _rbgyr20_60[3]=  { 0xff, 0x70, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_60", _rbgyr20_60));
      const unsigned char _rbgyr20_61[3] = { 0xff, 0xa4, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_61", _rbgyr20_61));
      const unsigned char _rbgyr20_62[3] = { 0xff, 0xba, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_62", _rbgyr20_62));
      const unsigned char _rbgyr20_63[3] = { 0xff, 0xd3, 0x00 };
      addPaletteColor(PaletteColor("_rbgyr20_63", _rbgyr20_63));
      const unsigned char _rbgyr20_64[3] = { 0x42, 0x21, 0xdb };
      addPaletteColor(PaletteColor("_rbgyr20_64", _rbgyr20_64));
      const unsigned char _rbgyr20_65[3] = { 0x10, 0x08, 0xf6 };
      addPaletteColor(PaletteColor("_rbgyr20_65", _rbgyr20_65));
      const unsigned char _rbgyr20_66[3] = { 0x00, 0x13, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_66", _rbgyr20_66));
      const unsigned char _rbgyr20_67[3] = { 0x00, 0x5b, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_67", _rbgyr20_67));
      const unsigned char _rbgyr20_68[3] = { 0x00, 0xb3, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_68", _rbgyr20_68));
      const unsigned char _rbgyr20_69[3] = { 0x00, 0xfc, 0xff };
      addPaletteColor(PaletteColor("_rbgyr20_69", _rbgyr20_69));
      const unsigned char _rbgyr20_70[3] = { 0x00, 0xff, 0xcd };
      addPaletteColor(PaletteColor("_rbgyr20_70", _rbgyr20_70));
      const unsigned char _rbgyr20_71[3] = { 0x00, 0xff, 0x74 };
      addPaletteColor(PaletteColor("_rbgyr20_71", _rbgyr20_71));
      const unsigned char _rbgyr20_72[3] = { 0xff, 0x00, 0xf9 };
      addPaletteColor(PaletteColor("_rbgyr20_72", _rbgyr20_72));
      const unsigned char _rbgyr20_73[3] = { 0x62, 0x31, 0xc9 };
      addPaletteColor(PaletteColor("_rbgyr20_73", _rbgyr20_73));

      //------------------------------------------------------------------------
      //
      // Palette by Russ H.
      //
      if (paletteNameExists("raich4_clrmid") == false) {
         Palette r4(this);
         r4.setName("raich4_clrmid");
         r4.addPaletteEntry(1.000000, "_rbgyr20_20");
         r4.addPaletteEntry(0.900000, "_rbgyr20_30");
         r4.addPaletteEntry(0.800000, "_rbgyr20_29");
         r4.addPaletteEntry(0.700000, "_rbgyr20_28");
         r4.addPaletteEntry(0.600000, "_rbgyr20_27");
         r4.addPaletteEntry(0.500000, "_rbgyr20_26");
         r4.addPaletteEntry(0.400000, "_rbgyr20_25");
         r4.addPaletteEntry(0.300000, "_rbgyr20_24");
         r4.addPaletteEntry(0.200000, "_rbgyr20_23");
         r4.addPaletteEntry(0.100000, "_rbgyr20_15");
         r4.addPaletteEntry(0.000000, "none");
         r4.addPaletteEntry(-0.100000, "_rbgyr20_10");
         r4.addPaletteEntry(-0.200000, "_rbgyr20_31");
         r4.addPaletteEntry(-0.300000, "_rbgyr20_32");
         r4.addPaletteEntry(-0.400000, "_rbgyr20_33");
         r4.addPaletteEntry(-0.500000, "_rbgyr20_34");
         r4.addPaletteEntry(-0.600000, "_rbgyr20_35");
         r4.addPaletteEntry(-0.700000, "_rbgyr20_36");
         r4.addPaletteEntry(-0.800000, "_rbgyr20_37");
         r4.addPaletteEntry(-0.900000, "_rbgyr20_38");
         r4.addPaletteEntry(-1.000000, "_rbgyr20_39");
         addPalette(r4);
      }
         
      //------------------------------------------------------------------------
      //
      // Palette by Russ H.
      //
      if (paletteNameExists("raich6_clrmid") == false) {
         Palette r6(this);
         r6.setName("raich6_clrmid");
         r6.addPaletteEntry(1.000000, "_rbgyr20_20");
         r6.addPaletteEntry(0.900000, "_rbgyr20_47");
         r6.addPaletteEntry(0.800000, "_rbgyr20_46");
         r6.addPaletteEntry(0.700000, "_rbgyr20_45");
         r6.addPaletteEntry(0.600000, "_rbgyr20_44");
         r6.addPaletteEntry(0.500000, "_rbgyr20_43");
         r6.addPaletteEntry(0.400000, "_rbgyr20_42");
         r6.addPaletteEntry(0.300000, "_rbgyr20_41");
         r6.addPaletteEntry(0.200000, "_rbgyr20_40");
         r6.addPaletteEntry(0.100000, "_rbgyr20_15");
         r6.addPaletteEntry(0.000000, "none");
         r6.addPaletteEntry(-0.100000, "_rbgyr20_10");
         r6.addPaletteEntry(-0.200000, "_rbgyr20_48");
         r6.addPaletteEntry(-0.300000, "_rbgyr20_49");
         r6.addPaletteEntry(-0.400000, "_rbgyr20_50");
         r6.addPaletteEntry(-0.500000, "_rbgyr20_51");
         r6.addPaletteEntry(-0.600000, "_rbgyr20_52");
         r6.addPaletteEntry(-0.700000, "_rbgyr20_53");
         r6.addPaletteEntry(-0.800000, "_rbgyr20_54");
         r6.addPaletteEntry(-0.900000, "_rbgyr20_55");
         r6.addPaletteEntry(-1.000000, "_rbgyr20_39");
         addPalette(r6);
      }
         
      //------------------------------------------------------------------------
      //
      // Palette by Russ H.
      //
      if (paletteNameExists("HSB8_clrmid") == false) {
         Palette hsb8(this);
         hsb8.setName("HSB8_clrmid");
         hsb8.addPaletteEntry(1.000000, "_rbgyr20_15");
         hsb8.addPaletteEntry(0.900000, "_rbgyr20_63");
         hsb8.addPaletteEntry(0.800000, "_rbgyr20_62");
         hsb8.addPaletteEntry(0.700000, "_rbgyr20_61");
         hsb8.addPaletteEntry(0.600000, "_rbgyr20_60");
         hsb8.addPaletteEntry(0.500000, "_rbgyr20_59");
         hsb8.addPaletteEntry(0.400000, "_rbgyr20_58");
         hsb8.addPaletteEntry(0.300000, "_rbgyr20_57");
         hsb8.addPaletteEntry(0.200000, "_rbgyr20_56");
         hsb8.addPaletteEntry(0.100000, "_rbgyr20_72");
         hsb8.addPaletteEntry(0.000000, "none");
         hsb8.addPaletteEntry(-0.100000, "_rbgyr20_73");
         hsb8.addPaletteEntry(-0.200000, "_rbgyr20_64");
         hsb8.addPaletteEntry(-0.300000, "_rbgyr20_65");
         hsb8.addPaletteEntry(-0.400000, "_rbgyr20_66");
         hsb8.addPaletteEntry(-0.500000, "_rbgyr20_67");
         hsb8.addPaletteEntry(-0.600000, "_rbgyr20_68");
         hsb8.addPaletteEntry(-0.700000, "_rbgyr20_69");
         hsb8.addPaletteEntry(-0.800000, "_rbgyr20_70");
         hsb8.addPaletteEntry(-0.900000, "_rbgyr20_71");
         hsb8.addPaletteEntry(-1.000000, "_rbgyr20_10");
         addPalette(hsb8);
      }
   }
   
   //----------------------------------------------------------------------
   // Orange-Yellow palette
   //
   if (paletteNameExists("Orange-Yellow") == false) {
      const unsigned char oy1[3] = { 0, 0, 0 };
      addPaletteColor(PaletteColor("_oy1", oy1));
      const unsigned char oy2[3] = { 130, 2, 0 };
      addPaletteColor(PaletteColor("_oy2", oy2));
      const unsigned char oy3[3] = { 254, 130, 2 };
      addPaletteColor(PaletteColor("_oy3", oy3));
      const unsigned char oy4[3] = { 254, 254, 126 };
      addPaletteColor(PaletteColor("_oy4", oy4));
      const unsigned char oy5[3] = { 254, 254, 254 };
      addPaletteColor(PaletteColor("_oy5", oy5));
      
         Palette orangeYellow(this);
         orangeYellow.setName("Orange-Yellow");
         orangeYellow.addPaletteEntry( 1.0, "_oy5");
         orangeYellow.addPaletteEntry( 0.5, "_oy4");
         orangeYellow.addPaletteEntry( 0.0, "_oy3");
         orangeYellow.addPaletteEntry(-0.5, "_oy2");
         orangeYellow.addPaletteEntry(-1.0, "_oy1");
         addPalette(orangeYellow);
   }
   
   //----------------------------------------------------------------------
   // Gray scale palette
   //
   if (paletteNameExists("Gray") == false) {
      const unsigned char maxIntensity = 255;  // 230;
      const unsigned char minIntensity =   0;  //  25;
      unsigned char intensity = maxIntensity;
      const int numSteps = 10;
      const int stepSize = (maxIntensity - minIntensity) / numSteps;
      const float tableStep = 2.0 / static_cast<float>(numSteps);
      float tableValue = 1.0;

      Palette pal(this);
      pal.setName("Gray");   
      for (int i = 0; i < numSteps; i++) {
         unsigned char rgb[3] = { intensity, intensity, intensity };
         std::ostringstream str;
         str << "_gray_" << i << std::ends;
         PaletteColor pc(str.str().c_str(), rgb);
         addPaletteColor(pc);

         pal.addPaletteEntry(tableValue, str.str().c_str());

         tableValue -= tableStep;
         intensity  -= stepSize;
      }
      addPalette(pal);
   }
   
   //
   // Create a palette with just white and black designed to be used
   // with the interpolate option
   //   
   if (paletteNameExists(grayInterpPaletteName) == false) {
      Palette palGrayInterp(this);
      palGrayInterp.setName(grayInterpPaletteName);
      const unsigned char white[3] = { 255, 255, 255 };
      addPaletteColor(PaletteColor("_white_gray_interp", white));
      const unsigned char black[3] = { 0, 0, 0 };
      addPaletteColor(PaletteColor("_black_gray_interp", black));

      palGrayInterp.addPaletteEntry( 1.0, "_white_gray_interp");
      palGrayInterp.addPaletteEntry(-1.0, "_black_gray_interp");
      addPalette(palGrayInterp);   
   }
   
   //
   // Inverse gray positive palette
   //
   if (paletteNameExists("Gray_Inverse_Interp") == false) {
      Palette inverseGray(this);
      inverseGray.setPositiveOnly(true);
      inverseGray.setName("Gray_Inverse_Interp");
      const unsigned char white[3] = { 255, 255, 255 };
      addPaletteColor(PaletteColor("_white_gray_interp", white));
      const unsigned char black[3] = { 0, 0, 0 };
      addPaletteColor(PaletteColor("_black_gray_interp", black));
      
      inverseGray.addPaletteEntry(1.0, "_black_gray_interp");
      inverseGray.addPaletteEntry(0.001, "_white_gray_interp");
      inverseGray.addPaletteEntry(0.0009, "none");
      addPalette(inverseGray);
   }
   
   if (paletteNameExists("Plus_Minus") == false) {
      Palette plusMinus(this);
      plusMinus.setName("Plus_Minus");
      
      plusMinus.addPaletteEntry(1.00, "_yellow");
      plusMinus.addPaletteEntry(0.00001, "none");
      plusMinus.addPaletteEntry(-0.00001, "_pbluecyan");
      addPalette(plusMinus);
   }
}

/**
 * See if a palette with specified name exists
 */
bool
PaletteFile::paletteNameExists(const QString& name)
{
   for (int i = 0; i < getNumberOfPalettes(); i++) {
      const Palette* palette = getPalette(i);
      if (palette->getName() == name) {
         return true;
      }
   }
   return false;
}

/**
 * The destructor.
 */
PaletteFile::~PaletteFile()
{
   clear();
}

/**
 * add a palette to the palette file.
 */
void 
PaletteFile::addPalette(const Palette& pal) 
{ 
   palettes.push_back(pal); 
   const int num = getNumberOfPalettes() - 1;
   palettes[num].myPaletteFile = this;
   setModified();
}

/** 
 * get the palette colors into a color file.
 */
void 
PaletteFile::getPaletteColorsUsingColorFile(ColorFile& cf) const
{
   //
   // Initially clear the color file
   //
   cf.clear();
   
   //
   // Get the number of palette colors
   //
   const int numPaletteColors = getNumberOfPaletteColors();
   
   //
   // Loop through the palette colors
   //
   for (int i = 0; i < numPaletteColors; i++) {
      //
      // Get the palette color
      //
      const PaletteColor* pc = getPaletteColor(i);
      
      //
      // Add onto the color file
      //
      unsigned char rgb[3];
      pc->getRGB(rgb);
      cf.addColor(pc->getName(), rgb[0], rgb[1], rgb[2]);
   }
}      
   
/**
 * replace the palette colors using a ColorFile (update color assignments too)
 */
void 
PaletteFile::replacePaletteColorsUsingColorFile(const ColorFile& cf)
{
   //
   // Keeps track of palette entries without a valid color
   //
   std::vector<PaletteEntry*> paletteEntriesMissingColor;
   
   //
   // Get the number of palette colors
   //
   const int numPaletteColors = getNumberOfPaletteColors();
   
   //
   // Loop through the palettes
   //
   for (int i = 0; i < getNumberOfPalettes(); i++) {
      //
      // Get the palette
      //
      Palette* pal = getPalette(i);
      
      //
      // Get the number of entries in the palette
      //
      const int numEntries = pal->getNumberOfPaletteEntries();
      
      //
      // Loop through the palette entries
      //
      for (int j = 0; j < numEntries; j++) {
         //
         // Get the palette entry
         //
         PaletteEntry* pe = pal->getPaletteEntry(j);

         //
         // Get the name of the current color for this palette entry
         //
         int colorIndex = pe->getColorIndex();
         
         //
         // Assume color is not found
         //
         bool colorFound = false;
         
         //
         // If valid color index
         //
         if ((colorIndex >= 0) && (colorIndex < numPaletteColors)) {
            //
            // Get the palette color for this index
            //
            PaletteColor* pc = getPaletteColor(colorIndex);
            
            //
            // Find the color in the color file
            //
            bool match = false;
            const int fileIndex = cf.getColorIndexByName(pc->getName(), match);
            if (match && (fileIndex >= 0)) {
               colorIndex = fileIndex;
               colorFound = true;
            }
         }
         
         //
         // If the color was found
         //
         if (colorFound) {
            //
            // Update palette entry's index
            //
            pe->setColorIndex(colorIndex);
         }
         else {
            //
            // keep track of palette entries missing the correct color
            //
            paletteEntriesMissingColor.push_back(pe);
         }
      }
   }
   
   //
   // Get rid of the existing colors.
   //
   colors.clear();
   
   //
   // Copy the colors from the palette file
   //
   const int numFileColors = cf.getNumberOfColors();
   for (int i = 0; i < numFileColors; i++) {
      //
      // Get name and color components from color file
      //
      unsigned char rgb[3];
      cf.getColorByIndex(i, rgb[0], rgb[1], rgb[2]);
      
      //
      // Add to palette colors
      //
      addPaletteColor(cf.getColorNameByIndex(i), rgb);
   }
   
   //
   // Are there missing colors
   //
   if (paletteEntriesMissingColor.empty() == false) {
      // 
      // Find index of missing color
      //
      int missingColorIndex = getColorIndexFromName(PaletteColor::missingColorName);
      if (missingColorIndex < 0) {
         unsigned char rgb[3] = { 0, 0, 0 };
         addPaletteColor(PaletteColor::missingColorName, rgb);
         missingColorIndex = getNumberOfPaletteColors() -1;
      }

      //
      // Set colors indices to point to missing colors
      //
      const int num = static_cast<int>(paletteEntriesMissingColor.size());
      for (int i = 0; i < num; i++) {
         paletteEntriesMissingColor[i]->setColorIndex(missingColorIndex);
      }
   }
}

/**
 * Add a color to the palette.
 */
void 
PaletteFile::addPaletteColor(const QString& name, const unsigned char rgb[3])
{
   addPaletteColor(PaletteColor(name, rgb));
}
      
/**
 * Add a color to the palette.
 */
void 
PaletteFile::addPaletteColor(const PaletteColor& pc)
{
   //
   // see if color exists
   //
   setModified();
   for (unsigned int i = 0; i < colors.size(); i++) {
      if (colors[i].getName() == pc.getName()) {
         unsigned char oldrgb[3];
         colors[i].getRGB(oldrgb);
         unsigned char newrgb[3];
         pc.getRGB(newrgb);
         if ((oldrgb[0] == newrgb[0]) &&
             (oldrgb[1] == newrgb[1]) &&
             (oldrgb[2] == newrgb[2])) {
            //printf("INFO: color \"%s\" specified multiple times in "
            //       "palette file with same RGB.\n", pc.getName());
         }
         else if (pc.getName() != "none") {
            std::cout << "WARNING: Color \""
                      << pc.getName().toAscii().constData()
                      << "\" defined multiple times in palette file\n";
            std::cout << "   old color ("
                << (int)oldrgb[0] << ", "
                << (int)oldrgb[1] << ", "
                << (int)oldrgb[2] << ") new color ("
                << (int)newrgb[0] << ", "
                << (int)newrgb[1] << ", "
                << (int)newrgb[2] << ")\n";
            colors[i].setRGB(newrgb);
         }
         return;
      }
   }
   
   colors.push_back(pc);
   
   PaletteColor* pt = getPaletteColor(getNumberOfPaletteColors() -1);
   pt->myPaletteFile = this;
   
   setModified();
}

/**
 * Append a palette file to this palette file
 */
void
PaletteFile::append(PaletteFile& pf)
{
   //
   // Copy colors
   //
   for (int i = 0; i < pf.getNumberOfPaletteColors(); i++) {
      addPaletteColor(*(pf.getPaletteColor(i)));
   }
   
   //
   // Copy palettes and be sure to update color indices
   //
   for (int i = 0; i < pf.getNumberOfPalettes(); i++) {
      //
      // Make copy of the palette
      //
      Palette p = *(pf.getPalette(i));
      
      //
      // Loop through the palette's entries
      //
      for (int j = 0; j < p.getNumberOfPaletteEntries(); j++) {
         //
         // Update the color indices
         //
         PaletteEntry* pe = p.getPaletteEntry(j);
         int colorIndex = pe->getColorIndex();
         QString colorName(PaletteColor::missingColorName);
         if (colorIndex >= 0) {
            const PaletteColor* pc = pf.getPaletteColor(colorIndex);
            if (pc != NULL) {
               colorName = pc->getName();
            }
         }
         colorIndex = getColorIndexFromName(colorName);
         if (colorIndex < 0) {
            colorIndex = getColorIndexFromName(PaletteColor::missingColorName);
            if (colorIndex < 0) {
               const unsigned char rgb[3] = { 0, 0, 0 };
               addPaletteColor(PaletteColor::missingColorName, rgb);
               colorIndex = getColorIndexFromName(PaletteColor::missingColorName);
            }
         }
         pe->setColorIndex(colorIndex);
      }
      
      //
      // Add the palette to this palette file
      //
      addPalette(p);
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(pf);
}

/**
 * Clear the palette file.
 */
void
PaletteFile::clear()
{
   clearAbstractFile();
   colors.clear();
   palettes.clear();

   //
   // Always have "none" color
   //
   const unsigned char none[3] = { 0xff, 0xff, 0xff };
   addPaletteColor(PaletteColor("none", none));
}

/**
 * get palette index from name or number (number ranges 1..N).
 */
int 
PaletteFile::getPaletteIndexFromNameOrNumber(const QString& nameOrNumber) const throw (FileException)
{
   //
   // Find the palette by name
   //
   const int numberOfPalettes = getNumberOfPalettes();
   for (int i = 0; i < numberOfPalettes; i++) {
      if (getPalette(i)->getName() == nameOrNumber) {
         return i;
      }
   }
   
   //
   // Search by number
   //
   bool ok = false;
   const int num = nameOrNumber.toInt(&ok);
   if (ok) {
      if ((num > 0) ||
          (num <= numberOfPalettes)) {
         return (num - 1);
      }

      throw FileException("ERROR Invalid palette number: "
                          + nameOrNumber
                          + ".  Number should range 1 to "
                          + QString::number(numberOfPalettes));
   }
   
   //
   // faild to find
   //
   throw FileException("ERROR palette name/number "
                       + nameOrNumber
                       + " not found in file "
                       + FileUtilities::basename(getFileName()));
}      

/**
 * Get the gray interpolate palette.
 */
int 
PaletteFile::getGrayInterPaletteIndex() const
{
   for (int i = 0; i < getNumberOfPalettes(); i++) {
      if (palettes[i].getName() == grayInterpPaletteName) {
         return i;
      }
   }
   return -1;
}

/**
 * get color compontents for a color with specified name.
 */
int 
PaletteFile::getColorComponents(const QString& colorName, 
                                bool& noneColorFlag,
                                unsigned char rgb[3]) const
{
   noneColorFlag = false;
   if (colorName == PaletteColor::noneColorName) {
      rgb[0] = 0;
      rgb[0] = 1;
      rgb[0] = 2;
      noneColorFlag = true;
      return 0;
   }
   for (unsigned int i = 0; i < colors.size(); i++) {
      if (colors[i].getName() == colorName) {
         colors[i].getRGB(rgb);
         return 0;
      }
   }
   return 1;
}

/**
 * Convert from hex to base 10
 */
int
PaletteFile::hexToInt(char c) const
{      
   if ((c >= 'a') && (c <= 'f')) {
      return (c - 'a' + 10);
   }
   return (c - '0');
}   

/**
 * Convert from X color format
 */
bool
PaletteFile::convertFromXColor(const char* colorString,
                               unsigned char rgb[3]) const
{
   // color is of the form "#ff4400"
   if (colorString[0] == '#') {
      const int r2 = hexToInt(colorString[1]);
      const int r1 = hexToInt(colorString[2]);
      const int g2 = hexToInt(colorString[3]);
      const int g1 = hexToInt(colorString[4]);
      const int b2 = hexToInt(colorString[5]);
      const int b1 = hexToInt(colorString[6]);
      rgb[0] = (r2 * 16) + r1;
      rgb[1] = (g2 * 16) + g1;
      rgb[2] = (b2 * 16) + b1;
   }
   // color is of the form "rgbi:0.5/0.3/1.0"
   else if (strncmp(colorString, "rgbi:", 5) == 0) {
      char* redStr = strtok((char*)&colorString[5], "/");
      char* greenStr = strtok(NULL, "/");
      char* blueStr = strtok(NULL, "/");
      rgb[0] = static_cast<unsigned char>(QString(redStr).toFloat() * 255.0);
      rgb[1] = static_cast<unsigned char>(QString(greenStr).toFloat() * 255.0);
      rgb[2] = static_cast<unsigned char>(QString(blueStr).toFloat() * 255.0);
   }
   else {
      std::cerr << "ERROR: Unable to determine RGB for Palette Color " << colorString << std::endl;
      return true;
   }
   return false;
}
 
/**
 * remove a palette.
 */
void 
PaletteFile::removePalette(const int palNum)
{
   if ((palNum >= 0) && (palNum < getNumberOfPalettes())) {
      palettes.erase(palettes.begin() + palNum);
   }
   setModified();
}      

/**
 * Read the Palette file.
 */
void
PaletteFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   bool readColors = false;
   bool readPalette = false;
   
   enum { READING_COLORS,
          READING_PALETTE,
          READING_OTHER
        } reading = READING_OTHER;
      
   QString line;
   readLine(stream, line);
   while(stream.atEnd() == false) {
      
      // comments are like C++ style comments
      int comment = line.indexOf("//");
      if (comment != -1) {
         line.resize(comment);
      }
      
      if (line.indexOf("***COLORS") != -1) {
         reading = READING_COLORS;
      }
      else if (line.indexOf("***PALETTES") != -1) {
         reading = READING_PALETTE;
         
         QString palettes;
         QString name;
         QString numberAndPlus;
         bool positiveOnly = false;
         int numColorsInPalette = 0;

         QTextStream stin(&line, QIODevice::ReadOnly);
         stin >> palettes >> name >> numberAndPlus;
         if (numberAndPlus.isEmpty()) {
            if (name[0] == '[') {
               numberAndPlus = name;
               name = "no-name";
            }
         }
         else if (name.isEmpty()) {
            throw FileException(filename, "Unreadable palette line");
         }

         const int openBracket = numberAndPlus.indexOf('[');
         int closeBracket = numberAndPlus.indexOf(']');
         const int positiveOnlyPos = numberAndPlus.indexOf('+');
         if (positiveOnlyPos != -1) {
            closeBracket = positiveOnlyPos;
            positiveOnly = true;
         }
         
         if ((openBracket  != -1) &&
             (closeBracket != -1)) {
            const int len = closeBracket - openBracket - 1;
            const QString s(numberAndPlus.mid(openBracket + 1, len));
            numColorsInPalette = s.toInt();
         }
         else {
            throw FileException(filename, "Unable to determine number of colors in palette");
         }

         Palette pal(this);
         pal.setPositiveOnly(positiveOnly);
         pal.setName(name);
         for (int i = 0; i < numColorsInPalette; i++) {
            readLine(stream, line);
            
            // comments are like C++ style comments
            int comment = line.indexOf("//");
            if (comment != -1) {
               line.resize(comment);
            }
            
            float value;
            QString pointer;
            QString colorName;
            QTextStream ins(&line, QIODevice::ReadOnly);
            ins >> value >> pointer >> colorName;
            if (colorName.isEmpty() == false) {
               pal.addPaletteEntry(value, colorName);
            }
            else {
               QString msg("Invalid line in ***PALETTES ");
               msg.append(name);
               msg.append(" section: \"");
               msg.append(line);
               msg.append("\"");
               throw FileException(filename, msg);
            }
         }
         addPalette(pal);
         readPalette = true;

      }
      else {
         switch(reading) {
            case READING_COLORS:
               {
                  QString equals;
                  QString name;
                  QString value;
                  QTextStream ins(&line, QIODevice::ReadOnly);
                  ins >> name >> equals >> value;
                  if (value.isEmpty() == false) {
                     unsigned char rgb[3];
                     if (convertFromXColor(value.toAscii().constData(), rgb) == 0) {
                        PaletteColor pc(name, rgb);
                        addPaletteColor(pc);
                     }
                  }
                  readColors = true;
               }
               break;
            case READING_PALETTE:
               break;
            case READING_OTHER:
               break;
         }
      }
      
      readLine(stream, line);
   }
}

/**
 * Convert RGB components to X hexadecimal form
 */
QString
PaletteFile::convertToXColor(unsigned char rgb[3]) const
{ 
   char line[256];
   sprintf(line, "#%02x%02x%02x", rgb[0], rgb[1], rgb[2]);
   return line;
}


/**
 * Write the palette file.
 */
void
PaletteFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   stream << "***COLORS\n";
   for (unsigned int i = 0; i < colors.size(); i++) {
      unsigned char rgb[3];
      colors[i].getRGB(rgb);
      const QString xColor = convertToXColor(rgb);
      stream << "  " << colors[i].getName() << " = " << xColor << "\n";
   }
   stream << "\n";
   
   for (int k = 0; k < getNumberOfPalettes(); k++) {
      const Palette* pal = getPalette(k);
      pal->writeFileData(stream);
   }
}
