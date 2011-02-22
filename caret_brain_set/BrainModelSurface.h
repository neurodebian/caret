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



#ifndef __VE_SURFACE_H__
#define __VE_SURFACE_H__

#include "vtkPolyData.h"

#include "BrainModel.h"
#include "CoordinateFile.h"
#include "Structure.h"

class BorderFile;
class BorderProjection;
class BrainModelSurfaceROINodeSelection;
class BrainVoyagerFile;
class CellProjectionFile;
class DeformationFieldFile;
class LatLonFile;
class MetricFile;
class MniObjSurfaceFile;
class PaintFile;
class RgbPaintFile;
class SurfaceShapeFile;
class TopologyFile;
class VectorFile;

/// BrainModelSurface stores geometry and topology for a brain surface
/**
 * Stores the geometry and topology for a surface and methods for
 * operations on the surface.
 */
class BrainModelSurface : public BrainModel {
   public:
      /// Types of surfaces
      enum SURFACE_TYPES {
         SURFACE_TYPE_RAW,
         SURFACE_TYPE_FIDUCIAL,
         SURFACE_TYPE_INFLATED,
         SURFACE_TYPE_VERY_INFLATED,
         SURFACE_TYPE_SPHERICAL,
         SURFACE_TYPE_ELLIPSOIDAL,
         SURFACE_TYPE_COMPRESSED_MEDIAL_WALL,
         SURFACE_TYPE_FLAT,
         SURFACE_TYPE_FLAT_LOBAR,
         SURFACE_TYPE_HULL,
         SURFACE_TYPE_UNKNOWN,
         SURFACE_TYPE_UNSPECIFIED
      };
      
      /// Plane for modifying coordinates
      enum COORDINATE_PLANE {
         COORDINATE_PLANE_NONE,
         COORDINATE_PLANE_MOVE_POSITIVE_X_TO_ZERO,
         COORDINATE_PLANE_MOVE_NEGATIVE_X_TO_ZERO,
         COORDINATE_PLANE_MOVE_POSITIVE_Y_TO_ZERO,
         COORDINATE_PLANE_MOVE_NEGATIVE_Y_TO_ZERO,
         COORDINATE_PLANE_MOVE_POSITIVE_Z_TO_ZERO,
         COORDINATE_PLANE_MOVE_NEGATIVE_Z_TO_ZERO,
         COORDINATE_PLANE_RESTORE
      };
      
      /// Constructor
      BrainModelSurface(BrainSet* bs, 
               const BrainModel::BRAIN_MODEL_TYPE bmt = BrainModel::BRAIN_MODEL_SURFACE);
      
      /// Copy constructor
      BrainModelSurface(const BrainModelSurface& bms);
      
      /// Destructor
      virtual ~BrainModelSurface();

      /// add a node to the surface
      void addNode(const float xyz[3]);
      
      /// Align to standard orientation (flat or spherical)
      void alignToStandardOrientation(const BrainModelSurface* fiducialSurface,
                                      const BorderProjection* centralSulcusBorderProjection,
                                      const bool generateSphericalLatitudeLongitude,
                                      const bool scaleToFiducialArea);
      
      /// Align to standard orientation (flat or spherical)
      void alignToStandardOrientation(const int ventralTipCentralSulcusNode, 
                                      const int dorsalMedialTipCentralSulcusNode,
                                      const bool generateSphericalLatitudeLongitude,
                                      const bool scaleToFiducialArea);
      
      /// Apply a transformation matrix to the surface
      void applyTransformationMatrix(TransformationMatrix& tm);
      
      /// apply a view (transformation) to the coordinates of the surface
      void applyViewToCoordinates(const BrainModel::STANDARD_VIEWS surfaceView);
      
      /// Apply current view to surface
      void applyCurrentView(const int surfaceViewNumber,
                            const bool applyTranslation,
                            const bool applyRotation,
                            const bool applyScaling);
      
      /// OLD Apply current view to surface that uses OpenGL
      void OLDapplyCurrentView(const int surfaceViewNumber,
                            const bool applyTranslation,
                            const bool applyRotation,
                            const bool applyScaling);

