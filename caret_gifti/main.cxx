
#include <algorithm>
#include <cstdlib>
#include <iostream>

#include <QFileInfo>
#include <QString>

#include "FileUtilities.h"
#include "GiftiDataArrayFile.h"

static void
printMetaData(const GiftiMetaData* metaData,
              const QString& indent = "")
{
   //
   // Get the meta data names and sort them
   //
   std::vector<QString> names;
   metaData->getAllNames(names);
   std::sort(names.begin(), names.end());
   
   //
   // for each meta data name
   //
   for (unsigned int i = 0; i < names.size(); i++) {
      //
      // get the value for the name as a string and print the metadata
      //
      QString value;
      metaData->get(names[i], value);
      std::cout << indent.toAscii().constData() 
                << "MetaData (" << names[i].toAscii().constData() << ") " 
                << value.toAscii().constData() << std::endl;
   }
}

/**
 * Time reading of the GIFTI file.
 */
static void
fileTime(int argc, char* argv[])
{
   if (argc == -1) {
      const QString programName(FileUtilities::basename(argv[0]));
      std::cout << "DISPLAY TIME TO READ A FILE" << std::endl;
      std::cout << "   " << programName.toAscii().constData() << " -time <file-name>" << std::endl;
      std::cout << std::endl;
      return;
   }
   if (argc <= 2) {
      std::cout << "ERROR: filename missing." << std::endl;
      exit(-1);
   }

   //
   // get the filename
   //   
   const QString filename = argv[2];
   
   //
   // Read the entire file into memory
   //
   const int numTimesToRead = 5;
   float totalTime = 0.0;
   try {
      for (int i = 0; i < numTimesToRead; i++) {
         GiftiDataArrayFile giftiFile;
         giftiFile.readFile(filename);
         totalTime += giftiFile.getTimeToReadFileInSeconds();
      }
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
      exit(-1);
   }
   
   //
   // Show the size of the file
   //
   const float oneMegaByte = 1024 * 1024;
   QFileInfo fileInfo(filename);
   const float fileSize = static_cast<float>(fileInfo.size()) / oneMegaByte;
   std::cout << "Size of File (MegaBytes): " << fileSize << std::endl;
   
   //
   // Show the average read time
   //
   const float averageTime = totalTime / static_cast<float>(numTimesToRead);
   std::cout << "Average Read Time (read " << numTimesToRead << " times) " << averageTime << std::endl;
   std::cout << std::endl;
}

/**
 * Show info about the GIFTI file.
 */
static void
fileInfo(int argc, char* argv[])
{
   if (argc == -1) {
      const QString programName(FileUtilities::basename(argv[0]));
      std::cout << "DISPLAY INFORMATION ABOUT A FILE" << std::endl;
      std::cout << "   " << programName.toAscii().constData() << " -info <file-name>" << std::endl;
      std::cout << std::endl;
      return;
   }
   if (argc <= 2) {
      std::cout << "ERROR: filename missing." << std::endl;
      exit(-1);
   }

   //
   // get the filename
   //   
   const QString filename = argv[2];
   
   //
   // Read the entire file into memory
   //
   GiftiDataArrayFile giftiFile;
   try {
      giftiFile.readFile(filename);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
      exit(-1);
   }
   
   //
   // Get the number of data arrays
   //
   const int numArrays = giftiFile.getNumberOfDataArrays();
   //
   // Echo filename, meta data, and number of data arrays
   //
   std::cout << "File: " << filename.toAscii().constData() << std::endl;
   printMetaData(giftiFile.getMetaData());
   std::cout << "Number of data arrays: " << numArrays << std::endl;
   
   //
   // Loop through the data arrays
   //
   for (int i = 0; i < numArrays; i++) {
      //
      // Get the data array
      //
      const GiftiDataArray* dataArray = giftiFile.getDataArray(i);
      
      //
      // Print info about the array
      //
      std::cout << "Data Array          " << i << std::endl;
      std::cout << "   Category:        " << dataArray->getCategory().toAscii().constData() << std::endl;
      std::cout << "   Data Type:       " 
                << GiftiDataArray::getDataTypeName(dataArray->getDataType()).toAscii().constData() << std::endl;
      std::cout << "Dimensions:         ";
      const std::vector<int> dim = dataArray->getDimensions();
      for (unsigned int i = 0; i < dim.size(); i++) {
         if (i != 0) {
            std::cout << ", ";
         }
         std::cout << dim[i];
      }
      std::cout << std::endl;
      std::cout << "   Subscript Order: " << GiftiDataArray::getArraySubscriptingOrderName(dataArray->getArraySubscriptingOrder()).toAscii().constData() << std::endl;
      std::cout << "   Encoding:        " << GiftiDataArray::getEncodingName(dataArray->getEncoding()).toAscii().constData() << std::endl;
      //std::cout << "   Location:        " << GiftiDataArray::getDataLocationName(dataArray->getArraySubscriptingOrder()).toAscii().constData() << std::endl;
      //std::cout << "   Endian:          " << GiftiDataArray::(dataArray->()).toAscii().constData() << std::endl;
     
       //
      // Get all of the meta data names
      //
      printMetaData(dataArray->getMetaData(), "   ");
   }
}

