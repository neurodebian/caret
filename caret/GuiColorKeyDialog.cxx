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

#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLayout>
#include <QMouseEvent>
#include <QPalette>
#include <QPushButton>
#include <QScrollArea>

#include "AreaColorFile.h"
#include "ArealEstimationFile.h"
#include "BorderColorFile.h"
#include "BorderProjectionFile.h"
#include "BrainSet.h"
#include "BrainModelBorderSet.h"
#include "BrainModelIdentification.h"
#include "BrainModelSurface.h"
#include "BrainModelVolume.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "ColorFile.h"
#include "DisplaySettingsArealEstimation.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsPaint.h"
#include "DisplaySettingsProbabilisticAtlas.h"
#include "DisplaySettingsScene.h"
#include "FociColorFile.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiColorKeyDialog.h"
#include "GuiIdentifyDialog.h"
#include "GuiMainWindow.h"
#include "NameIndexSort.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "StringUtilities.h"
#include "StudyMetaDataFile.h"
#include "VocabularyFile.h"
#include "VolumeFile.h"
#include "global_variables.h"

/**
 * constructor.
 */
GuiColorKeyDialog::GuiColorKeyDialog(QWidget* parent,
                                     const COLOR_KEY colorKeyIn)
   : WuQDialog(parent)
{
   colorKey = colorKeyIn;

   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
   buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(false);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));
   QPushButton* updatePushButton = new QPushButton("Update");
   buttonBox->addButton(updatePushButton,
                        QDialogButtonBox::ActionRole);
   QObject::connect(updatePushButton, SIGNAL(clicked()),
                    this, SLOT(updateDialog()));
                    
   QPushButton* clearPushButton = new QPushButton("Clear Highlighting");
   clearPushButton->setAutoDefault(false);
   QObject::connect(clearPushButton, SIGNAL(clicked()),
                    this, SLOT(slotClearHighlighting()));

   //
   // Place the names and a color swatch into the layout
   //
   QWidget* gridWidget = new QWidget;
   colorGridLayout = new QGridLayout(gridWidget);

   //
   // Place color key into a scroll view
   //
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidget(gridWidget);
   scrollArea->setWidgetResizable(true);
   
   //
   // Add the scroll area to the dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);  
   dialogLayout->addWidget(scrollArea);
   dialogLayout->addWidget(clearPushButton);
   dialogLayout->addWidget(buttonBox);
}

/**
 * destructor.
 */
GuiColorKeyDialog::~GuiColorKeyDialog()
{
}

/// get the title for the window
QString 
GuiColorKeyDialog::getTitleOfColorKey(const COLOR_KEY cc)
{
   QString title;
   
   switch (cc) {
      case COLOR_KEY_AREAL_ESTIMATION:
         title = "Areal Estimation Color Key";
         break;
      case COLOR_KEY_BORDERS:
         title = "Border Color Key";
         break;
      case COLOR_KEY_CELLS:
         title = "Cell Color Key";
         break;
      case COLOR_KEY_FOCI:
         title = "Foci Color Key";
         break;
      case COLOR_KEY_PAINT:
         title = "Paint Color Key";
         break;
      case COLOR_KEY_PROBABILISTIC_ATLAS:
         title = "Probabilistic Atlas Color Key";
         break;
      case COLOR_KEY_VOLUME_PAINT:
         title = "Volume Paint Color Key";
         break;
      case COLOR_KEY_VOLUME_PROBABILISTIC_ATLAS:
         title = "Volume Probabilistic Atlas Color Key";
         break;
   }
   
   return title;
}

/**
 * update the dialog.
 */
