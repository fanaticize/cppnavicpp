#include "Line.h"
#include "Station.h"

Line::Line() {
	name = "";
}
Line::Line(string name) : name(name) {
}
Line::~Line() {
	for (vector< Station* >::iterator IterPos0 = stationList.begin(); IterPos0 != stationList.end(); ++IterPos0)
		delete(*IterPos0);
}
string Line::getName() {
	return name;
}
vector<Station*>* Line::getStationList() {
	return &stationList;
}