
#include <QApplication>
#include <QDateTime>
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>

#include "BrainSet.h"
#include "GuiBrainModelOpenGL.h"
#include "BrainModelSurfaceFlattenFullHemisphere.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "DisplaySettingsBorders.h"
#include "GuiFilesModified.h"
#include "GuiFlattenHemisphereInstructionsDialog.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowSurfaceActions.h"
#include "GuiMorphingMeasurementsDialog.h"
#include "GuiMultiresolutionMorphingDialog.h"
#include "QtUtilities.h"
#include "global_variables.h"

static const QString flattenHemisphereText =
   "\n"
   "Examine the border that represents the medial wall boundary and the "
   "borders that represent the cuts.  Selecting Layers:Borders:Show Raised "
   "Borders may make the borders easier to see.  If they are satisfactory, "
   "press the Continue Flattening  button to continue flattening the full "
   "hemisphere.  "
   "\n\n"
   "To create a different medial wall boundary, select Layers:Borders:Draw Border.  "
   "\n\n"
   "In the Draw Border Dialog, select \"Closed Border\".\n"
   "There are two ways to enter the border's name:\n"
   "   a) Enter the name \"MEDIAL.WALL\" in the text box.\n"
   "   b) Press the \"Select Name\" pushbutton and then the \"Select From\n"
   "      Borders\" button in the dialog that pops up.  One more dialog will\n"
   "      pop up and in it, scroll to and select the name \"MEDIAL.WALL\"\n"
   "      and press the \"OK\" button on the Border Name Selection dialog\n"
   "\n"
   "Now, press the Apply button on the Draw Border dialog to start border "
   "drawing.  Draw the border by holding down the left mouse button and "
   "dragging the mouse.  Release the mouse button and then press the mouse "
   "button while holding down the shift key to close the border. "
   "\n\n"
   "To create borders that represent the cuts, switch to \"Open Border\" "
   "in the Draw Border Dialog and enter the name of the cut or use the "
   "Select Name button as described in the draw medial wall section above.  "
   "Assign an appropriate label for each new cut and draw the border. "
   "\n\n"
   "After all of the cuts have been drawn, click the \"Close\" button on "
   "the Draw Border dialog."
   "\n\n"
   "If the medial wall border and/or the cuts have been redrawn, the unwanted "
   "medial wall and/or cuts will need to be deleted.  To do this, select "
   "the \"Delete Border With Mouse\" item Layers:Borders menu and click "
   "the left mouse button on each of the unwanted borders."
   "\n\n"
   "When you are ready, press the \"Continue Flattening\" button to "
   "finish flattening the hemisphere.  "
   "\n\n";

const QString preMorphingInstructions =
   "\n"
   "Cuts should be made: (i) to reduce distortions in the final flat map, or "
   "(ii) to excise regions containing \"crossovers\" if these are near the "
   "perimeter of the map.  Crossovers initially appear red, and can be "
   "toggled on and off using the Display Control Dialog.  "
   "\n\n"
   "To make cuts, \n"
   "  * Select Surface: Cuts: Draw Cut. \n"
   "  * Draw the cut by holding down the left mouse button.  At the end of\n"
   "    the cut release the mouse button then hold down the shift key and\n"
   "    click the left mouse button to terminate the cut.\n"
   "  * To delete undesired cuts, select Surface: Cuts: Delete Cuts and click\n"
   "    the left mouse over undesired cuts.\n"
   "  * Select Surface: Cuts: Apply Cuts to cut the surface.\n"
   "  * Select File: Save Data File to save the topology file. with\n"
   "    an appropriate name.\n"
   "\n"
   "Cuts used to reduce distortion should begin outside the map perimeter at "
   "one end.   Cuts used to excise errors should begin and end outside the "
   "map perimeter."
   "\n\n"
   "When the surface is ready for multi-resolution morphing, Press "
   "the Continue Flattening button."
   "\n\n"
   "If there are major clusters of crossovers (red) that cannot be safely "
   "eliminated by excising regions along the perimeter of the map, it may "
   "be advisable to make additional corrections of the segmented volume "
   "in SureFit and to regenerate a new surface."
   "\n\n";

