
#ifndef __BRAIN_MODEL_SURFACE_BORDER_LANDMARK_IDENTIFICATION_H__
#define __BRAIN_MODEL_SURFACE_BORDER_LANDMARK_IDENTIFICATION_H__

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

#include "BrainModelAlgorithm.h"
#include "BrainModelSurfaceFindExtremum.h"
#include "BrainModelSurfaceROICreateBorderUsingMetricShape.h"
#include "StereotaxicSpace.h"
#include "Structure.h"

class AreaColorFile;
class BorderColorFile;
class BorderProjection;
class BorderProjectionFile;
class BrainModelSurface;
class BrainModelSurfaceROINodeSelection;
class FociColorFile;
class FociProjectionFile;
class MetricFile;
class PaintFile;
class SurfaceShapeFile;
class VocabularyFile;
class VolumeFile;

/// generate border landmarks for flattening and registration
class BrainModelSurfaceBorderLandmarkIdentification : public BrainModelAlgorithm {
   public:
      /// operations to perform
      enum OPERATION {
         OPERATION_ID_REGISTRATION_LANDMARKS  =  1,
         OPERATION_ID_FLATTENING_LANDMARKS    =  2,
         OPERATION_ID_ALL             = 0xffffffff
      };
      
      // constructor
      BrainModelSurfaceBorderLandmarkIdentification(BrainSet* bs,
                                         const StereotaxicSpace& stereotaxicSpaceIn,
                                         VolumeFile* anatomicalVolumeFileIn,
                                         const BrainModelSurface* fiducialSurfaceIn,
                                         const BrainModelSurface* inflatedSurfaceIn,
                                         const BrainModelSurface* veryInflatedSurfaceIn,
                                         const BrainModelSurface* ellipsoidSurfaceIn,
                                         const SurfaceShapeFile* depthSurfaceShapeFileIn,
                                         const int depthSurfaceShapeFileColumnNumberIn,
                                         PaintFile* paintFileInOut,
                                         const int paintFileGeographyColumnNumberIn,
                                         AreaColorFile* areaColorFileInOut,
                                         BorderProjectionFile* borderProjectionFileInOut,
                                         BorderColorFile* borderColorFileInOut,
                                         VocabularyFile* vocabularyFileInOut,
                                         const int operationSelectionMaskIn = OPERATION_ID_ALL);
                                         
      // destructor
      ~BrainModelSurfaceBorderLandmarkIdentification();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      // space supported for landmark identification?
      static bool isStereotaxicSpaceSupported(const StereotaxicSpace& stereotaxicSpaceIn);
      
      /// get probabilistic metric file of sulci locations
      const MetricFile* getMetricFile() const { return metricFile; }
      
      /// get foci projection file containing foci created during landmark determination
      const FociProjectionFile* getFociProjectionFile() const { return fociProjectionFile; }
      
      /// get foci color file containing colors for foci created during landmark creation
      const FociColorFile* getFociColorFile() const { return fociColorFile; }
      
      /// get supported stereotaxic spaces
      static void getSupportedStereotaxicSpaces(std::vector<StereotaxicSpace>& spacesOut);
      
      /// get the name of the medial wall for flattening
      static QString getFlattenMedialWallBorderName() { return "FLATTEN.HOLE.MedialWall"; }
      
      /// get the name prefix for flattening standard cuts
      static QString getFlattenStandardCutsBorderNamePrefix() { return "FLATTEN.CUT.Std."; }
      
      /// get the name of central sulcus registration landmark
      static QString getCentralSulcusRegistrationLandmarkName() { return "LANDMARK.CentralSulcus"; }
      
   protected:
      // identify the sulci
      void paintSulcalIdentification() throw (BrainModelAlgorithmException);
      
      // identify the central sulcus
      void identifyCentralSulcus() throw (BrainModelAlgorithmException);
      
      // identify the sylvian fissure
      void identifySylvianFissure() throw (BrainModelAlgorithmException);
      
      // identify the superior temporal gyrus
      void identifySuperiorTemporalGyrus() throw (BrainModelAlgorithmException);
      
      // identify the calcarine sulcus
      void identifyCalcarineSulcus() throw (BrainModelAlgorithmException);
      
      // extend the calcarine sulcus to the medial wall
      void extendCalcarineSulcusToMedialWall() throw (BrainModelAlgorithmException);
      
      // identify the medial wall
      void identifyMedialWall() throw (BrainModelAlgorithmException);
      
