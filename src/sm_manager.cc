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
    cout << "CreateTable\n"
         << "   relName     =" << relName << "\n"
         << "   attrCount   =" << attrCount << "\n";
    for (int i = 0; i < attrCount; i++)
        cout << "   attributes[" << i << "].attrName=" << attributes[i].attrName
             << "   attrType="
             << (attributes[i].attrType == INT ? "INT" :
                 attributes[i].attrType == FLOAT ? "FLOAT" : "STRING")
             << "   attrLength=" << attributes[i].attrLength << "\n";
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

