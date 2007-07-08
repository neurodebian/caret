
#ifndef __GUI_ALIGN_SURFACE_TO_STANDARD_ORIENTATION_DIALOG_H__
#define __GUI_ALIGN_SURFACE_TO_STANDARD_ORIENTATION_DIALOG_H__

#include "QtDialog.h"

class GuiBrainModelSelectionComboBox;
class QCheckBox;
class QLabel;

/// class for dialog used to align a surface to standard orientation
class GuiAlignSurfaceToStandardOrientationDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiAlignSurfaceToStandardOrientationDialog(QWidget* parent);
      
      /// Destructor
      ~GuiAlignSurfaceToStandardOrientationDialog();
   
      /// called to set medial tip node
      void setMedialTipNode(const int node);
      
      /// called to set medial tip node
      void setVentralTipNode(const int node);
      
   private slots:
      /// called when align sphere check box is toggled
      void slotAlignSphereCheckBox(bool onoff);
      
      /// called when apply pushbutton is pressed
      void slotApplyPushButton();
      
      /// called when reset pushbutton is pressed
      void slotResetPushButton();
      
   private:
      /// dorsal tip x label
      QLabel* medialTipXLabel;
      
      /// dorsal tip y label
      QLabel* medialTipYLabel;
      
      /// ventral tip x label
      QLabel* ventralTipXLabel;
      
      /// ventral tip y label
      QLabel* ventralTipYLabel;
      
      /// align sphere check box
      QCheckBox* alignSphereCheckBox;
      
      /// generate lat/lon check box
      QCheckBox* sphereLatLonCheckBox;
      
      /// scale to fiducial area check box
      QCheckBox* scaleSurfaceCheckBox;
      
      /// sphere selection combo box
      GuiBrainModelSelectionComboBox* sphereSurfaceComboBox;
      
      /// medial tip node
      int medialTipNode;
      
      /// ventral tip node
      int ventralTipNode;
};

#endif // __GUI_ALIGN_SURFACE_TO_STANDARD_ORIENTATION_DIALOG_H__