      /// smooth the surface by moving nodes along their normals multiplied by curvature.
      void smoothSurfaceUsingCurvature(const float strength,
                                       const int numSteps,
                                       const float curvatureMaximum);
                                               
      /// expand the surface by moving nodes along their normals by the specified amount
      void expandSurface(const float expandAmount);
      
      /// get the number of nodes in the surface
      int getNumberOfNodes() const { return coordinates.getNumberOfCoordinates(); }
      
      /// get information about the surface
      void getSurfaceInformation(std::vector<QString>& labels,
                                 std::vector<QString>& values) const;
                                 
      /// get the name of the coordinate file
      QString getFileName() const { return coordinates.getFileName(); }
      
      /// Get a descriptive name of the model
      virtual QString getDescriptiveName() const;
      
      /// append a string to the coordinate file's comment
      void appendToCoordinateFileComment(const QString& s);
      
      /// append a string to the topology file's comment
      void appendToTopologyFileComment(const QString& s);
      
      /// get node closest to point
      int getNodeClosestToPoint(const float xyz[3]) const;
      
      /// get access to coordinate file for this surface
      CoordinateFile* getCoordinateFile() { return &coordinates; }
      
      /// get access to coordinate file for this surface (const method)
      const CoordinateFile* getCoordinateFile() const { return &coordinates; }
      
      /// create latitude longitude for this surface
      void createLatitudeLongitude(LatLonFile* llf, 
                                   const int columnNumber,
                                   const QString& columnNameIn,
                                   const bool setDeformedLatLonFlag,
                                   const bool sphereInDorsalViewFlag);
      
      /// create spherical lat/lon borders for this surface
      void createSphericalLatLonBorders(BorderFile& bf,
                                        const bool sphereInDorsalViewFlag);
      
      /// create flat grid borders for this surface
      void createFlatGridBorders(BorderFile& bf, const float gridSpacing,
                                 const int pointSpacing);
      
      /// create flat grid borders for analysis
      void createFlatGridBordersForAnalysis(BorderFile& bf,
                                            const float bounds[4],
                                            const float resolution);
                                            
      /// Create deformation field vectors for an ATLAS surface.
      void createDeformationField(const BrainModelSurface* indivSourceSurface,
                                          const BrainModelSurface* indivDeformSurface,
                                          const int columnNumberIn,
                                          const QString& columnName,
                                          DeformationFieldFile& dff) const;
                                          
      /// Create deformation field vectors for a surface and its deformed surface.
      void createDeformationField(const BrainModelSurface* deformedSurface,
                                  const int columnNumber,
                                  const QString& columnName,
                                  DeformationFieldFile& dff) const;
                                          
      /// compute normals
      void computeNormals(const float* coordsIn = NULL);
      
      /// copy normals to vector file
      void copyNormalsToVectorFile(VectorFile* vf) const;
                                          
      /// convert to a sphere with the specified area
      void convertToSphereWithSurfaceArea(const float desiredSphereArea = 0.0);
      
      /// convert to a sphere with the specified radius
      void convertToSphereWithRadius(const float radius, 
                                     const int startNodeIndexIn = -1,
                                     const int endNodeIndexIn   = -1);
      
      /// convert an elliptical surface to a sphere (if area is zero the ellipsoid area will be used)
      void convertEllipsoidToSphereWithSurfaceArea(const float desiredSphereArea = 0);
      
      /// convert a sphere to a compressed medial wall surface
      void convertSphereToCompressedMedialWall(const float compressionFactor = 0.5);
      
      /// convert a sphere to a flat surface
      void convertSphereToFlat();
      
      /// convert to an ellipsoid surface
      void convertToEllipsoid();
      
      /// Create the inflated and ellipsoid surfaces starting from a fiducial surface.
      void createInflatedAndEllipsoidFromFiducial(const bool createInflated,
                                                  const bool createVeryInflated,
                                                  const bool createEllipsoid,
                                                  const bool createSphere,
                                                  const bool createCompressedMedialWall,
                                                  const bool enableFingerSmoothing,
                                                  const bool scaleToMatchFiducialArea,
                                                  const float iterationsScale,
                                                  MetricFile* metricMeasurementsFile) const;
      
      /// convert "this" surface to VTK PolyData
      vtkPolyData* convertToVtkPolyData() const;

