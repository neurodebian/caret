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



#ifndef __VOLUME_FILE_NEW_H__
#define __VOLUME_FILE_NEW_H__

#include <QFile>
#include <QTextStream>

#include <fstream>
#include <QString>
#include <vector>

#include "AbstractFile.h"
#include "AfniHeader.h"
#include "FileException.h"
#include "StudyMetaDataLinkSet.h"
#include "WuNilHeader.h"

#include "zlib.h"

class Border;
class ParamsFile;
class StatisticHistogram;
class TransformationMatrix;
class VectorFile;
class VolumeModification;
class VolumeITKImage;
class vtkImageData;
class vtkStructuredPoints;
class vtkTransform;

/// This class is used to store a volume and its attributes
class VolumeFile : public AbstractFile {

   private:
      /// Volume data read mode
      enum VOLUME_DATA_READ_MODE {
         VOLUME_DATA_READ_WITH_STDLIB,  // read with C++ Standard Library
         VOLUME_DATA_READ_WITH_ZLIB     // read with ZLIB
      };
      
   public:
      /// Class for storing voxel indices used while searching a volume
      class VoxelIJK {
         public:
            /// Constructor
            VoxelIJK() {
               ijkv[0] = -1;
               ijkv[1] = -1;
               ijkv[2] = -1;
            }
            
            /// Constructor
            VoxelIJK(const int i, const int j, const int k) {
               ijkv[0] = i; 
               ijkv[1] = j;
               ijkv[2] = k;
            }  
            /// Constructor
            VoxelIJK(const int ijkIn[3]) {
               ijkv[0] = ijkIn[0];
               ijkv[1] = ijkIn[1];
               ijkv[2] = ijkIn[2];
            }  

            /// get the voxel indices
            const int* getIJK() const { return &ijkv[0]; }
            
            /// get the I component
            int getI() const { return ijkv[0]; }
            
            /// get the J component
            int getJ() const { return ijkv[1]; }
            
            /// get the K component
            int getK() const { return ijkv[2]; }
            
            /// get the voxel indices
            void getIJK(int& i, int& j, int& k) const {
               i = ijkv[0];
               j = ijkv[1];
               k = ijkv[2];
            }

            /// get the voxel indices
            void getIJK(int ijkOut[3]) const {
               ijkOut[0] = ijkv[0];
               ijkOut[1] = ijkv[1];
               ijkOut[2] = ijkv[2];
            }

            /// set the voxel indices
            void setIJK(const int ijkIn[3]) { 
               ijkv[0] = ijkIn[0]; 
               ijkv[1] = ijkIn[1]; 
               ijkv[2] = ijkIn[2]; 
            }
            
            /// get the voxel indices
            void setIJK(const int i, const int j, const int k) {
               ijkv[0] = i;
               ijkv[1] = j;
               ijkv[2] = k;
            }

         private:
            /// the voxel indices
            int ijkv[3];
            
         friend class VolumeFile;
      };

      /// group of voxels
      class VoxelGroup {
         public:
            /// constructor
            VoxelGroup() { }

            /// destructor
            ~VoxelGroup() { clear(); }
            
            /// add a voxel
            void addVoxel(const VoxelIJK& v) { voxels.push_back(v); }
            
            /// clear the voxel group
            void clear() { voxels.clear(); }
            
            /// get the number of voxels in the group
            int getNumberOfVoxels() const { return voxels.size(); }
            
            /// get a voxel in the group
            VoxelIJK getVoxel(const int indx) const { return voxels[indx]; }
            
         protected:
            /// the voxels
            std::vector<VoxelIJK> voxels;
      };
      
      /// type of volume
      enum VOLUME_TYPE {
         ///  anatomical volume
         VOLUME_TYPE_ANATOMY,
         ///  functional volume
         VOLUME_TYPE_FUNCTIONAL,
         ///  paint volume
         VOLUME_TYPE_PAINT,
         ///  probabilistic atals volume
         VOLUME_TYPE_PROB_ATLAS,
         ///  Red-Green-Blue volume
         VOLUME_TYPE_RGB,
         ///  segmentation volume
         VOLUME_TYPE_SEGMENTATION,
         ///  vector volume file
         VOLUME_TYPE_VECTOR,
         ///  region of interest selected voxel
         VOLUME_TYPE_ROI,
         ///  unknown volume
         VOLUME_TYPE_UNKNOWN
      };
      
      /// Type of volume space
      enum VOLUME_SPACE {
         /// volume is read and manipulated to be LPI
         VOLUME_SPACE_COORD_LPI,    
         /// volume is read and left in "voxel native space"
         VOLUME_SPACE_VOXEL_NATIVE  
      };
      
      /// Volume file type
      enum FILE_READ_WRITE_TYPE {
         /// raw
         FILE_READ_WRITE_TYPE_RAW,
         /// AFNI
         FILE_READ_WRITE_TYPE_AFNI,
         /// Analyze
         FILE_READ_WRITE_TYPE_ANALYZE,
         /// NIFTI
         FILE_READ_WRITE_TYPE_NIFTI,
         /// SPM/MEDx
         FILE_READ_WRITE_TYPE_SPM_OR_MEDX,
         /// WU IFH
         FILE_READ_WRITE_TYPE_WUNIL,
         /// UNKNOWN
         FILE_READ_WRITE_TYPE_UNKNOWN
      };
      
      /// axis
      enum VOLUME_AXIS {
         /// X-axis
         VOLUME_AXIS_X,
         /// Y-axis
         VOLUME_AXIS_Y,
         /// Z-axis
         VOLUME_AXIS_Z,
         /// all axes
         VOLUME_AXIS_ALL,
         /// oblique axis
         VOLUME_AXIS_OBLIQUE,
         /// oblique axis X
         VOLUME_AXIS_OBLIQUE_X,
         /// oblique axis Y
         VOLUME_AXIS_OBLIQUE_Y,
         /// oblique axis Z
         VOLUME_AXIS_OBLIQUE_Z,
         /// oblique all axes
         VOLUME_AXIS_OBLIQUE_ALL,
         /// Unknown-axis
         VOLUME_AXIS_UNKNOWN
      };
      
      /// voxel data type
      enum VOXEL_DATA_TYPE {
         /// data type unknown
         VOXEL_DATA_TYPE_UNKNOWN,
         /// data type char
         VOXEL_DATA_TYPE_CHAR,
         /// data type unsigned char
         VOXEL_DATA_TYPE_CHAR_UNSIGNED,
         /// data type short
         VOXEL_DATA_TYPE_SHORT,
         /// data type unsigned short
         VOXEL_DATA_TYPE_SHORT_UNSIGNED,
         /// data type int
         VOXEL_DATA_TYPE_INT,
         /// data type unsigned int
         VOXEL_DATA_TYPE_INT_UNSIGNED,
         /// data type long
         VOXEL_DATA_TYPE_LONG,
         /// data type unsigned long
         VOXEL_DATA_TYPE_LONG_UNSIGNED,
         /// data type float
         VOXEL_DATA_TYPE_FLOAT,
         /// data type double
         VOXEL_DATA_TYPE_DOUBLE,
         /// data type RGB voxel interleaved
         VOXEL_DATA_TYPE_RGB_VOXEL_INTERLEAVED,
         /// data type RGB slice interleaved
         VOXEL_DATA_TYPE_RGB_SLICE_INTERLEAVED,
         /// data type vector
         VOXEL_DATA_TYPE_VECTOR
      };
      
