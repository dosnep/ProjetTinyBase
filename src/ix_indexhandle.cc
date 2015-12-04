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
RC IX_IndexHandle :: InsertKeyEmptyNode(const PageNum racine, char *key, char *&ptrAvant, char *&ptrApres)
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
	
	int ival;
	memcpy(&ival, key, sizeof(this->fh.tailleCle));
	//On insère la clé
	memcpy(pData, &ival, this->fh.tailleCle);
	
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
RC IX_IndexHandle :: InsertKey(PageNum noeud, char *key, char *&pDataPtr)
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
	int iKey;
	memcpy(&iKey, key, this->fh.tailleCle);
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
					if(res !=0)
						return res;
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
	
	char *ptrAvant;
	char *ptrApres;
	
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
		res = InsertKeyEmptyNode(noeud, key, ptrAvant, ptrApres);
		if(res !=0)
			return res;
					
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
		res = InsertKey(noeud, key, ptrApres);
		if(res !=0)
			return res;
						
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

//extrait la clé du milieu d'un noeud,modifie le fils gauche, modifie le fils droit
RC IX_IndexHandle :: ExtractKey(const PageNum noeud, char* &key, const PageNum splitNoeud)
{

int res;
char *tmp = new char[PF_PAGE_SIZE];

//On récupère la première page
PF_PageHandle *page = new PF_PageHandle();
res = this->pf->GetThisPage(noeud, *page);
if(res !=0)
	return res;	

//On récupère les données de la première page
char *pData;
res = page->GetData(pData);
if(res !=0)
	return res;	

//On récupère le noeud header de la première page
ix_NoeudHeader nh;
memcpy(&nh, pData, sizeof(ix_NoeudHeader));

//On récupère la clé du milieu
GetCle((nh.nbCleCrt/2)+1,key);

int nbCleCrt;
nbCleCrt = nh.nbCleCrt;

//On modifie le nombre de clé dans la première page
nh.nbCleCrt = nh.nbCleCrt/2;

//On modifie le noeud header dans la première page
memcpy(pData, &nh, sizeof(ix_NoeudHeader));

//On se place au pointeur après la clé
GetPtrSup((nbCleCrt/2)+1, pData);

//On récupère la nouvelle page
PF_PageHandle *splitPage = new PF_PageHandle();
res = this->pf->GetThisPage(splitNoeud, *splitPage);
if(res !=0)
	return res;	
	
//On récupère les données de la nouvelle page
char *pData2;
res = splitPage->GetData(pData2);
if(res !=0)
	return res;	

//On récupère le noeud header de la nouvelle page
ix_NoeudHeader nhSplit;
memcpy(&nhSplit, pData2, sizeof(ix_NoeudHeader));

//On va modifier le nbCleCrt dans la nouvelle page
if(nbCleCrt%2 == 1)
	nbCleCrt /= 2;

else	
	nbCleCrt = (nbCleCrt/2)-1;

//On recopie le nouveau header dans la nouvelle page
nhSplit.nbCleCrt = nbCleCrt;
	
memcpy(pData2, &nhSplit, sizeof(ix_NoeudHeader));


//on recopie la seconde partie de la première page dans la nouvelle page
memcpy(tmp, pData, this->fh.taillePtr+nbCleCrt*(this->fh.taillePtr+this->fh.tailleCle));
pData2 += sizeof(ix_NoeudHeader);
memcpy(pData2, tmp, this->fh.taillePtr+nbCleCrt*(this->fh.taillePtr+this->fh.tailleCle));

	

//On force l'écriture et on unpin
res = this->pf->ForcePages(noeud);
if(res !=0)
	return res;
		
res = this->pf->UnpinPage(noeud);
if(res !=0)
	return res;

res = this->pf->ForcePages(splitNoeud);
if(res !=0)
	return res;
		
res = this->pf->UnpinPage(splitNoeud);
if(res !=0)
	return res;

	
return 0;
}


