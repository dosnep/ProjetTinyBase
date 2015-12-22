//
// File:        SM component stubs
// Description: Print parameters of all SM_Manager methods
// Authors:     Dallan Quass (quass@cs.stanford.edu)
//

#include <cstdio>
#include <iostream>
#include "sm.h"
#include "unistd.h"
#include "stddef.h"

using namespace std;

SM_Manager::SM_Manager(IX_Manager &ixm, RM_Manager &rmm) : ixm(ixm), rmm(rmm)
{
}

SM_Manager::~SM_Manager()
{
}

RC SM_Manager::OpenDb(const char *dbName)
{
	int res;
	//On va se placer dans le répertoire de la bd
	chdir(dbName);
	
	//Imprime le chemin où l'on se trouve
    system ("pwd");

	//On va charger les catalogues
	res = rmm.OpenFile("relcat", relcatFH);
	if(res != 0)
		return res;             	
			
		

	res = rmm.OpenFile("attrcat", this->attrcatFH);
	if(res != 0)
		return res;
	
   return (0);
}

RC SM_Manager::CloseDb()
{
	
	int res;
	//On remonte dans la répertoire précédent
	chdir("..");

	//Imprime le chemin où l'on se trouve
    system ("pwd");
	

	//On va charger les catalogues
	res = rmm.CloseFile(this->relcatFH);
	if(res != 0)
		return res;

	res = rmm.CloseFile(this->attrcatFH);
	if(res != 0)
		return res;	
	
    return (0);
}

RC SM_Manager::CreateTable(const char *relName,
                           int        attrCount,
                           AttrInfo   *attributes)
{
	
	int res;
	int offset = 0; //Décalage de l'attribut par rapport au début du tuple
    RID rid;
    cout << "CreateTable\n"
         << "   relName     =" << relName << "\n"
         << "   attrCount   =" << attrCount << "\n";
    //Il faut parcourir chacun des attributs et les ajouter au catalogue attrcat
    for (int i = 0; i < attrCount; i++)
    {    cout << "   attributes[" << i << "].attrName=" << attributes[i].attrName
             << "   attrType="
             << (attributes[i].attrType == INT ? "INT" :
                 attributes[i].attrType == FLOAT ? "FLOAT" : "STRING")
             << "   attrLength=" << attributes[i].attrLength << "\n";
 
		//On initialise le nouveau tuple à ajouter dans attrcat
		attrcat newTupleAttr;
		strcpy(newTupleAttr.relName,relName); //Nom de la relation à laquelle appartient l'attribut
		strcpy(newTupleAttr.attrName, attributes[i].attrName); //Nom de l'attribut
		newTupleAttr.offset = offset;	//Décalage de l'attribut
		newTupleAttr.indexNo = -1; //Aucun index à l'initialisation
		
		//Il faut tester le type de l'attribut
		switch(attributes[i].attrType)
		{
			case INT :
			newTupleAttr.attrType = INT;
			newTupleAttr.attrLength = attributes[i].attrLength;
			offset += newTupleAttr.attrLength; //On incrémente l'offset mettre à jour le décalage
			break;
			
			case FLOAT :
			newTupleAttr.attrType = FLOAT;	
			newTupleAttr.attrLength = attributes[i].attrLength;	
			offset += newTupleAttr.attrLength;							
			break;
			
			case STRING :
			newTupleAttr.attrType = STRING;
			newTupleAttr.attrLength = attributes[i].attrLength;	
			offset += newTupleAttr.attrLength;								
			break;	
			
		}
	
		//On ajoute le tuple dans le fichier
		res = attrcatFH.InsertRec((char *)&newTupleAttr,rid);
			if(res != 0)
				return res;
	}
	
	//Il faut maintenant ajouter la nouvelle table dans le catalogue relcat
		//On initialise le nouveau tuple
	relcat newTupleRel;
	strcpy(newTupleRel.relName, relName);
	newTupleRel.tupleLength = offset;
	newTupleRel.attrCount = attrCount;
		//On l'insère dans le catalogue
	res = relcatFH.InsertRec((char*)&newTupleRel,rid);
			if(res != 0)
				return res;

	//Pour finir il faut créer le fichier de cette nouvelle relation
	res = rmm.CreateFile(relName, offset);	
			if(res != 0)
				return res;
 
    return (0);
}

RC SM_Manager::DropTable(const char *relName)
{
    cout << "DropTable\n   relName=" << relName << "\n";
    return (0);
}

RC SM_Manager::CreateIndex(const char *relName,
                           const char *attrName)
{
    cout << "CreateIndex\n"
         << "   relName =" << relName << "\n"
         << "   attrName=" << attrName << "\n";
    return (0);
}

RC SM_Manager::DropIndex(const char *relName,
                         const char *attrName)
{
    cout << "DropIndex\n"
         << "   relName =" << relName << "\n"
         << "   attrName=" << attrName << "\n";
    return (0);
}

RC SM_Manager::Load(const char *relName,
                    const char *fileName)
{
    cout << "Load\n"
         << "   relName =" << relName << "\n"
         << "   fileName=" << fileName << "\n";
    return (0);
}

RC SM_Manager::Print(const char *relName)
{
RM_FileScan fs;
RM_Record rec;
int res;
int attrCount;
RM_FileHandle rfh;
char *pData;
char *name = new char[MAXNAME+1];
strcpy(name,relName);
attrcat tmp;
relcat tmp2;

//Il faut aller chercher dans le catalogue des relations le nombres d'attributs de relName
//On ouvre un scan
res = fs.OpenScan(this->relcatFH,STRING,sizeof(name),offsetof(relcat,relName), EQ_OP,name, NO_HINT);
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;

	rec.GetData(pData);
	memcpy(&tmp2, pData, sizeof(relcat));
	attrCount = tmp2.attrCount;	
	printf("val : %d\n", attrCount);

	
}
//On ferme le scan
fs.CloseScan();

