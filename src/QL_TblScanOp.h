#include "ql.h"
class QL_TblScanOp : public virtual QL_Operator{

public:
    QL_TblScanOp(RM_Manager &rmm, SM_Manager &smm, char *relName);
    virtual ~QL_TblScanOp();
     RC Open ();
     RC GetNext(RM_Record &rec);
     RC Close();
private:
RM_Manager *rmm;
SM_Manager *smm;
RM_FileScan fs;
RM_FileHandle fh;

};
