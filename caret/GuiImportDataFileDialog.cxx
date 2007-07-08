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

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QSpinBox>

#include "BrainModelContours.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "ContourCellFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#define _GUI_IMPORT_DATA_FILE_DIALOG_MAIN_
#include "GuiImportDataFileDialog.h"
#undef _GUI_IMPORT_DATA_FILE_DIALOG_MAIN_
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiStructureComboBox.h"
#include "GuiSurfaceTypeComboBox.h"
#include "GuiTopologyTypeComboBox.h"
#include "GuiVolumeTypeComboBox.h"
#include "GuiVolumeVoxelDataTypeComboBox.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "PreferencesFile.h"
#include "QtUtilities.h"
#include "RgbPaintFile.h"
#include "SurfaceShapeFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiImportDataFileDialog::GuiImportDataFileDialog(QWidget* parent)
           : Q3FileDialog(parent, "Import Data File", false)
{
   setWindowTitle("Import Data File");
   setMode(Q3FileDialog::ExistingFile);
   setDir(QDir::currentPath());

   
   QObject::connect(this, SIGNAL(filterSelected(const QString&)),
                    this, SLOT(filterSelectedSlot(const QString&)));
                    
   //
   // Add import options to dialog
   //
   QWidget* caretWidget = createCaretUniqueSection();
   addWidgets(NULL, caretWidget, NULL);
   
   setFilter(analyzeVolumeFileFilter);
   addFilter(brainVoyagerFileFilter);
   addFilter(byuSurfaceFileFilter);
   addFilter(freeSurferAsciiSurfaceFileFilter);
   addFilter(freeSurferBinarySurfaceFileFilter);
   addFilter(freeSurferAsciiCurvatureFileFilter);
   addFilter(freeSurferBinaryCurvatureFileFilter);
   addFilter(freeSurferAsciiFunctionalFileFilter);
   addFilter(freeSurferBinaryFunctionalFileFilter);
   addFilter(freeSurferAsciiLabelFileFilter);
   addFilter(mdPlotFileFilter);
#ifdef HAVE_MINC
   addFilter(mincVolumeFileFilter);
#endif // HAVE_MINC
   addFilter(neurolucidaFileFilter);
   addFilter(rawVolumeFileFilter);
   addFilter(stlSurfaceFileFilter);
   addFilter(sumaRgbFileFilter);
   addFilter(vtkSurfaceFileFilter);
   addFilter(vtkXmlSurfaceFileFilter);
   addFilter(vtkVolumeFileFilter);
   
   setSelectedFilter(analyzeVolumeFileFilter);
   filterSelectedSlot(analyzeVolumeFileFilter);

   QtUtilities::setMaximumHeightToNinetyPercentOfScreenHeight(this);
}

/**
 * Destructor
 */
GuiImportDataFileDialog::~GuiImportDataFileDialog()
{
}

/**
 * show the dialog.
 */
void 
GuiImportDataFileDialog::show()
{
   setDir(QDir::currentPath());
   updateDialog();

   Q3FileDialog::show();
}

/**
 * update the dialog.
 */
void 
GuiImportDataFileDialog::updateDialog()
{
   previousDirectoryComboBox->clear();
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   pf->getRecentDataFileDirectories(previousDirectories);
   
   for (int i = 0; i < static_cast<int>(previousDirectories.size()); i++) {
      QString dirName(previousDirectories[i]);
      const int dirNameLength = static_cast<int>(dirName.length());
      if (dirNameLength > 50) {
         QString s("...");
         s.append(dirName.mid(dirNameLength - 50));
         dirName = s;
      }
      previousDirectoryComboBox->addItem(dirName);
   }

   structureComboBox->setStructure(theMainWindow->getBrainSet()->getStructure().getType());
}
      
/**
 * create the caret unique section.
 */
