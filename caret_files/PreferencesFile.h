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


#ifndef __VE_PREFERENCES_FILE_H__
#define __VE_PREFERENCES_FILE_H__

#include "AbstractFile.h"
#include "VolumeFile.h"

/// This class is used to store user preferences.
class PreferencesFile : public AbstractFile {
   public:
      /// class for storing user views
      class UserView {
         public:
            /// constructor
            UserView(const QString& nameIn,
                     const float rotationIn[16], 
                     const float translationIn[3],
                     const float scalingIn[3],
                     const bool  rotationValidIn,
                     const bool  translationValidIn,
                     const bool  scalingValidIn);
                     
            /// get user view information
            void getViewInfo(QString& nameOut,
                             float  rotationOut[16], 
                             float  translationOut[3],
                             float scalingOut[3],
                             bool&  rotationValidOut,
                             bool&  translationValidOut,
                             bool&  scalingValidOut) const;
            
            /// get the name of the view
            QString getViewName() const { return name; }

         private:
            /// name of view
            QString name;
            
            /// rotation matrix
            float rotation[16];
            
            /// translation
            float translation[3];
            
            /// scaling
            float scaling[3];
            
            /// rotation valid
            bool rotationValid;
            
            /// translation valid
            bool translationValid;
            
            /// scaling valid
            bool scalingValid;
      };

      /// image capture type
      enum IMAGE_CAPTURE_TYPE {
         IMAGE_CAPTURE_PIXMAP = 0,
         IMAGE_CAPTURE_OPENGL_BUFFER = 1
      };
      
      /// Constructor
      PreferencesFile();
      
      /// Destructor
      ~PreferencesFile();
      
