//
// File:        SM component stubs
// Description: Print parameters of all SM_Manager methods
// Authors:     Dallan Quass (quass@cs.stanford.edu)
//

#include <cstdio>
#include <iostream>
#include "tinybase.h"
#include "sm.h"
#include "ix.h"
#include "rm.h"
#include "unistd.h"

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
	res = rmm.OpenFile("relcat", this->relcatFH);
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
    cout << "Print\n"
         << "   relName=" << relName << "\n";
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
    cout << "Help\n";
    return (0);
}

RC SM_Manager::Help(const char *relName)
{
    cout << "Help\n"
         << "   relName=" << relName << "\n";
    return (0);
}

void SM_PrintError(RC rc)
{
    cout << "SM_PrintError\n   rc=" << rc << "\n";
}

