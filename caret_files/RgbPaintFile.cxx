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

#include <iostream>

#define _RGB_PAINT_FILE_MAIN_
#include "RgbPaintFile.h"
#undef  _RGB_PAINT_FILE_MAIN_      

#include "DeformationMapFile.h"
#include "FileUtilities.h"
#include "RgbPaintFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

/**
 * The constructor
 */
RgbPaintFile::RgbPaintFile()
   : NodeAttributeFile("RGB Paint File",
                       SpecFile::getRgbPaintFileExtension(),
                       AbstractFile::FILE_FORMAT_ASCII,
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_READ_AND_WRITE, 
                       FILE_IO_NONE, 
                       FILE_IO_NONE)
{
   clear();
}

/**
 * The destructor
 */
RgbPaintFile::~RgbPaintFile()
{
   clear();
}

/**
 * append a node attribute file to this one but selectively load/overwrite columns
 * columnDestination is where naf's columns should be (-1=new, -2=do not load).
 */
void 
RgbPaintFile::append(NodeAttributeFile& /*naf*/, 
                     std::vector<int> /*columnDestination*/,
                     const FILE_COMMENT_MODE /*fcm*/) throw (FileException)
{
   throw FileException("Not implemented yet.");
}


/**
  * append an rgb paint file to this one
  */
