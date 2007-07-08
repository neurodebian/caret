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



#ifndef __VE_CELL_PROJECTION_UNPROJECTOR_H__
#define __VE_CELL_PROJECTION_UNPROJECTOR_H__

#include "CellFile.h"
#include "CellProjectionFile.h"
#include "BrainModelSurface.h"

/// This class is used to unproject cell projections onto a surface
class CellProjectionUnprojector {

   private:
      /// position of unprojected cell
      float xyz[3];
      
      /// compute a projection point
      void  computeProjectionPoint(CellProjection& cp, float projection[3]);
              
      /// Unproject an inside triangle projection
      void unprojectInsideTriangle(CellProjection& cp, const CoordinateFile& cf);
      
      /// Unproject an outside triangle projection
      void unprojectOutsideTriangle(CellProjection& cp, const CoordinateFile& cf);
   
      /// unproject a cell projection
      void unprojectCellProjection(CellProjection& cp, const CoordinateFile& s);
      
   public:
      /// Constructor
      CellProjectionUnprojector();
      
      /// Destructor
      ~CellProjectionUnprojector();
      
      /// Unproject the cell projections      
      void unprojectCellProjections(CellProjectionFile& cpf, BrainModelSurface* s,
                                    CellFile& cf, const int startIndex);
      
};

#endif


