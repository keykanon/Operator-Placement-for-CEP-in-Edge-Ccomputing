#ifndef JUNCTION_H
#define JUNCTION_H

#include "../ModelTools.h"
#include "Edge.h"

class Junction
{
public:
	Junction(string id, string x, string y, 
		string intlane1, string intLanes2, string shape);
	~Junction(void);

	//getter
	string getID();
	double getX();
	double getY();
	vector<string>* getincLanes();
	vector<string>* getShape();


private:
	//id
	string id;
	//position
	double x, y;

	//the lane point
	vector<string>* incLanes;
	//the lane shape
	vector<string>* shape;
	//the edge connect to the junction
	
};




Junction::Junction(string id, string x, string y,
	string incLanes1, string incLanes2, string shape)
{
	//initial
	this->id.assign(id);
	this->x = atof(x.c_str());
	this->y = atof(y.c_str());
	this->incLanes = new vector<string>();
	this->shape = new vector<string>();
	
	//
	string s ;
	s.assign(" ");

	//handle incLanes
	split(incLanes1, s, incLanes); 
	split(incLanes2, s, incLanes);

	//handle shape
	split(shape, s, this->shape);

}


Junction::~Junction(void)
{
	delete this->incLanes;
	delete this->shape;
}


//getter
string Junction::getID(){
	return this->id;
}

double Junction::getX(){
	return this->x;
}

double Junction::getY(){
	return this->y;
}

vector<string>* Junction::getincLanes(){
	return incLanes;
}

vector<string>* Junction::getShape(){
	return shape;
}

#endif
