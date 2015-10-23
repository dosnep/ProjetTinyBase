#include "rm.h"
#include <cstring>


RM_Record :: RM_Record()
{
this->rid = new RID();
this->pData = NULL;
this->viableRecord = false;
this->recordSize = 0;

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

memcpy(pData, this->pData, this->recordSize);
return 0;
};

//rid prend la valeur de this->rid
RC RM_Record :: GetRid (RID &rid) const
{
	
	if(!this->viableRecord)
	{
		return RM_RECORD_NOT_VIABLE;
	}

rid = *this->rid;	
return 0;
}


/////setter
//this->pData = pData
RC RM_Record ::  SetData(const char *pData, const int recordSize)
{
	if(!this->viableRecord)
	{
		return RM_RECORD_NOT_VIABLE;
	}

this->recordSize = recordSize;
this->pData = new char[this->recordSize];
memcpy(this->pData, pData, this->recordSize);
	
return 0;
};

//this->rid = rid
RC RM_Record ::  SetRid(const RID &rid)
{
	if(!this->viableRecord)
	{
		return RM_RECORD_NOT_VIABLE;
	}	
	
*this->rid = rid;	
return 0;
};

//this->viableRecord = cond
RC RM_Record :: SetViableRecord(const bool cond)
{	
this->viableRecord = cond;	
return 0;
};