void 
RgbPaintFile::append(NodeAttributeFile &naf) throw (FileException)
{
   RgbPaintFile& rpf = dynamic_cast<RgbPaintFile&>(naf);
   
   if (getNumberOfNodes() != rpf.getNumberOfNodes()) {
      throw FileException("Cannot append RGB paint, number of columns does not match.");
   }
      
   const int oldNumCols  = getNumberOfColumns();
   
   const int appendNumCols = rpf.getNumberOfColumns();

   //
   // Add the additional columns to the file
   //   
   addColumns(appendNumCols);
   
   //
   // Transfer column attributes
   //
   for (int n = 0; n < appendNumCols; n++) {
      setColumnComment(oldNumCols + n, rpf.getColumnComment(n));
      setColumnName(oldNumCols + n, rpf.getColumnName(n));
      setTitleRed(oldNumCols + n, rpf.getTitleRed(n));
      setTitleGreen(oldNumCols + n, rpf.getTitleGreen(n));
      setTitleBlue(oldNumCols + n, rpf.getTitleBlue(n));
      setCommentRed(oldNumCols + n, rpf.getCommentRed(n));
      setCommentGreen(oldNumCols + n, rpf.getCommentGreen(n));
      setCommentBlue(oldNumCols + n, rpf.getCommentBlue(n));
      float minScale, maxScale;
      rpf.getScaleRed(n, minScale, maxScale);
      setScaleRed(oldNumCols + n, minScale, maxScale);
      rpf.getScaleGreen(n, minScale, maxScale);
      setScaleGreen(oldNumCols + n, minScale, maxScale);
      rpf.getScaleBlue(n, minScale, maxScale);
      setScaleBlue(oldNumCols + n, minScale, maxScale);
   }
   
   //
   // Transfer rgb data
   //
   float r, g, b;
   for (int i = 0; i < numberOfNodes; i++) {
      for (int k = 0; k < appendNumCols; k++) {
         rpf.getRgb(i, k, r, g, b);
         setRgb(i, oldNumCols + k, r, g, b);
      }
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(naf);
}
 
/** 
 * add columns to this rgb paint file
 */
void
RgbPaintFile::addColumns(const int numberOfNewColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   const int totalColumns = numberOfColumns + numberOfNewColumns;
   
   //
   // Keep track of existing rgb paint data
   //
   const std::vector<float> redData   = redComponents;
   const std::vector<float> greenData = greenComponents;
   const std::vector<float> blueData  = blueComponents;
   
   //
   // Setup file for new number of columns (will expand space for column naming)
   //
   setNumberOfNodesAndColumns(numberOfNodes, totalColumns);
   
   //
   // Transfer existing rgb paint data
   //
   for (int i = 0; i < numberOfNodes; i++) {
      for (int j = 0; j < numberOfColumns; j++) {
         float r = 0.0, g = 0.0, b = 0.0;
         if (j < oldNumberOfColumns) {
            const int oldIndex = (oldNumberOfColumns * i) + j;
            r = redData[oldIndex];
            g = greenData[oldIndex];
            b = blueData[oldIndex];
         }
         setRgb(i, j, r, g, b);
      }
   }
   
   setModified();
}

/**
 * Add nodes to the file
 */
void
RgbPaintFile::addNodes(const int numberOfNodesToAdd)
{
   setNumberOfNodesAndColumns(numberOfNodes + numberOfNodesToAdd, numberOfColumns);
}

/**
 * clear the RGB Paint file
 */
void 
RgbPaintFile::clear()
{
   clearNodeAttributeFile();
   setNumberOfNodesAndColumns(0, 0);
}

/**
 * get the comments for red component
 */
QString RgbPaintFile::getCommentRed(const int columnNumber) const 
{ 
   //return StringUtilities::setupCommentForDisplay(redComponentComment[columnNumber]); 
   return redComponentComment[columnNumber]; 
}

/**
 * get the comments for green component
 */
QString RgbPaintFile::getCommentGreen(const int columnNumber) const 
{ 
   //return StringUtilities::setupCommentForDisplay(greenComponentComment[columnNumber]); 
   return greenComponentComment[columnNumber]; 
}

/**
 * get the comments for blue component
 */
QString RgbPaintFile::getCommentBlue(const int columnNumber) const 
{ 
   //return StringUtilities::setupCommentForDisplay(blueComponentComment[columnNumber]); 
   return blueComponentComment[columnNumber]; 
}
   

/**
 * Deform the rgb paint file.
 */
void
RgbPaintFile::deformFile(const DeformationMapFile& dmf, 
                     NodeAttributeFile& deformedFile,
                     const DEFORM_TYPE /*dt*/) const throw (FileException)
{
   RgbPaintFile& deformedRgbPaintFile = dynamic_cast<RgbPaintFile&>(deformedFile);
   
   const int numNodes = dmf.getNumberOfNodes();
   
   deformedRgbPaintFile.setNumberOfNodesAndColumns(numNodes, getNumberOfColumns());
   
   //
   // Transfer stuff in AbstractFile and NodeAttributeFile
   //
   transferFileDataForDeformation(dmf, deformedRgbPaintFile);
   
   // 
   // transfer the descriptive data
   //
   for (int j = 0; j < getNumberOfColumns(); j++) {
      deformedRgbPaintFile.setTitleRed(j, getTitleRed(j));
      deformedRgbPaintFile.setTitleGreen(j, getTitleGreen(j));
      deformedRgbPaintFile.setTitleBlue(j, getTitleBlue(j));
      
      deformedRgbPaintFile.setCommentRed(j,  getCommentRed(j));
      deformedRgbPaintFile.setCommentGreen(j, getCommentGreen(j));
      deformedRgbPaintFile.setCommentBlue(j, getCommentBlue(j));
      
      float minScale, maxScale;
      getScaleRed(j, minScale, maxScale);
      deformedRgbPaintFile.setScaleRed(j, minScale, maxScale);
      
      getScaleGreen(j, minScale, maxScale);
      deformedRgbPaintFile.setScaleGreen(j, minScale, maxScale);
      
      getScaleBlue(j, minScale, maxScale);
      deformedRgbPaintFile.setScaleBlue(j, minScale, maxScale);
   }
   
   // 
   // transfer the node
   //
   int tileNodes[3];
   float tileAreas[3]; 
   for (int i = 0; i < numNodes; i++) {
      dmf.getDeformDataForNode(i, tileNodes, tileAreas);
      for (int j = 0; j < getNumberOfColumns(); j++) {
         float r = 0, g = 0, b = 0;
         if (tileNodes[0] > -1) {
            getRgb(tileNodes[0], j, r, g, b);
         }
         deformedRgbPaintFile.setRgb(i, j, r, g, b);
      }
   }
}

/**
 * reset a column of data.
 */
void 
RgbPaintFile::resetColumn(const int columnNumber)
{
   for (int i = 0; i < numberOfNodes; i++) {
      setRgb(i, columnNumber, 0.0, 0.0, 0.0);
   }
   setColumnName(columnNumber, "");
   setColumnComment(columnNumber, "");
   setTitleRed(columnNumber, "");
   setTitleGreen(columnNumber, "");
   setTitleBlue(columnNumber, "");
   setCommentRed(columnNumber, "");
   setCommentGreen(columnNumber, "");
   setCommentBlue(columnNumber, "");
   setScaleRed(columnNumber, 0.0, 0.0);
   setScaleGreen(columnNumber, 0.0, 0.0);
   setScaleBlue(columnNumber, 0.0, 0.0);
   setModified();
}

/**
 * remove a column of data.
 */
void 
RgbPaintFile::removeColumn(const int columnNumber)
{
   if (numberOfColumns <= 1) {
      clear();
      return;
   }
   
   //
   // Transfer column attributes
   //
   int ctr = 0;
   for (int n = 0; n < numberOfColumns; n++) {
      if (n != columnNumber) {
         setColumnComment(ctr, getColumnComment(n));
         setColumnName(ctr, getColumnName(n));
         setTitleRed(ctr, getTitleRed(n));
         setTitleGreen(ctr, getTitleGreen(n));
         setTitleBlue(ctr, getTitleBlue(n));
         setCommentRed(ctr, getCommentRed(n));
         setCommentGreen(ctr, getCommentGreen(n));
         setCommentBlue(ctr, getCommentBlue(n));
         float minScale, maxScale;
         getScaleRed(n, minScale, maxScale);
         setScaleRed(ctr, minScale, maxScale);
         getScaleGreen(n, minScale, maxScale);
         setScaleGreen(ctr, minScale, maxScale);
         getScaleBlue(n, minScale, maxScale);
         setScaleBlue(ctr, minScale, maxScale);
         ctr++;
      }
   }
   
   //
   // Transfer rgb data
   //
   RgbPaintFile tempRGB;
   tempRGB.setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1);   
   float r, g, b;
   for (int i = 0; i < numberOfNodes; i++) {
      int ctr = 0;
      for (int k = 0; k < numberOfColumns; k++) {
         if (k != columnNumber) {
            getRgb(i, k, r, g, b);
            tempRGB.setRgb(i, ctr, r, g, b);
            ctr++;
         }
      }
   }
   
   setNumberOfNodesAndColumns(numberOfNodes, numberOfColumns - 1); 
   redComponents = tempRGB.redComponents;
   greenComponents = tempRGB.greenComponents;
   blueComponents = tempRGB.blueComponents;
   setModified();
}

