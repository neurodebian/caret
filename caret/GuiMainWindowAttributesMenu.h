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


#ifndef __GUI_MAIN_WINDOW_ATTRIBUTES_MENU_H__
#define __GUI_MAIN_WINDOW_ATTRIBUTES_MENU_H__

#include <QMenu>

class GuiMainWindow;
class GuiMainWindowAttributesActions;

/// the main windows attributes menu
class GuiMainWindowAttributesMenu : public QMenu {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMainWindowAttributesMenu(GuiMainWindow* parent);
      
      /// Constructor
      ~GuiMainWindowAttributesMenu();
      
   private:
      /// create the area colors menu
      void createAreaColorsSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the lat/lon sub menu
      void createLatLonSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the metric sub menu
      void createMetricSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the models sub menu
      void createModelsSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the paint sub menu
      void createPaintSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the prob atlas sub menu
      void createProbAtlasSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the palette sub menu
      void createPaletteSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the parameters sub menu
      void createParametersSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the study metadata sub menu
      void createStudyMetaDataSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the surface shape sub menu
      void createSurfaceShapeSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// create the vectors sub menu
      void createVectorsSubMenu(GuiMainWindowAttributesActions* attributesActions);
      
      /// Create the vocabulary sub menu.
      void createVocabularySubMenu(GuiMainWindowAttributesActions* attributesActions);

      /// area colors menu
      QMenu* areaColorsSubMenu;
      
      /// lat/lon sub menu
      QMenu* latLonSubMenu;
      
      /// Metric sub menu
      QMenu* metricSubMenu;
      
      /// Models sub menu
      QMenu* modelsSubMenu;
      
      /// Paint sub menu
      QMenu* paintSubMenu;
      
      /// Prob atlas sub menu
      QMenu* probAtlasSubMenu;
      
      /// Palette sub menu
      QMenu* paletteSubMenu;
      
      /// Parameters sub menu
      QMenu* parametersSubMenu;
      
      /// Rgb paint sub menu
      QMenu* rgbPaintSubMenu;
      
      /// study meta data sub menu
      QMenu* studyMetaDataMenu;
      
      /// surface shape sub menu
      QMenu* surfaceShapeSubMenu;
      
      /// vectors sub menu
      QMenu* vectorsSubMenu;
      
      /// vocabulary sub menu
      QMenu* vocabularySubMenu;
};

#endif  // __GUI_MAIN_WINDOW_ATTRIBUTES_MENU_H__