      /// volume file orientation
      enum ORIENTATION {
         /// orientation - unknown
         ORIENTATION_UNKNOWN,
         /// orientation - right to left
         ORIENTATION_RIGHT_TO_LEFT,
         /// orientation - left to right
         ORIENTATION_LEFT_TO_RIGHT,
         /// orientation - posterior to anterior
         ORIENTATION_POSTERIOR_TO_ANTERIOR,
         /// orientation - anterior to posterior
         ORIENTATION_ANTERIOR_TO_POSTERIOR,
         /// orientation - inferior to superior
         ORIENTATION_INFERIOR_TO_SUPERIOR,
         /// orientation - superior to inferior
         ORIENTATION_SUPERIOR_TO_INFERIOR
      };

/*      
      /// standard volume spaces (DO NOT REARRANGE, add new spaces before "LAST")
      enum STANDARD_VOLUME_SPACE {
         /// non-standard volume space
         STANDARD_VOLUME_SPACE_NON_STANDARD,
         /// AFNI-TLRC volume space
         STANDARD_VOLUME_SPACE_AFNI_TALAIRACH,
         /// WU 111 volume space
         STANDARD_VOLUME_SPACE_WU_7112B_111,
         /// WU 222 volume space
         STANDARD_VOLUME_SPACE_WU_7112B_222,
         /// WU 333 volume space
         STANDARD_VOLUME_SPACE_WU_7112B_333,
         /// SPM Default volume space
         STANDARD_VOLUME_SPACE_SPM_DEFAULT,
         /// SPM Template volume space
         STANDARD_VOLUME_SPACE_SPM_TEMPLATE,
         /// number of volume space
         STANDARD_SPACE_LAST
      };
*/      
      /// mathematical operations on volumes
      enum VOLUME_MATH_OPERATION {
         /// math operation ADD
         VOLUME_MATH_OPERATION_ADD,
         /// math operation AND
         VOLUME_MATH_OPERATION_AND,
         /// math operation SUBTRACT
         VOLUME_MATH_OPERATION_SUBTRACT,
         /// math operation MULTIPLY
         VOLUME_MATH_OPERATION_MULTIPLY,
         /// math operation DIVIDE
         VOLUME_MATH_OPERATION_DIVIDE,
         /// math operation OR
         VOLUME_MATH_OPERATION_OR,
         /// math operation SUBTRACT POSITIVE (if (result < 0) result = 0)
         VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE,
         /// math operation MAX
         VOLUME_MATH_OPERATION_MAX,
         /// math operation DIFF_RATIO
         VOLUME_MATH_OPERATION_DIFFRATIO,
         /// math operation SQRT
         VOLUME_MATH_OPERATION_SQRT,
         /// math operation combine paint volumes
         VOLUME_MATH_OPERATION_COMBINE_PAINT,
         /// math operation NOR
         VOLUME_MATH_OPERATION_NOR,
         /// math operation NAND
         VOLUME_MATH_OPERATION_NAND,
         /// math operation AVERAGE
         VOLUME_MATH_OPERATION_AVERAGE,
         /// math operation EXCLUSIVE OR
         VOLUME_MATH_EXCLUSIVE_OR
      };
      
      /// operations on segmentation volumes
      enum SEGMENTATION_OPERATION {
         /// segmenation operation dilate
         SEGMENTATION_OPERATION_DILATE,
         /// segmenation operation erode
         SEGMENTATION_OPERATION_ERODE,
         /// segmenation operation flood fill 2D
         SEGMENTATION_OPERATION_FLOOD_FILL_2D,
         /// segmenation operation flood fill 3D
         SEGMENTATION_OPERATION_FLOOD_FILL_3D,
         /// segmenation operation remove connected piece in 2D
         SEGMENTATION_OPERATION_REMOVE_CONNECTED_2D,
         /// segmenation operation remove connected piece in 3D
         SEGMENTATION_OPERATION_REMOVE_CONNECTED_3D,
         /// segmenation operation turn voxels on
         SEGMENTATION_OPERATION_TOGGLE_ON,
         /// segmenation operation turn voxels off
         SEGMENTATION_OPERATION_TOGGLE_OFF
      };
      
      /// sculpt mode
      enum SCULPT_MODE {
         /// sculpt mode AND
         SCULPT_MODE_AND,
         /// sculpt mode SEED AND
         SCULPT_MODE_SEED_AND,
         /// sculpt mode AND NOT
         SCULPT_MODE_AND_NOT,
         /// sculpt mode SEED AND NOT
         SCULPT_MODE_SEED_AND_NOT
      };
      
      /// voxel search flags
      enum VOXEL_SEARCH_STATUS {
         /// voxel not searched
         VOXEL_NOT_SEARCHED = 0,
         /// voxel has been searched
         VOXEL_SEARCHED = 1
      };
      
      /// voxel coloring status
      enum VOXEL_COLOR_STATUS {
         /// voxel color invalid
         VOXEL_COLOR_STATUS_INVALID,
         /// voxel color valid and voxel should be displayed
         VOXEL_COLOR_STATUS_VALID,
         /// voxel color valid but voxel should not be displayed
         VOXEL_COLOR_STATUS_VALID_DO_NOT_SHOW_VOXEL
      };
      
      /// volume reading selections
      enum VOLUME_READING_SELECTION {
         /// read all sub volumes
         VOLUME_READ_SELECTION_ALL = -1,
         /// read header only
         VOLUME_READ_HEADER_ONLY   = -2
      };
      
      /// data order in slices
      enum SLICE_DATA_ORDER {
         /// increment column fastest
         SLICE_DATA_ORDER_COLUMN,
         /// increment row fastest
         SLICE_DATA_ORDER_ROW
      };
      
      /// interpolation type
      enum INTERPOLATION_TYPE {
         /// interpolation type cubic
         INTERPOLATION_TYPE_CUBIC,
         /// interpolation type linear
         INTERPOLATION_TYPE_LINEAR,
         /// interpolation type nearest neighbor (use for paint and atlas volumes)
         INTERPOLATION_TYPE_NEAREST_NEIGHBOR
      };
      
      /// get the name of the file (description only used if file name is isEmpty)
      virtual QString getFileName(const QString& description = "") const;

      /// get type of volume space used when volume files are read
      static VOLUME_SPACE getVolumeSpace() { return volumeSpace; }
      
      /// set type of volume space used when volume files are read
      static void setVolumeSpace(const VOLUME_SPACE vs) { volumeSpace = vs; }

/*      
      /// get name, dimensions, origin, and voxel spacing for standard volumes 
      static void getStandardSpaceParameters(const STANDARD_VOLUME_SPACE svs,
                                             QString& nameOut,
                                             int dimensionOut[3],
                                             float originOut[3],
                                             float voxelSpacingOut[3]);
*/                                             
      /// constructor
      VolumeFile();
      
      /// construct a volume consisting of a single slice 
      /// copy constructor
      VolumeFile(const VolumeFile& vf);
      
      /// constructor creates volume from a vector file
      VolumeFile(const VectorFile& vf);
      
      /// destructor
      ~VolumeFile();
      
