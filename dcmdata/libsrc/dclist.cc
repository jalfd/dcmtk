/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: generic list class
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dclist.h"


// *****************************************
// *** DcmListNode *************************
// *****************************************


DcmListNode::DcmListNode( DcmObject *obj )
  : nextNode(NULL),
    prevNode(NULL),
    objNodeValue(obj)
{
}


// ********************************


DcmListNode::~DcmListNode()
{
}

DcmListPosition::DcmListPosition(DcmList *list_)
: list(list_),
currentNode(nullptr) {}

// *****************************************
// *** DcmList *****************************
// *****************************************


DcmList::DcmList()
  : DcmListPosition(this),
    firstNode(NULL),
    lastNode(NULL),
    cardinality(0)
{
}


// ********************************


DcmList::~DcmList()
{
    if ( !DcmList::empty() )                      // list is not empty !
    {
        lastNode->nextNode = NULL;                // set to 0 for safety reasons
        do {
            DcmListNode *temp = firstNode;
            firstNode = firstNode->nextNode;
            // delete temp->objNodeValue;         // dangerous!
            delete temp;
        } while ( firstNode != NULL );
        firstNode = lastNode = NULL;
    }
}


// ********************************


DcmObject *DcmListPosition::append( DcmObject *obj )
{
    if (obj != NULL)
    {
        currentNode = list->insertBefore(obj, nullptr);
    } // obj == NULL
    return obj;
}

// ********************************


DcmObject *DcmListPosition::prepend( DcmObject *obj )
{
    if (obj != NULL)
    {
        currentNode = list->insertBefore(obj, list->head());
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmListPosition::insert( DcmObject *obj, E_ListPos pos )
{
    if ( obj != NULL )
    {
        if ( list->empty() )                 // list is empty !
        {
            currentNode = list->insertBefore(obj, nullptr);
            return obj;
        }

        if (pos == ELP_last)
            append(obj); // cardinality++;
        else if (pos == ELP_first)
            prepend(obj); // cardinality++;
        else if (!valid())
            // set current node to the end if there is no predecessor or
            // there are successors to be determined
            append(obj);          // cardinality++;
        else if (pos == ELP_prev) // insert before current node
        {
            currentNode = list->insertBefore(obj, currentNode);
        }
        else //( pos==ELP_next || pos==ELP_atpos )
             // insert after current node
        {
            currentNode = list->insertBefore(obj, currentNode->nextNode);
        }
    } // obj == NULL
    return obj;
}

DcmObject *DcmListPosition::remove()
{
    DcmListNode *node_to_remove = currentNode;
    currentNode = currentNode->nextNode;
    return list->removeNode(node_to_remove);
}

DcmListNode *DcmList::insertBefore( DcmObject *obj, DcmListNode *pos )
{
    DcmListNode *new_node = new DcmListNode(obj);

    if (empty())
    {
        firstNode = lastNode = new_node;
    }
    else if (!pos)
    {
        lastNode->nextNode = new_node;
        new_node->prevNode = lastNode;
        lastNode = new_node;
    }
    else
    {
        DcmListNode *old_prev = pos->prevNode;
        pos->prevNode = new_node;
        new_node->nextNode = pos;

        new_node->prevNode = old_prev;
        if (old_prev) {
            old_prev->nextNode = new_node;
        }
        if (firstNode == pos)
        {
            firstNode = new_node;
        }
    }

    DcmListPosition::reset(new_node);
    ++cardinality;
    return new_node;
}

// ********************************

DcmObject *DcmList::removeNode(DcmListNode *node)
{
    DcmObject *tempobj;
    DcmListNode *tempnode;

    if ( DcmList::empty() )                        // list is empty !
        return NULL;
    else if ( !node )
        return NULL;                               // current node is 0
    else
    {
        tempnode = node;

        if ( node->prevNode == NULL )
            firstNode = node->nextNode;     // delete first element
        else
            node->prevNode->nextNode = node->nextNode;

        if ( node->nextNode == NULL )
            lastNode = node->prevNode;      // delete last element
        else
            node->nextNode->prevNode = node->prevNode;

        DcmListPosition::reset(node->nextNode);
        tempobj = tempnode->value();
        delete tempnode;
        cardinality--;
        return tempobj;
    }
}


// ********************************


DcmObject *DcmListPosition::get( E_ListPos pos )
{
    return seek( pos );
}

void DcmListPosition::reset()
{
    reset(nullptr);
}

void DcmListPosition::reset(DcmListNode *new_pos)
{
    currentNode = new_pos;
}

// ********************************


DcmObject *DcmListPosition::seek( E_ListPos pos )
{
    switch (pos)
    {
        case ELP_first :
            currentNode = list->head();
            break;
        case ELP_last :
            currentNode = list->reverseHead();
            break;
        case ELP_prev :
            if ( valid() )
                currentNode = currentNode->prevNode;
            break;
        case ELP_next :
            if ( valid() )
                currentNode = currentNode->nextNode;
            break;
        default:
            break;
    }
    list->reset(currentNode);
    return valid() ? currentNode->value() : nullptr;
}

// ********************************


DcmObject *DcmListPosition::seek_to(unsigned long absolute_position)
{
    const unsigned long tmppos = absolute_position < list->card() ? absolute_position : list->card();
    DcmListNode *where = list->head();
    for (unsigned long i = 0; i < tmppos && where; ++i) {
        where = where->nextNode;
    }
    currentNode = where;
    list->reset(currentNode);
    return where ? where->value() : NULL;
}

// ********************************


void DcmList::deleteAllElements()
{
    unsigned long numElements = cardinality;
    DcmListNode* tmpNode = NULL;
    DcmObject* tmpObject = NULL;
    // delete all elements
    for (unsigned long i = 0; i < numElements; i++)
    {
        // always select first node so no search is necessary
        tmpNode = firstNode;
        // clear value of node
        tmpObject = tmpNode->value();
        if (tmpObject != NULL)
        {
          // delete load of selected list node
          delete tmpObject;
          tmpObject = NULL;
        }
        firstNode = tmpNode->nextNode;
        // delete the list node itself
        delete tmpNode;
    }
    // reset all attributes for later use
    firstNode = NULL;
    lastNode = NULL;
    DcmListPosition::reset();
    cardinality = 0;
}

    DcmListNode *DcmList::head()
    {
        return firstNode;
    }

    DcmListNode *DcmList::reverseHead()
    {
        return lastNode;
    }
