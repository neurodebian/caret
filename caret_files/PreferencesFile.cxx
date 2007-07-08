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

#include <QGlobalStatic>

#include <cstring>
#include <iostream>

#define __PREFERENCES_FILE_MAIN__
#include "PreferencesFile.h"
#undef __PREFERENCES_FILE_MAIN__

#include "DebugControl.h"
#include "GaussianComputation.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "SystemUtilities.h"

/**
 * The constructor.
 */
PreferencesFile::PreferencesFile()
   : AbstractFile("Preferences File", SpecFile::getPreferencesFileExtension())
{
   clear();
}

/**
 * Thre destructor.
 */
PreferencesFile::~PreferencesFile()
{
   clear();
}

/**
 * Clear this file's contents.
 */
void
PreferencesFile::clear()
{
   clearAbstractFile();
   
   surfaceBackgroundColor[0] = 0;
   surfaceBackgroundColor[1] = 0;
   surfaceBackgroundColor[2] = 0;
   
   surfaceForegroundColor[0] = 255;
   surfaceForegroundColor[1] = 255;
   surfaceForegroundColor[2] = 255;
   
   lightPosition[0] = 0.0;
   lightPosition[1] = 0.0;
   lightPosition[2] = 1000.0;
   
   mouseSpeed = 1.0;
   
   anatomyVolumeBrightness = 0;
   anatomyVolumeContrast   = 0;
   
   webBrowser = "";
   
   recentSpecFiles.clear();
   recentCopiedSpecFiles.clear();
   recentDataFileDirectories.clear();
   
   significantDigitsDisplay = 1;
   
   userViews.clear();
   
   sumsUserName = "";
   sumsHostName = "";
   sumsPassWord = "";
   sumsLoginWithUserNameAndPassWord = false;
   sumsUserInfoValid = false;
   
   sumsDatabaseHosts = "";
   sumsDataFileTimeOut = 600;
   
   //maximumNumberOfThreads = SystemUtilities::getNumberOfProcessors();
   maximumNumberOfThreads = 0;
   
#ifdef Q_OS_WIN32
   maximumNumberOfThreads = 1;
#endif
#ifdef Q_OS_MACX
   maximumNumberOfThreads = 1;
#endif
#ifdef Q_OS_LINUX
   maximumNumberOfThreads = 1;
#endif

   imageCaptureType = IMAGE_CAPTURE_OPENGL_BUFFER;
#ifdef Q_OS_WIN32
   imageCaptureType = IMAGE_CAPTURE_PIXMAP;
#endif // Q_OS_WIN32
#ifdef Q_OS_MACX
   imageCaptureType = IMAGE_CAPTURE_PIXMAP;
#endif // Q_OS_MACX

   speechType = SPEECH_TYPE_OFF;
   
   textFileDigitsRightOfDecimal = 6;
   
   preferredWriteDataType = AbstractFile::FILE_FORMAT_BINARY;
   
   //
   // Default display lists off since they blow away the X-server on Linux with 
   // Mesa OpenGL libraries.
   //
#ifdef Q_MAC_OSX
   displayListsEnabled = true;
#else
   displayListsEnabled = false;
#endif

   randomSeedOverrideFlag = false;
   randomSeedOverrideValue = 1;
   
   preferredVolumeWriteType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
   
   caretTipIndex = 0;
   caretTipsEnabled = false;
}

/**
 * get the surface background color
 */
void
PreferencesFile::getSurfaceBackgroundColor(unsigned char& r, unsigned char& g, unsigned char& b) const
{
   r = surfaceBackgroundColor[0];
   g = surfaceBackgroundColor[1];
   b = surfaceBackgroundColor[2];
}

/**
 * set the surface background color
 */
void 
PreferencesFile::setSurfaceBackgroundColor(const unsigned char r, const unsigned char g,
                                 const unsigned char b)
{
   surfaceBackgroundColor[0] = r;
   surfaceBackgroundColor[1] = g;
   surfaceBackgroundColor[2] = b;
   setModified();
}
                                 
/**
 * get the surface foreground color
 */
void 
PreferencesFile::getSurfaceForegroundColor(unsigned char& r, unsigned char& g, unsigned char& b) const
{
   r = surfaceForegroundColor[0];
   g = surfaceForegroundColor[1];
   b = surfaceForegroundColor[2];
}

/**
 * set the surface foreground color
 */
void 
PreferencesFile::setSurfaceForegroundColor(const unsigned char r, const unsigned char g,
                                 const unsigned char b)
{
   surfaceForegroundColor[0] = r;
   surfaceForegroundColor[1] = g;
   surfaceForegroundColor[2] = b;
   setModified();
}
         
/**
 * get the light position
 */
void 
PreferencesFile::getLightPosition(float& x, float& y, float& z) const
{
   x = lightPosition[0];
   y = lightPosition[1];
   z = lightPosition[2];
}

