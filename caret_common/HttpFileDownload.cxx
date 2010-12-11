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
#include <vector>

#include <QApplication>
#include <QDataStream>
#include <QFile>
#include <QHttp>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include <QWaitCondition>

#include "AbstractFile.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "HttpFileDownload.h"

/**
 * Constructor for download to a string.  Use getContent() to get the downloaded data.
 * If the timeout amount is less than zero, there is no timeout.
 */
HttpFileDownload::HttpFileDownload(const QString& fileUrlIn,
                                   const int timeoutAmountInSeconds)
   : QObject(0)
{
   initialize(fileUrlIn, timeoutAmountInSeconds, DOWNLOAD_MODE_TO_STRING);
}

/**
 * Constructor for downloading to a file.  
 * If the timeout amount is less than zero, there is no timeout.
 */
HttpFileDownload::HttpFileDownload(const QString& fileUrlIn,
                                   const QString& outputFileNameIn,
                                   const int timeoutAmountInSeconds)
   : QObject(0)
{
   initialize(fileUrlIn, timeoutAmountInSeconds, DOWNLOAD_MODE_TO_FILE);
   outputFileName = outputFileNameIn;
}

/**
 * Initialize this instance.
 */
void
HttpFileDownload::initialize(const QString& fileUrlIn,
                             const int timeoutAmountInSeconds,
                             const DOWNLOAD_MODE downloadModeIn)
{
   httpRequestType = HTTP_REQUEST_TYPE_GET;
   fileUrl = fileUrlIn;
   timeoutAmount = timeoutAmountInSeconds * 1000;
   downloadMode = downloadModeIn;
   
   timeoutTimer = NULL;
   http  = NULL;
   errorMessage = "";
   postRequestContent = "";
   
   fileDownloadComplete   = false;
   fileDownloadSuccessful = false;
   
   responseCode = -1;
}

/**
 * Destructor.
 */
HttpFileDownload::~HttpFileDownload()
{
   if (timeoutTimer != NULL) {
      delete timeoutTimer;
   }
   
   if (http != NULL) {
      //
      // close the connection immediately
      //
      http->abort();
   }
   // NOTE: do not delete "http" since it is attached to this object
}

/**
 * set the upload file name.
 */
void 
HttpFileDownload::setUploadFileName(const QString& dataFileName,
                                    const QString& sumsFileName) 
{ 
   uploadFileName = dataFileName; 
   uploadSumsFileName= sumsFileName;
}
      

/**
 * Upload a file to SuMS.
 */
