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
int tailleCle;	//Taille d'une clé
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
    
	//Pointe vers la cle à la position pos
	void GetCle(const int pos, char *&pData);
	
	//Pointe vers le ptr avant la cle
    void GetPtrInf(const int pos, char *&pData);

	//Pointe vers le ptr après la cle
    void GetPtrSup(const int pos, char *&pData);
    
    //Pour chaque fils de noeud, son parent sera noeud
    RC ModifParent(PageNum noeud);
    
    //Insère une clé dans un noeud non vide
    RC InsertKey(PageNum noeud, char *key, char *&pDataPtr);

	//Ajoute une clé dans un noeud vide
	RC InsertKeyEmptyNode(const PageNum racine, char *key, char *&ptrAvant, char *&ptrApres);
	
	//extrait la clé du milieu d'un noeud,modifie le fils gauche, modifie le fils droit
	RC ExtractKey(const PageNum noeud, char* &key, const PageNum splitNoeud);

	//Insert une clé dans une feuille sans éclatement
	RC InsertEntryToLeafNodeNoSplit(PageNum noeud, char *key);
	
	//Insert une clé dans une feuille avec éclatement
	RC InsertEntryToLeafNodeSplit(PageNum noeud, char *key);

	//Insert une clé dans une feuille
	RC InsertEntryToLeaf(PageNum noeud, char *key);

	//Insert une clé dans un parent sans éclatement
	RC InsertEntryToIntNodeNoSplit(PageNum noeud, char *key, PageNum splitNoeud);

	//Insert une clé dans un parent avec éclatement
	RC InsertEntryToIntNodeSplit(PageNum noeud, char *key,PageNum splitNoeud);

	//Insert une clé dans un noeud interne
	RC InsertEntryToIntNode(PageNum noeud, char *key, PageNum splitNoeud);	

	//Insert une clé dans un noeud
	RC InsertEntryToNode(PageNum noeud, char *key, int hauteur);	

	//Va chercher le sous arbre dans lequel nous allons insérer notre clé
	RC InsertEntryToInt1Node(PageNum noeud, char *key, int hauteur);	






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

#define IX_InsertKey    (START_IX_WARN + 0) //Clé non ajouté dans un noeud
#define IX_InsertLeafNoSplit    (START_IX_WARN + 1) //Clé non ajouté dans une feuille sans split



#endif