QWidget* 
GuiImportDataFileDialog::createCaretUniqueSection()
{
   //--------------------------------------------------------------------------
   //
   // Previous directories combo box
   //
   previousDirectoryComboBox = new QComboBox;
   QObject::connect(previousDirectoryComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPreviousDirectory(int)));

   //
   // Previous directories group box
   //
   QGroupBox* prevDirGroupBox = new QGroupBox("Previous Directories");
   QVBoxLayout* prevDirGroupLayout = new QVBoxLayout(prevDirGroupBox);
   prevDirGroupLayout->addWidget(previousDirectoryComboBox);
   
   //--------------------------------------------------------------------------
   //
   // Volume selections
   //
   QLabel* volumeTypeLabel = new QLabel("Volume Type");
   volumeTypeComboBox = new GuiVolumeTypeComboBox;
   QHBoxLayout* volumeTypeLayout = new QHBoxLayout;
   volumeTypeLayout->addWidget(volumeTypeLabel);
   volumeTypeLayout->addWidget(volumeTypeComboBox);
   volumeTypeLayout->addStretch();
   
   //
   // Raw volume dimensions
   //
   const int rawDimSize = 100;
   QLabel* rawVolumeDimensionsLabel = new QLabel("Dimensions ");
   rawVolumeDimXSpinBox = new QSpinBox;
   rawVolumeDimXSpinBox->setMinimum(0);
   rawVolumeDimXSpinBox->setMaximum(100000);
   rawVolumeDimXSpinBox->setSingleStep(1);
   rawVolumeDimXSpinBox->setFixedWidth(rawDimSize);
   rawVolumeDimYSpinBox = new QSpinBox;
   rawVolumeDimYSpinBox->setMinimum(0);
   rawVolumeDimYSpinBox->setMaximum(100000);
   rawVolumeDimYSpinBox->setSingleStep(1);
   rawVolumeDimYSpinBox->setFixedWidth(rawDimSize);
   rawVolumeDimZSpinBox = new QSpinBox;
   rawVolumeDimZSpinBox->setFixedWidth(rawDimSize);
   rawVolumeDimZSpinBox->setMinimum(0);
   rawVolumeDimZSpinBox->setMaximum(100000);
   rawVolumeDimZSpinBox->setSingleStep(1);
   QHBoxLayout* rawVolumeDimensionsLayout = new QHBoxLayout;
   rawVolumeDimensionsLayout->setSpacing(5);
   rawVolumeDimensionsLayout->addWidget(rawVolumeDimensionsLabel);
   rawVolumeDimensionsLayout->addWidget(rawVolumeDimXSpinBox);
   rawVolumeDimensionsLayout->addWidget(rawVolumeDimYSpinBox);
   rawVolumeDimensionsLayout->addWidget(rawVolumeDimZSpinBox);
   rawVolumeDimensionsLayout->addStretch();
   
   //
   // Raw volume data type
   //
   QLabel* rawVolumeVoxelDataTypeLabel = new QLabel("Voxel Data Type");
   rawVolumeVoxelDataTypeComboBox = new GuiVolumeVoxelDataTypeComboBox;
   QHBoxLayout* rawVolumeVoxelDataTypeLayout = new QHBoxLayout;
   rawVolumeVoxelDataTypeLayout->addWidget(rawVolumeVoxelDataTypeLabel);
   rawVolumeVoxelDataTypeLayout->addWidget(rawVolumeVoxelDataTypeComboBox);
   rawVolumeVoxelDataTypeLayout->addStretch();
   
   //
   // Raw volume byte swap
   //
   QLabel* rawVolumeByteSwapLabel = new QLabel("Byte Swap ");
   rawVolumeByteSwapComboBox = new QComboBox;
   rawVolumeByteSwapComboBox->addItem("No");
   rawVolumeByteSwapComboBox->addItem("Yes");
   QHBoxLayout* rawVolumeByteSwapLayout = new QHBoxLayout;
   rawVolumeByteSwapLayout->addWidget(rawVolumeByteSwapLabel);
   rawVolumeByteSwapLayout->addWidget(rawVolumeByteSwapComboBox);
   rawVolumeByteSwapLayout->addStretch();
   
   //
   // Widget and layout for raw volume options
   //
   rawVolumeWidget = new QWidget;
   QVBoxLayout* rawVolumeLayout = new QVBoxLayout(rawVolumeWidget);
   rawVolumeLayout->addLayout(rawVolumeDimensionsLayout);
   rawVolumeLayout->addLayout(rawVolumeVoxelDataTypeLayout);
   rawVolumeLayout->addLayout(rawVolumeByteSwapLayout);
   
   //--------------------------------------------------------------------------
   //
   // Volume Group box and layout
   //
   volumeOptionsGroupBox = new QGroupBox("Volume Import Selections");
   QVBoxLayout* volumeOptionsLayout = new QVBoxLayout(volumeOptionsGroupBox);
   volumeOptionsLayout->addLayout(volumeTypeLayout);
   volumeOptionsLayout->addWidget(rawVolumeWidget);
   
   //--------------------------------------------------------------------------
   //
   // Contour selections
   //
   importContoursCheckBox = new QCheckBox("Import Contours");
   importContoursCheckBox->setChecked(true);
   importContourCellsCheckBox = new QCheckBox("Import Contour Cells");
   importContourCellsCheckBox->setChecked(true);
   
   //
   // Group box for MDPlot items
   //
   contourGroupBox = new QGroupBox("MDPlot Import Selections");
   QVBoxLayout* mdPlotGroupLayout = new QVBoxLayout(contourGroupBox);
   mdPlotGroupLayout->addWidget(importContoursCheckBox);
   mdPlotGroupLayout->addWidget(importContourCellsCheckBox);

   //--------------------------------------------------------------------------
   //
   // Surface selections
   //
   //
   // Structure selection
   //
   structureWidget = new QWidget;
   QLabel* structureLabel = new QLabel("Structure ");
   structureComboBox = new GuiStructureComboBox;
   QHBoxLayout* structureLayout = new QHBoxLayout(structureWidget);
   structureLayout->addWidget(structureLabel);
   structureLayout->addWidget(structureComboBox);
   structureLayout->addStretch();
   
   //
   // Import type selections
   //
   importTopologyCheckBox = new QCheckBox("Import Topology");
   importTopologyCheckBox->setChecked(true);
   topologyTypeComboBox = new GuiTopologyTypeComboBox;
   topologyTypeWidget = new QWidget;
   QHBoxLayout* topologyTypeLayout = new QHBoxLayout(topologyTypeWidget);
   topologyTypeLayout->addWidget(importTopologyCheckBox);
   topologyTypeLayout->addWidget(topologyTypeComboBox);
   topologyTypeLayout->addStretch();
   
   importCoordinatesCheckBox = new QCheckBox("Import Coordinates");
   importCoordinatesCheckBox->setChecked(true);
   surfaceTypeComboBox = new GuiSurfaceTypeComboBox;
   surfaceTypeWidget = new QWidget;
   QHBoxLayout* surfaceTypeLayout = new QHBoxLayout(surfaceTypeWidget);
   surfaceTypeLayout->addWidget(importCoordinatesCheckBox);
   surfaceTypeLayout->addWidget(surfaceTypeComboBox);
   surfaceTypeLayout->addStretch();
   
   importPaintAreaColorsCheckBox = new QCheckBox("Import Colors as Paint and Area Colors");
   importPaintAreaColorsCheckBox->setChecked(true);
   
   importRgbColorsCheckBox = new QCheckBox("Import Colors as RGB Paint");
   importRgbColorsCheckBox->setChecked(true);
   
   surfaceGroupBox = new QGroupBox("Surface Import Selections");
   QVBoxLayout* surfaceGroupLayout = new QVBoxLayout(surfaceGroupBox);
   surfaceGroupLayout->addWidget(structureWidget);
   surfaceGroupLayout->addWidget(topologyTypeWidget);
   surfaceGroupLayout->addWidget(surfaceTypeWidget);
   surfaceGroupLayout->addWidget(importPaintAreaColorsCheckBox);
   surfaceGroupLayout->addWidget(importRgbColorsCheckBox);
   
   //--------------------------------------------------------------------------
   //
   // Caret widgets and layout
   //
   QWidget* caretWidget = new QWidget;
   QVBoxLayout* caretWidgetLayout = new QVBoxLayout(caretWidget);
   caretWidgetLayout->addWidget(prevDirGroupBox);
   caretWidgetLayout->addWidget(volumeOptionsGroupBox);
   caretWidgetLayout->addWidget(contourGroupBox);
   caretWidgetLayout->addWidget(surfaceGroupBox);
   
   return caretWidget;
}
      
