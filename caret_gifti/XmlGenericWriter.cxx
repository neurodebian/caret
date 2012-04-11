#include <iostream>

#include <QStringList>
#include <QTextStream>

#include "StringUtilities.h"
#include "XmlGenericWriter.h"

/**
 * Constructor.
 *
 * @param writer - Writer to which XML is written.
 */
XmlGenericWriter::XmlGenericWriter(QTextStream& writerIn)
   : writer(writerIn) {
   indentationSpaces = 0;
   this->numberOfDecimalPlaces = 6;
   StringUtilities::setFloatDigitsRightOfDecimal(this->numberOfDecimalPlaces);
}

/**
 * Write the XML Start Document.
 *
 * @param xmlVersion - Version number, eg: "1.0".
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeStartDocument(QString xmlVersion) throw (FileException) {
   writer << ("<?xml version=\"" + xmlVersion + "\" encoding=\"UTF-8\"?>\n"
      "");
}

/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeStartDocument(const char* localName) throw(FileException) {
//   QString ln = localName;
//   this->writeStartDocument(ln);
//}

/**
 * Write the XML Start document.
 *
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeStartDocument() throw (FileException) {
   this->writeStartDocument("1.0");
}

/**
 * Write a DTD section.
 *
 * @param rootTag - the root tag of the XML document.
 * @param dtdURL - URL of DTD.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeDTD(QString rootTag, QString dtdURL) throw(FileException) {
   writer << ("<!DOCTYPE "
                + rootTag + " SYSTEM \""
                + dtdURL + "\">\n");
}

/**
 * Closes any start tags and writes corresponding end tags.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeEndDocument() throw(FileException) {
   while (this->elementStack.empty() == false) {
      writeEndElement();
   }

   writer.flush();
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementCharacters(const QString localName, const float f) {
   QString text = StringUtilities::fromNumber(f); //QString::number(f, 'f', this->numberOfDecimalPlaces);
   this->writeElementCharacters(localName, text);
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementCharacters(const QString localName, const float* values, const int num) {
   QStringList sl;
   for (int i = 0; i < num; i++) {
      sl << StringUtilities::fromNumber(values[i]); //QString::number(values[i], 'f', this->numberOfDecimalPlaces);
   }
   this->writeElementCharacters(localName, sl.join(" "));
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementCharacters(const QString localName, const int value) {
   QString text = QString::number(value);
   this->writeElementCharacters(localName, text);
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementCharacters(const QString localName, const int* values, const int num) {
   QStringList sl;
   for (int i = 0; i < num; i++) {
      sl << QString::number(values[i]);
   }
   this->writeElementCharacters(localName, sl.join(" "));
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCharacters(const char* localName, const char* text) {
//   this->writeElementCharacters(localName, text);
//}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementCharacters(QString localName, QString text)
                                                 throw(FileException) {
   this->writeIndentation();
   writer << ("<" + localName + ">");
   this->writeCharacters(text);
   writer << ("</" + localName + ">\n");
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCharacters(const char* localName, QString text)
//                                                 throw(FileException) {
//   QString ln = localName;
//   this->writeElementCharacters(ln, text);
//}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCData(const char* localName, const char* text) {
//   this->writeElementCData(localName, text);
//}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCData(const char* localName, QString text) {
//   this->writeElementCData(localName, text);
//}

/**
 * Write a CData section on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementCData(QString localName, QString text)
                                                 throw(FileException) {
   this->writeIndentation();
   writer << ("<" + localName + ">");
   this->writeCData(text);
   writer << ("</" + localName + ">\n");
}

/**
 * Write a CData section on one line with attributes.
 *
 * @param localName - local name of tag to write.
 * @param attributes - attribute for element.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementCData(QString localName,
                              XmlGenericWriterAttributes& attributes,
                              QString text)
                                                 throw(FileException) {
   this->writeIndentation();
   writer << ("<" + localName);
   int numAtts = attributes.getNumberOfAttributes();
   for (int i = 0; i < numAtts; i++) {
      writer << (" "
                   + attributes.getAttributeName(i)
                   + "=\""
                   + attributes.getAttributeValue(i)
                   + "\"");
   }
   writer << (">");
   this->writeCData(text);
   writer << ("</" + localName + ">\n");
}

/**
 * Write an element with no spacing between start and end tags.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeElementNoSpace(QString localName, QString text)
                                                 throw(FileException) {
   this->writeIndentation();
   writer << ("<" + localName + ">");
   writer << (text);
   writer << ("</" + localName + ">\n");
}

/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeStartElement(const char* localName) throw(FileException) {
//   QString ln = localName;
//   this->writeStartElement(ln);
//}
/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeStartElement(QString localName) throw(FileException) {
   this->writeIndentation();
   writer << ("<" + localName + ">\n");
   this->indentationSpaces++;
   this->elementStack.push(localName);
}

/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @param attributes - attributes for start tag
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeStartElement(QString localName,
                  XmlGenericWriterAttributes& attributes) throw(FileException) {
   this->writeIndentation();
   writer << ("<" + localName + " ");

   int attIndentSpaces = localName.length() + 2;
   QString attIndentString(attIndentSpaces, ' ');

   int numAtts = attributes.getNumberOfAttributes();
   for (int i = 0; i < numAtts; i++) {
      if (i > 0) {
         this->writeIndentation();
         writer << (attIndentString);
      }
      writer << (attributes.getAttributeName(i)
                   + "=\""
                   + attributes.getAttributeValue(i)
                   + "\"");
      if (i < (numAtts - 1)) {
         writer << ("\n");
      }
   }

   writer << (">\n");
   this->indentationSpaces++;
   this->elementStack.push(localName);
}

/**
 * Writes an end tag to the output.
 *
 * @throws IOException if an I/O error occurs
 */