      /// Assignment operator.
      VolumeFile& operator=(const VolumeFile& vf);

      /// initialize (allocates memory for a volume of specified type and dimensions)
      void initialize(const VOXEL_DATA_TYPE vdt, const int dim[3],
                      const ORIENTATION orient[3],
                      const float org[3], const float space[3],
                      const bool doClear = false,
                      const bool allocateVoxelData = true);
      
      /// compare a file for unit testing (returns true if "within tolerance")
      bool compareFileForUnitTesting(const AbstractFile* af,
                                     const float tolerance,
                                     QString& messageOut) const;
          
      /// get axis from string
      static VOLUME_AXIS getAxisFromString(const QString& s);
      
      /// Get the label of an axis enumerated type.
      static QString getAxisLabel(const VOLUME_AXIS axis);
      
      /// get the label of an orientations
      static QString getOrientationLabel(const ORIENTATION orient);
      
      /// see if a file is a NIFTI volume file
      static bool isFileNifti(const QString& name);
      
      /// get NIFTI intention and tr
      void getNiftiInfo(QString& intentCodeAndParamStringOut,
                        QString& intentNameOut,
                        int& intentCodeOut,
                        float& intentParameter1Out,
                        float& intentParameter2Out,
                        float& intentParameter3Out,
                        float &trOut) const;
      
      /// get the string describing the type of volume (anatomy, functional, etc)
      QString getVolumeTypeDescription() const;
      
      /// get the minimum and maximum values for a data type
      static void getDataTypeMinMaxValues(const VOXEL_DATA_TYPE vdt,
                                          double& minValueForDataType,
                                          double& maxValueForDataType);
       
      /// get all volume types and names (anatomy, functional, etc)
      static void getAllVolumeTypesAndNames(std::vector<VOLUME_TYPE>& typesOut,
                                            std::vector<QString>& namesOut,
                                            const bool addUnknown = false,
                                            const bool addROI = false);
                                            
      /// get all voxel data types and names (float, int, etc)
      static void getAllVoxelDataTypesAndNames(std::vector<VOXEL_DATA_TYPE>& typesOut,
                                               std::vector<QString>& namesOut,
                                               const bool addUnknown = false);
                                            
      /// Clears current file data in memory.
      void clear();
      
      /// resize the volume; if provided, update params with cropping
      void resize(const int dimExtent[6],
                  ParamsFile* paramsFile = NULL);
      
      /// pad segmentation volume
      void padSegmentation(const int padding[6],
                           const bool erodePaddingFlag);
      
      /// copy a subvolume to "this" volume
      void copySubVolume(const VolumeFile* sourceVolume,
                         const int extent[6],
                         const unsigned char offRgbValue[4],
                         const unsigned char onRgbValue[4],
                         VolumeModification* modifiedVoxels = NULL) throw (FileException);
                         
      /// copy a slice to "this" volume
      void copySlice(const VolumeFile* sourceVolume,
                     const int sourceSliceNumber,
                     const VOLUME_AXIS axis,
                     const int destinationSliceNumber);
                     
      /// get the volume type
      VOLUME_TYPE getVolumeType() const { return volumeType; }

      /// set the volume type
      void setVolumeType(const VOLUME_TYPE vt) { volumeType = vt; };
      
      /// returns true if the file is isEmpty
      bool empty() const { return (voxels == NULL); }
      
      /// get the afni header
      AfniHeader* getAfniHeader() { return &afniHeader; }
      
      /// get the wu-nil header
      WuNilHeader* getWuNilHeader() { return &wunilHeader; }
      
      /// get the volume data (const method)
      const float* getVoxelData() const { return voxels; }
      
      /// get the coordinate of a voxel
      void getVoxelCoordinate(const int ijk[3], 
                             const bool centerOfVoxelFlag,
                             float coord[3]) const;
      
      /// get the coordinate of a voxel
      void getVoxelCoordinate(const int i, const int j, const int k, 
                              const bool centerOfVoxelFlag,
                              float coord[3]) const;
      
      /// get the coordinate of a voxel
      void getVoxelCoordinate(const VoxelIJK& v, 
                              const bool centerOfVoxelFlag,
                              float coord[3]) const;
      
      /// get the volume data
      float* getVoxelData() { return voxels; }
      
      /// get a voxel with a flat index
      float getVoxelWithFlatIndex(const int indx, const int component = 0) const;
      
      /// set a voxel with a flat index
      void setVoxelWithFlatIndex(const int indx, const int component, const float value);
      
      /// get a voxel
      float getVoxel(const int i, const int j, const int k, const int component = 0) const;
      
      /// get a voxel
      float getVoxel(const int ijk[3], const int component = 0) const;
      
      /// get a voxel
      float getVoxel(const VoxelIJK& v, const int component = 0) const;
      
      /// see if a voxel index is valid
      bool getVoxelIndexValid(const int ijk[3]) const;
      
      /// see if a voxel index is valid
      bool getVoxelIndexValid(const int i, const int j, const int k) const;
      
      /// see if a voxel index is valid
      bool getVoxelIndexValid(const VoxelIJK& v) const;
      
      /// get a voxel at the specified index
      bool getVoxelAllComponents(const int ijk[3], float* voxelValue) const;
      
      /// get a voxel at the specified index
      bool getVoxelAllComponents(const int i, const int j, const int k, float* voxelValue) const;
      
      /// set a voxel at the specified index
      void setVoxelAllComponents(const int ijk[3], const float* voxelValue);
      
      /// set a voxel at the specified index
      void setVoxelAllComponents(const int i, const int j, const int k, const float* voxelValue);
      
      /// set a voxel at the specified index
      void setVoxel(const int ijk[3], const int component, const float voxelValue);
      
      /// set a voxel at the specified index
      void setVoxel(const int i, const int j, const int k, const int component, 
                    const float voxelValue);
      
      /// set a bunch of voxels using one dimensional index
      void setVoxel(const std::vector<int> indicies, const float voxelValue);
      
      /// set all of the voxels to a value
      void setAllVoxels(const float value);
      
      /// set all voxels within a rectangle
      void setAllVoxelsInRectangle(const int extent[6],
                                   const float value);
                                   
      /// set all of the voxels in a slice
      void setAllVoxelsInSlice(const VOLUME_AXIS axis, const int sliceNumber,
                               const float value);
              
      /// get the size of a slice (number of voxels and components)
      int getSizeOfSlice(const VOLUME_AXIS axis) const;
      
      /// get a slice from a volume (sliceVoxelsOut should be size getSizeOfSlice())
      void getSlice(const VOLUME_AXIS axis,
                    const int sliceNumber,
                    const SLICE_DATA_ORDER dataOrder,
                    float* sliceVoxelsOut) const;
                    
      /// set a slice from a volume (sliceVoxelsIn should be size getSizeOfSlice())
      void setSlice(const VOLUME_AXIS axis,
                    const int sliceNumber,
                    const SLICE_DATA_ORDER dataOrder,
                    const float* sliceVoxelsIn);
                    
      /// get the number of non-zero voxels
      int getNumberOfNonZeroVoxels() const;
      
      /// set voxel coloring valid
      void setVoxelColoringInvalid() { voxelColoringValid = false; }
      
