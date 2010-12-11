
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
/*****===================================================================*****/
/*****     Sample functions to deal with NIFTI-1 and ANALYZE files       *****/
/*****...................................................................*****/
/*****            This code is released to the public domain.            *****/
/*****...................................................................*****/
/*****  Author: Robert W Cox, SSCC/DIRP/NIMH/NIH/DHHS/USA/EARTH          *****/
/*****  Date:   August 2003                                              *****/
/*****...................................................................*****/
/*****  Neither the National Institutes of Health (NIH), nor any of its  *****/
/*****  employees imply any warranty of usefulness of this software for  *****/
/*****  any purpose, and do not assume any liability for damages,        *****/
/*****  incidental or otherwise, caused by any use of this document.     *****/
/*****===================================================================*****/

#include <cmath>
#include <sstream>

#include "ByteSwapping.h"
#include "NiftiFileHeader.h"

/**
 * constructor.
 */
NiftiFileHeader::NiftiFileHeader()
{
   readExtensionFlag = false;
}

/**
 * constructor.
 */
NiftiFileHeader::NiftiFileHeader(const nifti_1_header niftiHeaderStructIn)
{
   readExtensionFlag = false;
   this->setNiftiHeaderStruct(niftiHeaderStructIn);
}

/**
 * destructor.
 */
NiftiFileHeader::~NiftiFileHeader()
{
}

/**
 * read the header.
 */
void
NiftiFileHeader::readHeader(const QString& filename) throw (FileException)
{
   gzFile dataFile = gzopen(filename.toAscii().constData(), "rb");
   if (dataFile == NULL) {
      throw FileException("Unable to open "
                          + filename
                          + "with ZLIB for reading.");
   }
   
   QString errorMessage;
   try {
      readHeader(dataFile, filename);
   }
   catch (FileException e) {
      errorMessage = e.whatQString();
   }
   gzclose(dataFile);

   if (errorMessage.isEmpty() == false) {
      throw FileException(errorMessage);
   }
}

/**
 * read the header from a compressed file.
 */
void
NiftiFileHeader::readHeader(gzFile dataFile,
                            const QString& filenameForErrorMessage) throw (FileException)
{
   if (dataFile == NULL) {
      throw FileException("Compressed file structure (gzFile) is invalid.");
   }

   //
   // Read the NIFTI header and close file after reading header
   //
   nifti_1_header hdr;
   const unsigned long headerSize = sizeof(hdr);
   const unsigned long numBytesRead = gzread(dataFile, (voidp)&hdr, headerSize);
   if (numBytesRead != headerSize) {
      std::ostringstream str;
      str << "Tried to read "
          << headerSize
          << " bytes from header.\n"
          << "Only read "
          << numBytesRead
          << " for file "
          << filenameForErrorMessage.toAscii().constData()
          << ".";
      throw FileException(str.str().c_str());
      return;
   }

   //
   // Make sure it is a NIFTI file
   //
   const int version = NIFTI_VERSION(hdr);
   switch (version) {
      case 0:
         throw FileException(filenameForErrorMessage + "Is not a NIFTI volume file.");
         return;
         break;
      case 1:
         break;
      default:
         {
            std::ostringstream str;
            str << filenameForErrorMessage.toAscii().constData()
                << " is an invalid NIFTI version: "
                << version
                << ".";
            throw FileException(str.str().c_str());
            return;
         }
         break;
   }

   //
   // Do bytes need to be swapped ?
   //
   dataNeedsByteSwapping = false;
   if (NIFTI_NEEDS_SWAP(hdr)) {
      dataNeedsByteSwapping = true;

      ByteSwapping::swapBytes(&hdr.sizeof_hdr, 1);
      ByteSwapping::swapBytes(&hdr.extents, 1);
      ByteSwapping::swapBytes(&hdr.session_error, 1);
      ByteSwapping::swapBytes(hdr.dim, 8);
      ByteSwapping::swapBytes(&hdr.intent_p1, 1);
      ByteSwapping::swapBytes(&hdr.intent_p2, 1);
      ByteSwapping::swapBytes(&hdr.intent_p3, 1);
      ByteSwapping::swapBytes(&hdr.intent_code, 1);
      ByteSwapping::swapBytes(&hdr.datatype, 1);
      ByteSwapping::swapBytes(&hdr.bitpix, 1);
      ByteSwapping::swapBytes(&hdr.slice_start, 1);
      ByteSwapping::swapBytes(hdr.pixdim, 8);
      ByteSwapping::swapBytes(&hdr.vox_offset, 1);
      ByteSwapping::swapBytes(&hdr.scl_slope, 1);
      ByteSwapping::swapBytes(&hdr.scl_inter, 1);
      ByteSwapping::swapBytes(&hdr.slice_end, 1);
      ByteSwapping::swapBytes(&hdr.cal_max, 1);
      ByteSwapping::swapBytes(&hdr.cal_min, 1);
      ByteSwapping::swapBytes(&hdr.slice_duration, 1);
      ByteSwapping::swapBytes(&hdr.toffset, 1);
      ByteSwapping::swapBytes(&hdr.glmax, 1);
      ByteSwapping::swapBytes(&hdr.glmin, 1);
      ByteSwapping::swapBytes(&hdr.qform_code, 1);
      ByteSwapping::swapBytes(&hdr.sform_code, 1);
      ByteSwapping::swapBytes(&hdr.quatern_b, 1);
      ByteSwapping::swapBytes(&hdr.quatern_c, 1);
      ByteSwapping::swapBytes(&hdr.quatern_d, 1);
      ByteSwapping::swapBytes(&hdr.qoffset_x, 1);
      ByteSwapping::swapBytes(&hdr.qoffset_y, 1);
      ByteSwapping::swapBytes(&hdr.qoffset_z, 1);
      ByteSwapping::swapBytes(hdr.srow_x, 4);
      ByteSwapping::swapBytes(hdr.srow_y, 4);
      ByteSwapping::swapBytes(hdr.srow_z, 4);
   }

   this->setNiftiHeaderStruct(hdr);

   if (readExtensionFlag) {
      readHeaderExtension(dataFile, hdr, dataNeedsByteSwapping);
   }
}

/**
 * read NIFTI extension from the header
 */
