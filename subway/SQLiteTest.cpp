#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <functional>
//#include <algorithm>
#include <queue>
#include "sqlite3.h"

using namespace std;

class Edge;
class Station;
class Line;

sqlite3 *ppSqlite = NULL;

int STATION_CNT = 0;

class Station {
public:
	Line* line;
	string name;
	int stationSeq;
	vector<Edge*> edgeList;
	Station() {
		name = "";
	}
};

class Edge {
public:
	int weight;
	int waittime;
	int distance;
	string fromStationName;
	string toStationName;
	Station* from;
	Station* to;
	bool isTransit;
	Edge() {
		fromStationName = "";
		toStationName = "";
		from = NULL;
		to = NULL;
		int weight = -1;
		int waittime = -1;
		int distance = -1;
		isTransit = false;
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

class Path {
public:
	vector<Station*> stationList;
	vector<Edge*> edgeList;
	int total_weight;
	bool isEnd;
	Path() {
		isEnd = false;
		total_weight = 0;
	}
	void toString() {
		cout << "�� �迭: ";
		for (vector<Station*>::iterator IterPos1 = stationList.begin(); IterPos1 != stationList.end(); ++IterPos1)
		{
			cout << (*IterPos1)->name<<" ";
		}
		cout << endl;
		cout << "���� �迭";
		for (vector<Edge*>::iterator IterPos1 = edgeList.begin(); IterPos1 != edgeList.end(); ++IterPos1)
		{
			cout << "(" << (*IterPos1)->from->line->name<< "/" << (*IterPos1)->fromStationName << ", " << (*IterPos1)->to->line->name << "/" << (*IterPos1)->toStationName << ") ";
		}
		cout << endl;
		cout << "�ɸ��½ð�: " << total_weight << endl;
	}
	void addEdge(Edge* edge) {
		if(stationList.empty())
			stationList.push_back(edge->from);
		stationList.push_back(edge->to);
		edgeList.push_back(edge);
		total_weight += edge->waittime + edge->weight;
	}
	void copyPath(Path* path) {
		for (vector<Station*>::iterator IterPos1 = path->stationList.begin(); IterPos1 != path->stationList.end(); ++IterPos1)
		{
			stationList.push_back(*IterPos1);
		}
		for (vector<Edge*>::iterator IterPos1 = path->edgeList.begin(); IterPos1 != path->edgeList.end(); ++IterPos1)
		{
			edgeList.push_back(*IterPos1);
		}
		total_weight = path->total_weight;
		isEnd = path->isEnd;
	}
	bool checkReturnBack() {
		int cnt=0;
		for (vector<Station*>::iterator IterPos1 = stationList.begin(); IterPos1 != stationList.end(); ++IterPos1)
		{
			if (stationList.back() == (*IterPos1)) {
				cnt++;
			}
		}
		if (cnt > 1) {
			return true;
		}
		else
			return false;
	}
};

class PathNode {
public:
	PathNode* parentsNode;
	Station* data;
	int totaltime=0;
	int totaldistance=0;
	bool operator > (const PathNode& pn) const {
		return totaltime > pn.totaltime;
	}
};

class PathController2 {
public:
	bool* visitList;
	PathNode** shortestTime;
	priority_queue<PathNode*, vector<PathNode*>, greater<PathNode*>> heap;
	void initPath(string startStation, string endStation, vector<Line*> *lineList) {
		visitList = new bool[STATION_CNT];
		shortestTime = new PathNode*[STATION_CNT];
		for (int i = 0; i < STATION_CNT; i++) {
			visitList[i] = false;
			shortestTime = NULL;
		}

		Line *tempLine;
		Station *tempStation;
		Edge* tempEdge;
		PathNode* tempPathNode;
		vector<Station*> startStationList;
		for (vector< Line* >::iterator IterPos0 = (*lineList).begin(); IterPos0 != (*lineList).end(); ++IterPos0) {
			tempLine = (*IterPos0);
			for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
			{
				if ((*IterPos1)->name == startStation) {
					tempStation = (*IterPos1);
					startStationList.push_back(tempStation);
				}
			}
		}
		//cout << tempStation->name << endl;

		//station�κ� ��ȯ
		for (vector< Station* >::iterator IterPos0 = startStationList.begin(); IterPos0 != startStationList.end(); ++IterPos0) {
			tempStation = (*IterPos0);
			visitList[tempStation->stationSeq] = true;
			tempPathNode = new PathNode();
			tempPathNode->parentsNode = NULL;
			tempPathNode->data = tempStation;
			tempPathNode->totaldistance = 0;
			tempPathNode->totaltime = 0;
			shortestTime[tempStation->stationSeq] = tempPathNode;
			heap.push(tempPathNode);
		}

		while (true) {
			if (heap.empty()) {
				break;
			}
			tempPathNode = heap.top();
			heap.pop();
			if (tempPathNode->data->name == endStation) {
				break;
			}
			if (visitList[tempPathNode->data->stationSeq])
				continue;
			else {
				visitList[tempPathNode->data->stationSeq] = true;
				for (vector<Edge* >::iterator IterPos0 = tempPathNode->data->edgeList.begin(); IterPos0 != tempPathNode->data->edgeList.end(); ++IterPos0) {
					tempEdge = (*IterPos0);
					visitList[tempStation->stationSeq] = true;
					tempPathNode = new PathNode();
					tempPathNode->parentsNode = NULL;
					tempPathNode->data = tempStation;
					tempPathNode->totaldistance = 0;
					tempPathNode->totaltime = 0;
					shortestTime[tempStation->stationSeq] = tempPathNode;
					heap.push(tempPathNode);
				}
			}
		}


	}

};

class PathController {
public:
	list<Path*> pathQueue;
	list<Path*> shortPathList;
	string endStation;
	Path* shortestPath;
	int path_time;
	priority_queue<PathNode*> heap;

	PathController() {
		path_time = 999999999;
	}
	void initPath(string startStation, string endStation, vector<Line*> *lineList) {
		this->endStation = endStation;

		//���ۿ� ����
		Line *tempLine;
		Station *tempStation;
		Edge *tempEdge;
		Path *tempPath;
		vector<Station*> startStationList;
		for (vector< Line* >::iterator IterPos0 = (*lineList).begin(); IterPos0 != (*lineList).end(); ++IterPos0) {
			tempLine = (*IterPos0);
			for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
			{
				if ((*IterPos1)->name == startStation) {
					tempStation = (*IterPos1);
					startStationList.push_back(tempStation);
				}
			}
		}
		//cout << tempStation->name << endl;

		//station�κ� ��ȯ
		for (vector< Station* >::iterator IterPos0 = startStationList.begin(); IterPos0 != startStationList.end(); ++IterPos0) {
			tempStation = (*IterPos0);

			//edge�κ� ��ȯ
			for (vector< Edge* >::iterator IterPos1 = tempStation->edgeList.begin(); IterPos1 != tempStation->edgeList.end(); ++IterPos1)
			{
				tempEdge = (*IterPos1);
				if (!tempEdge->isTransit) {
					tempPath = new Path();
					tempPath->addEdge(tempEdge);
					pathQueue.push_back(tempPath);
					shortPathList.push_back(tempPath);
				}
			}
		}

		for (list<Path*>::iterator IterPos1 = pathQueue.begin(); IterPos1 != pathQueue.end(); ++IterPos1)
		{
			(*IterPos1)->toString();
		}
	}
	void calPathArr() {
		Path* tempPath;
		Path* sucPath;
		cout << "������" << endl;
		int cnt = 0;
		bool flag = true;
		while (true) {
			tempPath = pathQueue.front();
			cnt++;
			cout << cnt << endl;
			/*if (pathQueue.size() > cnt)
				cnt = pathQueue.size();
			if (shortPathList.size() >50 && flag) {
				for (list<Path*>::iterator IterPos1 = shortPathList.begin(); IterPos1 != shortPathList.end(); ++IterPos1)
				{
					cout<<"("<< (*IterPos1)->stationList.back()->line->name<<"/"<<(*IterPos1)->stationList.back()->name<<")" << " ";
				}
				cout << endl;
				flag = false;
			}*/

			//tempPath->toString();
			pathQueue.pop_front();
			calPath(tempPath);

			//���ϴ� ��� ã�Ҵ��� üũ
			while (true) {
				sucPath = chkCalRst();
				cout << "�����Ϸ�"<<sucPath << endl;
				if (sucPath != NULL) {
					cout << "�ִܰ��:" << endl;
					sucPath->toString();
					cout << path_time<<" " << sucPath->total_weight << endl;
					path_time = sucPath->total_weight;
				}
				else {
					break;
				}
				
			}
			cout << "while��"<<endl;
			if (pathQueue.empty())
				break;
		}
		cout << cnt << endl;
		cout << "��곡" << endl;
	}

	void calPath(Path* path) {
		Station* lastStation = path->edgeList.back()->from;
		Station* endStation = path->stationList.back();
		Edge* tempEdge;
		Path* tempPath;
		//edge�κ� ��ȯ
		//cout << "���" << endl;
		for (vector< Edge* >::iterator IterPos1 = endStation->edgeList.begin(); IterPos1 != endStation->edgeList.end(); ++IterPos1)
		{
			tempEdge = (*IterPos1);
			//������ �� ��ġ�°� ����
			if (tempEdge->to != lastStation) {
				tempPath = new Path();
				tempPath->copyPath(path);
				tempPath->addEdge(tempEdge);

				bool flag = true;

				if (tempPath->total_weight > path_time)
					flag = false;
				if (flag) {
					for (list<Path*>::iterator IterPos1 = shortPathList.begin(); IterPos1 != shortPathList.end();)
					{
						if ((*IterPos1)->stationList.back() == tempPath->stationList.back()) {
							if ((*IterPos1)->total_weight < tempPath->total_weight) {
								flag = false;
								break;
							}
							else {
								//							list<Path*>::iterator itertemp = IterPos1;
								cout << "����" << endl;
								IterPos1 = shortPathList.erase(IterPos1);

							}
						}
						else {
							++IterPos1;
						}
					}
				}
				if (flag) {
					pathQueue.push_back(tempPath);
					shortPathList.push_back(tempPath);
				}
			}
		}
	}

	Path* chkCalRst() {
		for (list<Path*>::iterator IterPos1 = pathQueue.begin(); IterPos1 != pathQueue.end(); )
		{
			if ((*IterPos1)->total_weight > path_time){
				Path* temp = (*IterPos1);
				IterPos1 = pathQueue.erase(IterPos1);
				delete(temp);
				cout << "������!" << endl;
			}
			else if ((*IterPos1)->checkReturnBack()) {
				Path* temp = (*IterPos1);
				IterPos1 = pathQueue.erase(IterPos1);
				temp->toString();
			}else if ((*IterPos1)->stationList.back()->name == endStation) {
				Path* temp = *IterPos1;
				list<Path*>::iterator itertemp = IterPos1;
				IterPos1 = pathQueue.erase(IterPos1);
				return temp;
			}
			else
				++IterPos1;
		}
		return NULL;
	}
};

void setLineList(vector<Line*> *lineList) {
	sqlite3_stmt * pStmt = NULL;
	char* sql = "select line, current_station, next_station, weight, wait_time, distance from edge order by line, current_station, next_station";
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
	int weight = 0;
	int wait_time = 0;
	int distance = 0;
	int stationcnt = 0;
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		lineName = (char *)sqlite3_column_text(pStmt, 0);
		stationName = (char *)sqlite3_column_text(pStmt, 1);
		nextStation = (char *)sqlite3_column_text(pStmt, 2);
		weight = sqlite3_column_int(pStmt, 3);
		wait_time = sqlite3_column_int(pStmt, 4);
		distance = sqlite3_column_int(pStmt, 5);
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
			tempStation->line = tempLine;
			tempStation->name = stationName;
			tempStation->stationSeq = stationcnt;
			lastStationName = stationName;
			tempLine->stationList.push_back(tempStation);
			stationcnt++;
		}

		//���� push
		tempEdge = new Edge();
		tempEdge->fromStationName = stationName;
		tempEdge->toStationName = nextStation;
		tempEdge->weight = weight;
		tempEdge->waittime = wait_time;
		tempEdge->distance = distance;
		tempEdge->isTransit = false;
		tempStation->edgeList.push_back(tempEdge);
	}
	cout << stationcnt << endl;
	STATION_CNT = stationcnt;

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

