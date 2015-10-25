#include "rm.h"
#include <cstring>

RM_FileHandle :: RM_FileHandle()
{
	this->viableFile = false;
	this->pf = NULL;
	
};

RM_FileHandle :: ~RM_FileHandle()
{
	delete this->pf;
	this->pf = NULL;
};


//Récupère l'enregistrement à partir de rid et le stocke dans rec
RC RM_FileHandle :: GetRec(const RID &rid, RM_Record &rec) const
{
	
int res;
	
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;
		
rec.SetViableRecord(true);
rec.SetRid(rid);

//On récupère les coordonnées à partir du rid
PageNum pageNum;
SlotNum slotNum;

res = rid.GetPageNum(pageNum);
if(res !=0)
{
	return res;
}

res = rid.GetSlotNum(slotNum);
if(res !=0)
{
	return res;
}


//On va chercher la bonne page
PF_PageHandle *p = new PF_PageHandle();
res = this->pf->GetThisPage(pageNum,*p);

	if(res != 0)
	{
		delete p;
		return res;
		
	}

char *pData;
p->GetData(pData);
pData += sizeof(rm_PageHeader);//On passe le page header
pData +=slotNum*this->fh.recordSize;//On place le pointeur sur le slot slotNum

res = rec.SetData(pData, this->fh.recordSize);

if(res !=0)
{
	delete p;
	return res;
}



res = this->pf->UnpinPage(pageNum);


	if(res != 0)
	{
		delete p;
		return res;
		
	}

return 0;	
}

RC RM_FileHandle :: InsertRec  (const char *pData, RID &rid)
{
	
int res;
	
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;
	
int pageNum = GetNextFreePage();	//On récupère la premiere page libre	

rid.SetViableRid(true);
rid.SetPageNum(pageNum);
		
//On va chercher la bonne page
PF_PageHandle *page = new PF_PageHandle();
res = this->pf->GetThisPage(pageNum,*page);

	if(res != 0)
	{	delete page;
		return res;
	}
		
//On récupère les données de la page
char*tmp;
res = page->GetData(tmp);
	
	if(res !=0)
	{
		delete page;
		return res;
	}

//On récupère le header de la page
rm_PageHeader newPageHeader;
memcpy(&newPageHeader,tmp,sizeof(rm_PageHeader));

//On va chercher dans le bitmap le premier slot libre
SlotNum freeSlot;
res = newPageHeader.tab->GetFirstFree(freeSlot);
	
	if(res!=0)
	{
		delete page;
		return res;
	}

rid.SetSlotNum(freeSlot);

tmp += sizeof(rm_PageHeader); //On passe le header
tmp += freeSlot*sizeof(this->fh.recordSize);//On pointe sur le premier slot libre
memcpy(tmp, pData, sizeof(this->fh.recordSize));

//On va maintenant mettre le bit du slot utilisé à 1
res = newPageHeader.tab->SetSlot(freeSlot,1);

	if(res!=0)
	{
		delete page;
		return res;
	}	
//On test si la page est pleine à partir du bitmap
	if(newPageHeader.tab->IsFull())
	{
		this->fh.nextFreePage = newPageHeader.nextFreePage; 	//On change la premiere page libre dans le file header
		newPageHeader.nextFreePage = -1; //On retire notre page de la liste 
	}
	
//Il faut maintenant réécrire le page header dans le fichier

res = this->InsertPageHeader(pageNum, newPageHeader);
	if(res != 0)
	{
		delete page;
		return res;
	}


return 0;
}; 

//La fonction va tester si la page est plein auquel cas on va l'ajouter aux pages libres
//le bit du slot concerné sera mît à 0
RC RM_FileHandle :: DeleteRec(const RID &rid)
{
//On récupère le numéro de la page et le numéro du slot
PageNum pageNum;
SlotNum slotNum;
rid.GetPageNum(pageNum);
rid.GetSlotNum(slotNum);

//On va chercher la page correspondante
PF_PageHandle *page = new PF_PageHandle();
this->pf->GetThisPage(pageNum, *page);

//On récupère les données de la page
char *pData;
page->GetData(pData);

//On copie le pageHeader
rm_PageHeader pageHeader;
memcpy(&pageHeader, pData, sizeof(rm_PageHeader));

//On teste si la page est full
if(pageHeader.tab->IsFull())
{
	//On modifie les pointeurs
	pageHeader.nextFreePage = this->fh.nextFreePage;
	this->fh.nextFreePage = pageNum;
	
}


//On change le bit du slot slotNum à 0
pageHeader.tab->SetSlot(slotNum,0);

//On recopie le header dans la page
this->InsertPageHeader(pageNum, pageHeader);

this->pf->MarkDirty(pageNum);
this->pf->UnpinPage(pageNum);
	
	
return 0;
};