/**
 * Get the rgb values for a node
 */
void 
RgbPaintFile::getRgb(const int nodeNumber, const int columnNumber,
                     float& red, float& green, float& blue) const
{
   const int index = getOffset(nodeNumber, columnNumber);
   red   = redComponents[index];
   green = greenComponents[index];
   blue  = blueComponents[index];
}
            
/**
 * Get the red scale.
 */
void 
RgbPaintFile::getScaleRed(const int columnNumber, float& minScale, float& maxScale) const
{
   minScale = redComponentScaleMin[columnNumber];
   maxScale = redComponentScaleMax[columnNumber];
}

/**
 * Get the green scale.
 */
void 
RgbPaintFile::getScaleGreen(const int columnNumber, float& minScale, float& maxScale) const
{
   minScale = greenComponentScaleMin[columnNumber];
   maxScale = greenComponentScaleMax[columnNumber];
}

/**
 * Get the blue scale.
 */
void 
RgbPaintFile::getScaleBlue(const int columnNumber, float& minScale, float& maxScale) const
{
   minScale = blueComponentScaleMin[columnNumber];
   maxScale = blueComponentScaleMax[columnNumber];
}

/**
 * Set the RGB values for a node.
 */
void 
RgbPaintFile::setRgb(const int nodeNumber, const int columnNumber, 
                     const float red, const float green, const float blue)
{
   const int index = getOffset(nodeNumber, columnNumber);
   redComponents[index] = red;  
   greenComponents[index] = green;  
   blueComponents[index] = blue;  
   setModified();
}

/**
 * Set the RGB values for a node.
 */
void 
RgbPaintFile::setRgb(const int nodeNumber, const int columnNumber, 
                     const double red, const double green, const double blue)
{
   const int index = getOffset(nodeNumber, columnNumber);
   redComponents[index] = red;  
   greenComponents[index] = green;  
   blueComponents[index] = blue;  
   setModified();
}

/**
 * Set the red scale.
 */
void 
RgbPaintFile::setScaleRed(const int columnNumber, const float minScale, const float maxScale)
{
   redComponentScaleMin[columnNumber] = minScale;
   redComponentScaleMax[columnNumber] = maxScale;
   setModified();
}

/**
 * Set the green scale for a node.
 */
void 
RgbPaintFile::setScaleGreen(const int columnNumber, const float minScale, const float maxScale)
{
   greenComponentScaleMin[columnNumber] = minScale;
   greenComponentScaleMax[columnNumber] = maxScale;
   setModified();
}

/**
 * Set the blue scale for a node.
 */
void 
RgbPaintFile::setScaleBlue(const int columnNumber, const float minScale, const float maxScale)
{
   blueComponentScaleMin[columnNumber] = minScale;
   blueComponentScaleMax[columnNumber] = maxScale;
   setModified();
}

/**
 * Set the title for a red component
 */
