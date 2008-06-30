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


#ifndef __GUI_MAIN_WINDOW_ATTRIBUTES_ACTIONS_H__
#define __GUI_MAIN_WINDOW_ATTRIBUTES_ACTIONS_H__

#include <QObject>

class GuiMainWindow;
class QAction;

/// the main windows attributes actions
class GuiMainWindowAttributesActions : public QObject {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMainWindowAttributesActions(GuiMainWindow* parent);
      
      /// Destructor
      ~GuiMainWindowAttributesActions();
      
      ///  for generating topology
      QAction* getGenerateTopographyAction() { return generateTopographyAction; }
      
      ///  for generating an areal estimation map
      QAction* getGenerateArealEstimationMapAction() { return generateArealEstimationMapAction; }
      
      ///  for area color editing
      QAction* getAreaColorsEditAction() { return areaColorsEditAction; }
      
      ///  for display palette editor
      QAction* getDisplayPaletteEditor() { return displayPaletteEditorAction; }
      
      ///  for display params file editor
      QAction* getDisplayParamsFileEditor() { return displayParamsFileEditorAction; }
      
      ///  for metric math 
      QAction* getMetricMathAction() { return metricMathAction; }
      
      ///  for metric to rgb paint
      QAction* getMetricToRgbPaintAction() { return metricToRgbPaintAction; }
      
      ///  for metric average and deviation
      QAction* getMetricAverageDeviationAction() { return metricAverageDeviationAction; }
      
      ///  for converting metric column to a functional volume
      QAction* getMetricToVolumeAction() { return metricToVolumeAction; }
      
      ///  for clearing all or part of the metric file
      QAction* getMetricClearAllOrPartAction() { return metricClearAllOrPartAction; }
      
      ///  for volume to surface mapper
      QAction* getVolumeToSurfaceMapperAction() { return volumeToSurfaceMapperAction; }
      
      ///  for modifying metric data
      QAction* getMetricModificationAction() { return metricModificationAction; }
      
      ///  for converting surface shape to a functional volume
      QAction* getSurfaceShapeToVolumeAction() { return surfaceShapeToVolumeAction; }
      
      ///  for clearing all or part of the surface shape file
      QAction* getSurfaceShapeClearAllOrPartAction() { return surfaceShapeClearAllOrPartAction; }
      
      /// compute average and deviation of surface shape
      QAction* getSurfaceShapeAverageDeviationAction() { return surfaceShapeAverageDeviationAction; }
      
      ///  for shape math dialog
      QAction* getShapeMathAction() { return shapeMathAction; }
      
      ///  for modifying shape data
      QAction* getShapeModificationAction() { return shapeModificationAction; }
      
      ///  for cleaning paint file names
      QAction* getPaintCleanNamesAction() { return paintCleanNamesAction; }
      
      ///  for generates names for paints
      QAction* getGenerateColorsForPaints() { return generateColorsForPaints; }
      
      ///  for converting paint to volume
      QAction* getPaintToVolumeAction() { return paintToVolumeAction; }
      
      ///  for editing paint names
      QAction* getPaintNamesEditAction() { return paintNamesEditAction; }
      
      ///  for assigning paints within displayed borders
      QAction* getPaintAssignWithinDisplayedBordersAction() { return paintAssignWithinDisplayedBordersAction; }
      
      ///  for clearing all or part of the paint file
      QAction* getPaintClearAllOrPartAction() { return paintClearAllOrPartAction; }
      
      ///  for clearing all or part of areal estimationo file
      QAction* getArealEstimationClearAllOrPartAction() { return arealEstimationClearAllOrPartAction; }
      
      ///  for copying current coloring to rgb paint
      QAction* getCopyColoringToRgbPaintAction() { return copyColoringToRgbPaintAction; }
      
      /// Called to copy surface normals to RGB Paint.
      QAction* getCopyNormalsToRgbPaintAction() { return copyNormalsToRgbPaintAction; }

