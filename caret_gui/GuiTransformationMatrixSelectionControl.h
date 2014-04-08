
#ifndef __GUI_TRANSFORMATION_MATRIX_SELECTION_CONTROL_H__
#define __GUI_TRANSFORMATION_MATRIX_SELECTION_CONTROL_H__

#include <QComboBox>

class TransformationMatrix;
class TransformationMatrixFile;

/// combo box for selecting a matrix from a transformation matrix file
class GuiTransformationMatrixSelectionControl : public QComboBox {
   Q_OBJECT
   
   public:
      enum {
         /// "none" matrix index
         NONE_MATRIX_INDEX = -1
      };
      
      /// Constructor
      GuiTransformationMatrixSelectionControl(QWidget* parent,
                                              TransformationMatrixFile* matrixFile,
                                              const bool enableNoneSelectionIn,
                                              const char* name = 0);
                                              
      /// Destructor
      ~GuiTransformationMatrixSelectionControl();

      /// get the selected matrix
      TransformationMatrix* getSelectedMatrix();
      
      /// set the none label
      void setNoneLabel(const QString& s);
      
      /// get the selected matrix (returns NULL if "none" selected)
      const TransformationMatrix* getSelectedMatrix() const;
      
      /// set the selected matrix
      void setSelectedMatrix(const TransformationMatrix* tm);
      
      /// get the selected matrix index (returns -1 if "none" selected)
      int getSelectedMatrixIndex() const;
      
      /// set the selected matrix
      void setSelectedMatrixIndex(const int indx);
      
      /// update the control
      void updateControl();
   
   protected:
      /// the matrix file for this combo box
      TransformationMatrixFile* matrixFile;
      
      /// none selection enabled
      bool noneSelectionEnabled;
      
      /// label for none
      QString noneLabel;
};

#endif //  __GUI_TRANSFORMATION_MATRIX_SELECTION_CONTROL_H__