void
RgbPaintFile::setTitleRed(const int columnNumber, const QString& title)
{
   redComponentTitle[columnNumber] = title;
   setModified();
}

/**
 * Set the title for a green component
 */
void
RgbPaintFile::setTitleGreen(const int columnNumber, const QString& title)
{
   greenComponentTitle[columnNumber] = title;
   setModified();
}

/**
 * Set the title for a blue component
 */
void
RgbPaintFile::setTitleBlue(const int columnNumber, const QString& title)
{
   blueComponentTitle[columnNumber] = title;
   setModified();
}

/**
 * set the comment for a red column
 */
void 
RgbPaintFile::setCommentRed(const int columnNumber, const QString& comment)
{
   //redComponentComment[columnNumber] = StringUtilities::setupCommentForStorage(comment));
   redComponentComment[columnNumber] = comment;
   setModified();
}

/**
 * set the comment for a green column
 */
void 
RgbPaintFile::setCommentGreen(const int columnNumber, const QString& comment)
{
   //greenComponentComment[columnNumber] = StringUtilities::setupCommentForStorage(comment));
   greenComponentComment[columnNumber] = comment;
   setModified();
}

/**
 * set the comment for a blue column
 */
void 
RgbPaintFile::setCommentBlue(const int columnNumber, const QString& comment)
{
   //blueComponentComment[columnNumber] = StringUtilities::setupCommentForStorage(comment));
   blueComponentComment[columnNumber] = comment;
   setModified();
}

/**
 *  Set the number of nodes.
 */
void 
RgbPaintFile::setNumberOfNodesAndColumns(const int numNodes, const int numColumns)
{
   const int oldNumberOfColumns = numberOfColumns;
   
   if ((numNodes == 0) || (numColumns == 0)) {
      redComponentTitle.clear();
      greenComponentTitle.clear();
      blueComponentTitle.clear();
      redComponentComment.clear();
      greenComponentComment.clear();
      blueComponentComment.clear();
      redComponentScaleMin.clear();
      greenComponentScaleMin.clear();
      blueComponentScaleMin.clear();
      redComponentScaleMax.clear();
      greenComponentScaleMax.clear();
      blueComponentScaleMax.clear();
      redComponents.clear();
      greenComponents.clear();
      blueComponents.clear();
   }
   
   numberOfNodes  = numNodes;
   numberOfColumns = numColumns;
   
   if (numberOfNodes > 0) {
      const int totalItems = numberOfNodes * numberOfColumns * numberOfItemsPerColumn;
      redComponents.resize(totalItems);
      greenComponents.resize(totalItems);
      blueComponents.resize(totalItems);
      for (int i = 0; i < totalItems; i++) {
         redComponents[i]   = 0.0;
         greenComponents[i] = 0.0;
         blueComponents[i]  = 0.0;
      }
      
      redComponentTitle.resize(numberOfColumns);
      greenComponentTitle.resize(numberOfColumns);
      blueComponentTitle.resize(numberOfColumns);
      redComponentComment.resize(numberOfColumns);
      greenComponentComment.resize(numberOfColumns);
      blueComponentComment.resize(numberOfColumns);
      redComponentScaleMin.resize(numberOfColumns);
      greenComponentScaleMin.resize(numberOfColumns);
      blueComponentScaleMin.resize(numberOfColumns);
      redComponentScaleMax.resize(numberOfColumns);
      greenComponentScaleMax.resize(numberOfColumns);
      blueComponentScaleMax.resize(numberOfColumns);
      
      for (int i = 0; i < numberOfColumns; i++) {
         if (i >= oldNumberOfColumns) {
            redComponentTitle[i] = "Red";
            greenComponentTitle[i] = "Green";
            blueComponentTitle[i] = "Blue";
            redComponentScaleMin[i]   = 0.0;
            redComponentScaleMax[i]   = 255.0;
            greenComponentScaleMin[i] = 0.0;
            greenComponentScaleMax[i] = 255.0;
            blueComponentScaleMin[i]  = 0.0;
            blueComponentScaleMax[i]  = 255.0;
         }
      }
   }

   numberOfNodesColumnsChanged();
   setModified();
}

/**
 * Import colors from a SUMA file.
 */
