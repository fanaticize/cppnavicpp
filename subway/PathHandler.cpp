#include "PathHandler.h"

PathHandler::PathHandler() {
	lineList = new vector<Line*>();
}
PathHandler::~PathHandler() {
	for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0)
	{
		delete(*IterPos0);
	}
	delete(lineList);
}


void PathHandler::findShortestPath(string startStation, string endStation) {
	initPath(startStation, MINIMUM_TIME);
	PathNode* shortestPath = findPath(endStation);

	int totalSec = shortestPath->getTotaltime();
	cout << "최단시간경로검색----" << startStation << "~" << endStation << endl;
	cout << "시간: ";
	printTime(totalSec);
	cout << " /환승: " << shortestPath->getTotaltransit();
	shortestPath = reversePath(shortestPath);
	cout << " /요금: " << calCharge(startStation, endStation, shortestPath) << "원" << endl;
	cout << endl;
	printPath(shortestPath);
	cout << endl;

	initPath(startStation, MINIMUM_TRANSIT);
	shortestPath = findPath(endStation);
	totalSec = shortestPath->getTotaltime();
	cout << "최소환승경로검색----" << startStation << "~" << endStation << endl;
	cout << "시간: ";
	printTime(totalSec);
	cout << " /환승: " << shortestPath->getTotaltransit();
	shortestPath = reversePath(shortestPath);
	cout << " /요금: " << calCharge(startStation, endStation, shortestPath) << "원" << endl;
	cout << endl;
	printPath(shortestPath);
	cout << endl;	

	deletePathNode();
}

void  PathHandler::findShortestPath(string startStation, string endStation, string midStation) {
	PathNode* shortestPath1;
	PathNode* shortestPath2;
	PathNode* shortestTransitPath1;
	PathNode* shortestTransitPath2;
	int totalTimeShrt = 0;
	int totalTimeMin = 0;
	int totalTransitShrt = 0;
	int totalTransitMin = 0;
	int transitTimeAtMidShrt = 0;
	int transitTimeAtMidMin = 0;

	initPath(startStation, MINIMUM_TIME);
	shortestPath1 = findPath(midStation);
	totalTimeShrt += shortestPath1->getTotaltime();
	totalTransitShrt += shortestPath1->getTotaltransit();

	initPath(midStation, MINIMUM_TIME);
	shortestPath2 = findPath(endStation);
	totalTimeShrt += shortestPath2->getTotaltime();
	totalTransitShrt += shortestPath2->getTotaltransit();

	initPath(startStation, MINIMUM_TRANSIT);
	shortestTransitPath1 = findPath(midStation);
	totalTimeMin += shortestTransitPath1->getTotaltime();
	totalTransitMin += shortestTransitPath1->getTotaltransit();

	initPath(midStation, MINIMUM_TRANSIT);
	shortestTransitPath2 = findPath(endStation);
	totalTimeMin += shortestTransitPath2->getTotaltime();
	totalTransitMin += shortestTransitPath2->getTotaltransit();

	transitTimeAtMidShrt = chkTransitAtMid(shortestPath1, shortestPath2);
	transitTimeAtMidMin = chkTransitAtMid(shortestTransitPath1, shortestTransitPath2);
	if (transitTimeAtMidShrt != 0) {
		totalTimeShrt += transitTimeAtMidShrt;
		totalTransitShrt++;
	}
	if (transitTimeAtMidMin != 0) {
		totalTimeMin += transitTimeAtMidMin;
		totalTransitMin++;
	}

	shortestPath1 = reversePath(shortestPath1);
	shortestPath2 = reversePath(shortestPath2);
	shortestTransitPath1 = reversePath(shortestTransitPath1);
	shortestTransitPath2 = reversePath(shortestTransitPath2);

	cout << "최단시간경로검색----" << startStation << "~" << midStation << "~" << endStation << endl;
	cout << "시간: ";
	printTime(totalTimeShrt);
	cout << " /환승: " << totalTransitShrt;
	cout << " /요금: " << calCharge(startStation, endStation, shortestPath1, shortestPath2) << "원" << endl;
	printPath(shortestPath1);
	cout << "경유: " << midStation << endl;;
	if (transitTimeAtMidShrt != 0) {
		cout << "환승: ";
		printMinuteTime(transitTimeAtMidShrt);
		cout << endl;
	}
	printPath(shortestPath2);
	cout << endl;


	cout << "최소환승경로검색----" << startStation << "~" << midStation << "~" << endStation << endl;
	cout << "시간: ";
	printTime(totalTimeMin);
	cout << " /환승: " << totalTransitMin;
	cout << " /요금: " << calCharge(startStation, endStation, shortestTransitPath1, shortestTransitPath2) << "원" << endl;
	printPath(shortestTransitPath1);
	cout << "경유: " << midStation << endl;;
	if (transitTimeAtMidMin != 0) {
		cout << "환승: ";
		printMinuteTime(transitTimeAtMidMin);
		cout << endl;
	}
	printPath(shortestTransitPath2);
	cout << endl;

	deletePathNode();
}

