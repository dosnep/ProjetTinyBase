#include "ix.h"
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//Constructeur
IX_IndexScan :: IX_IndexScan()
{

};
//Destructeur
IX_IndexScan :: ~IX_IndexScan()
{

}

// Open index scan
RC IX_IndexScan :: OpenScan(const IX_IndexHandle &indexHandle,
			CompOp compOp,
			void *value)
{
	
this->ih = new IX_IndexHandle(*indexHandle.pf, indexHandle.fh);
this->compOp = compOp;
this->value = value;
this->isOpen = true;

//On teste si l'attribut du fichier est de type string, int ou float et on assigne la value dans son bon type
if(value != NULL){
switch(indexHandle.fh.attrType){
case INT:{
	
	this->valInt = *((int*)(value));
	break;
		}

case FLOAT:{
	this->valFloat = *((float*)(value));
	break;
			}

case STRING:{
	if(indexHandle.fh.tailleCle<=MAXSTRINGLEN&&indexHandle.fh.tailleCle>=1)
	this->valString = new char[indexHandle.fh.tailleCle];
	memcpy(valString, value, indexHandle.fh.tailleCle);
	break;
	
			}
}
}
//On se met à la première feuille
this->currentLeaf = 1;
//On se place à la première clé
this->indiceCle = 1;
//On se place au premier bucket
this->currentBucket = 2;
//On se place au premier rid du bucket
this->indiceRid = 0;
    

	return 0;

}


RC IX_IndexScan :: GetNextEntry(RID &rid)
{
	
	PF_PageHandle *leaf = new PF_PageHandle();
	PF_PageHandle *bucket = new PF_PageHandle();
	ix_NoeudHeader leafH;
	ix_BucketHeader bucketH;
	char *pData;
	char key[10];
	
//Tant qu'on a pas trouvé un rid ou qu'on a pas atteint la fin du fichier
while(1)	
{	
	//On ouvre la feuille
	this->ih->pf->GetThisPage(this->currentLeaf,*leaf);
	//On récupère les données de la page
	leaf->GetData(pData);
	//On récupère le header de la feuille
	memcpy(&leafH,pData, sizeof(ix_NoeudHeader));

	//Si on a rien dans notre feuille alors EOF
	if(leafH.nbCleCrt == 0)
	{
		this->ih->pf->ForcePages();
		this->ih->pf->UnpinPage(this->currentLeaf);
		this->ih->pf->UnpinPage(this->currentBucket);


		return 1;	
	}
	//On teste si nous avons lu toutes les clés de la feuille
	else if(this->indiceCle >leafH.nbCleCrt)
	{
		
	//On teste si nous avons atteint la fin de l'index
	if(leafH.right == -1)	
	{	//EOF

		this->ih->pf->ForcePages();
		this->ih->pf->UnpinPage(this->currentLeaf);
		this->ih->pf->UnpinPage(this->currentBucket);

		return 1;	
	}				
		//Si oui on passe à une autre feuille
		this->currentLeaf = leafH.right;

		//On modifie l'indice de la clé
		this->indiceCle = 1;
		
		//On modifie l'indice du rid dans le bucket
		this->indiceRid = 0;
		
		//On charge la feuille
		this->ih->pf->GetThisPage(this->currentLeaf,*leaf);
		//On charge les données de la feuille
		leaf->GetData(pData);			
		//On récupère le premier bucket de cette feuille
		this->ih->GetPtrInf(1,pData);
		//On le recopie 
		memcpy(&this->currentBucket, pData, sizeof(PageNum));
		
		
		
	}

	else
	{

	//On ouvre la page du bucket
	this->ih->pf->GetThisPage(currentBucket,*bucket);
	//On récupère les données du bucket
	bucket->GetData(pData);
	//On récupère le bucket header
	memcpy(&bucketH,pData, sizeof(ix_BucketHeader));
	


	//On teste si nous avons lu tout les rid
	if(this->indiceRid+1>bucketH.nbRidCrt)
	{
		//On change l'indice de la clé
		this->indiceCle++;
		
		//Si nous ne somme pas à la fin de la feuille
		if(this->indiceCle <= leafH.nbCleCrt)
		{	//On récupère les données de la feuille
			leaf->GetData(pData);
			//On pointe vers le nouveau bucket
			this->ih->GetPtrInf(this->indiceCle, pData);
			//On recopie le nouveau bucket
			memcpy(&this->currentBucket, pData, sizeof(int));
			this->indiceRid = 0;

		}
	}
	
	//Sinon on renvoie le bucket
	else {
		
		//On récupère la clé à l'indice
		leaf->GetData(pData);
		this->ih->GetCle(this->indiceCle, pData);
		memcpy(key, pData, sizeof(this->ih->fh.tailleCle));
		
		if(EstUnBonRecord(key))
		{	
		bucket->GetData(pData);		
		//On passe le bucket header
		pData += sizeof(ix_BucketHeader);
		//On se rend au bon rid
		pData += this->indiceRid * sizeof(RID);
		//On recopie le rid
		memcpy(&rid,  pData, sizeof(RID));
		//On incrémente le rid à lire pour la prochaine fois
		this->indiceRid++;
		this->ih->pf->ForcePages();
		this->ih->pf->UnpinPage(this->currentLeaf);
		this->ih->pf->UnpinPage(this->currentBucket);
		return 0;
		}
		
		else
		{
				this->indiceRid++;
	
		}	
		
		}
		
		
	}
	
}	
	
	return 0;
}












bool IX_IndexScan ::  EstUnBonRecord(char *key)
{
	
switch(this->ih->fh.attrType)
{	
	
	if(this->value == NULL) return true;
	
	//On associe à chaque type les tests associés
	case INT :{
	int tmpVal;
	memcpy(&tmpVal, key, sizeof(int));

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
		
		float tmpVal;
		memcpy(&tmpVal, key, sizeof(float));
	
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
		
		char *tmpString = key;
		
		switch(this->compOp)
		{
			case EQ_OP :{
				
				if(strncmp(tmpString, key, this->ih->fh.tailleCle) == 0) return true;
				
				break;
						}
			
			
			case LT_OP :{
				
				if(strncmp(tmpString, key, this->ih->fh.tailleCle)<0) return true;
				
				break;
						}
						
			case GT_OP :{
				
				if(strncmp(tmpString, key, this->ih->fh.tailleCle) > 0) return true;
				
				break;
						}			
			
			case LE_OP :{
				
				if(strncmp(tmpString, key, this->ih->fh.tailleCle) <= 0) return true;
				
				break;
						}
			
			
			case GE_OP :{
				
				if(strncmp(tmpString, key, this->ih->fh.tailleCle) >= 0) return true;
				
				break;
						}
			
			case NE_OP :{
				
				if(strncmp(tmpString, key, this->ih->fh.tailleCle) != 0) return true;
				
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

RC IX_IndexScan :: CloseScan()
{
	this->isOpen = false;
	return 0;
	
}