      /// copy the coordinates from the VTK PolyData to this surface
      void copyCoordinatesFromVTK(vtkPolyData* polyData);
      
      /// copy the topology from the VTK PolyData to this surface
      void copyTopologyFromVTK(vtkPolyData* polyData);
      
      /// Rotate a surface so that the node is on the positive Z-axis (facing user).
      void orientNodeToPositiveScreenZ(const int nodeNumber, 
                                       const int surfaceViewNumber);

      /// compress the front face of a surface.
      void compressFrontFace(const float compressionFactor,
                             const int surfaceViewNumber);
      
      /// convert normals to rgb paint
      void convertNormalsToRgbPaint(RgbPaintFile* rpf);
      
      /// get normal for a point
      const float* getNormal(const int coordinateNumber) const {
         return &normals[coordinateNumber * 3];
      }
      
      /// set a normal
      void setNormal(const int coordinateNumber,
                     const float normalVector[3]);
                     
      /// get the radius of a spherical surface (assumes spherical surface with center at origin)
      float getSphericalSurfaceRadius() const;

      /// perform a crossover check (returns number of crossover tiles
      void crossoverCheck(int& numberOfTileCrossoversOut,
                          int& numberOfNodeCrossoversOut,
                          const SURFACE_TYPES surfaceTypeHint = SURFACE_TYPE_UNKNOWN);
      
      /// surefit ellipsoid crossover check
      int crossoverCheckSureFitEllipsoid();
      
      /// get the bounds of the connected surface
      void getBounds(float bounds[6]) const;
      
      /// get the display list for this brain model
      unsigned int getDisplayListNumber();
      
      /// set the display list for this brain model 
      void setDisplayListNumber(unsigned int num);
      
      /// import from a brain voyager file
      void importFromBrainVoyagerFile(const BrainVoyagerFile& bvf) throw (FileException);
      
      /// import from a MNI OBJ Surface File
      void importFromMniObjSurfaceFile(const MniObjSurfaceFile& mni) throw (FileException);
      
      /// import from a VTK surface file
      void importFromVtkFile(vtkPolyData* polyData,
                             const QString& fileName) throw (FileException);
      
      /// allocate and initialize the normals
      void initializeNormals(const int numCoordsIn = -1);
      
      /// move disconnected nodes to origin
      void moveDisconnectedNodesToOrigin();
      
      /// reset the surface
      virtual void reset();
      
      /// get access to the topology for this surface
      TopologyFile* getTopologyFile() const { return topology; }
      
      /// see if the surface is topologically correct (has no handles)
      bool isTopologicallyCorrect() const;
      
      /// read the specified coordinate file
      void readCoordinateFile(const QString& filename) throw(FileException);
      
      /// read the specified surface file
      void readSurfaceFile(const QString& filename) throw(FileException);
      
      /// write the surface file
      void writeSurfaceFile(const QString& filename,
                            const AbstractFile::FILE_FORMAT fileFormat = AbstractFile::FILE_FORMAT_XML) throw (FileException);
      
      /// write the file's memory in caret6 format to the specified name
      QString writeSurfaceInCaret6Format(const QString& filenameIn,
                                         const QString& prependToFileNameExtension,
                                         Structure structure,
                                         const bool useCaret6ExtensionFlag) throw (FileException);

      /// get the structure
      Structure getStructure() const { return structure; }
      
      /// set the structure
      void setStructure(const Structure::STRUCTURE_TYPE st);
      
      /// set the structure
      void setStructure(const Structure s);
      
      /// see if surface is a fiducial surface
      bool getIsFiducialSurface() const;
      
      /// see if surface is a flat surface
      bool getIsFlatSurface() const;
      
      /// get the surface type
      SURFACE_TYPES getSurfaceType() const { return surfaceType; }
      
      /// get the surface type's name
      QString getSurfaceTypeName() const;
      
      /// get the surface type from a surface configuration ID
      static SURFACE_TYPES getSurfaceTypeFromConfigurationID(const QString& name);
      
      /// get the configuration ID from the surface type
      static QString getSurfaceConfigurationIDFromType(const SURFACE_TYPES st);

      /// Get the spec file tag from the coord type.
      static QString getCoordSpecFileTagFromSurfaceType(const SURFACE_TYPES st);

