#ifndef EDGE_H
#define EDGE_H

#include "../ModelTools.h"
#include "Junction.h"
#include "Lane.h"
#include <map>



class Edge
{
public:
	Edge(string id);
	~Edge(void);

	//add a lane
	void addLanes(string id, Lane* lane);
	
	//get and set
	string getID();
	Lane* getLane(string);
	map<string, Lane*> getLanes();
	string getFrom();
	void setFrom(string from);
	string getTo();
	void setTo(string to);
	double getFromX();
	void setFromX(double fromX);
	double getFromY();
	void setFromY(double fromY);
	double getToX();
	void setToX(double toX);
	double getToY();
	void setToY(double toY);

	void addJunction(Junction* junction);
	vector<Junction*> getJunctions();
private:
	//id
	string id;
	//hash map to record lanes in the edge
	map<string,Lane*> lanes;
	//the id junction of from and to
	string from, to;
	//the position of from
	double fromX, fromY;
	//the position of to
	double toX, toY;
	//the junction connect to the edge
	vector<Junction*> junctions;
	
};




Edge::Edge(string id)
{
	this->id.assign(id);
	this->from.assign("");
	this->to.assign("");
	fromX = fromY = toX = toY =0;
	
}



Edge::~Edge(void)
{
    for(int i = 0; i < junctions.size(); i ++){
        delete junctions[i];
    }
}

//add a lane
void Edge::addLanes(string id, Lane* lane){
	lanes[id] = lane;

	vector<string>* initPos = new vector<string>();
	string shape = (*(lane->getShape()))[0];
	string div(",");
	split(shape, div, initPos);

	this->fromX = atof((*initPos)[0].c_str());
	this->fromY = atof((*initPos)[1].c_str());
	
}

string Edge::getID(){
	return id;
}

Lane* Edge::getLane(string id){
	return lanes[id];
}

map<string, Lane*> Edge::getLanes(){
	return this->lanes;
}

string Edge::getFrom(){
	return from;
}

void Edge::setFrom(string from){
	this->from.assign(from);
}

string Edge::getTo(){
	return to;
}

void Edge::setTo(string to){
	this->to.assign(to);
}

double Edge::getFromX(){
	return fromX;
}

void Edge::setFromX(double fromX){
	this->fromX = fromX;
}

double Edge::getFromY(){
	return this->fromY;
}

void Edge::setFromY(double fromY){
	this->fromY = fromY;
}

double Edge::getToX(){
	return toX;
}

void Edge::setToX(double toX){
	this->toX = toX;
}

double Edge::getToY(){
	return toY;
}

void Edge::setToY(double toY){
	this->toY = toY;
}

void Edge::addJunction(Junction* junction){
	junctions.push_back(junction);
}

vector<Junction*> Edge::getJunctions(){
	return junctions;
}


#endif