void
XmlGenericWriter::writeEndElement() throw(FileException) {
   if (this->elementStack.empty()) {
      throw FileException("Unbalanced start/end element calls.");
   }

   QString localName = this->elementStack.pop();
   this->indentationSpaces--;
   this->writeIndentation();
   writer << ("</" + localName + ">\n");
}

/**
 * Writes a CData section.
 *
 * @param data - data to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeCData(QString data) throw(FileException) {
   writer << "<![CDATA[";
   this->writeCharacters(data);
   writer << "]]>";
}

/**
 * Writes text to the output.  Avoids unprintable characters which cause
 * problems with some XML parsers.
 *
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeCharacters(QString text) throw(FileException) {
   const ushort CARRIAGE_RETURN = 13;
   const ushort LINE_FEED = 10;
   const ushort TAB = 9;

   int num = text.length();
   for (int i = 0; i < num; i++) {
      QChar c = text[i];
      ushort u = c.unicode();

      bool printIt = true; //c.isPrint();

      if (u < 32) {
         printIt = false;
         
         if ((u == CARRIAGE_RETURN) ||
             (u == LINE_FEED) ||
             (u == TAB)) {
            printIt = true;
         }
      }
      if (printIt) {
         writer << c;
      }
      else {
         std::cout << "Unicode value of character not written: "
                   << u
                   << std::endl;
      }
   }
   //writer << (text);
}

/**
 * Writes text with indentation to the output.
 *
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
void
XmlGenericWriter::writeCharactersWithIndent(QString text) throw(FileException) {
   this->writeIndentation();
   writer << (text);
}

void
XmlGenericWriter::setNumberOfDecimalPlaces(int decimals) {
   this->numberOfDecimalPlaces = decimals;
}

/**
 * Write indentation spaces.
 *
 * @throws IOException if an I/O error occurs.
 */
void XmlGenericWriter::writeIndentation() throw(FileException) {
   if (this->indentationSpaces > 0) {
      QString sb(indentationSpaces * 3, ' ');
      writer << (sb);
   }
}