void
NiftiFileHeader::readHeaderExtension(gzFile dataFile,
                                     nifti_1_header& hdr,
                              const bool byteSwapFlag) throw (FileException)
{
   extensionInformation = "";
   //
   // Read the extender
   //
   if (hdr.vox_offset >= 352) {
      nifti1_extender extender;
      const unsigned long extLength = sizeof(extender);
      const unsigned long numBytesRead = gzread(dataFile, (voidp)&extender, extLength);
      //if (extLength == numBytesRead) {
      //   if (DebugControl::getDebugOn()) {
      //      std::cout << "NIFTI extension[0] " << static_cast<int>(extender.extension[0]) << std::endl;
      //   }
      //}

      int extCount = 1;
      z_off_t pos = gztell(dataFile);
      while (pos < hdr.vox_offset) {
         //
         // Read in the extension size and code
         //
         int extensionSize, extensionCode;
         if ((gzread(dataFile, (voidp)&extensionSize, sizeof(extensionSize)) != 4) ||
             (gzread(dataFile, (voidp)&extensionCode, sizeof(extensionCode)) != 4)) {
            extensionInformation += ("WARNING:  Problem reading extension"  + QString::number(extCount) + "\n"
                      + "This and all other extensions ignored.\n");
            break;
         }
         else {
            extensionInformation += (
                    "Extension " + QString::number(extCount) + " before byte swap\n"
                  + "   Size: " + QString::number(extensionSize) + "\n"
                  + "   Code: " + QString::number(extensionCode) + "\n");
            if (byteSwapFlag) {
               ByteSwapping::swapBytes(&extensionSize, 1);
               ByteSwapping::swapBytes(&extensionCode, 1);
            }

            extensionInformation += (
                    "Extension " + QString::number(extCount) + " after byte swap\n"
                  + "   Size: " + QString::number(extensionSize) + "\n"
                  + "   Code: " + QString::number(extensionCode) + "\n");

            //
            // Check the code
            //
            const int evenNum = (extensionCode / 2) * 2;
            if ((evenNum < 0) || (evenNum > 100)) {
               extensionInformation +=
                     ("WARNING:  Invalid extension code for extension " + QString::number(extCount) + "\n"
                      +  ".  This and all other extensions ignored.\n");
               break;
            }

            //
            // Check extension size
            //
            if ((extensionSize % 16) != 0) {
               extensionInformation += ("WARNING: NIFTI extension has size that is not a multiple of 16.\n");
            }

            //
            // The 8-byte extension size/code is included in extensionSize
            //
            const int dataSize = extensionSize - 8;

            if (dataSize > 0) {
               char* data = new char[dataSize + 1];
               if (gzread(dataFile, (voidp)data, dataSize) != dataSize) {
                  extensionInformation += ("WARNING:  Problem reading extension.\n"
                            "This and all other extensions ignored.\n");
                  break;
               }
               data[dataSize] = '\0';

               //
               // Is this the AFNI extension
               //
               if (extensionCode == 4) { // NIFTI_ECODE_AFNI) {
                  extensionInformation += "AFNI extension:\n";
               }

               delete[] data;
            }
         }

         extCount++;
         pos = gztell(dataFile);
      }
   }

}

/**
 * get the coordinate of a voxel.
 * Returns true if valid coordinate was created.
 */
bool
NiftiFileHeader::getVoxelCoordinate(const int ijk[3],
                       const STEREOTAXIC_TYPE stereotaxicType,
                       float xyzOut[3]) const
{
   bool validCoordFlag = false;

   switch (stereotaxicType) {
      case STEREOTAXIC_TYPE_NONE:
         xyzOut[0] = this->niftiHeaderStruct.pixdim[1] * ijk[0];
         xyzOut[1] = this->niftiHeaderStruct.pixdim[2] * ijk[1];
         xyzOut[2] = this->niftiHeaderStruct.pixdim[3] * ijk[2];
         validCoordFlag = true;
         break;
      case STEREOTAXIC_TYPE_QFORM:
         if (this->niftiHeaderStruct.qform_code > 0) {
             float p[4] = {
                this->niftiHeaderStruct.pixdim[1] * ijk[0],
                this->niftiHeaderStruct.pixdim[2] * ijk[1],
                (this->niftiHeaderStruct.pixdim[3] * ijk[2]
                        * this->qFormQFac),
                1.0
             };
             this->qFormTransformationMatrix.multiplyPoint(p);
             xyzOut[0] = p[0] + this->niftiHeaderStruct.qoffset_x;
             xyzOut[1] = p[1] + this->niftiHeaderStruct.qoffset_y;
             xyzOut[2] = p[2] + this->niftiHeaderStruct.qoffset_z;
             validCoordFlag = true;
         }
         break;
      case STEREOTAXIC_TYPE_SFORM:
         if (this->niftiHeaderStruct.sform_code > 0) {
             float p[4] = { ijk[0], ijk[1], ijk[2], 1.0 };
             this->sFormTransformationMatrix.multiplyPoint(p);
             xyzOut[0] = p[0];
             xyzOut[1] = p[1];
             xyzOut[2] = p[2];
             validCoordFlag = true;
         }
         break;
   }

   return validCoordFlag;
}

/**
 * get a copy of the NIFTI header struct.
 */
nifti_1_header
NiftiFileHeader::getNiftiHeaderStruct() const
{
   return this->niftiHeaderStruct;
}

/**
 * set the NIFTI header struct.
 */
void
NiftiFileHeader::setNiftiHeaderStruct(const nifti_1_header niftiHeaderStructIn)
{
   niftiHeaderStruct = niftiHeaderStructIn;


   VolumeFile::ORIENTATION qformOrientation[3] = {
      VolumeFile::ORIENTATION_UNKNOWN,
      VolumeFile::ORIENTATION_UNKNOWN,
      VolumeFile::ORIENTATION_UNKNOWN
   };
   VolumeFile::ORIENTATION sformOrientation[3] = {
      VolumeFile::ORIENTATION_UNKNOWN,
      VolumeFile::ORIENTATION_UNKNOWN,
      VolumeFile::ORIENTATION_UNKNOWN
   };

   if (niftiHeaderStruct.sform_code > 0) {
      //
      // NIFTI origin is in the center of the voxel
      // so move the origin to the corner of voxel
      //
      //volumeRead.origin[0] -= volumeRead.spacing[0] * 0.5;
      //volumeRead.origin[1] -= volumeRead.spacing[1] * 0.5;
      //volumeRead.origin[2] -= volumeRead.spacing[2] * 0.5;

      TransformationMatrix sm;
      sm.setMatrixElement(0, 0, niftiHeaderStruct.srow_x[0]);
      sm.setMatrixElement(0, 1, niftiHeaderStruct.srow_x[1]);
      sm.setMatrixElement(0, 2, niftiHeaderStruct.srow_x[2]);
      sm.setMatrixElement(0, 3, niftiHeaderStruct.srow_x[3]);
      sm.setMatrixElement(1, 0, niftiHeaderStruct.srow_y[0]);
      sm.setMatrixElement(1, 1, niftiHeaderStruct.srow_y[1]);
      sm.setMatrixElement(1, 2, niftiHeaderStruct.srow_y[2]);
      sm.setMatrixElement(1, 3, niftiHeaderStruct.srow_y[3]);
      sm.setMatrixElement(2, 0, niftiHeaderStruct.srow_z[0]);
      sm.setMatrixElement(2, 1, niftiHeaderStruct.srow_z[1]);
      sm.setMatrixElement(2, 2, niftiHeaderStruct.srow_z[2]);
      sm.setMatrixElement(2, 3, niftiHeaderStruct.srow_z[3]);
      sm.setMatrixElement(3, 0, 0.0);
      sm.setMatrixElement(3, 1, 0.0);
      sm.setMatrixElement(3, 2, 0.0);
      sm.setMatrixElement(3, 3, 1.0);
      NiftiFileHeader::mat44ToCaretOrientation(sm,
                                           sformOrientation[0],
                                           sformOrientation[1],
                                           sformOrientation[2]);

      this->sFormTransformationMatrix = sm;
   }
   else {
      this->sFormTransformationMatrix.identity();
   }

   if (niftiHeaderStruct.qform_code > 0) {
      this->qFormQFac = (niftiHeaderStruct.pixdim[0] < 0.0) ? -1.0 : 1.0 ;  /* left-handedness? */
      qFormTransformationMatrix =
                      nifti_quatern_to_mat44(niftiHeaderStruct.quatern_b, niftiHeaderStruct.quatern_c, niftiHeaderStruct.quatern_d,
                                             0.0, 0.0, 0.0,
                                             1.0, 1.0, 1.0,
                                             this->qFormQFac);

      NiftiFileHeader::mat44ToCaretOrientation(this->qFormTransformationMatrix,
                                           qformOrientation[0],
                                           qformOrientation[1],
                                           qformOrientation[2]);
   }
}

