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

#include <QApplication>
#include <QCheckBox>
#include <QFile>
#include <QFileDialog>
#include <QFontDialog>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPushButton>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextStream>
#include <QToolButton>

#include "FileUtilities.h"
#include "QtMultipleInputDialog.h"
#include "QtTextFileEditorDialog.h"
#include "QtUtilities.h"

/**
 * The constructor.
 */
QtTextFileEditorDialog::QtTextFileEditorDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   findReplaceDialog = NULL;
   setAttribute(Qt::WA_DeleteOnClose);
   resize(400, 200);

   setWindowTitle("Text File Editor");
   
   QVBoxLayout* rows = new QVBoxLayout(this);
   rows->setMargin(5);

   //
   // Create the toolbar
   //
   QHBoxLayout* toolbar = new QHBoxLayout;
   rows->addLayout(toolbar);
      
   //
   // Create the text editor
   //
   textEditor = new QTextEdit;
   rows->addWidget(textEditor);
   textEditor->setWordWrapMode(QTextOption::NoWrap);
   
   //
   // Open toolbar button
   //
   QToolButton* openToolButton = new QToolButton;
   toolbar->addWidget(openToolButton);
   openToolButton->setText("Open");
   openToolButton->setToolTip(
                 "Open a file and load\n"
                 "it into the editor.");
   QObject::connect(openToolButton, SIGNAL(clicked()),
                    this, SLOT(slotFileOpen()));
   
   //
   // Save toolbar button
   //
   saveToolButton = new QToolButton;
   toolbar->addWidget(saveToolButton);
   saveToolButton->setText("Save");
   saveToolButton->setToolTip(
                 "Save the text in the\n"
                 "editor to a file.");
   QObject::connect(saveToolButton, SIGNAL(clicked()),
                    this, SLOT(slotFileSave()));
   
   //
   // Save As toolbar button
   //
   QToolButton* saveAsToolButton = new QToolButton;
   toolbar->addWidget(saveAsToolButton);
   saveAsToolButton->setText("Save As");
   saveAsToolButton->setToolTip(
                 "Save the text in the\n"
                 "editor to a file.");
   QObject::connect(saveAsToolButton, SIGNAL(clicked()),
                    this, SLOT(slotFileSaveAs()));
   
   //
   // Print toolbar button
   //
   QToolButton* printToolButton = new QToolButton;
   toolbar->addWidget(printToolButton);
   printToolButton->setText("Print");
   printToolButton->setToolTip(
                 "Print the text.");
   QObject::connect(printToolButton, SIGNAL(clicked()),
                    this, SLOT(slotPrint()));
   
   //
   // Space after save
   //
   QLabel* spaceAfterSave = new QLabel(" ");
   spaceAfterSave->setFixedSize(spaceAfterSave->sizeHint());
   toolbar->addWidget(spaceAfterSave);
   
   //
   // Copy toolbar button
   //
   QToolButton* copyToolButton = new QToolButton;
   toolbar->addWidget(copyToolButton);
   copyToolButton->setText("Copy");
   copyToolButton->setToolTip(
                 "Copy Selected Text.");
   QObject::connect(copyToolButton, SIGNAL(clicked()),
                    textEditor, SLOT(copy()));
   
   //
   // Cut toolbar button
   //
   QToolButton* cutToolButton = new QToolButton;
   toolbar->addWidget(cutToolButton);
   cutToolButton->setText("Cut");
   cutToolButton->setToolTip(
                 "Cut Selected Text.");
   QObject::connect(cutToolButton, SIGNAL(clicked()),
                    textEditor, SLOT(cut()));
   
   //
   // Paste toolbar button
   //
   QToolButton* pasteToolButton = new QToolButton;
   toolbar->addWidget(pasteToolButton);
   pasteToolButton->setText("Paste");
   pasteToolButton->setToolTip(
                 "Paste Text From Clipboard.");
   QObject::connect(pasteToolButton, SIGNAL(clicked()),
                    textEditor, SLOT(paste()));
   
   //
   // Space after paste
   //
   QLabel* spaceAfterPaste = new QLabel(" ");
   spaceAfterPaste->setFixedSize(spaceAfterPaste->sizeHint());
   toolbar->addWidget(spaceAfterPaste);
   
   //
   // Font toolbar button
   //
   QToolButton* fontToolButton = new QToolButton;
   toolbar->addWidget(fontToolButton);
   fontToolButton->setText("Font");
   fontToolButton->setToolTip(
                 "Set the font.");
   QObject::connect(fontToolButton, SIGNAL(clicked()),
                    this, SLOT(slotFont()));
   
   //
   // Space after fond
   //
   QLabel* spaceAfterFont = new QLabel(" ");
   spaceAfterFont->setFixedSize(spaceAfterFont->sizeHint());
   toolbar->addWidget(spaceAfterFont);
   
   //
   // Find toolbar button
   //
   QToolButton* findToolButton = new QToolButton;
   toolbar->addWidget(findToolButton);
   findToolButton->setText("Find");
   findToolButton->setToolTip(
                 "Find in Text.");
   QObject::connect(findToolButton, SIGNAL(clicked()),
                    this, SLOT(slotFind()));
   
   //
   // Goto toolbar button
   //
   QToolButton* gotoToolButton = new QToolButton;
   toolbar->addWidget(gotoToolButton);
   gotoToolButton->setText("GoTo");
   gotoToolButton->setToolTip(
                 "Go to a line.");
   QObject::connect(gotoToolButton, SIGNAL(clicked()),
                    this, SLOT(slotGoTo()));
   
   //
   // Space after paste
   //
   QLabel* spaceAfterGoTo = new QLabel(" ");
   spaceAfterGoTo->setFixedSize(spaceAfterGoTo->sizeHint());
   toolbar->addWidget(spaceAfterGoTo);
   
   //
   // Wrap toolbar button
   //
   wrapToolButton = new QToolButton;
   wrapToolButton->setText("Wrap");
   wrapToolButton->setCheckable(true);
   wrapToolButton->setToolTip(
                 "Toggles text wrapping.");
   QObject::connect(wrapToolButton, SIGNAL(clicked()),
                    this, SLOT(slotWrap()));
   toolbar->addWidget(wrapToolButton);
   
   //
   // Pack toolbar items
   //
   toolbar->setStretchFactor(openToolButton, 0);
   toolbar->setStretchFactor(saveToolButton, 0);
   toolbar->setStretchFactor(spaceAfterSave, 0);
   toolbar->setStretchFactor(copyToolButton, 0);
   toolbar->setStretchFactor(cutToolButton, 0);
   toolbar->setStretchFactor(pasteToolButton, 0);
   toolbar->setStretchFactor(spaceAfterPaste, 0);
   toolbar->setStretchFactor(findToolButton, 0);
   toolbar->setStretchFactor(gotoToolButton, 0);
   toolbar->setStretchFactor(spaceAfterGoTo, 0);
   toolbar->setStretchFactor(wrapToolButton, 0);
   toolbar->addStretch();
