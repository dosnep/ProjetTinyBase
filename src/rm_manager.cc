#include "rm.h"
#include "pf.h"
#include <cstring>


RM_Manager :: RM_Manager(PF_Manager &pfm)
{
*this->pfm = pfm;
};

RM_Manager :: ~RM_Manager()
{
};

RC RM_Manager :: CreateFile(const char *fileName, int recordSize)
{
int res;

res = this->pfm->CreateFile(fileName); 

if(res != 0)
{
	return res;
}


PF_FileHandle * file = new PF_FileHandle();
this->pfm->OpenFile(fileName,*file); //On ouvre le fichier fileName
PF_PageHandle *firstPage = new PF_PageHandle();
file->AllocatePage(*firstPage);//On alloue une première page pour le file header
char *pData;
firstPage->GetData(pData); //On récupère les données de la page

//On initialise un nouveau file header
rm_FileHeader newFileHeader;
newFileHeader.nextFreePage = -1; //Pour le moment nous n'avons pas d'autre pages
newFileHeader.recordSize = recordSize;
newFileHeader.nbRecordsPerPage = PF_PAGE_SIZE-sizeof(rm_PageHeader)/recordSize;


	
	
return 0;
}

//Appel la fonction PF_Manager :: DestroyFile(const char *fileName)
RC RM_Manager :: DestroyFile(const char *fileName)
{
int res;

res = this->pfm->DestroyFile(fileName);

if(res !=0)
{
	return res;
}

return 0;
};


RC RM_Manager :: OpenFile(const char *fileName, RM_FileHandle &fileHandle)
{

//On ouvre le fichier
PF_FileHandle *pf = new PF_FileHandle();
this->pfm->OpenFile(fileName, *pf);

//On passe le fichier au RM_FileHandle
fileHandle.SetViableFile(true);
fileHandle.SetPf(pf);

//On récupère le file header
PF_PageHandle *page = new PF_PageHandle();
pf->GetThisPage(0,*page);
char *pData;
page->GetData(pData);

//On passe le file header au RM_FileHandle
rm_FileHeader fh;
memcpy(&fh, pData, sizeof(rm_FileHeader)); 
fileHandle.SetFh(fh);

//On unpin le file header
pf->UnpinPage(0);





return 0;
};


RC RM_Manager :: CloseFile(RM_FileHandle &fileHandle)
{
	
//On charge le file header du fichier
PF_PageHandle *page = new PF_PageHandle();
fileHandle.pf->GetThisPage(0,*page);

//On récupère les données du file header
char *pData;
page->GetData(pData);

//On copie les modifications du file header à l'intérieur du fichier
memcpy(pData, &fileHandle.fh,sizeof(rm_FileHeader));

//On marque les données en sales et on unpin le file header
fileHandle.pf->MarkDirty(0);
fileHandle.pf->UnpinPage(0);

//On ferme le fichier
this->pfm->CloseFile(*fileHandle.pf);

	
return 0;
};

