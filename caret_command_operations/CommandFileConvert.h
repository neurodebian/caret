
#ifndef __COMMAND_FILE_CONVERT_H__
#define __COMMAND_FILE_CONVERT_H__

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
#include "CommandBase.h"

class BrainSet;
class ProgramParameters;

/// class for data file conversion
class CommandFileConvert : public CommandBase {
   public:
      // constructor 
      CommandFileConvert();
      
      // destructor
      ~CommandFileConvert();
      
      // get full help information
      QString getHelpInformation() const;
      
      // get the script builder parameters
      virtual void getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const;
      
   protected:
      enum MODE { 
         MODE_NONE, 
         MODE_CONTOUR_CONVERT,
         MODE_FORMAT_CONVERT,
         MODE_FORMAT_INFO,
         MODE_SPEC_CONVERT,
         MODE_SURFACE_CONVERT,
         MODE_FREE_SURFER_CURVATURE_TO_CARET,
         MODE_FREE_SURFER_FUNCTIONAL_TO_CARET,
         MODE_FREE_SURFER_LABEL_TO_CARET,
         MODE_CARET_PAINT_TO_FREE_SURFER_LABEL,
         MODE_CARET_SHAPE_TO_FREE_SURFER_CURVATURE,
         MODE_VOLUME
      };

      enum SURFACE_FILE_TYPE {
         SURFACE_TYPE_UNKNOWN,
         SURFACE_TYPE_BYU,
         SURFACE_TYPE_CARET,
         SURFACE_TYPE_FREE_SURFER,
         SURFACE_TYPE_FREE_SURFER_PATCH,
         SURFACE_TYPE_GIFTI,
         SURFACE_TYPE_OPEN_INVENTOR,
         SURFACE_TYPE_STL,
         SURFACE_TYPE_VTK,
         SURFACE_TYPE_XML_VTK
      };

      // execute the command
      void executeCommand() throw (BrainModelAlgorithmException,
                                   CommandException,
                                   FileException,
                                   ProgramParametersException,
                                   StatisticException);

      // Convert caret files or display info 
      // info about the file.
      void fileFormatConvert(const std::vector<QString>& dataFileNames,
                             const QString& dataFileFormatList) throw (CommandException);
      
      // Write/Update a spec file.
      void updateSpecFile(const std::vector<QString>& tags, 
                          const std::vector<QString>& values) throw (CommandException);
      
      // Convert a free surfer curvature file to caret surface shape.
      void freeSurferCurvatureToCaretConvert() throw (CommandException);
      
      // Convert a caret paint file to free surfer label file
      void caretPaintToFreeSurferLabel() throw (CommandException);

      // Convert a caret shape column to free surfer curvature file
      void caretShapeToFreeSurferCurvature() throw (CommandException);
      
      // Convert a free surfer functional file to caret metric.
      void freeSurferLabelToCaretConvert() throw (CommandException);

      // Convert a free surfer functional file to caret metric.
      void freeSurferFunctionalToCaretConvert() throw (CommandException);

      // Convert a surface file to another type.
      void surfaceFileConversion() throw (CommandException);

      // Convert a surface type name to an enum.
      SURFACE_FILE_TYPE getSurfaceFileType(const QString& surfaceTypeName,
                                           const QString& inputOutputName)
                                             throw (CommandException);
      
      // convert a volulme
      void volumeConversion(const QString& inputVolumeName, 
                            const QString& outputVolumeName) throw (CommandException);
      
      // Convert a spec file's data files
      void specFileConvert(const QString& dataFileFormatList,
                                    const QString& specFileName) throw (CommandException);
      
                 
      // Convert a contour file
      void contourConversion(const QString& contourType,
                             const QString& contourFileName,
                             const QString& caretContourFileName,
                             const QString& caretContourCellFileName) throw (CommandException);                      
                                       
                                                  
                                                   

      SURFACE_FILE_TYPE inputSurfaceType;
      SURFACE_FILE_TYPE outputSurfaceType;

      MODE mode;
      AbstractFile::FILE_FORMAT fileWriteType;
      QString specFileName;
      QString structureName;
      QString inputSurfaceName;
      QString inputSurfaceName2;
      QString outputSurfaceName;
      QString outputSurfaceName2;
      QString outputCoordTypeName;
      QString outputTopoTypeName;
      QString inputShapeName;
      QString inputShapeColumn;
      QString outputCurvatureName;

      BrainSet* brainSet;
};

#endif // __COMMAND_FILE_CONVERT_H__