/**
 * get the data type as a string.
 */
QString
NiftiFileHeader::getDataTypeAsString() const
{
   QString dataCodeString = ("Unrecognized data type code = "
                             + QString::number(this->niftiHeaderStruct.datatype));
   switch (this->niftiHeaderStruct.datatype) {
      case DT_UNKNOWN:
         dataCodeString = "DT_UNKNOWN";
         break;
      case DT_BINARY:
         dataCodeString = "DT_BINARY";
         break;
      case NIFTI_TYPE_UINT8:
         dataCodeString = "NIFTI_TYPE_UINT8";
         break;
      case NIFTI_TYPE_INT16:
         dataCodeString = "NIFTI_TYPE_INT16";
         break;
      case NIFTI_TYPE_INT32:
         dataCodeString = "NIFTI_TYPE_INT32";
         break;
      case NIFTI_TYPE_FLOAT32:
         dataCodeString = "NIFTI_TYPE_FLOAT32";
         break;
      case NIFTI_TYPE_COMPLEX64:
         dataCodeString = "NIFTI_TYPE_COMPLEX64";
         break;
      case NIFTI_TYPE_FLOAT64:
         dataCodeString = "NIFTI_TYPE_FLOAT64";
         break;
      case NIFTI_TYPE_RGB24:
         dataCodeString = "NIFTI_TYPE_RGB24";
         break;
      case NIFTI_TYPE_INT8:
         dataCodeString = "NIFTI_TYPE_INT8";
         break;
      case NIFTI_TYPE_UINT16:
         dataCodeString = "NIFTI_TYPE_UINT16";
         break;
      case NIFTI_TYPE_UINT32:
         dataCodeString = "NIFTI_TYPE_UINT32";
         break;
      case NIFTI_TYPE_INT64:
         dataCodeString = "NIFTI_TYPE_INT64";
         break;
      case NIFTI_TYPE_UINT64:
         dataCodeString = "NIFTI_TYPE_UINT64";
         break;
      case NIFTI_TYPE_FLOAT128:
         dataCodeString = "NIFTI_TYPE_FLOAT128";
         break;
      case NIFTI_TYPE_COMPLEX128:
         dataCodeString = "NIFTI_TYPE_COMPLEX128";
         break;
      case NIFTI_TYPE_COMPLEX256:
         dataCodeString = "NIFTI_TYPE_COMPLEX256";
         break;
   }

   return dataCodeString;
}

/**
 * get the qform code as a string.
 */
QString
NiftiFileHeader::getQFormCodeAsString() const
{
   return this->getQOrSFormCodeAsString(this->niftiHeaderStruct.qform_code);
}

/**
 * get the sform code as a string.
 */
QString
NiftiFileHeader::getSFormCodeAsString() const
{
   return this->getQOrSFormCodeAsString(this->niftiHeaderStruct.sform_code);
}

/**
 * get the qform or sform code as a string.
 */
QString
NiftiFileHeader::getQOrSFormCodeAsString(const int qsForm) const
{
   QString s = "Invalid Code: " + QString::number(qsForm);

   switch (qsForm) {
      case NIFTI_XFORM_UNKNOWN:
         s = "NIFTI_XFORM_UNKNOWN";
         break;
      case NIFTI_XFORM_SCANNER_ANAT:
         s = "NIFTI_XFORM_SCANNER_ANAT";
         break;
      case NIFTI_XFORM_ALIGNED_ANAT:
         s = "NIFTI_XFORM_ALIGNED_ANAT";
         break;
      case NIFTI_XFORM_TALAIRACH:
         s = "NIFTI_XFORM_TALAIRACH";
         break;
      case NIFTI_XFORM_MNI_152:
         s = "NIFTI_XFORM_MNI_152";
         break;
   }

   return s;
}

/**
 * get a full description of the header's contents.
 */