void 
GuiColorKeyDialog::updateDialog()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   
   VocabularyFile* vf = bs->getVocabularyFile();
   
   nameTable.clear();
   
   //
   // Get the color key for the data type displayed
   //
   switch (colorKey) {
      case COLOR_KEY_AREAL_ESTIMATION:
         updateArealEstimationKey(bs, nameTable);
         break;
      case COLOR_KEY_BORDERS:
         updateBorderKey(bs, nameTable);
         break;
      case COLOR_KEY_CELLS:
         updateCellKey(bs, nameTable);
         break;
      case COLOR_KEY_FOCI:
         updateFociKey(bs, nameTable);
         break;
      case COLOR_KEY_PAINT:
         updatePaintKey(bs, nameTable);
         break;
      case COLOR_KEY_PROBABILISTIC_ATLAS:
         updateProbAtlasKey(bs, nameTable);
         break;
      case COLOR_KEY_VOLUME_PAINT:
         updateVolumePaintKey(bs, nameTable);
         break;
      case COLOR_KEY_VOLUME_PROBABILISTIC_ATLAS:
         updateVolumeProbAtlasKey(bs, nameTable);
         break;
   }

   //
   // Set the window title
   //
   setWindowTitle(getTitleOfColorKey(colorKey));
   
   //
   // Sort and remove any duplicates
   //
   std::sort(nameTable.begin(), nameTable.end());
   std::unique(nameTable.begin(), nameTable.end());

   //
   // Load color labels
   //   
   const int numTableNames = static_cast<int>(nameTable.size());
   for (int i = 0; i < numTableNames; i++) {
      //
      // Use existing label or create new label if it is needed
      //
      GuiColorKeyLabel* colorLabel = NULL;
      GuiColorKeyLabel* nameLabel  = NULL;
      if (i < static_cast<int>(colorLabelsInColorGrid.size())) {
         colorLabel = colorLabelsInColorGrid[i];
         nameLabel  = nameLabelsInColorGrid[i];
      }
      else {
         //
         // Create new labels
         //
         colorLabel = new GuiColorKeyLabel;
         QObject::connect(colorLabel, SIGNAL(signalNameTableIndex(const int)),
                          this, SLOT(slotColorLabelClicked(const int)));
         nameLabel  = new GuiColorKeyLabel;
         QObject::connect(nameLabel, SIGNAL(signalShowAreaName(const QString&)),
                          this, SLOT(slotNameLabelClicked(const QString&)));
         colorGridLayout->addWidget(colorLabel, i, 0);
         colorGridLayout->addWidget(nameLabel, i, 1);
         colorLabelsInColorGrid.push_back(colorLabel);
         nameLabelsInColorGrid.push_back(nameLabel);
         
         switch (colorKey) {
            case COLOR_KEY_AREAL_ESTIMATION:
               break;
            case COLOR_KEY_BORDERS:
               colorLabel->setToolTip("Click the left mouse button\n"
                                      "on this color swatch to \n"
                                      "highlight borders using \n"
                                      "with this name.");
               nameLabel->setToolTip("Click the left mouse button\n"
                                     "on this control to see vocabulary\n"
                                     "information about this name\n"
                                     "(if available)."); 
               break;
            case COLOR_KEY_CELLS:
               break;
            case COLOR_KEY_FOCI:
               break;
            case COLOR_KEY_PAINT:
               colorLabel->setToolTip("Click the left mouse button\n"
                                      "on this color swatch to \n"
                                      "highlight nodes identified \n"
                                      "with this name.");
               nameLabel->setToolTip("Click the left mouse button\n"
                                     "on this control to see vocabulary\n"
                                     "information about this name\n"
                                     "(if available)."); 
               break;
            case COLOR_KEY_PROBABILISTIC_ATLAS:
               break;
            case COLOR_KEY_VOLUME_PAINT:
               break;
            case COLOR_KEY_VOLUME_PROBABILISTIC_ATLAS:
               break;
         }
      }
      
      //
      // Set the color label background with color or text if no color available
      //
      updateColorLabel(i);
/*
      QColor labelColor(170, 170, 170, 0);
      bool autoFill = false;
      bool colorSelected = false;
      QPalette::ColorRole backgroundRole = QPalette::NoRole;
      if (nameTable[i].red >= 0) {
         labelColor.setRgb(nameTable[i].red, nameTable[i].green, nameTable[i].blue, 255);
         colorLabel->setText("    ");
         backgroundRole = QPalette::Window;
         autoFill = true;
         colorSelected = true;
      }
      else {
         colorLabel->setText("no color");
      }
      QPalette pal;
      pal.setColor(QPalette::Window, labelColor);
      if (colorSelected) {
         QBrush brush(labelColor, Qt::Dense3Pattern);
         pal.setBrush(QPalette::Window, brush);
      }
      colorLabel->setAutoFillBackground(autoFill);
      colorLabel->setBackgroundRole(backgroundRole);
      colorLabel->setPalette(pal);
      colorLabel->setAreaName(nameTable[i].name);
*/

      //
      // load the name label
      //
      if (vf->getBestMatchingVocabularyEntry(nameTable[i].name) != NULL) {
         nameLabel->setTextFormat(Qt::RichText);
         nameLabel->setText("<font color=blue><u>" + nameTable[i].name + "</u></font>");
      }
      else {
         nameLabel->setTextFormat(Qt::PlainText);
         nameLabel->setText(nameTable[i].name);
      }
      nameLabel->setAreaName(nameTable[i].name);

      //
      // Show the labels
      //
      colorLabel->setHidden(false);
      nameLabel->setHidden(false);
      
      //
      // Fix width of color labels
      //
      colorLabel->setFixedWidth(100);
   }
   
   //
   // Hide unneeded labels
   //
   const int numDisplayedLabels = colorLabelsInColorGrid.size();
   for (int i = 0; i < numDisplayedLabels; i++) {
      if (i < numTableNames) {
         colorLabelsInColorGrid[i]->setNameTableIndex(i);
         colorLabelsInColorGrid[i]->show();
         nameLabelsInColorGrid[i]->show();
      }
      else {
         colorLabelsInColorGrid[i]->hide();
         nameLabelsInColorGrid[i]->hide();
      }
   }
}

/**
 * update a color label.
 */
