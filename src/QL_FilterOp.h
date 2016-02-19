#include "ql.h"
class QL_FilterOp : public virtual QL_Operator{

public:
    QL_FilterOp(SM_Manager &smm, QL_Operator &op, const Condition cond);
    virtual ~QL_FilterOp();
     RC Open ();
     RC GetNext(RM_Record &rec);
     RC Close();

int offset;
Condition cond;
SM_Manager *smm;
QL_Operator *op;

};
