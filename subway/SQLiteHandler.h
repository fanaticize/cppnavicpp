#ifndef __SQLITE_HANDLER_H__
#define __SQLITE_HANDLER_H__

#include "sqlite3.h"

class SQLiteHandler {
private:
	sqlite3* db = NULL;
	sqlite3_stmt* pStmt = NULL;
public:
	bool openDB(char* dbFileName);
	void select(char* sql);
	void finalizeStatement();
	bool selectNextStep();
	char* getTextFromRst(int i);
	int getIntFromRst(int i);
	char* closeDB();
};

#endif