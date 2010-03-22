
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


#include "BrainModelContours.h"
#include "BrainSet.h"
#include "ContourCellFile.h"
#include "FileUtilities.h"
#include "MDPlotFile.h"
#include "NeurolucidaFile.h"
#include "vtkTransform.h"

/**
 * Constructor.
 */
BrainModelContours::BrainModelContours(BrainSet* bs)
   : BrainModel(bs, BrainModel::BRAIN_MODEL_CONTOURS)
{
   alignmentRotationMatrix = vtkTransform::New();
   reset();
}

/**
 * Destructor.
 */
BrainModelContours::~BrainModelContours()
{
   alignmentRotationMatrix->Delete();
}

/**
 * Get a descriptive name of the model.
 */
QString
BrainModelContours::getDescriptiveName() const
{
   QString name("CONTOURS ");
   const ContourFile* cf = getContourFile();
   name.append(FileUtilities::basename(cf->getFileName()));
   return name;
}


/**
 * Reset this brain model contours.
 */
void
BrainModelContours::reset()
{
   contours.clear();
   resetAlignmentTransformations();
}

/**
 * Read the specified contours file.  May throw FileException.
 */
void
BrainModelContours::readContourFile(const QString& filename,
                                    const bool append) throw(FileException)
{
   try {
      if (append && (contours.getNumberOfContours() > 0)) {
         ContourFile cf;
         cf.readFile(filename);
         QString msg;
         contours.append(cf, msg);
         if (msg.isEmpty() == false) {
            throw FileException(filename, msg);
         }
      }
      else {
         contours.readFile(filename);
         
         //
         // Use scaling saved in contour file if valid
         //
         float scale[3];
         contours.getMainWindowScaling(scale);
         if ((scale[0] > 0.0) &&
             (scale[1] > 0.0) &&
             (scale[2] > 0.0)) {
            setScaling(0, scale);
         }
         else {
            setScaling(0, 1.0, 1.0, 1.0);
         }
      }
   }
   catch (FileException& e) {
      reset();
      throw e;
   }
}

/**
 * import the NeurolucidaFile file.
 */
void 
BrainModelContours::importNeurolucidaFile(const NeurolucidaFile& nf,
                                          const bool append) throw (FileException)
{
   try {
      if (append == false) {
         contours.clear();
      }
      contours.importNeurolucidaFile(nf);
   }
   catch (FileException& e) {
      reset();
      throw e;
   }
}
                           
/**
 * import the MD Plot file.
 */
void 
BrainModelContours::importMDPlotFile(const MDPlotFile& mf,
                                     const bool append) throw(FileException)
{
   try {
      if (append == false) {
         contours.clear();
      }
      contours.importMDPlotFile(mf);
   }
   catch (FileException& e) {
      reset();
      throw e;
   }
}
   
/**
 * Reset the aligment transformations of the model
 */
void
BrainModelContours::resetAlignmentTransformations()
{
   alignmentMatrix.identity();
   alignmentRotationMatrix->Identity();
   alignmentScaling = 1.0;
   alignmentTranslation[0] = 0.0;
   alignmentTranslation[1] = 0.0;
   alignmentTranslation[2] = 0.0;
   aligningRegionFlag = false;
   resetAlignmentRegionBox();
}

/**
 * Get the alignment region box.
 */
void 
BrainModelContours::getAlignmentRegionBox(float bounds[4])
{
   bounds[0] = alignmentRegionBox[0];
   bounds[1] = alignmentRegionBox[1];
   bounds[2] = alignmentRegionBox[2];
   bounds[3] = alignmentRegionBox[3];
}

/**
 * Set alignment region box start.
 */
void
BrainModelContours::setAlignmentRegionBoxStart(const float xy[2])
{
   alignmentRegionBox[0] = xy[0];
   alignmentRegionBox[1] = xy[1];
   alignmentRegionBox[2] = xy[0];
   alignmentRegionBox[3] = xy[1];
   alignmentRegionBoxValid = true;
}

/**
 * Set alignment region box start.
 */
void
BrainModelContours::setAlignmentRegionBoxEnd(const float xy[2])
{
   alignmentRegionBox[2] = xy[0];
   alignmentRegionBox[3] = xy[1];
}

/**
 * Reset the alignment region box.
 */
void
BrainModelContours::resetAlignmentRegionBox()
{
   alignmentRegionBox[0] = 0.0;
   alignmentRegionBox[1] = 0.0;
   alignmentRegionBox[2] = 0.0;
   alignmentRegionBox[3] = 0.0;
   alignmentRegionBoxValid = false;
}

/**
 * Get the alignment rotation matrix as an array
 */
