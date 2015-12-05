#include "Edge.h"
#include "Station.h"

Edge::Edge() {
	toLineName = "";
	toStationName = "";
	from = NULL;
	to = NULL;
	weight = -1;
	waittime = -1;
	distance = -1;
	isTransit = false;
}
Edge::Edge(int weight, int waittime, int distance, string toLineName, string toStationName,
	bool isTransit, bool isMetropolitan, Station* from, Station* to)
	: weight(weight), waittime(waittime), distance(distance), toLineName(toLineName), toStationName(toStationName),
	isTransit(isTransit), isMetropolitan(isMetropolitan), from(from), to(to) {

}
int Edge::getWeight() {
	return weight;
}
int Edge::getWaitTime() {
	return waittime;
}
int Edge::getDistance() {
	return distance;
}
bool Edge::getIsTransit() {
	return isTransit;
}
bool Edge::getIsMetropolitan() {
	return isMetropolitan;
}
string Edge::getToLineName() {
	return toLineName;
}
string Edge::getToStationName() {
	return toStationName;
}
Station* Edge::getFrom() {
	return from;
}
Station* Edge::getTo() {
	return to;
}
void Edge::setTo(Station* to) {
	this->to = to;
}