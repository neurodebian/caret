
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

#include <QLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextBrowser>

#include "QtDialogWizard.h"
#include "QtUtilities.h"

/**
 * constructor.
 */
QtDialogWizard::QtDialogWizard(QWidget* parent,
                               const bool enableHelpSection,
                               Qt::WFlags f)
   : QDialog(parent, f)
{
   //
   // Widget stack for page widgets
   //
   pageStackedWidget = new QStackedWidget;
   
   //
   // Help browser
   //
   helpBrowser = NULL;
   if (enableHelpSection) {
      helpBrowser = new QTextBrowser;
   }
   
   //
   // prev push button
   //
   prevPushButton = new QPushButton("Prev");
   prevPushButton->setAutoDefault(false);
   QObject::connect(prevPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPrevPushButton()));
   
   //
   // next push button
   //
   nextPushButton = new QPushButton("Next");
   nextPushButton->setAutoDefault(false);
   QObject::connect(nextPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNextPushButton()));
   
   //
   // finish push button
   //
   finishPushButton = new QPushButton("Finish");
   finishPushButton->setAutoDefault(false);
   QObject::connect(finishPushButton, SIGNAL(clicked()),
                    this, SIGNAL(signalFinishButtonPressed()));
   
   //
   // close push button
   //
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(slotClosePushButton()));

   //
   // Make buttons same size
   //   
   QtUtilities::makeButtonsSameSize(prevPushButton,
                                    nextPushButton,
                                    finishPushButton,
                                    closePushButton);
   //
   // Layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(prevPushButton);
   buttonsLayout->addWidget(nextPushButton);
   buttonsLayout->addWidget(finishPushButton);
   buttonsLayout->addWidget(closePushButton);

   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addWidget(pageStackedWidget);
   if (helpBrowser != NULL) {
      dialogLayout->addWidget(helpBrowser);
   }
   dialogLayout->addLayout(buttonsLayout);
}

/**
 * destructor.
 */
QtDialogWizard::~QtDialogWizard()
{
}

/**
 * Initialize the dialog (call from derived classes constructor).
 */
void
QtDialogWizard::initializeDialog()
{
   //
   // Initialize the dialog
   //
   QWidget* firstWidget = pageStackedWidget->widget(0);
   if (firstWidget != NULL) {
      pageAboutToShow(firstWidget);
      pageStackedWidget->setCurrentWidget(firstWidget);
   }
   slotUpdatePageAndButtonValidity();
}

/**
 * update the dialog.
 */
void 
QtDialogWizard::updateDialog()
{
   slotUpdatePageAndButtonValidity();
}

/**
 * user should connect this to widgets that affect page and button validity.
 */
void 
QtDialogWizard::slotUpdatePageAndButtonValidity()
{
   //
   //  Disable all buttons and pages
   //
   prevPushButton->setEnabled(false);
   setNextButtonEnabled(false);
   setFinishButtonEnabled(false, finishPushButton->text());
   for (int i = 0; i < pageStackedWidget->count(); i++) {
      pageValidityFlags[pageStackedWidget->widget(i)] = false;
   }
   
   //
   // Let user update page and button validities
   //
   setValidPagesAndButtons(pageStackedWidget->currentWidget());
   
   //
   // Get index of current page
   //
   const int currentPageIndex = pageStackedWidget->currentIndex();
   
   //
   // Enable prev button if a page before the current is enabled
   // If finish button is NOT enabled,
   // enable next button if a page after the current is enabled
   //
   for (int i = 0; i < pageStackedWidget->count(); i++) {
      QWidget* w = pageStackedWidget->widget(i);
      if (i < currentPageIndex) {
         if (getPageValid(w)) {
            prevPushButton->setEnabled(true);
         }
      }
      else if (i > currentPageIndex) {
         if (finishPushButton->isEnabled() == false) {
            if (getPageValid(w)) {
               nextPushButton->setEnabled(true);
            }
         }
      }
   }
}

