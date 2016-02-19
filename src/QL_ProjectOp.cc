#include "QL_ProjectOp.h"
#include <stddef.h>
#include <stdio.h>
QL_ProjectOp :: QL_ProjectOp(SM_Manager &smm, QL_Operator &op,int nbSelect, const RelAttr selAttrs[],char *relation) : smm(&smm), op(&op)
{
int i;
RM_FileScan fs;
attrcat tmp;
relcat rel;
RM_Record rec;
char *pData;

//Si l'attribut à sélectionner est *
if(strcmp(selAttrs[0].attrName,"*") == 0)
{
	//On va récupérer le nombre d'attribut de la relation dans le catalogue des relations
	fs.OpenScan(smm.relcatFH,STRING,sizeof(MAXNAME+1),offsetof(relcat,relName),EQ_OP,relation, NO_HINT);
	fs.GetNextRec(rec);
	rec.GetData(pData);
	memcpy(&rel, pData, sizeof(relcat));	
	nbAttr = rel.attrCount;
	printf("nbAttr : %d\n",nbAttr);
	fs.CloseScan();
attributes = new DataAttrInfo[nbAttr];
	//On va chercher chacun des attributs à projeter dans le catalogue des attributs
	for(i = 0;i<nbAttr;i++)
	{
	fs.OpenScan(smm.attrcatFH,STRING,sizeof(MAXNAME+1),offsetof(attrcat,relName), EQ_OP,relation, NO_HINT);
	fs.GetNextRec(rec);
	rec.GetData(pData);
	memcpy(&tmp, pData, sizeof(attrcat));
	attributes[i].offset = tmp.offset;
	attributes[i].attrType = tmp.attrType;
	attributes[i].attrLength = tmp.attrLength;
	attributes[i].indexNo = tmp.indexNo;
	strcpy(attributes[i].relName,tmp.relName);
	strcpy(attributes[i].attrName,tmp.attrName);
	}

	fs.CloseScan();	

}

else
{
	nbAttr = nbSelect;
	attributes = new DataAttrInfo[nbAttr];
	//On va chercher chacun des attributs à projeter dans le catalogue des attributs
	for(i = 0;i<nbAttr;i++)
	{
	fs.OpenScan(smm.attrcatFH,STRING,sizeof(MAXNAME+1),offsetof(attrcat,attrName), EQ_OP,selAttrs[i].attrName, NO_HINT);
	fs.GetNextRec(rec);
	rec.GetData(pData);
	memcpy(&tmp, pData, sizeof(attrcat));
	attributes[i].offset = tmp.offset;
	attributes[i].attrType = tmp.attrType;
	attributes[i].attrLength = tmp.attrLength;
	attributes[i].indexNo = tmp.indexNo;
	strcpy(attributes[i].relName,tmp.relName);
	strcpy(attributes[i].attrName,tmp.attrName);
	fs.CloseScan();	
	}
}

};

QL_ProjectOp ::~QL_ProjectOp(){};

RC QL_ProjectOp ::  Open (){
op->Open();	
	
return 0;}

RC QL_ProjectOp :: GetNext(RM_Record &rec){
int res;
res = op->GetNext(rec);
if(res != 0)
		return res;

return 0;}
RC QL_ProjectOp ::  Close(){
op->Close();
return 0;}
