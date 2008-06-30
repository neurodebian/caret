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



#ifndef __VE_DISPLAY_SETTINGS_COCOMAC_H__
#define __VE_DISPLAY_SETTINGS_COCOMAC_H__

#include "DisplaySettings.h"

/// DisplaySettingsCoCoMac is a class that maintains parameters for controlling
/// the display of CoCoMac Files.
class DisplaySettingsCoCoMac : public DisplaySettings {
   public:
      /// Type of connection display
      enum CONNECTION_DISPLAY_TYPE {
         CONNECTION_DISPLAY_AFFERENT,
         CONNECTION_DISPLAY_EFFERENT,
         CONNECTION_DISPLAY_AFFERENT_AND_EFFERENT,
         CONNECTION_DISPLAY_AFFERENT_OR_EFFERENT
      };
      
      /// Constructor
      DisplaySettingsCoCoMac(BrainSet* bs);
      
      /// Constructor
      ~DisplaySettingsCoCoMac();
      
      /// get the connection display type
      CONNECTION_DISPLAY_TYPE getConnectionDisplayType() const { return connectionDisplayType; }
      
      /// set the connection display type
      void setConnectionDisplayType(const CONNECTION_DISPLAY_TYPE cdt) { connectionDisplayType = cdt; }
            
      /// get the selected paint column to use for area names
      int getSelectedPaintColumn() const { return selectedPaintColumn; }
      
      /// set the selected paint column to use for area names
      void setSelectedPaintColumn(const int col) { selectedPaintColumn = col; }
      
      /// get the selected node for cocomac info
      int getSelectedNode() const { return selectedNode; }
      
      /// set the selected node for cocomac info
      void setSelectedNode(const int node) { selectedNode = node; }
      
      /// get identification info produced when node coloring
      QString getIDInfo() { QString s(idInfo); idInfo = ""; return s; }
      
      /// set identification info produced when node coloring
      void setIDInfo(const QString& s) { idInfo = s; }
      
      /// Reinitialize all display settings
      void reset();
      
      /// Update any selections due to changes in loaded metric file
      void update();
      
      /// apply a scene (set display settings)
      virtual void showScene(const SceneFile::Scene& scene, QString& errorMessage) ;
      
      /// create a scene (read display settings)
      virtual void saveScene(SceneFile::Scene& scene, const bool onlyIfSelected,
                             QString& errorMessage);
                       

   private:
      /// connection display type
      CONNECTION_DISPLAY_TYPE connectionDisplayType;
      
      /// selected paint column for area names
      int selectedPaintColumn;

      /// selected node
      int selectedNode;     
      
      /// id info produced by node coloring
      QString idInfo;
};

#endif
