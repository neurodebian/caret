
#ifndef __BRAIN_MODEL_SURFACE_DEFORM_DATA_FILE_H__
#define __BRAIN_MODEL_SURFACE_DEFORM_DATA_FILE_H__

#include <QString>
#include <vector>

#include "BrainModelAlgorithmException.h"
#include "SpecFile.h"

class AbstractFile;
class BrainSet;
class DeformationMapFile;

/// This class is used to deform surface data files using a deformation map file
class BrainModelSurfaceDeformDataFile {
   public:
      /// data file types that are deformed
      enum DATA_FILE_TYPE {
         DATA_FILE_AREAL_ESTIMATION,
         DATA_FILE_ATLAS,
         DATA_FILE_BORDER_FLAT,
         DATA_FILE_BORDER_SPHERICAL,
         DATA_FILE_BORDER_PROJECTION,
         DATA_FILE_CELL,
         DATA_FILE_CELL_PROJECTION,
         DATA_FILE_COORDINATE,
         DATA_FILE_COORDINATE_FLAT,
         DATA_FILE_FOCI,
         DATA_FILE_FOCI_PROJECTION,
         DATA_FILE_LAT_LON,
         DATA_FILE_METRIC,
         DATA_FILE_PAINT,
         DATA_FILE_RGB_PAINT,
         DATA_FILE_SHAPE,
         DATA_FILE_TOPOGRAPHY
      };
      
      /// link color files to target from source
      static void linkColorFiles(const DeformationMapFile* dmf,
                                 const bool linkAreaColorFiles,
                                 const bool linkBorderColorFiles,
                                 const bool linkCellColorFiles,
                                 const bool linkFociColorFiles,
                                 QString& deformErrorsMessage)
                                       throw (BrainModelAlgorithmException);

      /// create a deformed spec file name
      static QString createDeformedSpecFileName(const QString& deformedFileNamePrefix,
                                                const QString& sourceSpecFileName,
                                                const QString& targetSpecFileName);

      /// create a deformed data file name
      static QString createDeformedFileName(
                                         const QString& deformingFromFileName,
                                         const QString& mySpecFileName,
                                         const QString& deformedFilePrefix,
                                         const int numberOfNodes,
                                         const bool deformationMapFileNameFlag);
       
      /// deform a border file
      static void deformBorderFile(BrainSet* sourceBrainSet,
                                   BrainSet* targetBrainSet,
                                   const DeformationMapFile* dmf,
                                   const bool useSourceTargetPathsFlag,
                                   const DATA_FILE_TYPE dataFileType,
                                   const QString& dataFileName,
                                   const QString& outputFileNameIn = "")
                                       throw (BrainModelAlgorithmException);
      
      /// deform border files
      static void deformBorderFiles(BrainSet* sourceBrainSet,
                                    BrainSet* targetBrainSet,
                                    const DeformationMapFile* dmf,
                                    const DATA_FILE_TYPE dataFileType,
                                    const SpecFile::Entry& dataFiles,
                                    QString& deformErrorsMessage)
                                             throw (BrainModelAlgorithmException);

      /// deform coordinate files
      static void deformCoordinateFiles(const DeformationMapFile* dmf,
                                        const SpecFile::Entry& dataFiles,
                                        QString& deformErrorsMessage)
                                          throw (BrainModelAlgorithmException);
                                 
      /// deform a coordinate file
      static void deformCoordinateFile(const DeformationMapFile* dmf,
                                       const QString& dataFileName,
                                       QString& outputFileNameInOut,
                                       const bool smoothCoordFileOneIteration,
                                       const bool useSourceTargetPathsFlag)
                                          throw (BrainModelAlgorithmException);

      /// deform flat coordinate files
      static void deformFlatCoordinateFiles(const DeformationMapFile* dmf,
                                            const SpecFile::Entry& dataFiles,
                                            QString& deformErrorsMessage)
                                          throw (BrainModelAlgorithmException);
                                       
