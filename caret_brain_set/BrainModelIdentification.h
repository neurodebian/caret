
#ifndef __BRAIN_MODEL_IDENTIFICATION_H__
#define __BRAIN_MODEL_IDENTIFICATION_H__

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

#include <QObject>

#include "SceneFile.h"
#include "StudyMetaDataFile.h"

class BrainModelOpenGL;
class BrainModelSurface;
class BrainModelVolume;
class BrainSet;
class StudyMetaDataLink;
class StudyMetaDataLinkSet;
class VolumeFile;

/// class for providing identification information
class BrainModelIdentification : public QObject {
   public:
      // constructor
      BrainModelIdentification(BrainSet* brainSetIn,
                               QObject* parent = 0);
      
      // destructor
      ~BrainModelIdentification();
      
      // get the identification text for all
      QString getIdentificationText(BrainModelOpenGL* openGL,
                                    const bool enableHtml,
                                    const bool enableVocabularyLinksIn);
                                    
      // get the identification text for node only
      QString getIdentificationTextForNode(BrainModelOpenGL* openGL,
                                           const bool enableHtml,
                                           const bool enableVocabularyLinksIn);
                                    
      // get the identification text for node by its number
      QString getIdentificationTextForNode(BrainSet* brainSet,
                                           const int NodeNumber,
                                           const bool enableHtml,
                                           const bool enableVocabularyLinksIn);
                                    
      // get the identification text for cell only
      QString getIdentificationTextForCell(BrainModelOpenGL* openGL,
                                           const bool enableHtml,
                                           const bool enableVocabularyLinksIn);
                                    
      // get the identification text for focus only
      QString getIdentificationTextForFocus(BrainModelOpenGL* openGL,
                                           const bool enableHtml,
                                           const bool enableVocabularyLinksIn);
                                    
      // get the identification text for border only
      QString getIdentificationTextForBorder(BrainModelOpenGL* openGL,
                                           const bool enableHtml,
                                           const bool enableVocabularyLinksIn);

      // get the identification text for studies
      QString getIdentificationTextForStudies(const bool enableHtml,
                                            const StudyMetaDataFile* smdf,
                                            const StudyMetaDataLinkSet& smdls);
      
      // get the identification text for a study
      QString getIdentificationTextForStudy(const bool enableHtml,
                                            const StudyMetaData* smd,
                                            const int studyIndex,
                                            const StudyMetaDataLink* smdl = NULL);
      
      // get the identification text for vocabulary
      QString getIdentificationTextForVocabulary(const bool enableHtml,
                                                 const QString& vocabularyName);
                                                 
      /// find out if border information should be displayed
      bool getDisplayBorderInformation() const { return idFilter.displayBorderInformation; }
      
      /// find out if cell information should be displayed
      bool getDisplayCellInformation() const { return idFilter.displayCellInformation; }
      
      /// find out if voxel information should be displayed
      bool getDisplayVoxelInformation() const { return idFilter.displayVoxelInformation; }
      
      /// find out if contour information should be displayed
      bool getDisplayContourInformation() const { return idFilter.displayContourInformation; }
      
      /// find out if node information should be displayed
      bool getDisplayNodeInformation() const { return idFilter.displayNodeInformation; }
      
      /// find out if node coordinate information should be displayed
      bool getDisplayNodeCoordInformation() const { return idFilter.displayNodeCoordInformation; }
      
      /// find out if node lat/lon information should be displayed
      bool getDisplayNodeLatLonInformation() const { return idFilter.displayNodeLatLonInformation;}
      
      /// find out if node paint information should be displayed
      bool getDisplayNodePaintInformation() const { return idFilter.displayNodePaintInformation; }
      
      /// find out if node prob atlas information should be displayed
      bool getDisplayNodeProbAtlasInformation() const { return idFilter.displayNodeProbAtlasInformation; }
      
      /// find out if node rgb paint information should be displayed
      bool getDisplayNodeRgbPaintInformation() const { return idFilter.displayNodeRgbPaintInformation; }
      
      /// find out if node metric information should be displayed
      bool getDisplayNodeMetricInformation() const { return idFilter.displayNodeMetricInformation; }
      
      /// find out if node surface shape information should be displayed
      bool getDisplayNodeShapeInformation() const { return idFilter.displayNodeShapeInformation; }
      
      /// find out if node metric information should be displayed
      bool getDisplayNodeSectionInformation() const { return idFilter.displayNodeSectionInformation; }
      
      /// find out if node areal est information should be displayed
      bool getDisplayNodeArealEstInformation() const { return idFilter.displayNodeArealEstInformation; }
      
      /// find out if node topography information should be displayed
      bool getDisplayNodeTopographyInformation() const { return idFilter.displayNodeTopographyInformation; }
      
      /// find out if foci information should be displayed
      bool getDisplayFociInformation() const { return idFilter.displayFociInformation; }
      
      /// foci name information should be displayed
      bool getDisplayFociNameInformation() const { return idFilter.displayFociNameInformation; }
      
      /// foci class information should be displayed
      bool getDisplayFociClassInformation() const { return idFilter.displayFociClassInformation; }
      
      /// foci original stereotaxic position information should be displayed
      bool getDisplayFociOriginalStereotaxicPositionInformation() const { return idFilter.displayFociOriginalStereotaxicPositionInformation; }
      
