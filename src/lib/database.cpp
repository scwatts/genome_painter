#include "database.h"


namespace db {


sqlite3 *open_and_configure(const char database_fp[]) {
    // Open
    sqlite3 *dbp;
    if (sqlite3_open(database_fp, &dbp) != SQLITE_OK) {
        fprintf(stderr, "Can't open database %s: %s\n", database_fp, sqlite3_errmsg(dbp));
        sqlite3_close(dbp);
        exit(1);
    }

    // Configure
    execute(dbp, "PRAGMA main.page_size=4096");
    execute(dbp, "PRAGMA main.cache_size=10000");
    execute(dbp, "PRAGMA main.locking_mode=EXCLUSIVE");
    execute(dbp, "PRAGMA main.synchronous=OFF");
    execute(dbp, "PRAGMA main.journal_mode=OFF");
    execute(dbp, "PRAGMA main.cache_size=5000");

    return dbp;
}


void execute(sqlite3 *dbp, const char statement[]) {
    char *errmsg;
    if (sqlite3_exec(dbp, statement, NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Could not execute statment: %s\n", errmsg);
        sqlite3_close(dbp);
        exit(1);
    }
}


void start_transaction(sqlite3 *dbp) {
    db::execute(dbp, "BEGIN TRANSACTION");
}


void end_transaction(sqlite3 *dbp) {
    db::execute(dbp, "END TRANSACTION");
}

sqlite3_stmt *prepare_statement(const char statement[], sqlite3 *dbp) {
    unsigned int buffer_size = 256;
    char zSQL[buffer_size] = "\0";
	char *ztail = 0;
    sqlite3_stmt *stmt;
    sprintf(zSQL, statement);
    if (sqlite3_prepare_v2(dbp, zSQL, buffer_size, &stmt, (const char **)&ztail) != SQLITE_OK) {
        fprintf(stderr, "Unable to prepare SQL statement: %s\n", sqlite3_errmsg(dbp));
        sqlite3_close(dbp);
        exit(1);
    }
    return stmt;
}


void bind_int64(sqlite3_stmt *stmt, int index, common::ullong data, sqlite3 *dbp) {
    if (SQLITE_OK != sqlite3_bind_int64(stmt, index, data)) {
        fprintf(stderr, "Error binding value in insert: %s\n", sqlite3_errmsg(dbp));
        sqlite3_close(dbp);
        exit(1);
    }
}


// TODO: can we template this? relation must be made to correct sqlite bind function...
void bind_double(sqlite3_stmt *stmt, int index, double data, sqlite3 *dbp) {
    if (SQLITE_OK != sqlite3_bind_double(stmt, index, data)) {
        fprintf(stderr, "Error binding value in insert: %s\n", sqlite3_errmsg(dbp));
        sqlite3_close(dbp);
        exit(1);
    }
}


void step(sqlite3_stmt *stmt, sqlite3 *dbp) {
    if (SQLITE_DONE != sqlite3_step(stmt)) {
        fprintf(stderr, "Error evaluating statement: %s\n", sqlite3_errmsg(dbp));
        sqlite3_close(dbp);
        exit(1);
    }
}


int vector_fill_probabilities(void *vector_void, int number_fields, char *fields[], char *column_names[]) {
    std::vector<float> *vector = static_cast<std::vector<float> *>(vector_void);
    for(int i = 1; i < number_fields; i++) {
        vector->push_back(std::stof(fields[i]));
    }
    return 0;
}


int vector_fill_species(void *vector_void, int number_fields, char *fields[], char *column_names[]) {
    std::vector<std::string> *vector = static_cast<std::vector<std::string> *>(vector_void);
    vector->push_back(std::string(fields[0]));
    return 0;
}



} // namespace db