DataAttrInfo *attributes = new DataAttrInfo[attrCount];

res = fs.OpenScan(this->attrcatFH,STRING,sizeof(name),offsetof(attrcat,relName), EQ_OP,name, NO_HINT);
if(res != 0)
	return res;

res = 0;
//On récupère l'ensemble des attributs pour la table relName
int i = 0;
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;

	rec.GetData(pData);
	memcpy(&tmp, pData, sizeof(attrcat));
	
	attributes[i].offset = tmp.offset;
	attributes[i].attrType = tmp.attrType;
	attributes[i].attrLength = tmp.attrLength;
	attributes[i].indexNo = tmp.indexNo;
	strcpy(attributes[i].relName,tmp.relName);
	strcpy(attributes[i].attrName,tmp.attrName);
	i++;
	
}
Printer p(attributes, attrCount);
//On imprime le header des attributs
p.PrintHeader(cout);
//On ferme le scan
fs.CloseScan();

//Il faut ouvrir la table  relName
rmm.OpenFile(relName,rfh);

//On ouvre une nouveau scan sur les tuples de la table relName
res = fs.OpenScan(rfh,STRING,sizeof(name),offsetof(relcat,relName), NO_OP,name, NO_HINT);
if(res != 0)
	return res;
	
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;
		
		//On récupère les données de l'enregistrement
		rec.GetData(pData);
		
		//On affiche les données
		p.Print(cout, pData);
	
}
//On ferme le scan
fs.CloseScan();

//On imprime le footer
p.PrintFooter(cout);

    return (0);
}

RC SM_Manager::Set(const char *paramName, const char *value)
{
    cout << "Set\n"
         << "   paramName=" << paramName << "\n"
         << "   value    =" << value << "\n";
    return (0);
}

RC SM_Manager::Help()
{

RM_FileScan fs;
RM_Record rec;
int res;
DataAttrInfo *attributes = new DataAttrInfo[3];
int attrCount;
RM_FileHandle rfh;
char *pData;
char *name = new char[MAXNAME+1];
strcpy(name,"relcat");
attrcat tmp;
res = fs.OpenScan(this->attrcatFH,STRING,sizeof(name),offsetof(attrcat,relName), EQ_OP,name, NO_HINT);
if(res != 0)
	return res;

res = 0;
//On récupère l'ensemble des attributs pour la table relcat
int i = 0;
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;

	rec.GetData(pData);
	memcpy(&tmp, pData, sizeof(attrcat));
	
	attributes[i].offset = tmp.offset;
	attributes[i].attrType = tmp.attrType;
	attributes[i].attrLength = tmp.attrLength;
	attributes[i].indexNo = tmp.indexNo;
	strcpy(attributes[i].relName,tmp.relName);
	strcpy(attributes[i].attrName,tmp.attrName);
	i++;
	
}
attrCount = 3;
Printer p(attributes, attrCount);
//On imprime le header des attributs
p.PrintHeader(cout);
//On ferme le scan
fs.CloseScan();

//On ouvre une nouveau scan sur le catalogue des relations
res = fs.OpenScan(this->relcatFH,STRING,sizeof(name),offsetof(relcat,relName), NO_OP,name, NO_HINT);
if(res != 0)
	return res;
	
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;
		
		//On récupère les données de l'enregistrement
		rec.GetData(pData);
		
		//On affiche les données
		p.Print(cout, pData);
	
}
//On ferme le scan
fs.CloseScan();

//On imprime le footer
p.PrintFooter(cout);

return 0;
}

RC SM_Manager::Help(const char *relName)
{
RM_FileScan fs;
RM_Record rec;
int res;
DataAttrInfo *attributes = new DataAttrInfo[6];
int attrCount;
RM_FileHandle rfh;
char *pData;
char *name = new char[MAXNAME+1];
strcpy(name,relName);
char *nameAttrCat = new char[MAXNAME+1];
strcpy(nameAttrCat,"attrcat");


attrcat tmp;
res = fs.OpenScan(this->attrcatFH,STRING,sizeof(nameAttrCat),offsetof(attrcat,relName), EQ_OP,nameAttrCat, NO_HINT);
if(res != 0)
	return res;

res = 0;
//On récupère l'ensemble des attributs pour la table attrcat
int i = 0;
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;

	rec.GetData(pData);
	memcpy(&tmp, pData, sizeof(attrcat));
	
	attributes[i].offset = tmp.offset;
	attributes[i].attrType = tmp.attrType;
	attributes[i].attrLength = tmp.attrLength;
	attributes[i].indexNo = tmp.indexNo;
	strcpy(attributes[i].relName,tmp.relName);
	strcpy(attributes[i].attrName,tmp.attrName);
	i++;
	
}
attrCount = 6;
Printer p(attributes, attrCount);
//On imprime le header des attributs
p.PrintHeader(cout);
//On ferme le scan
fs.CloseScan();

//On ouvre une nouveau scan sur le catalogue attrcat
res = fs.OpenScan(this->attrcatFH,STRING,sizeof(name),offsetof(attrcat,relName), EQ_OP,name, NO_HINT);
if(res != 0)
	return res;
	
while(res != RM_EOF)
{	res = fs.GetNextRec(rec);
		if(res==RM_EOF)
			break;
		
		//On récupère les données de l'enregistrement
		rec.GetData(pData);
		
		//On affiche les données
		p.Print(cout, pData);
	
}
//On ferme le scan
fs.CloseScan();

//On imprime le footer
p.PrintFooter(cout);
return 0;
}

void SM_PrintError(RC rc)
{
    cout << "SM_PrintError\n   rc=" << rc << "\n";
}

