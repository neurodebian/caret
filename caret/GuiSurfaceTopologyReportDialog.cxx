
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
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTextBrowser>

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "BrainSetNodeAttribute.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiMainWindow.h"
#include "GuiSurfaceTopologyReportDialog.h"
#include "QtUtilities.h"
#include "TopologyFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiSurfaceTopologyReportDialog::GuiSurfaceTopologyReportDialog(QWidget* parent)
   : QtDialog(parent, false)
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
   // Show non-manifold nodes check box
   //
   showNonManifoldNodesCheckBox = new QCheckBox("Show Non-manifold Nodes");
   showNonManifoldNodesCheckBox->setChecked(true);
   
   //
   // Show straggler tiles check box
   //
   showStragglerTilesCheckBox = new QCheckBox("Show Straggler Tiles");
   showStragglerTilesCheckBox->setChecked(true);
   
   //
   // Degenerate edges check box
   //
   degenEdgesCheckBox = new QCheckBox("Show Degenerate Links");
   degenEdgesCheckBox->setChecked(true);
   
   //
   // Query group box
   //
   QGroupBox* queryBox = new QGroupBox("Query");
   dialogLayout->addWidget(queryBox);
   QVBoxLayout* queryLayout = new QVBoxLayout(queryBox);
   queryLayout->addWidget(showNonManifoldNodesCheckBox);
   queryLayout->addWidget(showStragglerTilesCheckBox);
   queryLayout->addWidget(degenEdgesCheckBox);
   
   //
   // Group box for results
   //
   QGroupBox* reportBox = new QGroupBox("Results");
   dialogLayout->addWidget(reportBox);
   QGridLayout* reportLayout = new QGridLayout(reportBox);
   int rowNum = 0;
   
   //
   // toplogy type items
   //
   reportLayout->addWidget(new QLabel("Surface Type "), rowNum, 0);
   surfaceTypeLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(surfaceTypeLabel, rowNum, 1);
   rowNum++;
   
   //
   // non-manfifold nodes items
   //
   reportLayout->addWidget(new QLabel("Non-manifold Nodes "), rowNum, 0);
   nonManifoldNodesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(nonManifoldNodesLabel, rowNum, 1);
   rowNum++;
    
   //
   // straggler tiles items
   //
   reportLayout->addWidget(new QLabel("Straggler Tiles "), rowNum, 0);
   stragglerTilesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(stragglerTilesLabel, rowNum, 1);
   rowNum++;
   
   //
   // degenerate edges items
   //
   reportLayout->addWidget(new QLabel("Degenerate Links "), rowNum, 0);
   degenEdgesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(degenEdgesLabel, rowNum, 1);
   rowNum++;
    
   //
   // number of objects items
   //
   reportLayout->addWidget(new QLabel("Number of Objects "), rowNum, 0);
   numberOfObjectsLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(numberOfObjectsLabel, rowNum, 1);
   rowNum++;
    
   //
   // euler count items
   //
   reportLayout->addWidget(new QLabel("Euler Count"), rowNum, 0);
   eulerCountLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(eulerCountLabel, rowNum, 1);
   rowNum++;
   
   //
   // number of holes items
   //
   reportLayout->addWidget(new QLabel("Number of Handles "), rowNum, 0);
   numberOfHolesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(numberOfHolesLabel, rowNum, 1);
   rowNum++;
   
   //
   // number of nodes items
   //
   reportLayout->addWidget(new QLabel("Total Nodes "), rowNum, 0);
   numberOfNodesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(numberOfNodesLabel, rowNum, 1);
   rowNum++;
   
   //
   // number of connected nodes items
   //
   reportLayout->addWidget(new QLabel("Connected Nodes "), rowNum, 0);
   numberOfConnectedNodesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(numberOfConnectedNodesLabel, rowNum, 1);
   rowNum++;
    
   //
   // number of edges items
   //
   reportLayout->addWidget(new QLabel("Number of Edges "), rowNum, 0);
   numberOfEdgesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(numberOfEdgesLabel, rowNum, 1);
   rowNum++;
    
   //
   // number of tiles items
   //
   reportLayout->addWidget(new QLabel("Number of Tiles "), rowNum, 0);
   numberOfTilesLabel = new QLabel(defaultLabelValue);
   reportLayout->addWidget(numberOfTilesLabel, rowNum, 1);
   rowNum++;
    
   //
   // Help information box
   //
   QGroupBox* helpBox = new QGroupBox("Information");
   dialogLayout->addWidget(helpBox);
   QVBoxLayout* helpLayout = new QVBoxLayout(helpBox);
   
   //
   // Info text browser
   //
   infoTextBrowser = new QTextBrowser;
   helpLayout->addWidget(infoTextBrowser);
   
   QString infoText = 
      "<html>"
      "<B>Non-manifold Nodes</B><BR>"
      "Non-manifold nodes are nodes that have at least 4 boundary edges.  "
      "A simple example would be two triangles that are connected at only a single node.  "
      "In Caret terminology we might refer to this as a bowtie."
      "<P>"
      "<B>Straggler Tiles</B><BR>"
      "Straggler tiles are tiles that are connected to the surface at only a single node "
      "in the tile and can cause problems during multi-resolution morphing.  Stragglers "
      "can be removed by using Surface Menu:Topology:Remove Corner And Straggler Tiles.  "
      "<P>"
      "<B>Degenerate Links</B><BR>"
      "Degenerate links are links that are used by three or more triangles.  "
      "Interior links use two triangles and boundary links use one triangle.  "
      "<P>"
      "<B>Number of Objects</B><BR>"
      "Number of objects is the number of disjoint (unconnected) pieces of surface.  "
      "If there is more than one object, all but the largest object can be removed "
      "using Surface Menu:Topology:Remove Islands."
      "<P>"
      "<B>Euler Count</B><BR>"
      "An Euler Count is a formula that tells us if the surface is properly made up of "
      "triangles and nodes.  A 3D closed surface will have an Euler count of 2.  An flat "
      "surface will have an Euler count of 1.  The formula for an Euler count is "
      "Nodes - Links + Triangles.  More information about the Euler count is available "
      "at http://mathworld.wolfram.com/EulerCharacteristic.html."
      "<P>"
      "<B>Number of Handles</B><BR>"
      "The number of handles is derived using the Euler count.  If there is more than one "
      "object or there are degenerate edges, the number of handles may be incorrect.  "
      "The number of handles (holes) is also known as the genus.  See "
      "http://mathworld.wolfram.com/Genus.html for more information.  "
      "<P>"
      "</html>";
   infoTextBrowser->setHtml(infoText);
   
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
GuiSurfaceTopologyReportDialog::~GuiSurfaceTopologyReportDialog()
{
}

