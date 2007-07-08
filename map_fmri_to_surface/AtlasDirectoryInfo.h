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



#ifndef __ATLAS_DIRECTORY_INFO_H__
#define __ATLAS_DIRECTORY_INFO_H__

#include <string>

/// Information about an atlas directory
class AtlasDirectoryInfo {
   private:
      /// name of atlas directory
      std::string atlasDirectory;
      
      /// name of mapping surface list file
      std::string mappingSurfaceListFile;
      
      ///  name of mapping surface list file directory
      std::string mappingSurfaceFileDirectory;
      
      /// save in preferences flag
      bool   saveInPreferences;
      
   public:
      /// Constructor
      AtlasDirectoryInfo(const std::string& atlasDirNameIn, 
                         const bool saveInPreferencesIn);
      
      /// Get name of atlas directory
      std::string getAtlasDirectory() const { return atlasDirectory; }
      
      /// Get name of mapping surface list file
      std::string getMappingSurfaceListFile() const 
                                        { return mappingSurfaceListFile; }
      
      /// Get name of mapping surface list file directory
      std::string getMappingSurfaceFileDirectory() const
                                        { return mappingSurfaceFileDirectory; }
                                        
      /// Get save in preferences flag
      bool getSaveInPreferences() const { return saveInPreferences; }
      
      /// this item is valid
      bool isValid() const ;
};


#endif // __ATLAS_DIRECTORY_INFO_H__