      ///  for copying current coloring to a volume
      QAction* getCopyColoringToVolumeAction() { return copyColoringToVolumeAction; }
      
      /// clear all or part of a lat/long file
      QAction* getLatLonClearAllOrPartAction() { return latLonClearAllOrPartAction; }
      
      /// generate lat/long on a spherical surface
      QAction* getLatLonGenerateOnSphereAction() { return latLonGenerateOnSphereAction; }
      
      /// generate a deformation field
      QAction* getGenerateDeformationFieldAction() { return generateDeformationFieldAction; }
      
      /// convert prob atlas threshold coloring to paint
      QAction* getProbAtlasThresholdToPaintAction() { return probAtlasThresholdToPaintAction; }
      
      /// convert coord files to vectors
      QAction* getCoordsToVectorsAction() { return coordsToVectorsAction; }
      
      /// place normals in the vectors
      QAction* getNormalsToVectorsAction() { return normalsToVectorsAction; }
      
      /// get the models editor action
      QAction* getModelsEditorAction() { return modelsEditorAction; }

      /// get the study meta data editor action
      QAction* getStudyMetaDataEditorDialogAction() { return studyMetaDataEditorDialogAction; }
      
      /// get the vocabulary file editor action
      QAction* getVocabularyFileEditorDialogAction() { return vocabularyFileEditorDialogAction; }

      /// get the vocabulary study info to study metadata file action
      QAction* getVocabularyMoveStudyInfoToStudyMetaDataAction() { return vocabularyMoveStudyInfoToStudyMetaDataAction; }
      
   public slots:
      /// update the actions (typically called when menu is about to show)
      void updateActions();
   
   private slots:
      /// slot for generating topology
      void slotGenerateTopography();
      
      /// slot for generating an areal estimation map
      void slotGenerateArealEstimationMap();
      
      /// slot for area color editing
      void slotAreaColorsEdit();
      
      /// slot called when area colors are changed in color editor
      void slotAreaColorsChanged();
      
      /// slot for metric to rgb paint
      void slotMetricToRgbPaint();
      
      /// slot for metric average and deviation
      void slotMetricAverageDeviation();
      
      /// slot for converting metric column to a functional volume
      void slotMetricToVolume();
      
      /// slot for clearing all or part of the metric file
      void slotMetricClearAllOrPart();
      
      /// slot for volume to surface mapper
      void slotVolumeToSurfaceMapper();
      
      /// slot for volume to surface mapper complete
      void slotVolumeToSurfaceMapperComplete();
      
      /// slot for modifying metric data
      void slotMetricModification();
      
      /// slot for converting surface shape to a functional volume
      void slotSurfaceShapeToVolume();
      
      /// slot for clearing all or part of the surface shape file
      void slotSurfaceShapeClearAllOrPart();
      
      /// compute average and deviation of surface shape
      void slotSurfaceShapeAverageDeviation();
      
      /// slot for modifying shape data
      void slotShapeModification();
      
      /// slot for cleaning paint file names
      void slotPaintCleanNames();
      
      /// slot for generating colors for non-matching paint names
      void slotGenerateColorsForPaints();
      
      /// slot for converting paint to volume
      void slotPaintToVolume();
      
      /// slot for assigning paints within displayed borders
      void slotPaintAssignWithinDisplayedBorders();
      
      /// slot for clearing all or part of the paint file
      void slotPaintClearAllOrPart();
      
      /// slot for clearing all or part of the areal estimation file
      void slotArealEstimationClearAllOrPart();
      
      /// slot for copying current coloring to rgb paint
      void slotCopyColoringToRgbPaint();
      
      /// Called to copy surface normals to RGB Paint.
      void slotCopyNormalsToRgbPaint();

      /// slot for copying current coloring to a volume
      void slotCopyColoringToVolume();
      
      /// clear all or part of a lat/long file
      void slotLatLonClearAllOrPart();
      