/**
 * show the dialog (overrides QDialog's show()).
 */
void 
GuiSurfaceTopologyReportDialog::show()
{
   clearDialog();
   QtDialog::show();
   slotApplyButton();
}

/**
 * clear the dialog's data.
 */
void 
GuiSurfaceTopologyReportDialog::clearDialog()
{
   surfaceTypeLabel->setText(defaultLabelValue);
   nonManifoldNodesLabel->setText(defaultLabelValue);
   stragglerTilesLabel->setText(defaultLabelValue);
   degenEdgesLabel->setText(defaultLabelValue);
   numberOfObjectsLabel->setText(defaultLabelValue);
   eulerCountLabel->setText(defaultLabelValue);
   numberOfHolesLabel->setText(defaultLabelValue);
   numberOfNodesLabel->setText(defaultLabelValue);
   numberOfConnectedNodesLabel->setText(defaultLabelValue);
   numberOfEdgesLabel->setText(defaultLabelValue);
   numberOfTilesLabel->setText(defaultLabelValue);
}
      
/**
 * called when apply button pressed.
 */
void 
GuiSurfaceTopologyReportDialog::slotApplyButton()
{
   clearDialog();
   
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   const TopologyFile* tf = bms->getTopologyFile();
   
   //
   // Determine if the surface is 2D or 3D
   //
   bool twoDimFlag = true;
   const CoordinateFile* cf = bms->getCoordinateFile();
   const int numCoords = cf->getNumberOfCoordinates();
   for (int i = 0; i < numCoords; i++) {
      const float* xyz = cf->getCoordinate(i);
      if (xyz[2] != 0.0) {
         twoDimFlag = false;
         break;
      }
   }
      
   std::vector<int> nonManifoldNodes;
   tf->getNonManifoldNodes(nonManifoldNodes);
   
   int numDegenerateEdges;
   std::vector<int> nodesUsedByDegenerateEdges;
   tf->getDegenerateEdges(numDegenerateEdges, nodesUsedByDegenerateEdges);
   
   std::vector<int> stragglerTiles;
   tf->findCornerTiles(2, stragglerTiles);
   
   int numFaces, numVertices, numEdges, eulerCount, numHoles, numObjects;
   tf->getEulerCount(twoDimFlag, numFaces, numVertices, numEdges, eulerCount, 
                     numHoles, numObjects);
   
   int correctEulerCount = 2;
   if (twoDimFlag) {
      correctEulerCount = 1;
      surfaceTypeLabel->setText("2D");
   }
   else {
      surfaceTypeLabel->setText("3D");
   }
   setLabel(nonManifoldNodesLabel, nonManifoldNodes.size(), 0);
   setLabel(stragglerTilesLabel, stragglerTiles.size(), 0);
   setLabel(degenEdgesLabel, numDegenerateEdges, 0);
   setLabel(numberOfObjectsLabel, numObjects, 1);
   setLabel(eulerCountLabel, eulerCount, correctEulerCount);
   if (numObjects != 1) {
      setLabel(numberOfHolesLabel, numHoles, 0, "(Multiple Objects)");
   }
   else {
      setLabel(numberOfHolesLabel, numHoles, 0);
   }
   numberOfNodesLabel->setText(QString::number(bms->getNumberOfNodes()));
   numberOfConnectedNodesLabel->setText(QString::number(numVertices));
   numberOfEdgesLabel->setText(QString::number(numEdges));
   numberOfTilesLabel->setText(QString::number(numFaces));
   
   if (showNonManifoldNodesCheckBox->isChecked() ||
       degenEdgesCheckBox->isChecked() ||
       showStragglerTilesCheckBox->isChecked()) {
         theMainWindow->getBrainSet()->clearNodeHighlightSymbols();
   }
   if (showNonManifoldNodesCheckBox->isChecked()) {
      for (unsigned int i = 0; i < nonManifoldNodes.size(); i++) {
         BrainSetNodeAttribute* bna = theMainWindow->getBrainSet()->getNodeAttributes(nonManifoldNodes[i]);
         bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
      }
   }
   if (showStragglerTilesCheckBox->isChecked()) {
      for (unsigned int i = 0; i < stragglerTiles.size(); i++) {
         int n1, n2, n3;
         tf->getTile(stragglerTiles[i], n1, n2, n3);
         BrainSetNodeAttribute* bna = theMainWindow->getBrainSet()->getNodeAttributes(n1);
         bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
         bna = theMainWindow->getBrainSet()->getNodeAttributes(n2);
         bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
         bna = theMainWindow->getBrainSet()->getNodeAttributes(n3);
         bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);         
      }
   }
   
   if (degenEdgesCheckBox->isChecked()) {
      for (unsigned int i = 0; i < nodesUsedByDegenerateEdges.size(); i++) {
         BrainSetNodeAttribute* bna = theMainWindow->getBrainSet()->getNodeAttributes(nodesUsedByDegenerateEdges[i]);
         bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
      }
   }
   
   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
   QApplication::beep();
}

/**
 * set the label with green if the value is correct, else red.
 */
void
GuiSurfaceTopologyReportDialog::setLabel(QLabel* label, const int value, 
                                         const int correctValue,
                                         const QString& message)
{
   std::ostringstream str;
   str << "<font color=";
   if ((value == correctValue) && (message.isEmpty())) {
      str << "green>";
   }
   else {
      str << "red>";
   }
   str << value;
   
   if (message.isEmpty() == false) {
      str << " " << message.toAscii().constData();
   }
   
   str << "</font>";
   
   if ((value != correctValue) && (message.isEmpty())) {
      str << "<font color=black> (should be "
          << correctValue
          << ")</font>";
   }
   
   label->setText(str.str().c_str());
}

/**
 * called when close button pressed.
 */
void 
GuiSurfaceTopologyReportDialog::slotCloseButton()
{
   clearDialog();
   QDialog::close();
}
