
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

#include <map>
#include <vector>

#include <QDialog>

class QPushButton;
class QStackedWidget;
class QTextBrowser;

/// a wizard dialog created since the A**holes at Trolltech got rid of QWizard in QT4
class QtDialogWizard : public QDialog {
   Q_OBJECT
   
   public:
      // constructor
      QtDialogWizard(QWidget* parent = 0,
                     const bool enableHelpSection = false,
                     Qt::WFlags f = 0);
      
      // destructor
      ~QtDialogWizard();
   
      // update the dialog
      virtual void updateDialog();
      
   signals:
      // emitted when finish button is pressed
      void signalFinishButtonPressed();
      
   protected slots:
      // user should connect this to widgets that affect page and button validity
      void slotUpdatePageAndButtonValidity();
      
   protected:
      // Initialize the dialog (call from derived classes constructor).
      void initializeDialog();

      // add a page to the dialog
      void addPage(QWidget* pageWidget,
                   const QString& helpText = "");
      
      // see if a page is valid
      bool getPageValid(QWidget* pageWidget);
      
      // set a page's validity user should call this from setValidPagesAndButtons()
      void setPageValid(QWidget* pageWidget,
                        const bool pageValidFlag);
                        
      // called when a page is about to show user should override this
      virtual void pageAboutToShow(QWidget* pageWidget);
      
      // read from a page before going to next page user should override this
      virtual void pageAboutToChange(QWidget* pageWidget);
      
      // set the pages that are valid and buttons that are valid (all invalid when this called)
      virtual void setValidPagesAndButtons(QWidget* currentWidget) = 0;
      
      // set the finish button enabled  user should call this from setValidPagesAndButtons()
      void setFinishButtonEnabled(const bool enableIt,
                                  const QString& finishButtonText = "Finish");
                                  
   protected slots:
      // called when next button pressed
      void slotNextPushButton();
      
      // called when prev button pressed
      void slotPrevPushButton();
      
      // called when close button pressed
      void slotClosePushButton();
      
   private:
      // set the next button enabled user should call this from setValidPagesAndButtons()
      virtual void setNextButtonEnabled(const bool enableIt);
      
      // show a page in the dialog
      void showPage(QWidget* pageWidget);
      
      /// stacked widget containing the pages
      /// the page widgets
      QStackedWidget* pageStackedWidget;
      
      /// help text for a page
      std::map<QWidget*,QString> pageHelpText;
      
      /// the page widget validity flags
      std::map<QWidget*,bool> pageValidityFlags;

      /// the help section
      QTextBrowser* helpBrowser;
      
      /// the prev push button
      QPushButton* prevPushButton;
      
      /// the next pushbutton
      QPushButton* nextPushButton;
      
      /// the finish push button
      QPushButton* finishPushButton;

};
