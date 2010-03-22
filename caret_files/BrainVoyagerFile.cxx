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

#include <fstream>
#include <iostream>
#include <sstream>

#include <QFile>
#include <QGlobalStatic>
#include <QTime>

#include "BrainVoyagerFile.h"
#include "ByteSwapping.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "SpecFile.h"

/**
 * Constructor.
 */
BrainVoyagerFile::BrainVoyagerFile()
   : AbstractFile("Brain Voyager File", 
                  SpecFile::getBrainVoyagerFileExtension(),
                  false, 
                  AbstractFile::FILE_FORMAT_BINARY, 
                  FILE_IO_NONE, 
                  FILE_IO_READ_ONLY, 
                  FILE_IO_NONE,
                  FILE_IO_NONE)
{ 
   verticesX = NULL;
   verticesY = NULL;
   verticesZ = NULL;
   vertexRedComponent   = NULL;
   vertexGreenComponent = NULL;
   vertexBlueComponent  = NULL;
   vertexAlphaComponent = NULL;
   vertexColorTableIndices = NULL;
   triangles = NULL;
   
   clear();
}

/**
 * Destructor.
 */
BrainVoyagerFile::~BrainVoyagerFile()
{
   clear();
}

/**
 * clear the files contents.
 */
void
BrainVoyagerFile::clear()
{
   clearAbstractFile();
   version = -1.0;
   numberOfVertices  = 0;
   numberOfTriangles = 0;
   
   if (verticesX != NULL) {
      delete[] verticesX;
      verticesX = NULL;
   }
   if (verticesY != NULL) {
      delete[] verticesY;
      verticesY = NULL;
   }
   if (verticesZ != NULL) {
      delete[] verticesZ;
      verticesZ = NULL;
   }
   
   if (vertexRedComponent != NULL) {
      delete[] vertexRedComponent;
      vertexRedComponent = NULL;
   }
   if (vertexGreenComponent != NULL) {
      delete[] vertexGreenComponent;
      vertexGreenComponent = NULL;
   }
   if (vertexBlueComponent != NULL) {
      delete[] vertexBlueComponent;
      vertexBlueComponent = NULL;
   }
   if (vertexAlphaComponent != NULL) {
      delete[] vertexAlphaComponent;
      vertexAlphaComponent = NULL;
   }
   if (vertexColorTableIndices != NULL) {
      delete[] vertexColorTableIndices;
      vertexColorTableIndices = NULL;
   }
   if (triangles != NULL) {
      delete[] triangles;
      triangles = NULL;
   }
   
   colorTable.clear();
}

/**
 * get coordinates of a vertex.
 */
void 
BrainVoyagerFile::getVertexCoordinates(const int vertexNumber, float xyz[3]) const
{
   if (vertexNumber < numberOfVertices) {
      xyz[0] = verticesX[vertexNumber];
      xyz[1] = verticesY[vertexNumber];
      xyz[2] = verticesZ[vertexNumber];
   }
}
      
/**
 * get vertices of a tile.
 */
void 
BrainVoyagerFile::getTriangle(const int triangleNumber, int verts[3]) const
{
   if (triangleNumber < numberOfTriangles) {
      verts[0] = triangles[triangleNumber * 3];
      verts[1] = triangles[triangleNumber * 3 + 1];
      verts[2] = triangles[triangleNumber * 3 + 2];
   }
}      

/**
 * get the vertex color.
 */
void 
BrainVoyagerFile::getVertexColor(const int vertexNumber, unsigned char rgb[3]) const
{
   if (vertexNumber < numberOfVertices) {
      rgb[0] = static_cast<unsigned char>(vertexRedComponent[vertexNumber]);
      rgb[1] = static_cast<unsigned char>(vertexGreenComponent[vertexNumber]);
      rgb[2] = static_cast<unsigned char>(vertexBlueComponent[vertexNumber]);
   }
}

/**
 * Write the file's data (not used).
 */
