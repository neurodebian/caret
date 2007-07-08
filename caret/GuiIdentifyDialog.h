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


#ifndef __VE_GUI_IDENTIFY_DIALOG_H__
#define __VE_GUI_IDENTIFY_DIALOG_H__

#include <QAction>
#include "QtDialog.h"
#include "QtMainWindow.h"

#include "BrainModel.h"
#include "BrainSet.h"

class QCheckBox;
class QGroupBox;
class GuiHyperLinkTextBrowser;
class GuiIdentifyMainWindow;
class QSpinBox;

class BrainModelOpenGLSelectedItem;

/// Dialog that displays a scrolling text area containing identification information
class GuiIdentifyDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiIdentifyDialog(QWidget* parent);
      
      /// Destructor
      ~GuiIdentifyDialog();
      
      /// append to the text display
      void appendText(const QString& s);
      
      /// append html
      void appendHtml(const QString& html);
      
      /// apply a scene (update dialog)
      void showScene(const SceneFile::Scene& scene,
                     const int mainWindowX,
                     const int mainWindowY,
                     const int mainWindowSceneX,
                     const int mainWindowSceneY,
                     const int screenMaxX,
                     const int screenMaxY,
                     QString& errorMessage);
      
      /// create a scene (save dialog settings)
      void saveScene(std::vector<SceneFile::SceneClass>& scs);
  
      /// display vocabulary name in identify window
      void displayVocabularyNameData(const QString& name);
      
      /// update the dialog
      void updateDialog();
      
   private:
      /// display info for the volume
      void displayVolumeInfo(VolumeFile* vf, const int i, const int j, const int k,
                               const BrainModel::BRAIN_MODEL_VIEW_NUMBER windowNumber);

      /// the main window placed into the dialog
      GuiIdentifyMainWindow* idMainWindow;
      
      /// significant digits for display
      int sigDigits;
      
      /// cross timer window number
      BrainModel::BRAIN_MODEL_VIEW_NUMBER crossTimerWindowNumber;
};

/// Main Window that is placed in dialog to allow toolbars
class GuiIdentifyMainWindow : public QtMainWindow {
   Q_OBJECT
   
   public:
      /// Constructor.
      GuiIdentifyMainWindow(GuiIdentifyDialog* parent = 0);
      
      /// Destructor.
      ~GuiIdentifyMainWindow();
      
      /// append to the text display
      void appendText(const QString& s);
      
      /// append html
      void appendHtml(const QString& html);
      
      /// update the filtering toggle buttons
      void updateToolBarButtons();
      
      /// update the filtering selections
      void updateFilteringSelections();
      
   private slots:
      ///  all on/off button 
      void slotAllOnOff();
      
      /// clear all node ID symbols
      void clearNodeID();
      
      /// display border information toolbar button
      void slotBorderAction(bool val);
      
      /// display cell information toolbar button
      void slotCellAction(bool val);
      
      /// display foci information toolbar button
      void slotFociAction(bool val);    
      
      /// display foci name information toolbar button
      void slotFociNameAction(bool val);    
      
      /// display foci class information toolbar button
      void slotFociClassAction(bool val);    
      
      /// display foci original stereotaxic position information toolbar button
      void slotFociOriginalStereotaxicPositionAction(bool val);    
      
      /// display foci stereotaxic position information toolbar button
      void slotFociStereotaxicPositionAction(bool val);    
      
      /// display foci area information toolbar button
      void slotFociAreaAction(bool val);    
      
      /// display foci geography information toolbar button
      void slotFociGeographyAction(bool val);    
      
      /// display foci size information toolbar button
      void slotFociSizeAction(bool val);    
      
      /// display foci Statistic information toolbar button
      void slotFociStatisticAction(bool val);    
      
      /// display foci comment information toolbar button
      void slotFociCommentAction(bool val);    
      
      /// display voxel information toolbar button
      void slotVoxelAction(bool val);
      
      /// display contour action
      void slotContourAction(bool val);
      
