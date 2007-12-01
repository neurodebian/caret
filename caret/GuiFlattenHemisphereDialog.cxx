
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurfaceFlattenFullHemisphere.h"
#include "BrainModelSurfaceFlattenPartialHemisphere.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "FileUtilities.h"
#include "GuiFilesModified.h"
#include "GuiFlattenHemisphereDialog.h"
#include "GuiFlattenHemisphereInstructionsDialog.h"
#include "GuiMainWindow.h"
#include "PaintFile.h"
#include "ParamsFile.h"
#include "QtUtilities.h"
#include "TopologyFile.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiFlattenHemisphereDialog::GuiFlattenHemisphereDialog(QWidget* parent)
   : QtDialog(parent, true)
{
   setWindowTitle("Flatten Full or Partial Hemisphere");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // Group box for flattening type
   //
   QGroupBox* flatTypeGroupBox = new QGroupBox("Flattening Type"); 
   dialogLayout->addWidget(flatTypeGroupBox);
   QVBoxLayout* flatTypeGroupLayout = new QVBoxLayout(flatTypeGroupBox);
   
   //
   // Combo box for flattening type selection
   //
   flatTypeComboBox = new QComboBox;
   flatTypeGroupLayout->addWidget(flatTypeComboBox);
   flatTypeComboBox->addItem("Select Type", FLATTEN_TYPE_NONE);
   flatTypeComboBox->addItem("Full Hemisphere (Ellipsoid)", 
                                FLATTEN_TYPE_FULL_HEMISPHERE);
   flatTypeComboBox->addItem("Full Hemisphere (Ellipsoid) and Morph Sphere", 
                                FLATTEN_TYPE_FULL_HEMISPHERE_AND_MORPH_SPHERE);
   flatTypeComboBox->addItem("Partial Hemsiphere (Ellipsoid)", 
                                FLATTEN_TYPE_ELLIPSOID_PARTIAL_HEMISPHERE);
   flatTypeComboBox->addItem("Partial Hemisphere (Fiducial)", 
                                FLATTEN_TYPE_FIDUCIAL_PARTIAL_HEMISPHERE);
   QObject::connect(flatTypeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotFlatTypeComboBox(int)));
                       
   //
   // Group box for surfaces
   //
   QGroupBox* surfaceGroupBox = new QGroupBox("Surfaces");
   dialogLayout->addWidget(surfaceGroupBox);
   QGridLayout* surfaceGroupGridLayout = new QGridLayout(surfaceGroupBox);
   
   //
   // Fiducial surface
   //
   surfaceGroupGridLayout->addWidget(new QLabel("Fiducial Surface"), 0, 0);
   fiducialSurfaceComboBox = new GuiBrainModelSelectionComboBox(
                                             false, true, false,
                                             "", 0); 
   QObject::connect(fiducialSurfaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(loadAnteriorCommissureInformation()));
   fiducialSurfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
   surfaceGroupGridLayout->addWidget(fiducialSurfaceComboBox, 0, 1);
   
   //
   // flatten surface
   //
   surfaceGroupGridLayout->addWidget(new QLabel("Surface To Flatten"), 1, 0);
   flattenSurfaceComboBox = new GuiBrainModelSelectionComboBox(false, true, false,
                                                               "", 0);   
   surfaceGroupGridLayout->addWidget(flattenSurfaceComboBox);
   surfaceGroupBox->setFixedSize(surfaceGroupBox->sizeHint());

   //
   // Vertical group for full hemisphere parameters
   //
   fullHemisphereParamsGroupBox = new QGroupBox("Full Hemisphere Parameters");
   dialogLayout->addWidget(fullHemisphereParamsGroupBox);
   QVBoxLayout* fullHemisphereParamsLayout = new QVBoxLayout(fullHemisphereParamsGroupBox);

   //
   // grid for surfaces
   //
   QWidget* acGrid = new QWidget;
   QGridLayout* acGridLayout = new QGridLayout(acGrid);
   
   //
   // AC position
   //
   acPositionLineEdit[0] = new QLineEdit;
   acPositionLineEdit[0]->setFixedWidth(70);
   acPositionLineEdit[1] = new QLineEdit;
   acPositionLineEdit[1]->setFixedWidth(70);
   acPositionLineEdit[2] = new QLineEdit;
   acPositionLineEdit[2]->setFixedWidth(70);
   acGridLayout->addWidget(new QLabel("Anterior Commissure Position"), 0, 0);
   acGridLayout->addWidget(acPositionLineEdit[0], 0, 1);
   acGridLayout->addWidget(acPositionLineEdit[1], 0, 2);
   acGridLayout->addWidget(acPositionLineEdit[2], 0, 3);
   
   //
   // AC offset
   //
   acOffsetLineEdit[0] = new QLineEdit;
   acOffsetLineEdit[0]->setFixedWidth(70);
   acOffsetLineEdit[1] = new QLineEdit;
   acOffsetLineEdit[1]->setFixedWidth(70);
   acOffsetLineEdit[2] = new QLineEdit;
   acOffsetLineEdit[2]->setFixedWidth(70);
   acGridLayout->addWidget(new QLabel("Anterior Commissure Offset"), 1, 0);
   acGridLayout->addWidget(acOffsetLineEdit[0], 1, 1);
   acGridLayout->addWidget(acOffsetLineEdit[1], 1, 2);
   acGridLayout->addWidget(acOffsetLineEdit[2], 1, 3);
   
   //
   // squish the stuff together
   //
   acGrid->setFixedSize(acGrid->sizeHint());

   //
   // Horizontal box for border label and combo box
   QHBoxLayout* borderHBoxLayout = new QHBoxLayout;
   fullHemisphereParamsLayout->addLayout(borderHBoxLayout);
   
   //
   // Label & Combo box for border selection
   //
   borderHBoxLayout->addWidget(new QLabel("Border Template Cuts"));
   
   templateBorderComboBox = new QComboBox;
   borderHBoxLayout->addWidget(templateBorderComboBox);
   borderHBoxLayout->setStretchFactor(templateBorderComboBox, 100);
   loadTemplateBorderComboBox();
                                       
   //
   // Smooth fiducial medial wall check box
   //
   smoothFiducialMedialWallCheckBox = new QCheckBox("Smooth Fiducial Medial Wall");
   fullHemisphereParamsLayout->addWidget(smoothFiducialMedialWallCheckBox);
   smoothFiducialMedialWallCheckBox->setChecked(true);
                                                   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // OK button
   //
   QPushButton* okButton = new QPushButton("OK");
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   
   QtUtilities::makeButtonsSameSize(okButton, cancelButton);
   
   //
   // Initialize some items
   //
   flattenType = FLATTEN_TYPE_NONE;
   slotFlatTypeComboBox(static_cast<int>(FLATTEN_TYPE_NONE));
   loadAnteriorCommissureInformation();
}

