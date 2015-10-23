#include "rm_rid.h"

///////CONSTRUCTEURS ET DESTRUCTEUR
//constructeur sans arguments
RID :: RID()
{
	this->pageNum = -1;
	this->slotNum = -1;
	this->viableRid = false;
};

//constructeur avec arguments
RID :: RID(PageNum pageNum, SlotNum slotNum)
{
	this->pageNum = pageNum;
	this->slotNum = slotNum;
	this->viableRid = true;

};

//destructeur
RID :: ~RID()
{
	
};

//*this = rid
RID& RID :: operator=(const RID &rid)
{
	this->pageNum = rid.pageNum;
	this->slotNum = rid.slotNum;
	this->viableRid = rid.viableRid;
		
	return *this;
};




////////GETTER
//pageNum est initialisé avec la valeur PageNum de l'objet
RC RID :: GetPageNum(PageNum &pageNum) const
{
	if(!viableRid)
	{
		return RM_RID_NOT_VIABLE;
	}

pageNum = this->pageNum;
return 0;	
	
};

//slotNum est initialisé avec la valeur de SlotNum de l'objet
RC RID :: GetSlotNum(SlotNum &slotNum) const
{
	if(!viableRid)
	{
		return RM_RID_NOT_VIABLE;
	}

slotNum = this->slotNum;	
return 0;
};

///////SETTER
//Pour cette fonction et la suivante nous vérifions que le fichier a bien été chargé par RM_RECORD :: GetRid()
RC RID :: SetPageNum(const PageNum &pageNum)
{
if(!this->viableRid)
{
	return RM_RID_NOT_VIABLE;
}
this->pageNum = pageNum;
return 0;
};

RC RID :: SetSlotNum(const SlotNum &slotNum)
{
if(!this->viableRid)
{
	return RM_RID_NOT_VIABLE;
}

this->slotNum = slotNum;
return 0;
}

RC RID :: SetViableRid(const bool cond)
{
this->viableRid = cond;
return 0;
}

