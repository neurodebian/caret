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

#include <QButtonGroup>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QTextEdit>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"
#include "CommaSeparatedValueFile.h"
#include "BrainSet.h"
#include "DisplaySettingsBorders.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "GuiBorderOperationsDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSurfaceSelectionComboBox.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowSurfaceActions.h"
#include "GuiMultiresolutionMorphingDialog.h"
#include "GuiStandardMeshDialog.h"
#include "StringTable.h"
#include "TopologyFile.h"
#include "WuQFileDialog.h"
#include "WuQMessageBox.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiStandardMeshDialog::GuiStandardMeshDialog(QWidget* parent)
   : QWizard(parent)
{
   selectedAtlas = NULL;
   
   setOption(QWizard::NoCancelButton, false);
   setWizardStyle(ModernStyle);
   
   setWindowTitle("Standard Mesh Processing");
   
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
                                         
   const QString atlasErrorMessage = loadAtlases();   

   //
   // The introductory page
   //
   introPage = new GuiStandardMeshIntroPage;
   addPage(introPage);
   
   //
   // The fiducial surface page
   //
   fiducialSurfacePage = new GuiStandardMeshFiducialSurfacePage;
   addPage(fiducialSurfacePage);
   
   //
   // The surfaces page
   //
   surfacesPage = new GuiStandardMeshSurfacesPage;
   addPage(surfacesPage);
   
   //
   // The atlas selection page
   //
   atlasSelectionPage = new GuiStandardMeshAtlasSelectionPage(this);
   addPage(atlasSelectionPage);
   
   //
   // the landmark border generation page
   //
   landmarkBorderGenerationPage = new GuiStandardMeshLandmarkGenerationPage;
   addPage(landmarkBorderGenerationPage);
   
   //
   // border draw and update page
   //
   borderDrawUpdatePage = new GuiStandardMeshBorderUpdatePage;
   addPage(borderDrawUpdatePage);
   
   //
   // border validation page
   //
   borderValidationPage = new GuiStandardMeshBorderValidationPage;
   addPage(borderValidationPage);
   
   //
   // output spec file page
   //
   outputSpecFilePage = new GuiStandardMeshOutputSpecFilePage;
   addPage(outputSpecFilePage);
   
   //
   // morph & deform parameters page
   //
   morphDeformParamsPage = new GuiStandardMeshMorphDeformParamsPage(
                                               flatParameters,
                                               sphericalParameters);
   addPage(morphDeformParamsPage);
   
   //
   // the finshed page
   //
   finishedPage = new GuiStandardMeshFinishedPage;
   addPage(finishedPage);

   if (atlasErrorMessage.isEmpty() == false) {
      WuQMessageBox::critical(this, "ERROR", atlasErrorMessage);
      return;
   }
}

/**
 * destructor.
 */
GuiStandardMeshDialog::~GuiStandardMeshDialog()
{
}

/**
 * called to validate the current page.
 */
bool 
GuiStandardMeshDialog::validateCurrentPage()
{
/*
   BrainSet* bs = theMainWindow->getBrainSet();
*/   
   const QWizardPage* p = currentPage();

   if (p == introPage) {
      if (atlases.empty()) {
         WuQMessageBox::critical(this, "ERROR", 
            "No atlases found.  Is Caret installed correctly?");
         return false;
      }
   }
/*   
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
*/   
   return true;
}
      
/**
 * get the fiducial surface.
 */
BrainModelSurface* 
GuiStandardMeshDialog::getFiducialSurface()
{
   return fiducialSurfacePage->getFiducialSurface();
}
      
/**
 * get the inflated surface.
 */
BrainModelSurface* 
GuiStandardMeshDialog::getInflatedSurface()
{
   return surfacesPage->getInflatedSurface();
}
      
/**
 * get the very inflated surface.
 */
BrainModelSurface* 
GuiStandardMeshDialog::getVeryInflatedSurface()
{
   return surfacesPage->getVeryInflatedSurface();
}
      