/**
 * set the light position
 */
void 
PreferencesFile::setLightPosition(const float x, const float y, const float z)
{
   lightPosition[0] = x;
   lightPosition[1] = y;
   lightPosition[2] = z;
   setModified();
}

/**
 * get the mouse speed
 */
float 
PreferencesFile::getMouseSpeed() const
{
   return mouseSpeed;
}

/**
 * set the mouse speed
 */
void 
PreferencesFile::setMouseSpeed(const float speed)
{
   mouseSpeed = speed;
   setModified();
}

/**
 * add to recent spec files.
 */
void 
PreferencesFile::addToRecentSpecFiles(const QString& specFileName,
                                      const bool writePreferencesFile)
{
   int foundIndex = -1;
   for (unsigned int i = 0;  i < recentSpecFiles.size(); i++) {
      if (recentSpecFiles[i] == specFileName) {
         if (i == 0) {
            // name is last file - do not need to do anything
            return;
         }
         foundIndex = i;
         break;
      }
   }
   
   //
   // Make file just opened the first file..
   //
   std::vector<QString> filesOut;
   filesOut.push_back(specFileName);

   for (int j = 0; j < static_cast<int>(recentSpecFiles.size()); j++) {
      if (j != foundIndex) {
         filesOut.push_back(recentSpecFiles[j]);
      }
      if (filesOut.size() >= 20) {
         break;
      }
   }
   recentSpecFiles = filesOut;
   
   if (writePreferencesFile) {
      if (getFileName().isEmpty() == false) {
         try {
            writeFile(getFileName());
         }
         catch(FileException& /*e*/) {
         }
      }
   }
}
      
/**
 * get the recent spec files
 */
void 
PreferencesFile::getRecentSpecFiles(std::vector<QString>& files) const
{
  files = recentSpecFiles;
}

/**
 * set the recent spec files
 */
void 
PreferencesFile::setRecentSpecFiles(const std::vector<QString>& files)
{
   recentSpecFiles = files;
   setModified();
}

/**
 * add to recent data file directories.
 */
void 
PreferencesFile::addToRecentDataFileDirectories(const QString& dirName,
                                                const bool writePreferencesFile)
{
   int foundIndex = -1;
   for (unsigned int i = 0;  i < recentDataFileDirectories.size(); i++) {
      if (recentDataFileDirectories[i] == dirName) {
         if (i == 0) {
            // name is last file - do not need to do anything
            return;
         }
         foundIndex = i;
         break;
      }
   }
   
   //
   // Make file just opened the first file..
   //
   std::vector<QString> filesOut;
   filesOut.push_back(dirName);

   for (int j = 0; j < static_cast<int>(recentDataFileDirectories.size()); j++) {
      if (j != foundIndex) {
         filesOut.push_back(recentDataFileDirectories[j]);
      }
      if (filesOut.size() >= 20) {
         break;
      }
   }
   recentDataFileDirectories = filesOut;
   
   if (writePreferencesFile) {
      if (getFileName().isEmpty() == false) {
         try {
            writeFile(getFileName());
         }
         catch(FileException& /*e*/) {
         }
      }
   }
}
                                    
/**
 * get the recent data file directories.
 */
void 
PreferencesFile::getRecentDataFileDirectories(std::vector<QString>& dirs) const
{
   dirs = recentDataFileDirectories;
}      

/**
 * add to recent copied spec files.
 */
void 
PreferencesFile::addToRecentCopiedSpecFiles(const QString& specFileName,
                                            const bool writePreferencesFile)
{
   int foundIndex = -1;
   for (unsigned int i = 0;  i < recentCopiedSpecFiles.size(); i++) {
      if (recentCopiedSpecFiles[i] == specFileName) {
         if (i == 0) {
            // name is last file - do not need to do anything
            return;
         }
         foundIndex = i;
         break;
      }
   }
   
   //
   // Make file just opened the first file..
   //
   std::vector<QString> filesOut;
   filesOut.push_back(specFileName);

   for (int j = 0; j < static_cast<int>(recentCopiedSpecFiles.size()); j++) {
      if (j != foundIndex) {
         filesOut.push_back(recentCopiedSpecFiles[j]);
      }
      if (filesOut.size() >= 20) {
         break;
      }
   }
   recentCopiedSpecFiles = filesOut;
   
   if (writePreferencesFile) {
      if (getFileName().isEmpty() == false) {
         try {
            writeFile(getFileName());
         }
         catch(FileException& /*e*/) {
         }
      }
   }
}
      
/**
 * get the recent copied spec files
 */
void 
PreferencesFile::getRecentCopiedSpecFiles(std::vector<QString>& files) const
{
  files = recentCopiedSpecFiles;
}

/**
 * set the recent copied spec files
 */
void 
PreferencesFile::setRecentCopiedSpecFiles(const std::vector<QString>& files)
{
   recentCopiedSpecFiles = files;
   setModified();
}