      /// clear the preferences file
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return false; }
      
      /// get the fmri algorithm parameters (const method)
      const QString getFmriAlgorithmParameters() const { return fmriParametersString; }

      /// set the fmri algorithm parameters
      void setFmriAlgorithmParameters(const QString& s);
      
      /// get the surface background color
      void getSurfaceBackgroundColor(unsigned char& r, unsigned char& g, unsigned char& b) const;
      
      /// set the surface background color
      void setSurfaceBackgroundColor(const unsigned char r, const unsigned char g,
                                     const unsigned char b);
                                      
      /// get the surface foreground color
      void getSurfaceForegroundColor(unsigned char& r, unsigned char& g, unsigned char& b) const;
      
      /// set the surface foreground color
      void setSurfaceForegroundColor(const unsigned char r, const unsigned char g,
                                     const unsigned char b);
              
      /// get the light position
      void getLightPosition(float& x, float& y, float& z) const;
      
      /// set the light position
      void setLightPosition(const float x, const float y, const float z);
      
      /// get the mouse speed
      float getMouseSpeed() const;
      
      /// set the mouse speed
      void setMouseSpeed(const float speed);
      
      /// add to recent spec files
      void addToRecentSpecFiles(const QString& specFileName,
                                const bool writePreferencesFile);
      
      /// get the recent spec files
      void getRecentSpecFiles(std::vector<QString>& files) const;
      
      /// set the recent spec files
      void setRecentSpecFiles(const std::vector<QString>& files);

      /// add to recent copied spec files
      void addToRecentCopiedSpecFiles(const QString& specFileName,
                                      const bool writePreferencesFile);
      
      /// get the recent copied spec files (used in fmri mapper)
      void getRecentCopiedSpecFiles(std::vector<QString>& files) const;
      
      /// set the recent copied spec files (used in fmri mapper)
      void setRecentCopiedSpecFiles(const std::vector<QString>& files);

      /// add to recent data file directories
      void addToRecentDataFileDirectories(const QString& dirName,
                                          const bool writePreferencesFile);
                                          
      /// get the recent data file directories
      void getRecentDataFileDirectories(std::vector<QString>& dirs) const;
      
      /// get the recent data file directories
      QStringList getRecentDataFileDirectories() const;
      
      // get the test flag 1
      bool getTestFlag1() const;
      
      // set the test flag 1
      void setTestFlag1(const bool val);
      
      // get the test flag 2
      bool getTestFlag2() const;
      
      // set the test flag 2
      void setTestFlag2(const bool val);
      
      // get the OpenGL debug
      bool getOpenGLDebug() const;
      
      // set the OpenGL debug
      void setOpenGLDebug(const bool val);
      
      /// get the debug default value
      bool getDebugOn() const;    

      /// set the debug default value
      void setDebugOn(const bool val);

      /// get the debug node number
      int getDebugNodeNumber() const;

      /// set the debug node number
      void setDebugNodeNumber(const int num);

      /// get the iterative update
      int getIterativeUpdate() const;

      /// set the iterative update
      void setIterativeUpdate(const int iter);
      
      /// get the web browser
      QString getWebBrowser() const { return webBrowser; }
      
      /// set the web browser
      void setWebBrowser(const QString wb);
      
      /// get the number of user views
      int getNumberOfUserViews() const { return userViews.size(); }
      
      /// get a user view
      const UserView* getUserView(const int viewNumber) const;
                       
      /// add a user view
      void addUserView(const PreferencesFile::UserView& uv);
                       
      /// get the image capture type
      IMAGE_CAPTURE_TYPE getImageCaptureType() const { return imageCaptureType; }
      
      /// set the image capture type
      void setImageCaptureType(const IMAGE_CAPTURE_TYPE ict);
      
      /// get the anatomy volume contrast
      int getAnatomyVolumeContrast() const { return anatomyVolumeContrast; }
      
      /// set the anatomy volume contrast
      void setAnatomyVolumeContrast(const int c);
      
      /// get the anatomy volume brightness
      int getAnatomyVolumeBrightness() const { return anatomyVolumeBrightness; }
      
      /// set the anatomy volume brightness
      void setAnatomyVolumeBrightness(const int b);
      
      /// get number of file reading threads
      int getNumberOfFileReadingThreads() const { return numberOfFileReadingThreads; }
      
      /// set number of file reading threads
      void setNumberOfFileReadingThreads(const int num) { numberOfFileReadingThreads = num; }
      
      /// get the maximum number of threads
      int getMaximumNumberOfThreads() const;

      /// set the maximum number of threads
      void setMaximumNumberOfThreads(const int num);
      
      /// get the number of digits right of the decimal when writing float to text files
      int getTextFileDigitsRightOfDecimal()
                       { return textFileDigitsRightOfDecimal; }
                       
      /// set the number of digits right of the decimal when writing float to text files
      void setTextFileDigitsRightOfDecimal(const int num) 
                       { textFileDigitsRightOfDecimal = num; }
      
      /// get the preferred volume file type
      VolumeFile::FILE_READ_WRITE_TYPE getPreferredVolumeWriteType() const
                                           { return preferredVolumeWriteType; }

      /// set the prefered volume file type
      void setPreferredVolumeWriteType(const VolumeFile::FILE_READ_WRITE_TYPE pt) 
                                                  { preferredVolumeWriteType = pt; }
                         
      /// get the preferred file writing data type
      std::vector<AbstractFile::FILE_FORMAT> getPreferredWriteDataType() const
                                                  { return preferredWriteDataType; }
                                  
      /// set the preferred file writing data type
      void setPreferredWriteDataType(const std::vector<AbstractFile::FILE_FORMAT> pt)
                                                       { preferredWriteDataType = pt; }
                                  
      /// get SuMS login information
      void getSumsLoginInformation(QString& userName,
                                   QString& password,
                                   QString& hostName,
                                   bool& useLogin,
                                   bool& infoValid) const;
                                   
      /// set SuMS login information
      void setSumsLoginInformation(const QString& userName,
                                   const QString& password,
                                   const QString& hostName,
                                   const bool useLogin,
                                   const bool infoValid);
                                   
      /// get the SuMS database hosts
      QString getSumsDatabaseHosts() const { return sumsDatabaseHosts; }
      
      /// set the SuMS database hosts
      void setSumsDatabaseHosts(const QString& hosts) { sumsDatabaseHosts = hosts; }
      
      /// get the SuMS database file timeout length in seconds
      int getSumsDatabaseDataFileTimeout() const { return sumsDataFileTimeOut; }
      
      /// set the SuMS database file timeout length in seconds
      void setSumsDatabaseDataFileTimeout(const int t) { sumsDataFileTimeOut = t; }
      
      /// get the significant digits display
      int getSignificantDigitsDisplay() const { return significantDigitsDisplay; }
      
      /// set the significant digits display
      void setSignificantDigitsDisplay(const int n) { significantDigitsDisplay = n; }
      
      /// get display lists enabled
      bool getDisplayListsEnabled() const { return displayListsEnabled; }
      
      /// set display lists enabled
      void setDisplayListsEnabled(const bool b) { displayListsEnabled = b; }
      
      /// get override the random seed
      bool getRandomSeedOverride() const { return randomSeedOverrideFlag; }
      
      /// set override the random seed
      void setRandomSeedOverride(const bool b) { randomSeedOverrideFlag = b; }
      
      /// get override random seed value
      unsigned int getRandomSeedOverrideValue() const { return randomSeedOverrideValue; }
      
      /// set override random seed value
      void setRandomSeedOverrideValue(const unsigned int ui) { randomSeedOverrideValue = ui; }
      
      /// set caret tips
      void setCaretTips(const int tipIndex, const bool showTips);
      
      /// get caret tips
      void getCaretTips(int& tipIndex, bool& showTips) const;
      
   private:
      
      /// fmri mapping parameters string
      QString fmriParametersString;
      
      /// the user views
      std::vector<UserView> userViews;
      
      /// the image capture type
      IMAGE_CAPTURE_TYPE imageCaptureType;
      
      /// surface background color
      unsigned char surfaceBackgroundColor[3];
      
      /// surface foreground color
      unsigned char surfaceForegroundColor[3];
      
      /// light position
      float lightPosition[3];
      
      /// mouse speed multiplier
      float mouseSpeed;
      
      /// recent spec files
      std::vector<QString> recentSpecFiles;
      
      /// recent spec copied files
      std::vector<QString> recentCopiedSpecFiles;
      
      /// recent data file directories
      std::vector<QString> recentDataFileDirectories;
      
      /// web browser
      QString webBrowser;
      
      /// anatomy volume contrast
      int anatomyVolumeContrast;
      
      /// anatomy volume brightness
      int anatomyVolumeBrightness;
      
      /// maximum number of threads
      int maximumNumberOfThreads;
      
      /// number of file reading threads
      int numberOfFileReadingThreads;
      
      /// update debuggin stuff
      void updateDebugging();

      /// the number of digits right of the decimal when writing float to text files
      int textFileDigitsRightOfDecimal;
      
      /// preferred data write type for files
      std::vector<AbstractFile::FILE_FORMAT> preferredWriteDataType;
      
      /// SuMS user name
      QString sumsUserName;
      
      /// SuMS password
      QString sumsPassWord;
      
      /// SuMS host name
      QString sumsHostName;
      
      /// SuMS use login
      bool sumsLoginWithUserNameAndPassWord;
      
      /// SuMS user information valid
      bool sumsUserInfoValid;
      
      /// additional SuMS database host
      QString sumsDatabaseHosts;
      
      /// SuMS data file timeout (seconds)
      int sumsDataFileTimeOut;
      
      /// significant digits display
      int significantDigitsDisplay;
      
      /// display list enabled
      bool displayListsEnabled;
      
      /// random seed override
      bool randomSeedOverrideFlag;
      
      /// random seed override value
      unsigned int randomSeedOverrideValue;
      
      /// index of current tip
      int caretTipIndex;
      
      /// caret tips enabled
      bool caretTipsEnabled;
      
      /// preferred volume write type
      VolumeFile::FILE_READ_WRITE_TYPE preferredVolumeWriteType;
       
      /// Read the preference file data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// Write the preference file data 
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);   
      
   static const QString tagSurfaceBackgroundColor;
   static const QString tagSurfaceForegroundColor;
   static const QString tagMouseSpeed;
   static const QString tagLightPosition;
   static const QString tagRecentSpecFiles;
   static const QString tagRecentCopiedSpecFiles;
   static const QString tagRecentDataFileDirectories;
   static const QString tagOpenGLDebugOn;
   static const QString tagDebugOn;
   static const QString tagTestFlag1;
   static const QString tagTestFlag2;
   static const QString tagDebugNodeNumber;
   static const QString tagIterativeUpdate;
   static const QString tagWebBrowser;
   static const QString tagImageCaptureType;
   static const QString tagUserViewMatrix;
   static const QString tagAnatomyVolumeBrightness;
   static const QString tagAnatomyVolumeContrast;   
   static const QString tagMaximumNumberOfThreads;   
   static const QString tagNumberOfFileReadingThreads;   
   static const QString tagSpeechEnabled;
   static const QString tagFloatDigitsRightOfDecimal;
   static const QString tagPreferredWriteDataType;
   static const QString tagSumsLoginData;
   static const QString tagSumsDatabaseHosts;
   static const QString tagSumsDataFileTimeout;
   static const QString tagSignificantDigits;
   static const QString tagDisplayListsEnabled;
   static const QString tagRandomSeedOverride;
   static const QString tagPreferredVolumeWriteType;
   static const QString tagCaretTips;
   
   static const QString tagFmriParameters;
   static const QString tagFmriAlgorithm;
   static const QString tagFmriAvgVoxelNeighbors;
   static const QString tagFmriMaxVoxelNeighbors;
   static const QString tagFmriStrongestVoxelNeighbors;
   static const QString tagFmriGaussNeighbors;
   static const QString tagFmriGaussSigmaNorm;
   static const QString tagFmriGaussSigmaTang;
   static const QString tagFmriGaussNormBelow;
   static const QString tagFmriGaussNormAbove;
   static const QString tagFmriGaussTang;
   static const QString tagFmriBfMaxDist;
   static const QString tagFmriBfSplat;
};

