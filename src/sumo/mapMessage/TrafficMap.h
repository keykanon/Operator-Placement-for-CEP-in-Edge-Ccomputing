#ifndef TRAFFIC_MAP_H
#define TRAFFIC_MAP_H

#include "Edge.h"
#include "Junction.h"
#include "../ModelTools.h"
#include <map>
#include <string.h>

using namespace std;

class TrafficMap
{
public:
	TrafficMap(void);
	~TrafficMap(void);

	void addEdge(string edgeID, Edge* edge);
	void addJunction(string junctionID, Junction* junction);
	Edge* getEdge(string id);
	Junction* getJunction(string id);
	map<string, Edge*> getEdges();
	map<string, Junction*> getJunctions();
	void computePosition(Junction*  junction);

private:
	map<string, Edge*> edges;
	map<string, Junction*> junctions;
};



TrafficMap::TrafficMap(void)
{
}


TrafficMap::~TrafficMap(void)
{
    //delete all edges
    map<string,Edge*>::const_iterator eit;
    for(eit = edges.begin(); eit != edges.end(); eit++){
        delete eit->second;
    }

    map<string,Junction*>::const_iterator jit;
    for(jit = junctions.begin(); jit != junctions.end(); jit++){
        delete jit->second;
    }
}

void TrafficMap::computePosition(Junction* junction){
	// flag
	bool findFlag = false;

	//get junctions lanes
	vector<string>* junctionLanes;
	if((junctionLanes = junction->getincLanes()) != NULL){
		//loop the junction lines
		vector<string>::iterator jit;
		for(jit = junctionLanes->begin(); jit != junctionLanes->end(); jit ++){
			
			//loop to travel all edges
			map<string,Edge*>::const_iterator eit;
			for(eit = edges.begin(); eit != edges.end() && !findFlag; eit++){
				//get an edge
				Edge *edge = eit->second;
			
				//get from x and y
				if(strcmp(edge->getFrom().c_str(), "") != 0 &&
					strcmp(edge->getFrom().c_str(),junction->getID().c_str()) == 0){
					edge->setFromX(junction->getX());
					edge->setFromY(junction->getY());
				}
				
				//get to x and y
				if(strcmp(edge->getTo().c_str(), "") != 0 &&
					strcmp(edge->getTo().c_str(),junction->getID().c_str()) == 0){
					edge->setToX(junction->getX());
					edge->setToY(junction->getY());
				}

				//get lanes
				map<string, Lane*> lanes = edge->getLanes();
				map<string, Lane*>::iterator lit;

				for(lit = lanes.begin(); lit != lanes.end(); lit ++){
					//get a lane
					Lane* lane = lit->second;

					//find the position
					if(strcmp(lane->getID().c_str(), jit->c_str()) == 0){
						lane->setX(junction->getX());
						lane->setY(junction->getY());

						findFlag = true;
						break;
					}
				}
	//			junction->addEdge(edge);
				//edge->setFromX(junction->getX());
				//edge->setFromY(junction->getY());
				edge->addJunction(junction);
			}
			
			
		}
	}
}

//add a edge
void TrafficMap::addEdge(string edgeID, Edge* edge){
	this->edges[edgeID] = edge;
}

//add a junction
void TrafficMap::addJunction(string id, Junction* junction){
	this->junctions[id] = junction;

	//use a junction to get the lane position
	computePosition(junction);
}

//get an edge
Edge* TrafficMap::getEdge(string id){
	return edges[id];
}


//get a junction
Junction* TrafficMap::getJunction(string id){
	return junctions[id];
}

//get edges
map<string, Edge*> TrafficMap::getEdges(){
	return edges;
}

//get junctions
map<string, Junction*> TrafficMap::getJunctions(){
	return junctions;
}


#endif