void
BrainVoyagerFile::readFileData(QFile& /*file*/, QTextStream& /*stream*/, QDataStream&,
                               QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, 
          "Program Error: BrainVoyagerFile::readFileData should never be called");
}

/**
 * Read the file's data (not used).
 */
void
BrainVoyagerFile::writeFileData(QTextStream& /*stream*/, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, 
          "Program Error: BrainVoyagerFile::writeFileData should never be called");
}

/**
 * Read the file (overrides AbstractFile method).
 */
void
BrainVoyagerFile::readFile(const QString& filenameIn)  throw (FileException)
{
   const QString filename2(filenameIn);
   clear();
   filename = filename2;

   //
   // MSB systems will need to byte swap
   //
   //int wordSize;
   //bool bigEndian;
   //qSysInfo(&wordSize, &bigEndian);
   const bool needToByteSwap = (QSysInfo::ByteOrder == QSysInfo::BigEndian);

   QTime timer;
   timer.start();
   
   std::ifstream file(filename.toAscii().constData(), std::ios::in | std::ios::binary);
   if (file.bad() || file.fail()) {
      throw FileException(filename, "Failure trying to open: ");
   }
   else {
      try {
         
         int intDummy;
         float floatDummy;
         
         file.read((char*)&version, sizeof(version));;
         file.read((char*)&intDummy, sizeof(intDummy));
         file.read((char*)&numberOfVertices, sizeof(numberOfVertices));
         file.read((char*)&numberOfTriangles, sizeof(numberOfTriangles));
         
         if (DebugControl::getDebugOn()) {
            float fv = version;
            ByteSwapping::swapBytes(&fv, 1);
            std::cout << "version: " << version 
                      << " version byte swapped: " << fv << std::endl;
         }
         
         file.read((char*)&floatDummy, sizeof(floatDummy));   // mesh center x
         file.read((char*)&floatDummy, sizeof(floatDummy));   // mesh center y
         file.read((char*)&floatDummy, sizeof(floatDummy));   // mesh center z
         
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&version, 1);
            ByteSwapping::swapBytes(&numberOfVertices, 1);
            ByteSwapping::swapBytes(&numberOfTriangles, 1);
         }
         
         if (numberOfVertices > 0) {
            //
            // Read vertices
            //
            verticesX = new float[numberOfVertices];
            verticesY = new float[numberOfVertices];
            verticesZ = new float[numberOfVertices];
            
            file.read((char*)verticesX, (numberOfVertices * 4));
            file.read((char*)verticesY, (numberOfVertices * 4));
            file.read((char*)verticesZ, (numberOfVertices * 4));
            
            if (needToByteSwap) {
               ByteSwapping::swapBytes(verticesX, numberOfVertices);
               ByteSwapping::swapBytes(verticesY, numberOfVertices);
               ByteSwapping::swapBytes(verticesZ, numberOfVertices);
            }
            
            //
            // Skip normals
            //
            file.seekg(numberOfVertices * 4 * 3, std::ios::cur);
         }
         
         float redConvexCurvature, greenConvexCurvature, blueConvexCurvature;
         file.read((char*)&redConvexCurvature, sizeof(redConvexCurvature));
         file.read((char*)&greenConvexCurvature, sizeof(greenConvexCurvature));
         file.read((char*)&blueConvexCurvature, sizeof(blueConvexCurvature));
         file.read((char*)&floatDummy, sizeof(floatDummy));  // alpha convex curvature
         
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&redConvexCurvature, 1);
            ByteSwapping::swapBytes(&greenConvexCurvature, 1);
            ByteSwapping::swapBytes(&blueConvexCurvature, 1);
         }
         
         const unsigned char redConvex = 
            static_cast<unsigned char>(redConvexCurvature * 255.0);
         const unsigned char greenConvex = 
            static_cast<unsigned char>(greenConvexCurvature * 255.0);
         const unsigned char blueConvex = 
            static_cast<unsigned char>(blueConvexCurvature * 255.0);

         float redConcaveCurvature, greenConcaveCurvature, blueConcaveCurvature;
         file.read((char*)&redConcaveCurvature, sizeof(redConcaveCurvature));
         file.read((char*)&greenConcaveCurvature, sizeof(greenConcaveCurvature));
         file.read((char*)&blueConcaveCurvature, sizeof(blueConcaveCurvature));
         file.read((char*)&floatDummy, sizeof(floatDummy));  // alpha concave curvature
         
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&redConcaveCurvature, 1);
            ByteSwapping::swapBytes(&greenConcaveCurvature, 1);
            ByteSwapping::swapBytes(&blueConcaveCurvature, 1);
         }
         
         const unsigned char redConcave = 
            static_cast<unsigned char>(redConcaveCurvature * 255.0);
         const unsigned char greenConcave = 
            static_cast<unsigned char>(greenConcaveCurvature * 255.0);
         const unsigned char blueConcave = 
            static_cast<unsigned char>(blueConcaveCurvature * 255.0);
         
         //
         // Color indicies - Create color table
         //
         if (numberOfVertices > 0) {
            vertexColorTableIndices = new int[numberOfVertices];
            
            unsigned char dummyUnsignedChar;
            for (int i = 0; i < numberOfVertices; i++) {
               unsigned char index;
               file.read((char*)&index, sizeof(index));
               file.read((char*)&dummyUnsignedChar, sizeof(dummyUnsignedChar));
               file.read((char*)&dummyUnsignedChar, sizeof(dummyUnsignedChar));
               file.read((char*)&dummyUnsignedChar, sizeof(dummyUnsignedChar));
               
               //
               // See if color index already in color table
               //
               int colorTableIndex = -1;
               for (unsigned int j = 0; j < colorTable.size(); j++) {
                  if (colorTable[j].brainVoyagerIndex == index) {
                     colorTableIndex = static_cast<int>(j);
                  }
               }
               
               //
               // Create new color table element ?
               //
               if (colorTableIndex < 0) {
                  colorTable.push_back(BrainVoyagerColorTableElement(index));
                  colorTableIndex = static_cast<int>(colorTable.size()) - 1;
               }
               
               //
               // set vertice's color table index
               //
               vertexColorTableIndices[i] = colorTableIndex;
            }
               
            //
            // Set the convex and concave colors that are colors 0 & 1
            //
            if (colorTable.size() > 1) {
               colorTable[0].name.append("_convex");
               colorTable[0].rgb[0] = redConvex;
               colorTable[0].rgb[1] = greenConvex;
               colorTable[0].rgb[2] = blueConvex;
               colorTable[1].name.append("_concave");
               colorTable[1].rgb[0] = redConcave;
               colorTable[1].rgb[1] = greenConcave;
               colorTable[1].rgb[2] = blueConcave;
            }
         }
         
         
         
