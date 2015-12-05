#include "Station.h"
#include "Line.h"
#include "Edge.h"

Station::Station() {
	name = "";
}
Station::Station(Line* line, string name, int stationSeq)
	: line(line), name(name), stationSeq(stationSeq) {

}
Station::~Station() {
	for (vector< Edge* >::iterator IterPos0 = edgeList.begin(); IterPos0 != edgeList.end(); ++IterPos0)
		delete(*IterPos0);
}
Line* Station::getLine() {
	return line;
}
string Station::getName() {
	return name;
}
int Station::getStationSeq() {
	return stationSeq;
}
vector<Edge*>* Station::getEdgeList() {
	return &edgeList;
}