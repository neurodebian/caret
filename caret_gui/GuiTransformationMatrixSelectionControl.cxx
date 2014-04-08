

#include "GuiTransformationMatrixSelectionControl.h"
#include "StringUtilities.h"
#include "TransformationMatrixFile.h"

/**
 * Constructor.
 */
GuiTransformationMatrixSelectionControl::GuiTransformationMatrixSelectionControl(
                                                       QWidget* parent,
                                                       TransformationMatrixFile* matrixFileIn,
                                                       const bool enableNoneSelectionIn,
                                                       const char* /*name*/)
   : QComboBox(parent)
{
   noneLabel = "None";
   noneSelectionEnabled = enableNoneSelectionIn;
   matrixFile = matrixFileIn;
   updateControl();
}

                                     
/**
 * Destructor.
 */
GuiTransformationMatrixSelectionControl::~GuiTransformationMatrixSelectionControl()
{
}

/**
 * get the selected matrix.
 */
TransformationMatrix* 
GuiTransformationMatrixSelectionControl::getSelectedMatrix()
{
   const int item = getSelectedMatrixIndex();
   if ((item >= 0) && (item < matrixFile->getNumberOfMatrices())) {
      return matrixFile->getTransformationMatrix(item);
   }
   return NULL;
}

/**
 * get the selected matrix.
 */
const 
TransformationMatrix* GuiTransformationMatrixSelectionControl::getSelectedMatrix() const
{
   const int item = getSelectedMatrixIndex();
   if ((item >= 0) && (item < matrixFile->getNumberOfMatrices())) {
      return matrixFile->getTransformationMatrix(item);
   }
   return NULL;
}

/**
 * set the selected matrix.
 */
void 
GuiTransformationMatrixSelectionControl::setSelectedMatrix(const TransformationMatrix* tm)
{
   if (tm == NULL) {
      setSelectedMatrixIndex(NONE_MATRIX_INDEX);
   }
   else {
      for (int i = 0; i < matrixFile->getNumberOfMatrices(); i++) {
         if (matrixFile->getTransformationMatrix(i) == tm) {
            setSelectedMatrixIndex(i);
            break;
         }
      }
   }
}

/**
 * get the selected matrix index.
 */
int 
GuiTransformationMatrixSelectionControl::getSelectedMatrixIndex() const
{
   //
   // is NONE selected
   //
   if (currentIndex() >= matrixFile->getNumberOfMatrices()) {
      return -1;
   }
   return currentIndex();
}

/**
 * set the selected matrix.
 */
void 
GuiTransformationMatrixSelectionControl::setSelectedMatrixIndex(const int indxIn)
{
   int indx = indxIn;
   if (indx == NONE_MATRIX_INDEX) {
      indx = count() - 1;
   }
   setCurrentIndex(indx);
}
      
/**
 * set the none label.
 */
void 
GuiTransformationMatrixSelectionControl::setNoneLabel(const QString& s) 
{ 
   noneLabel = s; 
   updateControl();
}
      
/**
 * update the control.
 */
void 
GuiTransformationMatrixSelectionControl::updateControl()
{
   const TransformationMatrix* selectedMatrix = getSelectedMatrix();
   int defaultItem = 0;
   if ((selectedMatrix == NULL) && noneSelectionEnabled) {
      if (getSelectedMatrixIndex() == NONE_MATRIX_INDEX) {
         defaultItem = NONE_MATRIX_INDEX;
      }
   }
   
   clear();
   const int num = matrixFile->getNumberOfMatrices();
   for (int i = 0; i < num; i++) {
      const TransformationMatrix* tm = matrixFile->getTransformationMatrix(i);
      if (tm == selectedMatrix) {
         defaultItem = i;
      }
      QString name(tm->getMatrixName());
      if (name.isEmpty()) {
         name = "Matrix ";
         name.append(StringUtilities::fromNumber(i));
      }
      addItem(name);
   }
   
   if (noneSelectionEnabled) {
      addItem(noneLabel);
   }
   
   if ((defaultItem >= 0) && (defaultItem < num)) {
      setSelectedMatrixIndex(defaultItem);
   }
   else {
      //
      // Default to none
      //
      if (noneSelectionEnabled) {
         setSelectedMatrixIndex(NONE_MATRIX_INDEX);
      }
      else if (count() > 0) {
         setSelectedMatrixIndex(0);
      }
   }
}