      // create medial wall dorsal and ventral borders
      void createMedialWallDorsalAndVentralLandmarks() throw (BrainModelAlgorithmException);
      
      // identify the dorsal medial wall NEW
      void identifyDorsalMedialWallNew() throw (BrainModelAlgorithmException);
      
      // identify the dorsal medial wall
      void identifyDorsalMedialWallOld() throw (BrainModelAlgorithmException);
      
      // identify the ventral medial wall
      void identifyVentralMedialWall() throw (BrainModelAlgorithmException);
      
      // identify the cuts
      void identifyCuts() throw (BrainModelAlgorithmException);
      
      // identify the cut calcarine
      void identifyCutCalcarine(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException);
      
      // identify the cut cingulate
      void identifyCutCingulate(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException);
      
      // identify the cut frontal
      void identifyCutFrontal(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException);
      
      // identify the cut sylvian
      void identifyCutSylvian(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException);
      
      // identify the cut temporal
      void identifyCutTemporal(const int medialWallCogNodeNumber) throw (BrainModelAlgorithmException);
      
      // generate surface curvatures
      void generateSurfaceCurvatures() throw (BrainModelAlgorithmException);
      
      // get node nearby that has the value closest to the target value
      int getNearbyNodeWithShapeValue(const BrainModelSurface* surface,
                                      const SurfaceShapeFile* shapeFile,
                                      const int shapeColumnNumber,
                                      const float targetValue,
                                      const int startNodeNumber,
                                      const float maxDistance,
                                      const BrainModelSurfaceROINodeSelection* limitToWithinROI = NULL,
                                      const float* limitToExtent = NULL) const;
                                        
      // create a file name
      QString createFileName(const QString& description,
                             const QString& extension) const;
       
      // add a focus at a border link
      void addFocusAtBorderLink(const BorderProjection* bp,
                                const int borderLinkNumber,
                                const QString& focusName);
                                
      // add a focus at a node
      void addFocusAtNode(const QString& focusName,
                          const int placeAtNodeNumber) throw (BrainModelAlgorithmException);
      
      // add a focus at a location
      void addFocusAtXYZ(const QString& focusName,
                         const float xyz[3]);
      
      // add a focus at a location
      void addFocusAtXYZ(const BrainModelSurface* focusSurface,
                         const QString& focusName,
                         const float xyz[3]);
      
      // add a focus at the node nearest the XYZ
      int addFocusAtNodeNearestXYZ(const BrainModelSurface* surface,
                                   const QString& focusName,
                                   const float xyz[3]);
       
      // add focus at extremum
      int  addFocusAtExtremum(const BrainModelSurface* extremumSurface,
                              const int startingNodeNumber,
                              const BrainModelSurfaceFindExtremum::DIRECTION searchDirection,
                              const float xMaximumMovement,
                              const float yMaximumMovement,
                              const float zMaximumMovement,
                              const QString& focusName,
                              BrainModelSurfaceROINodeSelection* roiAlongPath = NULL,
                              const BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION xr =
                                 BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                              const BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION yr =
                                 BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE,
                              const BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION zr =
                                 BrainModelSurfaceFindExtremum::NORMAL_RESTRICTION_NONE) throw (BrainModelAlgorithmException);
      
      /// add focus color
      void addFocusColor(const QString& colorName,
                         const unsigned char red,
                         const unsigned char green, 
                         const unsigned char blue);
                         
      /// border nibble within modes
      enum BORDER_NIBBLE_MODE_DISTANCE {
         BORDER_NIBBLE_MODE_DISTANCE_X,
         BORDER_NIBBLE_MODE_DISTANCE_Y,
         BORDER_NIBBLE_MODE_DISTANCE_Z,
         BORDER_NIBBLE_MODE_DISTANCE_LINEAR
      };
      
      // nibble border within distance
      void nibbleBorderWithinDistance(const BrainModelSurface* surface,
                        const QString& borderName,
                        const int nodeNumber,
                        const BORDER_NIBBLE_MODE_DISTANCE nibbleMode,
                        const float nibbleDistance) throw (BrainModelAlgorithmException);
                        
      // nibble border within distance
      void nibbleBorderWithinDistance(const BrainModelSurface* surface,
                        const QString& borderName,
                        const float nibbleFromHereXYZ[3],
                        const BORDER_NIBBLE_MODE_DISTANCE nibbleMode,
                        const float nibbleDistance) throw (BrainModelAlgorithmException);
                        
