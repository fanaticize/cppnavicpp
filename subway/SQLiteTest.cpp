#include <iostream>
#include <string>
#include <vector>
#include "sqlite3.h"

using namespace std;

class Edge;
class Station;
class Line;

sqlite3 *ppSqlite = NULL;

class Station {
public:
	string name;
	vector<Edge*> edgeList;
	Station() {
		name = "";
	}
};

class Edge {
public:
	int weight;
	string fromStationName;
	string toStationName;
	Station* from;
	Station* to;
	Edge() {
		fromStationName = "";
		toStationName = "";
		from = NULL;
		to = NULL;
	}
};

class Line {
public:
	string name;
	vector<Station*> stationList;
	Line() {
		name = "";
	}
	void setLineName(const char *a) {
		name = a;
	}
};

void setLineList(vector<Line*> *lineList) {
	sqlite3_stmt * pStmt = NULL;
	char* sql = "select line, current_station, next_station from time_table group by line, current_station, next_station order by line, current_station, next_station";
	sqlite3_prepare(ppSqlite, sql, strlen(sql), &pStmt, NULL);
	sqlite3_reset(pStmt);
	Line *tempLine;
	Station *tempStation;
	Edge *tempEdge;
	string lastLineName = "";
	string lineName = "";
	string lastStationName = "";
	string stationName = "";
	string nextStation = "";
	bool flag = true;
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		lineName = (char *)sqlite3_column_text(pStmt, 0);
		stationName = (char *)sqlite3_column_text(pStmt, 1);
		nextStation = (char *)sqlite3_column_text(pStmt, 2);

		//호선이 바뀌면 새로운 호선 push
		if (lineName != lastLineName) {
			tempLine = new Line();
			tempLine->name = lineName;
			lastLineName = lineName;
			(*lineList).push_back(tempLine);
		}

		//역이 바뀌면 새로운 역 push
		if (stationName != lastStationName) {
			tempStation = new Station();
			tempStation->name = stationName;
			lastStationName = stationName;
			tempLine->stationList.push_back(tempStation);
		}

		//간선 push
		tempEdge = new Edge();
		tempEdge->fromStationName = stationName;
		tempEdge->toStationName = nextStation;
		tempStation->edgeList.push_back(tempEdge);
	}

	//간선의 from 과 to 부분 stationlist에서 찾아서 연결
	
	//line 부분 순환
	for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
		tempLine = (*IterPos0);

		//staion부분 순환
		for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
		{
			tempStation = (*IterPos1);

			//edge부분순환
			for (vector< Edge* >::iterator IterPos2 = (*IterPos1)->edgeList.begin(); IterPos2 != (*IterPos1)->edgeList.end(); ++IterPos2)
			{	
				//현재의 station을 from으로 지정
				(*IterPos2)->from = tempStation;

				//toStation을 찾기위한 해당line의 station 순환
				for (vector< Station* >::iterator IterPos3 = tempLine->stationList.begin(); IterPos3 != tempLine->stationList.end(); ++IterPos3)
				{
					if ((*IterPos2)->toStationName == (*IterPos3)->name) {
						(*IterPos2)->to = (*IterPos3);
						break;
					}
				}

			}
		}
	}

	//결과 출력
	/*for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
		tempLine = (*IterPos0);
		cout << tempLine->name << endl;
		for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
		{
			cout << (*IterPos1)->name << ": ";
			for (vector< Edge* >::iterator IterPos2 = (*IterPos1)->edgeList.begin(); IterPos2 != (*IterPos1)->edgeList.end(); ++IterPos2)
			{
				cout << "(" << (*IterPos2)->from->name << ",";
				if ((*IterPos2)->to != NULL)
					cout << (*IterPos2)->to->name;
				cout << ") ";
			}
			cout << endl;
		}
	}*/
}

int main() {
	std::cout << "!!" << std::endl;
	sqlite3 *pSQLite3 = NULL; // SQLite DB 객체 저장 변수
	char    *szErrMsg = NULL; // Error 발생시 메세지를 저장하는 변수							  
	int rst = sqlite3_open("test2.db", &pSQLite3);// 데이터베이스 열기 : 파일이 존재하지 않으면 생성한다.
	ppSqlite = pSQLite3;

	vector<Line*> lineList;
	
	//각호선별 역과 간선 설정()
	setLineList(&lineList);

	//결과 출력
	Line *tempLine;
	Station *tempStation;
	Edge *tempEdge;
	for (vector< Line* >::iterator IterPos0 = lineList.begin(); IterPos0 != lineList.end(); ++IterPos0) {
		tempLine = (*IterPos0);
		cout << tempLine->name << endl;
		for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
		{
			cout << (*IterPos1)->name << ": ";
			for (vector< Edge* >::iterator IterPos2 = (*IterPos1)->edgeList.begin(); IterPos2 != (*IterPos1)->edgeList.end(); ++IterPos2)
			{
				cout << "(" << (*IterPos2)->from->name << ",";
				if ((*IterPos2)->to != NULL)
					cout << (*IterPos2)->to->name;
				cout << ") ";
			}
			cout << endl;
		}
	}

	sqlite3_free(szErrMsg);
	sqlite3_close(pSQLite3);
	return 0;
}