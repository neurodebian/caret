
#ifndef __GIFTI_LABEL_TABLE_H__
#define __GIFTI_LABEL_TABLE_H__

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

#include <QString>
#include <vector>

#include "FileException.h"
#include "XmlGenericWriter.h"
class ColorFile;
class QTextStream;
class StringTable;


/// table for label indices and names
class GiftiLabelTable {
   public:
      // constructor
      GiftiLabelTable();
      
      // copy constructor
      GiftiLabelTable(const GiftiLabelTable& nlt);
      
      // destructor
      ~GiftiLabelTable();

      // assignment operator
      GiftiLabelTable& operator=(const GiftiLabelTable& nlt);
      
      // add a label (returns the label's index)
      int addLabel(const QString& labelName);
      
      // append a label table to this one
      void append(const GiftiLabelTable& nlt,
                  const std::vector<int>* labelsWithTheseIndicesOnly = NULL);

#ifdef CARET_FLAG
      // assign colors to the labels
      void assignColors(const ColorFile& colorFile);

      // get colors from the labels
      void addColorsToColorFile(ColorFile& colorFile);

      // Create a label from each of the colors in a color file
      void createLabelsFromColors(const ColorFile& colorFile);
#endif //CARET_FLAG

      // clear out the labels
      void clear();
      
      // delete a label
      void deleteLabel(const int labelIndex);
      
      // are there labels ?
      bool empty() const { return labels.empty(); }
      
      // get the index of a label
      int getLabelIndex(const QString& labelName) const;
      
      // get label using its index
      QString getLabel(const int indx) const;
      
      // get the label that best matches as a substring 
      // in the beginning of the given name
      int getBestMatchingLabelIndex(const QString& name) const;
    
      // get all labels
      void getAllLabels(std::vector<QString>& labelsOut) const;
           
      /// get the number of labels
      int getNumberOfLabels() const { return labels.size(); }
      
      // set the label for a specified index (index must be >= 0)
      void setLabel(const int indx,
                    const QString& labelName);
        
      // set a label's color file index
      void setColorFileIndex(const int indx,
                             const int colorFileIndex);
      
      // get a labels color file index
      int getColorFileIndex(const int indx) const;

      // get label enabled using its index
      bool getLabelEnabled(const int indx) const;
      
      // set label enabled for a specified index (index must be >= 0)
      void setLabelEnabled(const int indx,
                           const bool b);
      
      // set all labels enabled
      void setAllLabelsEnabled(const bool b);

      // Set LabelTable had colors when it was read
      void setHadColorsWhenRead(bool b) { this->hadColorsWhenRead = b; }

      // Get LabelTable had colors when it was read
      bool getHadColorsWhenRead() const { return this->hadColorsWhenRead; }

      // get teh color components as floats ranging 0.0 to 1.0
      void getColorFloat(const int indx,
                         float& red,
                         float& green,
                         float& blue,
                         float& alpha) const;

      // set the color components from floats ranging 0.0 to 1.0
      void setColorFloat(const int indx,
                         float red,
                         float green,
                         float blue,
                         float alpha);

      // get teh color components
      void getColor(const int indx,
                    unsigned char& red,
                    unsigned char& green,
                    unsigned char& blue,
                    unsigned char& alpha) const;

      // set the color components
      void setColor(const int indx,
                    unsigned char red,
                    unsigned char green,
                    unsigned char blue,
                    unsigned char alpha);

      // write the label table
      void writeAsXML(QTextStream& stream,
                      const int indentOffset) const;

      // write as Caret6 XML
      void writeAsXML(XmlGenericWriter& xmlWriter) const;

      /// write the data into a StringTable
      void writeDataIntoStringTable(StringTable& table) const;
      
      /// read the data from a StringTable
      void readDataFromStringTable(const StringTable& table) throw (FileException);
      
      /// get the default color components
      static void getDefaultColor(unsigned char& redOut,
                                  unsigned char& greenOut,
                                  unsigned char& blueOut,
                                  unsigned char& alphaOut) {
         redOut   = 255;
         greenOut = 255;
         blueOut  = 255;
         alphaOut = 255;
      }