      /// foci stereotaxic position information should be displayed
      bool getDisplayFociStereotaxicPositionInformation() const { return idFilter.displayFociStereotaxicPositionInformation; }
      
      /// foci area information should be displayed
      bool getDisplayFociAreaInformation() const { return idFilter.displayFociAreaInformation; }
      
      /// foci geography information should be displayed
      bool getDisplayFociGeographyInformation() const { return idFilter.displayFociGeographyInformation; }
      
      /// foci size information should be displayed
      bool getDisplayFociSizeInformation() const { return idFilter.displayFociSizeInformation; }
      
      /// foci statistic information should be displayed
      bool getDisplayFociStatisticInformation() const { return idFilter.displayFociStatisticInformation; }
      
      /// foci comment information should be displayed
      bool getDisplayFociCommentInformation() const { return idFilter.displayFociCommentInformation; }
            
      /// find out if study information should be displayed
      bool getDisplayStudyInformation() const { return idFilter.displayStudyInformation; }
      
      /// study title information should be displayed
      bool getDisplayStudyTitleInformation() const { return idFilter.displayStudyTitleInformation; }
      
      /// study authors information should be displayed
      bool getDisplayStudyAuthorsInformation() const { return idFilter.displayStudyAuthorsInformation; }
      
      /// study citation information should be displayed
      bool getDisplayStudyCitationInformation() const { return idFilter.displayStudyCitationInformation; }
      
      /// study comment information should be displayed
      bool getDisplayStudyCommentInformation() const { return idFilter.displayStudyCommentInformation; }
      
      /// study data format information should be displayed
      bool getDisplayStudyDataFormatInformation() const { return idFilter.displayStudyDataFormatInformation; }
      
      /// study data type information should be displayed
      bool getDisplayStudyDataTypeInformation() const { return idFilter.displayStudyDataTypeInformation; }
      
      /// study DOI information should be displayed
      bool getDisplayStudyDOIInformation() const { return idFilter.displayStudyDOIInformation; }
      
      /// study keywords information should be displayed
      bool getDisplayStudyKeywordsInformation() const { return idFilter.displayStudyKeywordsInformation; }
      
      /// study medical subject headings information should be displayed
      bool getDisplayStudyMedicalSubjectHeadingsInformation() const { return idFilter.displayStudyMedicalSubjectHeadingsInformation; }

      /// study meta-analysis information should be displayed
      bool getDisplayStudyMetaAnalysisInformation() const { return idFilter.displayStudyMetaAnalysisInformation; }
      
      /// study meta-analysis name information should be displayed
      bool getDisplayStudyMetaAnalysisNameInformation() const { return idFilter.displayStudyMetaAnalysisNameInformation; }
      
      /// study meta-analysis title information should be displayed
      bool getDisplayStudyMetaAnalysisTitleInformation() const { return idFilter.displayStudyMetaAnalysisTitleInformation; }
      
      /// study meta-analysis authors information should be displayed
      bool getDisplayStudyMetaAnalysisAuthorsInformation() const { return idFilter.displayStudyMetaAnalysisAuthorsInformation; }
      
      /// study meta-analysis citation information should be displayed
      bool getDisplayStudyMetaAnalysisCitationInformation() const { return idFilter.displayStudyMetaAnalysisCitationInformation; }
      
      /// study meta-analysis DOI/URL information should be displayed
      bool getDisplayStudyMetaAnalysisDoiUrlInformation() const { return idFilter.displayStudyMetaAnalysisDoiUrlInformation; }
      
      /// study name should be displayed
      bool getDisplayStudyNameInformation() const { return idFilter.displayStudyNameInformation; }
      
      /// study part scheme abbreviation information should be displayed
      bool getDisplayStudyPartSchemeAbbrevInformation() const { return idFilter.displayStudyPartSchemeAbbrevInformation; }
      
      /// study part scheme full name information should be displayed
      bool getDisplayStudyPartSchemeFullInformation() const { return idFilter.displayStudyPartSchemeFullInformation; }
      
      /// study PubMed ID information should be displayed
      bool getDisplayStudyPubMedIDInformation() const { return idFilter.displayStudyPubMedIDInformation; }
      
      /// study Project ID information should be displayed
      bool getDisplayStudyProjectIDInformation() const { return idFilter.displayStudyProjectIDInformation; }
      
      /// study stereotaxic space information should be displayed
      bool getDisplayStudyStereotaxicSpaceInformation() const { return idFilter.displayStudyStereotaxicSpaceInformation; }
      
      /// study stereotaxic space details information should be displayed
      bool getDisplayStudyStereotaxicSpaceDetailsInformation() const { return idFilter.displayStudyStereotaxicSpaceDetailsInformation; }
      
      /// study URL information should be displayed
      bool getDisplayStudyURLInformation() const { return idFilter.displayStudyURLInformation; }
      
      /// study table information should be displayed
      bool getDisplayStudyTableInformation() const { return idFilter.displayStudyTableInformation; }
      
      /// study table header information should be displayed
      bool getDisplayStudyTableHeaderInformation() const { return idFilter.displayStudyTableHeaderInformation; }
      
      /// study table footer information should be displayed
      bool getDisplayStudyTableFooterInformation() const { return idFilter.displayStudyTableFooterInformation; }
      
