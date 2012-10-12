#include "Grid.hpp"
GridNode::~GridNode(){}
GridConnection::~GridConnection(){}
YADE_PLUGIN((GridNode)(GridConnection));


void GridNode::addConnection(shared_ptr<Body> GC){
	ConnList.push_back(GC);
}