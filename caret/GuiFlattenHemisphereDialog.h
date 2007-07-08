
#ifndef __GUI_FLATTEN_HEMISPHERE_DIALOG_H__
#define __GUI_FLATTEN_HEMISPHERE_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

class GuiBrainModelSelectionComboBox;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;

/// Dialog for flattening hemispheres
class GuiFlattenHemisphereDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// type of flattening
      enum FLATTEN_TYPE {
         FLATTEN_TYPE_NONE,
         FLATTEN_TYPE_FULL_HEMISPHERE,
         FLATTEN_TYPE_FULL_HEMISPHERE_AND_MORPH_SPHERE,
         FLATTEN_TYPE_ELLIPSOID_PARTIAL_HEMISPHERE,
         FLATTEN_TYPE_FIDUCIAL_PARTIAL_HEMISPHERE         
      };
      
      /// Constructor
      GuiFlattenHemisphereDialog(QWidget* parent);
      
      /// Destructor
      ~GuiFlattenHemisphereDialog();
      
   private slots:
      /// Called when user press OK or Cancel buttons
      void done(int r);

      /// called when a flatten type is selected
      void slotFlatTypeComboBox(int item);
      
      /// load the anterior commissure information from the params file
      void loadAnteriorCommissureInformation();
      
   private:
      /// load the template border combo box
      void loadTemplateBorderComboBox();
      
      /// type of flattening
      FLATTEN_TYPE flattenType;
      
      /// fiducial surface selection combo box
      GuiBrainModelSelectionComboBox* fiducialSurfaceComboBox;
      
      /// flattening surface selection combo box
      GuiBrainModelSelectionComboBox* flattenSurfaceComboBox;
      
      /// flatten type combo box
      QComboBox* flatTypeComboBox;
      
      /// full hemisphere parameters group box
      QGroupBox* fullHemisphereParamsGroupBox;
      
      /// Anterior commissure offset
      QLineEdit* acOffsetLineEdit[3];

      /// Anterior commissure position
      QLineEdit* acPositionLineEdit[3];
      
      /// template border type selection
      QComboBox* templateBorderComboBox;
      
      /// the template border files
      std::vector<QString> templateBorderFiles;
      
      /// the template border color files
      std::vector<QString> templateBorderColorFiles;
      
      /// smooth fiducial's medial wall check box
      QCheckBox* smoothFiducialMedialWallCheckBox;
};

#endif //  __GUI_FLATTEN_HEMISPHERE_DIALOG_H__

