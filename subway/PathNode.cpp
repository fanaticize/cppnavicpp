#include "PathNode.h"

PathNode::PathNode() {

}
PathNode::PathNode(PathNode* parentsNode, Station* station, Edge* edge, int mode, int totalTime, int totaltransit, int totaldistance)
	:parentsNode(parentsNode), station(station), edge(edge), mode(mode), totaltime(totalTime), totaltransit(totaltransit), totaldistance(totaldistance) {

}
PathNode* PathNode::getParentsNode() {
	return parentsNode;
}
void PathNode::setParentsNode(PathNode* parentsNode) {
	this->parentsNode = parentsNode;
}
Station* PathNode::getStation() {
	return station;
}
Edge* PathNode::getEdge() {
	return edge;
}
int PathNode::getTotaltime() {
	return totaltime;
}
int PathNode::getTotaltransit() {
	return totaltransit;
}
int PathNode::getTotaldistance() {
	return totaldistance;
}
int PathNode::getMode() {
	return mode;
}

bool PathNodeCompare::operator() (PathNode* node1, PathNode* node2) {
	if (node1->getMode() == MINIMUM_DISTANCE) {
		return node1->getTotaldistance() > node2->getTotaldistance();
	}
	if (node1->getMode() == MINIMUM_TIME) {
		if (node1->getTotaltime() == node2->getTotaltime())
			return node1->getTotaltransit() > node2->getTotaltransit();
		else
			return node1->getTotaltime() > node2->getTotaltime();
	}
	else if (node1->getMode() == MINIMUM_TRANSIT) {
		if (node1->getTotaltransit() == node2->getTotaltransit())
			return node1->getTotaltime() > node2->getTotaltime();
		else
			return node1->getTotaltransit() > node2->getTotaltransit();
	}
	return node1->getTotaltime() > node2->getTotaltime();
}