/**
 * get the debug default value
 */
bool 
PreferencesFile::getDebugOn() const
{
   return DebugControl::getDebugOn();
}

/**
 * set the debug default value
 */
void 
PreferencesFile::setDebugOn(const bool val)
{
   DebugControl::setDebugOn(val);
   setModified();
}

/**
 * get the debug node number
 */
int 
PreferencesFile::getDebugNodeNumber() const
{
   return DebugControl::getDebugNodeNumber();
}

/**
 * set the debug node number
 */
void 
PreferencesFile::setDebugNodeNumber(const int num)
{
   DebugControl::setDebugNodeNumber(num);
   setModified();
}

/**
 * get the iterative update
 */
int 
PreferencesFile::getIterativeUpdate() const
{
  return DebugControl::getIterativeUpdate();
}

/**
 * set the iterative update
 */
void 
PreferencesFile::setIterativeUpdate(const int iter)
{
   DebugControl::setIterativeUpdate(iter);
   setModified();
}

/**
 * get a user view.
 */
const PreferencesFile::UserView*
PreferencesFile::getUserView(const int viewNumber) const
{
   if ((viewNumber >= 0) && (viewNumber < getNumberOfUserViews())) {
      return &userViews[viewNumber];
   }
   return NULL;
}
                  
/**
 * add a user view.
 */
void 
PreferencesFile::addUserView(const PreferencesFile::UserView& uv) 
{
   userViews.push_back(uv);
   setModified();
}

/**
 * set the web browser.
 */
void 
PreferencesFile::setWebBrowser(const QString wb) 
{ 
   webBrowser = wb; 
   setModified(); 
}
      
/**
 * set the image capture type.
 */
void 
PreferencesFile::setImageCaptureType(const IMAGE_CAPTURE_TYPE ict) 
{
   imageCaptureType = ict; 
   setModified(); 
}

/**
 * set the anatomy volume contrast.
 */
void 
PreferencesFile::setAnatomyVolumeContrast(const int c) 
{ 
   anatomyVolumeContrast = c; 
   setModified(); 
}

/**
 * set the anatomy volume brightness.
 */
void 
PreferencesFile::setAnatomyVolumeBrightness(const int b) 
{ 
   anatomyVolumeBrightness = b; 
   setModified(); 
}

/**
 * get the maximum number of threads.
 */
int 
PreferencesFile::getMaximumNumberOfThreads() const 
{
   if (DebugControl::getDebugOn()) { 
      return maximumNumberOfThreads;
   }
   return 1;
}

/**
 * get SuMS login information.
 */
void 
PreferencesFile::getSumsLoginInformation(QString& userName,
                                         QString& passWord,
                                         QString& hostName,
                                         bool& useLogin,
                                         bool& infoValid) const
{
   userName  = sumsUserName;
   passWord  = sumsPassWord;
   hostName  = sumsHostName;
   useLogin  = sumsLoginWithUserNameAndPassWord;
   infoValid = sumsUserInfoValid;
}
                             
/**
 * set SuMS login information.
 */
void 
PreferencesFile::setSumsLoginInformation(const QString& userName,
                                         const QString& passWord,
                                         const QString& hostName,
                                         const bool useLogin,
                                         const bool infoValid)
{
   sumsUserName = userName;
   sumsPassWord = passWord;
   sumsHostName = hostName;
   sumsLoginWithUserNameAndPassWord = useLogin;
   sumsUserInfoValid = infoValid;
}
                                   
/**
 * set caret tips.
 */
void 
PreferencesFile::setCaretTips(const int tipIndex, const bool showTips)
{
   caretTipIndex = tipIndex;
   caretTipsEnabled = showTips;
}

/**
 * get caret tips.
 */
void 
PreferencesFile::getCaretTips(int& tipIndex, bool& showTips) const
{
   tipIndex = caretTipIndex;
   showTips = caretTipsEnabled;
}
      
/**
 * Read the preferences file's data.
 */
