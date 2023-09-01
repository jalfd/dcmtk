// -*- mode: c++; c-basic-offset: 4;  -*-   (emacs c++ mode)
#include "dcmtk/dcmdata/dcelist.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/ofstd/ofconsol.h"
#include <vector>
#include <algorithm>

class DcmElementList::Impl {
public:
    Impl()
	: currentIndex(-1) {}

    DcmElement *append(DcmElement *obj) {
	return insert(obj);
    }

    DcmElement *prepend(DcmElement *obj) {
	return insert(obj);
    }

    DcmElement *insert(DcmElement *obj,
		       E_ListPos = ELP_next) {
	// We ignore listPost as we know that DcmItem keeps items strictly sorted
	if (obj) {
	    std::vector<DcmElement*>::iterator it =
		std::lower_bound(list.begin(), list.end(), obj, tag_compare);
	    currentIndex = it - list.begin();
	    list.insert(it, obj);
	}
	return obj;
    }

    DcmElement *remove() {
	if (!valid())
	    return 0;
	DcmElement *elem = list.at(currentIndex);
	list.erase(list.begin() + currentIndex);
	if (currentIndex > (int)(list.size())-1)
	    currentIndex = -1;
	return elem;
    }

    DcmElement *get(E_ListPos pos = ELP_atpos) {
	return seek(pos);
    }

    DcmElement *seek(E_ListPos pos = ELP_next) {
	switch (pos) {
	case ELP_first :
	    currentIndex = !list.empty() ? 0 : -1;
	    break;
	case ELP_last :
	    currentIndex = !list.empty() ? list.size()-1 : -1;
	    break;
	case ELP_prev :
	    if (currentIndex >= 0)
		--currentIndex;
	    break;
	case ELP_next :
	    if (currentIndex >= 0 && currentIndex <= (int)(list.size())-2)
		++currentIndex;
	    else
		currentIndex = -1;
	    break;
	default:
	    break;
	}
	return valid() ? list.at(currentIndex) : 0;
    }

    DcmElement *seek_to(unsigned long absolute_position) {
        currentIndex = std::min<int>(absolute_position, list.size()-1);;
	return valid() ?  list.at(currentIndex) : 0;
    }

    inline unsigned long card() const { return list.size(); }
    inline OFBool empty(void) const { return list.empty(); }
    inline OFBool valid(void) const { return currentIndex >= 0; }

    OFCondition insert(DcmElement *elem,
		       OFBool replaceOld,
		       OFBool checkInsertOrder) {
	OFCondition errorFlag = EC_Normal;

	std::vector<DcmElement*>::iterator it;
	if (!list.empty() && elem->getTag() > list.back()->getTag())
	    it = list.end();
	else
	    it = std::lower_bound(list.begin(),list.end(), elem, tag_compare);

	if (checkInsertOrder && it != list.end()) {
	    ofConsole.lockCerr()
		<< "DcmItem: Dataset not in ascending tag order, at element "
                << elem->getTag() << std::endl;
	    ofConsole.unlockCerr();
	}

	if (it != list.end() && (*it)->getTag() == elem->getTag()) {
	    if (!replaceOld) {
		errorFlag = EC_DoubledTag;
	    } else {
		currentIndex = it - list.begin();
		delete *it;
		*it = elem;
	    }
	} else {
	    currentIndex = it - list.begin();
	    list.insert(it, elem);
	}
	return errorFlag;
    }

    DcmElement *find(const DcmTagKey &tagKey) {
	std::vector<DcmElement*>::iterator it =
	    std::lower_bound(list.begin(), list.end(), tagKey, tag_compare_key);
	if (it != list.end() && (*it)->getTag() == tagKey) {
	    currentIndex = it - list.begin();
	    return *it;
	} else {
	    currentIndex = -1;
	    return 0;
	}
    }

    void deleteAllElements() {
        for (const auto element : list) {
            delete element;
        }
        list.clear();
    }

private:
    static inline bool tag_compare(const DcmElement *o1, const DcmElement *o2) {
	return o1->getTag() < o2->getTag();
    }

    static inline bool tag_compare_key(const DcmElement *o1, const DcmTagKey &key) {
	return o1->getTag() < key;
    }

    std::vector<DcmElement*> list;
    int currentIndex;
};

DcmElementList::DcmElementList()
  : impl(new Impl) {}

DcmElementList::~DcmElementList() {
    delete impl;
}

DcmElement *
DcmElementList::append(DcmElement *obj) {
    return impl->append(obj);
}

DcmElement *
DcmElementList::prepend(DcmElement *obj) {
    return impl->prepend(obj);
}

DcmElement *
DcmElementList::insert(DcmElement *obj, E_ListPos pos) {
    return impl->insert(obj, pos);
}

DcmElement *DcmElementList::remove() {
    return impl->remove();
}

DcmElement *DcmElementList::get(E_ListPos pos) {
    return impl->get(pos);
}

DcmElement *DcmElementList::seek(E_ListPos pos) {
    return impl->seek(pos);
}

DcmElement *DcmElementList::seek_to(unsigned long absolute_position) {
    return impl->seek_to(absolute_position);
}

unsigned long DcmElementList::card() const {
    return impl->card();
}

OFBool DcmElementList::empty(void) const {
    return impl->empty();
}

OFBool DcmElementList::valid(void) const {
    return impl->valid();
}

OFCondition DcmElementList::insert(DcmElement *elem,
				   OFBool replaceOld,
				   OFBool checkInsertOrder) {
    return impl->insert(elem, replaceOld, checkInsertOrder);
}

DcmElement *DcmElementList::find(const DcmTagKey &tagKey) {
    return impl->find(tagKey);
}

void DcmElementList::deleteAllElements() {
    impl->deleteAllElements();
}
