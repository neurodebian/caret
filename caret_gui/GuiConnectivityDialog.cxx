#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QDir>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "BrainSetAutoLoaderManager.h"
#include "BrainSetAutoLoaderFileFunctionalVolume.h"
#include "BrainSetAutoLoaderFileMetric.h"
#include "BrainSetAutoLoaderFileMetricByNode.h"
#include "BrainSetAutoLoaderFilePaintCluster.h"
#include "GuiBrainModelSurfaceSelectionComboBox.h"
#include "GuiConnectivityDialog.h"
#include "GuiMainWindow.h"
#include "GuiNodeAttributeColumnSelectionComboBox.h"
#include "GuiVolumeFileSelectionComboBox.h"
#include "WuQFileDialog.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiConnectivityDialog::GuiConnectivityDialog(QWidget* parent,
                                                 Qt::WindowFlags f)
   : WuQDialog(parent, f)
{
   updatingMetricAutoLoadControlsFlag = true;
   updatingMetricNodeAutoLoadControlsFlag = true;
   updatingFunctionalVolumeAutoLoadControlsFlag = true;
   updatingClusterAutoLoadControlsFlag = true;

   this->setWindowTitle("Connectivity");

   //
   // Create the auto loaders
   //
   pageClusterAutoLoaders = createClusterAutoLoaderWidget();
   pageMetricAutoLoaders = createMetricAutoLoadersWidget();
   pageMetricNodeAutoLoaders = createMetricNodeAutoLoadersWidget();
   pageFunctionalVolumeAutoLoaders = createFunctionalVolumeAutoLoaderWidget();

   //
   // Combo box for selecting pages
   //
   QLabel* pageLabel = new QLabel("Page");
   pageSelectionComboBox = new QComboBox;
   pageSelectionComboBox->addItem("Cluster Auto Loaders",
                                 PAGE_CLUSTER_AUTO_LOADER);
   pageSelectionComboBox->addItem("Functional Volume Auto Loaders",
                                  PAGE_FUNCTIONAL_VOLUME_AUTO_LOADER);
   pageSelectionComboBox->addItem("Metric Auto Loaders",
                                  PAGE_METRIC_AUTO_LOADER);
   pageSelectionComboBox->addItem("Metric Node Auto Loaders",
                                  PAGE_METRIC_NODE_AUTO_LOADER);
   QObject::connect(pageSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotPageSelectionComboBox()));
   QHBoxLayout* pageControlLayout = new QHBoxLayout();
   pageControlLayout->addWidget(pageLabel);
   pageControlLayout->addWidget(pageSelectionComboBox);
   pageControlLayout->addStretch();


   //
   // Stacked widget for displaying pages
   //
   pageStackedWidget = new QStackedWidget;
   pageStackedWidget->addWidget(pageClusterAutoLoaders);
   pageStackedWidget->addWidget(pageFunctionalVolumeAutoLoaders);
   pageStackedWidget->addWidget(pageMetricAutoLoaders);
   pageStackedWidget->addWidget(pageMetricNodeAutoLoaders);
   
   //
   // Scroll area for pages
   //
   QScrollArea* scrollArea = new QScrollArea();
   scrollArea->setWidget(pageStackedWidget);
   scrollArea->setWidgetResizable(true);
   
   //
   // Close button connects to QDialogs close() slot.
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setFixedSize(closeButton->sizeHint());
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));

   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   buttonsLayout->addWidget(closeButton);

   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addLayout(pageControlLayout);
   dialogLayout->addWidget(scrollArea);
   dialogLayout->addLayout(buttonsLayout);

   updatingMetricAutoLoadControlsFlag = false;
   updatingFunctionalVolumeAutoLoadControlsFlag = false;
   updatingClusterAutoLoadControlsFlag = false;

   slotPageSelectionComboBox();
   updateDialog();
}

/**
 * Called when a page selection is made.
 */
void
GuiConnectivityDialog::slotPageSelectionComboBox()
{
   const int indx = this->pageSelectionComboBox->currentIndex();
   const PAGES page = static_cast<PAGES>(
      this->pageSelectionComboBox->itemData(indx).toInt());

   QWidget* newPage = NULL;
   switch (page) {
      case PAGE_CLUSTER_AUTO_LOADER:
         newPage = pageClusterAutoLoaders;
         break;
      case PAGE_FUNCTIONAL_VOLUME_AUTO_LOADER:
         newPage = pageFunctionalVolumeAutoLoaders;
         break;
      case PAGE_METRIC_AUTO_LOADER:
         newPage = pageMetricAutoLoaders;
         break;
      case PAGE_METRIC_NODE_AUTO_LOADER:
         newPage = pageMetricNodeAutoLoaders;
         break;
   }

   if (newPage != NULL) {
      pageStackedWidget->setCurrentWidget(newPage);
   }
}