void
PreferencesFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Clear out version_id's since this file is frequentyly written and does not need
   // a version_id.
   //
   setHeaderTag(AbstractFile::headerTagVersionID, "");

   QString tag, value;
   
   QString fileNameSeparatorCharacter(" ");
   
   while(stream.atEnd() == false) {
      readTagLine(stream, tag, value);
      if (tag.isEmpty() == false) {
         if (tag == tagSurfaceBackgroundColor) {
            std::vector<QString> tokens;
            StringUtilities::token(value, " ", tokens);
            if (tokens.size() >= 3) {
               surfaceBackgroundColor[0] = tokens[0].toInt();
               surfaceBackgroundColor[1] = tokens[1].toInt();
               surfaceBackgroundColor[2] = tokens[2].toInt();
            }
         }
         else if (tag == tagFileVersion) {
            if (value.isEmpty() == false) {
               const int versionNumber = StringUtilities::toInt(value);
               switch (versionNumber) {
                  case 1:
                     fileNameSeparatorCharacter = "*";
                     break;
                  default:
                     fileNameSeparatorCharacter = " ";
                     break;
               }
            }
         }
         else if (tag == tagSurfaceForegroundColor) {
            std::vector<QString> tokens;
            StringUtilities::token(value, " ", tokens);
            if (tokens.size() >= 3) {
               surfaceForegroundColor[0] = tokens[0].toInt();
               surfaceForegroundColor[1] = tokens[1].toInt();
               surfaceForegroundColor[2] = tokens[2].toInt();
            }
         }
         else if (tag == tagMouseSpeed) {
            mouseSpeed = value.toFloat();
         }
         else if (tag == tagLightPosition) {
            std::vector<QString> tokens;
            StringUtilities::token(value, " ", tokens);
            if (tokens.size() >= 3) {
               lightPosition[0] = tokens[0].toFloat();
               lightPosition[1] = tokens[1].toFloat();
               lightPosition[2] = tokens[2].toFloat();
            }
         }
         else if ((tag == tagRecentSpecFiles) ||
                  (tag == "recent-spec-files")) {
            StringUtilities::token(value, fileNameSeparatorCharacter, recentSpecFiles);
         }
         else if (tag == tagRecentCopiedSpecFiles) {
            StringUtilities::token(value, fileNameSeparatorCharacter, recentCopiedSpecFiles);
         }
         else if (tag == tagRecentDataFileDirectories) {
            StringUtilities::token(value, fileNameSeparatorCharacter, recentDataFileDirectories);
         }
         else if (tag == tagDebugOn) {
            if (value == "true") {
               setDebugOn(true);
            }
            else if (value == "false") {
               setDebugOn(false);
            }
         }
         else if (tag == tagDebugNodeNumber) {
            setDebugNodeNumber(value.toInt());
         }
         else if (tag == tagDisplayListsEnabled) {
            setDisplayListsEnabled(value == "true");
         }
         else if (tag == tagIterativeUpdate) {
            setIterativeUpdate(value.toInt());
         }
         else if (tag == tagWebBrowser) {
            webBrowser = value;
            if (webBrowser == tagWebBrowser) {
               webBrowser = "";
            }
         }
         else if (tag == tagImageCaptureType) {
            imageCaptureType = static_cast<IMAGE_CAPTURE_TYPE>(QString(value).toInt());
         }
         else if (tag == tagUserViewMatrix) {
            std::vector<QString> tokens;
            StringUtilities::token(value, " ", tokens);
            if (tokens.size() == 17) {
               QString name = StringUtilities::replace(tokens[0], '~', ' ');
               float matrix[16];
               for (int i = 0; i < 16; i++) {
                  matrix[i] = QString(tokens[i+1]).toFloat();
               }
               const float t[3] = { 0.0, 0.0, 0.0 };
               const float s[3] = { 1.0, 1.0, 1.0 };
               UserView uv(name, matrix, t, s, true, false, false);
               addUserView(uv);
            }
            else if (tokens.size() >= 25) {
               QString name = StringUtilities::replace(tokens[0], '~', ' ');
               float matrix[16];
               for (int i = 0; i < 16; i++) {
                  matrix[i] = QString(tokens[i+1]).toFloat();
               }
               float trans[3];
               for (int i = 0; i < 3; i++) {
                  trans[i] = QString(tokens[i+17]).toFloat();
               }
               float scale[3];
               scale[0] = QString(tokens[20]).toFloat();
               scale[1] = QString(tokens[21]).toFloat();
               scale[2] = QString(tokens[22]).toFloat();
               const bool matrixValid = (tokens[23][0] == 'T');
               const bool transValid  = (tokens[24][0] == 'T');
               const bool scaleValid  = (tokens[25][0] == 'T');
               UserView uv(name, matrix, trans, scale, matrixValid, transValid, scaleValid);
               addUserView(uv);
            }
            else if (tokens.size() >= 23) {
               QString name = StringUtilities::replace(tokens[0], '~', ' ');
               float matrix[16];
               for (int i = 0; i < 16; i++) {
                  matrix[i] = QString(tokens[i+1]).toFloat();
               }
               float trans[3];
               for (int i = 0; i < 3; i++) {
                  trans[i] = QString(tokens[i+17]).toFloat();
               }
               const float s = QString(tokens[20]).toFloat();
               const float scale[3] = { s, s, s };
               const bool matrixValid = (tokens[21][0] == 'T');
               const bool transValid  = (tokens[22][0] == 'T');
               const bool scaleValid  = (tokens[23][0] == 'T');
               UserView uv(name, matrix, trans, scale, matrixValid, transValid, scaleValid);
               addUserView(uv);
            }
         }
         else if (tag == tagAnatomyVolumeBrightness) {
            setAnatomyVolumeBrightness(value.toInt());
         }
         else if (tag == tagAnatomyVolumeContrast) {
            setAnatomyVolumeContrast(value.toInt());
         }
         else if (tag == tagMaximumNumberOfThreads) {
            setMaximumNumberOfThreads(StringUtilities::toInt(value));
            setMaximumNumberOfThreads(1);
         }
         else if (tag == tagSpeechEnabled) {
            if (value == "true") {
               speechType = SPEECH_TYPE_NORMAL;
            }
            else if (value == "false") {
               speechType = SPEECH_TYPE_OFF;
            }
            else {
               speechType = static_cast<SPEECH_TYPE>(StringUtilities::toInt(value));
            }
         }
         else if (tag == tagSumsLoginData) {
            std::vector<QString> tokens;
            StringUtilities::token(value, " ", tokens);
            if (tokens.size() >= 1) {
               if (tokens[0] == "true") {
                  sumsUserInfoValid = true;
               }
               else {
                  sumsUserInfoValid = false;
               }
            }
            if (tokens.size() >= 2) {
               if (tokens[1] == "true") {
                  sumsLoginWithUserNameAndPassWord = true;
               }
               else {
                  sumsLoginWithUserNameAndPassWord = false;
               }
            }
            if (tokens.size() >= 3) {
               sumsHostName = tokens[2];
            }
            if (tokens.size() >= 4) {
               sumsUserName = tokens[3];
            }
            if (tokens.size() >= 5) {
               sumsPassWord = tokens[4];
            }
         }
         else if (tag == tagSumsDatabaseHosts) {
            sumsDatabaseHosts = value;
         }
         else if (tag == tagSumsDataFileTimeout) {
            sumsDataFileTimeOut = value.toInt();
         }
         else if (tag == tagFloatDigitsRightOfDecimal) {
            setTextFileDigitsRightOfDecimal(StringUtilities::toInt(value));
         }
         else if (tag == tagSignificantDigits) {
            setSignificantDigitsDisplay(StringUtilities::toInt(value));
         }
         else if (tag == tagRandomSeedOverride) {
            std::vector<QString> tokens;
            StringUtilities::token(value, " ", tokens);
            if (tokens.size() >= 2) {
               randomSeedOverrideFlag = (tokens[0] == "true");
               randomSeedOverrideValue = QString(tokens[1]).toUInt();
            }
         }
         else if (tag == tagCaretTips) {
             std::vector<QString> tokens;
             StringUtilities::token(value, " ", tokens);
             if (tokens.size() >= 2) {
                caretTipsEnabled = (tokens[0] == "true");
                caretTipIndex    = tokens[1].toInt();
             }
         }
         else if (tag == tagPreferredVolumeWriteType) {
            preferredVolumeWriteType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
            if (value == "AFNI") {
               preferredVolumeWriteType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
            }
            else if (value == "ANALYZE") {
               preferredVolumeWriteType = VolumeFile::FILE_READ_WRITE_TYPE_ANALYZE;
            }
            else if (value == "NIFTI") {
               preferredVolumeWriteType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
            }
            else if (value == "SPM") {
               preferredVolumeWriteType = VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX;
            }
            else if (value == "WUNIL") {
               preferredVolumeWriteType = VolumeFile::FILE_READ_WRITE_TYPE_WUNIL;
            }
         }
   
         else if (tag == tagFmriAlgorithm) {
            fmriParameters.algorithmName = value;
         }
         else if (tag == tagFmriAvgVoxelNeighbors) {
            fmriParameters.averageVoxelNeighbors = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriMaxVoxelNeighbors) {
            fmriParameters.maximumVoxelNeighbors = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriGaussNeighbors) {
            fmriParameters.gaussianNeighbors = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriGaussSigmaNorm) {
            fmriParameters.gaussianSigmaNorm = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriGaussSigmaTang) {
            fmriParameters.gaussianSigmaTang = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriGaussNormBelow) {
            fmriParameters.gaussianNormBelow = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriGaussNormAbove) {
            fmriParameters.gaussianNormAbove = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriGaussTang) {
            fmriParameters.gaussianTang = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriBfMaxDist) {
            fmriParameters.brainFishMaxDistance = StringUtilities::toFloat(value);
         }
         else if (tag == tagFmriBfSplat) {
            fmriParameters.brainFishSplatFactor = StringUtilities::toInt(value);
         }
         else if (tag == tagPreferredWriteDataType) {
            if (value == AbstractFile::headerTagEncodingValueAscii) {
               preferredWriteDataType = AbstractFile::FILE_FORMAT_ASCII;
            }
            else if (value == AbstractFile::headerTagEncodingValueBinary) {
               preferredWriteDataType = AbstractFile::FILE_FORMAT_BINARY;
            }
            else if (value == AbstractFile::headerTagEncodingValueXML) {
               preferredWriteDataType = AbstractFile::FILE_FORMAT_XML;
            }
            else if (value == AbstractFile::headerTagEncodingValueOther) {
               preferredWriteDataType = AbstractFile::FILE_FORMAT_OTHER;
            }
            else if (value == AbstractFile::headerTagEncodingValueCommaSeparatedValueFile) {
               preferredWriteDataType = AbstractFile::FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE;
            }
            else {
               std::cerr << "Unrecognized preferences data type " << value.toAscii().constData() << std::endl;
            }
         }
         else {
            if (DebugControl::getDebugOn()) {
               std::cerr << "Unrecognized preferences file tag " << tag.toAscii().constData() << std::endl;
            }
         }
      }
   }
}

