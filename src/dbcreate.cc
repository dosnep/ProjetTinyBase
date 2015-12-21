//
// dbcreate.cc
//
// Author: Jason McHugh (mchughj@cs.stanford.edu)
//
// This shell is provided for the student.

#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "rm.h"
#include "pf.h"
#include "sm.h"
#include "tinybase.h"
#include "stddef.h"

using namespace std;

//
// main
//

void PrintError(RC rc);


void PrintError(RC rc)
{
    if (abs(rc) <= END_PF_WARN)
        PF_PrintError(rc);
    //else if (abs(rc) <= END_RM_WARN)
        //RM_PrintError(rc);
    else
        cerr << "Error code out of range: " << rc << "\n";
}

int main(int argc, char *argv[])
{
    char *dbname;
    char command[255] = "mkdir ";
    RC rc;
    RID rid;

    // Look for 2 arguments. The first is always the name of the program
    // that was executed, and the second should be the name of the
    // database.
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " dbname \n";
        exit(1);
    }

    // The database name is the second argument
    dbname = argv[1];

    // Create a subdirectory for the database
    system (strcat(command,dbname));

    if (chdir(dbname) < 0) {
        cerr << argv[0] << " chdir error to " << dbname << "\n";
        exit(1);
    }

	
    // Create the system catalogs...
	PF_Manager pfm;
	RM_Manager rmm(pfm);
	//On va créer les deux catalogues : relcat et attrcat
	RM_FileHandle relcatFH;
	RM_FileHandle attrcatFH;
	rc = rmm.CreateFile("relcat",sizeof(relcat));
		if(rc != 0)
                PrintError(rc);
	rc = rmm.CreateFile("attrcat",sizeof(attrcat));
		if(rc != 0)
                PrintError(rc);
	//On ouvre les deux fichiers
	rc = rmm.OpenFile("relcat",relcatFH);
		if(rc != 0)
                PrintError(rc);
	rc = rmm.OpenFile("attrcat",attrcatFH);
		if(rc != 0)
                PrintError(rc);
	
	//Il faut maintenant remplir le relcat avec sa propre description
		//On initialise le tuple pour les relations
	relcat relInit;
	relInit.attrCount = 3;
	relInit.tupleLength = sizeof(relcat);
	strcpy(relInit.relName, "relcat");
		//On initialise le tuple pour les attributs
	relcat attrInit;
	attrInit.attrCount = 6;
	attrInit.tupleLength = sizeof(attrcat);
	strcpy(attrInit.relName, "attrCat");
	//On ajoute ces 2 tuples dans le catalogue relcat
	rc = relcatFH.InsertRec((char *)&relInit, rid);
		if(rc != 0)
                PrintError(rc);
	rc = relcatFH.InsertRec((char *)&attrInit, rid);
		if(rc != 0)
                PrintError(rc);
                
	//On peut fermer le fichier
	rc = rmm.CloseFile(relcatFH);
		if(rc != 0)
                PrintError(rc);
				
	//Il faut maintenant remplir le catalogue attrcat avec tout les attributs des 2 relations attrcat et relcat
		//On ajoute tout les attributs de la relation relcat
//1 er attribut
	attrcat attrcat_relcat_relName;
	strcpy(attrcat_relcat_relName.relName,"relcat");
	strcpy(attrcat_relcat_relName.attrName,"relName");
	attrcat_relcat_relName.offset = (int)offsetof(relcat,relName);
	attrcat_relcat_relName.attrType = STRING;
	attrcat_relcat_relName.attrLength = MAXNAME+1;
	attrcat_relcat_relName.indexNo = -1;

//2ème attribut
	attrcat attrcat_relcat_tupleLength;
	strcpy(attrcat_relcat_tupleLength.relName,"relcat");
	strcpy(attrcat_relcat_tupleLength.attrName,"tupleLength");
	attrcat_relcat_tupleLength.offset = (int)offsetof(relcat,tupleLength);
	attrcat_relcat_tupleLength.attrType = INT;
	attrcat_relcat_tupleLength.attrLength = 4;
	attrcat_relcat_tupleLength.indexNo = -1;

//3ème attribut
	attrcat attrcat_relcat_attrCount;
	strcpy(attrcat_relcat_attrCount.relName,"relcat");
	strcpy(attrcat_relcat_attrCount.attrName,"attrCount");
	attrcat_relcat_attrCount.offset = (int)offsetof(relcat,attrCount);
	attrcat_relcat_attrCount.attrType = INT;
	attrcat_relcat_attrCount.attrLength = 4;
	attrcat_relcat_attrCount.indexNo = -1;

		//On ajoute tout les attributs de la relation attrcat
