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

#include <cmath>
#include <iostream>

#include <QColor>
#include <QImageWriter>
#include <QTime>

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
 * find inclusive bounds (Left, top, right, bottom) of image object (pixel not background color).
 */
void 
ImageFile::findImageObject(const QImage& image,
                           const int backgroundColor[3],
                           int objectBoundsOut[4]) 
{
   //
   // Dimensions of image
   //
   const int numX = image.width();
   const int numY = image.height();
   
   //
   // Initialize output
   //
   objectBoundsOut[0] = 0;
   objectBoundsOut[1] = 0;
   objectBoundsOut[2] = numX - 1;
   objectBoundsOut[3] = numY - 1;

   //
   // Find left
   //
   bool gotPixelFlag = false;
   for (int i = 0; i < numX; i++) {
      for (int j = 0; j < numY; j++) {
         const QRgb pixel = image.pixel(i, j);
         if ((qRed(pixel) != backgroundColor[0]) ||
             (qGreen(pixel) != backgroundColor[1]) ||
             (qBlue(pixel)  != backgroundColor[2])) {
            objectBoundsOut[0] = i;
            gotPixelFlag = true;
            break;
         }  
      }
      if (gotPixelFlag) {
         break;
      }
   }
   
   //
   // Find right
   //
   gotPixelFlag = false;
   for (int i = (numX - 1); i >= 0; i--) {
      for (int j = 0; j < numY; j++) {
         const QRgb pixel = image.pixel(i, j);
         if ((qRed(pixel) != backgroundColor[0]) ||
             (qGreen(pixel) != backgroundColor[1]) ||
             (qBlue(pixel)  != backgroundColor[2])) {
            objectBoundsOut[2] = i;
            gotPixelFlag = true;
            break;
         }  
      }
      if (gotPixelFlag) {
         break;
      }
   }
   
   //
   // Find top
   //
   gotPixelFlag = false;
   for (int j = 0; j < numY; j++) {
      for (int i = 0; i < numX; i++) {
         const QRgb pixel = image.pixel(i, j);
         if ((qRed(pixel) != backgroundColor[0]) ||
             (qGreen(pixel) != backgroundColor[1]) ||
             (qBlue(pixel)  != backgroundColor[2])) {
            objectBoundsOut[1] = j;
            gotPixelFlag = true;
            break;
         }  
      }
      if (gotPixelFlag) {
         break;
      }
   }
   
   //
   // Find bottom
   //
   gotPixelFlag = false;
   for (int j = (numY - 1); j >= 0; j--) {
      for (int i = 0; i < numX; i++) {
         const QRgb pixel = image.pixel(i, j);
         if ((qRed(pixel) != backgroundColor[0]) ||
             (qGreen(pixel) != backgroundColor[1]) ||
             (qBlue(pixel)  != backgroundColor[2])) {
            objectBoundsOut[3] = j;
            gotPixelFlag = true;
            break;
         }  
      }
      if (gotPixelFlag) {
         break;
      }
   }
}
      
/**
 * add a margin to an image.
 */
void 
ImageFile::addMargin(QImage& image,
                     const int marginSize,
                     const int backgroundColor[3])
{
   if (marginSize <= 0) {
      return;
   }
   
   //
   // Add margin
   //
   const int width = image.width();
   const int height = image.height();
   const int newWidth = width + marginSize * 2;
   const int newHeight = height + marginSize * 2;
   QRgb backgroundColorRGB = qRgba(backgroundColor[0], 
                                   backgroundColor[1],
                                   backgroundColor[2],
                                   0);
   
   //
   // Insert image
   //
   ImageFile imageFile;
   imageFile.setImage(QImage(newWidth, newHeight, image.format()));
   imageFile.getImage()->fill(backgroundColorRGB);
   try {
      imageFile.insertImage(image, marginSize, marginSize);
      image = (*imageFile.getImage());
   }
   catch (FileException&) {
   }
}

/**
 * add a margin to an image.
 */
void 
ImageFile::addMargin(QImage& image,
                     const int marginSizeX,
                     const int marginSizeY,
                     const int backgroundColor[3])
{
   if ((marginSizeX <= 0) && (marginSizeY <= 0)) {
      return;
   }
   
   //
   // Add margin
   //
   const int width = image.width();
   const int height = image.height();
   const int newWidth = width + marginSizeX * 2;
   const int newHeight = height + marginSizeY * 2;
   QRgb backgroundColorRGB = qRgba(backgroundColor[0], 
                                   backgroundColor[1],
                                   backgroundColor[2],
                                   0);
   
   //
   // Insert image
   //
   ImageFile imageFile;
   imageFile.setImage(QImage(newWidth, newHeight, image.format()));
   imageFile.getImage()->fill(backgroundColorRGB);
   try {
      imageFile.insertImage(image, marginSizeX, marginSizeY);
      image = (*imageFile.getImage());
   }
   catch (FileException&) {
   }
}

/**
 * crop an image by removing the background from the image.
 */
void 
ImageFile::cropImageRemoveBackground(QImage& image,
                                     const int marginSize,
                                     const int backgroundColor[3])
{
   //
   // Get cropping bounds
   //
   int leftTopRightBottom[4];
   ImageFile::findImageObject(image,
                              backgroundColor,
                              leftTopRightBottom);
   if (DebugControl::getDebugOn()) {
      std::cout 
         << "cropping: "
         << leftTopRightBottom[0]
         << " "        
         << leftTopRightBottom[1]
         << " "        
         << leftTopRightBottom[2]
         << " "        
         << leftTopRightBottom[3]
         << std::endl;
   }
    
   //
   // If cropping is valid
   //
   const int width = leftTopRightBottom[2] - leftTopRightBottom[0] + 1;
   const int height = leftTopRightBottom[3] - leftTopRightBottom[1] + 1;
   if ((width > 0) &&
       (height > 0)) {
      image = image.copy(leftTopRightBottom[0],
                         leftTopRightBottom[1],
                         width,
                         height);
   
      //
      // Process margin
      //
      if (marginSize > 0) {
         ImageFile::addMargin(image,
                              marginSize,
                              backgroundColor);
      }
   }
}