/**
 * Write the preferences file's data.
 */
void
PreferencesFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Clear out version_id's since this file is frequentyly written and does not need
   // a version_id.
   //
   setHeaderTag(AbstractFile::headerTagVersionID, "");
   
   const QString fileNameSeparatorCharacter("*");
   
   stream << tagFileVersion << " 1" << "\n";
   
   stream << tagSurfaceBackgroundColor << " " 
          << surfaceBackgroundColor[0] << " "
          << surfaceBackgroundColor[1] << " "
          << surfaceBackgroundColor[2] << "\n";
   stream << tagSurfaceForegroundColor << " "
          << surfaceForegroundColor[0] << " " 
          << surfaceForegroundColor[1] << " " 
          << surfaceForegroundColor[2] << "\n";
   stream << tagMouseSpeed << " " << mouseSpeed << "\n";
   stream << tagLightPosition << " " 
          << lightPosition[0] << " "
          << lightPosition[1] << " "
          << lightPosition[2] << "\n";
   stream << tagRecentSpecFiles << " " 
          << StringUtilities::combine(recentSpecFiles, fileNameSeparatorCharacter) << "\n";
   stream << tagWebBrowser << " "
          << webBrowser << "\n";
   stream << tagRecentCopiedSpecFiles << " " 
          << StringUtilities::combine(recentCopiedSpecFiles, fileNameSeparatorCharacter) << "\n";
   stream << tagRecentDataFileDirectories << " "
          << StringUtilities::combine(recentDataFileDirectories, fileNameSeparatorCharacter) << "\n";
                 
   stream << tagDebugOn << " ";
   if (getDebugOn()) stream << "true" << "\n";
   else              stream << "false" << "\n";
   
   stream << tagDebugNodeNumber << " " 
          << getDebugNodeNumber() << "\n";
         
   if (getDisplayListsEnabled()) {
      stream << tagDisplayListsEnabled << " true" << "\n";
   }
   else {
      stream << tagDisplayListsEnabled << " false" << "\n";
   }

   stream << tagIterativeUpdate << " "
          << getIterativeUpdate() << "\n";
          
   stream << tagImageCaptureType << " "
          << imageCaptureType << "\n";
     
   stream << tagAnatomyVolumeBrightness << " "
          << anatomyVolumeBrightness << "\n";
   stream << tagAnatomyVolumeContrast << " "
          << anatomyVolumeContrast << "\n";
          
   stream << "\n";

   stream << tagMaximumNumberOfThreads << " " 
          << maximumNumberOfThreads << "\n";   
   stream << "\n";
   
   stream << tagSpeechEnabled << " "  << speechType << "\n";   
   stream << "\n";

   stream << tagFloatDigitsRightOfDecimal << " " << textFileDigitsRightOfDecimal << "\n";
   stream << "\n";
  
   QString writeStr(AbstractFile::headerTagEncodingValueOther); 
   switch(preferredWriteDataType) {
      case FILE_FORMAT_ASCII:
         writeStr = AbstractFile::headerTagEncodingValueAscii;
         break;
      case FILE_FORMAT_BINARY:
         writeStr = AbstractFile::headerTagEncodingValueBinary;
         break;
      case FILE_FORMAT_XML:
         writeStr = AbstractFile::headerTagEncodingValueXML;
         break;
      case FILE_FORMAT_XML_BASE64:
         writeStr = AbstractFile::headerTagEncodingValueXMLBase64;
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         writeStr = AbstractFile::headerTagEncodingValueXMLGZipBase64;
         break;
      case FILE_FORMAT_OTHER:
         writeStr = AbstractFile::headerTagEncodingValueOther;
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         writeStr = AbstractFile::headerTagEncodingValueCommaSeparatedValueFile;
         break;
   }
   stream << tagPreferredWriteDataType << " " << writeStr << "\n";
   stream << "\n";
   
   stream << tagSumsLoginData << " ";
   if (sumsUserInfoValid) {
      stream << "true ";
   }
   else {
      stream << "false ";
   }
   if (sumsLoginWithUserNameAndPassWord) {
      stream << "true ";
   }
   else {
      stream << "false ";
   }
   stream << sumsHostName << " ";
   stream << sumsUserName << " ";
   stream << sumsPassWord << " ";
   stream << "\n";
   stream << "\n";
   
   stream << tagRandomSeedOverride << " ";
   if (randomSeedOverrideFlag) {
      stream << "true";
   }
   else {
      stream << "false";
   }
   stream << " "
          << randomSeedOverrideValue
          << "\n";
          
   stream << tagSignificantDigits << " " << significantDigitsDisplay << "\n";
   stream << tagSumsDatabaseHosts << " " << sumsDatabaseHosts << "\n";
   stream << tagSumsDataFileTimeout << " " << sumsDataFileTimeOut << "\n";
   stream << "\n";
   
   stream << tagCaretTips << " " << (caretTipsEnabled ? "true" : "false") 
          << " " << caretTipIndex << "\n";
   stream << "\n";
   
   QString volType;
   switch (preferredVolumeWriteType) {
      case VolumeFile::FILE_READ_WRITE_TYPE_RAW:
         volType = "IGNORE";
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_AFNI:
         volType = "AFNI";
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_ANALYZE:
         volType = "ANALYZE";
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_NIFTI:
         volType = "NIFTI";
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX:
         volType = "SPM";
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_WUNIL:
         volType = "WUNIL";
         break;
      case VolumeFile::FILE_READ_WRITE_TYPE_UNKNOWN:
         volType = "IGNORE";
         break;
   }
   if (volType != "IGNORE") {
      stream << tagPreferredVolumeWriteType << " " << volType<< "\n";
      stream << "\n";
   }
   
   stream <<  tagFmriAlgorithm << " "
          << fmriParameters.algorithmName << "\n";
          
   stream <<  tagFmriAvgVoxelNeighbors << " "
          << fmriParameters.averageVoxelNeighbors << "\n";
          
   stream <<  tagFmriMaxVoxelNeighbors << " "
          << fmriParameters.maximumVoxelNeighbors << "\n";
          
   stream <<  tagFmriGaussNeighbors << " "
          << fmriParameters.gaussianNeighbors << "\n";
          
   stream <<  tagFmriGaussSigmaTang << " "
          << fmriParameters.gaussianSigmaTang << "\n";
          
   stream <<  tagFmriGaussSigmaNorm << " "
          << fmriParameters.gaussianSigmaNorm << "\n";
          
   stream <<  tagFmriGaussNormBelow << " "
          << fmriParameters.gaussianNormBelow << "\n";
          
   stream <<  tagFmriGaussNormAbove << " "
          << fmriParameters.gaussianNormAbove << "\n";
          
   stream <<  tagFmriGaussTang << " "
          << fmriParameters.gaussianTang << "\n";
          
   stream <<  tagFmriBfMaxDist << " "
          << fmriParameters.brainFishMaxDistance << "\n";
          
   stream <<  tagFmriBfSplat << " "
          << fmriParameters.brainFishSplatFactor << "\n";
          
   stream << "\n";
   for (int i = 0; i < getNumberOfUserViews(); i++) {
      QString name;
      float matrix[16];
      float trans[3];
      float scale[3];
      bool matrixValid;
      bool transValid;
      bool scaleValid;
      const UserView* uv = getUserView(i);
      uv->getViewInfo(name, matrix, trans, scale, matrixValid, transValid, scaleValid);
      stream << tagUserViewMatrix << " "
             << StringUtilities::replace(name, ' ', '~');
      for (int j = 0; j < 16; j++) {
         stream << " " << matrix[j];
      }
      for (int j = 0; j < 3; j++) {
         stream << " " << trans[j];
      }
      stream << " " << scale[0] << " " << scale[1] << " " << scale[2];
      stream << " " << (matrixValid ? 'T' : 'F');
      stream << " " << (transValid ? 'T' : 'F');
      stream << " " << (scaleValid ? 'T' : 'F');
      stream << "\n";
   }
}