/**
 * Copy meta data
 */
static void
copyMetaData(const GiftiMetaData* copyFrom,
             GiftiMetaData* copyTo)
{
   //
   // Get all of the meta data names
   //
   std::vector<QString> names;
   copyFrom->getAllNames(names);
   
   //
   // for each meta data name
   //
   for (unsigned int i = 0; i < names.size(); i++) {
      //
      // get the value for the name as a string
      //
      QString value;
      copyFrom->get(names[i], value);
      
      //
      // add to the metadata
      //
      copyTo->set(names[i], value);
   }
}

/**
 * Copy a GIFTI file.  This is a very verbose way to copy the but done this way to show
 * how to access the GIFTI data structures.
 */
static void
fileCopy(int argc, char* argv[])
{
   if (argc == -1) {
      const QString programName(FileUtilities::basename(argv[0]));
      std::cout << "COPY A FILE" << std::endl;
      std::cout << "   " << programName.toAscii().constData() << " -copy <input-name> <output-name>" << std::endl;
      std::cout << std::endl;
      return;
   }
   if (argc <= 3) {
      std::cout << "ERROR: filename(s) missing." << std::endl;
      exit(-1);
   }

   //
   // get the filename
   //   
   const QString inputFileName  = argv[2];
   const QString outputFileName = argv[3];
   
   //
   // Read the entire file into memory
   //
   GiftiDataArrayFile inputGiftiFile;
   try {
      inputGiftiFile.readFile(inputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
      exit(-1);
   }

   //
   // Create the output file
   //   
   GiftiDataArrayFile outputGiftiFile;
   
   //
   // copy the file's metadata
   // 
   copyMetaData(inputGiftiFile.getMetaData(),
                outputGiftiFile.getMetaData());
                
   //
   // Get the number of data arrays
   //
   const int numArrays = inputGiftiFile.getNumberOfDataArrays();
   
   //
   // Loop through the data arrays
   //
   for (int i = 0; i < numArrays; i++) {
      //
      // Get the data array that is to be copied
      //
      const GiftiDataArray* dataArrayToCopy = inputGiftiFile.getDataArray(i);
      
      //
      // Get the dimensions of the data
      const std::vector<int> dimensions = dataArrayToCopy->getDimensions();

      //
      // Only supports two-dimensional data at this time
      //
      if (dimensions.size() != 2) {
         std::cout << "ERROR: only 2-dimensional data supported for copying files." << std::endl;
         exit(-1);
      }
      
      //
      // Get the data type
      //
      const GiftiDataArray::DATA_TYPE dataType = dataArrayToCopy->getDataType();
      
      //
      //
      // Create a new data array
      //
      GiftiDataArray* newDataArray = new GiftiDataArray(&outputGiftiFile,
                                                        dataArrayToCopy->getCategory(),
                                                        dataType,
                                                        dimensions,
                                                        dataArrayToCopy->getEncoding());
      //
      // Copy the data
      //
      for (int i = 0; i < dimensions[0]; i++) {
         for (int j = 0; j < dimensions[1]; j++) {
            //
            // the data array index
            //
            const int indices[2] = { i, j };
            
            //
            // Copy the data value at the index
            //
            switch (dataType) {
               case GiftiDataArray::DATA_TYPE_FLOAT32:
                  newDataArray->setDataFloat32(indices, 
                                               dataArrayToCopy->getDataFloat32(indices));
                  break;
               case GiftiDataArray::DATA_TYPE_INT32:
                  newDataArray->setDataInt32(indices, 
                                             dataArrayToCopy->getDataInt32(indices));
                  break;
               case GiftiDataArray::DATA_TYPE_UINT8:
                  newDataArray->setDataUInt8(indices, 
                                             dataArrayToCopy->getDataUInt8(indices));
                  break;
            }
         }
      }
      
      //
      // Copy the array's metadata
      //
      copyMetaData(dataArrayToCopy->getMetaData(),
                   newDataArray->getMetaData());
                   
      //
      // Add the data array to the output file
      //
      outputGiftiFile.addDataArray(newDataArray);
   }
   
   //
   // Write the output file
   //
   try {
      outputGiftiFile.writeFile(outputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
      exit(-1);
   }
}

/**
 * Change a GIFTI file's data encoding.  The input file name may be the same
 * as the output file name.
 */
static void
fileEncoding(int argc, char* argv[])
{
   if (argc == -1) {
      const QString programName(FileUtilities::basename(argv[0]));
      std::cout << "CHANGE A FILE'S ENCODING" << std::endl;
      std::cout << "   " << programName.toAscii().constData() << " -encode <encoding> <input-name> <output-name>" << std::endl;
      std::cout << "      encoding is one of: " << std::endl;
      std::cout << "         \"text\"    data stored as ASCII text." << std::endl;
#ifdef HAVE_VTK
      std::cout << "         \"base64\"  data stored as base64 text." << std::endl;
      std::cout << "         \"gzip\"    data compressed and stored as base64 text." << std::endl;
#endif // HAVE_VTK
      std::cout << std::endl;
      return;
   }
   if (argc <= 4) {
      std::cout << "ERROR: encoding or filename(s) missing." << std::endl;
      exit(-1);
   }

   //
   // get the filename
   //   
   const QString encodingName   = argv[2];
   const QString inputFileName  = argv[3];
   const QString outputFileName = argv[4];
   
   //
   // Verify the encoding
   //
   GiftiDataArray::ENCODING encoding = GiftiDataArray::ENCODING_ASCII;
   if (encodingName == "text") {
      encoding = GiftiDataArray::ENCODING_ASCII;
   }
   else if (encodingName == "base64") {
#ifndef HAVE_VTK
      std::cout << "ERROR: base64 encoding unavailable since VTK was not built." << std::endl;
      exit(-1);
#endif
      encoding = GiftiDataArray::ENCODING_BASE64_BINARY;
   }
   else if (encodingName == "gzip") {
#ifndef HAVE_VTK
      std::cout << "ERROR: gzip encoding unavailable since VTK was not built." << std::endl;
      exit(-1);
#endif
      encoding = GiftiDataArray::ENCODING_COMPRESSED_BASE64_BINARY;
   }
   else {
      std::cout << "ERROR: Unrecognized encoding \"" << encodingName.toAscii().constData() << "\"" << std::endl;
   }
   
   //
   // Read the entire file into memory
   //
   GiftiDataArrayFile giftiFile; 
   try {
      giftiFile.readFile(inputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
      exit(-1);
   }

   //
   // Get the number of data arrays
   //
   const int numArrays = giftiFile.getNumberOfDataArrays();
   
   //
   // Loop through the data arrays
   //
   for (int i = 0; i < numArrays; i++) {
      //
      // Get the data array that is to be encoded
      //
      GiftiDataArray* dataArray = giftiFile.getDataArray(i);
      
      //
      // set the encoding on the array
      //
      dataArray->setEncoding(encoding);
   }
   
   //
   // Write the output file
   //
   try {
      giftiFile.writeFile(outputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.what() << std::endl;
      exit(-1);
   }
}

/**
 * The MAIN function.
 */
int
main(int argc, char* argv[]) 
{
   //
   // Check for valid number of arguments
   //
   if (argc <= 1) {
      std::cout << std::endl;
      fileCopy(-1, argv);
      fileEncoding(-1, argv);
      fileInfo(-1, argv);
      fileTime(-1, argv);
      exit(0);
   } 

   //
   // Determine the usage mode
   //   
   QString modeValue(argv[1]);
   
   if (modeValue == "-copy") {
      fileCopy(argc, argv);
   }
   else if (modeValue == "-encode") {
      fileEncoding(argc, argv);
   }
   else if ((modeValue == "-h") || 
            (modeValue == "-help")) {
      std::cout << std::endl;
      fileCopy(-1, argv);
      fileEncoding(-1, argv);
      fileInfo(-1, argv);
      fileTime(-1, argv);
      exit(0);
   }
   else if (modeValue == "-info") {
      fileInfo(argc, argv);
   }
   else if (modeValue == "-time") {
      fileTime(argc, argv);
   }
   else {
      std::cout << "ERROR: invalid mode." << std::endl;
   }
   
   return 0;   
}
