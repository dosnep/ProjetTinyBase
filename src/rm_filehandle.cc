#include "rm.h"
#include <cstring>

RM_FileHandle :: RM_FileHandle()
{
	this->viableFile = false;
	this->pf = NULL;
	this->fh.recordSize = -1;
	this->fh.nbRecordsPerPage = -1;
	this->fh.nextFreePage = -1;
};

RM_FileHandle :: RM_FileHandle(const PF_FileHandle &pf, const rm_FileHeader &fh)
{
	this->viableFile = true;
	this->pf = new PF_FileHandle(pf);
	this->fh.recordSize = fh.recordSize;
	this->fh.nbRecordsPerPage = fh.nbRecordsPerPage;
	this->fh.nextFreePage = fh.nextFreePage;
};


RM_FileHandle :: ~RM_FileHandle()
{
	delete this->pf;
	this->pf = NULL;
};


//Récupère l'enregistrement à partir de rid et le stocke dans rec
RC RM_FileHandle :: GetRec(const RID &rid, RM_Record &rec) const
{
	
//On teste si le fichier a bien été ouvert
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;


rec.rid = new RID(rid.pageNum,rid.slotNum);
rec.viableRecord = true;
rec.recordSize = this->fh.recordSize;
	
	//ON commence par récupérer la page courante
	PF_PageHandle *page= new PF_PageHandle();
	int res;
	res = this->pf->GetThisPage(rec.rid->pageNum,*page);
		if(res !=0)
		{
			delete page;
			return res;
		}
		
	//On récupère les données de la page
	char *pData;
	page->GetData(pData);
	pData += sizeof(rm_PageHeader)+rec.rid->slotNum*this->fh.recordSize;
	
	//On copie les données dans le record
	rec.pData = new char[this->fh.recordSize];
	memcpy(rec.pData, pData, this->fh.recordSize);

return 0;	
}

//Insère un enregistrement, retourne l'emplacement (rid) où il est stocké
RC RM_FileHandle :: InsertRec  (const char *pData, RID &rid)
{
	
//On vérifie que le fichier est bien ouvert
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;


int res;	
PageNum pageNum;
res = GetNextFreePage(pageNum);	//On récupère la premiere page libre	
if(res != 0)
{	
	return res;
}

		
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
		delete tmp;
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
		return res;
	}

tmp += sizeof(rm_PageHeader); //On passe le header
tmp += freeSlot*this->fh.recordSize;//On pointe sur le premier slot libre
memcpy(tmp, pData, this->fh.recordSize);

//On va maintenant mettre le bit du slot utilisé à 1
res = newPageHeader.tab->SetSlot(freeSlot,1);

	if(res!=0)
	{
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
		return res;
	}

//On force l'écriture sur disque
res = this->pf->ForcePages(pageNum);
	if(res != 0)
	{
	return res;
	}
//On marque la page comme unpin
res = this->pf->UnpinPage(pageNum);
	if(res != 0)
	{
	return res;
	}

//On initialise le rid
rid.viableRid = true;
rid.pageNum = pageNum;
rid.slotNum = freeSlot;

return 0;
}; 

//La fonction va tester si la page est pleine auquel cas on va l'ajouter aux pages libres
//le bit du slot concerné sera mît à 0
RC RM_FileHandle :: DeleteRec(const RID &rid)
{

//On récupère le numéro de la page et le numéro du slot
int res;
PageNum pageNum;
SlotNum slotNum;
res = rid.GetPageNum(pageNum);
	if(res != 0)
	{
	return res;
	}
	
res = rid.GetSlotNum(slotNum);
	if(res != 0)
	{
	return res;
	}

//On va chercher la page correspondante
PF_PageHandle *page = new PF_PageHandle();
res = this->pf->GetThisPage(pageNum, *page);
	if(res != 0)
	{
	delete page;
	return res;
	}

//On récupère les données de la page
char *pData;
res = page->GetData(pData);
	if(res != 0)
	{
	delete pData;
	return res;
	}

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
res = pageHeader.tab->SetSlot(slotNum,0);
	if(res != 0)
	{
	return res;
	}

//On recopie le header dans la page
res = this->InsertPageHeader(pageNum, pageHeader);
	if(res != 0)
	{
	return res;
	}

//On force l'enregistrement
res = this->pf->ForcePages(pageNum);
	if(res != 0)
	{
	return res;
	}

//On unpin la page
res = this->pf->UnpinPage(pageNum);
	if(res != 0)
	{
	return res;
	}	
	
return 0;
};


RC RM_FileHandle :: UpdateRec(const RM_Record &rec)
{
int res;
//On récupère les coordonnées de l'enregistrement
PageNum pageNum;
SlotNum slotNum;
RID *rid = new RID();
res = rec.GetRid(*rid);
	if(res != 0)
	{
	delete rid;
	return res;	
	}
res = rid->GetPageNum(pageNum);
	if(res != 0)
	{
	return res;	
	}
res = rid->GetSlotNum(slotNum);
	if(res != 0)
	{
	return res;	
	}

//On va chercher la page correspondante
PF_PageHandle *page = new PF_PageHandle();
res = this->pf->GetThisPage(pageNum, *page);
	if(res != 0)
	{
	delete page;
	return res;	
	}

//On récupère les données de la page
char *pData;
res = page->GetData(pData);
	if(res != 0)
	{
	delete pData;
	return res;	
	}
//On pointe vers le slot slotNum
pData += sizeof(rm_PageHeader); //On passe le header
pData += slotNum*this->fh.recordSize;

//On stocke les données
char *newData;
res = rec.GetData(newData);
if(res != 0)
{
delete newData;
return res;	
}

memcpy(pData, newData, this->fh.recordSize);

res = this->pf->ForcePages(pageNum);
	if(res != 0)
	{
	return res;	
	}
res = this->pf->UnpinPage(pageNum);
	if(res != 0)
	{
	return res;	
	}

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
RC RM_FileHandle :: GetNextFreePage(PageNum &pageNum)
{

//On teste si le fichier a bien été ouvert
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;	
	
int res;

//Si le file header pointe vers une page libre alors on la retourne	
	if(this->fh.nextFreePage != -1)	
	{
			pageNum = this->fh.nextFreePage;
			return 0;
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
		return res;
	}
	
//On insère le page header dans la page
res = this->InsertPageHeader(numNewPage, newPageHeader);
	if(res != 0)
	{
		return res;
	}
	
	
//On force l'écriture de la nouvelle page
res = this->pf->ForcePages(numNewPage);
	if(res != 0)
	{
		return res;
	}
	
//On unpin la page
res = this->pf->UnpinPage(numNewPage);
	
	if(res != 0)
	{
		return res;
	}

	pageNum = numNewPage;
	
//On oublie pas de modifier le pointeur du file header
this->fh.nextFreePage = numNewPage;

	return 0;
}

//La fonction récupère les data de la page pageNum et y insère le header pageHeader
RC RM_FileHandle :: InsertPageHeader(const PageNum &pageNum, const rm_PageHeader &pageHeader)
{

int res;	


PF_PageHandle *newPage = new PF_PageHandle();//On récupère la page
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
	delete pData;
	return res;
}

memcpy(pData, &pageHeader, sizeof(rm_PageHeader)); //On copie le pageheader en début de page

res = this->pf->ForcePages(pageNum); //On marque les données en dirty
if(res !=0)
{
	return res;
}

res = this->pf->UnpinPage(pageNum);	//On marque la page page comme unpin
if(res !=0)
{
	return res;
}
	
return 0;
}

