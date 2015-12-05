#ifndef __LINE_H__
#define __LINE_H__

#include "SubwayCommonDecl.h"
using namespace std;

class Station;
class Line {
private:
	string name;
	vector<Station*> stationList;
public:
	Line();
	Line(string);
	~Line();
	string getName();
	vector<Station*>* getStationList();
};
#endif