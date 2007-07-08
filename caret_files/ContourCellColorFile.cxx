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


#include "ContourCellColorFile.h"
#include "MDPlotFile.h"
#include "NeurolucidaFile.h"
#include "SpecFile.h"

/**
 * Constructor
 */
ContourCellColorFile::ContourCellColorFile()
   : ColorFile("Contour Cell Color File", SpecFile::getContourCellColorFileExtension())
{
}

/**
 * Destructor
 */
ContourCellColorFile::~ContourCellColorFile()
{
}

/**
 * import colors from an Neurolucida File.
 */
void 
ContourCellColorFile::importNeurolucidaFileColors(const NeurolucidaFile& nf) throw (FileException)
{
   for (int i = 0; i < nf.getNumberOfMarkerColors(); i++) {
      colors.push_back(*(nf.getMarkerColor(i)));
   }
   setModified();
}
      
/**
 * import colors from an MD Plot File.
 */
void 
ContourCellColorFile::importMDPlotFileColors() throw (FileException)
{
   for (int i = 0; i < MDPlotColor::getNumberOfColors(); i++) {
      const MDPlotColor::COLOR color = static_cast<MDPlotColor::COLOR>(i);
      const QString name = MDPlotColor::getColorName(color);
      bool exactMatch = false;
      getColorIndexByName(name, exactMatch);
      if (exactMatch == false) {
         unsigned char r, g, b;
         MDPlotColor::getColorComponents(color, r, g, b);
         addColor(name, r, g, b);
      }
   }
}      
