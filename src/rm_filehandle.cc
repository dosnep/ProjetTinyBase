#include "rm.h"
#include <string>

RM_FileHandle :: RM_FileHandle()
{
	this->viableFile = false;
	this->pf = NULL;
	
};

RM_FileHandle :: ~RM_FileHandle()
{
};

RC RM_FileHandle :: SetViableFile(const bool &cond)
{
	this->viableFile = cond;
	return 0;
};

RC RM_FileHandle :: SetPf(const PF_FileHandle *pf)
{
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;
		
	*this->pf = *pf;
	return 0;
};

RC RM_FileHandle :: SetFh(const rm_FileHeader &fh)
{
	if(!this->viableFile)
		return RM_FILEHANDLE_NOT_VIABLE;

	this->fh.recordSize = fh.recordSize;
	this->fh.nbRecordsPerPage = fh.nbRecordsPerPage;
	this->fh.NextFreePage = fh.NextFreePage;
	return 0;
};