void
RgbPaintFile::importFromSuma(const QString& name) throw (FileException)
{
/*
 * Sample SUMS RGB file
 *
 
#FileContents = Node Colors
#RowFormat = n R G B
#N_Nodes = 137680
#Source = SUMA, surface lh.inflated.asc (idcode: XYZ_xKtbVhfo4zqCj8PwZ07D9Q)
0       0.373684        0.373684        0.373684
1       0.386842        0.386842        0.386842
2       0.360526        0.360526        0.360526

*/
   QFile file(name);
   if (file.open(QIODevice::ReadOnly) == false) {
      QString msg("Unable to open file named ");
      msg.append(name);
      throw FileException(name, msg);
   }
   
   QTextStream stream(&file);
   
   int columnNumber = -1;
   bool readData = false;
   bool gotScale = false;
   QString line;
   readLine(stream, line);
   while (stream.atEnd() == false) {
      if (columnNumber >= 0) {
         if (line.isEmpty() == false) {
            if (line[0] != '#') {
               std::vector<QString> tokens;
               StringUtilities::token(line, " \t", tokens);
               if (tokens.size() >= 4) {
                  setRgb(StringUtilities::toInt(tokens[0]), 
                         columnNumber,
                         StringUtilities::toFloat(tokens[1]),
                         StringUtilities::toFloat(tokens[2]),
                         StringUtilities::toFloat(tokens[3]));
                  //
                  // Suma RGB files appear to be scaled [0.0, 1.0]
                  //
                  if (gotScale == false) {
                     for (int i = 1; i <= 3; i++) {
                        const float val = StringUtilities::toFloat(tokens[1]);
                        if ((val > 0.0) && (val < 1.0)) {
                           redComponentScaleMin[columnNumber]   = 0.0;
                           redComponentScaleMax[columnNumber]   = 1.0;
                           greenComponentScaleMin[columnNumber] = 0.0;
                           greenComponentScaleMax[columnNumber] = 1.0;
                           blueComponentScaleMin[columnNumber]  = 0.0;
                           blueComponentScaleMax[columnNumber]  = 1.0;
                           gotScale = true;
                           break;
                        }
                     }
                  }
                  readData = true;
               }
            }
         }
      }
      else {
         if (StringUtilities::startsWith(line, "#N_Nodes")) {
            std::vector<QString> tokens;
            StringUtilities::token(line, " \t", tokens);
            if (tokens.size() == 3) {
               const int num = StringUtilities::toInt(tokens[2]);
               if (numberOfNodes <= 0) {
                  setNumberOfNodesAndColumns(num, 1);
                  columnNumber = 0;
               }
               else {
                  if (num != numberOfNodes) {
                     file.close();
                     throw FileException(filename, "File has wrong number of nodes.");
                  }
                  else {
                     addColumns(1);
                     columnNumber = numberOfColumns - 1;
                  }
               }
            }
         }
      }
      
      readLine(stream, line);
   }
   
   file.close();
   
   if (readData == false) {
      throw FileException(filename, "Never found RGB data.");
   }
}

/** 
 * Import colors from a vtk surface file
 */
void
RgbPaintFile::importFromVtkFile(vtkPolyData* polyData)
{
   const int numPoints = polyData->GetNumberOfPoints();
   if (numPoints <= 0) {
      return;
   }
   
   //
   // Determine column to use in RGB Paint file.
   //
   int column = 0;
   if (getNumberOfNodes() == 0) {
      clear();
      setNumberOfNodesAndColumns(numPoints, 1);
   }
   else if (numPoints != getNumberOfNodes()) {
      std::cout << "Number of nodes in VTK file does not match number in RGB Paint file." << std::endl;
   }
   else {
      addColumns(1);
      column = getNumberOfColumns() - 1;
   }
   
   vtkPointData* pointData = polyData->GetPointData();
   vtkDataArray* scalars = pointData->GetScalars();
   if (scalars != NULL) {       
      if ( (scalars->GetDataType() == VTK_UNSIGNED_CHAR) &&
            (scalars->GetNumberOfComponents() == 3) ) {
         vtkUnsignedCharArray* colors = (vtkUnsignedCharArray*)scalars;
         for (int i = 0; i < numPoints; i++) {
#ifdef HAVE_VTK5
            double rgb[3];
#else // HAVE_VTK5
            float rgb[3];
#endif // HAVE_VTK5
            colors->GetTuple(i, rgb);
            setRgb(i, column, rgb[0], rgb[1], rgb[2]);
         }
      }
      else if ( (scalars->GetDataType() == VTK_FLOAT) &&
                  (scalars->GetNumberOfComponents() == 1) ) {
         float maxValue = -100000.0;
         for (int j = 0; j < numPoints; j++) {
            if (scalars->GetComponent(j, 0) > maxValue) {
               maxValue = scalars->GetComponent(j, 0);
            }
         }

         vtkLookupTable* lookupTable = scalars->GetLookupTable();
         if (maxValue <= 1.0) {
            setScaleRed(column, 0.0, 1.0);
            setScaleGreen(column, 0.0, 1.0);
            setScaleBlue(column, 0.0, 1.0);
         }
         for (int i = 0; i < numPoints; i++) {
            const float value = scalars->GetComponent(i, 0);
            if (lookupTable == NULL) {
               setRgb(i, column, value, value, value);
            }
            else {
#ifdef HAVE_VTK5
               double rgb[3] = { 0.0, 0.0, 0.0 };
               lookupTable->GetColor((double)value, rgb);
#else // HAVE_VTK5
               float rgb[3] = { 0.0, 0.0, 0.0 };
               lookupTable->GetColor((float)value, rgb);
#endif // HAVE_VTK5
               setRgb(i, column, rgb[0], rgb[1], rgb[2]);
            }
         }
      }
   }
}