void 
GuiColorKeyDialog::updateColorLabel(const int indx)
{
   //
   // Get the label
   //
   GuiColorKeyLabel* colorLabel = colorLabelsInColorGrid[indx];
   
   //
   // Set the color label background with color or text if no color available
   //
   QColor labelColor(170, 170, 170, 0);
   bool autoFill = false;
   QPalette::ColorRole backgroundRole = QPalette::NoRole;
   if (nameTable[indx].red >= 0) {
      labelColor.setRgb(nameTable[indx].red, nameTable[indx].green, nameTable[indx].blue, 255);
      colorLabel->setText("    ");
      backgroundRole = QPalette::Window;
      autoFill = true;
   }
   else {
      colorLabel->setText("no color");
   }
   
   //
   // Draw in a pattern if highlighted
   //
   QPalette pal;
   pal.setColor(QPalette::Window, labelColor);
   if (nameTable[indx].highlightedFlag) {
      pal.setColor(QPalette::Window, labelColor);
   }
   colorLabel->setAutoFillBackground(autoFill);
   colorLabel->setBackgroundRole(backgroundRole);
   colorLabel->setPalette(pal);
   colorLabel->setAreaName(nameTable[indx].name);
   colorLabel->repaint();
}

/**
 * update areal estimation key.
 */
void 
GuiColorKeyDialog::updateArealEstimationKey(BrainSet* bs,
                                            std::vector<NameAndColor>& names)
{
   //
   // Get files and settings
   //
   ArealEstimationFile* aef = bs->getArealEstimationFile();
   DisplaySettingsArealEstimation* dsea = bs->getDisplaySettingsArealEstimation();
   const int numNodes = aef->getNumberOfNodes();
   //const int numColumns = aef->getNumberOfColumns();
   const int brainModelIndex = theMainWindow->getBrainModelIndex();
   //const int aefColumn = dsea->getFirstSelectedColumnForBrainModel(brainModelIndex);
   AreaColorFile* acf = bs->getAreaColorFile();
   const int numberOfAreaNames = aef->getNumberOfAreaNames();
   std::vector<bool> areaNamesUsed(numberOfAreaNames, false);
   
   std::vector<bool> selectedColumnFlags;
   dsea->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
   for (int ic = 0; ic < static_cast<int>(selectedColumnFlags.size()); ic++) {
      if ((selectedColumnFlags[ic]) &&
          (numNodes > 0) &&
          (numberOfAreaNames > 0)) {   
         //
         // Get the colors used by the paint file and column and place them in a set so no duplicates
         //
         const int numNodes = aef->getNumberOfNodes();
         for (int i = 0; i < numNodes; i++) {
            int nameIndices[4];
            float areas[4];
            aef->getNodeData(i, ic, nameIndices, areas);
            areaNamesUsed[nameIndices[0]] = true;
            areaNamesUsed[nameIndices[1]] = true;
            areaNamesUsed[nameIndices[2]] = true;
            areaNamesUsed[nameIndices[3]] = true;
         }
      }
   }
      
   //
   // Update name table
   //
   for (int i = 0; i < numberOfAreaNames; i++) {
      if (areaNamesUsed[i]) {
         int red = -1;
         int green = -1;
         int blue = -1;
         unsigned char r, g, b;
         const QString name(aef->getAreaName(i));
         bool exactMatch = false;
         const int colorFileIndex = acf->getColorByName(name,
                                                        exactMatch,
                                                        r, g, b);
         if (colorFileIndex >= 0) {
            red = r;
            green = g;
            blue = b;
         }
         names.push_back(NameAndColor(name, red, green, blue));
      }
   }
}

/**
 * update border key.
 */
void 
GuiColorKeyDialog::updateBorderKey(BrainSet* bs,
                                   std::vector<NameAndColor>& names)
{
   //
   // Get files and settings
   //
   DisplaySettingsBorders* dsb = bs->getDisplaySettingsBorders();
   BrainModelBorderSet* bmbs = bs->getBorderSet();
   AreaColorFile* acf = bs->getAreaColorFile();
   const int numAreaColors = acf->getNumberOfColors();
   BorderColorFile* bcf = bs->getBorderColorFile();
   const int numBorderColors = bcf->getNumberOfColors();
   const int brainModelIndex = theMainWindow->getBrainModelIndex();
   
   //
   // Find a unique set of border names and their corresponding colors
   //
   std::set<QString> uniqueBorderNames;
   const int num = bmbs->getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      const BrainModelBorder* border = bmbs->getBorder(i);
      if (border->getDisplayFlag()) {
         if (border->getValidForBrainModel(brainModelIndex)) {
            const QString name = border->getName();
            if (uniqueBorderNames.find(name) == uniqueBorderNames.end()) {
               const int areaColorFileIndex = border->getAreaColorFileIndex();
               const int borderColorFileIndex = border->getBorderColorFileIndex();
               int red = -1;
               int green = -1;
               int blue = -1;
               if (dsb->getOverrideBorderColorsWithAreaColors() &&
                   (areaColorFileIndex >= 0) &&
                   (areaColorFileIndex < numAreaColors)) {
                  unsigned char r, g, b;
                  acf->getColorByIndex(areaColorFileIndex, r, g, b);
                  red = r;
                  green = g;
                  blue = b;
               }
               else if ((borderColorFileIndex >= 0) &&
                        (borderColorFileIndex < numBorderColors)) {
                  unsigned char r, g, b;
                  bcf->getColorByIndex(borderColorFileIndex, r, g, b);
                  red = r;
                  green = g;
                  blue = b;
               }
               names.push_back(NameAndColor(name, red, green, blue));
               uniqueBorderNames.insert(name);
            }
         }
      }
   }
}

/**
 * update cell key.
 */
