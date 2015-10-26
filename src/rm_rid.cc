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
RID :: RID(const PageNum &pageNum, const SlotNum &slotNum)
{
	this->pageNum = pageNum;
	this->slotNum = slotNum;
	this->viableRid = true;

};

//destructeur
RID :: ~RID()
{
	
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