RC RM_FileHandle :: UpdateRec(const RM_Record &rec)
{
//On récupère les coordonnées de l'enregistrement
PageNum pageNum;
SlotNum slotNum;
RID *rid = new RID();
rec.GetRid(*rid);
rid->GetPageNum(pageNum);
rid->GetSlotNum(slotNum);

//On va chercher la page correspondante
PF_PageHandle *page = new PF_PageHandle();
this->pf->GetThisPage(pageNum, *page);

//On récupère les données de la page
char *pData;
page->GetData(pData);

//On pointe vers le slot slotNum
pData += sizeof(rm_PageHeader); //On passe le header
pData += slotNum*this->fh.recordSize;

//On stocke les données
char *newData;
rec.GetData(newData);
memcpy(pData, newData, this->fh.recordSize);

this->pf->MarkDirty(pageNum);
this->pf->UnpinPage(pageNum);

return 0;
};

RC RM_FileHandle :: ForcePages (PageNum pageNum)
{
	int res;
	res = this->pf->ForcePages(pageNum);
	
	if(res!=0)
	{
		return res;
	}
	return 0;
};



//Renvoie le numéro de la première page libre.
//Si elle n'existe pas on en alloue une nouvelle et on renvoit son numéro
PageNum RM_FileHandle :: GetNextFreePage()
{

	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;	
	
	
	
	int res;
	
	if(this->fh.nextFreePage != -1)	//Si le file header pointe vers une page libre alors on la retourne
	{
			return this->fh.nextFreePage;
	}
	
	//Sinon on va devoir allouer une nouvelle page
	PF_PageHandle *newPage = new PF_PageHandle();
	
	res = this->pf->AllocatePage(*newPage);
	if(res!=0)
	{
		delete newPage;
		return res;
	}
	//On va rajouter un pageheader dans la nouvelle page
	//On initialise le nouveau page header
	rm_PageHeader newPageHeader;
	newPageHeader.nextFreePage = -1;
	newPageHeader.tab = new Bitmap(this->fh.nbRecordsPerPage);
		
	PageNum numNewPage;
	res = newPage->GetPageNum(numNewPage);
	if(res !=0)
	{
		delete newPage;
		return res;
	}
	
	//On insère le page header dans la page
	res = this->InsertPageHeader(numNewPage, newPageHeader);
	
	if(res != 0)
	{
		delete newPage;
		return res;
	}
	
	return numNewPage;
}

//La fonction récupère les data de la page pageNum et y insère le header pageHeader
RC RM_FileHandle :: InsertPageHeader(const PageNum &pageNum, const rm_PageHeader &pageHeader)
{

int res;	

//On récupère la page
PF_PageHandle *newPage = new PF_PageHandle();
res = this->pf->GetThisPage(pageNum,*newPage);
if(res !=0)
{
	delete newPage;
	return res;
}


char *pData;

res = newPage->GetData(pData); //On récupère les données
if(res !=0)
{
	delete newPage;
	return res;
}

memcpy(pData, &pageHeader, sizeof(rm_PageHeader)); //On copie le pageheader en début de page
res = this->pf->MarkDirty(pageNum);
if(res !=0)
{
	delete newPage;
	return res;
}

res = this->pf->UnpinPage(pageNum);
if(res !=0)
{
	delete newPage;
	return res;
}
	
return 0;
}




RC RM_FileHandle :: SetViableFile(const bool &cond)
{
	this->viableFile = cond;
	return 0;
};

RC RM_FileHandle :: SetPf(const PF_FileHandle *pf)
{
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;
		
	*this->pf = *pf;
	return 0;
};

RC RM_FileHandle :: SetFh(const rm_FileHeader &fh)
{
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;

	this->fh.recordSize = fh.recordSize;
	this->fh.nbRecordsPerPage = fh.nbRecordsPerPage;
	this->fh.nextFreePage = fh.nextFreePage;
	return 0;
};