//Insert une clé dans une feuille avec éclatement
RC IX_IndexHandle :: InsertEntryToLeafNodeSplit(PageNum noeud, char *key)
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
	if(res != 0)
		return res;
	
	//On récupère le noeud header de la page
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	
	//On instancie une nouvelle page qui sera le fils droit
	PF_PageHandle *filsDroit = new PF_PageHandle();
	res = this->pf->AllocatePage(*filsDroit);
		if(res !=0)
			return res;
				
	//On récupère le numéro de cette page
	PageNum filsDroitNum;
	res = filsDroit->GetPageNum(filsDroitNum);
		if(res !=0)
			return res;

	//On récupère les données du noeud fils droit
	char *pDataFilsDroit;
	res = filsDroit->GetData(pDataFilsDroit);
		if(res !=0)
			return res;	
	//On instancie un noeud header dans le fils droit
	ix_NoeudHeader nhFilsDroit;
	nhFilsDroit.nbCleCrt = 0;
	nhFilsDroit.mother = nh.mother;
	//On recopie le noeud header dans la page
	memcpy(pDataFilsDroit, &nhFilsDroit, sizeof(ix_NoeudHeader));

	//On fait une extraction de la clé
	char *val = pData;
	this->ExtractKey(noeud,val,filsDroitNum);

	//On teste dans quel fils insérer notre nouvelle clé
	int ival;
	int ikey;
	memcpy(&ikey, key, sizeof(int));
	memcpy(&ival, val, sizeof(int));

	char *ptrApres;
	char *ptrAvant;
	
	//Si la clé à insérer est supérieur à la clé du milieu on fait l'insertion dans le fils droit
	if(ikey >ival)
		InsertKey(filsDroitNum,key,ptrApres);
	//Sinon on fait l'insertion dans le fils gauche
	else
		InsertKey(noeud,key,ptrApres);

	//On teste si notre feuille est la racine
	if(this->fh.hauteur == 1)
	{	
		//Si oui on instancie une nouvelle racine
		PF_PageHandle *newRacine = new PF_PageHandle();
		res = this->pf->AllocatePage(*newRacine);
		if(res !=0)
			return res;		
		
		//On récupère le numéro de la page de la nouvelle racine
		PageNum newRacineNum;
		res = newRacine->GetPageNum(newRacineNum);
		if(res !=0)
			return res;
					
		//On insère un nouveau header dans ce noeud
		ix_NoeudHeader nhRacine;
		nhRacine.nbCleCrt = 0;
		nhRacine.mother = -1;
		char *pDataRacine;
		res = newRacine->GetData(pDataRacine);
		if(res !=0)
			return res;
					
		memcpy(pDataRacine,&nhRacine,sizeof(ix_NoeudHeader));
		//On y insère notre clé 
		char nouvelInsertion[10];
		memcpy(nouvelInsertion, &ival, sizeof(int));
		this->InsertKeyEmptyNode(newRacineNum, nouvelInsertion, ptrAvant, ptrApres);
		
		//On termine le chaînage
		memcpy(ptrAvant, &noeud, this->fh.taillePtr);
		memcpy(ptrApres, &filsDroitNum, this->fh.taillePtr);
		

		//On modifie le parent des fils gauche et droit
			//On doit recharger le noeuheader car il a été modifié
		memcpy(&nh, pData, sizeof(ix_NoeudHeader));
		nh.mother = newRacineNum;
			//On doit recharger le noeudheader car il a été modifié 
		memcpy(&nhFilsDroit, pDataFilsDroit, sizeof(ix_NoeudHeader));
		nhFilsDroit.mother = newRacineNum;
		//On écrit les headers dans les pages
		memcpy(pData, &nh, sizeof(ix_NoeudHeader));
		memcpy(pDataFilsDroit, &nhFilsDroit, sizeof(ix_NoeudHeader));
		
		//On change le numéro de la racine dans le file header
		this->fh.racine = newRacineNum;		
		
		//On termine en incrémentant la hauteur de l'arbre
		this->fh.hauteur++;
	
		//On force l'écriture et on uping les pages
		res = this->pf->ForcePages(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->ForcePages(filsDroitNum);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(filsDroitNum);
		if(res !=0)
		{return res;}	
		res = this->pf->ForcePages(newRacineNum);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(newRacineNum);
		if(res !=0)
		{return res;}		

	}
	
	else
	{
		//On ajoute une clé dans un noeud interne
		this->InsertEntryToIntNode(nh.mother,val,filsDroitNum);
		
		//On force l'écriture et on uping les pages
		res = this->pf->ForcePages(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->ForcePages(filsDroitNum);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(filsDroitNum);
		if(res !=0)
		{return res;}			
		
		
		 
	}
	
return 0;	
}


//Insert une clé dans un parent sans éclatement
RC IX_IndexHandle :: InsertEntryToIntNodeNoSplit(PageNum noeud, char *key, PageNum splitNoeud)
{
	char *ptr;
	this->InsertKey(noeud, key, ptr);
	memcpy(ptr, &splitNoeud,this->fh.taillePtr);

return 0;	
}

//Insert une clé dans un parent avec éclatement
RC IX_IndexHandle :: InsertEntryToIntNodeSplit(PageNum noeud, char *key,PageNum noeudSplit)
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
	if(res != 0)
		return res;
	
	//On récupère le noeud header de la page
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	
	//On instancie une nouvelle page qui sera le fils droit
	PF_PageHandle *filsDroit = new PF_PageHandle();
	res = this->pf->AllocatePage(*filsDroit);
		if(res !=0)
			return res;
				
	//On récupère le numéro de cette page
	PageNum filsDroitNum;
	res = filsDroit->GetPageNum(filsDroitNum);
		if(res !=0)
			return res;

	//On récupère les données du noeud fils droit
	char *pDataFilsDroit;
	res = filsDroit->GetData(pDataFilsDroit);
		if(res !=0)
			return res;	
	//On instancie un noeud header dans le fils droit
	ix_NoeudHeader nhFilsDroit;
	nhFilsDroit.nbCleCrt = 0;
	nhFilsDroit.mother = nh.mother;
	//On recopie le noeud header dans la page
	memcpy(pDataFilsDroit, &nhFilsDroit, sizeof(ix_NoeudHeader));

	//On fait une extraction de la clé
	char *val = pData;
	this->ExtractKey(noeud,val,filsDroitNum);

	//On teste dans quel fils insérer notre nouvelle clé
	int ival;
	int ikey;
	memcpy(&ikey, key, sizeof(int));
	memcpy(&ival, val, sizeof(int));

	char *ptrApres;
	char *ptrAvant;
	
	//Si la clé à insérer est supérieur à la clé du milieu on fait l'insertion dans le fils droit
	if(ikey >ival)
		InsertKey(filsDroitNum,key,ptrApres);
	//Sinon on fait l'insertion dans le fils gauche
	else
		InsertKey(noeud,key,ptrApres);
		
	//On met le pointeur à la valeur de son fils	
	memcpy(ptrApres, &noeudSplit, this->fh.taillePtr);

	//On modifie le parent de tout les fils de la partie droite
	this->ModifParent(filsDroitNum);
	
	//On teste si notre parent est la racine
	if(nh.mother == -1)
	{	
		//Si oui on instancie une nouvelle racine
		PF_PageHandle *newRacine = new PF_PageHandle();
		res = this->pf->AllocatePage(*newRacine);
		if(res !=0)
			return res;		
		
		//On récupère le numéro de la page de la nouvelle racine
		PageNum newRacineNum;
		res = newRacine->GetPageNum(newRacineNum);
		if(res !=0)
			return res;
					
		//On insère un nouveau header dans ce noeud
		ix_NoeudHeader nhRacine;
		nhRacine.nbCleCrt = 0;
		nhRacine.mother = -1;
		char *pDataRacine;
		res = newRacine->GetData(pDataRacine);
		if(res !=0)
			return res;
					
		memcpy(pDataRacine,&nhRacine,sizeof(ix_NoeudHeader));
		//On y insère notre clé 
		char nouvelInsertion[10];
		memcpy(nouvelInsertion, &ival, sizeof(int));
		this->InsertKeyEmptyNode(newRacineNum, nouvelInsertion, ptrAvant, ptrApres);
		
		//On termine le chaînage
		memcpy(ptrAvant, &noeud, this->fh.taillePtr);
		memcpy(ptrApres, &filsDroitNum, this->fh.taillePtr);
		
		//On modifie le parent des fils gauche et droit
			//On doit recharger le noeuheader car il a été modifié
		memcpy(&nh, pData, sizeof(ix_NoeudHeader));
		nh.mother = newRacineNum;
			//On doit recharger le noeudheader car il a été modifié 
		memcpy(&nhFilsDroit, pDataFilsDroit, sizeof(ix_NoeudHeader));
		nhFilsDroit.mother = newRacineNum;
		//On écrit les headers dans les pages
		memcpy(pData, &nh, sizeof(ix_NoeudHeader));
		memcpy(pDataFilsDroit, &nhFilsDroit, sizeof(ix_NoeudHeader));
		
		//On change le numéro de la racine dans le file header
		this->fh.racine = newRacineNum;
		
		//On termine en incrémentant la hauteur de l'arbre
		this->fh.hauteur++;
	
		//On force l'écriture et on uping les pages
		res = this->pf->ForcePages(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->ForcePages(filsDroitNum);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(filsDroitNum);
		if(res !=0)
		{return res;}	
		res = this->pf->ForcePages(newRacineNum);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(newRacineNum);
		if(res !=0)
		{return res;}		

	}
	
	else
	{

		//On ajoute une clé dans un noeud interne
		this->InsertEntryToIntNode(nh.mother,val,filsDroitNum); 
		//On force l'écriture et on uping les pages
		res = this->pf->ForcePages(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(noeud);
		if(res !=0)
		{return res;}
		res = this->pf->ForcePages(filsDroitNum);
		if(res !=0)
		{return res;}
		res = this->pf->UnpinPage(filsDroitNum);
		if(res !=0)
		{return res;}			

	}	
	
	
	
return 0;
}

//Insert une clé dans une feuille
RC IX_IndexHandle :: InsertEntryToLeaf(PageNum noeud, char *key)
{
	int res;
	//On récupère la page 
	PF_PageHandle *page = new PF_PageHandle();
	res = this->pf->GetThisPage(noeud, *page);
	if(res !=0)
		return res;
	
	
	
	
	//On charge les données de la page
	char *pData;
    res = page->GetData(pData);
	if(res !=0)
		return res;
 
    
	//On récupère le noeud header
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));


	//On teste si la feuille est pleine
	if(nh.nbCleCrt == this->fh.nbPointeurMax-1)
		this->InsertEntryToLeafNodeSplit(noeud, key);
	//Sinon la feuille n'est pas pleine
	else
		this->InsertEntryToLeafNodeNoSplit(noeud, key);
	
	res = this->pf->ForcePages(noeud);
	if(res !=0)
		return res;
	
	res = this->pf->UnpinPage(noeud);
	if(res !=0)
		return res;
	
	return 0;
	
}