/*
         //
         // Color components
         //
         if (numberOfVertices > 0) {
            vertexRedComponent   = new unsigned char[numberOfVertices];
            vertexGreenComponent = new unsigned char[numberOfVertices];
            vertexBlueComponent  = new unsigned char[numberOfVertices];
            vertexAlphaComponent = new unsigned char[numberOfVertices];
            
            for (int i = 0; i < numberOfVertices; i++) {
               stream >> vertexRedComponent[i];
               stream >> vertexGreenComponent[i];
               stream >> vertexBlueComponent[i];
               stream >> vertexAlphaComponent[i];
               
               if (vertexRedComponent[i] == 0) {
                  vertexRedComponent[i]   = redConvex;
                  vertexGreenComponent[i] = greenConvex;
                  vertexBlueComponent[i]  = blueConvex;
               }
               else if (vertexRedComponent[i] == 1) {
                  vertexRedComponent[i]   = redConcave;
                  vertexGreenComponent[i] = greenConcave;
                  vertexBlueComponent[i]  = blueConcave;
               }
               
               //if (vertexRedComponent[i] != 0) {
                  std::cout << "node " << i 
                            << " (" << static_cast<int>(vertexRedComponent[i])
                            << " ," << static_cast<int>(vertexGreenComponent[i])
                            << " ," << static_cast<int>(vertexBlueComponent[i])
                            << " ," << static_cast<int>(vertexAlphaComponent[i])
                            << ")" << std::endl;
               //}
            }
         }
*/
         
         //
         // Vertex Neighbors
         //
         for (int i = 0; i < numberOfVertices; i++) {
            int numNeighbors;
            file.read((char*)&numNeighbors, sizeof(numNeighbors));
            if (needToByteSwap) {
               ByteSwapping::swapBytes(&numNeighbors, 1);
            }
            if (i == 147644) {
               std::cout << "Node " << i << " has "
                         << numNeighbors << " neighbors." << std::endl;
            }
            for (int j = 0; j < numNeighbors; j++) {
               file.read((char*)&intDummy, sizeof(intDummy));
            }
         }
         
         //
         // Triangles
         //
         triangles = new int[numberOfTriangles * 3];
         file.read((char*)triangles, numberOfTriangles * 3 * 4);
         if (needToByteSwap) {
            ByteSwapping::swapBytes(triangles, numberOfTriangles * 3);
         }
         
         //
         // Number of string elements
         //
         int numberOfStripElements;
         file.read((char*)&numberOfStripElements, sizeof(numberOfStripElements));
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&numberOfStripElements, 1);
         }

         //if (DebugControl::getDebugOn()) {
            std::cout << "Version: " << version << std::endl;
            std::cout << "Number of vertices: " << numberOfVertices << std::endl;
            std::cout << "Number of triangles: " << numberOfTriangles << std::endl;
            std::cout << "Number of strip elements: " << numberOfStripElements
                      << std::endl;
            std::cout << "Color table size: " << colorTable.size() << std::endl;
         //}
         
         clearModified();
      }
      catch (FileException& e) {
         file.close();
         clearModified();
         throw e;
      }
   
      file.close();
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to read " << FileUtilities::basename(filename).toAscii().constData() 
                << " was "
                << (static_cast<float>(timer.elapsed()) / 1000.0)
                << " seconds." << std::endl;
   }
}