int PathHandler::chkTransitAtMid(PathNode* node1, PathNode* node2) {
	PathNode* tempNode2 = node2;
	while (tempNode2->getParentsNode() != NULL)
		tempNode2 = tempNode2->getParentsNode();

	if (node1->getStation()->getLine()->getName() == tempNode2->getStation()->getLine()->getName())
		return 0;
	else {
		Station* tempStation1 = node1->getStation();
		Station* tempStation2 = tempNode2->getStation();
		Station* tempStation = node1->getStation();
		Edge* tempEdge;
		for (vector< Edge* >::iterator IterPos0 = tempStation->getEdgeList()->begin(); IterPos0 != tempStation->getEdgeList()->end(); ++IterPos0)
		{
			tempEdge = (*IterPos0);
			if (tempEdge->getFrom() == tempStation1 && tempEdge->getTo() == tempStation2)
				return tempEdge->getWeight();
		}
	}
	return 0;
}

void PathHandler::printTime(int totalSec) {
	int hour = totalSec / 3600;
	int min = (totalSec % 3600) / 60;
	int sec = (totalSec % 3600) % 60;
	if (hour != 0)
		cout << hour << "시간";
	if (min != 0) {
		if (hour != 0)
			cout << " ";
		cout << min << "분";
	}
	if (sec != 0) {
		if (hour != 0 || min != 0)
			cout << " ";
		cout << sec << "초";
	}
	if (totalSec == 0)
		cout << "0초";
}

void PathHandler::printMinuteTime(int totalSec) {
	int min = totalSec / 60;
	int sec = totalSec % 60;
	if (min != 0)
		cout << min << "분";
	if (sec != 0) {
		if (min != 0)
			cout << " ";
		cout << sec << "초";
	}
	if (totalSec == 0)
		cout << "0초";
}

void PathHandler::resetHeap() {
	while (!heap.empty()) heap.pop();
}

void PathHandler::deletePathNode() {
	PathNode* tempNode = NULL;
	while (!nodeList.empty()) {
		tempNode = nodeList.back();
		nodeList.pop_back();
		delete(tempNode);
	}
	return;
}