/**
 * Destructor
 */
GuiFlattenHemisphereDialog::~GuiFlattenHemisphereDialog()
{
}

/**
 * Load the full hemisphere border selection combo box with
 * the template border files from the caret home directory.
 */
void
GuiFlattenHemisphereDialog::loadTemplateBorderComboBox()
{
   //
   // First and second entries correspond to "Choose Template Borders" and
   // and use currently loaded borders.
   //
   templateBorderFiles.push_back("");
   templateBorderFiles.push_back("");
   templateBorderColorFiles.push_back("");
   templateBorderColorFiles.push_back("");
   templateBorderComboBox->addItem("Choose Template Borders");
   templateBorderComboBox->addItem("Use Currently Loaded Ellipsoidal Borders");
   
   //
   // Look for template borders in Caret installation directory.
   //
   QString templateBorderDirectory(theMainWindow->getBrainSet()->getCaretHomeDirectory());
   templateBorderDirectory.append("/data_files/flatten_borders");   
   std::vector<QString> files;
   FileUtilities::findFilesInDirectory(templateBorderDirectory,
                                       QStringList("*.border"),
                                       files);
    
   //
   // Load template border information into combo box
   //
   for (unsigned int i = 0; i < files.size(); i++) {
      BorderFile bf;
      try {
         QString borderName(templateBorderDirectory);
         borderName.append("/");
         borderName.append(files[i]);
         templateBorderFiles.push_back(borderName);
         
         bf.readFile(borderName);
         
         const QString descriptiveName(bf.getHeaderTag("descriptive_name"));
         if (descriptiveName.isEmpty() == false) {
            templateBorderComboBox->addItem(descriptiveName);
         }
         else {
            templateBorderComboBox->addItem(files[i]);
         }
         
         const QString colorName(bf.getHeaderTag("matching_color_file"));
         if (colorName.isEmpty() == false) {
            QString colorFileName(templateBorderDirectory);
            colorFileName.append("/");
            colorFileName.append(colorName);
            templateBorderColorFiles.push_back(colorFileName);
         }
         else {
            templateBorderColorFiles.push_back("");
         }
      }
      catch (FileException& /*e*/) {
      }
   }
}