void 
GuiColorKeyDialog::updateCellKey(BrainSet* bs,
                                 std::vector<NameAndColor>& names)
{
   //
   // Files and settings
   //
   CellProjectionFile* cpf = bs->getCellProjectionFile();
   CellColorFile* ccf = bs->getCellColorFile();
   
   std::set<QString> uniqueCellNames;
   
   {
      const int num = cpf->getNumberOfCellProjections();
      for (int i = 0; i < num; i++) {
         const CellProjection* cp = cpf->getCellProjection(i);
         if (cp->getDisplayFlag()) {
            const QString name = cp->getName();
            uniqueCellNames.insert(name);
         }
      }
   }
      
   {
      CellFile* cf = bs->getVolumeCellFile();
      const int num = cf->getNumberOfCells();
      for (int i = 0; i < num; i++) {
         const CellData* cd = cf->getCell(i);
         if (cd->getDisplayFlag()) {
            const QString name = cd->getName();
            uniqueCellNames.insert(name);
         }
      }
   }
   
   {
      const int numFiles = bs->getNumberOfTransformationDataFiles();
      for (int j = 0; j < numFiles; j++) {
         AbstractFile* tdf = bs->getTransformationDataFile(j);
         CellFile* cf = dynamic_cast<CellFile*>(tdf);
         if (cf != NULL) {
            const int num = cf->getNumberOfCells();
            for (int i = 0; i < num; i++) {
               const CellData* cd = cf->getCell(i);
               const QString name = cd->getName();
               uniqueCellNames.insert(name);
            }
         }
      }
   }
   
   for (std::set<QString>::iterator iter = uniqueCellNames.begin();
        iter != uniqueCellNames.end();
        iter++) {
      const QString name = *iter;
      int red = -1;
      int green = -1;
      int blue = -1;
      unsigned char r, g, b, a;
      bool exactMatch = false;
      const int cellColorFileIndex = 
         ccf->getColorByName(name, exactMatch, r, g, b, a);
      if (cellColorFileIndex >= 0) {
         red = r;
         green = g;
         blue = b;
      }
      names.push_back(NameAndColor(name, red, green, blue));
      uniqueCellNames.insert(name);
   }
}

/**
 * update foci key.
 */
void 
GuiColorKeyDialog::updateFociKey(BrainSet* bs,
                                 std::vector<NameAndColor>& names)
{
   //
   // Files and settings
   //
   FociProjectionFile* fpf = bs->getFociProjectionFile();
   FociColorFile* fcf = bs->getFociColorFile();
   const int numberOfFociColors = fcf->getNumberOfColors();
   const DisplaySettingsFoci* dsf = bs->getDisplaySettingsFoci();
   const CellBase::CELL_COLOR_MODE colorMode = dsf->getColorMode();
   
   std::set<QString> uniqueCellNames;
   const int num = fpf->getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = fpf->getCellProjection(i);
      if (cp->getDisplayFlag()) {

         //
         // Foci are colored by either focus name or class name
         //
         QString name;         
         switch (colorMode) {
            case CellBase::CELL_COLOR_MODE_NAME:
               name = cp->getName();
               break;
            case CellBase::CELL_COLOR_MODE_CLASS:
               name = cp->getClassName();
               break;
         }
         if (name.isEmpty()) {
            name = "NO_NAME";
         }
         
         if (uniqueCellNames.find(name) == uniqueCellNames.end()) {
            int red = -1;
            int green = -1;
            int blue = -1;
            const int cellColorFileIndex = cp->getColorIndex();
            if ((cellColorFileIndex >= 0) &&
                (cellColorFileIndex < numberOfFociColors)) {
               unsigned char r, g, b;
               fcf->getColorByIndex(cellColorFileIndex, r, g, b);
               red = r;
               green = g;
               blue = b;
            }
            names.push_back(NameAndColor(name, red, green, blue));
            uniqueCellNames.insert(name);
         }
      }
   }
}

/**
 * update paint key.
 */
void 
GuiColorKeyDialog::updatePaintKey(BrainSet* bs,
                                  std::vector<NameAndColor>& names)
{
   //
   // Files and settings
   //
   AreaColorFile* acf = bs->getAreaColorFile();
   PaintFile* pf = bs->getPaintFile();
   DisplaySettingsPaint* dsp = bs->getDisplaySettingsPaint();
   const int brainModelIndex = theMainWindow->getBrainModelIndex();
   //const int paintColumn = dsp->getFirstSelectedColumnForBrainModel(brainModelIndex);
   const GiftiLabelTable* labelTable = pf->getLabelTable();
   const int numLabels = labelTable->getNumberOfLabels();
   if (numLabels <= 0) {
      return;
   }
   
   //
   // Get the colors used by the paint file and column and place them in a set so no duplicates
   //
   std::vector<bool> paintIndicesUsed(numLabels, false);
   std::vector<bool> selectedColumnFlags;
   dsp->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
   for (unsigned int ic = 0; ic < selectedColumnFlags.size(); ic++) {
      if (selectedColumnFlags[ic]) {
         const int numNodes = pf->getNumberOfNodes();
         for (int i = 0; i < numNodes; i++) {
            const int paintNum = pf->getPaint(i, ic);
            paintIndicesUsed[paintNum] = true;
         }
      }
   }
   
   //
   // Update name table
   //
   for (int i = 0; i < numLabels; i++) {
      if (paintIndicesUsed[i]) {
         const QString name(labelTable->getLabel(i));
         int red = -1;
         int green = -1;
         int blue = -1;
         unsigned char r, g, b;
         bool exactMatch;
         const int colorIndex = acf->getColorByName(name, exactMatch, r, g, b);
         if (colorIndex >= 0) {
            red = r;
            green = g;
            blue = b;
         }
         names.push_back(NameAndColor(name, red, green, blue));
      }
   }
}