      /// set the colors for a voxel (4th byte is VOXEL_COLOR_STATUS)
      void setVoxelColor(const int ijk[3], const unsigned char rgb[4]);
      
      /// set the colors for a voxel (4th byte is VOXEL_COLOR_STATUS)
      void setVoxelColor(const int i, const int j, const int k,
                         const unsigned char rgb[4]);
      
      /// get the colors at a voxel (returns true if a valid voxel index)
      bool getVoxelColor(const int ijk[3], unsigned char rgb[4]);

      /// get the colors at a voxel (returns true if a valid voxel index)
      bool getVoxelColor(const int i, const int j, const int k, unsigned char rgb[4]);

      /// Get an "interpolate" voxel at the specified coordinate
      bool getInterpolatedVoxel(const float xyz[3], float& voxelValue);
      
      /// get the voxel to surface distances (used by surface and volume rendering)
      float* getVoxelToSurfaceDistances();
      
      /// get the voxel to surface distances valid
      bool getVoxelToSurfaceDistancesValid() const { return voxelToSurfaceDistancesValid; }
      
      /// set the voxel to surface distances valid
      void setVoxelToSurfaceDistancesValid(const bool valid) { voxelToSurfaceDistancesValid = valid; }
      
      /// get the data type 
      VOXEL_DATA_TYPE getVoxelDataType() const { return voxelDataType; }
      
      /// set the data type 
      void setVoxelDataType(const VOXEL_DATA_TYPE vdt);
      
      /// get the SPM ac position 
      void getSpmAcPosition(float ac[3]) const;
      
      /// set the ac position
      void setSpmAcPosition(const float ac[3]);
      
      /// get the number of components per voxel
      int getNumberOfComponentsPerVoxel() const { return numberOfComponentsPerVoxel; }
      
      /// get the dimensions of the volume (const method)
      void getDimensions(int& dimX, int& dimY, int& dimZ) const;
      
      /// get the dimensions of the volume (const method)
      void getDimensions(int dim[3]) const;
      
      /// get the dimensions of the volume
      void getDimensions(int dim[3]);
      
      /// set the dimensions of the volume
      void setDimensions(const int dim[3]);
      
      /// get the origin
      void getOrigin(float originOut[3]) const;
      
      /// set the origin
      void setOrigin(const float originIn[3]);
      
      /// get the voxel spacing for the volume
      void getSpacing(float spacingOut[3]) const;
      
      /// set the voxel spacing for for the volume
      void setSpacing(const float spacingIn[3]);
      
      /// get the orientation
      void getOrientation(VolumeFile::ORIENTATION orient[3]) const;
  
      /// set the orientation
      void setOrientation(const VolumeFile::ORIENTATION orient[3]);

      /// Returns true if the orientation is valid, else false.
      static bool isValidOrientation(const VolumeFile::ORIENTATION orient[3]);

      /// Get the inverse orientation for an orientation (ie "left" is 
      /// opposite of "right")
      static VolumeFile::ORIENTATION  getInverseOrientation(const VolumeFile::ORIENTATION orient);

      /// Permute (reorient) the volume to the specified orientation.
      void permuteToOrientation(const VolumeFile::ORIENTATION newOrientation[3])
                                                         throw (FileException);      
      
      /// Get the volume extent of non-zero voxels.  Return voxel index range of non-zero voxels.
      void getNonZeroVoxelExtent(int extent[6]) const;
      
      /// get the volume's data file name
      QString getDataFileName() const { return dataFileName; }
      
      /// set the volume's data file name
      void setDataFileName(const QString& name) { dataFileName = name; }

      /// get the volume's descriptive label
      QString getDescriptiveLabel() const;
      
      /// set the volume's descriptive label
      void setDescriptiveLabel(const QString& s) { descriptiveLabel = s; }
      
      /// get the number of sub volumes
      int getNumberOfSubVolumes() const { return numberOfSubVolumes; }
      
      /// get the sub volume names
      void getSubVolumeNames(std::vector<QString>& names) const;

      /// get the volume file's type for reading
      FILE_READ_WRITE_TYPE getFileReadType() const { return fileReadType; }
      
      // /// set the volume file's type for reading (also defaults write type)
      //void setFileReadType(const FILE_READ_WRITE_TYPE ft) { fileReadType = ft;
      //                                                      fileWriteType = ft; }
      
      /// get the volume file's type for writing
      FILE_READ_WRITE_TYPE getFileWriteType() const { return fileWriteType; }
      
      /// set the volume file's type for writing
      void setFileWriteType(const FILE_READ_WRITE_TYPE ft);
      
      /// flip volume about an axis
      void flip(const VOLUME_AXIS axis, const bool updateOrientation = true);
      
      /// rotate volume about an axis
      void rotate(const VOLUME_AXIS axis);
      
      /// rescale voxel values (input min/max mapped to output min/max)
      void rescaleVoxelValues(const float inputMinimum,
                              const float inputMaximum,
                              const float outputMinimum,
                              const float outputMaximum);
                              
      /// Scale the voxel values.
      void scaleVoxelValues(const float scale, const float minimumValueAllowed,
                            const float maximumValueAllowed);
                            
      /// Scale Anatomy and Segmentation Volumes to be 0 to 255 
      /// BUT ONLY if the maximum value is 1.0 or less.
      void scaleAnatomyAndSegmentationVolumesTo255();

      /// assign voxels within border for paint volumes
      void assignVoxelsWithinBorder(const VOLUME_AXIS axis,
                                    const QString& paintName,
                                    const Border* border,
                                    const int slicesAboveAndBelowPlane);
      
      /// set highlight a region name
      void setHighlightRegionName(const QString& name,
                                  const bool highlightItFlag);
                                  
      /// get a region is highlighted
      bool getHighlightRegionNameByIndex(const int indx) const;
      
      /// clear region highlighting
      void clearRegionHighlighting();
      
      /// get the number of region names
      int getNumberOfRegionNames() const { return regionNames.size(); }
      
      /// get a region name by its index
      QString getRegionNameFromIndex(const int index) const;
      
      /// get the index of a region name
      int getRegionIndexFromName(const QString& name) const;
      
      /// delete all region names
      void deleteAllRegionNames();
      
      /// add a region name (returns its index)
      int addRegionName(const QString& name);
      
      /// synchronize the region names in the volumes (index X is always region Y)
      static void synchronizeRegionNames(std::vector<VolumeFile*>& volumeFiles);
      
      /// Get the ranges of the voxel values for a specific column.
      void getMinMaxVoxelValues(float& minVoxelValue, float& maxVoxelValue);

      /// Get the value of the 2% and 98% voxels.
      void getTwoToNinetyEightPercentMinMaxVoxelValues(float& minVoxelValue, float& maxVoxelValue); 
      
      /// get a histogram of the voxels (assumes one component per voxel)
      /// user must delete returned histogram
      StatisticHistogram* getHistogram(const int numBuckets = 256,
                              const float excludeLeftPercent = 0.0,
                              const float excludeRightPercent = 0.0) const;

      /// get a histogram of the voxels
      void getHistogram(const int numBuckets,
                        std::vector<int>& histogram,
                        float& minHistoVoxelValue,
                        float& maxHistoVoxelValue);
                        
      /// import an analyze volume
      void importAnalyzeVolume(const QString& fileName) throw (FileException);
      
