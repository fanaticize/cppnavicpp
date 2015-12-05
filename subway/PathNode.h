#ifndef __PATH_NODE_H__
#define __PATH_NODE_H__

#include "SubwayCommonDecl.h"
#include "Station.h"
#include "Edge.h"

class PathNode {
private:
	PathNode* parentsNode;
	Station* station;
	Edge* edge;//이전역과 현재역의 edge
	int totaltime = 0;
	int totaltransit = 0;
	int totaldistance = 0;
	int mode = 0; //1: shortest time, 2: shortest dest
public:
	PathNode();
	PathNode(PathNode* parentsNode, Station* station, Edge* edge, int mode, int totalTime = 0, int totaltransit = 0, int totaldistance = 0);
	PathNode* getParentsNode();
	void setParentsNode(PathNode* parentsNode);
	Station* getStation();
	Edge* getEdge();
	int getTotaltime();
	int getTotaltransit();
	int getTotaldistance();
	int getMode();
};

class PathNodeCompare {
public:
	bool operator() (PathNode* node1, PathNode* node2);
};


/*
struct PathNodeDistanceCompare {
	bool operator() (PathNode* node1, PathNode* node2) {
		return false;
	}
};*/
#endif