//   QLabel* spaceLabel = new QLabel(" ");
//   toolbar->setStretchFactor(spaceLabel, 1000);
   
   //
   // Close Buttons
   //
   QHBoxLayout* buttonsLayout2 = new QHBoxLayout;
   rows->addLayout(buttonsLayout2);
   buttonsLayout2->setSpacing(3);
   
   //
   // Close Button
   //
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout2->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotClose()));

   resize(500, 300);
   
   currentFileFilter = "Any File (*)";
   
   paragraphNum = 0;
   paragraphChar = 0;
   previousLineNumber = 0;

   QObject::connect(textEditor, SIGNAL(textChanged()),
                    this, SLOT(slotEnableSaveButton()));
   slotEnableSaveButton();
   
   wrapToolButton->setChecked(false);
   slotWrap();
}

/**
 * The destructor.
 */
QtTextFileEditorDialog::~QtTextFileEditorDialog()
{
}

/**
 * called when print button is pressed.
 */
void 
QtTextFileEditorDialog::slotPrint()
{
   QPrinter printer;
   QPrintDialog* printDialog = new QPrintDialog(&printer, this);
   if (printDialog->exec() == QPrintDialog::Accepted) {
      textEditor->document()->print(&printer);
   }
}

/**
 * called when font button pressed.
 */
void 
QtTextFileEditorDialog::slotFont()      
{
   bool ok;
   QFont font = QFontDialog::getFont(&ok, textEditor->font(), this);
   if (ok) {
      textEditor->setFont(font);
   }
}

/**
 * called to turn on/off text wrapping.
 */
