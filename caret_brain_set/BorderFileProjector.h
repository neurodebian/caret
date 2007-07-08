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



#ifndef __VE_BORDER_FILE_PROJECTOR_H__
#define __VE_BORDER_FILE_PROJECTOR_H__

class QProgressDialog;
class QWidget;

class BorderFile;
class BorderProjectionFile;
class BrainModelSurface;
class BrainModelSurfacePointProjector;

/// This class is used to project a BorderFile to a BrainModelSurface and store the 
/// results in a border projection file.
class BorderFileProjector {
   public:
      /// Constructor
      BorderFileProjector(const BrainModelSurface* bmsIn,
                          const bool barycentricModeIn);
                          
      /// Destructor
      ~BorderFileProjector();
      
      /// Project the border file to the border projection file using the BrainModelSurface
      /// passed to the constructor.
      void projectBorderFile(const BorderFile* bf,
                             BorderProjectionFile* bpf,
                             QWidget* progressDialogParent);
                             
   private:
      /// used to project the border points
      BrainModelSurfacePointProjector* pointProjector;
      
      /// barycentric projection flag
      bool barycentricMode;
      
};

#endif // __VE_BORDER_FILE_PROJECTOR_H__

