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



#include <iostream>
#include <sstream>

#include <qglobal.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qprogressdialog.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtooltip.h>
#include <qvgroupbox.h>
#include <qvbox.h>

#include "AtlasSurfaceDirectoryFile.h"
#include "CaretVersion.h"
#include "FileUtilities.h"
#include "GuiMapFmriAlgorithmDialog.h"
#include "GuiMapFmriAtlasSelectionDialog.h"
#include "GuiMapFmriIndivParametersDialog.h"
#include "GuiMapFmriMainWindow.h"
#include "GuiMapFmriMainWindowAtlasMenu.h"
#include "GuiMapFmriMainWindowFileMenu.h"
#include "GuiMapFmriMainWindowHelpMenu.h"
#include "GuiMapFmriMetricNamingDialog.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "SpecFileUtilities.h"

#include "map_fmri_global_variables.h"
#include "map_fmri_prototypes.h"

/**
 * Constructor.
 */
GuiMapFmriMainWindow::GuiMapFmriMainWindow()
   : QMainWindow(0, "MapFmriToSurface 5", WDestructiveClose)
{
   progressDialog = NULL;
   
   std::string title("CARET Map fMRI v");
   title.append(caretVersionString);
   title.append(" (");
   title.append(__DATE__);
   title.append(")");
   setCaption(title.c_str());
   
   //
   // Create the file menu.
   //
   fileMenu = new GuiMapFmriMainWindowFileMenu(this);
   menuBar()->insertItem("File", fileMenu);

#ifdef Q_OS_MAC
   //
   // Remove file menu on mac since exit gets moved to apple menu.
   //
   //menuBar()->removeItem(fileMenuItem);
#endif

   //
   // Create the atlas menu.
   //
   atlasMenu = new GuiMapFmriMainWindowAtlasMenu(this);
   menuBar()->insertItem("Atlas", atlasMenu);
   
   //
   // Create the help menu.
   //
   helpMenu = new GuiMapFmriMainWindowHelpMenu(this);
   menuBar()->insertItem("Help", helpMenu);
   
#ifdef Q_OS_MAC
   //
   // Remove help menu on mac since exit gets moved to apple menu.
   //
   //menuBar()->removeItem(helpMenuItem);
#endif
   //
   // Central Widget
   //
   QVBox* vbox = new QVBox(this, "vbox");
   vbox->setMargin(5);
   vbox->setSpacing(3);
   setCentralWidget(vbox);
   
   //
   // Create the working directory section
   //
   createWorkingDirectorySection(vbox);
   
   //
   // Create the volume selection section
   //
   createVolumeSection(vbox);
   
   //
   // Create the mapping surface type selection
   //
   createMappingSurfaceTypeSection(vbox);
   
   //
   // Create the mapping surface section
   //
   createMappingSurfaceSection(vbox);
   
   //
   // Create the button section
   //
   createButtonSection(vbox);
}

/**
 * Destructor.
 */
GuiMapFmriMainWindow::~GuiMapFmriMainWindow()
{
}

/**
 * Called when the program should close.
 */
void
GuiMapFmriMainWindow::closeEvent(QCloseEvent* event)
{
   //
   // Return value of zero is YES button.
   //
   if (QMessageBox::warning(this,
                           "Map fMRI to Surface",
                           "Are you sure you want to quit ?",
                           "Yes",
                           "No") == 0) {
      readVolumeDimensions();
      fmriMapper.saveUserPreferences();
      event->accept();
   }
   else {
      event->ignore();
   }
}

/**
 * Called when a volume type is selected.
 */
void
GuiMapFmriMainWindow::slotVolumeType(int item)
{
   fmriMapper.setVolumeType(static_cast<FMRIDataMapper::VOLUME_TYPE>(item));
   
   bool orientationEnabled = true;
   bool byteSwapEnabled = true;
   bool dimensionsEnabled = true;
   
   switch(fmriMapper.getVolumeType()) {
      case FMRIDataMapper::VOLUME_TYPE_AFNI:
         orientationEnabled = false;
         byteSwapEnabled = false;
         dimensionsEnabled = false;
         break;
      case FMRIDataMapper::VOLUME_TYPE_ANALYZE:
         byteSwapEnabled = false;
         dimensionsEnabled = false;
         break;
      case FMRIDataMapper::VOLUME_TYPE_WUNIL:
         orientationEnabled = false;
         byteSwapEnabled = false;
         dimensionsEnabled = false;
         break;
      case FMRIDataMapper::VOLUME_TYPE_SPM_LPI:
      case FMRIDataMapper::VOLUME_TYPE_SPM_RPI:
         orientationEnabled = false;
         byteSwapEnabled = false;
         dimensionsEnabled = false;
         break;
      case FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
      case FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
         break;
   }
   
   volumeOrientationHBox->setEnabled(orientationEnabled);
   volumeByteSwapHBox->setEnabled(byteSwapEnabled);
   volumeRawDimensionsHBox->setEnabled(dimensionsEnabled);
}

/**
 * Slot called to change working directory
 */
void
GuiMapFmriMainWindow::slotSelectWorkingDirectory()
{
   if (fmriMapper.getNumberOfSurfaces() > 0) {
      QMessageBox::warning(this, "Select Directory Error",
                           "The working directory must be set prior\n"
                           "to adding surfaces for mapping.", "OK");
   }
   else {
      QFileDialog fd(this, "working-directory-select", true);
      fd.setCaption("Choose Working Directory");
      fd.setMode(QFileDialog::Directory);
      if (fd.exec() == QFileDialog::Accepted) {
         workingDirectoryLineEdit->setText(fd.selectedFile());
         QDir::setCurrent(fd.selectedFile());
      }
   }
}

/**
 * Create the working directory section.
 */
QWidget*
GuiMapFmriMainWindow::createWorkingDirectorySection(QWidget* parent)
{
   QVGroupBox* qvGroup = new QVGroupBox("Working Directory Selection", parent, "qvGroup");
   
   //
   // Horizontal box for workign directory stuff
   //
   QHBox* volumeTypeHBox = new QHBox(qvGroup, "typeHBox");
   
   //
   // Pushbutton and label for workign directory
   //
   QPushButton* workingDirectoryPushButton = new QPushButton("Select...", volumeTypeHBox, 
                                                             "workingDirectoryPushButton");
   QObject::connect(workingDirectoryPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSelectWorkingDirectory()));
   QToolTip::add(workingDirectoryPushButton,
                 "Displays dialog for selecting\n"
                 "the working directory.");
   
   workingDirectoryLineEdit = new QLineEdit(volumeTypeHBox, "workingDirectoryLineEdit");
   workingDirectoryLineEdit->setMinimumWidth(300);
   workingDirectoryLineEdit->setReadOnly(true);
   workingDirectoryLineEdit->setText(QDir::currentDirPath());
   
   return qvGroup;
}

