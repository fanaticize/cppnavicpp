#ifndef __PATH_HANDLER_H__
#define __PATH_HANDLER_H__

#include "SubwayCommonDecl.h"
#include "Line.h"
#include "PathNode.h"
#include "SQLiteHandler.h"

class PathHandler {
private:
	int stationCnt;
	vector<Line*> *lineList;
	bool* visitList;
	vector<PathNode*> nodeList;
	priority_queue<PathNode*, vector<PathNode*>, PathNodeCompare> heap;
public:
	PathHandler();
	~PathHandler();
	void findShortestPath(string startStation, string endStation);
	void findShortestPath(string startStation, string endStation, string midStation);
	int chkTransitAtMid(PathNode* node1, PathNode* node2);
	void printTime(int totalSec);
	void printMinuteTime(int totalSec);
	void resetHeap();
	void deletePathNode();
	int calCharge(string startStation, string endStation, PathNode* node1, PathNode* node2=NULL);
	int divideUpper(int a, int b);
	void printPath(PathNode* node);
	PathNode* reversePath(PathNode* path);
	void initPath(string startStation, int mode);
	PathNode* findPath(string endStation);
	Line* findLineByName(string lineName);
	Station* findStationByName(string stationName, Line* line);
	void setLineList(SQLiteHandler* sqliteHandler);
	void connectToStation();
	bool isExistStation(string name);
};

#endif