#include "tinybase.h"
#include "rm_rid.h"
#include "ix.h"
#include "pf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


IX_IndexHandle :: IX_IndexHandle()
{
	this->pf = NULL;
	
};
IX_IndexHandle :: ~IX_IndexHandle()
{
};

//Pointe vers la cle à la position pos
void IX_IndexHandle :: GetCle(const int pos, char *&pData)
{	
	//On passe le noeud header
	pData += sizeof(ix_NoeudHeader);
	//On passe le premier pointeur pour être à la première cle	
	pData += this->fh.taillePtr;
	//On se déplace pour aller à la clé souhaité
	pData += (pos-1)*(this->fh.tailleCle+this->fh.taillePtr);	
}

//Pointe vers le ptr avant la cle
void IX_IndexHandle :: GetPtrInf(const int pos, char *&pData)
{
	//Nous allons à la clé 
	GetCle(pos, pData);
	//Nous remontons en arrière pour aller sur le ptr inf
	pData -= this->fh.taillePtr;
	
};

//Pointe vers le ptr après la cle
void IX_IndexHandle :: GetPtrSup(const int pos, char *&pData)
{
	//Nous allons à la clé 
	GetCle(pos, pData);
	//Nous passons la clé pour aller au ptr situé à droite
	pData += this->fh.tailleCle;	
	
};

//Ajoute une clé dans un noeud vide
RC IX_IndexHandle :: InsertKeyEmptyNode(const PageNum racine, char *key, char *ptrAvant, char *ptrApres)
{
		int res;
	
	//On récupère la page 
	
	PF_PageHandle *newpage = new PF_PageHandle();
	res = this->pf->GetThisPage(racine,*newpage);
	if(res != 0)
		return res;

	//On récupère les données de la page
	char *pData;
	res = newpage->GetData(pData);
	if(res !=0)
		return res;
	

	//On récupère le noeud header
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	
	//On incrémente le nombre de clé dans le fichier
	nh.nbCleCrt ++;
	
	//On réintègre le nouveau header dans le fichier
	memcpy(pData, &nh, sizeof(ix_NoeudHeader));					
		
	//On passe le noeud header
	pData += sizeof(ix_NoeudHeader);
	
	//On retourne le premier pointeur
	ptrAvant = pData;
	
	//On passe le pointeur
	pData += this->fh.taillePtr;
	
	int val = atoi(key);
	//On insère la clé
	memcpy(pData, &val, this->fh.tailleCle);
	
	//On passe la clé
	pData += this->fh.tailleCle;
	
	//On retourne le dernier pointeur
	ptrApres = pData; 
	

	//On force l'écriture et on unpin
	res = this->pf->ForcePages(racine);
	if(res !=0)
		return res;
			
	res = this->pf->UnpinPage(racine);
	if(res !=0)
		return res;
	
	return 0;
}


