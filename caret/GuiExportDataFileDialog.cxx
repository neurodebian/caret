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
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>

#include "BrainModelSurfaceNodeColoring.h"
#include "BrainSet.h"
#include "FileUtilities.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#define _GUI_EXPORT_DATA_FILE_DIALOG_MAIN_
#include "GuiExportDataFileDialog.h"
#undef _GUI_EXPORT_DATA_FILE_DIALOG_MAIN_
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "GuiVolumeSelectionControl.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "PreferencesFile.h"
#include "QtListBoxSelectionDialog.h"
#include "QtUtilities.h"
#include "SurfaceShapeFile.h"
#include "VolumeFile.h"

#include "global_variables.h"

/**
 * Constructor
 */
GuiExportDataFileDialog::GuiExportDataFileDialog(QWidget* parent)
           : Q3FileDialog(parent, "Export Data File", false)
{
   setWindowTitle("Export Data File");
   setMode(Q3FileDialog::AnyFile);
   setDir(QDir::currentPath());

   
   QObject::connect(this, SIGNAL(filterSelected(const QString&)),
                    this, SLOT(filterSelectedSlot(const QString&)));
                    
   //
   // Add export options to dialog
   //
   QWidget* caretWidget = createCaretUniqueSection();
   addWidgets(NULL, caretWidget, NULL);
   
   addFilter(analyzeVolumeFileFilter);
   addFilter(byuSurfaceFileFilter);
   addFilter(freeSurferAsciiSurfaceFileFilter);
   addFilter(freeSurferAsciiCurvatureFileFilter);
   addFilter(freeSurferAsciiFunctionalFileFilter);
   addFilter(freeSurferAsciiLabelFileFilter);
   addFilter(mincVolumeFileFilter);
   addFilter(openInventorSurfaceFileFilter);
   addFilter(stlSurfaceFileFilter);
   //addFilter(studio3DSurfaceFileFilter);
   addFilter(vrmlSurfaceFileFilter);
   addFilter(vtkSurfaceFileFilter);
   addFilter(vtkXmlSurfaceFileFilter);
   addFilter(vtkVolumeFileFilter);
   //addFilter(wavefrontObjFileFilter);
   setFilter(analyzeVolumeFileFilter);
   filterSelectedSlot(selectedFilter());

   QtUtilities::setMaximumHeightToNinetyPercentOfScreenHeight(this);
}

/**
 * Destructor
 */
GuiExportDataFileDialog::~GuiExportDataFileDialog()
{
}

/**
 * show the dialog.
 */
void 
GuiExportDataFileDialog::show()
{
   setDir(QDir::currentPath());
   updateDialog();

   Q3FileDialog::show();
}

/**
 * update the dialog.
 */
void 
GuiExportDataFileDialog::updateDialog()
{
   //
   // Update previous directories
   //
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
   
   //
   // update surface selection
   //
   surfaceSelectionComboBox->updateComboBox();
   
   //
   // update volume selection
   //
   volumeSelectionControl->updateControl();
}

/**
 * create the caret unique section.
 */
QWidget* 
GuiExportDataFileDialog::createCaretUniqueSection()
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
   // Surface selection
   //
   surfaceSelectionComboBox = new GuiBrainModelSelectionComboBox(false, true, false, "");
   surfaceGroupBox = new QGroupBox("Surface Export Selections");
   QVBoxLayout* surfaceGroupLayout = new QVBoxLayout(surfaceGroupBox);
   surfaceGroupLayout->addWidget(surfaceSelectionComboBox);
   
   //--------------------------------------------------------------------------
   //
   // Volume selection
   //
   volumeSelectionControl = new GuiVolumeSelectionControl(0,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          GuiVolumeSelectionControl::LABEL_MODE_FILE_NAME,
                                                          "name",
                                                          false,
                                                          false,
                                                          false);
                                                          
   volumeGroupBox = new QGroupBox("Volume Export Selections");
   QVBoxLayout* volumeGroupLayout = new QVBoxLayout(volumeGroupBox);
   volumeGroupLayout->addWidget(volumeSelectionControl);
   
   //--------------------------------------------------------------------------
   //
   // Caret widgets and layout
   //
   QWidget* caretWidget = new QWidget;
   QVBoxLayout* caretWidgetLayout = new QVBoxLayout(caretWidget);
   caretWidgetLayout->addWidget(prevDirGroupBox);
   caretWidgetLayout->addWidget(surfaceGroupBox);
   caretWidgetLayout->addWidget(volumeGroupBox);
   
   return caretWidget;
}
      
/**
 * Slot called when a file filter is selected
 */
