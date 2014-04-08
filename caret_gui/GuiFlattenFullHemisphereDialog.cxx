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

#include <algorithm>

#include <QApplication>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFileInfoList>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListIterator>
#include <QListWidget>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QTime>
#include <QVBoxLayout>

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceBorderLandmarkIdentification.h"
#include "BrainModelSurfaceFlattenHemisphere.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainSet.h"
#include "DisplaySettingsBorders.h"
#include "GuiBorderOperationsDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSurfaceSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiFlattenFullHemisphereDialog.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowSurfaceActions.h"
#include "GuiMorphingMeasurementsDialog.h"
#include "GuiMultiresolutionMorphingDialog.h"
#include "PaintFile.h"
#include "QtUtilities.h"
#include "TopologyFile.h"
#include "VocabularyFile.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiFlattenFullHemisphereDialog::GuiFlattenFullHemisphereDialog(QWidget* parent)
   : WuQWizard(parent)
{
   setWindowTitle("Flatten Full Hemisphere");
   
   flatParameters = new BrainModelSurfaceMultiresolutionMorphing(
                                         NULL,
                                         NULL,
                                         NULL,
                     BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT,
                                         NULL);          
                                         
   sphericalParameters = new BrainModelSurfaceMultiresolutionMorphing(
                                         NULL,
                                         NULL,
                                         NULL,
                     BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL,
                                         NULL);          
                                         
   setOption(QWizard::NoCancelButton, false);
   setWizardStyle(ModernStyle);
   
   introPage = new GuiFlattenFullHemisphereIntroPage;
   addPage(introPage);
   
   settingsPage = new GuiFlattenFullHemisphereSettingsPage;
   addPage(settingsPage);
   
   fiducialSurfacePage = new GuiFlattenFullHemisphereFiducialPage;
   addPage(fiducialSurfacePage);
   
   sphericalSurfacePage = new GuiFlattenFullHemisphereSphericalPage;
   addPage(sphericalSurfacePage);
   
   templateCutsPage = new GuiFlattenFullHemisphereTemplateCutsPage;
   addPage(templateCutsPage);
   
   borderDrawAndUpdatePage = new GuiFlattenFullHemisphereBorderUpdatePage;
   addPage(borderDrawAndUpdatePage);
   
   surfaceAlignmentPage = new GuiFlattenFullHemisphereSurfaceAlignmentPage;
   addPage(surfaceAlignmentPage);  

   startFlatteningPage = new GuiFlattenFullHemisphereStartFlatteningPage;
   addPage(startFlatteningPage);
   
   initialFlatPage = new GuiFlattenFullHemisphereInitialFlatPage;
   addPage(initialFlatPage);
   
   multiMorphPage = new GuiFlattenFullHemisphereMultiresolutionMorphingPage(
                                         flatParameters,
                                         sphericalParameters);
   addPage(multiMorphPage);
   
   finishedPage = new GuiFlattenFullHemisphereFinishedPage;
   addPage(finishedPage);
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereDialog::~GuiFlattenFullHemisphereDialog()
{
   if (flatParameters != NULL) {
      delete flatParameters;
   }
   if (sphericalParameters != NULL) {
      delete sphericalParameters;
   }
}

/**
 * show the first page.
 */
void 
GuiFlattenFullHemisphereDialog::showFirstPage()
{
   restart();
}
      
/**
 * called when cancel button pressed.
 */
void 
GuiFlattenFullHemisphereDialog::reject()
{
   if ((currentPage() != introPage) &&
       (currentPage() != finishedPage)) {
      if (WuQMessageBox::question(this,
                                  "ERROR",
                                  "Flattening is not complete.  Stop Flattening?",
                                  WuQMessageBox::Yes | WuQMessageBox::No,
                                  WuQMessageBox::No) == WuQMessageBox::Yes) {
         QWizard::reject();
      }
      else {
         return;
      }
   }
   
   QWizard::reject();
}

/**
 * set the central sulcus ventral tip node number.
 */
void 
GuiFlattenFullHemisphereDialog::setCentralSulcusVentralTip(const int nodeNum)
{
   surfaceAlignmentPage->setCentralSulcusVentralTip(nodeNum);
}

/**
 * set the central sulcus dorsal-medial tip node number.
 */
void 
GuiFlattenFullHemisphereDialog::setCentralSulcusDorsalMedialTip(const int nodeNum)
{
   surfaceAlignmentPage->setCentralSulcusDorsalMedialTip(nodeNum);
}
      
/**
 * get the spherical surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereDialog::getSphericalSurface()
{
   return sphericalSurfacePage->getSphericalSurface();
}
      
/**
 * get the inflated surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereDialog::getInflatedSurface()
{
   return sphericalSurfacePage->getInflatedSurface();
}
      
/**
 * get the very inflated surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereDialog::getVeryInflatedSurface()
{
   return sphericalSurfacePage->getVeryInflatedSurface();
}
      
/**
 * get the fiducial surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereDialog::getFiducialSurface()
{
   return fiducialSurfacePage->getFiducialSurface();
}
      
/**
 * get the auto save files status.
 */
bool 
GuiFlattenFullHemisphereDialog::getAutoSaveFilesSelected() const
{
   return settingsPage->getAutoSaveFilesSelected();
}
      
/**
 * called to validate the current page.
 */
bool 
GuiFlattenFullHemisphereDialog::validateCurrentPage()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   
   const QWizardPage* p = currentPage();

   if (p == fiducialSurfacePage) {
      //
      // Verify fiducial surface with topology
      //
      BrainModelSurface* fiducialSurface = getFiducialSurface();
      if (fiducialSurface== NULL) {
         WuQMessageBox::critical(this, "ERROR", "There must be a fiducial surface to continue.");
         return false;
      }
      const TopologyFile* tf = fiducialSurface->getTopologyFile();
      if (tf == NULL) {
         WuQMessageBox::critical(this, "ERROR", "The fiducial surface has no topology.");
         return false;
      }
      
      //
      // Check topology
      //
      showWaitCursor();
      int faces, vertices, edges, eulerCount, numberOfHoles, numberOfObjects;
      tf->getEulerCount(false, faces, vertices, edges, 
                        eulerCount, numberOfHoles, numberOfObjects);
      showNormalCursor();
      
      QString msg;
      if (eulerCount != 2) {
         msg += "The surface is not topologically correct.\n";
      }         
      if (numberOfHoles > 0) {
         msg += "The surface contains "
                + QString::number(numberOfHoles)
                + " holes.\n";
      }
      if (numberOfObjects > 1) {
         msg += "The surface contains "
                + QString::number(numberOfObjects)
                + " unconnected pieces.\n";
      }
      if (msg.isEmpty() == false) {
         msg += "These problems should be corrected before continuing.\n\n"
                "Do you want to continue?";
         
         if (WuQMessageBox::question(this, "INFO", msg,
                                     WuQMessageBox::Yes | WuQMessageBox::No,
                                     WuQMessageBox::No) == WuQMessageBox::No) {
            return false;
         }
      }
   }
   
   if (p == sphericalSurfacePage) {
      QString msg;
      if (getFiducialSurface() == NULL) {
         msg += "A Fiducial Surface is required.\n";
      }
      if (getSphericalSurface() == NULL) {
         msg += "A Spherical Surface is required.\n";
      }
      
      if (msg.isEmpty() == false) {
         WuQMessageBox::critical(this, "ERROR", msg);
         return false;
      }
   }
   
   if (p == templateCutsPage) {
      saveNewBorders();
   }
   
   if (p == borderDrawAndUpdatePage) {
      saveNewBorders();
      
      std::vector<QString> flattenBorderNames;
      startFlatteningPage->getLoadedFlatteningBorders(flattenBorderNames);
      
      bool haveMedialWallFlag = false;
      int numberOfCuts = 0;
      for (std::vector<QString>::iterator iter = flattenBorderNames.begin();
           iter != flattenBorderNames.end();
           iter++) {
         if (*iter == BrainModelSurfaceBorderLandmarkIdentification::getFlattenMedialWallBorderName()) {
            haveMedialWallFlag = true;
         }
         else if (iter->startsWith(
            BrainModelSurfaceBorderLandmarkIdentification::getFlattenStandardCutsBorderNamePrefix())) {
            numberOfCuts++;
         }
      }
      
      QString msg;
      if (haveMedialWallFlag == false) {
         msg += ("There is no medial wall border named \""
                 + BrainModelSurfaceBorderLandmarkIdentification::getFlattenMedialWallBorderName()
                 + "\".\n");
      }
      if (numberOfCuts < 1) {
         msg += ("There must be at least one cut border whose name begins with \""
                 + BrainModelSurfaceBorderLandmarkIdentification::getFlattenStandardCutsBorderNamePrefix()
                 + "\".\n");
      } 
      
      if (msg.isEmpty() == false) {
         WuQMessageBox::critical(this, "ERROR", msg);
         return false;
      }
   }

   if (p == surfaceAlignmentPage) {
      int ventralNodeNumber = -1;
      int dorsalMedialNodeNumber = -1;
      surfaceAlignmentPage->getCentralSulcusTips(ventralNodeNumber, 
                                                 dorsalMedialNodeNumber);
      if ((ventralNodeNumber < 0) ||
          (dorsalMedialNodeNumber < 0)) {
         WuQMessageBox::critical(this, "ERROR", 
                                 "The nodes at the ventral and dorsal-medial ends of the Central Sulcus must be set.");
         return false;
      }
   }
   
   if (p == startFlatteningPage) {
      BorderProjectionFile flattenBorderProjections;
      bs->getBorderSet()->copyBordersToBorderProjectionFile(
                                                   flattenBorderProjections);
      BrainModelSurfaceFlattenHemisphere 
         flatten(bs,
                 fiducialSurfacePage->getFiducialSurface(),
                 sphericalSurfacePage->getSphericalSurface(),
                 &flattenBorderProjections,
                 bs->getPaintFile(),
                 bs->getAreaColorFile(),
                 settingsPage->getCreateFiducialWithSmoothedMedialWallSelected(),
                 getAutoSaveFilesSelected());
                 
      try {
         showWaitCursor();
         flatten.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         showNormalCursor();
         WuQMessageBox::critical(this, "ERROR", e.whatQString());
         return false;
      }
      
      theMainWindow->displayNewestSurfaceInMainWindow();
      
      BrainModelSurface* initialFlatSurface =
         theMainWindow->getBrainModelSurface();
      if (initialFlatSurface != NULL) {
         if (initialFlatSurface->getIsFlatSurface()) {
            int numTileCrossovers = 0,
                numNodeCrossovers = 0;
            initialFlatSurface->crossoverCheck(numTileCrossovers, 
                                          numNodeCrossovers, 
                                          initialFlatSurface->getSurfaceType());
            if ((numNodeCrossovers > 0) || (numTileCrossovers > 0)) {
               bs->getPrimarySurfaceOverlay()->setOverlay(-1,
                                       BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS);
               theMainWindow->updateDisplayControlDialog();
               BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
               bsnc->assignColors();
            }
         }
      }
      
      DisplaySettingsBorders* dsb = bs->getDisplaySettingsBorders();
      dsb->setDisplayBorders(false);
      
      GuiFilesModified fm;
      fm.setPaintModified();
      fm.setAreaColorModified();
      fm.setCoordinateModified();
      fm.setTopologyModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();

      showNormalCursor();
   }
   
   if (p == initialFlatPage) {
      saveSurfaces(0, 0);
      saveNewBorders();
   }
   
   if (p == multiMorphPage) {
      showWaitCursor();
      
      //
      // Create a central sulcus border for surface alignment
      //
      int ventralNodeNumber, dorsalNodeNumber;
      surfaceAlignmentPage->getCentralSulcusTips(ventralNodeNumber, dorsalNodeNumber);
      BorderProjection centralSulcusBorderProjection(
         BrainModelSurfaceBorderLandmarkIdentification::getCentralSulcusRegistrationLandmarkName());
      BorderProjection* centralSulcusPointer = NULL;
      if ((ventralNodeNumber >= 0) &&
          (dorsalNodeNumber >= 0)) {         
         const int ventralVertices[3] = { ventralNodeNumber, ventralNodeNumber, ventralNodeNumber };
         const float ventralAreas[3] = { 1.0, 0.0, 0.0 };
         const int dorsalVertices[3] = { dorsalNodeNumber, dorsalNodeNumber, dorsalNodeNumber };
         const float dorsalAreas[3] = { 1.0, 0.0, 0.0 };
         
         BorderProjectionLink ventralLink(0,
                                          ventralVertices,
                                          ventralAreas,
                                          1.0);
         BorderProjectionLink dorsalLink(0,
                                         dorsalVertices,
                                         dorsalAreas,
                                         1.0);
         centralSulcusBorderProjection.addBorderProjectionLink(ventralLink);
         centralSulcusBorderProjection.addBorderProjectionLink(dorsalLink);
         centralSulcusPointer = &centralSulcusBorderProjection;
      }

      //
      // Measurements output by flat and spherical multi-res morph
      //
      std::vector<MorphingMeasurements> flatMeasurements;
      std::vector<MorphingMeasurements> sphericalMeasurements;
      float flatTime = 0.0, sphereTime = 0.0;

      //
      // Flat morphing
      //
      BrainModelSurface* surfaceToDisplay = NULL;
      if (multiMorphPage->getDoFlatMultiResMorphing()) {
         try {
            BrainModelSurfaceMultiresolutionMorphing morph(bs,
                             fiducialSurfacePage->getFiducialSurface(),
                             bs->getBrainModelSurfaceOfType(
                                BrainModelSurface::SURFACE_TYPE_FLAT),
                             BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT,
                             centralSulcusPointer);
            morph.copyParameters(*flatParameters);
            morph.setAutoSaveAllFiles(getAutoSaveFilesSelected());
            QTime flatTimer;
            flatTimer.start();
            morph.execute();
            flatTime = flatTimer.elapsed() * 0.001;
            morph.getMorphingMeasurements(flatMeasurements);
            surfaceToDisplay = bs->getBrainModelSurfaceOfType(
                                BrainModelSurface::SURFACE_TYPE_FLAT);
                                
            saveSurfaces(0, 0);
         }
         catch (BrainModelAlgorithmException& e) {
            showNormalCursor();
            WuQMessageBox::critical(this, "ERROR", e.whatQString());
            return false;
         }
      }
      
      //
      // spherical morphing
      //
      if (multiMorphPage->getDoSphericalMultiResMorphing()) {
         try {
            BrainModelSurfaceMultiresolutionMorphing morph(bs,
                             fiducialSurfacePage->getFiducialSurface(),
                             sphericalSurfacePage->getSphericalSurface(),
                             BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL,
                             centralSulcusPointer);
            morph.copyParameters(*sphericalParameters);
            morph.setAutoSaveAllFiles(getAutoSaveFilesSelected());
            QTime sphereTimer;
            sphereTimer.start();
            morph.execute();
            sphereTime = sphereTimer.elapsed() * 0.001;
            morph.getMorphingMeasurements(sphericalMeasurements);
            if (surfaceToDisplay == NULL) {
               surfaceToDisplay = bs->getBrainModelSurfaceOfType(
                                   BrainModelSurface::SURFACE_TYPE_SPHERICAL);
            }
                                
            saveSurfaces(0, 0);
         }
         catch (BrainModelAlgorithmException& e) {
            showNormalCursor();
            WuQMessageBox::critical(this, "ERROR", e.whatQString());
            return false;
         }
      }
      
      if (surfaceToDisplay != NULL) {
         theMainWindow->displayBrainModelInMainWindow(surfaceToDisplay);
      }
      
      GuiFilesModified fm;
      fm.setCoordinateModified();
      fm.setTopologyModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();

      if (flatMeasurements.empty() == false) {
         GuiMorphingMeasurementsDialog* flatMD = 
            new GuiMorphingMeasurementsDialog(flatMeasurements,
                                              flatTime,
                             BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT,
                                              true,
                                              this);
         flatMD->show();
      }
      
      if (sphericalMeasurements.empty() == false) {
         GuiMorphingMeasurementsDialog* sphereMD =
            new GuiMorphingMeasurementsDialog(sphericalMeasurements,
                                          sphereTime,
                             BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL,
                                          true,
                                          this);
         sphereMD->show();
      }
      
      saveNewBorders();
      
      showNormalCursor();
   }
   
   return true;
}
      