//Insert une clé dans un noeud interne
RC IX_IndexHandle :: InsertEntryToIntNode(PageNum noeud, char *key, PageNum splitNoeud)
{
	int res;
	
	//On récupère la page 
	PF_PageHandle *page = new PF_PageHandle();
	res = this->pf->GetThisPage(noeud, *page);
	if(res !=0)
		return res;
	
	//On charge les données de la page
	char *pData;
    res = page->GetData(pData);
	if(res !=0)
		return res;
    
	//On récupère le noeud header
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	
	//On teste si la feuille est pleine
	if(nh.nbCleCrt == this->fh.nbPointeurMax-1)
		this->InsertEntryToIntNodeSplit(noeud, key, splitNoeud);
	//Sinon la feuille n'est pas pleine
	else
		this->InsertEntryToIntNodeNoSplit(noeud, key, splitNoeud);
	
	
	res = this->pf->ForcePages(noeud);
	if(res !=0)
		return res;
	res = this->pf->UnpinPage(noeud);	
	if(res !=0)
		return res;	
	
	return 0;	
}

//Pour chaque fils de noeud, son parent sera noeud
RC IX_IndexHandle :: ModifParent(PageNum noeud)
{
	int res; 
	//On charge le noeud
	PF_PageHandle *page = new PF_PageHandle();
	PF_PageHandle *pageFils = new PF_PageHandle();
	res = this->pf->GetThisPage(noeud, *page);
	if(res !=0)
		return res;
	
	//On charge les données de la page
	char *pData;
	res = page->GetData(pData);
	if(res !=0)
		return res;
	
	//On récupère le noeud header 
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	int filsNum;
	char *pDataFils;
	ix_NoeudHeader nhFils;
	//On va parcourir chacun des fils du noeud
	int i;
	for (i = 1; i<=nh.nbCleCrt; i++)
	{
		//Si nous sommes au premier élément alors il faut aussi récupérer le premier pointeur
		if(i == 1)
		{
		
		//On récupère le pointeur d'après la clé
		this->GetPtrInf(i, pData);
		//On récupère le numéro de la page du fils
		memcpy(&filsNum, pData, this->fh.taillePtr);
		//On charge la page fils
		res = this->pf->GetThisPage(filsNum, *pageFils);
			if(res !=0)
		return res;

		//On charge les données
		res = pageFils->GetData(pDataFils);
			if(res !=0)
		return res;

		//On récupère le noeud header
		memcpy(&nhFils, pDataFils, sizeof(ix_NoeudHeader));
		//On modifie le parent du noeud header
		nhFils.mother = noeud;
		//On écrit le noeud header dans la page
		memcpy(pDataFils, &nhFils, sizeof(ix_NoeudHeader));
		//On force l'écriture et unpin la page
		res = this->pf->ForcePages(filsNum);
			if(res !=0)
		return res;

		res = this->pf->UnpinPage(filsNum);
			if(res !=0)
		return res;

		
		
		}
		
		//On remet la pointeur au début du fichier
		res = page->GetData(pData);
	if(res !=0)
		return res;

		
		//On récupère le pointeur d'après la clé
		this->GetPtrSup(i, pData);
		//On récupère le numéro de la page du fils
		memcpy(&filsNum, pData, this->fh.taillePtr);
		//On charge la page du fils
		res = this->pf->GetThisPage(filsNum, *pageFils);
	if(res !=0)
		return res;
		
		//On charge les données
		res = pageFils->GetData(pDataFils);
			if(res !=0)
		return res;

		//On récupère le noeud header
		memcpy(&nhFils, pDataFils, sizeof(ix_NoeudHeader));
		//On modifie le parent du noeud header
		nhFils.mother = noeud;
		
		//On écrit le noeud header dans la page
		memcpy(pDataFils, &nhFils, sizeof(ix_NoeudHeader));
		
		//On force l'écriture et unpin la page
		res = this->pf->ForcePages(filsNum);
			if(res !=0)
		return res;

		res = this->pf->UnpinPage(filsNum);
			if(res !=0)
		return res;		
		
		
		
	}

	
			//On force l'écriture et unpin la page
		res = this->pf->ForcePages(noeud);
			if(res !=0)
		return res;

		res = this->pf->UnpinPage(noeud);
	if(res !=0)
		return res;
	
	
	return 0;
}


