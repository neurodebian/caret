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

#ifndef __BRAIN_MODEL_CONTOURS_H__
#define __BRAIN_MODEL_CONTOURS_H__

#include "BrainModel.h"
#include "ContourFile.h"

class MDPlotFile;
class NeurolucidaFile;

/// Class for handling contours
class BrainModelContours : public BrainModel {
   public:   
      /// Constructor
      BrainModelContours(BrainSet* bs);
      
      /// Destructor
      ~BrainModelContours();
      
      /// get access to the contours
      ContourFile* getContourFile() { return &contours; }
      
      /// get access to the contours
      const ContourFile* getContourFile() const { return &contours; }
      
      /// Get a descriptive name of the model
      QString getDescriptiveName() const;
      
      /// read the contour file
      void readContourFile(const QString& filename,
                           const bool append) throw(FileException); 
   
      /// import the MD Plot file
      void importMDPlotFile(const MDPlotFile& mf,
                           const bool append) throw(FileException); 
   
      /// import the NeurolucidaFile file
      void importNeurolucidaFile(const NeurolucidaFile& nf,
                                 const bool append) throw (FileException);
                                 
      /// reset this contour file
      void reset();
      
      /// apply alignment transforms to a section
      void applyAlignmentToSection(const int sectionNumber);
      
      /// apply transformations to all section
      void applyTransformationsToAllContours();
      
      /// get the alignment rotation matrix
      void getAlignmentRotationMatrix(float matrix[16]) const;
      
      /// set the alignment rotation matrix
      void setAlignmentRotationMatrix(const float matrix[16]);
      
      /// get the alignment rotation matrix
      vtkTransform* getAlignmentRotationTransformMatrix() { 
         return alignmentRotationMatrix; 
      }
      
      /// get the model's alignment scaling
      float getAlignmentScaling() const { 
         return alignmentScaling; 
      }
      
      /// set the model's alignment scaling
      void setAlignmentScaling(const float scaleIn) { 
         alignmentScaling = scaleIn; 
      }
      
      /// get the model's alignment translation
      void getAlignmentTranslation(float translationOut[3]) const;
      
      /// set the model's alignment translation
      void setAlignmentTranslation(const float translationIn[3]);
      
      /// reset the alignment transformations
      void resetAlignmentTransformations();
      
      /// reset the viewing transform
      void resetViewingTransform(const int viewNumber);

      /// Reset the alignment region box.
      void resetAlignmentRegionBox();
      
      /// Set alignment region box start.
      void setAlignmentRegionBoxEnd(const float xy[2]);
      
      /// Set alignment region box start.
      void setAlignmentRegionBoxStart(const float xy[2]);

      /// Get the alignment region box
      void getAlignmentRegionBox(float bounds[4]);

      /// Get alignment region box valid
      bool getAlignmentRegionBoxValid() const { return alignmentRegionBoxValid; }
      
      /// Set alignment region box valid
      void setAlignmentRegionBoxValid(const int valid) { alignmentRegionBoxValid = valid; }
      
      /// Get alignment region flag
      bool getAligningRegionFlag() const { return aligningRegionFlag; }
      
      /// Set alignment region flag
      void setAligningRegionFlag(const int flag) { aligningRegionFlag = flag; }
      
      /// get the alignment matrix
      TransformationMatrix* getAlignmentMatrix() { return &alignmentMatrix; }
      
      /// get the alignment matrix
      const TransformationMatrix* getAlignmentMatrix() const { return &alignmentMatrix; }
      
      /// get the alignment matrix
      void setAlignmentMatrix(const TransformationMatrix tm) { alignmentMatrix = tm; }
      
      /// set the model's scaling
      virtual void setScaling(const int viewNumber, const float scaleIn[3]);

      /// set the model's scaling
      virtual void setScaling(const int viewNumber, const float sx, const float sy, const float sz);

   private:
      /// the contours
      ContourFile contours;      

      /// Rotation for section alignment
      vtkTransform* alignmentRotationMatrix;
      
      /// Scaling for section alignment
      float alignmentScaling;
      
      /// Translation for section alignment
      float alignmentTranslation[3];
      
      /// alignment region box
      float alignmentRegionBox[4];
      
      /// alignment region box valid
      bool alignmentRegionBoxValid;
      
      /// aligning region flag
      bool aligningRegionFlag;
      
      /// matrix for aligning
      TransformationMatrix alignmentMatrix;
};

#endif //  __BRAIN_MODEL_CONTOURS_H__