      /// get the default color components as floats
      static void getDefaultColorFloat(float& redOut,
                                       float& greenOut,
                                       float& blueOut,
                                       float& alphaOut) {
         unsigned char r, g, b, a;
         getDefaultColor(r, g, b, a);
         redOut   = ((float)r / 255.0f);
         greenOut = ((float)g / 255.0f);
         blueOut  = ((float)b / 255.0f);
         alphaOut = ((float)a / 255.0f);
      }

   protected:
      /// class used for storing a label and its corresponding color index
      class LabelData {
         public:
            /// constructor
            LabelData(const QString& labelNameIn) {
               labelName = labelNameIn;
               colorFileIndex = -1;
               labelEnabled = true;
               GiftiLabelTable::getDefaultColor(red,
                                                green,
                                                blue,
                                                alpha);
            };
            
            /// constructor
            LabelData(const QString& labelNameIn,
                      const unsigned char redIn,
                      const unsigned char greenIn,
                      const unsigned char blueIn,
                      const unsigned char alphaIn = 255) {
               labelName = labelNameIn;
               colorFileIndex = -1;
               labelEnabled = true;
               red = redIn;
               green = greenIn;
               blue = blueIn;
               alpha = alphaIn;
            };

            /// destructor
            ~LabelData() { }
            
            /// get the label name
            QString getLabelName() const { return labelName; }
            
            /// set the label name
            void setLabelName(const QString& labelNameIn) { labelName = labelNameIn; }
            
            /// get the color file index
            int getColorFileIndex() const { return colorFileIndex; }
            
            /// set the color file index
            void setColorFileIndex(const int indx) { colorFileIndex = indx; }
            
            /// set the label is enabled
            void setLabelEnabled(const bool b) { labelEnabled = b; }
            
            /// get the label is enabled
            bool getLabelEnabled() const { return labelEnabled; }
            
            /// get the color components
            void getColor(unsigned char& redOut,
                          unsigned char& greenOut,
                          unsigned char& blueOut,
                          unsigned char& alphaOut) const {
               redOut   = red;
               greenOut = green;
               blueOut  = blue;
               alphaOut = alpha;
            }

            /// set the color components
            void setColor(const unsigned char redIn,
                          const unsigned char greenIn,
                          const unsigned char blueIn,
                          const unsigned char alphaIn) {
               red   = redIn;
               green = greenIn;
               blue  = blueIn;
               alpha = alphaIn;
            }
            

            /// get the color components as floats ranging 0.0 to 1.0
            void getColorFloat(float& redOut,
                               float& greenOut,
                               float& blueOut,
                               float& alphaOut) const {
               redOut   = ((float)red / 255.0f);
               greenOut = ((float)green / 255.0f);
               blueOut  = ((float)blue / 255.0f);
               alphaOut = ((float)alpha / 255.0f);
            }

            /// set the color components from floats ranging 0.0 to 1.0
            void setColorFloat(const float redIn,
                               const float greenIn,
                               const float blueIn,
                               const float alphaIn) {
               red   = (unsigned char)(redIn * 255.0f);
               green = (unsigned char)(greenIn * 255.0f);
               blue  = (unsigned char)(blueIn * 255.0f);
               alpha = (unsigned char)(alphaIn * 255.0f);
            }

         protected:
            /// name of label
            QString labelName;

            /// red component
            unsigned char red;

            /// green component
            unsigned char green;

            /// blue component
            unsigned char blue;

            /// alpha component
            unsigned char alpha;

            /// index in color file
            int colorFileIndex;
            
            /// label is enabled
            bool labelEnabled;
      };
      
      // copy helper used by copy constructor and assignement operators
      void copyHelperGiftiLabelTable(const GiftiLabelTable& nlt);
      
      /// container for labels
      typedef std::vector<LabelData> LabelContainer;
      
      /// iterator for label container
      typedef LabelContainer::iterator LabelContainerIterator;
      
      /// iterator for label container
      typedef LabelContainer::const_iterator ConstLabelContainerIterator;
      
      // the labels
      LabelContainer labels;
      
      // colors were present when label table was read
      bool hadColorsWhenRead;

      /*** IF ADDITIONAL MEMBERS ARE ADDED UPDATE copyHelperGiftiLabelTable() ***/
};

#endif // __GIFTI_LABEL_TABLE_H__
