#include "QL_ProjectOp.h"
#include <stddef.h>

QL_ProjectOp :: QL_ProjectOp(SM_Manager &smm, QL_Operator &op,int nbSelect, const RelAttr selAttrs[]) : smm(&smm), op(&op)
{
int i;
attributes = new DataAttrInfo[nbSelect];
RM_FileScan fs;
int res;
attrcat tmp;
RM_Record rec;
char *pData;

//On va chercher chacun des attributs à projeter dans le catalogue des attributs
for(i = 0;i<nbSelect;i++)
{
fs.OpenScan(smm.attrcatFH,STRING,sizeof(MAXNAME+1),offsetof(attrcat,attrName), EQ_OP,selAttrs[i].attrName, NO_HINT);
res = 0;

res = fs.GetNextRec(rec);
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
