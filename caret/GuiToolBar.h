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


#ifndef __GUI_TOOLBAR_H__
#define __GUI_TOOLBAR_H__

#include <vector>

#include <QToolBar>

class GuiBrainModelOpenGL;
class GuiMainWindow;

#include "BrainModelSurface.h"

class GuiBrainSetAndModelSelectionControl;
class QComboBox;
class QDoubleSpinBox;
class QMainWindow;
class QToolButton;

/// This class creates a toolbar for use in main and viewing windows
class GuiToolBar : public QToolBar {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiToolBar(QMainWindow* parent, GuiMainWindow* mainWindowIn,
                 GuiBrainModelOpenGL* brainModelOpenGLIn,
                 const bool mainWindowFlag);
      
      /// Destructor
      ~GuiToolBar();
      
      /// load the models into the model combo box
      void loadModelComboBox();
      
      /// update the view controls based upon the loaded brain model
      void updateViewControls();
      
      /// update all toolbars (typically called when new surfaces loaded)
      static void updateAllToolBars(const bool additionalFilesLoaded);
      
      /// called when a standard view is selected
      void setViewSelection(const BrainModel::STANDARD_VIEWS standardView);      
      
      /// update mouse mode combo box
      void updateMouseModeComboBox();
      
   signals:
      /// Signal emitted when the model selection combo box is changed.  Normally,
      /// this signal does not need to be used by the parent of the toolbar.  The
      /// toolbar will update the model in the BrainModelOpenGL object that is
      /// passed to the constructor.
      void modelSelection(int modelNum);
      
      /// Signal emitted when rotation axis is changed.   Normally,
      /// this signal does not need to be used by the parent of the toolbar.  The
      /// toolbar will update the model in the BrainModelOpenGL object that is
      /// passed to the constructor.
      void rotationAxisSelection(int rotNum);
      
   public slots:
      /// called when model is selected
      void setModelSelection(int modelNum);
      
      /// called when rotation axis is selected
      void setRotationSelection(int axis);
      
      /// called when X slice changed
      void volumeSliceXChanged(double value);
      
      /// called when Y slice changed
      void volumeSliceYChanged(double value);
      
      /// called when Z slice changed
      void volumeSliceZChanged(double value);
      
      /// called when volume view axis changed
      void volumeAxisChanged(int value);
      
      /// called when volume stereotaxic coordinates is toggled
      void volumeStereotaxicCoordinatesSlot(bool b);
      
      /// called when a mouse mode is selected
      void slotMouseModeComboBoxActivated(int item);
      
      /// view mode button clicked
      void slotViewModeButtonClicked();
      
   protected:
      /// called when toolbar is resized
      void resizeEvent(QResizeEvent* e);
      
      /// Caret's main window
      GuiMainWindow* mainWindow;
      
      /// model selection combo box
      GuiBrainSetAndModelSelectionControl* modelFileComboBox;
      
      /// rotation combo box
      QComboBox* rotationAxisComboBox;
      
      /// yoke toggle button
      QToolButton* yokeButton;
      
      /// volume underlay only button
      QToolButton* volumeUnderlayOnlyButton;
      
      /// Medial View Tool Button
      QToolButton* medialViewToolButton;
      
      /// Lateral View Tool Button
      QToolButton* lateralViewToolButton;
      
      /// Anterior View Tool Button
      QToolButton* anteriorViewToolButton;
      
      /// Posterior View Tool Button
      QToolButton* posteriorViewToolButton;
      
      /// Dorsal View Tool Button
      QToolButton* dorsalViewToolButton;
      
      /// Ventral View Tool Button
      QToolButton* ventralViewToolButton;
      
      /// Reset View Tool Button
      QToolButton* resetViewToolButton;
      
      /// X 90 Rotation Tool Button
      QToolButton* xRotation90ToolButton;
      
      /// Y 90 Rotation Tool Button
      QToolButton* yRotation90ToolButton;
      
      /// Z 90 Rotation Tool Button
      QToolButton* zRotation90ToolButton;
      
      /// volume slice spin box
      QDoubleSpinBox* volumeSpinBoxSliceX;
      
      /// volume slice spin box
      QDoubleSpinBox* volumeSpinBoxSliceY;
      
      /// volume slice spin box
      QDoubleSpinBox* volumeSpinBoxSliceZ;
      
      /// volume view axis combo box
      QComboBox* volumeAxisComboBox;
      
      /// show stereotaxic coordinates button
      QToolButton* volumeStereotaxicCoordinatesToolButton;
      
      /// Windows OpenGL widget
      GuiBrainModelOpenGL* brainModelOpenGL;
      
      /// Keeps track of toolbars so that they can be updated when
      /// the models change.
      static std::vector<GuiToolBar*> allToolBars;
      
      /// mouse mode combo box
      QComboBox* mouseModeComboBox;
      
};

#ifdef __GUI_TOOLBAR_MAIN__
std::vector<GuiToolBar*> GuiToolBar::allToolBars;
#endif // __GUI_TOOLBAR_MAIN__

#endif // __GUI_TOOLBAR_H__


