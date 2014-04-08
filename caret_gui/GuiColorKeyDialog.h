#ifndef __GUI_COLOR_KEY_DIALOG_H__
#define __GUI_COLOR_KEY_DIALOG_H__

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

#include <vector>

#include <QLabel>

#include "WuQDialog.h"
#include "SceneFile.h"

class BrainSet;
class GuiColorKeyLabel;
class QGridLayout;
class QMouseEvent;

/// dialog for viewing colors assigned to names
class GuiColorKeyDialog : public WuQDialog { 
   Q_OBJECT
   
   public:
      /// types for color keys
      enum COLOR_KEY {
         /// areal estimation
         COLOR_KEY_AREAL_ESTIMATION,
         /// borders
         COLOR_KEY_BORDERS,
         /// cells
         COLOR_KEY_CELLS,
         /// foci
         COLOR_KEY_FOCI,
         /// paint file
         COLOR_KEY_PAINT,
         /// probabilistic atlas
         COLOR_KEY_PROBABILISTIC_ATLAS,
         /// volume paint
         COLOR_KEY_VOLUME_PAINT,
         /// volume probabilistic atlas
         COLOR_KEY_VOLUME_PROBABILISTIC_ATLAS
      };
      
      // constructor
      GuiColorKeyDialog(QWidget* parent, const COLOR_KEY colorKeyIn);
      
      // destructor
      ~GuiColorKeyDialog();
      
      /// save scene
      SceneFile::SceneClass saveScene();
      
      /// show scene
      void showScene(const SceneFile::SceneClass sc,
                     const int screenMaxX,
                     const int screenMaxY,
                     const int mainWindowSceneX,
                     const int mainWindowSceneY,
                     const int mainWindowX,
                     const int mainWindowY);

      /// get the title for the window
      static QString getTitleOfColorKey(const COLOR_KEY cc);
      
   public slots:
      // update the dialog
      void updateDialog();
   
   protected slots:
      // called when a color label is clicked
      void slotColorLabelClicked(const int);
      
      // called when a name label is clicked
      void slotNameLabelClicked(const QString& name);
      
      // clear highlighting
      void slotClearHighlighting();
      
   protected:
      /// name and color holder
      class NameAndColor {
         public:
            /// constructor
            NameAndColor(const QString& nameIn,
                         const int redIn,
                         const int greenIn,
                         const int blueIn);
                         
            /// destructor
            ~NameAndColor();
            
            /// less than operator
            bool operator<(const NameAndColor& nac) const;
            
            /// equality operator
            bool operator==(const NameAndColor& nac) const;
            
            /// name 
            QString name;
            
            /// red value
            int red;
            
            /// green value
            int green;
            
            /// blue value
            int blue;
            
            /// highlighted flag
            bool highlightedFlag;
      };
      
      // update areal estimation key
      void updateArealEstimationKey(BrainSet* bs,
                                    std::vector<NameAndColor>& names);
      
      // update border key
      void updateBorderKey(BrainSet* bs,
                           std::vector<NameAndColor>& names);
      
      // update cell key
      void updateCellKey(BrainSet* bs,
                         std::vector<NameAndColor>& names);
      
      // update foci key
      void updateFociKey(BrainSet* bs,
                         std::vector<NameAndColor>& names);
      
      // update paint key
      void updatePaintKey(BrainSet* bs,
                          std::vector<NameAndColor>& names);
      
      // update prob atlas key
      void updateProbAtlasKey(BrainSet* bs,
                              std::vector<NameAndColor>& names);
      
      // update volume paint key
      void updateVolumePaintKey(BrainSet* bs,
                                std::vector<NameAndColor>& names);
      
      // update volume prob atlas key
      void updateVolumeProbAtlasKey(BrainSet* bs,
                                    std::vector<NameAndColor>& names);
      
      // display study matching a foci color
      void displayStudyMatchingFociColor(const QString& colorName);
      
      /// the names and colors in the dialog
      std::vector<NameAndColor> nameTable;
   
      /// update a color label
      void updateColorLabel(const int indx);
      
      /// type of color key in the dialog
      COLOR_KEY colorKey;
      
      /// layout for color grid
      QGridLayout* colorGridLayout;
      
      /// color labels in the color grid
      std::vector<GuiColorKeyLabel*> colorLabelsInColorGrid;
      
      /// name labels in the color grid
      std::vector<GuiColorKeyLabel*> nameLabelsInColorGrid;
};

/// class for a color key label
class GuiColorKeyLabel : public QLabel {
   Q_OBJECT
   
   public:
      // constructor
      GuiColorKeyLabel(QWidget* parent = 0);
      
      // destructor
      ~GuiColorKeyLabel();
      
      // set the area name
      void setAreaName(const QString& areaNameIn);
      
      // set name table index
      void setNameTableIndex(const int indx);
      
   signals:
      // emitted when the mouse is pressed
      void signalShowAreaName(const QString&);
      
      // emitted when the mouse is pressed
      void signalNameTableIndex(const int);
      
   protected:
      // called when a mouse button is pressed over this widget
      void mousePressEvent(QMouseEvent* me);
      
      // name of area associated with this label
      QString areaName;
      
      // index in name table
      int nameTableIndex;
};


#endif // __GUI_COLOR_KEY_DIALOG_H__