/**
 * Create the volume section.
 */
QWidget*
GuiMapFmriMainWindow::createVolumeSection(QWidget* parent)
{
   QVGroupBox* qvGroup = new QVGroupBox("Volume Selection", parent, "qvGroup");
   
   //
   // Horizontal box for volume type
   //
   QHBox* volumeTypeHBox = new QHBox(qvGroup, "typeHBox");
   
   //
   // Label & Combo Box for volume type
   //
   new QLabel("Volume Type", volumeTypeHBox);
   volumeTypeComboBox = new QComboBox(volumeTypeHBox, "volumeTypeComboBox");
   QObject::connect(volumeTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotVolumeType(int)));
   volumeTypeComboBox->insertItem("AFNI (.HEAD)", FMRIDataMapper::VOLUME_TYPE_AFNI);
   volumeTypeComboBox->insertItem("Analyze (.hdr)", FMRIDataMapper::VOLUME_TYPE_ANALYZE);
   volumeTypeComboBox->insertItem("SPM99, SPM2 or MEDx in LPI Orientation (.hdr)", FMRIDataMapper::VOLUME_TYPE_SPM_LPI);
   volumeTypeComboBox->insertItem("SPM2 or MEDx in RPI Orientation (.hdr)", FMRIDataMapper::VOLUME_TYPE_SPM_RPI);
   volumeTypeComboBox->insertItem("WUSTL NIL 4dfp (.ifh)", FMRIDataMapper::VOLUME_TYPE_WUNIL);
   volumeTypeComboBox->insertItem("RAW - 8 Bit Signed", FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_SIGNED);
   volumeTypeComboBox->insertItem("RAW - 8 Bit Unsigned", FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_UNSIGNED);
   volumeTypeComboBox->insertItem("RAW - 16 Bit Signed", FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_SIGNED);
   volumeTypeComboBox->insertItem("RAW - 16 Bit Unsigned", FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_UNSIGNED);
   volumeTypeComboBox->insertItem("RAW - 32 Bit Signed", FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_SIGNED);
   volumeTypeComboBox->insertItem("RAW - 32 Bit Unsigned", FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_UNSIGNED);
   volumeTypeComboBox->insertItem("RAW - 64 Bit Signed", FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_SIGNED);
   volumeTypeComboBox->insertItem("RAW - 64 Bit Unsigned", FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_UNSIGNED);
   volumeTypeComboBox->insertItem("RAW - 32 Bit Floating Point", FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT);
   volumeTypeComboBox->insertItem("RAW - 64 Bit Floating Point", FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT);
   volumeTypeComboBox->setCurrentItem(fmriMapper.getVolumeType());
   //
   // Squish orientation items together
   //
   volumeTypeHBox->setFixedSize(volumeTypeHBox->sizeHint());
   
   //
   // Horizontal box for orientation
   //
   volumeOrientationHBox = new QHBox(qvGroup, "orientationHBox");
   volumeOrientationHBox->setSpacing(5);

   //
   // Get the orientation
   //   
   FMRIDataMapper::X_NEGATIVE_ORIENTATION_TYPE xOrient;
   FMRIDataMapper::Y_NEGATIVE_ORIENTATION_TYPE yOrient;
   FMRIDataMapper::Z_NEGATIVE_ORIENTATION_TYPE zOrient;
   fmriMapper.getVolumeOrientation(xOrient, yOrient, zOrient);
   //
   // Orientation label and combo boxes
   //
   new QLabel("Orientation", volumeOrientationHBox);
   volumeXOrientationComboBox = new QComboBox(volumeOrientationHBox, 
                                              "volumeXOrientationComboBox");
   volumeXOrientationComboBox->insertItem("Left (Anatomical)", 
                                          FMRIDataMapper::X_NEGATIVE_ORIENTATION_LEFT);
   volumeXOrientationComboBox->insertItem("Right (Radiological)", 
                                          FMRIDataMapper::X_NEGATIVE_ORIENTATION_RIGHT);
   volumeXOrientationComboBox->setCurrentItem(xOrient);
                                          
   volumeYOrientationComboBox = new QComboBox(volumeOrientationHBox, 
                                              "volumeYOrientationComboBox");
   volumeYOrientationComboBox->insertItem("Posterior", 
                                          FMRIDataMapper::Y_NEGATIVE_ORIENTATION_POSTERIOR);
   volumeYOrientationComboBox->insertItem("Anterior", 
                                          FMRIDataMapper::Y_NEGATIVE_ORIENTATION_ANTERIOR);
   volumeYOrientationComboBox->setCurrentItem(yOrient);
                                          
   volumeZOrientationComboBox = new QComboBox(volumeOrientationHBox, 
                                              "volumeZOrientationComboBox");
   volumeZOrientationComboBox->insertItem("Inferior", 
                                          FMRIDataMapper::Z_NEGATIVE_ORIENTATION_INFERIOR);
   volumeZOrientationComboBox->insertItem("Superior", 
                                          FMRIDataMapper::Z_NEGATIVE_ORIENTATION_SUPERIOR);
   volumeZOrientationComboBox->setCurrentItem(zOrient);
                                              
   //
   // Squish orientation items together
   //
   volumeOrientationHBox->setFixedSize(volumeOrientationHBox->sizeHint());
   
   //
   // Horizontal box for byte swap
   //
   volumeByteSwapHBox = new QHBox(qvGroup, "volumeByteSwapHBox");
   volumeByteSwapHBox->setSpacing(10);
   
   //
   // Button Group for byte swap radio buttons
   //
   QButtonGroup* volumeByteSwapButtonGroup = new QButtonGroup(volumeByteSwapHBox, 
                                                        "volumeByteSwapButtonGroup");
   volumeByteSwapButtonGroup->hide();
   
   //
   // Byte swap label and radio buttons
   //
   new QLabel("Byte Swap", volumeByteSwapHBox);
   volumeByteSwapYesRadioButton = new QRadioButton("Yes", 
                                                   volumeByteSwapHBox,
                                                   "volumeByteSwapYesRadioButton");
   volumeByteSwapButtonGroup->insert(volumeByteSwapYesRadioButton);
   volumeByteSwapNoRadioButton = new QRadioButton("No", 
                                                  volumeByteSwapHBox,
                                                  "volumeByteSwapNoRadioButton");
   volumeByteSwapButtonGroup->insert(volumeByteSwapNoRadioButton);
   
   if (fmriMapper.getVolumeByteSwapping()) {
      volumeByteSwapYesRadioButton->setChecked(true);
   }
   else {
      volumeByteSwapNoRadioButton->setChecked(true);
   }
   
   //
   // Squish byte swap items together
   //
   volumeByteSwapHBox->setFixedSize(volumeByteSwapHBox->sizeHint());
   
   //
   // Horizontal box for raw dimensions
   //
   volumeRawDimensionsHBox = new QHBox(qvGroup, "volumeRawDimensionsHBox");
   volumeRawDimensionsHBox->setSpacing(5);
   
   //
   // Get volume dimensions
   //
   int dimX, dimY, dimZ;
   fmriMapper.getRawVolumeDimensions(dimX, dimY, dimZ);
   
   //
   // Volume raw dimensions label and line edits
   //
   new QLabel("Raw Dimensions", volumeRawDimensionsHBox);
   volumeDimensionXLineEdit = new QLineEdit(volumeRawDimensionsHBox,
                                            "volumeDimensionXLineEdit");
   volumeDimensionXLineEdit->setText(QString::number(dimX));
   volumeDimensionYLineEdit = new QLineEdit(volumeRawDimensionsHBox,
                                            "volumeDimensionYLineEdit");
   volumeDimensionYLineEdit->setText(QString::number(dimY));
   volumeDimensionZLineEdit = new QLineEdit(volumeRawDimensionsHBox,
                                            "volumeDimensionZLineEdit");
   volumeDimensionZLineEdit->setText(QString::number(dimZ));
   
   //
   // Set width of dimension line edits
   //                                                                                 
   QSize lineEditDimSize = volumeDimensionXLineEdit->sizeHint();
   lineEditDimSize.setWidth(80);
   volumeDimensionXLineEdit->setFixedSize(lineEditDimSize);
   volumeDimensionYLineEdit->setFixedSize(lineEditDimSize);
   volumeDimensionZLineEdit->setFixedSize(lineEditDimSize);
   
   //
   // Squish raw dimension items together
   //
   volumeRawDimensionsHBox->setFixedSize(volumeRawDimensionsHBox->sizeHint());
   
   //
   // Horizontal box for volume selection
   //
   QHBox* volumeSelectionHBox = new QHBox(qvGroup, "volumeSelectionHBox");
   volumeSelectionHBox->setSpacing(5);
   
   //
   // Vertical box for volume selection buttons
   //
   QVBox* volumeSelectionButtonVBox = new QVBox(volumeSelectionHBox, "volumeSelectionButtonVBox");
   
   //
   // Volume Selection Buttons
   //
   QPushButton* volumeAddButton = new QPushButton("Add...", volumeSelectionButtonVBox,
                                                  "volumeAddButton");
   QObject::connect(volumeAddButton, SIGNAL(clicked()),
                    this, SLOT(slotVolumeAdd()));
   QToolTip::add(volumeAddButton, "Press this button to add a\n"
                                  "functional volumes for mapping.");
   QPushButton* volumeRemoveButton = new QPushButton("Remove", volumeSelectionButtonVBox,
                                                     "volumeRemoveButton");
   QObject::connect(volumeRemoveButton, SIGNAL(clicked()),
                    this, SLOT(slotVolumeRemove()));
   QToolTip::add(volumeRemoveButton, "Press this button to remove\n"
                                     "a functional volume.");
   QtUtilities::makeButtonsSameSize(volumeAddButton, volumeRemoveButton);
   
   //
   // List box for volume selection
   //
   volumeSelectionListBox = new QListBox(volumeSelectionHBox, "volumeSelectionListBox");
   volumeSelectionListBox->setMinimumWidth(400);
   
   //
   // Squish volume selection items together
   //
   volumeSelectionHBox->setFixedSize(volumeSelectionHBox->sizeHint());
   
   // Enable/Disable volume selections based upon volume file type   
   slotVolumeType(static_cast<int>(fmriMapper.getVolumeType()));

   return qvGroup;
}

