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



#ifndef __VE_GUI_MAP_FMRI_MAIN_WINDOW_H__
#define __VE_GUI_MAP_FMRI_MAIN_WINDOW_H__

#include <string>

#include <qmainwindow.h>

class GuiMapFmriMainWindowAtlasMenu;
class GuiMapFmriMainWindowFileMenu;
class GuiMapFmriMainWindowHelpMenu;

class QComboBox;
class QHBox;
class QLabel;
class QLineEdit;
class QListBox;
class QProgressDialog;
class QPushButton;
class QRadioButton;

/// Main Window for fMRI data mapper application.
class GuiMapFmriMainWindow : public QMainWindow {
   Q_OBJECT

   public:
      /// Constructor
      GuiMapFmriMainWindow();

      /// Destructor
      ~GuiMapFmriMainWindow();

      /// show selected atlas
      void showSelectedAtlas();
      
      /// display a progress dialog
      bool updateProgressDialog(const int progressCount, 
                                const int totalProgressCount,
                                const std::string& msg);
                                
   private slots:
      /// slot called when select working directory pushbutton pressed
      void slotSelectWorkingDirectory();
      
      /// slot called when volume Add pushbutton pressed
      void slotVolumeAdd();
      
      /// slot called when volume Remove pushbutton pressed
      void slotVolumeRemove();
      
      /// slot called when atlas selection pushbutton pressed
      void slotAtlasSelect();
      
      /// slot called when individual parameters pushbutton pressed
      void slotIndiviualParameters();
      
      /// slot called when surfce add pushbutton pressed
      void slotSurfaceAdd();
      
      /// slot called when surface remove pushbutton pressed
      void slotSurfaceRemove();
      
      /// slot called when metric naming pushbutton pressed
      void slotMetricNaming();
      
      /// slot called when algorithm pushbutton pressed
      void slotAlgorithm();
      
      /// slot called when Map to Atlas or Map to Individual radio button pressed
      void slotMappingSurfaceType(int item);
      
      /// slot called when run mapper pushbutton pressed
      void slotRunMapper();
      
      /// slot called when a volume type is selected
      void slotVolumeType(int item);
      
   private:
      
      /// Add atlas surfaces
      void addAtlasSurfaces();
      
      /// Add individual surfaces
      void addIndividualSurfaces();
      
      /// Called when application wants to close.
      void closeEvent(QCloseEvent* event);

      /// Copy the spec file from the atlas to a subdirectory of the current directory.
      std::string copyAtlasSpecFile(const std::string& directory,
                                    const std::string& subdirectory,
                                    const std::string& atlasSpecFileName,
                                    const bool copyDataFiles);
                             
      /// create the working directory section
      QWidget* createWorkingDirectorySection(QWidget* parent);
      
      /// create the volume section
      QWidget* createVolumeSection(QWidget* parent);
      
      /// create the mapping surface type section
      QWidget* createMappingSurfaceTypeSection(QWidget* parent);
      
      /// create the mapping surface section
      QWidget* createMappingSurfaceSection(QWidget* parent);
      
      /// create the button section
      QWidget* createButtonSection(QWidget* parent);
      
      /// enable mapping surface type options
      void enabledMappingSurfaceTypeItems();
      
      /// load volumes into the volume list box
      void loadVolumeListBox();
      
      /// load surfaces into the surface list box
      void loadSurfaceListBox();
      
      /// read the volume dimensions
      void readVolumeDimensions();
      
      /// show the individual's orientation
      void showIndivOrientation();
      
      /// the atlas menu
      GuiMapFmriMainWindowAtlasMenu* atlasMenu;

      /// the file menu
      GuiMapFmriMainWindowFileMenu* fileMenu;

      /// the help menu
      GuiMapFmriMainWindowHelpMenu* helpMenu;

      /// working directory line edit
      QLineEdit* workingDirectoryLineEdit;
      
      /// combo box for volume type
      QComboBox* volumeTypeComboBox;
      
      /// horizontal box for volume orientation
      QHBox* volumeOrientationHBox;
      
      /// volume orientation X combo box
      QComboBox* volumeXOrientationComboBox;

      /// volume orientation Y combo box
      QComboBox* volumeYOrientationComboBox;

      /// volume orientation Z combo box
      QComboBox* volumeZOrientationComboBox;
      
      /// horizontal box for volume byte swap
      QHBox* volumeByteSwapHBox;
      
      /// volume byte swap yes radio button
      QRadioButton* volumeByteSwapYesRadioButton;
      
      /// volume byte swap no radio button
      QRadioButton* volumeByteSwapNoRadioButton;
      
      /// horizontal box for volume raw dimensions
      QHBox* volumeRawDimensionsHBox;
      
      /// volume raw dimensions X line edit
      QLineEdit* volumeDimensionXLineEdit;
      
      /// volume raw dimensions Y line edit
      QLineEdit* volumeDimensionYLineEdit;
      
      /// volume raw dimensions Z line edit
      QLineEdit* volumeDimensionZLineEdit;
      
      /// list box for volume selection
      QListBox* volumeSelectionListBox;
      
      /// map to atlas surface radio button
      QRadioButton* mapToAtlasSurfaceRadioButton;
      
      /// map to individual surface radio button
      QRadioButton* mapToIndividualSurfaceRadioButton;
      
      /// map to atlas select button
      QPushButton* atlasSelectPushButton;
      
      /// map to indiv parameters pushbutton
      QPushButton* indivParametersPushButton;
      
      /// atlas surface label
      QLabel* surfaceTypeAtlasLabel;
      
      /// indiv surface orientationlabel
      QLabel* individualSurfaceOrientationLabel;
      
      /// surface selection list box
      QListBox* surfaceSelectionListBox;
      
      /// the progress dialog
      QProgressDialog* progressDialog;
};

#endif // __VE_GUI_MAP_FMRI_MAIN_WINDOW_H__


