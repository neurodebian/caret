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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <cmath>
#include <limits>
#include <sstream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>

#include "CoordinateFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "MathUtilities.h"
#include "MniObjSurfaceFile.h"
#include "GiftiDataArray.h"
#include "MetricFile.h"
#include "SpecFile.h"
#include "StatisticDataGroup.h"
#include "StatisticMeanAndDeviation.h"
#include "StatisticRandomNumber.h"
#include "StatisticRandomNumberOperator.h"
#include "StringUtilities.h"
#include "TransformationMatrixFile.h"
#include "vtkBase64Utilities.h"
#include "vtkPolyData.h"

#define USE_VTK_BASE64

/** 
 * The constructor.
 */
CoordinateFile::CoordinateFile()
   : GiftiNodeDataFile("Coordinate File", 
                  GiftiCommon::intentCoordinates,
                  GiftiDataArray::DATA_TYPE_FLOAT32,
                  3,
                  SpecFile::getCoordinateFileExtension(),
                  FILE_FORMAT_ASCII, 
                  FILE_IO_READ_AND_WRITE, 
                  FILE_IO_READ_AND_WRITE, 
                  FILE_IO_NONE,
                  FILE_IO_NONE)
{
   clear();
}

/** 
 * The copy constructor.
 */
CoordinateFile::CoordinateFile(const CoordinateFile& cf)
   : GiftiNodeDataFile(cf)
{
   copyHelperCoordinate(cf);
}

/** 
 * The destructor.
 */
CoordinateFile::~CoordinateFile()
{
   clear();
}

/**
 * assignment operator.
 */
CoordinateFile& 
CoordinateFile::operator=(const CoordinateFile& cf)
{
   if (this != &cf) {
      GiftiNodeDataFile::operator=(cf);
      copyHelperCoordinate(cf);
   }
   
   return *this;
}

/**
 * copy helper used by assignment operator and copy constructor.
 */
void 
CoordinateFile::copyHelperCoordinate(const CoordinateFile& /* cf */)
{
}
      
/**
 * Add a coordinate to the file.
 */
void
CoordinateFile::addCoordinate(const float xyz[3])
{
   addNodes(1);
   float* coords = &dataArrays[0]->getDataPointerFloat()[0];
   const int indx = getNumberOfNodes() - 1;
   coords[indx*3] = xyz[0];
   coords[indx*3 + 1] = xyz[1];
   coords[indx*3 + 2] = xyz[2];
}

/**
 * apply GIFTI transformation matrix.
 */
void
CoordinateFile::applyGiftiTransformationMatrix()
{
   if (getNumberOfDataArrays() > 0) {
      GiftiDataArray* gda = getDataArray(0);
      
      GiftiMatrix* bestMatrix = NULL;
      const int num = gda->getNumberOfMatrices();
      for (int i = 0; i < num; i++) {
         GiftiMatrix* gm = gda->getMatrix(i);
         
         if (gm->getDataSpaceName() == GiftiCommon::spaceLabelTalairach) {
            //
            // If data is already Talairach, do not need to do anything to the data
            //
            return;
         }
         else if (gm->getTransformedSpaceName() == GiftiCommon::spaceLabelTalairach) {
            // 
            // This matrix will make the coordinates Talairach
            //
            bestMatrix = gm;
         }
      }
      
      //
      // If a matrix was found, apply it
      //
      if (bestMatrix != NULL) {
         double m[4][4];
         bestMatrix->getMatrix(m);
         TransformationMatrix tm;
         tm.setMatrix(m);
         applyTransformationMatrix(tm);
         
         gda->removeAllMatrices();
         GiftiMatrix gm;
         gm.setDataSpaceName(GiftiCommon::spaceLabelTalairach);
         gm.setTransformedSpaceName(GiftiCommon::spaceLabelTalairach);
         gda->addMatrix(gm);
      }
   }
}      

/**
 * Apply transformation matrix to coordinate file.
 */
void 
CoordinateFile::applyTransformationMatrix(const TransformationMatrix& tmIn)
{
   TransformationMatrix& tm = const_cast<TransformationMatrix&>(tmIn);
   const int num = getNumberOfCoordinates();
   float* coords = dataArrays[0]->getDataPointerFloat();
   for (int i = 0; i < num; i++) {
      float  p[3] = { coords[i*3], coords[i*3+1], coords[i*3+2] };
      tm.multiplyPoint(p);
      coords[i*3]   = p[0];
      coords[i*3+1] = p[1];
      coords[i*3+2] = p[2];
   }
   setModified();
}

