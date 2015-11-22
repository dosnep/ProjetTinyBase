#include "tinybase.h"
#include "rm_rid.h"
#include "ix.h"
#include "pf.h"
#include <stdio.h>
#include <string.h>


IX_Manager  :: IX_Manager(PF_Manager &pfm) : pfm(pfm)
{
	
};

IX_Manager :: ~IX_Manager()
{
	
};


// Create a new Index
RC IX_Manager :: CreateIndex(const char *fileName, int indexNo, AttrType attrType, int attrLength)
{
	char newFileName[20];
	//on récupère le nom filename
	strcpy(newFileName,fileName);
	
	//on concatène avec le numéro d'index
	sprintf(newFileName,"%s.%d",newFileName,indexNo);
	
	int res;
	
	//nous allons créer un nouveau fichier "filename.indexNo"
	res = this->pfm.CreateFile(newFileName); 

	if(res != 0)
	{
		return res;
	}
	
	//Nous allons créer un fileHeader
	ix_FileHeader fh;
	fh.attrType = attrType;
	fh.tailleCle = attrLength;
	fh.taillePtr = sizeof(int);
	fh.racine = -1; //Pour le moment il n'y a pas de racine
	fh.hauteur = 0; //Pour le moment l'arbre est vide
	fh.nbPointeurMax = 4;
	
	
	//Nous allons ouvrir le nouveau fichier
	PF_FileHandle * file = new PF_FileHandle();
	res = this->pfm.OpenFile(newFileName,*file); //On ouvre le fichier fileName
	if(res != 0)
	{
		return res;
	}

//Nous allons instancier une première page au fichier qui sera le header
	PF_PageHandle *firstPage = new PF_PageHandle();
	res = file->AllocatePage(*firstPage);
		if(res != 0)
		{
			return res;
		}

//On récupère les données de la première page
char *pData;
res = firstPage->GetData(pData);
	if(res != 0)
	{
		return res;
	}



//Nous allons initialiser notre arbre en y insérant un noeud racine;
//Nous allons créer un noeud header
ix_NoeudHeader nh;
nh.nbCleCrt = 0; //Aucune clé pour le moment
nh.mother = -1;	//Aucun noeud parent car c'est la racine
nh.niveau = 1;	//Nous sommes au premier niveau car c'est la racine


//Nous allons instancier une page au fichier qui sera la racine
	PF_PageHandle *racinePage = new PF_PageHandle();
	res = file->AllocatePage(*racinePage);
		if(res != 0)
		{
			return res;
		}

//On récupère les données de la première page
char *pDataRacine;
res = racinePage->GetData(pDataRacine);
	if(res != 0)
	{
		return res;
	}


//On modifie le file header
fh.racine =  1; //Numéro de la page qui représente la racine
fh.hauteur = 1; //Avec l'ajout de la racine, la hauteur est maintenant de 1

//On y insère notre file header
memcpy(pData, &fh, sizeof(ix_FileHeader));

//On y insère notre noeud header
memcpy(pDataRacine, &nh, sizeof(ix_NoeudHeader));


//On marque comme sale et on unpin les 2 pages
res = file->ForcePages(0);
	if(res !=0)
	{return res;}
res = file->UnpinPage(0);
	if(res !=0)
	{return res;}
res = file->ForcePages(1);
	if(res !=0)
	{return res;}
res = file->UnpinPage(1);
	if(res !=0)
	{return res;}	
	
	
	return 0;
};

// Destroy and Index
RC IX_Manager :: DestroyIndex(const char *fileName, int indexNo)
{

char newFileName[20];
//on récupère le nom filename
strcpy(newFileName,fileName);

//on concatène avec le numéro d'index
sprintf(newFileName,"%s.%d",newFileName,indexNo);	

	
int res;
//On supprime le fichier
res = this->pfm.DestroyFile(newFileName);

if(res !=0)
{
	return res;
}

return 0;
}

// Open an Index
RC IX_Manager :: OpenIndex(const char *fileName, int indexNo, IX_IndexHandle &indexHandle)
{
int res;

char newFileName[20];
//on récupère le nom filename
strcpy(newFileName,fileName);

//on concatène avec le numéro d'index
sprintf(newFileName,"%s.%d",newFileName,indexNo);	


//On ouvre le fichier
PF_FileHandle *pf = new PF_FileHandle();
res = this->pfm.OpenFile(newFileName, *pf);
	if(res !=0)
	{
	delete pf;
	return res;
	}

//On passe le fichier au IX_indexHandle
indexHandle.viableFile = true;
indexHandle.pf = new PF_FileHandle(*pf);

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


//On passe le file header au IX_indexHandle
ix_FileHeader fh;
memcpy(&fh, pData, sizeof(ix_FileHeader)); 
indexHandle.fh = fh;


//On unpin le file header
res = pf->UnpinPage(0);
	if(res !=0)
	{return res;}
	

return 0;

}

// Close an Index
RC IX_Manager :: CloseIndex(IX_IndexHandle &indexHandle)
{
int res;
	
//On charge le file header du fichier
PF_PageHandle *page = new PF_PageHandle();
res = indexHandle.pf->GetThisPage(0,*page);
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
memcpy(pData, &indexHandle.fh,sizeof(ix_FileHeader));

//On marque les données en sales et on unpin le file header
res = indexHandle.pf->MarkDirty(0);
	if(res !=0)
	{return res;}
res = indexHandle.pf->UnpinPage(0);
	if(res !=0)
	{return res;}
//On ferme le fichier
res = this->pfm.CloseFile(*indexHandle.pf);
	if(res !=0)
	{return res;}
	
return 0;
}