/**
 * add a page to the dialog.
 */
void 
QtDialogWizard::addPage(QWidget* pageWidget,
                        const QString& helpText)
{
   pageStackedWidget->addWidget(pageWidget);
   pageHelpText[pageWidget] = helpText;
   pageValidityFlags[pageWidget] = false;
}

/**
 * see if a page is valid.
 */
bool 
QtDialogWizard::getPageValid(QWidget* pageWidget) 
{
   return pageValidityFlags[pageWidget];
}
      
/**
 * set a page's validity  user should call this from setValidPagesAndButtons()
 */
void 
QtDialogWizard::setPageValid(QWidget* pageWidget,
                  const bool pageValidFlag)
{
   pageValidityFlags[pageWidget] = pageValidFlag;
}
                  
/**
 * show a page in the dialog.
 */
void 
QtDialogWizard::showPage(QWidget* pageWidget)
{
   //
   // Notify that page is about to be removed
   //
   pageAboutToChange(pageStackedWidget->currentWidget());
   
   //
   // Notify that page is about to show
   //
   pageAboutToShow(pageWidget);
   
   //
   // Show the widget
   //
   pageStackedWidget->setCurrentWidget(pageWidget);
   
   //
   // Update help
   //
   if (helpBrowser != NULL) {
      helpBrowser->setHtml(pageHelpText[pageWidget]);
   }
   
   //
   // Set page and button validity
   //
   slotUpdatePageAndButtonValidity();
}

/**
 * called when a page is about to show. user should override this
 */
void
QtDialogWizard::pageAboutToShow(QWidget* /*pageWidget*/)
{
   //
   // Let user override this
   //
}

/**
 * read from a page before going to next page. user should override this
 */
void
QtDialogWizard::pageAboutToChange(QWidget* /*pageWidget*/)
{
   //
   // Let user override this
   //
}

/**
 * set the next button enabled.  user should call this from setValidPagesAndButtons()
 */
void 
QtDialogWizard::setNextButtonEnabled(const bool enableIt)
{
   nextPushButton->setEnabled(enableIt);
}

/**
 * set the finish button enabled.  user should call this from setValidPagesAndButtons()
 */
void 
QtDialogWizard::setFinishButtonEnabled(const bool enableIt,
                                       const QString& finishButtonText)
{
   finishPushButton->setEnabled(enableIt);
   if (finishButtonText.isEmpty() == false) {
      finishPushButton->setText(finishButtonText);
   }
}
                                  
/**
 * called when next button pressed.
 */
void 
QtDialogWizard::slotNextPushButton()
{
   //
   // index of next page
   //
   int indx = pageStackedWidget->currentIndex() + 1;
   
   //
   // Search for a valid page
   //
   while (indx < pageStackedWidget->count()) {
      //
      // If next page valid, switch to it
      //
      QWidget* nextPage = pageStackedWidget->widget(indx);
      if (nextPage != NULL) {
         if (pageValidityFlags[nextPage]) {
            showPage(nextPage);
            break;
         }
      }
      //
      // Try next page
      //
      indx++;
   }
}

/**
 * called when prev button pressed.
 */
void 
QtDialogWizard::slotPrevPushButton()
{
   //
   // index of previous page
   //
   int indx = pageStackedWidget->currentIndex() - 1;
   
   //
   // Search for a valid page
   //
   while (indx >= 0) {
      //
      // If next page valid, switch to it
      //
      QWidget* prevPage = pageStackedWidget->widget(indx);
      if (prevPage != NULL) {
         if (pageValidityFlags[prevPage]) {
            showPage(prevPage);
            break;
         }
      }
      //
      // Try prev page
      //
      indx--;
   }
}

/**
 * called when close button pressed.
 */
void 
QtDialogWizard::slotClosePushButton()
{
   QDialog::close();
}