QString
NiftiFileHeader::getDescriptionOfHeader() const
{
   int precision = 3;

   QStringList keys, values;

   keys << "sizeof_hdr";
   values << QString::number(this->niftiHeaderStruct.sizeof_hdr);

   keys << "data_type";
   QString dataTypeString;
   for (int i = 0; i < 10; i++) {
      if (this->niftiHeaderStruct.data_type[i] == '\0') {
         break;
      }
      dataTypeString += this->niftiHeaderStruct.data_type[i];
   }
   values << dataTypeString;

   keys << "db_name";
   QString dbNameString;
   for (int i = 0; i < 18; i++) {
      if (this->niftiHeaderStruct.db_name[i] == '\0') {
         break;
      }
      dbNameString += this->niftiHeaderStruct.db_name[i];
   }
   values << dbNameString;

   keys << "extents";
   values << QString::number(this->niftiHeaderStruct.extents);

   keys << "session_error";
   values << QString::number(this->niftiHeaderStruct.session_error);

   keys << "regular";
   values << QChar(this->niftiHeaderStruct.regular);

   keys << "dim_info";
   values << QString::number(this->niftiHeaderStruct.dim_info);

   keys << "dim";
   QString dimString;
   for (int i = 0; i < 8; i++) {
      dimString += QString::number(this->niftiHeaderStruct.dim[i]) + " ";
   }
   values << dimString;

   keys << "intent_p1";
   values << QString::number(this->niftiHeaderStruct.intent_p1, 'f', precision);

   keys << "intent_p2";
   values << QString::number(this->niftiHeaderStruct.intent_p2, 'f', precision);

   keys << "intent_p3";
   values << QString::number(this->niftiHeaderStruct.intent_p3, 'f', precision);

   keys << "intent_code";
   values << QString::number(this->niftiHeaderStruct.intent_code);

   keys << "datatype";
   values << QString::number(this->niftiHeaderStruct.datatype);

   keys << "bitpix";
   values << QString::number(this->niftiHeaderStruct.bitpix);

   keys << "slice_start";
   values << QString::number(this->niftiHeaderStruct.slice_start);

   keys << "pixdim";
   QString pixDimString;
   for (int i = 0; i < 8; i++) {
      pixDimString += QString::number(this->niftiHeaderStruct.pixdim[i], 'f', precision) + " ";
   }
   values << pixDimString;

   keys << "vox_offset";
   values << QString::number(this->niftiHeaderStruct.vox_offset, 'f', precision);

   keys << "scl_slope";
   values << QString::number(this->niftiHeaderStruct.scl_slope, 'f', precision);

   keys << "scl_inter";
   values << QString::number(this->niftiHeaderStruct.scl_inter, 'f', precision);

   keys << "slice_end";
   values << QString::number(this->niftiHeaderStruct.slice_end);

   keys << "slice_code";
   values << QString::number(this->niftiHeaderStruct.slice_code);

   keys << "xyzt_units";
   values << QString::number(this->niftiHeaderStruct.xyzt_units);

   keys << "cal_max";
   values << QString::number(this->niftiHeaderStruct.cal_max, 'f', precision);

   keys << "cal_min";
   values << QString::number(this->niftiHeaderStruct.cal_min, 'f', precision);

   keys << "slice_duration";
   values << QString::number(this->niftiHeaderStruct.slice_duration, 'f', precision);

   keys << "toffset";
   values << QString::number(this->niftiHeaderStruct.toffset, 'f', precision);

   keys << "glmax";
   values << QString::number(this->niftiHeaderStruct.glmax);

   keys << "glmin";
   values << QString::number(this->niftiHeaderStruct.glmin);

   QString description;
   for (int i = 0; i < 80; i++) {
      if (this->niftiHeaderStruct.descrip[i] == '\0') {
         break;
      }
      description += this->niftiHeaderStruct.descrip[i];
   }
   keys << "description";
   values << description;

   QString auxFile;
   for (int i = 0; i < 24; i++) {
      if (this->niftiHeaderStruct.aux_file[i] == '\0') {
         break;
      }
      auxFile += this->niftiHeaderStruct.aux_file[i];
   }
   keys << "aux_file";
   values << auxFile;

   keys << "qform_code";
   values << QString::number(this->niftiHeaderStruct.qform_code);

   keys << "sform_code";
   values << QString::number(this->niftiHeaderStruct.sform_code);

   keys << "quatern_b";
   values << QString::number(this->niftiHeaderStruct.quatern_b, 'f', precision);

   keys << "quatern_c";
   values << QString::number(this->niftiHeaderStruct.quatern_c, 'f', precision);

   keys << "quatern_d";
   values << QString::number(this->niftiHeaderStruct.quatern_d, 'f', precision);

   keys << "qoffset_x";
   values << QString::number(this->niftiHeaderStruct.qoffset_x, 'f', precision);

   keys << "qoffset_y";
   values << QString::number(this->niftiHeaderStruct.qoffset_y, 'f', precision);

   keys << "qoffset_z";
   values << QString::number(this->niftiHeaderStruct.qoffset_z, 'f', precision);

   QString sRowX;
   for (int i = 0; i < 4; i++) {
      sRowX += QString::number(this->niftiHeaderStruct.srow_x[i], 'f', precision) + " ";
   }
   keys << "srow_x";
   values << sRowX;

   QString sRowY;
   for (int i = 0; i < 4; i++) {
      sRowY += QString::number(this->niftiHeaderStruct.srow_y[i], 'f', precision) + " ";
   }
   keys << "srow_y";
   values << sRowY;

   QString sRowZ;
   for (int i = 0; i < 4; i++) {
      sRowZ += QString::number(this->niftiHeaderStruct.srow_z[i], 'f', precision) + " ";
   }
   keys << "srow_z";
   values << sRowZ;

   QString intentName;
   for (int i = 0; i < 16; i++) {
      if (this->niftiHeaderStruct.intent_name[i] == '\0') {
         break;
      }
      intentName += this->niftiHeaderStruct.intent_name[i];
   }
   keys << "intent_name";
   values << intentName;

   QString magic;
   for (int i = 0; i < 3; i++) {
      if (this->niftiHeaderStruct.magic[i] == '\0') {
         break;
      }
      magic += this->niftiHeaderStruct.magic[i];
   }
   keys << "magic";
   values << magic;

   //
   // Get key maximum length
   //
   int maxLength = 0;
   for (int i = 0; i < keys.count(); i++) {
      maxLength = std::max(maxLength, keys.at(i).length());
   }

   QString headerDescriptionOut;

   //
   // Place data into header description
   //
   for (int i = 0; i < keys.count(); i++) {
      headerDescriptionOut +=
              QString("%1: %2\n").arg(keys.at(i), maxLength).arg(values.at(i));
   }
   headerDescriptionOut += "\n";

   //
   // Intent Information
   //
   QString niftiIntentCodeAndParam;
   QString niftiIntentName;
   NiftiFileHeader::getNiftiIntentionInformation(niftiIntentCodeAndParam,
                                                 niftiIntentName);
   headerDescriptionOut += ("Intent Name:      " + niftiIntentName + "\n");
   headerDescriptionOut += ("Intent Parameters:" + niftiIntentCodeAndParam + "\n");
   headerDescriptionOut += "\n";

   int firstVoxelIJK[3] = { 0, 0, 0 };
   int secondVoxelIJK[3] = { 1, 1, 1 };
   float voxelXYZ[3];
   if (this->getVoxelCoordinate(firstVoxelIJK,
                                NiftiFileHeader::STEREOTAXIC_TYPE_NONE,
                                voxelXYZ)) {
      headerDescriptionOut +=
              ("   First Voxel XYZ (method 1): "
               + QString::number(voxelXYZ[0], 'f', precision) + ", "
               + QString::number(voxelXYZ[1], 'f', precision) + ", "
               + QString::number(voxelXYZ[2], 'f', precision)
               + "\n");
       float voxelTwoXYZ[3];
       if (this->getVoxelCoordinate(secondVoxelIJK,
                                    NiftiFileHeader::STEREOTAXIC_TYPE_NONE,
                                    voxelTwoXYZ)) {
          const float spacing[3] = {
             voxelTwoXYZ[0] - voxelXYZ[0],
             voxelTwoXYZ[1] - voxelXYZ[1],
             voxelTwoXYZ[2] - voxelXYZ[2]
          };
          headerDescriptionOut += (
                     "   Spacing: "
                     + QString::number(spacing[0], 'f', precision) + ", "
                     + QString::number(spacing[1], 'f', precision) + ", "
                     + QString::number(spacing[2], 'f', precision)
                     + "\n");


       }
   }
   headerDescriptionOut += "\n";
   //
   // qform matrix and orientation
   //
   headerDescriptionOut += ("QFORM: "
                            + this->getQFormCodeAsString()
                            + "\n");
   for (int i = 0; i < 4; i++) {
      headerDescriptionOut += "   ";
      for (int j = 0; j < 4; j++) {
         headerDescriptionOut +=
            QString::number(this->qFormTransformationMatrix.getMatrixElement(i, j),
                            'f', precision).rightJustified(12, ' ');
      }
      headerDescriptionOut += "\n";
   }
   headerDescriptionOut += 
           ("   Orientation: "
            + this->getQFormOrientationAsString()
            + "\n");

   float qformXYZ[3];
   if (this->getVoxelCoordinate(firstVoxelIJK,
                                NiftiFileHeader::STEREOTAXIC_TYPE_QFORM,
                                qformXYZ)) {
      headerDescriptionOut +=
              ("   First Voxel XYZ (Method 2): "
               + QString::number(qformXYZ[0], 'f', precision) + ", "
               + QString::number(qformXYZ[1], 'f', precision) + ", "
               + QString::number(qformXYZ[2], 'f', precision)
               + "\n");
       float voxelTwoXYZ[3];
       if (this->getVoxelCoordinate(secondVoxelIJK,
                                    NiftiFileHeader::STEREOTAXIC_TYPE_QFORM,
                                    voxelTwoXYZ)) {
          const float spacing[3] = {
             voxelTwoXYZ[0] - qformXYZ[0],
             voxelTwoXYZ[1] - qformXYZ[1],
             voxelTwoXYZ[2] - qformXYZ[2]
          };
          headerDescriptionOut += (
                     "   Spacing: "
                     + QString::number(spacing[0], 'f', precision) + ", "
                     + QString::number(spacing[1], 'f', precision) + ", "
                     + QString::number(spacing[2], 'f', precision)
                     + "\n");


       }
   }
   headerDescriptionOut += "\n";

   //
   // sform matrix and orientation
   //
   headerDescriptionOut += ("SFORM: "
                            + this->getSFormCodeAsString()
                            + "\n");
   for (int i = 0; i < 4; i++) {
      headerDescriptionOut += "   ";
      for (int j = 0; j < 4; j++) {
         headerDescriptionOut +=
            QString::number(this->sFormTransformationMatrix.getMatrixElement(i, j),
                            'f', precision).rightJustified(12, ' ');
      }
      headerDescriptionOut += "\n";
   }
   headerDescriptionOut += 
           ("   Orientation: "
            + this->getSFormOrientationAsString()
            + "\n");
   float sformXYZ[3];
   if (this->getVoxelCoordinate(firstVoxelIJK,
                                NiftiFileHeader::STEREOTAXIC_TYPE_SFORM,
                                sformXYZ)) {
      headerDescriptionOut +=
              ("   First Voxel XYZ (Method 3): "
               + QString::number(sformXYZ[0], 'f', precision) + ", "
               + QString::number(sformXYZ[1], 'f', precision) + ", "
               + QString::number(sformXYZ[2], 'f', precision)
               + "\n");
       float voxelTwoXYZ[3];
       if (this->getVoxelCoordinate(secondVoxelIJK,
                                    NiftiFileHeader::STEREOTAXIC_TYPE_SFORM,
                                    voxelTwoXYZ)) {
          const float spacing[3] = {
             voxelTwoXYZ[0] - sformXYZ[0],
             voxelTwoXYZ[1] - sformXYZ[1],
             voxelTwoXYZ[2] - sformXYZ[2]
          };
          headerDescriptionOut += (
                     "   Spacing: "
                     + QString::number(spacing[0], 'f', precision) + ", "
                     + QString::number(spacing[1], 'f', precision) + ", "
                     + QString::number(spacing[2], 'f', precision)
                     + "\n");


       }
   }   
   headerDescriptionOut += "\n";

   headerDescriptionOut += ("Data Type: " + this->getDataTypeAsString() + "\n");
   headerDescriptionOut += "\n";

   headerDescriptionOut +=("Space Units: " + this->getSpacingUnitsAsString() + "\n");

   headerDescriptionOut +=("Time Units: " + this->getTimeUnitsAsString() + "\n");
   headerDescriptionOut += "\n";

   if (extensionInformation.isEmpty() == false) {
      headerDescriptionOut += extensionInformation;
      headerDescriptionOut += "\n";
   }
   return headerDescriptionOut;
}