	pStmt = NULL;
	sql = "select line_from, station, line_to, weight from transit order by line_from, station, line_to";
	sqlite3_prepare(ppSqlite, sql, strlen(sql), &pStmt, NULL);
	sqlite3_reset(pStmt);

	tempLine = NULL;
	Station* tempFromStation = NULL;
	Station* tempToStation = NULL;
	tempEdge = NULL;
	string lineFrom = "";
	string lineTo = "";
	stationName = "";
	string lastLineFrom = "";
	lastStationName = "";
	weight = 0;
	bool isFirstLoop = true;
	vector< Line* >::iterator resultLineIter;
	vector< Station* >::iterator resultStationIter;
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		lineFrom = (char *)sqlite3_column_text(pStmt, 0);
		stationName = (char *)sqlite3_column_text(pStmt, 1);
		lineTo = (char *)sqlite3_column_text(pStmt, 2);
		weight = sqlite3_column_int(pStmt, 3);
		
		if (isFirstLoop){
			lastLineFrom = lineFrom;
			lastStationName = stationName;
			for (resultLineIter = lineList->begin(); resultLineIter != lineList->end(); ++resultLineIter) {
				if ((*resultLineIter)->name == lineFrom) {
					tempLine = (*resultLineIter);
					break;
				}
			}
			for (resultStationIter = tempLine->stationList.begin(); resultStationIter != tempLine->stationList.end(); ++resultStationIter) {
				if ((*resultStationIter)->name == stationName) {
					tempFromStation = (*resultStationIter);
					break;
				}
			}
			isFirstLoop = false;
		}

