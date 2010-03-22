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


#ifndef __PAINT_FILE_H__
#define __PAINT_FILE_H__

#include "FileException.h"
#include "GiftiNodeDataFile.h"
#include "SpecFile.h"

class AreaColorFile;
class ColorFile;
class CoordinateFile;
class DeformationMapFile;
class NodeRegionOfInterestFile;
class TopologyFile;
class VolumeFile;

/// This file contains node paint information.
class PaintFile : public GiftiNodeDataFile {
   public:
      /// name for "Lobes" column
      static const QString columnNameLobes;
      /// name for "Geography" column
      static const QString columnNameGeography;
      /// name for "Functional" column
      static const QString columnNameFunctional;
      /// name for "Brodmann" column
      static const QString columnNameBrodmann;
      /// name for "Modality" column
      static const QString columnNameModality;
      
      static const QString tagNumberOfPaintNames;

      /// number of original paint file columns
      enum {
         COLUMN_LOBE = 0,
         COLUMN_GEO  = 1,
         COLUMN_FCN  = 2,
         COLUMN_BROD = 3,
         COLUMN_MODALITY = 4
       //  NUMBER_OF_PAINT_COLUMNS = 5
      } COLUMN_INDICES;
      
      // constructors
      PaintFile(const QString& descriptiveName = "Paint File",
                const QString& defaultExtensionIn = SpecFile::getPaintFileExtension());
      
      // copy constructor
      PaintFile(const PaintFile& nndf);
      
      /// destructor
      virtual ~PaintFile();
      
      // assignment operator
      PaintFile& operator=(const PaintFile& nndf);
      
      // add a paint name - returns its index
      int addPaintName(const QString& nameIn);
            
      // assign colors to the labels
      void assignColors(const ColorFile& colorFile);
      
      // assign paint from an ROI file
      void assignNodesFromROIFile(const int columnNumber,
                                  const NodeRegionOfInterestFile& roiFile,
                                  const QString& paintName,
                                  const bool assignNodesInRoiOnlyFlag) throw (FileException);
                                  
      // Clean up paint names (eliminate unused ones)
      void cleanUpPaintNames();
      
      // clear contents of the current paint file
      void clear();
      
      // deform "this" node attribute file placing the output in "deformedFile".
      void deformFile(const DeformationMapFile& dmf, 
                      GiftiNodeDataFile& deformedFile,
                      const DEFORM_TYPE dt) const throw (FileException);
      
      // get the index for a paint name
      int getPaintIndexFromName(const QString& nameIn) const;
      
      // assign paints by intersecting with a volume file
      void assignPaintColumnWithVolumeFile(const VolumeFile* vf,
                                           const CoordinateFile* cf,
                                           const int columnToAssign,
                                           const QString& paintName) throw (FileException);
                                     
      // get the name for a paint index
      QString getPaintNameFromIndex(const int indexIn) const;
      
      // get indices to all paint names used by a column
      void getPaintNamesForColumn(const int column, std::vector<int>& indices) const;
      
      // get all the paints for a node
      void getPaints(const int nodeNumber, int* paints) const;
         
      // get paint for a node and column
      int getPaint(const int nodeNumber, const int columnNumber) const;
      
      // find the column of the paint file containing geography
      int getGeographyColumnNumber() const;
                    
      // get the number of paint names
      int getNumberOfPaintNames() const;

      /// get all paint names and indices sorted by name (may contain duplicate names)
      void getAllPaintNamesAndIndices(std::vector<QString>& namesOut,
                                      std::vector<int>& indicesOut) const;
                                      
      // get all paint names (could be duplicates)
      void getAllPaintNames(std::vector<QString>& namesOut) const;
      
      // get all paint count (# nodes using each paint)
      void getAllPaintCounts(std::vector<int>& countsOut) const;
                
      // copy one paint column to another in this paint file
      void copyColumns(const PaintFile* fromPaintFile,
                       const int fromColumnNumber,
                       const int newColumnNumber,
                       const QString& newColumnName = "") throw (FileException);

