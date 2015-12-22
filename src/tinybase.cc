//
// redbase.cc
//
// Author: Jason McHugh (mchughj@cs.stanford.edu)
//
// This shell is provided for the student.

#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "tinybase.h"
#include "rm.h"
#include "sm.h"

using namespace std;

//
// main
//


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
    RC rc;

    // Look for 2 arguments.  The first is always the name of the program
    // that was executed, and the second should be the name of the
    // database.
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " dbname \n";
        exit(1);
    }

      // initialize TinyBase components
      PF_Manager pfm;
      RM_Manager rmm(pfm);
      IX_Manager ixm(pfm);
      SM_Manager smm(ixm, rmm);
      //QL_Manager qlm(smm, ixm, rmm);
      // open the database
      if ((rc = smm.OpenDb(argv[1])))
                PrintError(rc);
		smm.Help();
		smm.Print("relcat");
		smm.Help("attrcat");
      // call the parser
      //RBparse(pfm, smm, qlm);
      // close the database
      if ((rc = smm.CloseDb()))
                PrintError(rc);


    cout << "Bye.\n";
exit(0);
}
