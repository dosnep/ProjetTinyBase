#include "ql.h"
class QL_ProjectOp : public virtual QL_Operator{

public:
    QL_ProjectOp(SM_Manager &smm, QL_Operator &op,int nbSelect, const RelAttr selAttrs[]);
    virtual ~QL_ProjectOp();
     RC Open ();
     RC GetNext(RM_Record &rec);
     RC Close();
     
DataAttrInfo *attributes;
SM_Manager *smm;
QL_Operator *op;

};
