/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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

#ifndef DCLIST_H
#define DCLIST_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/oftypes.h"

#include "dcmtk/dcmdata/dcobject.h"

/// index indicating "end of list"
const unsigned long DCM_EndOfListIndex = OFstatic_cast(unsigned long, -1L);

/** helper class maintaining an entry in a DcmList double-linked list
 */
class DCMTK_DCMDATA_EXPORT DcmListNode 
{

public:
    /** constructor
     *  @param obj object to be maintained by this list node
     */
    DcmListNode( DcmObject *obj );

    /// destructor
    ~DcmListNode();

    /// return pointer to object maintained by this list node
    inline DcmObject *value() { return objNodeValue; } 

private:
    friend class DcmList;
    friend class DcmListPosition;

    /// pointer to next node in double-linked list
    DcmListNode *nextNode;

    /// pointer to previous node in double-linked list
    DcmListNode *prevNode;

    /// pointer to DcmObject instance maintained by this list entry
    DcmObject *objNodeValue;

    /// private undefined copy constructor 
    DcmListNode(const DcmListNode &);

    /// private undefined copy assignment operator 
    DcmListNode &operator=(const DcmListNode &);

};

/// list position indicator
typedef enum
{
    /// at current position in list
    ELP_atpos,

    /// at list start
    ELP_first,

    /// at list end
    ELP_last,

    /// before current list position
    ELP_prev,

    /// after current list position
    ELP_next
} E_ListPos;

class DcmList;

class DCMTK_DCMDATA_EXPORT DcmListPosition
{
public:
    /// constructor
    DcmListPosition(DcmList *list_);

    /** insert object at end of list
     *  @param obj pointer to object
     *  @return pointer to object
     */
    DcmObject *append(  DcmObject *obj );

    /** insert object at start of list
     *  @param obj pointer to object
     *  @return pointer to object
     */
    DcmObject *prepend( DcmObject *obj );

    /** insert object relative to current position and indicator
     *  @param obj pointer to object
     *  @param pos position indicator
     *  @return pointer to object
     */
    DcmObject *insert(  DcmObject *obj,
                        E_ListPos pos = ELP_next );

    /** remove current entry from list, return element
     *  @return pointer to removed element, which is not deleted
     */
    DcmObject *remove();

    /** seek within element in list to given position
     *  (i.e. set current element to given position)
     *  @param pos position indicator
     *  @return pointer to new current object
     */
    DcmObject *seek(    E_ListPos pos = ELP_next );

    /** seek within element in list to given element index
     *  (i.e. set current element to given index)
     *  @param absolute_position position index < card()
     *  @return pointer to new current object
     */
    DcmObject *seek_to(unsigned long absolute_position);

    /** get pointer to element in list at given position
     *  @param pos position indicator
     *  @return pointer to object
     */
    DcmObject *get(     E_ListPos pos = ELP_atpos );

    void reset();
    void reset(DcmListNode *new_pos);

    /// return true if current node exists, false otherwise
    inline OFBool valid(void) const { return currentNode != NULL; }

private:
    DcmList *list;

    /// pointer to current node in list
    DcmListNode *currentNode;
};

/** double-linked list class that maintains pointers to DcmObject instances.
 *  The remove operation does not delete the object pointed to, however,
 *  the destructor will delete all elements pointed to
 */
class DCMTK_DCMDATA_EXPORT DcmList : public DcmListPosition
{
public:
    /// constructor
    DcmList();

    /// destructor
    ~DcmList();

    /** insert object relative to current position and indicator
     *  @param obj pointer to object
     *  @param pos node to insert before
     *  @return pointer inserted node
     */
    DcmListNode *insertBefore(DcmObject *obj,
                              DcmListNode *node);

    /** remove current entry from list, return element
     *  @return pointer to removed element, which is not deleted
     */
    DcmObject *removeNode(DcmListNode *node);

    /** Remove and delete all elements from list. Thus, the 
     *  elements' memory is also freed by this operation. The list
     *  is empty after calling this function.
     */  
    void deleteAllElements();

    /// return cardinality of list
    inline unsigned long card() const { return cardinality; }

    /// return true if list is empty, false otherwise
    inline OFBool empty(void) const { return firstNode == NULL; }

    DcmListNode *head();

    DcmListNode *reverseHead();

private:
    /// pointer to first node in list
    DcmListNode *firstNode;

    /// pointer to last node in list
    DcmListNode *lastNode;

    /// number of elements in list
    unsigned long cardinality;
 
    /// private undefined copy constructor 
    DcmList &operator=(const DcmList &);

    /** private undefined copy assignment operator
     * @param newList documented to avoid doxygen warnings
     */
    DcmList(const DcmList &newList);
    friend class DcmListPosition;
};

#endif  // DCLIST_H
