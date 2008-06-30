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


#ifndef __VE_GUI_BORDER_ATTRIBUTES_DIALOG_H__
#define __VE_GUI_BORDER_ATTRIBUTES_DIALOG_H__

#include <QString>
#include <vector>

#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "WuQDialog.h"

class QCheckBox;
class QLineEdit;

/// This class creates the border attributes editor dialog
class GuiBorderAttributesDialog : public WuQDialog {
   Q_OBJECT
    
   public:
      /// border file type
      enum BORDER_FILE_TYPE {
         /// border file
         BORDER_FILE_TYPE_BORDER,
         /// border projection file
         BORDER_FILE_TYPE_BORDER_PROJECTION,
         /// border brain model border set
         BORDER_FILE_TYPE_BORDER_SET
      };
      
      /// storage for unique border attributes
      class BorderAttributes {
         public:
            /// name of border
            QString name;
            
            /// border sampling;
            float sampling;
            
            /// border variance
            float variance;
            
            /// border topography
            float topography;
            
            /// border uncertainty
            float uncertainty;
            
            /// index in file
            int fileIndex;
            
            /// number of links in the border
            int numberOfLinks;
            
            /// retain this border
            bool retain;
            
            /// constructor
            BorderAttributes(const QString& nameIn, 
                             const int fileIndexIn,
                             const int numberOfLinksIn,
                             const float samplingIn,
                             const float varianceIn, 
                             const float topographyIn,
                             const float uncertaintyIn) {
               name = nameIn;
               fileIndex = fileIndexIn,
               numberOfLinks = numberOfLinksIn;
               sampling = samplingIn;
               variance = varianceIn;
               topography = topographyIn;
               uncertainty = uncertaintyIn;
               retain = true;
            }

            bool operator<(const BorderAttributes& ba) const {
               if (name == ba.name) {
                  return (fileIndex < ba.fileIndex);
               }
               return (name < ba.name);
            }
      };
      
      /// Constructor for use with borders loaded in Caret
      GuiBorderAttributesDialog(QWidget* parent);
       
      /// Constructor for use with borders in a border or border projection file
      GuiBorderAttributesDialog(QWidget* parent,
                                const QString fileNameIn,
                                const BORDER_FILE_TYPE borderFileTypeIn,
                                const bool showVarianceTopographyUncertaintyFlagIn = true);

      /// Destructor
      ~GuiBorderAttributesDialog();
   
   protected slots:
      /// called when ok/canel buttons pressed
      void done(int r);
      
   protected:   
      /// process the brain model border set borders
      void processBrainModelBorderSetBorders();
         
      /// process the border file
      void processBorderFile();
      
      /// process the border projection file
      void processBorderProjectionFile();
      
      /// create the dialog
      void createDialog();
      
      /// storage for the unique borders
      std::vector<BorderAttributes> attributes;
      
      /// retain check boxes
      std::vector<QCheckBox*> retainCheckBoxes;
      
      /// name line edits
      std::vector<QLineEdit*> nameLineEdits;
      
      /// sampling line edits
      std::vector<QLineEdit*> samplingLineEdits;
      
      /// variance line edits
      std::vector<QLineEdit*> varianceLineEdits;
      
      /// topography line edits
      std::vector<QLineEdit*> topographyLineEdits;
      
      /// uncertainty line edits
      std::vector<QLineEdit*> uncertaintyLineEdits;
      
      /// border file type
      BORDER_FILE_TYPE borderFileType;
      
      /// name of border file
      QString fileName;
      
      /// border file
      BorderFile borderFile;
      
      /// border projection file
      BorderProjectionFile borderProjectionFile;
      
      /// show variance topography and undertainty
      bool showVarianceTopographyUncertaintyFlag;
      
};

#endif // __VE_GUI_BORDER_ATTRIBUTES_DIALOG_H__