/**
 * combine images retaining aspect and stretching and filling if needed.
 */
void 
ImageFile::combinePreservingAspectAndFillIfNeeded(const std::vector<QImage>& images,
                                            const int numImagesPerRow,
                                            const int backgroundColor[3],
                                            QImage& imageOut)
{
   const int numImages = static_cast<int>(images.size());
   if (numImages <= 0) {
      return;
   }
   if (numImages == 1) {
      imageOut = images[0];
      return;
   }
   
   QRgb backgroundColorRGB = qRgba(backgroundColor[0], 
                                   backgroundColor[1],
                                   backgroundColor[2],
                                   0);
   //
   // Resize all images but do not stretch
   // need to retain aspect ratio but all must 
   // be the same size in X & Y
   //
   
   //
   // Find max width and height of input images
   //
   int maxImageWidth = 0;
   int maxImageHeight = 0;
   for (int i = 0; i < numImages; i++) {
      //
      // Track max width/height
      //
      maxImageWidth = std::max(maxImageWidth, images[i].width());
      maxImageHeight = std::max(maxImageHeight, images[i].height());
   }

   //
   // Compute size of output image and create it
   //
   const int outputImageSizeX = maxImageWidth * numImagesPerRow;
   const int numberOfRows = (numImages / numImagesPerRow)
                          + (((numImages % numImagesPerRow) != 0) ? 1 : 0);
   const int outputImageSizeY = maxImageHeight * numberOfRows;
   imageOut = QImage(outputImageSizeX,
                     outputImageSizeY,
                     images[0].format());
   imageOut.fill(backgroundColorRGB);
   

   //
   // Loop through the images
   //   
   int rowCounter = 0;
   int columnCounter = 0;
   for (int i = 0; i < numImages; i++) {
      //
      // Scale image
      //
      const QImage imageScaled = images[i].scaled(maxImageWidth,
                                                  maxImageHeight,
                                                  Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation);
                                    
      //
      // Compute position of where image should be inserted
      //
      const int marginX = (maxImageWidth - imageScaled.width()) / 2;
      const int marginY = (maxImageHeight - imageScaled.height()) / 2;
      const int positionX = columnCounter * maxImageWidth + marginX;
      const int positionY = rowCounter * maxImageHeight + marginY;
      
      //
      // Insert into output image
      //
      try {
         ImageFile::insertImage(imageScaled,
                                imageOut,
                                positionX,
                                positionY);
      }
      catch (FileException& e) {
         std::cout << "QImageFile::insertImage() error: "
                   << e.whatQString().toAscii().constData()
                   << std::endl;
      }
      
      //
      // Update row and column counters
      //
      columnCounter++;
      if (columnCounter >= numImagesPerRow) {
         columnCounter = 0;
         rowCounter++;
      }
   }
   
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
 * insert an image into another image.
 */
void 
ImageFile::insertImage(const QImage& insertThisImage,
                       QImage& intoThisImage,
                       const int positionX,
                       const int positionY) throw (FileException)
{
   if (positionX < 0) {
      throw FileException("X position is less than zero.");
   }
   if (positionY < 0) {
      throw FileException("Y position is less than zero.");
   }
   
   const int otherWidth = insertThisImage.width();
   const int otherHeight = insertThisImage.height();
   
   const int myWidth = intoThisImage.width();
   const int myHeight = intoThisImage.height();
   
   if ((otherWidth + positionX) > myWidth) {
      throw FileException("This image is not large enough to insert other image.");
   }
   if ((otherHeight + positionY) > myHeight) {
      throw FileException("This image is not large enough to insert other image.");
   }
   
   for (int i = 0; i < otherWidth; i++) {
      for (int j = 0; j < otherHeight; j++) {
         intoThisImage.setPixel(positionX + i,
                                positionY + j, 
                                insertThisImage.pixel(i, j));
      }
   }
}
                       
/**
 * compare a file for unit testing (tolerance ignored).
 */
bool 
ImageFile::compareFileForUnitTesting(const AbstractFile* af,
                                     const float tolerance,
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
         QColor im1 = image.pixel(i, j);
         QColor im2 = otherImage->pixel(i, j);
         if ((std::fabs(im1.red() - im2.red()) > tolerance) ||
             (std::fabs(im1.green() - im2.green()) > tolerance) ||
             (std::fabs(im1.blue() - im2.blue()) > tolerance)) {
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
   
   QString errorMessage;
   if (image.width() <= 0) {
      errorMessage = "Image width is zero.";
   }
   if (image.height() <= 0) {
      if (errorMessage.isEmpty() == false) errorMessage += "\n";
      errorMessage = "Image height is zero.";
   }
   if (errorMessage.isEmpty() == false) {
      throw FileException(FileUtilities::basename(filename)
                          + "  " + errorMessage);
   }
   
   QString format(StringUtilities::makeUpperCase(FileUtilities::filenameExtension(filename)));
   if (format == "JPG") {
      format = "JPEG";
   }
   
   QImageWriter writer(filename);
   writer.setFormat(format.toAscii().constData());
   writer.setFileName(filename);
   if (writer.write(image) == false) {
      throw FileException(writer.errorString());
   }
   
   //image.save(filename, format.toAscii().constData());
   
   //
   // Update file permissions ?
   //
   if (getFileWritePermissions() != 0) {
      QFile::setPermissions(filename, getFileWritePermissions());
   }
   
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
      
