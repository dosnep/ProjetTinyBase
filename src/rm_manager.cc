#include "rm.h"
#include "pf.h"
#include <cstring>


RM_Manager :: RM_Manager(PF_Manager &pfm) : pfm(pfm)
{
};

RM_Manager :: ~RM_Manager()
{
};

RC RM_Manager :: CreateFile(const char *fileName, int recordSize)
{
int res;

res = this->pfm.CreateFile(fileName); 

if(res != 0)
{
	return res;
}


PF_FileHandle * file = new PF_FileHandle();
res = this->pfm.OpenFile(fileName,*file); //On ouvre le fichier fileName
	if(res != 0)
	{
		return res;
	}

//On alloue une première page pour le file header
PF_PageHandle *firstPage = new PF_PageHandle();
res = file->AllocatePage(*firstPage);
	if(res != 0)
	{
		return res;
	}

//On récupère les données de la page
	char *pData;
	res = firstPage->GetData(pData); 
		if(res != 0)
		{
			return res;
		}


//On initialise un nouveau file header
rm_FileHeader newFileHeader;
newFileHeader.nextFreePage = -1; //Pour le moment nous n'avons pas d'autre pages
newFileHeader.recordSize = recordSize;
newFileHeader.nbRecordsPerPage = 3;	// 3 est pour tester plusieurs enregistrements par page
								//(PF_PAGE_SIZE-sizeof(rm_FileHeader))/(recordSize-sizeof(rm_PageHeader));

memcpy(pData, &newFileHeader, sizeof(rm_FileHeader));


file->ForcePages(0);
file->UnpinPage(0);
	
return 0;
}

//Appel la fonction PF_Manager :: DestroyFile(const char *fileName)
RC RM_Manager :: DestroyFile(const char *fileName)
{
int res;

res = this->pfm.DestroyFile(fileName);

if(res !=0)
{
	return res;
}

return 0;
};


RC RM_Manager :: OpenFile(const char *fileName, RM_FileHandle &fileHandle)
{
int res;

//On ouvre le fichier
PF_FileHandle *pf = new PF_FileHandle();
res = this->pfm.OpenFile(fileName, *pf);
	if(res !=0)
	{
	delete pf;
	return res;
	}


//On passe le fichier au RM_FileHandle
fileHandle.viableFile = true;
fileHandle.pf = new PF_FileHandle(*pf);

//On récupère le file header
PF_PageHandle *page = new PF_PageHandle();
res = pf->GetThisPage(0,*page);
	if(res !=0)
	{
	delete page;
	return res;
	}

char *pData;
res = page->GetData(pData);
	if(res !=0)
	{
	delete pData;
	return res;
	}


//On passe le file header au RM_FileHandle
rm_FileHeader fh;
memcpy(&fh, pData, sizeof(rm_FileHeader)); 
fileHandle.fh = fh;

//On unpin le file header
res = pf->UnpinPage(0);
	if(res !=0)
	{return res;}

return 0;
};


RC RM_Manager :: CloseFile(RM_FileHandle &fileHandle)
{
int res;
	
//On charge le file header du fichier
PF_PageHandle *page = new PF_PageHandle();
res = fileHandle.pf->GetThisPage(0,*page);
	if(res !=0)
	{
	delete page;
	return res;
	}

//On récupère les données du file header
char *pData;
res = page->GetData(pData);
	if(res !=0)
	{
	delete pData;
	return res;
	}

//On copie les modifications du file header à l'intérieur du fichier
memcpy(pData, &fileHandle.fh,sizeof(rm_FileHeader));

//On marque les données en sales et on unpin le file header
res = fileHandle.pf->MarkDirty(0);
	if(res !=0)
	{return res;}
res = fileHandle.pf->UnpinPage(0);
	if(res !=0)
	{return res;}
//On ferme le fichier
res = this->pfm.CloseFile(*fileHandle.pf);
	if(res !=0)
	{return res;}
	
return 0;
};