/**
 * Show the selected atlas.
 */
void
GuiMapFmriMainWindow::showSelectedAtlas()
{
   std::string txt;
   
   if (fmriMapper.getNumberOfAtlasDirectories() > 0) {
      const int atlasNum = fmriMapper.getSelectedAtlasDirectory();
      if (atlasNum >= 0) {
         AtlasDirectoryInfo* adi = fmriMapper.getAtlasDirectoryInfo(atlasNum);
         txt.assign(FileUtilities::basename(adi->getAtlasDirectory()));         
      }
   }
   
   surfaceTypeAtlasLabel->setText(txt.c_str());
}

/**
 * Show the individual orientation
 */
void
GuiMapFmriMainWindow::showIndivOrientation()
{
   std::string x, y, z;
   fmriMapper.getIndivSurfaceOrientationLabels(x, y, z);
   std::ostringstream str;
   str << x << "-" << y << "-" << z << std::ends;
   individualSurfaceOrientationLabel->setText(str.str().c_str());
}

/**
 * Create the mapping surface type section.
 */
QWidget*
GuiMapFmriMainWindow::createMappingSurfaceTypeSection(QWidget* parent)
{
   QVGroupBox* qvGroup = new QVGroupBox("Mapping Surface Type", parent, "qvGroup");
   
   //
   // Grid layout for surface type items
   //
   QWidget* surfaceTypeWidget = new QWidget(qvGroup, "surfaceTypeWidget");
   QGridLayout * surfaceTypeGrid = new QGridLayout(surfaceTypeWidget,
                                                   2, 3, 0, 0, "surfaceTypeGrid");
                                                
   //
   // Mapping to atlas items
   //
   mapToAtlasSurfaceRadioButton = new QRadioButton("Map To Atlas", surfaceTypeWidget,
                                                   "mapToAtlasSurfaceRadioButton");
   surfaceTypeGrid->addWidget(mapToAtlasSurfaceRadioButton, 0, 0, Qt::AlignLeft);
   atlasSelectPushButton = new QPushButton("Select...", surfaceTypeWidget,
                                                       "mapToAtlasPushButton");
   QObject::connect(atlasSelectPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAtlasSelect()));
   surfaceTypeGrid->addWidget(atlasSelectPushButton, 0, 1, Qt::AlignLeft);
   QToolTip::add(atlasSelectPushButton, "Press this button to\n"
                                        "select an atlas.");
   surfaceTypeAtlasLabel = new QLabel("                                       ",
                                      surfaceTypeWidget);
   surfaceTypeGrid->addWidget(surfaceTypeAtlasLabel, 0, 2, Qt::AlignLeft);

   //
   // Mapping to individual items
   //
   mapToIndividualSurfaceRadioButton = new QRadioButton("Map To Individual", surfaceTypeWidget,
                                                   "mapToIndividualSurfaceRadioButton");
   surfaceTypeGrid->addWidget(mapToIndividualSurfaceRadioButton, 1, 0, Qt::AlignLeft);
   indivParametersPushButton = new QPushButton("Parameters...", surfaceTypeWidget,
                                                       "indivParametersPushButton");
   QObject::connect(indivParametersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotIndiviualParameters()));
   surfaceTypeGrid->addWidget(indivParametersPushButton, 1, 1, Qt::AlignLeft);
   QToolTip::add(indivParametersPushButton, "Press this button to set\n"
                                            "parameters (cropping, padding,\n"
                                            "orientation) for an individual.");
   individualSurfaceOrientationLabel = new QLabel("                                       ",
                                      surfaceTypeWidget);
   surfaceTypeGrid->addWidget(individualSurfaceOrientationLabel, 1, 2, Qt::AlignLeft);

   if (fmriMapper.getMappingSurfaceType() == FMRIDataMapper::MAPPING_SURFACE_INDIVIDUAL) {
      mapToIndividualSurfaceRadioButton->setChecked(true);
   }
   else {
      mapToAtlasSurfaceRadioButton->setChecked(true);
   }
   
   //
   // Button Group for atlas/individual radio buttons
   //
   QButtonGroup* atlasIndivButtonGroup = new QButtonGroup(surfaceTypeWidget, 
                                                          "atlasIndivButtonGroup");
   atlasIndivButtonGroup->hide();
   atlasIndivButtonGroup->insert(mapToAtlasSurfaceRadioButton);
   atlasIndivButtonGroup->insert(mapToIndividualSurfaceRadioButton);
   QObject::connect(atlasIndivButtonGroup, SIGNAL(clicked(int)),
                    this, SLOT(slotMappingSurfaceType(int)));
                    
   //
   // Make buttons the same size
   //
   QtUtilities::makeButtonsSameSize(atlasSelectPushButton, indivParametersPushButton);
    
   showSelectedAtlas();
   showIndivOrientation();
   enabledMappingSurfaceTypeItems();
   
   return qvGroup;
}