/**
 * Slot called when a file filter is selected
 */
void
GuiImportDataFileDialog::filterSelectedSlot(const QString& filterName)
{
   surfaceGroupBox->setHidden(true);
   surfaceTypeWidget->setHidden(true);
   topologyTypeWidget->setHidden(true);
   structureWidget->setHidden(true);
   importPaintAreaColorsCheckBox->setHidden(true);
   importRgbColorsCheckBox->setHidden(true);
   volumeOptionsGroupBox->setHidden(true);
   rawVolumeWidget->setHidden(true);
   contourGroupBox->setHidden(true);
   
   if (structureWidget != NULL) {
      if (filterName == GuiDataFileDialog::brainVoyagerFileFilter) {
         surfaceGroupBox->setHidden(false);
         surfaceTypeWidget->setHidden(false);
         topologyTypeWidget->setHidden(false);
         structureWidget->setHidden(false);
         importPaintAreaColorsCheckBox->setHidden(false);
      }
      else if ((filterName == freeSurferBinarySurfaceFileFilter) ||
               (filterName == freeSurferAsciiSurfaceFileFilter)) {
         surfaceGroupBox->setHidden(false);
         surfaceTypeWidget->setHidden(false);
         topologyTypeWidget->setHidden(false);
         structureWidget->setHidden(false);
      }
      else if ((filterName == freeSurferAsciiFunctionalFileFilter) ||
               (filterName == freeSurferBinaryFunctionalFileFilter)) {
      }
      else if ((filterName == freeSurferAsciiCurvatureFileFilter) ||
               (filterName == freeSurferBinaryCurvatureFileFilter)) {
      }
      else if (filterName == freeSurferAsciiLabelFileFilter) {
      }
      else if (filterName == mdPlotFileFilter) {
         contourGroupBox->setHidden(false);   
         contourGroupBox->setTitle("MDPlot Import Selections");
         importContoursCheckBox->setText("Import MD Plot Lines as Contours");
         importContourCellsCheckBox->setText("Import MD Plot Points as Contour Cells");
      }
      else if (filterName == neurolucidaFileFilter) {
         contourGroupBox->setHidden(false);
         contourGroupBox->setTitle("Neurolucida Import Selections");
         importContoursCheckBox->setText("Import Contours");
         importContourCellsCheckBox->setText("Import Markers as Contour Cells");
      }
      else if (filterName == GuiDataFileDialog::vtkSurfaceFileFilter) {
         surfaceGroupBox->setHidden(false);
         surfaceTypeWidget->setHidden(false);
         topologyTypeWidget->setHidden(false);
         structureWidget->setHidden(false);
         importRgbColorsCheckBox->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::vtkXmlSurfaceFileFilter) {
         surfaceGroupBox->setHidden(false);
         surfaceTypeWidget->setHidden(false);
         topologyTypeWidget->setHidden(false);
         structureWidget->setHidden(false);
         importRgbColorsCheckBox->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::byuSurfaceFileFilter) {
         surfaceGroupBox->setHidden(false);
         surfaceTypeWidget->setHidden(false);
         topologyTypeWidget->setHidden(false);
         structureWidget->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::stlSurfaceFileFilter) {
         surfaceGroupBox->setHidden(false);
         surfaceTypeWidget->setHidden(false);
         topologyTypeWidget->setHidden(false);
         structureWidget->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::analyzeVolumeFileFilter) {
         volumeOptionsGroupBox->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::mincVolumeFileFilter) {
         volumeOptionsGroupBox->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::sumaRgbFileFilter) {
         surfaceGroupBox->setHidden(false);
         importRgbColorsCheckBox->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::vtkVolumeFileFilter) {
         volumeOptionsGroupBox->setHidden(false);
      }
      else if (filterName == GuiDataFileDialog::rawVolumeFileFilter) {
         volumeOptionsGroupBox->setHidden(false);
         rawVolumeWidget->setHidden(false);
      }
      else {
         std::cout << "PROGRAMMING ERROR: File filter not handled properly at "
                   << __LINE__ << " in " << __FILE__ << std::endl;
      }
   }
}