//Insère une clé dans un noeud non vide
RC IX_IndexHandle :: InsertKey(PageNum noeud, char *key, char *pDataPtr)
{
	int res;
	
	//On récupère la page 
	PF_PageHandle *page = new PF_PageHandle();
	res = this->pf->GetThisPage(noeud, *page);
	if(res != 0)
		return res;

	//On récupère les données de la page
	char *pData;
	res = page->GetData(pData);
	if(res !=0)
		return res;

	
	//On récupère le header
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	
	
	//On va chercher l'emplacement où insérer notre nouvelle clé
	int i;
	int iValue;	
	//float fValue;
	//char *sValue;
	int iKey = atoi(key);
	//float iKey;
	char tmp[PF_PAGE_SIZE];
	

			//On parcourt l'ensemble des clés du noeud
			for(i = 1; i<=nh.nbCleCrt;i++)
			{
				//On récupère la clé à la position i
				GetCle(i,pData);
				memcpy(&iValue, pData, this->fh.tailleCle);
				
				
				//si nous arrivons à la fin du fichier
				if(iKey>iValue && i == nh.nbCleCrt)
				{
					//On passe la dernière clé
					pData += this->fh.tailleCle;
					//On passe le dernier pointeur
					pData += this->fh.taillePtr;
					//On insère la nouvelle clé
					memcpy(pData, &iKey,this->fh.tailleCle);
					//On passe la clé pour y insérer le pointeur
					pData += this->fh.tailleCle;
					pDataPtr = pData;
					
					//On repointe au début du fichier
					res = page->GetData(pData);
					if(res !=0)
						return res;
					
					//On incrémente le nombre de clé dans le fichier
					nh.nbCleCrt ++;
					
					//On réintègre le nouveau header dans le fichier
					memcpy(pData, &nh, sizeof(ix_NoeudHeader));					
					
					//On force l'écriture et on unpin
					res = this->pf->ForcePages(noeud);
					res = this->pf->UnpinPage(noeud);
					if(res !=0)
						return res;
											
					return 0;
								
				}
				
							
				//si la clé a ajouter est inférieure à la clé courante alors nous avons trouvé le bon emplacement
				else if(iKey<iValue)
				{
					//Nous recopions toute la page de la clé courante jusqu'à la fin
					memcpy(tmp,pData,(nh.nbCleCrt-i+1)*(this->fh.tailleCle+this->fh.taillePtr));
					//On insère notre nouvelle clé
					memcpy(pData,&iKey, this->fh.tailleCle); 
					//On passe la clé et on laisse de la place pour un nouveau pointeur
					pData += this->fh.tailleCle;
					pDataPtr = pData;	
					pData += this->fh.taillePtr;
										
					//On y réintègre la suite de la page
					memcpy(pData, tmp , (nh.nbCleCrt-i+1)*(this->fh.tailleCle+this->fh.taillePtr));
					
					//On repointe au début du fichier
					res = page->GetData(pData);
					if(res !=0)
						return res;
											
					//On incrémente le nombre de clé dans le fichier
					nh.nbCleCrt ++;
					
					//On réintègre le nouveau header dans le fichier
					memcpy(pData, &nh, sizeof(ix_NoeudHeader));
					

					//On force l'écriture et on unpin
					res = this->pf->ForcePages(noeud);
					if(res !=0)
						return res;
											
					res = this->pf->UnpinPage(noeud);
					if(res !=0)
						return res;										
					
					return 0;
				}
				
				//On repointe au début du fichier
				res = page->GetData(pData);
					if(res !=0)
						return res;				

				
			}
	return IX_InsertKey;
	

};


//Insert une clé dans une feuille sans éclatement
RC IX_IndexHandle :: InsertEntryToLeafNodeNoSplit(PageNum noeud, char *key)
{
	int res; 
	
	char ptrAvant[sizeof(int)];
	char ptrApres[sizeof(int)];
	
	//On récupère la page
	PF_PageHandle *page = new PF_PageHandle();
	res = this->pf->GetThisPage(noeud, *page);
	if(res !=0)
		return res;	

	//On récupère les données de la page
	char *pData;
	res = page->GetData(pData);
	if(res !=0)
		return res;	
	
	//On récupère le noeud header
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	
	//On teste si le noeud est vide
	if(nh.nbCleCrt == 0)
	{
		//On insère dans un noeud vide
		InsertKeyEmptyNode(noeud, key, ptrAvant, ptrApres);
		//On force l'écriture et on unpin
		res = this->pf->ForcePages(noeud);
		if(res !=0)
			return res;
								
		res = this->pf->UnpinPage(noeud);
		if(res !=0)
			return res;				
		
		return 0;
	
	}
	
	//Le noeud n'est pas vide
	else
	{
		//On insère dans un noeud non vide
		InsertKey(noeud, key, ptrApres);
			//On force l'écriture et on unpin
			res = this->pf->ForcePages(noeud);
			if(res !=0)
				return res;
									
			res = this->pf->UnpinPage(noeud);
			if(res !=0)
				return res;				
		
		
		
		
		
		return 0;
		
	}
		
		return IX_InsertLeafNoSplit;
}

//extrait la clé du milieu d'un noeud, retourne la clé et le pointeur après la clé
RC IX_IndexHandle :: ExtractKey(const PageNum noeud, char* &key, char *ptrApres)
{

int res;

//On récupère la page
PF_PageHandle *page = new PF_PageHandle();
res = this->pf->GetThisPage(noeud, *page);
if(res !=0)
	return res;	

//On récupère les données de la page
char *pData;
res = page->GetData(pData);
if(res !=0)
	return res;	

//On récupère le noeud header
ix_NoeudHeader nh;
memcpy(&nh, pData, sizeof(ix_NoeudHeader));

//On récupère la clé du milieu
GetCle((nh.nbCleCrt/2)+1,key);
//On récupère le pointeur après la clé
GetPtrSup((nh.nbCleCrt/2)+1,ptrApres);	

//On modifie le nombre de clé dans le noeud
nh.nbCleCrt /= 2;

//On modifie le noeud header dans la page
memcpy(pData, &nh, sizeof(ix_NoeudHeader));

//On force l'écriture et on unpin
res = this->pf->ForcePages(noeud);
if(res !=0)
	return res;
		
res = this->pf->UnpinPage(noeud);
if(res !=0)
	return res;

	
return 0;
}
