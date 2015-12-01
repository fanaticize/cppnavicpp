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
		cout << "역 배열: ";
		for (vector<Station*>::iterator IterPos1 = stationList.begin(); IterPos1 != stationList.end(); ++IterPos1)
		{
			cout << (*IterPos1)->name<<" ";
		}
		cout << endl;
		cout << "간선 배열";
		for (vector<Edge*>::iterator IterPos1 = edgeList.begin(); IterPos1 != edgeList.end(); ++IterPos1)
		{
			cout << "(" << (*IterPos1)->from->line->name<< "/" << (*IterPos1)->fromStationName << ", " << (*IterPos1)->to->line->name << "/" << (*IterPos1)->toStationName << ") ";
		}
		cout << endl;
		cout << "걸리는시간: " << total_weight << endl;
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
	int totaltime = 0;
	int distance = 0;
	int totaltransit = 0;
	int mode = 0;//1: shortest time, 2: shortest dest
	/*bool operator > (const PathNode& pn) const {
		if (this->mode == 1) {
			if (totaltime == pn.totaltime)
				return totaltransit > pn.totaltransit;
			else
				return totaltime > pn.totaltime;
		}
		else if (this->mode == 2) {
			if (totaltransit == pn.totaltransit)
				return totaltime > pn.totaltime;
			else
				return totaltransit > pn.totaltransit;
		}
		return totaltime > pn.totaltime;
	}*/
};

struct PathNodeCompare {
	bool operator() (PathNode* node1, PathNode* node2) {
		if (node1->mode == 1) {
			if (node1->totaltime == node2->totaltime)
				return node1->totaltransit > node2->totaltransit;
			else
				return node1->totaltime > node2->totaltime;
		}
		else if (node1->mode == 2) {
			if (node1->totaltransit == node2->totaltransit)
				return node1->totaltime > node2->totaltime;
			else
				return node1->totaltransit > node2->totaltransit;
		}
		return node1->totaltime > node2->totaltime;
	}
};


class PathController2 {
public:
	bool* visitList;
	PathNode** shortestTime;
	priority_queue<PathNode*, vector<PathNode*>, PathNodeCompare> heap;
	string startStation;
	string endStation;
	vector<Line*> *lineList;
	void findShortestPath(string startStation, string endStation, vector<Line*> *lineList) {
		this->startStation = startStation;
		this->endStation = endStation;
		this->lineList = lineList;
		int mode = 1;
		initPath2(mode);
		
		PathNode* shortestPath = calShortestPath();
		int totalsec = shortestPath->totaltime;
		int hour = totalsec / 3600;
		int min = (totalsec % 3600) / 60;
		int sec = (totalsec % 3600) % 60;
		cout << "최단시간경로검색----"<<startStation<<"~"<<endStation << endl;
		cout<<"최단시간: "<<hour<<"시간 "<<min<<"분 "<<sec<<"초 "<< endl;
		cout << "환승: " << shortestPath->totaltransit << endl;
		shortestPath = reversePath(shortestPath);
		printPath(shortestPath);
		/*cout << endl;
		cout << heap.top()->totaltime << endl;
		heap.pop();
		cout << heap.top()->totaltime << endl;
		heap.pop();
		cout << heap.top()->totaltime << endl;
		heap.pop();
		cout << heap.top()->totaltime << endl;*/
		while (!heap.empty()) heap.pop();
		delete(visitList);
		delete(shortestTime);
		visitList = NULL;
		shortestTime = NULL;
	}