/**
 * called when a previous directory is selected.
 */
void 
GuiImportDataFileDialog::slotPreviousDirectory(int item)
{
   if ((item >= 0) && (item < static_cast<int>(previousDirectories.size()))) {
      setDir(previousDirectories[item]);
   }
}      

/**
 * Overrides QFileDialog::done(int).  This allows us to examine the file selected by the
 * user and to save the data file.
 */
void
GuiImportDataFileDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QString name(selectedFile());
      
      const QString filterName = selectedFilter();
      
      QString msg;      
      
      GuiFilesModified fm;
      
      //
      // Update previous directories
      //
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      pf->addToRecentDataFileDirectories(dirPath(), true);

      if (surfaceGroupBox->isHidden() == false) {
         //
         // Set the structure.  If there is no structure selection and the current
         // structure is unknown, allow user to make a structure selection.
         //
         if (structureWidget->isHidden() == false) {
            switch (structureComboBox->getSelectedStructure()) {
               case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
               case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
               case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
               case Structure::STRUCTURE_TYPE_CEREBELLUM:
                  theMainWindow->getBrainSet()->setStructure(structureComboBox->getSelectedStructure());
                  break;
               case Structure::STRUCTURE_TYPE_INVALID:
                  if (importCoordinatesCheckBox->isChecked() &&
                           importCoordinatesCheckBox->isEnabled()) {
                     if (theMainWindow->getBrainSet()->getStructure() == Structure::STRUCTURE_TYPE_INVALID) {
                        if (GuiMessageBox::information(this, "Structure Unknown",
                           "Coordinates are being imported and the structure is not set.\n",
                           "Change Selections", "Continue") == 0) {
                           return;
                        }
                     }
                  }
                  break;
            }
         }
         
         //
         // Verify valid topology
         //
         if (topologyTypeWidget->isHidden() == false) {
            switch (topologyTypeComboBox->getTopologyType()) {
               case TopologyFile::TOPOLOGY_TYPE_CLOSED:
               case TopologyFile::TOPOLOGY_TYPE_OPEN:
               case TopologyFile::TOPOLOGY_TYPE_CUT:
               case TopologyFile::TOPOLOGY_TYPE_LOBAR_CUT:
                  break;
               case TopologyFile::TOPOLOGY_TYPE_UNKNOWN:
               case TopologyFile::TOPOLOGY_TYPE_UNSPECIFIED:
                  if (GuiMessageBox::warning(this, "Warning - Invalid Topology",
                                 "An invalid unknown topology type is selected",
                                 "Continue", "Change Topology Type") != 0) {
                     return;
                  }
                  break;
            }
         }
         
         //
         // Verify valid topology
         //
         if (surfaceTypeWidget->isHidden() == false) {
            switch (surfaceTypeComboBox->getSurfaceType()) {
               case BrainModelSurface::SURFACE_TYPE_RAW:
               case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
               case BrainModelSurface::SURFACE_TYPE_INFLATED:
               case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
               case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
               case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
               case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
               case BrainModelSurface::SURFACE_TYPE_FLAT:
               case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
               case BrainModelSurface::SURFACE_TYPE_HULL:
                  break;
               case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
               case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
                  if (GuiMessageBox::warning(this, "Warning - Invalid Surface Type",
                                 "An invalid unknown surface type is selected",
                                 "Continue", "Change Surface Type") != 0) {
                     return;
                  }
                  break;
            }
         }
         
         //
         // Do not allow import of coordinates if there are no topology files
         // and topology is not selected for input.
         //
         if ((surfaceTypeWidget->isHidden() == false) &&
             (topologyTypeWidget->isHidden() == false)) {
            if (importCoordinatesCheckBox->isChecked() &&
                (importTopologyCheckBox->isChecked() == false)) {
               if (theMainWindow->getBrainSet()->getNumberOfTopologyFiles() == 0) {
                  GuiMessageBox::critical(this, "Error - No Topology",
                     "There are no topology files loaded and topology is not selected\n"
                     "for import.  Since a coordinate file is being imported, you must\n"
                     "change your selections to import topology.",
                     "OK");
                  return;
               }
            }
         }
      }

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      const BrainModelSurface::SURFACE_TYPES surfaceType = surfaceTypeComboBox->getSurfaceType();
      const TopologyFile::TOPOLOGY_TYPES topologyType = topologyTypeComboBox->getTopologyType();
      
      try {
         if (filterName == GuiDataFileDialog::brainVoyagerFileFilter) {
            fm.setCoordinateModified();
            fm.setPaintModified();
            fm.setAreaColorModified();
            fm.setTopologyModified();
            theMainWindow->getBrainSet()->importBrainVoyagerFile(name, 
                                          importCoordinatesCheckBox->isChecked(), 
                                          importTopologyCheckBox->isChecked(), 
                                          importPaintAreaColorsCheckBox->isChecked(),
                                          surfaceType,
                                          topologyType);
         }
         else if (filterName == freeSurferAsciiSurfaceFileFilter) {
            fm.setCoordinateModified();
            fm.setTopologyModified();
            theMainWindow->getBrainSet()->importFreeSurferSurfaceFile(name,
                                                importCoordinatesCheckBox->isChecked(), 
                                                importTopologyCheckBox->isChecked(),
                                                AbstractFile::FILE_FORMAT_ASCII,
                                                surfaceType,
                                                topologyType);
         }
         else if (filterName == freeSurferBinarySurfaceFileFilter) {
            fm.setCoordinateModified();
            fm.setTopologyModified();
            theMainWindow->getBrainSet()->importFreeSurferSurfaceFile(name,
                                                importCoordinatesCheckBox->isChecked(), 
                                                importTopologyCheckBox->isChecked(),
                                                AbstractFile::FILE_FORMAT_BINARY,
                                                surfaceType,
                                                topologyType);
         }
         else if (filterName == freeSurferAsciiCurvatureFileFilter) {
            SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
            ssf->importFreeSurferCurvatureFile(theMainWindow->getBrainSet()->getNumberOfNodes(),
                                               name,
                                               AbstractFile::FILE_FORMAT_ASCII);
            fm.setSurfaceShapeModified();
         }
         else if (filterName == freeSurferBinaryCurvatureFileFilter) {
            SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
            ssf->importFreeSurferCurvatureFile(theMainWindow->getBrainSet()->getNumberOfNodes(),
                                               name,
                                               AbstractFile::FILE_FORMAT_BINARY);
            fm.setSurfaceShapeModified();
         }
         else if (filterName == freeSurferAsciiFunctionalFileFilter) {
            MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
            mf->importFreeSurferFunctionalFile(theMainWindow->getBrainSet()->getNumberOfNodes(),
                                                    name,
                                                    AbstractFile::FILE_FORMAT_ASCII);
            fm.setMetricModified();
         }
         else if (filterName == freeSurferBinaryFunctionalFileFilter) {
            MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
            mf->importFreeSurferFunctionalFile(theMainWindow->getBrainSet()->getNumberOfNodes(),
                                                    name,
                                                    AbstractFile::FILE_FORMAT_BINARY);
            fm.setMetricModified();
         }
         else if (filterName == freeSurferAsciiLabelFileFilter) {
            fm.setPaintModified();
            fm.setAreaColorModified();
            QApplication::restoreOverrideCursor(); 
            bool importAll = false;
            if (GuiMessageBox::question(this, "Import All",
                                      "Import all label files in directory ?",
                                      "Yes", "No") == 0) {
               importAll = true;
            }
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
            pf->importFreeSurferAsciiLabelFile(theMainWindow->getBrainSet()->getNumberOfNodes(),
                                               name,
                                               theMainWindow->getBrainSet()->getAreaColorFile(),
                                               importAll);
         }
         else if (filterName == GuiDataFileDialog::mdPlotFileFilter) {
            fm.setContourModified();
            fm.setContourCellModified();
            fm.setContourCellColorModified();
            const BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours();
            bool appendContoursFlag = false;
            bool appendCellsFlag = false;
            if (importContoursCheckBox->isChecked()) {
               if (bmc != NULL) {
                  const ContourFile* cf = bmc->getContourFile();
                  if (cf->empty() == false) {
                     appendContoursFlag = (GuiMessageBox::question(this, "Import MD Plot",
                                                 "Append to existing contours?",
                                                 "Yes", "No") == 0);
                  }
               }
            }
            if (importContourCellsCheckBox->isChecked()) {
               const ContourCellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
               if (cf->empty() == false) {
                  appendCellsFlag = (GuiMessageBox::question(this, "Import MD Plot",
                                              "Append to existing contour cells?",
                                              "Yes", "No") == 0);
               }
            }
            theMainWindow->getBrainSet()->importMDPlotFile(name, 
                                    importContourCellsCheckBox->isChecked(),
                                    importContoursCheckBox->isChecked(),
                                    appendContoursFlag,
                                    appendCellsFlag);
         }
         else if (filterName == GuiDataFileDialog::neurolucidaFileFilter) {
            fm.setContourModified();
            fm.setContourCellModified();
            fm.setContourCellColorModified();
            const BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours();
            bool appendContoursFlag = false;
            bool appendCellsFlag = false;
            if (importContoursCheckBox->isChecked()) {
               if (bmc != NULL) {
                  const ContourFile* cf = bmc->getContourFile();
                  if (cf->empty() == false) {
                     appendContoursFlag = (GuiMessageBox::question(this, "Import Neurolucida",
                                                 "Append to existing contours?",
                                                 "Yes", "No") == 0);
                  }
               }
            }
            if (importContourCellsCheckBox->isChecked()) {
               const ContourCellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
               if (cf->empty() == false) {
                  appendCellsFlag = (GuiMessageBox::question(this, "Import Neurolucida",
                                              "Append to existing contour cells?",
                                              "Yes", "No") == 0);
               }
            }
            theMainWindow->getBrainSet()->importNeurolucidaFile(name, 
                                    importContourCellsCheckBox->isChecked(),
                                    importContoursCheckBox->isChecked(),
                                    appendContoursFlag,
                                    appendCellsFlag);
         }
         else if (filterName == GuiDataFileDialog::vtkSurfaceFileFilter) {
            fm.setCoordinateModified();
            fm.setRgbPaintModified();
            fm.setTopologyModified();
            theMainWindow->getBrainSet()->importVtkSurfaceFile(name, 
                                        importCoordinatesCheckBox->isChecked(), 
                                        importTopologyCheckBox->isChecked(), 
                                        importRgbColorsCheckBox->isChecked(),
                                        surfaceType,
                                        topologyType);
         }
         else if (filterName == GuiDataFileDialog::vtkXmlSurfaceFileFilter) {
            fm.setCoordinateModified();
            fm.setRgbPaintModified();
            fm.setTopologyModified();
            theMainWindow->getBrainSet()->importVtkXmlSurfaceFile(name, 
                                           importCoordinatesCheckBox->isChecked(), 
                                           importTopologyCheckBox->isChecked(), 
                                           importRgbColorsCheckBox->isChecked(),
                                           surfaceType,
                                           topologyType);
         }
         else if (filterName == GuiDataFileDialog::byuSurfaceFileFilter) {
            fm.setCoordinateModified();
            fm.setTopologyModified();
            theMainWindow->getBrainSet()->importByuSurfaceFile(name, 
                                        importCoordinatesCheckBox->isChecked(), 
                                        importTopologyCheckBox->isChecked(),
                                        surfaceType,
                                        topologyType);
         }
         else if (filterName == GuiDataFileDialog::stlSurfaceFileFilter) {
            fm.setCoordinateModified();
            fm.setTopologyModified();
            theMainWindow->getBrainSet()->importStlSurfaceFile(name, 
                                        importCoordinatesCheckBox->isChecked(), 
                                        importTopologyCheckBox->isChecked(),
                                        surfaceType,
                                        topologyType);
         }
         else if (filterName == GuiDataFileDialog::analyzeVolumeFileFilter) {
            fm.setVolumeModified();
            theMainWindow->getBrainSet()->importAnalyzeVolumeFile(name, 
                  static_cast<VolumeFile::VOLUME_TYPE>(volumeTypeComboBox->currentIndex()));
            
         }
         else if (filterName == GuiDataFileDialog::mincVolumeFileFilter) {
            fm.setVolumeModified();
            theMainWindow->getBrainSet()->importMincVolumeFile(name, 
                                            volumeTypeComboBox->getVolumeType());
         }
         else if (filterName == GuiDataFileDialog::sumaRgbFileFilter) {
            RgbPaintFile* rgb = theMainWindow->getBrainSet()->getRgbPaintFile();
            rgb->importFromSuma(name);
            fm.setRgbPaintModified();
         }
         else if (filterName == GuiDataFileDialog::vtkVolumeFileFilter) {
            fm.setVolumeModified();
            theMainWindow->getBrainSet()->importVtkStructuredPointsVolumeFile(name, 
                                            volumeTypeComboBox->getVolumeType());
         }
         else if (filterName == GuiDataFileDialog::rawVolumeFileFilter) {
            fm.setVolumeModified();
            
            const int dim[3] = {
               rawVolumeDimXSpinBox->value(),
               rawVolumeDimYSpinBox->value(),
               rawVolumeDimZSpinBox->value()
            };
            if ((dim[0] <= 0) || (dim[1] <= 0) || (dim[2] <= 0)) {
               QApplication::restoreOverrideCursor(); 
               GuiMessageBox::critical(this, "ERROR", "Invalid volume dimensions", "OK");
               return;
            }
            
            theMainWindow->getBrainSet()->importRawVolumeFile(name, 
                                            volumeTypeComboBox->getVolumeType(),
                    dim,
                    rawVolumeVoxelDataTypeComboBox->getVolumeVoxelDataType(),
                    rawVolumeByteSwapComboBox->currentIndex());
         }
         else {
            std::cout << "PROGRAMMING ERROR: File filter not handled properly at "
                     << __LINE__ << " in " << __FILE__ << std::endl;
         }
      }
      catch (FileException& e) {
         msg = e.whatQString();
      }
      
      //
      // Remove splash image
      //
      theMainWindow->getBrainSet()->setDisplaySplashImage(false);
      
      //
      // Update controls and redraw
      //
      theMainWindow->fileModificationUpdate(fm);
      if (fm.getCoordinateModified()) {
         theMainWindow->displayNewestSurfaceInMainWindow();
      }
      theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
      
      GuiBrainModelOpenGL::updateAllGL(NULL);
   
      QApplication::restoreOverrideCursor(); 
      if (msg.isEmpty() == false) {
         GuiMessageBox::critical(this, "Error Reading File", msg, "OK");
         return;
      }
   }
   
   Q3FileDialog::done(r);
}