/**
 * Clear the coordinate file.
 */
void
CoordinateFile::clear()
{
   GiftiNodeDataFile::clear();
}

/**
 * Set the number of coordinates and initialize the coordinates to all zeros.
 */
void
CoordinateFile::setNumberOfCoordinates(const int numCoordinates)
{
   setNumberOfNodesAndColumns(numCoordinates, 1, 3);
   setModified();
}

/**
 * Get a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::getCoordinate(const int coordinateNumber, 
                              float& x, float& y, float& z) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   x = coords[coordinateNumber * 3];
   y = coords[coordinateNumber * 3 + 1];
   z = coords[coordinateNumber * 3 + 2];
}
                         
/**
 * Get a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::getCoordinate(const int coordinateNumber, 
                              double& x, double& y, double& z) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   x = coords[coordinateNumber * 3];
   y = coords[coordinateNumber * 3 + 1];
   z = coords[coordinateNumber * 3 + 2];
}
                         
/**
 * Get a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::getCoordinate(const int coordinateNumber, 
                              float xyz[3]) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   xyz[0] = coords[coordinateNumber * 3];
   xyz[1] = coords[coordinateNumber * 3 + 1];
   xyz[2] = coords[coordinateNumber * 3 + 2];
}
                         
/**
 * Get a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::getCoordinate(const int coordinateNumber, 
                              double xyz[3]) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   xyz[0] = coords[coordinateNumber * 3];
   xyz[1] = coords[coordinateNumber * 3 + 1];
   xyz[2] = coords[coordinateNumber * 3 + 2];
}
                         
/**
 * Get a coordinate at the specified index.  No check is made of index validity.
 * Returns const pointer to XYZ coordinates.
 */
const float*
CoordinateFile::getCoordinate(const int coordinateNumber) const 
{
   return &dataArrays[0]->getDataPointerFloat()[coordinateNumber * 3];
}

/**
 * Get a coordinate at the specified index.  No check is made of index validity.
 * Returns pointer to XYZ coordinates.
 */
float*
CoordinateFile::getCoordinate(const int coordinateNumber)  
{
   return &dataArrays[0]->getDataPointerFloat()[coordinateNumber * 3];
}

/**
 * get all coordinates (3 elements per coordinate).
 */
void 
CoordinateFile::getAllCoordinates(std::vector<float>& coordsOut) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   const int num = getNumberOfNodes() * 3;
   coordsOut.clear();
   for (int i = 0; i < num; i++) {
      coordsOut.push_back(coords[i]);
   }
}      

/**
 * get all coordinates (allocate coords to 3 * getNumberOfCoordinates()).
 */
void 
CoordinateFile::getAllCoordinates(float* coordsOut) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   const int num = getNumberOfCoordinates();
   for (int i = 0; i < num; i++) {
      coordsOut[i*3]   = coords[i*3];
      coordsOut[i*3+1] = coords[i*3+1];
      coordsOut[i*3+2] = coords[i*3+2];
   }
}      

/**
 * set all coordinates (3 elements per coordinate).
 */
void 
CoordinateFile::setAllCoordinates(const std::vector<float>& coordsIn) 
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   const int num = getNumberOfNodes() * 3;
   for (int i = 0; i < num; i++) {
      coords[i] = coordsIn[i];
   }
   setModified();
}

/**
 * set all coordinates (allocate coords to 3 * getNumberOfCoordinates()).
 */
void 
CoordinateFile::setAllCoordinates(const float* coordsIn) 
{
   float* ptr = dataArrays[0]->getDataPointerFloat();
   const int num = getNumberOfNodes() * 3;
   for (int i = 0; i < num; i++) {
      ptr[i] = coordsIn[i];
   }
   setModified();
}      