      /// import a minc volume
      void importMincVolume(const QString& fileName) throw (FileException);
      
      /// import a vtk structured points volume
      void importVtkStructuredPointsVolume(const QString& fileName) throw (FileException);
      
      /// export a vtk structured points volume
      void exportVtkStructuredPointsVolume(const QString& fileName) throw (FileException);
      
      /// export an analyze volume
      void exportAnalyzeVolume(const QString& fileName) throw (FileException);
      
      /// export an minc volume
      void exportMincVolume(const QString& fileName) throw (FileException);
      
      /// read the file header and its first subvolume data
      void readFile(const QString& fileNameIn) throw (FileException);
      
      /// read the file header and the specified subvolume data
      void readFile(const QString& fileNameIn, 
                    const int subVolumeNumber,
                    const bool spmRightIsOnLeft = false) throw (FileException);
      
      /// read the specified sub-volumes in a volume file
      static void readFile(const QString& fileNameIn, 
                           const int subVolumeNumber,
                           std::vector<VolumeFile*>& volumesReadOut,
                           const bool spmRightIsOnLeft = false) throw (FileException);
      
      /// read the names of the volume's sub-volumes
      static void readSubVolumeNames(const QString& fileNameIn,
                                     std::vector<QString>& subVolumeNamesOut) throw (FileException);
                                     
      /// write the specified sub-volumes
      static void writeFile(const QString& fileNameIn,
                            const VOLUME_TYPE volumeType,
                            const VOXEL_DATA_TYPE writeVoxelDataType,
                            std::vector<VolumeFile*>& volumesToWrite,
                            const bool zipAfniBrikFile = false) throw (FileException);
                            
      /// read a raw volume file that has no header
      void readFileVolumeRaw(const QString& name,
                             const int subVolumeNumber,
                             const VOXEL_DATA_TYPE vdt, 
                             const int dim[3],
                             const ORIENTATION orient[3],
                             const float org[3], 
                             const float space[3],
                             const bool byteSwap) throw (FileException);
      
      /// write the volume file
      void writeFile(const QString& filenameIn) throw (FileException);
      
      /// write a volume in the specified type and add the correct file extension 
      /// (assumes no file extension)
      static void writeVolumeFile(VolumeFile* vf,
                                  const FILE_READ_WRITE_TYPE fileType,
                                  const QString filenameWithoutExtension,
                                  const bool compressVolumeFileFlag,
                                  QString& fileNameOut,
                                  QString& dataFileNameOut) throw (FileException);
                            
      /// change a range of values to zero
      static void setRangeOfValuesToZero(const VolumeFile* inputVolume,
                                         VolumeFile* outputVolume,
                                         const float minValue,
                                         const float maxValue,
                                         const bool inclusiveRangeFlag)
                                                    throw (FileException);
                                  
      /// create a segmentation volume mask
      static void createSegmentationMask(const QString& outputFileName,
                                         const std::vector<QString>& inputFileNames,
                                         const int numberOfDilationIterations) throw (FileException);
                                         
      /// perform unary operations on a volume
      static void performUnaryOperation(const UNARY_OPERATION operation,
                                        const VolumeFile* inputVolume,
                                        VolumeFile* outputVolume,
                                        const float scalar)
                                                    throw (FileException);
                                   
      /// perform binary operations on a volume
      static void performMathematicalOperation(const VOLUME_MATH_OPERATION operation,
                                         const VolumeFile* inputVolumeA,
                                         const VolumeFile* inputVolumeB,
                                         const VolumeFile* inputVolumeC,
                                         VolumeFile* outputVolume)
                                                    throw (FileException);
      
      /// perform segmentation operation
      void performSegmentationOperation(const SEGMENTATION_OPERATION operation,
                                        const VOLUME_AXIS axis,
                                        const bool threeDimensionalFlag,
                                        const int ijkMin[3],
                                        const int ijkMax[3],
                                        const float newVoxelValue,
                                        const unsigned char newRgbValue[4],
                                        VolumeModification* modifiedVoxel = NULL);
       
      /// assing paint volume voxels
      void assignPaintVolumeVoxels(const QString& paintName,
                                   const int ijkMin[3],
                                   const int ijkMax[3],
                                   VolumeModification* modifiedVoxel = NULL);
                                   
      /// get the types of volumes and names available for writing
      static void getVolumeFileTypesAndNames(std::vector<FILE_READ_WRITE_TYPE>& fileTypes,
                                             std::vector<QString>& fileTypeNames);
                                             
      /// Undo a volume modification
      void undoModification(const VolumeModification* modifiedVoxel);
      
      /// resample the image to the specified spacing
      void resampleToSpacing(const float newSpacing[3],
                             const INTERPOLATION_TYPE interpolationType);
      
      /// apply a transformation matrix to a volume
      void applyTransformationMatrix(vtkTransform* transform);

      /// apply a transformation matrix to a volume
      void applyTransformationMatrix(const TransformationMatrix& tm);

      /// get number of voxel elements (dims * components per voxel)
      int getTotalNumberOfVoxelElements() const;
      
      /// get number of voxel elements (dims only, does not include components per voxel)
      int getTotalNumberOfVoxels() const;
      
      /// compute a voxel index.
      inline int getVoxelDataIndex(const VoxelIJK& v, const int component = 0) const {
         const int indx = v.ijkv[0] + v.ijkv[1] * dimensions[0] + v.ijkv[2] * dimensions[0] * dimensions[1];
         const int compIndex = indx * numberOfComponentsPerVoxel + component;
         return compIndex;
      }

      /// compute a voxel data index
      inline int getVoxelDataIndex(const int ijk[3], const int component = 0) const {
         const int indx = ijk[0] + ijk[1] * dimensions[0] + ijk[2] * dimensions[0] * dimensions[1];
         const int compIndex = indx * numberOfComponentsPerVoxel + component;
         return compIndex;
      }
      
      /// compute a voxel data index
      inline int getVoxelDataIndex(const int i, const int j, const int k, 
                                   const int component = 0) const {
         const int indx = i + j * dimensions[0] + k * dimensions[0] * dimensions[1];
         const int compIndex = indx * numberOfComponentsPerVoxel + component;
         return compIndex;
      }
      
      /// compute a voxel color index.
      int getVoxelColorIndex(const VoxelIJK& v) const;

      /// compute a voxel color data index
      int getVoxelColorIndex(const int ijk[3]) const;
      
      /// compute a voxel color data index
      int getVoxelColorIndex(const int i, const int j, const int k) const;
      
      /// get the index of this voxel ignoring the components)
      int getVoxelNumber(const int i, const int j, const int k) const;
      
      /// get the index of this voxel ignoring the components)
      int getVoxelNumber(const VoxelIJK& v) const;
      
      /// get the index of this voxel ignoring the components)
      int getVoxelNumber(const int ijk[3]) const;
      
      /// convert coordinates into a voxel index (returns true if inside volume)
      bool convertCoordinatesToVoxelIJK(const float* xyz,
                                        int ikjOut[3],
                                        float offset[3]) const;
                                        
      /// convert coordinates into a voxel index (returns true if inside volume)
      bool convertCoordinatesToVoxelIJK(const float* xyz,
                                        int ikjOut[3]) const;
                                        
