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

#ifndef __GUI_RECORDING_DIALOG_H__
#define __GUI_RECORDING_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class QCheckBox;
class QComboBox;
class QGroupBox;
class QImage;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;

class vtkAVIWriter;
class vtkImageData;
class vtkMPEG1Writer;
class vtkMPEG2Writer;

/// Dialog for recording to movie file.
class GuiRecordingDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiRecordingDialog(QWidget* parent);
      
      /// Destructor
      ~GuiRecordingDialog();
      
      /// get the recoding on flag
      bool getRecordingOn() const { return recordingOnFlag; }
      
      /// add an image to the recording
      void addImageToRecording(const QImage& imageIn);
   
   public slots:
      /// delete any temporary images that have been created
      void deleteTemporaryImages();
      
   private slots:
      /// called when manual recording capture push button pressed
      void slotManualCapturePushButton();
      
      /// called when auto/manual radio button is selected
      void slotRecordModeSelection();
      
      /// called when an image size selection is made
      void slotImageSizeComboBox(int item);
      
      /// called when create movie button is pressed
      void slotCreateMovieButton();
      
      /// called when help button is pressed
      void slotHelpPushButton();
      
   private:
      /// image size type
      enum IMAGE_SIZE_TYPE {
         IMAGE_SIZE_TYPE_CARET_WINDOW,
         IMAGE_SIZE_TYPE_320x240,
         IMAGE_SIZE_TYPE_352x240,
         IMAGE_SIZE_TYPE_640x480,
         IMAGE_SIZE_TYPE_720x480,
         IMAGE_SIZE_TYPE_CUSTOM,
      };
      
      /// Movie file type
      enum MOVIE_FILE_TYPE {
         /// MPEG with "mpeg_create"
         MOVIE_FILE_TYPE_MPEG_CREATE,
         /// MPEG1 using VTK
         MOVIE_FILE_TYPE_MPEG1_VTK,
         /// MPEG2 using VTK
         MOVIE_FILE_TYPE_MPEG2_VTK,
         /// AVI using VTK
         MOVIE_FILE_TYPE_AVI_VTK
      };
      
      /// get the movie file type
      MOVIE_FILE_TYPE getMovieFileType() const;
      
      /// create the recording section
      QWidget* createRecordingSection();

      /// create the image size section
      QWidget* createImageSizeSection();

      /// create the output movie section
      QWidget* createOutputMovieSection();

      /// create the mpeg create options section
      QWidget* createMpegCreateOptionsSection();
      
      /// create the movie using mpeg_create
      void createMovieWithMpegCreate(const QString& movieName);
      
      /// create the movie using VTK's MPEG2
      void createMovieWithMpeg2VTK();
      
      /// create the movie using VTK's MPEG1
      void createMovieWithMpeg1VTK();
      
      /// create the movie using VTK's AVI
      void createMovieWithAviVTK();
      
      /// add image to movie being created with mpeg_create
      void addImageToMpegCreate(const QImage& image);
      
      /// add image to movie being created with VTK's MPEG2
      void addImageToMpeg2VTK(const QImage& image);
      
      /// add image to movie being created with VTK's MPEG1
      void addImageToMpeg1VTK(const QImage& image);
      
      /// add image to movie being created with VTK's AVI
      void addImageToAviVTK(const QImage& image);
      
      /// convert a QImage into VTK's ImageData (user must delete returned vtkImageData object)
      vtkImageData* convertQImagetoVTKImageData(const QImage& image);
      
      /// update the dialog controls
      void updateControls();
      
      /// automatic recording radio button
      QRadioButton* recordAutomaticRadioButton;
      
      /// manual recording radio button
      QRadioButton* recordManualRadioButton;
      
      /// manual recording capture button
      QPushButton* manualCapturePushButton;
      
      /// image size combo box
      QComboBox* imageSizeComboBox;
      
      /// custom image size X spin box
      QSpinBox* customImageXSpinBox;
      
      /// custom image size Y spin box
      QSpinBox* customImageYSpinBox;
      
      /// custom size label
      QLabel* customSizeLabel;
      
      /// recording on flag
      bool recordingOnFlag;
      
      /// number of images label
      QLabel* numberOfImagesLabel;
      
      /// create movie pushbutton
      QPushButton* createMovieButton;
      
      /// the image size group box
      QGroupBox* imageSizeGroupBox;
      
      /// movie name line edit
      QLineEdit* movieNameLineEdit;
      
      /// names of images
      std::vector<QString> imageNames;
      
      /// compress temporary image check box
      QCheckBox* compressTemporaryImages;
      
      /// quality spin box
      QSpinBox* qualitySpinBox;
      
      /// reset push button
      QPushButton* resetPushButton;
      
      /// duplicate image spin box
      QSpinBox* duplicateSpinBox;
      
      /// counts images
      int imageCounter;
      
      /// movie name widget
      QWidget* movieNameWidget;
      
      /// movie file type widget
      QWidget* movieFileTypeWidget;
      
      /// movie file type combo box
      QComboBox* movieFileTypeComboBox;

      /// pointer for VTK's AVI writer
      vtkAVIWriter* vtkAviMovieWriter;
      
      /// pointer for caret's modified VTK's MPEG2 writer that writes MPEG1
      vtkMPEG1Writer* vtkMpeg1MovieWriter;
      
      /// pointer for VTK's MPEG2 writer
      vtkMPEG2Writer* vtkMpeg2MovieWriter;
};


#endif // __GUI_RECORDING_DIALOG_H__

