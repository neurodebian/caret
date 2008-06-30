
#ifndef __COMMAND_CARET_HELP_CREATE_HTML_INDEX_FILE_H__
#define __COMMAND_CARET_HELP_CREATE_HTML_INDEX_FILE_H__

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

#include "CommandBase.h"

class QTextStream;

/// class for generating an index to the caret help files
class CommandCaretHelpCreateHtmlIndexFile : public CommandBase {
   public:
      // constructor 
      CommandCaretHelpCreateHtmlIndexFile();
      
      // destructor
      ~CommandCaretHelpCreateHtmlIndexFile();
      
      // get full help information
      QString getHelpInformation() const;
      
      // get the script builder parameters
      virtual void getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const;
      
   protected:
      // execute the command
      void executeCommand() throw (BrainModelAlgorithmException,
                                   CommandException,
                                   FileException,
                                   ProgramParametersException,
                                   StatisticException);
                                   
      // get the title from an HTML page
      QString getHtmlPageTitle(const QString& directoryName,
                               const QString& fileName); 
      
      // add to the output file
      void addHtml(QTextStream& streamOut,
                   const QString& htmlStartTag,
                   const QString& htmlEndTag,
                   const QString& text,
                   const int indentation,
                   const bool addLineBreakFlag = false);
                   
      // convert directory name to title
      QString convertDirectoryNameToTitle(const QString& dirName);

};

#endif // __COMMAND_CARET_HELP_CREATE_HTML_INDEX_FILE_H__