/**
 * Set a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::setCoordinate(const int coordinateNumber, 
                              const float x, const float y, const float z)
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   coords[coordinateNumber * 3]     = x;
   coords[coordinateNumber * 3 + 1] = y;
   coords[coordinateNumber * 3 + 2] = z;
   setModified();
}

/**
 * Set a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::setCoordinate(const int coordinateNumber, 
                              const double x, const double y, const double z)
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   coords[coordinateNumber * 3]     = x;
   coords[coordinateNumber * 3 + 1] = y;
   coords[coordinateNumber * 3 + 2] = z;
   setModified();
}

/**
 * Set a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::setCoordinate(const int coordinateNumber, 
                              const float xyz[3])
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   coords[coordinateNumber * 3]     = xyz[0];
   coords[coordinateNumber * 3 + 1] = xyz[1];
   coords[coordinateNumber * 3 + 2] = xyz[2];
   setModified();
}

/**
 * Set a coordinate at the specified index.  No check is made of index validity.
 */
void 
CoordinateFile::setCoordinate(const int coordinateNumber, 
                              const double xyz[3])
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   coords[coordinateNumber * 3]     = xyz[0];
   coords[coordinateNumber * 3 + 1] = xyz[1];
   coords[coordinateNumber * 3 + 2] = xyz[2];
   setModified();
}

/**
 * deform "this" node data file placing the output in "deformedFile".
 */
void 
CoordinateFile::deformFile(const DeformationMapFile& /*dmf*/, 
                           GiftiNodeDataFile& /*deformedFile*/,
                           const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   throw FileException("Coordinate file deformation not supported.");
}
      
/**
 * Get the bounds of the coordinate file.
 * Bounds contains minx, maxx, miny, maxy, minz, maxz.
 */
void 
CoordinateFile::getBounds(float bounds[6]) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   
   bounds[0] = coords[0];
   bounds[1] = coords[0];
   bounds[2] = coords[1];
   bounds[3] = coords[1];
   bounds[4] = coords[2];
   bounds[5] = coords[2];
   
   const int num = getNumberOfCoordinates();
   for (int i = 0; i < num; i++) {
      if (coords[i * 3] < bounds[0]) bounds[0] = coords[i * 3];
      if (coords[i * 3] > bounds[1]) bounds[1] = coords[i * 3];
      if (coords[i * 3 + 1] < bounds[2]) bounds[2] = coords[i * 3 + 1];
      if (coords[i * 3 + 1] > bounds[3]) bounds[3] = coords[i * 3 + 1];
      if (coords[i * 3 + 2] < bounds[4]) bounds[4] = coords[i * 3 + 2];
      if (coords[i * 3 + 2] > bounds[5]) bounds[5] = coords[i * 3 + 2];
   }
}

/**
 * Get the coordinate closest to the point at (xp, yp, zp).
 */
int 
CoordinateFile::getCoordinateIndexClosestToPoint(const float xyz[3],
                                                 const int startSearchAtCoordinateIndex) const
{
   return getCoordinateIndexClosestToPoint(xyz[0],
                                           xyz[1],
                                           xyz[2],
                                           startSearchAtCoordinateIndex);
}

/**
 * Get the coordinate closest to the point at (xp, yp, zp).
 */
int 
CoordinateFile::getCoordinateIndexClosestToPoint(const float xp, const float yp, 
                                                 const float zp,
                                                 const int startSearchAtCoordinateIndex) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();

   int closest = -1;
   float closestDist = std::numeric_limits<float>::max();
   const int num = getNumberOfCoordinates();
   for (int i = startSearchAtCoordinateIndex; i < num; i++) {
      const float dx = coords[i * 3] - xp;
      const float dy = coords[i * 3 + 1] - yp;
      const float dz = coords[i * 3 + 2] - zp;
      const float dist = dx*dx + dy*dy + dz*dz;
      if (dist < closestDist) {
         closest = i;
         closestDist = dist;
      }
   }
   return closest;
}

/**
 * get the distance from a 3D point to a coordinate.
 */
float 
CoordinateFile::getDistanceToPoint(const int coordIndex, 
                                   const float* point3D) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   const float dx = coords[coordIndex*3]   - point3D[0];
   const float dy = coords[coordIndex*3+1] - point3D[1];
   const float dz = coords[coordIndex*3+2] - point3D[2];
   return sqrt(dx*dx + dy*dy + dz*dz);
}

/**
 * get the distance squared from a 3D point to a coordinate.
 */
float 
CoordinateFile::getDistanceToPointSquared(const int coordIndex, 
                                          const float* point3D) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   const float dx = coords[coordIndex*3]   - point3D[0];
   const float dy = coords[coordIndex*3+1] - point3D[1];
   const float dz = coords[coordIndex*3+2] - point3D[2];
   return (dx*dx + dy*dy + dz*dz);
}