      /// display node information
      void slotNodeAction(bool val);
      
      /// display node coordinate information
      void slotNodeCoordAction(bool val);
      
      /// display node lat/lon information
      void slotNodeLatLonAction(bool val);
      
      /// display node paint information
      void slotNodePaintAction(bool val);
      
      /// display node probabilistic atlas information
      void slotNodeProbAtlasAction(bool val);
      
      /// display node rgb paint information
      void slotNodeRgbPaintAction(bool val);
      
      /// display node metric information
      void slotNodeMetricAction(bool val);
      
      /// display node shape information
      void slotNodeShapeAction(bool val);
      
      /// display node section information
      void slotNodeSectionAction(bool val);
      
      /// display node areal estimation information
      void slotNodeArealEstAction(bool val);
      
      /// display node topography information
      void slotNodeTopographyAction(bool val);
      
      /// show study meta-analysis info group box
      void slotShowMetaAnalysisInfoGroupBox(bool val);
   
      /// show study meta-analysis name info check box
      void slotShowMetaAnalysisNameInfoCheckBox(bool val);

      /// show study meta-analysis title info check box
      void slotShowMetaAnalysisTitleInfoCheckBox(bool val);

      /// show study meta-analysis authors info check box
      void slotShowMetaAnalysisAuthorsInfoCheckBox(bool val);

      /// show study meta-analysis citation info check box
      void slotShowMetaAnalysisCitationInfoCheckBox(bool val);

      /// show study meta-analysis DOI/URL info check box
      void slotShowMetaAnalysisDoiUrlInfoCheckBox(bool val);

      /// display study information
      void slotStudyAction(bool val);
      
      /// show study title info check box;
      void slotShowStudyTitleInfoCheckBox(bool val);
      
      /// show study author info check box;
      void slotShowStudyAuthorInfoCheckBox(bool val);
      
      /// show study citation info check box;
      void slotShowStudyCitationInfoCheckBox(bool val);
      
      /// show study comment info check box;
      void slotShowStudyCommentInfoCheckBox(bool val);
      
      /// show study DOI info check box;
      void slotShowStudyDOIInfoCheckBox(bool val);
      
      /// show study keywords info check box;
      void slotShowStudyKeywordsInfoCheckBox(bool val);
      
      /// show study medical subject headings info check box
      void slotShowStudyMedicalSubjectHeadingInfoCheckBox(bool val);
   
      /// show study name info check box
      void slotShowStudyNameInfoCheckBox(bool val);
   
      /// show study part scheme abbrev info check box;
      void slotShowStudyPartSchemeAbbrevInfoCheckBox(bool val);
      
      /// show study part scheme full info check box;
      void slotShowStudyPartSchemeFullInfoCheckBox(bool val);
      
      /// show study PubMed ID info check box;
      void slotShowStudyPubMedIDInfoCheckBox(bool val);
      
      /// show study project ID info check box;
      void slotShowStudyProjectIDInfoCheckBox(bool val);
      
      /// show study stereotaxic space info check box;
      void slotShowStudyStereotaxicSpaceInfoCheckBox(bool val);
      
      /// show study stereotaxic space details info check box;
      void slotShowStudyStereotaxicSpaceDetailsInfoCheckBox(bool val);
      
      /// show study URL info check box;
      void slotShowStudyURLInfoCheckBox(bool val);
      
      /// show study table info group box;
      void slotShowStudyTableInfoGroupBox(bool val);
      
      /// show study table header info check box;
      void slotShowStudyTableHeaderInfoCheckBox(bool val);
      
      /// show study table footer info check box;
      void slotShowStudyTableFooterInfoCheckBox(bool val);
      
      /// show study table size units info check box;
      void slotShowStudyTableSizeUnitsInfoCheckBox(bool val);
      
      /// show study table voxel size info check box;
      void slotShowStudyTableVoxelSizeInfoCheckBox(bool val);
      