//==============================================================================

/**
 * constructor.
 */
PreferencesFile::UserView::UserView(const QString& nameIn,
                                    const float rotationIn[16], 
                                    const float translationIn[3],
                                    const float scalingIn[3],
                                    const bool  rotationValidIn,
                                    const bool  translationValidIn,
                                    const bool  scalingValidIn)
{
   name = nameIn;
   for (int i = 0; i < 16; i++) {
      rotation[i] = rotationIn[i];
   }
   for (int i = 0; i < 3; i++) {
      translation[i] = translationIn[i];
      scaling[i] = scalingIn[i];
   }
   rotationValid = rotationValidIn;
   translationValid = translationValidIn;
   scalingValid = scalingValidIn;
}

         
/**
 * get user view information.
 */
void 
PreferencesFile::UserView::getViewInfo(QString& nameOut,
                                       float  rotationOut[16], 
                                       float  translationOut[3],
                                       float  scalingOut[3],
                                       bool&  rotationValidOut,
                                       bool&  translationValidOut,
                                       bool&  scalingValidOut) const
{
   nameOut = name;
   for (int i = 0; i < 16; i++) {
      rotationOut[i] = rotation[i];
   }
   for (int i = 0; i < 3; i++) {
      translationOut[i] = translation[i];
      scalingOut[i] = scaling[i];
   }
   rotationValidOut = rotationValid;
   translationValidOut = translationValid;
   scalingValidOut = scalingValid;
}