/**
 * Destructor.
 */
GuiConnectivityDialog::~GuiConnectivityDialog()
{
}

/**
 *  update this dialog.
 */
void
GuiConnectivityDialog::updateDialog()
{
   this->updateClusterAutoLoadControls();
   this->updateMetricAutoLoadControls();
   this->updateMetricNodeAutoLoadControls();
   this->updateFunctionalVolumeAutoLoadControls();
}

/**
 * Called to read metric controls.
 */
void
GuiConnectivityDialog::readMetricAutoLoadControls()
{
   if (updatingMetricAutoLoadControlsFlag) {
      return;
   }

   BrainSet* bs = theMainWindow->getBrainSet();
   BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS; i++) {
       BrainSetAutoLoaderFileMetric* alm = autoLoaderManager->getMetricAutoLoader(i);
       alm->setAutoLoadMetricDisplaySurface(
               this->metricAutoLoadDisplaySurfaceSelectionControl[i]->getSelectedBrainModelSurface());
       alm->setAutoLoadAnatomyVolumeFile(
               this->metricAutoLoadVolumeSelectionControl[i]->getSelectedVolumeFile());
       alm->setAutoLoadVolumeIntersectionSurface(
               this->metricAutoLoadVolumeIntersectionSurfaceSelectionControl[i]->getSelectedBrainModelSurface());
       alm->setAutoLoadDirectoryName(this->metricAutoLoadDirectoryLineEdit[i]->text());
       alm->setAutoLoadEnabled(this->metricAutoLoadGroupBox[i]->isChecked());
       alm->setAutoLoadReplaceLastFileEnabled(this->metricAutoLoadReplaceColumnCheckBox[i]->isChecked());
   }
}

/**
 * Update the metric controls.
 */
void
GuiConnectivityDialog::updateMetricAutoLoadControls()
{
   updatingMetricAutoLoadControlsFlag = true;

   BrainSet* bs = theMainWindow->getBrainSet();
   BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS; i++) {
       BrainSetAutoLoaderFileMetric* alm = autoLoaderManager->getMetricAutoLoader(i);
       this->metricAutoLoadDisplaySurfaceSelectionControl[i]->updateComboBox();
       this->metricAutoLoadDisplaySurfaceSelectionControl[i]->setSelectedBrainModelSurface(
              alm->getAutoLoadMetricDisplaySurface());
       this->metricAutoLoadVolumeSelectionControl[i]->updateComboBox();
       this->metricAutoLoadVolumeSelectionControl[i]->setSelectedVolumeFile(
                                     alm->getAutoLoadAnatomyVolumeFile());
       this->metricAutoLoadVolumeIntersectionSurfaceSelectionControl[i]->updateComboBox();
       this->metricAutoLoadVolumeIntersectionSurfaceSelectionControl[i]->setSelectedBrainModelSurface(
                      alm->getAutoLoadVolumeIntersectionSurface());
       this->metricAutoLoadDirectoryLineEdit[i]->setText(
                    alm->getAutoLoadDirectoryName());
       this->metricAutoLoadGroupBox[i]->setChecked(alm->getAutoLoadEnabled());
       this->metricAutoLoadReplaceColumnCheckBox[i]->setChecked(alm->getAutoLoadReplaceLastFileEnabled());

       VoxelIJK lastVoxel = alm->getLastAutoLoadAnatomyVoxelIndices();
       QString indicesString = "("
                             + QString::number(lastVoxel.getI()) + ", "
                             + QString::number(lastVoxel.getJ()) + ", "
                             + QString::number(lastVoxel.getK()) + ")";
       this->metricAutoLoadLastVoxelLabel[i]->setText(indicesString);
   }

   updatingMetricAutoLoadControlsFlag = false;
}

/**
 * Called when metric auto load directory button pressed.
 */
void
GuiConnectivityDialog::slotMetricAutoLoadDirectoryPushButton(int indx)
{
   QString dirName = WuQFileDialog::getExistingDirectory(this,
                                   "Choose Metric File Directory",
                                   QDir::currentPath());
   if (dirName.isEmpty() == false) {
      this->metricAutoLoadDirectoryLineEdit[indx]->setText(dirName);
      readMetricAutoLoadControls();
   }
}

/**
 * Create the metric auto loader widget.
 */