/**
 * Read the file.
 */
/*
void
BrainVoyagerFile::readFile(const QString& filenameIn)  throw (FileException)
{
   const QString filename2(filenameIn);
   clear();
   filename = filename2);

   //
   // We always need to byte swap on both MSB & LSB systems.  It is
   // something to do with the way QDataStream works.
   //
   const bool needToByteSwap = true;
   
   QTime timer;
   timer.start();
   
   QFile file(filename);
   if (file.open(IO_ReadOnly)) {

      try {
         QDataStream stream(&file);
         
         if (DebugControl::getDebugOn()) {
            switch(stream.byteOrder()) {
               case QDataStream::BigEndian:
                  std::cout << "Brain Voyager File stream is big endian" << std::endl;
                  break;
               case QDataStream::LittleEndian:
                  std::cout << "Brain Voyager File stream is little endian" << std::endl;
                  break;
            }
         }
         
         int intDummy;
         float floatDummy;
         
         stream >> version;
         stream >> intDummy;
         stream >> numberOfVertices;
         stream >> numberOfTriangles;
         
         if (DebugControl::getDebugOn()) {
            float fv = version;
            ByteSwapping::swapBytes(&fv, 1);
            std::cout << "version: " << version 
                      << " version byte swapped: " << fv << std::endl;
         }
         
         stream >> floatDummy;   // mesh center x
         stream >> floatDummy;   // mesh center y
         stream >> floatDummy;   // mesh center z
         
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&version, 1);
            ByteSwapping::swapBytes(&numberOfVertices, 1);
            ByteSwapping::swapBytes(&numberOfTriangles, 1);
         }
         
         if (numberOfVertices > 0) {
            //
            // Read vertices
            //
            verticesX = new float[numberOfVertices];
            verticesY = new float[numberOfVertices];
            verticesZ = new float[numberOfVertices];
            
            stream.readRawBytes((char*)verticesX, (numberOfVertices * 4));
            stream.readRawBytes((char*)verticesY, (numberOfVertices * 4));
            stream.readRawBytes((char*)verticesZ, (numberOfVertices * 4));
            
            if (needToByteSwap) {
               ByteSwapping::swapBytes(verticesX, numberOfVertices);
               ByteSwapping::swapBytes(verticesY, numberOfVertices);
               ByteSwapping::swapBytes(verticesZ, numberOfVertices);
            }
            
            //
            // Skip normals
            //
            QIODevice::Offset pos = file.at();
            pos += numberOfVertices * 4 * 3;
            file.at(pos);
         }
         
         float redConvexCurvature, greenConvexCurvature, blueConvexCurvature;
         stream >> redConvexCurvature;
         stream >> greenConvexCurvature;
         stream >> blueConvexCurvature;
         stream >> floatDummy;  // alpha convex curvature
         
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&redConvexCurvature, 1);
            ByteSwapping::swapBytes(&greenConvexCurvature, 1);
            ByteSwapping::swapBytes(&blueConvexCurvature, 1);
         }
         
         const unsigned char redConvex = 
            static_cast<unsigned char>(redConvexCurvature * 255.0);
         const unsigned char greenConvex = 
            static_cast<unsigned char>(greenConvexCurvature * 255.0);
         const unsigned char blueConvex = 
            static_cast<unsigned char>(blueConvexCurvature * 255.0);

         float redConcaveCurvature, greenConcaveCurvature, blueConcaveCurvature;
         stream >> redConcaveCurvature;
         stream >> greenConcaveCurvature;
         stream >> blueConcaveCurvature;
         stream >> floatDummy;  // alpha concave curvature
         
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&redConcaveCurvature, 1);
            ByteSwapping::swapBytes(&greenConcaveCurvature, 1);
            ByteSwapping::swapBytes(&blueConcaveCurvature, 1);
         }
         
         const unsigned char redConcave = 
            static_cast<unsigned char>(redConcaveCurvature * 255.0);
         const unsigned char greenConcave = 
            static_cast<unsigned char>(greenConcaveCurvature * 255.0);
         const unsigned char blueConcave = 
            static_cast<unsigned char>(blueConcaveCurvature * 255.0);
         
         //
         // Color indicies - Create color table
         //
         if (numberOfVertices > 0) {
            vertexColorTableIndices = new int[numberOfVertices];
            
            unsigned char dummyUnsignedChar;
            for (int i = 0; i < numberOfVertices; i++) {
               unsigned char index;
               stream >> index;
               stream >> dummyUnsignedChar;
               stream >> dummyUnsignedChar;
               stream >> dummyUnsignedChar;
               
               //
               // See if color index already in color table
               //
               int colorTableIndex = -1;
               for (unsigned int j = 0; j < colorTable.size(); j++) {
                  if (colorTable[j].brainVoyagerIndex == index) {
                     colorTableIndex = static_cast<int>(j);
                  }
               }
               
               //
               // Create new color table element ?
               //
               if (colorTableIndex < 0) {
                  colorTable.push_back(BrainVoyagerColorTableElement(index));
                  colorTableIndex = static_cast<int>(colorTable.size());
               }
               
               //
               // set vertice's color table index
               //
               vertexColorTableIndices[i] = colorTableIndex;
            }
               
            //
            // Set the convex and concave colors that are colors 0 & 1
            //
            if (colorTable.size() > 1) {
               colorTable[0].name.append("_convex");
               colorTable[0].rgb[0] = redConvex;
               colorTable[0].rgb[1] = greenConvex;
               colorTable[0].rgb[2] = blueConvex;
               colorTable[1].name.append("_concave");
               colorTable[1].rgb[0] = redConcave;
               colorTable[1].rgb[1] = greenConcave;
               colorTable[1].rgb[2] = blueConcave;
            }
         }
         
         //
         // Vertex Neighbors
         //
         for (int i = 0; i < numberOfVertices; i++) {
            int numNeighbors;
            stream >> numNeighbors;
            if (needToByteSwap) {
               ByteSwapping::swapBytes(&numNeighbors, 1);
            }
            if (i == 147644) {
               std::cout << "Node " << i << " has "
                         << numNeighbors << " neighbors." << std::endl;
            }
            for (int j = 0; j < numNeighbors; j++) {
               stream >> intDummy;
            }
         }
         
         //
         // Triangles
         //
         triangles = new int[numberOfTriangles * 3];
         stream.readRawBytes((char*)triangles, numberOfTriangles * 3 * 4);
         if (needToByteSwap) {
            ByteSwapping::swapBytes(triangles, numberOfTriangles * 3);
         }
         
         //
         // Number of string elements
         //
         int numberOfStripElements;
         stream >> numberOfStripElements;
         if (needToByteSwap) {
            ByteSwapping::swapBytes(&numberOfStripElements, 1);
         }

         //if (DebugControl::getDebugOn()) {
            std::cout << "Version: " << version << std::endl;
            std::cout << "Number of vertices: " << numberOfVertices << std::endl;
            std::cout << "Number of triangles: " << numberOfTriangles << std::endl;
            std::cout << "Number of strip elements: " << numberOfStripElements
                      << std::endl;
         //}
         
         clearModified();
      }
      catch (FileException& e) {
         file.close();
         clearModified();
         throw e;
      }
   
      file.close();
   }
   else {
      throw FileException(filename, "Failure trying to open: ");
   }
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Time to read " << FileUtilities::basename(filename) 
                << " was "
                << (static_cast<float>(timer.elapsed()) / 1000.0)
                << " seconds." << std::endl;
   }
}
*/

