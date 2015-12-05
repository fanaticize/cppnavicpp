#include "SubwayCommonDecl.h"
#include "Line.h"
#include "Station.h"
#include "Edge.h"
#include "PathNode.h"
#include "SQLiteHandler.h"
#include "PathHandler.h"

int main() {
	SQLiteHandler* sqliteHandler = new SQLiteHandler();
	sqliteHandler->openDB("subwayDB.db");
	PathHandler *pc = new PathHandler();
	pc->setLineList(sqliteHandler);
	sqliteHandler->closeDB();
	cout << "로딩완료" << endl;
	cout << endl;
	
	string order = "0";
	int intorder = 0;
	string startStation="";
	string centerStation = "";
	string endStation = "";
	bool isExistStation;
	bool flag = true;
	while (flag) {
		cout << "명령을 선택해주세요(1: 경로검색, 2: 프로그램종료):";
		cin >> order;
		if(order != "1" && order != "2"){
			cout << "올바른 명령이 아닙니다." << endl;
			order = "0";
			continue;
		}
		intorder = atoi(order.c_str());
		switch (intorder) {
		case FIND_PATH:
			while (true) {
				cout << "출발역을 입력해주세요(ex: 수원): ";
				cin >> startStation;
				isExistStation = pc->isExistStation(startStation);				
				if (isExistStation)
					break;
				cout << "해당역은 존재하지 않습니다." << endl;
			}
			while (true) {
				cout << "도착역을 입력해주세요: ";
				cin >> endStation;
				isExistStation = pc->isExistStation(endStation);
				if (isExistStation)
					break;
				cout << "해당역은 존재하지 않습니다." << endl;
			}
			while (true) {
				cout << "경유역을 입력해주세요(없으면 x 입력):";
				cin >> centerStation;
				if (centerStation == "x")
					break;
				isExistStation = pc->isExistStation(centerStation);
				if (isExistStation)
					break;
				cout << "해당역은 존재하지 않습니다." << endl;
			}
			cout << endl;
			if (centerStation == "x")
				pc->findShortestPath(startStation, endStation);
			else
				pc->findShortestPath(startStation, endStation, centerStation);
			cout << endl;
			startStation = "";
			centerStation = "";
			endStation = "";
			break;
		case EXIT:
			flag = false;
		}
	}

	delete(sqliteHandler);
	delete(pc);

	return 0;
}