      // dilate a paint column
      void dilateColumn(const TopologyFile* tf,
                        const int columnNumber,
                        const int iterations) throw (FileException);

      // dilate paint ID "paintIndex" if neighbors paint index >= 0 do only those
      int dilatePaintID(const TopologyFile* tf,
                        const CoordinateFile* cf,
                        const int columnNumber,
                        const int iterations,
                        const int paintIndex,
                        const int neighborOnlyWithPaintIndex,
                        const float maximumExtent[6]) throw (FileException);
                         
      // set all the paints for a node
      void setPaints(const int nodeNumber, const int* paints);
      
      // set paint for a node and column
      void setPaint(const int nodeNumber, const int columnNumber,
                    const int value);
                    
      // set the name of a paint at an index.
      void setPaintName(const int indexIn, const QString& name);
      
      // get paint enabled by index
      bool getPaintNameEnabled(const int paintIndex) const;
      
      // set paint enable by index
      void setPaintNameEnabled(const int paintIndex,
                               const bool b);
           
      // set all paint names enabled
      void setAllPaintNamesEnabled(const bool b);
      
      // append most common column (if name empty, column is not created)
      void appendMostCommon(const QString& mostCommonColumnName,
                            const QString& mostCommonExcludeQuestionColumnName) throw (FileException);
                            
      // delete a paint any nodes using paint become ???
      void deletePaintName(const int paintIndex);
      
      // deassign a paint any nodes in column using paint become ???
      // if columnNumber is negative, operation is applied to all columns
      void deassignPaintName(const int columnNumber,
                             const int paintIndex);
                         
      // reassign a paint 
      // if columnNumber is negative, operation is applied to all columns
      void reassignPaintName(const int columnNumber,
                             const int oldPaintIndex,
                             const int newPaintIndex);
                             
      // Export to a free surfer label file.
      void exportFreeSurferAsciiLabelFile(const int columnNumber,
                                          const QString& filenamePrefix,
                                          const CoordinateFile* cf)
                                                    throw (FileException);
                                                    
      // import free surfer label file(s) 
      void importFreeSurferAsciiLabelFile(const int numNodes,
                                          const QString& filename,
                                          AreaColorFile* cf,
                                          const bool importAllInDirectory) throw (FileException);

      /// write the file's memory in caret6 format to the specified name
      virtual QString writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException);

   protected:
      // import a single free surfer label file
      void importSingleFreeSurferLabelFile(const int column,
                                 const int numNodes,
                                 AreaColorFile* cf,
                                 const QString& filename) throw (FileException);
                                 
      // read the paint node data
      void readPaintDataForNodes(const std::vector<int>& paintToPaintNameIndex,
                                 QFile& file,
                                 QTextStream& stream,
                                 QDataStream& binStream) throw (FileException);
      
      // read paint file data
      void readLegacyNodeFileData(QFile& file, QTextStream& stream, 
                              QDataStream& binStream) throw (FileException);
      
      // read paint file data
      void readFileDataVersion0(QFile& file,
                                QTextStream& stream, 
                                QDataStream& binStream) throw (FileException);
      
      // read paint file data
      void readFileDataVersion1(QFile& file,
                                QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      // write paint file data
      void writeLegacyNodeFileData(QTextStream& stream, QDataStream& binStream) throw (FileException);

   private:
      // copy helper used by assignment operator and copy constructor
      void copyHelperPaint(const PaintFile& pf);
};

#endif // __PAINT_FILE_H__

#ifdef _PAINT_FILE_MAIN_
    const QString PaintFile::columnNameLobes = "Lobes";
    const QString PaintFile::columnNameGeography = "Geography";
    const QString PaintFile::columnNameFunctional = "Functional";
    const QString PaintFile::columnNameBrodmann = "Brodmann";
    const QString PaintFile::columnNameModality = "Modality";
    const QString PaintFile::tagNumberOfPaintNames = "tag-number-of-paint-names";
#endif // _PAINT_FILE_MAIN_