      /// Get the spec file tag from the surface type.
      static QString getSurfaceSpecFileTagFromSurfaceType(const SURFACE_TYPES st);

      /// Get all surface types and names
      static void getSurfaceTypesAndNames(std::vector<SURFACE_TYPES>& typesOut,
                                          std::vector<QString>& typeNamesOut);
                                          
      /// flip the normals
      void flipNormals();
      
      /// orient the normals so that they point out of the surface (returns true if flipped)
      bool orientNormalsOut();

      /// orient the tiles consistently
      void orientTilesConsistently();
      
      /// orient all tiles so that each tile's normal points outward (flat and sphere only)
      void orientTilesOutward(const SURFACE_TYPES st);
      
      /// get the area of the surface
      float getSurfaceArea(const TopologyFile* tfin = NULL) const;
      
      // get the volume displacment of the surface
      float getSurfaceVolumeDisplacement() const;
      
      /// Get the area of a tile in the surface
      float getTileArea(const int tileNum) const;
      
      ///  Get the area of a tile in the surface using three nodes
      float getTileArea(const int n1, const int n2, const int n3) const;    
            
      /// get the area of all tiles
      void getAreaOfAllTiles(std::vector<float>& tileAreas) const;
      
      /// get the area of all nodes
      void getAreaOfAllNodes(std::vector<float>& nodeAreas) const;
      
      /// get the mean distance between nodes
      float getMeanDistanceBetweenNodes(BrainModelSurfaceROINodeSelection* surfaceROI = NULL) const;
      
      /// push (save) the coordinates
      void pushCoordinates();
      
      /// pop (restore) the coordinates (must have done a push prior to this call)
      void popCoordinates();
      
      /// project the coordinates to a plane 
      void projectCoordinatesToPlane(const COORDINATE_PLANE plane);
      
      /// Scale a surface to the specified area.
      void scaleSurfaceToArea(const float desiredArea,
                              const bool flatRatioFlag);

      /// set the surface type
      void setSurfaceType(const SURFACE_TYPES st);
      
      /// set the topology file for this surface (returns true if topology has more nodes than coord)
      bool setTopologyFile(TopologyFile* topologyIn);
      
      /// get the rotation matrix
      //vtkTransform* getRotationTransformMatrix(const int surfaceViewNumber) { 
      //   return rotationMatrix[surfaceViewNumber]; 
      //}
      
      /// get the surface's scaling
      //float getScaling(const int surfaceViewNumber) const { 
      //   return scaling[surfaceViewNumber]; 
      //}
      
      /// set the surface's scaling
      //void setScaling(const int surfaceViewNumber, const float scaleIn) { 
      //   scaling[surfaceViewNumber] = scaleIn; 
      //}
      
      /// get the default scale
      float getDefaultScaling() const { return defaultScaling; }
      
      /// set the default scaling
      void setDefaultScaling(const float scale);
      
      /// set the default scale from orthographic projection
      void setDefaultScaling(const double orthoRight,
                             const double orthoTop);
           
      /// update the default scaling
      void updateForDefaultScaling();
      
      /// set to a standard view
      virtual void setToStandardView(const int surfaceViewNumber, const STANDARD_VIEWS view);
          
      /// apply surface shape to a surface (typically a flat or spherical surface)
      void applyShapeToSurface(const SurfaceShapeFile& ssf,
                               const int shapeColumn,
                               const float shapeMultiplier);
                               
      /// inflate the surface
      void inflate(const int smoothingIterations,
                   const int inflationIterations,
                   const float inflationFactor);
                   
      /// Inflate surface and smooth fingers
      void inflateSurfaceAndSmoothFingers(const BrainModelSurface* fiducialSurfaceIn,
                                          const int numberSmoothingCycles,
                                          const float regularSmoothingStrength,
                                          const int regularSmoothingIterations,
                                          const float inflationFactor,
                                          const float compressStretchThreshold,
                                          const float fingerSmoothingStrength,
                                          const int fingerSmoothingIterations,
                                          MetricFile* metricMeasurementsFile);
      
      /// translate a surface to its center of mass
      void translateToCenterOfMass();
      