      /// deform a flat coordinate file
      static void deformFlatCoordinateFile(const DeformationMapFile* dmf,
                                       const QString& atlasTopoFileName,
                                       const bool useSourceTargetPathsFlag,
                                       const QString& coordFileName,
                                       const QString& topoFileName,
                                       const QString& outputCoordFileNameIn = "",
                                       const QString& outputTopoFileName = "",
                                       const float maxLength = 10.0)
                                          throw (BrainModelAlgorithmException);

      /// deform a cell or foci file
      static void deformCellOrFociFile(BrainSet* sourceBrainSet,
                                       BrainSet* targetBrainSet,
                                       const DeformationMapFile* dmf,
                                       const bool useSourceTargetPathsFlag,
                                       const QString& dataFileName,
                                       const bool fociFileFlag,
                                       const QString& outputFileNameIn = "")
                                       throw (BrainModelAlgorithmException);
      
      /// deform cell or foci files
      static void deformCellOrFociFiles(BrainSet* sourceBrainSet,
                                       BrainSet* targetBrainSet,
                                       const DeformationMapFile* dmf,
                                       const SpecFile::Entry& dataFiles,
                                       const bool fociFileFlag,
                                       QString& deformErrorsMessage)
                                 throw (BrainModelAlgorithmException);
                                 
      /// deform a cell or foci projection file
      static void deformCellOrFociProjectionFile(BrainSet* sourceBrainSet,
                                       BrainSet* targetBrainSet,
                                       const DeformationMapFile* dmf,
                                       const bool useSourceTargetPathsFlag,
                                       const QString& dataFileName,
                                       const bool fociFileFlag,
                                       const QString& outputFileNameIn = "")
                                       throw (BrainModelAlgorithmException);
      
      /// deform cell or foci projection files
      static void deformCellOrFociProjectionFiles(BrainSet* sourceBrainSet,
                                       BrainSet* targetBrainSet,
                                       const DeformationMapFile* dmf,
                                       const SpecFile::Entry& dataFiles,
                                       const bool fociFileFlag,
                                       QString& deformErrorsMessage)
                                 throw (BrainModelAlgorithmException);
                                 
      /// deform a node attribute data file
      static void deformNodeAttributeFile(const DeformationMapFile* dmf,
                                          const DATA_FILE_TYPE dataFileType,
                                          const bool useSourceTargetPathsFlag,
                                          const QString& dataFileName,
                                          const QString& outputFileNameIn = "") 
                                             throw (BrainModelAlgorithmException);
                                         
      /// deform a nifti node data file
      static void deformGiftiNodeDataFile(const DeformationMapFile* dmf,
                                          const DATA_FILE_TYPE dataFileType,
                                          const bool useSourceTargetPathsFlag,
                                          const QString& dataFileName,
                                          const QString& outputFileNameIn = "") 
                                             throw (BrainModelAlgorithmException);
                                         
      /// deform node attribute data files
      static void deformNodeAttributeFiles(const DeformationMapFile* dmf,
                                           const DATA_FILE_TYPE dataFileType,
                                           const SpecFile::Entry& dataFiles,
                                           QString& deformErrorsMessage) 
                                              throw (BrainModelAlgorithmException);

      /// deform node attribute data files
      static void deformGiftiNodeDataFiles(const DeformationMapFile* dmf,
                                           const DATA_FILE_TYPE dataFileType,
                                           const SpecFile::Entry& dataFiles,
                                           QString& deformErrorsMessage) 
                                              throw (BrainModelAlgorithmException);

      /// Add comment information about deformation.
      static void addCommentAboutDeformation(const DeformationMapFile& dmf,
                                             const AbstractFile* inputFile,
                                             AbstractFile* outputFile);
                                                            
   protected:
      /// Help out with linking one type of color files.
      static void linkColorFileHelper(const SpecFile::Entry& colorFiles,
                                      const QString& sourceSpecFilePath,
                                      SpecFile& outputSpecFile);
                                      
      /// get a string containing names of surfaces loaded.
      static QString getLoadedSurfaces(BrainSet* bs);
};

#endif // __BRAIN_MODEL_SURFACE_DEFORM_DATA_FILE_H__