/**
 * update prob atlas key.
 */
void 
GuiColorKeyDialog::updateProbAtlasKey(BrainSet* bs,
                                      std::vector<NameAndColor>& names)
{
   //
   // Files and settings
   //
   AreaColorFile* acf = bs->getAreaColorFile();
   ProbabilisticAtlasFile* pf = bs->getProbabilisticAtlasSurfaceFile();
   const GiftiLabelTable* labelTable = pf->getLabelTable();
   const int numLabels = labelTable->getNumberOfLabels();
   const int numColumns = pf->getNumberOfColumns();
   const int numNodes = pf->getNumberOfNodes();
   if ((numLabels > 0) && 
       (numNodes > 0) &&
       (numColumns > 0)) {   
      //
      // Get the colors used by the paint file and column and place them in a set so no duplicates
      //
      std::vector<bool> paintIndicesUsed(numLabels, false);
      for (int i = 0; i < numNodes; i++) {
         for (int j = 0; j < numColumns; j++) {
            const int paintNum = pf->getPaint(i, j);
            paintIndicesUsed[paintNum] = true;
         }
      }
      
      //
      // Update name table
      //
      for (int i = 0; i < numLabels; i++) {
         if (paintIndicesUsed[i]) {
            const QString name(labelTable->getLabel(i));
            int red = -1;
            int green = -1;
            int blue = -1;
            unsigned char r, g, b;
            bool exactMatch;
            const int colorIndex = acf->getColorByName(name, exactMatch, r, g, b);
            if (colorIndex >= 0) {
               red = r;
               green = g;
               blue = b;
            }
            names.push_back(NameAndColor(name, red, green, blue));
         }
      }
   }
}

/**
 * update volume paint key.
 */
void 
GuiColorKeyDialog::updateVolumePaintKey(BrainSet* bs,
                                        std::vector<NameAndColor>& names)
{
   const BrainModelVolume* bmv = bs->getBrainModelVolume();
   if (bmv != NULL) {
      const VolumeFile* vf = bmv->getSelectedVolumePaintFile();
      if (vf != NULL) {
         const int numVoxels = vf->getTotalNumberOfVoxelElements();
         const float* voxelData = vf->getVoxelData();
         const int numRegionNames = vf->getNumberOfRegionNames();
         AreaColorFile* acf = bs->getAreaColorFile();
         const int numberOfColors = acf->getNumberOfColors();
         
         if ((numVoxels > 0) &&
             (numRegionNames > 0) &&
             (numberOfColors > 0)) {
            //
            // Get the colors used by the paint volume 
            //
            std::vector<bool> regionNamesUsed(numRegionNames, false);
            for (int i = 0; i < numVoxels; i++) {
               const int voxel = static_cast<int>(voxelData[i]);
               if ((voxel >= 0) && (voxel < numRegionNames)) {
                  regionNamesUsed[voxel] = true;
               }
            }
            
            //
            // Update name table
            //
            for (int i = 0; i < numRegionNames; i++) {
               if (regionNamesUsed[i]) {
                  int red = -1;
                  int green = -1;
                  int blue = -1;
                  unsigned char r, g, b;
                  const QString name(vf->getRegionNameFromIndex(i));
                  bool exactMatch = false;
                  const int colorFileIndex = acf->getColorByName(name,
                                                                 exactMatch,
                                                                 r, g, b);
                  if (colorFileIndex >= 0) {
                     red = r;
                     green = g;
                     blue = b;
                  }
                  names.push_back(NameAndColor(name, red, green, blue));
               }
            }
         }
      }
   }
}

/**
 * update volume prob atlas key.
 */