QWidget*
GuiConnectivityDialog::createMetricAutoLoadersWidget()
{
   //
   // Metric auto load controls
   //
   QWidget* autoLoadWidget = new QWidget;
   QVBoxLayout* autoLoadLayouts = new QVBoxLayout(autoLoadWidget);

   QButtonGroup* autoLoadButtonGroup = new QButtonGroup(this);
   QObject::connect(autoLoadButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotMetricAutoLoadDirectoryPushButton(int)));

   std::vector<BrainModelSurface::SURFACE_TYPES> surfaceTypes;
   std::vector<QString> surfaceNames;
   BrainModelSurface::getSurfaceTypesAndNames(surfaceTypes, surfaceNames);

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_AUTO_LOADERS; i++) {
       this->metricAutoLoadReplaceColumnCheckBox[i] = new QCheckBox("Replace Previously Loaded Column");
       QObject::connect(this->metricAutoLoadReplaceColumnCheckBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readMetricAutoLoadControls()));

       QPushButton* autoLoadDirectoryPushButton = new QPushButton("Metric Directory...");
       autoLoadButtonGroup->addButton(autoLoadDirectoryPushButton, i);
       this->metricAutoLoadDirectoryLineEdit[i] = new QLineEdit;
       this->metricAutoLoadDirectoryLineEdit[i]->setEnabled(false);

       QLabel* autoLoadVolumeLabel = new QLabel("Anatomy Volume");
       this->metricAutoLoadVolumeSelectionControl[i] =
               new GuiVolumeFileSelectionComboBox(VolumeFile::VOLUME_TYPE_ANATOMY);
       QObject::connect(this->metricAutoLoadVolumeSelectionControl[i], SIGNAL(activated(int)),
                        this, SLOT(readMetricAutoLoadControls()));

       QLabel* displaySurfaceLabel = new QLabel("Display Surface");
       this->metricAutoLoadDisplaySurfaceSelectionControl[i] =
               new GuiBrainModelSurfaceSelectionComboBox(surfaceTypes);
       QObject::connect(this->metricAutoLoadDisplaySurfaceSelectionControl[i], SIGNAL(activated(int)),
                        this, SLOT(readMetricAutoLoadControls()));

       QLabel* autoLoadVolumeSurfaceLabel = new QLabel("Volume Intersect Surface");
       this->metricAutoLoadVolumeIntersectionSurfaceSelectionControl[i] =
               new GuiBrainModelSurfaceSelectionComboBox(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
       QObject::connect(this->metricAutoLoadVolumeIntersectionSurfaceSelectionControl[i], SIGNAL(activated(int)),
                        this, SLOT(readMetricAutoLoadControls()));

       QLabel* lastVoxelLabel = new QLabel("Last Voxel");
       metricAutoLoadLastVoxelLabel[i] = new QLabel("                         ");

       this->metricAutoLoadGroupBox[i] = new QGroupBox("Auto Load Metric " + QString::number(i + 1));
       this->metricAutoLoadGroupBox[i]->setCheckable(true);
       QObject::connect(this->metricAutoLoadGroupBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readMetricAutoLoadControls()));
       QGridLayout* metricAutoLoadLayout = new QGridLayout(metricAutoLoadGroupBox[i]);
       metricAutoLoadLayout->addWidget(metricAutoLoadReplaceColumnCheckBox[i], 0, 0, 1, 2);
       metricAutoLoadLayout->addWidget(autoLoadDirectoryPushButton, 1, 0);
       metricAutoLoadLayout->addWidget(this->metricAutoLoadDirectoryLineEdit[i], 1, 1);
       metricAutoLoadLayout->addWidget(displaySurfaceLabel, 2, 0);
       metricAutoLoadLayout->addWidget(this->metricAutoLoadDisplaySurfaceSelectionControl[i], 2, 1);
       metricAutoLoadLayout->addWidget(autoLoadVolumeLabel, 3, 0);
       metricAutoLoadLayout->addWidget(this->metricAutoLoadVolumeSelectionControl[i], 3, 1);
       metricAutoLoadLayout->addWidget(autoLoadVolumeSurfaceLabel, 4, 0);
       metricAutoLoadLayout->addWidget(this->metricAutoLoadVolumeIntersectionSurfaceSelectionControl[i], 4, 1);
       metricAutoLoadLayout->addWidget(lastVoxelLabel, 5, 0);
       metricAutoLoadLayout->addWidget(metricAutoLoadLastVoxelLabel[i], 5, 1);
       autoLoadLayouts->addWidget(this->metricAutoLoadGroupBox[i]);
   }

   autoLoadLayouts->addStretch();
   return autoLoadWidget;
}

/**
 * Create the metric node auto loader widget.
 */