      /// show study table statistic info check box;
      void slotShowStudyTableStatisticInfoCheckBox(bool val);
      
      /// show study table statistic description info check box;
      void slotShowStudyTableStatisticDescriptionInfoCheckBox(bool val);
      
      /// show study figure info group box;
      void slotShowStudyFigureInfoGroupBox(bool val);
      
      /// show study figure legend info check box;
      void slotShowStudyFigureLegendInfoCheckBox(bool val);
      
      /// show study figure panel info group box;
      void slotShowStudyFigurePanelInfoGroupBox(bool val);
      
      /// show study figure panel description info check box;
      void slotShowStudyFigurePanelDescriptionInfoCheckBox(bool val);
      
      /// show study figure panel task description info check box;
      void slotShowStudyFigurePanelTaskDescriptionInfoCheckBox(bool val);
      
      /// show study figure panel task baseline info check box;
      void slotShowStudyFigurePanelTaskBaselineInfoCheckBox(bool val);
      
      /// show study figure panel test attributes info check box;
      void slotShowStudyFigurePanelTestAttributesInfoCheckBox(bool val);
      
      /// show study subheader info group box
      void slotShowStudySubHeaderInfoGroupBox(bool val);
      
      /// show study subheader name info check box
      void slotShowStudySubHeaderNameInfoCheckBox(bool val);
      
      /// show study subheader short name info check box
      void slotShowStudySubHeaderShortNameInfoCheckBox(bool val);
      
      /// show study subheader task description info check box
      void slotShowStudySubHeaderTaskDescriptionInfoCheckBox(bool val);
      
      /// show study subheader task baseline info check box
      void slotShowStudySubHeaderTaskBaselineInfoCheckBox(bool val);
      
      /// show study subheader test attributes info check box
      void slotShowStudySubHeaderTestAttributesInfoCheckBox(bool val);
      
      /// show study page reference info group box;
      void slotShowStudyPageReferenceInfoGroupBox(bool val);
      
      /// show study page reference header info check box;
      void slotShowStudyPageReferenceHeaderInfoCheckBox(bool val);
      
      /// show study page reference comment info check box;
      void slotShowStudyPageReferenceCommentInfoCheckBox(bool val);
      
      /// show study page reference size units info check box;
      void slotShowStudyPageReferenceSizeUnitsInfoCheckBox(bool val);
      
      /// show study page reference voxel size info check box;
      void slotShowStudyPageReferenceVoxelSizeInfoCheckBox(bool val);
      
      /// show study page reference statistic info check box;
      void slotShowStudyPageReferenceStatisticInfoCheckBox(bool val);
      
      /// show study page reference statistic description info check box;
      void slotShowStudyPageReferenceStatisticDescriptionInfoCheckBox(bool val);
      
      /// show study page number info check box;
      void slotShowStudyPageNumberInfoCheckBox(bool val);

      /// show ID symbols on surface
      void slotShowIDAction(bool val);
      
      /// significant digits display
      void slotSignificantDigitsSpinBox(int val);
      
   private:
      // create the text display widget
      QWidget* createTextDisplayWidget();
      
      // create the widget for the display filtering
      QWidget* createDisplayFilterWidget();
      
      // create the tool bar
      QToolBar* createToolBar();
      
      /// text editor for display of identification information
      GuiHyperLinkTextBrowser* textDisplayBrowser;
      
      /// show ID symbols on surface
      QAction* showIDAction;
      
      /// significant digits display
      QSpinBox* significantDigitsSpinBox;
      
      /// show node info group box
      QGroupBox* showNodeInfoGroupBox;
      
      /// show node areal estimation info check box
      QCheckBox* showNodeArealEstInfoCheckBox;
      
      /// show node coord info check box
      QCheckBox* showNodeCoordInfoCheckBox;
      
      /// show node lat/lon info check box
      QCheckBox* showNodeLatLonInfoCheckBox;
      
      /// show node metric info check box
      QCheckBox* showNodeMetricInfoCheckBox;
      
