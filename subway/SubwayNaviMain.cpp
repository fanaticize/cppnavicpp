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
	cout << "�ε��Ϸ�" << endl;
	cout << endl;
	
	string order = "0";
	int intorder = 0;
	string startStation="";
	string centerStation = "";
	string endStation = "";
	bool isExistStation;
	bool flag = true;
	while (flag) {
		cout << "����� �������ּ���(1: ��ΰ˻�, 2: ���α׷�����):";
		cin >> order;
		if(order != "1" && order != "2"){
			cout << "�ùٸ� ����� �ƴմϴ�." << endl;
			order = "0";
			continue;
		}
		intorder = atoi(order.c_str());
		switch (intorder) {
		case FIND_PATH:
			while (true) {
				cout << "��߿��� �Է����ּ���(ex: ����): ";
				cin >> startStation;
				isExistStation = pc->isExistStation(startStation);				
				if (isExistStation)
					break;
				cout << "�ش翪�� �������� �ʽ��ϴ�." << endl;
			}
			while (true) {
				cout << "�������� �Է����ּ���: ";
				cin >> endStation;
				isExistStation = pc->isExistStation(endStation);
				if (isExistStation)
					break;
				cout << "�ش翪�� �������� �ʽ��ϴ�." << endl;
			}
			while (true) {
				cout << "�������� �Է����ּ���(������ x �Է�):";
				cin >> centerStation;
				if (centerStation == "x")
					break;
				isExistStation = pc->isExistStation(centerStation);
				if (isExistStation)
					break;
				cout << "�ش翪�� �������� �ʽ��ϴ�." << endl;
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