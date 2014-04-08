
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

#include <sstream>

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTextBrowser>

#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "GuiMainWindow.h"
#include "GuiVolumeTopologyReportDialog.h"
#include "QtUtilities.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiVolumeTopologyReportDialog::GuiVolumeTopologyReportDialog(QWidget* parent)
   : WuQDialog(parent)
{
   setWindowTitle("Topology Error Report");
   
   defaultLabelValue = "                              ";
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(3);
   
   //
   // number of cavities items
   //
   QLabel* cavitiesLabel = new QLabel("Number of Cavities ");
   numberOfCavitiesLabel = new QLabel(defaultLabelValue);
    
   //
   // number of objects items
   //
   QLabel* objectsLabel = new QLabel("Number of Objects ");
   numberOfObjectsLabel = new QLabel(defaultLabelValue);
    
   //
   // euler count items
   //
   QLabel* eulerLabel = new QLabel("Euler Count");
   eulerCountLabel = new QLabel(defaultLabelValue);
    
   //
   // number of holes items
   //
   QLabel* holesLabel = new QLabel("Number of Handles ");
   numberOfHolesLabel = new QLabel(defaultLabelValue);
    
   //
   // Group box for results
   //
   QGroupBox* reportGroupBox = new QGroupBox("Results");
   dialogLayout->addWidget(reportGroupBox);
   QGridLayout* reportLayout = new QGridLayout(reportGroupBox);
   reportLayout->addWidget(cavitiesLabel, 0, 0);
   reportLayout->addWidget(numberOfCavitiesLabel, 0, 1);
   reportLayout->addWidget(objectsLabel, 1, 0);
   reportLayout->addWidget(numberOfObjectsLabel, 1, 1);
   reportLayout->addWidget(eulerLabel, 2, 0);
   reportLayout->addWidget(eulerCountLabel, 2, 1);
   reportLayout->addWidget(holesLabel, 3, 0);
   reportLayout->addWidget(numberOfHolesLabel, 3, 1);

   //
   // Info text browser
   //
   infoTextBrowser = new QTextBrowser;   
   QString infoText = 
      "<html>"
      "<B>Number of Cavities</B><BR>"
      "Number of cavities is the number of contiguous enclosed regions in the  "
      "with voxels equal to zero.  "
      "<P>"
      "<B>Number of Objects</B><BR>"
      "Number of objects is the number of disjoint (unconnected) pieces of voxels.  "
      "If there is more than one object, all but the largest object can be removed "
      "using Volume Menu:Segmentation:Remove Islands."
      "<P>"
      "<B>Euler Count</B><BR>"
      "An Euler Count is a formula that tells us if the voxels will form a closed surface "
      "after reconstruction.  A 3D closed surface will have an Euler count of 2.  More "
      "information about the Euler count is available at "
      "http://mathworld.wolfram.com/EulerCharacteristic.html."
      "<P>"
      "<B>Number of Handles</B><BR>"
      "The number of handles is derived using the Euler count.  If there is more than one "
      "object or cavities, the number of holes may be incorrect.  "
      "The formula used is Nodes - Links + Triangles = 2 - (2 * number_of_holes).  "
      "The number of holes is also known as the genus.  See "
      "http://mathworld.wolfram.com/Genus.html for more information.  "
      "<P>"
      "</html>";
   infoTextBrowser->setHtml(infoText);
   
   //
   // Help information box
   //
   QGroupBox* helpBox = new QGroupBox("Information");
   dialogLayout->addWidget(helpBox);
   QVBoxLayout* helpLayout = new QVBoxLayout(helpBox);
   helpLayout->addWidget(infoTextBrowser);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonLayout = new QHBoxLayout;
   buttonLayout->setSpacing(5);
   dialogLayout->addLayout(buttonLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   // close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseButton()));
                    
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);   
}

/**
 * destructor.
 */
GuiVolumeTopologyReportDialog::~GuiVolumeTopologyReportDialog()
{
}

/**
 * show the dialog (overrides QDialog's show()).
 */
void 
GuiVolumeTopologyReportDialog::show()
{
   clearDialog();
   WuQDialog::show();
   slotApplyButton();
}

/**
 * clear the dialog's data.
 */
void 
GuiVolumeTopologyReportDialog::clearDialog()
{
   numberOfCavitiesLabel->setText(defaultLabelValue);
   numberOfObjectsLabel->setText(defaultLabelValue);
   eulerCountLabel->setText(defaultLabelValue);
   numberOfHolesLabel->setText(defaultLabelValue);
}
      
/**
 * called when apply button pressed.
 */
void 
GuiVolumeTopologyReportDialog::slotApplyButton()
{
   clearDialog();
   
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv == NULL) {
      return;
   }
   const VolumeFile* vf = bmv->getSelectedVolumeSegmentationFile();
   if (vf == NULL) {
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
/*
   const int numCavities = vf->getNumberOfSegmentationCavities();
   const int numObjects  = vf->getNumberOfSegmentationObjects();
   const int eulerCount = vf->getEulerNumberForSegmentationVolume();
   const int numHoles = numObjects + numCavities - eulerCount; //(eulerCount - 2) / (-2);
*/   
   int numObjects,
       numberOfCavities,
       numberOfHoles,
       eulerCount;
   vf->getSegmentationTopologyInformation(numObjects,
                                      numberOfCavities,
                                      numberOfHoles,
                                      eulerCount);
   setLabel(numberOfCavitiesLabel, numberOfCavities, 0);
   setLabel(numberOfObjectsLabel, numObjects, 1);
   setLabel(eulerCountLabel, eulerCount, 2);
   setLabel(numberOfHolesLabel, numberOfHoles, 0);
   
   QApplication::restoreOverrideCursor();
   QApplication::beep();
}

/**
 * set the label with green if the value is correct, else red.
 */
void
GuiVolumeTopologyReportDialog::setLabel(QLabel* label, const int value, const int correctValue)
{
   std::ostringstream str;
   str << "<font color=";
   if (value == correctValue) {
      str << "green>";
   }
   else {
      str << "red>";
   }
   str << value
       << " </font>";
   
   if (value != correctValue) {
      str << "<font color=black>  (should be "
          << correctValue
          << ")</font>";
   }
   
   label->setText(str.str().c_str());
}

/**
 * called when close button pressed.
 */
void 
GuiVolumeTopologyReportDialog::slotCloseButton()
{
   clearDialog();
   QDialog::close();
}