      /// show node paint info check box
      QCheckBox* showNodePaintInfoCheckBox;
      
      /// show node prob atlas info check box
      QCheckBox* showNodeProbAtlasInfoCheckBox;
      
      /// show node rgb paint info check box
      QCheckBox* showNodeRgbPaintInfoCheckBox;
      
      /// show node section info check box
      QCheckBox* showNodeSectionInfoCheckBox;
      
      /// show node shape info check box
      QCheckBox* showNodeShapeInfoCheckBox;
      
      /// show node topography info check box
      QCheckBox* showNodeTopographyInfoCheckBox;
      
      /// show border info check box;
      QCheckBox* showBorderInfoCheckBox;
      
      /// show cell info check box;
      QCheckBox* showCellInfoCheckBox;
      
      /// show contour info check box;
      QCheckBox* showContourInfoCheckBox;
      
      /// show foci info group box;
      QGroupBox* showFociInfoGroupBox;
      
      /// show foci name info check box;
      QCheckBox* showFociNameInfoCheckBox;
      
      /// show foci class info check box;
      QCheckBox* showFociClassInfoCheckBox;
      
      /// show foci original stereotaxic position info check box;
      QCheckBox* showFociOriginalStereotaxicPositionInfoCheckBox;
      
      /// show foci stereotaxic position info check box;
      QCheckBox* showFociStereotaxicPositionInfoCheckBox;
      
      /// show foci area info check box;
      QCheckBox* showFociAreaInfoCheckBox;
      
      /// show foci geography info check box;
      QCheckBox* showFociGeographyInfoCheckBox;
      
      /// show foci size info check box;
      QCheckBox* showFociSizeInfoCheckBox;
      
      /// show foci statistic info check box;
      QCheckBox* showFociStatisticInfoCheckBox;
      
      /// show foci comment info check box;
      QCheckBox* showFociCommentInfoCheckBox;
      
      /// show voxel info check box;
      QCheckBox* showVoxelInfoCheckBox;
      
      /// show study meta-analysis info group box
      QGroupBox* showStudyMetaAnalysisInfoGroupBox;
      
      /// show study name info check box;
      QCheckBox* showStudyMetaAnalysisNameInfoCheckBox;
      
      /// show study title info check box;
      QCheckBox* showStudyMetaAnalysisTitleInfoCheckBox;
      
      /// show study author info check box;
      QCheckBox* showStudyMetaAnalysisAuthorInfoCheckBox;
      
      /// show study citation info check box;
      QCheckBox* showStudyMetaAnalysisCitationInfoCheckBox;
      
      /// show study DOI info check box;
      QCheckBox* showStudyMetaAnalysisDOIInfoCheckBox;
      
      /// show study info group box;
      QGroupBox* showStudyInfoGroupBox;
      
      /// show study title info check box;
      QCheckBox* showStudyTitleInfoCheckBox;
      
      /// show study author info check box;
      QCheckBox* showStudyAuthorInfoCheckBox;
      
      /// show study citation info check box;
      QCheckBox* showStudyCitationInfoCheckBox;
      
      /// show study comment info check box;
      QCheckBox* showStudyCommentInfoCheckBox;
      
      /// show study DOI info check box;
      QCheckBox* showStudyDOIInfoCheckBox;
      
      /// show study keywords info check box;
      QCheckBox* showStudyKeywordsInfoCheckBox;
      
      /// show study medical subject info check box
      QCheckBox* showStudyMedicalSubjectHeadingInfoCheckBox;
      
      /// show study name info check box
      QCheckBox* showStudyNameInfoCheckBox;
      
      /// show study part scheme abbrev info check box;
      QCheckBox* showStudyPartSchemeAbbrevInfoCheckBox;
      
      /// show study part scheme full info check box;
      QCheckBox* showStudyPartSchemeFullInfoCheckBox;
      
      /// show study PubMed ID info check box;
      QCheckBox* showStudyPubMedIDInfoCheckBox;
      
