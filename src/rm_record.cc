#include "rm.h"
#include <cstring>


RM_Record :: RM_Record()
{
	this->rid = NULL;
	this->pData = NULL;
	this->viableRecord = false;
	this->recordSize = -1;

};

RM_Record :: RM_Record(const PageNum &pageNum, const SlotNum &slotNum, const char* pData, const int &recordSize)
{
	this->rid = new RID(pageNum, slotNum);
	this->recordSize = recordSize;	
	this->viableRecord = true;
	this->pData = new char[this->recordSize];
	strcpy(this->pData, pData);
};


RM_Record :: ~RM_Record()
{
	delete this->rid;
	this->rid = NULL;
	
	if(pData)
	{
		delete [] pData;
		pData = NULL;
	}
	
};

//////getter
//pData prend la valeur de this->pData
RC RM_Record :: GetData(char *&pData) const
{
	if(!this->viableRecord)
	{
		return RM_RECORD_NOT_VIABLE;
	}

	//memcpy(pData, this->pData, this->recordSize);
	pData = this->pData;
	return 0;
};

//rid prend la valeur de this->rid
RC RM_Record :: GetRid (RID &rid) const
{
	
	if(!this->viableRecord)
	{
		return RM_RECORD_NOT_VIABLE;
	}

	this->rid->GetPageNum(rid.pageNum);
	this->rid->GetSlotNum(rid.slotNum);	
	return 0;
}