	void findShortestTransitPath(string startStation, string endStation, vector<Line*> *lineList) {
		this->startStation = startStation;
		this->endStation = endStation;
		this->lineList = lineList;
		int mode = 2;
		initPath2(mode);
		PathNode* shortestPath = calShortestTransitPath();
		int totalsec = shortestPath->totaltime;
		int hour = totalsec / 3600;
		int min = (totalsec % 3600) / 60;
		int sec = (totalsec % 3600) % 60;
		cout << "최소환승경로검색----" << startStation << "~" << endStation << endl;
		cout << "최단시간: " << hour << "시간 " << min << "분 " << sec << "초 " << endl;
		cout << "환승: " << shortestPath->totaltransit << endl;
		shortestPath = reversePath(shortestPath);
		PathNode* temp = shortestPath;
		printPath(shortestPath);

		while (!heap.empty()) heap.pop();
		delete(visitList);
		delete(shortestTime);
		visitList = NULL;
		shortestTime = NULL;
	}

	void printPath(PathNode* node) {
		PathNode* temp = node;
		PathNode* lastnode;
		bool flag = true;
		while (temp != NULL) {
			if (flag) {
				cout << "(" << temp->data->line->name << ", " << temp->data->name << ") ->";
				flag = false;
			}
			else {
				if (lastnode->data->line != temp->data->line) {
					cout << "(" << lastnode->data->line->name << ", " << lastnode->data->name << ") ->";
					cout << "(" << temp->data->line->name << ", " << temp->data->name << ") ->";
				}
			}
			lastnode = temp;
			temp = temp->parentsNode;
		}
		cout << "(" << lastnode->data->line->name << ", " << lastnode->data->name << ")" << endl;
		cout << endl;
		/*while (temp != NULL) {
			cout << "(" << temp->data->line->name << ", " << temp->data->name << ") ->";
			temp = temp->parentsNode;
		}*/
	}

	PathNode* reversePath(PathNode* path) {
		PathNode* trail;
		PathNode* middle;
		middle = NULL;
		while (path != NULL) {
			trail = middle;
			middle = path;
			path = path->parentsNode;
			middle->parentsNode = trail;
		}
		return middle;
	}

	void initPath2(int mode) {
		visitList = new bool[STATION_CNT];
		shortestTime = new PathNode*[STATION_CNT];
		for (int i = 0; i < STATION_CNT; i++) {
			visitList[i] = false;
			shortestTime[i] = NULL;
		}

		Line *tempLine;
		Station *tempStation;
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

		//station부분 순환
		for (vector< Station* >::iterator IterPos0 = startStationList.begin(); IterPos0 != startStationList.end(); ++IterPos0) {	
			tempStation = (*IterPos0);
			tempPathNode = new PathNode();
			tempPathNode->parentsNode = NULL;
			tempPathNode->data = tempStation;
			tempPathNode->distance = 0;
			tempPathNode->totaltime = 0;
			tempPathNode->totaltransit = 0;
			tempPathNode->mode = mode;
			heap.push(tempPathNode);
		}
	}

	PathNode* calShortestPath() {
		Edge* tempEdge;
		PathNode* tempPathNode;
		PathNode* insPathNode;
		while (true) {
			if (heap.empty())
				break;

			tempPathNode = heap.top();
			heap.pop();
			if (tempPathNode->data->name == endStation) {
				return tempPathNode;
			}
			if (visitList[tempPathNode->data->stationSeq])
				continue;
			else {
				visitList[tempPathNode->data->stationSeq] = true;
				for (vector<Edge* >::iterator IterPos0 = tempPathNode->data->edgeList.begin(); IterPos0 != tempPathNode->data->edgeList.end(); ++IterPos0) {
					tempEdge = (*IterPos0);
					if (tempEdge->to != tempPathNode->data) {
						insPathNode = new PathNode();
						insPathNode->parentsNode = tempPathNode;
						insPathNode->data = tempEdge->to;
						insPathNode->distance = tempEdge->distance;
						if (tempPathNode->totaltime == 0) {
							insPathNode->totaltime = tempPathNode->totaltime + tempEdge->weight;
						}
						else if (tempPathNode->parentsNode->data->line != tempPathNode->data->line) {
							insPathNode->totaltime = tempPathNode->totaltime + tempEdge->weight;
						}
						else
							insPathNode->totaltime = tempPathNode->totaltime + tempEdge->weight + tempEdge->waittime;
						insPathNode->totaltransit = tempPathNode->totaltransit;
						insPathNode->mode = tempPathNode->mode;
						if (tempEdge->isTransit)
							insPathNode->totaltransit++;
						shortestTime[tempPathNode->data->stationSeq] = insPathNode;
						heap.push(insPathNode);
					}
				}
			}
		}
		return NULL;
	}