QWidget*
GuiConnectivityDialog::createMetricNodeAutoLoadersWidget()
{
   //
   // Metric auto load controls
   //
   QWidget* autoLoadWidget = new QWidget;
   QVBoxLayout* autoLoadLayouts = new QVBoxLayout(autoLoadWidget);

   QButtonGroup* autoLoadButtonGroup = new QButtonGroup(this);
   QObject::connect(autoLoadButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotMetricNodeAutoLoadDirectoryPushButton(int)));

   std::vector<BrainModelSurface::SURFACE_TYPES> surfaceTypes;
   std::vector<QString> surfaceNames;
   BrainModelSurface::getSurfaceTypesAndNames(surfaceTypes, surfaceNames);

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_NODE_AUTO_LOADERS; i++) {
       this->metricNodeAutoLoadReplaceColumnCheckBox[i] = new QCheckBox("Replace Previously Loaded Column");
       QObject::connect(this->metricNodeAutoLoadReplaceColumnCheckBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readMetricNodeAutoLoadControls()));

       QPushButton* autoLoadDirectoryPushButton = new QPushButton("Metric Directory...");
       autoLoadButtonGroup->addButton(autoLoadDirectoryPushButton, i);
       this->metricNodeAutoLoadDirectoryLineEdit[i] = new QLineEdit;
       this->metricNodeAutoLoadDirectoryLineEdit[i]->setEnabled(false);

       QLabel* displaySurfaceLabel = new QLabel("Display Surface");
       this->metricNodeAutoLoadDisplaySurfaceSelectionControl[i] =
               new GuiBrainModelSurfaceSelectionComboBox(surfaceTypes);
       QObject::connect(this->metricNodeAutoLoadDisplaySurfaceSelectionControl[i], SIGNAL(activated(int)),
                        this, SLOT(readMetricNodeAutoLoadControls()));

       this->metricNodeAutoLoadGroupBox[i] = new QGroupBox("Auto Load Metric By Node " + QString::number(i + 1));
       this->metricNodeAutoLoadGroupBox[i]->setCheckable(true);
       QObject::connect(this->metricNodeAutoLoadGroupBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readMetricNodeAutoLoadControls()));
       QGridLayout* metricAutoLoadLayout = new QGridLayout(metricNodeAutoLoadGroupBox[i]);
       metricAutoLoadLayout->addWidget(metricNodeAutoLoadReplaceColumnCheckBox[i], 0, 0, 1, 2);
       metricAutoLoadLayout->addWidget(autoLoadDirectoryPushButton, 1, 0);
       metricAutoLoadLayout->addWidget(this->metricNodeAutoLoadDirectoryLineEdit[i], 1, 1);
       metricAutoLoadLayout->addWidget(displaySurfaceLabel, 2, 0);
       metricAutoLoadLayout->addWidget(this->metricNodeAutoLoadDisplaySurfaceSelectionControl[i], 2, 1);
       autoLoadLayouts->addWidget(this->metricNodeAutoLoadGroupBox[i]);
   }

   autoLoadLayouts->addStretch();
   return autoLoadWidget;
}


/**
 * Called when metric node auto load directory button pressed.
 */
void
GuiConnectivityDialog::slotMetricNodeAutoLoadDirectoryPushButton(int indx)
{
    QString dirName = WuQFileDialog::getExistingDirectory(this,
                                    "Choose Metric File Directory",
                                    QDir::currentPath());
    if (dirName.isEmpty() == false) {
       this->metricNodeAutoLoadDirectoryLineEdit[indx]->setText(dirName);
       readMetricNodeAutoLoadControls();
    }
}

/**
 * Called to read metric not auto load controls.
 */
void
GuiConnectivityDialog::readMetricNodeAutoLoadControls()
{
    if (updatingMetricNodeAutoLoadControlsFlag) {
       return;
    }

    BrainSet* bs = theMainWindow->getBrainSet();
    BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
    for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_NODE_AUTO_LOADERS; i++) {
        BrainSetAutoLoaderFileMetricByNode* alm = autoLoaderManager->getMetricNodeAutoLoader(i);
        alm->setAutoLoadDirectoryName(this->metricNodeAutoLoadDirectoryLineEdit[i]->text());
        alm->setAutoLoadEnabled(this->metricNodeAutoLoadGroupBox[i]->isChecked());
        alm->setAutoLoadReplaceLastFileEnabled(this->metricNodeAutoLoadReplaceColumnCheckBox[i]->isChecked());
        alm->setAutoLoadMetricDisplaySurface(
                this->metricNodeAutoLoadDisplaySurfaceSelectionControl[i]->getSelectedBrainModelSurface());
    }
}

/**
 * Update the metric node auto loadcontrols.
 */
