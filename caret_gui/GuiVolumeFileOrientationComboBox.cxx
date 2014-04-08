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

#include "GuiVolumeFileOrientationComboBox.h"

/**
 * Constructor.
 */
GuiVolumeFileOrientationComboBox::GuiVolumeFileOrientationComboBox(QWidget* parent, 
                                               const VolumeFile::VOLUME_AXIS axisIn,
                                               const char* /* name */)
   : QComboBox(parent)
{
   switch (axisIn) {
      case VolumeFile::VOLUME_AXIS_X:
         addItem("X-Unknown", VolumeFile::ORIENTATION_UNKNOWN);
         addItem("X-Right to Left", VolumeFile::ORIENTATION_RIGHT_TO_LEFT);
         addItem("X-Left to Right", VolumeFile::ORIENTATION_LEFT_TO_RIGHT);
         addItem("X-Posterior to Anterior", VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR);
         addItem("X-Anterior to Posterior", VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR);
         addItem("X-Inferior to Superior", VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR);
         addItem("X-Superior to Inferior", VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR);
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         addItem("Y-Unknown", VolumeFile::ORIENTATION_UNKNOWN);
         addItem("Y-Right to Left", VolumeFile::ORIENTATION_RIGHT_TO_LEFT);
         addItem("Y-Left to Right", VolumeFile::ORIENTATION_LEFT_TO_RIGHT);
         addItem("Y-Posterior to Anterior", VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR);
         addItem("Y-Anterior to Posterior", VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR);
         addItem("Y-Inferior to Superior", VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR);
         addItem("Y-Superior to Inferior", VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR);
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         addItem("Z-Unknown", VolumeFile::ORIENTATION_UNKNOWN);
         addItem("Z-Right to Left", VolumeFile::ORIENTATION_RIGHT_TO_LEFT);
         addItem("Z-Left to Right", VolumeFile::ORIENTATION_LEFT_TO_RIGHT);
         addItem("Z-Posterior to Anterior", VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR);
         addItem("Z-Anterior to Posterior", VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR);
         addItem("Z-Inferior to Superior", VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR);
         addItem("Z-Superior to Inferior", VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR);
         break;
      case VolumeFile::VOLUME_AXIS_ALL:
         addItem("Unknown", VolumeFile::ORIENTATION_UNKNOWN);
         addItem("Right to Left", VolumeFile::ORIENTATION_RIGHT_TO_LEFT);
         addItem("Left to Right", VolumeFile::ORIENTATION_LEFT_TO_RIGHT);
         addItem("Posterior to Anterior", VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR);
         addItem("Anterior to Posterior", VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR);
         addItem("Inferior to Superior", VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR);
         addItem("Superior to Inferior", VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR);
         break;
      case VolumeFile::VOLUME_AXIS_OBLIQUE:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
      case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
         addItem("Unknown", VolumeFile::ORIENTATION_UNKNOWN);
         addItem("Right to Left", VolumeFile::ORIENTATION_RIGHT_TO_LEFT);
         addItem("Left to Right", VolumeFile::ORIENTATION_LEFT_TO_RIGHT);
         addItem("Posterior to Anterior", VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR);
         addItem("Anterior to Posterior", VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR);
         addItem("Inferior to Superior", VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR);
         addItem("Superior to Inferior", VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR);
         break;
      case VolumeFile::VOLUME_AXIS_UNKNOWN:
         addItem("Unknown", VolumeFile::ORIENTATION_UNKNOWN);
         addItem("Right to Left", VolumeFile::ORIENTATION_RIGHT_TO_LEFT);
         addItem("Left to Right", VolumeFile::ORIENTATION_LEFT_TO_RIGHT);
         addItem("Posterior to Anterior", VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR);
         addItem("Anterior to Posterior", VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR);
         addItem("Inferior to Superior", VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR);
         addItem("Superior to Inferior", VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR);
         break;
   }
}

/**
 * Destructor.
 */
GuiVolumeFileOrientationComboBox::~GuiVolumeFileOrientationComboBox()
{
}

/**
 * set the orientation.
 */
void 
GuiVolumeFileOrientationComboBox::setOrientation(const VolumeFile::ORIENTATION orient)
{
   setCurrentIndex(static_cast<int>(orient));
}

/**
 * get the orientation.
 */
VolumeFile::ORIENTATION 
GuiVolumeFileOrientationComboBox::getOrientation() const
{
   return static_cast<VolumeFile::ORIENTATION>(currentIndex());
}