/**
 * Constructor.
 */
GuiFlattenHemisphereInstructionsDialog::GuiFlattenHemisphereInstructionsDialog(
                                 QWidget* parent,
                                 BrainModelSurfaceFlattenFullHemisphere* algorithmIn,
                                 const DIALOG_MODE dialogModeIn,
                                 Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   setModal(false);
   dialogMode = dialogModeIn;
   algorithm = algorithmIn;
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // Create the text display section
   //
   textEdit = new QTextEdit;
   textEdit->setMinimumSize(500, 300);
   textEdit->setReadOnly(true);
   dialogLayout->addWidget(textEdit);
   
   switch(dialogMode) {
      case DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_PART2:
      case DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_MORPH_SPHERE_PART2:
         setWindowTitle("Continue Flattening Full Hemisphere");
         textEdit->setPlainText(flattenHemisphereText);
         break;
      case DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_PART_HEM:
      case DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM:
      case DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM_MORPH_SPHERE:
         setWindowTitle("Initial Flattening");
         theMainWindow->speakText("Initial flat surface is ready.", false);
         textEdit->setPlainText(preMorphingInstructions);
         break;
   }
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Continue button
   //
   QPushButton* continueButton = new QPushButton("Continue Flattening");
   buttonsLayout->addWidget(continueButton);
   QObject::connect(continueButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   //
   // Cancel button 
   //
   QPushButton* cancelButton = new QPushButton("Cancel Flattening");
   buttonsLayout->addWidget(cancelButton);
   QObject::connect(cancelButton, SIGNAL(clicked()),
                    this, SLOT(reject()));
   
   QtUtilities::makeButtonsSameSize(continueButton, cancelButton);
   
   QApplication::restoreOverrideCursor();
}

/**
 * Destructor.
 */
GuiFlattenHemisphereInstructionsDialog::~GuiFlattenHemisphereInstructionsDialog()
{
}

/**
 * Called when Continue or stop buttons are pressed
 */
void
GuiFlattenHemisphereInstructionsDialog::done(int r)
{
   if (r == QDialog::Accepted) {
      switch(dialogMode) {
         case DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_PART2:
            {
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               
               //
               // Do the remaining part of hemisphere flattening
               //
               doFlattenFullHemispherePart2();
               showCrossovers();
               QApplication::restoreOverrideCursor();
      
               //
               // Popup post initial flattening instructions
               //
               GuiFlattenHemisphereInstructionsDialog* gid = 
                  new GuiFlattenHemisphereInstructionsDialog(theMainWindow,
                                 NULL,
                                 DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM);
               gid->setAttribute(Qt::WA_DeleteOnClose);
               gid->show();
            }
            break;
         case DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_MORPH_SPHERE_PART2:
            {
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

               //
               // Do the remaining part of hemisphere flattening
               //
               doFlattenFullHemispherePart2();
               showCrossovers();
               
               QApplication::restoreOverrideCursor();

               //
               // Popup post initial flattening instructions
               //
               GuiFlattenHemisphereInstructionsDialog* gid = 
                  new GuiFlattenHemisphereInstructionsDialog(theMainWindow,
                                 NULL,
                                 DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM_MORPH_SPHERE);
               gid->setAttribute(Qt::WA_DeleteOnClose);
               gid->show();
            }
            break;
         case DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_PART_HEM:
            {
               //
               // Popup multiresolution morphing dialog to morph flat surface.
               //
               showCrossovers();
               QApplication::restoreOverrideCursor();
               GuiMultiresolutionMorphingDialog mmd(this, 
                           theMainWindow->getSurfaceActions()->getFlatMultiresolutionMorphingObject(),
                           false);
               mmd.exec();
            }
            break;
         case DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM:
            {
               //
               // Popup multiresolution morphing dialog to morph flat surface.
               //
               showCrossovers();
               QApplication::restoreOverrideCursor();
               GuiMultiresolutionMorphingDialog mmd(this, 
                           theMainWindow->getSurfaceActions()->getFlatMultiresolutionMorphingObject(),
                                false);

               if (mmd.exec() == QDialog::Accepted) {               
                  //
                  // Allow user to align flat surface.
                  //
                  theMainWindow->speakText("Ready to align the flat surface.", false);
                  QApplication::restoreOverrideCursor();
                  GuiMainWindowSurfaceActions* surfaceActions = 
                     theMainWindow->getSurfaceActions();
                  surfaceActions->slotAlignSurfacesToStandardOrientation();
               }
            }
            break;
         case DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM_MORPH_SPHERE:
            {
               QApplication::restoreOverrideCursor();
 
               //
               // Multiresolution Morph Flat Surface parameters
               //
               QApplication::restoreOverrideCursor();
               GuiMultiresolutionMorphingDialog flatMorphParameters(
                                 this,
                           theMainWindow->getSurfaceActions()->getFlatMultiresolutionMorphingObject(),
                                 true);
               flatMorphParameters.exec();
               
               //
               // Multiresolution Morph Spherical Surface parameters
               //
               QApplication::restoreOverrideCursor();
               GuiMultiresolutionMorphingDialog sphereMorphParameters(
                                 this,
                           theMainWindow->getSurfaceActions()->getSphericalMultiresolutionMorphingObject(),
                                 true);
               sphereMorphParameters.exec();
               
               //
               // Get the fiducial surface
               //
               BrainModelSurface* fiducialSurface = theMainWindow->getBrainSet()->getActiveFiducialSurface();
               if (fiducialSurface == NULL) {
                  QMessageBox::critical(this, "Morph Error",
                     "Unable to find fiducial surface.");
                  return;
               }   
               
               //
               // Get the flat surface
               //
               BrainModelSurface* flatSurface = 
                  theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
               if (flatSurface == NULL) {
                  QMessageBox::critical(this, "Morph Error",
                     "Unable to find flat surface.");
                  return;
               }   
               
               //
               // Get the spherical surface
               //
               BrainModelSurface* sphericalSurface = 
                  theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
               if (sphericalSurface == NULL) {
                  QMessageBox::critical(this, "Morph Error",
                     "Unable to find spherical surface.");
                  return;
               }   
               
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
               
               //
               // Execute flat morphing
               //
               BrainModelSurfaceMultiresolutionMorphing flatMRM(
                                    theMainWindow->getBrainSet(),
                                    fiducialSurface,
                                    flatSurface,
                                    BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT);
               flatMRM.copyParameters(
                  *theMainWindow->getSurfaceActions()->getFlatMultiresolutionMorphingObject());
               QTime flatTimer;
               flatTimer.start();
               try {
                  flatMRM.execute();
               }
               catch (BrainModelAlgorithmException& e) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(theMainWindow, "Error", e.whatQString());
                  return;
               }
               const float flatElapsedTime = flatTimer.elapsed() * 0.001;
               
               //
               // Execute spherical morphing
               //
               BrainModelSurfaceMultiresolutionMorphing sphericalMRM(
                                    theMainWindow->getBrainSet(),
                                    fiducialSurface,
                                    sphericalSurface,
                                    BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL);
               sphericalMRM.copyParameters(
                  *theMainWindow->getSurfaceActions()->getSphericalMultiresolutionMorphingObject());
               QTime sphericalTimer;
               sphericalTimer.start();
               try {
                  sphericalMRM.execute();
               }
               catch (BrainModelAlgorithmException& e) {
                  QApplication::restoreOverrideCursor();
                  QMessageBox::critical(theMainWindow, "Error", e.whatQString());
                  return;
               }
               const float sphericalElapsedTime = sphericalTimer.elapsed() * 0.001;
               
               QApplication::restoreOverrideCursor();
               
               //
               // Pop up the flat morphing measurements dialog
               //
               std::vector<MorphingMeasurements> fmm;
               flatMRM.getMorphingMeasurements(fmm);
               GuiMorphingMeasurementsDialog* fmmd = 
                  new GuiMorphingMeasurementsDialog(fmm, flatElapsedTime,
                                      BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT,
                                      theMainWindow);
               fmmd->show();
               
               //
               // Pop up the spherical morphing measurements dialog
               //
               std::vector<MorphingMeasurements> smm;
               sphericalMRM.getMorphingMeasurements(smm);
               GuiMorphingMeasurementsDialog* smmd = 
                  new GuiMorphingMeasurementsDialog(smm, sphericalElapsedTime,
                                 BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL,
                                 theMainWindow);
               smmd->show();
               
               //
               // Notify about modified files and redraw all displays
               //
               GuiFilesModified fm;
               fm.setCoordinateModified();
               theMainWindow->fileModificationUpdate(fm);
               GuiBrainModelOpenGL::updateAllGL(NULL);
               
               //
               // Display the flat surface
               //
               flatSurface = 
                  theMainWindow->getBrainSet()->getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
               theMainWindow->displayBrainModelInMainWindow(flatSurface);
               
               showCrossovers(flatSurface);
               
               QApplication::restoreOverrideCursor();

               theMainWindow->speakText("Ready to align surfaces.", false);
               
               //
               // Show dialog to align flat and spherical surfaces
               //
               QApplication::restoreOverrideCursor();
               GuiMainWindowSurfaceActions* surfaceActions = 
                  theMainWindow->getSurfaceActions();
               surfaceActions->slotAlignSurfacesToStandardOrientation();
               
            }
            break;
      }
   }
   
   //
   // update display control dialog and update graphics
   //
   theMainWindow->updateDisplayControlDialog();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   QApplication::restoreOverrideCursor();
   
   QDialog::done(r);
}