      /// study table size units information should be displayed
      bool getDisplayStudyTableSizeUnitsInformation() const { return idFilter.displayStudyTableSizeUnitsInformation; }
      
      /// study table voxel size information should be displayed
      bool getDisplayStudyTableVoxelSizeInformation() const { return idFilter.displayStudyTableVoxelSizeInformation; }
      
      /// study table statistic information should be displayed
      bool getDisplayStudyTableStatisticInformation() const { return idFilter.displayStudyTableStatisticInformation; }
      
      /// study table statistic description information should be displayed
      bool getDisplayStudyTableStatisticDescriptionInformation() const { return idFilter.displayStudyTableStatisticDescriptionInformation; }
      
      /// study figure information should be displayed
      bool getDisplayStudyFigureInformation() const { return idFilter.displayStudyFigureInformation; }
      
      /// study figure legend information should be displayed
      bool getDisplayStudyFigureLegendInformation() const { return idFilter.displayStudyFigureLegendInformation; }
      
      /// study figure panel information should be displayed
      bool getDisplayStudyFigurePanelInformation() const { return idFilter.displayStudyFigurePanelInformation; }
      
      /// study figure panel description information should be displayed
      bool getDisplayStudyFigurePanelDescriptionInformation() const { return idFilter.displayStudyFigurePanelDescriptionInformation; }
      
      /// study figure panel task description information should be displayed
      bool getDisplayStudyFigurePanelTaskDescriptionInformation() const { return idFilter.displayStudyFigurePanelTaskDescriptionInformation; }
      
      /// study figure panel task baseline information should be displayed
      bool getDisplayStudyFigurePanelTaskBaselineInformation() const { return idFilter.displayStudyFigurePanelTaskBaselineInformation; }
      
      /// study figure panel test attributesinformation should be displayed
      bool getDisplayStudyFigurePanelTestAttributesInformation() const { return idFilter.displayStudyFigurePanelTestAttributesInformation; }
      
      /// study sub header information should be displayed
      bool getDisplayStudySubHeaderInformation() const { return idFilter.displayStudySubHeaderInformation; }
      
      /// study sub header name information should be displayed
      bool getDisplayStudySubHeaderNameInformation() const { return idFilter.displayStudySubHeaderNameInformation; }
      
      /// study sub header short name information should be displayed
      bool getDisplayStudySubHeaderShortNameInformation() const { return idFilter.displayStudySubHeaderShortNameInformation; }
      
      /// study sub header task description information should be displayed
      bool getDisplayStudySubHeaderTaskDescriptionInformation() const { return idFilter.displayStudySubHeaderTaskDescriptionInformation; }
      
      /// study sub header task baseline information should be displayed
      bool getDisplayStudySubHeaderTaskBaselineInformation() const { return idFilter.displayStudySubHeaderTaskBaselineInformation; }
      
      /// study sub header test attributes information should be displayed
      bool getDisplayStudySubHeaderTestAttributesInformation() const { return idFilter.displayStudySubHeaderTestAttributesInformation; }
            
      /// study page reference information should be displayed
      bool getDisplayStudyPageReferenceInformation() const { return idFilter.displayStudyPageReferenceInformation; }
      
      /// study page reference header information should be displayed
      bool getDisplayStudyPageReferenceHeaderInformation() const { return idFilter.displayStudyPageReferenceHeaderInformation; }
      
      /// study page reference comment information should be displayed
      bool getDisplayStudyPageReferenceCommentInformation() const { return idFilter.displayStudyPageReferenceCommentInformation; }
      
      /// study page reference size units information should be displayed
      bool getDisplayStudyPageReferenceSizeUnitsInformation() const { return idFilter.displayStudyPageReferenceSizeUnitsInformation; }
      
      /// study page reference voxel size information should be displayed
      bool getDisplayStudyPageReferenceVoxelSizeInformation() const { return idFilter.displayStudyPageReferenceVoxelSizeInformation; }
      
      /// study page reference statistic information should be displayed
      bool getDisplayStudyPageReferenceStatisticInformation() const { return idFilter.displayStudyPageReferenceStatisticInformation; }
      
      /// study page reference statistic description information should be displayed
      bool getDisplayStudyPageReferenceStatisticDescriptionInformation() const { return idFilter.displayStudyPageReferenceStatisticDescriptionInformation; }
            
      /// study page number information should be displayed
      bool getDisplayStudyPageNumberInformation() const { return idFilter.displayStudyPageNumberInformation; }
            
      /// find out if green symbols should be displayed on surface
      bool getDisplayIDSymbol() const { return displayIDSymbol; }
      
      /// get significant digits for floating point numbers
      int getSignificantDigits() const { return significantDigits; }
      
      // apply a scene (update dialog)
      void showScene(const SceneFile::Scene& scene,
                     QString& errorMessage);
      
      // create a scene (save dialog settings)
      void saveScene(SceneFile::Scene& scene);
  
   public slots:
      /// set border information should be displayed
      void setDisplayBorderInformation(const bool displayIt) { idFilter.displayBorderInformation = displayIt; }
      
      /// set cell information should be displayed
      void setDisplayCellInformation(const bool displayIt) { idFilter.displayCellInformation = displayIt; }
      