      /// show study project ID info check box;
      QCheckBox* showStudyProjectIDInfoCheckBox;
      
      /// show study stereotaxic space info check box;
      QCheckBox* showStudyStereotaxicSpaceInfoCheckBox;
      
      /// show study stereotaxic space details info check box;
      QCheckBox* showStudyStereotaxicSpaceDetailsInfoCheckBox;
      
      /// show study URL info check box;
      QCheckBox* showStudyURLInfoCheckBox;
      
      /// show study table info group box;
      QGroupBox* showStudyTableInfoGroupBox;
      
      /// show study table header info check box;
      QCheckBox* showStudyTableHeaderInfoCheckBox;
      
      /// show study table footer info check box;
      QCheckBox* showStudyTableFooterInfoCheckBox;
      
      /// show study table size units info check box;
      QCheckBox* showStudyTableSizeUnitsInfoCheckBox;
      
      /// show study table voxel size info check box;
      QCheckBox* showStudyTableVoxelSizeInfoCheckBox;
      
      /// show study table statistic info check box;
      QCheckBox* showStudyTableStatisticInfoCheckBox;
      
      /// show study table statistic description info check box;
      QCheckBox* showStudyTableStatisticDescriptionInfoCheckBox;
      
      /// show study figure info group box;
      QGroupBox* showStudyFigureInfoGroupBox;
      
      /// show study figure legend info check box;
      QCheckBox* showStudyFigureLegendInfoCheckBox;
      
      /// show study figure panel info group box;
      QGroupBox* showStudyFigurePanelInfoGroupBox;
      
      /// show study figure panel description info group box;
      QCheckBox* showStudyFigurePanelDescriptionInfoCheckBox;
      
      /// show study figure panel task description info check box;
      QCheckBox* showStudyFigurePanelTaskDescriptionInfoCheckBox;
      
      /// show study figure panel task baseline info check box;
      QCheckBox* showStudyFigurePanelTaskBaselineInfoCheckBox;
      
      /// show study figure panel test attributes info check box;
      QCheckBox* showStudyFigurePanelTestAttributesInfoCheckBox;
      
      /// show study subheader info group box
      QGroupBox* showStudySubHeaderInfoGroupBox;
      
      /// show study subheader name info check box
      QCheckBox* showStudySubHeaderNameInfoCheckBox;
      
      /// show study subheader short name info check box
      QCheckBox* showStudySubHeaderShortNameInfoCheckBox;
      
      /// show study subheader task description info check box
      QCheckBox* showStudySubHeaderTaskDescriptionInfoCheckBox;
      
      /// show study subheader task baseline info check box
      QCheckBox* showStudySubHeaderTaskBaselineInfoCheckBox;
      
      /// show study subheader test attributes info check box
      QCheckBox* showStudySubHeaderTestAttributesInfoCheckBox;
      
      /// show study page reference info group box;
      QGroupBox* showStudyPageReferenceInfoGroupBox;
      
      /// show study page reference header info check box;
      QCheckBox* showStudyPageReferenceHeaderInfoCheckBox;
      
      /// show study page reference comment info check box;
      QCheckBox* showStudyPageReferenceCommentInfoCheckBox;
      
      /// show study page reference size units info check box;
      QCheckBox* showStudyPageReferenceSizeUnitsInfoCheckBox;
      
      /// show study page reference voxel size info check box;
      QCheckBox* showStudyPageReferenceVoxelSizeInfoCheckBox;
      
      /// show study page reference statistic info check box;
      QCheckBox* showStudyPageReferenceStatisticInfoCheckBox;
      
      /// show study page reference statistic description info check box;
      QCheckBox* showStudyPageReferenceStatisticDescriptionInfoCheckBox;
      
      /// show study page number info check box;
      QCheckBox* showStudyPageNumberInfoCheckBox;
      
};

#endif // __VE_GUI_IDENTIFY_DIALOG_H__