      /// convert to VTK structured points
      vtkStructuredPoints* convertToVtkStructuredPoints(const bool makeUnsignedCharType = false) const;
      
      /// convert from VTK structured points
      void convertFromVtkStructuredPoints(vtkStructuredPoints* sp);
      
      /// convert from VTK image data
      void convertFromVtkImageData(vtkImageData* sp);
      
      /// convert to VTK image data
      vtkImageData* convertToVtkImageData(const bool makeUnsignedCharType = false) const;
      
      /// bias correction using AFNI's 3dUniformize code
      void biasCorrectionWithAFNI(const int grayMinimum,
                                  const int whiteMaximum,
                                  const int numberOfIterations) throw (FileException);
                                  
      /// Dual hreshold volume (voxels between thresholds become 255, all other voxels 0).
      void dualThresholdVolume(const float thresholdLow, const float thresholdHigh);

      /// threshold volume (voxels above become 255, voxels below 0)
      void thresholdVolume(const float thresholdValue);
      
      /// threshold volume (voxels below become 255, voxels above 0)
      void inverseThresholdVolume(const float thresholdValue);

      /// Find each of the disconnect objects (islands) within the volume.
      void findObjectsWithinSegmentationVolume(std::vector<VoxelGroup>& objectsOut) const;
      
      /// Fill the largest connect set of voxels within the specified bounds
      bool fillBiggestObjectWithinMask(const int iminIn, 
                                       const int imaxIn, 
                                       const int jminIn, 
                                       const int jmaxIn, 
                                       const int kminIn, 
                                       const int kmaxIn,
                                       const float minValue,
                                       const float maxValue);
                                       
      /// Fill the largest connect set of voxels within the specified bounds
      bool fillBiggestObjectWithinMask(const int extent[6],
                                       const float minValue,
                                       const float maxValue);
                                       
      /// Find the largest connect set of voxels within the specified bounds
      int findBiggestObjectWithinMask(const int iminIn, 
                                       const int imaxIn, 
                                       const int jminIn, 
                                       const int jmaxIn, 
                                       const int kminIn, 
                                       const int kmaxIn,
                                       const float minValue,
                                       const float maxValue, 
                                       VoxelIJK& bigseed) const;
                                       
      /// Find the largest connect set of voxels within the specified bounds
      int findBiggestObjectWithinMask(const int extent[6],
                                       const float minValue,
                                       const float maxValue, 
                                       VoxelIJK& bigseed) const;
                                       
      /// Find the largest connect set of voxels within the specified bounds
      int findBiggestObjectWithinMask(const int extent[6],
                                       const float minValue,
                                       const float maxValue, 
                                       int bigseed[3]) const;
                                       
      /// Find the largest connect set of voxels in the volume
      int findBiggestObject(const float minValue,
                             const float maxValue, 
                             VoxelIJK& bigseed) const;
                                       
      /// find voxel with "value" that has not been searched
      bool findUnsearchedVoxel(const float minValue,
                              const float maxValue, 
                              VOXEL_SEARCH_STATUS searchFlags[],
                              VoxelIJK& seedOut) const;


      /// clamp a voxel index to within valid values (0 to dim-1)
      void clampVoxelIndex(const VOLUME_AXIS axis,
                           int& voxelIndex) const;
      
      /// clamp a voxel index to within valid values (0 to dim-1)
      void clampVoxelIndex(int voxelIJK[3]) const;
      
      /// clamp a voxel dimensions to within valid values (0 to dim)
      void clampVoxelDimension(const VOLUME_AXIS axis,
                               int& voxelIndex) const;
      
      /// clamp a voxel dimensions to within valid values (0 to dim)
      void clampVoxelDimension(int voxelIJK[3]) const;
      
      /// get 6 connected neighbors for a voxel
      void getNeighbors(const VoxelIJK& voxel, std::vector<VoxelIJK>& neighbors) const;
      
      /// flood fill slice with VTK
      void floodFillSliceWithVTK(const VolumeFile::VOLUME_AXIS axis,
                            const int seed[3],
                            const int connectedValueIn,
                            const int connectedValueOut,
                            const int unconnectedValueOut,
                            VolumeModification* modifiedVoxels = NULL);
                            
      /// flood fill with VTK
      void floodFillWithVTK(const VoxelIJK& seedVoxel,
                            const int connectedValueIn,
                            const int connectedValueOut,
                            const int unconnectedValueOut,
                            VolumeModification* modifiedVoxels = NULL);
                            
      /// flood fill with VTK
      void floodFillWithVTK(const int seed[3],
                            const int connectedValueIn,
                            const int connectedValueOut,
                            const int unconnectedValueOut,
                            VolumeModification* modifiedVoxels = NULL);
                            
      /// remove islands (all but the largest connected piece of surface)
      void removeIslandsFromSegmentation();
      
      /// Find non-zero voxel extent and write limits file if filename is not isEmpty
      void findLimits(const QString& limitfileName, int extent[6]);
      
      /// Shift the volume "offset" voxels in the specified axis
      void shiftAxis(const VOLUME_AXIS axis, const int offset);
      
      /// Smear voxel data along a specified axis
      void smearAxis(const VOLUME_AXIS axis, 
                         const int mag, 
                         const int sign, 
                         const int core) throw (FileException);

      /// Stretch the voxels in the volume to range 0 to 255.
      void stretchVoxelValues();

      /// Stretch the voxel values and exlucde top and bottom percentage (0 to 100)
      void stretchVoxelValuesExcludePercentage(const float bottomPercentToExclude,
                                               const float topPercentToExclude);
                                               
      /// Copy a volume but only copy voxels within a specified region.
      void maskVolume(const int limitsIn[6]);
      
      /// apply a volume mask to "this" volume using STEREOTAXIC coordinates
      void maskWithVolume(const VolumeFile* maskVolume) throw (FileException);
      
      /// voxels in "this" volume are set to "newVoxelValue" if the corresponding voxel
      /// value in "maskVolume" is within the min and max values
      void maskWithVolumeThreshold(const VolumeFile* maskVolume,
                                   const float maskMinimumVoxelValue,
                                   const float maskMaximumVoxelValue,
                                   const float newVoxelValue) throw (FileException);
                                
      /// Generate a Cerebral Hull volume from "this" segmentation volume
      void createCerebralHullVolume(VolumeFile& cerebralHullOut) const;
      
      /// ???
      void sculptVolume(const int  sculptMode, const VolumeFile* vol2, 
                       const int numsteps, int seed[3], int limits[6]);
                       
      /// ???
      void imposeLimits(const int limits[6]);

      /// Get the neighbors for a node given one-dimensional offsets.
      int computeNeighbors(const int idx, 
                            const int neighborOffsets[], 
                            const int numNeighs,
                            int neighsOut[]) const;   
   
      /// dilation and erosion
      void doVolMorphOps(const int nDilation, const int nErosion);
      
      /// dilation and erosion within mask
      void doVolMorphOpsWithinMask(const int extent[6], const int nDilation, const int nErosion);
      
      /// ??
      int stripBorderVoxels(const int neighborOffsets[],
                            int numNeighs, 
                            VolumeFile *vol2);
      
      /// ??
      int stripBorderVoxels(const int neighborOffsets[], 
                            const int numNeighs);
      