/**
 * Called when map to atlas or map to individual is selected
 */
void
GuiMapFmriMainWindow::slotMappingSurfaceType(int item)
{
   fmriMapper.setMappingSurfaceType(static_cast<FMRIDataMapper::MAPPING_SURFACE_TYPE>(item));
   enabledMappingSurfaceTypeItems();
}

/**
 * Called when map to atlas or map to individual is selected
 */
void
GuiMapFmriMainWindow::enabledMappingSurfaceTypeItems()
{
   bool atlasItemsEnabled = false;
   bool indivItemsEnabled = false;
   
   if (fmriMapper.getMappingSurfaceType() == FMRIDataMapper::MAPPING_SURFACE_INDIVIDUAL) {
      indivItemsEnabled = true;
   }
   else {
      atlasItemsEnabled = true;
   }
   atlasSelectPushButton->setEnabled(atlasItemsEnabled);
   surfaceTypeAtlasLabel->setEnabled(atlasItemsEnabled);
   indivParametersPushButton->setEnabled(indivItemsEnabled);
   individualSurfaceOrientationLabel->setEnabled(indivItemsEnabled);
}   

/**
 * Create the mapping surface section.
 */
QWidget*
GuiMapFmriMainWindow::createMappingSurfaceSection(QWidget* parent)
{
   QVGroupBox* qvGroup = new QVGroupBox("Mapping Surface Selection", parent, "qvGroup");
   
   //
   // Horizontal box for surface selection
   //
   QHBox* surfaceSelectionHBox = new QHBox(qvGroup, "surfaceSelectionHBox");
   surfaceSelectionHBox->setSpacing(5);
   
   //
   // Vertical box for volume selection buttons
   //
   QVBox* surfaceSelectionButtonVBox = new QVBox(surfaceSelectionHBox, "surfaceSelectionButtonVBox");
   
   //
   // Volume Selection Buttons
   //
   QPushButton* surfaceAddButton = new QPushButton("Add...", surfaceSelectionButtonVBox,
                                                  "surfaceAddButton");
   QObject::connect(surfaceAddButton, SIGNAL(clicked()),
                    this, SLOT(slotSurfaceAdd()));
   QToolTip::add(surfaceAddButton, "Press this button to add an\n"
                                   "atlas or individual surface.");
   QPushButton* surfaceRemoveButton = new QPushButton("Remove", surfaceSelectionButtonVBox,
                                                     "surfaceRemoveButton");
   QObject::connect(surfaceRemoveButton, SIGNAL(clicked()),
                    this, SLOT(slotSurfaceRemove()));
   QToolTip::add(surfaceRemoveButton, "Press this button to\n"
                                   "remove a surface.");
   QtUtilities::makeButtonsSameSize(surfaceAddButton, surfaceRemoveButton);
   
   //
   // List box for volume selection
   //
   surfaceSelectionListBox = new QListBox(surfaceSelectionHBox, "surfaceSelectionListBox");
   surfaceSelectionListBox->setMinimumWidth(400);
   
   return qvGroup;
}

/**
 * Create the button section.
 */
