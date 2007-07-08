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


#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qvgroupbox.h>

#include "GuiMapFmriMetricNamingDialog.h"
#include "QtTextEditDialog.h"
#include "QtUtilities.h"
#include "map_fmri_global_variables.h"

/** 
 * Constructor
 */
GuiMapFmriMetricNamingDialog::GuiMapFmriMetricNamingDialog(QWidget* parent)
   : QDialog(parent, "GuiMapFmriMetricNamingDialog", true)
{
   setCaption("fMRI Metric Naming");

   QVBoxLayout* layout = new QVBoxLayout(this, 5, 5, "layout");
   
   //
   // Vertical Groupbox for surface information
   //
   QVGroupBox* surfaceGroupBox = new QVGroupBox("File Information", this, "surfaceGroupBox");
   layout->addWidget(surfaceGroupBox);
   
   //
   // Grid Layout for file information
   QWidget* fileWidget = new QWidget(surfaceGroupBox, "fileWidget");
   QGridLayout* fileGrid = new QGridLayout(fileWidget, 4, 3, 5, 5, "fileGrid");
   
   //
   // surface being mapped to
   //
   fileGrid->addWidget(new QLabel("Surface", fileWidget), 0, 0, Qt::AlignLeft);
   surfaceComboBox = new QComboBox(fileWidget, "surfaceComboBox");
   QObject::connect(surfaceComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotSurfaceSelection(int)));
   fileGrid->addWidget(surfaceComboBox, 0, 1, Qt::AlignLeft);
   
   //
   // Load surface combo box
   //
   for (int i = 0; i < fmriMapper.getNumberOfSurfaces(); i++) {
      const FMRISurfaceInfo* si = fmriMapper.getSurface(i);
      surfaceComboBox->insertItem(si->getDescriptiveName().c_str());
   }
   
   //
   // spec file
   //
   const int fileLineEditWidth = 400;
   fileGrid->addWidget(new QLabel("Spec File", fileWidget), 1, 0, Qt::AlignLeft);
   specFileNameLineEdit = new QLineEdit(fileWidget, "specFileNameLineEdit");
   specFileNameLineEdit->setFixedWidth(fileLineEditWidth);
   specFileNameLineEdit->setReadOnly(true);
   fileGrid->addWidget(specFileNameLineEdit, 1, 1, Qt::AlignLeft);
   QPushButton* specFileButton = new QPushButton("Select...", fileWidget, "specFileButton");
   specFileButton->setAutoDefault(false);
   QObject::connect(specFileButton, SIGNAL(clicked()),
                    this, SLOT(slotSpecFile()));
   fileGrid->addWidget(specFileButton, 1, 2, Qt::AlignLeft);
   
   //
   // metric file
   //
   fileGrid->addWidget(new QLabel("Metric File", fileWidget), 2, 0, Qt::AlignLeft);
   metricFileNameLineEdit = new QLineEdit(fileWidget, "metricFileNameLineEdit");
   metricFileNameLineEdit->setFixedWidth(fileLineEditWidth);
   fileGrid->addWidget(metricFileNameLineEdit, 2, 1, Qt::AlignLeft);
   QPushButton* metricFileButton = new QPushButton("Select...", fileWidget, "metricFileButton");
   metricFileButton->setAutoDefault(false);
   QObject::connect(metricFileButton, SIGNAL(clicked()),
                    this, SLOT(slotMetricFile()));
   fileGrid->addWidget(metricFileButton, 2, 2, Qt::AlignLeft);
   
   //
   // Metric Title Row
   //
   fileGrid->addWidget(new QLabel("Metric Title", fileWidget), 3, 0, Qt::AlignLeft);
   metricTitleLineEdit = new QLineEdit(fileWidget, "metricLineEdit");
   metricTitleLineEdit->setFixedWidth(fileLineEditWidth);
   fileGrid->addWidget(metricTitleLineEdit, 3, 1, Qt::AlignLeft);
   
   //
   //
   // Metric Data attributes Groupbox for surface information
   //
   QVGroupBox* metricGroupBox = new QVGroupBox("Metric Data Attributes", this, "metricGroupBox");
   layout->addWidget(metricGroupBox);
   
   FMRISurfaceInfo* si = fmriMapper.getSurface(0);
   const int numVolumes = si->getNumberOfMetricColumnNames();
   
   //
   // Scroll view for metric column info
   //
   QScrollView* sv = new QScrollView(metricGroupBox, "metric-scrollview");
   //sv-enableClipper(true);
   
   //
   // Grid Layout for metric information
   QWidget* metricWidget = new QWidget(sv, "metricWidget");
   sv->addChild(metricWidget);
   QGridLayout* metricGrid = new QGridLayout(metricWidget, (numVolumes + 1), 3, 5, 5, "metricGrid");
   
   //
   // Metric Column Titles
   //
   metricGrid->addWidget(new QLabel("Column",  metricWidget), 1, 0, Qt::AlignHCenter);
   metricGrid->addWidget(new QLabel("Name",    metricWidget), 1, 1, Qt::AlignHCenter);
   metricGrid->addWidget(new QLabel("Comment", metricWidget), 1, 2, Qt::AlignHCenter);

   //
   // Button group used for metric column view/edit buttons
   //
   QButtonGroup* buttGroup = new QButtonGroup(metricWidget, "buttGroup");
   buttGroup->hide();
   QObject::connect(buttGroup, SIGNAL(clicked(int)),
                    this, SLOT(slotMetricColumnComment(int)));
   
   //
   // Metric column names and comment edit buttons
   //
   const int nameLineWidth = 400;
   for (int j = 0; j < numVolumes; j++) {      
      metricGrid->addWidget(new QLabel(QString::number(j), metricWidget), 2 + j, 0, Qt::AlignHCenter);
      QLineEdit* le = new QLineEdit(metricWidget, "le");
      le->setMinimumWidth(nameLineWidth);
      metricGrid->addWidget(le, 2 + j, 1, Qt::AlignHCenter);
      metricColumnNameLineEdits.push_back(le);
      
      QPushButton* pb = new QPushButton("View/Edit...", metricWidget, "pb");
      pb->setAutoDefault(false);
      buttGroup->insert(pb);
      metricGrid->addWidget(pb, 2 + j, 2, Qt::AlignHCenter);
   }
      
   //
   // Dialog button(s)
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout(layout, 5, "buttonsLayout");
   QPushButton* okButton = new QPushButton("OK", this);
   okButton->setFixedSize(okButton->sizeHint());
   buttonsLayout->addWidget(okButton);
   QObject::connect(okButton, SIGNAL(clicked()),
                    this, SLOT(accept()));
                    
   currentSurfaceFileIndex = -1;
   slotSurfaceSelection(0);
}

