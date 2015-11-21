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

		//ȣ���� �ٲ�� ���ο� ȣ�� push
		if (lineName != lastLineName) {
			tempLine = new Line();
			tempLine->name = lineName;
			lastLineName = lineName;
			(*lineList).push_back(tempLine);
		}

		//���� �ٲ�� ���ο� �� push
		if (stationName != lastStationName) {
			tempStation = new Station();
			tempStation->name = stationName;
			lastStationName = stationName;
			tempLine->stationList.push_back(tempStation);
		}

		//���� push
		tempEdge = new Edge();
		tempEdge->fromStationName = stationName;
		tempEdge->toStationName = nextStation;
		tempStation->edgeList.push_back(tempEdge);
	}

	//������ from �� to �κ� stationlist���� ã�Ƽ� ����
	
	//line �κ� ��ȯ
	for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
		tempLine = (*IterPos0);

		//staion�κ� ��ȯ
		for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
		{
			tempStation = (*IterPos1);

			//edge�κм�ȯ
			for (vector< Edge* >::iterator IterPos2 = (*IterPos1)->edgeList.begin(); IterPos2 != (*IterPos1)->edgeList.end(); ++IterPos2)
			{	
				//������ station�� from���� ����
				(*IterPos2)->from = tempStation;

				//toStation�� ã������ �ش�line�� station ��ȯ
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

	//��� ���
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
	sqlite3 *pSQLite3 = NULL; // SQLite DB ��ü ���� ����
	char    *szErrMsg = NULL; // Error �߻��� �޼����� �����ϴ� ����							  
	int rst = sqlite3_open("test2.db", &pSQLite3);// �����ͺ��̽� ���� : ������ �������� ������ �����Ѵ�.
	ppSqlite = pSQLite3;

	vector<Line*> lineList;
	
	//��ȣ���� ���� ���� ����()
	setLineList(&lineList);

	//��� ���
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