/**
 * update the dialog.
 */
void 
GuiFlattenFullHemisphereDialog::updateDialog()
{
   const QWizardPage* p = currentPage();
   if (p == introPage) {
      introPage->initializePage();
   }
   else if (p == settingsPage) {
      settingsPage->initializePage();
   }
   else if (p == fiducialSurfacePage) {
      fiducialSurfacePage->initializePage();
   }
   else if (p == sphericalSurfacePage) {
      sphericalSurfacePage->initializePage();
   }
   else if (p == templateCutsPage) {
      templateCutsPage->initializePage();
   }
   else if (p == borderDrawAndUpdatePage) {
      borderDrawAndUpdatePage->initializePage();
   }
   else if (p == surfaceAlignmentPage) {
      surfaceAlignmentPage->initializePage();
   }
   else if (p == startFlatteningPage) {
      startFlatteningPage->initializePage();
   }
   else if (p == initialFlatPage) {
      initialFlatPage->initializePage();
   }
   else if (p == multiMorphPage) {
      multiMorphPage->initializePage();
   }
   else if (p == finishedPage) {
      finishedPage->initializePage();
   }
   else if (p != NULL) {
      std::cout << "PROGRAM ERROR: unknown page in "
                   "GuiFlattenFullHemisphereDialog::updateDialog()" << std::endl;
   }
}
      