void
HttpFileDownload::uploadFileToSums(const std::vector<QString>& additionalTags,
                                   const std::vector<QString>& additionalValues)
{
  // fileUrl = "http://pulvinar.wustl.edu/cgi-bin/upload2.cgi";
   
   if (DebugControl::getDebugOn()) {
      std::cout << "Trying to upload file: " << uploadSumsFileName.toAscii().constData()  << std::endl;
   }
   
   //
   // Create a temporary file for uploading
   //
   QString temporaryFileName("caret_sums_upload.tmp");
   QFile::remove(temporaryFileName);
   QFile file(temporaryFileName);
   if (file.open(QIODevice::WriteOnly) == false) {
      errorMessage = "Unable to open temporary file for writing: ";
      errorMessage.append(temporaryFileName);
      return;
   }
   QTextStream textStream(&file);
   
   const QString carriageReturnLineFeed = "\015\012";  //\r\n";  //"\n\r";
   
   //
   // Tag for separating different parts in the file
   //
   const QString headerBoundaryTag = "----------0xKhTmLbOuNdArY";
   const QString boundaryTag = "--" + headerBoundaryTag;
   
//   const QString uploadFileNameNoPath(FileUtilities::basename(uploadFileName));
   //
   // Place additional header info into body of file being uploaded.
   //
   std::vector<QString> tags;
   std::vector<QString> values;
   tags.push_back("upload");        values.push_back("true");
   tags.push_back("dispatch");      values.push_back("upload");
   tags.push_back("dir_id");        values.push_back("");
   tags.push_back("process");       values.push_back("1");
   tags.push_back("longfilename");  values.push_back(""); //uploadFileNameNoPath);
   tags.push_back("comment");       values.push_back(uploadFileComment);
   tags.push_back("caret_xml");     values.push_back("yes");
   if (additionalTags.empty() == false) {
      tags.insert(tags.end(), additionalTags.begin(), additionalTags.end());
      values.insert(values.end(), additionalValues.begin(), additionalValues.end());
   }
   for (unsigned int i = 0; i < tags.size(); i++) {
         textStream << boundaryTag << carriageReturnLineFeed;
         textStream << "Content-Disposition: "
                    << "form-data; name=\"" << tags[i] << "\""
                    << carriageReturnLineFeed;
      //   textStream << "Content-Type: text/plain" << carriageReturnLineFeed;
         textStream << carriageReturnLineFeed;
         textStream << values[i] << carriageReturnLineFeed;
   }
   
   //
   // Add on the upload file's information
   //
   textStream << boundaryTag << carriageReturnLineFeed;
   textStream << "Content-Disposition: form-data; name=\"theFile\"; "
              << "filename=\"" << uploadSumsFileName << "\"" << carriageReturnLineFeed;
//              << "filename=\"" << uploadFileNameNoPath << "\"" << carriageReturnLineFeed;
   textStream << "Content-Type: application/octet-stream" << carriageReturnLineFeed;
   textStream << carriageReturnLineFeed;

// STILL BROKEN IN QT 4.2.2
//#ifdef QT4_FILE_POS_BUG
   file.close();   
   if (file.open(QIODevice::Append) == false) {
      errorMessage = "Unable to open temporary file for appending due to QT4 bug: ";
      errorMessage.append(temporaryFileName);
      return;
   }
//#endif // QT4_FILE_POS_BUG
   QDataStream dataStream(&file);
   dataStream.setVersion(QDataStream::Qt_4_3);

   // file contents
   QFile inputFile(uploadFileName);
   if (inputFile.open(QIODevice::ReadOnly)) {
      QDataStream inputStream(&inputFile);
      inputStream.setVersion(QDataStream::Qt_4_3);
      
      const int inputFileSize = inputFile.size();
      const int bufferSize = 4096;
      char buffer[bufferSize];
      int bytesRemaining = inputFileSize - inputFile.pos();
      int bytesToRead = std::min(bufferSize, bytesRemaining);
      inputStream.readRawData(buffer, bytesToRead);
      while (bytesToRead > 0) {
         dataStream.writeRawData(buffer, bytesToRead);
         bytesRemaining = inputFileSize - inputFile.pos();
         bytesToRead = std::min(bufferSize, bytesRemaining);
         if (bytesToRead > 0) {
            inputStream.readRawData(buffer, bytesToRead);
         }
      }
      inputFile.close();
   }
   else {
      errorMessage = "Unable to open data file for reading: ";
      errorMessage.append(uploadFileName);
      return;
   }
  
// STILL BROKEN IN QT 4.2.2
//#ifdef QT4_FILE_POS_BUG
   file.close();
   if (file.open(QIODevice::Append) == false) {
      errorMessage = "Unable to open temporary file for appending due to QT4 bug: ";
      errorMessage.append(temporaryFileName);
      return;
   }
   QTextStream textStream2(&file);
   textStream2 << carriageReturnLineFeed;   
   textStream2 << boundaryTag << "--";
   textStream2 << carriageReturnLineFeed;
//#else // QT4_FILE_POS_BUG
//   textStream << carriageReturnLineFeed;   
//   textStream << boundaryTag << "--";
//   textStream << carriageReturnLineFeed;
//#endif // QT4_FILE_POS_BUG
   
   //
   // close the file
   //
   file.close();
   if (DebugControl::getDebugOn()) {
      std::cout << "File Size: " << file.size() << std::endl;
   }
 
   //
   // Reopen the file as readonly
   //
   if (file.open(QIODevice::ReadOnly) == false) {
      errorMessage = "Unable to open temporary file for reading: ";
      errorMessage.append(temporaryFileName);
      return;
   }
   
   //
   // Get the hostname, port, and file name
   //   
   if (DebugControl::getDebugOn()) {
      std::cout << "Uploading to: " << fileUrl.toAscii().constData() << std::endl;
   }

   QUrl u(fileUrl);
   const QString urlHostName(u.host());
   int urlPort = u.port();
   if (urlPort < 0) {
      urlPort = 80;
   }
   //const QString urlFileAndPath(u.encodedPathAndQuery());

   if (DebugControl::getDebugOn()) {
      std::cout << "host:   " << urlHostName.toAscii().constData() << std::endl;
      std::cout << "port:   " << urlPort << std::endl;
      //std::cout << "file:   " << urlFileAndPath << std::endl;
      //std::cout << "filename: " << u.fileName() << std::endl;
      //std::cout << "path:   " << u.path() << std::endl;
      //NOTE: printing query value causes all printing to terminal to stop.  Why?
      //std::cout << "query:  " << u.query() << std::endl;
   }
   QString pathAndQuery(u.path());
   if (u.encodedQuery().isEmpty() == false) {
      pathAndQuery.append("?");
      pathAndQuery.append(u.encodedQuery());
   }
   
   //
   // create the HTTP object
   //
   http = new QHttp(urlHostName, urlPort, this);
   QObject::connect(http, SIGNAL(done(bool)),
                    this, SLOT(slotDone(bool)));
   QObject::connect(http, SIGNAL(stateChanged(int)),
                    this, SLOT(slotStateChanged(int)));
   QObject::connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
                    this, SLOT(slotResponseHeaderReceived(const QHttpResponseHeader&)));
   QObject::connect(http, SIGNAL(readyRead(const QHttpResponseHeader&)),
                    this, SLOT(slotReadyRead(const QHttpResponseHeader&)));
   //
   // Create a timeout timer if requested
   //
   if (timeoutAmount > 0) {
      timeoutTimer = new QTimer;
      QObject::connect(timeoutTimer, SIGNAL(timeout()),
                       this, SLOT(slotTimeout()));
      timeoutTimer->setSingleShot(true);
      timeoutTimer->start(timeoutAmount);
   }
   
   QHttpRequestHeader header("POST", pathAndQuery);
   QString hp = u.host();
   if (urlPort != 80) { 
      hp += ":";
      hp += QString::number(urlPort);
   }
   header.setValue("host", hp);
   header.setValue("connection", "close");
   http->setHost(urlHostName, urlPort);
   header.setValue("Accept", "*/*");
   header.setContentType("multipart/form-data; boundary=" + headerBoundaryTag);
   for (std::map<QString, QString>::iterator pos = requestHeaderElements.begin();
        pos != requestHeaderElements.end(); pos++) {
      header.setValue(pos->first, pos->second);
   }
           
   http->request(header, &file);

   printRequestHeader();
   
   if (DebugControl::getDebugOn()) {
      QHttpRequestHeader req = http->currentRequest();
      if (req.isValid()) {
         std::cout << "Request header path: " << req.path().toAscii().constData() << std::endl;
         std::cout << "Request method:      " << req.method().toAscii().constData() << std::endl;
      }
   }
 
   //
   // Wait until done
   //
   while (getDownloadComplete() == false) {      
      //
      // Allow other stuff to do things
      //
      qApp->processEvents();  //100);
   }
   
   file.close();

   //
   // Remove the temporary file if debug is off
   //
   if (DebugControl::getDebugOn() == false) {
      QFile::remove(temporaryFileName);
   }
}