#endif // __VE_PREFERENCES_FILE_H__

#ifdef __PREFERENCES_FILE_MAIN__

   const QString PreferencesFile::tagSurfaceBackgroundColor = "tag-surface-background-color";
   const QString PreferencesFile::tagSurfaceForegroundColor = "tag-surface-foreground-color";
   const QString PreferencesFile::tagMouseSpeed = "tag-mouse-speed";
   const QString PreferencesFile::tagLightPosition = "tag-light-position";
   const QString PreferencesFile::tagRecentSpecFiles = "tag-recent-spec-files";
   const QString PreferencesFile::tagRecentCopiedSpecFiles = "tag-recent-copied-spec-files";
   const QString PreferencesFile::tagRecentDataFileDirectories = "tag-recent-data-file-directories";
   const QString PreferencesFile::tagTestFlag1 = "tag-flag-1";
   const QString PreferencesFile::tagTestFlag2 = "tag-flag-2";
   const QString PreferencesFile::tagOpenGLDebugOn = "tag-opengl-debug-on";
   const QString PreferencesFile::tagDebugOn = "tag-debug-on";
   const QString PreferencesFile::tagDebugNodeNumber = "tag-debug-node";
   const QString PreferencesFile::tagIterativeUpdate = "tag-iterative-update";
   const QString PreferencesFile::tagWebBrowser = "tag-web-browser";
   const QString PreferencesFile::tagImageCaptureType = "tag-image-capture-type";
   const QString PreferencesFile::tagUserViewMatrix = "tag-user-view-matrix";
   const QString PreferencesFile::tagAnatomyVolumeBrightness = "tag-anatomy-volume-brightness";
   const QString PreferencesFile::tagAnatomyVolumeContrast = "tag-anatomy-volume-contrast";   
   const QString PreferencesFile::tagMaximumNumberOfThreads = "tag-maximum-number-of-threads";
   const QString PreferencesFile::tagNumberOfFileReadingThreads = "tag-number-of-file-reading-threads";
   const QString PreferencesFile::tagSpeechEnabled = "tag-speech-enabled";
   const QString PreferencesFile::tagFloatDigitsRightOfDecimal = "tag-float-digits-right-of-decimal";
   const QString PreferencesFile::tagPreferredWriteDataType = "tag-preferred-data-type";
   const QString PreferencesFile::tagSumsLoginData = "tag-sums-login-data";
   const QString PreferencesFile::tagSumsDatabaseHosts = "tag-sums-database-hosts";
   const QString PreferencesFile::tagSumsDataFileTimeout = "tag-sums-data-file-timeout";
   const QString PreferencesFile::tagSignificantDigits = "tag-significant-digits";
   const QString PreferencesFile::tagDisplayListsEnabled = "tag-display-lists-enabled";
   const QString PreferencesFile::tagRandomSeedOverride = "tag-random-seed";
   const QString PreferencesFile::tagPreferredVolumeWriteType = "tag-volume-write-type";
   const QString PreferencesFile::tagCaretTips = "tag-caret-tips";
   
   const QString PreferencesFile::tagFmriParameters         = "tag-fmri-parameters";
   const QString PreferencesFile::tagFmriAlgorithm         = "tag-fmri-algorithm";
   const QString PreferencesFile::tagFmriAvgVoxelNeighbors = "tag-fmri-av-neighbors";
   const QString PreferencesFile::tagFmriMaxVoxelNeighbors = "tag-fmri-mv-neighbors";
   const QString PreferencesFile::tagFmriStrongestVoxelNeighbors = "tag-fmri-sv-neighbors";
   const QString PreferencesFile::tagFmriGaussNeighbors    = "tag-fmri-gauss-algorithm-neighbors";
   const QString PreferencesFile::tagFmriGaussSigmaNorm    = "tag-fmri-gauss-algorithm-sigma-norm";
   const QString PreferencesFile::tagFmriGaussSigmaTang    = "tag-fmri-gauss-algorithm-sigma-tang";
   const QString PreferencesFile::tagFmriGaussNormBelow    = "tag-fmri-gauss-algorithm-norm-below";
   const QString PreferencesFile::tagFmriGaussNormAbove    = "tag-fmri-gauss-algorithm-norm-above";
   const QString PreferencesFile::tagFmriGaussTang         = "tag-fmri-gauss-algorithm-tang";
   const QString PreferencesFile::tagFmriBfMaxDist         = "tag-fmri-bf-algorithm-max-dist";
   const QString PreferencesFile::tagFmriBfSplat           = "tag-fmri-bf-algorithm-splat";

#endif // __PREFERENCES_FILE_MAIN__