      /// ??
      void makeShellVolume(const int Ndilation, 
                           const int Nerosion);
                           
      /// ???
      void makePlane(const float xslope, 
                     const float xoffset, 
                     const float yslope, 
                     const float yoffset, 
                     const float zslope, 
                     const float zoffset, 
                     const float offset, 
                     const float thickness);
      
      /// ???
      void	classifyIntensities(const float mean, 
                                const float low, 
                                const float high, 
                                const float signum);
      
      /// blue a volume
      void blur();
      
      ///
      static void seperableConvolve(int ncol, int nrow, int nslices, 
	                              	float *volume, float *filter);
                                    
      ///
      static void oneDimConvolve(float *voxel, float *tempResult, float *filter, 
                         const int dim, const int inc,
                         const int ncol, const int nrow, const int nslices);
      
      /// voxel 6 connected to seed are found in "this" volume
      /// and set to "markValue" in the "mark" volume
      void breadthFirstFloodFill(const VoxelIJK& seed, 
                                 const float valueToFind, 
                                 VolumeFile *markVolume,
                                 const float markValue,
                                 const bool onlyDoUnmarkedVoxels = false) const;
      
      /// Fill internal cavities in a segmentation volume.
      void fillSegmentationCavities(const VolumeFile* maskVolumeIn = NULL);

      /// Fill internal cavities in a segmentation volume but only one slice.
      void fillSegmentationCavitiesInSingleSlice(const VOLUME_AXIS axis,
                                                 const int sliceNumber);

      /// invert the voxels in a segmentation volume
      void invertSegmentationVoxels();
      
      /// get the number of objects in a segmentation volume
      int getNumberOfSegmentationObjects() const;
      
      /// get the number of objects in a segmentation volume
      int getNumberOfSegmentationObjectsSubVolume(const int extent[6]) const;
      
      /// get the number of cavities in a segmentation volume
      int getNumberOfSegmentationCavities() const;
      
      /// get the number of cavities in a segmentation volume
      int getNumberOfSegmentationCavitiesSubVolume(const int extent[6]) const;
      
      /// get the euler data for this volume
      void getEulerCountsForSegmentationVolume(int& numberOfObjects,
                                               int& numberOfCavities,
                                               int& numberOfHoles,
                                               int& eulerCount) const;
                         
      /// get the euler data for this volume
      void getEulerCountsForSegmentationSubVolume(int& numberOfObjects,
                                               int& numberOfCavities,
                                               int& numberOfHoles,
                                               int& eulerCount,
                                               const int extent[6]) const;
                         
      /// determine the euler number of a segmentation volume
      int getEulerNumberForSegmentationVolume() const;
      
      /// determine the euler number of a subvolume of segmentation volume
      int getEulerNumberForSegmentationSubVolume(const int extent[6]) const;
      
      /// make all voxels in a segmentation volume 0 or 255
      void makeSegmentationZeroTwoFiftyFive();
      
      /// make a sphere within the volume
      void makeSphere(const int center[3], const float radius);

      /// AC-PC align a volume
      void acPcAlign(const int superiorAcVoxel[3],
                     const int inferiorPcVoxel[3],
                     const int superiorLateralFissureVoxel[3]) throw (FileException);
      
      /// see if the data file was zipped (compressed)
      bool getDataFileWasZipped() const { return dataFileWasZippedFlag; }
      
      /// perform uniformity correction with AFNI 3duniformize
      void afniUniformityCorrection(const int grayMin = 70,
                                    const int whiteMax = 210,
                                    const int iterations = 5) throw (FileException);
                                    
      /// get study meta data link
      //StudyMetaDataLinkSet getStudyMetaDataLinkSet() const { return studyMetaDataLinkSet; }

      // set study meta data link
      //void setStudyMetaDataLinkSet(const StudyMetaDataLinkSet& smdls);
      
      //************************************************************************
      //
      // The following methods require ITK
      // They will throw an exception if ITK was not available.
      //
      //************************************************************************

      /// convert from ITK Image
      void convertFromITKImage(VolumeITKImage& itkImageIn) throw (FileException);
      
      /// convert to ITK Image
      void convertToITKImage(VolumeITKImage& itkImageOut) throw (FileException);

      //************************************************************************
      //
      // The previous methods require ITK
      // They will throw an exception if ITK was not available.
      //
      //************************************************************************

   protected:
      /// read the specified sub-volumes in a volume file
      static void readFileAfni(const QString& fileNameIn, 
                           const int readSelection,
                           std::vector<VolumeFile*>& volumesReadOut) throw (FileException);
      
      /// read the specified sub-volumes in a volume file
      static void readFileAnalyze(const QString& fileNameIn, 
                           const int readSelection,
                           std::vector<VolumeFile*>& volumesReadOut,
                           const bool spmFlag = false) throw (FileException);
      
      /// read the specified sub-volumes in a volume file
      static void readFileNifti(const QString& fileNameIn, 
                           const int readSelection,
                           std::vector<VolumeFile*>& volumesReadOut) throw (FileException);
      
      /// read the specified sub-volumes in an SPM volume file
      static void readFileSpm(const QString& fileNameIn, 
                           const int readSelection,
                           std::vector<VolumeFile*>& volumesReadOut,
                           const bool rightIsOnLeft = false) throw (FileException);
      
      /// read the specified sub-volumes in a Wash U. volume file
      static void readFileWuNil(const QString& fileNameIn, 
                           const int readSelection,
                           std::vector<VolumeFile*>& volumesReadOut) throw (FileException);
      
      /// write the specified AFNI sub-volumes
      static void writeFileAfni(const QString& fileNameIn,
                            const VOXEL_DATA_TYPE writeVoxelDataType,
                            std::vector<VolumeFile*>& volumesToWrite,
                            const bool zipAfniBrikFile = false) throw (FileException);
                            
      /// write the specified Analyze sub-volumes
      static void writeFileAnalyze(const QString& fileNameIn,
                            const VOXEL_DATA_TYPE writeVoxelDataType,
                            std::vector<VolumeFile*>& volumesToWrite) throw (FileException);
                            
      /// write the specified NIFTI sub-volumes
      static void writeFileNifti(const QString& fileNameIn,
                            const VOXEL_DATA_TYPE writeVoxelDataType,
                            std::vector<VolumeFile*>& volumesToWrite) throw (FileException);
                            
      /// write the specified SPM sub-volumes
      static void writeFileSPM(const QString& fileNameIn,
                            const VOXEL_DATA_TYPE writeVoxelDataType,
                            std::vector<VolumeFile*>& volumesToWrite,
                            const bool analyzeFlag = false) throw (FileException);
                            
      /// write the specified WU NIL sub-volumes
      static void writeFileWuNil(const QString& fileNameIn,
                            const VOXEL_DATA_TYPE writeVoxelDataType,
                            std::vector<VolumeFile*>& volumesToWrite) throw (FileException);
                            
      /// read the volume data
      void readVolumeFileData(const bool byteSwapNeeded,
                              const float scaleFact,
                              const float offsetFact,
                              gzFile dataFile) throw (FileException);
      
      /// read the volume data
      void readVolumeFileDataSubVolume(const bool byteSwapNeeded,
                                       const float scaleFact,
                                       const float offsetFact,
                                       const long dataOffset,
                                       const int subVolumeNumber,
                                       gzFile dataFile) throw (FileException);
      
