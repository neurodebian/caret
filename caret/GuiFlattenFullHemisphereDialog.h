

#ifndef __GUI_FLATTEN_FULL_HEMISPHERE_DIALOG_H__
#define __GUI_FLATTEN_FULL_HEMISPHERE_DIALOG_H__

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

#include <vector>

#include <QWizardPage>

#include "WuQWizard.h"

class BrainModelSurface;
class BrainModelSurfaceMultiresolutionMorphing;
class GuiBrainModelSurfaceSelectionComboBox;
class GuiFlattenFullHemisphereBorderUpdatePage;
class GuiFlattenFullHemisphereFiducialPage;
class GuiFlattenFullHemisphereFinishedPage;
class GuiFlattenFullHemisphereInitialFlatPage;
class GuiFlattenFullHemisphereIntroPage;
class GuiFlattenFullHemisphereMultiresolutionMorphingPage;
class GuiFlattenFullHemisphereSettingsPage;
class GuiFlattenFullHemisphereStartFlatteningPage;
class GuiFlattenFullHemisphereSurfaceAlignmentPage;
class GuiFlattenFullHemisphereSphericalPage;
class GuiFlattenFullHemisphereTemplateCutsPage;
class QCheckBox;
class QLabel;

/// class for dialog that flattens a full hemisphere
class GuiFlattenFullHemisphereDialog : public WuQWizard {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereDialog(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereDialog();
      
      // update the dialog
      void updateDialog();
      
      // get the fiducial surface
      BrainModelSurface* getFiducialSurface();
      
      // get the inflated surface
      BrainModelSurface* getInflatedSurface();
      
      // get the very inflated surface
      BrainModelSurface* getVeryInflatedSurface();
      
      // get the spherical surface
      BrainModelSurface* getSphericalSurface();
      
      // get the auto save files status
      bool getAutoSaveFilesSelected() const;
      
      // called to validate the current page
      bool validateCurrentPage();
      
      // save new surfaces
      void saveSurfaces(const int indexOfFirstCoordinateFileToSave,
                        const int indexOfFirstTopologyFileToSave = -1);
                        
      // set the central sulcus ventral tip node number
      void setCentralSulcusVentralTip(const int nodeNum);
      
      // set the central sulcus dorsal-medial tip node number
      void setCentralSulcusDorsalMedialTip(const int nodeNum);
      
      // show the first page
      void showFirstPage();
      
   public slots:
      /// called when cancel button pressed
      void reject();
      
   protected:
      // save borders, if needed
      void saveNewBorders();
      
      /// intro page
      GuiFlattenFullHemisphereIntroPage* introPage;
      
      /// settings page
      GuiFlattenFullHemisphereSettingsPage* settingsPage;
      
      /// topology page
      GuiFlattenFullHemisphereFiducialPage* fiducialSurfacePage;
      
      /// surface page
      GuiFlattenFullHemisphereSphericalPage* sphericalSurfacePage;
      
      /// template cuts page
      GuiFlattenFullHemisphereTemplateCutsPage* templateCutsPage;
      
      /// border draw and update page
      GuiFlattenFullHemisphereBorderUpdatePage* borderDrawAndUpdatePage;
      
      /// surface alignment page
      GuiFlattenFullHemisphereSurfaceAlignmentPage* surfaceAlignmentPage;
      
      /// start flattening page
      GuiFlattenFullHemisphereStartFlatteningPage* startFlatteningPage;
      
      /// initial flat page
      GuiFlattenFullHemisphereInitialFlatPage* initialFlatPage;
      
      /// multiresolution morph page
      GuiFlattenFullHemisphereMultiresolutionMorphingPage* multiMorphPage;
      
      /// finished page
      GuiFlattenFullHemisphereFinishedPage* finishedPage;
      
      /// flat multi-resolution morphing parameters
      BrainModelSurfaceMultiresolutionMorphing* flatParameters;
      
      /// spherical multi-resolution morphing parameters
      BrainModelSurfaceMultiresolutionMorphing* sphericalParameters;
};

/// class for introduction page
class GuiFlattenFullHemisphereIntroPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereIntroPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereIntroPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();

   protected:
   
};

/// class for settings
class GuiFlattenFullHemisphereSettingsPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereSettingsPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereSettingsPage();
      
      // get the auto save files checkbox status
      bool getAutoSaveFilesSelected() const;
      
      // get the create fiducial with smoothed medial wall checkbox status
      bool getCreateFiducialWithSmoothedMedialWallSelected() const;
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected:
      /// auto save files check box
      QCheckBox* autoSaveFilesCheckBox;

      // smooth medial wall check box
      QCheckBox* smoothMedialWallCheckBox;
};

/// class for topology
class GuiFlattenFullHemisphereFiducialPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereFiducialPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereFiducialPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
      // get the fiducial surface
      BrainModelSurface* getFiducialSurface();
      
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected:
      /// surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* fiducialSurfaceComboBox;      
};

