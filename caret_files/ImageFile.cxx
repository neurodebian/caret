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

#include <QDateTime>

#include "DebugControl.h"
#include "FileUtilities.h"
#include "ImageFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor.
 */
ImageFile::ImageFile()
   : AbstractFile("Image File",    
                  ".jpg", //SpecFile::getImageFileExtension(),
                  false, 
                  AbstractFile::FILE_FORMAT_OTHER,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_READ_AND_WRITE)  // be sure to update all constructors
{
   clear();
}

/**
 * Constructor.
 */
ImageFile::ImageFile(const QImage& img)
   : AbstractFile("Image File",    
                  ".jpg", //SpecFile::getImageFileExtension(),
                  false, 
                  AbstractFile::FILE_FORMAT_OTHER,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_READ_AND_WRITE)  // be sure to update all constructors
{
   clear();
   image = img;
}
      
/**
 * Destructor.
 */
ImageFile::~ImageFile()
{
   clear();
}

/**
 * Clears current file data in memory.
 */
void 
ImageFile::clear()
{
   image = QImage();
}

/**
 * returns true if the file is isEmpty.
 */

bool 
ImageFile::empty() const
{
   return (image.width() <= 0);
}

/**
 * read the file (overridden since file has no header).
 */
void 
ImageFile::readFile(const QString& fileNameIn) throw (FileException)
{
   clear();
   
   if (fileNameIn.isEmpty()) {
      throw FileException(fileNameIn, "Filename for reading is isEmpty");   
   }

   filename = fileNameIn;
  
   if (image.load(filename) == false) {
      throw FileException(filename, "Unable to load file.");
   }
   
   QTime timer;
   timer.start();

   if (DebugControl::getDebugOn()) {
      std::cout << "Time to read " << FileUtilities::basename(filename).toAscii().constData()
                << " was "   
                << (static_cast<float>(timer.elapsed()) / 1000.0)
                << " seconds." << std::endl;
   }
   
   clearModified();
}   

/**
 * append an image file to the bottom of this image file.
 */
void 
ImageFile::appendImageAtBottom(const ImageFile& img)
{
   //
   // Determine size of new image
   //
   const QImage* otherImage = img.getImage();
   const int newWidth = std::max(image.width(), otherImage->width());
   const int newHeight = image.height() + otherImage->height();
   const int oldHeight = image.height();

   //
   // Copy the current image
   //
   const QImage currentImage = image;
   if (DebugControl::getDebugOn()) {
      std::cout << "cw: " << currentImage.width() << std::endl;
      std::cout << "ch: " << currentImage.height() << std::endl;
   }
   
   //
   // Create the new image and make it "this" image
   //
   QImage newImage(newWidth, newHeight, QImage::Format_RGB32);
   if (DebugControl::getDebugOn()) {
      std::cout << "nw: " << newImage.width() << std::endl;
      std::cout << "nh: " << newImage.height() << std::endl;
   }
   setImage(newImage);
   if (DebugControl::getDebugOn()) {
      std::cout << "iw2: " << image.width() << std::endl;
      std::cout << "ih2: " << image.height() << std::endl;
   }
   
   //
   // Insert current image into new image
   //
   insertImage(currentImage, 0, 0);
   
   //
   // Insert other image into new image
   //
   insertImage(*otherImage, 0, oldHeight);
   
   setModified();
}      

/**
 * insert an image into this image which must be large enough for insertion of image.
 */
void 
ImageFile::insertImage(const QImage& otherImage,
                       const int x,
                       const int y) throw (FileException)
{
   if (x < 0) {
      throw FileException("X position is less than zero.");
   }
   if (y < 0) {
      throw FileException("Y position is less than zero.");
   }
   
   const int otherWidth = otherImage.width();
   const int otherHeight = otherImage.height();
   
   const int myWidth = image.width();
   const int myHeight = image.height();
   
   if ((otherWidth + x) > myWidth) {
      throw FileException("This image is not large enough to insert other image.");
   }
   if ((otherHeight + y) > myHeight) {
      throw FileException("This image is not large enough to insert other image.");
   }
   
   for (int i = 0; i < otherWidth; i++) {
      for (int j = 0; j < otherHeight; j++) {
         image.setPixel(x + i, y + j, otherImage.pixel(i, j));
      }
   }
   
   setModified();
}
                       
/**
 * compare a file for unit testing (tolerance ignored).
 */
bool 
ImageFile::compareFileForUnitTesting(const AbstractFile* af,
                                     const float /*tolerance*/,
                                     QString& messageOut) const
{
   //
   // Cast to an image file
   //
   const ImageFile* img = dynamic_cast<const ImageFile*>(af);
   if (img == NULL) {
      messageOut = ("ERROR: File for comparison ("
                    + af->getFileName()
                    + " does not appear to be an image file.");
      return false;
   }
   
   //
   // Get the image from the other file
   //
   const QImage* otherImage = img->getImage();
   
   //
   // Confirm width/height
   //
   const int width = image.width();
   const int height = image.height();
   if ((width != otherImage->width()) ||
       (height != otherImage->height())) {
      messageOut = "The images are of different height and/or width.";
      return false;
   }
   
   //
   // compare pixels
   //
   int pixelCount = 0;
   for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
         if (image.pixel(i, j) != otherImage->pixel(i, j)) {
            pixelCount++;
         }
      }
   }

   if (pixelCount > 0) {
      const float pct = static_cast<float>(pixelCount * 100.0) 
                      / static_cast<float>(width * height);
      messageOut = QString::number(pct, 'f', 2)
                   + "% pixels in the image do not match.";
      return false;
   }
   
   return true;
}
                                     
/**
 * write the file.
 */
void 
ImageFile::writeFile(const QString& fileNameIn) throw (FileException)
{
   if (fileNameIn.isEmpty()) {
      throw FileException(fileNameIn, "Filename for reading is isEmpty");   
   }
   filename = fileNameIn;
   
   QString format(StringUtilities::makeUpperCase(FileUtilities::filenameExtension(filename)));
   if (format == "JPG") {
      format = "JPEG";
   }
   image.save(filename, format.toAscii().constData());
   
   clearModified();
}

/**
 * Read the vtk model file data (should never be called).
 */
void 
ImageFile::readFileData(QFile& /*file*/, QTextStream& /*stream*/, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, "Program Error: ImageFile::readFileData should never be called.");
}

/**
 * Write the vtk model file data (should never be called).
 */
void 
ImageFile::writeFileData(QTextStream& /*stream*/, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   throw FileException(filename, "Program Error: ImageFile::writeFileData should never be called.");
}
      