		//ȣ���� �ٲ�� ���ο� ȣ�� push
		if (lineFrom != lastLineFrom) {
			for (; resultLineIter != lineList->end(); ++resultLineIter) {
				if ((*resultLineIter)->name == lineFrom) {
					tempLine = (*resultLineIter);
					break;
				}
			}
		}

		//���� �ٲ�� ���ο� �� push
		if (stationName != lastStationName) {
			for (resultStationIter = tempLine->stationList.begin(); resultStationIter != tempLine->stationList.end(); ++resultStationIter) {
				if ((*resultStationIter)->name == stationName) {
					tempFromStation = (*resultStationIter);
					break;
				}
			}
		}

		//staion�κ� ��ȯ
		//line �κ� ��ȯ
		for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
			if ((*IterPos0)->name == lineTo) {
				//staion�κ� ��ȯ
				for (vector< Station* >::iterator IterPos1 = (*IterPos0)->stationList.begin(); IterPos1 != (*IterPos0)->stationList.end(); ++IterPos1)
				{
					if ((*IterPos1)->name == stationName) {
						tempToStation = (*IterPos1);
						break;
					}
				}
				break;
			}			
		}

		//���� push
		tempEdge = new Edge();
		tempEdge->fromStationName = tempFromStation->name;
		tempEdge->toStationName = tempToStation->name;
		tempEdge->from = tempFromStation;
		tempEdge->to = tempToStation;
		tempEdge->weight = weight;
		tempEdge->waittime = 0;
		tempEdge->distance = 0;
		tempEdge->isTransit = true;
		tempFromStation->edgeList.push_back(tempEdge);
	}

	//��� ���
	/*int cnt = 0;
	for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
		tempLine = (*IterPos0);
		cout << tempLine->name << endl;
		for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
		{
			cout << (*IterPos1)->name << ": ";
			for (vector< Edge* >::iterator IterPos2 = (*IterPos1)->edgeList.begin(); IterPos2 != (*IterPos1)->edgeList.end(); ++IterPos2)
			{
				cout << "("<< (*IterPos2)->weight << "/" << (*IterPos2)->waittime<<"/" << (*IterPos2)->isTransit<<"/" << (*IterPos2)->from->name << ",";
				if ((*IterPos2)->to != NULL)
					cout << (*IterPos2)->to->name;
				cout << ") ";
			}
			cout << endl;
		}
		if (cnt > 2)
			break;
		cnt++;
	}*/
}

int main() {
	std::cout << "!!" << std::endl;
	sqlite3 *pSQLite3 = NULL; // SQLite DB ��ü ���� ����
	char    *szErrMsg = NULL; // Error �߻��� �޼����� �����ϴ� ����							  
	int rst = sqlite3_open("test10.db", &pSQLite3);// �����ͺ��̽� ���� : ������ �������� ������ �����Ѵ�.
	ppSqlite = pSQLite3;

	vector<Line*> lineList;
	
	//��ȣ���� ���� ���� ����()
	setLineList(&lineList);

	/*PathController *pc = new PathController();
	pc->initPath("ȭ��", "��ž", &lineList);
	pc->calPathArr();*/
	//��� ���
	/*Line *tempLine;
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
				cout << ", " << (*IterPos2)->weight;
				cout << ", " << (*IterPos2)->waittime;
				cout << ") ";
			}
			cout << endl;
		}
	}*/

	sqlite3_free(szErrMsg);
	sqlite3_close(pSQLite3);
	return 0;
}