/**
 * get the sform transformation matrix.
 */
TransformationMatrix
NiftiFileHeader::getSFormTransformationMatrix() const
{
   return this->sFormTransformationMatrix;
}

/**
 * get the qform transformation matrix.
 */
TransformationMatrix
NiftiFileHeader::getQFormTransformationMatrix() const
{
   return this->qFormTransformationMatrix;
}

/**
 * get the sform orientation.
 */
void
NiftiFileHeader::getSFormOrientation(VolumeFile::ORIENTATION orientationOut[3]) const
{
   orientationOut[0] = VolumeFile::ORIENTATION_UNKNOWN;
   orientationOut[1] = VolumeFile::ORIENTATION_UNKNOWN;
   orientationOut[2] = VolumeFile::ORIENTATION_UNKNOWN;
   if (this->niftiHeaderStruct.sform_code > 0) {
      NiftiFileHeader::mat44ToCaretOrientation(this->sFormTransformationMatrix,
                                          orientationOut[0],
                                          orientationOut[1],
                                          orientationOut[2]);
   }
}

void
NiftiFileHeader::nifti_mat44_to_orientation( mat44 R , int *icod, int *jcod, int *kcod )
{
   float xi,xj,xk , yi,yj,yk , zi,zj,zk , val,detQ,detP ;
   mat33 P , Q , M ;
   int i,j,k=0,p,q,r , ibest,jbest,kbest,pbest,qbest,rbest ;
   float vbest ;

   if( icod == NULL || jcod == NULL || kcod == NULL ) return ; /* bad */

   *icod = *jcod = *kcod = 0 ; /* error returns, if sh*t happens */

   /* load column vectors for each (i,j,k) direction from matrix */

   /*-- i axis --*/ /*-- j axis --*/ /*-- k axis --*/

   xi = R.m[0][0] ; xj = R.m[0][1] ; xk = R.m[0][2] ;
   yi = R.m[1][0] ; yj = R.m[1][1] ; yk = R.m[1][2] ;
   zi = R.m[2][0] ; zj = R.m[2][1] ; zk = R.m[2][2] ;

   /* normalize column vectors to get unit vectors along each ijk-axis */

   /* normalize i axis */

   val = std::sqrt( xi*xi + yi*yi + zi*zi ) ;
   if( val == 0.0 ) return ;                 /* stupid input */
   xi /= val ; yi /= val ; zi /= val ;

   /* normalize j axis */

   val = std::sqrt( xj*xj + yj*yj + zj*zj ) ;
   if( val == 0.0 ) return ;                 /* stupid input */
   xj /= val ; yj /= val ; zj /= val ;

   /* orthogonalize j axis to i axis, if needed */

   val = xi*xj + yi*yj + zi*zj ;    /* dot product between i and j */
   if( std::fabs(val) > 1.e-4 ){
     xj -= val*xi ; yj -= val*yi ; zj -= val*zi ;
     val = std::sqrt( xj*xj + yj*yj + zj*zj ) ;  /* must renormalize */
     if( val == 0.0 ) return ;              /* j was parallel to i? */
     xj /= val ; yj /= val ; zj /= val ;
   }

   /* normalize k axis; if it is zero, make it the cross product i x j */

   val = std::sqrt( xk*xk + yk*yk + zk*zk ) ;
   if( val == 0.0 ){ xk = yi*zj-zi*yj; yk = zi*xj-zj*xi ; zk=xi*yj-yi*xj ; }
   else            { xk /= val ; yk /= val ; zk /= val ; }

   /* orthogonalize k to i */

   val = xi*xk + yi*yk + zi*zk ;    /* dot product between i and k */
   if( std::fabs(val) > 1.e-4 ){
     xk -= val*xi ; yk -= val*yi ; zk -= val*zi ;
     val = std::sqrt( xk*xk + yk*yk + zk*zk ) ;
     if( val == 0.0 ) return ;      /* bad */
     xk /= val ; yk /= val ; zk /= val ;
   }

   /* orthogonalize k to j */

   val = xj*xk + yj*yk + zj*zk ;    /* dot product between j and k */
   if( std::fabs(val) > 1.e-4 ){
     xk -= val*xj ; yk -= val*yj ; zk -= val*zj ;
     val = std::sqrt( xk*xk + yk*yk + zk*zk ) ;
     if( val == 0.0 ) return ;      /* bad */
     xk /= val ; yk /= val ; zk /= val ;
   }

   Q.m[0][0] = xi ; Q.m[0][1] = xj ; Q.m[0][2] = xk ;
   Q.m[1][0] = yi ; Q.m[1][1] = yj ; Q.m[1][2] = yk ;
   Q.m[2][0] = zi ; Q.m[2][1] = zj ; Q.m[2][2] = zk ;

   /* at this point, Q is the rotation matrix from the (i,j,k) to (x,y,z) axes */

   detQ = nifti_mat33_determ( Q ) ;
   if( detQ == 0.0 ) return ; /* shouldn't happen unless user is a DUFIS */

   /* Build and test all possible +1/-1 coordinate permutation matrices P;
      then find the P such that the rotation matrix M=PQ is closest to the
      identity, in the sense of M having the smallest total rotation angle. */

   /* Despite the formidable looking 6 nested loops, there are
      only 3*3*3*2*2*2 = 216 passes, which will run very quickly. */

   vbest = -666.0 ; ibest=pbest=qbest=rbest=1 ; jbest=2 ; kbest=3 ;
   for( i=1 ; i <= 3 ; i++ ){     /* i = column number to use for row #1 */
    for( j=1 ; j <= 3 ; j++ ){    /* j = column number to use for row #2 */
     if( i == j ) continue ;
      for( k=1 ; k <= 3 ; k++ ){  /* k = column number to use for row #3 */
       if( i == k || j == k ) continue ;
       P.m[0][0] = P.m[0][1] = P.m[0][2] =
        P.m[1][0] = P.m[1][1] = P.m[1][2] =
         P.m[2][0] = P.m[2][1] = P.m[2][2] = 0.0 ;
       for( p=-1 ; p <= 1 ; p+=2 ){    /* p,q,r are -1 or +1      */
        for( q=-1 ; q <= 1 ; q+=2 ){   /* and go into rows #1,2,3 */
         for( r=-1 ; r <= 1 ; r+=2 ){
           P.m[0][i-1] = p ; P.m[1][j-1] = q ; P.m[2][k-1] = r ;
           detP = nifti_mat33_determ(P) ;           /* sign of permutation */
           if( detP * detQ <= 0.0 ) continue ;  /* doesn't match sign of Q */
           M = nifti_mat33_mul(P,Q) ;

           /* angle of M rotation = 2.0*acos(0.5*sqrt(1.0+trace(M)))       */
           /* we want largest trace(M) == smallest angle == M nearest to I */

           val = M.m[0][0] + M.m[1][1] + M.m[2][2] ; /* trace */
           if( val > vbest ){
             vbest = val ;
             ibest = i ; jbest = j ; kbest = k ;
             pbest = p ; qbest = q ; rbest = r ;
           }
   }}}}}}

   /* At this point ibest is 1 or 2 or 3; pbest is -1 or +1; etc.

      The matrix P that corresponds is the best permutation approximation
      to Q-inverse; that is, P (approximately) takes (x,y,z) coordinates
      to the (i,j,k) axes.

      For example, the first row of P (which contains pbest in column ibest)
      determines the way the i axis points relative to the anatomical
      (x,y,z) axes.  If ibest is 2, then the i axis is along the y axis,
      which is direction P2A (if pbest > 0) or A2P (if pbest < 0).

      So, using ibest and pbest, we can assign the output code for
      the i axis.  Mutatis mutandis for the j and k axes, of course. */

   switch( ibest*pbest ){
     case  1: i = NIFTI_L2R ; break ;
     case -1: i = NIFTI_R2L ; break ;
     case  2: i = NIFTI_P2A ; break ;
     case -2: i = NIFTI_A2P ; break ;
     case  3: i = NIFTI_I2S ; break ;
     case -3: i = NIFTI_S2I ; break ;
   }

   switch( jbest*qbest ){
     case  1: j = NIFTI_L2R ; break ;
     case -1: j = NIFTI_R2L ; break ;
     case  2: j = NIFTI_P2A ; break ;
     case -2: j = NIFTI_A2P ; break ;
     case  3: j = NIFTI_I2S ; break ;
     case -3: j = NIFTI_S2I ; break ;
   }

   switch( kbest*rbest ){
     case  1: k = NIFTI_L2R ; break ;
     case -1: k = NIFTI_R2L ; break ;
     case  2: k = NIFTI_P2A ; break ;
     case -2: k = NIFTI_A2P ; break ;
     case  3: k = NIFTI_I2S ; break ;
     case -3: k = NIFTI_S2I ; break ;
   }

   *icod = i ; *jcod = j ; *kcod = k ; return ;
}

