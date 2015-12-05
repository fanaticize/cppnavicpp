#ifndef __STATION_H__
#define __STATION_H__

#include "SubwayCommonDecl.h"

using namespace std;

class Line;
class Edge;
class Station {
private:
	Line* line;
	string name;
	int stationSeq;
	vector<Edge*> edgeList;
public:
	Station();
	Station(Line* line, string name, int stationSeq);
	~Station();
	Line* getLine();
	string getName();
	int getStationSeq();
	vector<Edge*>* getEdgeList();
};

#endif