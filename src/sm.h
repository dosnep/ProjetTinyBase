//
// sm.h
//   Data Manager Component Interface
//

#ifndef SM_H
#define SM_H

// Please do not include any other files than the ones below in this file.

#include <stdlib.h>
#include <string.h>
#include "tinybase.h"  // Please don't change these lines
#include "parser.h"
#include "rm.h"
#include "ix.h"
#include "printer.h"

typedef struct relcat relcat;
struct relcat{

char relName[MAXNAME+1]; //Nom de la relation
int tupleLength;	//Taille du tuple (en Bytes)
int attrCount;	//Nb d'attributs dans le tuple
	
};

typedef struct attrcat attrcat;
struct attrcat{
	
char relName[MAXNAME+1];	//Nom de la relation où se situe l'attribut
char attrName[MAXNAME+1];	//Nom de l'attribut
int offset;	//Décalage de l'attribut par rapport au début du tuple (en Bytes)
AttrType attrType;	//Type de l'attribut (String, int ou float)
int attrLength;	//Taille de l'attribut
int indexNo;	//Numéro de l'index de l'attribut

	
};



//
// SM_Manager: provides data management
//
class SM_Manager {
    friend class QL_Manager;
    friend class QL_ProjectOp;
    friend class QL_FilterOp;
public:
    SM_Manager    (IX_Manager &ixm, RM_Manager &rmm);
    ~SM_Manager   ();                             // Destructor

    RC OpenDb     (const char *dbName);           // Open the database
    RC CloseDb    ();                             // close the database

    RC CreateTable(const char *relName,           // create relation relName
                   int        attrCount,          //   number of attributes
                   AttrInfo   *attributes);       //   attribute data
    RC CreateIndex(const char *relName,           // create an index for
                   const char *attrName);         //   relName.attrName
    RC DropTable  (const char *relName);          // destroy a relation

    RC DropIndex  (const char *relName,           // destroy index on
                   const char *attrName);         //   relName.attrName
    RC Load       (const char *relName,           // load relName from
                   const char *fileName);         //   fileName
    RC Help       ();                             // Print relations in db
    RC Help       (const char *relName);          // print schema of relName

    RC Print      (const char *relName);          // print relName contents

    RC Set        (const char *paramName,         // set parameter to
                   const char *value);            //   value

private:
IX_Manager &ixm;
RM_Manager &rmm;
RM_FileHandle relcatFH;
RM_FileHandle attrcatFH;
};

//
// Print-error function
//
void SM_PrintError(RC rc);

#endif
