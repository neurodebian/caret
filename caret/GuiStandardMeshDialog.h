
#ifndef __GUI_STANDARD_MESH_DIALOG_H__
#define __GUI_STANDARD_MESH_DIALOG_H__

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

#include <QWizard>
#include <QWizardPage>

class BrainModelSurface;
class BrainModelSurfaceMultiresolutionMorphing;
class GuiBrainModelSurfaceSelectionComboBox;
class GuiStandardMeshAtlasSelectionPage;
class GuiStandardMeshBorderUpdatePage;
class GuiStandardMeshBorderValidationPage;
class GuiStandardMeshFiducialSurfacePage;
class GuiStandardMeshFinishedPage;
class GuiStandardMeshIntroPage;
class GuiStandardMeshLandmarkGenerationPage;
class GuiStandardMeshMorphDeformParamsPage;
class GuiStandardMeshOutputSpecFilePage;
class GuiStandardMeshSurfacesPage;
class QCheckBox;
class QLineEdit;

/// the atlas class
class StandardMeshAtlas {
   public:
      /// constructor
      StandardMeshAtlas(const QString& nameIn,
                        const QString& specFilePathIn,
                        const int numberOfNodesIn)
         : name(nameIn),
           specFilePath(specFilePathIn),
           numberOfNodes(numberOfNodesIn) { }
      
      /// destructor
      ~StandardMeshAtlas() { }
      
      /// get name
      QString getName() const { return name; }
      
      /// get spec file's path
      QString getSpecFilePath() const { return specFilePath; }
      
      /// get the number of nodes
      int getNumberOfNodes() const { return numberOfNodes; }
      
   protected:
      /// name of atlas
      QString name;
      
      /// spec file path
      QString specFilePath;
      
      /// number of nodes
      int numberOfNodes;
};

/// dialog for generating standard meshes
class GuiStandardMeshDialog : public QWizard {
   Q_OBJECT
   
   public:
      // constructor
      GuiStandardMeshDialog(QWidget* parent);
      
      // destructor
      ~GuiStandardMeshDialog();
      
      // get the fiducial surface
      BrainModelSurface* getFiducialSurface();
      
      // get the inflated surface
      BrainModelSurface* getInflatedSurface();
      
      // get the very inflated surface
      BrainModelSurface* getVeryInflatedSurface();
      
      // get the spherical surface
      BrainModelSurface* getSphericalSurface();
      
      // show the first page of the dialog
      void showFirstPage();
      
      // update the dialog
      void updateDialog();
      
      // called to validate the current page
      bool validateCurrentPage();
      
      // save new surfaces
      void saveSurfaces(const int indexOfFirstCoordinateFileToSave,
                        const int indexOfFirstTopologyFileToSave = -1);
           
      /// get the number of atlases
      int getNumberOfAtlases() const { return atlases.size(); }
      
      /// get an atlas
      const StandardMeshAtlas* getAtlas(const int indx) const
                                          { return &atlases[indx]; }
          
      /// get the selected atlas
      const StandardMeshAtlas* getSelectedAtlas() const { return selectedAtlas; }
      
      /// set the selected atlas
      void setSelectedAtlas(StandardMeshAtlas* a) { selectedAtlas = a; }
      
      /// set the selected atlas
      void setSelectedAtlas(const int indx) { selectedAtlas = &atlases[indx]; }
      
   public slots:
      /// called when cancel button pressed
      void reject();
      
   protected:
      
      // load the atlases
      QString loadAtlases();
      
      /// the atlases
      std::vector<StandardMeshAtlas> atlases;
      
      /// the selected atlas
      StandardMeshAtlas* selectedAtlas;
      
      /// the intro page
      GuiStandardMeshIntroPage* introPage;
      
      /// the fidicual surface page
      GuiStandardMeshFiducialSurfacePage* fiducialSurfacePage;
      
      /// the surfaces page
      GuiStandardMeshSurfacesPage* surfacesPage;
      
      /// the atlas selection page
      GuiStandardMeshAtlasSelectionPage* atlasSelectionPage;
      
      /// the landmark border generation page
      GuiStandardMeshLandmarkGenerationPage* landmarkBorderGenerationPage;
      
      /// the border draw and update page
      GuiStandardMeshBorderUpdatePage* borderDrawUpdatePage;
      
      /// the border validation page
      GuiStandardMeshBorderValidationPage* borderValidationPage;
      
