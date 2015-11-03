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
this->value = value;



//On teste si l'attribut du fichier est de type string, int ou float et on assigne la value dans son bon type
if(value != NULL){
switch(attrType){
case INT:{
	
	this->valInt = *((int*)(value));
	break;
		}

case FLOAT:{
	this->valFloat = *((float*)(value));
	break;
			}

case STRING:{
	if(attrLength<=MAXSTRINGLEN&&attrLength>=1)
	this->valString = new char[attrLength];
	memcpy(valString, value, attrLength);
	break;
	
			}
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

//On récupère le numéro de la page courante
res = tmpPage->GetPageNum(this->currentPage);
		if(res !=0)
	{
		return res;
	}

this->currentSlot = -1;


//On récupère le numéro de la dernière page
res = this->rfh->pf->GetLastPage(*tmpPage);
	if(res !=0)
	{
		delete tmpPage;
		return res;
	}
res = tmpPage->GetPageNum(this->numLastPage);
		if(res !=0)
	{
		return res;
	}



this->rfh->pf->UnpinPage(currentPage);
return 0;	
};


RC RM_FileScan :: GetNextRec(RM_Record &rec)
{

	int res;
	int resGetSlot;
	bool goodRecord = false;
	
	//ON commence par récupérer la page courante
	PF_PageHandle *page= new PF_PageHandle();
	res = this->rfh->pf->GetThisPage(this->currentPage,*page);
		if(res !=0)
		{
			delete page;
			return res;
		}
		
	//On récupère les données de la page
	char *pData;
	page->GetData(pData);
	
	//On récupère le header de la page
	rm_PageHeader currentPageHeader;
	memcpy(&currentPageHeader, pData, sizeof(rm_PageHeader));
	
	//On va récupérer le prochaine slot à partir du bitmap du page header
	SlotNum nextSlot;
	resGetSlot = currentPageHeader.tab->GetNextSlot(this->currentSlot, nextSlot);
	
	//Tant que l'on a pas trouvé un slot qui ne remplit pas les conditions on charge la page suivante
	while(!goodRecord)
	{	
		//Si on a trouvé un enregistrement alors nous pouvons vérifier s'il remplit les conditions
		if(resGetSlot == 0)
		{
			//Si on a trouvé un slot on change la valeur courante du slot
			this->currentSlot = nextSlot;
			goodRecord = EstUnBonRecord(pData);


		}
		
		else{
		
			//Si nous sommes dans la dernière page, impossible d'aller à la suivante
			if(this->currentPage == this->numLastPage)
			{
				return RM_EOF;
			}
			
		
		//On charge la page suivante
		res = this->rfh->pf->GetNextPage(this->currentPage,*page);
				if(res != 0)
					return res;
		//On met à jour le numéro de page courant
		res = page->GetPageNum(this->currentPage);
				if(res != 0)
					return res;
		//On met à -1 le currentSlot car on va repartir du début du bitmap
		this->currentSlot = -1
		;
		
		//On récupère les données de la page
		res = page->GetData(pData);
			if(res != 0)
					return res;
		
		//On charge le page header
		memcpy(&currentPageHeader, pData, sizeof(rm_PageHeader));

		//On va chercher un slot 
		resGetSlot = currentPageHeader.tab->GetNextSlot(this->currentSlot, nextSlot);
		
		}

	}


RID *rid = new RID(this->currentPage,this->currentSlot);
this->rfh->GetRec(*rid,rec);
return 0;	
};           


bool RM_FileScan ::  EstUnBonRecord(char *pData)
{
	
switch(this->attrType)
{	
	
	if(this->value == NULL) return true;
	
	//On associe à chaque type les tests associés
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
//On ferme le scan
this->scanOpen = false;
return 0;	
};                    