//------------------------------------------------------------------------------

/**
 * constructor.
 */
PreferencesFile::FmriAlgorithm::FmriAlgorithm()
{
   resetParametersToDefault();
}                            


/**
 * destructor.
 */
PreferencesFile::FmriAlgorithm::~FmriAlgorithm()
{
}                            


/**
 * reset parameters to their default values.
 */
void 
PreferencesFile::FmriAlgorithm::resetParametersToDefault()
{
   algorithmName = "";
   averageVoxelNeighbors = 0.0;
   maximumVoxelNeighbors = 6.0;
   gaussianNeighbors = 6.0;
   gaussianSigmaTang = 1.0;
   gaussianSigmaNorm = 2.0;
   gaussianNormBelow = 2.0;
   gaussianNormAbove = 2.0;
   gaussianTang      = 3.0;
   brainFishMaxDistance = 3.0;
   brainFishSplatFactor = 3;
   
   GaussianComputation::getDefaultParameters(gaussianNormBelow,
                                             gaussianNormAbove,
                                             gaussianSigmaNorm,
                                             gaussianSigmaTang,
                                             gaussianTang);
}                            


/**
 * get algorithm name.
 */
QString 
PreferencesFile::FmriAlgorithm::getAlgorithmName() const 
{ 
   return algorithmName; 
}