/**
 * Get the distance between two coordinates
 */
float
CoordinateFile::getDistanceBetweenCoordinates(const int c1, const int c2) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   const float dx = coords[c1*3]   - coords[c2*3];
   const float dy = coords[c1*3+1] - coords[c2*3+1];
   const float dz = coords[c1*3+2] - coords[c2*3+2];
   return sqrt(dx*dx + dy*dy + dz*dz);
}

/**
 * Get the distance between two coordinates squared.
 */
float
CoordinateFile::getDistanceBetweenCoordinatesSquared(const int c1, 
                                                     const int c2) const
{
   float* coords = dataArrays[0]->getDataPointerFloat();
   const float dx = coords[c1*3]   - coords[c2*3];
   const float dy = coords[c1*3+1] - coords[c2*3+1];
   const float dz = coords[c1*3+2] - coords[c2*3+2];
   return (dx*dx + dy*dy + dz*dz);
}

/**
 * Compute shuffled average coordinate files.  The inputs files are randomly split into 
 * two groups and from these two groups two average coordinate files are created.  
 * numberInGroup1 is the size of the first group but if this value is non-positive, the
 * groups are sized to one-half of the number of input files.
 */
void 
CoordinateFile::createShuffledAverageCoordinatesFiles(const std::vector<CoordinateFile*>& files,
                                                      const int numberInGroup1,
                                                      CoordinateFile& coordFileOut1,
                                                      CoordinateFile& coordFileOut2)
                                                                      throw (FileException)
{
   //
   // Check inputs
   //
   const int numFiles = static_cast<int>(files.size());
   if (numFiles < 2) {
      throw FileException("Shuffled average coordinate files requires at least two files.");
   }
   const int numCoords = files[0]->getNumberOfCoordinates();
   if (numCoords <= 0) {
      throw FileException("Shuffled average coordinate files has at least one file with no nodes.");
   }
   for (int i = 1; i < numFiles; i++) {
      if (files[i]->getNumberOfCoordinates() != numCoords) {
         throw FileException("Shuffled average coordinate files have different numbers of nodes.");
      }
   }
   if (numberInGroup1 >= numFiles) {
      throw FileException("Shuffled average coordinate files group one size equals number of files or larger.");
   }
   
   //
   // Create shuffled files' indices
   //
   std::vector<int> indicesShuffled(numFiles);
   for (int i = 0; i < numFiles; i++) {
      indicesShuffled[i] = i;
   }
   //RandomNumberOp randOp;  // used to rand() is called
   StatisticRandomNumberOperator randOp;  // used to rand() is called
   std::random_shuffle(indicesShuffled.begin(), indicesShuffled.end(), randOp); 
   
   //
   // set the half files index
   //
   int halfIndex = numFiles / 2;
   if (numberInGroup1 > 0) {
      halfIndex = numberInGroup1;
   }
   
   //
   // Create two groups of files
   //
   std::vector<CoordinateFile*> group1;
   std::vector<CoordinateFile*> group2;
   for (int i = 0; i < numFiles; i++) {
      const int indx = indicesShuffled[i];
      if (i < halfIndex) {
         group1.push_back(files[indx]);
      }
      else {
         group2.push_back(files[indx]);
      }
   }
   
   //
   // Create the two average coordinate files
   //
   createAverageCoordinateFile(group1, coordFileOut1);
   createAverageCoordinateFile(group2, coordFileOut2);
}
                                             
/**
 * compute an average coordinate file.
 */
