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

#include "CaretLinkedList.h"

/**
 * Constructor.
 */
CaretLinkedList::Node::Node()
{
   next = 0;
   prev = 0;
}

/**
 * Destructor.
 */
CaretLinkedList::Node::~Node()
{
}            

//*******************************************************************************

/**
 * Constructor.
 */
CaretLinkedList::CaretLinkedList()
{
   front = 0;
   back  = 0;
}

/**
 * Destructor.
 */
CaretLinkedList::~CaretLinkedList()
{
   front = 0;
   back  = 0;
}

/** 
 * clear the list.
 */
void 
CaretLinkedList::clear()
{
   Node* n = front;
   while (n != 0) {
      Node* temp = n->next;
      delete n;
      n = temp;
   }
   front = 0;
   back  = 0;
}
      
/**
 * see if an item is already in the list.
 */
bool 
CaretLinkedList::isInList(Node* n)
{
   Node* l = front;
   while (l != 0) {
      if (l == n) {
         return true;
      }
      l = l->next;
   }
   return false;
}

/**
 * add to front of list (list passed has all elements removed).
 */
void 
CaretLinkedList::pushFront(CaretLinkedList* list)
{
   if (list->front == 0) {
      return;
   }
   
   if (front == 0) {
      front = list->front;
      back  = list->back;
   }
   else {
      Node* n = list->back;
      n->next = front;
      front->prev = n;
      front = list->front;
   }
   list->front = 0;
   list->back  = 0;
}

/**
 * add to front of list.
 */
void 
CaretLinkedList::pushFront(Node* n)
{
   if (front != 0) {
      front->prev = n;
   }
   
   n->next = front;
   n->prev = 0;
   front = n;
   if (back == 0) {
      back = n;
   }
}

/**
 * add to end of list.
 */
void 
CaretLinkedList::pushBack(Node* n)
{
   if (back != 0) {
      back->next = n;
   }
   
   n->prev = back;
   n->next = 0;
   back = n;
   if (front == 0) {
      front = n;
   }
}

/**
 * remove a node from the list.
 */
void 
CaretLinkedList::remove(Node* n, const bool deleteNode)
{
   Node* nextNode = n->next;
   Node* prevNode = n->prev;

   if (prevNode != 0) {
      prevNode->next = nextNode;
   }
   if (nextNode != 0) {
      nextNode->prev = prevNode;
   }
   
   if (front == n) {
      front = nextNode;
   }
   if (back == n) {
      back = prevNode;
   }
   
   n->next = 0;
   n->prev = 0;
   if (deleteNode) {
      delete n;
   }
}

/**
 * get number of nodes in list.
 */
int 
CaretLinkedList::size()
{
   int cnt = 0;
   
   Node* n = front;
   while (n != 0) {
      cnt++;
      n = n->next;
   }
   
   return cnt;
}