int PathHandler::calCharge(string startStation, string endStation, PathNode* node1, PathNode* node2) {
	int inRange = 0;
	int outRange = 0;
	int totalRange = 0;
	int totalCharge = 0;
	bool isUseShinbundang = false;
	bool isUseEverline = false;
	bool isUseULine = false;
	PathNode* tempPath = NULL;
	Edge* tempEdge;
	string lineName = "";
	tempPath = node1;
	while (tempPath != NULL) {
		tempEdge = tempPath->getEdge();
		if (tempEdge != NULL) {
			lineName = tempEdge->getTo()->getLine()->getName();
			if (lineName == "신분당선" && !isUseShinbundang)
				isUseShinbundang = true;
			else if (lineName == "의정부선" && !isUseULine)
				isUseULine = true;
			else if (lineName == "에버라인" && !isUseEverline)
				isUseEverline = true;
		}
		tempPath = tempPath->getParentsNode();
	}
	tempPath = node2;
	while (tempPath != NULL) {
		tempEdge = tempPath->getEdge();
		if (tempEdge != NULL) {
			lineName = tempEdge->getTo()->getLine()->getName();
			if (lineName == "신분당선" && !isUseShinbundang)
				isUseShinbundang = true;
			else if (lineName == "의정부선" && !isUseULine)
				isUseULine = true;
			else if (lineName == "에버라인" && !isUseEverline)
				isUseEverline = true;
		}
		tempPath = tempPath->getParentsNode();
	}

	initPath(startStation, MINIMUM_DISTANCE);
	tempPath= findPath(endStation);
	tempPath = reversePath(tempPath);

	while (tempPath != NULL) {
		tempEdge = tempPath->getEdge();
		if (tempEdge != NULL) {
			if (tempEdge->getIsMetropolitan())
				inRange += tempEdge->getDistance();
			else
				outRange += tempEdge->getDistance();
		}
		tempPath = tempPath->getParentsNode();
	}
	if (inRange == 0 || outRange == 0) {
		totalRange = inRange + outRange;
		totalRange -= 10000;
		totalCharge += 1250;
		if (totalRange > 0) {
			if (totalRange > 40000) {
				totalCharge += divideUpper(40000, 5000) * 100;
				totalRange -= 40000;
				totalCharge += divideUpper(totalRange, 8000) * 100;
			}
			else {
				totalCharge += divideUpper(totalRange, 5000) * 100;
			}
		}
	}
	else {
		totalRange += inRange;
		totalRange -= 10000;
		totalCharge += 1250;
		if (totalRange > 0) {
			if (totalRange > 40000) {
				totalCharge += divideUpper(40000, 5000) * 100;
				totalRange -= 40000;
				totalCharge += divideUpper(totalRange, 8000) * 100;
			}
			else {
				totalCharge += divideUpper(totalRange, 5000) * 100;
			}
		}
		totalCharge += divideUpper(outRange, 4000) * 100;
	}
	if (isUseShinbundang)
		totalCharge += 900;
	if (isUseEverline)
		totalCharge += 200;
	if (isUseULine)
		totalCharge += 100;
	return totalCharge;
}

int PathHandler::divideUpper(int a, int b) {
	return a%b == 0 ? a / b : a / b + 1;
}

void PathHandler::printPath(PathNode* node) {
	PathNode* temp = node;
	PathNode* lastnode;
	int lastTotalTime = 0;
	string direction = "";
	int passStationCnt = 0;
	bool flag = true;
	while (temp != NULL) {
		if (flag) {
			cout << "출발: " << temp->getStation()->getName() << "(" << temp->getStation()->getLine()->getName() << ")" << endl;;
			direction = temp->getParentsNode()->getStation()->getName();
			flag = false;
		}
		else {
			if (lastnode->getStation()->getLine() != temp->getStation()->getLine()) {
				cout << "→" << direction << " 방면 " << passStationCnt - 1 << "개 역 이동 (";
				direction = temp->getParentsNode()->getStation()->getName();
				printMinuteTime(lastnode->getTotaltime() - lastTotalTime);
				cout << ")" << endl;
				passStationCnt = 0;

				cout << "도착: " << lastnode->getStation()->getName() << "(" << lastnode->getStation()->getLine()->getName() << ") " << endl;

				cout << "환승: 도보 ";
				lastTotalTime = lastnode->getTotaltime();
				printMinuteTime(temp->getTotaltime() - lastTotalTime);
				cout << endl;

				cout << "출발: " << temp->getStation()->getName() << "(" << temp->getStation()->getLine()->getName() << ") " << endl;

				lastTotalTime = temp->getTotaltime();
			}
		}
		lastnode = temp;
		temp = temp->getParentsNode();
		passStationCnt++;
	}
	cout << "→" << direction << " 방면 " << passStationCnt - 1 << "개 역 이동 (";
	printMinuteTime(lastnode->getTotaltime() - lastTotalTime);
	cout << ")" << endl;
	cout << "도착: " << lastnode->getStation()->getName() << "(" << lastnode->getStation()->getLine()->getName() << ") ";
	cout << endl;
}

