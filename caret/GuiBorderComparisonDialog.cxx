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
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "BorderFile.h"
#include "GuiBorderComparisonDialog.h"
#include "MathUtilities.h"
#include "QtTextEditDialog.h"
#include "QtUtilities.h"
#include "SpecFile.h"
#include "WuQFileDialog.h"

/**
 * constructor.
 */
GuiBorderComparisonDialog::GuiBorderComparisonDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   setWindowTitle("Border Comparison");
   
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(3);
   dialogLayout->setSpacing(3);
   
   //
   // Grid layout for file section
   //
   QGridLayout* fileGridLayout = new QGridLayout;
   fileGridLayout->setMargin(3);
   fileGridLayout->setColumnStretch(0, 0);
   fileGridLayout->setColumnStretch(1, 100);
   dialogLayout->addLayout(fileGridLayout);
   
   //
   // File A button and line edit
   //
   QPushButton* fileAButton = new QPushButton("Border File A...");
   QObject::connect(fileAButton, SIGNAL(clicked()),
                    this, SLOT(slotButtonBorderFileA()));
   fileAButton->setAutoDefault(false);
   fileGridLayout->addWidget(fileAButton, 0, 0);
   borderFileALineEdit = new QLineEdit;
   borderFileALineEdit->setMinimumWidth(300);
   fileGridLayout->addWidget(borderFileALineEdit, 0, 1);
   
   //
   // File B button and line edit
   //
   QPushButton* fileBButton = new QPushButton("Border File B...");
   QObject::connect(fileBButton, SIGNAL(clicked()),
                    this, SLOT(slotButtonBorderFileB()));
   fileBButton->setAutoDefault(false);
   fileGridLayout->addWidget(fileBButton, 1, 0);
   borderFileBLineEdit = new QLineEdit;
   borderFileBLineEdit->setMinimumWidth(300);
   fileGridLayout->addWidget(borderFileBLineEdit, 1, 1);
   
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonLayout = new QHBoxLayout;
   buttonLayout->setSpacing(3);
   dialogLayout->addLayout(buttonLayout);
   
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   buttonLayout->addWidget(applyButton);
   applyButton->setAutoDefault(false);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
                    
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   
   //
   // Make buttons the same size
   //
   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
}

/**
 * destructor.
 */
GuiBorderComparisonDialog::~GuiBorderComparisonDialog()
{
}

/**
 * called when apply button is pressed.
 */
void 
GuiBorderComparisonDialog::slotApplyButton()
{
   //
   // See if border entered
   //
   const QString fileNameA = borderFileALineEdit->text();
   if (fileNameA.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Border file A not specified.");
      return;
   }
   const QString fileNameB = borderFileBLineEdit->text();
   if (fileNameB.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "Border file B not specified.");
      return;
   }

   //
   // Read the border files
   //
   BorderFile fileA;
   try {
      fileA.readFile(fileNameA);
   }
   catch (FileException& e) {
      QMessageBox::critical(this, "ERROR", e.whatQString());
      return;
   }
   BorderFile fileB;
   try {
      fileB.readFile(fileNameB);
   }
   catch (FileException& e) {
      QMessageBox::critical(this, "ERROR", e.whatQString());
      return;
   }
   
   //
   // Make sure there are borders
   //
   const int num = fileA.getNumberOfBorders();
   if (num <= 0) {
      QMessageBox::critical(this, "ERROR", "File A contains 0 borders.");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   //
   // string stream used for comparison messages
   //
   std::ostringstream str;
   
   //
   // loop through border file A's borders
   //
   for (int i = 0; i < num; i++) {
      //
      // Get border from file A and its name
      //
      Border* borderA = fileA.getBorder(i);
      const QString name(borderA->getName());
      
      //
      // If border A has links
      //
      const int numLinks = borderA->getNumberOfLinks();
      if (numLinks > 0) {
         
         //
         // Get border from other file with name of border from file A
         //
         Border* borderB = fileB.getBorderByName(name);
         if (borderB != NULL) {
            //
            // Make sure number of links match
            //
            if (borderB->getNumberOfLinks() == numLinks) {
               //
               // compute average distance between links
               //
               double sum = 0.0;
               for (int j = 0; j < numLinks; j++) {
                  sum += MathUtilities::distance3D(borderA->getLinkXYZ(j),
                                                   borderB->getLinkXYZ(j));
               }
               str << name.toAscii().constData() << " average distance between points "
                           << (sum / static_cast<double>(numLinks)) << ".";
            }
            else {
               str << name.toAscii().constData() << " has different number of links in file B";
            }
         }
         else {
            str << name.toAscii().constData() << " not found in file border file B.";
         }
      }
      else {
         str << name.toAscii().constData() << " has no links - ignored.";
      }
      str << "\n\n";
   }
   
   QApplication::beep();
   QApplication::restoreOverrideCursor();
   
   //
   // Display results
   //
   QtTextEditDialog* te = new QtTextEditDialog(this, true);
   te->setText(str.str().c_str());
   te->show();
}

/**
 * called when border file A button is pressed.
 */
void 
GuiBorderComparisonDialog::slotButtonBorderFileA()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setWindowTitle("Choose Border File");
   fd.setFileMode(WuQFileDialog::ExistingFile); 
   fd.setFilter(QString("Border Files (*%1)").arg(SpecFile::getBorderFileExtension()));
   if (fd.exec() == QDialog::Accepted) {
      borderFileALineEdit->setText(fd.selectedFiles().at(0));
   }
}

/**
 * called when border file B button is pressed.
 */
void 
GuiBorderComparisonDialog::slotButtonBorderFileB()
{
   WuQFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setWindowTitle("Choose Border File");
   fd.setFileMode(WuQFileDialog::ExistingFile); 
   fd.setFilter(QString("Border Files (*%1)").arg(SpecFile::getBorderFileExtension()));
   if (fd.exec() == QDialog::Accepted) {
      borderFileBLineEdit->setText(fd.selectedFiles().at(0));
   }
}