      // nibble border beyond distance
      void nibbleBorderBeyondDistance(const BrainModelSurface* surface,
                        const QString& borderName,
                        const float nibbleFromHereXYZ[3],
                        const BORDER_NIBBLE_MODE_DISTANCE nibbleMode,
                        const float nibbleDistance) throw (BrainModelAlgorithmException);
                        
      /// border nibble within modes
      enum BORDER_NIBBLE_MODE_OFFSET {
         BORDER_NIBBLE_MODE_OFFSET_GREATER_THAN_X,
         BORDER_NIBBLE_MODE_OFFSET_GREATER_THAN_Y,
         BORDER_NIBBLE_MODE_OFFSET_GREATER_THAN_Z,
         BORDER_NIBBLE_MODE_OFFSET_LESS_THAN_X,
         BORDER_NIBBLE_MODE_OFFSET_LESS_THAN_Y,
         BORDER_NIBBLE_MODE_OFFSET_LESS_THAN_Z
      };
         
      // nibble border with offset
      void nibbleBorderWithinOffset(const BrainModelSurface* surface,
                        const QString& borderName,
                        const float xyz[3],
                        const BORDER_NIBBLE_MODE_OFFSET nibbleMode,
                        const float nibbleOffset) throw (BrainModelAlgorithmException);
      
      // resample a border
      void resampleBorder(const BrainModelSurface* surface,
                          const QString& borderName,
                          const float samplingDistance,
                          const bool projectToTilesFlag = true) throw (BrainModelAlgorithmException);

      // remove loops from a border
      void removeLoopsFromBorder(const BrainModelSurface* surface,
                                 const QString& borderName,
                                 const char axisChar) throw (BrainModelAlgorithmException);

      // project the foci
      void projectFoci();
      
      // draw a border moving along the "most-lateral" nodes 
      /** DOES NOT WORK
      void drawBorderMostLateral(
                        const BrainModelSurface* borderSurface,
                        const BrainModelSurfaceROINodeSelection* roiIn,
                        const QString borderName,
                        const std::vector<int>& nodeNumbers,
                        const float samplingDistance) throw (BrainModelAlgorithmException);
      */
      
      // draw a border using geodesic method
      void drawBorderGeodesic(const BrainModelSurface* borderSurface,
                              const BrainModelSurfaceROINodeSelection* roi,
                              const QString borderName,
                              const int startNodeNumber,
                              const int endNodeNumber,
                              const float samplingDistance) throw (BrainModelAlgorithmException);
     
      // draw a border using geodesic method connecting a group of nodes
      void drawBorderGeodesic(const BrainModelSurface* borderSurface,
                              const BrainModelSurfaceROINodeSelection* roi,
                              const QString borderName,
                              const std::vector<int>& nodeNumbers,
                              const float samplingDistance) throw (BrainModelAlgorithmException);

      // draw a border using heuristic geodesic method connecting a group of nodes
      void drawBorderTargetedGeodesic(const BrainModelSurface* borderSurface,
                              const BrainModelSurfaceROINodeSelection* roi,
                              const QString borderName,
                              const std::vector<int>& nodeNumbers,
                              const float samplingDistance,
                              float target[3],
                              float targetweight) throw (BrainModelAlgorithmException);

      // draw a border using heuristic geodesic method connecting a group of nodes
      void drawBorderMetricGeodesic(const BrainModelSurface* borderSurface,
                              const BrainModelSurfaceROINodeSelection* roi,
                              const QString borderName,
                              const std::vector<int>& nodeNumbers,
                              const float samplingDistance,
                              MetricFile* nodeCost,
                              int metricColumn,
                              float metricWeight) throw (BrainModelAlgorithmException);

      // Find node along geodesic path between nodes
      int findNodeAlongGeodesicPathBetweenNodes(
                            const BrainModelSurface* surface,
                            const int startNodeNumber,
                            const int endNodeNumber,
                            const float distanceFromStartNode,
                            const BrainModelSurfaceROINodeSelection* roi = NULL) throw (BrainModelAlgorithmException);
                              
      // draw a border using metric method
      void drawBorderMetric(const BrainModelSurface* borderSurface,
                            const BrainModelSurfaceROICreateBorderUsingMetricShape::MODE drawMode,
                            const MetricFile* metricShapeFile,
                            const int metricShapeFileColumn,
                            const QString borderName,
                            const int startNodeNumber,
                            const int endNodeNumber,
                            const float samplingDistance,
                            const BrainModelSurfaceROINodeSelection* optionalROI = NULL) throw (BrainModelAlgorithmException);
                            