/// class for verifying surfaces
class GuiFlattenFullHemisphereSphericalPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereSphericalPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereSphericalPage();
      
      // get the spherical surface
      BrainModelSurface* getSphericalSurface();
      
      // get the inflated surface
      BrainModelSurface* getInflatedSurface();
      
      // get the very inflated surface
      BrainModelSurface* getVeryInflatedSurface();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected slots:
      // called when generate surfaces button clicked
      void slotGenerateSurfacesPushButton();
      
   protected:
      /// surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* inflatedSurfaceComboBox;
      
      /// surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* veryInflatedSurfaceComboBox;
      
      /// surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* sphericalSurfaceComboBox;
      
};


/// class for generating template cuts
class GuiFlattenFullHemisphereTemplateCutsPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereTemplateCutsPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereTemplateCutsPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected slots:
      // called to add border template cuts
      void slotAddTemplateCutBordersPushButton();
      
      // called to add landmark border cuts
      void slotAddLandmarkBordersPushButton();
      
   protected:
};

/// class for drawing updating medial wall and cuts
class GuiFlattenFullHemisphereBorderUpdatePage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereBorderUpdatePage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereBorderUpdatePage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected slots:
      // called when draw borders button pressed
      void slotDrawBordersPushButton();
      
      // called when update borders button pressed
      void slotUpdateBordersPushButton();
      
      // called when example images button pressed
      void slotBorderExampleImages();
      
      // called when show only flattening borders selected
      void slotShowOnlyFlattenBordersPushButton();
      
   protected:
};

/// class for cleaning up initial flat surface
class GuiFlattenFullHemisphereInitialFlatPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereInitialFlatPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereInitialFlatPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected:
};

/// class for starting flattening surface
class GuiFlattenFullHemisphereStartFlatteningPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereStartFlatteningPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereStartFlatteningPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
      
      // get loaded flattening borders
      void getLoadedFlatteningBorders(std::vector<QString>& flattenBorderNamesOut) const;
      
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected:
      /// names of borders that will be used
      QLabel* bordersListLabel;
};

/// class for multi-resolution morphing
class GuiFlattenFullHemisphereMultiresolutionMorphingPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereMultiresolutionMorphingPage(
         BrainModelSurfaceMultiresolutionMorphing* flatParametersIn,
         BrainModelSurfaceMultiresolutionMorphing* sphericalParametersIn,
         QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereMultiresolutionMorphingPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
      // is flat multi-res morphing selected
      bool getDoFlatMultiResMorphing() const;
      
      // is spherical multi-res morphing selected
      bool getDoSphericalMultiResMorphing() const;
      
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected slots:
      // called to adjust flat multi-res morph parameters
      void slotFlatParamsPushButton();

      // called to adjust spherical multi-res morph parameters
      void slotSphericalParamsPushButton();

   protected:
      /// perform flat multi-res morphing check box
      QCheckBox* flatMultiMorphCheckBox;

      /// perform spherical multi-res morphing check box
      QCheckBox* sphericalMultiMorphCheckBox;
      
      /// flat multi-resolution morphing parameters
      BrainModelSurfaceMultiresolutionMorphing* flatParameters;
      
      /// spherical multi-resolution morphing parameters
      BrainModelSurfaceMultiresolutionMorphing* sphericalParameters;
};

/// class for standard surface alignment page
class GuiFlattenFullHemisphereSurfaceAlignmentPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereSurfaceAlignmentPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereSurfaceAlignmentPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
      // get the central suclus ventral and dorsal-medial tips
      void getCentralSulcusTips(int& ventralTipNodeNumberOut,
                                int& dorsalMedialTipNodeNumberOut) const;
                                
      // set the central sulcus ventral tip
      void setCentralSulcusVentralTip(const int nodeNum);
      
      // set the central sulcus dorsal-medial tip
      void setCentralSulcusDorsalMedialTip(const int nodeNum);
      
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();
      
   protected slots:
      // called when enable mouse button pressed
      void slotEnableMousePushButton();
      
      // called when Use LANDMARK.CentralSulcus button pressed
      void slotUseLandmarkCentralSulcusPushButton();
      
   protected:
      // update the ces node numbers
      void updateCentralSulcusLabels();
      
      /// Use LANDMARK.CentralSulcus button
      QPushButton* useLandmarkCentralSulcusPushButton;
      
      /// ventral node label
      QLabel* ventralNodeLabel;
      
      /// dorsal-medial node Label
      QLabel* dorsalMedialNodeLabel;   
      
      /// ventral tip node number
      int ventralTipNodeNumber;
      
      /// dorsal-medial tip node number
      int dorsalMedialTipNodeNumber;
};

/// class for finished page
class GuiFlattenFullHemisphereFinishedPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiFlattenFullHemisphereFinishedPage(QWidget* parent = 0);
      
      // destructor
      ~GuiFlattenFullHemisphereFinishedPage();
      
      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();
      
      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();
      
      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();
   
   signals:
      /// emitted when "complete" status of page changes
      void completeChanged();      
};

#endif // __GUI_FLATTEN_FULL_HEMISPHERE_DIALOG_H__