	PathNode* calShortestTransitPath() {
		Edge* tempEdge;
		PathNode* tempPathNode;
		PathNode* insPathNode;
		while (true) {
			if (heap.empty())
				break;

			tempPathNode = heap.top();
			heap.pop();
			if (tempPathNode->data->name == endStation) {
				return tempPathNode;
			}
			if (visitList[tempPathNode->data->stationSeq])
				continue;
			else {
				visitList[tempPathNode->data->stationSeq] = true;
				for (vector<Edge* >::iterator IterPos0 = tempPathNode->data->edgeList.begin(); IterPos0 != tempPathNode->data->edgeList.end(); ++IterPos0) {
					tempEdge = (*IterPos0);
					if (tempEdge->to != tempPathNode->data) {
						insPathNode = new PathNode();
						insPathNode->parentsNode = tempPathNode;
						insPathNode->data = tempEdge->to;
						insPathNode->distance = tempEdge->distance;
						insPathNode->totaltime = tempPathNode->totaltime + tempEdge->weight + tempEdge->waittime;
						insPathNode->totaltransit = tempPathNode->totaltransit;
						insPathNode->mode = tempPathNode->mode;
						if (tempEdge->isTransit)
							insPathNode->totaltransit++;
						shortestTime[tempPathNode->data->stationSeq] = insPathNode;
						heap.push(insPathNode);
					}
				}
			}
		}
		return NULL;
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

		//시작역 설정
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

		//station부분 순환
		for (vector< Station* >::iterator IterPos0 = startStationList.begin(); IterPos0 != startStationList.end(); ++IterPos0) {
			tempStation = (*IterPos0);

			//edge부분 순환
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
		cout << "계산시작" << endl;
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

			//원하는 경로 찾았는지 체크
			while (true) {
				sucPath = chkCalRst();
				cout << "검증완료"<<sucPath << endl;
				if (sucPath != NULL) {
					cout << "최단경로:" << endl;
					sucPath->toString();
					cout << path_time<<" " << sucPath->total_weight << endl;
					path_time = sucPath->total_weight;
				}
				else {
					break;
				}
				
			}
			cout << "while밖"<<endl;
			if (pathQueue.empty())
				break;
		}
		cout << cnt << endl;
		cout << "계산끝" << endl;
	}

	void calPath(Path* path) {
		Station* lastStation = path->edgeList.back()->from;
		Station* endStation = path->stationList.back();
		Edge* tempEdge;
		Path* tempPath;
		//edge부분 순환
		//cout << "계산" << endl;
		for (vector< Edge* >::iterator IterPos1 = endStation->edgeList.begin(); IterPos1 != endStation->edgeList.end(); ++IterPos1)
		{
			tempEdge = (*IterPos1);
			//이전역 과 겹치는것 제외
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
								cout << "삭제" << endl;
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
				cout << "지웠다!" << endl;
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
			tempStation->line = tempLine;
			tempStation->name = stationName;
			tempStation->stationSeq = stationcnt;
			lastStationName = stationName;
			tempLine->stationList.push_back(tempStation);
			stationcnt++;
		}

		//간선 push
		tempEdge = new Edge();
		tempEdge->fromStationName = stationName;
		tempEdge->toStationName = nextStation;
		tempEdge->weight = weight;
		tempEdge->waittime = wait_time;
		tempEdge->distance = distance;
		tempEdge->isTransit = false;
		tempStation->edgeList.push_back(tempEdge);
	}
	STATION_CNT = stationcnt;

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
		
