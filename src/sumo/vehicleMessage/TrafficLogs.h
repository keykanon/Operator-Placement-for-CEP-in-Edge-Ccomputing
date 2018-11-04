#pragma once

#include <map>
#include "VehicleLogs.h"

using namespace std;



class TrafficLogs
{
public:
	TrafficLogs(void);
	~TrafficLogs(void);

	//get traffic logs
	map<double,VehicleLogs*> getTrafficLogs();

	//get the status of a vehicle at a time
	vehicleStatus* getStatus(double time, string vehicleID);

	//add a log
	void addLog(string vehicleID, double time, vehicleStatus* status);

	//search the vehicle status
	vector<vehicleStatus*>* getVehicles(double time, UpdateStrategies& update,double timeSpan, string csmID, double spatial, vehicleStatus** csmStatus);
	vector<vehicleStatus*>* getVehicles(double time, int deviation, double timeSpan, string csmID, double spatial, vehicleStatus** csmStatus, int* num);
	//void reset the sent flag
	void resetSentFlag();
	//reset AR
	void resetAR();
	void resetTempAR();
	// get precision
	double getPrecision();
	double getRecall();
	double getTempRecall();
private:
	map<double,VehicleLogs*> trafficLogs;
	int AR_ARa;
	int ARa;
	int AR;
	int tempAR_ARa;
	int tempARa;
	int tempAR;
};



TrafficLogs::TrafficLogs(void)
{
	AR_ARa = 0;
	ARa = 0;
	AR = 0;
}


TrafficLogs::~TrafficLogs(void)
{
    //delete all edges
    map<double,VehicleLogs*>::const_iterator eit;
    for(eit = trafficLogs.begin(); eit != trafficLogs.end(); eit++){
        delete eit->second;
    }
}

void TrafficLogs::resetAR(){
	AR_ARa = 0;
	ARa = 0;
	AR = 0;
}

void TrafficLogs::resetTempAR(){
	tempAR_ARa = 0;
	tempARa = 0;
	tempAR = 0;
}

double TrafficLogs::getPrecision(){
	if(ARa == 0){
		return 0;
	}
	return ((double)AR_ARa)/(double)ARa;
}

double TrafficLogs::getTempRecall(){
	if(tempAR == 0){
		return 0;
	}
	return ((double) tempAR_ARa)/(double)tempAR;
}

double TrafficLogs::getRecall(){
	if(AR == 0){
		return 0;
	}
	return ((double) AR_ARa)/(double)AR;
}

//get traffic logs
map<double, VehicleLogs*>
	TrafficLogs::getTrafficLogs(){
		return this->trafficLogs;
}

//get the status of a vehicle at a time
vehicleStatus* TrafficLogs::getStatus(double time, string vehicleID){
	return this->trafficLogs[time]->getStatus(vehicleID);
}

//add a log
void TrafficLogs::addLog(string vehicleID, double time, vehicleStatus* status){
	//get vehicle logs
	VehicleLogs* vehicleLogs = trafficLogs[time];
	
	//there is no log in the traffic logs at the time
	if(vehicleLogs == NULL){
		vehicleLogs = new VehicleLogs();
		vehicleLogs->addLog(vehicleID, status);
		trafficLogs[time] = vehicleLogs;
	}
	// add a log
	else{
		vehicleLogs->addLog(vehicleID, status);
	}
}

