#ifndef __GIFTI_COMMON_H__
#define __GIFTI_COMMON_H__

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

#include <QString>
#include <QStringList>

class QTextStream;

/// class for things common to various GIFTI containers
class GiftiCommon {
   public:
      // get name for this system's endian
      static QString getSystemsEndianName();
      
      // used to write indentation space
      static void writeIndentationXML(QTextStream& stream,
                                      const int indentOffset);
                                   
      /// get attribute for a specific dimensions
      static QString getAttDim(const int dimNum);
      
      /// get all space labels
      static void getAllSpaceLabels(QStringList& labels);
      
      /// tag for reading/writing GIFTI files
      static const QString tagGIFTI;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagMetaData;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagMD;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagName;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagValue;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagDataArray;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagData;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagLabelTable;
                                   
      /// tag for reading/writing GIFTI files
      static const QString tagLabel;
                                 
      /// tag for reading/writing GIFTI files
      static const QString tagMatrix;
      
      /// tag for reading/writing GIFTI files
      static const QString tagMatrixDataSpace;
      
      /// tag for reading/writing GIFTI files
      static const QString tagMatrixTransformedSpace;
      
      /// tag for reading/writing GIFTI files
      static const QString tagMatrixData;
      
      /// tag for reading/writing GIFTI files
      //static const QString tagExternalFileName;
      
      /// tag for reading/writing GIFTI files
      //static const QString tagExternalFileOffset;

      /// attribute for reading/writing GIFTI files
      static const QString attVersion;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attNumberOfDataArrays;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attArraySubscriptingOrder;
      
      /// attribute for reading/writing GIFTI files
      static const QString attIndex;
                                   
      /// attribute for reading/writing GIFTI files
      //static const QString attDataLocation;
      
      /// attribute for reading/writing GIFTI files
      static const QString attIntent;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attDataType;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attDimensionality;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attDim;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attEncoding;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attEndian;
      
      /// attribute for reading/writing GIFTI files
      static const QString attExternalFileName;
      
      /// attribute for reading/writing GIFTI files
      static const QString attExternalFileOffset;
      
      /// data type name
      static const QString typeNameFloat32;
      
      /// data type name
      static const QString typeNameInt32;
      
      /// data type name
      static const QString typeNameUInt8;
      
      /// encoding type name
      static const QString encodingNameInternalAscii;
      
      /// encoding type name
      static const QString encodingNameInternalBase64Binary;
      
      /// encoding type name
      static const QString encodingNameInternalBase64BinaryGZip;
      
      /// encoding type name
      static const QString encodingNameExternalBinary;
      
      /// encoding type name
      static const QString endianBig;
      
      /// encoding type name
      static const QString endianLittle;
      
      /// data location internal name
      //static const QString dataLocationInternal;
      
      /// data location external name
      //static const QString dataLocationExternal;
      
      /// array subscripting highest order first 
      static const QString arraySubscriptingOrderHighestFirst;
      
      /// array subscripting lowest order first
      static const QString arraySubscriptingOrderLowestFirst;
      
      /// label for unknown space 
      static const QString spaceLabelUnknown;
      
      /// label for scanner space 
      static const QString spaceLabelScannerCoords;
      
      /// label for aligned to anatomical space 
      static const QString spaceLabelAlignedAnatomical;
      
      /// label for talairach space 
      static const QString spaceLabelTalairach;
      
      /// label for MNI 152 space 
      static const QString spaceLabelMNI152;
      
      /// nifti intent unknown
      static const QString intentUnknown;
   
      /// nifti intent prefix
      static const QString intentPrefix;
   
      /// intent for coordinates
      static const QString intentCoordinates;
      
      /// intent for time series
      static const QString intentTimeSeries;
      
      /// intent for normals
      static const QString intentNormals;
      
      /// intent for labels
      static const QString intentLabels;
      
      /// intent for rgba
      static const QString intentRGBA;
      
      /// intent for rgb
      static const QString intentRGB;
      
      /// intent for shape
      static const QString intentShape;
      
      /// intent for tensors
      static const QString intentTensors;
      
      /// intent for triangles
      static const QString intentTopologyTriangles;
      
      /// metadata unique id name
      static const QString metaDataNameUniqueID;
      
      /// topological type metadata name
      static const QString metaDataNameTopologicalType;
      
      /// topological type closed value
      static const QString metaDataValueTopologicalTypeClosed;
      
      /// topological type open value
      static const QString metaDataValueTopologicalTypeOpen;
      
      /// topological type cut value
      static const QString metaDataValueTopologicalTypeCut;
      
      /// topological type cut lobar value
      static const QString metaDataValueTopologicalTypeCutLobar;
      
      /// geometric type metadata name
      static const QString metaDataNameGeometricType;
      
      /// geometric type reconstruction value
      static const QString metaDataValueGeometricTypeReconstruction;
      