/**
 * Write the file.
 */
void
BrainVoyagerFile::writeFile(const QString& filenameIn)  throw (FileException)
{
   throw FileException(filenameIn, "Writing brain voyager file not supported.");
}

//---------------------------------------------------------------------------------

/**
 * Constructor.
 */
BrainVoyagerColorTableElement::BrainVoyagerColorTableElement(const int brainVoyagerIndexIn)
{
   initialize();
   brainVoyagerIndex = brainVoyagerIndexIn;
   
   std::ostringstream str;
   str << "Color_"
       << brainVoyagerIndex;
   name = str.str().c_str();
   
   switch(brainVoyagerIndex) {
      case 24:
         name.append("_white_med_wall_and_cuts");
         rgb[0] = 255;
         rgb[1] = 255;
         rgb[2] = 255;
         break;
      case 32:
         name.append("_bright_blue");
         rgb[0] = 24;
         rgb[1] = 93;
         rgb[2] = 255;
         break;
      case 33:
         name.append("_dark_blue");
         rgb[0] = 16;
         rgb[1] = 80;
         rgb[2] = 222;
         break;
      case 34:
         name.append("_bright_green");
         rgb[0] = 16;
         rgb[1] = 255;
         rgb[2] = 98;
         break;
      case 35:
         name.append("_dark_green");
         rgb[0] = 24;
         rgb[1] = 218;
         rgb[2] = 82;
         break;
      case 240:
         name.append("_light_gray_cuts");
         rgb[0] = 200;
         rgb[1] = 200;
         rgb[2] = 200;
         break;
   }
   
}

/**
 * Constructor.
 */
BrainVoyagerColorTableElement::BrainVoyagerColorTableElement()
{
   initialize();
}

/**
 * Destructor.
 */
BrainVoyagerColorTableElement::~BrainVoyagerColorTableElement()
{
}

/**
 *
 */
void
BrainVoyagerColorTableElement::getRgb(unsigned char rgbOut[3]) const
{
   rgbOut[0] = rgb[0];
   rgbOut[1] = rgb[1];
   rgbOut[2] = rgb[2];
}

/**
 * Initialize the elements.
 */ 
void
BrainVoyagerColorTableElement::initialize()
{
   brainVoyagerIndex = -1;
   rgb[0] = 255;
   rgb[1] = 0;
   rgb[2] = 0;
}