/**
 *  Read the RGB Paint file's data.
 */
void
RgbPaintFile::readFileData(QFile& file, QTextStream& stream, 
                           QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // save file position since version 0 has no tags
   //
   qint64 oldFilePosition = stream.pos(); //file.pos();
   
   QString line;
   QString versionStr, versionNumberStr;
   readTagLine(stream, versionStr, versionNumberStr);
      
   int fileVersion = 0;

   if (versionStr == tagFileVersion) {
      fileVersion = versionNumberStr.toInt();
   }
   
   switch(fileVersion) {
      case 0:
         file.seek(oldFilePosition);
         stream.seek(oldFilePosition);
         readFileDataVersion0(file, stream);
         break;
      case 1:
         readFileDataVersion1(stream);
         break;
      case 2:
         readFileDataVersion2(stream, binStream);
         break;
      default:
         throw FileException(filename, "Invalid RGB paint file version");
         break;
   }   
}

/**
 * Read file version 0
 */
void 
RgbPaintFile::readFileDataVersion0(QFile& file, QTextStream& stream) throw (FileException)
{
   long oldFilePosition = stream.pos(); //file.pos();
   
   //
   // version 0 RGB paint file does not store the number of nodes in it
   // so figure out how many.
   //
   int numNodes = 0;
   QString line;
   readLine(stream, line);
   while (stream.atEnd() == false) {
      numNodes++;
      readLine(stream, line);
   }
   setNumberOfNodesAndColumns(numNodes, 1);
   if (numberOfNodes <= 0) {
      throw FileException(filename, "RGB Paint file has no data");
   }
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   file.seek(oldFilePosition);
   stream.seek(oldFilePosition);
   
   std::vector<QString> tokens;
   QString line1;
   for (int i = 0; i < numberOfNodes; i++) {
      readLineIntoTokens(stream, line1, tokens);
      if (tokens.size() == 3) {
         redComponents[i]   = tokens[0].toInt();
         greenComponents[i] = tokens[1].toInt();
         blueComponents[i]  = tokens[2].toInt();
      }
      else {
         throw FileException(filename, "Reading RGB Paint file data line ");
      }
   }
}

/**
 * Read version 1 of the RGB paint file.
 */
