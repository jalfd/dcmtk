// -*- mode: c++; c-basic-offset: 4;  -*-   (emacs c++ mode)
#ifndef __DCELIST_H
#define __DCELIST_H

#include "dcmtk/dcmdata/dclist.h"

class DcmElement;

/// DcmElementList is a list with an interface that is similar enough to DcmList, that
/// it can be used almost like a drop-in replacement as DcmItems internal representation
class DCMTK_DCMDATA_EXPORT DcmElementList {
public:
    DcmElementList();
    ~DcmElementList();

    DcmElement *append(DcmElement *obj);

    DcmElement *prepend(DcmElement *obj);

    DcmElement *insert(DcmElement *obj,
		       E_ListPos = ELP_next);

    DcmElement *remove();

    DcmElement *get(E_ListPos pos = ELP_atpos);

    DcmElement *seek(E_ListPos pos = ELP_next);

    DcmElement *seek_to(unsigned long absolute_position);

    unsigned long card() const;
    OFBool empty(void) const;
    OFBool valid(void) const;

    int getCurrentIndex() const;

    OFCondition insert(DcmElement *elem,
		       OFBool replaceOld,
		       OFBool checkInsertOrder);

    DcmElement *find(const DcmTagKey &tagKey);

    void deleteAllElements();

private:
    DcmElementList &operator=(const DcmElementList &);
    DcmElementList(const DcmElementList &);

    class Impl;
    Impl *impl;
};


#endif // __DCELIST_H
