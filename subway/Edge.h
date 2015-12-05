#ifndef __EDGE_H__
#define __EDGE_H__

#include "SubwayCommonDecl.h"

class Station;
class Edge {
private:
	int weight;
	int waittime;
	int distance;
	bool isTransit;
	bool isMetropolitan;
	string toLineName;
	string toStationName;
	Station* from;
	Station* to;
public:
	Edge();
	Edge(int weight, int waittime, int distance, string toLineName, string toStationName,
		bool isTransit, bool isMetropolitan, Station* from = NULL, Station* to = NULL);
	int getWeight();
	int getWaitTime();
	int getDistance();
	bool getIsTransit();
	bool getIsMetropolitan();
	string getToLineName();
	string getToStationName();
	Station* getFrom();
	Station* getTo();
	void setTo(Station* to);
};

#endif