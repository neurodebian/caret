
#ifndef __GIFTI_DATA_ARRAY_FILE_STREAM_READER_H__
#define __GIFTI_DATA_ARRAY_FILE_STREAM_READER_H__

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

#include <QXmlStreamReader>

#include "FileException.h"

class GiftiDataArrayFile;
class GiftiLabelTable;
class GiftiMetaData;
class QFile;

/// class for reading a GIFTI Data Array file
class GiftiDataArrayFileStreamReader : QXmlStreamReader {
   public:
      /// constructor
      GiftiDataArrayFileStreamReader(QFile* file,
                                     GiftiDataArrayFile* giftiFileIn);
      
      /// destructor
      ~GiftiDataArrayFileStreamReader();
      
      // read the data
      void readData() throw (FileException);
      
   protected:
      // read GIFTI element
      void readGiftiVersion1();
      
      // read MetaData element
      void readMetaData(GiftiMetaData* metaData);
      
      // read LabelTable element
      void readLabelTable(GiftiLabelTable* labelTable);
      
      // read DataArray element
      void readDataArray();
      
      // read the coordinate transform matrix
      void readCoordinateTransformMatrix(GiftiMatrix* matrix);
      
      /// GIFTI Data Array File being read
      GiftiDataArrayFile* giftiFile;
};

#endif // __GIFTI_DATA_ARRAY_FILE_STREAM_READER_H__