QWidget*
GuiMapFmriMainWindow::createButtonSection(QWidget* parent)
{
   QWidget* buttonsWidget = new QWidget(parent, "buttonsWidget");
   QVBoxLayout* buttonsLayout = new QVBoxLayout(buttonsWidget, 2, 3, "buttonsLayout");
   
   //
   // Metric naming pushbutton
   //
   QPushButton* metricNamingPushButton = new QPushButton("Metric Naming...",
                                                         buttonsWidget,
                                                         "metricNamingPushButton");
   buttonsLayout->addWidget(metricNamingPushButton, 0, Qt::AlignHCenter);
   QObject::connect(metricNamingPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMetricNaming()));
   QToolTip::add(metricNamingPushButton, "Press this button to set the name of the\n"
                                     "output spec file, metric file, and the\n"
                                     "names of the metric file columns.");
                                                         
   //
   // Algorithm pushbutton
   //
   QPushButton* algorithmPushButton = new QPushButton("Algorithm...",
                                                      buttonsWidget,
                                                      "algorithmPushButton");
   buttonsLayout->addWidget(algorithmPushButton, 0, Qt::AlignHCenter);
   QObject::connect(algorithmPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAlgorithm()));
   QToolTip::add(algorithmPushButton, "Press this button to set the\n"
                                      "mapping algorithm and parameters.");
                                                         
   //
   // Run Mapper pushbutton
   //
   QPushButton* runMapperPushButton = new QPushButton("Run Mapper...",
                                                      buttonsWidget,
                                                      "runMapperPushButton");
   buttonsLayout->addWidget(runMapperPushButton, 0, Qt::AlignHCenter);
   QObject::connect(runMapperPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRunMapper()));
                                                      
   QtUtilities::makeButtonsSameSize(metricNamingPushButton, algorithmPushButton,
                                    runMapperPushButton);
   QToolTip::add(runMapperPushButton, "Press this button to map the selected\n"
                                      "functional volumes to the selected surfaces.");
                                                         
   return buttonsWidget;
}

/**
 * Slot called when volume Add pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotVolumeAdd()
{
   std::string fileExtension("*");
   
   switch(fmriMapper.getVolumeType()) {
      case FMRIDataMapper::VOLUME_TYPE_AFNI:
         fileExtension.assign("AFNI Files (*.HEAD)");
         break;
      case FMRIDataMapper::VOLUME_TYPE_ANALYZE:
         fileExtension.assign("Analyze Files (*.hdr)");
         break;
      case FMRIDataMapper::VOLUME_TYPE_SPM_LPI:
      case FMRIDataMapper::VOLUME_TYPE_SPM_RPI:
         fileExtension.assign("SPM/MEDx Files (*.hdr)");
         break;
      case FMRIDataMapper::VOLUME_TYPE_WUNIL:
         fileExtension.assign("Wash U. NIL Files (*.ifh)");
         break;
      case FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_SIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_UNSIGNED:
      case FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT:
      case FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT:
         fileExtension.assign("Raw Files (*.*)");
         break;
   }
   
   QFileDialog fd(this, "volume-file-dialog", true);
   fd.setCaption("Choose Volume File(s)");
   fd.setFilter(fileExtension.c_str());
   fd.setMode(QFileDialog::ExistingFiles);
   if (fd.exec() == QDialog::Accepted) {
      QStringList fileNames(fd.selectedFiles());
      for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); it++) {
         const std::string fileName((*it).latin1());
         if (fileName.empty() == false) {
            std::string msg;
            if (fmriMapper.addVolume(fileName, msg)) {
               displayErrorMessage(msg);
            }
         }
      }
      loadVolumeListBox();         
   }
}

/**
 * Load the volume names into the volume list box.
 */
void
GuiMapFmriMainWindow::loadVolumeListBox()
{
   volumeSelectionListBox->clear();
   const int numVolumes = fmriMapper.getNumberOfVolumes();
   for (int i = 0; i < numVolumes; i++) {
      const FMRIVolumeInfo* vol = fmriMapper.getVolume(i);
      volumeSelectionListBox->insertItem(FileUtilities::basename(vol->getVolumeName()).c_str());
   }
}

/**
 * Slot called when volume Remove pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotVolumeRemove()
{
   if (volumeSelectionListBox->count() > 0) {
      if (volumeSelectionListBox->currentItem() >= 0) {
         fmriMapper.deleteVolume(volumeSelectionListBox->currentItem());
      }
   }
   loadVolumeListBox();
}

/**
 * Slot called when map to atlas select pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotAtlasSelect()
{
   const int num = fmriMapper.getNumberOfAtlasDirectories();
   if (num <= 0) {
      QMessageBox::information(this, "No Atlases", 
                               "There are no atlases.  Add them with \"Add New\"\n"
                               "on the Atlas Menu.", "OK");
      return;
   }
   
   std::vector<std::string> names;
   for (int i = 0; i < num; i++) {
      AtlasDirectoryInfo* adi = fmriMapper.getAtlasDirectoryInfo(i);
      names.push_back(FileUtilities::basename(adi->getAtlasDirectory()));
   }
   
   QtListBoxSelectionDialog lbd(this, "Choose Atlas", names, 
                                fmriMapper.getSelectedAtlasDirectory());
   if (lbd.exec() == QDialog::Accepted) { 
      if (lbd.getSelectedItemIndex() >= 0) {
         fmriMapper.setSelectedAtlasDirectory(lbd.getSelectedItemIndex());
      }  
      showSelectedAtlas();
   }
}

/**
 * Slot called when map to individual parameters pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotIndiviualParameters()
{
   GuiMapFmriIndivParametersDialog ipd(this);
   ipd.exec();
   showIndivOrientation();
}

/**
 * Load the surface list box.
 */
void
GuiMapFmriMainWindow::loadSurfaceListBox()
{
   surfaceSelectionListBox->clear();
   for (int i = 0; i < fmriMapper.getNumberOfSurfaces(); i++) {
      FMRISurfaceInfo* fsi = fmriMapper.getSurface(i);
      surfaceSelectionListBox->insertItem(fsi->getDescriptiveName().c_str());
   }
}
 
/**
 * Add atlas surfaces
 */
