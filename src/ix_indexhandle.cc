#include "tinybase.h"
#include "rm_rid.h"
#include "ix.h"
#include "pf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


IX_IndexHandle :: IX_IndexHandle()
{
	
};
IX_IndexHandle :: ~IX_IndexHandle()
{
};

//Pointe vers la cle à la position pos
void IX_IndexHandle :: GetCle(const int pos, char *pData)
{	
	//On passe le noeud header
	pData += sizeof(ix_NoeudHeader);
	
	//On passe le premier pointeur pour être à la première cle
	//On se déplace pour aller à la clé souhaité
	pData += sizeof(int) + (pos-1)*(this->fh.tailleCle+this->fh.taillePtr);	
}

//Pointe vers le ptr avant la cle
void IX_IndexHandle :: GetPtrInf(const int pos, char *pData)
{
	//Nous allons à la clé 
	GetCle(pos, pData);
	//Nous remontons en arrière pour aller sur le ptr inf
	pData -= this->fh.taillePtr;
	
};

//Pointe vers le ptr après la cle
void IX_IndexHandle :: GetPtrSup(const int pos, char *pData)
{
	//Nous allons à la clé 
	GetCle(pos, pData);
	//Nous passons la clé pour aller au ptr situé à droite
	pData += this->fh.tailleCle;	
	
};


//Insère une clé dans un noeud
RC IX_IndexHandle :: InsertKey(PageNum noeud, char *key, char *pDataPtr)
{
	int res;
	
	//On récupère la page 
	PF_PageHandle *page = new PF_PageHandle();
	res = this->pf->GetThisPage(noeud, *page);
	if(res != 0)
		return res;


return 0;
/**
	//On récupère les données de la page
	char *pData;
	res = page->GetData(pData);
	if(res !=0)
		return res;

	
	//On récupère le header
	ix_NoeudHeader nh;
	memcpy(&nh, pData, sizeof(ix_NoeudHeader));
	

	if(nh.nbCleCrt == 0)
		return 0;
	
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
					pDataPtr = pData;
					
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
					page->GetData(pData);
					
					//On incrémente le nombre de clé dans le fichier
					nh.nbCleCrt ++;
					
					//On réintègre le nouveau header dans le fichier
					memcpy(pData, &nh, sizeof(ix_NoeudHeader));
					
					return 0;
				}
				
				//On repointe au début du fichier
				page->GetData(pData);

				
			}
	return 0;
	
**/
};

