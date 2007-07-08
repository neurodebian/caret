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



#include <qfileinfo.h>

#include "AtlasDirectoryInfo.h"
#include "FileUtilities.h"

/**
 * Constructor.
 */
AtlasDirectoryInfo::AtlasDirectoryInfo(const std::string& atlasDirNameIn, 
                                       const bool saveInPreferencesIn) 
{
   QFileInfo atlasFileInfo(atlasDirNameIn.c_str());
   //
   // atlasDirNameIn may be either the "map_fmri_atlas.directory" file
   // or the top level atlas directory.
   //
   if (atlasFileInfo.isDir()) {
      atlasDirectory.assign(atlasDirNameIn);
      const int len = atlasDirectory.length();
      if (len >= 2) { // chop off trailing directory slash
         if (atlasDirectory[len - 1] == '/') {
            atlasDirectory.resize(len - 1);
         }
      }

      mappingSurfaceFileDirectory.assign(atlasDirectory);
      mappingSurfaceFileDirectory.append("/MAP_MASTER_FILES");

      mappingSurfaceListFile.assign(mappingSurfaceFileDirectory);
      mappingSurfaceListFile.append("/map_fmri_atlas.directory");
   }
   else if (atlasFileInfo.isFile()) {
      mappingSurfaceListFile.assign(atlasDirNameIn);
      mappingSurfaceFileDirectory.assign(FileUtilities::dirname(atlasDirNameIn));
      atlasDirectory.assign(FileUtilities::dirname(mappingSurfaceFileDirectory));
   }         
   saveInPreferences = saveInPreferencesIn;
}

/**
 * This item valid
 */
bool 
AtlasDirectoryInfo::isValid() const 
{
   QFileInfo atlasDirInfo(atlasDirectory.c_str());
   QFileInfo mapSurfaceInfo(mappingSurfaceFileDirectory.c_str());
   QFileInfo mapSurfaceListInfo(mappingSurfaceListFile.c_str());
   
   return atlasDirInfo.isDir() &&
          mapSurfaceInfo.isDir() &&
          mapSurfaceListInfo.isFile();
}