void 
CoordinateFile::createAverageCoordinateFile(const std::vector<CoordinateFile*>& files,
                                            CoordinateFile& averageFile,
                                            MetricFile* ssf) throw (FileException)
{
   const int numFiles = static_cast<int>(files.size());
   if (numFiles <= 0) {
      return;
   }
   
   //
   // Verify that files all have the same number of coordinates
   //
   int numCoords = files[0]->getNumberOfCoordinates();
   for (int j = 1; j < numFiles; j++) {
      if (files[j]->getNumberOfCoordinates() != numCoords) {
         throw FileException("Files have different numbers of coordinates");
      }
   }

   //
   // Setup the average file
   //
   averageFile.clear();
   averageFile.setNumberOfCoordinates(numCoords);
   averageFile.setHeaderTag(headerTagStructure, files[0]->getHeaderTag(headerTagStructure));
   QString comment("This file is the average of:");
   for (int j = 0; j < numFiles; j++) {
      comment.append("\n   ");
      comment.append(FileUtilities::basename(files[j]->getFileName()));
   }
   averageFile.setFileComment(comment);
   averageFile.setHeaderTag(AbstractFile::headerTagConfigurationID,
         files[0]->getHeaderTag(AbstractFile::headerTagConfigurationID));
   
   //
   // Setup surface shape file
   //
   int surfaceShapeColumn = -1;
   if (ssf != NULL) {
      if (ssf->getNumberOfNodes() == 0) {
         ssf->setNumberOfNodesAndColumns(numCoords, 1);
      }
      else {
         ssf->addColumns(1);
      }
      surfaceShapeColumn = ssf->getNumberOfColumns() - 1;
      ssf->setColumnName(surfaceShapeColumn, "SHAPE_STANDARD_UNCERTAINTY");
      ssf->setColumnComment(surfaceShapeColumn, comment);
      ssf->setColumnColorMappingMinMax(surfaceShapeColumn, 0.0, 5.0);
   }
   
   //
   // Average the coordinates
   //
   const float numFilesFloat = numFiles;
   for (int i = 0; i < numCoords; i++) {
      float sum[3] = { 0.0, 0.0, 0.0 };
      for (int j = 0; j < numFiles; j++) {
         float xyz[3];
         files[j]->getCoordinate(i, xyz);
         sum[0] += xyz[0];
         sum[1] += xyz[1];
         sum[2] += xyz[2];
      }
      sum[0] /= numFilesFloat;
      sum[1] /= numFilesFloat;
      sum[2] /= numFilesFloat;
      averageFile.setCoordinate(i, sum);
      
      //
      // Should uncertainty be placed in surface shape file
      //
      if (surfaceShapeColumn >= 0) {
         //
         // Get distance of each surfaces' node from the average
         //
         std::vector<float> deltas(numFiles);
         for (int k = 0; k < numFiles; k++) {
            deltas[k] = MathUtilities::distance3D(files[k]->getCoordinate(i), sum);
         }
         
         //
         // Get mean of distances
         //
         StatisticDataGroup sdg(&deltas,
                                StatisticDataGroup::DATA_STORAGE_MODE_POINT);
         StatisticMeanAndDeviation smad;
         smad.addDataGroup(&sdg);
         try {
            smad.execute();
         }
         catch (StatisticException&) {
         }
         //
         // Place the average into the surface shape file
         //
         ssf->setValue(i, surfaceShapeColumn, smad.getMean());
      }
   }
}

/**
 * get the coordinates from a MNI OBJ surface file.
 */
void 
CoordinateFile::importFromMniObjSurfaceFile(const MniObjSurfaceFile& mni) throw (FileException)
{
   clear();
   
   const int numVertices = mni.getNumberOfPoints();
   if (numVertices > 0) {
      setNumberOfCoordinates(numVertices);
      for (int i = 0; i < numVertices; i++) {
         const float* xyz = mni.getPointXYZ(i);
         setCoordinate(i, xyz);
      }
   }
   appendToFileComment(" Imported from ");
   appendToFileComment(FileUtilities::basename(mni.getFileName()));
   setModified();
}
      
/**
 * Get the coordinates out of a brain voyager file.
 */
void 
CoordinateFile::importFromBrainVoyagerFile(const BrainVoyagerFile& bvf)
{
   clear();
   
   const int numVertices = bvf.getNumberOfVertices();
   if (numVertices > 0) {
      setNumberOfCoordinates(numVertices);
      for (int i = 0; i < numVertices; i++) {
         float xyz[3];
         bvf.getVertexCoordinates(i, xyz);
         setCoordinate(i, xyz);
      }
   }
   appendToFileComment(" Imported from ");
   appendToFileComment(FileUtilities::basename(bvf.getFileName()));
   setModified();
}

/**
 * get the coordinates out of a free surfer surface file.
 */