void 
RgbPaintFile::readFileDataVersion1(QTextStream& stream) throw (FileException)
{
   int numNodes = -1;

   QString fileTitleTemp;
   QString commentRed;
   QString commentGreen;
   QString commentBlue;
   QString titleRed;
   QString titleGreen;
   QString titleBlue;
   QString scaleRed;
   QString scaleGreen;
   QString scaleBlue;
   float redMin   = 0.0, redMax = 0.0;
   float greenMin = 0.0, greenMax = 0.0;
   float blueMin  = 0.0, blueMax = 0.0;
   
   bool readingTags = true;
   while(readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      
      if (tag == tagBeginData) {
         readingTags = false;
      }
      else if (tag == tagNumberOfNodes) {
         numNodes = tagValue.toInt();
      }
      else if (tag == tagFileTitle) {
         fileTitleTemp = tagValue;
      }
      else if (tag == tagCommentRed) {
         commentRed = tagValue;
      }
      else if (tag == tagCommentGreen) {
         commentGreen = tagValue;
      }
      else if (tag == tagCommentBlue) {
         commentBlue = tagValue;
      }
      else if (tag == tagTitleRed) {
         titleRed = tagValue;
      }
      else if (tag == tagTitleGreen) {
         titleGreen = tagValue;
      }
      else if (tag == tagTitleBlue) {
         titleBlue = tagValue;
      }
      else if (tag == tagScaleRed) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " \t", tokens);
         if (tokens.size() == 2) {
            redMin = tokens[0].toFloat();
            redMax = tokens[1].toFloat();
         }
      }
      else if (tag == tagScaleGreen) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " \t", tokens);
         if (tokens.size() == 2) {
            greenMin = tokens[0].toFloat();
            greenMax = tokens[1].toFloat();
         }
      }
      else if (tag == tagScaleBlue) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " \t", tokens);
         if (tokens.size() == 2) {
            blueMin = tokens[0].toFloat();
            blueMax = tokens[1].toFloat();
         }
      }
      else {
         std::cerr << "WARNING: Unknown RGB Paint File Tag: " << tag.toAscii().constData() << std::endl;
      }
   }
   
   if (numNodes <= 0) {
      throw FileException(filename, "No data in RGB paint file");
   }
   setNumberOfNodesAndColumns(numNodes, 1);
   fileTitle = fileTitleTemp;
   columnNames[0] = fileTitleTemp;
   columnComments[0] = fileTitle;
   redComponentTitle[0] = titleRed;
   greenComponentTitle[0] = titleGreen;
   blueComponentTitle[0] = titleBlue;
   
   redComponentComment[0] = StringUtilities::setupCommentForDisplay(commentRed);
   
   greenComponentComment[0] = StringUtilities::setupCommentForDisplay(commentGreen);
   
   blueComponentComment[0] = StringUtilities::setupCommentForDisplay(commentBlue);
   
   redComponentScaleMin[0]  = redMin;
   redComponentScaleMax[0]  = redMax;
   greenComponentScaleMin[0] = greenMin;
   greenComponentScaleMax[0] = greenMax;
   blueComponentScaleMin[0]  = blueMin;
   blueComponentScaleMax[0]  = blueMax;
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   std::vector<QString> tokens;
   QString line;
   for (int i = 0; i < numberOfNodes; i++) {
      readLineIntoTokens(stream, line, tokens);
      if (tokens.size() == 4) {
         redComponents[i]   = tokens[1].toFloat();
         greenComponents[i] = tokens[2].toFloat();
         blueComponents[i]  = tokens[3].toFloat();
      }
      else {
         throw FileException(filename, "Reading RGB Paint file");
      }
   }
}

/**
 * Read version 2 of the RGB paint file.
 */