/**
 * show crossovers on surface.
 */
void 
GuiFlattenHemisphereInstructionsDialog::showCrossovers(BrainModelSurface* bmsIn)
{
   //
   // Do crossover check and set overlay node coloring to crossovers
   //
   BrainModelSurface* bms = bmsIn;
   if (bms == NULL) {
      bms = theMainWindow->getBrainSet()->getBrainModelSurface(
                                 theMainWindow->getBrainSet()->getNumberOfBrainModels() - 1);
   }
   if (bms != NULL) {
      int numTileCrossovers, numNodeCrossovers;
      bms->crossoverCheck(numTileCrossovers, numNodeCrossovers,
                          BrainModelSurface::SURFACE_TYPE_FLAT);
   }
   int modelIndex = -1;  // -1 -> all models
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   theMainWindow->getBrainSet()->getSecondarySurfaceOverlay()->setOverlay(modelIndex, BrainModelSurfaceOverlay::OVERLAY_NONE);
   theMainWindow->getBrainSet()->getPrimarySurfaceOverlay()->setOverlay(modelIndex, BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS);
   bsnc->assignColors();

   //
   // update display control dialog and update graphics
   //
   theMainWindow->updateDisplayControlDialog();
   GuiBrainModelOpenGL::updateAllGL(NULL);
}
      
