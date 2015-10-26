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
    RID(const PageNum &pageNum, const SlotNum &slotNum);
    ~RID();                                        // Destructor


	//getter
    RC GetPageNum(PageNum &pageNum) const;         // Return page number
    RC GetSlotNum(SlotNum &slotNum) const;         // Return slot number


private:
	PageNum pageNum; // Numéro associé à la page
	SlotNum slotNum; //Slot associé à l'enregistrement
	bool viableRid;	//true si le RID a était initialisé, false sinon
	
friend class RM_Record;
friend class RM_FileHandle;
};

#endif

#define RM_RID_NOT_VIABLE 1; //Erreur si viableRid est false