/** 
 * Destructor
 */
GuiMapFmriMetricNamingDialog::~GuiMapFmriMetricNamingDialog()
{
}

/**
 * Called when users closes dialog.
 */
void
GuiMapFmriMetricNamingDialog::done(int r)
{
   if (r == Accepted) {
      readCurrentSelections();
   }
   QDialog::done(r);
}

/**
 * called when spec file select button is pressed
 */
void 
GuiMapFmriMetricNamingDialog::slotSpecFile()
{
   QFileDialog fd(this, "spec-file-select-dialog", true);
   fd.setCaption("Choose Spec File");
   fd.setFilter("Spec Files (*.spec)");
   fd.setMode(QFileDialog::AnyFile);
   if (fd.exec() == QDialog::Accepted) {
      QString fileName(fd.selectedFile().latin1());
      if (fileName.isEmpty() == false) {
         specFileNameLineEdit->setText(fileName);
      }
   }
}

      
/**
 * called when metric file select button is pressed
 */
void 
GuiMapFmriMetricNamingDialog::slotMetricFile()
{
   QFileDialog fd(this, "metric-file-select-dialog", true);
   fd.setCaption("Choose Metric File");
   fd.setFilter("Metric Files (*.metric)");
   fd.setMode(QFileDialog::AnyFile);
   if (fd.exec() == QDialog::Accepted) {
      QString fileName(fd.selectedFile().latin1());
      if (fileName.isEmpty() == false) {
         metricFileNameLineEdit->setText(fileName);
      }
   }
}

/**
 *
 */
void
GuiMapFmriMetricNamingDialog::readCurrentSelections()
{
   const int numSurfaces = fmriMapper.getNumberOfSurfaces();
   if ((currentSurfaceFileIndex >= 0) && (currentSurfaceFileIndex < numSurfaces)) {
      FMRISurfaceInfo* si = fmriMapper.getSurface(currentSurfaceFileIndex);
   
      si->setSpecFileName(specFileNameLineEdit->text().latin1());
      si->setMetricFileName(metricFileNameLineEdit->text().latin1());
      si->setMetricTitle(metricTitleLineEdit->text().latin1());
      
      for (int i = 0; i < si->getNumberOfMetricColumnNames(); i++) {
         si->setMetricColumnName(i, metricColumnNameLineEdits[i]->text().latin1());
      }
   }
   
}

/**
 * called by surface selection combo box
 */
void 
GuiMapFmriMetricNamingDialog::slotSurfaceSelection(int item)
{   
   //
   // Save any current selections
   //
   readCurrentSelections();
   currentSurfaceFileIndex = item;

   //
   // Load new selections
   //
   const int numSurfaces = fmriMapper.getNumberOfSurfaces();
   if ((currentSurfaceFileIndex >= 0) && (currentSurfaceFileIndex < numSurfaces)) {
      const FMRISurfaceInfo* si = fmriMapper.getSurface(currentSurfaceFileIndex);
   
      specFileNameLineEdit->setText(si->getSpecFileName().c_str());
      metricFileNameLineEdit->setText(si->getMetricFileName().c_str());
      metricTitleLineEdit->setText(si->getMetricTitle().c_str());
      
      for (int i = 0; i < si->getNumberOfMetricColumnNames(); i++) {
         metricColumnNameLineEdits[i]->setText(si->getMetricColumnName(i).c_str());
      }
   }   
}

/**
 * Called when a view/edit comment button is pressed.
 */
void
GuiMapFmriMetricNamingDialog::slotMetricColumnComment(int item)
{
   FMRISurfaceInfo* si = fmriMapper.getSurface(item);
   QtTextEditDialog td(this);
   td.setText(si->getMetricColumnComments(item));
   td.exec();
   si->setMetricColumnComments(item, td.getText());
}

      
      