void 
CoordinateFile::importFromFreeSurferSurfaceFile(const FreeSurferSurfaceFile& fssf,
                                                const int numNodesIn)
                                                          throw (FileException)
{
   clear();
   
   const int numVertices = fssf.getNumberOfVertices();
   
   int numNodes = numNodesIn;
   if (numNodes < 0) {
      numNodes = numVertices;
   }

   //
   // If this is a patch file
   //
   if (fssf.getIsAsciiPatchFile() ||
       fssf.getIsBinaryPatchFile()) {
      //
      // A non-patch surface must have been read prior to reading a patch file
      //
      if (numNodes <= 0) {
         throw FileException(fssf.getFileName(), "This is a FreeSurfer patch surface file.\n"
                                             "It must be read after a non-patch surface file.");
      }
      setNumberOfCoordinates(numNodes);
   }
   else {
      if (numNodes > 0) {
         if (numNodes != numVertices) {
            throw FileException(fssf.getFileName(), "This free surfer file has a different "
                                  "number of nodes than the current surface(s).");
         }
      }
      setNumberOfCoordinates(numVertices);
   }

   if (numVertices > 0) {
      for (int i = 0; i < numVertices; i++) {
         float xyz[3];
         int vertNumber;
         fssf.getVertexCoordinates(i, vertNumber, xyz);
         setCoordinate(vertNumber, xyz);
      }
   }
   appendToFileComment(" Imported from ");
   appendToFileComment(FileUtilities::basename(fssf.getFileName()));
   setModified();
}

/**
 * add the coordinates to of a free surfer surface file
 */
void 
CoordinateFile::exportToFreeSurferSurfaceFile(FreeSurferSurfaceFile& fssf)
{
   const int numCoords = getNumberOfCoordinates();
   for (int i = 0; i < numCoords; i++) {
      float xyz[3];
      getCoordinate(i, xyz);
      fssf.setVertexCoordinates(i, i, xyz);
   }
}

/**
 * Get the coordinates out of a vtkPolyData object
 */
void
CoordinateFile::importFromVtkFile(vtkPolyData* polyData)
{
   clear();
   
   const int numVertices = polyData->GetNumberOfPoints();
   if (numVertices > 0) {
      setNumberOfCoordinates(numVertices);
      vtkPoints* points = polyData->GetPoints();
      for (int i = 0; i < numVertices; i++) {
#ifdef HAVE_VTK5
         double xyz[3];
#else // HAVE_VTK5
         float xyz[3];
#endif // HAVE_VTK5
         points->GetPoint(i, xyz);
         setCoordinate(i, xyz);
      }
   }
   setModified();
}

/**
 * Read the coordinate file data.  May throw FileException.
 */
void
CoordinateFile::readLegacyNodeFileData(QFile& /*file*/, QTextStream& stream, 
                             QDataStream& binStream) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            QString line;
            
            readLine(stream, line);
            const int num = line.toInt();
            if (num < 0) {
               throw FileException(filename, "Number of coordinates is less than zero.");
            }
            setNumberOfCoordinates(num);
            float* coordPtr = dataArrays[0]->getDataPointerFloat();
            
            
            float x, y, z;
            int index;
            for (int i = 0; i < num; i++) {
               readLine(stream, line);
               sscanf(line.toAscii().constData(), "%d %f %f %f", &index, &x, &y, &z);
               const int i3 = i * 3;
               coordPtr[i3]     = x;
               coordPtr[i3 + 1] = y;
               coordPtr[i3 + 2] = z;
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
            int numCoords;
            binStream >> numCoords;
            if (numCoords > 0) {
               setNumberOfCoordinates(static_cast<int>(numCoords));
               float* coordPtr = dataArrays[0]->getDataPointerFloat();
            
               for (int i = 0; i < numCoords; i++) {
                  const int i3 = i * 3;
                  binStream >> coordPtr[i3];
                  binStream >> coordPtr[i3 + 1];
                  binStream >> coordPtr[i3 + 2];
               }
            }
         }
         break;
      case FILE_FORMAT_XML:
