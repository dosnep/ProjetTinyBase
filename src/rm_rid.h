//
// rm_rid.h
//
//   The Record Id interface
//

#ifndef RM_RID_H
#define RM_RID_H

// We separate the interface of RID from the rest of RM because some
// components will require the use of RID but not the rest of RM.

#include "tinybase.h"
#include "iostream"

//
// PageNum: uniquely identifies a page in a file
//
typedef int PageNum;

//
// SlotNum: uniquely identifies a record in a page
//
typedef int SlotNum;

//
// RID: Record id interface
//
class RID {
public:
    RID();                                         // Default constructor
    RID(PageNum pageNum, SlotNum slotNum);
    ~RID();                                        // Destructor

    RID& operator=(const RID &rid);


	//getter
    RC GetPageNum(PageNum &pageNum) const;         // Return page number
    RC GetSlotNum(SlotNum &slotNum) const;         // Return slot number


	//setter
	RC SetPageNum(const PageNum &pageNum); 			// Set this->pageNum
	RC SetSlotNum(const SlotNum &slotNum);			// Set this->slotNum
	RC SetViableRid(const bool cond);				// Set this->viableRid
	
private:
PageNum pageNum; // Numéro associé à la page
SlotNum slotNum; //Slot associé à l'enregistrement
bool viableRid;	//true si le RID a était initialisé, false sinon
};

#endif

#define RM_RID_NOT_VIABLE 1; //Erreur si viableRid est false


