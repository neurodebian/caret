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

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTextEdit>

#include "BrainSet.h"
#include "GuiCaretTipsDialog.h"
#include "GuiMainWindow.h"
#include "PreferencesFile.h"
#include "QtUtilities.h"

#include "global_variables.h"

/**
 * constructor.
 */
GuiCaretTipsDialog::GuiCaretTipsDialog(QWidget* parent)
   : WuQDialog(parent)
{
   //
   // Set title of dialog
   //
   setWindowTitle("Caret Tips");
   
   //
   // text edit for displaying tips
   //
   tipsTextEdit = new QTextEdit;
   tipsTextEdit->setReadOnly(true);
   
   //
   // previous tip push button
   //
   QPushButton* previousTipPushButton = new QPushButton("Previous");
   previousTipPushButton->setAutoDefault(false);
   QObject::connect(previousTipPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPreviousTipPushButton()));
                    
   //
   // previous tip push button
   //
   QPushButton* nextTipPushButton = new QPushButton("Next");
   nextTipPushButton->setAutoDefault(false);
   QObject::connect(nextTipPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNextTipPushButton()));
                    
   //
   // previous tip push button
   //
   moreInfoTipPushButton = new QPushButton("More Info...");
   moreInfoTipPushButton->setAutoDefault(false);
   QObject::connect(moreInfoTipPushButton, SIGNAL(clicked()),
                    this, SLOT(slotMoreInfoPushButton()));
   
   //
   // Make buttons same size and place in layout
   //
   QtUtilities::makeButtonsSameSize(previousTipPushButton,
                                    nextTipPushButton,
                                    moreInfoTipPushButton);
   QHBoxLayout* tipsButtonsLayout = new QHBoxLayout;
   tipsButtonsLayout->addWidget(previousTipPushButton);
   tipsButtonsLayout->addWidget(nextTipPushButton);
   tipsButtonsLayout->addWidget(moreInfoTipPushButton);
   
   //
   // Group box and layout for tips and selection
   //
   QGroupBox* tipsGroupBox = new QGroupBox("Tips");
   QVBoxLayout* tipsLayout = new QVBoxLayout(tipsGroupBox);
   tipsLayout->addWidget(tipsTextEdit);
   tipsLayout->addLayout(tipsButtonsLayout);
   
   //
   // Show tips at caret startup check box
   //
   showTipsAtCaretStartupCheckBox = new QCheckBox("Show Tips After First Spec File Loaded");
   int dummy;
   bool startupFlag;
   PreferencesFile* preferencesFile = theMainWindow->getBrainSet()->getPreferencesFile();
   preferencesFile->getCaretTips(dummy, startupFlag);
   showTipsAtCaretStartupCheckBox->setChecked(startupFlag);
   QObject::connect(showTipsAtCaretStartupCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowTipsAtStartupCheckBox(bool)));
   
   //
   // Get the layout and add widgets to it
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(tipsGroupBox);
   dialogLayout->addWidget(showTipsAtCaretStartupCheckBox);
   
   //
   // Dialog buttons
   //
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
   buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(false);
   dialogLayout->addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(rejected()),
                    this, SLOT(close()));

   slotNextTipPushButton();
}

/**
 * destructor.
 */
GuiCaretTipsDialog::~GuiCaretTipsDialog()
{
}

/**
 * called when tips at startup toggled.
 */
void 
GuiCaretTipsDialog::slotShowTipsAtStartupCheckBox(bool showTips)
{
   bool dummy;
   int tipNumber;
   PreferencesFile* preferencesFile = theMainWindow->getBrainSet()->getPreferencesFile();
   preferencesFile->getCaretTips(tipNumber, dummy);
   
   preferencesFile->setCaretTips(tipNumber, showTips);
   try {
      preferencesFile->writeFile(preferencesFile->getFileName());
   }
   catch (FileException&) {
   }
}

/**
 * show a tip.
 */
void 
GuiCaretTipsDialog::showTip(const int tipNumber)
{
   QString tip, helpPage;
   caretTips.getTip(tipNumber, tip, helpPage);
   
   tipsTextEdit->setText(tip);
   
   moreInfoTipPushButton->setEnabled(helpPage.isEmpty() == false);
}

/**
 * called when previous tip button pressed.
 */
void 
GuiCaretTipsDialog::slotPreviousTipPushButton()
{
   int tipNumber;
   bool dummy;
   PreferencesFile* preferencesFile = theMainWindow->getBrainSet()->getPreferencesFile();
   preferencesFile->getCaretTips(tipNumber, dummy);
   
   tipNumber--;
   if (tipNumber < 0) {
      tipNumber = caretTips.getNumberOfCaretTips() - 1;
   }
   preferencesFile->setCaretTips(tipNumber, dummy);
   try {
      preferencesFile->writeFile(preferencesFile->getFileName());
   }
   catch (FileException&) {
   }
   
   showTip(tipNumber);
}

/**
 * called when next tip button pressed.
 */
void 
GuiCaretTipsDialog::slotNextTipPushButton()
{
   int tipNumber;
   bool dummy;
   PreferencesFile* preferencesFile = theMainWindow->getBrainSet()->getPreferencesFile();
   preferencesFile->getCaretTips(tipNumber, dummy);
   
   tipNumber++;
   if (tipNumber >= caretTips.getNumberOfCaretTips()) {
      tipNumber = 0;
   }
   
   preferencesFile->setCaretTips(tipNumber, dummy);
   try {
      preferencesFile->writeFile(preferencesFile->getFileName());
   }
   catch (FileException&) {
   }
   
   showTip(tipNumber);
}

/**
 * called when more info button pressed.
 */
void 
GuiCaretTipsDialog::slotMoreInfoPushButton()
{
   int tipNumber;
   bool dummy;
   PreferencesFile* preferencesFile = theMainWindow->getBrainSet()->getPreferencesFile();
   preferencesFile->getCaretTips(tipNumber, dummy);

   QString tip, helpPage;
   caretTips.getTip(tipNumber, tip, helpPage);
   
   if (helpPage.isEmpty() == false) {
      theMainWindow->showHelpViewerDialog(helpPage);
   }
}