/**
 * save borders, if needed.
 */
void 
GuiFlattenFullHemisphereDialog::saveNewBorders()
{
   //
   // Clear modification status of surface borders that may occur
   // after unprojecting borders
   //   
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelBorderSet* bmbs = bs->getBorderSet();
   bmbs->setAllBordersModifiedStatus(false);

   if (getAutoSaveFilesSelected() == false) {
      return;
   }
   
   
   if (bmbs->getProjectionsModified()) {
      try {
         QString projFileName;
         BrainModelBorderFileInfo* projInfo = bmbs->getBorderProjectionFileInfo();
         if (projInfo->getFileName().isEmpty()) {
            BorderProjectionFile bpf;
            projFileName = bpf.makeDefaultFileName("TemplateBorders");
         }
         else {
            projFileName = projInfo->getFileName();
         }
         bs->writeBorderProjectionFile(projFileName,
                                       "",
                                       "");
      }
      catch (FileException& e) {
         WuQMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }
   
   if (bs->getBorderColorFile()->getModified()) {
      try {
         bs->writeBorderColorFile(bs->getBorderColorFile()->getFileName());
      }
      catch (FileException& e) {
         WuQMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }
   
   if (bs->getAreaColorFile()->getModified()) {
      try {
         bs->writeAreaColorFile(bs->getAreaColorFile()->getFileName());
      }
      catch (FileException& e) {
         WuQMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }
   
   if (bs->getPaintFile()->getModified()) {
      try {
         bs->writePaintFile(bs->getPaintFile()->getFileName());
      }
      catch (FileException& e) {
         WuQMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }

   if (bs->getVocabularyFile()->getModified()) {
      try {
         bs->writeVocabularyFile(bs->getVocabularyFile()->getFileName());
      }
      catch (FileException& e) {
         WuQMessageBox::critical(this, "ERROR", e.whatQString());
      }
   }
}

/**
 * save new surfaces.
 */
void 
GuiFlattenFullHemisphereDialog::saveSurfaces(const int indexOfFirstCoordinateFileToSave,
                                             const int indexOfFirstTopologyFileToSave) 
{
   if (getAutoSaveFilesSelected() == false) {
      return;
   }

   BrainSet* bs = theMainWindow->getBrainSet();
   
   if (indexOfFirstCoordinateFileToSave >= 0) {
      for (int i = indexOfFirstCoordinateFileToSave; 
           i < bs->getNumberOfBrainModels(); 
           i++) {
         BrainModelSurface* bms = bs->getBrainModelSurface(i);
         if (bms != NULL) {
            CoordinateFile* cf = bms->getCoordinateFile();
            if (cf->getModified()) {
               try {
                  bs->writeCoordinateFile(cf->getFileName(),
                                          bms->getSurfaceType(),
                                          cf);
               }
               catch (FileException& e) {
                  WuQMessageBox::critical(this, "ERROR", e.whatQString());
               }
            }
         }
      }
   }
   
   if (indexOfFirstTopologyFileToSave >= 0) {
      for (int i = indexOfFirstTopologyFileToSave; 
           i < bs->getNumberOfTopologyFiles(); 
           i++) {
         TopologyFile* tf = bs->getTopologyFile(i);
         if (tf != NULL) {
            if (tf->getModified()) {
               try {
                  bs->writeTopologyFile(tf->getFileName(),
                                        tf->getTopologyType(),
                                        tf);
               }
               catch (FileException& e) {
                  WuQMessageBox::critical(this, "ERROR", e.whatQString());
               }
            }
         }
      }
   }
}
                        
//=========================================================================
//=========================================================================
//=========================================================================

/**
 * constructor.
 */
GuiFlattenFullHemisphereIntroPage::GuiFlattenFullHemisphereIntroPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Introduction to Full Hemisphere Flattening");
   
   const QString instructionsText =
      "This flattening process requires a topologically correct spherical surface, "
      "a closed border that "
      "encloses the medial wall, and several open borders that indicate where "
      "cuts are to be made.  If the spherical surface or any of the borders are "
      "not present at this time, subsequent pages will allow you to generate "
      "these missing items.";
   QLabel* instructionsLabel = new QLabel;
   instructionsLabel->setText(instructionsText);
   instructionsLabel->setTextFormat(Qt::RichText);
   instructionsLabel->setWordWrap(true);
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(instructionsLabel);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereIntroPage::~GuiFlattenFullHemisphereIntroPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereIntroPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereIntroPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereIntroPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereIntroPage::validatePage()
{
   return true;
}

//=========================================================================
//=========================================================================
//=========================================================================

/**
 * constructor.
 */
GuiFlattenFullHemisphereSettingsPage::GuiFlattenFullHemisphereSettingsPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Flattening Options");
   
   const QString settingsText =
      "New data files will be generated during this process.  If you would like "
      "these new data files saved, select the <B>Auto Save Files</B> option below.<P><P>"
      "";
   QLabel* settingsLabel = new QLabel;
   settingsLabel->setText(settingsText);
   settingsLabel->setTextFormat(Qt::RichText);
   settingsLabel->setWordWrap(true);

   autoSaveFilesCheckBox = new QCheckBox("Auto Save Files");
   autoSaveFilesCheckBox->setChecked(true);
   
   smoothMedialWallCheckBox = new QCheckBox("Create Fiducial Surface with Smoothed Medial Wall");
   smoothMedialWallCheckBox->setChecked(true);
   
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(settingsLabel);
   layout->addWidget(autoSaveFilesCheckBox);
   layout->addWidget(smoothMedialWallCheckBox);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereSettingsPage::~GuiFlattenFullHemisphereSettingsPage()
{
}

/**
 * get the auto save files checkbox status.
 */
bool 
GuiFlattenFullHemisphereSettingsPage::getAutoSaveFilesSelected() const
{
   return autoSaveFilesCheckBox->isChecked();
}

/**
 * get the create fiducial with smoothed medial wall checkbox status.
 */
bool 
GuiFlattenFullHemisphereSettingsPage::getCreateFiducialWithSmoothedMedialWallSelected() const
{
   return smoothMedialWallCheckBox->isChecked();
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereSettingsPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereSettingsPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereSettingsPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereSettingsPage::validatePage()
{
   return true;
}

//=========================================================================
//=========================================================================
//=========================================================================

/**
 * constructor.
 */
GuiFlattenFullHemisphereFiducialPage::GuiFlattenFullHemisphereFiducialPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Fiducial Surface");
   
   const QString topologyText =
      "If the Fiducial Surface is not topologically correct, it will cause "
      "problems, possibly severe, during the flattening process.  The "
      "topology will be checked when the <B>Next</B> button is pressed.  If "
      "there are topolgical problems, you will have the option to cancel and "
      "correct the problems.<P>"
      "A surface that is topologically correct contains no holes (also "
      "referred to as handles or topological defects) and one connected "
      "piece of surface.  The best way to correct a topologically "
      "incorrect surface is to correct the errors in the segmentation volume "
      "and regenerate the surface.  If the segmentation volume is not "
      "available or the errors are small, <B>Surface Menu->Topology->Correct "
      " Fiducial Surface Topology</B> should be able to correct the topological "
      "errors.  If there are multiple unconnected pieces of surface (islands), "
      "<B>Surface Menu->Topology->Remove Islands</B> can remove the islands.";
   QLabel* topologyLabel = new QLabel;
   topologyLabel->setText(topologyText);
   topologyLabel->setTextFormat(Qt::RichText);
   topologyLabel->setWordWrap(true);
   
   //
   // Fiducial Surface selection
   //
   QLabel* fiducialSurfaceLabel = new QLabel("Fiducial");
   fiducialSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
                                  BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   QHBoxLayout* surfaceLayout = new QHBoxLayout;
   surfaceLayout->addWidget(fiducialSurfaceLabel);
   surfaceLayout->addWidget(fiducialSurfaceComboBox);
   surfaceLayout->addStretch();
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(topologyLabel);
   layout->addLayout(surfaceLayout);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereFiducialPage::~GuiFlattenFullHemisphereFiducialPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereFiducialPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereFiducialPage::initializePage()
{
   fiducialSurfaceComboBox->updateComboBox();
}

/**
 * get the fiducial surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereFiducialPage::getFiducialSurface()
{
   return fiducialSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereFiducialPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereFiducialPage::validatePage()
{
   return true;
}

//=========================================================================
//=========================================================================
//=========================================================================
/**
 * constructor.
 */
GuiFlattenFullHemisphereTemplateCutsPage::GuiFlattenFullHemisphereTemplateCutsPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Flattening Borders Generation");
   
   QPushButton* addTemplateCutBordersPushButton = 
       new QPushButton("Add Template Border Medial Wall and Cuts...");
   addTemplateCutBordersPushButton->setFixedSize(addTemplateCutBordersPushButton->sizeHint());
   addTemplateCutBordersPushButton->setAutoDefault(false);
   QObject::connect(addTemplateCutBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddTemplateCutBordersPushButton()));
                    
   QPushButton* addLandmarkBordersPushButton = 
       new QPushButton("Add Landmark Border Medial Wall and Cuts...");
   addLandmarkBordersPushButton->setFixedSize(addLandmarkBordersPushButton->sizeHint());
   addLandmarkBordersPushButton->setAutoDefault(false);
   QObject::connect(addLandmarkBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddLandmarkBordersPushButton()));
                    
   const QString text = 
      "Borders are needed that identify the medial wall and where cuts should "
      "be made.  Pressing the \"Add Landmark Border Medial Wall and Cuts\" "
      "pushbutton will display controls for generating landmark borders.  The "
      "landmark borders process examines the surfaces to determine the locations "
      "of the medial wall and cuts.  Pressing the \"Add Template Border Medial "
      "Wall and Cuts\" pushbutton will create borders that roughly identify "
      "the location of the medial wall and cuts.  \n\n"
      ""
      "Subsequent pages provide controls for drawing replacement borders and "
      "updating existing borders for the medial wall and cuts.";
   QLabel* textLabel = new QLabel(text);
   textLabel->setWordWrap(true);
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(addLandmarkBordersPushButton);
   layout->addWidget(addTemplateCutBordersPushButton);
   layout->addWidget(textLabel);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereTemplateCutsPage::~GuiFlattenFullHemisphereTemplateCutsPage()
{
}

/**
 * called to add landmark border cuts.
 */
void 
GuiFlattenFullHemisphereTemplateCutsPage::slotAddLandmarkBordersPushButton()
{
   GuiFlattenFullHemisphereDialog* fd = 
      dynamic_cast<GuiFlattenFullHemisphereDialog*>(wizard());
   GuiBorderOperationsDialog* borderOperationsDialog =
      theMainWindow->getBorderOperationsDialog(true);
   borderOperationsDialog->showPageCreateLandmarkBorders(true, 
                                                         true,
                                                         fd->getFiducialSurface(),
                                                         fd->getInflatedSurface(),
                                                         fd->getVeryInflatedSurface(),
                                                         fd->getSphericalSurface());
}
      
/**
 * called to add border template cuts.
 */
void 
GuiFlattenFullHemisphereTemplateCutsPage::slotAddTemplateCutBordersPushButton()
{
   BrainSet* bs = theMainWindow->getBrainSet();

   //
   // Get directory containing template borders
   //
   const QString templateDirectoryName =
      (BrainSet::getCaretHomeDirectory()
       + QDir::separator()
       + "data_files/flatten_landmarks");
   QDir templateDirectory(templateDirectoryName);
   if (templateDirectory.exists() == false) {
      WuQMessageBox::critical(this,
                              "ERROR",
                              "Unable to find template border files.  is Caret "
                              "installed correctly?\n"
                              "Looking in "
                              + templateDirectoryName);
      return;
   }
   
   //
   // Get all border files in directory
   //
   QFileInfoList infoList = 
      templateDirectory.entryInfoList(QStringList("*.border"),
                          (QDir::Files | QDir::Readable | QDir::CaseSensitive));
   QListIterator<QFileInfo> iter(infoList);
   QStringList borderFileNames;
   QStringList borderColorFileNames;
   QStringList borderFileDescriptions;
   while (iter.hasNext()) {
      QString borderFileName = iter.next().absoluteFilePath();
      
      BorderFile borderFile;
      try {
         borderFile.readFile(borderFileName);
         const QString description = borderFile.getHeaderTag("descriptive_name");
         if (description.isEmpty() == false) {
            borderFileNames        += borderFileName;
            borderFileDescriptions += description;
            borderColorFileNames   += (BrainSet::getCaretHomeDirectory()
                                       + QDir::separator()
                                       + "data_files/flatten_landmarks"
                                       + QDir::separator()
                                       + borderFile.getHeaderTag("matching_color_file"));
         }
      }
      catch (FileException&) {
      }
   }
   borderFileDescriptions.sort();
   
   const QString speciesString = bs->getSpecies().getName().toLower();
   const QString structureString = bs->getStructure().getTypeAsString().toLower();
   int defaultIndex = -1;
   for (int i = 0; i < borderFileDescriptions.count(); i++) {
      const QString descrip(borderFileDescriptions.at(i).toLower());
      if ((descrip.indexOf("standard") >= 0) &&
          (descrip.indexOf(speciesString) >= 0) &&
          (descrip.indexOf(structureString) >= 0)) {
         defaultIndex = i;
         break;
      }
   }
   
   if (borderFileNames.isEmpty()) {
      WuQMessageBox::critical(this,
                              "ERROR",
                              "Unable to find template border files.  is Caret "
                              "installed correctly?\n"
                              "Looking in "
                              + templateDirectoryName);
      return;
   }
   
   //
   // Allow user to choose borders
   //
   WuQDataEntryDialog ded(this);
   ded.setWindowTitle("Choose Template Borders");
   QListWidget* lw = ded.addListWidget("", borderFileDescriptions);
   if (defaultIndex >= 0) {
      lw->setCurrentRow(defaultIndex);
   }
   if (ded.exec() == WuQDataEntryDialog::Accepted) {
      const QString description = lw->currentItem()->text();
      
      QString templateBorderFileName;
      QString templateColorFileName;
      for (int i = 0; i < borderFileDescriptions.count(); i++) {
         if (description == borderFileDescriptions.at(i)) {
            templateBorderFileName = borderFileNames.at(i);
            templateColorFileName  = borderColorFileNames.at(i);
            break;
         }
      }
      
      //
      // Load template borders
      //
      if (templateBorderFileName.isEmpty() == false) {
         BrainModelBorderSet* bmbs = bs->getBorderSet();
         try {
            bs->readBorderFile(templateBorderFileName,
                               BrainModelSurface::SURFACE_TYPE_SPHERICAL,
                               true,
                               false);
         }
         catch (FileException& e) {
            WuQMessageBox::critical(this, "ERROR", e.whatQString());
            return;
         }
         try {
            bs->readBorderColorFile(templateColorFileName,
                                    true,
                                    false);
            bs->getBorderColorFile()->setModified();
         }
         catch (FileException&) {
         }
         
         GuiFlattenFullHemisphereDialog* flattenDialog = 
            dynamic_cast<GuiFlattenFullHemisphereDialog*>(wizard());
         BrainModelSurface* sphericalSurface =
            flattenDialog->getSphericalSurface();
         if (sphericalSurface != NULL) {
            bmbs->projectBorders(sphericalSurface, true);
         }
         
         DisplaySettingsBorders* dsb = bs->getDisplaySettingsBorders();
         dsb->setDisplayBorders(true);
           
         GuiFilesModified fm;
         fm.setBorderModified();
         fm.setBorderColorModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
      }
   }
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereTemplateCutsPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereTemplateCutsPage::initializePage()
{
   
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   dsb->setDisplayBorders(true);
   dsb->determineDisplayedBorders();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereTemplateCutsPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereTemplateCutsPage::validatePage()
{
   return true;
}
      
//=========================================================================
//=========================================================================
//=========================================================================

/**
 * constructor.
 */
GuiFlattenFullHemisphereSphericalPage::GuiFlattenFullHemisphereSphericalPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Spherical Surface");
   
   //
   // Inflated Surface selection
   //
   QLabel* inflatedSurfaceLabel = new QLabel("Inflated");
   inflatedSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
                                  BrainModelSurface::SURFACE_TYPE_INFLATED);

   //
   // Very Inflated Surface selection
   //
   QLabel* veryInflatedSurfaceLabel = new QLabel("Very Inflated");
   veryInflatedSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
                                  BrainModelSurface::SURFACE_TYPE_VERY_INFLATED);

   //
   // Spherical Surface selection
   //
   QLabel* sphericalSurfaceLabel = new QLabel("Spherical");
   sphericalSurfaceComboBox = new GuiBrainModelSurfaceSelectionComboBox(
                                  BrainModelSurface::SURFACE_TYPE_SPHERICAL);

   //
   // Layout for surface selection
   //
   QGridLayout* surfaceGridLayout = new QGridLayout;
   surfaceGridLayout->addWidget(sphericalSurfaceLabel, 0, 0);
   surfaceGridLayout->addWidget(sphericalSurfaceComboBox, 0, 1);
   surfaceGridLayout->addWidget(inflatedSurfaceLabel, 1, 0);
   surfaceGridLayout->addWidget(inflatedSurfaceComboBox, 1, 1);
   surfaceGridLayout->addWidget(veryInflatedSurfaceLabel, 2, 0);
   surfaceGridLayout->addWidget(veryInflatedSurfaceComboBox, 2, 1);
   surfaceGridLayout->setColumnStretch(0, 0);
   surfaceGridLayout->setColumnStretch(1, 100);

   const QString instructionsText =
      "A spherical surface is required.  Inflated and very inflated surfaces "
      "are very helpful for drawing borders.<P>"
      ""
      "If you need to generate surfaces, click the <B>Generate Surfaces</B> "
      "button below.";
   QLabel* instructionsLabel = new QLabel;
   instructionsLabel->setText(instructionsText);
   instructionsLabel->setTextFormat(Qt::RichText);
   instructionsLabel->setWordWrap(true);

   //
   // Button for generating surfaces
   //
   QPushButton* generateSurfacesPushButton = new QPushButton("Generate Surfaces...");
   generateSurfacesPushButton->setAutoDefault(false);
   generateSurfacesPushButton->setFixedSize(generateSurfacesPushButton->sizeHint());
   QObject::connect(generateSurfacesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotGenerateSurfacesPushButton()));
                    
   //
   // Layout for page
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addLayout(surfaceGridLayout);
   layout->addWidget(instructionsLabel);
   layout->addWidget(generateSurfacesPushButton);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereSphericalPage::~GuiFlattenFullHemisphereSphericalPage()
{
}

/**
 * get the spherical surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereSphericalPage::getSphericalSurface()
{
   return sphericalSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * get the inflated surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereSphericalPage::getInflatedSurface()
{
   return inflatedSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * get the very inflated surface.
 */
BrainModelSurface* 
GuiFlattenFullHemisphereSphericalPage::getVeryInflatedSurface()
{
   return veryInflatedSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * called when generate surfaces button clicked.
 */
void 
GuiFlattenFullHemisphereSphericalPage::slotGenerateSurfacesPushButton()
{
   const int oldNumberOfBrainModels = theMainWindow->getBrainSet()->getNumberOfBrainModels();
   
   BrainModelSurface* fiducialSurface = 
      dynamic_cast<GuiFlattenFullHemisphereDialog*>(wizard())->getFiducialSurface();
   if (fiducialSurface == NULL) {
      WuQMessageBox::critical(this, "ERROR", "There must be a fiducial surface.");
      return;
   }
   
   GuiMainWindowSurfaceActions* surfaceActions = 
      theMainWindow->getSurfaceActions();
   
   surfaceActions->generateInflatedAndOtherSurfaces(
      fiducialSurface,
      false,
      false,
      (inflatedSurfaceComboBox->getSelectedBrainModelSurface() == NULL),
      (veryInflatedSurfaceComboBox->getSelectedBrainModelSurface() == NULL),
      (sphericalSurfaceComboBox->getSelectedBrainModelSurface() == NULL));
      
   GuiFlattenFullHemisphereDialog* dialog = 
      dynamic_cast<GuiFlattenFullHemisphereDialog*>(wizard());
   dialog->saveSurfaces(oldNumberOfBrainModels);
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereSphericalPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereSphericalPage::initializePage()
{
   inflatedSurfaceComboBox->updateComboBox();
   veryInflatedSurfaceComboBox->updateComboBox();
   sphericalSurfaceComboBox->updateComboBox();
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereSphericalPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereSphericalPage::validatePage()
{
   return true;
}
      
//=========================================================================
//=========================================================================
//=========================================================================

/**
 * constructor.
 */
GuiFlattenFullHemisphereBorderUpdatePage::GuiFlattenFullHemisphereBorderUpdatePage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Draw and Update Borders");
   
   QPushButton* drawBordersPushButton = new QPushButton("Draw Borders...");
   drawBordersPushButton->setToolTip("Press this button to\n"
                                     "draw a new border.");
   drawBordersPushButton->setAutoDefault(false);
   drawBordersPushButton->setFixedSize(drawBordersPushButton->sizeHint());
   QObject::connect(drawBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotDrawBordersPushButton()));
                    
   QPushButton* updateBordersPushButton = new QPushButton("Update Borders...");
   updateBordersPushButton->setToolTip("Press this button to\n"
                                     "modify one of the\n"
                                     "existing borders.");
   updateBordersPushButton->setAutoDefault(false);
   updateBordersPushButton->setFixedSize(updateBordersPushButton->sizeHint());
   QObject::connect(updateBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotUpdateBordersPushButton()));
                    
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(drawBordersPushButton);
   buttonsLayout->addWidget(updateBordersPushButton);
   buttonsLayout->addStretch();
   
   QPushButton* exampleImagesPushButton = new QPushButton("Images of Medial Wall and Cuts...");
   exampleImagesPushButton->setAutoDefault(false);
   exampleImagesPushButton->setToolTip("View examples of Medial Wall and Cut Borders");
   exampleImagesPushButton->setFixedSize(exampleImagesPushButton->sizeHint());
   QObject::connect(exampleImagesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotBorderExampleImages()));
                    
   QPushButton* showOnlyFlattenBordersPushButton = new QPushButton("Show Only Flattening Borders");
   showOnlyFlattenBordersPushButton->setAutoDefault(false);
   showOnlyFlattenBordersPushButton->setFixedSize(showOnlyFlattenBordersPushButton->sizeHint());
   QObject::connect(showOnlyFlattenBordersPushButton, SIGNAL(clicked()),
                    this, SLOT(slotShowOnlyFlattenBordersPushButton()));
                    
   const QString infoText = 
      "For proper flattening, the medial wall must to be accurately identified "
      "so that no non-cortical material remains in the flattened surface.  "
      "The accuracy required of the cuts is less than that of the medial wall and "
      "if the cuts were automatically generated, their locations are sufficiently "
      "correct.  However, cuts MUST intersect the Medial Wall.<P><P>  "
      "The Medial Wall border must be named <I>FLATTEN.HOLE.MedialWall</I>.  All "
      "of the cuts border names must begin with <I>FLATTEN.CUT</I>.  Borders "
      "whose name does not begin with \"FLATTEN\" are ignored by the flattening "
      "process.<P><P>"
      "The 3D border drawing mode enabled the drawing of borders on any surface "
      "and the drawn borders is automatically projected.<P><P>."
      "The Medial Wall border is drawn as an <I>closed</I> border and the cuts are "
      "drawn as <I>open</I> borders.<P><P>"
      "It may be helpful to display the surface shape column named "
      "<I>Folding (Mean Curvature</I> to aid in border drawing.<P><P>"
      "<B>FLATTEN.HOLE.MedialWall</B> This closed border is drawn so that it "
      "encloses the Medial Wall.  The dorsal medial wall landmark runs along the "
      "corpus callosum, from a starting point just posterior to the olfactory "
      "sulcus to a termination just anterior to the calcarine sulcus. The "
      "ventral medial wall landmark runs from the same termination points, "
      "but in the opposite direction and along the medial margin of the "
      "hippocampal sulcus (15 mm medial to the parahippocampal gyrus at its "
      "maximum), and across the margins of cortex with the basal forebrain.<P><P>"
      "<B>FLATTEN.CUT.Std.Calcarine</B> This cut starts inside the medial wall, "
      "is drawn along the calcarine sulcus to the occipital pole, and then "
      "continues in a lateral direction for several millimeters.<P><P>"
      "<B>FLATTEN.CUT.Std.Cingulate</B> This cut starts inside the medial wall "
      "and is drawn to the dorsal/medial tip of the central sulcus.<P><P>"
      "<B>FLATTEN.CUT.Std.Frontal</B> This cut starts inside the medial wall, "
      "is drawn in an anterior direction along the inferior frontal cortex, and "
      "concludes at the most posterior location in the Inferior Frontal Sulcus. "
      "<P><P>"
      "<B>FLATTEN.CUT.Std.Sylvian</B> This cut starts inside the medial wall "
      "and is drawn so that it ends approximately one-third of the way into "
      "the Sylvian Fissure.<P><P>"
      "<B>FLATTEN.CUT.Std.Temporal</B> This cut starts inside the medial wall, "
      "is drawn in an inferior direction so that it passes through the most "
      "inferior node in the temporal lobe, and then terminates about 15 to 20 "
      "millimeters below the Superior Temporal Sulcus.";
   QTextEdit* borderInfoTextEdit = new QTextEdit;
   borderInfoTextEdit->setHtml(infoText);
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addLayout(buttonsLayout);
   layout->addWidget(exampleImagesPushButton);
   layout->addWidget(showOnlyFlattenBordersPushButton);
   layout->addWidget(borderInfoTextEdit);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereBorderUpdatePage::~GuiFlattenFullHemisphereBorderUpdatePage()
{
}

/**
 * called when draw borders button pressed.
 */
void 
GuiFlattenFullHemisphereBorderUpdatePage::slotDrawBordersPushButton()
{
   GuiBorderOperationsDialog* borderOperationsDialog =
      theMainWindow->getBorderOperationsDialog(true);
   borderOperationsDialog->showPageDrawBorders();
}

/**
 * called when update borders button pressed.
 */
void 
GuiFlattenFullHemisphereBorderUpdatePage::slotUpdateBordersPushButton()
{
   GuiBorderOperationsDialog* borderOperationsDialog =
      theMainWindow->getBorderOperationsDialog(true);
   borderOperationsDialog->showPageUpdateBorders();
}
      
/**
 * called when show only flattening borders selected.
 */
void 
GuiFlattenFullHemisphereBorderUpdatePage::slotShowOnlyFlattenBordersPushButton()
{
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   const int num = bmbs->getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      BrainModelBorder* b = bmbs->getBorder(i);
      if (b->getName().startsWith("FLATTEN")) {
         b->setNameDisplayFlag(true);
      }
      else {
         b->setNameDisplayFlag(false);
      }
   }
   
   DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
   dsb->determineDisplayedBorders();
   
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * called when example images button pressed.
 */
void 
GuiFlattenFullHemisphereBorderUpdatePage::slotBorderExampleImages()
{
   
   static WuQDialog* d = NULL;
   if (d == NULL) {
      //
      // Get directory containing borders images
      //
      const QString templateDirectoryName =
         (BrainSet::getCaretHomeDirectory()
          + QDir::separator()
          + "data_files/flatten_landmarks/example_images");
      QDir templateDirectory(templateDirectoryName);
      if (templateDirectory.exists() == false) {
         WuQMessageBox::critical(this,
                                 "ERROR",
                                 "Unable to find example border images.  is Caret "
                                 "installed correctly?\n"
                                 "Looking in "
                                 + templateDirectoryName);
         return;
      }
      
      //
      // Get all border files in directory
      //
      QFileInfoList infoList = 
         templateDirectory.entryInfoList(QStringList("*.jpg"),
                             (QDir::Files | QDir::Readable | QDir::CaseSensitive),
                             QDir::Name);
      if (infoList.count() <= 0) {
         WuQMessageBox::critical(this,
                                 "ERROR",
                                 "Unable to find example border images.  \n"
                                 "No images found in "
                                 + templateDirectoryName);
         return;
      }
      QListIterator<QFileInfo> iter(infoList);

      QWidget* imagesWidget = new QWidget;
      QVBoxLayout* imagesLayout = new QVBoxLayout(imagesWidget);
      while (iter.hasNext()) {
         const QString imageFileName(iter.next().absoluteFilePath());
         QPixmap p;
         if (p.load(imageFileName)) {
            QLabel* imageLabel = new QLabel("");
            imageLabel->setPixmap(p);
            imagesLayout->addWidget(imageLabel);
         }
      }
      QScrollArea* scrollArea = new QScrollArea;
      scrollArea->setWidget(imagesWidget);
      
      d = new WuQDialog(this);
      d->setWindowTitle("Example Images of Medial Wall and Cuts");
      QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close);
      QObject::connect(bb, SIGNAL(rejected()),
                       d, SLOT(close()));      
      QVBoxLayout* layout = new QVBoxLayout(d);
      layout->addWidget(scrollArea);
      layout->addWidget(bb);
   }
   d->show();
   
}
      
/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereBorderUpdatePage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereBorderUpdatePage::initializePage()
{
   if (theMainWindow->getBrainModelOpenGL()->getMouseMode() ==
       GuiBrainModelOpenGL::MOUSE_MODE_ALIGN_STANDARD_ORIENTATION_FULL_HEM_FLATTEN) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
         GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereBorderUpdatePage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereBorderUpdatePage::validatePage()
{
   return true;
}
      
//=========================================================================
//=========================================================================
//=========================================================================
/**
 * constructor.
 */
GuiFlattenFullHemisphereStartFlatteningPage::GuiFlattenFullHemisphereStartFlatteningPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Start Flattening");
   
   bordersListLabel = new QLabel("");
   bordersListLabel->setTextFormat(Qt::RichText);
   bordersListLabel->setWordWrap(true);
   
   QLabel* startLabel = new QLabel("Pressing the <B>Next</B> button will start the "
                                   "flattening process.");
   startLabel->setTextFormat(Qt::RichText);
   startLabel->setWordWrap(true);
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(bordersListLabel);
   layout->addWidget(startLabel);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereStartFlatteningPage::~GuiFlattenFullHemisphereStartFlatteningPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereStartFlatteningPage::cleanupPage()
{
}

/**
 * get loaded flattening borders.
 */
void 
GuiFlattenFullHemisphereStartFlatteningPage::getLoadedFlatteningBorders(std::vector<QString>& flattenBorderNamesOut) const
{
   flattenBorderNamesOut.clear();

   std::vector<QString> sortedNames;
   const BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   for (int i = 0; i < bmbs->getNumberOfBorders(); i++) {
      const BrainModelBorder* border = bmbs->getBorder(i);
      if (border->getType() == BrainModelBorder::BORDER_TYPE_PROJECTION) {
         const QString name = border->getName();
         if (name.startsWith(BrainModelSurfaceBorderLandmarkIdentification::getFlattenStandardCutsBorderNamePrefix()) ||
             name.startsWith(BrainModelSurfaceBorderLandmarkIdentification::getFlattenMedialWallBorderName())) {
            flattenBorderNamesOut.push_back(border->getName());
         }
      }
   }
   std::sort(flattenBorderNamesOut.begin(), flattenBorderNamesOut.end());
}
      
/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereStartFlatteningPage::initializePage()
{
   if (theMainWindow->getBrainModelOpenGL()->getMouseMode() ==
       GuiBrainModelOpenGL::MOUSE_MODE_ALIGN_STANDARD_ORIENTATION_FULL_HEM_FLATTEN) {
      theMainWindow->getBrainModelOpenGL()->setMouseMode(
         GuiBrainModelOpenGL::MOUSE_MODE_VIEW);
   }

   std::vector<QString> flattenBorderNames;
   getLoadedFlatteningBorders(flattenBorderNames);
   
   QString msg =
      "The following borders will be used to remove the medial wall and to "
      "make cuts.  If the name of a border is listed more than once, the "
      "duplicate borders should be deleted prior to continuing.<BR>";
   for (std::vector<QString>::iterator iter = flattenBorderNames.begin();
        iter != flattenBorderNames.end();
        iter++) {
      msg += ("&nbsp;&nbsp;&nbsp;"
              + *iter
              + "<BR>");
   }
   
   bordersListLabel->setText(msg);
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereStartFlatteningPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereStartFlatteningPage::validatePage()
{
   return true;
}
      
//=========================================================================
//=========================================================================
//=========================================================================
/**
 * constructor.
 */
GuiFlattenFullHemisphereInitialFlatPage::GuiFlattenFullHemisphereInitialFlatPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Initial Flat");

   const QString initialFlatText = 
      "The initial flat surface is shown in the Main Window.  If there are "
      "any crossovers, they are shown in red.  If the crossovers are near "
      "the perimeter of the surface they can be removed by drawing and "
      "applying cuts.  When drawing a cut, the cut should start outside "
      "the surface, go over the surface near the cut, and then end outside "
      "of the surface.<P>"
      ""
      "Cuts are drawn similar to borders.  Move the mouse to the start of the "
      "cut, hold down the left mouse button and move the mouse to the end of the "
      "cut, release the mouse button, hold down the shift key a click the mouse."
      "<P>"
      ""
      "To draw and apply cuts:<BR>"
      "&nbsp;&nbsp;&nbsp;Press the Draw Cuts Button.<BR>"
      "&nbsp;&nbsp;&nbsp;Draw one or more cuts as needed.<BR>"
      "&nbsp;&nbsp;&nbsp;Press the Apply Cuts Button.<BR>"
      "&nbsp;&nbsp;&nbsp;Delete All Cuts.<BR>";
      
   QLabel* initialFlatLabel = new QLabel("");
   initialFlatLabel->setTextFormat(Qt::RichText);
   initialFlatLabel->setWordWrap(true);
   initialFlatLabel->setText(initialFlatText);
   
   GuiMainWindowSurfaceActions* surfaceActions = theMainWindow->getSurfaceActions();
   
   //
   // Draw Cuts push button
   //
   QPushButton* drawCutsPushButton = new QPushButton("Draw Cuts");
   drawCutsPushButton->setAutoDefault(false);
   QObject::connect(drawCutsPushButton, SIGNAL(clicked()),
                    surfaceActions, SLOT(slotCutsDraw()));
                    
   //
   // Apply Cuts push button
   //
   QPushButton* applyCutsPushButton = new QPushButton("Apply Cuts");
   applyCutsPushButton->setAutoDefault(false);
   QObject::connect(applyCutsPushButton, SIGNAL(clicked()),
                    surfaceActions, SLOT(slotCutsApply()));
                    
   //
   // Delete All Cuts push button
   //
   QPushButton* deleteAllCutsPushButton = new QPushButton("Delete All Cuts");
   deleteAllCutsPushButton->setAutoDefault(false);
   QObject::connect(deleteAllCutsPushButton, SIGNAL(clicked()),
                    surfaceActions, SLOT(slotCutsDeleteAll()));
                    
   //
   // Delete Cuts with mouse
   //
   QPushButton* deleteCutsWithMousePushButton = new QPushButton("Delete Cuts With Mouse");
   deleteCutsWithMousePushButton->setAutoDefault(false);
   QObject::connect(deleteCutsWithMousePushButton, SIGNAL(clicked()),
                    surfaceActions, SLOT(slotCutsDeleteWithMouse()));
                    
   //
   // Make buttons same size
   //
   QtUtilities::makeButtonsSameSize(drawCutsPushButton,
                                    applyCutsPushButton,
                                    deleteAllCutsPushButton,
                                    deleteCutsWithMousePushButton);
                                     
   //
   // Layout for cuts push buttons
   //
   QGridLayout* cutsGridLayout = new QGridLayout;
   cutsGridLayout->addWidget(drawCutsPushButton, 0, 0);
   cutsGridLayout->addWidget(applyCutsPushButton, 1, 0);
   cutsGridLayout->addWidget(deleteAllCutsPushButton, 0, 1);
   cutsGridLayout->addWidget(deleteCutsWithMousePushButton, 1, 1);
   cutsGridLayout->setColumnStretch(0, 0);
   cutsGridLayout->setColumnStretch(1, 0);
                    
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(initialFlatLabel);
   layout->addLayout(cutsGridLayout);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereInitialFlatPage::~GuiFlattenFullHemisphereInitialFlatPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereInitialFlatPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereInitialFlatPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereInitialFlatPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereInitialFlatPage::validatePage()
{
   return true;
}
      
//=========================================================================
//=========================================================================
//=========================================================================
/**
 * constructor.
 */
GuiFlattenFullHemisphereMultiresolutionMorphingPage::GuiFlattenFullHemisphereMultiresolutionMorphingPage(
         BrainModelSurfaceMultiresolutionMorphing* flatParametersIn,
         BrainModelSurfaceMultiresolutionMorphing* sphericalParametersIn,
         QWidget* parent)
   : QWizardPage(parent)
{
   flatParameters      = flatParametersIn;
   sphericalParameters = sphericalParametersIn;
   
   setTitle("Multi-resolution Morphing");
   
   const QString morphtText = 
      "Multi-resolution morphing will reduces the distortion in the flat "
      "and spherical surfaces.";
      
   QLabel* morphInfoLabel = new QLabel("");
   morphInfoLabel->setTextFormat(Qt::RichText);
   morphInfoLabel->setWordWrap(true);
   morphInfoLabel->setText(morphtText);
   
   //
   // flat multi-res morph
   //
   flatMultiMorphCheckBox = new QCheckBox("Perform Flat Multi-resolution Morphing");
   QPushButton* flatParamsPushButton = new QPushButton("Edit Parameters...");
   flatParamsPushButton->setAutoDefault(false);
   QObject::connect(flatParamsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotFlatParamsPushButton()));

   //
   // flat multi-res morph
   //
   sphericalMultiMorphCheckBox = new QCheckBox("Perform Spherical Multi-resolution Morphing");
   QPushButton* sphericalParamsPushButton = new QPushButton("Edit Parameters...");
   sphericalParamsPushButton->setAutoDefault(false);
   QObject::connect(sphericalParamsPushButton, SIGNAL(clicked()),
                    this, SLOT(slotSphericalParamsPushButton()));

   //
   // Disable edit buttons when morphing not selected
   //
   QObject::connect(flatMultiMorphCheckBox, SIGNAL(toggled(bool)),
                    flatParamsPushButton, SLOT(setEnabled(bool)));
   QObject::connect(sphericalMultiMorphCheckBox, SIGNAL(toggled(bool)),
                    sphericalParamsPushButton, SLOT(setEnabled(bool)));
   flatMultiMorphCheckBox->setChecked(true);
   sphericalMultiMorphCheckBox->setChecked(true);
                    
   //
   // Layout for mrm 
   //
   QGridLayout* mrmGridLayout = new QGridLayout;
   mrmGridLayout->addWidget(flatMultiMorphCheckBox, 0, 0);
   mrmGridLayout->addWidget(flatParamsPushButton, 0, 1);
   mrmGridLayout->addWidget(sphericalMultiMorphCheckBox, 1, 0);
   mrmGridLayout->addWidget(sphericalParamsPushButton, 1, 1);
   mrmGridLayout->setColumnStretch(0, 0);
   mrmGridLayout->setColumnStretch(1, 0);
   
   //
   // Layout for page
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(morphInfoLabel);
   layout->addLayout(mrmGridLayout);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereMultiresolutionMorphingPage::~GuiFlattenFullHemisphereMultiresolutionMorphingPage()
{
}

/**
 * is flat multi-res morphing selected.
 */
bool 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::getDoFlatMultiResMorphing() const
{
   return flatMultiMorphCheckBox->isChecked();
}

/**
 * is spherical multi-res morphing selected.
 */
bool 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::getDoSphericalMultiResMorphing() const
{
   return sphericalMultiMorphCheckBox->isChecked();
}
      
/**
 * called to adjust flat multi-res morph parameters.
 */
void 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::slotFlatParamsPushButton()
{
   GuiMultiresolutionMorphingDialog mmd(this,
                                        flatParameters,
                                        true);
   mmd.exec();
}

/**
 * called to adjust spherical multi-res morph parameters.
 */
void 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::slotSphericalParamsPushButton()
{
   GuiMultiresolutionMorphingDialog mmd(this,
                                        sphericalParameters,
                                        true);
   mmd.exec();
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereMultiresolutionMorphingPage::validatePage()
{
   return true;
}
      
//=========================================================================
//=========================================================================
//=========================================================================
/**
 * constructor.
 */
GuiFlattenFullHemisphereSurfaceAlignmentPage::GuiFlattenFullHemisphereSurfaceAlignmentPage(QWidget* parent)
   : QWizardPage(parent)
{
   ventralTipNodeNumber = -1;
   dorsalMedialTipNodeNumber = -1;
   
   setTitle("Surface Alignment Presets");
   
   const QString alignmentText = 
      "At the conclusion of the flattening process, the final flat and spherical "
      "surfaces will be automatically aligned to <I>Standard Orientation</I>.  "
      "If there is a border named <I>"
      + BrainModelSurfaceBorderLandmarkIdentification::getCentralSulcusRegistrationLandmarkName()
      + "</I> (one of the "
      "registration landmarks), the <I>Use "
      + BrainModelSurfaceBorderLandmarkIdentification::getCentralSulcusRegistrationLandmarkName()
      + " For Alignment</I> button will be enabled and can be used to preset the "
      "ventral and dorsal-medial tips of the central sulcus.<P>"
      ""
      "To set the nodes used for standard surface alignment, display the very "
      "inflated (or other) surface in the Main Window.  Click the mouse over "
      "the ventral (inferior) tip of the central sulcus.  Next, hold down the "
      "shift key and click the mouse over the dorsal-medial (superior) tip "
      "of the central sulcus.  If necessary, press the <I>Enable Mouse</I> "
      "button so that mouse clicks update the central sulcus tips.";
      
   QLabel* alignmentLabel = new QLabel("");
   alignmentLabel->setTextFormat(Qt::RichText);
   alignmentLabel->setWordWrap(true);
   alignmentLabel->setText(alignmentText);
   //
   // Central Sulcus Tips column titles
   //
   QLabel* tipEndColumnLabel = new QLabel("Central Sulcus Tip");
   QLabel* nodeLabel = new QLabel("X");
   QLabel* ventralLabel = new QLabel("Ventral (click)");
   QLabel* dorsalMedialLabel = new QLabel("Dorsal-Medial (shift-click)");
   const QString spaces(10, ' ');
   ventralNodeLabel = new QLabel(spaces);
   dorsalMedialNodeLabel = new QLabel(spaces);
   
   //
   // Layout and group box for central sulcus
   //
   QGroupBox* cesGroupBox = new QGroupBox("Central Sulcus Tips");
   QGridLayout* cesGridLayout = new QGridLayout(cesGroupBox);
   cesGridLayout->addWidget(tipEndColumnLabel, 0, 0);
   cesGridLayout->addWidget(nodeLabel, 0, 1);
   cesGridLayout->addWidget(ventralLabel, 1, 0);
   cesGridLayout->addWidget(ventralNodeLabel, 1, 1);
   cesGridLayout->addWidget(dorsalMedialLabel, 2, 0);
   cesGridLayout->addWidget(dorsalMedialNodeLabel, 2, 1);

   //
   // Use Central Sulcus to set tips
   //
   useLandmarkCentralSulcusPushButton   = 
      new QPushButton("Use "      
                      + BrainModelSurfaceBorderLandmarkIdentification::getCentralSulcusRegistrationLandmarkName()
                      + " For Alignment");
   useLandmarkCentralSulcusPushButton->setAutoDefault(false);
   useLandmarkCentralSulcusPushButton->setFixedSize(useLandmarkCentralSulcusPushButton->sizeHint());
   QObject::connect(useLandmarkCentralSulcusPushButton, SIGNAL(clicked()),
                    this, SLOT(slotUseLandmarkCentralSulcusPushButton()));

   //
   // Enable mouse push button
   //   
   QPushButton* enableMousePushButton = new QPushButton("Enable Mouse");
   enableMousePushButton->setAutoDefault(false);
   enableMousePushButton->setFixedSize(enableMousePushButton->sizeHint());
   QObject::connect(enableMousePushButton, SIGNAL(clicked()),
                    this, SLOT(slotEnableMousePushButton()));
                    
   //
   // Layout for page
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(alignmentLabel);
   layout->addWidget(cesGroupBox);
   layout->addWidget(useLandmarkCentralSulcusPushButton);
   layout->addWidget(enableMousePushButton);
   layout->addStretch();
   
   updateCentralSulcusLabels();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereSurfaceAlignmentPage::~GuiFlattenFullHemisphereSurfaceAlignmentPage()
{
}

/**
 * called when enable mouse button pressed.
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::slotEnableMousePushButton()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
      GuiBrainModelOpenGL::MOUSE_MODE_ALIGN_STANDARD_ORIENTATION_FULL_HEM_FLATTEN);
}
      
/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::initializePage()
{
   BorderProjectionFile borderProjFile;
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->copyBordersToBorderProjectionFile(borderProjFile);
   
   const BorderProjection* bp = borderProjFile.getLastBorderProjectionByName(
                BrainModelSurfaceBorderLandmarkIdentification::getCentralSulcusRegistrationLandmarkName());
   useLandmarkCentralSulcusPushButton->setEnabled(bp != NULL);
   
   slotEnableMousePushButton();
   
   updateCentralSulcusLabels();
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereSurfaceAlignmentPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereSurfaceAlignmentPage::validatePage()
{
   return true;
}
      
/**
 * called when Use LANDMARK.CentralSulcus button pressed.
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::slotUseLandmarkCentralSulcusPushButton()
{
   BorderProjectionFile borderProjFile;
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->copyBordersToBorderProjectionFile(borderProjFile);
   
   const BorderProjection* bp = borderProjFile.getLastBorderProjectionByName(
                BrainModelSurfaceBorderLandmarkIdentification::getCentralSulcusRegistrationLandmarkName());
   if (bp == NULL) {
      WuQMessageBox::critical(this, "ERROR", "Unable to find border named: "
               + BrainModelSurfaceBorderLandmarkIdentification::getCentralSulcusRegistrationLandmarkName()
);
      return;
   }
   
   const BrainModelSurface* fiducialSurface = 
      dynamic_cast<GuiFlattenFullHemisphereDialog*>(wizard())->getFiducialSurface();
   if (fiducialSurface == NULL) {
      return;
   }
   const CoordinateFile* cf = fiducialSurface->getCoordinateFile();
   
   //
   // Find first and last links
   //
   const int numLinks = bp->getNumberOfLinks();
   if (numLinks >= 2) {
      const BorderProjectionLink* bpFirst = bp->getBorderProjectionLink(0);
      float firstXYZ[3];
      bpFirst->unprojectLink(cf, firstXYZ);
      int firstSection, firstVertices[3];
      float firstAreas[3], firstRadius;
      bpFirst->getData(firstSection, firstVertices, firstAreas, firstRadius);

      const BorderProjectionLink* bpLast  = bp->getBorderProjectionLink(numLinks - 1);
      float lastXYZ[3];
      bpLast->unprojectLink(cf, lastXYZ);
      int lastSection, lastVertices[3];
      float lastAreas[3], lastRadius;
      bpLast->getData(lastSection, lastVertices, lastAreas, lastRadius);
      
      //
      // Link with largest Z is dorsal-medial
      //
      if (lastXYZ[2] > firstXYZ[2]) {
         ventralTipNodeNumber = firstVertices[0];
         dorsalMedialTipNodeNumber = lastVertices[0];
         updateCentralSulcusLabels();
      }
   }
}
      
/**
 * update the ces node numbers.
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::updateCentralSulcusLabels()
{
   const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
   
   if (ventralTipNodeNumber >= numNodes) {
      ventralTipNodeNumber = -1;
   }
   if (dorsalMedialTipNodeNumber >= numNodes) {
      dorsalMedialTipNodeNumber = -1;
   }
   
   if (ventralTipNodeNumber >= 0) {
      ventralNodeLabel->setNum(ventralTipNodeNumber);
   }
   else {
      ventralNodeLabel->setText("Invalid");
   }
   
   if (dorsalMedialTipNodeNumber >= 0) {
      dorsalMedialNodeLabel->setNum(dorsalMedialTipNodeNumber);
   }
   else {
      dorsalMedialNodeLabel->setText("Invalid");
   }
}
      
/**
 * get the central suclus ventral and dorsal-medial tips.
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::getCentralSulcusTips(
                                        int& ventralTipNodeNumberOut,
                                        int& dorsalMedialTipNodeNumberOut) const
{
   ventralTipNodeNumberOut = ventralTipNodeNumber;
   dorsalMedialTipNodeNumberOut = dorsalMedialTipNodeNumber;
}

/**
 * set the central sulcus ventral tip.
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::setCentralSulcusVentralTip(const int nodeNum)
{
   ventralTipNodeNumber = nodeNum;
   updateCentralSulcusLabels();
}

/**
 * set the central sulcus dorsal-medial tip.
 */
void 
GuiFlattenFullHemisphereSurfaceAlignmentPage::setCentralSulcusDorsalMedialTip(const int nodeNum)
{
   dorsalMedialTipNodeNumber = nodeNum;
   updateCentralSulcusLabels();
}

//=========================================================================
//=========================================================================
//=========================================================================
/**
 * constructor.
 */
GuiFlattenFullHemisphereFinishedPage::GuiFlattenFullHemisphereFinishedPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Finished");
   
   const QString finishedText = 
      "Flattening has been completed.";
      
   QLabel* finishedLabel = new QLabel("");
   finishedLabel->setTextFormat(Qt::RichText);
   finishedLabel->setWordWrap(true);
   finishedLabel->setText(finishedText);
                    
   //
   // Layout for page
   //
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(finishedLabel);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiFlattenFullHemisphereFinishedPage::~GuiFlattenFullHemisphereFinishedPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiFlattenFullHemisphereFinishedPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiFlattenFullHemisphereFinishedPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiFlattenFullHemisphereFinishedPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiFlattenFullHemisphereFinishedPage::validatePage()
{
   return true;
}