/**
 * downloads the file.
 */
void 
HttpFileDownload::download()
{
   if (DebugControl::getDebugOn()) {
      std::cout << "Trying to download file: " << fileUrl.toAscii().constData() << std::endl;
   }
   
   //
   // Get the hostname, port, and file name
   //   
   QUrl u(fileUrl);
   const QString urlHostName(u.host());
   int urlPort = u.port();
   if (urlPort < 0) {
      urlPort = 80;
   }
   //const QString urlFileAndPath(u.encodedPathAndQuery());

   if (DebugControl::getDebugOn()) {
      std::cout << "host:   " << urlHostName.toAscii().constData() << std::endl;
      std::cout << "port:   " << urlPort << std::endl;
      //std::cout << "file:   " << urlFileAndPath << std::endl;
      //std::cout << "filename: " << u.fileName() << std::endl;
      //std::cout << "path:   " << u.path() << std::endl;
      //NOTE: printing query value causes all printing to terminal to stop.  Why?
      //std::cout << "query:  " << u.query() << std::endl;
   }
   QString pathAndQuery(u.path());
   if (u.encodedQuery().isEmpty() == false) {
      pathAndQuery.append("?");
      pathAndQuery.append(u.encodedQuery());
   }
   
   //
   // create the HTTP object
   //
   http = new QHttp(urlHostName, urlPort, this);
   QObject::connect(http, SIGNAL(done(bool)),
                    this, SLOT(slotDone(bool)));
   QObject::connect(http, SIGNAL(stateChanged(int)),
                    this, SLOT(slotStateChanged(int)));
   QObject::connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
                    this, SLOT(slotResponseHeaderReceived(const QHttpResponseHeader&)));
   QObject::connect(http, SIGNAL(readyRead(const QHttpResponseHeader&)),
                    this, SLOT(slotReadyRead(const QHttpResponseHeader&)));
   //
   // Create a timeout timer if requested
   //
   if (timeoutAmount > 0) {
      timeoutTimer = new QTimer;
      QObject::connect(timeoutTimer, SIGNAL(timeout()),
                       this, SLOT(slotTimeout()));
      timeoutTimer->setSingleShot(true);
      timeoutTimer->start(timeoutAmount);
   }
   
   //
   // the http request header
   //
   QString modeString;
   switch (httpRequestType) {
      case HTTP_REQUEST_TYPE_GET:
         modeString = "GET";
         break;
      case HTTP_REQUEST_TYPE_POST:
         modeString = "POST";
         break;
   }
   
   QHttpRequestHeader header(modeString, pathAndQuery);
   QString hp = u.host();
   if (urlPort != 80) {
      hp += ":";
      hp += QString::number(urlPort);
   }
   header.setValue("host", hp);
   for (std::map<QString, QString>::iterator pos = requestHeaderElements.begin();
        pos != requestHeaderElements.end(); pos++) {
      header.setValue(pos->first, pos->second);
   }
   header.setValue("connection", "close");
   http->setHost(urlHostName, urlPort);
   switch (httpRequestType) {
      case HTTP_REQUEST_TYPE_GET:
         http->request(header);
         break;
      case HTTP_REQUEST_TYPE_POST:
         {
            header.setValue("pragma", "no-cache");
            //header.setValue("Accept", "*/*");
            header.setContentType("application/x-www-form-urlencoded");
            const QString content(postRequestContent);
            http->request(header, content.toUtf8());
         }
         break;
   }
   printRequestHeader();
   
   if (DebugControl::getDebugOn()) {
      QHttpRequestHeader req = http->currentRequest();
      if (req.isValid()) {
         std::cout << "Request header path: " << req.path().toAscii().constData() << std::endl;
         std::cout << "Request method:      " << req.method().toAscii().constData() << std::endl;
      }
   }
 
   //
   // Wait until done
   //
   while (getDownloadComplete() == false) {      
      //
      // Allow other stuff to do things
      //
      qApp->processEvents();  //100);
   }
}

