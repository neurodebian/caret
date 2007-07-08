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


#ifndef __BRAIN_MODEL_OPENGL_SELECTED_ITEM_H__
#define __BRAIN_MODEL_OPENGL_SELECTED_ITEM_H__

class BrainModel;
class BrainModelContours;
class BrainModelSurface;
class BrainModelVolume;
class BrainSet;

/// This class contains information about a selected item
class BrainModelOpenGLSelectedItem { 
   public:

      /// types of selected items
      enum ITEM_TYPE {
         ITEM_TYPE_NONE,
         ITEM_TYPE_TILE,
         ITEM_TYPE_NODE,
         ITEM_TYPE_BORDER,
         ITEM_TYPE_BORDER_PROJ,
         ITEM_TYPE_VOLUME_BORDER,
         ITEM_TYPE_CELL_PROJECTION,
         ITEM_TYPE_VOLUME_CELL,
         ITEM_TYPE_CONTOUR,
         ITEM_TYPE_TRANSFORM_CONTOUR,
         ITEM_TYPE_CONTOUR_CELL,
         ITEM_TYPE_TRANSFORM_CONTOUR_CELL,
         ITEM_TYPE_TRANSFORM_CELL,
         ITEM_TYPE_CUT,
         ITEM_TYPE_FOCUS_PROJECTION,
         ITEM_TYPE_VOLUME_FOCI,
         ITEM_TYPE_TRANSFORM_FOCI,
         ITEM_TYPE_PALETTE_METRIC,
         ITEM_TYPE_PALETTE_SHAPE,
         ITEM_TYPE_VOXEL_UNDERLAY,
         ITEM_TYPE_VOXEL_OVERLAY_SECONDARY,
         ITEM_TYPE_VOXEL_OVERLAY_PRIMARY,
         ITEM_TYPE_VOXEL_FUNCTIONAL_CLOUD,
         ITEM_TYPE_LINK,
         ITEM_TYPE_TRANSFORMATION_MATRIX_AXES,
         ITEM_TYPE_VTK_MODEL
      };
      
      /// Constructor   
      BrainModelOpenGLSelectedItem();
      
      /// Destructor
      ~BrainModelOpenGLSelectedItem();
      
      /// Replace the current instance if these parameters are "better".
      /// returns true if replacement took place.
      bool replaceIfCloser(const float depthIn,
                           const float distIn,
                           const ITEM_TYPE itemTypeIn,
                           const int index1In,
                           const int index2In = -1,
                           const int index3In = -1,
                           const int index4In = -1,
                           const int index5In = -1,
                           const int index6In = -1);
                           
      /// Get the item index1
      int getItemIndex1() const { return index1; }
      
      /// Get the item index2
      int getItemIndex2() const { return index2; }
      
      /// Get the item index3
      int getItemIndex3() const { return index3; }
      
      /// Get the item index4
      int getItemIndex4() const { return index4; }
      
      /// Get the item index5
      int getItemIndex5() const { return index5; }
      
      /// Get the item index6
      int getItemIndex6() const { return index6; }
      
      /// Set the item index1
      void setItemIndex1(const int i)  {  index1 = i; }
      
      /// Set the item index2
      void setItemIndex2(const int i)  {  index2 = i; }
      
      /// Set the item index3
      void setItemIndex3(const int i)  {  index3 = i; }
      
      /// Set the item index4
      void setItemIndex4(const int i)  {  index4 = i; }
      
      /// Set the item index5
      void setItemIndex5(const int i)  {  index5 = i; }
      
      /// Set the item index6
      void setItemIndex6(const int i)  {  index6 = i; }
      
      /// Get the item type
      ITEM_TYPE getItemType() const { return itemType; }
   
      /// Get the depth of the selection (smaller is nearer to user)
      float getDepth() const { return depth; }
      
      /// Reinitialize this item
      void reset(BrainSet* brainSetIn,
                 BrainModel* brainModelIn,
                 const int viewingWindowNumberIn);
      
      /// get the brain set
      BrainSet* getBrainSet() { return brainSet; }
      
      /// get the brain model
      BrainModel* getBrainModel() { return brainModel; }
      
      /// get the brain model surface
      BrainModelSurface* getBrainModelSurface();
      
      /// get the brain model volume
      BrainModelVolume* getBrainModelVolume();
      
      /// get the brain model contours
      BrainModelContours* getBrainModelContours();
      
      /// get the viewing window number
      int getViewingWindowNumber() const { return viewingWindowNumber; }
      
   private:   
      /// BrainSet of the selected item
      BrainSet* brainSet;
      
      /// BrainModel of the selected item
      BrainModel* brainModel;
      
      /// viewing window number of the selected item
      int viewingWindowNumber;
      
      /// Depth of selected item
      float depth;
      
      /// Distance of item from pick position
      float distToPick;
      
      /// Item type
      ITEM_TYPE itemType;
      
      /// First index
      int index1;
      
      /// Second index
      int index2;
      
      /// Third index
      int index3;
      
      /// Fourth index
      int index4;
      
      /// Fifth index
      int index5;
      
      /// Sixth index
      int index6;
};

#endif


