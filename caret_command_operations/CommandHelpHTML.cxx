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

#include <QDateTime>

#include "CaretVersion.h"
#include "CommandHelpHTML.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandHelpHTML::CommandHelpHTML()
   : CommandBase("-help-html",
                 "HELP WRITE TO HTML FILE")
{
}

/**
 * destructor.
 */
CommandHelpHTML::~CommandHelpHTML()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandHelpHTML::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Output HTML File Name", FileFilters::getHTMLFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandHelpHTML::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<output-html-file-name>\n"
       + indent9 + "\n"
       + indent9 + "   Print the help information to an HTML file.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandHelpHTML::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Store the commands
   //
   std::vector<CommandBase*> commands;
   CommandBase::getAllCommandsSortedBySwitch(commands);
   const int numCommands = static_cast<int>(commands.size());
   
   //
   // Get name of output file
   // 
   const QString htmlFileExtension(".html");
   QString htmlFileName = parameters->getNextParameterAsString("PDF File Name");
   if (htmlFileName.endsWith(htmlFileExtension) == false) {
      htmlFileName += htmlFileExtension;
   }
   
   TextFile htmlFile;
   
   htmlFile.appendLine("<html>");
   htmlFile.appendLine("<body>");
   
   htmlFile.appendLine("<head>");
   htmlFile.appendLine("<title>Caret Command Help</title>");
   htmlFile.appendLine("</head>");
   
   htmlFile.appendLine("<h1>Caret Command Manual</h1>");
   htmlFile.appendLine("<h2>Version " 
                       + CaretVersion::getCaretVersionAsString()
                       + "</h2>");
   htmlFile.appendLine("<h2>Date Printed "
                       + QDateTime::currentDateTime().toString("MMM d, yyyy")
                       + "</h2>");
   htmlFile.appendLine("<p></p>");
   htmlFile.appendLine("<p></p>");
   
   htmlFile.appendLine(convertToHTML(getGeneralHelpInformation()));
   
   if (numCommands > 0) {
      std::vector<QString> anchorNames(numCommands, "");
      
      for (int i = 0; i < numCommands; i++) {
         const CommandBase* command = commands[i];
         const QString anchorName = command->getShortDescription().replace(' ', '_');
         anchorNames[i] = anchorName;
         
         htmlFile.appendLine("<a href=\"#"
                             + anchorName
                             + "\">"
                             + command->getShortDescription() 
                             + "</a><br></br>");
      }
   
      for (int i = 0; i < numCommands; i++) {
         if (i > 0) {
            htmlFile.appendLine("<hr=2></hr>");
         }
         
         const CommandBase* command = commands[i];
         QString htmlString = convertToHTML(command->getHelpInformation());
/*
         htmlString = htmlString.replace(' ', "&nbsp;");
         htmlString = htmlString.replace('<', "&lt;");
         htmlString = htmlString.replace('>', "&gt;");
         htmlString = htmlString.replace("\n", "<br></br>");  // do after less than/greater than conversion
*/
         htmlFile.appendLine("<a name=\""
                             + anchorNames[i]
                             + "\"></a>"
                             + htmlString);
      }
   }

   htmlFile.appendLine("</body>");
   htmlFile.appendLine("</html>");

   htmlFile.writeFile(htmlFileName);
}

/**
 * convert a string to HTML.
 */
QString 
CommandHelpHTML::convertToHTML(const QString& sin) const
{
   QString s = sin;
   s = s.replace(' ', "&nbsp;");
   s = s.replace('<', "&lt;");
   s = s.replace('>', "&gt;");
   s = s.replace("\n", "<br></br>");  // do after less than/greater than conversion
   return s;
}

      