      /// set voxel information should be displayed
      void setDisplayVoxelInformation(const bool displayIt) { idFilter.displayVoxelInformation = displayIt; }
      
      /// set contour information should be displayed
      void setDisplayContourInformation(const bool displayIt) { idFilter.displayContourInformation = displayIt; }
      
      /// set node information should be displayed
      void setDisplayNodeInformation(const bool displayIt) { idFilter.displayNodeInformation = displayIt; }
      
      /// set node coordinate information should be displayed
      void setDisplayNodeCoordInformation(const bool displayIt) { idFilter.displayNodeCoordInformation = displayIt; }
      
      /// set node lat/lon information should be displayed
      void setDisplayNodeLatLonInformation(const bool displayIt) { idFilter.displayNodeLatLonInformation = displayIt;}
      
      /// set node paint information should be displayed
      void setDisplayNodePaintInformation(const bool displayIt) { idFilter.displayNodePaintInformation = displayIt; }
      
      /// set node prob atlas information should be displayed
      void setDisplayNodeProbAtlasInformation(const bool displayIt) { idFilter.displayNodeProbAtlasInformation = displayIt; }
      
      /// set node rgb paint information should be displayed
      void setDisplayNodeRgbPaintInformation(const bool displayIt) { idFilter.displayNodeRgbPaintInformation = displayIt; }
      
      /// set node metric information should be displayed
      void setDisplayNodeMetricInformation(const bool displayIt) { idFilter.displayNodeMetricInformation = displayIt; }
      
      /// set node surface shape information should be displayed
      void setDisplayNodeShapeInformation(const bool displayIt) { idFilter.displayNodeShapeInformation = displayIt; }
      
      /// set node metric information should be displayed
      void setDisplayNodeSectionInformation(const bool displayIt) { idFilter.displayNodeSectionInformation = displayIt; }
      
      /// set node areal est information should be displayed
      void setDisplayNodeArealEstInformation(const bool displayIt) { idFilter.displayNodeArealEstInformation = displayIt; }
      
      /// set node topography information should be displayed
      void setDisplayNodeTopographyInformation(const bool displayIt) { idFilter.displayNodeTopographyInformation = displayIt; }
      
      /// set foci information should be displayed
      void setDisplayFociInformation(const bool displayIt) { idFilter.displayFociInformation = displayIt; }
      
      /// foci name information should be displayed
      void setDisplayFociNameInformation(const bool displayIt) { idFilter.displayFociNameInformation = displayIt; }
      
      /// foci class information should be displayed
      void setDisplayFociClassInformation(const bool displayIt) { idFilter.displayFociClassInformation = displayIt; }
      
      /// foci original stereotaxic position information should be displayed
      void setDisplayFociOriginalStereotaxicPositionInformation(const bool displayIt) { idFilter.displayFociOriginalStereotaxicPositionInformation = displayIt; }
      
      /// foci stereotaxic position information should be displayed
      void setDisplayFociStereotaxicPositionInformation(const bool displayIt) { idFilter.displayFociStereotaxicPositionInformation = displayIt; }
      
      /// foci area information should be displayed
      void setDisplayFociAreaInformation(const bool displayIt) { idFilter.displayFociAreaInformation = displayIt; }
      
      /// foci geography information should be displayed
      void setDisplayFociGeographyInformation(const bool displayIt) { idFilter.displayFociGeographyInformation = displayIt; }
      
      /// foci size information should be displayed
      void setDisplayFociSizeInformation(const bool displayIt) { idFilter.displayFociSizeInformation = displayIt; }
      
      /// foci statistic information should be displayed
      void setDisplayFociStatisticInformation(const bool displayIt) { idFilter.displayFociStatisticInformation = displayIt; }
      
      /// foci comment information should be displayed
      void setDisplayFociCommentInformation(const bool displayIt) { idFilter.displayFociCommentInformation = displayIt; }
            
      /// set study information should be displayed
      void setDisplayStudyInformation(const bool displayIt) { idFilter.displayStudyInformation = displayIt; }
      
      /// study title information should be displayed
      void setDisplayStudyTitleInformation(const bool displayIt) { idFilter.displayStudyTitleInformation = displayIt; }
      
      /// study authors information should be displayed
      void setDisplayStudyAuthorsInformation(const bool displayIt) { idFilter.displayStudyAuthorsInformation = displayIt; }
      
      /// study citation information should be displayed
      void setDisplayStudyCitationInformation(const bool displayIt) { idFilter.displayStudyCitationInformation = displayIt; }
      
      /// study comment information should be displayed
      void setDisplayStudyCommentInformation(const bool displayIt) { idFilter.displayStudyCommentInformation = displayIt; }
      
      /// study data format information should be displayed
      void setDisplayStudyDataFormatInformation(const bool displayIt) { idFilter.displayStudyDataFormatInformation = displayIt; }
      
      /// study data type information should be displayed
      void setDisplayStudyDataTypeInformation(const bool displayIt) { idFilter.displayStudyDataTypeInformation = displayIt; }
      
      /// study DOI information should be displayed
      void setDisplayStudyDOIInformation(const bool displayIt) { idFilter.displayStudyDOIInformation = displayIt; }
      
      /// study keywords information should be displayed
      void setDisplayStudyKeywordsInformation(const bool displayIt) { idFilter.displayStudyKeywordsInformation = displayIt; }
      
