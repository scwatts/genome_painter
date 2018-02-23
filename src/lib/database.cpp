#include "database.h"


namespace db {


sqlite3 *open(const char database_fp[]) {
    sqlite3 *dbp;
    if (sqlite3_open(database_fp, &dbp) != SQLITE_OK) {
        fprintf(stderr, "Can't open database %s: %s\n", database_fp, sqlite3_errmsg(dbp));
        sqlite3_close(dbp);
        exit(1);
    }
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


void bind_int64(sqlite3_stmt *stmt, int index, common::ullong data, sqlite3 *db) {
    if (SQLITE_OK != sqlite3_bind_int64(stmt, index, data)) {
        fprintf(stderr, "Error binding value in insert: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}


// TODO: can we template this? relation must be made to correct sqlite bind function...
void bind_double(sqlite3_stmt *stmt, int index, double data, sqlite3 *db) {
    if (SQLITE_OK != sqlite3_bind_double(stmt, index, data)) {
        fprintf(stderr, "Error binding value in insert: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}


void step(sqlite3_stmt *stmt, sqlite3 *db) {
    if (SQLITE_DONE != sqlite3_step(stmt)) {
        fprintf(stderr, "Error evaluating statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}


int vector_fill(void *vector_void, int number_fields, char *fields[], char *column_names[]) {
    std::vector<float> *vector = static_cast<std::vector<float> *> vector_void;
    vector->emplace_back(fields, fields + number_fields);
    return 0;
}



} // namespace db
