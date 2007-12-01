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

#ifndef __HTTP_FILE_DOWNLOAD_H__
#define __HTTP_FILE_DOWNLOAD_H__

#include <map>
#include <vector>

#include <QMutex>
#include <QObject>

class QHttp;
class QHttpResponseHeader;
class QTimer;

/// class for downloading a file specified by a URL synchronously
class HttpFileDownload : public QObject  {
   Q_OBJECT
   
   public:
      /// the type of HTTP request
      enum HTTP_REQUEST_TYPE {
         HTTP_REQUEST_TYPE_GET,
         HTTP_REQUEST_TYPE_POST
      };
      
      /// where to place downloaded data
      enum DOWNLOAD_MODE {
         DOWNLOAD_MODE_TO_FILE,
         DOWNLOAD_MODE_TO_STRING
      };
      
      /// Constructor for downloading to a file
      HttpFileDownload(const QString& fileUrlIn, 
                       const QString& outputFileNameIn,
                       const int timeoutAmountInSeconds);
      
      /// Constructor for downloading to a string 
      HttpFileDownload(const QString& fileUrlIn, 
                       const int timeoutAmountInSeconds);

      /// Destructor
      ~HttpFileDownload();

      /// set the type of HTTP request
      void setHttpRequestType(const HTTP_REQUEST_TYPE hrt) { httpRequestType = hrt; }
      
      /// get the type of HTTP request
      HTTP_REQUEST_TYPE getHttpRequestType() const { return httpRequestType; }
      
      /// set the post request content
      void setPostRequestContent(const QString& s) { postRequestContent = s; }
      
      /// get the post request content
      QString getPostRequestContent() const { return postRequestContent; }
      
      /// downloads the file
      void download();

      /// find out if the download was successful
      bool getDownloadSuccessful() const { return fileDownloadSuccessful; }
      
      /// get the error message if the download is unsuccessful
      QString getErrorMessage() const { return errorMessage; }
       
      /// Get the content of the downloaded data.
      void getContent(QString& content) const;
      
      /// Get the content of the downloaded data as UTF-8 string.
      void getContentUTF8(QString& content) const;
      
      /// get the response code
      int getResponseCode() const { return responseCode; }
      
      /// get all response header elements
      std::map<QString, QString> getResponseHeader() const
                                       { return responseHeaderElements; }
      
      /// additional items for the request header
      void addToRequestHeader(const std::map<QString, QString>& rh) {
         requestHeaderElements = rh;
      }
      
      /// get a respone header element with the specified key
      QString getResponseHeaderElement(const QString& key) const;
      
      /// set the upload file name
      void setUploadFileName(const QString& dataFileName,
                             const QString& sumsFileName);
      
      /// get the upload file name
      QString getUploadFileName() const { return uploadFileName; }
      
      /// Upload a file to SuMS.
      void uploadFileToSums(const std::vector<QString>& additionalTags,
                            const std::vector<QString>& additionalValues);
      
      /// set the upload file comment
      void setUploadFileComment(const QString& s) { uploadFileComment = s; }
      
      /// get the upload file comment
      QString getUploadFileComment() const { return uploadFileComment; }
      
   protected slots:
      /// Called when the download completes
      void slotDone(bool error);
      
      /// Called when timeout occurs
      void slotTimeout();
      
      /// States of http request.
      void slotStateChanged(int state);
   
      /// http header from get
      void slotResponseHeaderReceived(const QHttpResponseHeader& resp);
      
      /// called when data available to read
      void slotReadyRead(const QHttpResponseHeader& resp);
      
   protected:
      /// Initialize this instance.
      void initialize(const QString& fileUrlIn,
                      const int timeoutAmountInSeconds,
                      const DOWNLOAD_MODE downloadModeIn);
                             
      /// see if the download is complete
      bool getDownloadComplete();
      
      /// print the request header
      void printRequestHeader();

      /// set the download complete
      void setDownloadComplete(const bool b);
      
      /// type of http request
      HTTP_REQUEST_TYPE httpRequestType;
      
      /// content for a post request
      QString postRequestContent;
      
      /// URL of file that is to be downloaded
      QString fileUrl;
      
      /// timeout amount 
      int timeoutAmount;
      
      /// the timeout timer
      QTimer* timeoutTimer;
      
      /// the HTTP object used to download the file
      QHttp* http;
      
      /// mutex for download complete flag
      QMutex downloadCompleteMutex;
      
      /// completed flag
      bool fileDownloadComplete;
      
      /// file download was successful
      bool fileDownloadSuccessful;
      
      /// error message if a failure
      QString errorMessage;
      
      /// output file name when writing to a file
      QString outputFileName;
      
      /// the download mode
      DOWNLOAD_MODE downloadMode;
      
      /// the response code from the HTTP request
      int responseCode;
      
      /// response header elements
      std::map<QString, QString> responseHeaderElements;
      
      /// additional header items for request
      std::map<QString, QString> requestHeaderElements;
      
      /// name of file for uploading
      QString uploadFileName;
      
      /// name of file to put in sums when uploading
      QString uploadSumsFileName;
      
      /// upload file comment
      QString uploadFileComment;
};

#endif // __HTTP_FILE_DOWNLOAD_H__