/**
 * Methods below are caret unique.
 */
void
NiftiFileHeader::mat44ToCaretOrientation(const TransformationMatrix& tm,
                                     VolumeFile::ORIENTATION& xOrient,
                                     VolumeFile::ORIENTATION& yOrient,
                                     VolumeFile::ORIENTATION& zOrient)
{
   mat44 m;
   tm.getMatrix(m.m);
   int x, y, z;
   NiftiFileHeader::nifti_mat44_to_orientation(m, &x, &y, &z);

   xOrient = niftiOrientationToCaretOrientation(x);
   yOrient = niftiOrientationToCaretOrientation(y);
   zOrient = niftiOrientationToCaretOrientation(z);
}

/**
 * convert NIFTI orientation to Caret orientation.
 */
VolumeFile::ORIENTATION
NiftiFileHeader::niftiOrientationToCaretOrientation(const int niftiOrientation)
{
  VolumeFile::ORIENTATION orient = VolumeFile::ORIENTATION_UNKNOWN;

  switch(niftiOrientation) {
     case NiftiFileHeader::NIFTI_L2R:
        orient = VolumeFile::ORIENTATION_LEFT_TO_RIGHT;
        break;
     case NiftiFileHeader::NIFTI_R2L:
        orient = VolumeFile::ORIENTATION_RIGHT_TO_LEFT;
        break;
     case NiftiFileHeader::NIFTI_P2A:
        orient = VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR;
        break;
     case NiftiFileHeader::NIFTI_A2P:
        orient = VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR;
        break;
     case NiftiFileHeader::NIFTI_I2S:
        orient = VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR;
        break;
     case NiftiFileHeader::NIFTI_S2I:
        orient = VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR;
        break;
  }

  return orient;
}

/**
 * get the qform orientation.
 */
void
NiftiFileHeader::getQFormOrientation(VolumeFile::ORIENTATION orientationOut[3]) const
{
   orientationOut[0] = VolumeFile::ORIENTATION_UNKNOWN;
   orientationOut[1] = VolumeFile::ORIENTATION_UNKNOWN;
   orientationOut[2] = VolumeFile::ORIENTATION_UNKNOWN;
   if (this->niftiHeaderStruct.qform_code > 0) {
      NiftiFileHeader::mat44ToCaretOrientation(this->qFormTransformationMatrix,
                                          orientationOut[0],
                                          orientationOut[1],
                                          orientationOut[2]);
   }
}

/**
 * get the sform orientation as a string.
 */
QString
NiftiFileHeader::getSFormOrientationAsString() const
{
   VolumeFile::ORIENTATION orientation[3];
   this->getSFormOrientation(orientation);

   const QString s = (VolumeFile::getOrientationLabel(orientation[0]) + ", "
                      + VolumeFile::getOrientationLabel(orientation[1]) + ", "
                      + VolumeFile::getOrientationLabel(orientation[2]));

   return s;
}

/**
 * get the qform orientation as a string.
 */
