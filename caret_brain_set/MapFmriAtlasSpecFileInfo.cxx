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

#include <algorithm>
#include <iostream>

#include "BrainSet.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MapFmriAtlasSpecFileInfo.h"
#include "SpecFile.h"

/** 
 * Constructor.
 */
MapFmriAtlasSpecFileInfo::MapFmriAtlasSpecFileInfo(const QString& specFileNameIn)
{
   dataValid = false;
   
   SpecFile sf;
   sf.setSorting(SpecFile::SORT_NAME);
   try {
      //
      // Get path to spec file and its data files
      //
      specFilePath = FileUtilities::dirname(specFileNameIn);

      //
      // Do not allow the spec file to be sorted
      //
      //sf.setSorting(SpecFile::SORT_NONE);
      
      //
      // Read the spec file
      //
      sf.readFile(specFileNameIn);
      
      //
      // Get the description from the spec file
      //
      description = sf.getHeaderTag("description");
      if (description.isEmpty()) {
         description = FileUtilities::basename(specFileNameIn);
      }
      
      //
      // Get the fiducial coordinate files
      //
      coordFiles.clear();
      for (int i = 0; i < sf.fiducialCoordFile.getNumberOfFiles(); i++) {
         coordFiles.push_back(sf.fiducialCoordFile.getFileName(i));
      }
      
      //
      // get the average coordinate file
      //
      averageCoordinateFile = "";
      if (sf.averageFiducialCoordFile.getNumberOfFiles() > 0) {
         averageCoordinateFile = sf.averageFiducialCoordFile.getFileName(0);
      }
      
      //
      // There should be only one topology file
      //
      if (sf.closedTopoFile.getNumberOfFiles() > 0) {
         topoFile  = sf.closedTopoFile.getFileName(0);
         if (sf.closedTopoFile.getNumberOfFiles() > 1) {
            std::cout << "INFO: atlas spec file has more than one topology file. "
                      << "First topology file used, others ignored: "
                      << specFileNameIn.toAscii().constData() << std::endl;
         }
      }
      
      //
      // Get structure and species
      //
      structure = sf.getStructure().getTypeAsString();
      species = sf.getSpecies().getName();
      
      //
      // Get the space
      //
      space = sf.getSpace().getName();
      if (space.isEmpty()) {
         space = "UNKNOWN";
      }
      
      //
      // Get the metric name hint
      //
      metricNameHint = sf.getHeaderTag("metric_name_hint");
      
      //
      // spec file is valid if it has both topo and coord files
      //
      if ((coordFiles.empty() == false) && 
          (topoFile.isEmpty() == false)) {
         dataValid = true;
      }
      
      if (DebugControl::getDebugOn()) {
         QString msg;
         if (topoFile.isEmpty()) {
            msg += "\n   has no closed topology file.";
         }
         if (averageCoordinateFile.isEmpty()) {
            msg += "\n   has no average coordinate file.";
         }
         if (coordFiles.empty()) {
            msg += "\n   has no fiducial coordinate files.";
         } 
         if (msg.isEmpty() == false) {
            std::cout << "INFO: Atlas space \""
                      << space.toAscii().constData()
                      << "\" structure \""
                      << structure.toAscii().constData()
                      << "\""
                      << msg.toAscii().constData()
                      << "\n   from spec file "
                      << FileUtilities::basename(specFileNameIn).toAscii().constData()
                      << std::endl;
         }
      }
   }
   catch (FileException& e) {
   }
}

/**
 * Destructor.
 */
MapFmriAtlasSpecFileInfo::~MapFmriAtlasSpecFileInfo()
{
}

/**
 * read the atlases.
 */
void 
MapFmriAtlasSpecFileInfo::getAtlases(BrainSet* bs,
                                     std::vector<MapFmriAtlasSpecFileInfo>& atlases)
{
   atlases.clear();
   
   //
   // Look for atlas spec files in Caret installation directory.
   //
   QString atlasFilesDirectory = bs->getCaretHomeDirectory();
   atlasFilesDirectory.append("/data_files/fmri_mapping_files");
   std::vector<QString> files;
   QString fileExt("*");
   fileExt.append(SpecFile::getSpecFileExtension());
   FileUtilities::findFilesInDirectory(atlasFilesDirectory,
                                       QStringList(fileExt),
                                       files);
   
   //
   // Process each of the atlas spec files
   //
   for (int i = 0; i < static_cast<int>(files.size()); i++) {
      //
      // Prepend path to name of atlas spec file
      //
      QString name(atlasFilesDirectory);
      name.append("/");
      name.append(files[i]);
      
      //
      // See if spec files is valid
      //
      MapFmriAtlasSpecFileInfo asfi(name);
      if (asfi.getDataValid()) {
         atlases.push_back(asfi);
      }
      else {
         std::cout << "WARNING: invalid atlas spec file: "
                   << name.toAscii().constData() << std::endl;
      }
   }
   
   //
   // Sort atlas spec file info
   //
   std::sort(atlases.begin(), atlases.end());
}