void 
GuiColorKeyDialog::updateVolumeProbAtlasKey(BrainSet* bs,
                                            std::vector<NameAndColor>& names)
{
   const BrainModelVolume* bmv = bs->getBrainModelVolume();
   if (bmv != NULL) {
      std::set<QString> regionNames;
      
      //
      // Loop through all of the prob atlas volumes
      //
      const int numProbAtlasVolumeFiles = bs->getNumberOfVolumeProbAtlasFiles();
      for (int m = 0; m < numProbAtlasVolumeFiles; m++) {
         const VolumeFile* vf = bs->getVolumeProbAtlasFile(m);
         if (vf != NULL) {
            const int numVoxels = vf->getTotalNumberOfVoxelElements();
            const float* voxelData = vf->getVoxelData();
            const int numRegionNames = vf->getNumberOfRegionNames();
            AreaColorFile* acf = bs->getAreaColorFile();
            const int numberOfColors = acf->getNumberOfColors();
            
            if ((numVoxels > 0) &&
                (numRegionNames > 0) &&
                (numberOfColors > 0)) {
               //
               // Get the colors used by the paint volume 
               //
               std::vector<bool> regionNamesUsed(numRegionNames, false);
               for (int i = 0; i < numVoxels; i++) {
                  const int voxel = static_cast<int>(voxelData[i]);
                  if ((voxel >= 0) && (voxel < numRegionNames)) {
                     regionNamesUsed[voxel] = true;
                  }
               }
               
               //
               // Update name table
               //
               for (int i = 0; i < numRegionNames; i++) {
                  if (regionNamesUsed[i]) {
                     regionNames.insert(vf->getRegionNameFromIndex(i));
                  }
               }
            }
         }
      }
      
      AreaColorFile* acf = bs->getAreaColorFile();
      for (std::set<QString>::iterator iter = regionNames.begin();
           iter != regionNames.end();
           iter++) {
         int red = -1;
         int green = -1;
         int blue = -1;
         unsigned char r, g, b;
         const QString name(*iter);
         bool exactMatch = false;
         const int colorFileIndex = acf->getColorByName(name,
                                                        exactMatch,
                                                        r, g, b);
         if (colorFileIndex >= 0) {
            red = r;
            green = g;
            blue = b;
         }
         names.push_back(NameAndColor(name, red, green, blue));
      }
   }
}
      
/**
 * save scene.
 */
SceneFile::SceneClass 
GuiColorKeyDialog::saveScene()
{      
   SceneFile::SceneClass sc(getTitleOfColorKey(colorKey));
      
   const int geometry[4] = {
      x(),
      y(),
      width(),
      height()
   };
   std::ostringstream str;
   str << geometry[0] << " "
       << geometry[1] << " "
       << geometry[2] << " "
       << geometry[3];
      sc.addSceneInfo(SceneFile::SceneInfo("Geometry", str.str().c_str()));
   
   return sc;
}

/**
 * show scene.
 */
void 
GuiColorKeyDialog::showScene(const SceneFile::SceneClass sc,
                             const int screenMaxX,
                             const int screenMaxY,
                             const int mainWindowSceneX,
                             const int mainWindowSceneY,
                             const int mainWindowX,
                             const int mainWindowY)
{
   int geometry[4] = { x(), y(), width(), height() };
      
   const int num = sc.getNumberOfSceneInfo();
   for (int i = 0; i < num; i++) {
      const SceneFile::SceneInfo* si = sc.getSceneInfo(i);
      const QString infoName = si->getName();      
      
      if (infoName == "Geometry") {
         std::vector<QString> tokens;
         StringUtilities::token(si->getValueAsString(), " ", tokens);
         if (tokens.size() >= 4) {
            geometry[0] = StringUtilities::toInt(tokens[0]);
            geometry[1] = StringUtilities::toInt(tokens[1]);
            geometry[2] = StringUtilities::toInt(tokens[2]);
            geometry[3] = StringUtilities::toInt(tokens[3]);
         }
      }
   }
   
   const DisplaySettingsScene* dss = theMainWindow->getBrainSet()->getDisplaySettingsScene();
   
   if (geometry[0] > 0) {
      int x = geometry[0];
      int y = geometry[1];
      x = std::min(x, screenMaxX);
      y = std::min(y, screenMaxY);
      switch(dss->getWindowPositionPreference()) {
         case DisplaySettingsScene::WINDOW_POSITIONS_USE_ALL:
            move(x, y);
            break;
         case DisplaySettingsScene::WINDOW_POSITIONS_IGNORE_MAIN_OTHERS_RELATIVE:
            x = (x - mainWindowSceneX) + mainWindowX;
            y = (y - mainWindowSceneY) + mainWindowY;
            move(x, y);
            break;
         case DisplaySettingsScene::WINDOW_POSITIONS_IGNORE_ALL:
            break;
      }
      move(x, y);
      resize(geometry[2], geometry[3]);
   }
   
   updateDialog();
}

/** 
 * called when a color label is clicked.
 */