/**
 * load the anterior commissure information from the params file.
 */
void
GuiFlattenHemisphereDialog::loadAnteriorCommissureInformation()
{
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   float ac[3];
   if (pf->getParameter(ParamsFile::keyACx, ac[0]) &&
       pf->getParameter(ParamsFile::keyACy, ac[1]) &&
       pf->getParameter(ParamsFile::keyACz, ac[2])) {
      acPositionLineEdit[0]->setText(QString::number(ac[0], 'f', 1));
      acPositionLineEdit[1]->setText(QString::number(ac[1], 'f', 1));
      acPositionLineEdit[2]->setText(QString::number(ac[2], 'f', 1));
   }
   acOffsetLineEdit[0]->setText("0.0");
   acOffsetLineEdit[1]->setText("-10.0");
   acOffsetLineEdit[2]->setText("10.0");
   
   const BrainModelSurface* fid = fiducialSurfaceComboBox->getSelectedBrainModelSurface();
   if (fid != NULL) {
      float bounds[6];
      fid->getBounds(bounds);
      
      //
      // Check min/max Y coordinates
      //
      if ((bounds[2] < 0.0) && (bounds[3] > 0.0)) {
         //
         // Surface appears to have the Anterior Commissure as its origin
         //
         acPositionLineEdit[0]->setText("0");
         acPositionLineEdit[1]->setText("0");
         acPositionLineEdit[2]->setText("0");         
      }
   }
}

/**
 * Called when a flatten type is selected.
 */
void
GuiFlattenHemisphereDialog::slotFlatTypeComboBox(int item)
{
   flattenType = static_cast<FLATTEN_TYPE>(item);
   
   fullHemisphereParamsGroupBox->setEnabled(false);
   
   switch(flattenType) {
      case FLATTEN_TYPE_NONE:
         break;
      case FLATTEN_TYPE_FULL_HEMISPHERE:
      case FLATTEN_TYPE_FULL_HEMISPHERE_AND_MORPH_SPHERE:
         flattenSurfaceComboBox->setSelectedBrainModel(
            theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(
                  BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL));
         fullHemisphereParamsGroupBox->setEnabled(true);
         break;
      case FLATTEN_TYPE_ELLIPSOID_PARTIAL_HEMISPHERE:
         flattenSurfaceComboBox->setSelectedBrainModel(
            theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(
                  BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL));
         break;
      case FLATTEN_TYPE_FIDUCIAL_PARTIAL_HEMISPHERE:
         flattenSurfaceComboBox->setSelectedBrainModel(theMainWindow->getBrainSet()->getActiveFiducialSurface());
         break;
   }
   
   loadAnteriorCommissureInformation();
}

/**
 * Called when OK or Cancel buttons pressed
 */