      // save an ROI to a file
      void saveRoiToFile(const BrainModelSurfaceROINodeSelection& roi,
                         const QString& roiFileName) throw (BrainModelAlgorithmException);
      
      // merge borders (returned border was added to border projection file)
      BorderProjection* mergeBorders(const QString& outputBorderName,
                                     const QString& border1Name,
                                     const QString& border2Name,
                                     const bool deleteInputBordersFlag,
                                     const bool closeBorderFlag,
                                     const BrainModelSurface* smoothingSurface,
                                     const int smoothingIterations,
                                     const int smoothingNeighbors) throw (BrainModelAlgorithmException);
      
      // merge borders (returned border was added to border projection file)
      BorderProjection* mergeBorders(const QString& outputBorderName,
                                     const std::vector<QString>& borderNames,
                                     const bool deleteInputBordersFlag,
                                     const bool closeBorderFlag,
                                     const BrainModelSurface* smoothingSurface,
                                     const int smoothingIterations,
                                     const int smoothingNeighbors) throw (BrainModelAlgorithmException);
      
      // get intersection of two borders (returns true if intersection valid)
      bool getBorderIntersection(const BrainModelSurface* borderSurface,
                                 const QString& border1Name,
                                 const QString& border2Name,
                                 const QString& intersectionFocusName,
                                 const float intersectionTolerance,
                                 float* itersectionXYZOut = NULL,
                                 int* border1IntersectionLinkNumberOut = NULL,
                                 int* border2IntersectionLinkNumberOut = NULL) throw (BrainModelAlgorithmException);
      
      // get intersection of two borders (returns true if intersection valid)
      bool getBorderIntersection(const BrainModelSurface* borderSurface,
                                 const QString& border1Name,
                                 const QString& border2Name,
                                 const QString& intersectionFocusName,
                                 const float intersectionToleranceMinimum,
                                 const float intersectionToleranceMaximum,
                                 const float intersectionToleranceStep,
                                 float itersectionXYZOut[3]) throw (BrainModelAlgorithmException);
      
      // create a fiducial surface that is approximately scaled to 711-2* space
      void createAndScaleFiducialSurface() throw (BrainModelAlgorithmException);
      
      // get scaling for stereotaxic space (returns true if supported space)
      static bool getScalingForStereotaxicSpace(const StereotaxicSpace& space,
                                           float scalingOutLeft[3],
                                           float scalingOutRight[3]);
                                         
      /// delete the debug files directory and the files within it
      void deleteDebugFilesDirectoryAndContents();
      
      /// get the node that is most lateral in the extent
      int getMostLateralNodeInExtent(const BrainModelSurface* surface,
                                     const float startXYZ[6],
                                     const float extent[6],
                                     const float maxGeodesicDistance) const;

      /// get the node that is most lateral in the extent
      int getClosestNodeInExtent(const BrainModelSurface* surface,
                                     const float startXYZ[6],
                                     const float extent[6],
                                     const float maxGeodesicDistance,
                                     const float target[3]) const;
      
      /// node type for heuristic search
      struct searchNode
      {
         float cost, heur;
         int node, prev;
      };

      /// TSC: yes, these methods are overloaded, so sue me
      /// connect nodes via graph search, favoring nodes closer to the target point
      Border drawHeuristic(const BrainModelSurface* borderSurface,
                           BrainModelSurfaceROINodeSelection* roi,
                           int startNodeNumber,
                           int endNodeNumber,
                           float target[],
                           float targetWeight) throw (BrainModelAlgorithmException);

      /// connect nodes via graph search, favoring nodes closer to the target point
      Border drawHeuristic(const BrainModelSurface* borderSurface,
                           BrainModelSurfaceROINodeSelection* roi,
                           int startNodeNumber,
                           int endNodeNumber,
                           MetricFile* nodeCost,
                           int metricColumn,
                           float metricWeight) throw (BrainModelAlgorithmException);

      /// stereotaxic space
      const StereotaxicSpace stereotaxicSpace;
      
      /// anatomical volume file
      VolumeFile* anatomicalVolumeFile;
      
      /// the input fiducial surface
      const BrainModelSurface* inputFiducialSurface;
      
