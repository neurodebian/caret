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



#include "BorderProjectionUnprojector.h"

/**
 * Unproject a border projection point
 */
void
BorderProjectionUnprojector::unprojectBorderProjectionLink(const BorderProjectionLink* bl, 
                                const CoordinateFile& cf,
                                float xyz[3], int& section, float& radius)
{
   section = 0;
   xyz[0] = 0.0;
   xyz[1] = 0.0;
   xyz[2] = 0.0;
   radius = 0.0;

   int vertices[3];
   float areas[3];
   
   bl->getData(section, vertices, areas, radius);
   
   const float totalArea = areas[0] + areas[1] + areas[2];
   if (totalArea > 0.0) {
      const float* v1 = cf.getCoordinate(vertices[0]);
      const float* v2 = cf.getCoordinate(vertices[1]);
      const float* v3 = cf.getCoordinate(vertices[2]);

      xyz[0] = (v1[0] * areas[1] + v2[0] * areas[2] + v3[0] * areas[0])
                     / totalArea;
      xyz[1] = (v1[1] * areas[1] + v2[1] * areas[2] + v3[1] * areas[0])
                     / totalArea;
      xyz[2] = (v1[2] * areas[1] + v2[2] * areas[2] + v3[2] * areas[0])
                     / totalArea;
/*
      xyz[0] = (v1[0] * areas[2] + v2[0] * areas[0] + v3[0] * areas[1])
                     / totalArea;
      xyz[1] = (v1[1] * areas[2] + v2[1] * areas[0] + v3[1] * areas[1])
                     / totalArea;
      xyz[2] = (v1[2] * areas[2] + v2[2] * areas[0] + v3[2] * areas[1])
                     / totalArea;
*/
   }
}

/**
 * Constructor
 */
BorderProjectionUnprojector::BorderProjectionUnprojector()
{
}

/**
 * Destructor
 */
BorderProjectionUnprojector::~BorderProjectionUnprojector()
{
}

/**
 * Unproject a border projection file into surfaces projected borders
 */
void
BorderProjectionUnprojector::unprojectBorderProjections(const CoordinateFile& cf, 
                                                        const BorderProjectionFile& bpf, 
                                                        BorderFile& bf,
                                                        const int startAtProjection)
{
   const int numProj = bpf.getNumberOfBorderProjections();

   for (int i = startAtProjection; i < numProj; i++) {
      const BorderProjection* bp = bpf.getBorderProjection(i);
      
      const int numLinks = bp->getNumberOfLinks();
      
      QString name;
      float center[3];
      float sampling, variance, topography, uncertainty;
      bp->getData(name, center, sampling, variance, topography, uncertainty);
      
      Border b(name, center, sampling, variance, topography, uncertainty);
      b.setBorderColorIndex(bp->getBorderColorIndex());
      
      for (int j = 0; j < numLinks; j++) {
         const BorderProjectionLink* bpl = bp->getBorderProjectionLink(j);
         int section;
         float xyz[3];
         float radius;
         unprojectBorderProjectionLink(bpl, cf, xyz, section, radius);
         
         b.addBorderLink(xyz, section, radius);
      }
      
      b.setBorderProjectionID(bp->getUniqueID());
      b.setBorderColorIndex(bp->getBorderColorIndex());
      bf.addBorder(b);
   }
}