void
GuiConnectivityDialog::updateMetricNodeAutoLoadControls()
{
    updatingMetricNodeAutoLoadControlsFlag = true;

    BrainSet* bs = theMainWindow->getBrainSet();
    BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
    for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_METRIC_NODE_AUTO_LOADERS; i++) {
        BrainSetAutoLoaderFileMetricByNode* alm = autoLoaderManager->getMetricNodeAutoLoader(i);
        this->metricNodeAutoLoadReplaceColumnCheckBox[i]->setChecked(alm->getAutoLoadReplaceLastFileEnabled());
        this->metricNodeAutoLoadDirectoryLineEdit[i]->setText(
                     alm->getAutoLoadDirectoryName());
        this->metricNodeAutoLoadGroupBox[i]->setChecked(alm->getAutoLoadEnabled());
        this->metricNodeAutoLoadDisplaySurfaceSelectionControl[i]->updateComboBox();
        this->metricNodeAutoLoadDisplaySurfaceSelectionControl[i]->setSelectedBrainModelSurface(
               alm->getAutoLoadMetricDisplaySurface());
    }

    updatingMetricNodeAutoLoadControlsFlag = false;
}



/**
 * Create the cluster auto loaders widget.
 */
QWidget*
GuiConnectivityDialog::createClusterAutoLoaderWidget()
{
   //
   // Cluster auto load controls
   //
   QWidget* autoLoadWidget = new QWidget;
   QVBoxLayout* autoLoadLayouts = new QVBoxLayout(autoLoadWidget);

   QButtonGroup* autoLoadMetricButtonGroup = new QButtonGroup(this);
   QObject::connect(autoLoadMetricButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotClusterAutoLoadMetricDirectoryPushButton(int)));
   QButtonGroup* autoLoadFunctionalVolumeButtonGroup = new QButtonGroup(this);
   QObject::connect(autoLoadFunctionalVolumeButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotClusterAutoLoadFunctionalVolumeDirectoryPushButton(int)));

   std::vector<BrainModelSurface::SURFACE_TYPES> surfaceTypes;
   std::vector<QString> surfaceNames;
   BrainModelSurface::getSurfaceTypesAndNames(surfaceTypes, surfaceNames);

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS; i++) {
       this->clusterAutoLoadReplaceFilesCheckBox[i] = new QCheckBox("Replace Previously Loaded Files");
       QObject::connect(this->clusterAutoLoadReplaceFilesCheckBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readClusterAutoLoadControls()));

       QPushButton* autoLoadMetricDirectoryPushButton = new QPushButton("Metric Directory...");
       autoLoadMetricButtonGroup->addButton(autoLoadMetricDirectoryPushButton, i);
       this->clusterAutoLoadMetricDirectoryLineEdit[i] = new QLineEdit;
       this->clusterAutoLoadMetricDirectoryLineEdit[i]->setEnabled(false);

       QPushButton* autoLoadFunctionalVolumeDirectoryPushButton = new QPushButton("Functional Volume Directory...");
       autoLoadFunctionalVolumeButtonGroup->addButton(autoLoadFunctionalVolumeDirectoryPushButton, i);
       this->clusterAutoLoadFunctionalVolumeDirectoryLineEdit[i] = new QLineEdit;
       this->clusterAutoLoadFunctionalVolumeDirectoryLineEdit[i]->setEnabled(false);

       QLabel* paintColumnLabel = new QLabel("Paint Column");
       this->clusterAutoLoadPaintSelectionComboBox[i] =
               new GuiNodeAttributeColumnSelectionComboBox(GUI_NODE_FILE_TYPE_PAINT,
                                                           false, false, false);
       QObject::connect(this->clusterAutoLoadPaintSelectionComboBox[i], SIGNAL(itemSelected(int)),
                        this, SLOT(readClusterAutoLoadControls()));
       QLabel* displaySurfaceLabel = new QLabel("Display Surface");

       this->clusterAutoLoadDisplaySurfaceSelectionControl[i] =
               new GuiBrainModelSurfaceSelectionComboBox(surfaceTypes);
       QObject::connect(this->clusterAutoLoadDisplaySurfaceSelectionControl[i], SIGNAL(activated(int)),
                        this, SLOT(readClusterAutoLoadControls()));

       QLabel* lastNodeLabel = new QLabel("Last Node");
       clusterAutoLoadLastNodeNumberLabel[i] = new QLabel("                         ");

       this->clusterAutoLoadGroupBox[i] = new QGroupBox("Auto Load Cluster " + QString::number(i + 1));
       this->clusterAutoLoadGroupBox[i]->setCheckable(true);
       QObject::connect(this->clusterAutoLoadGroupBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readClusterAutoLoadControls()));
       QGridLayout* clusterAutoLoadLayout = new QGridLayout(clusterAutoLoadGroupBox[i]);
       clusterAutoLoadLayout->addWidget(clusterAutoLoadReplaceFilesCheckBox[i], 0, 0, 1, 2);
       clusterAutoLoadLayout->addWidget(autoLoadMetricDirectoryPushButton, 1, 0);
       clusterAutoLoadLayout->addWidget(this->clusterAutoLoadMetricDirectoryLineEdit[i], 1, 1);
       clusterAutoLoadLayout->addWidget(autoLoadFunctionalVolumeDirectoryPushButton, 2, 0);
       clusterAutoLoadLayout->addWidget(this->clusterAutoLoadFunctionalVolumeDirectoryLineEdit[i], 2, 1);
       clusterAutoLoadLayout->addWidget(paintColumnLabel, 3, 0);
       clusterAutoLoadLayout->addWidget(this->clusterAutoLoadPaintSelectionComboBox[i], 3, 1);
       clusterAutoLoadLayout->addWidget(displaySurfaceLabel, 4, 0);
       clusterAutoLoadLayout->addWidget(this->clusterAutoLoadDisplaySurfaceSelectionControl[i], 4, 1);
       clusterAutoLoadLayout->addWidget(lastNodeLabel, 5, 0);
       clusterAutoLoadLayout->addWidget(clusterAutoLoadLastNodeNumberLabel[i], 5, 1);
       autoLoadLayouts->addWidget(this->clusterAutoLoadGroupBox[i]);
   }

   autoLoadLayouts->addStretch();
   return autoLoadWidget;
}

