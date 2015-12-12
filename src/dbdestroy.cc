#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "rm.h"
#include "pf.h"
#include "sm.h"
#include "tinybase.h"
#include "stddef.h"

using namespace std;

//
// main
//
int main(int argc, char *argv[])
{
    char *dbname;
    char command[255] = "rm -r ";
    RID rid;

    // Look for 2 arguments. The first is always the name of the program
    // that was executed, and the second should be the name of the
    // database.
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " dbname \n";
        exit(1);
    }

    // The database name is the second argument
    dbname = argv[1];

    // Create a subdirectory for the database
    system (strcat(command,dbname));
    exit(0);
}
