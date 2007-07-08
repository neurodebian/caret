
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

#include "SpecFile.h"
#include "TextFile.h"

/**
 * constructor.
 */
TextFile::TextFile()
   : AbstractFile("Text File",
                  SpecFile::getTextFileExtension(),
                  false,
                  FILE_FORMAT_ASCII,
                  FILE_IO_READ_AND_WRITE)
{
   clear();
}

/**
 * copy constructor.
 */
TextFile::TextFile(const TextFile& tf)
   : AbstractFile(tf)
{
   copyHelperText(tf);
}

/**
 * assignment operator.
 */
TextFile& 
TextFile::operator=(const TextFile& tf)
{
   if (&tf != this) {
      AbstractFile::operator=(tf);
      copyHelperText(tf);
   }
   return *this;
}
      
/**
 * copy helper used by copy constructor and assignment operator.
 */
void 
TextFile::copyHelperText(const TextFile& tf)
{
   setFileName("");
   text = tf.text;
   setModified();
}
      
/**
 * destructor.
 */
TextFile::~TextFile()
{
   clear();
}

/**
 * set the text.
 */
void 
TextFile::setText(const QString& s)
{
   text = s;
   setModified();
}

/**
 * append a line to the text (new line is also added).
 */
void 
TextFile::appendLine(const QString& s)
{
   text += s;
   text += "\n";
   setModified();
}

/**
 * get the text.
 */
QString 
TextFile::getText() const 
{ 
   return text; 
}
      
/**
 * clear the contents.
 */
void 
TextFile::clear()
{
   AbstractFile::clearAbstractFile();
   text = "";
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
TextFile::empty() const
{
   return (text.isEmpty());
}

/**
 * compare a file for unit testing (returns true if "within tolerance").
 */
bool 
TextFile::compareFileForUnitTesting(const AbstractFile* af,
                                    const float /*tolerance*/,
                                    QString& messageOut) const
{
   const TextFile* tf = dynamic_cast<const TextFile*>(af);
   if (tf == NULL) {
      messageOut = "File for comparison is not a text file.";
      return false;
   }
   const bool theSame = (text == tf->text);
   return theSame;
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
TextFile::readFileData(QFile& /*file*/,
                       QTextStream& stream,
                       QDataStream& /*binStream*/,
                       QDomElement& /*rootElement*/) throw (FileException)
{
   text = stream.readAll();
}

/**
 * Write the file's data (header has already been written).
 */
void 
TextFile::writeFileData(QTextStream& stream,
                        QDataStream& /*binStream*/,
                        QDomDocument& /*xmlDoc*/,
                        QDomElement& /*rootElement*/) throw (FileException)
{
   stream << text;
}
