//
// ix.h
//
//   Index Manager Component Interface
//

#ifndef IX_H
#define IX_H

// Please do not include any other files than the ones below in this file.

#include "tinybase.h"  // Please don't change these lines
#include "rm_rid.h"  // Please don't change these lines
#include "pf.h"

//Header de l'index
typedef struct fileheader ix_FileHeader;
struct fileheader{
	
AttrType attrType; //Type de l'attribut indexé
int tailleCle;	//Taille d'un clé
int taillePtr;	//Taille d'un pointeur
PageNum racine; //racine de l'arbre b-tree
int hauteur; //hauteur de l'abre b-tree
int nbPointeurMax;	//Nombre de pointeurs dans un noeud (ce qui veut dire qu'il y a nbPointeursMax-1 clés)

};

//Header d'un noeud de l'abre b-tree
typedef struct noeudheader ix_NoeudHeader;
struct noeudheader{
	
int nbCleCrt;	//Nombre courant de clé dans le noeud (ce qui veut dire qu'il y a nbclecrt+1 pointeurs)
PageNum mother;	//Noeud parent du noeud courant
int niveau;		//si niveau = 0, nous sommes à la racine, sinon si niveau = h-1 nous sommes sur une feuille

};


//
// IX_IndexHandle: IX Index File interface
//
class IX_IndexHandle {
public:
    IX_IndexHandle();
    ~IX_IndexHandle();

    // Insert a new index entry
    RC InsertEntry(void *pData, const RID &rid);

    // Delete a new index entry
    RC DeleteEntry(void *pData, const RID &rid);

    // Force index files to disk
    RC ForcePages();

PF_FileHandle *pf;
bool viableFile; //bool qui teste si le fichier a été ouvert
ix_FileHeader fh;	//FileHeader propre au fichier chargé

    
};

//
// IX_IndexScan: condition-based scan of index entries
//
class IX_IndexScan {
public:
    IX_IndexScan();
    ~IX_IndexScan();

    // Open index scan
    RC OpenScan(const IX_IndexHandle &indexHandle,
                CompOp compOp,
                void *value,
                ClientHint  pinHint = NO_HINT);

    // Get the next matching entry return IX_EOF if no more matching
    // entries.
    RC GetNextEntry(RID &rid);

    // Close index scan
    RC CloseScan();
};

//
// IX_Manager: provides IX index file management
//
class IX_Manager {
public:
    IX_Manager(PF_Manager &pfm);
    ~IX_Manager();

    // Create a new Index
    RC CreateIndex(const char *fileName, int indexNo,
                   AttrType attrType, int attrLength);

    // Destroy and Index
    RC DestroyIndex(const char *fileName, int indexNo);

    // Open an Index
    RC OpenIndex(const char *fileName, int indexNo,
                 IX_IndexHandle &indexHandle);

    // Close an Index
    RC CloseIndex(IX_IndexHandle &indexHandle);
    
    
private:
PF_Manager& pfm;
};

//
// Print-error function
//
void IX_PrintError(RC rc);

#endif