void 
GuiColorKeyDialog::slotColorLabelClicked(const int nameTableIndex)
{
   if ((nameTableIndex < 0) ||
       (nameTableIndex >= static_cast<int>(nameTable.size()))) {
      return;
   }
   
   const QString name = nameTable[nameTableIndex].name;
   nameTable[nameTableIndex].highlightedFlag = (! nameTable[nameTableIndex].highlightedFlag);
   const bool highlightFlag = nameTable[nameTableIndex].highlightedFlag;
   
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   const int modelIndex = theMainWindow->getBrainModelIndex();
   
   if ((modelIndex >= 0) && 
       (modelIndex < bs->getNumberOfBrainModels())) {
      switch (colorKey) {
         case COLOR_KEY_AREAL_ESTIMATION:
            break;
         case COLOR_KEY_BORDERS:
            {
               BrainModelBorderSet* bmbs = bs->getBorderSet();
               const int numBorders = bmbs->getNumberOfBorders();
               for (int i = 0; i < numBorders; i++) {
                  BrainModelBorder* b = bmbs->getBorder(i);
                  if (b->getName() == name) {
                     b->setHighlightFlag(highlightFlag);
                  }
               }
            }
            break;
         case COLOR_KEY_CELLS:
            break;
         case COLOR_KEY_FOCI:
            {
               const DisplaySettingsFoci* dsf = bs->getDisplaySettingsFoci();
               const CellBase::CELL_COLOR_MODE colorMode = dsf->getColorMode();
               FociProjectionFile* fpf = bs->getFociProjectionFile();
               const int numFoci = fpf->getNumberOfCellProjections();
               for (int i = 0; i < numFoci; i++) {
                  CellProjection* cp = fpf->getCellProjection(i);

                  //
                  // Foci are colored by either focus name or class name
                  //
                  QString focusName;         
                  switch (colorMode) {
                     case CellBase::CELL_COLOR_MODE_NAME:
                        focusName = cp->getName();
                        break;
                     case CellBase::CELL_COLOR_MODE_CLASS:
                        focusName = cp->getClassName();
                        break;
                  }
                  if (focusName.isEmpty()) {
                     focusName = "NO_NAME";
                  }
                  
                  if (focusName == name) {
                     cp->setHighlightFlag(highlightFlag);
                  }
               }
            }
            break;
         case COLOR_KEY_PAINT:
            if (bms != NULL) {
               const PaintFile* pf = bs->getPaintFile();
               const DisplaySettingsPaint* dsp = bs->getDisplaySettingsPaint();
               const int numNodes = pf->getNumberOfNodes();
               std::vector<bool> selectedColumnFlags;
               const int brainModelIndex = theMainWindow->getBrainModelIndex();
               dsp->getSelectedColumnFlags(brainModelIndex, selectedColumnFlags);
               //const int columnNumber = dsp->getFirstSelectedColumnForBrainModel(modelIndex);
               for (unsigned int ic = 0; ic < selectedColumnFlags.size(); ic++) {
                  if (selectedColumnFlags[ic]) {
                     const int paintIndex = pf->getPaintIndexFromName(name);
                     if (paintIndex >= 0) {
                        for (int i = 0; i < numNodes; i++) {
                           if (pf->getPaint(i, ic) == paintIndex) {
                              BrainSetNodeAttribute* bna = bs->getNodeAttributes(i);
                              if (highlightFlag) {
                                 bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
                              }
                              else {
                                 bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE);
                              }
                           }
                        }
                     }
                  }
               }
            }
            break;
         case COLOR_KEY_PROBABILISTIC_ATLAS:
            if (bms != NULL) {
               const PaintFile* pf = bs->getProbabilisticAtlasSurfaceFile();
               const int numCols = pf->getNumberOfColumns();
               const DisplaySettingsProbabilisticAtlas* dsp = bs->getDisplaySettingsProbabilisticAtlasSurface();
               const int numNodes = pf->getNumberOfNodes();
               std::vector<bool> selectedColumnFlags;
               const int paintIndex = pf->getPaintIndexFromName(name);
               if (paintIndex >= 0) {
                  for (int i = 0; i < numNodes; i++) {
                     for (int ic = 0; ic < numCols; ic++) {
                        if (dsp->getChannelSelected(ic)) {
                           if (pf->getPaint(i, ic) == paintIndex) {
                              BrainSetNodeAttribute* bna = bs->getNodeAttributes(i);
                              if (highlightFlag) {
                                 bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
                              }
                              else {
                                 bna->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_NONE);
                              }
                              break;  // go to next node
                           }
                        }
                     }
                  }
               }
            }
            break;
         case COLOR_KEY_VOLUME_PAINT:
            {
               const int numVolumes = bs->getNumberOfVolumePaintFiles();
               for (int i = 0; i < numVolumes; i++) {
                  VolumeFile* vf = bs->getVolumePaintFile(i);
                  if (i == 0) {
                     vf->setHighlightRegionName(name, highlightFlag);
                  }
                  vf->setVoxelColoringInvalid();
               }
            }
            break;
         case COLOR_KEY_VOLUME_PROBABILISTIC_ATLAS:
            {
               const int numVolumes = bs->getNumberOfVolumeProbAtlasFiles();
               for (int i = 0; i < numVolumes; i++) {
                  VolumeFile* vf = bs->getVolumeProbAtlasFile(i);
                  if (i == 0) {
                     vf->setHighlightRegionName(name, highlightFlag);
                  }
                  vf->setVoxelColoringInvalid();
               }
            }
            break;
      }
      
      updateColorLabel(nameTableIndex);
   }
   
   theMainWindow->getBrainSet()->clearAllDisplayLists();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * called when a name label is clicked.
 */
void 
GuiColorKeyDialog::slotNameLabelClicked(const QString& name)
{
   //std::cout << "name label: " << name.toAscii().constData() << std::endl;
   switch (colorKey) {
      case COLOR_KEY_AREAL_ESTIMATION:
         break;
      case COLOR_KEY_BORDERS:
         break;
      case COLOR_KEY_CELLS:
         break;
      case COLOR_KEY_FOCI:
         displayStudyMatchingFociColor(name);
         break;
      case COLOR_KEY_PAINT:
         break;
      case COLOR_KEY_PROBABILISTIC_ATLAS:
         break;
      case COLOR_KEY_VOLUME_PAINT:
         break;
      case COLOR_KEY_VOLUME_PROBABILISTIC_ATLAS:
         break;
   }
   
   GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(false);
   id->displayVocabularyNameData(name);
}