QString
NiftiFileHeader::getQFormOrientationAsString() const
{
   VolumeFile::ORIENTATION orientation[3];
   this->getQFormOrientation(orientation);

   const QString s = (VolumeFile::getOrientationLabel(orientation[0]) + ", "
                      + VolumeFile::getOrientationLabel(orientation[1]) + ", "
                      + VolumeFile::getOrientationLabel(orientation[2]));

   return s;
}

/**
 * get the spacing units as a string.
 */
QString
NiftiFileHeader::getSpacingUnitsAsString() const
{
   const int spaceUnits = XYZT_TO_SPACE(this->niftiHeaderStruct.xyzt_units);
   QString spaceUnitString("Unrecognized space code: "
                           + QString::number(spaceUnits));
   switch (spaceUnits) {
      case NIFTI_UNITS_UNKNOWN:
         spaceUnitString = "NIFTI_UNITS_UNKNOWN";
         break;
      case NIFTI_UNITS_METER:
         spaceUnitString = "NIFTI_UNITS_METER";
         break;
      case NIFTI_UNITS_MM:
         spaceUnitString = "NIFTI_UNITS_MM";
         break;
      case NIFTI_UNITS_MICRON:
         spaceUnitString = "NIFTI_UNITS_MICRON";
         break;
   }

   return spaceUnitString;
}

/**
 * get the time units as a string.
 */
QString
NiftiFileHeader::getTimeUnitsAsString() const
{
   const int timeUnits  = XYZT_TO_TIME(this->niftiHeaderStruct.xyzt_units);
   QString timeUnitString("Unrecognized time code: "
                          + QString::number(timeUnits));
   switch (timeUnits) {
      case NIFTI_UNITS_UNKNOWN:
         timeUnitString = "NIFTI_UNITS_UNKNOWN";
         break;
      case NIFTI_UNITS_SEC:
         timeUnitString = "NIFTI_UNITS_SEC";
         break;
      case NIFTI_UNITS_MSEC:
         timeUnitString = "NIFTI_UNITS_MSEC";
         break;
      case NIFTI_UNITS_USEC:
         timeUnitString = "NIFTI_UNITS_USEC";
         break;
      case NIFTI_UNITS_HZ:
         timeUnitString = "NIFTI_UNITS_HZ";
         break;
      case NIFTI_UNITS_PPM:
         timeUnitString = "NIFTI_UNITS_PPM";
         break;
   }

   return timeUnitString;
}

/**
 * get the NIFTI intention (such as statistical parameters) information.
 */
void
NiftiFileHeader::getNiftiIntentionInformation(QString& niftiIntentCodeAndParam,
                                          QString& niftiIntentName) const
{
   niftiIntentName = ("NIFTI_INTENT_NOT_RECOGNIZED_CODE_"
                      + QString::number(this->niftiHeaderStruct.intent_code));

   QString intentDescription;
   QString p1;
   QString p2;
   QString p3;

   switch (this->niftiHeaderStruct.intent_code) {
     case NIFTI_INTENT_NONE:
        niftiIntentName = "NIFTI_INTENT_NONE";
        intentDescription = "None" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_CORREL:
        niftiIntentName = "NIFTI_INTENT_CORREL";
        intentDescription = "Correlation statistic" ;
        p1 = "DOF";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_TTEST:
        niftiIntentName = "NIFTI_INTENT_TTEST";
        intentDescription = "T-statistic" ;
        p1 = "DOF";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_FTEST:
        niftiIntentName = "NIFTI_INTENT_FTEST";
        intentDescription = "F-statistic" ;
        p1 = "Numerator DOF";
        p2 = "Denorminator DOF";
        p3 = "";
        break;
     case NIFTI_INTENT_ZSCORE:
        niftiIntentName = "NIFTI_INTENT_ZSCORE";
        intentDescription = "Z-score"     ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_CHISQ:
        niftiIntentName = "NIFTI_INTENT_CHISQ";
        intentDescription = "Chi-squared distribution" ;
        p1 = "DOF";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_BETA:
        niftiIntentName = "NIFTI_INTENT_BETA";
        intentDescription = "Beta distribution" ;
        p1 = "a";
        p2 = "b";
        p3 = "";
        break;
     case NIFTI_INTENT_BINOM:
        niftiIntentName = "NIFTI_INTENT_BINOM";
        intentDescription = "Binomial distribution" ;
        p1 = "Number of Trials";
        p2 = "Probability per Trial";
        p3 = "";
        break;
     case NIFTI_INTENT_GAMMA:
        niftiIntentName = "NIFTI_INTENT_GAMMA";
        intentDescription = "Gamma distribution" ;
        p1 = "Shape";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_POISSON:
        niftiIntentName = "NIFTI_INTENT_POISSON";
        intentDescription = "Poisson distribution" ;
        p1 = "Mean";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_NORMAL:
        niftiIntentName = "NIFTI_INTENT_NORMAL";
        intentDescription = "Normal distribution" ;
        p1 = "Mean";
        p2 = "Standard Deviation";
        p3 = "";
        break;
     case NIFTI_INTENT_FTEST_NONC:
        niftiIntentName = "NIFTI_INTENT_FTEST_NONC";
        intentDescription = "F-statistic noncentral" ;
        p1 = "Numerator DOF";
        p2 = "Denominator DOF";
        p3 = "Numerator Noncentrality Parameter";
        break;
     case NIFTI_INTENT_CHISQ_NONC:
        niftiIntentName = "NIFTI_INTENT_CHISQ_NONC";
        intentDescription = "Chi-squared noncentral" ;
        p1 = "DOF";
        p2 = "Noncentrality Parameter";
        p3 = "";
        break;
     case NIFTI_INTENT_LOGISTIC:
        niftiIntentName = "NIFTI_INTENT_LOGISTIC";
        intentDescription = "Logistic distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_LAPLACE:
        niftiIntentName = "NIFTI_INTENT_LAPLACE";
        intentDescription = "Laplace distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_UNIFORM:
        niftiIntentName = "NIFTI_INTENT_UNIFORM";
        intentDescription = "Uniform distribition" ;
        p1 = "Lower End";
        p2 = "Upper End";
        p3 = "";
        break;
     case NIFTI_INTENT_TTEST_NONC:
        niftiIntentName = "NIFTI_INTENT_TTEST_NONC";
        intentDescription = "T-statistic noncentral" ;
        p1 = "DOF";
        p2 = "Noncentrality Parameter";
        p3 = "";
        break;
     case NIFTI_INTENT_WEIBULL:
        niftiIntentName = "NIFTI_INTENT_WEIBULL";
        intentDescription = "Weibull distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "Power";
        break;
     case NIFTI_INTENT_CHI:
        niftiIntentName = "NIFTI_INTENT_CHI";
        intentDescription = "Chi distribution" ;
        p1 = "Half Normal Distribution";
        p2 = "Rayleigh Distritibution";
        p3 = "Maxwell-Boltzmann Distribution";
        break;
     case NIFTI_INTENT_INVGAUSS:
        niftiIntentName = "NIFTI_INTENT_INVGAUSS";
        intentDescription = "Inverse Gaussian distribution" ;
        p1 = "MU";
        p2 = "Lambda";
        p3 = "";
        break;
     case NIFTI_INTENT_EXTVAL:
        niftiIntentName = "NIFTI_INTENT_EXTVAL";
        intentDescription = "Extreme Value distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_PVAL:
        niftiIntentName = "NIFTI_INTENT_PVAL";
        intentDescription = "P-value" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_LOGPVAL:
        niftiIntentName = "NIFTI_INTENT_LOGPVAL";
        intentDescription = "Log P-value" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_LOG10PVAL:
        niftiIntentName = "NIFTI_INTENT_LOG10PVAL";
        intentDescription = "Log10 P-value" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_ESTIMATE:
        niftiIntentName = "NIFTI_INTENT_ESTIMATE";
        intentDescription = "Estimate" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_LABEL:
        niftiIntentName = "NIFTI_INTENT_LABEL";
        intentDescription = "Label index" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_NEURONAME:
        niftiIntentName = "NIFTI_INTENT_NEURONAME";
        intentDescription = "NeuroNames index" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_GENMATRIX:
        niftiIntentName = "NIFTI_INTENT_GENMATRIX";
        intentDescription = "General matrix" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_SYMMATRIX:
        niftiIntentName = "NIFTI_INTENT_SYMMATRIX";
        intentDescription = "Symmetric matrix" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_DISPVECT:
        niftiIntentName = "NIFTI_INTENT_DISPVECT";
        intentDescription = "Displacement vector" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_VECTOR:
        niftiIntentName = "NIFTI_INTENT_VECTOR";
        intentDescription = "Vector" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_POINTSET:
        niftiIntentName = "NIFTI_INTENT_POINTSET";
        intentDescription = "Pointset" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_TRIANGLE:
        niftiIntentName = "NIFTI_INTENT_TRIANGLE";
        intentDescription = "Triangle" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_QUATERNION:
        niftiIntentName = "NIFTI_INTENT_QUATERNION";
        intentDescription = "Quaternion" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_DIMLESS:
        niftiIntentName = "NIFTI_INTENT_DIMLESS";
        intentDescription = "Dimensionless number" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
   }

   QString s;

   if (intentDescription.isEmpty() == false) {
      s += intentDescription;

      if (p1.isEmpty() == false) {
         s += ("  "
               + p1
               + "="
               + QString::number(this->niftiHeaderStruct.intent_p1, 'f', 2));
      }

      if (p2.isEmpty() == false) {
         s += ("  "
               + p2
               + "="
               + QString::number(this->niftiHeaderStruct.intent_p2, 'f', 2));
      }

      if (p3.isEmpty() == false) {
         s += ("  "
               + p3
               + "="
               + QString::number(this->niftiHeaderStruct.intent_p3, 'f', 2));
      }
   }

   niftiIntentCodeAndParam = s;
}