      /// study medical subject headings should be displayed
      void setDisplayStudyMedicalSubjectHeadingsInformation(const bool displayIt) { idFilter.displayStudyMedicalSubjectHeadingsInformation = displayIt; }
      
      /// study meta-analysis should be displayed
      void setDisplayStudyMetaAnalysisInformation(const bool displayIt) { idFilter.displayStudyMetaAnalysisInformation = displayIt; }
      
      /// study meta-analysis name should be displayed
      void setDisplayStudyMetaAnalysisNameInformation(const bool displayIt) { idFilter.displayStudyMetaAnalysisNameInformation = displayIt; }
      
      /// study meta-analysis title should be displayed
      void setDisplayStudyMetaAnalysisTitleInformation(const bool displayIt) { idFilter.displayStudyMetaAnalysisTitleInformation = displayIt; }
      
      /// study meta-analysis authors should be displayed
      void setDisplayStudyMetaAnalysisAuthorsInformation(const bool displayIt) { idFilter.displayStudyMetaAnalysisAuthorsInformation = displayIt; }
      
      /// study meta-analysis citation should be displayed
      void setDisplayStudyMetaAnalysisCitationInformation(const bool displayIt) { idFilter.displayStudyMetaAnalysisCitationInformation = displayIt; }
      
      /// study meta-analysis DOI/URL should be displayed
      void setDisplayStudyMetaAnalysisDoiUrlInformation(const bool displayIt) { idFilter.displayStudyMetaAnalysisDoiUrlInformation = displayIt; }
      
      /// study name should be displayed
      void setDisplayStudyNameInformation(const bool displayIt) { idFilter.displayStudyNameInformation = displayIt; }
      
      /// study part scheme abbreviation information should be displayed
      void setDisplayStudyPartSchemeAbbrevInformation(const bool displayIt) { idFilter.displayStudyPartSchemeAbbrevInformation = displayIt; }
      
      /// study part scheme full name information should be displayed
      void setDisplayStudyPartSchemeFullInformation(const bool displayIt) { idFilter.displayStudyPartSchemeFullInformation = displayIt; }
      
      /// study PubMed ID information should be displayed
      void setDisplayStudyPubMedIDInformation(const bool displayIt) { idFilter.displayStudyPubMedIDInformation = displayIt; }
      
      /// study Project ID information should be displayed
      void setDisplayStudyProjectIDInformation(const bool displayIt) { idFilter.displayStudyProjectIDInformation = displayIt; }
      
      /// study stereotaxic space information should be displayed
      void setDisplayStudyStereotaxicSpaceInformation(const bool displayIt) { idFilter.displayStudyStereotaxicSpaceInformation = displayIt; }
      
      /// study stereotaxic space details information should be displayed
      void setDisplayStudyStereotaxicSpaceDetailsInformation(const bool displayIt) { idFilter.displayStudyStereotaxicSpaceDetailsInformation = displayIt; }
      
      /// study URL information should be displayed
      void setDisplayStudyURLInformation(const bool displayIt) { idFilter.displayStudyURLInformation = displayIt; }
      
      /// study table information should be displayed
      void setDisplayStudyTableInformation(const bool displayIt) { idFilter.displayStudyTableInformation = displayIt; }
      
      /// study table header information should be displayed
      void setDisplayStudyTableHeaderInformation(const bool displayIt) { idFilter.displayStudyTableHeaderInformation = displayIt; }
      
      /// study table footer information should be displayed
      void setDisplayStudyTableFooterInformation(const bool displayIt) { idFilter.displayStudyTableFooterInformation = displayIt; }
      
      /// study table size units information should be displayed
      void setDisplayStudyTableSizeUnitsInformation(const bool displayIt) { idFilter.displayStudyTableSizeUnitsInformation = displayIt; }
      
      /// study table voxel size information should be displayed
      void setDisplayStudyTableVoxelSizeInformation(const bool displayIt) { idFilter.displayStudyTableVoxelSizeInformation = displayIt; }
      
      /// study table statistic information should be displayed
      void setDisplayStudyTableStatisticInformation(const bool displayIt) { idFilter.displayStudyTableStatisticInformation = displayIt; }
      
      /// study table statistic description information should be displayed
      void setDisplayStudyTableStatisticDescriptionInformation(const bool displayIt) { idFilter.displayStudyTableStatisticDescriptionInformation = displayIt; }
      
      /// study figure information should be displayed
      void setDisplayStudyFigureInformation(const bool displayIt) { idFilter.displayStudyFigureInformation = displayIt; }
      
      /// study figure legend information should be displayed
      void setDisplayStudyFigureLegendInformation(const bool displayIt) { idFilter.displayStudyFigureLegendInformation = displayIt; }
      
      /// study figure panel information should be displayed
      void setDisplayStudyFigurePanelInformation(const bool displayIt) { idFilter.displayStudyFigurePanelInformation = displayIt; }
      
      /// study figure panel description information should be displayed
      void setDisplayStudyFigurePanelDescriptionInformation(const bool displayIt) { idFilter.displayStudyFigurePanelDescriptionInformation = displayIt; }
      
      /// study figure panel task description information should be displayed
      void setDisplayStudyFigurePanelTaskDescriptionInformation(const bool displayIt) { idFilter.displayStudyFigurePanelTaskDescriptionInformation = displayIt; }
      