		/*if (isFirstLoop){
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

		//호선이 바뀌면 새로운 호선 push
		if (lineFrom != lastLineFrom) {
			//cout << "#변경: " << lastLineFrom << " " << lineFrom << " " << endl;
			for (; resultLineIter != lineList->end(); ++resultLineIter) {
				if ((*resultLineIter)->name == lineFrom) {
					//cout << "##변경: " << lastLineFrom << " " << lineFrom<<" "<< (*resultLineIter)->name << endl;
					tempLine = (*resultLineIter);
					lastLineFrom = tempLine->name;
					break;
				}
			}
		}

		//역이 바뀌면 새로운 역 push
		if (stationName != lastStationName) {
			for (resultStationIter = tempLine->stationList.begin(); resultStationIter != tempLine->stationList.end(); ++resultStationIter) {
				if ((*resultStationIter)->name == stationName) {
					tempFromStation = (*resultStationIter);
					lastStationName = tempFromStation->name;
					break;
				}
			}
		}*/
		tempToStation = NULL;
		tempFromStation = NULL;
		//line 부분 순환
		for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
			if ((*IterPos0)->name == lineFrom) {
				//staion부분 순환
				for (vector< Station* >::iterator IterPos1 = (*IterPos0)->stationList.begin(); IterPos1 != (*IterPos0)->stationList.end(); ++IterPos1)
				{
					if ((*IterPos1)->name == stationName) {
						tempFromStation = (*IterPos1);
						break;
					}
				}
				break;
			}
		}

		//line 부분 순환
		for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
			if ((*IterPos0)->name == lineTo) {
				//staion부분 순환
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
		//cout << tempFromStation->line->name <<" " << tempFromStation->name << " " << tempToStation->line->name << " " << tempToStation->name << endl;
		//간선 push
		tempEdge = new Edge();
		tempEdge->fromStationName = tempFromStation->name;
		tempEdge->toStationName = tempToStation->name;
		tempEdge->from = tempFromStation;
		tempEdge->to = tempToStation;
		tempEdge->weight = weight;
		tempEdge->waittime = 0;
		tempEdge->distance = 0;
		tempEdge->isTransit = true;
		//cout <<tempEdge->from->line->name<<", "<< tempEdge->from->name << " "<< tempEdge->to->line->name<<", " << tempEdge->to->name << " " << tempEdge->isTransit << endl;
		tempFromStation->edgeList.push_back(tempEdge);
	}

	//결과 출력
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
	sqlite3 *pSQLite3 = NULL; // SQLite DB 객체 저장 변수
	char    *szErrMsg = NULL; // Error 발생시 메세지를 저장하는 변수							  
	int rst = sqlite3_open("test12.db", &pSQLite3);// 데이터베이스 열기 : 파일이 존재하지 않으면 생성한다.
	ppSqlite = pSQLite3;

	vector<Line*> lineList;
	
	//각호선별 역과 간선 설정()
	setLineList(&lineList);
	cout << "로딩완료" << endl;
	PathController2 *pc2 = new PathController2();
	pc2->findShortestPath("수원", "압구정", &lineList);
	pc2->findShortestTransitPath("수원", "압구정", &lineList);
	/*PathController *pc = new PathController();
	pc->initPath("화서", "야탑", &lineList);
	pc->calPathArr();*/
	//결과 출력
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
	/*Line *tempLine;
	Station *tempStation;
	Edge *tempEdge;
	for (vector< Line* >::iterator IterPos0 = lineList.begin(); IterPos0 != lineList.end(); ++IterPos0) {
		tempLine = (*IterPos0);
		cout << tempLine->name << endl;
		for (vector< Station* >::iterator IterPos1 = tempLine->stationList.begin(); IterPos1 != tempLine->stationList.end(); ++IterPos1)
		{
			cout << (*IterPos1)->name << ": ";
			cout << (*IterPos1)->edgeList.size();
			cout << endl;
		}
	}*/

	sqlite3_free(szErrMsg);
	sqlite3_close(pSQLite3);
	return 0;
}