/**
 * get the spherical surface.
 */
BrainModelSurface* 
GuiStandardMeshDialog::getSphericalSurface()
{
   return surfacesPage->getSphericalSurface();
}
      
/**
 * show the first page of the dialog.
 */
void 
GuiStandardMeshDialog::showFirstPage()
{
   restart();
}
      
/**
 * update the dialog.
 */
void 
GuiStandardMeshDialog::updateDialog()
{
   const QWizardPage* p = currentPage();
   if (p == introPage) {
      introPage->initializePage();
   }
   else if (p == fiducialSurfacePage) {
      fiducialSurfacePage->initializePage();
   }
   else if (p == surfacesPage) {
      surfacesPage->initializePage();
   }
   else if (p == atlasSelectionPage) {
      atlasSelectionPage->initializePage();
   }
   else if (p == landmarkBorderGenerationPage) {
      landmarkBorderGenerationPage->initializePage();
   }
   else if (p == borderDrawUpdatePage) {
      borderDrawUpdatePage->initializePage();
   }
   else if (p == borderValidationPage) {
      borderValidationPage->initializePage();
   }
   else if (p == outputSpecFilePage) {
      outputSpecFilePage->initializePage();
   }
   else if (p == morphDeformParamsPage) {
      morphDeformParamsPage->initializePage();
   }
   else if (p == finishedPage) {
      finishedPage->initializePage();
   }
   else if (p != NULL) {
      std::cout << "PROGRAM ERROR: unknown page in "
                   "GuiStandardMeshDialog::updateDialog()" << std::endl;
   }
}

/**
 * load the atlases.
 */
QString 
GuiStandardMeshDialog::loadAtlases()
{
   QString atlasErrorMessage = "";
   atlases.clear();

   //
   // Name of atlas directory file
   //
   const QString atlasDirectoryName =
      (BrainSet::getCaretHomeDirectory()
       + "/data_files/standard_mesh_atlases/");
   const QString atlasDirectoryFileName =
      (atlasDirectoryName
       + "/atlases.csv");
   
   CommaSeparatedValueFile csvFile;
   try {
      csvFile.readFile(atlasDirectoryFileName);
      
      const QString atlasSectionName("atlases");
      const StringTable* st = csvFile.getDataSectionByName(atlasSectionName);
      if (st == NULL) {
         atlasErrorMessage += ("Unable to find section named \""
                              + atlasSectionName
                              + "\" in the file "
                              + atlasDirectoryFileName
                              + "\n");
      }
      else {      
         const int nameColumn = st->getColumnIndexFromName("Name");
         const int directoryColumn = st->getColumnIndexFromName("Directory");
         const int specFileColumn = st->getColumnIndexFromName("SpecFileName");
         const int numberOfNodesColumn = st->getColumnIndexFromName("NumberOfNodes");
         const int numRows = st->getNumberOfRows();

         if (nameColumn < 0) {
            atlasErrorMessage = "Unable to find \"Name\" column.\n";
         }
         if (directoryColumn < 0) {
            atlasErrorMessage = "Unable to find \"Directory\" column.\n";
         }
         if (specFileColumn < 0) {
            atlasErrorMessage = "Unable to find \"SpecFileName\" column.\n";
         }
         if (numberOfNodesColumn < 0) {
            atlasErrorMessage = "Unable to find \"NumberOfNodes\" column.\n";
         }
         if (atlasErrorMessage.isEmpty() == false) {
            atlasErrorMessage.insert(0, "Error in " + atlasDirectoryFileName);
         }
         else {
            for (int i = 0; i < numRows; i++) {
               const QString name = st->getElement(i, nameColumn);
               const QString subDirectory = st->getElement(i, directoryColumn);
               const QString specFileName = st->getElement(i, specFileColumn);
               const int numNodes = st->getElementAsInt(i, numberOfNodesColumn);
               
               const QString specFilePathName(
                  atlasDirectoryName
                  + "/"
                  + subDirectory
                  + "/"
                  + specFileName);
                  
               atlases.push_back(StandardMeshAtlas(name,
                                                   specFilePathName,
                                                   numNodes));
                                                   
               SpecFile sf;
               try {
                  sf.readFile(specFilePathName);
               }
               catch (FileException& e) {
                  atlasErrorMessage += "Unable to load atlas named \""
                                      + name
                                      + "\" located at "
                                      + specFilePathName
                                      + "\n";
               }
            }
         }
      }
   }
   catch (FileException& e) {
      atlasErrorMessage += e.whatQString();
   }
   
   if (atlasErrorMessage.isEmpty() == false) {
      atlasErrorMessage.insert(0, "Error loading atlases.\n\n");
   }
   return atlasErrorMessage;
}