      /// geometric type anatomical value
      static const QString metaDataValueGeometricTypeAnatomical;
      
      /// geometric type inflated value
      static const QString metaDataValueGeometricTypeInflated;
      
      /// geometric type very inflated value
      static const QString metaDataValueGeometricTypeVeryInflated;
      
      /// geometric type spherical value
      static const QString metaDataValueGeometricTypeSpherical;
      
      /// geometric type spherical value
      static const QString metaDataValueGeometricTypeSemiSpherical;
      
      /// geometric type ellipsoid value
      static const QString metaDataValueGeometricTypeEllipsoid;
      
      /// geometric type flat value
      static const QString metaDataValueGeometricTypeFlat;
      
      /// geometric type flat lobar value
      static const QString metaDataValueGeometricTypeFlatLobar;
      
      /// geometric type hull value
      static const QString metaDataValueGeometricTypeHull;

      /// anatomical structure primary metadata name
      static const QString metaDataNameAnatomicalStructurePrimary;
      
      /// anatomical structure primary cortex left value
      static const QString metaDataValueAnatomicalStructurePrimaryCortexLeft;
      
      /// anatomical structure primary cortex right value
      static const QString metaDataValueAnatomicalStructurePrimaryCortexRight;
      
      /// anatomical structure primary cortex right and left value
      static const QString metaDataValueAnatomicalStructurePrimaryCortexRightAndLeft;
      
      /// anatomical structure primary cerebellum value
      static const QString metaDataValueAnatomicalStructurePrimaryCerebellum;
      
      /// anatomical structure primary head value
      static const QString metaDataValueAnatomicalStructurePrimaryHead;
      
      /// anatomical structure secondary metadata name
      static const QString metaDataNameAnatomicalStructureSecondary;
      
      /// anatomical structure secondary  white/gray boundary value
      static const QString metaDataValueAnatomicalStructureSecondaryWhiteGray;
      
      /// anatomical structure secondary gray/CSF boundary value
      static const QString metaDataValueAnatomicalStructureSecondarGrayCSF;
      
      /// anatomical structure secondary MidLayer value
      static const QString metaDataValueAnatomicalStructureSecondaryMidLayer;
      
};