/*
         {
            //throw FileException(filename, "Reading in XML format not supported.");
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) { 
                  if (DebugControl::getDebugOn()) {      
                     std::cout << "Tag Name: " << elem.tagName() << std::endl;
                  }
                  if (elem.tagName() == "number-of-coords") {
                     const QString strNum = getXmlElementFirstChildAsString(elem);
                     if (strNum.isEmpty()) {
                        throw FileException(filename,
                           "XML tag \"number-of-coords\" is isEmpty.");
                     }
                     const int num = StringUtilities::toInt(strNum);
                     if (num > 0) {
                        setNumberOfCoordinates(num);
                     }
                  }
                  else if (elem.tagName() == "coords") {
                     if (getNumberOfCoordinates() <= 0) {
                        throw FileException(filename,
                           "XML file have coords but number of coordinates is zero.");
                     }
                     QDomNode node = static_cast<QDomNode>(elem.firstChild());
                     if (node.isNull() == false) {
                        const QDomText textNode = node.toText();
                        if (textNode.isNull() == false) {
#ifdef USE_VTK_BASE64
                           const unsigned long numBytes = getNumberOfCoordinates() * 12;
                           const unsigned long numDecoded = vtkBase64Utilities::Decode(
                                                      (const unsigned char*)textNode.data(),
                                                      numBytes,
                                                      (unsigned char*)&coords[0]);
                           if (numBytes != numDecoded) {
                              std::ostringstream str;
                              str << "Error reading XML coordinates.\n"
                                  << "Number of decoded bytes: "
                                  << numDecoded
                                  << "\n."
                                  << "Number of bytes expected: "
                                  << numBytes
                                  << ".";
                              throw FileException(filename, str.str().c_str());
                           }
#else
                           QCodecs::base64Decode(textNode.data(),
                                                 textNode.data().length(),
                                                 &coords[0]);
#endif
/ *
                           QByteArray ba;
                           ba.duplicate(textNode.data(), textNode.data().length());
                           QByteArray base64;
                           QCodecs::base64Decode(ba, base64);  // xyz = 12 bytes
                           memcpy(&coords[0], base64.data(), 12 * getNumberOfCoordinates());
* /
                        }
                     }
                  }
               }
               node = node.nextSibling();
            }
         }
*/
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing Comma Separated Value File Format not supported.");
         break;
   }
   
   setModified();
}

/**
 * Read the coordinate file data.  May throw FileException.
 */
void
CoordinateFile::writeLegacyNodeFileData(QTextStream& stream, QDataStream& binStream) throw (FileException)
{
   const int numCoords = getNumberOfCoordinates();
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         stream << numCoords << "\n";
         for (int i = 0; i < numCoords; i++) {
            float x, y, z;
            getCoordinate(i, x, y, z);
            stream << i << " " << x << " " << y << " " << z << "\n";
         }
         break;
      case FILE_FORMAT_BINARY:
         binStream << static_cast<qint32>(numCoords);
         if (numCoords > 0) {
            const float* xyz = getCoordinate(0);
            for (int i = 0; i < numCoords; i++) {
               const int i3 = i * 3;
               binStream << xyz[i3];
               binStream << xyz[i3+1];
               binStream << xyz[i3+2];
            }
         }
         break;
      case FILE_FORMAT_XML:
/*
         {
            //throw FileException(filename, "Writing in XML format not supported.");
            QDomElement numElem = xmlDoc.createElement("number-of-coords");
            QDomText    numText = xmlDoc.createTextNode(QString::number(numCoords));
            numElem.appendChild(numText);

#ifdef USE_VTK_BASE64
            const unsigned long numBytes = numCoords * 12;
            const unsigned long buffSize = (numBytes * 2);
            unsigned char* buff = new unsigned char[buffSize];
            const float* xyz = getCoordinate(0);
            const int num = vtkBase64Utilities::Encode((const unsigned char*)&xyz[0],
                                                       numBytes,
                                                       buff);
            buff[num] = '\0';
            QDomElement coordsElem = xmlDoc.createElement("coords");
            QDomText    coordsText = xmlDoc.createTextNode((const char*)buff);
            coordsElem.appendChild(coordsText);
#else            
            const float* xyz = getCoordinate(0);
            QByteArray ba;
            ba.duplicate((const char*)&xyz[0], numCoords * 12);  // xyz = 12 bytes
            QByteArray base64;
            QCodecs::base64Encode(ba, base64, true);
            QDomElement coordsElem = xmlDoc.createElement("coords");
            QDomText    coordsText = xmlDoc.createTextNode(base64);
            coordsElem.appendChild(coordsText);
#endif            
            rootElement.appendChild(numElem);
            rootElement.appendChild(coordsElem);
         }
*/
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing Comma Separated Value File Format not supported.");
         break;
   }
   
}