/**
 * called when cancel button pressed.
 */
void 
GuiStandardMeshDialog::reject()
{
   if ((currentPage() != introPage) &&
       (currentPage() != finishedPage)) {
      if (WuQMessageBox::question(this,
                                  "ERROR",
                                  "Conversion to Standard Mesh is not complete.  Stop Conversion?",
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
 * save new surfaces.
 */
void 
GuiStandardMeshDialog::saveSurfaces(const int indexOfFirstCoordinateFileToSave,
                                    const int indexOfFirstTopologyFileToSave) 
{
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
                        
//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshIntroPage::GuiStandardMeshIntroPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Introduction");
   
   const QString instructionsText =
      "This process will convert this subject to an <I>Atlas Surface Space</I>.<P> "
      " "
      "Once in an <I>Atlas Surface Space</I>, data files can be exchanged with "
      "that atlas and all other subjects in the <I>Atlas Surface Space</I>.<P> "
      " "
      "This process requires a fiducial surface, a spherical surface, and borders "
      "that identify landmarks on the subject.  If the spherical surface or the "
      "borders are not present at this time, subsequent pages will allow you to "
      "generate these missing items. ";

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
GuiStandardMeshIntroPage::~GuiStandardMeshIntroPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshIntroPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshIntroPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshIntroPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshIntroPage::validatePage()
{
   return true;
}


//=========================================================================
//=========================================================================
//=========================================================================

/**
 * constructor.
 */
GuiStandardMeshFiducialSurfacePage::GuiStandardMeshFiducialSurfacePage(QWidget* parent)
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
GuiStandardMeshFiducialSurfacePage::~GuiStandardMeshFiducialSurfacePage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshFiducialSurfacePage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshFiducialSurfacePage::initializePage()
{
   fiducialSurfaceComboBox->updateComboBox();
}

/**
 * get the fiducial surface.
 */
BrainModelSurface* 
GuiStandardMeshFiducialSurfacePage::getFiducialSurface()
{
   return fiducialSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * page is complete.
 */
bool 
GuiStandardMeshFiducialSurfacePage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshFiducialSurfacePage::validatePage()
{
   return true;
}

//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshSurfacesPage::GuiStandardMeshSurfacesPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Surface Selection and Generation");
   
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
   surfaceGridLayout->addWidget(sphericalSurfaceLabel, 1, 0);
   surfaceGridLayout->addWidget(sphericalSurfaceComboBox, 1, 1);
   surfaceGridLayout->addWidget(inflatedSurfaceLabel, 2, 0);
   surfaceGridLayout->addWidget(inflatedSurfaceComboBox, 2, 1);
   surfaceGridLayout->addWidget(veryInflatedSurfaceLabel, 3, 0);
   surfaceGridLayout->addWidget(veryInflatedSurfaceComboBox, 3, 1);
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
GuiStandardMeshSurfacesPage::~GuiStandardMeshSurfacesPage()
{
}

/**
 * get the inflated surface.
 */
BrainModelSurface* 
GuiStandardMeshSurfacesPage::getInflatedSurface()
{
   return inflatedSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * get the very inflated surface.
 */
BrainModelSurface* 
GuiStandardMeshSurfacesPage::getVeryInflatedSurface()
{
   return veryInflatedSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * get the spherical surface.
 */
BrainModelSurface* 
GuiStandardMeshSurfacesPage::getSphericalSurface()
{
   return sphericalSurfaceComboBox->getSelectedBrainModelSurface();
}
      
/**
 * called when generate surfaces button clicked.
 */
void 
GuiStandardMeshSurfacesPage::slotGenerateSurfacesPushButton()
{
   const int oldNumberOfBrainModels = theMainWindow->getBrainSet()->getNumberOfBrainModels();
   
   BrainModelSurface* fiducialSurface = 
      dynamic_cast<GuiStandardMeshDialog*>(wizard())->getFiducialSurface();
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
      
   GuiStandardMeshDialog* dialog = 
      dynamic_cast<GuiStandardMeshDialog*>(wizard());
   dialog->saveSurfaces(oldNumberOfBrainModels);
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshSurfacesPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshSurfacesPage::initializePage()
{
   inflatedSurfaceComboBox->updateComboBox();
   veryInflatedSurfaceComboBox->updateComboBox();
   sphericalSurfaceComboBox->updateComboBox();
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshSurfacesPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshSurfacesPage::validatePage()
{
   return true;
}
      

//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshAtlasSelectionPage::GuiStandardMeshAtlasSelectionPage(GuiStandardMeshDialog* smd,
                                                                     QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Atlas Selection");
   
   const int numAtlases = smd->getNumberOfAtlases();
   QButtonGroup* buttGroup = new QButtonGroup(this);
   QObject::connect(buttGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotAtlasSelection(int)));
   QVBoxLayout* atlasLayout = new QVBoxLayout;
   for (int i = 0; i < numAtlases; i++) {
      const StandardMeshAtlas* sma = smd->getAtlas(i);
      QRadioButton* rb = new QRadioButton(sma->getName());
      buttGroup->addButton(rb, i);
      atlasLayout->addWidget(rb);
   }
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addLayout(atlasLayout);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiStandardMeshAtlasSelectionPage::~GuiStandardMeshAtlasSelectionPage()
{
}

/**
 * called when atlas button selected.
 */
void 
GuiStandardMeshAtlasSelectionPage::slotAtlasSelection(int id)
{
   GuiStandardMeshDialog* smd = dynamic_cast<GuiStandardMeshDialog*>(wizard());
   smd->setSelectedAtlas(id);
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshAtlasSelectionPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshAtlasSelectionPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshAtlasSelectionPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshAtlasSelectionPage::validatePage()
{
   return true;
}

//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshLandmarkGenerationPage::GuiStandardMeshLandmarkGenerationPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Registration and Flattening Borders Generation");
   
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
   layout->addWidget(textLabel);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiStandardMeshLandmarkGenerationPage::~GuiStandardMeshLandmarkGenerationPage()
{
}

/**
 * called to add landmark border cuts.
 */
void 
GuiStandardMeshLandmarkGenerationPage::slotAddLandmarkBordersPushButton()
{
   GuiStandardMeshDialog* fd = 
      dynamic_cast<GuiStandardMeshDialog*>(wizard());
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
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshLandmarkGenerationPage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshLandmarkGenerationPage::initializePage()
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
GuiStandardMeshLandmarkGenerationPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshLandmarkGenerationPage::validatePage()
{
   return true;
}
      

//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshBorderUpdatePage::GuiStandardMeshBorderUpdatePage(QWidget* parent)
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
GuiStandardMeshBorderUpdatePage::~GuiStandardMeshBorderUpdatePage()
{
}

/**
 * called when draw borders button pressed.
 */
void 
GuiStandardMeshBorderUpdatePage::slotDrawBordersPushButton()
{
   GuiBorderOperationsDialog* borderOperationsDialog =
      theMainWindow->getBorderOperationsDialog(true);
   borderOperationsDialog->showPageDrawBorders();
}

/**
 * called when update borders button pressed.
 */
void 
GuiStandardMeshBorderUpdatePage::slotUpdateBordersPushButton()
{
   GuiBorderOperationsDialog* borderOperationsDialog =
      theMainWindow->getBorderOperationsDialog(true);
   borderOperationsDialog->showPageUpdateBorders();
}
      
/**
 * called when show only flattening borders selected.
 */
void 
GuiStandardMeshBorderUpdatePage::slotShowOnlyFlattenBordersPushButton()
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
GuiStandardMeshBorderUpdatePage::slotBorderExampleImages()
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
GuiStandardMeshBorderUpdatePage::cleanupPage()
{
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshBorderUpdatePage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshBorderUpdatePage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshBorderUpdatePage::validatePage()
{
   return true;
}

//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshBorderValidationPage::GuiStandardMeshBorderValidationPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Border Validation");
   
   QLabel* label = new QLabel(" Page");
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(label);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiStandardMeshBorderValidationPage::~GuiStandardMeshBorderValidationPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshBorderValidationPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshBorderValidationPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshBorderValidationPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshBorderValidationPage::validatePage()
{
   return true;
}


//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshOutputSpecFilePage::GuiStandardMeshOutputSpecFilePage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Output Spec File");
   
   const QString instructionsText = 
      "Choose the directory in which the output spec file and its data files"
      "will be placed.  If the directory does not exist, it will be created.  "
      "Also, enter the name for the output spec file. "
      "";
   QLabel* instructionsLabel = new QLabel;
   instructionsLabel->setText(instructionsText);
   instructionsLabel->setTextFormat(Qt::RichText);
   instructionsLabel->setWordWrap(true);
   
   //
   // Directory button and line edit
   //
   QPushButton* directoryButton = new QPushButton("Directory...");
   directoryButton->setFixedSize(directoryButton->sizeHint());
   directoryButton->setAutoDefault(false);
   QObject::connect(directoryButton, SIGNAL(clicked()),
                    this, SLOT(slotDirectorySelection()));
   directoryLineEdit = new QLineEdit;
   
   //
   // Spec File name and label
   //
   QPushButton* specFileButton = new QPushButton("Spec File...");
   specFileButton->setFixedSize(specFileButton->sizeHint());
   specFileButton->setAutoDefault(false);
   QObject::connect(specFileButton, SIGNAL(clicked()),
                    this, SLOT(slotSpecFileSelection()));
   specFileLineEdit = new QLineEdit;

   //
   // Layout for directory and spec file name
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(directoryButton, 0, 0);
   gridLayout->addWidget(directoryLineEdit, 0, 1);
   gridLayout->addWidget(specFileButton, 1, 0);
   gridLayout->addWidget(specFileLineEdit, 1, 1);
   gridLayout->setColumnStretch(0, 0);
   gridLayout->setColumnStretch(1, 100);
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(instructionsLabel);
   layout->addLayout(gridLayout);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiStandardMeshOutputSpecFilePage::~GuiStandardMeshOutputSpecFilePage()
{
}

/**
 * called to select directory.
 */
void 
GuiStandardMeshOutputSpecFilePage::slotDirectorySelection()
{
   WuQFileDialog fd(theMainWindow);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setFileMode(WuQFileDialog::DirectoryOnly);
   QString dirText = directoryLineEdit->text();
   QFileInfo fi(dirText);
   if (fi.exists() && fi.isDir()) {
      // ok, do nothing
   }
   else {
      dirText = QDir::currentPath();
   }
   fd.setDirectory(dirText);
   fd.setWindowTitle("Choose Directory");
   if (fd.exec() == QDialog::Accepted) {
      const QString path(fd.directory().absolutePath());
      directoryLineEdit->setText(path);
   }
}

/**
 * called to select output spec file.
 */
void 
GuiStandardMeshOutputSpecFilePage::slotSpecFileSelection()
{
   WuQFileDialog fd(theMainWindow);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   QString dirText = directoryLineEdit->text();
   QFileInfo fi(dirText);
   if (fi.exists() && fi.isDir()) {
      // ok, do nothing
   }
   else {
      dirText = QDir::currentPath();
   }
   fd.setFileMode(WuQFileDialog::ExistingFile);
   fd.setFilters(QStringList(FileFilters::getSpecFileFilter()));
   if (fd.exec() == WuQFileDialog::Accepted) {
      if (fd.selectedFiles().size() > 0) {
         const QString path = FileUtilities::dirname(fd.directory().absolutePath());
         directoryLineEdit->setText(path);
         const QString name = FileUtilities::basename(fd.selectedFiles().at(0));
         specFileLineEdit->setText(name);
      }
   }
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshOutputSpecFilePage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshOutputSpecFilePage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshOutputSpecFilePage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshOutputSpecFilePage::validatePage()
{
   return true;
}


//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshMorphDeformParamsPage::GuiStandardMeshMorphDeformParamsPage(
                 BrainModelSurfaceMultiresolutionMorphing* flatParametersIn,
                 BrainModelSurfaceMultiresolutionMorphing* sphericalParametersIn,
                 QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Multi-resolution Morphing and Deformation Parameters");
   
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
GuiStandardMeshMorphDeformParamsPage::~GuiStandardMeshMorphDeformParamsPage()
{
}

/**
 * is flat multi-res morphing selected.
 */
bool 
GuiStandardMeshMorphDeformParamsPage::getDoFlatMultiResMorphing() const
{
   return flatMultiMorphCheckBox->isChecked();
}

/**
 * is spherical multi-res morphing selected.
 */
bool 
GuiStandardMeshMorphDeformParamsPage::getDoSphericalMultiResMorphing() const
{
   return sphericalMultiMorphCheckBox->isChecked();
}
      
/**
 * called to adjust flat multi-res morph parameters.
 */
void 
GuiStandardMeshMorphDeformParamsPage::slotFlatParamsPushButton()
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
GuiStandardMeshMorphDeformParamsPage::slotSphericalParamsPushButton()
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
GuiStandardMeshMorphDeformParamsPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshMorphDeformParamsPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshMorphDeformParamsPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshMorphDeformParamsPage::validatePage()
{
   return true;
}


//==============================================================================
//==============================================================================
//==============================================================================
/**
 * constructor.
 */
GuiStandardMeshFinishedPage::GuiStandardMeshFinishedPage(QWidget* parent)
   : QWizardPage(parent)
{
   setTitle("Finished");
   
   QLabel* label = new QLabel(" Page");
   
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(label);
   layout->addStretch();
}

/**
 * destructor.
 */
GuiStandardMeshFinishedPage::~GuiStandardMeshFinishedPage()
{
}

/**
 * cleanup page (called when showing page after "Back" pressed).
 */
void 
GuiStandardMeshFinishedPage::cleanupPage()
{
   initializePage();
}

/**
 * initialize the page (called when showing page after "Next" pressed).
 */
void 
GuiStandardMeshFinishedPage::initializePage()
{
}

/**
 * page is complete.
 */
bool 
GuiStandardMeshFinishedPage::isComplete()
{
   return true;
}

/**
 * validate a page.
 */
bool 
GuiStandardMeshFinishedPage::validatePage()
{
   return true;
}

//==============================================================================
//==============================================================================
//==============================================================================

//==============================================================================
//==============================================================================
//==============================================================================

//==============================================================================
//==============================================================================
//==============================================================================

//==============================================================================
//==============================================================================
//==============================================================================

//==============================================================================
//==============================================================================
//==============================================================================

//==============================================================================
//==============================================================================
//==============================================================================