/**
 * Called when cluster auto load metric directory button pressed.
 */
void
GuiConnectivityDialog::slotClusterAutoLoadMetricDirectoryPushButton(int indx)
{
   QString dirName = WuQFileDialog::getExistingDirectory(this,
                                   "Choose Metric Data File Directory",
                                   QDir::currentPath());
   if (dirName.isEmpty() == false) {
      this->clusterAutoLoadMetricDirectoryLineEdit[indx]->setText(dirName);
      readClusterAutoLoadControls();
   }
}

/**
 * Called when cluster auto load functional volumedirectory button pressed.
 */
void
GuiConnectivityDialog::slotClusterAutoLoadFunctionalVolumeDirectoryPushButton(int indx)
{
   QString dirName = WuQFileDialog::getExistingDirectory(this,
                                   "Choose Functional Volume Data File Directory",
                                   QDir::currentPath());
   if (dirName.isEmpty() == false) {
      this->clusterAutoLoadFunctionalVolumeDirectoryLineEdit[indx]->setText(dirName);
      readClusterAutoLoadControls();
   }
}

/**
 * Called to read cluster auto load controls.
 */
void
GuiConnectivityDialog::readClusterAutoLoadControls()
{
   if (updatingClusterAutoLoadControlsFlag) {
      return;
   }

   BrainSet* bs = theMainWindow->getBrainSet();
   BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS; i++) {
       BrainSetAutoLoaderFilePaintCluster* alc = autoLoaderManager->getClusterAutoLoader(i);
       alc->setAutoLoadDisplaySurface(
               this->clusterAutoLoadDisplaySurfaceSelectionControl[i]->getSelectedBrainModelSurface());
       alc->setAutoLoadDirectoryName(this->clusterAutoLoadMetricDirectoryLineEdit[i]->text());
       alc->setAutoLoadSecondaryDirectoryName(this->clusterAutoLoadFunctionalVolumeDirectoryLineEdit[i]->text());
       alc->setAutoLoadEnabled(this->clusterAutoLoadGroupBox[i]->isChecked());
       alc->setAutoLoadReplaceLastFileEnabled(this->clusterAutoLoadReplaceFilesCheckBox[i]->isChecked());
       alc->setPaintColumnNumber(this->clusterAutoLoadPaintSelectionComboBox[i]->currentIndex());
   }
}

/**
 * Update the cluster auto loadcontrols.
 */
void
GuiConnectivityDialog::updateClusterAutoLoadControls()
{
   updatingClusterAutoLoadControlsFlag = true;

   BrainSet* bs = theMainWindow->getBrainSet();
   BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_CLUSTER_AUTO_LOADERS; i++) {
       BrainSetAutoLoaderFilePaintCluster* alc = autoLoaderManager->getClusterAutoLoader(i);
       this->clusterAutoLoadDisplaySurfaceSelectionControl[i]->updateComboBox();
       this->clusterAutoLoadDisplaySurfaceSelectionControl[i]->setSelectedBrainModelSurface(
              alc->getAutoLoadDisplaySurface());
       this->clusterAutoLoadMetricDirectoryLineEdit[i]->setText(
                    alc->getAutoLoadDirectoryName());
       this->clusterAutoLoadFunctionalVolumeDirectoryLineEdit[i]->setText(
                    alc->getAutoLoadSecondaryDirectoryName());
       this->clusterAutoLoadGroupBox[i]->setChecked(alc->getAutoLoadEnabled());
       this->clusterAutoLoadReplaceFilesCheckBox[i]->setChecked(alc->getAutoLoadReplaceLastFileEnabled());

       this->clusterAutoLoadPaintSelectionComboBox[i]->updateComboBox();
       this->clusterAutoLoadPaintSelectionComboBox[i]->setCurrentIndex(alc->getPaintColumnNumber());

       const int lastNodeNumber = alc->getLastAutoLoadNodeNumber();
       QString nodeString = QString::number(lastNodeNumber);
       this->clusterAutoLoadLastNodeNumberLabel[i]->setText(nodeString);
   }

   updatingClusterAutoLoadControlsFlag = false;
}