      /// study figure panel task baseline information should be displayed
      void setDisplayStudyFigurePanelTaskBaselineInformation(const bool displayIt) { idFilter.displayStudyFigurePanelTaskBaselineInformation = displayIt; }
      
      /// study figure panel test attributes information should be displayed
      void setDisplayStudyFigurePanelTestAttributesInformation(const bool displayIt) { idFilter.displayStudyFigurePanelTestAttributesInformation = displayIt; }

      /// study subh header information should be displayed      
      void setDisplayStudySubHeaderInformation(const bool displayIt) { idFilter.displayStudySubHeaderInformation = displayIt; }

      /// study sub header name information should be displayed
      void setDisplayStudySubHeaderNameInformation(const bool displayIt) { idFilter.displayStudySubHeaderNameInformation = displayIt; }
      
      /// study sub header short name information should be displayed
      void setDisplayStudySubHeaderShortNameInformation(const bool displayIt) { idFilter.displayStudySubHeaderShortNameInformation = displayIt; }
      
      /// study sub header task description information should be displayed
      void setDisplayStudySubHeaderTaskDescriptionInformation(const bool displayIt) { idFilter.displayStudySubHeaderTaskDescriptionInformation = displayIt; }
      
      /// study sub header task baseline information should be displayed
      void setDisplayStudySubHeaderTaskBaselineInformation(const bool displayIt) { idFilter.displayStudySubHeaderTaskBaselineInformation = displayIt; }
      
      /// study sub header test attributes information should be displayed
      void setDisplayStudySubHeaderTestAttributesInformation(const bool displayIt) { idFilter.displayStudySubHeaderTestAttributesInformation = displayIt; }
            
      /// study page reference information should be displayed
      void setDisplayStudyPageReferenceInformation(const bool displayIt) { idFilter.displayStudyPageReferenceInformation = displayIt; }
      
      /// study page reference header information should be displayed
      void setDisplayStudyPageReferenceHeaderInformation(const bool displayIt) { idFilter.displayStudyPageReferenceHeaderInformation = displayIt; }
      
      /// study page reference comment information should be displayed
      void setDisplayStudyPageReferenceCommentInformation(const bool displayIt) { idFilter.displayStudyPageReferenceCommentInformation = displayIt; }
      
      /// study page reference size units information should be displayed
      void setDisplayStudyPageReferenceSizeUnitsInformation(const bool displayIt) { idFilter.displayStudyPageReferenceSizeUnitsInformation = displayIt; }
      
      /// study page reference voxel size information should be displayed
      void setDisplayStudyPageReferenceVoxelSizeInformation(const bool displayIt) { idFilter.displayStudyPageReferenceVoxelSizeInformation = displayIt; }
      
      /// study page reference statistic information should be displayed
      void setDisplayStudyPageReferenceStatisticInformation(const bool displayIt) { idFilter.displayStudyPageReferenceStatisticInformation = displayIt; }
      
      /// study page reference statistic description information should be displayed
      void setDisplayStudyPageReferenceStatisticDescriptionInformation(const bool displayIt) { idFilter.displayStudyPageReferenceStatisticDescriptionInformation = displayIt; }
            
      /// study page number information should be displayed
      void setDisplayStudyPageNumberInformation(const bool displayIt) { idFilter.displayStudyPageNumberInformation = displayIt; }
            
      /// set green symbols should be displayed on surface
      void setDisplayIDSymbol(const bool displayIt) { displayIDSymbol = displayIt; }
      
      /// get significant digits for floating point numbers
      void setSignificantDigits(const int num);
      
      // all identification off (if any on, all turned off, otherwise all off) 
      void toggleAllIdentificationOnOff();
      
   protected:
      /// class for identification filtering
      class IdFilter {
         public:
            // constructor
            IdFilter();
            
            // destructor
            ~IdFilter();
            
            // turn all off
            void allOff();
            
            // turn all on
            void allOn();
            
            // toggle all on/off
            void toggleAllOnOff();
            
            /// report if any node information should be displayed
            bool anyNodeDataOn() const;
            
            /// border information should be displayed
            bool displayBorderInformation;
            
            /// cell information should be displayed
            bool displayCellInformation;
            
            /// voxel information should be displayed
            bool displayVoxelInformation;
            
            /// contour information should be displayed
            bool displayContourInformation;
            
            /// node information should be displayed
            bool displayNodeInformation;
            
            /// node coordinate information should be displayed
            bool displayNodeCoordInformation;
            
            /// node lat/lon information should be displayed
            bool displayNodeLatLonInformation;
            
            /// node paint information should be displayed
            bool displayNodePaintInformation;
            
            /// node prob atlas information should be displayed
            bool displayNodeProbAtlasInformation;
            
            /// node rgb paint information should be displayed
            bool displayNodeRgbPaintInformation;
            
            /// node metric information should be displayed
            bool displayNodeMetricInformation;
            
            /// node surface shape information should be displayed
            bool displayNodeShapeInformation;
            
            /// node metric information should be displayed
            bool displayNodeSectionInformation;
            
            /// node areal est information should be displayed
            bool displayNodeArealEstInformation;
            
            /// node topography information should be displayed
            bool displayNodeTopographyInformation;
            
