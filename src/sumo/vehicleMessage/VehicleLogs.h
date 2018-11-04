#pragma once

#include <map>
#include <vector>
#include "VehicleStatus.h"
class VehicleLogs
{
public:
	VehicleLogs(void);
	~VehicleLogs(void);

	//get vehicle logs
	map<string,vehicleStatus*> getVehicleLogs();
	
	//get a vehicle status
	vehicleStatus* getStatus(string vehicleID);
	
	//add a vehicle status
	void addLog(string vehicleID, vehicleStatus* status);

	void resetSent();

private:
	//a hash map to record the vehicles at a time
	map<string,vehicleStatus*> vehicleLogs;

};



VehicleLogs::VehicleLogs(void)
{
}


VehicleLogs::~VehicleLogs(void)
{
    //delete all edges
    map<string,vehicleStatus*>::const_iterator eit;
    for(eit = vehicleLogs.begin(); eit != vehicleLogs.end(); eit++){
        delete eit->second;
    }

}

//get vehicle logs 
map<string,vehicleStatus*>
	VehicleLogs::getVehicleLogs(){
		return this->vehicleLogs;
}

//get a vehicle status
vehicleStatus* VehicleLogs::getStatus(string vehicleID){
	return vehicleLogs[vehicleID];
}

//add a vehicle status
void VehicleLogs::addLog(string vehicleID, vehicleStatus* status){
	vehicleLogs[vehicleID] = status;
}

void VehicleLogs::resetSent(){
	
}