void
GuiFlattenHemisphereInstructionsDialog::doFlattenFullHemispherePart2()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   try {
      algorithm->executePart2();
      DisplaySettingsBorders* dsb = theMainWindow->getBrainSet()->getDisplaySettingsBorders();
      dsb->setDisplayBorders(false);
      theMainWindow->updateDisplayControlDialog();
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, "Flatten Error",
                           e.whatQString());               
   }
   delete algorithm;
   
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
   int modelIndex = -1;
   if (bms != NULL) {
      int numTileCrossovers, numNodeCrossovers;
      bms->crossoverCheck(numTileCrossovers, numNodeCrossovers, BrainModelSurface::SURFACE_TYPE_FLAT);
      modelIndex = bms->getBrainModelIndex();
   }
   BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
   bsnc->assignColors();
   if (theMainWindow->getBrainSet()->isASurfaceOverlay(modelIndex,
               BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS) == false) {
      theMainWindow->getBrainSet()->getSecondarySurfaceOverlay()->setOverlay(modelIndex, 
                     theMainWindow->getBrainSet()->getPrimarySurfaceOverlay()->getOverlay(modelIndex));
      theMainWindow->getBrainSet()->getPrimarySurfaceOverlay()->setOverlay(modelIndex, 
                     BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS);
   }
   
   //
   // update display control dialog and update graphics
   //
   theMainWindow->updateDisplayControlDialog();
   GuiBrainModelOpenGL::updateAllGL(NULL);
   
   QApplication::restoreOverrideCursor();
}

