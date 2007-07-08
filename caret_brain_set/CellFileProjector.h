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



#ifndef __VE_CELL_FILE_PROJECTOR_H__
#define __VE_CELL_FILE_PROJECTOR_H__

class QProgressDialog;

class CellData;
class CellProjection;
class CellProjectionFile;
class CoordinateFile;
class TopologyFile;
class BrainModelSurface;
class BrainModelSurfacePointProjector;

/// This class is used to project a CellFile to a BrainModelSurface and store the 
/// results in a cell projection file.
class CellFileProjector {
   public:
      /// Type of hemisphere projection
      enum PROJECTION_TYPE {
         PROJECTION_TYPE_ALL,
         PROJECTION_TYPE_HEMISPHERE_ONLY,
         PROJECTION_TYPE_FLIP_TO_MATCH_HEMISPHERE
      };
      
      /// Constructor
      CellFileProjector(const BrainModelSurface* bmsIn);
                          
      /// Destructor
      ~CellFileProjector();
      
      /// Project the cell projection file using the BrainModelSurface
      /// passed to the constructor.
      void projectFile(CellProjectionFile* cpf,
                       const int startWithCell,
                       const PROJECTION_TYPE projectionType,
                       const float projectOntoSurfaceAboveDistance,
                       const bool projectOntoSurface,
                       QWidget* progressDialogParent);
                           
      /// project a single cell 
      void projectCell(CellProjection& cp, 
                       const PROJECTION_TYPE projectionType,
                       const float projectOntoSurfaceAboveDistance,
                       const bool projectOntoSurface);
                               
      /// update cell number if projected to the cell projection file
      //int updateCellProjection(const CellFile* cf, const int cellNumber, 
      //                         CellProjectionFile* cpf,
      //                         const PROJECTION_TYPE projectionType);
                               
   private:
      /// used to project the cell points
      BrainModelSurfacePointProjector* pointProjector;
      
      /// hemisphere of brain model
      Structure hemisphere;
      
      /// coordinate file used for projecting
      const CoordinateFile* coordinateFile;
      
      /// topology file used for projecting
      const TopologyFile* topologyFile;
      
      /// the brain model surface
      const BrainModelSurface* bms;
      
      /// fiducial surface flag
      bool fiducialSurfaceFlag;
};

#endif // __VE_CELL_FILE_PROJECTOR_H__