#ifdef __GIFTI_COMMON_MAIN__
   const QString GiftiCommon::tagGIFTI  = "GIFTI";
   const QString GiftiCommon::tagMetaData = "MetaData";
   const QString GiftiCommon::tagMD = "MD";
   const QString GiftiCommon::tagName = "Name";
   const QString GiftiCommon::tagValue = "Value";
   const QString GiftiCommon::tagDataArray = "DataArray";
   const QString GiftiCommon::tagData = "Data";
   const QString GiftiCommon::tagLabelTable = "LabelTable";
   const QString GiftiCommon::tagLabel = "Label";
   const QString GiftiCommon::tagMatrix = "CoordinateSystemTransformMatrix";
   const QString GiftiCommon::tagMatrixDataSpace = "DataSpace";
   const QString GiftiCommon::tagMatrixTransformedSpace = "TransformedSpace";
   const QString GiftiCommon::tagMatrixData = "MatrixData";
   //const QString GiftiCommon::tagExternalFileName = "ExternalFileName";
   //const QString GiftiCommon::tagExternalFileOffset = "ExternalFileOffset";

   const QString GiftiCommon::attVersion = "Version";
   const QString GiftiCommon::attNumberOfDataArrays = "NumberOfDataArrays";
   const QString GiftiCommon::attArraySubscriptingOrder = "ArrayIndexingOrder";
   const QString GiftiCommon::attIndex = "Index";
   const QString GiftiCommon::attIntent = "Intent";
   //const QString GiftiCommon::attIntent = "Category";
   //const QString GiftiCommon::attDataLocation = "DataLocation";
   const QString GiftiCommon::attDataType = "DataType";
   const QString GiftiCommon::attDimensionality = "Dimensionality";
   const QString GiftiCommon::attDim = "Dim";
   const QString GiftiCommon::attEncoding = "Encoding";
   const QString GiftiCommon::attEndian = "Endian";
   const QString GiftiCommon::attExternalFileName = "ExternalFileName";
   const QString GiftiCommon::attExternalFileOffset = "ExternalFileOffset";
   
   const QString GiftiCommon::typeNameFloat32 = "NIFTI_TYPE_FLOAT32";
   const QString GiftiCommon::typeNameInt32 = "NIFTI_TYPE_INT32";
   const QString GiftiCommon::typeNameUInt8 = "NIFTI_TYPE_UINT8";
   const QString GiftiCommon::encodingNameInternalAscii = "ASCII";
   const QString GiftiCommon::encodingNameInternalBase64Binary = "Base64Binary";
   const QString GiftiCommon::encodingNameInternalBase64BinaryGZip = "GZipBase64Binary";
   const QString GiftiCommon::encodingNameExternalBinary = "ExternalFileBinary";
   const QString GiftiCommon::endianBig = "BigEndian";
   const QString GiftiCommon::endianLittle = "LittleEndian";
   const QString GiftiCommon::arraySubscriptingOrderHighestFirst = "RowMajorOrder";
   const QString GiftiCommon::arraySubscriptingOrderLowestFirst = "ColumnMajorOrder";
   //const QString GiftiCommon::dataLocationInternal = "Internal";
   //const QString GiftiCommon::dataLocationExternal = "External";
   
   const QString GiftiCommon::spaceLabelUnknown = "NIFTI_XFORM_UNKNOWN";
   const QString GiftiCommon::spaceLabelScannerCoords = "NIFTI_XFORM_SCANNER_ANAT";
   const QString GiftiCommon::spaceLabelAlignedAnatomical = "NIFTI_XFORM_ALIGNED_ANAT";
   const QString GiftiCommon::spaceLabelTalairach = "NIFTI_XFORM_TALAIRACH";
   const QString GiftiCommon::spaceLabelMNI152 = "NIFTI_XFORM_MNI_152";
   
   const QString GiftiCommon::intentUnknown = "NIFTI_INTENT_UNKNOWN";
   const QString GiftiCommon::intentPrefix = "NIFTI_INTENT";
   const QString GiftiCommon::intentCoordinates = "NIFTI_INTENT_POINTSET";
   const QString GiftiCommon::intentTimeSeries = "NIFTI_INTENT_TIME_SERIES";
   const QString GiftiCommon::intentNormals = "NIFTI_INTENT_VECTOR";
   const QString GiftiCommon::intentLabels = "NIFTI_INTENT_LABEL";
   const QString GiftiCommon::intentRGB = "NIFTI_INTENT_RGB_VECTOR";
   const QString GiftiCommon::intentRGBA = "NIFTI_INTENT_RGBA_VECTOR";
   const QString GiftiCommon::intentShape = "NIFTI_INTENT_SHAPE";
   const QString GiftiCommon::intentTensors = "NIFTI_INTENT_GENMATRIX";
   const QString GiftiCommon::intentTopologyTriangles = "NIFTI_INTENT_TRIANGLE";

   const QString GiftiCommon::metaDataNameUniqueID = "UniqueID";
   
   const QString GiftiCommon::metaDataNameTopologicalType = "TopologicalType";
   const QString GiftiCommon::metaDataValueTopologicalTypeClosed = "Closed";
   const QString GiftiCommon::metaDataValueTopologicalTypeOpen = "Open";
   const QString GiftiCommon::metaDataValueTopologicalTypeCut = "Cut";
   const QString GiftiCommon::metaDataValueTopologicalTypeCutLobar = "CutLobar";

   const QString GiftiCommon::metaDataNameGeometricType = "GeometricType";
   const QString GiftiCommon::metaDataValueGeometricTypeReconstruction = "Reconstruction";
   const QString GiftiCommon::metaDataValueGeometricTypeAnatomical = "Anatomical";
   const QString GiftiCommon::metaDataValueGeometricTypeInflated = "Inflated";
   const QString GiftiCommon::metaDataValueGeometricTypeVeryInflated = "VeryInflated";
   const QString GiftiCommon::metaDataValueGeometricTypeSpherical = "Spherical";
   const QString GiftiCommon::metaDataValueGeometricTypeSemiSpherical = "SemiSpherical";
   const QString GiftiCommon::metaDataValueGeometricTypeEllipsoid = "Ellipsoid";
   const QString GiftiCommon::metaDataValueGeometricTypeFlat = "Flat";
   const QString GiftiCommon::metaDataValueGeometricTypeFlatLobar = "FlatLobar";
   const QString GiftiCommon::metaDataValueGeometricTypeHull = "Hull";

   const QString GiftiCommon::metaDataNameAnatomicalStructurePrimary = "AnatomicalStructurePrimary";
   const QString GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexLeft = "CortexLeft";
   const QString GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexRight = "CortexRight";
   const QString GiftiCommon::metaDataValueAnatomicalStructurePrimaryCortexRightAndLeft = "CortexRightAndLeft";
   const QString GiftiCommon::metaDataValueAnatomicalStructurePrimaryCerebellum = "Cerebellum";
   const QString GiftiCommon::metaDataValueAnatomicalStructurePrimaryHead = "Head";
   const QString GiftiCommon::metaDataNameAnatomicalStructureSecondary = "AnatomicalStructureSecondary";
   const QString GiftiCommon::metaDataValueAnatomicalStructureSecondaryWhiteGray = "WhiteGray";
   const QString GiftiCommon::metaDataValueAnatomicalStructureSecondarGrayCSF = "GrayCSF";
   const QString GiftiCommon::metaDataValueAnatomicalStructureSecondaryMidLayer = "MidLayer";
#endif // __GIFTI_COMMON_MAIN__

#endif // __GIFTI_COMMON_H__