void 
QtTextFileEditorDialog::slotWrap()
{
   if (wrapToolButton->isChecked()) {
      textEditor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
   }
   else {
      textEditor->setWordWrapMode(QTextOption::NoWrap);
   }
}
      
/**
 * called when find button pressed.
 */
void 
QtTextFileEditorDialog::slotFind()
{
   if (findReplaceDialog == NULL) {
      findReplaceDialog = new QtTextFileEditorSearchDialog(textEditor,
                                                           this);
   }
   findReplaceDialog->show();
   findReplaceDialog->activateWindow();
}

/**
 * called when go to button pressed.
 */
void 
QtTextFileEditorDialog::slotGoTo()
{
   bool ok = false;
   const int lineNum = QInputDialog::getInteger(this,
                                                "Go To Line Number",
                                                "Go To Line Number",
                                                previousLineNumber,
                                                0,
                                                1000000000,
                                                1,
                                                &ok);
   if (ok) {
      previousLineNumber = lineNum;
      QTextCursor tc = textEditor->textCursor();
      tc.movePosition(QTextCursor::Start);
      textEditor->setTextCursor(tc);
      tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNum - 1);
      textEditor->setTextCursor(tc);
      //textEditor->setCursorPosition(previousLineNumber - 1, 0);
   }
}
      
/**
 * called enable save button.
 */
void 
QtTextFileEditorDialog::slotEnableSaveButton()
{
   saveToolButton->setEnabled(false);
   if (filename.isEmpty() == false) {
      if (textEditor->document()->isModified()) {
         saveToolButton->setEnabled(true);
      }
   }
}      

/**
 * called when open button is pressed.
 */
void 
QtTextFileEditorDialog::slotFileOpen()
{
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setDirectory(QDir::currentPath());
   fd.setWindowTitle("Choose File");
   fd.setFileMode(QFileDialog::ExistingFile); 
   fd.setAcceptMode(QFileDialog::AcceptOpen);
   QStringList filters;
   filters << "Any File (*)";
   filters << "Comma Separated Value File (*.csv)";
   filters << "Text File (*.txt)";
   const QString richTextFilter("Rich Text (*.rtf)");
   // RTF does not work //fd.addFilter(richTextFilter);
   if (additionalFileFilters.empty() == false) {
      for (QStringList::Iterator it = additionalFileFilters.begin(); it != additionalFileFilters.end(); ++it) {
         filters << *it;
      }
   }
   fd.setFilters(filters);
   fd.selectFilter(currentFileFilter);
   if (fd.exec() == QDialog::Accepted) {
      currentFileFilter = fd.selectedFilter();
      loadFile(fd.selectedFiles().at(0), (currentFileFilter == richTextFilter));
   }
}

/**
 * load a file (returns true if an error occurs)
 */
void 
QtTextFileEditorDialog::loadFile(const QString& fileNameIn,
                                 const bool richTextFlag)
{
   QFile file(fileNameIn);
   if (file.open(QIODevice::ReadOnly) == false) {
      QString msg;
      msg = "Unable to open: " + fileNameIn;
      QMessageBox::critical(this, "ERROR", msg, "OK");
      return;
   }
   else {
      QTextStream stream(&file);
      
      textEditor->clear();
      if (richTextFlag) {
         textEditor->setHtml(stream.readAll());
      }
      else {
         textEditor->setPlainText(stream.readAll());
      }
      
      
      file.close();
      
      textEditor->document()->setModified(false);
      
      filename = fileNameIn;
   }
   
   QString caption("Text File Editor - ");
   caption.append(FileUtilities::basename(filename));
   setWindowTitle(caption);
   
   paragraphNum = 0;
   paragraphChar = 0;
   previousLineNumber = 0;
}
      
/**
 * called when save button is pressed.
 */
void 
QtTextFileEditorDialog::slotFileSaveAs()
{
   QFileDialog fd(this);
   fd.setModal(true);
   fd.setAcceptMode(QFileDialog::AcceptSave);
   fd.setWindowTitle("Choose File");
   fd.setFileMode(QFileDialog::AnyFile); 
   QStringList filters;
   filters << "Any File (*)";
   filters << "Comma Separated Value File (*.csv)";
   filters << "Text File (*.txt)";
   if (additionalFileFilters.empty() == false) {
      for (QStringList::Iterator it = additionalFileFilters.begin(); it != additionalFileFilters.end(); ++it) {
         filters << *it;
      }
   }
   fd.setFilters(filters);
   fd.selectFilter(currentFileFilter);
   fd.setDirectory(FileUtilities::dirname(filename));
   fd.selectFile(FileUtilities::basename(filename));
   
   if (fd.exec() == QDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         filename = fd.selectedFiles().at(0);
         saveFile(filename);
      }
   }
}

