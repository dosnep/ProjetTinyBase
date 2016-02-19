#include "QL_FilterOp.h"
#include <stddef.h>
#include <stdio.h>

QL_FilterOp :: QL_FilterOp(SM_Manager &smm, QL_Operator &op, const Condition cond) : smm(&smm), op(&op)
{

//On teste si notre élément de droite est une valeur.
if(!cond.bRhsIsAttr)
{
	
RM_FileScan fs;
RM_Record rec;
char *pData;
attrcat tmp;
	//On va récupérer le offset de l'attribut que nous voulons tester
	fs.OpenScan(smm.attrcatFH,STRING,sizeof(MAXNAME+1),offsetof(attrcat,attrName), EQ_OP,cond.lhsAttr.attrName, NO_HINT);
	fs.GetNextRec(rec);
	rec.GetData(pData);
	memcpy(&tmp, pData, sizeof(attrcat));		
	offset = tmp.offset;
	fs.CloseScan();
	
	this->cond = cond;
	
}
}

QL_FilterOp :: ~QL_FilterOp(){}
RC QL_FilterOp :: Open ()
{	
	op->Open();
		
	return 0;}
RC QL_FilterOp :: GetNext(RM_Record &rec)
{
RM_Record r;
bool isGood = false;
char *pData;
int res;
int iValLeft;
int iValRight;

while(!isGood)
{	res = op->GetNext(rec);
	if(res != 0)
			return res;

	rec.GetData(pData);
	memcpy(&iValLeft, pData, sizeof(int));
	iValRight = (*(int *)cond.rhsValue.data); 

	if(iValLeft == iValRight)
	 {
		 isGood = true;
	}

}
return 0;
}
RC QL_FilterOp :: Close()
{
	op->Close();
	return 0;}
