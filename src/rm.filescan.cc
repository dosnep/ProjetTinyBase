#include "rm.h"
#include <cstring>
#include <stdlib.h>

   
RM_FileScan :: RM_FileScan  ()
{
	this->scanOpen = false;
	this->rfh = NULL;
	
};

RM_FileScan :: ~RM_FileScan ()
{
	
};

RC RM_FileScan :: OpenScan  (const RM_FileHandle &fileHandle,
                  AttrType   attrType,
                  int        attrLength,
                  int        attrOffset,
                  CompOp     compOp,
                  void       *value,
                  ClientHint pinHint)
{
int res;

this->scanOpen = true; //On ouvre le scan du fichier
this->rfh = new RM_FileHandle(*fileHandle.pf, fileHandle.fh);
this->attrType = attrType;
this->attrLength = attrLength;
this->attrOffset = attrOffset;
this->compOp = compOp;

switch(attrType){
case INT:{
	
	valInt = *((int*)(value));
	break;
		}

case FLOAT:{
	valFloat = *((float*)(value));
	break;
			}

case STRING:{
	if(attrLength<=MAXSTRINGLEN&&attrLength>=1)
	valString = new char[attrLength];
	memcpy(valString, value, attrLength);
	break;
	
			}
}

PF_PageHandle *tmpPage = new PF_PageHandle();
//On passe le file header
res = this->rfh->pf->GetNextPage(0, *tmpPage);
	if(res !=0)
	{
		delete tmpPage;
		return res;
	}


res = tmpPage->GetPageNum(this->currentPage);
		if(res !=0)
	{
		return res;
	}



return 0;	
};


RC RM_FileScan :: GetNextRec(RM_Record &rec)
{
	
	
return 0;	
};           


bool RM_FileScan ::  EstUnBonRecord(char *pData)
{
	
switch(this->attrType)
{	
	case INT :{
		
		pData += this->attrOffset;
		int tmpVal = atoi(pData);
		
		switch(this->compOp)
		{
			case EQ_OP :{
				
				if(tmpVal == valInt) return true;
				
				break;
						}
			
			
			case LT_OP :{
				
				if(tmpVal < valInt) return true;
				
				break;
						}
						
			case GT_OP :{
				
				if(tmpVal > valInt) return true;
				
				break;
						}			
			
			case LE_OP :{
				
				if(tmpVal <= valInt) return true;
				
				break;
						}
			
			
			case GE_OP :{
				
				if(tmpVal >= valInt) return true;
				
				break;
						}
			
			case NE_OP :{
				
				if(tmpVal != valInt) return true;
				
				break;
						}
			
			
			case NO_OP :{
				
				return true;
				break;
						}
			
				
		}
		
		
		break;
	}
	
	case FLOAT :{
		
		pData += this->attrOffset;
		float tmpVal = atof(pData);
		
		switch(this->compOp)
		{
					case EQ_OP :{
				
				if(tmpVal == valFloat) return true;
				
				break;
						}
			
			
			case LT_OP :{
				
				if(tmpVal < valFloat) return true;
				
				break;
						}
						
			case GT_OP :{
				
				if(tmpVal > valFloat) return true;
				
				break;
						}			
			
			case LE_OP :{
				
				if(tmpVal <= valFloat) return true;
				
				break;
						}
			
			
			case GE_OP :{
				
				if(tmpVal >= valFloat) return true;
				
				break;
						}
			
			case NE_OP :{
				
				if(tmpVal != valFloat) return true;
				
				break;
						}
			
			
			case NO_OP :{
				
				return true;
				break;
						}
			
			
		}
		
		
		
		
		
		
		break;
	}
	
	case STRING :{
		
		char *tmpString = pData+this->attrOffset;
		
		switch(this->compOp)
		{
			case EQ_OP :{
				
				if(strncmp(tmpString, pData, this->attrLength) == 0) return true;
				
				break;
						}
			
			
			case LT_OP :{
				
				if(strncmp(tmpString, pData, this->attrLength)<0) return true;
				
				break;
						}
						
			case GT_OP :{
				
				if(strncmp(tmpString, pData, this->attrLength) > 0) return true;
				
				break;
						}			
			
			case LE_OP :{
				
				if(strncmp(tmpString, pData, this->attrLength) <= 0) return true;
				
				break;
						}
			
			
			case GE_OP :{
				
				if(strncmp(tmpString, pData, this->attrLength) >= 0) return true;
				
				break;
						}
			
			case NE_OP :{
				
				if(strncmp(tmpString, pData, this->attrLength) != 0) return true;
				
				break;
						}
			
			
			case NO_OP :{
				
				return true;
				break;
						}
			
			
		}
		
		
		
		break;
	}
	
	
	
}	
	
return false;	
}


RC RM_FileScan :: CloseScan ()
{
this->scanOpen = false;
return 0;	
};                    