/**
 * Convert quaternions to a transformation matrix.
 */
TransformationMatrix
NiftiFileHeader::nifti_quatern_to_mat44( float qb, float qc, float qd,
                                     float qx, float qy, float qz,
                                     float dx, float dy, float dz, float qfac )
{
   float m[4][4] ;
   double a,b=qb,c=qc,d=qd , xd,yd,zd ;

   /* last row is always [ 0 0 0 1 ] */

   m[3][0]=m[3][1]=m[3][2] = 0.0 ; m[3][3]= 1.0 ;

   /* compute a parameter from b,c,d */

   a = 1.0l - (b*b + c*c + d*d) ;
   if( a < 1.e-7l ){                   /* special case */
     a = 1.0l / std::sqrt(b*b+c*c+d*d) ;
     b *= a ; c *= a ; d *= a ;        /* normalize (b,c,d) vector */
     a = 0.0l ;                        /* a = 0 ==> 180 degree rotation */
   } else{
     a = std::sqrt(a) ;                     /* angle = 2*arccos(a) */
   }

   /* load rotation matrix, including scaling factors for voxel sizes */

   xd = (dx > 0.0) ? dx : 1.0l ;       /* make sure are positive */
   yd = (dy > 0.0) ? dy : 1.0l ;
   zd = (dz > 0.0) ? dz : 1.0l ;

   if( qfac < 0.0 ) zd = -zd ;         /* left handedness? */

   m[0][0] =        (a*a+b*b-c*c-d*d) * xd ;
   m[0][1] = 2.0l * (b*c-a*d        ) * yd ;
   m[0][2] = 2.0l * (b*d+a*c        ) * zd ;
   m[1][0] = 2.0l * (b*c+a*d        ) * xd ;
   m[1][1] =        (a*a+c*c-b*b-d*d) * yd ;
   m[1][2] = 2.0l * (c*d-a*b        ) * zd ;
   m[2][0] = 2.0l * (b*d-a*c        ) * xd ;
   m[2][1] = 2.0l * (c*d+a*b        ) * yd ;
   m[2][2] =        (a*a+d*d-c*c-b*b) * zd ;

   /* load offsets */

   m[0][3] = qx ; m[1][3] = qy ; m[2][3] = qz ;

   TransformationMatrix tm;
   tm.setMatrix(m);
   return tm;
}

/*----------------------------------------------------------------------*/
/*! compute the determinant of a 3x3 matrix
*//*--------------------------------------------------------------------*/
float
NiftiFileHeader::nifti_mat33_determ( mat33 R )   /* determinant of 3x3 matrix */
{
   double r11,r12,r13,r21,r22,r23,r31,r32,r33 ;
                                                       /*  INPUT MATRIX:  */
   r11 = R.m[0][0]; r12 = R.m[0][1]; r13 = R.m[0][2];  /* [ r11 r12 r13 ] */
   r21 = R.m[1][0]; r22 = R.m[1][1]; r23 = R.m[1][2];  /* [ r21 r22 r23 ] */
   r31 = R.m[2][0]; r32 = R.m[2][1]; r33 = R.m[2][2];  /* [ r31 r32 r33 ] */

   return r11*r22*r33-r11*r32*r23-r21*r12*r33
         +r21*r32*r13+r31*r12*r23-r31*r22*r13 ;
}

/*----------------------------------------------------------------------*/
/*! multiply 2 3x3 matrices
*//*--------------------------------------------------------------------*/
NiftiFileHeader::mat33
NiftiFileHeader::nifti_mat33_mul( mat33 A , mat33 B )  /* multiply 2 3x3 matrices */
{
   mat33 C ; int i,j ;
   for( i=0 ; i < 3 ; i++ )
    for( j=0 ; j < 3 ; j++ )
      C.m[i][j] =  A.m[i][0] * B.m[0][j]
                 + A.m[i][1] * B.m[1][j]
                 + A.m[i][2] * B.m[2][j] ;
   return C ;
}

/**
 * analyze HDR file is actually a NIFTI HDR file.
 */
bool
NiftiFileHeader::hdrIsNiftiFile(const QString& hdrFileName)
{
   QFile file(hdrFileName);
   if (file.open(QIODevice::ReadOnly)) {
      //
      // read bytes 348
      //
      int numBytesToRead = 348;
      char bytes[numBytesToRead];
      QDataStream stream(&file);
      stream.setVersion(QDataStream::Qt_4_3);
      const bool errorFlag = (stream.readRawData(bytes, numBytesToRead) != numBytesToRead);
      file.close();
      if (errorFlag) {
         return false;
      }

      //
      // Is this the NIFTI code "ni1"?
      //
      if ((bytes[344] == 'n') &&
          (bytes[345] == 'i') &&
          (bytes[346] == '1')) {
         return true;
      }
   }

   return false;
}
