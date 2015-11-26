//
// File:        ix_error.cc
// Description: ix_PrintError function


#include <cerrno>
#include <cstdio>
#include <iostream>
#include "pf_internal.h"

using namespace std;

//
// Error table
//

static char *IX_ErrorMsg[] = {

(char*)"Clé non ajouté dans le noeud non vide",
(char*)"Clé non ajouté dans la feuille sans split"

};

//
// PF_PrintError
//
// Desc: Send a message corresponding to a PF return code to cerr
//       Assumes PF_UNIX is last valid PF return code
// In:   rc - return code for which a message is desired
//
void IX_PrintError(RC rc)
{
  // Check the return code is within proper limits
    // Print error
    cerr << "IX_Error: " << IX_ErrorMsg[rc - START_IX_ERR] << "\n";
}