//search the vehicle status
vector<vehicleStatus*>* TrafficLogs::getVehicles(double time, UpdateStrategies& update, double timeSpan, string csmID, double spatial, vehicleStatus** csmStatus){
	//initial
	vector<vehicleStatus*>* vehicles = new vector<vehicleStatus*>();

	//get the vehicle logs
	VehicleLogs* vLogs = trafficLogs[time];
	
	map<string, vehicleStatus*> vehicles_hash = vLogs->getVehicleLogs();

	//printf("csmID = %d\n", csmID);
	//get the consumer status
	if(vLogs->getStatus(csmID) != NULL){
		*csmStatus = vLogs->getStatus(csmID);
	}

	if(*csmStatus == NULL){
		return vehicles;
	}
	update.isUpdate = false;
	if(update.updateStrategies == 0){
		//printf("recall = %f\n", getRecall());
		if(this->getTempRecall() < 0.9){
			update.x = (*csmStatus)->getX();
			update.y = (*csmStatus)->getY();
			update.isUpdate = true;
			resetTempAR();
		}
	}else if(update.updateStrategies == 1){
		double distance = calDistance( (*csmStatus)->getX(),(*csmStatus)->getY(), update.x , update.y );
		printf("distance = %f,update.x = %f, update.y = %f, x = %f , y = %f speed = %f pos = %f\n",
			distance, update.x, update.y, (*csmStatus)->getX(),(*csmStatus)->getY(), (*csmStatus)->getSpeed(), (*csmStatus)->getPos());

		update.distance = distance;
		if(update.distance > update.updateMeters){
			update.distance = 0;
			update.x = (*csmStatus)->getX();
			update.y = (*csmStatus)->getY();
			//printf("update:distance = %f,update.x = %f, update.y = %f, x = %f , y = %f speed = %f\n", distance, (update.x), update.y, (*csmStatus)->getX(),(*csmStatus)->getY(), (*csmStatus)->getSpeed());
			update.isUpdate = true;
		}
	}
	else if(update.updateStrategies == 2){
		update.temporal ++;
		if(update.temporal >= update.updateTemporal){
			update.temporal = 0;
			update.x = (*csmStatus)->getX();
			update.y = (*csmStatus)->getY();
			update.isUpdate = true;
		}
	}

	if(update.isUpdate){
		resetTempAR();
		//travel to search the vehicle with the spatial interest
		map<double, VehicleLogs*>::iterator vlit;

		//time span windows
		double winBegin = trafficLogs.begin()->first;
		if(time - timeSpan > trafficLogs.begin()->first){
			winBegin = time - timeSpan;
		}
	
	
		for(; winBegin < time; winBegin ++){
			map<string, vehicleStatus*>::iterator sit;
		
			VehicleLogs* vl = trafficLogs[winBegin];
			map<string, vehicleStatus*> vh = vl->getVehicleLogs();
			for(sit = vh.begin(); sit != vh.end(); sit ++){
			//map<string, vehicleStatus*> vh = vLogs->getVehicleLogs();

			//for(sit = vh.begin(); sit != vh.end(); sit ++){
				if(sit->second == NULL){
				    break;
				}
			    vehicleStatus* vStatus = sit->second;
				if(vStatus->isSent()){
					continue;
				}
				if(vStatus->intersted(*csmStatus,update, spatial)){
					vehicles->push_back(vStatus);
					ARa ++;
					tempARa ++;
				}
				double d = calDistance(vStatus->getX(), vStatus->getY(), (*csmStatus)->getX(), (*csmStatus)->getY());
				if(d < spatial){
					AR ++;
					tempAR ++;
					d = calDistance(vStatus->getX(),vStatus->getY(), update.x, update.y);
					if(d < spatial){
						AR_ARa ++;
						tempAR_ARa ++;
					}
				}

			}
		}
	}
	else {
		map<string, vehicleStatus*>::iterator sit;
		
		VehicleLogs* vl = trafficLogs[time];

		map<string, vehicleStatus*> vh = vl->getVehicleLogs();
		//map<string, vehicleStatus*> vh = vLogs->getVehicleLogs();

		for(sit = vh.begin(); sit != vh.end(); sit ++){
			vehicleStatus* vStatus = sit->second;
			if(vStatus->isSent()){
					continue;
			}
			if(vStatus->intersted(*csmStatus,update, spatial)){
				vehicles->push_back(vStatus);
				ARa ++;
				tempARa ++;
			}
			double d = calDistance(vStatus->getX(), vStatus->getY(), (*csmStatus)->getX(), (*csmStatus)->getY());
			if(d < spatial){
				AR ++;
				tempAR ++;
				d = calDistance(vStatus->getX(),vStatus->getY(), update.x, update.y);
				if(d < spatial){
					AR_ARa ++;
					tempAR_ARa ++;
				}
			}

		}
		
	}

	return vehicles;
}

vector<vehicleStatus*>*  TrafficLogs::getVehicles(double time, int deviation,
	double timeSpan, string csmID, double spatial, vehicleStatus** csmStatus, int* num){
		//initial
	vector<vehicleStatus*>* vehicles = new vector<vehicleStatus*>();

	//get the vehicle logs
	VehicleLogs* vLogs = trafficLogs[time];
	
	map<string, vehicleStatus*> vehicles_hash = vLogs->getVehicleLogs();

	//get the consumer status
	if(vLogs->getStatus(csmID) != NULL){
		*csmStatus = vLogs->getStatus(csmID);
	}

	if(*csmStatus == NULL){
		return vehicles;
	}

	//travel to search the vehicle with the spatial interest
	map<double, VehicleLogs*>::iterator vlit;

	//time span windows
	double winBegin = trafficLogs.begin()->first;
	if(time - timeSpan > trafficLogs.begin()->first){
		winBegin = time - timeSpan;
	}
	
	
	for(; winBegin < time; winBegin ++){
		map<string, vehicleStatus*>::iterator sit;
		
		VehicleLogs* vl = trafficLogs[winBegin];
		map<string, vehicleStatus*> vh = vl->getVehicleLogs();
		//map<string, vehicleStatus*> vh = vLogs->getVehicleLogs();

		for(sit = vh.begin(); sit != vh.end(); sit ++){
			vehicleStatus* vStatus = sit->second;
			int vnum;
			if((vnum = vStatus->intersted(*csmStatus,deviation, spatial)) > 0){
				*num += vnum;
				vehicles->push_back(vStatus);
					
			}
		}
	}

	return vehicles;
}

//reset sent flag
void TrafficLogs::resetSentFlag(){
	map<double,VehicleLogs*>::iterator vlit;
	//printf("flag\n");
	for(vlit = trafficLogs.begin(); vlit != trafficLogs.end(); vlit ++){
		map<string, vehicleStatus*>::iterator vsit;
		//printf("flag1\n");
		map<string, vehicleStatus*> vh = vlit->second->getVehicleLogs();
		for(vsit = vh.begin(); vsit != vh.end(); vsit ++){
			//printf("flag2\n");
			vehicleStatus* vs = vsit->second;
			if(vs != NULL){
				vsit->second->setSent(false);
			}
			
			//printf("flag3\n");
		}
	}
}
