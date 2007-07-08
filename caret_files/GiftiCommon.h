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
      
      /// attribute for reading/writing GIFTI files
      static const QString attVersion;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attArraySubscriptingOrder;
      
      /// attribute for reading/writing GIFTI files
      static const QString attIndex;
                                   
      /// attribute for reading/writing GIFTI files
      static const QString attDataLocation;
      
      /// attribute for reading/writing GIFTI files
      static const QString attCategory;
                                   
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
      
      /// data type name
      static const QString typeNameFloat32;
      
      /// data type name
      static const QString typeNameInt32;
      
      /// data type name
      static const QString typeNameUInt8;
      
      /// encoding type name
      static const QString encodingNameAscii;
      
      /// encoding type name
      static const QString encodingNameBase64Binary;
      
      /// encoding type name
      static const QString encodingNameBase64BinaryGZip;
      
      /// encoding type name
      static const QString endianBig;
      
      /// encoding type name
      static const QString endianLittle;
      
      /// data location internal name
      static const QString dataLocationInternal;
      
      /// data location external name
      static const QString dataLocationExternal;
      
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
      
      /// category for coordinates
      static const QString categoryCoordinates;
      
      /// category for functional
      static const QString categoryFunctional;
      
      /// category for normals
      static const QString categoryNormals;
      
      /// category for labels
      static const QString categoryLabels;
      
      /// category for rgba
      static const QString categoryRGBA;
      
      /// category for shape
      static const QString categoryShape;
      
      /// category for tensors
      static const QString categoryTensors;
      
      /// category for triangles
      static const QString categoryTopologyTriangles;
      
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

   const QString GiftiCommon::attVersion = "Version";
   const QString GiftiCommon::attArraySubscriptingOrder = "ArrayIndexingOrder";
   const QString GiftiCommon::attIndex = "Index";
   const QString GiftiCommon::attCategory = "Category";
   const QString GiftiCommon::attDataLocation = "DataLocation";
   const QString GiftiCommon::attDataType = "DataType";
   const QString GiftiCommon::attDimensionality = "Dimensionality";
   const QString GiftiCommon::attDim = "Dim";
   const QString GiftiCommon::attEncoding = "Encoding";
   const QString GiftiCommon::attEndian = "Endian";
   
   const QString GiftiCommon::typeNameFloat32 = "NIFTI_TYPE_FLOAT32";
   const QString GiftiCommon::typeNameInt32 = "NIFTI_TYPE_INT32";
   const QString GiftiCommon::typeNameUInt8 = "NIFTI_TYPE_UINT8";
   const QString GiftiCommon::encodingNameAscii = "ASCII";
   const QString GiftiCommon::encodingNameBase64Binary = "Base64Binary";
   const QString GiftiCommon::encodingNameBase64BinaryGZip = "GZipBase64Binary";
   const QString GiftiCommon::endianBig = "BigEndian";
   const QString GiftiCommon::endianLittle = "LittleEndian";
   const QString GiftiCommon::arraySubscriptingOrderHighestFirst = "HighestFirst";
   const QString GiftiCommon::arraySubscriptingOrderLowestFirst = "LowestFirst";
   const QString GiftiCommon::dataLocationInternal = "Internal";
   const QString GiftiCommon::dataLocationExternal = "External";
   
   const QString GiftiCommon::spaceLabelUnknown = "NIFTI_XFORM_UNKNOWN";
   const QString GiftiCommon::spaceLabelScannerCoords = "NIFTI_XFORM_SCANNER_ANAT";
   const QString GiftiCommon::spaceLabelAlignedAnatomical = "NIFTI_XFORM_ALIGNED_ANAT";
   const QString GiftiCommon::spaceLabelTalairach = "NIFTI_XFORM_TALAIRACH";
   const QString GiftiCommon::spaceLabelMNI152 = "NIFTI_XFORM_MNI_152";
   
   const QString GiftiCommon::categoryCoordinates = "Coordinates";
   const QString GiftiCommon::categoryFunctional = "Functional";
   const QString GiftiCommon::categoryNormals = "Normals";
   const QString GiftiCommon::categoryLabels = "Labels";
   const QString GiftiCommon::categoryRGBA = "RGBA";
   const QString GiftiCommon::categoryShape = "Shape";
   const QString GiftiCommon::categoryTensors = "Tensors";
   const QString GiftiCommon::categoryTopologyTriangles = "TopologyTriangles";
#endif // __GIFTI_COMMON_MAIN__

#endif // __GIFTI_COMMON_H__