void
HttpFileDownload::printRequestHeader()
{
   if (DebugControl::getDebugOn() == false) {
      return;
   }

   QHttpRequestHeader head = http->currentRequest();
   if (head.isValid()) {
      QStringList keys = head.keys();
      for (QStringList::Iterator it = keys.begin();
           it != keys.end(); it++) {
         const QString key = *it;
         const QString value = head.value(key);
         std::cout << "REQUEST HEADER key (" << key.toAscii().constData() << ") value (" << value.toAscii().constData() << ")"
                   << std::endl;
      }
   }
}

/**
 * Get the content of the downloaded data.
 */
void
HttpFileDownload::getContent(QString& content) const
{
   content = QString(http->readAll());
}

/**
 * Get the content of the downloaded data as UTF-8 string.
 */
void 
HttpFileDownload::getContentUTF8(QString& content) const
{
   content = QString::fromUtf8(http->readAll().data());
}

/**
 * Called when the download completes.
 */
void 
HttpFileDownload::slotDone(bool error)
{
   if (timeoutTimer != NULL) {
      timeoutTimer->stop();
   }
   if (error == false) {
      if (responseCode == 200) {
         fileDownloadSuccessful = true;
      }
      
      switch (downloadMode) {
         case DOWNLOAD_MODE_TO_FILE:
            {
               //
               // Create the file
               //
               const QByteArray ba = http->readAll();
               const int num = ba.size();
               if (DebugControl::getDebugOn()) {
                  std::cout << "File size downloaded: " << num << std::endl;
               }
               QFile file(outputFileName);
               if (file.open(QIODevice::WriteOnly)) {
                  QDataStream stream(&file);
                  stream.setVersion(QDataStream::Qt_4_3);
                  stream.writeRawData(ba.data(), num);
                  file.close();
               }
               else {
                  errorMessage.append("Unable to open file for writing: ");
                  errorMessage.append(outputFileName);
               }
            }
            break;
         case DOWNLOAD_MODE_TO_STRING:
            break;
      }
   }
   else {
      errorMessage.append(http->errorString());
   }
   //http->closeConnection();
   http->abort();  // force connection to close
   setDownloadComplete(true);
   
   if (DebugControl::getDebugOn()) {
      std::cout << "In slotDone" << std::endl;
   }
}