void
BrainModelContours::getAlignmentRotationMatrix(float matrix[16]) const
{
   vtkMatrix4x4* m = vtkMatrix4x4::New();
   alignmentRotationMatrix->GetMatrix(m);
   
   int cnt = 0;
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[cnt] = m->GetElement(i, j);
         cnt++;
      }
   }
   m->Delete();
}

/**
 * Set the alignment rotation matrix as an array.
 */
void
BrainModelContours::setAlignmentRotationMatrix(const float matrix[16])
{
   vtkMatrix4x4* m = vtkMatrix4x4::New();
   alignmentRotationMatrix->GetMatrix(m);
   
   int cnt = 0;
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m->SetElement(i, j, matrix[cnt]);
         cnt++;
      }
   }
   alignmentRotationMatrix->SetMatrix(m);
   m->Delete();
}

/**
 * Get the model's alignment translation.
 */
void 
BrainModelContours::getAlignmentTranslation(float translationOut[3]) const
{
   translationOut[0] = alignmentTranslation[0];
   translationOut[1] = alignmentTranslation[1];
   translationOut[2] = alignmentTranslation[2];
}

/**
 * Set the model's alignment translation.
 */
void 
BrainModelContours::setAlignmentTranslation(const float translationIn[3])
{
   alignmentTranslation[0] = translationIn[0];
   alignmentTranslation[1] = translationIn[1];
   alignmentTranslation[2] = translationIn[2];
}

/**
 * apply alignment transforms to a section.
 */
void 
BrainModelContours::applyAlignmentToSection(const int sectionNumber)
{
/*
   TransformationMatrix tm ;

   float cogX, cogY;
   contours.getSectionCOG(sectionNumber, cogX, cogY);
   tm.translate(cogX, cogY, 0.0f);
   
   tm.translate(alignmentTranslation[0],
                alignmentTranslation[1],
                alignmentTranslation[2]);

   TransformationMatrix rot;
   rot.setMatrix(alignmentRotationMatrix);
   rot.transpose();  // I'm not sure why this works JWH perhaps OpenGL backwards from VTK.
   tm.multiply(rot);
   
   tm.scale(alignmentScaling, alignmentScaling, 1.0f);

   tm.translate(-cogX, -cogY, 0.0f);
*/
   contours.applyTransformationMatrix(sectionNumber,
                                      sectionNumber,
                                      alignmentMatrix,
                                      getAligningRegionFlag());

   ContourCellFile* contourCells = brainSet->getContourCellFile();
   contourCells->applyTransformationMatrix(sectionNumber,
                                           sectionNumber,
                                           alignmentMatrix,
                                           getAligningRegionFlag());
}

/**
 * apply transformations to all section.
 */
void 
BrainModelContours::applyTransformationsToAllContours()
{
   TransformationMatrix tm;
   tm.translate(translation[0][0], translation[0][1], translation[0][2]);
   TransformationMatrix rot;
   rot.setMatrix(rotationMatrix[0]);
   tm.preMultiply(rot);
   tm.scale(scaling[0][0], scaling[0][1], 1.0f);
   contours.applyTransformationMatrix(contours.getMinimumSection(),
                                      contours.getMaximumSection(),
                                      tm,
                                      false);
   resetViewingTransformations();
}
      
/**
 * reset the viewing transform.
 */
void 
BrainModelContours::resetViewingTransform(const int viewNumber)
{
   float m[16];
   m[0]  =  1.0;
   m[1]  =  0.0;
   m[2]  =  0.0;
   m[3]  =  0.0;
   m[4]  =  0.0;
   m[5]  =  1.0;
   m[6]  =  0.0;
   m[7]  =  0.0;
   m[8]  =  0.0;
   m[9]  =  0.0;
   m[10] =  1.0;
   m[11] =  0.0;
   m[12] =  0.0;
   m[13] =  0.0;
   m[14] =  0.0;
   m[15] =  1.0;
   scaling[viewNumber][0] = 1.0;
   scaling[viewNumber][1] = 1.0;
   scaling[viewNumber][2] = 1.0;
   translation[viewNumber][0] = 0.0;
   translation[viewNumber][1] = 0.0;
   translation[viewNumber][2] = 0.0;
   setRotationMatrix(viewNumber, m);
}

/**
 * set the model's scaling.
 */
void
BrainModelContours::setScaling(const int viewNumber, const float scaleIn[3])
{
   BrainModel::setScaling(viewNumber, scaleIn);
   if (viewNumber == 0) {
      contours.setMainWindowScaling(scaleIn);
   }
}

/**
 * set the model's scaling.
 */
void
BrainModelContours::setScaling(const int viewNumber, const float sx, const float sy, const float sz)
{
   float s[3] = { sx, sy, sz };
   setScaling(viewNumber, s);
}


