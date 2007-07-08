
#ifndef __SEGMENTATION_MASK_LIST_FILE_H__
#define __SEGMENTATION_MASK_LIST_FILE_H__

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

#include "AbstractFile.h"

/// class for reading a segmentation mask list file used in stereotaxic segmentation
class SegmentationMaskListFile : public AbstractFile {
   public:
      /// class for a segmentation mask
      class SegmentationMask {
         public:
            /// stereotaxic space name
            QString stereotaxicSpaceName;
            
            /// structure name
            QString structureName;
            
            /// mask volume file name
            QString maskVolumeFileName;
            
            /// less than operator
            bool operator<(const SegmentationMask& sm) const;
      };
      
      // constructor
      SegmentationMaskListFile();
      
      // destructor
      ~SegmentationMaskListFile();

      // clear the file
      void clear();
      
      // returns true if file contains no data
      bool empty() const;
      
      // get the number of segmentation masks
      int getNumberOfSegmentationMasks() const { return masks.size(); }
      
      // get a mask
      SegmentationMask getSegmentationMask(const int indx) const { return masks[indx]; }
      
      // get name of segmentation mask volume file
      QString getSegmentationMaskFileName(const QString& stereotaxicSpaceName,
                                          const QString& structureName) const;
                                         
      // find out if comma separated file conversion supported
      void getCommaSeparatedFileSupport(bool& readFromCSV,
                                        bool& writeToCSV) const;
      
      // read the file's data from a comma separated values file (throws exception if not supported)
      void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);

      // get the available masks
      QString getAvailableMasks(const QString& indentation) const;
      
   protected:
      // Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                        QDomElement& rootElement) throw (FileException);

      // Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);

      /// the masks
      std::vector<SegmentationMask> masks;
};

#endif // __SEGMENTATION_MASK_LIST_FILE_H__
