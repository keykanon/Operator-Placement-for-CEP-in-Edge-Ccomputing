#ifndef LANE_H
#define LANE_H

#include "../ModelTools.h"

class Lane
{
public:
	Lane(string id, double length, string shape);
	~Lane(void);

	//getter
	string getID();
	double getX();
	double getY();
	double getLength();
	vector<string>* getShape();

	//setter
	void setX(double x);
	void setY(double y);
private:
	//id
	string id;
	//position
	double x , y;
	//length
	double length;
	//shape
	vector<string>* shape;
	
};



Lane::Lane(string id, double length, string shape)
{
	//initial
	this->id.assign(id);
	this->length = length;
	this->shape = new vector<string>();

	string s;
	s.assign(" ");

	//handle the shape
	split(shape, s, this->shape);
}


Lane::~Lane(void)
{
    delete shape;
}

//get
string Lane::getID(){
	return id;
}

double Lane::getLength(){
	return this->length;
}

double Lane::getX(){
	return this->x;
}

double Lane::getY(){
	return this->y;
}

vector<string>* Lane::getShape(){
	return this->shape;
}

//setter
void Lane::setX(double x){
	this->x = x;
}

void Lane::setY(double y){
	this->y = y;
}

#endif
