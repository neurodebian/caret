
#ifndef __TEXT_FILE_H__
#define __TEXT_FILE_H__

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

#include "AbstractFile.h"

/// a simple text file
class TextFile : public AbstractFile {
   public:
      // constructor
      TextFile();
      
      // destructor
      ~TextFile();
      
      // copy constructor
      TextFile(const TextFile& tf);
      
      // assignment operator
      TextFile& operator=(const TextFile& tf);
      
      // get the text 
      QString getText() const;
      
      // set the text
      void setText(const QString& s);
      
      // append a line to the text (new line is also added)
      void appendLine(const QString& s);
      
      // clear the contents
      void clear();
      
      // returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      // compare a file for unit testing (returns true if "within tolerance")
      bool compareFileForUnitTesting(const AbstractFile* af,
                                             const float tolerance,
                                             QString& messageOut) const;

   protected:
      // copy helper used by copy constructor and assignment operator
      void copyHelperText(const TextFile& tf);
      
      // Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                        QDomElement& rootElement) throw (FileException);

      // Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
      // the text
      QString text;
};

#endif // __TEXT_FILE_H__
