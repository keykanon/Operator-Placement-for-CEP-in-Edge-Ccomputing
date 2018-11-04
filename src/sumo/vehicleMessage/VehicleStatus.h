#ifndef VEHICLE_STATUS_H
#define VEHICLE_STATUS_H

#include "../ModelTools.h"
#include "../mapMessage/TrafficMap.h"
#include <cmath>


struct UpdateStrategies{
	int updateStrategies;
	double updateMeters;
	double distance;
	int updateTemporal;
	int temporal;
	bool isUpdate;
	double x, y;
};

/*-----------vehicle status-------------
This class is used to record the vehicle status.
include vehicle's id, speed, position.
position include edge, lane and x ,y 
and the distance between the begin of the lane and the position vehicle in.
but the status need time.
Details see in the TrafficLogs
*/

class vehicleStatus
{
public:
	vehicleStatus(string id, string edgeID, string laneID,
		double pos, double speed, TrafficMap* map);
	~vehicleStatus(void);

	//getter
	string getVehicleID();
	string getEdgeID();
	string getLaneID();
	double getX();
	double getY();
	double getSpeed();
	double getPos();
	bool isSent();
	void setSent(bool sent);
	bool intersted(vehicleStatus* csmStatus,UpdateStrategies update, double spatial);
	int intersted(vehicleStatus* csmStatus, int deviation, double spatial);
private:
	//id
	string id;
	//pos which is caculated by pos
	double x, y;
	//edge and lane
	string edgeID;
	string laneID;
	//speed
	double speed;
	//position from the begin point of lane
	double pos;
	//is sent
	bool sent;

};



vehicleStatus::vehicleStatus(string id, string edgeID, string laneID,
		double pos, double speed, TrafficMap* map)
{
	//assign the initial 
	this->id.assign(id);
	this->edgeID.assign(edgeID);
	this->laneID.assign(laneID);
	this->pos = pos;
	this->speed = speed;

	sent = false;

	//calculate position x and y
	Edge* edge = map->getEdge(edgeID);
	Lane* lane = edge->getLane(laneID);
	
	double tempx = 1,  tempy = 1;
	string s(",") ;
	//string splitxy(",");
	
	//loop to travel shape
	vector<string>::iterator sit;

	x = edge->getFromX();
	y = edge->getFromY();
	if(x == 0 && y == 0){
		
	}
	for(sit = lane->getShape()->begin(); sit != lane->getShape()->end() && pos > 0; sit ++){
		//get vector
		vector<string>* vec = new vector<string> ();
		split(*sit,s , vec);
		//get vec(tempx,tempy)
		tempx = atof( (*vec)[0].c_str());
		tempy = atof( (*vec)[1].c_str() );
		//calculate ctan
		double dir = atan((tempy - y)/(tempx - x));

		//calculate length
		double length = sqrt(pow(tempx - x, 2) + pow(tempy - y, 2));
		pos -= length;
		//calculate new position x and y
		if(pos < 0){
			x += pos * cos(dir);
			y += pos * sin(dir);
			delete vec;
			break;
		}
		else{
			x = tempx;
			y = tempy;
			delete vec;
		}
	}
	//cout << id << "," << x << "," << y << endl;
	
}

vehicleStatus::~vehicleStatus(void)
{
	this->id.clear();
	this->edgeID.clear();
	this->laneID.clear();
}


//getter
string vehicleStatus::getVehicleID(){
	return this->id;
}

string vehicleStatus::getEdgeID(){
	return this->edgeID;
}

string vehicleStatus::getLaneID(){
	return this->laneID;
}

double vehicleStatus::getSpeed(){
	return this->speed;
}

double vehicleStatus::getPos(){
	return this->pos;
}

double vehicleStatus::getX(){
	return this->x;
}

double vehicleStatus::getY(){
	return this->y;	
}

bool vehicleStatus::intersted(vehicleStatus* csmStatus,UpdateStrategies update, double spatial){
	//double csmDistance = calDistance(x,y, csmStatus->getX(), csmStatus->getY());
	double updateDistance = calDistance(x, y ,update.x , update.y);
	
	return( updateDistance < spatial);
}

int vehicleStatus::intersted(vehicleStatus* csmStatus, int deviation, double spatial){
	
	deviation *= 10;
	int count = 0;// tempCount = 0;
	double begin_x = ((int)(csmStatus->getX() - spatial)/ deviation) * deviation;
	double begin_y = ((int)(csmStatus->getY() - spatial)/ deviation) * deviation;
	double end_x = ((int)(csmStatus->getX() + spatial)/ deviation + 1) * deviation;
	double end_y = ((int)(csmStatus->getY() + spatial)/ deviation + 1) * deviation;

	for(double tempx = begin_x; tempx < end_x; tempx += deviation){
		for(double tempy = begin_y; tempy < end_y; tempy += deviation){
			double distance = calDistance(tempx, tempy, x, y);
			if(distance <= deviation){
				count ++;
			}
		}
	}
	printf("count = %d\n", count);
	return count;
	
}

bool vehicleStatus::isSent(){
	return this->sent;
}

void vehicleStatus::setSent(bool sent){
	this->sent = sent;
}

#endif