void
GuiExportDataFileDialog::filterSelectedSlot(const QString& filterNameIn)
{
   volumeGroupBox->setHidden(true);
   surfaceGroupBox->setHidden(true);
   
   if ((filterNameIn == byuSurfaceFileFilter) ||
       (filterNameIn == freeSurferAsciiSurfaceFileFilter) ||
       (filterNameIn == freeSurferAsciiCurvatureFileFilter) ||
       (filterNameIn == freeSurferAsciiFunctionalFileFilter) ||
       (filterNameIn == freeSurferAsciiLabelFileFilter) ||
       (filterNameIn == openInventorSurfaceFileFilter) ||
       (filterNameIn == stlSurfaceFileFilter) ||
       (filterNameIn == studio3DSurfaceFileFilter) ||
       (filterNameIn == vrmlSurfaceFileFilter) ||
       (filterNameIn == vtkSurfaceFileFilter) ||
       (filterNameIn == vtkXmlSurfaceFileFilter) ||
       (filterNameIn == wavefrontObjFileFilter)) {
      surfaceGroupBox->setHidden(false);
   }
   else if ((filterNameIn == vtkVolumeFileFilter) ||
            (filterNameIn == mincVolumeFileFilter) ||
            (filterNameIn == analyzeVolumeFileFilter)) {
      volumeGroupBox->setHidden(false);
   }
}

/**
 * called when a previous directory is selected.
 */
