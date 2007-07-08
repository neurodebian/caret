
#ifndef __CARET_LINKED_LIST_H__
#define __CARET_LINKED_LIST_H__

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

/// Class for a linked list
class CaretLinkedList {
   public:
      /// class for a node in a linked list
      class Node {
         public:
            /// Constructor
            Node();
            
            /// Destructor
            virtual ~Node();
            
            /// get the next node
            Node* getNext() { return next; }
            
            /// get the prev node
            Node* getPrev() { return prev; }
            
         private:
            /// next node in list
            Node* next;
            
            /// prev node in list
            Node* prev;
         
         friend class CaretLinkedList;
      };
      
      /// Constructor
      CaretLinkedList();
      
      /// Destructor (Does not delete node, use clear for that)
      ~CaretLinkedList();
      
      /// clear the list
      void clear();
      
      /// see if an item is already in the list
      bool isInList(Node* n);
      
      /// add to front of list
      void pushFront(Node* n);
      
      /// add to front of list
      void pushFront(CaretLinkedList* n);
      
      /// add to end of list
      void pushBack(Node* n);
      
      /// get front of list
      Node* getFront() { return front; }
      
      /// get back of list
      Node* getBack() { return back; }
      
      /// remove a node from the list
      void remove(Node* n, const bool deleteNode);
      
      /// get list is empty
      bool empty() { return (front == 0); }
      
      /// get number of nodes in list
      int size();
      
   protected:
      /// front of list
      Node* front;
      
      /// back of list
      Node* back;
};

#endif //