PathNode* PathHandler::reversePath(PathNode* path) {
	PathNode* temp = path;
	PathNode* trail;
	PathNode* middle;
	middle = NULL;
	while (temp != NULL) {
		trail = middle;
		middle = temp;
		temp = temp->getParentsNode();
		middle->setParentsNode(trail);
	}
	return middle;
}

void PathHandler::initPath(string startStation, int mode) {
	visitList = new bool[stationCnt];
	for (int i = 0; i < stationCnt; i++)
		visitList[i] = false;

	Line *tempLine;
	PathNode* tempPathNode;
	Station* tempStation;
	for (vector< Line* >::iterator IterPos0 = (*lineList).begin(); IterPos0 != (*lineList).end(); ++IterPos0) {
		tempLine = (*IterPos0);
		tempStation = findStationByName(startStation, tempLine);
		if (tempStation != NULL) {
			tempPathNode = new PathNode(NULL, tempStation, NULL, mode);
			nodeList.push_back(tempPathNode);
			heap.push(tempPathNode);
		}
	}
}

PathNode* PathHandler::findPath(string endStation) {
	Edge* tempEdge;
	PathNode* tempPathNode;
	PathNode* insPathNode;
	PathNodeCompare compare;
	int tempTotalTime;
	int tempTotalTransit;
	int tempTotalDistance;

	while (true) {
		if (heap.empty())
			break;

		tempPathNode = heap.top();
		heap.pop();
		if (tempPathNode->getStation()->getName() == endStation) {
			resetHeap();
			delete(visitList);
			visitList = NULL;
			return tempPathNode;
		}
		if (visitList[tempPathNode->getStation()->getStationSeq()])
			continue;
		else {
			visitList[tempPathNode->getStation()->getStationSeq()] = true;
			for (vector<Edge* >::iterator IterPos0 = tempPathNode->getStation()->getEdgeList()->begin(); IterPos0 != tempPathNode->getStation()->getEdgeList()->end(); ++IterPos0) {
				tempEdge = (*IterPos0);
				if (tempEdge->getTo() != tempPathNode->getStation()) {
					//걸린 시간 계산
					if (tempPathNode->getTotaltime() == 0)
						tempTotalTime = tempPathNode->getTotaltime() + tempEdge->getWeight();
					else if (tempPathNode->getParentsNode()->getStation()->getLine() != tempPathNode->getStation()->getLine())
						tempTotalTime = tempPathNode->getTotaltime() + tempEdge->getWeight();
					else
						tempTotalTime = tempPathNode->getTotaltime() + tempEdge->getWeight() + tempEdge->getWaitTime();

					//환승횟수 계산
					tempTotalTransit = tempPathNode->getTotaltransit();
					if (tempEdge->getIsTransit())
						tempTotalTransit++;

					//거리계산
					tempTotalDistance = tempPathNode->getTotaldistance() + tempEdge->getDistance();

					insPathNode = new PathNode(tempPathNode, tempEdge->getTo(), tempEdge, tempPathNode->getMode(), tempTotalTime, tempTotalTransit, tempTotalDistance);
					nodeList.push_back(insPathNode);
					heap.push(insPathNode);
				}
			}
		}
	}
	
	return NULL;
}

Line* PathHandler::findLineByName(string lineName) {
	for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
		if ((*IterPos0)->getName() == lineName)
			return (*IterPos0);
	}
	return NULL;
}

Station* PathHandler::findStationByName(string stationName, Line* line) {
	for (vector< Station* >::iterator IterPos0 = line->getStationList()->begin(); IterPos0 != line->getStationList()->end(); ++IterPos0) {
		if ((*IterPos0)->getName() == stationName) {
			return (*IterPos0);
		}
	}
	return NULL;
}