void 
GuiExportDataFileDialog::slotPreviousDirectory(int item)
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
GuiExportDataFileDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QString name(selectedFile());
      
      //
      // Update previous directories
      //
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      pf->addToRecentDataFileDirectories(dirPath(), true);
      
      //
      // If file exists, see if user wants to overwrite it.
      //
      QFileInfo fileInfo(name);
      if (fileInfo.exists()) {
         if (GuiMessageBox::warning(this, "Replace", "File exists.  Do you want to replace it?",
                                  "Yes", "No") != 0) {
            return;
         }
         if (fileInfo.isWritable() == false) {
            GuiMessageBox::critical(this, "File Error", "File is not writable!",
                                  "OK");
            return;
         }
      }
      
      const QString filterName = selectedFilter();
      
      bool exportingVolume = false;
      
      QString extension;
      if (filterName == GuiDataFileDialog::vtkSurfaceFileFilter) {
         extension = ".vtk";
      }
      else if (filterName == GuiDataFileDialog::vtkXmlSurfaceFileFilter) {
         extension = ".vtp";
      }
      else if (filterName == GuiDataFileDialog::byuSurfaceFileFilter) {
         extension = ".byu";
      }
      else if (filterName == freeSurferAsciiSurfaceFileFilter) {
         extension = ".asc";
      }
      else if (filterName == freeSurferAsciiCurvatureFileFilter) {
         extension = ".curve.asc";
      }
      else if (filterName == freeSurferAsciiFunctionalFileFilter) {
         extension = "w.asc";
      }
      else if (filterName == freeSurferAsciiLabelFileFilter) {
         extension = ".label";
      }
      else if (filterName == GuiDataFileDialog::openInventorSurfaceFileFilter) {
         extension = ".iv";
      }
      else if (filterName == GuiDataFileDialog::stlSurfaceFileFilter) {
         extension = ".stl";
      }
      else if (filterName == GuiDataFileDialog::studio3DSurfaceFileFilter) {
         extension = ".ase";
      }
      else if (filterName == GuiDataFileDialog::vrmlSurfaceFileFilter) {
         extension = ".wrl";
      }
      else if (filterName == GuiDataFileDialog::vtkVolumeFileFilter) {
         extension = ".vtk";
         exportingVolume = true;
      }
      else if (filterName == GuiDataFileDialog::analyzeVolumeFileFilter) {
         extension = SpecFile::getAnalyzeVolumeFileExtension();
         exportingVolume = true;
      }
      else if (filterName == GuiDataFileDialog::mincVolumeFileFilter) {
         extension = SpecFile::getMincVolumeFileExtension();
         exportingVolume = true;
      }
      else if (filterName == GuiDataFileDialog::wavefrontObjFileFilter) {
         extension = ".obj";
      }
      else {
         std::cout << "PROGRAMMING ERROR: File filter not handled properly at "
                  << __LINE__ << " in " << __FILE__ << std::endl;
         return;
      }
      
      const QString filesExtension = (".") + FileUtilities::filenameExtension(name);
      if (filesExtension != extension) {
         name.append(extension);
      }
      
      //
      // Get the brain model surface for export
      //
      BrainModelSurface* bms = NULL;
      VolumeFile* volumeFile = volumeSelectionControl->getSelectedVolumeFile();
      if (exportingVolume) {
         if (volumeFile == NULL) {
            GuiMessageBox::critical(this, "Error - No Volume"
                       "No Volume selectd for export.", "OK");
            return;
         }
      }
      else {
         bms = surfaceSelectionComboBox->getSelectedBrainModelSurface();
         if (bms == NULL) {
            GuiMessageBox::critical(this, "Error - No Topology",
               "Model selected for export is not a surface.",
               "OK");
            return;
         }
      }

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      QString msg;      
      try {
         if (filterName == GuiDataFileDialog::vtkSurfaceFileFilter) {
            theMainWindow->getBrainSet()->exportVtkSurfaceFile(bms, name, true); 
         }
         else if (filterName == GuiDataFileDialog::vtkXmlSurfaceFileFilter) {
            theMainWindow->getBrainSet()->exportVtkXmlSurfaceFile(bms, name, true); 
         }
         else if (filterName == GuiDataFileDialog::byuSurfaceFileFilter) {
            theMainWindow->getBrainSet()->exportByuSurfaceFile(bms, name); 
         }
         else if (filterName == freeSurferAsciiSurfaceFileFilter) {
            theMainWindow->getBrainSet()->exportFreeSurferAsciiSurfaceFile(bms, name);
         }
         else if (filterName == freeSurferAsciiCurvatureFileFilter) {
            SurfaceShapeFile* ssf = theMainWindow->getBrainSet()->getSurfaceShapeFile();
            QApplication::restoreOverrideCursor();
            QApplication::beep();
            std::vector<QString> labels;
            ssf->getAllColumnNames(labels);
            QtListBoxSelectionDialog lbd(this,
                                         "Choose Surface Shape Column",
                                         "",
                                         labels,
                                         0);
            if (lbd.exec() == QDialog::Accepted) {
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               const int column = lbd.getSelectedItemIndex();
               ssf->exportFreeSurferAsciiCurvatureFile(column,
                                                       bms->getCoordinateFile(),
                                                       name);
            }
            else {
               return;
            }
         }
         else if (filterName == freeSurferAsciiFunctionalFileFilter) {
            MetricFile* mf = theMainWindow->getBrainSet()->getMetricFile();
            QApplication::restoreOverrideCursor();
            QApplication::beep();
            std::vector<QString> labels;
            mf->getAllColumnNames(labels);
            QtListBoxSelectionDialog lbd(this,
                                         "Choose Metric Column",
                                         "",
                                         labels,
                                         0);
            if (lbd.exec() == QDialog::Accepted) {
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               const int column = lbd.getSelectedItemIndex();
               mf->exportFreeSurferAsciiFunctionalFile(column,
                                                       name);
            }
            else {
               return;
            }            
         }
         else if (filterName == freeSurferAsciiLabelFileFilter) {            
            PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
            QString prefixName(FileUtilities::dirname(name));
            if (prefixName.isEmpty() == false) {
               prefixName.append("/");
            }
            /*
            *switch(theMainWindow->getBrainSet()->getStructure()) {
            *   case BrainModelSurface::HEMISPHERE_LEFT:
            *      prefixName.append("lh-");
            *      break;
            *   case BrainModelSurface::HEMISPHERE_RIGHT:
            *      prefixName.append("rh-");
            *      break;
            *   case BrainModelSurface::HEMISPHERE_BOTH:
            *      prefixName.append("lrh-");
            *      break;
            *   case BrainModelSurface::HEMISPHERE_UNKNOWN:
            *      prefixName.append("uh-");
            *      break;
            *}
            */
            QApplication::restoreOverrideCursor();
            QApplication::beep();
            std::vector<QString> labels;
            pf->getAllColumnNames(labels);
            QtListBoxSelectionDialog lbd(this,
                                         "Choose Paint Column",
                                         "",
                                         labels,
                                         0);
            if (lbd.exec() == QDialog::Accepted) {
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               const int column = lbd.getSelectedItemIndex();
               pf->exportFreeSurferAsciiLabelFile(column, prefixName, 
                                                   bms->getCoordinateFile());
            }
            else {
               return;
            }
         }
         else if (filterName == GuiDataFileDialog::openInventorSurfaceFileFilter) {
            theMainWindow->getBrainSet()->exportInventorSurfaceFile(bms, name); 
         }
         else if (filterName == GuiDataFileDialog::stlSurfaceFileFilter) {
            theMainWindow->getBrainSet()->exportStlSurfaceFile(bms, name); 
         }
         else if (filterName == GuiDataFileDialog::studio3DSurfaceFileFilter) {
            theMainWindow->getBrainSet()->export3DStudioASESurfaceFile(bms, name);
         }
         else if (filterName == GuiDataFileDialog::vrmlSurfaceFileFilter) {
            theMainWindow->getBrainSet()->exportVrmlSurfaceFile(bms, name); 
         }
         else if (filterName == GuiDataFileDialog::wavefrontObjFileFilter) {
            theMainWindow->getBrainSet()->exportWavefrontSurfaceFile(bms, name);
         }
         else if (filterName == GuiDataFileDialog::vtkVolumeFileFilter) {
            theMainWindow->getBrainSet()->exportVtkStructuredPointsVolumeFile(volumeFile, name);
         }
         else if (filterName == GuiDataFileDialog::analyzeVolumeFileFilter) {
            theMainWindow->getBrainSet()->exportAnalyzeVolumeFile(volumeFile, name);
         }
         else if (filterName == GuiDataFileDialog::mincVolumeFileFilter) {
            theMainWindow->getBrainSet()->exportMincVolumeFile(volumeFile, name);
         }
         else {
            std::cout << "PROGRAMMING ERROR: File filter not handled properly at "
                     << __LINE__ << " in " << __FILE__ << std::endl;
         }
      }
      catch (FileException& e) {
         msg = e.whatQString();
      }
      
      QApplication::restoreOverrideCursor(); 
      
      if (msg.isEmpty() == false) {
         GuiMessageBox::critical(this, "Error Writing File", msg, "OK");
         return;
      }
   }
   
   Q3FileDialog::done(r);
}
