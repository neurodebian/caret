
#ifndef __FOCI_PROJECTION_FILE_H__
#define __FOCI_PROJECTION_FILE_H__

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

#include "CellProjectionFile.h"

/// class for foci projection file
class FociProjectionFile : public CellProjectionFile {
   public:
      /// Constructor
      FociProjectionFile();
                
      /// Destructor
      ~FociProjectionFile();
      
      /// write the file's memory in caret6 format to the specified name
      virtual QString writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException);

    /// write the file's memory in caret7 format to the specified name
    virtual QString writeFileInCaret7Format(const QString& filenameIn, 
                                            Structure structure,
                                            const ColorFile* colorFileIn, 
                                            const bool useCaret7ExtensionFlag) throw (FileException);
    
   protected:
};


#endif // __FOCI_PROJECTION_FILE_H__

