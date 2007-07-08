
#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "BrainSet.h"
#include "GuiAlignSurfaceToStandardOrientationDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiMainWindow.h"
#include "GuiMessageBox.h"
#include "QtUtilities.h"
#include "global_variables.h"

/**
 * Constructor
 */
GuiAlignSurfaceToStandardOrientationDialog::GuiAlignSurfaceToStandardOrientationDialog(
                                                                        QWidget* parent)
   : QtDialog(parent, false, 0)
{
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle("Align Surface(s) to Standard Orientation");
   
   medialTipNode  = -1;
   ventralTipNode = -1;
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
  
   //
   // Grid for ventral and dorsal-medial tips
   //
   QGroupBox* centralSulcusGroupBox = new QGroupBox("Central Sulcus Tips");
   dialogLayout->addWidget(centralSulcusGroupBox);
   QGridLayout* centralSulcusGridLayout = new QGridLayout(centralSulcusGroupBox);
   
   //
   // Column headers
   //
   centralSulcusGridLayout->addWidget(new QLabel("Central Sulcus Tip"), 0, 0);
   centralSulcusGridLayout->addWidget(new QLabel("Mouse"), 0, 1);
   centralSulcusGridLayout->addWidget(new QLabel("X"), 0, 2);
   centralSulcusGridLayout->addWidget(new QLabel("Y"), 0, 3);
   
   //
   // Ventral tip
   // 
   centralSulcusGridLayout->addWidget(new QLabel("Ventral"), 1, 0);
   centralSulcusGridLayout->addWidget(new QLabel("Left Click"), 1, 1);
   ventralTipXLabel = new QLabel("            ");
   ventralTipYLabel = new QLabel("            ");
   centralSulcusGridLayout->addWidget(ventralTipXLabel, 1, 2);
   centralSulcusGridLayout->addWidget(ventralTipYLabel, 1, 3);
   
   //
   // Medial tip
   // 
   centralSulcusGridLayout->addWidget(new QLabel("Dorsal-Medial"), 2, 0);
   centralSulcusGridLayout->addWidget(new QLabel("Shift Left Click"), 2, 1);
   medialTipXLabel = new QLabel("            ");
   medialTipYLabel = new QLabel("            ");
   centralSulcusGridLayout->addWidget(medialTipXLabel, 2, 2);
   centralSulcusGridLayout->addWidget(medialTipYLabel, 2, 3);
   
   //
   // Surface scaling box
   //
   QGroupBox* scaleGroupBox = new QGroupBox("Surface Scaling");
   QVBoxLayout* scaleGroupBoxLayout = new QVBoxLayout(scaleGroupBox);
   dialogLayout->addWidget(scaleGroupBox);
   
   //
   // Scale to fiducial area check box
   //
   scaleSurfaceCheckBox = new QCheckBox("Scale to Fiducial Area");
   scaleGroupBoxLayout->addWidget(scaleSurfaceCheckBox);
   scaleSurfaceCheckBox->setChecked(true);
   
   //
   // vertical group box for spherical surface
   //
   QGroupBox* sphereGroupBox = new QGroupBox("Spherical Surface");
   QVBoxLayout* sphereGroupBoxLayout = new QVBoxLayout(sphereGroupBox);
   dialogLayout->addWidget(sphereGroupBox);
   
   //
   // Align sphere check box
   //
   alignSphereCheckBox = new QCheckBox("Align Sphere");
   sphereGroupBoxLayout->addWidget(alignSphereCheckBox);
   alignSphereCheckBox->setChecked(true);
   QObject::connect(alignSphereCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotAlignSphereCheckBox(bool)));
       
   //
   // Generate Lat/Lon check box
   //
   sphereLatLonCheckBox = new QCheckBox("Generate Lat/Lon on Sphere");
   sphereGroupBoxLayout->addWidget(sphereLatLonCheckBox);
   sphereLatLonCheckBox->setChecked(true);
   
   //
   // Brain Mode surface combo box
   //
   sphereSurfaceComboBox = new GuiBrainModelSelectionComboBox(false, true, false, "",
                                                              0,
                                                              "sphereSurfaceComboBox");
   sphereGroupBoxLayout->addWidget(sphereSurfaceComboBox);
   sphereSurfaceComboBox->setSelectedBrainModelToSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyPushButton()));
                    
   //
   // Reset button
   //
   QPushButton* resetButton = new QPushButton("Reset");
   resetButton->setAutoDefault(false);
   buttonsLayout->addWidget(resetButton);
   QObject::connect(resetButton, SIGNAL(clicked()),
                    this, SLOT(slotResetPushButton()));
                    
   //
   // Close button 
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   
   QtUtilities::makeButtonsSameSize(applyButton, resetButton, closeButton);
   
   //
   // initialize some dialog items
   //
   slotAlignSphereCheckBox(alignSphereCheckBox->isChecked());
   slotResetPushButton();

   //
   // Register dialog with main window
   //
   theMainWindow->setAlignSurfaceToStandardOrientationDialog(this);
}

