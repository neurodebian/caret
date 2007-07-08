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



#ifndef __VE_DISPLAY_SETTINGS_H__
#define __VE_DISPLAY_SETTINGS_H__

#include <vector>

#include "SceneFile.h"

class BrainSet;
class ColorFile;
class GiftiNodeDataFile;
class NodeAttributeFile;

/// DisplaySettings is an abstract class for controlling the display
/// of data files.
class DisplaySettings {
   public:
      /// destructor
      virtual ~DisplaySettings();
      
      /// reinitialize all display settings
      virtual void reset() = 0;
      
      /// update any selections due to changes with loaded data files
      virtual void update() = 0;
   
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene,
                             QString& errorMessage) = 0;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene,
                             const bool onlyIfSelectedFlag) = 0;
        
      /// for node attribute files - all column selections for each surface are the same
      virtual bool columnSelectionsAreTheSame(const int bm1, const int bm2) const;
      
   protected:
      /// constructor
      DisplaySettings(BrainSet* bsIn);
      
      /// Set the default value for selected column of any new surfaces
      void updateSelectedColumnIndices(const NodeAttributeFile* naf,
                                       std::vector<int>& selCol);
      
      /// Set the default value for selected column of any new surfaces
      void updateSelectedColumnIndices(const GiftiNodeDataFile* naf,
                                       std::vector<int>& selCol);
      
      /// Set the default value for for a column
      void updateSelectedColumnIndex(const NodeAttributeFile* naf,
                                     int& selCol);
       
      /// Set the default value for for a column
      void updateSelectedColumnIndex(const GiftiNodeDataFile* naf,
                                     int& selCol);
       
      /// apply a scene for node attribute display settings
      void showSceneNodeAttribute(const SceneFile::SceneClass& sc,
                                  const QString& infoName,
                                  const NodeAttributeFile* naf,
                                  const QString& fileTypeName,
                                  std::vector<int>& selectedColumn,
                                  QString& errorMessage);
                                  
      /// apply a scene for node attribute display settings
      void showSceneNodeAttribute(const SceneFile::SceneClass& sc,
                                  const QString& infoName,
                                  const GiftiNodeDataFile* naf,
                                  const QString& fileTypeName,
                                  std::vector<int>& selectedColumn,
                                  QString& errorMessage);
                                  
      /// create a scene for node attribute display settings
      void saveSceneNodeAttribute(SceneFile::SceneClass& sc,
                                  const QString& infoName,
                                  const NodeAttributeFile* naf,
                                  const std::vector<int>& selectedColumn);
      
      /// create a scene for node attribute display settings
      void saveSceneNodeAttribute(SceneFile::SceneClass& sc,
                                  const QString& infoName,
                                  const GiftiNodeDataFile* naf,
                                  const std::vector<int>& selectedColumn);
      
      /// apply a scene for a node attribute column
      void showSceneNodeAttributeColumn(const SceneFile::SceneInfo* si,
                                        const NodeAttributeFile* naf,
                                        const QString& fileTypeName,
                                        int& displayColumn,
                                        QString& errorMessage);
                                        
      /// apply a scene for a node attribute column
      void showSceneNodeAttributeColumn(const SceneFile::SceneInfo* si,
                                        const GiftiNodeDataFile* naf,
                                        const QString& fileTypeName,
                                        int& displayColumn,
                                        QString& errorMessage);
                                        
      /// create a scene for node attribute display settings
      void saveSceneNodeAttributeColumn(SceneFile::SceneClass& sc,
                                        const QString& infoName,
                                        const NodeAttributeFile* naf,
                                        const int displayColumn);
      
      /// create a scene for node attribute display settings
      void saveSceneNodeAttributeColumn(SceneFile::SceneClass& sc,
                                        const QString& infoName,
                                        const GiftiNodeDataFile* naf,
                                        const int displayColumn);
      
      /// apply scene for a color file
      void showSceneColorFile(const SceneFile::SceneInfo& si,
                              ColorFile* cf,
                              const QString& errorName,
                              QString& errorMessage);
      
      /// save color file settings
      void saveSceneColorFile(SceneFile::SceneClass& sc, 
                              const QString& sceneInfoName, const ColorFile* cf);
      
      /// the brain set
      BrainSet* brainSet;
};

#endif