void
GuiMapFmriMainWindow::addAtlasSurfaces()
{
   const int num = fmriMapper.getNumberOfAtlasDirectories();
   if (num <= 0) {
      QMessageBox::information(this, "No Atlases", 
                               "There are no atlases.  Add them with \"Add New\"\n"
                               "on the Atlas Menu.", "OK");
      return;
   }
   //
   // Popup the atlas selection dialog
   //
   const int selectedAtlasIndex = fmriMapper.getSelectedAtlasDirectory();
   if (selectedAtlasIndex < 0) {
      QApplication::beep();
      QMessageBox::critical(this, "Error", "There is no selected atlas.", "OK");
      return;
   }
   AtlasDirectoryInfo* asdi =
            fmriMapper.getAtlasDirectoryInfo(selectedAtlasIndex);
   AtlasSurfaceDirectoryFile asdf;
   asdf.setFileLocationPathName(asdi->getMappingSurfaceFileDirectory());
   asdf.readFile(asdi->getMappingSurfaceListFile());
   
   //
   // Make sure the version is supported
   //
   if (asdf.getFileVersion() > 1) {
      QApplication::beep();
      QMessageBox::critical(this, "ERROR",
         "The atlas directory file you have selected is a newer version that\n"
         "is not supported by this program.  You have two choices:\n"
         "   1) Get an older version of the atlas that will work with this program.\n"
         "   2) Use the replacement for this program named \"caret_map_fmri\"", "OK");
      return;
   }
   
   GuiMapFmriAtlasSelectionDialog asd(this, &asdf);
   
   enum SPEC_FILE_STATUS {
      SPEC_FILE_STATUS_NOT_SELECTED,
      SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS,
      SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST,
      SPEC_FILE_STATUS_COPIED_SPEC
   };
   
   SPEC_FILE_STATUS leftHemStatus = SPEC_FILE_STATUS_NOT_SELECTED;
   SPEC_FILE_STATUS rightHemStatus = SPEC_FILE_STATUS_NOT_SELECTED;
   SPEC_FILE_STATUS cerebellumStatus = SPEC_FILE_STATUS_NOT_SELECTED;
   
   if (asd.exec() == QDialog::Accepted) {
      //
      // Get the atlas selections
      //
      std::vector<int> atlasesSelected;
      asd.getSelectedAtlas(atlasesSelected);
      const int numSelected = atlasesSelected.size();
      
      if (numSelected > 0) {
         bool needToCreateRight = false;
         bool needToCreateLeft  = false;
         bool needToCreateCerebellum = false;
         
         bool needToGetRightSpecFile = false;
         bool needToGetLeftSpecFile = false;
         bool needToGetCerebellumSpecFile = false;
         
         bool alreadyInLeftHemDirectory = false;
         bool alreadyInRightHemDirectory = false;
         bool alreadyInCerebellumDirectory = false;
         
         std::string leftSpecFileName;
         std::string rightSpecFileName;
         std::string cerebellumSpecFileName;
         
         std::string leftAtlasSpecFileName;
         std::string rightAtlasSpecFileName;
         std::string cerebellumAtlasSpecFileName;
         
         //
         // See if the proper subdirectory exists and if it has a spec file.
         //
         for (int j = 0; j < numSelected; j++) {
            AtlasSurface* as = asdf.getAtlasSurface(atlasesSelected[j]);
            std::string subdirName;
            switch(as->getAnatomyType()) {
               case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                  subdirName.assign("LEFT_HEM");
                  leftAtlasSpecFileName.assign(as->getSpecFileName());
                  break;
               case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                  subdirName.assign("RIGHT_HEM");
                  rightAtlasSpecFileName.assign(as->getSpecFileName());
                  break;
               case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                  subdirName.assign("CEREBELLUM");
                  cerebellumAtlasSpecFileName.assign(as->getSpecFileName());
                  break;
               case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                  break;
            }
            
            if (subdirName.length() > 0) {
               QFileInfo subdirInfo(subdirName.c_str());
               
               //
               // See if we are in the "subdirName" directory
               //
               const std::string currentDirectory(QDir::currentDirPath().latin1());
               const std::string lastPartOfDirectory(FileUtilities::basename(currentDirectory));
               if (lastPartOfDirectory == subdirName) {
                     //
                     // see if there are spec files in the current directory
                     //
                     std::vector<std::string> files;
                     SpecFileUtilities::findSpecFilesInDirectory(currentDirectory, files);
                     if (files.size() == 0) {
                        //
                        // Subdirectory contains no spec files
                        //
                        switch(as->getAnatomyType()) {
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                              needToGetLeftSpecFile = true;
                              leftHemStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                              needToGetRightSpecFile = true;
                              rightHemStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                              needToGetCerebellumSpecFile = true;
                              cerebellumStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                              break;
                        }
                     }
                     else if (files.size() == 1) {
                        std::string filename(files[0]);
                        //
                        // Subdirectory contains exactly one spec file, so use it
                        //
                        switch(as->getAnatomyType()) {
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                              leftSpecFileName.assign(filename);
                              alreadyInLeftHemDirectory = true;
                              leftHemStatus = SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                              rightSpecFileName.assign(filename);
                              alreadyInRightHemDirectory = true;
                              rightHemStatus = SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                              cerebellumSpecFileName.assign(filename);
                              alreadyInCerebellumDirectory = true;
                              cerebellumStatus = SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                              break;
                        }
                     }
                     else if (files.size() > 1) {
                        switch(as->getAnatomyType()) {
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                              leftHemStatus = SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                              rightHemStatus = SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                              cerebellumStatus = SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                              break;
                        }
                     }
               }            
               //
               // Check to see if needed subdirectory exists
               //
               else if (subdirInfo.exists()) {
                  if (subdirInfo.isDir()) {
                  
                     //
                     // Subdirectory exists, see if it contains spec files
                     //
                     std::vector<std::string> files;
                     SpecFileUtilities::findSpecFilesInDirectory(subdirName, files);
                     if (files.size() == 0) {
                        //
                        // Subdirectory contains no spec files
                        //
                        switch(as->getAnatomyType()) {
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                              needToGetLeftSpecFile = true;
                              leftHemStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                              needToGetRightSpecFile = true;
                              rightHemStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                              needToGetCerebellumSpecFile = true;
                              cerebellumStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                              break;
                        }
                     }
                     else if (files.size() == 1) {
                        std::string filename(subdirName);
                        filename.append("/");
                        filename.append(files[0]);
                        //
                        // Subdirectory contains exactly one spec file, so use it
                        //
                        switch(as->getAnatomyType()) {
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                              leftSpecFileName.assign(filename);
                              leftHemStatus = SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                              rightSpecFileName.assign(filename);
                              rightHemStatus = SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                              cerebellumSpecFileName.assign(filename);
                              cerebellumStatus = SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                              break;
                        }
                     }
                     else if (files.size() > 1) {
                        switch(as->getAnatomyType()) {
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                              leftHemStatus = SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                              rightHemStatus = SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                              cerebellumStatus = SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST;
                              break;
                           case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                              break;
                        }
                     }
                  }
               }
               else {
                  //
                  // Subdirectory does not exist, will need to create it and
                  // copy the spec file from the atlas
                  //
                  switch(as->getAnatomyType()) {
                     case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                        needToCreateLeft      = true;
                        needToGetLeftSpecFile = true;
                        leftHemStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                        break;
                     case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                        needToCreateRight      = true;
                        needToGetRightSpecFile = true;
                        rightHemStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                        break;
                     case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                        needToCreateCerebellum      = true;
                        needToGetCerebellumSpecFile = true;
                        cerebellumStatus = SPEC_FILE_STATUS_COPIED_SPEC;
                        break;
                     case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                        break;
                  }
               }
            }
         }
         
         //
         // Create directories as needed
         //
         if (needToCreateLeft) {
            QDir dir;
            dir.mkdir("LEFT_HEM");
         }
         if (needToCreateRight) {
            QDir dir;
            dir.mkdir("RIGHT_HEM");
         }
         if (needToCreateCerebellum) {
            QDir dir;
            dir.mkdir("CEREBELLUM");
         }
         
         if (needToGetLeftSpecFile ||
            needToGetRightSpecFile ||
            needToGetCerebellumSpecFile) {
            
            //
            // Copy needed spec files and data files
            //
            const bool copyDataFiles = 
               (QMessageBox::information(this, "Copy Data Files",
                   "Do you want to copy the data files ?\n\n"
                   "If you are not sure, choose \"Point to Data Files\".",
                   "Point to Data Files", "Copy Data Files") == 1);
            
            if (needToGetLeftSpecFile) {
               leftSpecFileName.assign(copyAtlasSpecFile(asdi->getAtlasDirectory(),
                                                         "LEFT_HEM",
                                                         leftAtlasSpecFileName,
                                                         copyDataFiles));
            }
            if (needToGetRightSpecFile) {
               rightSpecFileName.assign(copyAtlasSpecFile(asdi->getAtlasDirectory(),
                                                         "RIGHT_HEM",
                                                         rightAtlasSpecFileName,
                                                         copyDataFiles));
            }
            if (needToGetCerebellumSpecFile) {
               cerebellumSpecFileName.assign(copyAtlasSpecFile(asdi->getAtlasDirectory(),
                                                   "CEREBELLUM",
                                                   cerebellumAtlasSpecFileName,
                                                   copyDataFiles));
            }
         }
         
         //
         // Popup message about existing spec files
         //
         std::string msg;
         switch (leftHemStatus) {
            case SPEC_FILE_STATUS_NOT_SELECTED:
               break;
            case SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS:
               msg.append("Left hemisphere uses existing spec file: ");
               msg.append(leftSpecFileName);
               msg.append("\n");
               break;
            case SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST:
               msg.append("Multiple left hemisphere spec files exist.  None Selected.\n");
               msg.append("Use Metric Naming button to set spec file.\n");
               break;
            case SPEC_FILE_STATUS_COPIED_SPEC:
               break;
         }
         switch (rightHemStatus) {
            case SPEC_FILE_STATUS_NOT_SELECTED:
               break;
            case SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS:
               msg.append("Right hemisphere uses existing spec file: ");
               msg.append(rightSpecFileName);
               msg.append("\n");
               break;
            case SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST:
               msg.append("Multiple right hemisphere spec files exist.  None Selected.\n");
               msg.append("Use Metric Naming button to set spec file.\n");
               break;
            case SPEC_FILE_STATUS_COPIED_SPEC:
               break;
         }
         switch (cerebellumStatus) {
            case SPEC_FILE_STATUS_NOT_SELECTED:
               break;
            case SPEC_FILE_STATUS_SINGLE_SPEC_EXISTS:
               msg.append("Cerebellum uses existing spec file: ");
               msg.append(cerebellumSpecFileName);
               msg.append("\n");
               break;
            case SPEC_FILE_STATUS_MULTIPLE_SPEC_EXIST:
               msg.append("Multiple cerebellum spec files exist.  None Selected.\n");
               msg.append("Use Metric Naming button to set spec file.\n");
               break;
            case SPEC_FILE_STATUS_COPIED_SPEC:
               break;
         }
         if (msg.empty() == false) {
            QApplication::beep();
            QMessageBox::warning(this, "Spec File Warning", msg.c_str(), "OK");
         }
         
         for (int k = 0; k < numSelected; k++) {
            //printf("Selected atlas %d\n", atlasesSelected[k]);
            AtlasSurface* as = asdf.getAtlasSurface(atlasesSelected[k]);
            
            std::string anatomyName;
            std::string subdirName;
            std::string specFileName;
            switch(as->getAnatomyType()) {
               case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_LEFT:
                  anatomyName.assign("LEFT");
                  if (alreadyInLeftHemDirectory == false) {
                     subdirName.assign("LEFT_HEM");
                  }
                  specFileName.assign(leftSpecFileName);
                  break;
               case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBRAL_RIGHT:
                  anatomyName.assign("RIGHT");
                  if (alreadyInRightHemDirectory == false) {
                     subdirName.assign("RIGHT_HEM");
                  }
                  specFileName.assign(rightSpecFileName);
                  break;
               case AtlasSurface::ATLAS_SURFACE_TYPE_CEREBELLUM:
                  anatomyName.assign("CEREBELLUM");
                  if (alreadyInCerebellumDirectory  == false) {
                     subdirName.assign("CEREBELLUM");
                  }
                  specFileName.assign(cerebellumSpecFileName);
                  break;
               case AtlasSurface::ATLAS_SURFACE_TYPE_NONE:
                  break;
            }
            
            fmriMapper.addSurface(subdirName, as->getFileName(), 
                              as->getDescriptiveName(), anatomyName,
                              specFileName);
         }
      }
      loadSurfaceListBox();
   }
}