/**
 * set algorithm name.
 */
void 
PreferencesFile::FmriAlgorithm::setAlgorithmName(const QString& name) 
{
   algorithmName = name; 
}


/**
 * get averge voxel parameters.
 */
void 
PreferencesFile::FmriAlgorithm::getAverageVoxelParameters(float& neighborsOut) const
{
   neighborsOut = averageVoxelNeighbors;
}                            


/**
 * set averge voxel parameters.
 */
void 
PreferencesFile::FmriAlgorithm::setAverageVoxelParameters(const float neighborsIn)
{
   averageVoxelNeighbors = neighborsIn;
}                            


/**
 * get maximum voxel parameters.
 */
void 
PreferencesFile::FmriAlgorithm::getMaximumVoxelParameters(float& neighborsOut) const
{
   neighborsOut = maximumVoxelNeighbors;
}                            


/**
 * set maximum voxel parameters.
 */
void 
PreferencesFile::FmriAlgorithm::setMaximumVoxelParameters(const float neighborsIn)
{
   maximumVoxelNeighbors = neighborsIn;
}                            


/**
 * get gaussian algorithm parameters.
 */
void 
PreferencesFile::FmriAlgorithm::getGaussianParameters(float& neighborsOut,
                          float& sigmaTangOut,
                          float& sigmaNormOut,
                          float& normBelowOut,
                          float& normAboveOut,
                          float& tangOut) const
{
   neighborsOut = gaussianNeighbors;
   sigmaTangOut = gaussianSigmaTang;
   sigmaNormOut = gaussianSigmaNorm;
   normBelowOut = gaussianNormBelow;
   normAboveOut = gaussianNormAbove;
   tangOut      = gaussianTang;
}                            

                              
/**
 * set gaussian algorithm parameters.
 */
void 
PreferencesFile::FmriAlgorithm::setGaussianParameters(const float neighborsIn,
                          const float sigmaTangIn,
                          const float sigmaNormIn,
                          const float normBelowIn,
                          const float normAboveIn,
                          const float tangIn)
{
   gaussianNeighbors = neighborsIn;
   gaussianSigmaTang = sigmaTangIn;
   gaussianSigmaNorm = sigmaNormIn;
   gaussianNormBelow = normBelowIn;
   gaussianNormAbove = normAboveIn;
   gaussianTang      = tangIn;
}                            

                              
/** 
 * get brain fish algorithm parameters.
 */
void 
PreferencesFile::FmriAlgorithm::getBrainFishParameters(float& maxDistanceOut,
                            int& splatFactorOut) const
{
   maxDistanceOut = brainFishMaxDistance;
   splatFactorOut = brainFishSplatFactor;
}                            


/**
 * set brain fish algorithm parameters.
 */
void 
PreferencesFile::FmriAlgorithm::setBrainFishParameters(const float maxDistanceIn,
                            const int splatFactorIn)
{
   brainFishMaxDistance = maxDistanceIn;
   brainFishSplatFactor = splatFactorIn;
}                            

