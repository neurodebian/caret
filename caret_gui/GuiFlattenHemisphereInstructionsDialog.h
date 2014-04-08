
#ifndef __GUI_FLATTEN_HEMISPHERE_INSTRUCTIONS_DIALOG_H__
#define __GUI_FLATTEN_HEMISPHERE_INSTRUCTIONS_DIALOG_H__

#include "WuQDialog.h"

class BrainModelSurface;
class BrainModelSurfaceFlattenFullHemisphere;
class QTextEdit;

/// Dialog for continuing flattening 
class GuiFlattenHemisphereInstructionsDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// mode of the dialog
      enum DIALOG_MODE {
         DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_PART2,
         DIALOG_MODE_FLATTEN_FULL_HEMISPHERE_MORPH_SPHERE_PART2,
         DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_PART_HEM,
         DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM,
         DIALOG_MODE_FLATTEN_POST_INITIAL_FLAT_FULL_HEM_MORPH_SPHERE         
      };
      
      /// constructor
      GuiFlattenHemisphereInstructionsDialog(QWidget* parent,
                                 BrainModelSurfaceFlattenFullHemisphere* algorithmIn,
                                 const DIALOG_MODE dialogModeIn,
                                 Qt::WindowFlags f = 0);
                     
      /// destructor
      ~GuiFlattenHemisphereInstructionsDialog();
      
   private slots:
      /// Called when user press OK or Cancel buttons
      void done(int r);

   private:
      /// do second half of flattening
      void doFlattenFullHemispherePart2();
      
      // show crossovers on surface
      void showCrossovers(BrainModelSurface* bms = NULL);
      
      /// Mode of the dialog
      DIALOG_MODE dialogMode;
      
      /// Full hemisphere algorithm
      BrainModelSurfaceFlattenFullHemisphere* algorithm;
      
      /// the text section
      QTextEdit* textEdit;
};

#endif // __GUI_FLATTEN_HEMISPHERE_INSTRUCTIONS_DIALOG_H__