void 
RgbPaintFile::readFileDataVersion2(QTextStream& stream,
                                   QDataStream& binStream) throw (FileException)
{
   int numNodes = -1;
   int numCols  = -1;

   bool readingTags = true;
   while(readingTags) {
      QString tag, tagValue;
      readTagLine(stream, tag, tagValue);
      
      if (tag == tagBeginData) {
         readingTags = false;
      }
      else if (tag == tagNumberOfNodes) {
         numNodes = tagValue.toInt();
         if ((numNodes > 0) && (numCols > 0)) {
            setNumberOfNodesAndColumns(numNodes, numCols);
         }
      }
      else if (tag == tagNumberOfColumns) {
         numCols = tagValue.toInt();
         if ((numNodes > 0) && (numCols > 0)) {
            setNumberOfNodesAndColumns(numNodes, numCols);
         }
      }
      else if (tag == tagFileTitle) {
         fileTitle = tagValue;
      }
      else if (tag == tagColumnName) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         columnNames[index] = name;
      }
      else if (tag == tagColumnComment) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         columnComments[index] = StringUtilities::setupCommentForDisplay(name);
      }
      else if (tag == tagCommentRed) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         redComponentComment[index] = StringUtilities::setupCommentForDisplay(name);
      }
      else if (tag == tagCommentGreen) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         greenComponentComment[index] = StringUtilities::setupCommentForDisplay(name);
      }
      else if (tag == tagCommentBlue) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         blueComponentComment[index] = StringUtilities::setupCommentForDisplay(name);
      }
      else if (tag == tagTitleRed) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         redComponentTitle[index] = name;
      }
      else if (tag == tagTitleGreen) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         greenComponentTitle[index] = name;
      }
      else if (tag == tagTitleBlue) {
         QString name;
         const int index = splitTagIntoColumnAndValue(tagValue, name);
         blueComponentTitle[index] = name;
      }
      else if (tag == tagScaleRed) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " \t", tokens);
         if (tokens.size() == 3) {
            const int index = tokens[0].toInt();
            redComponentScaleMin[index] = tokens[1].toFloat();
            redComponentScaleMax[index] = tokens[2].toFloat();
         }
      }
      else if (tag == tagScaleGreen) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " \t", tokens);
         if (tokens.size() == 3) {
            const int index = tokens[0].toInt();
            greenComponentScaleMin[index] = tokens[1].toFloat();
            greenComponentScaleMax[index] = tokens[2].toFloat();
         }
      }
      else if (tag == tagScaleBlue) {
         std::vector<QString> tokens;
         StringUtilities::token(tagValue, " \t", tokens);
         if (tokens.size() == 3) {
            const int index = tokens[0].toInt();
            blueComponentScaleMin[index] = tokens[1].toFloat();
            blueComponentScaleMax[index] = tokens[2].toFloat();
         }
      }
   
   }
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   if (numNodes <= 0) {
      throw FileException(filename, "No data in RGB paint file");
   }
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            std::vector<QString> tokens;
            QString line;
            for (int i = 0; i < numberOfNodes; i++) {
               readLineIntoTokens(stream, line, tokens);
               if (static_cast<int>(tokens.size()) == (numberOfColumns * 3 + 1)) {
                  for (int j = 0; j < numberOfColumns; j++) {
                     const float r = tokens[j * 3 + 1].toFloat();
                     const float g = tokens[j * 3 + 2].toFloat();
                     const float b = tokens[j * 3 + 3].toFloat();
                     setRgb(i, j, r, g, b);
                  }
               }
               else {
                  QString msg("Reading RGB Paint file line: ");
                  msg.append(line);
                  throw FileException(filename, msg);
               }
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
            binStream.device()->seek(stream.pos());
            float r, g, b;      
            for (int i = 0; i < numberOfNodes; i++) {
               for (int j = 0; j < numberOfColumns; j++) {
                  binStream >> r >> g >> b;
                  setRgb(i, j, r, g, b);
               }
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing in XML format not supported.");
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
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
}

/**
 * Write the RGB paint file.
 */
void 
RgbPaintFile::writeFileData(QTextStream& stream, 
                            QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // output tags
   //
   stream << tagFileVersion << " 2\n";
   stream << tagNumberOfNodes << " " << numberOfNodes << "\n";
   stream << tagNumberOfColumns << " " << numberOfColumns << "\n";
   stream << tagFileTitle << " " << fileTitle << "\n";
   
   for (int j = 0; j < numberOfColumns; j++) {
      stream << tagColumnComment << " " << j 
             << " " << StringUtilities::setupCommentForStorage(columnComments[j]) << "\n";
      
      stream << tagColumnName << " " << j << " " << columnNames[j] << "\n";
      
      stream << tagCommentRed << " " << j 
             << " " << StringUtilities::setupCommentForStorage(redComponentComment[j]) << "\n";
      
      stream << tagCommentGreen << " " << j 
             << " " << StringUtilities::setupCommentForStorage(greenComponentComment[j]) << "\n";
      
      stream << tagCommentBlue << " " << j 
             << " " << StringUtilities::setupCommentForStorage(blueComponentComment[j]) << "\n";
      
      stream << tagTitleRed << " " <<  j << " " << redComponentTitle[j] << "\n";
      stream << tagTitleGreen << " " <<  j << " " << greenComponentTitle[j] << "\n";
      stream << tagTitleBlue << " " <<  j << " " << blueComponentTitle[j] << "\n";
      stream << tagScaleRed << " " << j << " " <<
            redComponentScaleMin[j] << " " << redComponentScaleMax[j] << "\n";
      stream << tagScaleGreen << " " <<  j << " " <<
            greenComponentScaleMin[j] << " " << greenComponentScaleMax[j] << "\n";
      stream << tagScaleBlue << " " <<  j << " " <<
            blueComponentScaleMin[j] << " " << blueComponentScaleMax[j] << "\n";
   }

   stream << tagBeginData << "\n";

   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         {
            float r, g, b;      
            for (int i = 0; i < numberOfNodes; i++) {
               stream << i;
               for (int j = 0; j < numberOfColumns; j++) {
                  getRgb(i, j, r, g, b);
                  stream << " " <<  r << " " << g << " " << b;
               }
               stream << "\n";
            }
         }
         break;
      case FILE_FORMAT_BINARY:
#ifdef QT4_FILE_POS_BUG
         setBinaryFilePosQT4Bug();
#else  // QT4_FILE_POS_BUG
         //
         // still a bug in QT 4.2.2
         //
         setBinaryFilePosQT4Bug();
#endif // QT4_FILE_POS_BUG
         {
            float r, g, b;      
            for (int i = 0; i < numberOfNodes; i++) {
               for (int j = 0; j < numberOfColumns; j++) {
                  getRgb(i, j, r, g, b);
                  binStream << r << g << b;
               }
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing in XML format not supported.");
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
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
}