      /// get the center of mass for a surface
      void getCenterOfMass(float centerOfMass[3]) const;

      /// orient nodes with paint to be on negative Z axis 
      /// (returns true if matching nodes were NOT found)
      bool orientPaintedNodesToNegativeZAxis(const PaintFile* paintFile,
                                             const std::vector<QString> paintNames,
                                             const int paintColumn,
                                             QString& errorMessage);
          
      /// orient a sphere so that the point is placed on the negative Z axis
      void orientPointToNegativeZAxis(const float p[3]);
      
      /// orient a sphere so that the point is placed on the positive Z axis
      void orientPointToPositiveZAxis(const float p[3]);
      
      /// linearly smooth the surface
      void linearSmoothing(const float strength, const int iterations,
                           const int smoothEdgesEveryXIterations,
                           const std::vector<bool>* smoothOnlyTheseNodes = NULL,
                           const int projectToSphereEveryXIterations = -1);
                              
      /// linearly smooth the surface
      void arealSmoothing(const float strength, const int iterations,
                          const int smoothEdgesEveryXIterations,
                          const std::vector<bool>* smoothOnlyTheseNodes = NULL,
                          const int projectToSphereEveryXIterations = -1);
      
      /// landmark constrained areal smoothing
      void landmarkConstrainedSmoothing(const float strength, 
                                        const int iterations, 
                                        const std::vector<bool>& landmarkNodeFlag,
                                        const int projectToSphereEveryXIterations);

      /// Performed landmark neighbor constrained smoothing.  
      void landmarkNeighborConstrainedSmoothing(const float strength, 
                                                const int iterations, 
                                                const std::vector<bool>& landmarkNodeFlag,
                                                const int smoothNeighborsEveryX,
                                                const int projectToSphereEveryXIterations);
      
      /// (try to) smooth out flat surface overlap 
      bool smoothOutFlatSurfaceOverlap(const float strength = 0.5,
                                       const int numberOfCycles = 5,
                                       const int iterationsPerCycle = 50,
                                       const int smoothEdgesEveryXIterations = 10,
                                       const int neighborDepth = 10);
      
      /// Smooth out crossovers in a surface
      void smoothOutSurfaceCrossovers(const float strength = 1.0,
                                      const int numberOfCycles = 5,
                                      const int iterationsPerCycle = 10,
                                      const int smoothEdgesEveryXIterations = 10,
                                      const int projectToSphereEveryXIterations = -1,
                                      const int neighborDepth = 5,
                                      const SURFACE_TYPES surfaceTypeHint = SURFACE_TYPE_UNKNOWN);
                                    
      /// mark neighbor nodes to specified depth 
      void markNeighborNodesToDepth(std::vector<bool>& nodeFlags, const int depth);
      
      /// simplify the surface to a fewer number of polygons
      vtkPolyData* simplifySurface(const int maxPolygons) const;
      
      /// Create a string of c-language arrays containing vertices, normals, triangles
      QString convertToCLanguageArrays() const;

   protected:
      //
      // NOTE NOTE NOTE NOTE NOTE NOTE NOTE    !!!!!!!!!!!!!!!!!!!!!!!!!!!
      //
      // If any members are added, update the copy constructor
      //
      
      /// Coordinates for this surface
      CoordinateFile coordinates;
      
      /// Pointer to topology for this surface (Do not ever "delete" topology)
      TopologyFile*  topology;
      
      /// Normal vectors
      std::vector<float> normals;
      
      /// surface type
      SURFACE_TYPES surfaceType;
      
      /// structure 
      Structure structure;
      
      /// Default scale
      float defaultScaling;
      
      /// default perspective zooming
      float defaultPerspectiveZooming;
      
      /// Saved coordinates for push and pop operations
      std::vector<float> pushPopCoordinates;
      
      /// half of display for default scaling in X
      float displayHalfX;
      
      /// half of display for default scaling in Y
      float displayHalfY;
      
      /// last topology modification status
      unsigned long lastTopologyModificationNumber;
      
      //
      // NOTE NOTE NOTE NOTE NOTE NOTE NOTE    !!!!!!!!!!!!!!!!!!!!!!!!!!!
      //
      // If any members are added, update the copy constructor
      //
};

#endif