/**
 * save a file.
 */
void 
QtTextFileEditorDialog::saveFile(const QString& name)
{
   QFile file(name);
   if (file.open(QIODevice::WriteOnly) == false) {
      QString msg;
      msg = "Unable to open for writing: " + name;
      QApplication::beep();
      QMessageBox::critical(this, "ERROR", msg, "OK");
      return;
   }
   else {
      QTextStream stream(&file);
      stream << textEditor->toPlainText();
      file.close();
      
      textEditor->document()->setModified(false);
   }
}

/**
 * save a file.
 */
void 
QtTextFileEditorDialog::slotFileSave()
{
   if (filename.isEmpty()) {
      QMessageBox::critical(this, "ERROR", "File name is empty, use \"Save As\".", "OK");
      return;
   }
   
   if (QFile::exists(filename)) {
      QString msg(FileUtilities::basename(filename));
      msg += "already exists.\n"
             "Do you want to replace it?";
      if (QMessageBox::warning(this, "Confirm", msg, "Yes", "No") != 0) {
         return;
      }
   }
   saveFile(filename);
}
      
/**
 * called to close the text file editor.
 */
void 
QtTextFileEditorDialog::slotClose()
{
   if (textEditor->document()->isModified()) {
      QApplication::beep();
      if (QMessageBox::question(this, "File Changed Warning",
          "File in editor has been modified but not saved.\n"
          "Are you sure you want to close the text editor?",
          "Yes, Close Editor", "Cancel") == 1) {
         return;
      }
   }
   
   QDialog::close();
}

//=====================================================================================
// FIND DIALOG
//=====================================================================================

/**
 * constructor.
 */
QtTextFileEditorSearchDialog::QtTextFileEditorSearchDialog(QTextEdit* editorIn,
                                                           QWidget* parent)
   : QDialog(parent)
{
   editor = editorIn;
   
   //
   // Minimum width for line edit
   //
   const int minLineEditWidth = 300;
   //
   // Find and replace line edits and labels
   //
   QLabel* findLabel = new QLabel("Find");
   findLineEdit = new QLineEdit;
   findLineEdit->setMinimumWidth(minLineEditWidth);
   QLabel* replaceLabel = new QLabel("Replace");
   replaceLineEdit = new QLineEdit;
   replaceLineEdit->setMinimumWidth(minLineEditWidth);
   
   //
   // next push button
   //
   QPushButton* nextPushButton = new QPushButton("Next");
   nextPushButton->setAutoDefault(false);
   QObject::connect(nextPushButton, SIGNAL(clicked()),
                    this, SLOT(slotNextPushButton()));
   
   //
   // previous push button
   //
   QPushButton* previousPushButton = new QPushButton("Previous");
   previousPushButton->setAutoDefault(false);
   QObject::connect(previousPushButton, SIGNAL(clicked()),
                    this, SLOT(slotPreviousPushButton()));
   
   //
   // replace push button
   //
   QPushButton* replacePushButton = new QPushButton("Replace");
   replacePushButton->setAutoDefault(false);
   QObject::connect(replacePushButton, SIGNAL(clicked()),
                    this, SLOT(slotReplacePushButton()));
   
   //
   // replace and find push button
   //
   QPushButton* replaceAndFindPushButton = new QPushButton("Replace and Find");
   replaceAndFindPushButton->setAutoDefault(false);
   QObject::connect(replaceAndFindPushButton, SIGNAL(clicked()),
                    this, SLOT(slotReplaceAndFindPushButton()));
   
   //
   // replace push button
   //
   QPushButton* replaceAllPushButton = new QPushButton("Replace All");
   replaceAllPushButton->setAutoDefault(false);
   QObject::connect(replaceAllPushButton, SIGNAL(clicked()),
                    this, SLOT(slotReplaceAllPushButton()));
                    
   //
   // Make all of the buttons the same size
   //
   QtUtilities::makeButtonsSameSize(nextPushButton,
                                    previousPushButton,
                                    replacePushButton,
                                    replaceAndFindPushButton,
                                    replaceAllPushButton);
                                    
   //
   // case sensitive check box
   //
   caseSensitiveCheckBox = new QCheckBox("Case Sensitive");
   
   //
   // Grid layout for search controls
   //
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(findLabel, 0, 0);
   gridLayout->addWidget(findLineEdit, 0, 1);
   gridLayout->addWidget(replaceLabel, 1, 0);
   gridLayout->addWidget(replaceLineEdit, 1, 1);
   gridLayout->addWidget(nextPushButton, 0, 2);
   gridLayout->addWidget(previousPushButton, 1, 2);
   gridLayout->addWidget(replacePushButton, 2, 2);
   gridLayout->addWidget(replaceAndFindPushButton, 3, 2);
   gridLayout->addWidget(replaceAllPushButton, 4, 2);
   gridLayout->addWidget(caseSensitiveCheckBox, 2, 0, 1, 2, Qt::AlignLeft);
   
   //
   // Close button
   //
   QPushButton* closePushButton = new QPushButton("Close");
   closePushButton->setAutoDefault(false);
   closePushButton->setFixedSize(closePushButton->sizeHint());
   QObject::connect(closePushButton, SIGNAL(clicked()),
                    this, SLOT(close()));
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->addWidget(closePushButton);
                    
   //
   // Layout for dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->addLayout(gridLayout);
   dialogLayout->addLayout(buttonsLayout);
}