/**
 * Destructor
 */
GuiAlignSurfaceToStandardOrientationDialog::~GuiAlignSurfaceToStandardOrientationDialog()
{
   //
   // Unregister dialog with main window
   //
   theMainWindow->setAlignSurfaceToStandardOrientationDialog(NULL);
}

/**
 * called when align sphere checkbox is pressed
 */
void
GuiAlignSurfaceToStandardOrientationDialog::slotAlignSphereCheckBox(bool onoff)
{
   sphereLatLonCheckBox->setEnabled(onoff);
   sphereSurfaceComboBox->setEnabled(onoff);
}

/**
 * called when apply pushbutton is pressed.
 */
void 
GuiAlignSurfaceToStandardOrientationDialog::slotApplyPushButton()
{
   if ((medialTipNode < 0) || (ventralTipNode < 0)) {
      GuiMessageBox::critical(this, "Missing Nodes",
         "You must select both medial-dorsal and ventral node points of central sulcus", "OK");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   const bool scaleSurface = scaleSurfaceCheckBox->isChecked();
   
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      bms->alignToStandardOrientation(ventralTipNode, medialTipNode, false, scaleSurface);
      
      if (alignSphereCheckBox->isChecked()) {
         bms = sphereSurfaceComboBox->getSelectedBrainModelSurface();
         if (bms != NULL) {
            bms->alignToStandardOrientation(ventralTipNode, medialTipNode, 
                                            sphereLatLonCheckBox->isChecked(),
                                            scaleSurface);
         }
      }
      
      theMainWindow->getBrainSet()->applyAllProjectedFiles();
      GuiBrainModelOpenGL::updateAllGL(NULL);
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * called when reset pushbutton is pressed.
 */
void 
GuiAlignSurfaceToStandardOrientationDialog::slotResetPushButton()
{
   sphereSurfaceComboBox->updateComboBox();
   setMedialTipNode(-1);
   setVentralTipNode(-1);
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                  GuiBrainModelOpenGL::MOUSE_MODE_ALIGN_STANDARD_ORIENTATION);
}

/**
 * called to set medial tip node.
 */
void 
GuiAlignSurfaceToStandardOrientationDialog::setMedialTipNode(const int node)
{
   medialTipNode = node;
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   QString xValue, yValue;
   if (bms != NULL) {
      int numNodes = bms->getNumberOfNodes();
      if ((node >= 0) && (node < numNodes)) {
         CoordinateFile* cf = bms->getCoordinateFile();
         float xyz[3];
         cf->getCoordinate(node, xyz);
         xValue = QString::number(xyz[0], 'f', 1);
         yValue = QString::number(xyz[1], 'f', 1);
      }
   }
   medialTipXLabel->setText(xValue);
   medialTipYLabel->setText(yValue);
}

/**
 * called to set medial tip node.
 */
void 
GuiAlignSurfaceToStandardOrientationDialog::setVentralTipNode(const int node)
{
   ventralTipNode = node;
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   QString xValue, yValue;
   if (bms != NULL) {
      int numNodes = bms->getNumberOfNodes();
      if ((node >= 0) && (node < numNodes)) {
         CoordinateFile* cf = bms->getCoordinateFile();
         float xyz[3];
         cf->getCoordinate(node, xyz);
         xValue = QString::number(xyz[0], 'f', 1);
         yValue = QString::number(xyz[1], 'f', 1);
      }
   }
   ventralTipXLabel->setText(xValue);
   ventralTipYLabel->setText(yValue);
}
      