      /// the output spec file page
      GuiStandardMeshOutputSpecFilePage* outputSpecFilePage;
      
      /// morphing and deformation parameters page
      GuiStandardMeshMorphDeformParamsPage* morphDeformParamsPage;
      
      /// the finished page
      GuiStandardMeshFinishedPage* finishedPage;

      /// flat multi-resolution morphing parameters
      BrainModelSurfaceMultiresolutionMorphing* flatParameters;
      
      /// spherical multi-resolution morphing parameters
      BrainModelSurfaceMultiresolutionMorphing* sphericalParameters;
};

//=============================================================================
/// class for introduction page
class GuiStandardMeshIntroPage : public QWizardPage {
   Q_OBJECT

   public:
      // constructor
      GuiStandardMeshIntroPage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshIntroPage();

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


//=============================================================================
/// class for fiducial surface and topology page
class GuiStandardMeshFiducialSurfacePage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiStandardMeshFiducialSurfacePage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshFiducialSurfacePage();
      
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

//=============================================================================
/// class for selecting surfaces
class GuiStandardMeshSurfacesPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiStandardMeshSurfacesPage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshSurfacesPage();
      
      // get the inflated surface
      BrainModelSurface* getInflatedSurface();
      
      // get the very inflated surface
      BrainModelSurface* getVeryInflatedSurface();
      
      // get the spherical surface
      BrainModelSurface* getSphericalSurface();
      
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

//=============================================================================
/// class for atlas selection page
class GuiStandardMeshAtlasSelectionPage : public QWizardPage {
   Q_OBJECT

   public:
      // constructor
      GuiStandardMeshAtlasSelectionPage(GuiStandardMeshDialog* smd,
                                        QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshAtlasSelectionPage();

      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();

      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();

      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();

   protected slots:
      // called when atlas button selected
      void slotAtlasSelection(int id);
      
   protected:
      
};

//=============================================================================
/// class for generating template cuts
class GuiStandardMeshLandmarkGenerationPage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiStandardMeshLandmarkGenerationPage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshLandmarkGenerationPage();
      
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
      // called to add landmark border cuts
      void slotAddLandmarkBordersPushButton();
      
   protected:
};

//=============================================================================
/// class for drawing updating medial wall and cuts
class GuiStandardMeshBorderUpdatePage : public QWizardPage {
   Q_OBJECT
   
   public:
      // constructor
      GuiStandardMeshBorderUpdatePage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshBorderUpdatePage();
      
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

//=============================================================================
/// class for border validation page
class GuiStandardMeshBorderValidationPage : public QWizardPage {
   Q_OBJECT

   public:
      // constructor
      GuiStandardMeshBorderValidationPage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshBorderValidationPage();

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

//=============================================================================
/// class for output spec file page
class GuiStandardMeshOutputSpecFilePage : public QWizardPage {
   Q_OBJECT

   public:
      // constructor
      GuiStandardMeshOutputSpecFilePage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshOutputSpecFilePage();

      // cleanup page (called when showing page after "Back" pressed)
      void cleanupPage();

      // initialize the page (called when showing page after "Next" pressed)
      void initializePage();

      // page is complete
      bool isComplete();
      
      // validate a page
      bool validatePage();

   protected slots:
      // called to select directory
      void slotDirectorySelection();

      // called to select output spec file
      void slotSpecFileSelection();

   protected:
      /// line edit for directory
      QLineEdit* directoryLineEdit;
      
      /// line edit for spec file name
      QLineEdit* specFileLineEdit;
};

//=============================================================================
//=============================================================================
/// class for morphing and deformation parameters page
class GuiStandardMeshMorphDeformParamsPage : public QWizardPage {
   Q_OBJECT

   public:
      // constructor
      GuiStandardMeshMorphDeformParamsPage(
               BrainModelSurfaceMultiresolutionMorphing* flatParametersIn,
               BrainModelSurfaceMultiresolutionMorphing* sphericalParametersIn,
               QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshMorphDeformParamsPage();

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

//=============================================================================
/// class for final page
class GuiStandardMeshFinishedPage : public QWizardPage {
   Q_OBJECT

   public:
      // constructor
      GuiStandardMeshFinishedPage(QWidget* parent = 0);
      
      // destructor
      ~GuiStandardMeshFinishedPage();

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
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

#endif  // __GUI_STANDARD_MESH_DIALOG_H__

