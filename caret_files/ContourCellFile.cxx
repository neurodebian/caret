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


#include "ContourCellFile.h"
#include "MDPlotFile.h"
#include "NeurolucidaFile.h"
#include "SpecFile.h"

/**
 * Constructor
 */
ContourCellFile::ContourCellFile()
 : CellFile("Contour Cell File", SpecFile::getContourCellFileExtension())
{
}

/**
 * Destructor
 */
ContourCellFile::~ContourCellFile()
{
}

/**
 * import cells from an MD Plot File.
 */
void 
ContourCellFile::importMDPlotFile(const MDPlotFile& mdf) throw (FileException)
{
   for (int i = 0; i < mdf.getNumberOfPoints(); i++) {
      const MDPlotPoint* mp = mdf.getPoint(i);
      const MDPlotColor::COLOR color = mp->getColor();
      const int vertexIndex = mp->getVertex();
      const MDPlotVertex* vertex = mdf.getVertex(vertexIndex);
      const float* xyz = vertex->getXYZ();
      const QString name = MDPlotColor::getColorName(color);
      CellData cd(name, xyz[0], xyz[1], xyz[2], static_cast<int>(xyz[2]));
      addCell(cd);
   }
}

/**
 * import cells from a Nuerolucida File.
 */
void 
ContourCellFile::importNeurolucidaFile(const NeurolucidaFile& nf) throw (FileException)
{
   for (int i = 0; i < nf.getNumberOfMarkers(); i++) {
      addCell(*(nf.getMarker(i)));
   }
}

      