/**
 * clear highlighting.
 */
void 
GuiColorKeyDialog::slotClearHighlighting()
{
   bool clearNodesFlag = false;
   BrainSet* bs = theMainWindow->getBrainSet();
   switch (colorKey) {
      case COLOR_KEY_AREAL_ESTIMATION:
         clearNodesFlag = true;
         break;
      case COLOR_KEY_BORDERS:
         {
            BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
            bmbs->clearBorderHighlighting();
         }
         break;
      case COLOR_KEY_CELLS:
         break;
      case COLOR_KEY_FOCI:
         {
            FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
            fpf->clearAllHighlightFlags();
         }
         break;
      case COLOR_KEY_PAINT:
         clearNodesFlag = true;
         break;
      case COLOR_KEY_PROBABILISTIC_ATLAS:
         clearNodesFlag = true;
         break;
      case COLOR_KEY_VOLUME_PAINT:
         if (bs->getNumberOfVolumePaintFiles() > 0) {
            bs->getVolumePaintFile(0)->clearRegionHighlighting();
            GuiBrainModelOpenGL::updateAllGL();
         }
         break;
      case COLOR_KEY_VOLUME_PROBABILISTIC_ATLAS:
         if (bs->getNumberOfVolumeProbAtlasFiles() > 0) {
            bs->getVolumeProbAtlasFile(0)->clearRegionHighlighting();
         }
         break;
   }
   
   if (clearNodesFlag) {
      theMainWindow->getBrainSet()->clearNodeHighlightSymbols();
   }
   
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * display study matching a foci color.
 */
void 
GuiColorKeyDialog::displayStudyMatchingFociColor(const QString& colorName)
{
   BrainSet* bs = theMainWindow->getBrainSet();
   
   //
   // Get index of color in foci 
   //
   const StudyMetaDataFile* smdf = bs->getStudyMetaDataFile();
   const ColorFile* cf = bs->getFociColorFile();
   bool match = false;
   const int colorIndex = cf->getColorIndexByName(colorName, match);
   if (colorIndex > 0) {
      const FociProjectionFile* fpf = bs->getFociProjectionFile();
      const int numFoci = fpf->getNumberOfCellProjections();
      for (int i = 0; i < numFoci; i++) {
         const CellProjection* cp = fpf->getCellProjection(i);
         if (cp->getColorIndex() == colorIndex) {
            StudyMetaDataLinkSet smdls = cp->getStudyMetaDataLinkSet();
            for (int j = 0; j < smdls.getNumberOfStudyMetaDataLinks(); j++) {
               StudyMetaDataLink* smdl = smdls.getStudyMetaDataLinkPointer(j);
               smdl->setTableNumber("");
               smdl->setFigureNumber("");
               smdl->setPageReferencePageNumber("");
               smdl->setPageNumber("");
            }
            BrainModelIdentification* bmi = bs->getBrainModelIdentification();
            const QString studyMessage = bmi->getIdentificationTextForStudies(true,
                                                                              smdf,
                                                                              smdls);
            if (studyMessage.isEmpty() == false) {
               GuiIdentifyDialog* idDialog = theMainWindow->getIdentifyDialog(true);
               idDialog->appendHtml(studyMessage);
               break;
            }
         }
      }
   }
}
      
//-----------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiColorKeyLabel::GuiColorKeyLabel(QWidget* parent)
   : QLabel(parent)
{
   nameTableIndex = -1;
}

/**
 * destructor.
 */
GuiColorKeyLabel::~GuiColorKeyLabel()
{
}

/**
 * called when a mouse button is pressed over this widget.
 */
void 
GuiColorKeyLabel::mousePressEvent(QMouseEvent* me)
{
   if (me->button() == Qt::LeftButton) {
      if (me->modifiers() == Qt::NoButton) {
         if (areaName.isEmpty() == false) {
            emit signalShowAreaName(areaName);
         }
         if (nameTableIndex >= 0) {
            emit signalNameTableIndex(nameTableIndex);
         }
      }
   }
}

/**
 * set the area name.
 */
void 
GuiColorKeyLabel::setAreaName(const QString& areaNameIn)
{
   areaName = areaNameIn;
}

/**
 * set name table index.
 */
void 
GuiColorKeyLabel::setNameTableIndex(const int indx)
{
   nameTableIndex = indx;
}
      
//-----------------------------------------------------------------------------------

/**
 * constructor.
 */
GuiColorKeyDialog::NameAndColor::NameAndColor(const QString& nameIn,
             const int redIn,
             const int greenIn,
             const int blueIn)
{
   name  = nameIn;
   red   = redIn;
   green = greenIn;
   blue  = blueIn;
   highlightedFlag = false;
}
             
/**
 * destructor.
 */
GuiColorKeyDialog::NameAndColor::~NameAndColor()
{
}

/**
 * less than operator.
 */
bool 
GuiColorKeyDialog::NameAndColor::operator<(const NameAndColor& nac) const
{
   return (name < nac.name);
}

/**
 * equality operator.
 */
bool 
GuiColorKeyDialog::NameAndColor::operator==(const NameAndColor& nac) const
{
   return (name == nac.name);
}