//1 er attribut
	attrcat attrcat_attrcat_relName;
	strcpy(attrcat_attrcat_relName.relName,"attrcat");
	strcpy(attrcat_attrcat_relName.attrName,"relName");
	attrcat_attrcat_relName.offset = (int)offsetof(attrcat,relName);
	attrcat_attrcat_relName.attrType = STRING;
	attrcat_attrcat_relName.attrLength = MAXNAME+1;
	attrcat_attrcat_relName.indexNo = -1;
	
//2 ème attribut
	attrcat attrcat_attrcat_attrName;
	strcpy(attrcat_attrcat_attrName.relName,"attrcat");
	strcpy(attrcat_attrcat_attrName.attrName,"attrName");
	attrcat_attrcat_attrName.offset = (int)offsetof(attrcat,attrName);
	attrcat_attrcat_attrName.attrType = STRING;
	attrcat_attrcat_attrName.attrLength = MAXNAME+1;
	attrcat_attrcat_attrName.indexNo = -1;	
//3 ème attribut
	attrcat attrcat_attrcat_offset;
	strcpy(attrcat_attrcat_offset.relName,"attrcat");
	strcpy(attrcat_attrcat_offset.attrName,"offset");
	attrcat_attrcat_offset.offset = (int)offsetof(attrcat,offset);
	attrcat_attrcat_offset.attrType = INT;
	attrcat_attrcat_offset.attrLength = 4;
	attrcat_attrcat_offset.indexNo = -1;		
//4 ème attribut
	attrcat attrcat_attrcat_attrType;
	strcpy(attrcat_attrcat_attrType.relName,"attrcat");
	strcpy(attrcat_attrcat_attrType.attrName,"attrType");
	attrcat_attrcat_attrType.offset = (int)offsetof(attrcat,attrType);
	attrcat_attrcat_attrType.attrType = INT;
	attrcat_attrcat_attrType.attrLength = sizeof(AttrType);
	attrcat_attrcat_attrType.indexNo = -1;		
//5 ème attribut
	attrcat attrcat_attrcat_attrLength;
	strcpy(attrcat_attrcat_attrLength.relName,"attrcat");
	strcpy(attrcat_attrcat_attrLength.attrName,"attrLength");
	attrcat_attrcat_attrLength.offset = (int)offsetof(attrcat,attrLength);
	attrcat_attrcat_attrLength.attrType = INT;
	attrcat_attrcat_attrLength.attrLength = 4;
	attrcat_attrcat_attrLength.indexNo = -1;			
//6 ème attribut
	attrcat attrcat_attrcat_indexNo;
	strcpy(attrcat_attrcat_indexNo.relName,"attrcat");
	strcpy(attrcat_attrcat_indexNo.attrName,"indexNo");
	attrcat_attrcat_indexNo.offset = (int)offsetof(attrcat,indexNo);
	attrcat_attrcat_indexNo.attrType = INT;
	attrcat_attrcat_indexNo.attrLength = 4;
	attrcat_attrcat_indexNo.indexNo = -1;		
	
//On insère l'ensemble des tuples dans le catalogue attrcat

	rc = attrcatFH.InsertRec((char *)&attrcat_relcat_relName, rid);
		if(rc != 0)
                PrintError(rc);
                
 	rc = attrcatFH.InsertRec((char *)&attrcat_relcat_tupleLength, rid);
		if(rc != 0)
                PrintError(rc);
                
 	rc = attrcatFH.InsertRec((char *)&attrcat_relcat_attrCount, rid);
		if(rc != 0)
                PrintError(rc);         


	rc = attrcatFH.InsertRec((char *)&attrcat_attrcat_relName, rid);
		if(rc != 0)
                PrintError(rc);
	rc = attrcatFH.InsertRec((char *)&attrcat_attrcat_attrName, rid);
		if(rc != 0)
                PrintError(rc);
	rc = attrcatFH.InsertRec((char *)&attrcat_attrcat_offset, rid);
		if(rc != 0)
                PrintError(rc);
	rc = attrcatFH.InsertRec((char *)&attrcat_attrcat_attrType, rid);
		if(rc != 0)
                PrintError(rc);
	rc = attrcatFH.InsertRec((char *)&attrcat_attrcat_attrLength, rid);
		if(rc != 0)
                PrintError(rc);
	rc = attrcatFH.InsertRec((char *)&attrcat_attrcat_indexNo, rid);
		if(rc != 0)
                PrintError(rc);

      
                               
        	
//Pour finir il faut fermer le second fichier
	rc = rmm.CloseFile(attrcatFH);
		if(rc != 0)
                PrintError(rc);

    return(0);
}
