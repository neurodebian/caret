
#ifndef __CONTOUR_CELL_COLOR_FILE_H__
#define __CONTOUR_CELL_COLOR_FILE_H__

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

#include "ColorFile.h"
#include "MDPlotFile.h"
#include "NeurolucidaFile.h"

/// class for contour cell colors file
class ContourCellColorFile : public ColorFile {
   public:
      /// Constructor
      ContourCellColorFile();
                
      /// Destructor
      ~ContourCellColorFile();
      
      /// import colors from an MD Plot File
      void importMDPlotFileColors() throw (FileException);
      
      /// import colors from an Neurolucida File
      void importNeurolucidaFileColors(const NeurolucidaFile& nf) throw (FileException);
      
   protected:
};

#endif // __CONTOUR_CELL_COLOR_FILE_H__