      /// fiducial surface
      BrainModelSurface* fiducialSurface;
      
      /// inflated surface
      const BrainModelSurface* inflatedSurface;
      
      /// very inflated surface
      const BrainModelSurface* veryInflatedSurface;
      
      /// ellipsoid surface
      const BrainModelSurface* ellipsoidSurface;
      
      /// surface shape file containing depth information
      const SurfaceShapeFile* depthSurfaceShapeFile;
      
      /// surface shape file depth column number
      const int depthSurfaceShapeFileColumnNumber;      
      
      /// paint file 
      PaintFile* paintFile;
      
      /// column number of geography in paint file
      const int paintFileGeographyColumnNumber;
      
      /// area color file
      AreaColorFile* areaColorFile;

      /// border projection file
      BorderProjectionFile* borderProjectionFile;
      
      /// border color file 
      BorderColorFile* borderColorFile;
      
      /// probabilistic metric file
      MetricFile* metricFile;
      
      /// foci projection file
      FociProjectionFile* fociProjectionFile;
      
      /// foci color file
      FociColorFile* fociColorFile;
      
      /// vocabulary file
      VocabularyFile* vocabularyFile;
      
      /// curvature shape file
      SurfaceShapeFile* curvatureShapeFile;
      
      /// fiducial curvature shape column number
      int curvatureFiducialMeanColumnNumber;
      
      /// smoothed fiducial curvature shape column number
      int curvatureFiducialSmoothedMeanColumnNumber;
      
      /// inflated curvature shape column number
      int curvatureInflatedMeanColumnNumber;
      
      /// paint file's Sulcus ID column name
      QString paintFileSulcusIdColumnName;
      
      /// paint file's Sulcus ID column number
      int paintFileSulcusIdColumnNumber;
      
      /// inflated surface coordinate of central sulcus ventral tip
      float inflatedSurfaceCentralSulcusVentralTipXYZ[3];
      
      /// fiducial coordinate of the temporal pole
      float fiducialSurfaceTemporalPoleXYZ[3];
      
      /// inflated coordinate of the temporal pole
      float inflatedSurfaceTemporalPoleXYZ[3];
      
      /// node at temporal pole
      int temporalPoleNodeNumber;
      
      /// left hemisphere flag
      bool leftHemisphereFlag;
      
      /// the structure
      Structure::STRUCTURE_TYPE surfaceStructure;
      
      /// operation mask
      const int operationSelectionMask;
      
      /// node at anterior or calcarine sulcus
      int calcarineAnteriorNodeNumber;
      
      /// name of flatten frontal cut
      QString flattenCutFrontalName;
      
      /// name of calcarine cut
      QString flattenCutCalcarineName;
      
      /// name of dorsal section of medial wall
      QString medialWallDorsalSectionName;
      
      /// name of ventral part of medial wall
      QString medialWallVentralSectionName;
      
      /// medial node of central sulcus
      int cesMedialNodeNumber;
      
      /// calcarine posterior extreme node number
      int calcarinePosteriorExtremeNodeNumber;
      
      /// corpus callosum genu beginning node number
      int ccGenuBeginningNodeNumber;
      
      /// corpus callosum splenium limit node number
      int ccSpleniumLimitNodeNumber;
      
      /// corpus callosum splenium end node number
      int ccSpleniumEndNodeNumber;
      
      /// start node for medial wall
      int medialWallStartNodeNumber; 
      
      /// node at ventral/frontal end of sylvian fissure inferior branch
      int sfInferiorBranchBeginNodeNumber;
      
      /// node at sylvian fissure ventral/frontal region
      int sfVentralFrontalNodeNumber;
      
      /// name of landmark
      QString calcarineSulcusLandmarkName;
      
      /// name of border file for debugging
      QString borderDebugFileName;
      
      /// name of foci projection file for debugging
      QString fociProjectionDebugFileName;

      /// name of foci color file for debugging
      QString fociColorDebugFileName;

      /// surface scale factors for "rough" transformation to 711-2 space
      float surfaceSpaceScaling[3];      
      
      /// name of directory containing debug files
      QString debugFilesDirectoryName;
      
      /// save all intermediate files
      bool saveIntermediateFilesFlag;
      
      /// all landmarks were sucessfully created
      bool allLandmarksSuccessfulFlag;
};

#endif // __BRAIN_MODEL_SURFACE_BORDER_LANDMARK_IDENTIFICATION_H__

