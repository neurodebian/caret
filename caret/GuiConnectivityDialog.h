#ifndef __GUI_CONNECTIVITY_DIALOG_H__
#define	__GUI_CONNECTIVITY_DIALOG_H__

#include "BrainSetAutoLoaderManager.h"
#include "WuQDialog.h"

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QStackedWidget;
class GuiBrainModelSurfaceSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class GuiVolumeFileSelectionComboBox;

/// dialog for connectivity operations
class GuiConnectivityDialog : public WuQDialog {
   Q_OBJECT

   public:
      // constructor
      GuiConnectivityDialog(QWidget* parent = 0,
                            Qt::WindowFlags f = 0);

      // destructor
      ~GuiConnectivityDialog();

      // update this dialog
      void updateDialog();

   private slots:
      /// called when metric auto load directory button pressed
      void slotMetricAutoLoadDirectoryPushButton(int indx);

      /// called to read metric auto load controls
      void readMetricAutoLoadControls();

      /// called when functional volume auto load directory button pressed
      void slotFunctionalVolumeAutoLoadDirectoryPushButton(int indx);

      /// called to read functional volume auto load controls
      void readFunctionalVolumeAutoLoadControls();

      /// called when a page selection is made
      void slotPageSelectionComboBox();
      
      /// called when cluster auto load metric directory button pressed
      void slotClusterAutoLoadMetricDirectoryPushButton(int indx);

      /// called when cluster auto load functional volume directory button pressed
      void slotClusterAutoLoadFunctionalVolumeDirectoryPushButton(int indx);

      /// called to read cluster auto load controls
      void readClusterAutoLoadControls();

   private:
      enum PAGES {
         PAGE_CLUSTER_AUTO_LOADER,
         PAGE_FUNCTIONAL_VOLUME_AUTO_LOADER,
         PAGE_METRIC_AUTO_LOADER
      };

      /// update the metric auto loadcontrols
      void updateMetricAutoLoadControls();

      /// update the functional volume auto loadcontrols
      void updateFunctionalVolumeAutoLoadControls();

      /// update the cluster auto loadcontrols
      void updateClusterAutoLoadControls();

      /// create the metric auto loader widget
      QWidget* createMetricAutoLoadersWidget();

      /// create the functional volume loader widget
      QWidget* createFunctionalVolumeAutoLoaderWidget();

      /// create the cluster auto loaders widget
      QWidget* createClusterAutoLoaderWidget();

      /// metric auto load replace column check box
      QCheckBox* metricAutoLoadReplaceColumnCheckBox[BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS];

      /// metric auto load group box
      QGroupBox* metricAutoLoadGroupBox[BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS];

      /// metric auto load directory line edit
      QLineEdit* metricAutoLoadDirectoryLineEdit[BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS];

      /// metric auto load anatomical volume selection control
      GuiVolumeFileSelectionComboBox* metricAutoLoadVolumeSelectionControl[BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS];

      /// metric auto load volume intersection surface selection control
      GuiBrainModelSurfaceSelectionComboBox* metricAutoLoadVolumeIntersectionSurfaceSelectionControl[BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS];

      /// metric auto load display surface selection control
      GuiBrainModelSurfaceSelectionComboBox* metricAutoLoadDisplaySurfaceSelectionControl[BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS];

      /// metric auto load last voxel indices label
      QLabel* metricAutoLoadLastVoxelLabel[BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS];

      /// an update of the metric controls is in progress
      bool updatingMetricAutoLoadControlsFlag;



      /// functional volume auto load replace file check box
      QCheckBox* functionalVolumeAutoLoadReplaceFileCheckBox[BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS];

      /// functional volume auto load group box
      QGroupBox* functionalVolumeAutoLoadGroupBox[BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS];

      /// functional volume auto load directory line edit
      QLineEdit* functionalVolumeAutoLoadDirectoryLineEdit[BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS];

      /// functional volume auto load anatomical volume selection control
      GuiVolumeFileSelectionComboBox* functionalVolumeAutoLoadVolumeSelectionControl[BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS];

      /// functional volume auto load volume intersection surface selection control
      GuiBrainModelSurfaceSelectionComboBox* functionalVolumeAutoLoadVolumeIntersectionSurfaceSelectionControl[BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS];

      /// functional volume auto load last voxel label
      QLabel* functionalVolumeAutoLoadLastVoxelLabel[BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS];

      /// an update of the functional volume controls is in progress
      bool updatingFunctionalVolumeAutoLoadControlsFlag;
      

      /// cluster auto load replace files check box
      QCheckBox* clusterAutoLoadReplaceFilesCheckBox[BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// cluster auto load group box
      QGroupBox* clusterAutoLoadGroupBox[BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// cluster auto load metric directory line edit
      QLineEdit* clusterAutoLoadMetricDirectoryLineEdit[BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// cluster auto load functional volume directory line edit
      QLineEdit* clusterAutoLoadFunctionalVolumeDirectoryLineEdit[BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// cluster auto load display surface selection control
      GuiBrainModelSurfaceSelectionComboBox* clusterAutoLoadDisplaySurfaceSelectionControl[BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// cluster auto load last node number label
      QLabel* clusterAutoLoadLastNodeNumberLabel[BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// cluster auto load paint column selection combo box
      GuiNodeAttributeColumnSelectionComboBox* clusterAutoLoadPaintSelectionComboBox[BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS];

      /// an update of the cluster controls is in progress
      bool updatingClusterAutoLoadControlsFlag;


      /// metric auto loaders widget
      QWidget* pageMetricAutoLoaders;

      /// functional volumes widget
      QWidget* pageFunctionalVolumeAutoLoaders;

      /// cluster auto loaders widget
      QWidget* pageClusterAutoLoaders;

      /// page selection combo box
      QComboBox* pageSelectionComboBox;


      /// stacked widget for pages
      QStackedWidget* pageStackedWidget;


};

#endif	/* __GUI_CONNECTIVITY_DIALOG_H__ */

