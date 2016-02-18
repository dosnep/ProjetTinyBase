#include "QL_TblScanOp.h"

QL_TblScanOp :: QL_TblScanOp(RM_Manager &rmm, SM_Manager &smm, char *relName): rmm(&rmm), smm(&smm)
{
//On ouvre la relation relName
rmm.OpenFile(relName,fh);

};

QL_TblScanOp ::~QL_TblScanOp(){};

RC QL_TblScanOp ::  Open (){

int res;
//On ouvre le scan	
res = fs.OpenScan(fh,STRING,0,0, NO_OP,(void*)"", NO_HINT);
if(res != 0)
	return res;	
	
return 0;}

RC QL_TblScanOp :: GetNext(RM_Record &rec){
int res;
//On retourne le prochain enregistrement
res = fs.GetNextRec(rec);
if(res!=0)
	return res;
return 0;}
RC QL_TblScanOp ::  Close(){

int res;
//On ferme le full scan.
res = fs.CloseScan();
if(res!=0)
	return res;
		
return 0;}