//Insert une clé dans un noeud
RC IX_IndexHandle :: InsertEntryToNode(PageNum noeud, char *key, int hauteur)
{
	//On teste à partir de la hauteur si noeud est une feuille
	//Si oui, on fait une insertion dans une feuille


	if(hauteur == this->fh.hauteur)
		this->InsertEntryToLeaf(noeud, key);
	
	//Sinon, On fait une insertion dans un noeud interne
	else
		this->InsertEntryToInt1Node(noeud, key, hauteur);
	
	return 0;
}

//Va chercher le sous arbre dans lequel nous allons insérer notre clé
RC IX_IndexHandle :: InsertEntryToInt1Node(PageNum noeud, char *key, int hauteur)
{
	int res;
	//On charge la page
	PF_PageHandle *page = new PF_PageHandle();
	res = this->pf->GetThisPage(noeud, *page);
	if(res !=0)
		return res;
	
	//On récupère les données de la page
	char *pData;
	res = page->GetData(pData);
	if(res !=0)
		return res;
	
	//On récupère le noeud de la page
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	
	//On va parcourir l'ensemble des fils du noeud
	int i;
	int iVal;
	int iKey;
	PageNum ptr;
	memcpy(&iKey, key, this->fh.tailleCle);
	for(i = 1; i<=nh.nbCleCrt;i++)
	{
		//On pointe vers la clé à la position i
		this->GetCle(i, pData);
		
		//On récupère la valeur de la clé
		memcpy(&iVal, pData, this->fh.tailleCle);
		
		//On teste où insérer notre clé
		if(iVal > iKey)
		{
			//On répointe au début du fichier
			res = page->GetData(pData);
				if(res !=0)
		return res;

			//On pointe vers le pointeur avant ikey
			this->GetPtrInf(i, pData);
			
			//On récupère le numéro de la page
			memcpy(&ptr, pData, this->fh.taillePtr);
			
	
			//On appel notre fonction InsertNode
			this->InsertEntryToNode(ptr, key, hauteur+1);
			
			//On unpin la page
			res = this->pf->UnpinPage(noeud);
				if(res !=0)
		return res;

			return 0;
	
			
		}
		
		//Si on arrive à la fin du fichier
		else if(iVal < iKey && i == nh.nbCleCrt)
		{
			
			//On répointe au début du fichier
			res = page->GetData(pData);
				if(res !=0)
		return res;

			//On pointe vers le pointeur après ikey
			this->GetPtrSup(i, pData);
			
			//On récupère le numéro de la page
			memcpy(&ptr, pData, this->fh.taillePtr);
			

			//On appel notre fonction InsertNode
			this->InsertEntryToNode(ptr, key, hauteur+1);
			
			//On unpin la page
			res = this->pf->UnpinPage(noeud);
				if(res !=0)
		return res;

			return 0;
		
		}
		
		//On repointe au début du fichie
		res = page->GetData(pData);
		if(res !=0)
		return res;

	}
	
	//On unpin la page
	res = this->pf->UnpinPage(noeud);
	if(res !=0)
		return res;
	
	return 0;
}	

RC IX_IndexHandle :: InsertEntry(void *pData, const RID &rid)
{

		this->InsertEntryToNode(this->fh.racine, (char*)pData, 1);

	return 0;
}