/** 
 * Copy the spec file from the atlas to a subdirectory of the current directory.
 */
std::string
GuiMapFmriMainWindow::copyAtlasSpecFile(const std::string& directory,
                                        const std::string& subdirectory,
                                        const std::string& atlasSpecFileName,
                                        const bool copyDataFiles)
{
   std::string sourceDirectory(directory);
   sourceDirectory.append("/");
   sourceDirectory.append(subdirectory);
   
   //
   // Source spec file
   //
   std::string sourceSpecFile(sourceDirectory);
   sourceSpecFile.append("/");
   sourceSpecFile.append(atlasSpecFileName);
   
   //
   // Output spec file
   //
   std::string outputSpecFileName(subdirectory);
   outputSpecFileName.append("/");
   outputSpecFileName.append(atlasSpecFileName);
   
   SpecFileUtilities::MODE_COPY_SPEC_FILE copyMode = 
          SpecFileUtilities::MODE_COPY_SPEC_FILE_POINT_TO_DATA_FILES;
   if (copyDataFiles) {
      copyMode = SpecFileUtilities::MODE_COPY_SPEC_FILE_COPY_ALL;
   }
   
   QApplication::setOverrideCursor(waitCursor);
   std::string errorMessage;
   if (SpecFileUtilities::copySpecFile(sourceSpecFile, subdirectory,
                                       copyMode, errorMessage) != 0) {
      errorMessage.append("\n\nMost likely the atlas spec file is missing.");
      displayErrorMessage(errorMessage);
   }
   QApplication::restoreOverrideCursor();
   
   return outputSpecFileName;
}

