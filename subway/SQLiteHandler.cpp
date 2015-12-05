#include "SQLiteHandler.h"
#include <string>

bool SQLiteHandler::openDB(char* dbFileName) {
	int rc = sqlite3_open(dbFileName, &db);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}
	return true;
}
void SQLiteHandler::select(char* sql) {
	pStmt = NULL;
	sqlite3_prepare_v2(db, sql, strlen(sql), &pStmt, NULL);
	sqlite3_reset(pStmt);
}
void SQLiteHandler::finalizeStatement() {
	sqlite3_finalize(pStmt);
}
bool SQLiteHandler::selectNextStep() {
	return sqlite3_step(pStmt) == SQLITE_ROW;
}
char* SQLiteHandler::getTextFromRst(int i) {
	return (char *)sqlite3_column_text(pStmt, i);
}
int SQLiteHandler::getIntFromRst(int i) {
	return sqlite3_column_int(pStmt, i);
}
char* SQLiteHandler::closeDB() {
	char *szErrMsg = NULL;
	sqlite3_free(szErrMsg);	
	sqlite3_close(db);
	return szErrMsg;
}