/**
 * Called when timeout occurs.
 */
void 
HttpFileDownload::slotTimeout()
{
   http->abort();
   errorMessage.append("file download timed out.");
   setDownloadComplete(true);
}

/** 
 * see if the download is complete.
 */
bool 
HttpFileDownload::getDownloadComplete()
{
   bool done = false;
   downloadCompleteMutex.lock();
   done = fileDownloadComplete;
   downloadCompleteMutex.unlock();
   return done;
}

/**
 * set the download complete.
 */
void 
HttpFileDownload::setDownloadComplete(const bool b)
{
   downloadCompleteMutex.lock();
   fileDownloadComplete = b;
   downloadCompleteMutex.unlock();
}

/**
 * called when data available to read.
 */
void 
HttpFileDownload::slotReadyRead(const QHttpResponseHeader&)
{
/*
   if (DebugControl::getDebugOn()) {
      std::cout << "Bytes available to read: " << http->bytesAvailable() << std::endl;
   }
*/
}

/**
 * States of http request.
 */
void     
HttpFileDownload::slotStateChanged(int state)
{     
   if (DebugControl::getDebugOn()) {
      std::cout << "Download State: ";
      switch (state) {
        case QHttp::Unconnected:
           std::cout << "Unconnected" << std::endl;
           break;
        case QHttp::HostLookup:
           std::cout << "Host Lookup" << std::endl;
           break;
        case QHttp::Connecting:
           std::cout << "Connecting" << std::endl;
           break;
        case QHttp::Sending:
           std::cout << "Sending" << std::endl;
           break;
        case QHttp::Reading:
           std::cout << "Reading" << std::endl;
           break;
        case QHttp::Connected:
           std::cout << "Connected" << std::endl;
           break;
        case QHttp::Closing:
           std::cout << "Closing" << std::endl;
           break;
      }
      printRequestHeader();
   }
}        

/**
 * http header from get.
 */
void 
HttpFileDownload::slotResponseHeaderReceived(const QHttpResponseHeader& resp)
{
   responseCode = resp.statusCode();
   errorMessage.append(resp.reasonPhrase());
   if (DebugControl::getDebugOn()) {
      std::cout << "HTTP Response Code: " << responseCode << std::endl;
   }
   if (resp.isValid()) {
      QStringList keys = resp.keys();
      for (QStringList::Iterator it = keys.begin();
           it != keys.end(); it++) {
         const QString key = *it;
         const QString value = resp.value(key);
            
         responseHeaderElements[key] = value;
            
         if (DebugControl::getDebugOn()) {
            std::cout << "RESPONSE HEADER key (" << key.toAscii().constData() << ") value (" << value.toAscii().constData() << ")"
                         << std::endl;
         }
      }
   }
}      

/**
 * get a respone header element with the specified key.
 */
QString 
HttpFileDownload::getResponseHeaderElement(const QString& key) const
{
   const std::map<QString, QString>::const_iterator iter = 
                                           responseHeaderElements.find(key);
   if (iter != responseHeaderElements.end()) {
      return iter->second;
   }
   return "";
}