void PathHandler::setLineList(SQLiteHandler* sqliteHandler) {
	char* sql = "select line, current_station, next_station, weight, wait_time, distance, isMetropolitan from edge order by line, current_station, next_station";
	sqliteHandler->select(sql);

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
	bool isMetropolitan = true;
	stationCnt = 0;
	while (sqliteHandler->selectNextStep()) {
		lineName = sqliteHandler->getTextFromRst(0);
		stationName = sqliteHandler->getTextFromRst(1);
		nextStation = sqliteHandler->getTextFromRst(2);
		weight = sqliteHandler->getIntFromRst(3);
		wait_time = sqliteHandler->getIntFromRst(4);
		distance = sqliteHandler->getIntFromRst(5);
		isMetropolitan = (sqliteHandler->getIntFromRst(6) == 1) ? true : false;

		//호선이 바뀌면 새로운 호선 push
		if (lineName != lastLineName) {
			tempLine = new Line(lineName);
			lastLineName = lineName;
			this->lineList->push_back(tempLine);
		}

		//역이 바뀌면 새로운 역 push
		if (stationName != lastStationName) {
			tempStation = new Station(tempLine, stationName, stationCnt);
			tempLine->getStationList()->push_back(tempStation);
			lastStationName = stationName;
			stationCnt++;
		}

		//간선 push
		tempEdge = new Edge(weight, wait_time, distance, lineName, nextStation, false, isMetropolitan, tempStation);
		tempStation->getEdgeList()->push_back(tempEdge);
	}
	sqliteHandler->finalizeStatement();

	sql = "select line_from, station, line_to, weight from transit order by line_from, station, line_to";
	sqliteHandler->select(sql);
	tempLine = NULL;
	Station* tempFromStation = NULL;
	tempEdge = NULL;
	string lineFrom = "";
	string lineTo = "";
	stationName = "";
	weight = 0;

	while (sqliteHandler->selectNextStep()) {
		lineFrom = sqliteHandler->getTextFromRst(0);
		stationName = sqliteHandler->getTextFromRst(1);
		lineTo = sqliteHandler->getTextFromRst(2);
		weight = sqliteHandler->getIntFromRst(3);
		tempFromStation = NULL;

		tempLine = findLineByName(lineFrom);
		tempFromStation = findStationByName(stationName, tempLine);

		tempEdge = new Edge(weight, 0, 0, lineTo, stationName, true, true, tempFromStation);
		tempFromStation->getEdgeList()->push_back(tempEdge);
	}
	sqliteHandler->finalizeStatement();

	connectToStation();
}

void PathHandler::connectToStation() {
	Line* tempLine;
	Line* tempToLine;
	Station* tempStation;
	Station* tempToStation;
	Edge* tempEdge;

	for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
		tempLine = (*IterPos0);
		for (vector< Station* >::iterator IterPos1 = tempLine->getStationList()->begin(); IterPos1 != tempLine->getStationList()->end(); ++IterPos1)
		{
			tempStation = (*IterPos1);
			for (vector< Edge* >::iterator IterPos2 = (*IterPos1)->getEdgeList()->begin(); IterPos2 != (*IterPos1)->getEdgeList()->end(); ++IterPos2)
			{
				tempEdge = (*IterPos2);
				//toLineName과 tempLine과 일치하면 tempLine에서 toStation을 찾는다. 없으면 모든 line에서 station을 검색
				if (tempEdge->getToLineName() == tempLine->getName()) {
					tempToStation = findStationByName(tempEdge->getToStationName(), tempLine);
					tempEdge->setTo(tempToStation);
				}
				else {
					tempToLine = findLineByName(tempEdge->getToLineName());
					tempToStation = findStationByName(tempEdge->getToStationName(), tempToLine);
					tempEdge->setTo(tempToStation);
				}

			}
		}
	}
}

bool PathHandler::isExistStation(string name) {
	Station* tempStation = NULL;
	for (vector< Line* >::iterator IterPos0 = lineList->begin(); IterPos0 != lineList->end(); ++IterPos0) {
		tempStation = findStationByName(name, (*IterPos0));
		if (tempStation != NULL)
			return true;
	}
	return false;
}