            /// study information should be displayed
            bool displayStudyInformation;
            
            /// study title information should be displayed
            bool displayStudyTitleInformation;
            
            /// study authors information should be displayed
            bool displayStudyAuthorsInformation;
            
            /// study citation information should be displayed
            bool displayStudyCitationInformation;
            
            /// study comment information should be displayed
            bool displayStudyCommentInformation;
            
            /// study data format information should be displayed
            bool displayStudyDataFormatInformation;
            
            /// study data type information should be displayed
            bool displayStudyDataTypeInformation;
            
            /// study DOI information should be displayed
            bool displayStudyDOIInformation;
            
            /// study keywords information should be displayed
            bool displayStudyKeywordsInformation;
            
            /// study medical subject headings should be displayed
            bool displayStudyMedicalSubjectHeadingsInformation;
            
            /// study meta-analysis should be displayed
            bool displayStudyMetaAnalysisInformation;
            
            /// study meta-analysis name should be displayed
            bool displayStudyMetaAnalysisNameInformation;
            
            /// study meta-analysis title should be displayed
            bool displayStudyMetaAnalysisTitleInformation;
            
            /// study meta-analysis authors should be displayed
            bool displayStudyMetaAnalysisAuthorsInformation;
            
            /// study meta-analysis citation should be displayed
            bool displayStudyMetaAnalysisCitationInformation;
            
            /// study meta-analysis DOI/URL should be displayed
            bool displayStudyMetaAnalysisDoiUrlInformation;
            
            /// study name should be displayed
            bool displayStudyNameInformation;
            
            /// study part scheme abbreviation information should be displayed
            bool displayStudyPartSchemeAbbrevInformation;
            
            /// study part scheme full name information should be displayed
            bool displayStudyPartSchemeFullInformation;
            
            /// study PubMed ID information should be displayed
            bool displayStudyPubMedIDInformation;
            
            /// study Project ID information should be displayed
            bool displayStudyProjectIDInformation;
            
            /// study stereotaxic space information should be displayed
            bool displayStudyStereotaxicSpaceInformation;
            
            /// study stereotaxic space details information should be displayed
            bool displayStudyStereotaxicSpaceDetailsInformation;
            
            /// study URL information should be displayed
            bool displayStudyURLInformation;
            
            /// study table information should be displayed
            bool displayStudyTableInformation;
            
            /// study table header information should be displayed
            bool displayStudyTableHeaderInformation;
            
            /// study table footer information should be displayed
            bool displayStudyTableFooterInformation;
            
            /// study table size units information should be displayed
            bool displayStudyTableSizeUnitsInformation;
            
            /// study table voxel size information should be displayed
            bool displayStudyTableVoxelSizeInformation;
            
            /// study table statistic information should be displayed
            bool displayStudyTableStatisticInformation;
            
            /// study table statistic description information should be displayed
            bool displayStudyTableStatisticDescriptionInformation;
            
            /// study figure information should be displayed
            bool displayStudyFigureInformation;
            
            /// study figure legend information should be displayed
            bool displayStudyFigureLegendInformation;
            
            /// study figure panel information should be displayed
            bool displayStudyFigurePanelInformation;
            
            /// study figure panel description information should be displayed
            bool displayStudyFigurePanelDescriptionInformation;
            
            /// study figure panel task description information should be displayed
            bool displayStudyFigurePanelTaskDescriptionInformation;
            
            /// study figure panel task baseline information should be displayed
            bool displayStudyFigurePanelTaskBaselineInformation;
            
            /// study figure panel test attributes information should be displayed
            bool displayStudyFigurePanelTestAttributesInformation;
            
            /// study sub header information should be displayed
            bool displayStudySubHeaderInformation;
            
            /// study sub header name information should be displayed
            bool displayStudySubHeaderNameInformation;
            
            /// study sub header short name information should be displayed
            bool displayStudySubHeaderShortNameInformation;
            
            /// study sub header task description information should be displayed
            bool displayStudySubHeaderTaskDescriptionInformation;
            
            /// study sub header task baseline information should be displayed
            bool displayStudySubHeaderTaskBaselineInformation;
            
            /// study sub header test attributes information should be displayed
            bool displayStudySubHeaderTestAttributesInformation;
            
            /// study page reference information should be displayed
            bool displayStudyPageReferenceInformation;
            
            /// study page reference header information should be displayed
            bool displayStudyPageReferenceHeaderInformation;
            
            /// study page reference comment information should be displayed
            bool displayStudyPageReferenceCommentInformation;
            
            /// study page reference size units information should be displayed
            bool displayStudyPageReferenceSizeUnitsInformation;
            
            /// study page reference voxel size information should be displayed
            bool displayStudyPageReferenceVoxelSizeInformation;
            
            /// study page reference statistic information should be displayed
            bool displayStudyPageReferenceStatisticInformation;
            
            /// study page reference statistic description information should be displayed
            bool displayStudyPageReferenceStatisticDescriptionInformation;
            
            /// study page number information should be displayed
            bool displayStudyPageNumberInformation;
            
            /// foci information should be displayed
            bool displayFociInformation;
            
            /// report if any foci information should be displayed
            bool anyFociDataOn() const;
            
            /// report if any study meta-analysis information should be displayed
            bool anyStudyMetaAnalysisDataOn() const;
            