/**
 * Create the functional volume loader widget.
 */
QWidget*
GuiConnectivityDialog::createFunctionalVolumeAutoLoaderWidget()
{
   //
   // functional volume auto load controls
   //
   QWidget* autoLoadWidget = new QWidget;
   QVBoxLayout* autoLoadLayouts = new QVBoxLayout(autoLoadWidget);

   QButtonGroup* autoLoadButtonGroup = new QButtonGroup(this);
   QObject::connect(autoLoadButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotFunctionalVolumeAutoLoadDirectoryPushButton(int)));

   std::vector<BrainModelSurface::SURFACE_TYPES> surfaceTypes;
   std::vector<QString> surfaceNames;
   BrainModelSurface::getSurfaceTypesAndNames(surfaceTypes, surfaceNames);

   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS; i++) {
       this->functionalVolumeAutoLoadReplaceFileCheckBox[i] = new QCheckBox("Replace Previously Loaded Functional Volume File");
       QObject::connect(this->functionalVolumeAutoLoadReplaceFileCheckBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readFunctionalVolumeAutoLoadControls()));

       QPushButton* autoLoadDirectoryPushButton = new QPushButton("Functional Volume Directory...");
       autoLoadButtonGroup->addButton(autoLoadDirectoryPushButton, i);
       this->functionalVolumeAutoLoadDirectoryLineEdit[i] = new QLineEdit;
       this->functionalVolumeAutoLoadDirectoryLineEdit[i]->setEnabled(false);

       QLabel* autoLoadVolumeLabel = new QLabel("Anatomy Volume");
       this->functionalVolumeAutoLoadVolumeSelectionControl[i] =
               new GuiVolumeFileSelectionComboBox(VolumeFile::VOLUME_TYPE_ANATOMY);
       QObject::connect(this->functionalVolumeAutoLoadVolumeSelectionControl[i], SIGNAL(activated(int)),
                        this, SLOT(readFunctionalVolumeAutoLoadControls()));

       QLabel* autoLoadVolumeSurfaceLabel = new QLabel("Volume Intersect Surface");
       this->functionalVolumeAutoLoadVolumeIntersectionSurfaceSelectionControl[i] =
               new GuiBrainModelSurfaceSelectionComboBox(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
       QObject::connect(this->functionalVolumeAutoLoadVolumeIntersectionSurfaceSelectionControl[i], SIGNAL(activated(int)),
                        this, SLOT(readFunctionalVolumeAutoLoadControls()));

       QLabel* lastFileLabel = new QLabel("Last File");
       functionalVolumeAutoLoadLastVoxelLabel[i] = new QLabel("                         ");

       this->functionalVolumeAutoLoadGroupBox[i] = new QGroupBox("Auto Load FunctionalVolume " + QString::number(i + 1));
       this->functionalVolumeAutoLoadGroupBox[i]->setCheckable(true);
       QObject::connect(this->functionalVolumeAutoLoadGroupBox[i], SIGNAL(toggled(bool)),
                        this, SLOT(readFunctionalVolumeAutoLoadControls()));
       QGridLayout* functionalVolumeAutoLoadLayout = new QGridLayout(functionalVolumeAutoLoadGroupBox[i]);
       functionalVolumeAutoLoadLayout->addWidget(functionalVolumeAutoLoadReplaceFileCheckBox[i], 0, 0, 1, 2);
       functionalVolumeAutoLoadLayout->addWidget(autoLoadDirectoryPushButton, 1, 0);
       functionalVolumeAutoLoadLayout->addWidget(this->functionalVolumeAutoLoadDirectoryLineEdit[i], 1, 1);
       functionalVolumeAutoLoadLayout->addWidget(autoLoadVolumeLabel, 3, 0);
       functionalVolumeAutoLoadLayout->addWidget(this->functionalVolumeAutoLoadVolumeSelectionControl[i], 3, 1);
       functionalVolumeAutoLoadLayout->addWidget(autoLoadVolumeSurfaceLabel, 4, 0);
       functionalVolumeAutoLoadLayout->addWidget(this->functionalVolumeAutoLoadVolumeIntersectionSurfaceSelectionControl[i], 4, 1);
       functionalVolumeAutoLoadLayout->addWidget(lastFileLabel, 5, 0);
       functionalVolumeAutoLoadLayout->addWidget(functionalVolumeAutoLoadLastVoxelLabel[i], 5, 1);
       autoLoadLayouts->addWidget(this->functionalVolumeAutoLoadGroupBox[i]);
   }

   autoLoadLayouts->addStretch();
   return autoLoadWidget;
}