      /// write the volume data
      void writeVolumeFileData(const VOXEL_DATA_TYPE voxelDataTypeForWriting,
                               const bool byteSwapNeeded,
                               const bool compressDataWithZlib,
                               gzFile zipStream,
                               std::ofstream* cppStream) throw (FileException);
                                
      /// copy volume data (used by copy contructor and assignment operator)
      void copyVolumeData(const VolumeFile& vf,
                          const bool copyVoxelData = true);
      
      /// Flood fill starting at a voxel or remove a connected set of voxels.
      void floodFillAndRemoveConnected(const SEGMENTATION_OPERATION operation,
                                       const VOLUME_AXIS axis,
                                       const int ijkStart[3],
                                       const float voxelValue,
                                       const unsigned char rgbValue[4],
                                       VolumeModification* modifiedVoxels = NULL);
      
      /// Allocate the voxel coloring
      void allocateVoxelColoring();
      
      /// Set all voxels' colors invalid if the global coloring invalid flag is set
      void checkForInvalidVoxelColors();

      /// initialize the sub volume information
      void initializeSubVolumes(const int num);
      
      /// Read the spec file data (should never be called)
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the spec file data (should never be called)
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Read voxels from the data file
      void readCharData(gzFile dataFile) throw (FileException);
      
      /// Read voxels from the data file
      void readUnsignedCharData(gzFile dataFile) throw (FileException);
      
      /// Read voxels from the data file
      void readShortData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file
      void readUnsignedShortData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file
      void readIntData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file 
      void readUnsignedIntData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file 
      void readLongLongData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file
      void readUnsignedLongLongData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file
      void readFloatData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file
      void readDoubleData(gzFile dataFile, const bool byteSwapData) throw (FileException);
      
      /// Read voxels from the data file
      void readRgbDataVoxelInterleaved(gzFile dataFile) throw (FileException);
      
      /// Read voxels from the data file
      void readRgbDataSliceInterleaved(gzFile dataFile) throw (FileException);
      
      /// Get a double attribute from a minc file.
      void get_minc_attribute(int mincid, char *varname, char *attname, 
                              int maxvals, double vals[]);
      
      /// create the euler table
      static void createEulerTable();
      
      /// compute the euler value for a voxel.
      int computeEulerOctant(const int i, const int j, const int k, const int D[3]) const;

      /// volume space for reading of volumes
      static VOLUME_SPACE volumeSpace;
      
      /// the type of volume
      VOLUME_TYPE volumeType;

      /// type of volume file for reading
      FILE_READ_WRITE_TYPE fileReadType;
      
      /// type of volume file for writing
      FILE_READ_WRITE_TYPE fileWriteType;
      
      /// the data file's name
      QString dataFileName;
      
      /// voxel data type
      VOXEL_DATA_TYPE voxelDataType;
      
      /// SPM AC position
      float spmAcPosition[3];
      
      /// scale slope used when reading (voxels multiplied by this)
      std::vector<float> scaleSlope;
      
      /// scale offset used when reading (added to voxels)
      std::vector<float> scaleOffset;
      
      /// orientation of volume
      ORIENTATION orientation[3];
      
      /// names of sub volumes
      std::vector<QString> subVolumeNames;
      
      /// number of sub volumes
      int numberOfSubVolumes;
      
      /// mode read reading volume data
      VOLUME_DATA_READ_MODE volumeDataReadMode;
      
      /// the afni header
      AfniHeader afniHeader;

      /// the wunil header
      WuNilHeader wunilHeader;
      
      /// the region names
      std::vector<QString> regionNames;

      /// indices of highlighted region names
      std::vector<int> regionNameHighlighted;
      
      /// the voxel colors (3 components (RGB) per voxel)
      unsigned char* voxelColoring;
      
      /// voxel coloring valid
      bool voxelColoringValid;
      
      /// voxel distances
      float* voxelToSurfaceDistances;
      
      /// voxel distances valid
      bool voxelToSurfaceDistancesValid;
      
      /// dimensions of volume
      int dimensions[3];
      
      /// voxel sizes
      float spacing[3];
      
      /// coordinates for origin (first, typically most LPI voxel)
      float origin[3];
      
      /// number of components per voxel
      int numberOfComponentsPerVoxel;
      
      /// the voxels
      float* voxels;
      
      /// minimum voxel value
      float minimumVoxelValue;
      
      /// maximum voxel value
      float maximumVoxelValue;
      
      /// min/max voxel values valid
      bool minMaxVoxelValuesValid;
      
      /// 2% minimum voxel value
      float minMaxTwoPercentVoxelValue;
      
      /// 98% maximum voxel value
      float minMaxNinetyEightPercentVoxelValue;
      
      /// min/max 2% and 98% voxel values valid
      bool minMaxTwoToNinetyEightPercentVoxelValuesValid;
      
      /// data file was zipped flag
      bool dataFileWasZippedFlag;
      
      /// the volume's descriptive label
      QString descriptiveLabel;
      
      /// NIFTI read data offset
      unsigned long niftiReadDataOffset;
      
      /// NIFTI intention code and parameters
      QString niftiIntentCodeAndParamString;
      
      /// NIFTI intent name
      QString niftiIntentName;
      
      /// NIFTI intent code
      int niftiIntentCode;
      
      /// NIFTI intention parameter 1
      float niftiIntentParameter1;
      
      /// NIFTI intention parameter 2
      float niftiIntentParameter2;
      
      /// NIFTI intention parameter 3
      float niftiIntentParameter3;
      
      /// NIFTI TR
      float niftiTR;
      
      /// study meta data link
      //StudyMetaDataLinkSet studyMetaDataLinkSet;
      
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // IF NEW VARIABLES ADDED UPDATE copyVolumeData() !!!!!!!!!!!!!!!!!!
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      
      /// local neighbor indices (26 connected)
      static int localNeighbors[26][3];
      
      /// the euler table
      static float eulerTable[256];
      
      /// the euler table is valid
      static bool eulerTableValid;
};

#ifdef __VOLUME_FILE_MAIN_H__
bool VolumeFile::eulerTableValid = false;
float VolumeFile::eulerTable[256];

VolumeFile::VOLUME_SPACE VolumeFile::volumeSpace = VolumeFile::VOLUME_SPACE_COORD_LPI;
int VolumeFile::localNeighbors[26][3] = {
	{0, 0, 1},
	{0, 1, 0},
	{1, 0, 0},
	{-1, 0, 0},
	{0, -1, 0},
	{0, 0, -1},
	{1, 1, 1},
	{0, 1, 1},
	{-1, 1, 1},
	{1, 0, 1},
	{-1, 0, 1},
	{1, -1, 1},
	{0, -1, 1},
	{-1, -1, 1},
	{1, 1, 0},
	{-1, 1, 0},
	{1, -1, 0},
	{-1, -1, 0},
	{1, 1, -1},
	{0, 1, -1},
	{-1, 1, -1},
	{1, 0, -1},
	{-1, 0, -1},
	{1, -1, -1},
	{0, -1, -1},
	{-1, -1, -1}
};

#endif // __VOLUME_FILE_MAIN_H__

#endif // __VE_VOLUME_FILE_NEW_H__