            /// report if any study information should be displayed
            bool anyStudyDataOn() const;
            
            /// report if any study table information should be displayed
            bool anyStudyTableDataOn() const;

            /// report if any study figure panel information should be displayed
            bool anyStudyFigurePanelDataOn() const;
            
            /// report if any study figure information should be displayed
            bool anyStudyFigureDataOn() const;
            
            /// report if any sub header information should be displayed
            bool anySubHeaderDataOn() const;
            
            /// report if any page reference information should be displayed
            bool anyPageReferenceDataOn() const;
            
            /// foci name information should be displayed
            bool displayFociNameInformation;
            
            /// foci class information should be displayed
            bool displayFociClassInformation;
            
            /// foci original stereotaxic position information should be displayed
            bool displayFociOriginalStereotaxicPositionInformation;
            
            /// foci stereotaxic position information should be displayed
            bool displayFociStereotaxicPositionInformation;
            
            /// foci area information should be displayed
            bool displayFociAreaInformation;
            
            /// foci geography information should be displayed
            bool displayFociGeographyInformation;
            
            /// foci size information should be displayed
            bool displayFociSizeInformation;
            
            /// foci statistic information should be displayed
            bool displayFociStatisticInformation;
            
            /// foci comment information should be displayed
            bool displayFociCommentInformation;
            
      };
            
      // get identification text for node
      QString getIdentificationTextForNode();
      
      // get identification text for node
      QString getIdentificationTextForNode(const int nodeNumber,
                                           const int windowNumber,
                                           BrainSet* brainSet,
                                           BrainModelSurface* bms);
      
      // get the identification text for a meta-analysis studies attached to the input study
      QString getIdentificationTextForMetaAnalysisStudies(const StudyMetaData* smd);
      
      // get the identification text for a meta-analysis study
      QString getIdentificationTextForMetaAnalysisStudy(const QString& pubMedID);
      
      // get the identification text for studies
      QString getIdentificationTextForStudies(const StudyMetaDataFile* smdf,
                                              const StudyMetaDataLinkSet& smdls,
                                              const bool showMetaAnalysisFlag);
                                              
      // get the identification text for a study
      QString getIdentificationTextForStudy(const StudyMetaData* smd,
                                            const int studyIndex,
                                            const StudyMetaDataLink* smdl = NULL);
      
      // setup the tags for html or text
      void setupHtmlOrTextTags(const bool doHTML);
      
      // get identification text for surface border
      QString getIdentificationTextForSurfaceBorder();
      
      // get identification text for volume border
      QString getIdentificationTextForVolumeBorder();
      
      // get identification text for cell
      QString getIdentificationTextForCellProjection();
      
      // get identification text for foci
      QString getIdentificationTextForFoci();
      
      // get identification text for voxel
      QString getIdentificationTextForVoxel();
      
      // get identification text for volume cell
      QString getIdentificationTextForVolumeCell();
      
      // get identification text for volume foci
      QString getIdentificationTextForVolumeFoci();
      
      // get identification text for transform cell
      QString getIdentificationTextForTransformCell();
      
      // get identification text for transform contour cell
      QString getIdentificationTextForTransformContourCell();
      
      // get identification text for transform foci
      QString getIdentificationTextForTransformFoci();
      
      // get identification text for palette
      QString getIdentificationTextForPalette(const bool metricFlag);
      
      // get identification text for voxel cloud functional
      QString getIdentificationTextForVoxelCloudFunctional();
      
      // get identification text for VTK model
      QString getIdentificationTextForVtkModel();
      
      // get identification text for Contour
      QString getIdentificationTextForContour();
      
      // get identification text for contour cell
      QString getIdentificationTextForContourCell();

      // Get ID string for study meta data sub header
      QString getStudyMetaDataSubHeaderIdentificationText(
                                     const StudyMetaData::SubHeader* subHeader,
                                     const QString& namePrefix) const;
      
      // get identification text for a volume file
      QString getVolumeFileIdentificationText(BrainSet* brainSet,
                                              BrainModelVolume* bmv,
                                              VolumeFile* selectionVolume,
                                              const int vi,
                                              const int vj,
                                              const int vk);
      
      // get the name of a window
      QString getWindowName(const int windowNumber) const;
      
      // make name a link to vocabulary file if it matches a vocabulary file entry
      QString linkToVocabulary(BrainSet* brainSet,
                               const QString& name);
      
      /// translate special HTML characters to HTML special characters
      QString htmlTranslate(const QString& ss) const;
      
      /// current identification filter
      IdFilter idFilter;
      
      /// OpenGL Drawing class
      BrainModelOpenGL* openGL;
      
      /// bold start tag
      QString tagBoldStart;
      
      /// bold end tag
      QString tagBoldEnd;
      
      /// newline tag
      QString tagNewLine;
      
      /// indentation
      QString tagIndentation;
      
      /// brain set with which this identification is associated
      BrainSet* brainSetParent;
            
      /// green symbols should be displayed on surface (not used in all on/off)
      bool displayIDSymbol;
      
      /// number of significant digits for floating point number display
      int significantDigits;
      
      /// enable links to vocabulary in html
      bool enableVocabularyLinks;
      
      /// enable HTML output
      bool htmlFlag;
};

#endif // __BRAIN_MODEL_IDENTIFICATION_H__