/**
 * Called when functional volume auto load directory button pressed.
 */
void
GuiConnectivityDialog::slotFunctionalVolumeAutoLoadDirectoryPushButton(int indx)
{
   QString dirName = WuQFileDialog::getExistingDirectory(this,
                                   "Choose Functional Volume File Directory",
                                   QDir::currentPath());
   if (dirName.isEmpty() == false) {
      this->functionalVolumeAutoLoadDirectoryLineEdit[indx]->setText(dirName);
      readFunctionalVolumeAutoLoadControls();
   }
}

/**
 * Called to read functional volume auto load controls.
 */
void
GuiConnectivityDialog::readFunctionalVolumeAutoLoadControls()
{
   if (updatingFunctionalVolumeAutoLoadControlsFlag) {
      return;
   }

   BrainSet* bs = theMainWindow->getBrainSet();
   BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS; i++) {
       BrainSetAutoLoaderFileFunctionalVolume* alm = autoLoaderManager->getFunctionalVolumeAutoLoader(i);
       alm->setAutoLoadAnatomyVolumeFile(
               this->functionalVolumeAutoLoadVolumeSelectionControl[i]->getSelectedVolumeFile());
       alm->setAutoLoadVolumeIntersectionSurface(
               this->functionalVolumeAutoLoadVolumeIntersectionSurfaceSelectionControl[i]->getSelectedBrainModelSurface());
       alm->setAutoLoadDirectoryName(this->functionalVolumeAutoLoadDirectoryLineEdit[i]->text());
       alm->setAutoLoadEnabled(this->functionalVolumeAutoLoadGroupBox[i]->isChecked());
       alm->setAutoLoadReplaceLastFileEnabled(this->functionalVolumeAutoLoadReplaceFileCheckBox[i]->isChecked());
   }
}

/**
 * Update the functional volume auto loadcontrols.
 */
void
GuiConnectivityDialog::updateFunctionalVolumeAutoLoadControls()
{
   updatingFunctionalVolumeAutoLoadControlsFlag = true;

   BrainSet* bs = theMainWindow->getBrainSet();
   BrainSetAutoLoaderManager* autoLoaderManager = bs->getAutoLoaderManager();
   for (int i = 0; i < BrainSetAutoLoaderManager::NUMBER_OF_FUNCTIONAL_VOLUME_AUTO_LOADERS; i++) {
       BrainSetAutoLoaderFileFunctionalVolume* alm = autoLoaderManager->getFunctionalVolumeAutoLoader(i);
       this->functionalVolumeAutoLoadVolumeSelectionControl[i]->updateComboBox();
       this->functionalVolumeAutoLoadVolumeSelectionControl[i]->setSelectedVolumeFile(
                                     alm->getAutoLoadAnatomyVolumeFile());
       this->functionalVolumeAutoLoadVolumeIntersectionSurfaceSelectionControl[i]->updateComboBox();
       this->functionalVolumeAutoLoadVolumeIntersectionSurfaceSelectionControl[i]->setSelectedBrainModelSurface(
                      alm->getAutoLoadVolumeIntersectionSurface());
       this->functionalVolumeAutoLoadDirectoryLineEdit[i]->setText(
                    alm->getAutoLoadDirectoryName());
       this->functionalVolumeAutoLoadGroupBox[i]->setChecked(alm->getAutoLoadEnabled());
       this->functionalVolumeAutoLoadReplaceFileCheckBox[i]->setChecked(alm->getAutoLoadReplaceLastFileEnabled());

       VoxelIJK lastVoxel = alm->getLastAutoLoadAnatomyVoxelIndices();
       QString indicesString = "("
                             + QString::number(lastVoxel.getI()) + ", "
                             + QString::number(lastVoxel.getJ()) + ", "
                             + QString::number(lastVoxel.getK()) + ")";
       this->functionalVolumeAutoLoadLastVoxelLabel[i]->setText(indicesString);
   }

   updatingFunctionalVolumeAutoLoadControlsFlag = false;
}

