#include "NoSQLDatabase.h"
#include "BTree.h"

int main() {

    // Create an instance of NoSQLDatabase
    // TODO: value is the blocking factor
    NoSQLDatabase db;

    // Run the command-line interface
    db.runCLI();

    return 0;
}
