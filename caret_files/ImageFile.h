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

#ifndef __IMAGE_FILE_H__
#define __IMAGE_FILE_H__

#include <QImage>

#include "AbstractFile.h"

/// class for reading/writing accessing images
class ImageFile : public AbstractFile {
   public:
      /// Constructor
      ImageFile();
      
      /// Constructor
      ImageFile(const QImage& img);
      
      /// Destructor
      ~ImageFile();
      
      /// append an image file to the bottom of this image file
      void appendImageAtBottom(const ImageFile& img);
      
      /// Clears current file data in memory.
      void clear();
      
      /// compare a file for unit testing (tolerance ignored)
      virtual bool compareFileForUnitTesting(const AbstractFile* af,
                                             const float tolerance,
                                             QString& messageOut) const;
                                     
      /// returns true if the file is isEmpty
      bool empty() const;
      
      /// get the image
      QImage* getImage() { return &image; }
      
      /// get the pixmap (const method)
      const QImage* getImage() const { return &image; }
      
      /// insert an image into this image which must be large enought for insertion of image
      void insertImage(const QImage& otherImage,
                       const int x,
                       const int y) throw (FileException);
                       
      /// set the image
      void setImage(const QImage img) { image = img; }
      
      /// read the file header and its volume data
      void readFile(const QString& fileNameIn) throw (FileException);
      
      /// write the volume file
      void writeFile(const QString& filenameIn) throw (FileException);
      
   protected:
      /// Read the spec file data (should never be called)
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the spec file data (should never be called)
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// the image data
      QImage image;
};

#endif // __VTK_MODEL_FILE_H__
