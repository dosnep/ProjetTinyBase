#include "rm.h"
#include "pf.h"
#include <cstring>


RM_Manager :: RM_Manager(PF_Manager &pfm)
{
	this->pfm = new PF_Manager(pfm);
	
};

RM_Manager :: ~RM_Manager()
{
};

RC RM_Manager :: CreateFile(const char *fileName, int recordSize)
{
	int res; //Retour des fonctions
	rm_FileHeader FileHeader; //Entête du fichier
	char *NewData;
	
	
	res = this->pfm->CreateFile(fileName); //Creation d'un fichier filename
	
	if(res)
		return res; //Retourne le code de res si une erreur s'est produite
		
	PF_FileHandle *openFile = new PF_FileHandle(); 
	
	res = this->pfm->OpenFile(fileName,*openFile);//Ouverture d'un fichier filename
	
	if(res)
		return res; //Retourne le code de res si une erreur s'est produite
		
	if(recordSize > PF_PAGE_SIZE) //On test si la taille d'un enregistrement est > à la taille d'une page
		return 34;
	
	FileHeader.RecordSize = recordSize;
	FileHeader.TotalPage = 1;
	FileHeader.NextFreePage = (PageNum)-1;
	
	PF_PageHandle *FirstPage = new PF_PageHandle(); 
	res = openFile->AllocatePage(*FirstPage); //On alloue une nouvelle page dans le fichier
	
	if(res)
		return res;
		
	res = FirstPage->GetData(NewData); //On récupère l'adresse de stockage
	
	if(res)
		return res;
		
		memcpy(&NewData[0], &FileHeader, sizeof(rm_FileHeader)); //On copie le header en début de page
		
	//Il faut maintenant marquer la page comme sale et forcer l'écriture sur disque.
	
	PageNum CurrentPage;
	res = FirstPage->GetPageNum(CurrentPage);
	
	if(res)
		return res;
	
	res = openFile->MarkDirty(CurrentPage); //On marque notre page comme sale
	
	if(res)
		return res;
	
	res = openFile->UnpinPage(CurrentPage); //La page est unpinned
	
	if(res)
		return res;
	res = this->pfm->CloseFile(*openFile);
	
	
	return 0;
}

RC RM_Manager :: DestroyFile(const char *fileName)
{
	return this->pfm->DestroyFile(fileName);
}