      /// generate lat/long on a spherical surface
      void slotLatLonGenerateOnSphere();
      
      /// generate a deformation field
      void slotGenerateDeformationField();
      
      /// convert prob atlas threshold coloring to paint
      void slotProbAtlasThresholdToPaint();
      
      /// convert coord files to vectors
      void slotCoordsToVectors();
      
      /// convert normals to vectors
      void slotNormalsToVectors();
      
      /// convert vocabulary study info to study metadata file
      void slotVocabularyMoveStudyInfoToStudyMetaData();
      
   protected:
      ///  for generating topology
      QAction* generateTopographyAction;
      
      ///  for generating an areal estimation map
      QAction* generateArealEstimationMapAction;
      
      ///  for area color editing
      QAction* areaColorsEditAction;
      
      ///  for display palette editor
      QAction* displayPaletteEditorAction;
      
      ///  for display params file editor
      QAction* displayParamsFileEditorAction;
      
      ///  for metric math 
      QAction* metricMathAction;
      
      ///  for metric to rgb paint
      QAction* metricToRgbPaintAction;
      
      ///  for metric average and deviation
      QAction* metricAverageDeviationAction;
      
      ///  for converting metric column to a functional volume
      QAction* metricToVolumeAction;
      
      ///  for clearing all or part of the metric file
      QAction* metricClearAllOrPartAction;
      
      ///  for volume to surface mapper
      QAction* volumeToSurfaceMapperAction;
      
      ///  for modifying metric data
      QAction* metricModificationAction;
      
      ///  for converting surface shape to a functional volume
      QAction* surfaceShapeToVolumeAction;
      
      ///  for clearing all or part of the surface shape file
      QAction* surfaceShapeClearAllOrPartAction;
      
      /// compute average and deviation of surface shape
      QAction* surfaceShapeAverageDeviationAction;
      
      ///  for shape math dialog
      QAction* shapeMathAction;
      
      ///  for modifying shape data
      QAction* shapeModificationAction;
      
      ///  for cleaning paint file names
      QAction* paintCleanNamesAction;
      
      ///  for generate colors for paints
      QAction* generateColorsForPaints;
      
      ///  for converting paint to volume
      QAction* paintToVolumeAction;
      
      ///  for editing paint names
      QAction* paintNamesEditAction;
      
      ///  for assigning paints within displayed borders
      QAction* paintAssignWithinDisplayedBordersAction;
      
      ///  for clearing all or part of the paint file
      QAction* paintClearAllOrPartAction;
      
      /// for clearing all or part of areal estimation file
      QAction* arealEstimationClearAllOrPartAction;
      
      ///  for copying current coloring to rgb paint
      QAction* copyColoringToRgbPaintAction;
      
      /// Called to copy surface normals to RGB Paint.
      QAction* copyNormalsToRgbPaintAction;

      ///  for copying current coloring to a volume
      QAction* copyColoringToVolumeAction;
      
      /// clear all or part of a lat/long file
      QAction* latLonClearAllOrPartAction;
      
      /// generate lat/long on a spherical surface
      QAction* latLonGenerateOnSphereAction;
      
      /// generate a deformation field
      QAction* generateDeformationFieldAction;
      
      /// convert prob atlas threshold coloring to paint
      QAction* probAtlasThresholdToPaintAction;
      
      /// convert coord files to vectors
      QAction* coordsToVectorsAction;
      
      /// convert normals to vectors action
      QAction* normalsToVectorsAction;
      
      /// models editor action
      QAction* modelsEditorAction;
      
      /// study meta data action
      QAction* studyMetaDataEditorDialogAction;
      
      /// vocabulary file editor action
      QAction* vocabularyFileEditorDialogAction;
      
      /// vocabulary study info to study metadata action
      QAction* vocabularyMoveStudyInfoToStudyMetaDataAction;
};

#endif  // __GUI_MAIN_WINDOW_ATTRIBUTES_ACTIONS_H__

