// SQLiteTest.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//
#include <iostream>
#include <string>
#include "sqlite3.h"


static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for( i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
};

int main()
{
	std::cout<<"!!"<<std::endl;
    sqlite3 *pSQLite3 = NULL; // SQLite DB ��ü ���� ����
	sqlite3_stmt * pStmt = NULL;
    char    *szErrMsg = NULL; // Error �߻��� �޼����� �����ϴ� ����

    // �����ͺ��̽� ���� : ������ �������� ������ �����Ѵ�.
    int rst = sqlite3_open( "test.db", &pSQLite3);

    if ( rst )
    {
        printf( "Can't open database: %s\n", sqlite3_errmsg( pSQLite3 ));

        sqlite3_close( pSQLite3 );
        pSQLite3 = NULL;
    }
    else
    {
        printf("Database opened!!\n");

        // ���̺� ����
       /* rst = sqlite3_exec(pSQLite3, 
            "CREATE TABLE member ( name TEXT(20), age INTEGER )",
            callback, 0, &szErrMsg);
		std::cout<<"!!"<<std::endl;*/
		/*rst = sqlite3_exec(pSQLite3, 
            "DELETE FROM member WHERE name='andy'", 
            callback, 0, &szErrMsg);
        // ������ ����
        rst = sqlite3_exec(pSQLite3, 
            "INSERT INTO member ( name, age ) values ( 'andy', 20 )", 
            callback, 0, &szErrMsg);
		rst = sqlite3_exec(pSQLite3, 
            "INSERT INTO member ( name, age ) values ( 'andy1', 20 )", 
            callback, 0, &szErrMsg);
		rst = sqlite3_exec(pSQLite3, 
            "INSERT INTO member ( name, age ) values ( 'andy3', 30 )", 
            callback, 0, &szErrMsg);
		std::cout<<"!!"<<std::endl;*/
        // ������ ����
        /*rst = sqlite3_exec(pSQLite3, 
            "SELECT * FROM member", 
            callback, 0, &szErrMsg);*/

		char* sql = "SELECT * FROM STATION WHERE NAME=? ";
		sqlite3_prepare(pSQLite3, sql, strlen(sql), &pStmt, NULL);
		sqlite3_reset(pStmt);
		const char* sn = "����";
		sqlite3_bind_text(pStmt, 1, sn, strlen(sn), SQLITE_STATIC);
		while(sqlite3_step(pStmt)==SQLITE_ROW){
			const unsigned char* name = sqlite3_column_text(pStmt, 1);
			int seq = sqlite3_column_int(pStmt, 0);
			const unsigned char* linename = sqlite3_column_text(pStmt, 2);
			std::cout<<seq<<" "<<name<<" "<<linename<<std::endl;

		}
		/*
		rst = sqlite3_exec(pSQLite3, 
            "DROp TABLE member",
            callback, 0, &szErrMsg);
		std::cout<<"!!"<<std::endl;*/
    }

    //��ü ����
    sqlite3_free( szErrMsg );
    sqlite3_close( pSQLite3 ); 

	return 0;
}