/**
 * Add individual surfaces
 */
void
GuiMapFmriMainWindow::addIndividualSurfaces()
{
   QFileDialog fd(this, "surface-file-dialog", true);
   std::string fileExtension("VTK Surface File (*.vtk)");
   switch(fmriMapper.getSurfaceFileType()) {
      case FMRIDataMapper::SURFACE_FILE_TYPE_VTK_POLYDATA:
         std::string fileExtension("VTK Surface File (*.vtk)");
         break;
   }
   fd.setCaption("Choose Surface File(s)");
   fd.setFilter(fileExtension.c_str());
   fd.setMode(QFileDialog::ExistingFiles);
   if (fd.exec() == QDialog::Accepted) {
      QStringList fileNames(fd.selectedFiles());
      for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); it++) {
         const std::string fileName((*it).latin1());
         if (fileName.empty() == false) {
            std::string filedir(FileUtilities::dirname(fileName));
            std::vector<std::string> specFiles;
            SpecFileUtilities::findSpecFilesInDirectory(filedir, specFiles);
            std::string specFileName;
            if (specFiles.size() == 1) {
               specFileName.assign(specFiles[0]);
               if (filedir.empty() == false) {
                  specFileName.assign(filedir);
                  specFileName.append("/");
                  specFileName.append(specFiles[0]);
               }
            }
            fmriMapper.addSurface("", fileName, FileUtilities::basename(fileName), 
                                 "", specFileName);
         }
      }
      loadSurfaceListBox();         
   }
}

/**
 * Slot called when surface Add pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotSurfaceAdd()
{
   switch(fmriMapper.getMappingSurfaceType()) {
      case FMRIDataMapper::MAPPING_SURFACE_ATLAS:
         addAtlasSurfaces();
         break;
      case FMRIDataMapper::MAPPING_SURFACE_INDIVIDUAL:
         addIndividualSurfaces();
         break;
   }

}

/**
 * Slot called when surface Remove pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotSurfaceRemove()
{
   if (surfaceSelectionListBox->count() > 0) {
      if (surfaceSelectionListBox->currentItem() >= 0) {
         fmriMapper.deleteSurface(surfaceSelectionListBox->currentItem());
      }
   }
   loadSurfaceListBox();
}

/**
 * Slot called when Metric Naming pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotMetricNaming()
{
   if ((fmriMapper.getNumberOfVolumes() <= 0) ||
       (fmriMapper.getNumberOfSurfaces() <= 0)) {
      QMessageBox::warning(this, "No Files",
                           "You must select both volumes and surfaces\n"
                           "prior to editing metric attributes.", "OK");
      return;
   }
   GuiMapFmriMetricNamingDialog mnd(this);
   mnd.exec();
}

/**
 * Slot called when Algorithm pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotAlgorithm()
{
   GuiMapFmriAlgorithmDialog gad(this);
   gad.exec();
}

/**
 * Read the volume dimension text boxes
 */
void
GuiMapFmriMainWindow::readVolumeDimensions()
{
   const int x = volumeDimensionXLineEdit->text().toInt();
   const int y = volumeDimensionYLineEdit->text().toInt();
   const int z = volumeDimensionZLineEdit->text().toInt();
   fmriMapper.setRawVolumeDimensions(x, y, z);
}

/**
 * Slot called when Run Mapper pushbutton pressed.
 */
void
GuiMapFmriMainWindow::slotRunMapper()
{
   QApplication::setOverrideCursor(waitCursor);
   readVolumeDimensions();
   fmriMapper.saveUserPreferences();
   const bool result = fmriMapper.execute();
   QApplication::restoreOverrideCursor();
   if (result == false) { 
      displaySuccessMessage();
   }
}

/**
 * Update/display the progress dialog.
 */
bool
GuiMapFmriMainWindow::updateProgressDialog(const int progressCount, 
                                           const int totalProgressCount,
                                           const std::string& msg)
{
   bool cancelled = false;
   
   qApp->processEvents();  // note: qApp is global in qapplication.h
   
   if (progressDialog != NULL) {
      bool removeDialog = false;
      if (progressCount >= totalProgressCount) {
         removeDialog = true;
      }
      else if (progressDialog->wasCancelled()) {
         cancelled = true;
         removeDialog = true;
      }
      
      if (removeDialog) {
         //
         // Remove the progress dialog
         //
         progressDialog->setProgress(totalProgressCount);
         delete progressDialog;
         progressDialog = NULL;
      }
   }
   else {
      //
      // Create the progress dialog
      //
      progressDialog = new QProgressDialog(this, "mapFmriProgressDialog");
   }
   
   if (progressDialog != NULL) {
      progressDialog->setLabelText(msg.c_str());
      progressDialog->setProgress(progressCount, totalProgressCount);
      qApp->processEvents();  // note: qApp is global in qapplication.h
      progressDialog->show();
   }
   
   return cancelled;
}