void
GuiFlattenHemisphereDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      //
      // border file used during full hemisphere flattening
      //
      BorderFile* flattenBorderFile = NULL;
      bool flattenBordersUseTemplateFile = false;
      
      //
      // Get the selected surfaces
      //
      BrainModelSurface* fiducialSurface = 
         fiducialSurfaceComboBox->getSelectedBrainModelSurface();
      BrainModelSurface* flattenSurface = 
         flattenSurfaceComboBox->getSelectedBrainModelSurface();
      
      //
      // declare the border file here so that it is in scope when execute is called
      //
      BorderFile borderFile;
      
      //
      // Create the algorithm
      //
      BrainModelAlgorithm* bma = NULL;
      switch(flattenType) {
         case FLATTEN_TYPE_NONE:
            {
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(this, "Flatten Error",
                                    "You must select a flattening type.");
               return;
            }
            break;
         case FLATTEN_TYPE_ELLIPSOID_PARTIAL_HEMISPHERE:
            bma = new BrainModelSurfaceFlattenPartialHemisphere(
                                    fiducialSurface,
                                    flattenSurface,
                     BrainModelSurfaceFlattenPartialHemisphere::FLATTEN_TYPE_ELLIPSOID);
            break;
         case FLATTEN_TYPE_FIDUCIAL_PARTIAL_HEMISPHERE:
            bma = new BrainModelSurfaceFlattenPartialHemisphere(
                                    fiducialSurface,
                                    flattenSurface,
                     BrainModelSurfaceFlattenPartialHemisphere::FLATTEN_TYPE_FIDUCIAL);
            break;
         case FLATTEN_TYPE_FULL_HEMISPHERE:
         case FLATTEN_TYPE_FULL_HEMISPHERE_AND_MORPH_SPHERE:
            {
               PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
               if (pf->getNumberOfColumns() <= 0) {
                  QApplication::restoreOverrideCursor();
                  if (QMessageBox::question(this, "Paint File Query",
                     "There is no paint file loaded.  The paint file may assist "
                     "you in identifying cortical areas.  Do you want to continue "
                     "or do you want to cancel and load a paint file ?",
                     (QMessageBox::Ok | QMessageBox::Cancel),
                        QMessageBox::Ok) == QMessageBox::Cancel) {
                     return;
                  }
                  
                  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               }
               const int templateBorderIndex = templateBorderComboBox->currentIndex();
               if (templateBorderIndex <= 0) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(this, "Missing Borders",
                      "You must selecte template borders.");
                  return;
               }
               else if (templateBorderIndex == 1) {
                  BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
                  flattenBorderFile = 
                     bmbs->copyBordersOfSpecifiedType(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL);
                  if (flattenBorderFile == NULL) {
                     QApplication::restoreOverrideCursor();
                     QMessageBox::critical(this, "Missing Borders",
                        "There are no ellipsoidal borders loaded.");
                     return;
                  }
               }
               else {
                  flattenBorderFile = new BorderFile;
                  try {
                     flattenBorderFile->readFile(templateBorderFiles[templateBorderIndex]);
                     flattenBordersUseTemplateFile = true;
                  }
                  catch (FileException& e) {
                     QApplication::restoreOverrideCursor();
                     QMessageBox::critical(this, "Missing Borders",
                                           e.whatQString());
                     return;
                  }
                  
                  try {
                     theMainWindow->getBrainSet()->readBorderColorFile(templateBorderColorFiles[templateBorderIndex],
                                                false,
                                                false);
                  }
                  catch (FileException& /*e*/) {
                  }
               }
               
               //
               // Check for topological errors
               //
               if (flattenSurface != NULL) {
                  const TopologyFile* tf = flattenSurface->getTopologyFile();
                  if (tf != NULL) {
                     int faces, vertices, edges, eulerCount, numberOfHoles, numberOfObjects;
                     tf->getEulerCount(false,
                                       faces,
                                       vertices,
                                       edges,
                                       eulerCount,
                                       numberOfHoles,
                                       numberOfObjects);
                     if (numberOfHoles > 0) {
                        const QString msg = (QString("There appear to be ") + 
                                             QString::number(numberOfHoles) +
                                             " handles (topological errors) in the surface."
                                             "  These handles may cause problems "
                                             " during surface flattening.\n\n"
                                             "Do you want to continue?");
                        QApplication::restoreOverrideCursor();
                        if (QMessageBox::warning(this, 
                                                   "Toplogical Errors",
                                                   msg,
                                                   (QMessageBox::Yes | QMessageBox::No),
                                                   QMessageBox::No)
                                                      == QMessageBox::No) {
                           return;
                        }
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                     }
                  }
               }
               
               //
               // Get the Anterior Commissure position and offset
               //
               float acPosition[3];
               float acOffset[3];
               acPosition[0] = acPositionLineEdit[0]->text().toFloat();
               acPosition[1] = acPositionLineEdit[1]->text().toFloat();
               acPosition[2] = acPositionLineEdit[2]->text().toFloat();
               acOffset[0]   = acOffsetLineEdit[0]->text().toFloat();
               acOffset[1]   = acOffsetLineEdit[1]->text().toFloat();
               acOffset[2]   = acOffsetLineEdit[2]->text().toFloat();
               if ((acPosition[0] == 0.0) &&
                   (acPosition[1] == 0.0) &&
                   (acPosition[2] == 0.0)) {
                  QApplication::restoreOverrideCursor();
                  if (QMessageBox::warning(this, "Missing AC",
                      "The Anterior Commissure position is (0.0, 0.0, 0.0).\n"
                      "Is this correct?",
                      (QMessageBox::Yes | QMessageBox::No),
                      QMessageBox::No) 
                         == QMessageBox::No) {
                     return;
                  }
                  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               }
               
               //
               // Create the flattening algorithm
               //
               bma = new BrainModelSurfaceFlattenFullHemisphere(
                                       fiducialSurface,
                                       flattenSurface,
                                       flattenBorderFile,
                                       acPosition,
                                       acOffset,
                                       smoothFiducialMedialWallCheckBox->isChecked());
            }
            break;
      }
      
      try {
         bma->execute();
      
         if (flattenBordersUseTemplateFile) {
            if (flattenBorderFile != NULL) {
               delete flattenBorderFile;
            }
         }
         
         theMainWindow->speakText("The initial flat surface is ready.", false);
         
         //
         // Do not delete the flattening algorithm for the full
         // hemisphere since there is more to do.
         //
         switch(flattenType) {
            case FLATTEN_TYPE_NONE:
               break;
            case FLATTEN_TYPE_ELLIPSOID_PARTIAL_HEMISPHERE:
            case FLATTEN_TYPE_FIDUCIAL_PARTIAL_HEMISPHERE:
               {
                  //
                  // Popup instructions for post initial flattening
                  //
                  GuiFlattenHemisphereInstructionsDialog* hid = 
                     new GuiFlattenHemisphereInstructionsDialog(theMainWindow,
                        NULL,
                        GuiFlattenHemisphereInstructionsDialog::DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_PART_HEM);
                  hid->setAttribute(Qt::WA_DeleteOnClose);
                  hid->show();  
                  delete bma;
               }
               break;
            case FLATTEN_TYPE_FULL_HEMISPHERE:
               {
                  //
                  // Popup instructions for second half of full
                  // hemisphere initial flattening
                  //
                  GuiFlattenHemisphereInstructionsDialog* hid = 
                     new GuiFlattenHemisphereInstructionsDialog(theMainWindow,
                        dynamic_cast<BrainModelSurfaceFlattenFullHemisphere*>(bma),
   GuiFlattenHemisphereInstructionsDialog::DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_PART2);
                  hid->setAttribute(Qt::WA_DeleteOnClose);
                  hid->show();  
               }
               break;
            case FLATTEN_TYPE_FULL_HEMISPHERE_AND_MORPH_SPHERE:
               {
                  //
                  // Popup instructions for second half of full
                  // hemisphere initial flattening
                  //
                  GuiFlattenHemisphereInstructionsDialog* hid = 
                     new GuiFlattenHemisphereInstructionsDialog(theMainWindow,
                        dynamic_cast<BrainModelSurfaceFlattenFullHemisphere*>(bma),
   GuiFlattenHemisphereInstructionsDialog::DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_MORPH_SPHERE_PART2);
                  hid->setAttribute(Qt::WA_DeleteOnClose);
                  hid->show();  
               }
               break;
         }
      }
      catch (BrainModelAlgorithmException& e) {
         delete bma;
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(this, "Error", e.whatQString());
         return;
      }
      
      //
      // Notify about file changes
      //
      GuiFilesModified fm;
      fm.setCoordinateModified();
      fm.setTopologyModified();
      theMainWindow->fileModificationUpdate(fm);
      theMainWindow->displayBrainModelInMainWindow(-1);
      
      //
      // Do crossover check and set overlay node coloring to crossovers
      //
      BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(
                                    theMainWindow->getBrainSet()->getNumberOfBrainModels() - 1);
      if (bms != NULL) {
         int numTileCrossovers, numNodeCrossovers;
         bms->crossoverCheck(numTileCrossovers, numNodeCrossovers,
                             BrainModelSurface::SURFACE_TYPE_FLAT);
      }
      int modelIndex = -1;  // -1 -> all models
      BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
      bsnc->setSecondaryOverlay(modelIndex, BrainModelSurfaceNodeColoring::OVERLAY_NONE);
      bsnc->setPrimaryOverlay(modelIndex, BrainModelSurfaceNodeColoring::OVERLAY_SHOW_CROSSOVERS);
      bsnc->assignColors();

      //
      // update display control dialog and update graphics
      //
      theMainWindow->updateDisplayControlDialog();
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
   
   QDialog::done(r);
}