/**
 * destructor.
 */
QtTextFileEditorSearchDialog::~QtTextFileEditorSearchDialog()
{
}

/**
 * called to search for text.
 */
void 
QtTextFileEditorSearchDialog::searchForText(const bool searchBackwards)
{
   QTextDocument::FindFlags findFlags;
   if (caseSensitiveCheckBox->isChecked()) {
      findFlags |= QTextDocument::FindCaseSensitively;
   }
   if (searchBackwards) {
      findFlags |= QTextDocument::FindBackward;
   }
   
   previousSearchText = findLineEdit->text();
   if (previousSearchText.isEmpty() == false) {
      const bool textFound = editor->find(previousSearchText, findFlags);
      if (textFound == false) {
         QString msg("Text not found, search from beginning of file ?");
         if (searchBackwards) {
            msg = "Text not found, search from end of file ?";
         }
         if (QMessageBox::question(this, "Question", msg, "Yes", "No") == 0) {
            QTextCursor tc = editor->textCursor();
            tc.movePosition(QTextCursor::Start);
            if (searchBackwards) {
               tc.movePosition(QTextCursor::End);
            }
            editor->setTextCursor(tc);
            editor->find(previousSearchText, findFlags);   
         }
      }
   }
}
      
/**
 * called when next button pressed.
 */
void 
QtTextFileEditorSearchDialog::slotNextPushButton()
{
   searchForText(false);
}

// called when previous button pressed
void 
QtTextFileEditorSearchDialog::slotPreviousPushButton()
{
   searchForText(true);
}
      
/**
 * called to replace text (returns true if text was found and replaced).
 */
bool 
QtTextFileEditorSearchDialog::replaceText()
{
   QTextCursor tc = editor->textCursor();
   if (tc.hasSelection()) {
      tc.removeSelectedText();
      tc.insertText(replaceLineEdit->text());
      return true;
   }
   return false;
}
      
/**
 * called when replace button pressed.
 */
void 
QtTextFileEditorSearchDialog::slotReplacePushButton()
{
   replaceText();
}

/**
 * called when replace & find button pressed.
 */
void 
QtTextFileEditorSearchDialog::slotReplaceAndFindPushButton()
{
   if (replaceText()) {
      slotNextPushButton();
   }
}

/**
 * called when replace all button pressed.
 */
void 
QtTextFileEditorSearchDialog::slotReplaceAllPushButton()
{
   previousSearchText = findLineEdit->text();
   if (previousSearchText.isEmpty() == false) {
      QTextDocument::FindFlags findFlags;
      if (caseSensitiveCheckBox->isChecked()) {
         findFlags |= QTextDocument::FindCaseSensitively;
      }

      //
      // Move to start of file
      //
      QTextCursor tc = editor->textCursor();
      tc.movePosition(QTextCursor::Start);
      editor->setTextCursor(tc);
      
      while (editor->find(previousSearchText, findFlags)) {
         editor->cut();
         editor->insertPlainText(replaceLineEdit->text());
      }
   }
}
