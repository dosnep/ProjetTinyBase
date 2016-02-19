//
// ql_manager_stub.cc
//

// Note that for the SM component (HW3) the QL is actually a
// simple stub that will allow everything to compile.  Without
// a QL stub, we would need two parsers.

#include <cstdio>
#include <iostream>
#include <sys/times.h>
#include <sys/types.h>
#include <cassert>
#include <unistd.h>
#include "tinybase.h"
#include "sm.h"
#include "ix.h"
#include "rm.h"
#include "unistd.h"
#include "stddef.h"
#include "QL_TblScanOp.h"
#include "QL_ProjectOp.h"
#include "QL_FilterOp.h"
#include "stdio.h"
#include "stdlib.h"


using namespace std;

//
// QL_Manager::QL_Manager(SM_Manager &smm, IX_Manager &ixm, RM_Manager &rmm)
//
// Constructor for the QL Manager
//
QL_Manager::QL_Manager(SM_Manager &smm, IX_Manager &ixm, RM_Manager &rmm) : smm(&smm), ixm(&ixm), rmm(&rmm)
{
    // Can't stand unused variable warnings!
    assert (&smm && &ixm && &rmm);
}

//
// QL_Manager::~QL_Manager()
//
// Destructor for the QL Manager
//
QL_Manager::~QL_Manager()
{
}

//
// Handle the select clause
//
RC QL_Manager::Select(int nSelAttrs, const RelAttr selAttrs[],
                      int nRelations, const char * const relations[],
                      int nConditions, const Condition conditions[])
{
    int i;

    cout << "Select\n";

    cout << "   nSelAttrs = " << nSelAttrs << "\n";
    for (i = 0; i < nSelAttrs; i++)
        cout << "   selAttrs[" << i << "]:" <<  selAttrs[i].attrName << "\n";

    cout << "   nRelations = " << nRelations << "\n";
    for (i = 0; i < nRelations; i++)
        cout << "   relations[" << i << "] " << relations[i] << "\n";

    cout << "   nCondtions = " << nConditions << "\n";
    for (i = 0; i < nConditions; i++)
        cout << "   conditions[" << i << "]:" << conditions[i] << "\n";

int res;
QL_Operator *f = new QL_TblScanOp(*rmm,*smm,"tst");

for(i = 0; i<nConditions; i++)
{
f = new QL_FilterOp(*smm,*f,conditions[i]);
}


QL_ProjectOp *tst = new QL_ProjectOp(*smm,*f,nSelAttrs,selAttrs,"tst");

Printer p(tst->attributes, tst->nbAttr);
//On imprime le header des attributs
p.PrintHeader(cout);
RM_Record rec;
char *pData;
tst->Open();
res = 0;

while(res != RM_EOF)
{
	res = tst->GetNext(rec);
	if(res == RM_EOF)
		break;
	rec.GetData(pData);
	p.Print(cout, pData);

}
tst->Close();
p.PrintFooter(cout);
    return 0;
}

//
// Insert the values into relName
//
RC QL_Manager::Insert(const char *relName,
                      int nValues, const Value values[])
{
    int i;

    cout << "Insert\n";

    cout << "   relName = " << relName << "\n";
    cout << "   nValues = " << nValues << "\n";
    for (i = 0; i < nValues; i++)
        cout << "   values[" << i << "]:" << values[i] << "\n";

	
//Nous allons ouvrir le catalogue rel pour y chercher les informations
RM_FileScan fs;
RM_Record rec;
int res;
RM_FileHandle rfh;
char *name = new char[MAXNAME+1];
strcpy(name,relName);
int attrCount = 0;
int tupleLength = 0;
relcat tmpRel;
char *pData;

res = fs.OpenScan(smm->relcatFH,STRING,sizeof(name),offsetof(relcat,relName), EQ_OP,name, NO_HINT);
if(res != 0)
	return res;

res = 0;
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;
		
		rec.GetData(pData);
		memcpy(&tmpRel, pData, sizeof(relcat));
		attrCount = tmpRel.attrCount;
		tupleLength = tmpRel.tupleLength;
		printf("attrcount : %d, tupleLength : %d\n", attrCount, tupleLength);
	
}	
fs.CloseScan();

//Nous allons ouvrir le catalogue des attributs pour y chercher toutes les infos sur les attributs.
res = fs.OpenScan(smm->attrcatFH,STRING,sizeof(name),offsetof(attrcat,relName), EQ_OP,name, NO_HINT);
if(res != 0)
	return res;

res = 0;
//On récupère l'ensemble des attributs pour la table relcat
DataAttrInfo *attributes = new DataAttrInfo[attrCount];
i = 0;
attrcat tmpAttr;
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;

	rec.GetData(pData);
	memcpy(&tmpAttr, pData, sizeof(attrcat));
	
	attributes[i].offset = tmpAttr.offset;
	attributes[i].attrType = tmpAttr.attrType;
	attributes[i].attrLength = tmpAttr.attrLength;
	attributes[i].indexNo = tmpAttr.indexNo;
	strcpy(attributes[i].relName,tmpAttr.relName);
	strcpy(attributes[i].attrName,tmpAttr.attrName);
	i++;
	
}

	char tuple[tupleLength+10];
	char tmp[MAXNAME+1];
	RID rid;
	rmm->OpenFile(relName,rfh);
	i = 0;
	int tmpIVal;
	float tmpFVal;

			for(i = 0; i<nValues; i++)
			{
			
			switch(values[i].type)
			{
				case INT :
			tmpIVal = (*(int*) values[i].data);
		memcpy(&tuple[attributes[i].offset],&tmpIVal,sizeof(int));
					break;
					
				case FLOAT:
			tmpFVal = (*(float*) values[i].data);
		memcpy(&tuple[attributes[i].offset],&tmpFVal,sizeof(float));
	
				break;
				case STRING:
				//memcpy(tmp,values[i].data,MAXNAME+1);
				memcpy(&tuple[attributes[i].offset],values[i].data,MAXNAME+1);
				
				break;
				
			};
						

			}
			
			rfh.InsertRec(tuple,rid);
			rmm->CloseFile(rfh);


    return 0;
}

//
// Delete from the relName all tuples that satisfy conditions
//
RC QL_Manager::Delete(const char *relName,
                      int nConditions, const Condition conditions[])
{
    int i;

    cout << "Delete\n";

    cout << "   relName = " << relName << "\n";
    cout << "   nCondtions = " << nConditions << "\n";
    for (i = 0; i < nConditions; i++)
        cout << "   conditions[" << i << "]:" << conditions[i] << "\n";

    return 0;
}


//
// Update from the relName all tuples that satisfy conditions
//
RC QL_Manager::Update(const char *relName,
                      const RelAttr &updAttr,
                      const int bIsValue,
                      const RelAttr &rhsRelAttr,
                      const Value &rhsValue,
                      int nConditions, const Condition conditions[])
{
    int i;

    cout << "Update\n";

    cout << "   relName = " << relName << "\n";
    cout << "   updAttr:" << updAttr << "\n";
    if (bIsValue)
        cout << "   rhs is value: " << rhsValue << "\n";
    else
        cout << "   rhs is attribute: " << rhsRelAttr << "\n";

    cout << "   nCondtions = " << nConditions << "\n";
    for (i = 0; i < nConditions; i++)
        cout << "   conditions[" << i << "]:" << conditions[i] << "\n";

    return 0;
}

//
// void QL_PrintError(RC rc)
//
// This function will accept an Error code and output the appropriate
// error.
//
void QL_PrintError(RC rc)
{
    cout << "QL_PrintError\n   rc=" << rc << "\n";
}
