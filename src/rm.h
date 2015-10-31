//
// rm.h
//
//   Record Manager component interface
//
// This file does not include the interface for the RID class.  This is
// found in rm_rid.h
//

#ifndef RM_H
#define RM_H

// Please DO NOT include any files other than redbase.h and pf.h in this
// file.  When you submit your code, the test program will be compiled
// with your rm.h and your redbase.h, along with the standard pf.h that
// was given to you.  Your rm.h, your redbase.h, and the standard pf.h
// should therefore be self-contained (i.e., should not depend upon
// declarations in any other file).

// Do not change the following includes
#include "tinybase.h"
#include "rm_rid.h"
#include "pf.h"

typedef struct rm_fileheader rm_FileHeader;
struct rm_fileheader{
	int recordSize;	//Taille d'un enregistrement
	int nbRecordsPerPage;	//Nb d'enregistrements par page
	PageNum nextFreePage; //Première page libre du fichier
		
};


//class Bitmap, utile pour donner une vision des slots libres et occupés
class Bitmap{
	
	public:
	Bitmap(int taille);
	~Bitmap();
	bool IsFull(); //Teste si tout les bits sont à 1
	RC GetFirstFree(SlotNum &slotnum) ; //Retourne le premier slot où le bit est à 0
	RC GetNextSlot(const SlotNum &currentSlotNum,SlotNum &NextslotNum); //Retourne le prochaine slot occupé à partir du slot courrant
	RC SetSlot(const SlotNum &slotnum,const int &value);//Initialise le bit de slotnum à value
	
	
	public:
	int *tabBitmap;
	int taille;
	friend class RM_FileHandle;
	
};


typedef struct rm_pageheader rm_PageHeader;
struct rm_pageheader{
	
	PageNum nextFreePage; //Prochaine page libre
	Bitmap *tab;	//Bitmap de la page	
	
};


//
// RM_Record: RM Record interface
//
class RM_Record {
public:
    RM_Record ();
    RM_Record(const PageNum &pageNum, const SlotNum &slotNum, const char* pData, const int &recordSize);
    ~RM_Record();
	
	//getter
    // Return the data corresponding to the record.  Sets *pData to the
    // record contents.
    RC GetData(char *&pData) const;

    // Return the RID associated with the record
    RC GetRid (RID &rid) const;
 
 
private:
	char *pData; //Données de l'enregistrement
	RID *rid; //Coordonnées RID de l'enregistrement
	bool viableRecord; //Test si l'enregistrement a été chargé avec rm_filescan ou rm_filehandle 
	int recordSize;	//Taille de l'enregistrement;
   
friend class RM_FileHandle;
friend class RM_FileScan;
   
};

//
// RM_FileHandle: RM File interface
//
class RM_FileHandle {
public:
    RM_FileHandle ();
	RM_FileHandle(const PF_FileHandle &pf, const rm_FileHeader &fh);
    ~RM_FileHandle();

    // Given a RID, return the record
    RC GetRec     (const RID &rid, RM_Record &rec) const;

    RC InsertRec  (const char *pData, RID &rid);       // Insert a new record

    RC DeleteRec  (const RID &rid);                    // Delete a record
    RC UpdateRec  (const RM_Record &rec);              // Update a record

    // Forces a page (along with any contents stored in this class)
    // from the buffer pool to disk.  Default value forces all pages.
    RC ForcePages (PageNum pageNum = ALL_PAGES);

	RC InsertPageHeader(const PageNum &pagenum, const rm_PageHeader &pageHeader);
	RC GetNextFreePage(PageNum &pageNum); //pageNum prend la première page où il y a des slots de libres
	

private :

PF_FileHandle *pf;
bool viableFile; //bool qui teste si le fichier a été ouvert
rm_FileHeader fh;	//FileHeader propre au fichier chargé

friend class RM_Manager;
friend class RM_FileScan;


};

//
// RM_FileScan: condition-based scan of records in the file
//
class RM_FileScan {
public:
    RM_FileScan  ();
    ~RM_FileScan ();

    RC OpenScan  (const RM_FileHandle &fileHandle,
                  AttrType   attrType,
                  int        attrLength,
                  int        attrOffset,
                  CompOp     compOp,
                  void       *value,
                  ClientHint pinHint = NO_HINT); // Initialize a file scan
    RC GetNextRec(RM_Record &rec);               // Get next matching record
    RC CloseScan ();                             // Close the scan
    bool EstUnBonRecord(char *pData);

private:
RM_FileHandle *rfh;
bool scanOpen;
AttrType   attrType;
int        attrLength;
int        attrOffset;
CompOp     compOp;
int valInt;
float valFloat;
char *valString;
PageNum currentPage;
SlotNum currentSlot;
void *value;
PageNum numLastPage;
};

//
// RM_Manager: provides RM file management
//
class RM_Manager {
public:
    RM_Manager    (PF_Manager &pfm);
    ~RM_Manager   ();

    RC CreateFile (const char *fileName, int recordSize);
    RC DestroyFile(const char *fileName);
    RC OpenFile   (const char *fileName, RM_FileHandle &fileHandle);

    RC CloseFile  (RM_FileHandle &fileHandle);
    
private:
PF_Manager& pfm;
  
};

//
// Print-error function
//
void RM_PrintError(RC rc);

#define RM_RECORD_NOT_VIABLE 2;
#define BITMAP_NO_FREE_SLOT 3;
#define BITMAP_EOF 5;
#define BITMAP_NO_NEXT_SLOT 6;
#define RM_FILEHANDLE_NOT_VIABLE 4;

#endif
