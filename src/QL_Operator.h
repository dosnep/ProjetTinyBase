#include "ql.h"

class QL_Operator {
public:
    QL_Operator(){};
    virtual ~QL_Operator(){};
    virtual RC Open () = 0;
    virtual RC GetNext(RM_Record &rec) = 0;
    virtual RC Close() = 0;

private:

};
